#ifndef MACHINEDIALOG_H
#define MACHINEDIALOG_H

#include <QDialog>
#include "data/machine.h"

namespace Ui {
class MachineDialog;
} // namespace Ui

class MachineDialog : public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(MachineDialog)

public:
    explicit MachineDialog(QWidget *parent = nullptr);
    ~MachineDialog() override;

    [[nodiscard]] Machine machine() const;
    void setMachine(const Machine &machine);

private slots:
    void onAdvancedButtonToggled();

private:
    Ui::MachineDialog *mUi;
    Machine mMachine;

    void setMachineVariables();
};

#endif // MACHINEDIALOG_H
