#ifndef AUTH_CONTROLLER_H
#define AUTH_CONTROLLER_H

#include <QObject>
#include "../models/User.h"
#include "../database/AsyncExecutor.h"

/**
 * @brief Handles login, registration, and session management.
 *
 * Emits signals so the UI can react to auth state changes
 * without blocking on database queries.
 */
class AuthController : public QObject {
    Q_OBJECT

public:
    explicit AuthController(AsyncExecutor* executor, QObject* parent = nullptr);

    /** Get the currently logged-in user (invalid if not logged in). */
    User currentUser() const { return m_currentUser; }
    bool isLoggedIn() const { return m_currentUser.isValid(); }

public slots:
    /** Attempt to log in with username + password. */
    void login(const QString& username, const QString& password);

    /** Register a new account. */
    void registerUser(const QString& username, const QString& email, const QString& password);

    /** Try to restore a previous session from saved token. */
    void restoreSession();

    /** Log out and clear the session. */
    void logout();

signals:
    void loginSuccess(const User& user);
    void loginFailed(const QString& error);
    void registerSuccess(const User& user);
    void registerFailed(const QString& error);
    void sessionRestored(const User& user);
    void sessionExpired();
    void loggedOut();

private:
    AsyncExecutor* m_executor;
    User m_currentUser;
};

#endif // AUTH_CONTROLLER_H
