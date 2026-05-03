#include "TaskColumn.h"
#include "TaskCard.h"
#include <QMimeData>
#include <QDrag>
#include <QDropEvent>

TaskColumn::TaskColumn(TaskStatus status, QWidget* parent)
    : QWidget(parent), m_status(status) {
    setupUi();
}

void TaskColumn::setupUi() {
    setObjectName("taskColumn");

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    // -- Header area --
    auto* headerWidget = new QWidget(this);
    headerWidget->setObjectName("columnHeader");
    auto* headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(12, 8, 12, 8);

    // status indicator dot
    auto* dot = new QWidget(this);
    dot->setFixedSize(10, 10);
    QString dotColor;
    switch (m_status) {
        case TaskStatus::Todo:  dotColor = "#6C63FF"; break;
        case TaskStatus::Doing: dotColor = "#FFB347"; break;
        case TaskStatus::Done:  dotColor = "#4ECDC4"; break;
    }
    dot->setStyleSheet(QString("background-color: %1; border-radius: 5px;").arg(dotColor));
    headerLayout->addWidget(dot);

    QString displayName;
    switch (m_status) {
        case TaskStatus::Todo:  displayName = "To Do"; break;
        case TaskStatus::Doing: displayName = "Doing"; break;
        case TaskStatus::Done:  displayName = "Done"; break;
    }
    m_headerLabel = new QLabel(displayName, this);
    m_headerLabel->setObjectName("columnHeaderLabel");
    headerLayout->addWidget(m_headerLabel);

    headerLayout->addStretch();

    m_countLabel = new QLabel("0", this);
    m_countLabel->setObjectName("columnCount");
    headerLayout->addWidget(m_countLabel);
    
    // -- Add task icon button in header --
    m_addButton = new QPushButton("+", this);
    m_addButton->setObjectName("columnAddBtn");
    m_addButton->setCursor(Qt::PointingHandCursor);
    m_addButton->setFixedSize(24, 24);
    m_addButton->setToolTip("Add Task");
    connect(m_addButton, &QPushButton::clicked, this, [this]() {
        emit addTaskRequested(m_status);
    });
    headerLayout->addWidget(m_addButton);

    layout->addWidget(headerWidget);

    // -- Task list with drag and drop --
    m_listWidget = new QListWidget(this);
    m_listWidget->setObjectName("taskList");
    m_listWidget->setDragEnabled(true);
    m_listWidget->setAcceptDrops(true);
    m_listWidget->setDragDropMode(QAbstractItemView::DragDrop);
    m_listWidget->setDefaultDropAction(Qt::MoveAction);
    m_listWidget->setDropIndicatorShown(true);
    m_listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listWidget->setSpacing(4);

    // when an item is dropped, figure out what happened
    connect(m_listWidget->model(), &QAbstractItemModel::rowsInserted, this, [this](const QModelIndex&, int first, int) {
        // an item landed in our column
        auto* item = m_listWidget->item(first);
        if (item) {
            int taskId = item->data(Qt::UserRole).toInt();
            if (taskId > 0) {
                emit taskDropped(taskId, m_status, first);
            }
        }
        updateCount();
    });

    connect(m_listWidget->model(), &QAbstractItemModel::rowsRemoved, this, [this]() {
        updateCount();
    });

    layout->addWidget(m_listWidget, 1);

    // No bottom add button anymore
}

void TaskColumn::setTasks(const QVector<Task>& tasks) {
    m_listWidget->clear();

    for (const auto& task : tasks) {
        // create the list item
        auto* item = new QListWidgetItem(m_listWidget);
        item->setData(Qt::UserRole, task.id());
        item->setData(Qt::UserRole + 1, task.title()); // for drag text
        item->setSizeHint(QSize(0, 90));

        // create the card widget
        auto* card = new TaskCard(task, m_listWidget);
        connect(card, &TaskCard::clicked, this, &TaskColumn::taskClicked);
        connect(card, &TaskCard::doubleClicked, this, &TaskColumn::taskDoubleClicked);

        m_listWidget->setItemWidget(item, card);
    }

    updateCount();
}

void TaskColumn::updateCount() {
    m_countLabel->setText(QString::number(m_listWidget->count()));
}
