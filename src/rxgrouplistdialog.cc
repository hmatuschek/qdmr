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
  : QDialog(parent), _config(config), _groupList(""), _list(nullptr)
{
  construct();
}

RXGroupListDialog::RXGroupListDialog(Config *config, RXGroupList *list, QWidget *parent)
  : QDialog(parent), _config(config), _groupList(""), _list(list)
{
  if (_list)
    _groupList = *_list;
  construct();
}

RXGroupList *
RXGroupListDialog::groupList() {
  // Select which list to modify
  RXGroupList *list = _list;
  if (nullptr == list)
    list = new RXGroupList(groupListName->text().simplified());

  // Apply changes to group list
  _groupList.setName(groupListName->text().simplified());

  // Copy group list to target (either new or edited group list).
  (*list) = _groupList;

  // Return modified list.
  return list;
}

void
RXGroupListDialog::construct() {
  setupUi(this);

  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  connect(addContact, SIGNAL(clicked()), this, SLOT(onAddGroup()));
  connect(remContact, SIGNAL(clicked()), this, SLOT(onRemGroup()));

  groupListName->setText(_groupList.name());
  contactListView->setModel(new GroupListWrapper(&_groupList, contactListView));
}


void
RXGroupListDialog::onAddGroup() {
  MultiGroupCallSelectionDialog dialog(_config->contacts());
  if (QDialog::Accepted != dialog.exec())
    return;

  QList<DigitalContact *> contacts = dialog.contacts();
  foreach (DigitalContact *contact, contacts) {
    if (0 <= _groupList.contacts()->indexOf(contact))
      continue;
    _groupList.addContact(contact);
  }
}

void
RXGroupListDialog::onRemGroup() {
  if (!contactListView->hasSelection()) {
    QMessageBox::information(nullptr, tr("Cannot remove group call"),
                             tr("Cannot remove group call: You have to select at least one group call first."));
    return;
  }
  QPair<int,int> selection = contactListView->selection();
  // collect all selected group lists
  // need to collect them first as rows change when deleting
  QList<DigitalContact *> lists;
  for (int row=selection.first; row<=selection.second; row++)
    lists.push_back(_groupList.contact(row));
  // remove list
  foreach (DigitalContact *cont, lists)
    _groupList.contacts()->del(cont);
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
  for (int i=0; i<groups->count(); i++) {
    box->addItem(groups->list(i)->name(), QVariant::fromValue(groups->list(i)));
    if (groups->list(i) == list)
      box->setCurrentIndex(i+1);
  }
}
