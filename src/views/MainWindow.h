#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QDate>

#include "LoginView.h"
#include "RegisterView.h"
#include "DashboardView.h"
#include "MainView.h"
#include "JournalView.h"
#include "TaskEditorDialog.h"

#include "../controllers/AuthController.h"
#include "../controllers/WorkspaceController.h"
#include "../controllers/TaskController.h"
#include "../controllers/JournalController.h"
#include "../widgets/ThemeManager.h"
#include "../database/AsyncExecutor.h"

/**
 * @brief The main window orchestrator.
 *
 * Owns the controllers and the stacked widget. Switches between
 * login, register, dashboard, and the main task board.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    // Auth flow
    void showLogin();
    void showRegister();
    void showDashboard();
    void handleLoginSuccess(const User& user);
    void handleSessionRestored(const User& user);
    void handleLogout();

    // Dashboard flow
    void handleWorkspaceSelected(int workspaceId);
    void handleCreateWorkspace();
    void handleWorkspacesLoaded(const QVector<Workspace>& workspaces);

    // Main view flow
    void handleDailyViewSelected();
    void handleAllTasksViewSelected();
    void handleJournalsViewSelected();
    void handleTasksLoaded(const QVector<Task>& tasks);

    // Journal operations
    void handleEntriesLoaded(const QVector<JournalEntry>& entries);
    void handleSaveJournalRequested(const QString& title, const QString& content);
    void handleDeleteJournalRequested(int id);
    void handleCreateJournalRequested();

    // Task operations
    void handleTaskClicked(int taskId);
    void handleTaskDoubleClicked(int taskId);
    void handleTaskDropped(int taskId, TaskStatus newStatus, int newIndex);
    void handleAddTaskRequested(TaskStatus status);
    void handleSearchRequested(const QString& query);

private:
    void setupUi();
    void setupControllers();
    void connectSignals();
    void showError(const QString& message);
    void reloadCurrentView();

    AsyncExecutor* m_executor;
    ThemeManager* m_themeManager;

    // Controllers
    AuthController* m_authController;
    WorkspaceController* m_workspaceController;
    TaskController* m_taskController;
    JournalController* m_journalController;

    // Views
    QStackedWidget* m_stackedWidget;
    LoginView* m_loginView;
    RegisterView* m_registerView;
    DashboardView* m_dashboardView;
    MainView* m_mainView;
    JournalView* m_journalView;

    // State
    int m_currentWorkspaceId = -1;
    int m_currentJournalId = -1;
    bool m_isDailyView = false;
    bool m_isJournalView = false;
};

#endif // MAIN_WINDOW_H
