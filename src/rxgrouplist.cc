#include "rxgrouplist.hh"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QDialog>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QLineEdit>
#include <QLabel>
#include <QInputDialog>
#include "config.hh"
#include <QDebug>


/* ********************************************************************************************* *
 * Implementation of RXGroupList
 * ********************************************************************************************* */
RXGroupList::RXGroupList(const QString &name, QObject *parent)
  : QAbstractListModel(parent), _name(name), _contacts()
{
  // pass...
}

int
RXGroupList::count() const {
  return _contacts.size();
}

void
RXGroupList::clear() {
  beginResetModel();
  _contacts.clear();
  endResetModel();
  emit modified();
}

const QString &
RXGroupList::name() const {
  return _name;
}

bool
RXGroupList::setName(const QString &name) {
  if (name.simplified().isEmpty())
    return false;
  _name = name.simplified();
  return true;
}

DigitalContact *
RXGroupList::contact(int idx) const {
  if (idx >= _contacts.size())
    return nullptr;
  return _contacts.at(idx);
}

int
RXGroupList::addContact(DigitalContact *contact) {
  if (_contacts.contains(contact))
    return -1;
  int idx = _contacts.size();
  beginInsertRows(QModelIndex(), idx, idx);
  connect(contact, SIGNAL(modified()), this, SIGNAL(modified()));
  connect(contact, SIGNAL(destroyed(QObject*)), this, SLOT(onContactDeleted(QObject*)));
  _contacts.append(contact);
  endInsertRows();
  emit modified();
  return idx;
}

bool
RXGroupList::remContact(int idx) {
  if (idx >= _contacts.size())
    return false;
  beginRemoveRows(QModelIndex(), idx, idx);
  _contacts.remove(idx);
  endRemoveRows();
  emit modified();
  return true;
}

bool
RXGroupList::remContact(DigitalContact *contact) {
  if (! _contacts.contains(contact))
    return false;
  int idx = _contacts.indexOf(contact);
  return remContact(idx);
}

void
RXGroupList::onContactDeleted(QObject *obj) {
  if (DigitalContact *contact = reinterpret_cast<DigitalContact *>(obj))
    remContact(contact);
}

int
RXGroupList::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return _contacts.size();
}

QVariant
RXGroupList::data(const QModelIndex &index, int role) const {
  if ((Qt::DisplayRole!=role) || (! index.isValid()) || (index.row()>=_contacts.size()))
    return QVariant();
  return _contacts[index.row()]->name();
}

QVariant
RXGroupList::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((0!=section) || (Qt::Horizontal!=orientation) || (Qt::DisplayRole!=role))
    return QVariant();
  return tr("Group Call Contacts");
}


/* ********************************************************************************************* *
 * Implementation of RXGroupLists
 * ********************************************************************************************* */
RXGroupLists::RXGroupLists(QObject *parent)
  : QAbstractListModel(parent), _lists()
{
  // pass...
}

int
RXGroupLists::count() const {
  return _lists.size();
}

void
RXGroupLists::clear() {
  for (int i=0; i<count(); i++)
    _lists[i]->deleteLater();
}


int
RXGroupLists::indexOf(RXGroupList *list) const {
  if (! _lists.contains(list))
    return -1;
  return _lists.indexOf(list);
}

RXGroupList *
RXGroupLists::list(int idx) const {
  if (idx >= _lists.size())
    return nullptr;
  return _lists.at(idx);
}

int
RXGroupLists::addList(RXGroupList *list) {
  if (_lists.contains(list))
    return -1;
  int idx = _lists.size();
  beginInsertRows(QModelIndex(), idx, idx);
  connect(list, SIGNAL(modified()), this, SIGNAL(modified()));
  connect(list, SIGNAL(destroyed(QObject *)), this, SLOT(onListDeleted(QObject *)));
  list->setParent(this);
  _lists.append(list);
  endInsertRows();
  emit modified();
  return idx;
}

bool
RXGroupLists::remList(int idx) {
  if (idx >= _lists.size())
    return false;
  beginRemoveRows(QModelIndex(), idx, idx);
  _lists.remove(idx);
  endRemoveRows();
  emit modified();
  return true;
}

bool
RXGroupLists::remList(RXGroupList *list) {
  if (! _lists.contains(list))
    return false;
  int idx = _lists.indexOf(list);
  return remList(idx);
}

bool
RXGroupLists::moveUp(int row) {
  if ((row <= 0) || (row>=count()))
    return false;
  beginMoveRows(QModelIndex(), row, row, QModelIndex(), row-1);
  std::swap(_lists[row-1],_lists[row]);
  endMoveRows();
  emit modified();
  return true;
}

bool
RXGroupLists::moveDown(int row) {
  if ((row >= (count()-1)) || (row>=count()))
    return false;
  beginMoveRows(QModelIndex(), row, row, QModelIndex(), row+2);
  std::swap(_lists[row+1], _lists[row]);
  endMoveRows();
  emit modified();
  return true;
}

int
RXGroupLists::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return _lists.size();
}

QVariant
RXGroupLists::data(const QModelIndex &index, int role) const {
  if ((Qt::DisplayRole!=role) || (! index.isValid()) || (index.row()>=_lists.size()))
    return QVariant();
  return _lists[index.row()]->name();
}

QVariant
RXGroupLists::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((0!=section) || (Qt::Horizontal!=orientation) || (Qt::DisplayRole!=role))
    return QVariant();
  return tr("RX Group Lists");
}

void
RXGroupLists::onListDeleted(QObject *obj) {
  if (RXGroupList *list = reinterpret_cast<RXGroupList *>(obj))
    remList(list);
}


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
    if (DigitalContact::GroupCall != contact->type())
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
                             tr("Cannot remove group call: You have to select a group call first."));
    return;
  }

  QListWidgetItem *item = groupListWidget->takeItem(groupListWidget->currentRow());
  delete item;
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
