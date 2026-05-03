#ifndef REGISTER_VIEW_H
#define REGISTER_VIEW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

/**
 * @brief Registration screen for new users.
 *
 * Same visual style as LoginView — centered card with fields
 * for username, email, password, confirm password.
 */
class RegisterView : public QWidget {
    Q_OBJECT

public:
    explicit RegisterView(QWidget* parent = nullptr);

    /** Show an error message below the form. */
    void showError(const QString& message);

    /** Clear everything. */
    void reset();

signals:
    void registerRequested(const QString& username, const QString& email, const QString& password);
    void switchToLogin();

private:
    void setupUi();

    QLineEdit* m_usernameInput;
    QLineEdit* m_emailInput;
    QLineEdit* m_passwordInput;
    QLineEdit* m_confirmInput;
    QPushButton* m_registerBtn;
    QPushButton* m_loginLink;
    QLabel* m_errorLabel;
};

#endif // REGISTER_VIEW_H
