#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

PreferencesDialog::PreferencesDialog(QWidget *parent)
    : QDialog(parent)
    , mUi(new Ui::PreferencesDialog)
{
    mUi->setupUi(this);
}

PreferencesDialog::~PreferencesDialog()
{
    delete mUi;
}
