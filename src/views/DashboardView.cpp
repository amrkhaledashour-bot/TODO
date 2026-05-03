#include "DashboardView.h"
#include <QScrollArea>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>

DashboardView::DashboardView(QWidget* parent) : QWidget(parent) {
    setupUi();
}

void DashboardView::setupUi() {
    setObjectName("dashboardView");

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 30, 40, 30);
    mainLayout->setSpacing(20);

    // -- Top bar: greeting + logout --
    auto* topBar = new QHBoxLayout();

    m_greetingLabel = new QLabel("Good morning!", this);
    m_greetingLabel->setObjectName("dashboardGreeting");
    topBar->addWidget(m_greetingLabel);

    topBar->addStretch();

    m_logoutBtn = new QPushButton("🚪 Logout", this);
    m_logoutBtn->setObjectName("dashboardLogoutBtn");
    m_logoutBtn->setCursor(Qt::PointingHandCursor);
    connect(m_logoutBtn, &QPushButton::clicked, this, &DashboardView::logoutClicked);
    topBar->addWidget(m_logoutBtn);

    mainLayout->addLayout(topBar);

    // -- Section header: Workspaces --
    auto* sectionHeader = new QHBoxLayout();

    auto* sectionLabel = new QLabel("Your Workspaces", this);
    sectionLabel->setObjectName("dashboardSectionLabel");
    sectionHeader->addWidget(sectionLabel);

    sectionHeader->addStretch();

    m_newWorkspaceBtn = new QPushButton("+ New Workspace", this);
    m_newWorkspaceBtn->setObjectName("dashboardNewBtn");
    m_newWorkspaceBtn->setCursor(Qt::PointingHandCursor);
    connect(m_newWorkspaceBtn, &QPushButton::clicked, this, &DashboardView::createWorkspaceClicked);
    sectionHeader->addWidget(m_newWorkspaceBtn);

    mainLayout->addLayout(sectionHeader);

    // -- Scrollable grid of workspace cards --
    auto* scrollArea = new QScrollArea(this);
    scrollArea->setObjectName("dashboardScroll");
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_gridContainer = new QWidget(scrollArea);
    m_gridLayout = new QGridLayout(m_gridContainer);
    m_gridLayout->setSpacing(16);
    m_gridLayout->setContentsMargins(0, 0, 0, 0);

    scrollArea->setWidget(m_gridContainer);
    mainLayout->addWidget(scrollArea, 1);
}

void DashboardView::setUsername(const QString& username) {
    // figure out a nice time-based greeting
    int hour = QTime::currentTime().hour();
    QString greeting;
    if (hour < 12) greeting = "Good morning";
    else if (hour < 17) greeting = "Good afternoon";
    else greeting = "Good evening";

    m_greetingLabel->setText(greeting + ", " + username + "! 👋");
}

void DashboardView::setWorkspaces(const QVector<Workspace>& workspaces) {
    clearGrid();

    int col = 0, row = 0;
    for (const auto& ws : workspaces) {
        auto* card = createWorkspaceCard(ws);
        m_gridLayout->addWidget(card, row, col);

        col++;
        if (col >= 3) {  // 3 cards per row
            col = 0;
            row++;
        }
    }

    // if no workspaces, show a friendly empty state
    if (workspaces.isEmpty()) {
        auto* emptyLabel = new QLabel("No workspaces yet.\nClick \"+ New Workspace\" to get started!", m_gridContainer);
        emptyLabel->setObjectName("emptyState");
        emptyLabel->setAlignment(Qt::AlignCenter);
        m_gridLayout->addWidget(emptyLabel, 0, 0, 1, 3);
    }
}

QWidget* DashboardView::createWorkspaceCard(const Workspace& ws) {
    auto* card = new QWidget(m_gridContainer);
    card->setObjectName("workspaceCard");
    card->setMinimumSize(220, 140);
    card->setMaximumSize(320, 180);
    card->setCursor(Qt::PointingHandCursor);

    // card shadow
    auto* shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(16);
    shadow->setColor(QColor(0, 0, 0, 35));
    shadow->setOffset(0, 4);
    card->setGraphicsEffect(shadow);

    auto* layout = new QVBoxLayout(card);
    layout->setContentsMargins(20, 16, 20, 16);
    layout->setSpacing(8);

    // icon + name
    auto* iconLabel = new QLabel(ws.icon(), card);
    iconLabel->setObjectName("wsCardIcon");
    layout->addWidget(iconLabel);

    auto* nameLabel = new QLabel(ws.name(), card);
    nameLabel->setObjectName("wsCardName");
    layout->addWidget(nameLabel);

    // description (truncated)
    if (!ws.description().isEmpty()) {
        auto* descLabel = new QLabel(ws.description(), card);
        descLabel->setObjectName("wsCardDesc");
        descLabel->setWordWrap(true);
        descLabel->setMaximumHeight(40);
        layout->addWidget(descLabel);
    }

    layout->addStretch();

    // clicking the card selects the workspace
    int wsId = ws.id();
    card->setProperty("workspaceId", wsId);

    // install event filter to catch mouse press on the card
    card->installEventFilter(this);

    return card;
}

void DashboardView::clearGrid() {
    // remove all widgets from grid
    QLayoutItem* item;
    while ((item = m_gridLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
}

bool DashboardView::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::MouseButtonPress) {
        if (obj->property("workspaceId").isValid()) {
            int wsId = obj->property("workspaceId").toInt();
            emit workspaceSelected(wsId);
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}
