// Copyright (C) 2024 Ossi Saukko <osaukko@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file  machine.cpp
 * @brief Machine class implementation
 */

#include "machine.h"

#include <QIcon>
#include <QVariantMap>

// MachineData
//--------------------------------------------------------------------------------------------------

/**
 * @brief Implicitly shared data for Machine objects
 */
class MachineData : public QSharedData
{
public:
    void loadIcon();

    //NOLINTBEGIN(misc-non-private-member-variables-in-classes)
    /// @brief Icon type tells us how to interpret *iconName*
    Machine::IconType iconType{Machine::NoIcon};
    QIcon icon;              /*!< @brief The icon will be loaded into this object */
    QString iconName;        /*!< @brief Either the icon theme name or path to the icon file */
    QString name;            /*!< @brief Name of the emulated machine */
    QString summary;         /*!< @brief A summary that appears below the name */
    QString configFile;      /*!< @brief Path to the machine configuration file*/
    QString startCommand;    /*!< @brief Custom start command or empty for the default */
    QString settingsCommand; /*!< @brief Custom settings command or empty for the default */

    /**
     * @brief Extra variables from the restore content
     * 
     * We keep any extra variables here so that we save them back. Additional variables can
     * be user-defined or from the newer version of the launcher.
     */
    QVariantMap extraVariables;
    //NOLINTEND(misc-non-private-member-variables-in-classes)
};

/**
 * @brief Set the *icon* by *iconType* and *iconName*
 * @note If the icon loading fails, the *iconType* is changed to Machine::NoIcon,
 *       and *iconName* is cleared.
 */
void MachineData::loadIcon()
{
    switch (iconType) {
    case Machine::NoIcon:
        icon = {};
        return;

    case Machine::IconFromTheme:
        icon = QIcon::fromTheme(iconName);
        break;

    case Machine::IconFromFile:
        icon = QIcon(iconName);
        break;
    }

    if (icon.isNull()) {
        iconType = Machine::NoIcon;
        iconName.clear();
    }
}

// Machine
//--------------------------------------------------------------------------------------------------

/**
 * @brief Construct empty Machine object
 *
 * All strings are empty and no icon is set on the machine.
 */
Machine::Machine()
    : data(new MachineData)
{}

/**
 * @brief Construct Machine object with reference to *other*
 * @param[in] other   Use the same values as this object
 */
Machine::Machine(const Machine &other) = default;

/**
 * @brief Construct Machine object and move the reference from *other*. 
 * @param[in] other   Move reference from this object
 */
Machine::Machine(Machine &&other) noexcept
    : data(std::move(other.data))
{}

/**
 * @brief Construct Machine object and restore content
 * @param[in] machine   Restore content from this map
 */
Machine::Machine(const QVariantMap &machine)
    : data(new MachineData)
{
    restore(machine);
}

Machine::~Machine() = default;

/**
 * @brief Icon type getter
 * @return Icon type for the machine
 */
Machine::IconType Machine::iconType() const
{
    return data->iconType;
}

/**
 * @brief Icon name getter
 * @return Icon name for the machine
 */
QString Machine::iconName() const
{
    return data->iconName;
}

/**
 * @brief Icon getter
 * @return Icon for the machine
 */
QIcon Machine::icon() const
{
    return data->icon;
}

/**
 * @brief Set icon for the machine
 * 
 * The *type* decides how the *name* is used. 
 * 
 * - Machine::IconFromTheme -> *name* is the icon name from the built-in theme
 * - Machine::IconFromFile -> *name* is the filename for the user-defined icon
 * 
 * @param[in] type   Icon type
 * @param[in] name   Icon name
 * 
 * @note If the icon loading fails, the *iconType* is changed to Machine::NoIcon,
 *       and *iconName* is cleared.
 * @todo Resetting the icon type and name on failure may not be right for the user.
 *       We should consider whether the type and name could be kept in the settings
 *       even if the loading fails.
 */
void Machine::setIcon(IconType type, const QString &name)
{
    data->iconType = type;
    data->iconName = name;
    data->loadIcon();
}

/**
 * @brief Machine name getter
 * @return Name for the machine
 */
QString Machine::name() const
{
    return data->name;
}

/**
 * @brief Machinee name setter
 * @param[in] name   New name for the machine
 */
void Machine::setName(const QString &name)
{
    data->name = name;
}

/**
 * @brief Machine summary getter
 * @return Summary for the machine
 */
QString Machine::summary() const
{
    return data->summary;
}

/**
 * @brief Machine summary setter
 * @param[in] summary   New summary for the machine
 */
void Machine::setSummary(const QString &summary)
{
    data->summary = summary;
}

/**
 * @brief Configuration filename getter
 * @return Path to the configuration file
 */
QString Machine::configFile() const
{
    return data->configFile;
}

/**
 * @brief Configuration filename setter
 * @param[in] configFile   Path to the configuration file
 */
void Machine::setConfigFile(const QString &configFile)
{
    data->configFile = configFile;
}

/**
 * @brief Custom start command getter
 * @return Custom start command or empty for the default
 */
QString Machine::startCommand() const
{
    return data->startCommand;
}

/**
 * @brief Custom start command setter
 * @param[in] startCommand   New custom start command or empty to use the default
 */
void Machine::setStartCommand(const QString &startCommand)
{
    data->startCommand = startCommand;
}

/**
 * @brief Custom settings command getter
 * @return Custom settings command or empty for the default
 */
QString Machine::settingsCommand() const
{
    return data->settingsCommand;
}

/**
 * @brief Custom settings command setter
 * @param[in] settingsCommand   New custom settings command or empty to use the default
 */
void Machine::setSettingsCommand(const QString &settingsCommand)
{
    data->settingsCommand = settingsCommand;
}

/**
 * @brief Save machine data to the QVariantMap
 * @return QVariantMap with all machine properties, including extra properties found when the restore was called.
 */
QVariantMap Machine::save() const
{
    auto map = data->extraVariables;
    map["iconType"] = data->iconType;
    map["iconName"] = data->iconName;
    map["name"] = data->name;
    map["summary"] = data->summary;
    map["configFile"] = data->configFile;
    map["startCommand"] = data->startCommand;
    map["settingsCommand"] = data->settingsCommand;
    return map;
}

/**
 * @brief Restore machine properties from the *machine* map
 * 
 * Sets the machine's properties from the given *machine* map. Any extra properties are retained because they
 * may be from a newer version of the program or added by the user.
 * 
 * @param[in] machine   Set properties from this map
 */
void Machine::restore(const QVariantMap &machine)
{
    data->extraVariables = machine;
    setIcon(data->extraVariables.take("iconType").value<IconType>(),
            data->extraVariables.take("iconName").toString());
    data->name = data->extraVariables.take("name").toString();
    data->summary = data->extraVariables.take("summary").toString();
    data->configFile = data->extraVariables.take("configFile").toString();
    data->startCommand = data->extraVariables.take("startCommand").toString();
    data->settingsCommand = data->extraVariables.take("settingsCommand").toString();
}

/**
 * @brief Assignment operator
 * @param[in] other   Use the same values as this object 
 * @return Reference to this object
 */
Machine &Machine::operator=(const Machine &other) = default;

/**
 * @brief Move assignment operator
 * @param[in] other   Move reference from this object
 * @return Reference to this object
 */
Machine &Machine::operator=(Machine &&other) noexcept
{
    data = std::move(other.data);
    return *this;
}
