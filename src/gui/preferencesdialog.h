#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>

namespace Ui {
class PreferencesDialog;
} // namespace Ui

class PreferencesDialog : public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(PreferencesDialog)

public:
    explicit PreferencesDialog(QWidget *parent = nullptr);
    ~PreferencesDialog() override;

private:
    Ui::PreferencesDialog *mUi;
};

#endif // PREFERENCESDIALOG_H
