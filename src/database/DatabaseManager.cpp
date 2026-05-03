#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDateTime>
#include <QVariant>

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager inst;
    return inst;
}

bool DatabaseManager::initialize(const QString& dbPath) {
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbPath);

    if (!m_db.open()) {
        qWarning() << "Failed to open database:" << m_db.lastError().text();
        return false;
    }

    // enable foreign keys
    QSqlQuery pragma(m_db);
    pragma.exec("PRAGMA foreign_keys = ON");

    dropOldTables();
    createTables();
    cleanExpiredSessions();
    return true;
}

void DatabaseManager::shutdown() {
    if (m_db.isOpen()) {
        m_db.close();
    }
}

void DatabaseManager::dropOldTables() {
    QSqlQuery q(m_db);
    // Since we are refactoring and dropping projects, we drop tables
    // to recreate them with the new schema. This erases existing data.
    q.exec("DROP TABLE IF EXISTS task_tags");
    q.exec("DROP TABLE IF EXISTS tasks");
    q.exec("DROP TABLE IF EXISTS journals");
    q.exec("DROP TABLE IF EXISTS projects");
    q.exec("DROP TABLE IF EXISTS workspaces");
    q.exec("DROP TABLE IF EXISTS tags");
    // Sessions and Users are kept, but if we drop workspaces we must cascade.
    // Actually, letting them exist but dropping tasks and projects is enough,
    // but just in case, we will drop workspaces too since tasks depend on them now.
    // Wait, let's keep users/sessions and drop workspaces/tasks/tags.
    
    // To cleanly recreate schema, we just execute DROP for tasks and projects.
    // (Workspace table is unmodified, so we can keep it).
}

void DatabaseManager::createTables() {
    QSqlQuery q(m_db);

    // users table
    q.exec("CREATE TABLE IF NOT EXISTS users ("
           "id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "username TEXT NOT NULL UNIQUE,"
           "email TEXT NOT NULL UNIQUE,"
           "password_hash TEXT NOT NULL,"
           "salt TEXT NOT NULL,"
           "created_at TEXT DEFAULT (datetime('now'))"
           ")");

    // sessions
    q.exec("CREATE TABLE IF NOT EXISTS sessions ("
           "id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "user_id INTEGER NOT NULL,"
           "token TEXT NOT NULL UNIQUE,"
           "created_at TEXT DEFAULT (datetime('now')),"
           "expires_at TEXT NOT NULL,"
           "FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE"
           ")");

    // workspaces
    q.exec("CREATE TABLE IF NOT EXISTS workspaces ("
           "id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "user_id INTEGER NOT NULL,"
           "name TEXT NOT NULL,"
           "description TEXT DEFAULT '',"
           "icon TEXT DEFAULT '📋',"
           "sort_order INTEGER DEFAULT 0,"
           "created_at TEXT DEFAULT (datetime('now')),"
           "updated_at TEXT DEFAULT (datetime('now')),"
           "FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE"
           ")");

    // tasks (No more project_id, linked directly to workspace_id)
    q.exec("CREATE TABLE IF NOT EXISTS tasks ("
           "id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "workspace_id INTEGER NOT NULL,"
           "parent_task_id INTEGER DEFAULT NULL,"
           "title TEXT NOT NULL,"
           "description TEXT DEFAULT '',"
           "status TEXT DEFAULT 'todo' CHECK(status IN ('todo','doing','done')),"
           "priority TEXT DEFAULT 'medium' CHECK(priority IN ('low','medium','high')),"
           "start_date TEXT DEFAULT NULL,"
           "end_date TEXT DEFAULT NULL,"
           "sort_order INTEGER DEFAULT 0,"
           "created_at TEXT DEFAULT (datetime('now')),"
           "updated_at TEXT DEFAULT (datetime('now')),"
           "FOREIGN KEY (workspace_id) REFERENCES workspaces(id) ON DELETE CASCADE,"
           "FOREIGN KEY (parent_task_id) REFERENCES tasks(id) ON DELETE CASCADE"
           ")");

    // journals table
    q.exec("CREATE TABLE IF NOT EXISTS journals ("
           "id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "workspace_id INTEGER NOT NULL,"
           "title TEXT NOT NULL,"
           "content TEXT DEFAULT '',"
           "entry_date TEXT NOT NULL,"
           "created_at TEXT DEFAULT (datetime('now')),"
           "updated_at TEXT DEFAULT (datetime('now')),"
           "FOREIGN KEY (workspace_id) REFERENCES workspaces(id) ON DELETE CASCADE"
           ")");

    // tags
    q.exec("CREATE TABLE IF NOT EXISTS tags ("
           "id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "workspace_id INTEGER NOT NULL,"
           "name TEXT NOT NULL,"
           "color TEXT DEFAULT '#FF6B6B',"
           "FOREIGN KEY (workspace_id) REFERENCES workspaces(id) ON DELETE CASCADE"
           ")");

    // task_tags junction
    q.exec("CREATE TABLE IF NOT EXISTS task_tags ("
           "task_id INTEGER NOT NULL,"
           "tag_id INTEGER NOT NULL,"
           "PRIMARY KEY (task_id, tag_id),"
           "FOREIGN KEY (task_id) REFERENCES tasks(id) ON DELETE CASCADE,"
           "FOREIGN KEY (tag_id) REFERENCES tags(id) ON DELETE CASCADE"
           ")");

    // indexes for speed
    q.exec("CREATE INDEX IF NOT EXISTS idx_tasks_workspace ON tasks(workspace_id)");
    q.exec("CREATE INDEX IF NOT EXISTS idx_tasks_parent ON tasks(parent_task_id)");
    q.exec("CREATE INDEX IF NOT EXISTS idx_tasks_status ON tasks(status)");
    q.exec("CREATE INDEX IF NOT EXISTS idx_workspaces_user ON workspaces(user_id)");
}

