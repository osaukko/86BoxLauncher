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

QByteArray Settings::mainWindowGeometry() const
{
    return mSettings->value("MainWindow/geometry").toByteArray();
}

QString Settings::emulatorBinary() const
{
    return mSettings->value("86box/emulatorBinary").toString();
}

QString Settings::startCommand() const
{
    return mSettings->value("86box/startCommand", DEFAULT_START_COMMAND).toString();
}

QString Settings::settingsCommand() const
{
    return mSettings->value("86box/settingsCommand", DEFAULT_SETTINGS_COMMAND).toString();
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

void Settings::setMainWindowGeometry(const QByteArray &value)
{
    mSettings->setValue("MainWindow/geometry", value);
    mSettings->sync();
}

void Settings::setEmulatorBinary(const QString &value)
{
    if (emulatorBinary() != value) {
        mSettings->setValue("86box/emulatorBinary", value);
        mSettings->sync();
    }
}

void Settings::setStartCommand(const QString &value)
{
    if (startCommand() != value) {
        mSettings->setValue("86box/startCommand", value);
        mSettings->sync();
    }
}

void Settings::setSettingsCommand(const QString &value)
{
    if (settingsCommand() != value) {
        mSettings->setValue("86box/settingsCommand", value);
        mSettings->sync();
    }
}
