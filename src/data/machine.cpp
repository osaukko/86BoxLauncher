#include "machine.h"

class MachineData : public QSharedData
{
public:
    QString name;
};

Machine::Machine()
    : data(new MachineData)
{}

Machine::Machine(const Machine &other) = default;

Machine::Machine(Machine &&other) noexcept
    : data(std::move(other.data))
{}

Machine::~Machine() = default;

QString Machine::name() const
{
    return data->name;
}

void Machine::setName(const QString &name)
{
    data->name = name;
}

Machine &Machine::operator=(const Machine &other) = default;

Machine &Machine::operator=(Machine &&other) noexcept
{
    data = std::move(other.data);
    return *this;
}
