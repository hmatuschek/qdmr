#include "scanlist.hh"
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
 * Implementation of ScanList
 * ********************************************************************************************* */
ScanList::ScanList(const QString &name, QObject *parent)
  : QAbstractListModel(parent), _name(name), _channels(), _priorityChannel(nullptr),
    _secPriorityChannel(nullptr)
{
  // pass...
}

int
ScanList::count() const {
  return _channels.size();
}

void
ScanList::clear() {
  beginResetModel();
  _channels.clear();
  endResetModel();
  emit modified();
}

const QString &
ScanList::name() const {
  return _name;
}
bool
ScanList::setName(const QString &name) {
  if (name.simplified().isEmpty())
    return false;
  _name = name.simplified();
  emit modified();
  return true;
}

bool
ScanList::contains(Channel *channel) const {
  return _channels.contains(channel);
}

Channel *
ScanList::channel(int idx) const {
  if ((0>idx) || (idx>=_channels.size()))
    return nullptr;
  return _channels[idx];
}

bool
ScanList::addChannel(Channel *channel) {
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
ScanList::remChannel(int idx) {
  if ((0>idx) || (idx>=_channels.size()))
    return false;
  Channel *channel = _channels[idx];
  beginRemoveRows(QModelIndex(), idx, idx);
  _channels.remove(idx);
  disconnect(channel, SIGNAL(destroyed(QObject*)), this, SLOT(onChannelDeleted(QObject*)));
  endRemoveRows();
  emit modified();
  return true;
}

Channel *
ScanList::priorityChannel() const {
  return _priorityChannel;
}

void
ScanList::setPriorityChannel(Channel *channel) {
  _priorityChannel = channel;
  if (_priorityChannel)
    connect(_priorityChannel, SIGNAL(destroyed(QObject *)), this, SLOT(onChannelDeleted(QObject *)));
  emit modified();
}

Channel *
ScanList::secPriorityChannel() const {
  return _secPriorityChannel;
}

void
ScanList::setSecPriorityChannel(Channel *channel) {
  _secPriorityChannel = channel;
  if (_secPriorityChannel)
    connect(_secPriorityChannel, SIGNAL(destroyed(QObject *)), this, SLOT(onChannelDeleted(QObject *)));
  emit modified();
}

bool
ScanList::remChannel(Channel *channel) {
  if (! _channels.contains(channel))
    return false;
  int idx = _channels.indexOf(channel);
  return remChannel(idx);
}

int
ScanList::rowCount(const QModelIndex &idx) const {
  Q_UNUSED(idx);
  return _channels.size();
}

QVariant
ScanList::data(const QModelIndex &index, int role) const {
  if ((Qt::DisplayRole!=role) || (index.row()>=_channels.size()) || (0 != index.column()))
    return QVariant();
  return _channels[index.row()]->name();
}

QVariant
ScanList::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((Qt::DisplayRole!=role) || (Qt::Horizontal!=orientation) || (0 != section))
    return QVariant();
  return tr("Channel");
}

void
ScanList::onChannelDeleted(QObject *obj) {
  if (Channel *channel = dynamic_cast<Channel *>(obj)) {
    remChannel(channel);
    if (_priorityChannel == channel)
      _priorityChannel = nullptr;
    if (_secPriorityChannel == channel)
      _secPriorityChannel = nullptr;
  }
}


/* ********************************************************************************************* *
 * Implementation of ScanLists
 * ********************************************************************************************* */
ScanLists::ScanLists(QObject *parent)
  : QAbstractListModel(parent), _scanlists()
{
  // pass...
}

int
ScanLists::count() const {
  return _scanlists.size();
}

int
ScanLists::indexOf(ScanList *list) const {
  if (! _scanlists.contains(list))
    return -1;
  return _scanlists.indexOf(list);
}

void
ScanLists::clear() {
  for (int i=0; i<count(); i++)
    _scanlists[i]->deleteLater();
  _scanlists.clear();
}

ScanList *
ScanLists::scanlist(int idx) const {
  if ((0>idx) || (idx>=_scanlists.size()))
    return nullptr;
  return _scanlists[idx];
}

bool
ScanLists::addScanList(ScanList *list, int row) {
  if (_scanlists.contains(list))
    return false;
  if ((row<=0) || (row>_scanlists.size()))
    row = _scanlists.size();
  beginInsertRows(QModelIndex(), row, row);
  list->setParent(this);
  connect(list, SIGNAL(destroyed(QObject*)), this, SLOT(onScanListDeleted(QObject*)));
  connect(list, SIGNAL(modified()), this, SIGNAL(modified()));
  _scanlists.insert(row, list);
  endInsertRows();
  emit modified();
  return true;
}

bool
ScanLists::remScanList(int idx) {
  if ((0>idx) || (idx>=_scanlists.size()))
    return false;
  ScanList *scanlist = _scanlists[idx];
  beginRemoveRows(QModelIndex(), idx, idx);
  _scanlists.remove(idx);
  scanlist->deleteLater();
  endRemoveRows();
  emit modified();
  return true;
}

bool
ScanLists::remScanList(ScanList *scanlist) {
  if (! _scanlists.contains(scanlist))
    return false;
  int idx = _scanlists.indexOf(scanlist);
  return remScanList(idx);
}

bool
ScanLists::moveUp(int row) {
  if ((0>=row) || (row>=count()))
    return false;
  beginMoveRows(QModelIndex(), row, row, QModelIndex(), row-1);
  std::swap(_scanlists[row], _scanlists[row-1]);
  endMoveRows();
  emit modified();
  return true;
}

bool
ScanLists::moveDown(int row) {
  if ((0>row) || ((row-1)>=count()))
    return false;
  beginMoveRows(QModelIndex(), row, row, QModelIndex(), row+2);
  std::swap(_scanlists[row], _scanlists[row+1]);
  endMoveRows();
  emit modified();
  return true;
}


int
ScanLists::rowCount(const QModelIndex &idx) const {
  Q_UNUSED(idx);
  return _scanlists.size();
}

QVariant
ScanLists::data(const QModelIndex &index, int role) const {
  if ((Qt::DisplayRole!=role) || (index.row()>=_scanlists.size()) || (0 != index.column()))
    return QVariant();
  return _scanlists[index.row()]->name();
}

QVariant
ScanLists::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((Qt::DisplayRole!=role) || (Qt::Horizontal!=orientation) || (0 != section))
    return QVariant();
  return tr("Scan-List");
}

void
ScanLists::onScanListDeleted(QObject *obj) {
  if (ScanList *scanlist = reinterpret_cast<ScanList *>(obj))
    remScanList(scanlist);
}
