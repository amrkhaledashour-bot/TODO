#include "MainView.h"
#include <QScrollArea>

MainView::MainView(QWidget* parent) : QWidget(parent) {
    setupUi();
}

void MainView::setupUi() {
    setObjectName("mainView");

    auto* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Sidebar
    m_sidebar = new SidebarWidget(this);
    mainLayout->addWidget(m_sidebar);

    // Board area (right side)
    auto* boardWidget = new QWidget(this);
    boardWidget->setObjectName("boardArea");
    auto* boardLayout = new QVBoxLayout(boardWidget);
    boardLayout->setContentsMargins(30, 30, 30, 30);
    boardLayout->setSpacing(20);

    // Board header (Title + Search)
    auto* headerLayout = new QHBoxLayout();
    
    m_projectTitleLabel = new QLabel("Loading tasks...", this);
    m_projectTitleLabel->setObjectName("projectTitle");
    headerLayout->addWidget(m_projectTitleLabel);
    
    headerLayout->addStretch();
    
    m_searchInput = new QLineEdit(this);
    m_searchInput->setObjectName("searchInput");
    m_searchInput->setPlaceholderText("🔍 Search tasks...");
    m_searchInput->setFixedWidth(250);
    connect(m_searchInput, &QLineEdit::textChanged, this, &MainView::searchRequested);
    headerLayout->addWidget(m_searchInput);

    boardLayout->addLayout(headerLayout);

    // Kanban Columns container (scrollable horizontally if needed)
    auto* columnsContainer = new QWidget(this);
    auto* columnsLayout = new QHBoxLayout(columnsContainer);
    columnsLayout->setContentsMargins(0, 0, 0, 0);
    columnsLayout->setSpacing(20);

    m_todoColumn = new TaskColumn(TaskStatus::Todo, this);
    m_doingColumn = new TaskColumn(TaskStatus::Doing, this);
    m_doneColumn = new TaskColumn(TaskStatus::Done, this);

    columnsLayout->addWidget(m_todoColumn);
    columnsLayout->addWidget(m_doingColumn);
    columnsLayout->addWidget(m_doneColumn);

    auto* scrollArea = new QScrollArea(this);
    scrollArea->setWidget(columnsContainer);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setObjectName("boardScrollArea");

    boardLayout->addWidget(scrollArea, 1);

    mainLayout->addWidget(boardWidget, 1);

    // Connect column signals to main view signals
    auto connectColumn = [this](TaskColumn* col) {
        connect(col, &TaskColumn::taskDropped, this, &MainView::taskDropped);
        connect(col, &TaskColumn::taskClicked, this, &MainView::taskClicked);
        connect(col, &TaskColumn::taskDoubleClicked, this, &MainView::taskDoubleClicked);
        connect(col, &TaskColumn::addTaskRequested, this, &MainView::addTaskRequested);
    };

    connectColumn(m_todoColumn);
    connectColumn(m_doingColumn);
    connectColumn(m_doneColumn);
}

void MainView::setProjectTitle(const QString& title) {
    m_projectTitleLabel->setText(title);
}

void MainView::setTasks(const QVector<Task>& tasks) {
    QVector<Task> todoTasks, doingTasks, doneTasks;
    
    for (const auto& task : tasks) {
        switch (task.status()) {
            case TaskStatus::Todo: todoTasks.append(task); break;
            case TaskStatus::Doing: doingTasks.append(task); break;
            case TaskStatus::Done: doneTasks.append(task); break;
        }
    }
    
    m_todoColumn->setTasks(todoTasks);
    m_doingColumn->setTasks(doingTasks);
    m_doneColumn->setTasks(doneTasks);
}
