#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <QString>
#include <QDateTime>

/**
 * @brief Represents a workspace — the top-level container for projects.
 *
 * Think of it like a Notion workspace: each user can have multiple,
 * and each workspace holds its own set of projects + tags.
 */
class Workspace {
public:
    Workspace() : m_id(-1), m_userId(-1), m_sortOrder(0) {}

    Workspace(int id, int userId, const QString& name, const QString& description,
              const QString& icon, int sortOrder,
              const QDateTime& createdAt, const QDateTime& updatedAt)
        : m_id(id), m_userId(userId), m_name(name), m_description(description),
          m_icon(icon), m_sortOrder(sortOrder),
          m_createdAt(createdAt), m_updatedAt(updatedAt) {}

    // -- Getters --
    int id() const { return m_id; }
    int userId() const { return m_userId; }
    QString name() const { return m_name; }
    QString description() const { return m_description; }
    QString icon() const { return m_icon; }
    int sortOrder() const { return m_sortOrder; }
    QDateTime createdAt() const { return m_createdAt; }
    QDateTime updatedAt() const { return m_updatedAt; }

    // -- Setters --
    void setId(int id) { m_id = id; }
    void setUserId(int userId) { m_userId = userId; }
    void setName(const QString& name) { m_name = name; }
    void setDescription(const QString& desc) { m_description = desc; }
    void setIcon(const QString& icon) { m_icon = icon; }
    void setSortOrder(int order) { m_sortOrder = order; }
    void setCreatedAt(const QDateTime& dt) { m_createdAt = dt; }
    void setUpdatedAt(const QDateTime& dt) { m_updatedAt = dt; }

    bool isValid() const { return m_id > 0; }

private:
    int m_id;
    int m_userId;
    QString m_name;
    QString m_description;
    QString m_icon;
    int m_sortOrder;
    QDateTime m_createdAt;
    QDateTime m_updatedAt;
};

#endif // WORKSPACE_H
