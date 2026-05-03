#include "MainWindow.h"
#include "../database/DatabaseManager.h"
#include <QMessageBox>
#include <QInputDialog>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      m_executor(new AsyncExecutor(this)),
      m_themeManager(new ThemeManager(this))
{
    setupUi();
    setupControllers();
    connectSignals();

    setWindowTitle("Notion Todo");
    resize(1200, 800);
    setMinimumSize(800, 600);

    m_themeManager->loadSavedTheme();
    m_authController->restoreSession();
}

MainWindow::~MainWindow() {
}

void MainWindow::setupUi() {
    m_stackedWidget = new QStackedWidget(this);
    setCentralWidget(m_stackedWidget);

    m_loginView = new LoginView(this);
    m_registerView = new RegisterView(this);
    m_dashboardView = new DashboardView(this);
    m_mainView = new MainView(this);
    m_journalView = new JournalView(this);

    m_stackedWidget->addWidget(m_loginView);
    m_stackedWidget->addWidget(m_registerView);
    m_stackedWidget->addWidget(m_dashboardView);
    m_stackedWidget->addWidget(m_mainView);
    m_stackedWidget->addWidget(m_journalView);
}

void MainWindow::setupControllers() {
    m_authController = new AuthController(m_executor, this);
    m_workspaceController = new WorkspaceController(m_executor, this);
    m_taskController = new TaskController(m_executor, this);
    m_journalController = new JournalController(this);
}

void MainWindow::connectSignals() {
    // Auth Controller
    connect(m_authController, &AuthController::loginSuccess, this, &MainWindow::handleLoginSuccess);
    connect(m_authController, &AuthController::loginFailed, m_loginView, &LoginView::showError);
    connect(m_authController, &AuthController::registerSuccess, this, &MainWindow::handleLoginSuccess);
    connect(m_authController, &AuthController::registerFailed, m_registerView, &RegisterView::showError);
    connect(m_authController, &AuthController::sessionRestored, this, &MainWindow::handleSessionRestored);
    connect(m_authController, &AuthController::sessionExpired, this, &MainWindow::showLogin);
    connect(m_authController, &AuthController::loggedOut, this, &MainWindow::handleLogout);

    // Login View
    connect(m_loginView, &LoginView::loginRequested, m_authController, &AuthController::login);
    connect(m_loginView, &LoginView::switchToRegister, this, &MainWindow::showRegister);

    // Register View
    connect(m_registerView, &RegisterView::registerRequested, m_authController, &AuthController::registerUser);
    connect(m_registerView, &RegisterView::switchToLogin, this, &MainWindow::showLogin);

    // Dashboard View
    connect(m_dashboardView, &DashboardView::logoutClicked, m_authController, &AuthController::logout);
    connect(m_dashboardView, &DashboardView::workspaceSelected, this, &MainWindow::handleWorkspaceSelected);
    connect(m_dashboardView, &DashboardView::createWorkspaceClicked, this, &MainWindow::handleCreateWorkspace);

    // Workspace Controller
    connect(m_workspaceController, &WorkspaceController::workspacesLoaded, this, &MainWindow::handleWorkspacesLoaded);
    connect(m_workspaceController, &WorkspaceController::workspaceCreated, [this](const Workspace&) {
        m_workspaceController->loadWorkspaces(m_authController->currentUser().id());
    });
    connect(m_workspaceController, &WorkspaceController::error, this, &MainWindow::showError);

    // Main View Sidebar
    connect(m_mainView->sidebar(), &SidebarWidget::logoutClicked, m_authController, &AuthController::logout);
    connect(m_mainView->sidebar(), &SidebarWidget::themeToggleClicked, m_themeManager, &ThemeManager::toggleTheme);
    connect(m_mainView->sidebar(), &SidebarWidget::backToDashboard, this, &MainWindow::showDashboard);
    connect(m_mainView->sidebar(), &SidebarWidget::dailyViewSelected, this, &MainWindow::handleDailyViewSelected);
    connect(m_mainView->sidebar(), &SidebarWidget::allTasksViewSelected, this, &MainWindow::handleAllTasksViewSelected);
    connect(m_mainView->sidebar(), &SidebarWidget::journalsViewSelected, this, &MainWindow::handleJournalsViewSelected);

    // Task Board
    connect(m_mainView, &MainView::addTaskRequested, this, &MainWindow::handleAddTaskRequested);
    connect(m_mainView, &MainView::taskClicked, this, &MainWindow::handleTaskClicked);
    connect(m_mainView, &MainView::taskDoubleClicked, this, &MainWindow::handleTaskDoubleClicked);
    connect(m_mainView, &MainView::taskDropped, this, &MainWindow::handleTaskDropped);
    connect(m_mainView, &MainView::searchRequested, this, &MainWindow::handleSearchRequested);

    // Task Controller
    connect(m_taskController, &TaskController::tasksLoaded, this, &MainWindow::handleTasksLoaded);
    connect(m_taskController, &TaskController::taskCreated, [this](const Task&) {
        reloadCurrentView();
    });
    connect(m_taskController, &TaskController::taskUpdated, [this]() {
        reloadCurrentView();
    });
    connect(m_taskController, &TaskController::tasksReordered, [this]() {
        reloadCurrentView();
    });
    connect(m_taskController, &TaskController::error, this, &MainWindow::showError);

    // Journal View
    connect(m_journalView, &JournalView::backRequested, this, &MainWindow::handleAllTasksViewSelected);
    connect(m_journalView, &JournalView::createNewEntryRequested, this, &MainWindow::handleCreateJournalRequested);
    connect(m_journalView, &JournalView::saveEntryRequested, this, &MainWindow::handleSaveJournalRequested);
    connect(m_journalView, &JournalView::deleteEntryRequested, this, &MainWindow::handleDeleteJournalRequested);
    connect(m_journalView, &JournalView::entrySelected, [this](int id) { m_currentJournalId = id; });

    // Journal Controller
    connect(m_journalController, &JournalController::entriesLoaded, this, &MainWindow::handleEntriesLoaded);
    connect(m_journalController, &JournalController::entryCreated, [this](const JournalEntry&) {
        m_journalController->loadEntries(m_currentWorkspaceId);
    });
    connect(m_journalController, &JournalController::entryUpdated, [this](const JournalEntry&) {
        m_journalController->loadEntries(m_currentWorkspaceId);
    });
    connect(m_journalController, &JournalController::entryDeleted, [this](int) {
        m_journalView->clearEditor();
        m_journalController->loadEntries(m_currentWorkspaceId);
    });
    connect(m_journalController, &JournalController::error, this, &MainWindow::showError);
}

