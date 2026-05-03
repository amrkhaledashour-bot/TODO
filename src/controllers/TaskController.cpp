#include "TaskController.h"
#include "../database/DatabaseManager.h"
#include <QDebug>

TaskController::TaskController(AsyncExecutor* executor, QObject* parent)
    : QObject(parent), m_executor(executor) {}

void TaskController::loadTasksForWorkspace(int workspaceId) {
    auto future = m_executor->run<QVector<Task>>([workspaceId]() {
        return DatabaseManager::instance().getTasks(workspaceId);
    });

    connect(future, &QFutureWatcher<QVector<Task>>::finished, this, [this, future]() {
        emit tasksLoaded(future->result());
        future->deleteLater();
    });
}

void TaskController::loadDailyTasks(int workspaceId, const QDate& date) {
    auto future = m_executor->run<QVector<Task>>([workspaceId, date]() {
        return DatabaseManager::instance().getDailyTasks(workspaceId, date);
    });

    connect(future, &QFutureWatcher<QVector<Task>>::finished, this, [this, future]() {
        emit tasksLoaded(future->result());
        future->deleteLater();
    });
}

void TaskController::createTask(int workspaceId, const QString& title, const QString& desc,
                                TaskStatus status, TaskPriority priority, 
                                const QDateTime& startDate, const QDateTime& endDate) {
    auto future = m_executor->run<std::optional<Task>>([=]() {
        return DatabaseManager::instance().createTask(workspaceId, title, desc, status, priority, startDate, endDate);
    });

    connect(future, &QFutureWatcher<std::optional<Task>>::finished, this, [this, future]() {
        auto result = future->result();
        if (result.has_value()) {
            emit taskCreated(result.value());
        } else {
            emit error("Failed to create task in the database.");
        }
        future->deleteLater();
    });
}

void TaskController::updateTask(const Task& task) {
    auto future = m_executor->run<bool>([task]() {
        return DatabaseManager::instance().updateTask(task);
    });

    connect(future, &QFutureWatcher<bool>::finished, this, [this, task, future]() {
        if (future->result()) {
            emit taskUpdated(task);
        } else {
            emit error("Failed to update task.");
        }
        future->deleteLater();
    });
}

void TaskController::deleteTask(int taskId) {
    auto future = m_executor->run<bool>([taskId]() {
        return DatabaseManager::instance().deleteTask(taskId);
    });

    connect(future, &QFutureWatcher<bool>::finished, this, [this, taskId, future]() {
        if (future->result()) {
            emit taskDeleted(taskId);
        } else {
            emit error("Failed to delete task.");
        }
        future->deleteLater();
    });
}

void TaskController::reorderTasks(const QVector<QPair<int, int>>& idOrderPairs) {
    auto future = m_executor->run<bool>([idOrderPairs]() {
        return DatabaseManager::instance().reorderTasks(idOrderPairs);
    });

    connect(future, &QFutureWatcher<bool>::finished, this, [this, future]() {
        if (future->result()) {
            emit tasksReordered();
        } else {
            emit error("Failed to save new task order.");
        }
        future->deleteLater();
    });
}
