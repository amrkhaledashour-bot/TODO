#include "SidebarWidget.h"
#include <QFrame>
#include <QSpacerItem>
#include <QVariant>
#include <QCursor>

SidebarWidget::SidebarWidget(QWidget* parent)
    : QWidget(parent) {
    setupUi();
}

void SidebarWidget::setupUi() {
    this->setFixedWidth(260);
    this->setObjectName("sidebar");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 20, 15, 20);
    mainLayout->setSpacing(20);

    // Header: Workspace info
    QVBoxLayout* headerLayout = new QVBoxLayout();
    headerLayout->setSpacing(5);

    m_workspaceLabel = new QLabel("Workspace", this);
    m_workspaceLabel->setObjectName("sidebarWorkspaceName");
    QFont wsFont;
    wsFont.setBold(true);
    wsFont.setPointSize(12);
    m_workspaceLabel->setFont(wsFont);

    m_userLabel = new QLabel("👤 user", this);
    m_userLabel->setObjectName("sidebarUserName");
    QFont userFont;
    userFont.setPointSize(9);
    m_userLabel->setFont(userFont);

    headerLayout->addWidget(m_workspaceLabel);
    headerLayout->addWidget(m_userLabel);
    mainLayout->addLayout(headerLayout);

    // Separator
    QFrame* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setObjectName("sidebarSeparator");
    mainLayout->addWidget(line);

    // Views List
    m_viewList = new QListWidget(this);
    m_viewList->setObjectName("sidebarProjectList"); // reuse styling
    m_viewList->setFocusPolicy(Qt::NoFocus);
    m_viewList->setCursor(Qt::PointingHandCursor);

    QListWidgetItem* dailyItem = new QListWidgetItem("☀️ Daily", m_viewList);
    dailyItem->setData(Qt::UserRole, "Daily");
    
    QListWidgetItem* allItem = new QListWidgetItem("📋 All Tasks", m_viewList);
    allItem->setData(Qt::UserRole, "All Tasks");

    QListWidgetItem* journalItem = new QListWidgetItem("📔 Journals", m_viewList);
    journalItem->setData(Qt::UserRole, "Journals");

    mainLayout->addWidget(m_viewList);

    connect(m_viewList, &QListWidget::currentItemChanged, this, &SidebarWidget::onViewSelectionChanged);

    // Spacer
    mainLayout->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

    // Bottom actions
    m_backBtn = new QPushButton("⬅ Back to Workspaces", this);
    m_backBtn->setObjectName("sidebarActionBtn");
    m_backBtn->setCursor(Qt::PointingHandCursor);

    m_themeBtn = new QPushButton("🌗 Toggle Theme", this);
    m_themeBtn->setObjectName("sidebarActionBtn");
    m_themeBtn->setCursor(Qt::PointingHandCursor);

    m_logoutBtn = new QPushButton("🚪 Log Out", this);
    m_logoutBtn->setObjectName("sidebarActionBtn");
    m_logoutBtn->setCursor(Qt::PointingHandCursor);

    mainLayout->addWidget(m_backBtn);
    mainLayout->addWidget(m_themeBtn);
    mainLayout->addWidget(m_logoutBtn);

    connect(m_backBtn, &QPushButton::clicked, this, &SidebarWidget::backToDashboard);
    connect(m_themeBtn, &QPushButton::clicked, this, &SidebarWidget::themeToggleClicked);
    connect(m_logoutBtn, &QPushButton::clicked, this, &SidebarWidget::logoutClicked);

    // default selection
    m_viewList->setCurrentRow(1); // Select All Tasks by default
}

void SidebarWidget::setUserInfo(const QString& username) {
    m_userLabel->setText("👤 " + username);
}

void SidebarWidget::setWorkspaceName(const QString& name) {
    m_workspaceLabel->setText(name);
}

void SidebarWidget::selectView(const QString& viewName) {
    for (int i = 0; i < m_viewList->count(); ++i) {
        if (m_viewList->item(i)->data(Qt::UserRole).toString() == viewName) {
            m_viewList->setCurrentRow(i);
            break;
        }
    }
}

void SidebarWidget::onViewSelectionChanged(QListWidgetItem* current, QListWidgetItem* previous) {
    Q_UNUSED(previous);
    if (!current) return;

    QString viewName = current->data(Qt::UserRole).toString();
    if (viewName == "Daily") {
        emit dailyViewSelected();
    } else if (viewName == "Journals") {
        emit journalsViewSelected();
    } else {
        emit allTasksViewSelected();
    }
}
