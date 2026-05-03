#ifndef LOGIN_VIEW_H
#define LOGIN_VIEW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

/**
 * @brief The login screen — first thing users see.
 *
 * Clean centered form with username/password fields, a login button,
 * and a link to switch to registration. Emits signals for the
 * main window to wire up to AuthController.
 */
class LoginView : public QWidget {
    Q_OBJECT

public:
    explicit LoginView(QWidget* parent = nullptr);

    /** Show an error message below the form. */
    void showError(const QString& message);

    /** Clear all fields and errors. */
    void reset();

signals:
    void loginRequested(const QString& username, const QString& password);
    void switchToRegister();

private:
    void setupUi();

    QLineEdit* m_usernameInput;
    QLineEdit* m_passwordInput;
    QPushButton* m_loginBtn;
    QPushButton* m_registerLink;
    QLabel* m_errorLabel;
    QLabel* m_titleLabel;
};

#endif // LOGIN_VIEW_H
