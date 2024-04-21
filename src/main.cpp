#include <QApplication>
#include <QDebug>
#include <QFile>

#include "gui/mainwindow.h"

void setIconTheme();

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
    QIcon::setThemeName("86BoxLauncher");
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

    qDebug() << "Icon theme search paths:" << themeSearchPaths;
    qDebug() << "Icon theme:" << QIcon::themeName();
    qDebug() << "Fallback icon theme:" << QIcon::fallbackThemeName();
}
