// Copyright (C) 2023 Ossi Saukko <osaukko@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file  mainwindow.cpp
 * @brief MainWindow class implementation
 */

#include "mainwindow.h"
#include "machinedialog.h"
#include "preferencesdialog.h"

#include "data/settings.h"
#include "mvc/machinedelegate.h"
#include "mvc/machinelistmodel.h"
#include "utils/formatter.h"

#include <QDir>
#include <QFile>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QListView>
#include <QMenu>
#include <QMessageBox>
#include <QProcess>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>

/**
 * @brief Icon size for tool bar buttons
 */
const QSize TOOL_BAR_ICON_SIZE = {48, 48};

/**
 * @brief Construct the main window widget
 * @param[in] parent   Pointer to the parent widget
 * @note In our case there is no parent widget, so main window is
 *       constructed using nullptr for parent.
 */
MainWindow::MainWindow(QWidget *parent)
    : QWidget{parent}
    , mSaveTimer{new QTimer(this)}
{
    setupUi();
    restoreMachines();

    // We use a timer so that we can save model content when all changes to the model have been made
    const auto saveAfterNoChangesForMsec = 200;
    mSaveTimer->setSingleShot(true);
    mSaveTimer->setInterval(saveAfterNoChangesForMsec);
    connect(mVmModel, &MachineListModel::modelChanged, mSaveTimer, qOverload<>(&QTimer::start));
    connect(mSaveTimer, &QTimer::timeout, this, &MainWindow::saveMachines);
}

/**
 * @brief This event is triggered when the user closes the main window
 * 
 * We use this event to save the main window geometry into settings.
 * 
 * @param[in] event   Event information object
 */
void MainWindow::closeEvent(QCloseEvent *event)
{
    mSettings->setMainWindowGeometry(saveGeometry());
    QWidget::closeEvent(event);
}

/**
 * @brief The user pressed the add tool button
 *
 * We open the MachineDialog for the user. If the user accepts the
 * dialog, a new machine from the dialog is added to the model.
 */
void MainWindow::onAddClicked()
{
    MachineDialog dialog(this);
    dialog.setWindowTitle(tr("Add Machine"));

    Machine newMachine;
    newMachine.setIcon(Machine::IconFromTheme, "pc");
    dialog.setMachine(newMachine);

    if (dialog.exec() == MachineDialog::Accepted) {
        newMachine = dialog.machine();
        mVmModel->addMachine(newMachine);

        // Automatically open settings dialog if config file does not exist
        if (!QFile::exists(newMachine.configFile())) {
            mVmView->setCurrentIndex(mVmModel->index(mVmModel->rowCount({}) - 1));
            onSettingsClicked();
        }
    }
}

/**
 * @brief The user triggered the context menu for the list view
 * 
 * The method selects an item under *pos* and opens the context popup
 * menu. If there is no item under *pos*, the method exits without
 * doing anything.
 * 
 * @param[in] pos   Context menu is requested for this position
 */
void MainWindow::onContextMenuRequest(const QPoint &pos)
{
    const auto index = mVmView->indexAt(pos);
    if (!index.isValid()) {
        return;
    }
    mVmView->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
    mContextMenu->popup(mVmView->mapToGlobal(pos));
}

/**
 * @brief The user selected edit machine from the settings button menu
 * 
 * We create a MachineDialog object and set the selected machine item for
 * it. We then show the dialog for the user, and if the dialog is accepted,
 * then new settings are applied for the selected machine item.
 */
void MainWindow::onEditClicked()
{
    MachineDialog dialog(this);
    dialog.setWindowTitle(tr("Edit Machine"));
    dialog.setMachine(mVmModel->machineForIndex(mVmView->currentIndex()));

    if (dialog.exec() == MachineDialog::Accepted) {
        mVmModel->setMachineForIndex(mVmView->currentIndex(), dialog.machine());
    }
}

/**
 * @brief A user double-clicked a machine item with the mouse.
 *
 * The method ensures that the machine item is selected and starts
 * the 86Box emulator with its configuration.
 * 
 * @param[in] index   Index for double-clicked item
 */
void MainWindow::onMachineDoubleClicked(const QModelIndex &index)
{
    mVmView->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
    onStartClicked();
}

