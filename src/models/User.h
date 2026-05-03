#ifndef USER_H
#define USER_H

#include <QString>
#include <QDateTime>

/**
 * @brief Represents a user account in the system.
 *
 * Simple data container — holds user info pulled from the database.
 * No business logic lives here, just fields and accessors.
 */
class User {
public:
    /** Default constructor — creates an empty/invalid user */
    User() : m_id(-1) {}

    /**
     * @brief Full constructor for building a user from DB data.
     * @param id        Database row id
     * @param username  Unique username
     * @param email     User's email address
     * @param hash      Hashed password (never store raw passwords!)
     * @param salt      Salt used for hashing
     * @param createdAt When the account was created
     */
    User(int id, const QString& username, const QString& email,
         const QString& hash, const QString& salt,
         const QDateTime& createdAt)
        : m_id(id), m_username(username), m_email(email),
          m_passwordHash(hash), m_salt(salt), m_createdAt(createdAt) {}

    // -- Getters --
    int id() const { return m_id; }
    QString username() const { return m_username; }
    QString email() const { return m_email; }
    QString passwordHash() const { return m_passwordHash; }
    QString salt() const { return m_salt; }
    QDateTime createdAt() const { return m_createdAt; }

    // -- Setters --
    void setId(int id) { m_id = id; }
    void setUsername(const QString& username) { m_username = username; }
    void setEmail(const QString& email) { m_email = email; }
    void setPasswordHash(const QString& hash) { m_passwordHash = hash; }
    void setSalt(const QString& salt) { m_salt = salt; }
    void setCreatedAt(const QDateTime& dt) { m_createdAt = dt; }

    /** Check if this user object actually has data */
    bool isValid() const { return m_id > 0; }

private:
    int m_id;
    QString m_username;
    QString m_email;
    QString m_passwordHash;
    QString m_salt;
    QDateTime m_createdAt;
};

#endif // USER_H
