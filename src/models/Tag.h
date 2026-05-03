#ifndef TAG_H
#define TAG_H

#include <QString>

/**
 * @brief Represents a label/tag that can be attached to tasks.
 *
 * Tags are scoped to a workspace so different workspaces
 * can have their own set of labels without clashing.
 */
class Tag {
public:
    Tag() : m_id(-1), m_workspaceId(-1) {}

    Tag(int id, int workspaceId, const QString& name, const QString& color)
        : m_id(id), m_workspaceId(workspaceId),
          m_name(name), m_color(color) {}

    // -- Getters --
    int id() const { return m_id; }
    int workspaceId() const { return m_workspaceId; }
    QString name() const { return m_name; }
    QString color() const { return m_color; }

    // -- Setters --
    void setId(int id) { m_id = id; }
    void setWorkspaceId(int wsId) { m_workspaceId = wsId; }
    void setName(const QString& name) { m_name = name; }
    void setColor(const QString& color) { m_color = color; }

    bool isValid() const { return m_id > 0; }

private:
    int m_id;
    int m_workspaceId;
    QString m_name;
    QString m_color;
};

#endif // TAG_H
