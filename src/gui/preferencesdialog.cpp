#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

#include "data/settings.h"

#include <QFileDialog>

PreferencesDialog::PreferencesDialog(Settings *settings, QWidget *parent)
    : QDialog(parent)
    , mUi(new Ui::PreferencesDialog)
    , mSettings(settings)
{
    mUi->setupUi(this);
    connect(mUi->buttonBox, &QDialogButtonBox::accepted, this, &PreferencesDialog::onAccepted);
    connect(mUi->buttonBox, &QDialogButtonBox::clicked, this, &PreferencesDialog::onButtonClicked);
    connect(mUi->emulatorBrowseToolButton,
            &QToolButton::clicked,
            this,
            &PreferencesDialog::onEmulatorBrowseButtonClicked);
    loadSettings();
}

PreferencesDialog::~PreferencesDialog()
{
    delete mUi;
}

void PreferencesDialog::onAccepted()
{
    mSettings->setEmulatorBinary(QDir::fromNativeSeparators(mUi->emulatorLineEdit->text()));
    mSettings->setStartCommand(mUi->startCommandLineEdit->text());
    mSettings->setSettingsCommand(mUi->settingsCommandLineEdit->text());
    accept();
}

void PreferencesDialog::onButtonClicked(QAbstractButton *button)
{
    if (mUi->buttonBox->buttonRole(button) == QDialogButtonBox::ResetRole) {
        mSettings->resetDefaults();
        mUi->startCommandLineEdit->setText(mSettings->startCommand());
        mUi->settingsCommandLineEdit->setText(mSettings->settingsCommand());
    }
}

void PreferencesDialog::onEmulatorBrowseButtonClicked()
{
#ifdef Q_OS_WIN
    const auto filter = tr("86Box Emulator (86Box.exe);;Executable (*.exe);;All files (*)");
#else
    const auto filter = tr("86Box Emulator (86Box);;All files (*)");
#endif
    const auto path = QFileInfo(mSettings->emulatorBinary()).absolutePath();
    const QString emulatorPath
        = QFileDialog::getOpenFileName(this,
                                       tr("Select the 86Box emulator executable"),
                                       path,
                                       filter);
    if (!emulatorPath.isNull()) {
        mUi->emulatorLineEdit->setText(QDir::toNativeSeparators(emulatorPath));
    }
}

void PreferencesDialog::loadSettings()
{
    mUi->emulatorLineEdit->setText(QDir::toNativeSeparators(mSettings->emulatorBinary()));
    mUi->startCommandLineEdit->setText(mSettings->startCommand());
    mUi->settingsCommandLineEdit->setText(mSettings->settingsCommand());
}
