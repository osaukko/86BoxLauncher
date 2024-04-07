#ifndef MACHINEDELEGATE_H
#define MACHINEDELEGATE_H

#include <QStyledItemDelegate>

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
