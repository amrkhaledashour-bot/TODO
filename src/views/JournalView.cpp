#include "JournalView.h"

JournalView::JournalView(QWidget* parent) : QWidget(parent) {
    setupUi();
}

void JournalView::setupUi() {
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Sidebar for entries
    QWidget* sidebar = new QWidget(this);
    sidebar->setFixedWidth(250);
    sidebar->setObjectName("journalSidebar");
    QVBoxLayout* sideLayout = new QVBoxLayout(sidebar);
    
    m_backBtn = new QPushButton("← Back to Board", this);
    m_backBtn->setObjectName("backBtn");
    m_backBtn->setCursor(Qt::PointingHandCursor);
    sideLayout->addWidget(m_backBtn);

    sideLayout->addSpacing(10);
    
    m_newBtn = new QPushButton("+ New Entry", this);
    m_newBtn->setObjectName("primaryBtn");
    sideLayout->addWidget(m_newBtn);

    m_entryList = new QListWidget(this);
    m_entryList->setObjectName("journalList");
    sideLayout->addWidget(m_entryList);

    mainLayout->addWidget(sidebar);

    // Editor area
    QWidget* editorArea = new QWidget(this);
    editorArea->setObjectName("journalEditorArea");
    QVBoxLayout* editLayout = new QVBoxLayout(editorArea);
    editLayout->setContentsMargins(40, 40, 40, 40);
    editLayout->setSpacing(20);

    m_titleEdit = new QLineEdit(this);
    m_titleEdit->setPlaceholderText("Untitled Entry");
    m_titleEdit->setObjectName("journalTitleEdit");
    QFont titleFont;
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    m_titleEdit->setFont(titleFont);
    editLayout->addWidget(m_titleEdit);

    m_contentEdit = new QTextEdit(this);
    m_contentEdit->setPlaceholderText("Start writing...");
    m_contentEdit->setObjectName("journalContentEdit");
    editLayout->addWidget(m_contentEdit, 1);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    m_deleteBtn = new QPushButton("Delete", this);
    m_deleteBtn->setObjectName("deleteBtn");
    
    m_saveBtn = new QPushButton("Save Changes", this);
    m_saveBtn->setObjectName("primaryBtn");
    m_saveBtn->setFixedWidth(120);

    btnLayout->addWidget(m_deleteBtn);
    btnLayout->addStretch();
    btnLayout->addWidget(m_saveBtn);
    editLayout->addLayout(btnLayout);

    mainLayout->addWidget(editorArea, 1);

    // Connections
    connect(m_backBtn, &QPushButton::clicked, this, &JournalView::backRequested);
    connect(m_entryList, &QListWidget::itemSelectionChanged, this, &JournalView::onEntrySelectionChanged);
    connect(m_saveBtn, &QPushButton::clicked, this, &JournalView::onSaveClicked);
    connect(m_deleteBtn, &QPushButton::clicked, this, &JournalView::onDeleteClicked);
    connect(m_newBtn, &QPushButton::clicked, this, &JournalView::createNewEntryRequested);
}

void JournalView::setEntries(const QVector<JournalEntry>& entries) {
    m_entryList->clear();
    for (const auto& entry : entries) {
        QListWidgetItem* item = new QListWidgetItem(entry.title(), m_entryList);
        item->setData(Qt::UserRole, entry.id());
        item->setData(Qt::UserRole + 1, entry.content());
    }
}

void JournalView::clearEditor() {
    m_currentEntryId = -1;
    m_titleEdit->clear();
    m_contentEdit->clear();
}

void JournalView::onEntrySelectionChanged() {
    QListWidgetItem* item = m_entryList->currentItem();
    if (item) {
        m_currentEntryId = item->data(Qt::UserRole).toInt();
        m_titleEdit->setText(item->text());
        m_contentEdit->setHtml(item->data(Qt::UserRole + 1).toString());
        emit entrySelected(m_currentEntryId);
    }
}

void JournalView::onSaveClicked() {
    emit saveEntryRequested(m_titleEdit->text(), m_contentEdit->toHtml());
}

void JournalView::onDeleteClicked() {
    if (m_currentEntryId > 0) {
        emit deleteEntryRequested(m_currentEntryId);
    }
}
