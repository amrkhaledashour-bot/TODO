#ifndef PASSWORD_HASHER_H
#define PASSWORD_HASHER_H

#include <QString>

/**
 * @brief Handles password hashing and verification.
 *
 * Uses SHA-256 with a random salt. Not the most secure option
 * for production (you'd want argon2 or bcrypt), but good enough
 * for a local desktop app without external dependencies.
 */
class PasswordHasher {
public:
    /**
     * @brief Generate a random salt string.
     * @return 32-character hex string
     */
    static QString generateSalt();

    /**
     * @brief Hash a password with the given salt.
     * @param password  Raw password from the user
     * @param salt      Salt to mix in before hashing
     * @return SHA-256 hex digest
     */
    static QString hashPassword(const QString& password, const QString& salt);

    /**
     * @brief Verify a password against a known hash.
     * @param password  Raw password to check
     * @param salt      Salt that was used originally
     * @param hash      The stored hash to compare against
     * @return true if the password matches
     */
    static bool verifyPassword(const QString& password, const QString& salt, const QString& hash);
};

#endif // PASSWORD_HASHER_H
