// Copyright (C) 2024 Ossi Saukko <osaukko@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file  settings.cpp
 * @brief Settings class implementation
 */

#include "settings.h"

#include <QDir>
#include <QSettings>

/**
 * @brief Default start command known to work
 */
const auto DEFAULT_START_COMMAND = "{86box} --config {config}";

/**
 * @brief Default settings command known to work
 */
const auto DEFAULT_SETTINGS_COMMAND = "{86box} --config {config} --settings";

/**
 * @brief Construct a Settings object
 * 
 * Initializes the mSettings object to use the `settings.ini` file in the program's configuration folder
 * 
 * @param[in] parent   Pointer to parent object
 */
Settings::Settings(QObject *parent)
    : QObject{parent}
{
    mSettings = new QSettings(configHome() + "/settings.ini", QSettings::IniFormat, this);
}

Settings::~Settings() = default;

/**
 * @brief Restore the geometry of the main window
 * @return Byte array that can be used with QWidget::restoreGeometry()
 * @bug Restoring geometry does not work with Wayland
 */
QByteArray Settings::mainWindowGeometry() const
{
    return mSettings->value("MainWindow/geometry").toByteArray();
}

/**
 * @brief Restores 86Box emulator location
 * @return Path to 86Box emulator
 * 
 * @todo Set this automatically if it is empty, and the 86Box program is
 *       found in the operating system paths (checking path environment variable)
 */
QString Settings::emulatorBinary() const
{
    return mSettings->value("86box/emulatorBinary").toString();
}

/**
 * @brief Restores default start command
 * @return Default command for starting the emulator
 */
QString Settings::startCommand() const
{
    return mSettings->value("86box/startCommand", DEFAULT_START_COMMAND).toString();
}

/**
 * @brief Restores default settings command
 * @return Default command for opening emulator settings
 */
QString Settings::settingsCommand() const
{
    return mSettings->value("86box/settingsCommand", DEFAULT_SETTINGS_COMMAND).toString();
}

/**
 * @brief Configuration files directory
 * @return Returns path based on the operating system where the program's
 *         configuration files can be placed
 */
QString Settings::configHome()
{
#ifdef Q_OS_WINDOWS
    return QDir::cleanPath(QDir::homePath() + "/AppData/Local/86BoxLauncher");
#else
    return QDir::cleanPath(QDir::homePath() + "/.config/86BoxLauncher");
#endif
}

/**
 * @brief Restore settings back to default
 * 
 * Restores the start and setting commands back to known working ones.
 */
void Settings::resetDefaults()
{
    setStartCommand(DEFAULT_START_COMMAND);
    setSettingsCommand(DEFAULT_SETTINGS_COMMAND);
}

/**
 * @brief Write main window geometry to the settings
 * @param[in] value   Geometry data from the QWidget::saveGeometry() 
 */
void Settings::setMainWindowGeometry(const QByteArray &value)
{
    mSettings->setValue("MainWindow/geometry", value);
    mSettings->sync();
}

/**
 * @brief Write 86Box emulator location to the settings
 * @param[in] value  Path to the 86Box emulator binary
 */
void Settings::setEmulatorBinary(const QString &value)
{
    if (emulatorBinary() != value) {
        mSettings->setValue("86box/emulatorBinary", value);
        mSettings->sync();
    }
}

/**
 * @brief Write the default start command to the settings
 * @param[in] value   New default starting command
 */
void Settings::setStartCommand(const QString &value)
{
    if (startCommand() != value) {
        mSettings->setValue("86box/startCommand", value);
        mSettings->sync();
    }
}

/**
 * @brief Write the default emulator settings command to the settings
 * @param[in] value   New default emulator settings command
 */
void Settings::setSettingsCommand(const QString &value)
{
    if (settingsCommand() != value) {
        mSettings->setValue("86box/settingsCommand", value);
        mSettings->sync();
    }
}
