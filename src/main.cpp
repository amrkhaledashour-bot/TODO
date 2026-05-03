#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include "views/MainWindow.h"
#include "database/DatabaseManager.h"

int main(int argc, char *argv[]) {
    // Enable High DPI support
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication app(argc, argv);
    app.setApplicationName("Notion Todo");
    app.setOrganizationName("NotionTodoApp");

    // Initialize database in the user's AppData directory
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir;
    if (!dir.exists(dataPath)) {
        dir.mkpath(dataPath);
    }
    
    QString dbPath = QDir(dataPath).filePath("notion_todo.db");

    if (!DatabaseManager::instance().initialize(dbPath)) {
        // Normally show an error dialog here
        return -1;
    }

    MainWindow window;
    window.show();

    int ret = app.exec();

    DatabaseManager::instance().shutdown();
    return ret;
}