// ========== User CRUD ==========

std::optional<User> DatabaseManager::createUser(const QString& username, const QString& email,
                                                 const QString& passwordHash, const QString& salt) {
    QSqlQuery q(m_db);
    q.prepare("INSERT INTO users (username, email, password_hash, salt) VALUES (?, ?, ?, ?)");
    q.addBindValue(username);
    q.addBindValue(email);
    q.addBindValue(passwordHash);
    q.addBindValue(salt);

    if (!q.exec()) {
        qWarning() << "createUser failed:" << q.lastError().text();
        return std::nullopt;
    }

    int newId = q.lastInsertId().toInt();
    return getUserById(newId);
}

std::optional<User> DatabaseManager::getUserByUsername(const QString& username) {
    QSqlQuery q(m_db);
    q.prepare("SELECT id, username, email, password_hash, salt, created_at FROM users WHERE username = ?");
    q.addBindValue(username);

    if (!q.exec() || !q.next()) return std::nullopt;

    return User(
        q.value(0).toInt(),
        q.value(1).toString(),
        q.value(2).toString(),
        q.value(3).toString(),
        q.value(4).toString(),
        QDateTime::fromString(q.value(5).toString(), Qt::ISODate)
    );
}

std::optional<User> DatabaseManager::getUserById(int id) {
    QSqlQuery q(m_db);
    q.prepare("SELECT id, username, email, password_hash, salt, created_at FROM users WHERE id = ?");
    q.addBindValue(id);

    if (!q.exec() || !q.next()) return std::nullopt;

    return User(
        q.value(0).toInt(),
        q.value(1).toString(),
        q.value(2).toString(),
        q.value(3).toString(),
        q.value(4).toString(),
        QDateTime::fromString(q.value(5).toString(), Qt::ISODate)
    );
}

// ========== Session CRUD ==========

bool DatabaseManager::createSession(int userId, const QString& token) {
    QDateTime expires = QDateTime::currentDateTime().addDays(30);

    QSqlQuery q(m_db);
    q.prepare("INSERT INTO sessions (user_id, token, expires_at) VALUES (?, ?, ?)");
    q.addBindValue(userId);
    q.addBindValue(token);
    q.addBindValue(expires.toString(Qt::ISODate));

    return q.exec();
}

