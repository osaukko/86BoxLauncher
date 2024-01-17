#include "mainwindow.h"

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

void MainWindow::setupUi()
{
    constexpr auto initialWidth = 640;
    constexpr auto initialHeight = 480;
    constexpr auto toolbarIconSize = 48;
    resize({initialWidth, initialHeight});

    // Commands toolbar
    mNewAction = new QAction(QIcon::fromTheme("86box-new"), tr("New"), this);
    mSettingsAction = new QAction(QIcon::fromTheme("86box-settings"), tr("Settings"), this);
    mCommandSettingsAction = new QAction(tr("Command settings"));
    mStartAction = new QAction(QIcon::fromTheme("86box-start"), tr("Start"), this);
    mRemoveAction = new QAction(QIcon::fromTheme("86box-remove"), tr("Remove"), this);
    mCommandsToolbar = new QToolBar(tr("Commands"), this);
    mCommandsToolbar->setIconSize({toolbarIconSize, toolbarIconSize});
    mCommandsToolbar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    mCommandsToolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    mCommandsToolbar->layout()->setContentsMargins({});
    mCommandsToolbar->addAction(mNewAction);
    mCommandsToolbar->addAction(mStartAction);
    mSettingsButton = new QToolButton(mCommandsToolbar);
    mSettingsButton->setDefaultAction(mSettingsAction);
    mSettingsButton->setToolButtonStyle(mCommandsToolbar->toolButtonStyle());
    mSettingsButton->setPopupMode(QToolButton::MenuButtonPopup);
    mSettingsMenu = new QMenu(mSettingsButton);
    mSettingsMenu->addAction(mCommandSettingsAction);
    mSettingsButton->setMenu(mSettingsMenu);
    mCommandsToolbar->addWidget(mSettingsButton);
    mCommandsToolbar->addSeparator();
    mCommandsToolbar->addAction(mRemoveAction);
    mCommandsToolbar->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    // Configure toolbar
    mConfigureAction = new QAction(QIcon::fromTheme("configure"), tr("Configure"), this);
    mConfigureToolbar = new QToolBar(tr("Configure"), this);
    mConfigureToolbar->layout()->setContentsMargins({});
    mConfigureToolbar->setIconSize({toolbarIconSize, toolbarIconSize});
    mConfigureToolbar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    mConfigureToolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    mConfigureToolbar->addAction(mConfigureAction);

    // Layout for toolbars
    mToolbarLayout = new QHBoxLayout;
    mToolbarLayout->addWidget(mCommandsToolbar);
    mToolbarLayout->addStretch();
    mToolbarLayout->addWidget(mConfigureToolbar);

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
}
