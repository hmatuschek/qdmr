#include "configobjecttypeselectiondialog.hh"
#include "ui_configobjecttypeselectiondialog.h"
#include "logger.hh"
#include <QMetaClassInfo>

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

  if (0 <= ui->typeSelection->currentIndex())
    onSelectionChanged(ui->typeSelection->currentIndex());

  ui->description->setWordWrap(true);
  ui->description->setTextFormat(Qt::RichText);

  connect(ui->typeSelection, SIGNAL(currentIndexChanged(int)), this, SLOT(onSelectionChanged(int)));
}

ConfigObjectTypeSelectionDialog::~ConfigObjectTypeSelectionDialog()
{
  delete ui;
}

const QMetaObject &
ConfigObjectTypeSelectionDialog::selectedType() const {
  return _types.at(ui->typeSelection->currentIndex());
}

void
ConfigObjectTypeSelectionDialog::onSelectionChanged(int currentIndex) {
  ui->description->clear();
  if (_types.count() <= currentIndex)
    return;

  QMetaObject meta = _types.at(currentIndex);
  QString description = tr("An instance of %1.").arg(meta.className());
  QString longDescription;

  for (int i=meta.classInfoOffset(); i<meta.classInfoCount(); i++) {
    QMetaClassInfo info = meta.classInfo(i);
    if (0 == strcmp("description", info.name()))
      description = info.value();
    if (0 == strcmp("longDescription", info.name()))
      longDescription = info.value();
  }

  ui->description->setText(tr("<p>%1<p><p style=\"margin-left:10px;\">%2</p>").arg(description).arg(longDescription));
}
