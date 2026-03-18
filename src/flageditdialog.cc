#include "flageditdialog.hh"
#include "ui_flageditdialog.h"

FlagEditDialog::FlagEditDialog(QMetaEnum metaFlag, QWidget *parent)
  : QDialog(parent), ui(new Ui::FlagEditDialog), _metaFlag(metaFlag)
{
  ui->setupUi(this);
  for (int i=0; i<metaFlag.keyCount(); i++) {
    auto item = new QListWidgetItem(metaFlag.key(i));
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable
                   | Qt::ItemNeverHasChildren);
    item->setData(Qt::UserRole, QByteArray(metaFlag.key(i)));
    ui->list->addItem(item);
  }
}


int
FlagEditDialog::value() const {
  QByteArray keys;
  for (int i=0; i<ui->list->count(); i++) {
    if (Qt::Checked == ui->list->item(i)->checkState()) {
      if (! keys.isEmpty())
        keys += "|";
      keys.append(ui->list->item(i)->data(Qt::UserRole).toByteArray());
    }
  }
  if (keys.isEmpty())
    return 0;
  return _metaFlag.keysToValue(keys);
}


void
FlagEditDialog::setValue(int value) {
  QSet<QByteArray> keys;
  for (auto key: _metaFlag.valueToKeys(value).split('|'))
    keys.insert(key);
  for (int i=0; i<ui->list->count(); i++) {
    if (keys.contains(ui->list->item(i)->data(Qt::UserRole).toByteArray()))
      ui->list->item(i)->setCheckState(Qt::Checked);
    else
      ui->list->item(i)->setCheckState(Qt::Unchecked);
  }
}

FlagEditDialog::~FlagEditDialog() {
  delete ui;
}
