#include "WorkspaceController.h"
#include "../database/DatabaseManager.h"

WorkspaceController::WorkspaceController(AsyncExecutor* executor, QObject* parent)
    : QObject(parent), m_executor(executor) {}

void WorkspaceController::loadWorkspaces(int userId) {
    m_executor->run<QVector<Workspace>>(
        [userId]() { return DatabaseManager::instance().getWorkspaces(userId); },
        [this](const QVector<Workspace>& list) { emit workspacesLoaded(list); }
    );
}

void WorkspaceController::createWorkspace(int userId, const QString& name,
                                           const QString& desc, const QString& icon) {
    m_executor->run<std::optional<Workspace>>(
        [userId, name, desc, icon]() {
            return DatabaseManager::instance().createWorkspace(userId, name, desc, icon);
        },
        [this](const std::optional<Workspace>& ws) {
            if (ws.has_value()) emit workspaceCreated(ws.value());
            else emit error("Failed to create workspace.");
        }
    );
}

void WorkspaceController::updateWorkspace(const Workspace& ws) {
    m_executor->run<bool>(
        [ws]() { return DatabaseManager::instance().updateWorkspace(ws); },
        [this](bool ok) {
            if (ok) emit workspaceUpdated();
            else emit error("Failed to update workspace.");
        }
    );
}

void WorkspaceController::deleteWorkspace(int id, int userId) {
    m_executor->run<bool>(
        [id]() { return DatabaseManager::instance().deleteWorkspace(id); },
        [this, userId](bool ok) {
            if (ok) emit workspaceDeleted();
            else emit error("Failed to delete workspace.");
        }
    );
}
