#ifndef JOURNAL_VIEW_H
#define JOURNAL_VIEW_H

#include <QWidget>
#include <QListWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "../models/JournalEntry.h"

/**
 * @brief View for writing and managing journal entries.
 */
class JournalView : public QWidget {
    Q_OBJECT
public:
    explicit JournalView(QWidget* parent = nullptr);

    /** Populate the list of entries. */
    void setEntries(const QVector<JournalEntry>& entries);

    /** Clear the editor. */
    void clearEditor();

signals:
    void entrySelected(int id);
    void saveEntryRequested(const QString& title, const QString& content);
    void deleteEntryRequested(int id);
    void createNewEntryRequested();
    void backRequested();

private slots:
    void onEntrySelectionChanged();
    void onSaveClicked();
    void onDeleteClicked();

private:
    void setupUi();

    QListWidget* m_entryList;
    QLineEdit* m_titleEdit;
    QTextEdit* m_contentEdit;
    QPushButton* m_saveBtn;
    QPushButton* m_deleteBtn;
    QPushButton* m_newBtn;
    QPushButton* m_backBtn;

    int m_currentEntryId = -1;
};

#endif // JOURNAL_VIEW_H
