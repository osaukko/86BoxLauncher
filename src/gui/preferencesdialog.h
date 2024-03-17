#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>

class Settings;
class QAbstractButton;

namespace Ui {
class PreferencesDialog;
} // namespace Ui

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
    Ui::PreferencesDialog *mUi;
    Settings *mSettings;

    void loadSettings();
};

#endif // PREFERENCESDIALOG_H
