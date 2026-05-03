#ifndef WORKSPACE_CONTROLLER_H
#define WORKSPACE_CONTROLLER_H

#include <QObject>
#include <QVector>
#include "../models/Workspace.h"
#include "../database/AsyncExecutor.h"

/**
 * @brief Manages workspace CRUD with async database calls.
 */
class WorkspaceController : public QObject {
    Q_OBJECT

public:
    explicit WorkspaceController(AsyncExecutor* executor, QObject* parent = nullptr);

public slots:
    void loadWorkspaces(int userId);
    void createWorkspace(int userId, const QString& name, const QString& desc = "", const QString& icon = "📋");
    void updateWorkspace(const Workspace& ws);
    void deleteWorkspace(int id, int userId);

signals:
    void workspacesLoaded(const QVector<Workspace>& workspaces);
    void workspaceCreated(const Workspace& workspace);
    void workspaceUpdated();
    void workspaceDeleted();
    void error(const QString& message);

private:
    AsyncExecutor* m_executor;
};

#endif // WORKSPACE_CONTROLLER_H
