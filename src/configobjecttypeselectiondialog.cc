#include "configobjecttypeselectiondialog.hh"
#include "ui_configobjecttypeselectiondialog.h"
#include "logger.hh"

ConfigObjectTypeSelectionDialog::ConfigObjectTypeSelectionDialog(
    const QList<QMetaObject> &cls, QWidget *parent)
  : QDialog(parent), ui(new Ui::ConfigObjectTypeSelectionDialog), _types(cls)
{
  ui->setupUi(this);
  // Iterate over all classes
  foreach (const QMetaObject type, _types) {
    logDebug() << "Inspect class '" << type.className() << "'.";
    ui->typeSelection->addItem(type.className());
  }
}

ConfigObjectTypeSelectionDialog::~ConfigObjectTypeSelectionDialog()
{
  delete ui;
}

const QMetaObject &
ConfigObjectTypeSelectionDialog::selectedType() const {
  return _types.at(ui->typeSelection->currentIndex());
}
