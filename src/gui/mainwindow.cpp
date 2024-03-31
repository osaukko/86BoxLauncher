#include "mainwindow.h"
#include "machinedialog.h"
#include "mvc/machinelistmodel.h"
#include "preferencesdialog.h"

#include "data/settings.h"

#include <QHBoxLayout>
#include <QListView>
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
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    mSettings->setMainWindowGeometry(saveGeometry());
    QWidget::closeEvent(event);
}

void MainWindow::addMachine()
{
    MachineDialog dialog(this);
    dialog.setWindowTitle(tr("Add Machine"));

    Machine newMachine;
    newMachine.setIcon(Machine::IconFromTheme, "pc");
    dialog.setMachine(newMachine);

    if (dialog.exec() == MachineDialog::Accepted) {
        mVmModel->addMachine(dialog.machine());
    }
}

void MainWindow::showPreferences()
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
    mEditAction = new QAction(tr("Edit Machine"));
    mStartAction = new QAction(QIcon::fromTheme("86box-start"), tr("Start"), this);
    mRemoveAction = new QAction(QIcon::fromTheme("86box-remove"), tr("Remove"), this);
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
    connect(mAddAction, &QAction::triggered, this, &MainWindow::addMachine);
    connect(mPreferencesAction, &QAction::triggered, this, &MainWindow::showPreferences);
}
