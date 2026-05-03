#ifndef TASK_CARD_H
#define TASK_CARD_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "../models/Task.h"
#include "../models/Tag.h"

/**
 * @brief A visual card representing a single task.
 *
 * Shows the task title, priority badge, due date, and tag chips.
 * Used inside TaskColumn list items. Has hover effects for polish.
 */
class TaskCard : public QWidget {
    Q_OBJECT

public:
    explicit TaskCard(const Task& task, QWidget* parent = nullptr);

    /** Update the card with new task data. */
    void setTask(const Task& task);

    /** Set the tags displayed as chips. */
    void setTags(const QVector<Tag>& tags);

    /** Get the task id this card represents. */
    int taskId() const { return m_task.id(); }

    /** Get the full task data. */
    Task task() const { return m_task; }

signals:
    void clicked(int taskId);
    void doubleClicked(int taskId);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    void setupUi();
    void updateContent();
    QWidget* createPriorityBadge(TaskPriority priority);

    Task m_task;
    QLabel* m_titleLabel;
    QLabel* m_dueDateLabel;
    QWidget* m_priorityBadge;
    QHBoxLayout* m_tagsLayout;
    QVector<Tag> m_tags;
};

#endif // TASK_CARD_H
