#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QDateTime>
#include <QVariant>

/**
 * @brief Represents the status of a Task in the Kanban board.
 */
enum class TaskStatus {
    Todo,
    Doing,
    Done
};

/**
 * @brief Represents the priority level of a Task.
 */
enum class TaskPriority {
    Low,
    Medium,
    High
};

/**
 * @brief The Task model represents a single actionable item.
 *
 * Tasks are directly linked to a Workspace. They feature a start and
 * end time, allowing them to be scheduled or displayed in a "Daily" view.
 */
class Task {
public:
    /**
     * @brief Default constructor for an empty/invalid task.
     */
    Task() : m_id(-1), m_workspaceId(-1), m_status(TaskStatus::Todo),
             m_priority(TaskPriority::Medium), m_sortOrder(0) {}

    /**
     * @brief Constructs a Task with all required fields.
     *
     * @param id The unique database identifier of the task.
     * @param workspaceId The ID of the workspace this task belongs to.
     * @param title The task's title.
     * @param description Detailed description of the task.
     * @param status Current progress status (Todo, Doing, Done).
     * @param priority Importance level.
     * @param startDate When the task should begin.
     * @param endDate When the task should be completed.
     * @param sortOrder Vertical ordering position in the Kanban column.
     */
    Task(int id, int workspaceId, const QString& title, const QString& description,
         TaskStatus status, TaskPriority priority, 
         const QDateTime& startDate, const QDateTime& endDate, int sortOrder = 0)
        : m_id(id), m_workspaceId(workspaceId), m_title(title),
          m_description(description), m_status(status), m_priority(priority),
          m_startDate(startDate), m_endDate(endDate), m_sortOrder(sortOrder) {}

    // -- Getters --

    /** @return The unique task ID. */
    int id() const { return m_id; }
    
    /** @return The ID of the workspace this task belongs to. */
    int workspaceId() const { return m_workspaceId; }
    
    /** @return The task title. */
    QString title() const { return m_title; }
    
    /** @return The task description. */
    QString description() const { return m_description; }
    
    /** @return The current status of the task. */
    TaskStatus status() const { return m_status; }
    
    /** @return The priority level of the task. */
    TaskPriority priority() const { return m_priority; }
    
    /** @return The start date and time of the task. */
    QDateTime startDate() const { return m_startDate; }
    
    /** @return The end date and time of the task. */
    QDateTime endDate() const { return m_endDate; }
    
    /** @return The ordering index inside its status column. */
    int sortOrder() const { return m_sortOrder; }

    // -- Setters --

    /** @param id Set the unique task ID. */
    void setId(int id) { m_id = id; }
    
    /** @param workspaceId Set the workspace ID. */
    void setWorkspaceId(int workspaceId) { m_workspaceId = workspaceId; }
    
    /** @param title Set the task title. */
    void setTitle(const QString& title) { m_title = title; }
    
    /** @param description Set the task description. */
    void setDescription(const QString& description) { m_description = description; }
    
    /** @param status Set the task's progress status. */
    void setStatus(TaskStatus status) { m_status = status; }
    
    /** @param priority Set the task's priority level. */
    void setPriority(TaskPriority priority) { m_priority = priority; }
    
    /** @param startDate Set the date and time the task starts. */
    void setStartDate(const QDateTime& startDate) { m_startDate = startDate; }
    
    /** @param endDate Set the date and time the task ends. */
    void setEndDate(const QDateTime& endDate) { m_endDate = endDate; }
    
    /** @param order Set the sorting index. */
    void setSortOrder(int order) { m_sortOrder = order; }

    /**
     * @brief Checks if this task has valid data.
     * @return True if the ID is valid (>0), false otherwise.
     */
    bool isValid() const { return m_id > 0; }

    // -- Helpers for database mapping --

    /**
     * @brief Converts the TaskStatus enum to a string for database storage.
     * @return "todo", "doing", or "done".
     */
    static QString statusToString(TaskStatus status) {
        switch (status) {
            case TaskStatus::Doing: return "doing";
            case TaskStatus::Done: return "done";
            default: return "todo";
        }
    }

    /**
     * @brief Converts a database string back to a TaskStatus enum.
     * @param str The string from the database.
     * @return The corresponding TaskStatus.
     */
    static TaskStatus statusFromString(const QString& str) {
        QString s = str.toLower();
        if (s == "doing") return TaskStatus::Doing;
        if (s == "done") return TaskStatus::Done;
        return TaskStatus::Todo;
    }

    /**
     * @brief Converts the TaskPriority enum to a string for database storage.
     * @return "low", "medium", or "high".
     */
    static QString priorityToString(TaskPriority priority) {
        switch (priority) {
            case TaskPriority::Low: return "low";
            case TaskPriority::High: return "high";
            default: return "medium";
        }
    }

    /**
     * @brief Converts a database string back to a TaskPriority enum.
     * @param str The string from the database.
     * @return The corresponding TaskPriority.
     */
    static TaskPriority priorityFromString(const QString& str) {
        QString s = str.toLower();
        if (s == "low") return TaskPriority::Low;
        if (s == "high") return TaskPriority::High;
        return TaskPriority::Medium;
    }

private:
    int m_id;
    int m_workspaceId;
    QString m_title;
    QString m_description;
    TaskStatus m_status;
    TaskPriority m_priority;
    QDateTime m_startDate;
    QDateTime m_endDate;
    int m_sortOrder;
};

#endif // TASK_H
