#include "machinedialog.h"
#include "ui_machinedialog.h"

#include <QDir>
#include <QFileDialog>

#ifdef Q_OS_WINDOWS
#include "utilities.h"
#endif

MachineDialog::MachineDialog(QWidget *parent)
    : QDialog(parent)
    , mUi(new Ui::MachineDialog)
{
    mUi->setupUi(this);
    mUi->commandsGroupBox->hide();

#ifdef Q_OS_WINDOWS
    utilities::setDialogBoxIcons(mUi->buttonBox);
#endif

    setupIconsCompoBox();
    onAdvancedButtonToggled();

    connect(mUi->advancedPushButton,
            &QPushButton::toggled,
            this,
            &MachineDialog::onAdvancedButtonToggled);
    connect(mUi->configToolButton,
            &QToolButton::clicked,
            this,
            &MachineDialog::onConfigToolButtonClicked);
    connect(mUi->iconToolButton,
            &QToolButton::clicked,
            this,
            &MachineDialog::onIconToolButtonClicked);
    connect(mUi->buttonBox, &QDialogButtonBox::accepted, this, &MachineDialog::onButtonBoxAccepted);
    connect(mUi->buttonBox, &QDialogButtonBox::rejected, this, &MachineDialog::reject);
}

MachineDialog::~MachineDialog()
{
    delete mUi;
}

Machine MachineDialog::machine() const
{
    return mMachine;
}

void MachineDialog::setMachine(const Machine &machine)
{
    mMachine = machine;
    mUi->nameLineEdit->setText(mMachine.name());
    mUi->summaryLineEdit->setText(mMachine.summary());
    mUi->configLineEdit->setText(QDir::toNativeSeparators(mMachine.configFile()));
    mUi->startCommandLineEdit->setText(mMachine.startCommand());
    mUi->settingsCommandLineEdit->setText(mMachine.settingsCommand());
    setIcon();

    // Set the advanced button checked if we have any custom command
    if (!mMachine.startCommand().isEmpty() || !mMachine.settingsCommand().isEmpty()) {
        mUi->advancedPushButton->setChecked(true);
    }
}

void MachineDialog::onAdvancedButtonToggled()
{
    if (mUi->advancedPushButton->isChecked()) {
        mUi->advancedPushButton->setIcon(QIcon::fromTheme("go-up"));
    } else {
        mUi->advancedPushButton->setIcon(QIcon::fromTheme("go-down"));
    }
    mUi->commandsGroupBox->setVisible(mUi->advancedPushButton->isChecked());
}

void MachineDialog::onButtonBoxAccepted()
{
    mMachine.setIcon(mUi->iconComboBox->currentData().value<Machine::IconType>(),
                     mUi->iconComboBox->currentText());
    mMachine.setName(mUi->nameLineEdit->text());
    mMachine.setSummary(mUi->summaryLineEdit->text());
    mMachine.setConfigFile(QDir::fromNativeSeparators(mUi->configLineEdit->text()));
    mMachine.setStartCommand(mUi->startCommandLineEdit->text());
    mMachine.setSettingsCommand(mUi->settingsCommandLineEdit->text());
    accept();
}

void MachineDialog::onConfigToolButtonClicked()
{
    auto directory = QFileInfo(QDir::fromNativeSeparators(mUi->configLineEdit->text()))
                         .absoluteFilePath();
    if (directory.isEmpty()) {
        // Sets the default file name
        directory = "86box.cfg";
    }
    QFileDialog fileDialog(this,
                           tr("Select config file"),
                           directory,
                           tr("Config file (*.cfg);;All files(*)"));
    fileDialog.setFileMode(QFileDialog::AnyFile);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setDefaultSuffix("cfg");
    if (fileDialog.exec() == QFileDialog::Accepted) {
        mUi->configLineEdit->setText(QDir::toNativeSeparators(fileDialog.selectedFiles().value(0)));
    }
}

void MachineDialog::onIconToolButtonClicked()
{
    const auto iconFile = QFileDialog::getOpenFileName(this,
                                                       tr("Select icon"),
                                                       {},
                                                       tr("Icon file (*.ico *.png *.svg *.svgz)"));
    if (!iconFile.isNull()) {
        mMachine.setIcon(Machine::IconFromFile, iconFile);
        setIcon();
    }
}

void MachineDialog::setIcon()
{
    auto *icb = mUi->iconComboBox;
    auto lastIndex = icb->count() - 1;

    switch (mMachine.iconType()) {
    case Machine::NoIcon:
        icb->setCurrentIndex(0);
        break;

    case Machine::IconFromTheme:
        icb->setCurrentText(mMachine.iconName());
        break;

    case Machine::IconFromFile:
        if (icb->itemData(lastIndex).value<Machine::IconType>() == Machine::IconFromFile) {
            icb->setItemIcon(lastIndex, mMachine.icon());
            icb->setItemText(lastIndex, mMachine.iconName());
        } else {
            mUi->iconComboBox->addItem(mMachine.icon(), mMachine.iconName(), Machine::IconFromFile);
            ++lastIndex;
        }
        icb->setCurrentIndex(lastIndex);
        break;
    }
}

void MachineDialog::setupIconsCompoBox()
{
    mUi->iconComboBox->addItem(tr("No icon"), Machine::NoIcon);
    const auto machine_icons = {
        "cpu-8086",
        "cpu-8088",
        "cpu-80286",
        "cpu-80386",
        "cpu-80486",
        "cpu-80586",
        "cpu-80686",
        "pc",
    };
    for (const auto &name : machine_icons) {
        auto icon = QIcon::fromTheme(name);
        if (!icon.isNull()) {
            mUi->iconComboBox->addItem(icon, name, Machine::IconFromTheme);
        }
    }
}
