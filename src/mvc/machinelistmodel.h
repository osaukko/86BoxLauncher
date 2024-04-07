#ifndef MACHINELISTMODEL_H
#define MACHINELISTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include "data/machine.h"

class MachineListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(MachineListModel)

public:
    enum ItemRole { SummaryRole = Qt::UserRole + 1 };
    Q_ENUM(ItemRole);

    explicit MachineListModel(QObject *parent = nullptr);
    ~MachineListModel() override;

    void addMachine(const Machine &machine);
    [[nodiscard]] Machine machineForIndex(const QModelIndex &index) const;
    void setMachineForIndex(const QModelIndex &index, const Machine &machine);
    void remove(const QModelIndex &index);

    [[nodiscard]] QVariantList save() const;
    void restore(const QVariantList &machines);

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
    QList<Machine> mMachines;
};

#endif // MACHINELISTMODEL_H
