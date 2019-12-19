#include "gpssystem.hh"
#include "contact.hh"
#include "channel.hh"


/* ********************************************************************************************* *
 * Implementation of GPSSystem
 * ********************************************************************************************* */
GPSSystem::GPSSystem(const QString &name, DigitalContact *contact,
                     DigitalChannel *revertChannel, uint period,
                     QObject *parent)
  : QObject(parent), _name(name), _contact(contact), _revertChannel(revertChannel), _period(period)
{
  // pass...
}

const QString &
GPSSystem::name() const {
  return _name;
}

void
GPSSystem::setName(const QString &name) {
  _name = name;
  emit modified();
}

uint
GPSSystem::period() const {
  return _period;
}

void
GPSSystem::setPeriod(uint period) {
  _period = period;
}

bool
GPSSystem::hasContact() const {
  return nullptr != _contact;
}

DigitalContact *
GPSSystem::contact() const {
  return _contact;
}

void
GPSSystem::setContact(DigitalContact *contact) {
  if (_contact)
    disconnect(_contact, SIGNAL(destroyed(QObject*)), this, SLOT(onContactDeleted()));
  _contact = contact;
  connect(_contact, SIGNAL(destroyed(QObject*)), this, SLOT(onContactDeleted()));
}

bool
GPSSystem::hasRevertChannel() const {
  return nullptr != _revertChannel;
}

DigitalChannel *
GPSSystem::revertChannel() const {
  return _revertChannel;
}

void
GPSSystem::setRevertChannel(DigitalChannel *channel) {
  if (_revertChannel)
    disconnect(_revertChannel, SIGNAL(destroyed(QObject*)), this, SLOT(onRevertChannelDeleted()));
  _revertChannel = channel;
  if (_revertChannel)
    connect(_revertChannel, SIGNAL(destroyed(QObject*)), this, SLOT(onRevertChannelDeleted()));
}

void
GPSSystem::onContactDeleted() {
  _contact = nullptr;
}

void
GPSSystem::onRevertChannelDeleted() {
  _revertChannel = nullptr;
}


/* ********************************************************************************************* *
 * Implementation of GPSSystems table
 * ********************************************************************************************* */
GPSSystems::GPSSystems(QObject *parent)
  : QAbstractTableModel(parent), _gpsSystems()
{
  connect(this, SIGNAL(modified()), this, SLOT(onGPSSystemEdited()));
}

int
GPSSystems::count() const {
  return _gpsSystems.size();
}

void
GPSSystems::clear() {
  for (int i=0; i<count(); i++)
    _gpsSystems[i]->deleteLater();
  _gpsSystems.clear();
}

int
GPSSystems::indexOf(GPSSystem *gps) const {
if (! _gpsSystems.contains(gps))
  return -1;
return _gpsSystems.indexOf(gps);
}

GPSSystem *
GPSSystems::gpsSystem(int idx) const {
  if ((0>idx) || (idx >= _gpsSystems.size()))
    return nullptr;
  return _gpsSystems.at(idx);
}

int
GPSSystems::addGPSSystem(GPSSystem *gps, int row) {
  if (_gpsSystems.contains(gps))
    return -1;
  if ((row<0) || (row>_gpsSystems.size()))
    row = _gpsSystems.size();
  beginInsertRows(QModelIndex(), row, row);
  connect(gps, SIGNAL(modified()), this, SIGNAL(modified()));
  connect(gps, SIGNAL(destroyed(QObject *)), this, SLOT(onGPSSystemDeleted(QObject *)));
  _gpsSystems.insert(row, gps);
  endInsertRows();
  emit modified();
  return row;
}

bool
GPSSystems::remGPSSystem(int idx) {
  if ((0>idx) || (idx >= _gpsSystems.size()))
    return false;
  beginRemoveRows(QModelIndex(), idx, idx);
  GPSSystem *gps = _gpsSystems.at(idx);
  _gpsSystems.remove(idx);
  gps->deleteLater();
  endRemoveRows();
  emit modified();
  return true;
}

bool
GPSSystems::remGPSSystem(GPSSystem *gps) {
  if (! _gpsSystems.contains(gps))
    return false;
  int idx = _gpsSystems.indexOf(gps);
  return remGPSSystem(idx);
}

bool
GPSSystems::moveUp(int row) {
  if ((0>=row) || (row>=count()))
    return false;
  beginMoveRows(QModelIndex(), row, row, QModelIndex(), row-1);
  std::swap(_gpsSystems[row], _gpsSystems[row-1]);
  endMoveRows();
  emit modified();
  return true;
}

bool
GPSSystems::moveDown(int row) {
  if ((0>row) || ((row-1)>=count()))
    return false;
  beginMoveRows(QModelIndex(), row, row, QModelIndex(), row+2);
  std::swap(_gpsSystems[row], _gpsSystems[row+1]);
  endMoveRows();
  emit modified();
  return true;
}

int
GPSSystems::rowCount(const QModelIndex &idx) const {
  Q_UNUSED(idx);
  return _gpsSystems.size();
}
int
GPSSystems::columnCount(const QModelIndex &idx) const {
  Q_UNUSED(idx);
  return 4;
}

QVariant
GPSSystems::data(const QModelIndex &index, int role) const {
  if ((! index.isValid()) || (index.row()>=_gpsSystems.size()))
    return QVariant();
  if ((Qt::DisplayRole!=role) && (Qt::EditRole!=role))
    return QVariant();

  GPSSystem *gps = _gpsSystems[index.row()];

  switch (index.column()) {
  case 0:
    return gps->name();
  case 1:
    return gps->contact()->name();
  case 2:
    return gps->period();
  case 3:
    return (gps->hasRevertChannel() ? gps->revertChannel()->name() : "-");
  default:
    break;
  }

  return QVariant();
}

QVariant
GPSSystems::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((Qt::DisplayRole!=role) || (Qt::Horizontal!=orientation))
    return QVariant();
  switch (section) {
  case 0: return tr("Name");
  case 1: return tr("Contact");
  case 2: return tr("Period [ms]");
  case 3: return tr("Revert Channel");
  default:
    break;
  }
  return QVariant();
}

void
GPSSystems::onGPSSystemDeleted(QObject *obj) {
  if (GPSSystem *gps = reinterpret_cast<GPSSystem *>(obj))
    remGPSSystem(gps);
}

void
GPSSystems::onGPSSystemEdited() {
  if (0 == count())
    return;
  QModelIndex tl = index(0,0), br = index(count()-1, columnCount(QModelIndex()));
  emit dataChanged(tl, br);
}



