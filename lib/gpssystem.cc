#include "gpssystem.hh"
#include "contact.hh"
#include "channel.hh"
#include "logger.hh"

/* ********************************************************************************************* *
 * Implementation of PositioningSystem
 * ********************************************************************************************* */
PositioningSystem::PositioningSystem(const QString &name, uint period, QObject *parent)
  : ConfigObject("aprs", parent), _name(name), _period(period)
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
  if (_contact)
    connect(_contact, SIGNAL(destroyed(QObject*)), this, SLOT(onContactDeleted()));
  if (_revertChannel)
    connect(_revertChannel, SIGNAL(destroyed(QObject*)), this, SLOT(onRevertChannelDeleted()));
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
 * Implementation of APRSSystem
 * ********************************************************************************************* */
APRSSystem::APRSSystem(const QString &name, AnalogChannel *channel, const QString &dest, uint destSSID,
                       const QString &src, uint srcSSID, const QString &path, Icon icon, const QString &message,
                       uint period, QObject *parent)
  : PositioningSystem(name, period, parent), _channel(channel), _destination(dest), _destSSID(destSSID),
    _source(src), _srcSSID(srcSSID), _path(path), _icon(icon), _message(message)
{
  if (_channel)
    connect(_channel, SIGNAL(destroyed(QObject*)), this, SLOT(onChannelDeleted(QObject*)));
}

AnalogChannel *
APRSSystem::channel() const {
  return _channel;
}
void
APRSSystem::setChannel(AnalogChannel *channel) {
  if (_channel)
    disconnect(_channel, SIGNAL(destroyed(QObject*)), this, SLOT(onChannelDeleted(QObject*)));
  _channel = channel;
  if (_channel)
    connect(_channel, SIGNAL(destroyed(QObject*)), this, SLOT(onChannelDeleted(QObject*)));
}

const QString &
APRSSystem::destination() const {
  return _destination;
}
uint
APRSSystem::destSSID() const {
  return _destSSID;
}
void
APRSSystem::setDestination(const QString &call, uint ssid) {
  _destination = call;
  _destSSID = ssid;
}

const QString &
APRSSystem::source() const {
  return _source;
}
uint
APRSSystem::srcSSID() const {
  return _srcSSID;
}
void
APRSSystem::setSource(const QString &call, uint ssid) {
  _source = call;
  _srcSSID = ssid;
}

const QString &
APRSSystem::path() const {
  return _path;
}
void
APRSSystem::setPath(const QString &path) {
  _path = path.toUpper();
  _path.replace(" ","");
}

APRSSystem::Icon
APRSSystem::icon() const {
  return _icon;
}
void
APRSSystem::setIcon(Icon icon) {
  _icon = icon;
}

const QString &
APRSSystem::message() const {
  return _message;
}
void
APRSSystem::setMessage(const QString &msg) {
  _message = msg;
}

void
APRSSystem::onChannelDeleted(QObject *obj) {
  if (_channel == obj)
    _channel = nullptr;
}


/* ********************************************************************************************* *
 * Implementation of GPSSystems table
 * ********************************************************************************************* */
PositioningSystems::PositioningSystems(QObject *parent)
  : ConfigObjectList(parent)
{
  // pass...
}

PositioningSystem *
PositioningSystems::system(int idx) const {
  if (ConfigObject *obj = get(idx))
    return obj->as<PositioningSystem>();
  return nullptr;
}

int
PositioningSystems::add(ConfigObject *obj, int row) {
  if (obj && obj->is<PositioningSystem>())
    return ConfigObjectList::add(obj, row);
  return -1;
}

int
PositioningSystems::gpsCount() const {
  int c=0;
  for (int i=0; i<_items.size(); i++)
    if (_items.at(i)->is<GPSSystem>())
      c++;
  return c;
}

int
PositioningSystems::indexOfGPSSys(const GPSSystem *gps) const {
  if (! _items.contains((GPSSystem * const)gps))
    return -1;

  int idx=0;
  for (int i=0; i<count(); i++) {
    if (gps == _items.at(i))
      return idx;
    if (_items.at(i)->is<GPSSystem>())
      idx++;
  }

  return -1;
}

GPSSystem *
PositioningSystems::gpsSystem(int idx) const {
  if ((0>idx) || (idx >= _items.size()))
    return nullptr;
  for (int i=0; i<_items.size(); i++) {
    if (_items.at(i)->is<GPSSystem>()) {
      if (0==idx)
        return _items.at(i)->as<GPSSystem>();
      else
        idx--;
    }
  }
  return nullptr;
}


int
PositioningSystems::aprsCount() const {
  int c=0;
  for (int i=0; i<count(); i++) {
    if (_items.at(i)->is<APRSSystem>())
      c++;
  }
  return c;
}

int
PositioningSystems::indexOfAPRSSys(APRSSystem *aprs) const {
  if (! _items.contains(aprs))
    return -1;

  int idx=0;
  for (int i=0; i<count(); i++) {
    if (aprs == _items.at(i))
      return idx;
    if (_items.at(i)->is<APRSSystem>())
      idx++;
  }

  return -1;
}

APRSSystem *
PositioningSystems::aprsSystem(int idx) const {
  if ((0>idx) || (idx >= _items.size()))
    return nullptr;
  for (int i=0; i<_items.size(); i++) {
    if (_items.at(i)->is<APRSSystem>()) {
      if (0==idx)
        return _items.at(i)->as<APRSSystem>();
      else
        idx--;
    }
  }
  return nullptr;
}



