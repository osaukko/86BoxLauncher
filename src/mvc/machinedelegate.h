// Copyright (C) 2024 Ossi Saukko <osaukko@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file  machinedelegate.h
 * @brief MachineDelegate class definition
 */

#ifndef MACHINEDELEGATE_H
#define MACHINEDELEGATE_H

#include <QStyledItemDelegate>

/**
 * @brief Custom painting for Machine items
 * 
 * This delegate is installed in the main window's list view to display
 * machine items with icons, names, and summaries.
 * 
 * This delegate uses the following layout:\n
 * <img src="MachineDelegate-Layout.svg" alt="Machine item layout">
 */
class MachineDelegate : public QStyledItemDelegate
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(MachineDelegate)

public:
    explicit MachineDelegate(QObject *parent = nullptr);
    ~MachineDelegate() override;

    // QAbstractItemDelegate interface
public:
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
    [[nodiscard]] QSize sizeHint(const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const override;

private:
    [[nodiscard]] QStyle *getStyle() const;
    QSize calculateLayout(const QStyleOptionViewItem &option,
                          const QModelIndex &index,
                          QRect *iconArea = nullptr,
                          QRect *nameArea = nullptr,
                          QRect *summaryArea = nullptr) const;
};

#endif // MACHINEDELEGATE_H
