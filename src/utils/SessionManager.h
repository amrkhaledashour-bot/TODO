#ifndef SESSION_MANAGER_H
#define SESSION_MANAGER_H

#include <QString>

/**
 * @brief Manages local session persistence using QSettings.
 *
 * When a user logs in we generate a session token and store it
 * both in the database and in QSettings. On next app launch we
 * check if there's a saved token and try to restore the session
 * so the user doesn't have to log in again.
 */
class SessionManager {
public:
    /**
     * @brief Generate a new unique session token.
     * @return UUID-based token string
     */
    static QString generateToken();

    /**
     * @brief Save a session token to persistent storage.
     * @param token The session token to save
     */
    static void saveSession(const QString& token);

    /**
     * @brief Load the previously saved session token.
     * @return Token string, or empty if no session was saved
     */
    static QString loadSession();

    /**
     * @brief Clear the saved session (logout).
     */
    static void clearSession();
};

#endif // SESSION_MANAGER_H
