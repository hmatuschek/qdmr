#include "roaming.hh"

/* ********************************************************************************************* *
 * Implementation of RoamingZone
 * ********************************************************************************************* */
RoamingZone::RoamingZone(const QString &name, QObject *parent)
  : QAbstractListModel(parent), _name(name), _channel()
{
  // pass...
}

int
RoamingZone::count() const {
  return _channel.size();
}

void
RoamingZone::clear() {
  _channel.clear();
}


const QString &
RoamingZone::name() const {
  return _name;
}

void
RoamingZone::setName(const QString &name) {
  _name = name;
  emit modified();
}


DigitalChannel *
RoamingZone::channel(int idx) const {
  if ((idx < 0) || (idx >= count()))
    return nullptr;
  return _channel.at(idx);
}

bool
RoamingZone::addChannel(DigitalChannel *ch, int row) {
  if (_channel.contains(ch))
    return false;
  if (row <=0 || row>=count())
    row = count();
  beginInsertRows(QModelIndex(), row, row);
  connect(ch, SIGNAL(destroyed(QObject*)), this, SLOT(onChannelDeleted(QObject*)));
  connect(ch, SIGNAL(modified()), this, SIGNAL(modified()));
  _channel.insert(row, ch);
  endInsertRows();
  emit modified();
  return false;
}

bool
RoamingZone::remChannel(int row) {
  if (row<0 || row>=count())
    return false;
  DigitalChannel *ch = _channel[row];
  beginRemoveRows(QModelIndex(), row, row);
  _channel.removeAt(row);
  endRemoveRows();
  disconnect(ch, SIGNAL(destroyed(QObject*)), this, SLOT(onChannelDeleted(QObject*)));
  disconnect(ch, SIGNAL(modified()), this, SIGNAL(modified()));
  emit modified();
  return true;
}

bool
RoamingZone::remChannel(DigitalChannel *ch) {
  if (! _channel.contains(ch))
    return false;
  return remChannel(_channel.indexOf(ch));
}


int
RoamingZone::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return count();
}

QVariant
RoamingZone::data(const QModelIndex &index, int role) const {
  if ((Qt::DisplayRole!=role) || (index.row()>=_channel.size()) || (0 != index.column()))
    return QVariant();
  return _channel[index.row()]->name();
}

QVariant
RoamingZone::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((Qt::DisplayRole!=role) || (Qt::Horizontal!=orientation) || (0 != section))
    return QVariant();
  return tr("Channel");
}

void
RoamingZone::onChannelDeleted(QObject *obj) {
  if (DigitalChannel *ch = reinterpret_cast<DigitalChannel *>(obj))
    remChannel(ch);
}


/* ********************************************************************************************* *
 * Implementation of RoamingZoneList
 * ********************************************************************************************* */
RoamingZoneList::RoamingZoneList(QObject *parent)
  : QAbstractListModel(parent), _zones()
{
  // pass...
}

int
RoamingZoneList::count() const {
  return _zones.size();
}

void
RoamingZoneList::clear() {
  for (int i=0; i<count(); i++)
    _zones[i]->deleteLater();
  _zones.clear();
}

RoamingZone *
RoamingZoneList::zone(int idx) const {
  if ((0>idx) || (idx>=_zones.size()))
    return nullptr;
  return _zones[idx];
}

bool
RoamingZoneList::addZone(RoamingZone *zone, int row) {
  if (_zones.contains(zone))
    return false;
  if ((row<0) || (row>=_zones.size()))
    row = _zones.size();
  beginInsertRows(QModelIndex(), row, row);
  zone->setParent(this);
  connect(zone, SIGNAL(destroyed(QObject*)), this, SLOT(onZoneDeleted(QObject*)));
  connect(zone, SIGNAL(modified()), this, SIGNAL(modified()));
  _zones.insert(row, zone);
  endInsertRows();
  emit modified();
  return true;
}

bool
RoamingZoneList::remZone(int idx) {
  if ((0>idx) || (idx>=_zones.size()))
    return false;
  RoamingZone *zone = _zones[idx];
  beginRemoveRows(QModelIndex(), idx, idx);
  _zones.remove(idx);
  zone->deleteLater();
  endRemoveRows();
  emit modified();
  return true;
}

bool
RoamingZoneList::remZone(RoamingZone *zone) {
  if (! _zones.contains(zone))
    return false;
  int idx = _zones.indexOf(zone);
  return remZone(idx);
}

bool
RoamingZoneList::moveUp(int row) {
  if ((0>=row) || (row>=count()))
    return false;
  beginMoveRows(QModelIndex(), row, row, QModelIndex(), row-1);
  std::swap(_zones[row], _zones[row-1]);
  endMoveRows();
  emit modified();
  return true;
}

bool
RoamingZoneList::moveDown(int row) {
  if ((0>row) || ((row-1)>=count()))
    return false;
  beginMoveRows(QModelIndex(), row, row, QModelIndex(), row+2);
  std::swap(_zones[row], _zones[row+1]);
  endMoveRows();
  emit modified();
  return true;
}

int
RoamingZoneList::rowCount(const QModelIndex &idx) const {
  Q_UNUSED(idx);
  return _zones.size();
}

QVariant
RoamingZoneList::data(const QModelIndex &index, int role) const {
  if ((Qt::DisplayRole!=role) || (index.row()>=_zones.size()) || (0 != index.column()))
    return QVariant();
  return _zones[index.row()]->name();
}

QVariant
RoamingZoneList::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((Qt::DisplayRole!=role) || (Qt::Horizontal!=orientation) || (0 != section))
    return QVariant();
  return tr("Roaming zone");
}

void
RoamingZoneList::onZoneDeleted(QObject *obj) {
  if (RoamingZone *zone = reinterpret_cast<RoamingZone *>(obj))
    remZone(zone);
}
