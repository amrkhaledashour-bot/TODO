#include "PasswordHasher.h"
#include <QCryptographicHash>
#include <QRandomGenerator>

/**
 * @brief Generate a random 16-byte salt, returned as a 32-char hex string.
 */
QString PasswordHasher::generateSalt() {
    // grab 16 random bytes and convert to hex
    QByteArray saltBytes;
    saltBytes.resize(16);
    for (int i = 0; i < 16; ++i) {
        saltBytes[i] = static_cast<char>(QRandomGenerator::global()->bounded(256));
    }
    return QString(saltBytes.toHex());
}

/**
 * @brief Hash password by prepending salt and running SHA-256.
 */
QString PasswordHasher::hashPassword(const QString& password, const QString& salt) {
    // simple approach: concatenate salt + password, then hash
    QString combined = salt + password;
    QByteArray hash = QCryptographicHash::hash(
        combined.toUtf8(), QCryptographicHash::Sha256
    );
    return QString(hash.toHex());
}

/**
 * @brief Verify by re-hashing and comparing.
 */
bool PasswordHasher::verifyPassword(const QString& password, const QString& salt, const QString& hash) {
    // just hash the input the same way and see if it matches
    return hashPassword(password, salt) == hash;
}
