#include "TaskCard.h"
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>

TaskCard::TaskCard(const Task& task, QWidget* parent)
    : QWidget(parent), m_task(task) {
    setupUi();
    updateContent();
}

void TaskCard::setupUi() {
    setObjectName("taskCard");
    setCursor(Qt::PointingHandCursor);
    setMinimumHeight(70);
    setMaximumHeight(140);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(14, 10, 14, 10);
    mainLayout->setSpacing(6);

    // top row: priority dot + title
    auto* topRow = new QHBoxLayout();
    topRow->setSpacing(8);

    m_priorityBadge = createPriorityBadge(m_task.priority());
    topRow->addWidget(m_priorityBadge);

    m_titleLabel = new QLabel(this);
    m_titleLabel->setObjectName("taskCardTitle");
    m_titleLabel->setWordWrap(true);
    topRow->addWidget(m_titleLabel, 1);

    mainLayout->addLayout(topRow);

    // due date row
    m_dueDateLabel = new QLabel(this);
    m_dueDateLabel->setObjectName("taskCardDueDate");
    mainLayout->addWidget(m_dueDateLabel);

    // tags row
    auto* tagsWidget = new QWidget(this);
    m_tagsLayout = new QHBoxLayout(tagsWidget);
    m_tagsLayout->setContentsMargins(0, 0, 0, 0);
    m_tagsLayout->setSpacing(4);
    m_tagsLayout->addStretch();
    mainLayout->addWidget(tagsWidget);

    // subtle shadow for depth
    auto* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(8);
    shadow->setColor(QColor(0, 0, 0, 30));
    shadow->setOffset(0, 2);
    setGraphicsEffect(shadow);
}

void TaskCard::updateContent() {
    m_titleLabel->setText(m_task.title());

    // show time period if set
    if (m_task.startDate().isValid() && m_task.endDate().isValid()) {
        QString startStr;
        QString endStr;
        
        if (m_task.startDate().date() == m_task.endDate().date()) {
            // Same day: "Oct 12, 10:00 AM - 12:00 PM"
            startStr = m_task.startDate().toString("MMM d, hh:mm AP");
            endStr = m_task.endDate().toString("hh:mm AP");
        } else {
            // Different days: "Oct 12, 10:00 AM - Oct 13, 12:00 PM"
            startStr = m_task.startDate().toString("MMM d, hh:mm AP");
            endStr = m_task.endDate().toString("MMM d, hh:mm AP");
        }
        
        m_dueDateLabel->setText("⏱ " + startStr + " - " + endStr);
        m_dueDateLabel->setStyleSheet("font-size: 11px; color: #888;");
        m_dueDateLabel->setVisible(true);
    } else if (m_task.startDate().isValid()) {
        m_dueDateLabel->setText("⏱ " + m_task.startDate().toString("MMM d, hh:mm AP"));
        m_dueDateLabel->setStyleSheet("font-size: 11px; color: #888;");
        m_dueDateLabel->setVisible(true);
    } else {
        m_dueDateLabel->setVisible(false);
    }
}

QWidget* TaskCard::createPriorityBadge(TaskPriority priority) {
    auto* badge = new QWidget(this);
    badge->setFixedSize(10, 10);

    // color based on priority
    QString color;
    switch (priority) {
        case TaskPriority::High:   color = "#FF6B6B"; break;
        case TaskPriority::Medium: color = "#FFB347"; break;
        case TaskPriority::Low:    color = "#4ECDC4"; break;
    }

    badge->setStyleSheet(QString(
        "background-color: %1; border-radius: 5px;"
    ).arg(color));

    return badge;
}

void TaskCard::setTask(const Task& task) {
    m_task = task;
    updateContent();

    // rebuild priority badge
    auto* topLayout = qobject_cast<QHBoxLayout*>(layout()->itemAt(0)->layout());
    if (topLayout) {
        topLayout->removeWidget(m_priorityBadge);
        delete m_priorityBadge;
        m_priorityBadge = createPriorityBadge(task.priority());
        topLayout->insertWidget(0, m_priorityBadge);
    }
}

void TaskCard::setTags(const QVector<Tag>& tags) {
    m_tags = tags;

    // clear old tag chips
    QLayoutItem* item;
    while ((item = m_tagsLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // add new ones
    for (const auto& tag : tags) {
        auto* chip = new QLabel(tag.name(), this);
        chip->setObjectName("tagChip");
        chip->setStyleSheet(QString(
            "background-color: %1; color: white; border-radius: 4px; "
            "padding: 2px 8px; font-size: 10px; font-weight: bold;"
        ).arg(tag.color()));
        m_tagsLayout->addWidget(chip);
    }
    m_tagsLayout->addStretch();
}

void TaskCard::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit clicked(m_task.id());
    }
    QWidget::mousePressEvent(event);
}

void TaskCard::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit doubleClicked(m_task.id());
    }
    QWidget::mouseDoubleClickEvent(event);
}

void TaskCard::enterEvent(QEnterEvent* event) {
    // lift the card slightly on hover
    auto* shadow = qobject_cast<QGraphicsDropShadowEffect*>(graphicsEffect());
    if (shadow) {
        shadow->setBlurRadius(16);
        shadow->setColor(QColor(0, 0, 0, 50));
        shadow->setOffset(0, 4);
    }
    QWidget::enterEvent(event);
}

void TaskCard::leaveEvent(QEvent* event) {
    // settle back down
    auto* shadow = qobject_cast<QGraphicsDropShadowEffect*>(graphicsEffect());
    if (shadow) {
        shadow->setBlurRadius(8);
        shadow->setColor(QColor(0, 0, 0, 30));
        shadow->setOffset(0, 2);
    }
    QWidget::leaveEvent(event);
}
