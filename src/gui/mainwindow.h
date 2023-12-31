#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

class QHBoxLayout;
class QListView;
class QToolBar;
class QVBoxLayout;

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

signals:

private:
    void setupUi();

    // Layouts
    QVBoxLayout *mainLayout{};
    QHBoxLayout *toolbarLayout{};
    QHBoxLayout *vmLayout{};

    // Commands toolbar
    QAction *newAction{};
    QAction *removeAction{};
    QAction *settingsAction{};
    QAction *startAction{};
    QToolBar *commandsToolbar{};

    // Configure toolbar
    QAction *configureAction{};
    QToolBar *configureToolbar{};

    // List view for virtual machines
    QListView *vmView{};
};

#endif // MAINWINDOW_H
