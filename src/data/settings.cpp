#include "settings.h"

#include <QDir>
#include <QSettings>

const auto DEFAULT_START_COMMAND = "{86box} --config {config}";
const auto DEFAULT_SETTINGS_COMMAND = "{86box} --config {config} --settings";

Settings::Settings(QObject *parent)
    : QObject{parent}
{
    mSettings = new QSettings(configHome() + "/settings.ini", QSettings::IniFormat, this);
}

Settings::~Settings() = default;

QString Settings::emulatorBinary() const
{
    return mSettings->value("emulatorBinary").toString();
}

QString Settings::startCommand() const
{
    return mSettings->value("startCommand", DEFAULT_START_COMMAND).toString();
}

QString Settings::settingsCommand() const
{
    return mSettings->value("settingsCommand", DEFAULT_SETTINGS_COMMAND).toString();
}

QString Settings::configHome()
{
#ifdef Q_OS_WIN
    return QDir::cleanPath(QDir::homePath() + "/AppData/Local/86BoxLauncher");
#else
    return QDir::cleanPath(QDir::homePath() + "/.config/86BoxLauncher");
#endif
}

void Settings::resetDefaults()
{
    setStartCommand(DEFAULT_START_COMMAND);
    setSettingsCommand(DEFAULT_SETTINGS_COMMAND);
}

void Settings::setEmulatorBinary(const QString &value)
{
    if (emulatorBinary() != value) {
        mSettings->setValue("emulatorBinary", value);
        mSettings->sync();
        emit emulatorBinaryChanged(value);
    }
}

void Settings::setStartCommand(const QString &value)
{
    if (startCommand() != value) {
        mSettings->setValue("startCommand", value);
        mSettings->sync();
        emit startCommandChanged(value);
    }
}

void Settings::setSettingsCommand(const QString &value)
{
    if (settingsCommand() != value) {
        mSettings->setValue("settingsCommand", value);
        mSettings->sync();
        emit settingsCommandChanged(value);
    }
}
