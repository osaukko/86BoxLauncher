#include "mainwindow.h"
#include "machinedialog.h"
#include "mvc/machinelistmodel.h"
#include "preferencesdialog.h"

#include "data/settings.h"

#include <QDir>
#include <QFile>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QListView>
#include <QMessageBox>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>

namespace {
const QSize machine_icon_size = {32, 32};
} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QWidget{parent}
{
    setupUi();
    restoreMachines();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    mSettings->setMainWindowGeometry(saveGeometry());
    QWidget::closeEvent(event);
}

void MainWindow::onAddMachineTriggered()
{
    MachineDialog dialog(this);
    dialog.setWindowTitle(tr("Add Machine"));

    Machine newMachine;
    newMachine.setIcon(Machine::IconFromTheme, "pc");
    dialog.setMachine(newMachine);

    if (dialog.exec() == MachineDialog::Accepted) {
        mVmModel->addMachine(dialog.machine());
        saveMachines();
    }
}

void MainWindow::onEditMachineTriggered()
{
    MachineDialog dialog(this);
    dialog.setWindowTitle(tr("Edit Machine"));
    dialog.setMachine(mVmModel->machineForIndex(mVmView->currentIndex()));

    if (dialog.exec() == MachineDialog::Accepted) {
        mVmModel->setMachineForIndex(mVmView->currentIndex(), dialog.machine());
        saveMachines();
    }
}

void MainWindow::onMachineSelectionChanged(const QItemSelection &selected,
                                           const QItemSelection & /*deselected*/)
{
    const auto gotSelection = !selected.isEmpty();
    mStartAction->setEnabled(gotSelection);
    mEditAction->setEnabled(gotSelection);
    mSettingsAction->setEnabled(gotSelection);
    mRemoveAction->setEnabled(gotSelection);
}

void MainWindow::onRemoveMachineTriggered()
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
        saveMachines();
    }
}

void MainWindow::onShowPreferencesTriggered()
{
    PreferencesDialog dialog(mSettings, this);
    dialog.exec();
}

void MainWindow::setupUi()
{
    constexpr auto initialWidth = 640;
    constexpr auto initialHeight = 480;
    constexpr auto toolbarIconSize = 48;
    resize({initialWidth, initialHeight});
    setWindowIcon(QIcon::fromTheme("86boxlauncher"));

    // Restore settings
    mSettings = new Settings(this);
    restoreGeometry(mSettings->mainWindowGeometry());

    // Virtual machines toolbar
    mAddAction = new QAction(QIcon::fromTheme("86box-new"), tr("Add"), this);
    mSettingsAction = new QAction(QIcon::fromTheme("86box-settings"), tr("Settings"), this);
    mSettingsAction->setEnabled(false);
    mEditAction = new QAction(QIcon::fromTheme("document-edit"), tr("Edit Machine"));
    mEditAction->setEnabled(false);
    mStartAction = new QAction(QIcon::fromTheme("86box-start"), tr("Start"), this);
    mStartAction->setEnabled(false);
    mRemoveAction = new QAction(QIcon::fromTheme("86box-remove"), tr("Remove"), this);
    mRemoveAction->setEnabled(false);
    mMachinesToolbar = new QToolBar(tr("Actions"), this);
    mMachinesToolbar->setIconSize({toolbarIconSize, toolbarIconSize});
    mMachinesToolbar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    mMachinesToolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    mMachinesToolbar->layout()->setContentsMargins({});
    mMachinesToolbar->addAction(mAddAction);
    mMachinesToolbar->addAction(mStartAction);
    mSettingsButton = new QToolButton(mMachinesToolbar);
    mSettingsButton->setDefaultAction(mSettingsAction);
    mSettingsButton->setToolButtonStyle(mMachinesToolbar->toolButtonStyle());
    mSettingsButton->setPopupMode(QToolButton::MenuButtonPopup);
    mSettingsMenu = new QMenu(mSettingsButton);
    mSettingsMenu->addAction(mEditAction);
    mSettingsButton->setMenu(mSettingsMenu);
    mMachinesToolbar->addWidget(mSettingsButton);
    mMachinesToolbar->addSeparator();
    mMachinesToolbar->addAction(mRemoveAction);
    mMachinesToolbar->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    // Preferences toolbar
    mPreferencesAction = new QAction(QIcon::fromTheme("86box-preferences"), tr("Preferences"), this);
    mPreferencesToolbar = new QToolBar(tr("Preferences"), this);
    mPreferencesToolbar->layout()->setContentsMargins({});
    mPreferencesToolbar->setIconSize({toolbarIconSize, toolbarIconSize});
    mPreferencesToolbar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    mPreferencesToolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    mPreferencesToolbar->addAction(mPreferencesAction);

    // Layout for toolbars
    mToolbarLayout = new QHBoxLayout;
    mToolbarLayout->addWidget(mMachinesToolbar);
    mToolbarLayout->addStretch();
    mToolbarLayout->addWidget(mPreferencesToolbar);

    // List view and model for virtual machines
    mVmModel = new MachineListModel(this);
    mVmView = new QListView;
    mVmView->setIconSize(machine_icon_size);
    mVmView->setModel(mVmModel);

    // Layout for virtual machines
    mVmLayout = new QHBoxLayout;
    mVmLayout->addWidget(mVmView);

    // Main layout
    mMainLayout = new QVBoxLayout;
    mMainLayout->addLayout(mToolbarLayout);
    mMainLayout->addLayout(mVmLayout);
    setLayout(mMainLayout);

    // Connecting actions
    connect(mAddAction, &QAction::triggered, this, &MainWindow::onAddMachineTriggered);
    connect(mEditAction, &QAction::triggered, this, &MainWindow::onEditMachineTriggered);
    connect(mPreferencesAction, &QAction::triggered, this, &MainWindow::onShowPreferencesTriggered);
    connect(mRemoveAction, &QAction::triggered, this, &MainWindow::onRemoveMachineTriggered);
    connect(mVmView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            &MainWindow::onMachineSelectionChanged);
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

    QJsonParseError error;
    const auto jsonDocument = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        QMessageBox::critical(this,
                              tr("Could not restore machines"),
                              tr("JSON error: %1").arg(error.errorString()));
        return;
    }

    mVmModel->restore(jsonDocument.toVariant().toList());
}