/**
 * @brief The selection on the list view changed.
 *
 * The following tool buttons are enabled or disabled based on whether
 * something is selected or not:
 * 
 * - Start button
 * - Settings button
 * - Remove button
 * 
 * Also, the edit machine menu item is enabled or disabled at the same
 * time, although the menu should not be available when the settings
 * button is disabled.
 * 
 * @param[in] selected   List of selected items
 */
void MainWindow::onMachineSelectionChanged(const QItemSelection &selected,
                                           const QItemSelection & /*deselected*/)
{
    const auto gotSelection = !selected.isEmpty();
    mStartAction->setEnabled(gotSelection);
    mEditAction->setEnabled(gotSelection);
    mSettingsAction->setEnabled(gotSelection);
    mRemoveAction->setEnabled(gotSelection);
}

/**
 * @brief The user pressed the preferences button.
 *
 * We create PreferencesDialog and borrow the settings object for it.
 * Dialog is then shown to the user.
 */
void MainWindow::onPreferencesClicked()
{
    PreferencesDialog dialog(mSettings, this);
    dialog.exec();
}

/**
 * @brief The user pressed the remove button.
 *
 * We create a question message box with information about what we are
 * about to remove. If the user selects yes from the question message
 * box, the corresponding machine item from the model is removed.
 */
void MainWindow::onRemoveClicked()
{
    QMessageBox messageBox(
        QMessageBox::Question,
        tr("Removing Virtual Machine"),
        tr("This function removes the selected virtual machine from the list. The virtual machine "
           "files will not be deleted, but you may want to delete them yourself.\n\nContinue?"),
        QMessageBox::Yes | QMessageBox::No,
        this);
    const auto machine = mVmModel->machineForIndex(mVmView->currentIndex());
    messageBox.setDetailedText(
        tr("Virtual machine: %1\nSummary: %2\nConfig file: %3")
            .arg(machine.name(), machine.summary(), QDir::toNativeSeparators(machine.configFile())));
    if (messageBox.exec() == QMessageBox::Yes) {
        mVmModel->remove(mVmView->currentIndex());
    }
}

/**
 * @brief The user pressed the settings button.
 *
 * This function takes an alternative settings command from the selected
 * machine item. If the alternative settings command is empty, the
 * function uses the default settings command from the settings object.
 * 
 * The command is then run using the runCommand().
 */
void MainWindow::onSettingsClicked()
{
    const auto machine = mVmModel->machineForIndex(mVmView->currentIndex());
    auto command = machine.settingsCommand();
    if (command.isEmpty()) {
        command = mSettings->settingsCommand();
    }
    runCommand(command, machine);
}

/** 
 * @brief The user pressed the start button.
 *
 * This function takes an alternative start command from the selected
 * machine item. If the alternative start command is empty, the
 * function uses the default start command from the settings object.
 *
 * The command is then run using the runCommand().
 */
void MainWindow::onStartClicked()
{
    const auto machine = mVmModel->machineForIndex(mVmView->currentIndex());
    auto command = machine.startCommand();
    if (command.isEmpty()) {
        command = mSettings->startCommand();
    }
    runCommand(command, machine);
}

/**
 * @brief Save all machines to the `machines.json` file
 * 
 * @pre This method is run on the mSaveTimer timeout signal.
 * 
 * The method requests a QVariantList of all machine items from the
 * model using its @ref MachineListModel::save() "save()" method.
 * The QVariantList is then turned into an indented JSON and written
 * to the `machines.json` file in the config directory.
 */
void MainWindow::saveMachines()
{
    QFile file(Settings::configHome() + "/machines.json");
    if (!file.open(QFile::WriteOnly)) {
        QMessageBox::critical(this, tr("Could not save machines"), file.errorString());
        return;
    }
    const auto json = QJsonDocument::fromVariant(mVmModel->save()).toJson(QJsonDocument::Indented);
    if (file.write(json) != json.size()) {
        QMessageBox::critical(this, tr("Could not save machines"), file.errorString());
        return;
    }
    qDebug() << "Machines saved";
}

