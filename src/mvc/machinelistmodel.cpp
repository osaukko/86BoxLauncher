#include "machinelistmodel.h"

#include <QDebug>
#include <QIcon>

MachineListModel::MachineListModel(QObject *parent)
    : QAbstractListModel{parent}
{}

MachineListModel::~MachineListModel() = default;

void MachineListModel::addMachine(const Machine &machine)
{
    auto row = static_cast<int>(mMachines.size());
    beginInsertRows({}, row, row);
    mMachines.append(machine);
    endInsertRows();
}

Machine MachineListModel::machineForIndex(const QModelIndex &index) const
{
    return mMachines.value(index.row());
}

void MachineListModel::setMachineForIndex(const QModelIndex &index, const Machine &machine)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= mMachines.size()) {
        qCritical() << "Invalid index:" << index;
        return;
    }
    mMachines[index.row()] = machine;
    emit dataChanged(index, index, {Qt::DecorationRole, Qt::DisplayRole, SummaryRole});
}

void MachineListModel::remove(const QModelIndex &index)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= mMachines.size()) {
        qCritical() << "Invalid index:" << index;
        return;
    }
    beginRemoveRows({}, index.row(), index.row());
    mMachines.removeAt(index.row());
    endRemoveRows();
}

QVariantList MachineListModel::save() const
{
    QVariantList machineList;
    for (const auto &machine : mMachines) {
        machineList.append(machine.save());
    }
    return machineList;
}

void MachineListModel::restore(const QVariantList &machines)
{
    beginResetModel();
    mMachines.clear();
    for (const auto &machine : machines) {
        if (machine.canConvert<QVariantMap>()) {
            mMachines.append(Machine(machine.toMap()));
        } else {
            qCritical() << "Invalid machine config:" << machine;
        }
    }
    endResetModel();
}

int MachineListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0; // No children items
    }
    return static_cast<int>(mMachines.size());
}

QVariant MachineListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    if (index.row() < 0 || index.row() >= mMachines.size()) {
        return {};
    }

    switch (role) {
    case Qt::DecorationRole:
        return machineForIndex(index).icon();

    case Qt::DisplayRole:
        return machineForIndex(index).name();

    case SummaryRole:
        return machineForIndex(index).summary();

    default:
        return {};
    }
}

QVariant MachineListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return {};
    }

    switch (orientation) {
    case Qt::Horizontal:
        if (section == 0) {
            return tr("Machines");
        }
        break;
    case Qt::Vertical:
        return QString::number(section + 1);
        break;
    }

    return {};
}
