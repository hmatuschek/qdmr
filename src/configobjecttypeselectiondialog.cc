#include "configobjecttypeselectiondialog.hh"
#include "ui_configobjecttypeselectiondialog.h"
#include "logger.hh"

ConfigObjectTypeSelectionDialog::ConfigObjectTypeSelectionDialog(const QMetaObject &cls, QWidget *parent)
  : QDialog(parent), ui(new Ui::ConfigObjectTypeSelectionDialog)
{
  ui->setupUi(this);
  // Iterate over all classes that inherit from the given type and have an invokeable constructor.
  for (int i=QMetaType::User; QMetaType(i).isValid(); i++) {
    const QMetaObject *meta = QMetaType::metaObjectForType(i);
    if (nullptr == meta)
      continue;
    logDebug() << "Check type '" << meta->className() << "'...";
    if (meta->inherits(&cls))
      ui->typeSelection->addItem(meta->className(), i);
  }
}

ConfigObjectTypeSelectionDialog::~ConfigObjectTypeSelectionDialog()
{
  delete ui;
}