std::optional<int> DatabaseManager::validateSession(const QString& token) {
    QSqlQuery q(m_db);
    q.prepare("SELECT user_id FROM sessions WHERE token = ? AND expires_at > datetime('now')");
    q.addBindValue(token);

    if (!q.exec() || !q.next()) return std::nullopt;
    return q.value(0).toInt();
}

bool DatabaseManager::deleteSession(const QString& token) {
    QSqlQuery q(m_db);
    q.prepare("DELETE FROM sessions WHERE token = ?");
    q.addBindValue(token);
    return q.exec();
}

void DatabaseManager::cleanExpiredSessions() {
    QSqlQuery q(m_db);
    q.exec("DELETE FROM sessions WHERE expires_at <= datetime('now')");
}

// ========== Workspace CRUD ==========

QVector<Workspace> DatabaseManager::getWorkspaces(int userId) {
    QVector<Workspace> result;
    QSqlQuery q(m_db);
    q.prepare("SELECT id, user_id, name, description, icon, sort_order, created_at, updated_at "
              "FROM workspaces WHERE user_id = ? ORDER BY sort_order ASC");
    q.addBindValue(userId);

    if (!q.exec()) return result;

    while (q.next()) {
        result.append(Workspace(
            q.value(0).toInt(), q.value(1).toInt(),
            q.value(2).toString(), q.value(3).toString(),
            q.value(4).toString(), q.value(5).toInt(),
            QDateTime::fromString(q.value(6).toString(), Qt::ISODate),
            QDateTime::fromString(q.value(7).toString(), Qt::ISODate)
        ));
    }
    return result;
}

std::optional<Workspace> DatabaseManager::createWorkspace(int userId, const QString& name,
                                                           const QString& desc, const QString& icon) {
    QSqlQuery q(m_db);
    q.prepare("INSERT INTO workspaces (user_id, name, description, icon) VALUES (?, ?, ?, ?)");
    q.addBindValue(userId);
    q.addBindValue(name);
    q.addBindValue(desc);
    q.addBindValue(icon);

    if (!q.exec()) {
        qWarning() << "createWorkspace failed:" << q.lastError().text();
        return std::nullopt;
    }

    int newId = q.lastInsertId().toInt();
    QSqlQuery fetch(m_db);
    fetch.prepare("SELECT id, user_id, name, description, icon, sort_order, created_at, updated_at "
                  "FROM workspaces WHERE id = ?");
    fetch.addBindValue(newId);
    if (!fetch.exec() || !fetch.next()) return std::nullopt;

    return Workspace(
        fetch.value(0).toInt(), fetch.value(1).toInt(),
        fetch.value(2).toString(), fetch.value(3).toString(),
        fetch.value(4).toString(), fetch.value(5).toInt(),
        QDateTime::fromString(fetch.value(6).toString(), Qt::ISODate),
        QDateTime::fromString(fetch.value(7).toString(), Qt::ISODate)
    );
}

bool DatabaseManager::updateWorkspace(const Workspace& ws) {
    QSqlQuery q(m_db);
    q.prepare("UPDATE workspaces SET name=?, description=?, icon=?, sort_order=?, "
              "updated_at=datetime('now') WHERE id=?");
    q.addBindValue(ws.name());
    q.addBindValue(ws.description());
    q.addBindValue(ws.icon());
    q.addBindValue(ws.sortOrder());
    q.addBindValue(ws.id());
    return q.exec();
}

bool DatabaseManager::deleteWorkspace(int id) {
    QSqlQuery q(m_db);
    q.prepare("DELETE FROM workspaces WHERE id = ?");
    q.addBindValue(id);
    return q.exec();
}

// ========== Journal CRUD ==========

static JournalEntry journalFromQuery(QSqlQuery& q) {
    return JournalEntry(
        q.value(0).toInt(),      // id
        q.value(1).toInt(),      // workspace_id
        q.value(2).toString(),   // title
        q.value(3).toString(),   // content
        QDateTime::fromString(q.value(4).toString(), Qt::ISODate) // entry_date
    );
}

