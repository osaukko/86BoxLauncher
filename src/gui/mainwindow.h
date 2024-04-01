#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMenu>
#include <QWidget>

class MachineListModel;
class QHBoxLayout;
class QItemSelection;
class QListView;
class QToolBar;
class QToolButton;
class QVBoxLayout;
class Settings;

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onAddMachineTriggered();
    void onEditMachineTriggered();
    void onMachineSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void onRemoveMachineTriggered();
    void onShowPreferencesTriggered();

private:
    void setupUi();
    void saveMachines();
    void restoreMachines();

    // Settings manager
    Settings *mSettings{};

    // Layouts
    QVBoxLayout *mMainLayout{};
    QHBoxLayout *mToolbarLayout{};
    QHBoxLayout *mVmLayout{};

    // Virtual machines toolbar
    QAction *mAddAction{};
    QAction *mRemoveAction{};
    QAction *mSettingsAction{};
    QAction *mEditAction{};
    QToolButton *mSettingsButton{};
    QAction *mStartAction{};
    QToolBar *mMachinesToolbar{};

    // Preferences toolbar
    QAction *mPreferencesAction{};
    QToolBar *mPreferencesToolbar{};

    // Menus
    QMenu *mSettingsMenu{};

    // List view for virtual machines
    QListView *mVmView{};

    // Model for virtual machines
    MachineListModel *mVmModel{};
};

#endif // MAINWINDOW_H
