#include "gpssystem.hh"
#include "contact.hh"
#include "channel.hh"
#include "logger.hh"

/* ********************************************************************************************* *
 * Implementation of PositioningSystem
 * ********************************************************************************************* */
PositioningSystem::PositioningSystem(const QString &name, uint period, QObject *parent)
  : QObject(parent), _name(name), _period(period)
{
  // pass...
}

PositioningSystem::~PositioningSystem() {
  // pass...
}

const QString &
PositioningSystem::name() const {
  return _name;
}

void
PositioningSystem::setName(const QString &name) {
  _name = name;
  emit modified();
}

uint
PositioningSystem::period() const {
  return _period;
}

void
PositioningSystem::setPeriod(uint period) {
  _period = period;
}


/* ********************************************************************************************* *
 * Implementation of GPSSystem
 * ********************************************************************************************* */
GPSSystem::GPSSystem(const QString &name, DigitalContact *contact,
                     DigitalChannel *revertChannel, uint period,
                     QObject *parent)
  : PositioningSystem(name, period, parent), _contact(contact), _revertChannel(revertChannel)
{
  // pass...
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
PositioningSystems::PositioningSystems(QObject *parent)
  : QAbstractTableModel(parent), _posSystems()
{
  connect(this, SIGNAL(modified()), this, SLOT(onSystemEdited()));
}

int
PositioningSystems::count() const {
  return _posSystems.size();
}

int
PositioningSystems::gpsCount() const {
  int c=0;
  for (int i=0; i<_posSystems.size(); i++)
    if (_posSystems.at(i)->is<GPSSystem>())
      c++;
  return c;
}

int
PositioningSystems::aprsCount() const {
  int c=0;
  for (int i=0; i<_posSystems.size(); i++)
    if (_posSystems.at(i)->is<APRSSystem>())
      c++;
  return c;
}

void
PositioningSystems::clear() {
  for (int i=0; i<count(); i++)
    _posSystems[i]->deleteLater();
}

int
PositioningSystems::indexOfGPSSys(GPSSystem *gps) const {
  if (! _posSystems.contains(gps))
    return -1;

  int idx=0;
  for (int i=0; i<count(); i++) {
    if (gps == _posSystems.at(i))
      return idx;
    if (_posSystems.at(i)->is<GPSSystem>())
      idx++;
  }

  return -1;
}

PositioningSystem *
PositioningSystems::system(int idx) const {
  if ((0>idx) || (idx >= _posSystems.size()))
    return nullptr;
  return _posSystems.at(idx);
}

GPSSystem *
PositioningSystems::gpsSystem(int idx) const {
  if ((0>idx) || (idx >= _posSystems.size()))
    return nullptr;
  for (int i=0; i<_posSystems.size(); i++) {
    if (_posSystems.at(i)->is<GPSSystem>()) {
      if (0==idx)
        return _posSystems.at(i)->as<GPSSystem>();
      else
        idx--;
    }
  }
  return nullptr;
}

int
PositioningSystems::addSystem(PositioningSystem *sys, int row) {
  if (_posSystems.contains(sys))
    return -1;
  if (nullptr == sys)
    return -1;
  if ((row<0) || (row>_posSystems.size()))
    row = _posSystems.size();
  beginInsertRows(QModelIndex(), row, row);
  sys->setParent(this);
  connect(sys, SIGNAL(modified()), this, SIGNAL(modified()));
  connect(sys, SIGNAL(destroyed(QObject *)), this, SLOT(onSystemDeleted(QObject *)));
  _posSystems.insert(row, sys);
  endInsertRows();
  emit modified();
  return row;
}

bool
PositioningSystems::remSystem(int idx) {
  if ((0>idx) || (idx >= count()))
    return false;
  beginRemoveRows(QModelIndex(), idx, idx);
  PositioningSystem *sys = _posSystems.at(idx);
  _posSystems.remove(idx);
  sys->deleteLater();
  endRemoveRows();
  emit modified();
  return true;
}

bool
PositioningSystems::remSystem(PositioningSystem *sys) {
  if (! _posSystems.contains(sys))
    return false;
  int idx = _posSystems.indexOf(sys);
  return remSystem(idx);
}

bool
PositioningSystems::moveUp(int row) {
  if ((0>=row) || (row>=count()))
    return false;
  beginMoveRows(QModelIndex(), row, row, QModelIndex(), row-1);
  std::swap(_posSystems[row], _posSystems[row-1]);
  endMoveRows();
  emit modified();
  return true;
}

bool
PositioningSystems::moveDown(int row) {
  if ((0>row) || ((row-1)>=count()))
    return false;
  beginMoveRows(QModelIndex(), row, row, QModelIndex(), row+2);
  std::swap(_posSystems[row], _posSystems[row+1]);
  endMoveRows();
  emit modified();
  return true;
}

int
PositioningSystems::rowCount(const QModelIndex &idx) const {
  Q_UNUSED(idx);
  return count();
}
int
PositioningSystems::columnCount(const QModelIndex &idx) const {
  Q_UNUSED(idx);
  return 4;
}

QVariant
PositioningSystems::data(const QModelIndex &index, int role) const {
  if ((! index.isValid()) || (index.row()>=count()))
    return QVariant();
  if ((Qt::DisplayRole!=role) && (Qt::EditRole!=role))
    return QVariant();

  PositioningSystem *sys = _posSystems.at(index.row());

  switch (index.column()) {
  case 0:
    return sys->name();
  case 1:
    if (sys->is<GPSSystem>())
      return sys->as<GPSSystem>()->contact()->name();
  case 2:
    return sys->period();
  case 3:
    if (sys->is<GPSSystem>())
      return (sys->as<GPSSystem>()->hasRevertChannel() ?
                sys->as<GPSSystem>()->revertChannel()->name() : tr("[Selected]"));
  default:
    break;
  }

  return QVariant();
}

QVariant
PositioningSystems::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((Qt::DisplayRole!=role) || (Qt::Horizontal!=orientation))
    return QVariant();
  switch (section) {
  case 0: return tr("Name");
  case 1: return tr("Destination");
  case 2: return tr("Period [s]");
  case 3: return tr("Channel");
  default:
    break;
  }
  return QVariant();
}

void
PositioningSystems::onSystemDeleted(QObject *obj) {
  if (PositioningSystem *gps = reinterpret_cast<PositioningSystem *>(obj))
    remSystem(gps);
}

void
PositioningSystems::onSystemEdited() {
  if (0 == count())
    return;
  QModelIndex tl = index(0,0), br = index(count()-1, columnCount(QModelIndex()));
  emit dataChanged(tl, br);
}



