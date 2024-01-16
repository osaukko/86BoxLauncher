#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMenu>
#include <QWidget>

class QHBoxLayout;
class QListView;
class QToolBar;
class QToolButton;
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
    QVBoxLayout *mMainLayout{};
    QHBoxLayout *mToolbarLayout{};
    QHBoxLayout *mVmLayout{};

    // Commands toolbar
    QAction *mNewAction{};
    QAction *mRemoveAction{};
    QAction *mSettingsAction{};
    QAction *mCommandSettingsAction{};
    QToolButton *mSettingsButton{};
    QAction *mStartAction{};
    QToolBar *mCommandsToolbar{};

    // Configure toolbar
    QAction *mConfigureAction{};
    QToolBar *mConfigureToolbar{};

    // Menus
    QMenu *mSettingsMenu{};

    // List view for virtual machines
    QListView *mVmView{};
};

#endif // MAINWINDOW_H
