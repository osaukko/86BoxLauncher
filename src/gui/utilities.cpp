// Copyright (C) 2024 Ossi Saukko <osaukko@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file  utilities.cpp
 * @brief Helper tool implementations
 */

#include "utilities.h"

#include <QAbstractButton>
#include <QDialogButtonBox>

/**
 * @brief Set icons for known buttons in the QDialogButtonBox.
 * 
 * This function was added when we found that some or even all dialog
 * buttons were missing icons on some operating systems. The function
 * iterates over all buttons in the button box and sets icons for known
 * types, the ones that we use. This may be redundant on some systems
 * but does not cause too much extra processing.
 * 
 * @param[in] buttonBox   Set icons for this button box
 */
void utilities::setDialogBoxIcons(QDialogButtonBox *buttonBox)
{
    Q_ASSERT(buttonBox != nullptr);
    for (auto *button : buttonBox->buttons()) {
        switch (buttonBox->standardButton(button)) {
        case QDialogButtonBox::Ok:
            button->setIcon(QIcon::fromTheme("dialog-ok"));
            break;
        case QDialogButtonBox::Cancel:
            button->setIcon(QIcon::fromTheme("dialog-cancel"));
            break;
        case QDialogButtonBox::RestoreDefaults:
            button->setIcon(QIcon::fromTheme("document-revert"));
            break;
        default:
            break;
        }
    }
}
