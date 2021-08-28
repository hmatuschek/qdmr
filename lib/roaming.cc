#include "roaming.hh"
#include <QSet>

/* ********************************************************************************************* *
 * Implementation of RoamingZone
 * ********************************************************************************************* */
RoamingZone::RoamingZone(const QString &name, QObject *parent)
  : ConfigObject("roam", parent), _name(name), _channel()
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
  emit modified(this);
}


DigitalChannel *
RoamingZone::channel(int idx) const {
  if ((idx < 0) || (idx >= count()))
    return nullptr;
  return _channel.at(idx);
}

bool
RoamingZone::addChannel(DigitalChannel *ch, int row) {
  if ((nullptr==ch) || _channel.contains(ch))
    return false;
  if (row <=0 || row>=count())
    row = count();
  connect(ch, SIGNAL(destroyed(QObject*)), this, SLOT(onChannelDeleted(QObject*)));
  connect(ch, SIGNAL(modified()), this, SIGNAL(modified()));
  _channel.insert(row, ch);
  emit modified(this);
  return false;
}

bool
RoamingZone::remChannel(int row) {
  if (row<0 || row>=count())
    return false;
  DigitalChannel *ch = _channel[row];
  _channel.removeAt(row);
  disconnect(ch, SIGNAL(destroyed(QObject*)), this, SLOT(onChannelDeleted(QObject*)));
  emit modified(this);
  return true;
}

bool
RoamingZone::remChannel(DigitalChannel *ch) {
  if (! _channel.contains(ch))
    return false;
  return remChannel(_channel.indexOf(ch));
}

void
RoamingZone::onChannelDeleted(QObject *obj) {
  if (DigitalChannel *ch = reinterpret_cast<DigitalChannel *>(obj))
    remChannel(ch);
}


/* ********************************************************************************************* *
 * Implementation of DefaultRoamingZone
 * ********************************************************************************************* */
DefaultRoamingZone *DefaultRoamingZone::_instance = nullptr;

DefaultRoamingZone::DefaultRoamingZone(QObject *parent)
  : RoamingZone(tr("[Default]"), parent)
{
  // pass...
}

DefaultRoamingZone *
DefaultRoamingZone::get() {
  if (nullptr == _instance)
    _instance = new DefaultRoamingZone();
  return _instance;
}


/* ********************************************************************************************* *
 * Implementation of RoamingZoneList
 * ********************************************************************************************* */
RoamingZoneList::RoamingZoneList(QObject *parent)
  : ConfigObjectList(parent)
{
  // pass...
}

QSet<DigitalChannel *>
RoamingZoneList::uniqueChannels() const {
  QSet<DigitalChannel *> channels;
  uniqueChannels(channels);
  return channels;
}

void
RoamingZoneList::uniqueChannels(QSet<DigitalChannel *> &channels) const {
  for (int i=0; i<count(); i++) {
    RoamingZone *zone = this->zone(i);
    for (int j=0; j<zone->count(); j++) {
      channels.insert(zone->channel(j));
    }
  }
}

RoamingZone *
RoamingZoneList::zone(int idx) const {
  if (ConfigObject *obj = get(idx))
    return obj->as<RoamingZone>();
  return nullptr;
}

int
RoamingZoneList::add(ConfigObject *obj, int row) {
  if (obj && obj->is<RoamingZone>())
    return ConfigObjectList::add(obj, row);
  return -1;
}