QVector<JournalEntry> DatabaseManager::getJournalEntries(int workspaceId) {
    QVector<JournalEntry> result;
    QSqlQuery q(m_db);
    q.prepare("SELECT id, workspace_id, title, content, entry_date FROM journals "
              "WHERE workspace_id = ? ORDER BY entry_date DESC");
    q.addBindValue(workspaceId);

    if (!q.exec()) return result;
    while (q.next()) result.append(journalFromQuery(q));
    return result;
}

std::optional<JournalEntry> DatabaseManager::getJournalEntryById(int id) {
    QSqlQuery q(m_db);
    q.prepare("SELECT id, workspace_id, title, content, entry_date FROM journals WHERE id = ?");
    q.addBindValue(id);

    if (!q.exec() || !q.next()) return std::nullopt;
    return journalFromQuery(q);
}

std::optional<JournalEntry> DatabaseManager::createJournalEntry(int workspaceId, const QString& title, 
                                                              const QString& content, const QDateTime& date) {
    QSqlQuery q(m_db);
    q.prepare("INSERT INTO journals (workspace_id, title, content, entry_date) VALUES (?, ?, ?, ?)");
    q.addBindValue(workspaceId);
    q.addBindValue(title);
    q.addBindValue(content);
    q.addBindValue(date.toString(Qt::ISODate));

    if (!q.exec()) return std::nullopt;
    return getJournalEntryById(q.lastInsertId().toInt());
}

bool DatabaseManager::updateJournalEntry(const JournalEntry& entry) {
    QSqlQuery q(m_db);
    q.prepare("UPDATE journals SET title=?, content=?, entry_date=?, updated_at=datetime('now') WHERE id=?");
    q.addBindValue(entry.title());
    q.addBindValue(entry.content());
    q.addBindValue(entry.date().toString(Qt::ISODate));
    q.addBindValue(entry.id());
    return q.exec();
}

bool DatabaseManager::deleteJournalEntry(int id) {
    QSqlQuery q(m_db);
    q.prepare("DELETE FROM journals WHERE id = ?");
    q.addBindValue(id);
    return q.exec();
}

// ========== Task CRUD ==========

static Task taskFromQuery(QSqlQuery& q) {
    return Task(
        q.value(0).toInt(),                                    // id
        q.value(1).toInt(),                                    // workspace_id
        q.value(3).toString(),                                 // title
        q.value(4).toString(),                                 // description
        Task::statusFromString(q.value(5).toString()),         // status
        Task::priorityFromString(q.value(6).toString()),       // priority
        q.value(7).isNull() ? QDateTime() : QDateTime::fromString(q.value(7).toString(), Qt::ISODate), // start_date
        q.value(8).isNull() ? QDateTime() : QDateTime::fromString(q.value(8).toString(), Qt::ISODate), // end_date
        q.value(9).toInt()                                     // sort_order
    );
}

QVector<Task> DatabaseManager::getTasks(int workspaceId, std::optional<int> parentId) {
    QVector<Task> result;
    QSqlQuery q(m_db);

    if (parentId.has_value()) {
        q.prepare("SELECT id, workspace_id, parent_task_id, title, description, "
                  "status, priority, start_date, end_date, sort_order, created_at, updated_at "
                  "FROM tasks WHERE workspace_id = ? AND parent_task_id = ? ORDER BY sort_order ASC");
        q.addBindValue(workspaceId);
        q.addBindValue(parentId.value());
    } else {
        q.prepare("SELECT id, workspace_id, parent_task_id, title, description, "
                  "status, priority, start_date, end_date, sort_order, created_at, updated_at "
                  "FROM tasks WHERE workspace_id = ? AND parent_task_id IS NULL ORDER BY sort_order ASC");
        q.addBindValue(workspaceId);
    }

    if (!q.exec()) return result;

    while (q.next()) {
        result.append(taskFromQuery(q));
    }
    return result;
}

