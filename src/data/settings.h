// Copyright (C) 2024 Ossi Saukko <osaukko@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file  settings.h
 * @brief Settings class definition
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>

class QSettings;

/**
 * @brief Program settings
 * 
 * This class contains methods for restoring and saving settings. 
 * The static configHome() function can be used to get a place to
 * store application configuration files.
 */
class Settings final : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(Settings)

public:
    explicit Settings(QObject *parent = nullptr);
    ~Settings() final;

    [[nodiscard]] QByteArray mainWindowGeometry() const;

    [[nodiscard]] QString emulatorBinary() const;
    [[nodiscard]] QString startCommand() const;
    [[nodiscard]] QString settingsCommand() const;

    static QString configHome();

public slots:
    void resetDefaults();

    void setMainWindowGeometry(const QByteArray &);

    void setEmulatorBinary(const QString &);
    void setSettingsCommand(const QString &);
    void setStartCommand(const QString &);

private:
    QSettings *mSettings{}; /*!< @brief Settings are handled by this object */
};

#endif // SETTINGS_H
