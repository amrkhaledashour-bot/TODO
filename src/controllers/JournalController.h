#ifndef JOURNAL_CONTROLLER_H
#define JOURNAL_CONTROLLER_H

#include <QObject>
#include <QVector>
#include <optional>
#include "../models/JournalEntry.h"
#include "../database/AsyncExecutor.h"

/**
 * @brief Controller for managing journal entries asynchronously.
 */
class JournalController : public QObject {
    Q_OBJECT
public:
    explicit JournalController(QObject* parent = nullptr);

    /** Load all entries for a workspace. */
    void loadEntries(int workspaceId);

    /** Create a new entry. */
    void createEntry(int workspaceId, const QString& title, 
                     const QString& content, const QDateTime& date);

    /** Update an existing entry. */
    void updateEntry(const JournalEntry& entry);

    /** Delete an entry. */
    void deleteEntry(int id);

signals:
    void entriesLoaded(const QVector<JournalEntry>& entries);
    void entryCreated(const JournalEntry& entry);
    void entryUpdated(const JournalEntry& entry);
    void entryDeleted(int id);
    void error(const QString& message);

private:
    AsyncExecutor* m_executor;
};

#endif // JOURNAL_CONTROLLER_H
