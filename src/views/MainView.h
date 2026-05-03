#ifndef MAIN_VIEW_H
#define MAIN_VIEW_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QLabel>
#include <QLineEdit>

#include "../widgets/SidebarWidget.h"
#include "../widgets/TaskColumn.h"
#include "../models/Task.h"

/**
 * @brief The main task board view.
 *
 * Contains the sidebar on the left and the Kanban board on the right.
 */
class MainView : public QWidget {
    Q_OBJECT

public:
    explicit MainView(QWidget* parent = nullptr);

    /** Get access to the sidebar to populate it */
    SidebarWidget* sidebar() const { return m_sidebar; }

    /** Update the project title displayed in the header */
    void setProjectTitle(const QString& title);

    /** Populate the columns with tasks */
    void setTasks(const QVector<Task>& tasks);

signals:
    void taskDropped(int taskId, TaskStatus newStatus, int newIndex);
    void taskClicked(int taskId);
    void taskDoubleClicked(int taskId);
    void addTaskRequested(TaskStatus status);
    void searchRequested(const QString& query);

private:
    void setupUi();

    SidebarWidget* m_sidebar;
    QLabel* m_projectTitleLabel;
    QLineEdit* m_searchInput;
    
    TaskColumn* m_todoColumn;
    TaskColumn* m_doingColumn;
    TaskColumn* m_doneColumn;
};

#endif // MAIN_VIEW_H
