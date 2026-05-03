#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QString>
#include <QVector>
#include <QPair>
#include <QDate>
#include <optional>

#include "../models/User.h"
#include "../models/Workspace.h"
#include "../models/Task.h"
#include "../models/JournalEntry.h"
#include "../models/Tag.h"

/**
 * @brief Singleton that owns the SQLite connection and all CRUD operations.
 *
 * Controllers never write raw SQL — they call methods here.
 * All methods are synchronous; async wrapping is done by AsyncExecutor.
 */
class DatabaseManager : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Gets the singleton instance.
     * @return Reference to the DatabaseManager.
     */
    static DatabaseManager& instance();

    /**
     * @brief Initializes the database connection and ensures schema is up to date.
     * @param dbPath Absolute path to the SQLite file.
     * @return True if initialized successfully.
     */
    bool initialize(const QString& dbPath);

    /**
     * @brief Closes the database connection.
     */
    void shutdown();

    // -- User Ops --
    std::optional<User> createUser(const QString& username, const QString& email,
                                   const QString& passwordHash, const QString& salt);
    std::optional<User> getUserByUsername(const QString& username);
    std::optional<User> getUserById(int id);

    // -- Session Ops --
    bool createSession(int userId, const QString& token);
    std::optional<int> validateSession(const QString& token);
    bool deleteSession(const QString& token);
    void cleanExpiredSessions();

    // -- Workspace Ops --
    QVector<Workspace> getWorkspaces(int userId);
    std::optional<Workspace> createWorkspace(int userId, const QString& name,
                                              const QString& desc = "", const QString& icon = "📋");
    bool updateWorkspace(const Workspace& ws);
    bool deleteWorkspace(int id);

    // -- Task Ops --
    /**
     * @brief Retrieves all tasks for a given workspace.
     */
    QVector<Task> getTasks(int workspaceId, std::optional<int> parentId = std::nullopt);
    
    /**
     * @brief Retrieves all tasks for a given workspace that overlap with the specific date.
     */
    QVector<Task> getDailyTasks(int workspaceId, const QDate& date);

    std::optional<Task> getTaskById(int id);
    
    /**
     * @brief Creates a new task.
     */
    std::optional<Task> createTask(int workspaceId, const QString& title,
                                    const QString& desc = "", TaskStatus status = TaskStatus::Todo,
                                    TaskPriority priority = TaskPriority::Medium,
                                    const QDateTime& startDate = QDateTime(), 
                                    const QDateTime& endDate = QDateTime(), 
                                    int parentTaskId = -1);
    bool updateTask(const Task& task);
    bool deleteTask(int id);
    bool reorderTasks(const QVector<QPair<int, int>>& idOrderPairs);
    QVector<Task> searchTasks(int workspaceId, const QString& query);
    QVector<Task> filterTasks(int workspaceId, std::optional<TaskStatus> status = std::nullopt,
                              std::optional<TaskPriority> priority = std::nullopt,
                              const QString& sortBy = "sort_order", bool ascending = true);

    // -- Journal Ops --
    QVector<JournalEntry> getJournalEntries(int workspaceId);
    std::optional<JournalEntry> getJournalEntryById(int id);
    std::optional<JournalEntry> createJournalEntry(int workspaceId, const QString& title, 
                                                   const QString& content, const QDateTime& date);
    bool updateJournalEntry(const JournalEntry& entry);
    bool deleteJournalEntry(int id);

    // -- Tag Ops --
    QVector<Tag> getTags(int workspaceId);
    std::optional<Tag> createTag(int workspaceId, const QString& name, const QString& color = "#FF6B6B");
    bool deleteTag(int id);
    bool addTagToTask(int taskId, int tagId);
    bool removeTagFromTask(int taskId, int tagId);
    QVector<Tag> getTaskTags(int taskId);

private:
    DatabaseManager() = default;
    ~DatabaseManager() = default;
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
    
    void createTables();
    void dropOldTables();
    
    QSqlDatabase m_db;
};

#endif // DATABASE_MANAGER_H