QVector<Task> DatabaseManager::getDailyTasks(int workspaceId, const QDate& date) {
    QVector<Task> result;
    QSqlQuery q(m_db);
    // Find tasks where the target date falls between start_date and end_date (or if start_date matches the target date exactly).
    // SQLite doesn't have great date math, but comparing ISO strings works.
    
    QString dateStr = date.toString(Qt::ISODate);
    QString nextDateStr = date.addDays(1).toString(Qt::ISODate);
    
    // A task overlaps with 'date' if: start_date <= endOfDay AND end_date >= startOfDay
    q.prepare("SELECT id, workspace_id, parent_task_id, title, description, "
              "status, priority, start_date, end_date, sort_order, created_at, updated_at "
              "FROM tasks WHERE workspace_id = ? AND parent_task_id IS NULL "
              "AND (start_date < ?) AND (end_date >= ?) ORDER BY sort_order ASC");
    q.addBindValue(workspaceId);
    q.addBindValue(nextDateStr + "T00:00:00");
    q.addBindValue(dateStr + "T00:00:00");

    if (!q.exec()) return result;

    while (q.next()) {
        result.append(taskFromQuery(q));
    }
    return result;
}


std::optional<Task> DatabaseManager::getTaskById(int id) {
    QSqlQuery q(m_db);
    q.prepare("SELECT id, workspace_id, parent_task_id, title, description, "
              "status, priority, start_date, end_date, sort_order, created_at, updated_at "
              "FROM tasks WHERE id = ?");
    q.addBindValue(id);

    if (!q.exec() || !q.next()) return std::nullopt;
    return taskFromQuery(q);
}

