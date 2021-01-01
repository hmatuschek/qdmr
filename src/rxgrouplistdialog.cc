#include "rxgrouplistdialog.hh"
#include "config.hh"
#include "contact.hh"
#include "contactselectiondialog.hh"

#include <QInputDialog>
#include <QMessageBox>


/* ********************************************************************************************* *
 * Implementation of RXGroupListDialog
 * ********************************************************************************************* */
RXGroupListDialog::RXGroupListDialog(Config *config, QWidget *parent)
  : QDialog(parent), _config(config), _list(nullptr)
{
  construct();
}

RXGroupListDialog::RXGroupListDialog(Config *config, RXGroupList *list, QWidget *parent)
  : QDialog(parent), _config(config), _list(list)
{
  construct();
}

RXGroupList *
RXGroupListDialog::groupList() {
  if (_list) {
    _list->setName(groupListName->text());
    _list->clear();
    for (int i=0; i<groupListWidget->count(); i++) {
      _list->addContact(groupListWidget->item(i)->data(Qt::UserRole).value<DigitalContact*>());
    }
    return _list;
  }

  RXGroupList *list = new RXGroupList(groupListName->text());
  for (int i=0; i<list->count(); i++) {
    list->addContact(groupListWidget->item(i)->data(Qt::UserRole).value<DigitalContact*>());
  }
  return list;
}

void
RXGroupListDialog::construct() {
  setupUi(this);

  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  connect(addContact, SIGNAL(clicked()), this, SLOT(onAddGroup()));
  connect(remContact, SIGNAL(clicked()), this, SLOT(onRemGroup()));
  connect(groupUp, SIGNAL(clicked(bool)), this, SLOT(onGroupUp()));
  connect(groupDown, SIGNAL(clicked(bool)), this, SLOT(onGroupDown()));

  if (_list) {
    groupListName->setText(_list->name());
    for (int i=0; i<_list->rowCount(QModelIndex()); i++) {
      QListWidgetItem *item = new QListWidgetItem(_list->contact(i)->name());
      item->setData(Qt::UserRole, QVariant::fromValue(_list->contact(i)));
      groupListWidget->addItem(item);
    }
  }
}


void
RXGroupListDialog::onAddGroup() {
  MultiGroupCallSelectionDialog dialog(_config->contacts());
  if (QDialog::Accepted != dialog.exec())
    return;

  QList<DigitalContact *> contacts = dialog.contacts();
  foreach (DigitalContact *contact, contacts) {
    if (groupListWidget->findItems(contact->name(), Qt::MatchExactly).size())
      continue;
    QListWidgetItem *item = new QListWidgetItem(contact->name());
    item->setData(Qt::UserRole, QVariant::fromValue(contact));
    groupListWidget->addItem(item);
  }
}

void
RXGroupListDialog::onRemGroup() {
  if (0 == groupListWidget->selectedItems().size()) {
    QMessageBox::information(nullptr, tr("Cannot remove group call"),
                             tr("Cannot remove group call: You have to select at least one group call first."));
    return;
  }

  QList<QListWidgetItem *> selection = groupListWidget->selectedItems();
  foreach (QListWidgetItem *item, selection) {
    int row = groupListWidget->row(item);
    groupListWidget->takeItem(row);
    delete item;
  }
}

void
RXGroupListDialog::onGroupUp() {
  if (1 != groupListWidget->selectedItems().size())
    return;
  int idx = groupListWidget->currentRow();
  if (0 == idx)
    return;
  QListWidgetItem *item = groupListWidget->takeItem(idx);
  groupListWidget->insertItem(idx-1, item);
  groupListWidget->setCurrentRow(idx-1);
}

void
RXGroupListDialog::onGroupDown() {
  if (1 != groupListWidget->selectedItems().size())
    return;
  int idx = groupListWidget->currentRow();
  if ((groupListWidget->count()-1) <= idx)
    return;
  QListWidgetItem *item = groupListWidget->takeItem(idx);
  groupListWidget->insertItem(idx+1, item);
  groupListWidget->setCurrentRow(idx+1);
}

/* ********************************************************************************************* *
 * Implementation of RXGroupListBox
 * ********************************************************************************************* */
RXGroupListBox::RXGroupListBox(RXGroupLists *groups, QWidget *parent)
  : QComboBox(parent)
{
  populateRXGroupListBox(this, groups);
}

void
populateRXGroupListBox(QComboBox *box, RXGroupLists *groups, RXGroupList *list) {
  box->addItem(QObject::tr("[None]"), QVariant::fromValue(nullptr));
  for (int i=0; i<groups->rowCount(QModelIndex()); i++) {
    box->addItem(groups->list(i)->name(), QVariant::fromValue(groups->list(i)));
    if (groups->list(i) == list)
      box->setCurrentIndex(i+1);
  }
}
