#include "machine.h"

#include <QIcon>
#include <QVariantMap>

// MachineData
//--------------------------------------------------------------------------------------------------

class MachineData : public QSharedData
{
public:
    void loadIcon();

    //NOLINTBEGIN(misc-non-private-member-variables-in-classes)
    Machine::IconType iconType{Machine::NoIcon};
    QIcon icon;
    QString iconName;
    QString name;
    QString summary;
    QString configFile;
    QString startCommand;
    QString settingsCommand;

    // We keep any extra variables here so that we save them back.
    // Additional variables can be user-defined or from the new version of the launcher.
    QVariantMap extraVariables;
    //NOLINTEND(misc-non-private-member-variables-in-classes)
};

void MachineData::loadIcon()
{
    switch (iconType) {
    case Machine::NoIcon:
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

Machine::Machine()
    : data(new MachineData)
{}

Machine::Machine(const Machine &other) = default;

Machine::Machine(Machine &&other) noexcept
    : data(std::move(other.data))
{}

Machine::Machine(const QVariantMap &machine)
    : data(new MachineData)
{
    restore(machine);
}

Machine::~Machine() = default;

Machine::IconType Machine::iconType() const
{
    return data->iconType;
}

QString Machine::iconName() const
{
    return data->iconName;
}

QIcon Machine::icon() const
{
    return data->icon;
}

void Machine::setIcon(IconType type, const QString &name)
{
    data->iconType = type;
    data->iconName = name;
    data->loadIcon();
}

QString Machine::name() const
{
    return data->name;
}

void Machine::setName(const QString &name)
{
    data->name = name;
}

QString Machine::summary() const
{
    return data->summary;
}

void Machine::setSummary(const QString &summary)
{
    data->summary = summary;
}

QString Machine::configFile() const
{
    return data->configFile;
}

void Machine::setConfigFile(const QString &configFile)
{
    data->configFile = configFile;
}

QString Machine::startCommand() const
{
    return data->startCommand;
}

void Machine::setStartCommand(const QString &startCommand)
{
    data->startCommand = startCommand;
}

QString Machine::settingsCommand() const
{
    return data->settingsCommand;
}

void Machine::setSettingsCommand(const QString &settingsCommand)
{
    data->settingsCommand = settingsCommand;
}

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

Machine &Machine::operator=(const Machine &other) = default;

Machine &Machine::operator=(Machine &&other) noexcept
{
    data = std::move(other.data);
    return *this;
}
