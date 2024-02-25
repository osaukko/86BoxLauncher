#include "mainwindow.h"
#include "preferencesdialog.h"

#include <QHBoxLayout>
#include <QListView>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QWidget{parent}
{
    setupUi();
}

void MainWindow::showPreferences()
{
    PreferencesDialog dialog(this);
    dialog.exec();
}

void MainWindow::setupUi()
{
    constexpr auto initialWidth = 640;
    constexpr auto initialHeight = 480;
    constexpr auto toolbarIconSize = 48;
    resize({initialWidth, initialHeight});

    // Virtual machines toolbar
    mAddAction = new QAction(QIcon::fromTheme("86box-new"), tr("Add"), this);
    mSettingsAction = new QAction(QIcon::fromTheme("86box-settings"), tr("Settings"), this);
    mCommandSettingsAction = new QAction(tr("Commands"));
    mStartAction = new QAction(QIcon::fromTheme("86box-start"), tr("Start"), this);
    mRemoveAction = new QAction(QIcon::fromTheme("86box-remove"), tr("Remove"), this);
    mMachinesToolbar = new QToolBar(tr("Commands"), this);
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
    mSettingsMenu->addAction(mCommandSettingsAction);
    mSettingsButton->setMenu(mSettingsMenu);
    mMachinesToolbar->addWidget(mSettingsButton);
    mMachinesToolbar->addSeparator();
    mMachinesToolbar->addAction(mRemoveAction);
    mMachinesToolbar->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    // Preferences toolbar
    mPreferencesAction = new QAction(QIcon::fromTheme("preferences"), tr("Preferences"), this);
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

    // List view for virtual machines
    mVmView = new QListView;

    // Layout for virtual machines
    mVmLayout = new QHBoxLayout;
    mVmLayout->addWidget(mVmView);

    // Main layout
    mMainLayout = new QVBoxLayout;
    mMainLayout->addLayout(mToolbarLayout);
    mMainLayout->addLayout(mVmLayout);
    setLayout(mMainLayout);

    // Connecting actions
    connect(mPreferencesAction, &QAction::triggered, this, &MainWindow::showPreferences);
}
