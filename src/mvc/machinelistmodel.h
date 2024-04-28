// Copyright (C) 2024 Ossi Saukko <osaukko@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file  machinelistmodel.h
 * @brief MachineListModel class definition
 */

#ifndef MACHINELISTMODEL_H
#define MACHINELISTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include "data/machine.h"

/**
 * @brief List model of Machine items
 *
 * This model provides the following information according to their role:
 * - `Qt::DecorationRole`            -> Machine icon
 * - `Qt::DisplayRole`               -> Machine name
 * - `MachineListModel::SummaryRole` -> Machine summary
 * 
 * In addition, @ref machineForIndex allows the Machine object to be
 * retrieved from the given index.
 * 
 * A @ref modelChanged signal is sent for any changes to the model.
 * Main window uses this signal to know when machine configurations 
 * should be written into the file.
 */
class MachineListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(MachineListModel)

public:
    /**
     * @brief Custom item roles for this model
     */
    enum ItemRole {
        SummaryRole = Qt::UserRole + 1 /*!< @brief The summary data for the machine. (QString) */
    };
    Q_ENUM(ItemRole); /*!< @brief Registering ItemRole to meta-object system */

    explicit MachineListModel(QObject *parent = nullptr);
    ~MachineListModel() override;

    void addMachine(const Machine &machine);
    [[nodiscard]] Machine machineForIndex(const QModelIndex &index) const;
    void setMachineForIndex(const QModelIndex &index, const Machine &machine);
    void remove(const QModelIndex &index);

    [[nodiscard]] QVariantList save() const;
    void restore(const QVariantList &machines);

signals:
    /**
     * @brief This model was just modified
     */
    void modelChanged();

    // QAbstractItemModel interface
public:
    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    [[nodiscard]] QVariant headerData(int section,
                                      Qt::Orientation orientation,
                                      int role) const override;
    [[nodiscard]] QStringList mimeTypes() const override;
    [[nodiscard]] QMimeData *mimeData(const QModelIndexList &indexes) const override;
    [[nodiscard]] bool canDropMimeData(const QMimeData *data,
                                       Qt::DropAction action,
                                       int row,
                                       int column,
                                       const QModelIndex &parent) const override;
    [[nodiscard]] bool dropMimeData(const QMimeData *data,
                                    Qt::DropAction action,
                                    int row,
                                    int column,
                                    const QModelIndex &parent) override;
    [[nodiscard]] Qt::DropActions supportedDropActions() const override;
    [[nodiscard]] bool removeRows(int row, int count, const QModelIndex &parent) override;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    QList<Machine> mMachines; /*!< @brief Data for the model */
};

#endif // MACHINELISTMODEL_H
