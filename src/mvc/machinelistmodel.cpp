#include "machinelistmodel.h"

#include <QDebug>
#include <QIcon>
#include <QJsonDocument>
#include <QMimeData>

namespace {
const auto jsonMimeType = "application/json";
} // namespace

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
    emit modelChanged();
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
    emit modelChanged();
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
    emit modelChanged();
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

QStringList MachineListModel::mimeTypes() const
{
    return {jsonMimeType};
}

QMimeData *MachineListModel::mimeData(const QModelIndexList &indexes) const
{
    if (indexes.isEmpty()) {
        return {};
    }

    QVariantList list;
    for (const auto &index : indexes) {
        if (index.isValid()) {
            list.append(machineForIndex(index).save());
        }
    }

    const auto json = QJsonDocument::fromVariant(list).toJson(QJsonDocument::Compact);
    auto *mimeData = new QMimeData;
    mimeData->setData(jsonMimeType, json);
    return mimeData;
}

bool MachineListModel::canDropMimeData(const QMimeData *data,
                                       Qt::DropAction action,
                                       int row,
                                       int column,
                                       const QModelIndex &parent) const
{
    // Check that we have valid data and action
    if (data == nullptr || !supportedDropActions().testFlag(action)) {
        return false;
    }

    // Check that we have correct format
    if (!data->hasFormat(jsonMimeType)) {
        return false;
    }

    // Dropping over existing item, column and row must be -1
    if (parent.isValid()) {
        return column == -1 && row == -1;
    }

    // Dropping as last item?
    if (column == -1 && row == -1) {
        return true;
    }

    // Dropping outside of existing items
    return column == 0 && row >= 0 && row <= mMachines.size();
}

bool MachineListModel::dropMimeData(
    const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if (!canDropMimeData(data, action, row, column, parent)) {
        return false;
    }

    // Decode JSON data
    const auto json = data->data(jsonMimeType);
    QJsonParseError error{};
    const auto jsonDoc = QJsonDocument::fromJson(json, &error);
    if (error.error != QJsonParseError::NoError) {
        qCritical() << "Drop mime data got invalid JSON:" << error.errorString();
        return false;
    }

    // Decode machines
    const auto machinesVariantList = jsonDoc.toVariant().toList();
    QList<Machine> machines;
    for (const auto &machineVariant : machinesVariantList) {
        machines.append(Machine(machineVariant.toMap()));
    }

    // Find insert position
    int first = 0;
    if (row != -1) {
        first = row;
    } else if (parent.isValid()) {
        first = parent.row();
    } else {
        first = static_cast<int>(mMachines.size());
    }

    // Insert machines
    beginInsertRows({}, first, first + static_cast<int>(machines.size()) - 1);
    for (const auto &machine : machines) {
        mMachines.insert(first, machine);
        ++first;
    }
    endInsertRows();
    emit modelChanged();

    return true;
}

Qt::DropActions MachineListModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

bool MachineListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid() || count <= 0 || row < 0 || (row + count) > mMachines.size()) {
        return false;
    }
    beginRemoveRows({}, row, row + count - 1);
    auto it = mMachines.constBegin() + row;
    mMachines.erase(it, it + count);
    endRemoveRows();
    emit modelChanged();
    return true;
}

Qt::ItemFlags MachineListModel::flags(const QModelIndex &index) const
{
    auto flags = QAbstractListModel::flags(index) | Qt::ItemIsDropEnabled;
    if (index.isValid()) {
        flags.setFlag(Qt::ItemIsDragEnabled);
    }
    return flags;
}
