// Copyright (C) 2024 Ossi Saukko <osaukko@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file  machinelistmodel.cpp
 * @brief MachineListModel class implementation
 */

#include "machinelistmodel.h"

#include <QDebug>
#include <QIcon>
#include <QJsonDocument>
#include <QMimeData>

namespace {
const auto jsonMimeType = "application/json";
} // namespace

/**
 * @brief Construct a machine list model
 * @param[in] parent   Pointer to parent object
 */
MachineListModel::MachineListModel(QObject *parent)
    : QAbstractListModel{parent}
{
    connect(this, &MachineListModel::modelReset, this, &MachineListModel::modelChanged);
    connect(this, &MachineListModel::dataChanged, this, &MachineListModel::modelChanged);
    connect(this, &MachineListModel::rowsInserted, this, &MachineListModel::modelChanged);
    connect(this, &MachineListModel::rowsMoved, this, &MachineListModel::modelChanged);
    connect(this, &MachineListModel::rowsRemoved, this, &MachineListModel::modelChanged);
}

MachineListModel::~MachineListModel() = default;

/**
 * @brief Add a new *machine* to the model.
 *
 * This convenience function allows for the easy addition of a new
 * *machine* at the end of the list model.
 * 
 * @param[in] machine   Add this machine to the model
 */
void MachineListModel::addMachine(const Machine &machine)
{
    auto row = static_cast<int>(mMachines.size());
    beginInsertRows({}, row, row);
    mMachines.append(machine);
    endInsertRows();
}

/**
 * @brief Get Machine item from the *index*
 * @param[in] index   Get Machine from this index
 * @return Machine item from given *index*
 * @note Default machine is returned if given *index* is invalid
 */
Machine MachineListModel::machineForIndex(const QModelIndex &index) const
{
    return mMachines.value(index.row());
}

/**
 * @brief Replace the Machine item at the given *index*.
 *
 * This convenience function allows easy change of Machine items.
 * Only an error message is printed to the console if the given *index*
 * is invalid.
 * 
 * @param[in] index     Replace machine item at this index
 * @param[in] machine   Replace it with this machine
 */
void MachineListModel::setMachineForIndex(const QModelIndex &index, const Machine &machine)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= mMachines.size()) {
        qCritical() << "Invalid index:" << index;
        return;
    }
    mMachines[index.row()] = machine;
    emit dataChanged(index, index, {Qt::DecorationRole, Qt::DisplayRole, SummaryRole});
}

/**
 * @brief The convenience function is to remove the machine at *index*.
 * 
 * Only an error message is printed to the console if the given *index*
 * is invalid.
 * 
 * @param[in] index   Remove machine from this index
 */
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

/**
 * @brief Save all machine items into QVariantList
 *
 * This method calls the @ref Machine::save "save()" method for each
 * Machine item in the list, and stores returned data into QVariantList.
 * The complete QVariantList object is then returned.
 * 
 * @return QVariantList containing all the data for machines
 * @see MachineListModel::restore
 */
QVariantList MachineListModel::save() const
{
    QVariantList machineList;
    for (const auto &machine : mMachines) {
        machineList.append(machine.save());
    }
    return machineList;
}

/**
 * @brief Restore machines from the QVariantList
 * 
 * This method clears the model and restores all machines from the list.
 * If restoring a machine fails, an error message is printed to the
 * console, and the restoration is continued from the next machine.
 * 
 * @param[in] machines   Restore machines from this list
 * @see MachineListModel::save
 */
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

/**
 * @brief Row count for given *parent* index
 * 
 * Since this is a list model, we only return the machine list item
 * count when the index is invalid, aka when the view requests the row
 * count from the root index.
 * 
 * On valid indexes, we return zero.
 * 
 * @param[in] parent   Requesting row count under this parent
 * @return Machine item count for invalid indexes and zero for valid indexes
 */
int MachineListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0; // No children items
    }
    return static_cast<int>(mMachines.size());
}

/**
 * @brief Get item data from given *index* with the *role*
 * 
 * For valid indexes, we return the following information according to
 * the requested role:
 * 
 * - For `Qt::DecorationRole` we return Machine icon
 * - For `Qt::DisplayRole` we return Machine name
 * - For `MachineListModel::SummaryRole` we return Machine summary
 * 
 * For all other cases, an invalid variant is returned.
 * 
 * @param[in] index   Get data from this index
 * @param[in] role    Get data using this role
 * @return Machine icon, name, summary or invalid variant
 */
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

