// Copyright (C) 2024 Ossi Saukko <osaukko@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file  preferencesdialog.cpp
 * @brief PreferencesDialog class implementation
 */

#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

#include "data/settings.h"
#include "utils/utilities.h"

#include <QFileDialog>

/**
 * @brief Constructs a preferences dialog
 * 
 * This dialog needs a pointer to the settings object to restore settings
 * from it and save new settings if the user accepts the dialog.
 * The PreferencesDialog object only borrows the given settings object
 * and does not take ownership. Ensure that the settings object remains
 * in memory as long as the dialog exists.
 * 
 * @param[in] settings   Pointer to the settings object.
 * @param[in] parent     Pointer to parent widget.
 */
PreferencesDialog::PreferencesDialog(Settings *settings, QWidget *parent)
    : QDialog(parent)
    , mUi(new Ui::PreferencesDialog)
    , mSettings(settings)
{
    mUi->setupUi(this);

    utilities::setDialogBoxIcons(mUi->buttonBox);

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

/**
 * @brief The user has accepted the dialog with new settings
 *
 * Settings from the dialog are saved into the settings object, and the
 * dialog is closed with the Accepted result code.
 */
void PreferencesDialog::onAccepted()
{
    mSettings->setEmulatorBinary(QDir::fromNativeSeparators(mUi->emulatorLineEdit->text()));
    mSettings->setStartCommand(mUi->startCommandLineEdit->text());
    mSettings->setSettingsCommand(mUi->settingsCommandLineEdit->text());
    accept();
}

/**
 * @brief The user pressed a button in the QDialogButtonBox
 *
 * We use this generic handler to detect if the restore defaults button
 * is clicked. If it is, then known good default commands are restored.
 * 
 * @param[in] button   Pointer to the button that the user clicked
 */
void PreferencesDialog::onButtonClicked(QAbstractButton *button)
{
    if (mUi->buttonBox->buttonRole(button) == QDialogButtonBox::ResetRole) {
        mSettings->resetDefaults();
        mUi->startCommandLineEdit->setText(mSettings->startCommand());
        mUi->settingsCommandLineEdit->setText(mSettings->settingsCommand());
    }
}

/**
 * @brief The user pressed the tool button to browse the emulator binary
 * 
 * We show the user a file dialog to find and select the 86Box emulator
 * executable binary. If the user accepts the dialog, the new path is
 * set to the related line edit widget.
 */
void PreferencesDialog::onEmulatorBrowseButtonClicked()
{
#ifdef Q_OS_WINDOWS
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

/**
 * @brief Restoring settings to the widgets
 */
void PreferencesDialog::loadSettings()
{
    mUi->emulatorLineEdit->setText(QDir::toNativeSeparators(mSettings->emulatorBinary()));
    mUi->startCommandLineEdit->setText(mSettings->startCommand());
    mUi->settingsCommandLineEdit->setText(mSettings->settingsCommand());
}
