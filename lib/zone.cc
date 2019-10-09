#include "zone.hh"
#include "channel.hh"
#include "config.hh"
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QInputDialog>


/* ********************************************************************************************* *
 * Implementation of Zone
 * ********************************************************************************************* */
Zone::Zone(const QString &name, QObject *parent)
  : QAbstractListModel(parent), _name(name), _channels()
{
  // pass...
}

int
Zone::count() const {
  return _channels.size();
}

void
Zone::clear() {
  beginResetModel();
  _channels.clear();
  endResetModel();
  emit modified();
}

const QString &
Zone::name() const {
  return _name;
}
bool
Zone::setName(const QString &name) {
  if (name.simplified().isEmpty())
    return false;
  _name = name;
  emit modified();
  return true;
}

Channel *
Zone::channel(int idx) const {
  if ((0>idx) || (idx>=_channels.size()))
    return nullptr;
  return _channels[idx];
}

bool
Zone::addChannel(Channel *channel) {
  if (_channels.contains(channel))
    return false;
  int idx = _channels.size();
  beginInsertRows(QModelIndex(), idx, idx);
  channel->setParent(this);
  connect(channel, SIGNAL(destroyed(QObject*)), this, SLOT(onChannelDeleted(QObject*)));
  _channels.append(channel);
  endInsertRows();
  emit modified();
  return true;
}

bool
Zone::remChannel(int idx) {
  if ((0>idx) || (idx>=_channels.size()))
    return false;
  Channel *channel = _channels[idx];
  beginRemoveRows(QModelIndex(), idx, idx);
  _channels.remove(idx);
  channel->deleteLater();
  endRemoveRows();
  emit modified();
  return true;
}

bool
Zone::remChannel(Channel *channel) {
  if (! _channels.contains(channel))
    return false;
  int idx = _channels.indexOf(channel);
  return remChannel(idx);
}

int
Zone::rowCount(const QModelIndex &idx) const {
  Q_UNUSED(idx);
  return _channels.size();
}

QVariant
Zone::data(const QModelIndex &index, int role) const {
  if ((Qt::DisplayRole!=role) || (index.row()>=_channels.size()) || (0 != index.column()))
    return QVariant();
  return _channels[index.row()]->name();
}

QVariant
Zone::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((Qt::DisplayRole!=role) || (Qt::Horizontal!=orientation) || (0 != section))
    return QVariant();
  return tr("Channel");
}

void
Zone::onChannelDeleted(QObject *obj) {
  if (Channel *channel = reinterpret_cast<Channel *>(obj))
    remChannel(channel);
}


/* ********************************************************************************************* *
 * Implementation of ZoneList
 * ********************************************************************************************* */
ZoneList::ZoneList(QObject *parent)
  : QAbstractListModel(parent), _zones()
{
  // pass...
}

int
ZoneList::count() const {
  return _zones.size();
}

void
ZoneList::clear() {
  for (int i=0; i<count(); i++)
    _zones[i]->deleteLater();
}

Zone *
ZoneList::zone(int idx) const {
  if ((0>idx) || (idx>=_zones.size()))
    return nullptr;
  return _zones[idx];
}

bool
ZoneList::addZone(Zone *zone, int row) {
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
ZoneList::remZone(int idx) {
  if ((0>idx) || (idx>=_zones.size()))
    return false;
  Zone *zone = _zones[idx];
  beginRemoveRows(QModelIndex(), idx, idx);
  _zones.remove(idx);
  zone->deleteLater();
  endRemoveRows();
  emit modified();
  return true;
}

bool
ZoneList::remZone(Zone *zone) {
  if (! _zones.contains(zone))
    return false;
  int idx = _zones.indexOf(zone);
  return remZone(idx);
}

bool
ZoneList::moveUp(int row) {
  if ((0>=row) || (row>=count()))
    return false;
  beginMoveRows(QModelIndex(), row, row, QModelIndex(), row-1);
  std::swap(_zones[row], _zones[row-1]);
  endMoveRows();
  emit modified();
  return true;
}

bool
ZoneList::moveDown(int row) {
  if ((0>row) || ((row-1)>=count()))
    return false;
  beginMoveRows(QModelIndex(), row, row, QModelIndex(), row+2);
  std::swap(_zones[row], _zones[row+1]);
  endMoveRows();
  emit modified();
  return true;
}

int
ZoneList::rowCount(const QModelIndex &idx) const {
  Q_UNUSED(idx);
  return _zones.size();
}

QVariant
ZoneList::data(const QModelIndex &index, int role) const {
  if ((Qt::DisplayRole!=role) || (index.row()>=_zones.size()) || (0 != index.column()))
    return QVariant();
  return _zones[index.row()]->name();
}

QVariant
ZoneList::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((Qt::DisplayRole!=role) || (Qt::Horizontal!=orientation) || (0 != section))
    return QVariant();
  return tr("Zone");
}

void
ZoneList::onZoneDeleted(QObject *obj) {
  if (Zone *zone = reinterpret_cast<Zone *>(obj))
    remZone(zone);
}


