// Copyright (C) 2024 Ossi Saukko <osaukko@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file  machinedialog.h
 * @brief MachineDialog class definition
 */

#ifndef MACHINEDIALOG_H
#define MACHINEDIALOG_H

#include <QDialog>
#include "data/machine.h"

namespace Ui {
class MachineDialog;
} // namespace Ui

/**
 * @brief Dialog for configuring emulated machine setup
 * 
 * This dialog is used when the user defines a new machine setup to be
 * emulated or edits an existing one.
 *
 * Most setups will work fine using the 86Box emulator and default
 * commands specified in the settings dialog. However, if the user opens
 * the advanced settings view, he can define alternative start and settings
 * commands per profile. The advanced view can be toggled with the button
 * at the bottom left of the dialog.
 * 
 * @todo Allow users also to define alternative 86Box emulator binary from
 *       this dialog
 */
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
    void onButtonBoxAccepted();
    void onConfigToolButtonClicked();
    void onIconToolButtonClicked();

private:
    Ui::MachineDialog *mUi; /*!< @brief User interface generated from `machinedialog.ui` */

    /**
     * @brief Machine settings object
     *
     * This object allows us to keep any extra properties for the machine when editing an existing setup.
     * 
     * @see machine(), setMachine()
     */
    Machine mMachine;

    void setIcon();
    void setupIconsComboBox();
};

#endif // MACHINEDIALOG_H
