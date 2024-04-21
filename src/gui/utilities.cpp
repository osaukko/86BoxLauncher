#include "utilities.h"
#include <QAbstractButton>
#include <QDialogButtonBox>

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
