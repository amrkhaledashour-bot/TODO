#ifndef SIDEBAR_WIDGET_H
#define SIDEBAR_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QListWidget>
#include "../models/Workspace.h"

/**
 * @brief Collapsible sidebar with user info, global views, and navigation.
 *
 * Shows the current workspace name, views (Daily, All Tasks),
 * and action buttons for toggling theme, logging out, and back to dashboard.
 */
class SidebarWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructs the SidebarWidget.
     * @param parent The parent widget.
     */
    explicit SidebarWidget(QWidget* parent = nullptr);

    /**
     * @brief Update the displayed user info.
     * @param username The username to show.
     */
    void setUserInfo(const QString& username);

    /**
     * @brief Update workspace name in the header.
     * @param name The workspace name.
     */
    void setWorkspaceName(const QString& name);

    /**
     * @brief Select a specific view manually.
     * @param viewName Either "Daily" or "All Tasks".
     */
    void selectView(const QString& viewName);

signals:
    /** @brief Emitted when "Daily" view is clicked. */
    void dailyViewSelected();

    /** @brief Emitted when "All Tasks" view is clicked. */
    void allTasksViewSelected();

    /** @brief Emitted when "Journals" view is clicked. */
    void journalsViewSelected();

    /** @brief Emitted to return to the Workspace selection screen. */
    void backToDashboard();

    /** @brief Emitted to toggle the application theme. */
    void themeToggleClicked();

    /** @brief Emitted to log the user out. */
    void logoutClicked();

private slots:
    void onViewSelectionChanged(QListWidgetItem* current, QListWidgetItem* previous);

private:
    void setupUi();

    QLabel* m_userLabel;
    QLabel* m_workspaceLabel;
    QListWidget* m_viewList;
    QPushButton* m_backBtn;
    QPushButton* m_themeBtn;
    QPushButton* m_logoutBtn;
};

#endif // SIDEBAR_WIDGET_H