void MainWindow::reloadCurrentView() {
    if (m_currentWorkspaceId == -1) return;
    
    if (m_isDailyView) {
        m_taskController->loadDailyTasks(m_currentWorkspaceId, QDate::currentDate());
    } else {
        m_taskController->loadTasksForWorkspace(m_currentWorkspaceId);
    }
}

// --- Auth Flow ---

void MainWindow::showLogin() {
    m_loginView->reset();
    m_stackedWidget->setCurrentWidget(m_loginView);
}

void MainWindow::showRegister() {
    m_registerView->reset();
    m_stackedWidget->setCurrentWidget(m_registerView);
}

void MainWindow::showDashboard() {
    m_currentWorkspaceId = -1;
    m_dashboardView->setUsername(m_authController->currentUser().username());
    m_workspaceController->loadWorkspaces(m_authController->currentUser().id());
    m_stackedWidget->setCurrentWidget(m_dashboardView);
}

void MainWindow::handleLoginSuccess(const User& user) {
    showDashboard();
}

void MainWindow::handleSessionRestored(const User& user) {
    showDashboard();
}

void MainWindow::handleLogout() {
    showLogin();
}

// --- Dashboard Flow ---

void MainWindow::handleWorkspacesLoaded(const QVector<Workspace>& workspaces) {
    m_dashboardView->setWorkspaces(workspaces);
}

void MainWindow::handleWorkspaceSelected(int workspaceId) {
    m_currentWorkspaceId = workspaceId;
    m_mainView->sidebar()->setUserInfo(m_authController->currentUser().username());
    
    // Switch to "All Tasks" by default
    m_mainView->sidebar()->selectView("All Tasks");
    m_stackedWidget->setCurrentWidget(m_mainView);
}

