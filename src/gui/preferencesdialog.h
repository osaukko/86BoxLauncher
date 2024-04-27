// Copyright (C) 2024 Ossi Saukko <osaukko@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file  preferencesdialog.h
 * @brief PreferencesDialog class definition
 */

#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>

class Settings;
class QAbstractButton;

namespace Ui {
class PreferencesDialog;
} // namespace Ui

/**
 * @brief 86BoxLauncher settings dialog
 * 
 * This dialog allows users to specify the default settings to be used
 * when emulated machines are started or their settings are opened.
 */
class PreferencesDialog : public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(PreferencesDialog)

public:
    explicit PreferencesDialog(Settings *settings, QWidget *parent = nullptr);
    ~PreferencesDialog() override;

private slots:
    void onAccepted();
    void onButtonClicked(QAbstractButton *button);
    void onEmulatorBrowseButtonClicked();

private:
    Ui::PreferencesDialog *mUi; /*!< @brief User interface generated from `preferencesdialog.ui` */
    Settings *mSettings;        /*!< @brief Borrowed settings object */

    void loadSettings();
};

#endif // PREFERENCESDIALOG_H
