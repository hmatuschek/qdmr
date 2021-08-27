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
ZoneChannelList::ZoneChannelList(QObject *parent)
  : QAbstractListModel(parent), _channels()
{
  // pass...
}

int
ZoneChannelList::count() const {
  return _channels.size();
}

void
ZoneChannelList::clear() {
  beginResetModel();
  _channels.clear();
  endResetModel();
  emit modified();
}

Channel *
ZoneChannelList::channel(int idx) const {
  if ((0>idx) || (idx>=_channels.size()))
    return nullptr;
  return _channels[idx];
}

bool
ZoneChannelList::addChannel(Channel *channel) {
  if (_channels.contains(channel) || (nullptr == channel))
    return false;
  int idx = _channels.size();
  beginInsertRows(QModelIndex(), idx, idx);
  connect(channel, SIGNAL(destroyed(QObject*)), this, SLOT(onChannelDeleted(QObject*)));
  _channels.append(channel);
  endInsertRows();
  emit modified();
  return true;
}

bool
ZoneChannelList::remChannel(int idx) {
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
ZoneChannelList::remChannel(Channel *channel) {
  if (! _channels.contains(channel))
    return false;
  int idx = _channels.indexOf(channel);
  return remChannel(idx);
}

int
ZoneChannelList::rowCount(const QModelIndex &idx) const {
  Q_UNUSED(idx);
  return _channels.size();
}

QVariant
ZoneChannelList::data(const QModelIndex &index, int role) const {
  if ((Qt::DisplayRole!=role) || (index.row()>=_channels.size()) || (0 != index.column()))
    return QVariant();
  return _channels[index.row()]->name();
}

QVariant
ZoneChannelList::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((Qt::DisplayRole!=role) || (Qt::Horizontal!=orientation) || (0 != section))
    return QVariant();
  return tr("Channel");
}

void
ZoneChannelList::onChannelDeleted(QObject *obj) {
  if (Channel *channel = reinterpret_cast<Channel *>(obj)) {
    remChannel(channel);
    emit modified();
  }
}



/* ********************************************************************************************* *
 * Implementation of Zone
 * ********************************************************************************************* */
Zone::Zone(const QString &name, QObject *parent)
  : ConfigObject("zone", parent), _name(name),
    _A(new ZoneChannelList(this)), _B(new ZoneChannelList(this))
{
  connect(_A, SIGNAL(modified()), this, SIGNAL(modified()));
  connect(_B, SIGNAL(modified()), this, SIGNAL(modified()));
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

const ZoneChannelList *
Zone::A() const {
  return _A;
}
ZoneChannelList *
Zone::A() {
  return _A;
}

const ZoneChannelList *
Zone::B() const {
  return _B;
}
ZoneChannelList *
Zone::B() {
  return _B;
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
  beginResetModel();
  for (int i=0; i<count(); i++)
    _zones[i]->deleteLater();
  _zones.clear();
  endResetModel();
  emit modified();
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
ZoneList::moveUp(int first, int last) {
  if ((0>=first) || (last>=count()))
    return false;
  beginMoveRows(QModelIndex(), first, last, QModelIndex(), first-1);
  for (int row=first; row<=last; row++)
    std::swap(_zones[row], _zones[row-1]);
  endMoveRows();
  emit modified();
  return true;
}

bool
ZoneList::moveDown(int row) {
  if ((0>row) || ((row+1)>=count()))
    return false;
  beginMoveRows(QModelIndex(), row, row, QModelIndex(), row+2);
  std::swap(_zones[row], _zones[row+1]);
  endMoveRows();
  emit modified();
  return true;
}

bool
ZoneList::moveDown(int first, int last) {
  if ((0>first) || ((last+1)>=count()))
    return false;
  beginMoveRows(QModelIndex(), first, last, QModelIndex(), last+2);
  for (int row=last; row>=first; row--)
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


