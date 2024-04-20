#include <QApplication>
#include <QDebug>
#include <QFile>

#include "gui/mainwindow.h"

QStringList iconThemeSearchPaths()
{
    QStringList paths = QIcon::themeSearchPaths();

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
    auto fromEnv = qEnvironmentVariable("ICONS_PATH") if (!fromEnv.isEmpty())
    {
        checkPaths.append(fromEnv);
    }
#endif

    for (const auto &path : checkPaths) {
        if (QFile::exists(path)) {
            paths.append(path);
        }
    }

    qDebug() << "Icon theme search paths:" << paths;

    return paths;
}

int main(int argc, char *argv[])
{
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
    QApplication const app(argc, argv);

    QIcon::setThemeSearchPaths(iconThemeSearchPaths());
    QIcon::setFallbackThemeName(QIcon::themeName());
    QIcon::setThemeName("86BoxLauncher");

    MainWindow mainWindow;
    mainWindow.show();

    return QApplication::exec();
}
