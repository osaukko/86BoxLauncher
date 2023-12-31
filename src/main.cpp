#include <QApplication>
#include "gui/mainwindow.h"

int main(int argc, char *argv[])
{
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
    QApplication const app(argc, argv);

    MainWindow mainWindow;
    mainWindow.show();

    return QApplication::exec();
}
