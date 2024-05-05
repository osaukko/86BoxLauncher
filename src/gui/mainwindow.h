// Copyright (C) 2023 Ossi Saukko <osaukko@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file  mainwindow.h
 * @brief MainWindow class definition
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

class Machine;
class MachineListModel;
class QAction;
class QFrame;
class QHBoxLayout;
class QItemSelection;
class QListView;
class QMenu;
class QToolButton;
class QVBoxLayout;
class Settings;

/**
 * @brief Main window for the program
 * 
 * <img src="arch-linux-kde-plasma-6.png" alt="86BoxLauncher running on Arch Linux in KDE Plasma 6">
 * <img src="windows-10.png" alt="86BoxLauncher running on Windows 10">
 *
 * At the top of the window is a row of tool buttons. Below the tool
 * buttons is a list view displaying the user's 86Box emulation items. 
 * The list view is set to use MachineDelegate, which draws an icon, 
 * name, and summary for the emulation items.
 * 
 * This window will be active throughout the entire program. Other modal
 * dialogs are opened when needed. Emulation is started, or settings are
 * opened by asking the operating system to run the 86Box emulator with
 * the appropriate arguments.
 */
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
    void onContextMenuRequest(const QPoint &pos);
    void onEditClicked();
    void onMachineDoubleClicked(const QModelIndex &index);
    void onMachineSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void onPreferencesClicked();
    void onRemoveClicked();
    void onSettingsClicked();
    void onStartClicked();
    void saveMachines();

private:
    static QToolButton *createToolButton(QAction *action, QWidget *parent = nullptr);

    void restoreMachines();
    void runCommand(const QString &command, const Machine &machine);
    void setupUi();
    [[nodiscard]] QHash<QString, QString> variablesForMachine(const Machine &machine) const;

    /**
     * @brief Settings manager
     * 
     * This object is initialized in the setupUi() method when the main
     * window is created. The object is released at the same time with
     * the main window object. The pointer is temporarily given to the
     * prefernces dialog, but ownership remains with the main window.
     */
    Settings *mSettings{};

    /**
     * @brief Main layout
     *
     * Contains:
     * - Tool buttons layout
     * - List view for emulated machines
     */
    QVBoxLayout *mMainLayout{};

    /**
     * @brief Layout for tool buttons
     * 
     * Contains:
     * - Add button for creating or importing emulation setup
     * - Start button to launch selected emulation setup
     * - Settings button to launch 86Box only with the settings dialog
     * - Separator line to reduce accidental clicks to remove button
     * - Remove button to allow remove emulation setups
     * - Horizontal spacing
     * - Preferences button for opening the preferences dialog
     */
    QHBoxLayout *mToolBarLayout{};

    // Actions for buttons and menus
    QAction *mAddAction{};         /*!< @brief Add or import machine configuration */
    QAction *mEditAction{};        /*!< @brief Edit action to open the MachineDialog */
    QAction *mPreferencesAction{}; /*!< @brief Preferences for the 86BoxLauncher */
    QAction *mRemoveAction{};      /*!< @brief Remove selected machine item */
    QAction *mSettingsAction{};    /*!< @brief Launch settings dialog for selected machine */
    QAction *mStartAction{};       /*!< @brief Launch the 86Box emulator with selected machine */

    // Tool bar widgets
    QToolButton *mAddButton{};      /*!< @brief Button for adding or importing emulation setups */
    QToolButton *mStartButton{};    /*!< @brief Button for starting the emulation */
    QToolButton *mSettingsButton{}; /*!< @brief Button for opening 86Box settings dialog */
    QFrame *mSeparatorLine{};       /*!< @brief Line between settings button and remove button */
    QToolButton *mRemoveButton{};   /*!< @brief Button for removing emulation setup */
    QToolButton *mPreferencesButton{}; /*!< @brief Button for opening the preferences dialog */

    /**
     * @brief Alternative menu for the settings button
     *
     * This menu contains edit action that allows the user to open
     * MachineDialog and edit settings for selected emulation item.
     */
    QMenu *mSettingsMenu{};

    /**
     * @brief Context menu for list view
     *
     * This context menu is displayed when requested, and the list view has a selected item.
     */
    QMenu *mContextMenu{};

    /**
     * @brief List view for virtual machines
     */
    QListView *mVmView{};

    /**
     * @brief Model for virtual machines
     */
    MachineListModel *mVmModel{};

    /**
     * @brief Timer for saving changes on the model
     * 
     * This timer is designed to eliminate unnecessary writes. Any change
     * in the machine model will restart this timer. Usually, several
     * changes happen quickly, and it is not worth writing configurations
     * each time. Configurations are written to the file when the timer
     * expires without more restarts, which means all edits are done.
     */
    QTimer *mSaveTimer;
};

#endif // MAINWINDOW_H
