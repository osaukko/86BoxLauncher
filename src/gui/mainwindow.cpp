#include "mainwindow.h"

#include <QHBoxLayout>
#include <QListView>
#include <QToolBar>
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
    newAction = new QAction(QIcon::fromTheme("document-new"), tr("New"), this);
    settingsAction = new QAction(QIcon::fromTheme("document-edit"), tr("Settings"), this);
    removeAction = new QAction(QIcon::fromTheme("document-close"), tr("Remove"), this);
    startAction = new QAction(QIcon::fromTheme("media-playback-start"), tr("Start"), this);
    commandsToolbar = new QToolBar(tr("Commands"), this);
    commandsToolbar->setIconSize({toolbarIconSize, toolbarIconSize});
    commandsToolbar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    commandsToolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    commandsToolbar->layout()->setContentsMargins({});
    commandsToolbar->addAction(newAction);
    commandsToolbar->addAction(settingsAction);
    commandsToolbar->addAction(removeAction);
    commandsToolbar->addAction(startAction);

    // Configure toolbar
    configureAction = new QAction(QIcon::fromTheme("configure"), tr("Configure"), this);
    configureToolbar = new QToolBar(tr("Configure"), this);
    configureToolbar->layout()->setContentsMargins({});
    configureToolbar->setIconSize({toolbarIconSize, toolbarIconSize});
    configureToolbar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    configureToolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    configureToolbar->addSeparator();
    configureToolbar->addAction(configureAction);

    // Layout for toolbars
    toolbarLayout = new QHBoxLayout;
    toolbarLayout->addWidget(commandsToolbar);
    toolbarLayout->addStretch();
    toolbarLayout->addWidget(configureToolbar);

    // List view for virtual machines
    vmView = new QListView;

    // Layout for virtual machines
    vmLayout = new QHBoxLayout;
    vmLayout->addWidget(vmView);

    // Main layout
    mainLayout = new QVBoxLayout;
    mainLayout->addLayout(toolbarLayout);
    mainLayout->addLayout(vmLayout);
    setLayout(mainLayout);
}
