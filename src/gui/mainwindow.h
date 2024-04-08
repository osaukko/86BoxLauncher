#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMenu>
#include <QWidget>

class Machine;
class MachineListModel;
class QAction;
class QFrame;
class QHBoxLayout;
class QItemSelection;
class QListView;
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
    void onAddClicked();
    void onEditClicked();
    void onMachineSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void onPreferencesClicked();
    void onRemoveClicked();
    void onSettingsClicked();
    void onStartClicked();
    void saveMachines();

private:
    static QToolButton *createToolButton(const QIcon &icon,
                                         const QString &text,
                                         QWidget *parent = nullptr);
    void restoreMachines();
    void runCommand(const QString &command, const Machine &machine);
    void setupUi();
    [[nodiscard]] QHash<QString, QString> variablesForMachine(const Machine &machine) const;

    // Settings manager
    Settings *mSettings{};

    // Layouts
    QVBoxLayout *mMainLayout{};
    QHBoxLayout *mToolBarLayout{};

    // Tool bar widgets
    QToolButton *mAddButton{};
    QToolButton *mStartButton{};
    QToolButton *mSettingsButton{};
    QFrame *mSeparatorLine{};
    QToolButton *mRemoveButton{};
    QToolButton *mPreferencesButton{};

    // Settings menu
    QMenu *mSettingsMenu{};
    QAction *mEditAction{};

    // List view for virtual machines
    QListView *mVmView{};

    // Model for virtual machines
    MachineListModel *mVmModel{};

    // Timer for saving changes on the model
    QTimer *mSaveTimer;
};

#endif // MAINWINDOW_H
