// Copyright (C) 2023 Ossi Saukko <osaukko@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file  main.cpp
 * @brief The entry point for the program
 */

#include "gui/mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QIcon>

void setIconTheme();

/**
 * @brief The entry point for the program
 * @param[in] argc   Argument count
 * @param[in] argv   Argument values
 * @return Zero if the program exits without problems
 */
int main(int argc, char *argv[])
{
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
    QApplication const app(argc, argv);

    setIconTheme();

    MainWindow mainWindow;
    mainWindow.show();

    return QApplication::exec();
}

/**
 * @brief Set the icon themes of the program
 * 
 * Selects the search folders for icons according to the operating system.
 * In addition, if the `ICONS_PATH` environment variable is set, it will
 * be added to the search folders. This allows the use of an icon theme in
 * unusual cases.
 * 
 * We set the application's icon theme as the main theme. On Linux, we use
 * the desktop's current theme as a fallback theme, and on Windows, we use
 * the default palette to choose between the breeze or breeze-dark theme.
 */
void setIconTheme()
{
    QStringList themeSearchPaths = QIcon::themeSearchPaths();
    QStringList checkPaths = {QApplication::applicationDirPath() + "/icons"};

#ifdef Q_OS_UNIX
    checkPaths.append("/usr/share/86BoxLauncher/icons");
    checkPaths.append("/usr/local/share/86BoxLauncher/icons");
    auto fromEnv = QString::fromLocal8Bit(qgetenv("ICONS_PATH"));
    if (!fromEnv.isEmpty()) {
        checkPaths.append(fromEnv);
    }
#endif

#ifdef Q_OS_WINDOWS
    auto fromEnv = qEnvironmentVariable("ICONS_PATH");
    if (!fromEnv.isEmpty()) {
        checkPaths.append(fromEnv);
    }
#endif

    for (const auto &path : checkPaths) {
        if (QFile::exists(path)) {
            themeSearchPaths.append(path);
        }
    }

    QIcon::setThemeSearchPaths(themeSearchPaths);
#ifdef Q_OS_WINDOWS
    const auto palette = QApplication::palette();
    if (palette.window().color().value() < palette.windowText().color().value()) {
        QIcon::setFallbackThemeName("breeze-dark");
    } else {
        QIcon::setFallbackThemeName("breeze");
    }
#else
    QIcon::setFallbackThemeName(QIcon::themeName());
#endif
    QIcon::setThemeName("86BoxLauncher");

    qDebug() << "Icon theme search paths:" << themeSearchPaths;
    qDebug() << "Icon theme:" << QIcon::themeName();
    qDebug() << "Fallback icon theme:" << QIcon::fallbackThemeName();
}
