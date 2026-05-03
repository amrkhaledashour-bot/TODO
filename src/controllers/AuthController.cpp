#include "AuthController.h"
#include "../database/DatabaseManager.h"
#include "../utils/PasswordHasher.h"
#include "../utils/SessionManager.h"

AuthController::AuthController(AsyncExecutor* executor, QObject* parent)
    : QObject(parent), m_executor(executor) {}

void AuthController::login(const QString& username, const QString& password) {
    m_executor->run<std::optional<User>>(
        [username]() -> std::optional<User> {
            return DatabaseManager::instance().getUserByUsername(username);
        },
        [this, password](const std::optional<User>& userOpt) {
            if (!userOpt.has_value()) {
                emit loginFailed("User not found. Check your username.");
                return;
            }

            const User& user = userOpt.value();

            // verify the password against stored hash
            if (!PasswordHasher::verifyPassword(password, user.salt(), user.passwordHash())) {
                emit loginFailed("Wrong password. Try again.");
                return;
            }

            // create a session so they stay logged in
            QString token = SessionManager::generateToken();
            DatabaseManager::instance().createSession(user.id(), token);
            SessionManager::saveSession(token);

            m_currentUser = user;
            emit loginSuccess(user);
        }
    );
}

void AuthController::registerUser(const QString& username, const QString& email, const QString& password) {
    // hash the password before sending to DB
    QString salt = PasswordHasher::generateSalt();
    QString hash = PasswordHasher::hashPassword(password, salt);

    m_executor->run<std::optional<User>>(
        [username, email, hash, salt]() -> std::optional<User> {
            return DatabaseManager::instance().createUser(username, email, hash, salt);
        },
        [this](const std::optional<User>& userOpt) {
            if (!userOpt.has_value()) {
                emit registerFailed("Username or email already taken.");
                return;
            }

            const User& user = userOpt.value();

            // auto-login after registration
            QString token = SessionManager::generateToken();
            DatabaseManager::instance().createSession(user.id(), token);
            SessionManager::saveSession(token);

            m_currentUser = user;
            emit registerSuccess(user);
        }
    );
}

void AuthController::restoreSession() {
    QString token = SessionManager::loadSession();
    if (token.isEmpty()) {
        emit sessionExpired();
        return;
    }

    m_executor->run<std::optional<int>>(
        [token]() -> std::optional<int> {
            return DatabaseManager::instance().validateSession(token);
        },
        [this](const std::optional<int>& userIdOpt) {
            if (!userIdOpt.has_value()) {
                SessionManager::clearSession();
                emit sessionExpired();
                return;
            }

            // session is valid, load the user
            auto userOpt = DatabaseManager::instance().getUserById(userIdOpt.value());
            if (!userOpt.has_value()) {
                SessionManager::clearSession();
                emit sessionExpired();
                return;
            }

            m_currentUser = userOpt.value();
            emit sessionRestored(m_currentUser);
        }
    );
}

void AuthController::logout() {
    QString token = SessionManager::loadSession();
    if (!token.isEmpty()) {
        DatabaseManager::instance().deleteSession(token);
    }
    SessionManager::clearSession();
    m_currentUser = User();
    emit loggedOut();
}
