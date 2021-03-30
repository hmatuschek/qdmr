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
  beginResetModel();
  for (int i=0; i<count(); i++)
    _lists[i]->deleteLater();
  _lists.clear();
  endResetModel();
  emit modified();
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
RXGroupLists::addList(RXGroupList *list, int row) {
  if (_lists.contains(list))
    return -1;
  if ((row<0) || (row>_lists.size()))
    row = _lists.size();
  beginInsertRows(QModelIndex(), row, row);
  connect(list, SIGNAL(modified()), this, SIGNAL(modified()));
  connect(list, SIGNAL(destroyed(QObject *)), this, SLOT(onListDeleted(QObject *)));
  list->setParent(this);
  _lists.insert(row, list);
  endInsertRows();
  emit modified();
  return row;
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
RXGroupLists::moveUp(int first, int last) {
  if ((first <= 0) || (last >= count()))
    return false;
  beginMoveRows(QModelIndex(), first, last, QModelIndex(), first-1);
  for (int row=first; row<=last; row++)
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

bool
RXGroupLists::moveDown(int first, int last) {
  if ((last >= (count()-1)) || (first<0))
    return false;
  beginMoveRows(QModelIndex(), first, last, QModelIndex(), last+2);
  for (int row=last; row>=first; row--)
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


