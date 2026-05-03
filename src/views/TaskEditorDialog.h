#ifndef TASK_EDITOR_DIALOG_H
#define TASK_EDITOR_DIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QPushButton>

#include "../models/Task.h"

/**
 * @brief Dialog to create or edit a task.
 *
 * Provides a form interface for users to specify the task's title,
 * description, status, priority, and the time period (Start/End Date & Time).
 */
class TaskEditorDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Constructs the dialog.
     * @param parent The parent widget.
     */
    explicit TaskEditorDialog(QWidget* parent = nullptr);

    /**
     * @brief Initializes the dialog for creating a new task.
     * @param defaultStatus Pre-selects the column status where the user clicked "+ Add Task".
     */
    void initForCreate(TaskStatus defaultStatus = TaskStatus::Todo);

    /**
     * @brief Initializes the dialog for editing an existing task.
     * @param task The task data to pre-fill the form with.
     */
    void initForEdit(const Task& task);

    /**
     * @brief Retrieves the data entered by the user.
     * @return The resulting Task object.
     */
    Task getTaskData() const;

private:
    void setupUi();

    Task m_task; // the original task if editing, or an empty one

    QLineEdit* m_titleInput;
    QTextEdit* m_descInput;
    QComboBox* m_statusCombo;
    QComboBox* m_priorityCombo;
    QDateTimeEdit* m_startDateInput;
    QDateTimeEdit* m_endDateInput;
    QPushButton* m_saveBtn;
    QPushButton* m_cancelBtn;
};

#endif // TASK_EDITOR_DIALOG_H
