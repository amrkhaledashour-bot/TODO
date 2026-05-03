#include "JournalController.h"
#include "../database/DatabaseManager.h"

JournalController::JournalController(QObject* parent) 
    : QObject(parent), m_executor(new AsyncExecutor(this)) {}

void JournalController::loadEntries(int workspaceId) {
    auto future = m_executor->run<QVector<JournalEntry>>([workspaceId]() {
        return DatabaseManager::instance().getJournalEntries(workspaceId);
    });

    connect(future, &QFutureWatcher<QVector<JournalEntry>>::finished, this, [this, future]() {
        emit entriesLoaded(future->result());
        future->deleteLater();
    });
}

void JournalController::createEntry(int workspaceId, const QString& title, 
                                   const QString& content, const QDateTime& date) {
    auto future = m_executor->run<std::optional<JournalEntry>>([=]() {
        return DatabaseManager::instance().createJournalEntry(workspaceId, title, content, date);
    });

    connect(future, &QFutureWatcher<std::optional<JournalEntry>>::finished, this, [this, future]() {
        if (future->result().has_value()) {
            emit entryCreated(future->result().value());
        } else {
            emit error("Failed to create journal entry.");
        }
        future->deleteLater();
    });
}

void JournalController::updateEntry(const JournalEntry& entry) {
    auto future = m_executor->run<bool>([entry]() {
        return DatabaseManager::instance().updateJournalEntry(entry);
    });

    connect(future, &QFutureWatcher<bool>::finished, this, [this, entry, future]() {
        if (future->result()) {
            emit entryUpdated(entry);
        } else {
            emit error("Failed to update journal entry.");
        }
        future->deleteLater();
    });
}

void JournalController::deleteEntry(int id) {
    auto future = m_executor->run<bool>([id]() {
        return DatabaseManager::instance().deleteJournalEntry(id);
    });

    connect(future, &QFutureWatcher<bool>::finished, this, [this, id, future]() {
        if (future->result()) {
            emit entryDeleted(id);
        } else {
            emit error("Failed to delete journal entry.");
        }
        future->deleteLater();
    });
}
