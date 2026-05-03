#include "TaskEditorDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFormLayout>
#include <QDateTime>

TaskEditorDialog::TaskEditorDialog(QWidget* parent)
    : QDialog(parent) {
    setupUi();
    setWindowTitle("Task Editor");
    resize(400, 500);
}

void TaskEditorDialog::setupUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    QFormLayout* formLayout = new QFormLayout();
    formLayout->setSpacing(15);

    m_titleInput = new QLineEdit(this);
    m_titleInput->setPlaceholderText("Task Title...");
    m_titleInput->setObjectName("taskEditorTitle");
    formLayout->addRow("Title:", m_titleInput);

    m_descInput = new QTextEdit(this);
    m_descInput->setPlaceholderText("Description...");
    m_descInput->setObjectName("taskEditorDesc");
    formLayout->addRow("Description:", m_descInput);

    m_statusCombo = new QComboBox(this);
    m_statusCombo->addItem("To Do");
    m_statusCombo->addItem("Doing");
    m_statusCombo->addItem("Done");
    formLayout->addRow("Status:", m_statusCombo);

    m_priorityCombo = new QComboBox(this);
    m_priorityCombo->addItem("Low");
    m_priorityCombo->addItem("Medium");
    m_priorityCombo->addItem("High");
    formLayout->addRow("Priority:", m_priorityCombo);

    m_startDateInput = new QDateTimeEdit(QDateTime::currentDateTime(), this);
    m_startDateInput->setCalendarPopup(true);
    m_startDateInput->setDisplayFormat("MMM dd yyyy - hh:mm AP");
    formLayout->addRow("Start Time:", m_startDateInput);

    m_endDateInput = new QDateTimeEdit(QDateTime::currentDateTime().addSecs(3600), this);
    m_endDateInput->setCalendarPopup(true);
    m_endDateInput->setDisplayFormat("MMM dd yyyy - hh:mm AP");
    formLayout->addRow("End Time:", m_endDateInput);

    mainLayout->addLayout(formLayout);

    // Buttons
    QHBoxLayout* btnLayout = new QHBoxLayout();
    m_cancelBtn = new QPushButton("Cancel", this);
    m_saveBtn = new QPushButton("Save Task", this);
    m_saveBtn->setObjectName("primaryBtn");

    btnLayout->addStretch();
    btnLayout->addWidget(m_cancelBtn);
    btnLayout->addWidget(m_saveBtn);

    mainLayout->addLayout(btnLayout);

    connect(m_saveBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

void TaskEditorDialog::initForCreate(TaskStatus defaultStatus) {
    m_task = Task();
    m_titleInput->clear();
    m_descInput->clear();
    m_statusCombo->setCurrentIndex(static_cast<int>(defaultStatus));
    m_priorityCombo->setCurrentIndex(1); // Medium

    // Round current time to nearest half hour
    QDateTime now = QDateTime::currentDateTime();
    int mins = now.time().minute();
    int addMins = (30 - (mins % 30)) % 30;
    QDateTime start = now.addSecs(addMins * 60);
    
    m_startDateInput->setDateTime(start);
    m_endDateInput->setDateTime(start.addSecs(3600)); // 1 hour duration
}

void TaskEditorDialog::initForEdit(const Task& task) {
    m_task = task;
    m_titleInput->setText(task.title());
    m_descInput->setText(task.description());
    m_statusCombo->setCurrentIndex(static_cast<int>(task.status()));
    m_priorityCombo->setCurrentIndex(static_cast<int>(task.priority()));
    
    if (task.startDate().isValid()) {
        m_startDateInput->setDateTime(task.startDate());
    } else {
        m_startDateInput->setDateTime(QDateTime::currentDateTime());
    }

    if (task.endDate().isValid()) {
        m_endDateInput->setDateTime(task.endDate());
    } else {
        m_endDateInput->setDateTime(QDateTime::currentDateTime().addSecs(3600));
    }
}

Task TaskEditorDialog::getTaskData() const {
    Task t = m_task;
    t.setTitle(m_titleInput->text().isEmpty() ? "Untitled Task" : m_titleInput->text());
    t.setDescription(m_descInput->toPlainText());
    t.setStatus(static_cast<TaskStatus>(m_statusCombo->currentIndex()));
    t.setPriority(static_cast<TaskPriority>(m_priorityCombo->currentIndex()));
    t.setStartDate(m_startDateInput->dateTime());
    t.setEndDate(m_endDateInput->dateTime());
    return t;
}
