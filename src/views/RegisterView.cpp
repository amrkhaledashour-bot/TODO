#include "RegisterView.h"
#include <QVBoxLayout>
#include <QGraphicsDropShadowEffect>

RegisterView::RegisterView(QWidget* parent) : QWidget(parent) {
    setupUi();
}

void RegisterView::setupUi() {
    setObjectName("registerView");

    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->setAlignment(Qt::AlignCenter);

    // floating card
    auto* card = new QWidget(this);
    card->setObjectName("authCard");
    card->setFixedSize(400, 560);

    auto* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(40);
    shadow->setColor(QColor(0, 0, 0, 60));
    shadow->setOffset(0, 8);
    card->setGraphicsEffect(shadow);

    auto* layout = new QVBoxLayout(card);
    layout->setContentsMargins(40, 36, 40, 36);
    layout->setSpacing(12);

    // title
    auto* title = new QLabel("✏️ Create Account", card);
    title->setObjectName("authTitle");
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    auto* subtitle = new QLabel("Join us and start organizing your tasks.", card);
    subtitle->setObjectName("authSubtitle");
    subtitle->setAlignment(Qt::AlignCenter);
    layout->addWidget(subtitle);

    layout->addSpacing(8);

    // username
    auto* userLabel = new QLabel("Username", card);
    userLabel->setObjectName("inputLabel");
    layout->addWidget(userLabel);

    m_usernameInput = new QLineEdit(card);
    m_usernameInput->setObjectName("authInput");
    m_usernameInput->setPlaceholderText("Choose a username");
    m_usernameInput->setMinimumHeight(40);
    layout->addWidget(m_usernameInput);

    // email
    auto* emailLabel = new QLabel("Email", card);
    emailLabel->setObjectName("inputLabel");
    layout->addWidget(emailLabel);

    m_emailInput = new QLineEdit(card);
    m_emailInput->setObjectName("authInput");
    m_emailInput->setPlaceholderText("you@example.com");
    m_emailInput->setMinimumHeight(40);
    layout->addWidget(m_emailInput);

    // password
    auto* passLabel = new QLabel("Password", card);
    passLabel->setObjectName("inputLabel");
    layout->addWidget(passLabel);

    m_passwordInput = new QLineEdit(card);
    m_passwordInput->setObjectName("authInput");
    m_passwordInput->setPlaceholderText("Create a password");
    m_passwordInput->setEchoMode(QLineEdit::Password);
    m_passwordInput->setMinimumHeight(40);
    layout->addWidget(m_passwordInput);

    // confirm password
    auto* confirmLabel = new QLabel("Confirm Password", card);
    confirmLabel->setObjectName("inputLabel");
    layout->addWidget(confirmLabel);

    m_confirmInput = new QLineEdit(card);
    m_confirmInput->setObjectName("authInput");
    m_confirmInput->setPlaceholderText("Repeat your password");
    m_confirmInput->setEchoMode(QLineEdit::Password);
    m_confirmInput->setMinimumHeight(40);
    layout->addWidget(m_confirmInput);

    // error
    m_errorLabel = new QLabel(card);
    m_errorLabel->setObjectName("authError");
    m_errorLabel->setAlignment(Qt::AlignCenter);
    m_errorLabel->setWordWrap(true);
    m_errorLabel->setVisible(false);
    layout->addWidget(m_errorLabel);

    layout->addSpacing(4);

    // register button
    m_registerBtn = new QPushButton("Create Account", card);
    m_registerBtn->setObjectName("authPrimaryBtn");
    m_registerBtn->setMinimumHeight(44);
    m_registerBtn->setCursor(Qt::PointingHandCursor);
    layout->addWidget(m_registerBtn);

    // link back to login
    m_loginLink = new QPushButton("Already have an account? Sign In", card);
    m_loginLink->setObjectName("authLinkBtn");
    m_loginLink->setCursor(Qt::PointingHandCursor);
    m_loginLink->setFlat(true);
    layout->addWidget(m_loginLink);

    outerLayout->addWidget(card);

    // validation and signal
    connect(m_registerBtn, &QPushButton::clicked, this, [this]() {
        QString user = m_usernameInput->text().trimmed();
        QString email = m_emailInput->text().trimmed();
        QString pass = m_passwordInput->text();
        QString confirm = m_confirmInput->text();

        if (user.isEmpty() || email.isEmpty() || pass.isEmpty()) {
            showError("Please fill in all fields.");
            return;
        }
        if (pass.length() < 6) {
            showError("Password must be at least 6 characters.");
            return;
        }
        if (pass != confirm) {
            showError("Passwords don't match.");
            return;
        }
        m_errorLabel->setVisible(false);
        emit registerRequested(user, email, pass);
    });

    connect(m_confirmInput, &QLineEdit::returnPressed, m_registerBtn, &QPushButton::click);
    connect(m_loginLink, &QPushButton::clicked, this, &RegisterView::switchToLogin);
}

void RegisterView::showError(const QString& message) {
    m_errorLabel->setText(message);
    m_errorLabel->setVisible(true);
}

void RegisterView::reset() {
    m_usernameInput->clear();
    m_emailInput->clear();
    m_passwordInput->clear();
    m_confirmInput->clear();
    m_errorLabel->setVisible(false);
    m_usernameInput->setFocus();
}
