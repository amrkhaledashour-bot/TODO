#include "ThemeManager.h"
#include <QApplication>
#include <QFile>
#include <QSettings>
#include <QFont>

ThemeManager::ThemeManager(QObject* parent) : QObject(parent) {}

void ThemeManager::setTheme(Theme theme) {
    m_currentTheme = theme;

    // pick the right stylesheet
    QString path = (theme == Dark) ? ":/styles/dark.qss" : ":/styles/light.qss";

    QFile file(path);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QString styleSheet = file.readAll();
        qApp->setStyleSheet(styleSheet);
        file.close();
    }

    // save the choice for next launch
    QSettings settings("NotionTodo", "NotionTodo");
    settings.setValue("theme", (theme == Dark) ? "dark" : "light");

    emit themeChanged(theme);
}

void ThemeManager::toggleTheme() {
    setTheme(m_currentTheme == Dark ? Light : Dark);
}

void ThemeManager::loadSavedTheme() {
    QSettings settings("NotionTodo", "NotionTodo");
    QString saved = settings.value("theme", "dark").toString();

    // set a nice default font for the whole app
    QFont appFont("Segoe UI", 10);
    qApp->setFont(appFont);

    setTheme(saved == "light" ? Light : Dark);
}
