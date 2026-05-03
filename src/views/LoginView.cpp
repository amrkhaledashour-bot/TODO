#include "LoginView.h"
#include <QGraphicsDropShadowEffect>
#include <QKeyEvent>

LoginView::LoginView(QWidget* parent) : QWidget(parent) {
    setupUi();
}

void LoginView::setupUi() {
    setObjectName("loginView");

    // main centered layout
    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->setAlignment(Qt::AlignCenter);

    // card container — gives that floating card feel
    auto* card = new QWidget(this);
    card->setObjectName("authCard");
    card->setFixedSize(400, 460);

    // subtle shadow behind the card
    auto* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(40);
    shadow->setColor(QColor(0, 0, 0, 60));
    shadow->setOffset(0, 8);
    card->setGraphicsEffect(shadow);

    auto* cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(40, 40, 40, 40);
    cardLayout->setSpacing(16);

    // app branding
    m_titleLabel = new QLabel("✏️ Notion Todo", card);
    m_titleLabel->setObjectName("authTitle");
    m_titleLabel->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(m_titleLabel);

    // subtitle
    auto* subtitle = new QLabel("Welcome back! Sign in to continue.", card);
    subtitle->setObjectName("authSubtitle");
    subtitle->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(subtitle);

    cardLayout->addSpacing(12);

    // username field
    auto* userLabel = new QLabel("Username", card);
    userLabel->setObjectName("inputLabel");
    cardLayout->addWidget(userLabel);

    m_usernameInput = new QLineEdit(card);
    m_usernameInput->setObjectName("authInput");
    m_usernameInput->setPlaceholderText("Enter your username");
    m_usernameInput->setMinimumHeight(40);
    cardLayout->addWidget(m_usernameInput);

    // password field
    auto* passLabel = new QLabel("Password", card);
    passLabel->setObjectName("inputLabel");
    cardLayout->addWidget(passLabel);

    m_passwordInput = new QLineEdit(card);
    m_passwordInput->setObjectName("authInput");
    m_passwordInput->setPlaceholderText("Enter your password");
    m_passwordInput->setEchoMode(QLineEdit::Password);
    m_passwordInput->setMinimumHeight(40);
    cardLayout->addWidget(m_passwordInput);

    // error label (hidden by default)
    m_errorLabel = new QLabel(card);
    m_errorLabel->setObjectName("authError");
    m_errorLabel->setAlignment(Qt::AlignCenter);
    m_errorLabel->setWordWrap(true);
    m_errorLabel->setVisible(false);
    cardLayout->addWidget(m_errorLabel);

    cardLayout->addSpacing(8);

    // login button
    m_loginBtn = new QPushButton("Sign In", card);
    m_loginBtn->setObjectName("authPrimaryBtn");
    m_loginBtn->setMinimumHeight(44);
    m_loginBtn->setCursor(Qt::PointingHandCursor);
    cardLayout->addWidget(m_loginBtn);

    // switch to register
    m_registerLink = new QPushButton("Don't have an account? Sign Up", card);
    m_registerLink->setObjectName("authLinkBtn");
    m_registerLink->setCursor(Qt::PointingHandCursor);
    m_registerLink->setFlat(true);
    cardLayout->addWidget(m_registerLink);

    outerLayout->addWidget(card);

    // wire up the login button
    connect(m_loginBtn, &QPushButton::clicked, this, [this]() {
        QString user = m_usernameInput->text().trimmed();
        QString pass = m_passwordInput->text();

        if (user.isEmpty() || pass.isEmpty()) {
            showError("Please fill in all fields.");
            return;
        }
        m_errorLabel->setVisible(false);
        emit loginRequested(user, pass);
    });

    // pressing enter in password field triggers login
    connect(m_passwordInput, &QLineEdit::returnPressed, m_loginBtn, &QPushButton::click);

    // switch to register
    connect(m_registerLink, &QPushButton::clicked, this, &LoginView::switchToRegister);
}

void LoginView::showError(const QString& message) {
    m_errorLabel->setText(message);
    m_errorLabel->setVisible(true);
}

void LoginView::reset() {
    m_usernameInput->clear();
    m_passwordInput->clear();
    m_errorLabel->setVisible(false);
    m_usernameInput->setFocus();
}
