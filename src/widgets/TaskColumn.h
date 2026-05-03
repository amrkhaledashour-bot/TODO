#ifndef TASK_COLUMN_H
#define TASK_COLUMN_H

#include <QWidget>
#include <QListWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include "../models/Task.h"

/**
 * @brief A kanban column (Todo / Doing / Done).
 *
 * Contains a header with the status name and count,
 * a list of TaskCards with drag-and-drop support,
 * and an "Add Task" button at the bottom.
 */
class TaskColumn : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Create a column for a specific task status.
     * @param status  Which status this column represents
     * @param parent  Parent widget
     */
    explicit TaskColumn(TaskStatus status, QWidget* parent = nullptr);

    /** Load tasks into the column. Clears existing items first. */
    void setTasks(const QVector<Task>& tasks);

    /** Get the status this column represents. */
    TaskStatus status() const { return m_status; }

    /** Get the inner list widget (for connecting signals). */
    QListWidget* listWidget() const { return m_listWidget; }

signals:
    void taskDropped(int taskId, TaskStatus newStatus, int newIndex);
    void taskClicked(int taskId);
    void taskDoubleClicked(int taskId);
    void addTaskRequested(TaskStatus status);

private:
    void setupUi();
    void updateCount();

    TaskStatus m_status;
    QLabel* m_headerLabel;
    QLabel* m_countLabel;
    QListWidget* m_listWidget;
    QPushButton* m_addButton;
};

#endif // TASK_COLUMN_H
