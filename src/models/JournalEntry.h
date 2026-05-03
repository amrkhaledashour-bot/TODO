#ifndef JOURNAL_ENTRY_H
#define JOURNAL_ENTRY_H

#include <QString>
#include <QDateTime>

/**
 * @brief Represents a single journal entry in a workspace.
 * 
 * Each entry has a title, rich-text content, and an associated date.
 */
class JournalEntry {
public:
    /**
     * @brief Default constructor for an invalid entry.
     */
    JournalEntry() : m_id(-1), m_workspaceId(-1) {}

    /**
     * @brief Constructs a JournalEntry with initial data.
     * 
     * @param id Database ID.
     * @param workspaceId ID of the workspace this entry belongs to.
     * @param title Entry title.
     * @param content Entry content (HTML or plain text).
     * @param date The date for this entry.
     */
    JournalEntry(int id, int workspaceId, const QString& title, 
                 const QString& content, const QDateTime& date)
        : m_id(id), m_workspaceId(workspaceId), m_title(title),
          m_content(content), m_date(date) {}

    // Getters
    int id() const { return m_id; }
    int workspaceId() const { return m_workspaceId; }
    QString title() const { return m_title; }
    QString content() const { return m_content; }
    QDateTime date() const { return m_date; }

    // Setters
    void setId(int id) { m_id = id; }
    void setWorkspaceId(int workspaceId) { m_workspaceId = workspaceId; }
    void setTitle(const QString& title) { m_title = title; }
    void setContent(const QString& content) { m_content = content; }
    void setDate(const QDateTime& date) { m_date = date; }

    /** @return True if the entry has a valid database ID. */
    bool isValid() const { return m_id > 0; }

private:
    int m_id;
    int m_workspaceId;
    QString m_title;
    QString m_content;
    QDateTime m_date;
};

#endif // JOURNAL_ENTRY_H