/**
 * @brief Model headers
 * 
 * In the horizontal direction, "Machines" is returned to the first
 * column. For other columns, an invalid variant is returned.
 * 
 * In the vertical direction, the header is returned as the line
 * number.
 * 
 * Only Qt::DisplayRole is accepted, and we return an invalid variant
 * for other roles.
 * 
 * @param[in] section       Header section number 
 * @param[in] orientation   Horizontal or vertical header
 * @param[in] role          Requestin header data with this role
 * @return Header test or invalid variant
 */
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

/**
 * @brief MIME types allowed for this model.
 *
 * We allow `"application/json"` for this model. This was added to
 * allow drag-and-drop sorting of Machine items in the list view.
 * 
 * @return MIME types allowed.
 */
QStringList MachineListModel::mimeTypes() const
{
    return {jsonMimeType};
}

/**
 * @brief Construct MIME data for machines from the *indexes*
 *
 * This function creates a QVariantList of machines from the given
 * *indexes*.  The list is then converted to JSON and returned as a
 * MIME data object.
 * 
 * @param[in] indexes   Get MIME data for these machines
 * @return MIME data for requested machines
 */
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

/**
 * @brief Check if the MIME data can be accepted
 * 
 * The function checks that *data* is valid and *action* is allowed.
 * It then checks that the MIME type is `"application/json"`. 
 * Finally, it checks where the object is being dropped. 
 * 
 * @param[in] data     Data about to be dropped
 * @param[in] action   Drop action
 * @param[in] row      Current row where to drop
 * @param[in] column   Current column where to drop
 * @param[in] parent   Index for the parent item
 * 
 * @return If everything is OK, then `true` and `false` otherwise.
 */
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

/**
 * @brief Handle dropped MIME data.
 *
 * The method checks that data can be accepted with the 
 * @ref canDropMimeData. It then decodes JSON data from the dropped
 * MIME data. The decoded JSON is converted to Machine items and
 * inserted into the drop location.
 * 
 * @param[in] data     Dropped MIME data
 * @param[in] action   Drop action
 * @param[in] row      Row where data was dropped
 * @param[in] column   Column where data was dropped
 * @param[in] parent   Parent index where data was dropped
 * 
 * @return `true` if data was accepted and inserted into the model,
 *         `false` otherwise 
 */
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

    return true;
}

/**
 * @brief Flags for supported drop actions
 * 
 * We only support `Qt::MoveAction`, which allows users to sort machine
 * items in the list view using drag and drop.
 * 
 * @return Qt::MoveAction
 */
Qt::DropActions MachineListModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

/**
 * @brief Remove machines from the model.
 *
 * This method allows removing one or multiple machines from the model.
 * 
 * @param[in] row      Start removing from this row
 * @param[in] count    How many machines to remove
 * @param[in] parent   Parent index (should be invalid)
 * 
 * @return `true` if machines were removed, `false` if something is wrong with the request
 * 
 * @see MachineListModel::remove
 */
bool MachineListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid() || count <= 0 || row < 0 || (row + count) > mMachines.size()) {
        return false;
    }
    beginRemoveRows({}, row, row + count - 1);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    auto it = mMachines.begin() + row;
#else
    auto it = mMachines.constBegin() + row;
#endif
    mMachines.erase(it, it + count);
    endRemoveRows();
    return true;
}

/**
 * @brief Item flags for given *index*
 * 
 * The method takes the item's default flags and adds
 * `Qt::ItemIsDropEnabled`. If the index is valid, 
 * `Qt::ItemIsDragEnabled` is also added.
 * 
 * @param[int] index   Get flags for this item
 * @return Flags for the item
 */
Qt::ItemFlags MachineListModel::flags(const QModelIndex &index) const
{
    auto flags = QAbstractListModel::flags(index) | Qt::ItemIsDropEnabled;
    if (index.isValid()) {
        flags.setFlag(Qt::ItemIsDragEnabled);
    }
    return flags;
}
