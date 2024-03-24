#include "machinedialog.h"
#include "ui_machinedialog.h"

#include <QDir>

MachineDialog::MachineDialog(QWidget *parent)
    : QDialog(parent)
    , mUi(new Ui::MachineDialog)
{
    mUi->setupUi(this);
    mUi->commandsGroupBox->hide();
    setMachineVariables();
    onAdvancedButtonToggled();
    connect(mUi->advancedPushButton,
            &QPushButton::toggled,
            this,
            &MachineDialog::onAdvancedButtonToggled);
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
    setMachineVariables();
}

void MachineDialog::onAdvancedButtonToggled()
{
    if (mUi->advancedPushButton->isChecked()) {
        mUi->advancedPushButton->setIcon(QIcon::fromTheme("arrow-up"));
    } else {
        mUi->advancedPushButton->setIcon(QIcon::fromTheme("arrow-down"));
    }
}

void MachineDialog::setMachineVariables()
{
    mUi->nameLineEdit->setText(mMachine.name());
    mUi->summaryLineEdit->setText(mMachine.summary());
    mUi->configLineEdit->setText(QDir::toNativeSeparators(mMachine.configFile()));
    mUi->startCommandLineEdit->setText(mMachine.startCommand());
    mUi->settingsCommandLineEdit->setText(mMachine.settingsCommand());

    // Set the advanced button checked if we have any custom command
    if (!mMachine.startCommand().isEmpty() || !mMachine.settingsCommand().isEmpty()) {
        mUi->advancedPushButton->setChecked(true);
    }
}