/**
 * @brief A helper function to create tool buttons from actions
 * 
 * Set the following properties from given *action*
 * 
 * - checkable
 * - checked
 * - enabled
 * - font
 * - icon
 * - popupMode (assuming the action has a menu)
 * - statusTip
 * - text
 * - toolTip
 * - whatsThis
 *
 * Then sets:
 * 
 * - Tool button style: Text under icon
 * - Auto raise: true
 * - Icon size: @ref TOOL_BAR_ICON_SIZE
 * 
 * @param[in] action      Setup tool button with this action
 * @param[in] parent      Set this parent widget for the button
 * 
 * @return  Pointer to the new tool button widget
 */
QToolButton *MainWindow::createToolButton(QAction *action, QWidget *parent)
{
    auto *button = new QToolButton(parent);
    button->setDefaultAction(action);
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    button->setAutoRaise(true);
    button->setIconSize(TOOL_BAR_ICON_SIZE);
    return button;
}

/**
 * @brief Restore machines from the `machines.json` file.
 *
 * @pre This method is run when the main window is constructed.
 * 
 * This function tries to read the `machines.json` from the config
 * directory. Content is then parsed as JSON. If there are no errors,
 * the loaded content is passed as QVariantList to the model using its
 * @ref MachineListModel::restore "restore()" method.
 */
void MainWindow::restoreMachines()
{
    QFile file(Settings::configHome() + "/machines.json");
    if (!file.exists()) {
        return;
    }

    if (!file.open(QFile::ReadOnly)) {
        QMessageBox::critical(this, tr("Could not restore machines"), file.errorString());
        return;
    }

    QJsonParseError error{};
    const auto jsonDocument = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        QMessageBox::critical(this,
                              tr("Could not restore machines"),
                              tr("JSON error: %1").arg(error.errorString()));
        return;
    }

    mVmModel->restore(jsonDocument.toVariant().toList());
}

/**
 * @brief Running commands
 *
 * This method takes the *command* and uses the @ref Formatter::format()
 * function to fill its variable fields using information from the
 * *machine* item. The operating system is then requested to run
 * the completed command.
 *
 * If something goes wrong, the user will receive an error message box.
 * 
 * @param[in] command   Command that should be run
 * @param[in] machine   Machine item to fill variable field in the *command*
 * 
 * @todo Show a visual indicator for the user that the command is run
 */
void MainWindow::runCommand(const QString &command, const Machine &machine)
{
    bool ok = false;
    const auto formattedCommand = Formatter::format(command, variablesForMachine(machine), &ok);
    if (!ok || formattedCommand.isEmpty()) {
        QMessageBox::critical(
            this,
            tr("Error with settings command"),
            tr("Could not format the settings command. Please check your settings."));
        return;
    }
    auto arguments = QProcess::splitCommand(formattedCommand);
    auto program = arguments.takeFirst();

    QProcess process(this);
    process.setProgram(program);
    process.setArguments(arguments);
    process.setWorkingDirectory(QFileInfo(program).absolutePath());
    if (!process.startDetached()) {
        QMessageBox::critical(this, tr("Could not start the program"), process.errorString());
    }
}

/**
 * @brief Creates the user interface for the main window
 * @pre This method is run when the main window is constructed
 */
