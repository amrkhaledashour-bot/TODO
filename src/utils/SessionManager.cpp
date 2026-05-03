#include "SessionManager.h"
#include <QSettings>
#include <QUuid>

/**
 * @brief Create a fresh token using a random UUID.
 */
QString SessionManager::generateToken() {
    // QUuid gives us a nice random unique string
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

/**
 * @brief Persist the token to disk via QSettings.
 */
void SessionManager::saveSession(const QString& token) {
    QSettings settings("NotionTodo", "NotionTodo");
    settings.setValue("session/token", token);
}

/**
 * @brief Try to load a previously saved token.
 */
QString SessionManager::loadSession() {
    QSettings settings("NotionTodo", "NotionTodo");
    return settings.value("session/token", "").toString();
}

/**
 * @brief Wipe the saved token — used on logout.
 */
void SessionManager::clearSession() {
    QSettings settings("NotionTodo", "NotionTodo");
    settings.remove("session/token");
}
