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
#include <QMessageBox>
#include <QProcess>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>

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

void MainWindow::closeEvent(QCloseEvent *event)
{
    mSettings->setMainWindowGeometry(saveGeometry());
    QWidget::closeEvent(event);
}

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

void MainWindow::onEditClicked()
{
    MachineDialog dialog(this);
    dialog.setWindowTitle(tr("Edit Machine"));
    dialog.setMachine(mVmModel->machineForIndex(mVmView->currentIndex()));

    if (dialog.exec() == MachineDialog::Accepted) {
        mVmModel->setMachineForIndex(mVmView->currentIndex(), dialog.machine());
    }
}

void MainWindow::onMachineSelectionChanged(const QItemSelection &selected,
                                           const QItemSelection & /*deselected*/)
{
    const auto gotSelection = !selected.isEmpty();
    mStartButton->setEnabled(gotSelection);
    mEditAction->setEnabled(gotSelection);
    mSettingsButton->setEnabled(gotSelection);
    mRemoveButton->setEnabled(gotSelection);
}

void MainWindow::onPreferencesClicked()
{
    PreferencesDialog dialog(mSettings, this);
    dialog.exec();
}

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

void MainWindow::onSettingsClicked()
{
    const auto machine = mVmModel->machineForIndex(mVmView->currentIndex());
    auto command = machine.settingsCommand();
    if (command.isEmpty()) {
        command = mSettings->settingsCommand();
    }
    runCommand(command, machine);
}

void MainWindow::onStartClicked()
{
    const auto machine = mVmModel->machineForIndex(mVmView->currentIndex());
    auto command = machine.startCommand();
    if (command.isEmpty()) {
        command = mSettings->startCommand();
    }
    runCommand(command, machine);
}

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

QToolButton *MainWindow::createToolButton(const QIcon &icon, const QString &text, QWidget *parent)
{
    constexpr auto toolbarIconSize = 48;
    auto *button = new QToolButton(parent);
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    button->setAutoRaise(true);
    button->setIconSize({toolbarIconSize, toolbarIconSize});
    button->setIcon(icon);
    button->setText(text);
    return button;
}

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

void MainWindow::setupUi()
{
    constexpr auto initialWidth = 640;
    constexpr auto initialHeight = 480;
    constexpr QSize machine_icon_size = {32, 32};

    resize({initialWidth, initialHeight});
    setWindowIcon(QIcon::fromTheme("86boxlauncher"));

    // Restore settings
    mSettings = new Settings(this);
    restoreGeometry(mSettings->mainWindowGeometry());

    // Toolbar widgets
    mAddButton = createToolButton(QIcon::fromTheme("86box-new"), tr("Add"), this);
    mStartButton = createToolButton(QIcon::fromTheme("86box-start"), tr("Start"), this);
    mSettingsButton = createToolButton(QIcon::fromTheme("86box-settings"), tr("Settings"), this);
    mSeparatorLine = new QFrame(this);
    mRemoveButton = createToolButton(QIcon::fromTheme("86box-remove"), tr("Remove"), this);
    mPreferencesButton = createToolButton(QIcon::fromTheme("86box-preferences"),
                                          tr("Preferences"),
                                          this);
    mStartButton->setEnabled(false);
    mSettingsButton->setEnabled(false);
    mSeparatorLine->setFrameShape(QFrame::VLine);
    mRemoveButton->setEnabled(false);

    // Add menu for settings button
    mEditAction = new QAction(QIcon::fromTheme("document-edit"), tr("Edit Machine"));
    mEditAction->setEnabled(false);
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
    mVmView->setIconSize(machine_icon_size);
    mVmView->setModel(mVmModel);
    mVmView->setDragDropMode(QListView::InternalMove);
    mVmView->setItemDelegateForColumn(0, new MachineDelegate(mVmView));

    // Main layout
    mMainLayout = new QVBoxLayout;
    mMainLayout->addLayout(mToolBarLayout);
    mMainLayout->addWidget(mVmView);
    setLayout(mMainLayout);

    // Connecting actions
    connect(mAddButton, &QToolButton::clicked, this, &MainWindow::onAddClicked);
    connect(mEditAction, &QAction::triggered, this, &MainWindow::onEditClicked);
    connect(mPreferencesButton, &QToolButton::clicked, this, &MainWindow::onPreferencesClicked);
    connect(mRemoveButton, &QToolButton::clicked, this, &MainWindow::onRemoveClicked);
    connect(mSettingsButton, &QToolButton::clicked, this, &MainWindow::onSettingsClicked);
    connect(mStartButton, &QToolButton::clicked, this, &MainWindow::onStartClicked);
    connect(mVmView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            &MainWindow::onMachineSelectionChanged);
}

QHash<QString, QString> MainWindow::variablesForMachine(const Machine &machine) const
{
    // Adding double quotes to ensure paths with spaces work correctly.
    const auto emulator = QLatin1Char('"') + mSettings->emulatorBinary() + QLatin1Char('"');
    const auto config = QLatin1Char('"') + machine.configFile() + QLatin1Char('"');
    return {{"86box", emulator}, {"config", config}};
}
