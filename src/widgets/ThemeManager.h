#ifndef THEME_MANAGER_H
#define THEME_MANAGER_H

#include <QObject>
#include <QString>

/**
 * @brief Manages dark/light theme switching.
 *
 * Loads QSS stylesheets from resources and applies them
 * to the entire application. Saves preference in QSettings.
 */
class ThemeManager : public QObject {
    Q_OBJECT

public:
    enum Theme { Light, Dark };
    Q_ENUM(Theme)

    explicit ThemeManager(QObject* parent = nullptr);

    /** Apply a theme to the whole app. */
    void setTheme(Theme theme);

    /** Get the current theme. */
    Theme currentTheme() const { return m_currentTheme; }

    /** Toggle between light and dark. */
    void toggleTheme();

    /** Load whatever theme was saved last time. */
    void loadSavedTheme();

signals:
    void themeChanged(Theme newTheme);

private:
    Theme m_currentTheme = Theme::Dark;
};

#endif // THEME_MANAGER_H
