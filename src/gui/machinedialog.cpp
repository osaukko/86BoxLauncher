// Copyright (C) 2024 Ossi Saukko <osaukko@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file  machinedialog.cpp
 * @brief MachineDialog class implementation
 */

#include "machinedialog.h"
#include "ui_machinedialog.h"

#include "utilities.h"

#include <QDir>
#include <QFileDialog>

/**
 * @brief Create MachineDialog object
 * @param[in] parent   Pointer to the parent widget
 */
MachineDialog::MachineDialog(QWidget *parent)
    : QDialog(parent)
    , mUi(new Ui::MachineDialog)
{
    mUi->setupUi(this);
    mUi->commandsGroupBox->hide();

    utilities::setDialogBoxIcons(mUi->buttonBox);

    setupIconsComboBox();
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

/**
 * @brief Machine setup
 *
 * This function returns the @ref mMachine object as is, but please
 * note that settings in the object are updated when the user accepts
 * the dialog.
 * 
 * @return @ref mMachine object
 */
Machine MachineDialog::machine() const
{
    return mMachine;
}

/**
 * @brief Set machine setup
 * 
 * The function saves the given settings to the @ref mMachine object so
 * that any additional properties are kept there. The interface widgets
 * are then set to display the settings for the given machine.
 *
 * If alternative commands are defined, then the advanced settings view
 * is enabled.
 * 
 * @param[in] machine   Use these settings in the dialog
 */
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

/**
 * @brief The user toggled the advanced view button
 */
void MachineDialog::onAdvancedButtonToggled()
{
    if (mUi->advancedPushButton->isChecked()) {
        mUi->advancedPushButton->setIcon(QIcon::fromTheme("go-up"));
    } else {
        mUi->advancedPushButton->setIcon(QIcon::fromTheme("go-down"));
    }
    mUi->commandsGroupBox->setVisible(mUi->advancedPushButton->isChecked());
}

/**
 * @brief The user pressed the accept button on the dialog
 *
 * The @ref mMachine object is updated with the settings from the dialog,
 * and then the dialog is closed, and its result code is set to Accepted.
 */
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

/**
 * @brief The user pressed machine configuration tool button
 * 
 * We show the user a file dialog in which he can specify the location
 * where the emulator configuration for this setup is stored. If the
 * user accepts the file dialog, the line edit for the configuration
 * path is updated.
 */
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

/**
 * @brief The user pressed the icon button next to the icon combo box
 * 
 * We show the user a file dialog to allow the user to select which file
 * is used as the icon for the machine configuration.
 */
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

/**
 * @brief Updates icon combo box to show the correct icon
 */
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

/**
 * @brief Adds built-in options for the icon combo box
 */
void MachineDialog::setupIconsComboBox()
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
