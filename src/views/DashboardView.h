#ifndef DASHBOARD_VIEW_H
#define DASHBOARD_VIEW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QTime>
#include "../models/Workspace.h"

/**
 * @brief Dashboard / workspace selector screen.
 *
 * After login, users land here to pick a workspace or create a new one.
 * Shows workspace cards in a grid with their icons and descriptions.
 */
class DashboardView : public QWidget {
    Q_OBJECT

public:
    explicit DashboardView(QWidget* parent = nullptr);

    /** Populate the grid with workspace cards. */
    void setWorkspaces(const QVector<Workspace>& workspaces);

    /** Update the greeting with the user's name. */
    void setUsername(const QString& username);

signals:
    void workspaceSelected(int workspaceId);
    void createWorkspaceClicked();
    void logoutClicked();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    void setupUi();
    QWidget* createWorkspaceCard(const Workspace& ws);
    void clearGrid();

    QLabel* m_greetingLabel;
    QGridLayout* m_gridLayout;
    QWidget* m_gridContainer;
    QPushButton* m_logoutBtn;
    QPushButton* m_newWorkspaceBtn;
};

#endif // DASHBOARD_VIEW_H
