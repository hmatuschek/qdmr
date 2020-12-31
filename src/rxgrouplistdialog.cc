#include "rxgrouplistdialog.hh"
#include "config.hh"
#include "contact.hh"
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
  QStringList names;
  QVector<DigitalContact *> contacts;
  ContactList *lst = _config->contacts();
  for (int i=0; i<lst->rowCount(QModelIndex()); i++) {
    if (! lst->contact(i)->is<DigitalContact>())
      continue;
    DigitalContact *contact = lst->contact(i)->as<DigitalContact>();
    if (DigitalContact::PrivateCall == contact->type())
      continue;
    names.append(contact->name());
    contacts.append(contact);
  }

  bool ok=false;
  QString name  = QInputDialog::getItem(0, tr("Select group call"),
                                        tr("Select a group call to add:"), names, 0, false, &ok);
  if ((! ok) || (! names.contains(name)))
    return;

  QListWidgetItem *item = new QListWidgetItem(name);
  item->setData(Qt::UserRole, QVariant::fromValue(contacts[names.indexOf(name)]));
  groupListWidget->addItem(item);
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
