#ifndef TASK_CONTROLLER_H
#define TASK_CONTROLLER_H

#include <QObject>
#include <QVector>
#include <QDate>
#include <optional>
#include "../models/Task.h"
#include "../database/AsyncExecutor.h"

/**
 * @brief Handles background processing of tasks.
 *
 * Exposes non-blocking methods for the UI to interact with Tasks.
 * Automatically signals the UI when data is ready or an operation succeeds/fails.
 */
class TaskController : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Constructs the TaskController.
     * @param executor The executor for background work.
     * @param parent The parent QObject (typically the MainWindow).
     */
    explicit TaskController(AsyncExecutor* executor, QObject* parent = nullptr);

    /**
     * @brief Loads all tasks for a specific workspace.
     * @param workspaceId The target workspace.
     */
    void loadTasksForWorkspace(int workspaceId);

    /**
     * @brief Loads tasks that overlap with the specific date (Daily view).
     * @param workspaceId The target workspace.
     * @param date The specific day to filter tasks for.
     */
    void loadDailyTasks(int workspaceId, const QDate& date);

    /**
     * @brief Creates a new task and adds it to the database asynchronously.
     * @param workspaceId ID of the owning workspace.
     * @param title Task title.
     * @param desc Task description.
     * @param status Progress status.
     * @param priority Importance level.
     * @param startDate Start date/time.
     * @param endDate End date/time.
     */
    void createTask(int workspaceId, const QString& title, const QString& desc,
                    TaskStatus status, TaskPriority priority, 
                    const QDateTime& startDate, const QDateTime& endDate);

    /**
     * @brief Updates an existing task.
     * @param task The task object containing modified data.
     */
    void updateTask(const Task& task);

    /**
     * @brief Deletes a task from the database.
     * @param taskId The task to delete.
     */
    void deleteTask(int taskId);

    /**
     * @brief Persists the new visual sort order of tasks to the database.
     * @param idOrderPairs Vector of pairs containing (TaskId, NewSortOrder).
     */
    void reorderTasks(const QVector<QPair<int, int>>& idOrderPairs);

signals:
    /** @brief Emitted when tasks are loaded successfully. */
    void tasksLoaded(const QVector<Task>& tasks);
    
    /** @brief Emitted when a task is successfully created. */
    void taskCreated(const Task& task);
    
    /** @brief Emitted when a task is successfully updated. */
    void taskUpdated(const Task& task);
    
    /** @brief Emitted when a task is successfully deleted. */
    void taskDeleted(int taskId);

    /** @brief Emitted when tasks are reordered. */
    void tasksReordered();
    
    /** @brief Emitted if any task operation fails. */
    void error(const QString& errorMessage);

private:
    AsyncExecutor* m_executor;
};

#endif // TASK_CONTROLLER_H
