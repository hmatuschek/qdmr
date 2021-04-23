#include "radioid.hh"


/* ********************************************************************************************* *
 * Implementation of RadioID
 * ********************************************************************************************* */
RadioID::RadioID(uint32_t id, QObject *parent)
  : QObject(parent), _id(id)
{
  // pass...
}

uint32_t
RadioID::id() const {
  return _id;
}

void
RadioID::setId(uint32_t id) {
  if (id == _id)
    return;
  _id = id;
  emit modified();
}


/* ********************************************************************************************* *
 * Implementation of RadioIDList
 * ********************************************************************************************* */
RadioIDList::RadioIDList(QObject *parent)
  : QAbstractListModel(parent), _ids()
{
  _ids.append(new RadioID(0));
  connect(_ids.at(0), SIGNAL(destroyed(QObject*)), this, SLOT(onIdDeleted(QObject*)));
}

void
RadioIDList::clear() {
  foreach (RadioID *id, _ids) {
    id->deleteLater();
  }
  _ids.clear();
  _ids.push_back(new RadioID(0));
  connect(_ids.at(0), SIGNAL(destroyed(QObject*)), this, SLOT(onIdDeleted(QObject*)));
}

int
RadioIDList::count() const {
  return _ids.size();
}

RadioID *
RadioIDList::getId(uint idx) const {
  if (int(idx) >= _ids.count())
    return nullptr;
  return _ids.at(idx);
}

RadioID *
RadioIDList::find(uint32_t id) const {
  foreach (RadioID *item, _ids) {
    if (id == item->id())
      return item;
  }
  return nullptr;
}

int
RadioIDList::indexOf(RadioID *id) const {
  return _ids.indexOf(id);
}

int
RadioIDList::addId(RadioID *id) {
  if (nullptr == id)
    return -1;
  int r = _ids.count();
  beginInsertRows(QModelIndex(), r,r);
  id->setParent(this);
  _ids.append(id);
  connect(id, SIGNAL(destroyed(QObject*)), this, SLOT(onIdDeleted(QObject*)));
  endInsertRows();
  return r;
}

int
RadioIDList::addId(uint32_t id) {
  return addId(new RadioID(id, this));
}

bool
RadioIDList::setDefault(uint idx) {
  if (int(idx) >= count())
    return false;
  if (0 == idx)
    return true;
  beginMoveRows(QModelIndex(), idx, idx, QModelIndex(), 0);
  RadioID *obj = _ids.at(idx);
  _ids.removeAt(idx);
  _ids.prepend(obj);
  endMoveRows();
  return true;
}

bool
RadioIDList::remId(RadioID *id) {
  if ((nullptr == id) || (! _ids.contains(id)))
    return false;
  int idx = _ids.indexOf(id);
  beginRemoveRows(QModelIndex(), idx, idx);
  _ids.removeAt(idx);
  disconnect(id, SIGNAL(destroyed(QObject*)), this, SLOT(onIdDeleted(QObject*)));
  id->deleteLater();
  endRemoveRows();
  return true;
}

bool
RadioIDList::remId(uint32_t id) {
  return remId(find(id));
}

void
RadioIDList::onIdDeleted(QObject *obj) {
  remId(qobject_cast<RadioID *>(obj));
}

int
RadioIDList::rowCount(const QModelIndex &parent) const {
  return count();
}

QVariant
RadioIDList::data(const QModelIndex &index, int role) const {
  if (index.row() >= count())
    return QVariant();
  if (Qt::DisplayRole == role)
    return QVariant(QString::number(getId(index.row())->id()));
  else if (Qt::EditRole == role)
    return QVariant(getId(index.row())->id());
  return QVariant();
}