void MainWindow::setupUi()
{
    constexpr auto initialWidth = 640;
    constexpr auto initialHeight = 480;
    constexpr QSize machineIconSize = {32, 32};

    resize({initialWidth, initialHeight});
    setWindowIcon(QIcon::fromTheme("86boxlauncher"));

    // Restore settings
    mSettings = new Settings(this);
    restoreGeometry(mSettings->mainWindowGeometry());

    // Setup actions
    mAddAction = new QAction(QIcon::fromTheme("86box-new"), tr("Add"), this);
    mEditAction = new QAction(QIcon::fromTheme("document-edit"), tr("Edit Machine"), this);
    mRemoveAction = new QAction(QIcon::fromTheme("86box-remove"), tr("Remove"), this);
    mSettingsAction = new QAction(QIcon::fromTheme("86box-settings"), tr("Settings"), this);
    mStartAction = new QAction(QIcon::fromTheme("86box-start"), tr("Start"), this);
    mPreferencesAction = new QAction(QIcon::fromTheme("86box-preferences"), tr("Preferences"), this);
    mEditAction->setEnabled(false);
    mRemoveAction->setEnabled(false);
    mSettingsAction->setEnabled(false);
    mStartAction->setEnabled(false);

    // Toolbar widgets
    mAddButton = createToolButton(mAddAction, this);
    mStartButton = createToolButton(mStartAction, this);
    mSettingsButton = createToolButton(mSettingsAction, this);
    mSeparatorLine = new QFrame(this);
    mRemoveButton = createToolButton(mRemoveAction, this);
    mPreferencesButton = createToolButton(mPreferencesAction, this);
    mSeparatorLine->setFrameStyle(QFrame::VLine | QFrame::Sunken);

    // Add menu for settings button
    mSettingsMenu = new QMenu(mSettingsButton);
    mSettingsMenu->addAction(mEditAction);
    mSettingsButton->setPopupMode(QToolButton::MenuButtonPopup);
    mSettingsButton->setMenu(mSettingsMenu);

    // Layout for tool bar
    mToolBarLayout = new QHBoxLayout;
    mToolBarLayout->addWidget(mAddButton);
    mToolBarLayout->addWidget(mStartButton);
    mToolBarLayout->addWidget(mSettingsButton);
    mToolBarLayout->addWidget(mSeparatorLine);
    mToolBarLayout->addWidget(mRemoveButton);
    mToolBarLayout->addStretch();
    mToolBarLayout->addWidget(mPreferencesButton);

    // List view and model for virtual machines
    mVmModel = new MachineListModel(this);
    mVmView = new QListView;
    mVmView->setIconSize(machineIconSize);
    mVmView->setModel(mVmModel);
    mVmView->setDragDropMode(QListView::InternalMove);
    mVmView->setItemDelegateForColumn(0, new MachineDelegate(mVmView));

    // Add context menu for list view
    mContextMenu = new QMenu(mVmView);
    mContextMenu->addAction(mStartAction);
    mContextMenu->addAction(mSettingsAction);
    mContextMenu->addAction(mEditAction);
    mContextMenu->addSeparator();
    mContextMenu->addAction(mRemoveAction);
    mVmView->setContextMenuPolicy(Qt::CustomContextMenu);

    // Main layout
    mMainLayout = new QVBoxLayout;
    mMainLayout->addLayout(mToolBarLayout);
    mMainLayout->addWidget(mVmView);
    setLayout(mMainLayout);

    // Connecting actions
    connect(mAddAction, &QAction::triggered, this, &MainWindow::onAddClicked);
    connect(mEditAction, &QAction::triggered, this, &MainWindow::onEditClicked);
    connect(mPreferencesAction, &QAction::triggered, this, &MainWindow::onPreferencesClicked);
    connect(mRemoveAction, &QAction::triggered, this, &MainWindow::onRemoveClicked);
    connect(mSettingsAction, &QAction::triggered, this, &MainWindow::onSettingsClicked);
    connect(mStartAction, &QAction::triggered, this, &MainWindow::onStartClicked);
    connect(mVmView, &QListView::doubleClicked, this, &MainWindow::onMachineDoubleClicked);
    connect(mVmView,
            &QListView::customContextMenuRequested,
            this,
            &MainWindow::onContextMenuRequest);
    connect(mVmView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            &MainWindow::onMachineSelectionChanged);
}

/**
 * @brief Create an information map for the given machine
 *
 * This method creates an information map that can be used with
 * the @ref Formatter tool.
 * 
 * Map contains the following items:
 * - `86box`: with path to the 86Box emulator binary from the settings.
 * - `config`: with path to the configuration file of the *machine* item.
 * 
 * @param[in] machine   Make information map for this machine item
 * @return Map with machine information
 */
QHash<QString, QString> MainWindow::variablesForMachine(const Machine &machine) const
{
    // Adding double quotes to ensure paths with spaces work correctly.
    const auto emulator = QLatin1Char('"') + mSettings->emulatorBinary() + QLatin1Char('"');
    const auto config = QLatin1Char('"') + machine.configFile() + QLatin1Char('"');
    return {{"86box", emulator}, {"config", config}};
}
