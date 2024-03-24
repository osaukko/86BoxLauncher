#include "machine.h"
#include <QIcon>

// MachineData
//--------------------------------------------------------------------------------------------------

class MachineData : public QSharedData
{
public:
    MachineData();
    void loadIcon();

    //NOLINTBEGIN(misc-non-private-member-variables-in-classes)
    Machine::IconType iconType{Machine::FromTheme};
    QIcon icon;
    QString iconName{"pc"};
    QString name;
    QString summary;
    QString configFile;
    QString startCommand;
    QString settingsCommand;
    //NOLINTEND(misc-non-private-member-variables-in-classes)
};

MachineData::MachineData()
{
    loadIcon();
}

void MachineData::loadIcon()
{
    switch (iconType) {
    case Machine::NoIcon:
        return;

    case Machine::FromTheme:
        icon = QIcon::fromTheme(iconName);
        break;

    case Machine::FromFile:
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

Machine &Machine::operator=(const Machine &other) = default;

Machine &Machine::operator=(Machine &&other) noexcept
{
    data = std::move(other.data);
    return *this;
}