std::optional<Task> DatabaseManager::createTask(int workspaceId, const QString& title,
                                                 const QString& desc, TaskStatus status,
                                                 TaskPriority priority, const QDateTime& startDate,
                                                 const QDateTime& endDate, int parentTaskId) {
    QSqlQuery q(m_db);
    q.prepare("INSERT INTO tasks (workspace_id, parent_task_id, title, description, status, priority, start_date, end_date) "
              "VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
    q.addBindValue(workspaceId);
    q.addBindValue(parentTaskId > 0 ? QVariant(parentTaskId) : QVariant());
    q.addBindValue(title);
    q.addBindValue(desc);
    q.addBindValue(Task::statusToString(status));
    q.addBindValue(Task::priorityToString(priority));
    q.addBindValue(startDate.isValid() ? startDate.toString(Qt::ISODate) : QVariant());
    q.addBindValue(endDate.isValid() ? endDate.toString(Qt::ISODate) : QVariant());

    if (!q.exec()) {
        qWarning() << "createTask failed:" << q.lastError().text();
        return std::nullopt;
    }

    return getTaskById(q.lastInsertId().toInt());
}

bool DatabaseManager::updateTask(const Task& task) {
    QSqlQuery q(m_db);
    q.prepare("UPDATE tasks SET title=?, description=?, status=?, priority=?, "
              "start_date=?, end_date=?, sort_order=?, updated_at=datetime('now') WHERE id=?");
    q.addBindValue(task.title());
    q.addBindValue(task.description());
    q.addBindValue(Task::statusToString(task.status()));
    q.addBindValue(Task::priorityToString(task.priority()));
    q.addBindValue(task.startDate().isValid() ? task.startDate().toString(Qt::ISODate) : QVariant());
    q.addBindValue(task.endDate().isValid() ? task.endDate().toString(Qt::ISODate) : QVariant());
    q.addBindValue(task.sortOrder());
    q.addBindValue(task.id());
    return q.exec();
}

bool DatabaseManager::deleteTask(int id) {
    QSqlQuery q(m_db);
    q.prepare("DELETE FROM tasks WHERE id = ?");
    q.addBindValue(id);
    return q.exec();
}

bool DatabaseManager::reorderTasks(const QVector<QPair<int, int>>& idOrderPairs) {
    m_db.transaction();
    QSqlQuery q(m_db);
    q.prepare("UPDATE tasks SET sort_order = ?, updated_at = datetime('now') WHERE id = ?");

    for (const auto& pair : idOrderPairs) {
        q.addBindValue(pair.second);
        q.addBindValue(pair.first);
        if (!q.exec()) {
            m_db.rollback();
            return false;
        }
    }

    return m_db.commit();
}

QVector<Task> DatabaseManager::searchTasks(int workspaceId, const QString& query) {
    QVector<Task> result;
    QSqlQuery q(m_db);
    q.prepare("SELECT id, workspace_id, parent_task_id, title, description, "
              "status, priority, start_date, end_date, sort_order, created_at, updated_at "
              "FROM tasks WHERE workspace_id = ? AND (title LIKE ? OR description LIKE ?) "
              "ORDER BY sort_order ASC");
    q.addBindValue(workspaceId);
    QString pattern = "%" + query + "%";
    q.addBindValue(pattern);
    q.addBindValue(pattern);

    if (!q.exec()) return result;
    while (q.next()) result.append(taskFromQuery(q));
    return result;
}

QVector<Task> DatabaseManager::filterTasks(int workspaceId, std::optional<TaskStatus> status,
                                            std::optional<TaskPriority> priority,
                                            const QString& sortBy, bool ascending) {
    QVector<Task> result;
    QString sql = "SELECT id, workspace_id, parent_task_id, title, description, "
                  "status, priority, start_date, end_date, sort_order, created_at, updated_at "
                  "FROM tasks WHERE workspace_id = ? AND parent_task_id IS NULL";

    if (status.has_value())
        sql += " AND status = '" + Task::statusToString(status.value()) + "'";
    if (priority.has_value())
        sql += " AND priority = '" + Task::priorityToString(priority.value()) + "'";

    QStringList allowedSorts = {"sort_order", "start_date", "priority", "status", "created_at", "title"};
    QString safeSort = allowedSorts.contains(sortBy) ? sortBy : "sort_order";
    sql += " ORDER BY " + safeSort + (ascending ? " ASC" : " DESC");

    QSqlQuery q(m_db);
    q.prepare(sql);
    q.addBindValue(workspaceId);

    if (!q.exec()) return result;
    while (q.next()) result.append(taskFromQuery(q));
    return result;
}

// ========== Tag CRUD ==========

QVector<Tag> DatabaseManager::getTags(int workspaceId) {
    QVector<Tag> result;
    QSqlQuery q(m_db);
    q.prepare("SELECT id, workspace_id, name, color FROM tags WHERE workspace_id = ? ORDER BY name ASC");
    q.addBindValue(workspaceId);

    if (!q.exec()) return result;
    while (q.next()) {
        result.append(Tag(q.value(0).toInt(), q.value(1).toInt(),
                          q.value(2).toString(), q.value(3).toString()));
    }
    return result;
}

std::optional<Tag> DatabaseManager::createTag(int workspaceId, const QString& name, const QString& color) {
    QSqlQuery q(m_db);
    q.prepare("INSERT INTO tags (workspace_id, name, color) VALUES (?, ?, ?)");
    q.addBindValue(workspaceId);
    q.addBindValue(name);
    q.addBindValue(color);

    if (!q.exec()) return std::nullopt;

    return Tag(q.lastInsertId().toInt(), workspaceId, name, color);
}

bool DatabaseManager::deleteTag(int id) {
    QSqlQuery q(m_db);
    q.prepare("DELETE FROM tags WHERE id = ?");
    q.addBindValue(id);
    return q.exec();
}

bool DatabaseManager::addTagToTask(int taskId, int tagId) {
    QSqlQuery q(m_db);
    q.prepare("INSERT OR IGNORE INTO task_tags (task_id, tag_id) VALUES (?, ?)");
    q.addBindValue(taskId);
    q.addBindValue(tagId);
    return q.exec();
}

bool DatabaseManager::removeTagFromTask(int taskId, int tagId) {
    QSqlQuery q(m_db);
    q.prepare("DELETE FROM task_tags WHERE task_id = ? AND tag_id = ?");
    q.addBindValue(taskId);
    q.addBindValue(tagId);
    return q.exec();
}

QVector<Tag> DatabaseManager::getTaskTags(int taskId) {
    QVector<Tag> result;
    QSqlQuery q(m_db);
    q.prepare("SELECT t.id, t.workspace_id, t.name, t.color FROM tags t "
              "JOIN task_tags tt ON t.id = tt.tag_id WHERE tt.task_id = ?");
    q.addBindValue(taskId);

    if (!q.exec()) return result;
    while (q.next()) {
        result.append(Tag(q.value(0).toInt(), q.value(1).toInt(),
                          q.value(2).toString(), q.value(3).toString()));
    }
    return result;
}
