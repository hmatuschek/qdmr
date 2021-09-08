#include "roaming.hh"
#include <QSet>

/* ********************************************************************************************* *
 * Implementation of DigitalChannelRefList
 * ********************************************************************************************* */
DigitalChannelRefList::DigitalChannelRefList(QObject *parent)
  : ConfigObjectRefList(DigitalChannel::staticMetaObject, parent)
{
  // pass...
}


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
  return _channel.count();
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
  return _channel.get(idx)->as<DigitalChannel>();
}

int
RoamingZone::addChannel(DigitalChannel *ch, int row) {
  row = _channel.add(ch, row);
  if (0 > row)
    return row;
  emit modified(this);
  return row;
}

bool
RoamingZone::remChannel(int row) {
  return _channel.del(_channel.get(row));
}

bool
RoamingZone::remChannel(DigitalChannel *ch) {
  return _channel.del(ch);
}

const DigitalChannelRefList *
RoamingZone::channels() const {
  return &_channel;
}

DigitalChannelRefList *
RoamingZone::channels() {
  return &_channel;
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
  : ConfigObjectList(RoamingZone::staticMetaObject, parent)
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
