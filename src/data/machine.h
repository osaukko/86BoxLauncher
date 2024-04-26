// Copyright (C) 2024 Ossi Saukko <osaukko@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file  machine.h
 * @brief Machine class definition
 */

#ifndef MACHINE_H
#define MACHINE_H

#include <QSharedDataPointer>
#include <QVariantMap>

class MachineData;
class QIcon;
class QString;

/**
 * @brief Information and settings for a single emulated machine
 * 
 * The Machine object collects information about how the emulated machine
 * is presented in the program and what is run when the user wants to start
 * the emulation or edit machine settings.
 */
class Machine
{
public:
    /**
     * @brief Possible icon types for the machine
     * 
     * Icon type is used to decide how the icon name is interpreted.
     */
    enum IconType {
        NoIcon,        /*!< The machine does not have an icon */
        IconFromTheme, /*!< The machine uses the built-in icons from the theme */
        IconFromFile   /*!< The machine uses a custom icon from the user-defined file location */
    };

    Machine();
    Machine(Machine &&other) noexcept;
    Machine(const Machine &other);
    explicit Machine(const QVariantMap &machine);
    ~Machine();

    [[nodiscard]] IconType iconType() const;
    [[nodiscard]] QString iconName() const;
    [[nodiscard]] QIcon icon() const;
    void setIcon(IconType type, const QString &name = {});

    [[nodiscard]] QString name() const;
    void setName(const QString &name);

    [[nodiscard]] QString summary() const;
    void setSummary(const QString &summary);

    [[nodiscard]] QString configFile() const;
    void setConfigFile(const QString &configFile);

    [[nodiscard]] QString startCommand() const;
    void setStartCommand(const QString &startCommand);

    [[nodiscard]] QString settingsCommand() const;
    void setSettingsCommand(const QString &settingsCommand);

    [[nodiscard]] QVariantMap save() const;
    void restore(const QVariantMap &machine);

    Machine &operator=(const Machine &other);
    Machine &operator=(Machine &&other) noexcept;

private:
    QSharedDataPointer<MachineData> data; /*!< @brief Pointer to the implicitly shared data */
};

// This allows us to convert IconType as QVariant and vice versa.
Q_DECLARE_METATYPE(Machine::IconType)

#endif // MACHINE_H
