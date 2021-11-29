#include "rxgrouplistdialog.hh"
#include "config.hh"
#include "contact.hh"
#include "contactselectiondialog.hh"
#include "settings.hh"

#include <QInputDialog>
#include <QMessageBox>


/* ********************************************************************************************* *
 * Implementation of RXGroupListDialog
 * ********************************************************************************************* */
RXGroupListDialog::RXGroupListDialog(Config *config, QWidget *parent)
  : QDialog(parent), _config(config), _myGroupList(new RXGroupList(this)), _list(nullptr)
{
  setWindowTitle(tr("Create Group List"));
  construct();
}

RXGroupListDialog::RXGroupListDialog(Config *config, RXGroupList *list, QWidget *parent)
  : QDialog(parent), _config(config), _myGroupList(new RXGroupList(this)), _list(list)
{
  if (_list)
    _myGroupList->copy(*_list);

  construct();
}

RXGroupList *
RXGroupListDialog::groupList() {
  _myGroupList->setName(groupListName->text().simplified());

  RXGroupList *list = _myGroupList;
  if (_list) {
    _list->copy(*_myGroupList);
    list = _myGroupList;
  } else {
    _myGroupList->setParent(nullptr);
  }

  // Return modified list.
  return list;
}

void
RXGroupListDialog::construct() {
  setupUi(this);
  Settings settings;

  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  connect(addContact, SIGNAL(clicked()), this, SLOT(onAddGroup()));
  connect(remContact, SIGNAL(clicked()), this, SLOT(onRemGroup()));

  groupListName->setText(_myGroupList->name());
  contactListView->setModel(new GroupListWrapper(_myGroupList, contactListView));

  extensionView->setObject(_myGroupList);
  if (! settings.showCommercialFeatures())
    tabWidget->tabBar()->hide();
}


void
RXGroupListDialog::onAddGroup() {
  MultiGroupCallSelectionDialog dialog(_config->contacts());
  if (QDialog::Accepted != dialog.exec())
    return;

  QList<DigitalContact *> contacts = dialog.contacts();
  foreach (DigitalContact *contact, contacts) {
    if (0 <= _myGroupList->contacts()->indexOf(contact))
      continue;
    _myGroupList->addContact(contact);
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
    lists.push_back(_myGroupList->contact(row));
  // remove list
  foreach (DigitalContact *cont, lists)
    _myGroupList->contacts()->del(cont);
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