void MainWindow::handleCreateWorkspace() {
    bool ok;
    QString text = QInputDialog::getText(this, "New Workspace",
                                         "Workspace name:", QLineEdit::Normal,
                                         "", &ok);
    if (ok && !text.isEmpty()) {
        m_workspaceController->createWorkspace(m_authController->currentUser().id(), text);
    }
}

// --- Main View Flow ---

void MainWindow::handleDailyViewSelected() {
    m_isDailyView = true;
    m_mainView->setProjectTitle(QString("☀️ Daily Tasks - %1").arg(QDate::currentDate().toString("MMMM d")));
    reloadCurrentView();
}

void MainWindow::handleAllTasksViewSelected() {
    m_isDailyView = false;
    m_isJournalView = false;
    m_mainView->setProjectTitle("📋 All Tasks");
    m_stackedWidget->setCurrentWidget(m_mainView);
    reloadCurrentView();
}

void MainWindow::handleJournalsViewSelected() {
    m_isJournalView = true;
    m_isDailyView = false;
    m_journalController->loadEntries(m_currentWorkspaceId);
    m_stackedWidget->setCurrentWidget(m_journalView);
}

void MainWindow::handleTasksLoaded(const QVector<Task>& tasks) {
    m_mainView->setTasks(tasks);
}

// --- Journal Operations ---

void MainWindow::handleEntriesLoaded(const QVector<JournalEntry>& entries) {
    m_journalView->setEntries(entries);
}

void MainWindow::handleSaveJournalRequested(const QString& title, const QString& content) {
    if (m_currentJournalId > 0) {
        m_executor->run<std::optional<JournalEntry>>(
            [this]() { return DatabaseManager::instance().getJournalEntryById(m_currentJournalId); },
            [this, title, content](const std::optional<JournalEntry>& opt) {
                if (opt.has_value()) {
                    JournalEntry e = opt.value();
                    e.setTitle(title);
                    e.setContent(content);
                    m_journalController->updateEntry(e);
                }
            }
        );
    }
}

void MainWindow::handleDeleteJournalRequested(int id) {
    m_journalController->deleteEntry(id);
}

void MainWindow::handleCreateJournalRequested() {
    if (m_currentWorkspaceId == -1) return;
    m_journalController->createEntry(m_currentWorkspaceId, "Untitled Entry", "", QDateTime::currentDateTime());
}

// --- Task Operations ---

void MainWindow::handleAddTaskRequested(TaskStatus status) {
    if (m_currentWorkspaceId == -1) return;
    
    TaskEditorDialog dlg(this);
    dlg.initForCreate(status);
    
    if (dlg.exec() == QDialog::Accepted) {
        Task t = dlg.getTaskData();
        m_taskController->createTask(m_currentWorkspaceId, t.title(), t.description(), 
                                     t.status(), t.priority(), t.startDate(), t.endDate());
    }
}

void MainWindow::handleTaskClicked(int taskId) {
    handleTaskDoubleClicked(taskId);
}

void MainWindow::handleTaskDoubleClicked(int taskId) {
    m_executor->run<std::optional<Task>>(
        [taskId]() { return DatabaseManager::instance().getTaskById(taskId); },
        [this](const std::optional<Task>& taskOpt) {
            if (!taskOpt.has_value()) return;
            
            TaskEditorDialog dlg(this);
            dlg.initForEdit(taskOpt.value());
            
            if (dlg.exec() == QDialog::Accepted) {
                m_taskController->updateTask(dlg.getTaskData());
            }
        }
    );
}

void MainWindow::handleTaskDropped(int taskId, TaskStatus newStatus, int newIndex) {
    m_executor->run<std::optional<Task>>(
        [taskId]() { return DatabaseManager::instance().getTaskById(taskId); },
        [this, newStatus, newIndex](const std::optional<Task>& taskOpt) {
            if (!taskOpt.has_value()) return;
            
            Task t = taskOpt.value();
            t.setStatus(newStatus);
            t.setSortOrder(newIndex);
            
            m_taskController->updateTask(t);
        }
    );
}

void MainWindow::handleSearchRequested(const QString& query) {
    if (m_currentWorkspaceId == -1) return;
    
    // For now we don't have search implemented properly on task controller
    // but if it was, we would filter here. Let's just reload.
    reloadCurrentView();
}

void MainWindow::showError(const QString& message) {
    QMessageBox::warning(this, "Error", message);
}
