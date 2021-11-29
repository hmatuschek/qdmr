#include "roaming.hh"
#include "channel.hh"
#include <QSet>


/* ********************************************************************************************* *
 * Implementation of RoamingZone
 * ********************************************************************************************* */
RoamingZone::RoamingZone(QObject *parent)
  : ConfigObject("roam", parent), _channel()
{
  // pass...
}

RoamingZone::RoamingZone(const QString &name, QObject *parent)
  : ConfigObject(name, "roam", parent), _channel()
{
  // pass...
}

RoamingZone &
RoamingZone::operator =(const RoamingZone &other) {
  copy(other);
  return *this;
}

ConfigItem *
RoamingZone::clone() const {
  RoamingZone *z = new RoamingZone();
  if (! z->copy(*this)) {
    z->deleteLater();
    return nullptr;
  }
  return z;
}

int
RoamingZone::count() const {
  return _channel.count();
}

void
RoamingZone::clear() {
  _channel.clear();
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

ConfigItem *
RoamingZone::allocateChild(QMetaProperty &prop, const YAML::Node &node, const Context &ctx, const ErrorStack &err) {
  Q_UNUSED(prop); Q_UNUSED(node); Q_UNUSED(ctx); Q_UNUSED(err)
  return nullptr;
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
  if (ConfigItem *obj = get(idx))
    return obj->as<RoamingZone>();
  return nullptr;
}

int
RoamingZoneList::add(ConfigObject *obj, int row) {
  if (obj && obj->is<RoamingZone>())
    return ConfigObjectList::add(obj, row);
  return -1;
}

ConfigItem *
RoamingZoneList::allocateChild(const YAML::Node &node, ConfigItem::Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx)

  if (! node)
    return nullptr;

  if (! node.IsMap()) {
    errMsg(err) << node.Mark().line << ":" << node.Mark().column
                << ": Cannot create roaming zone: Expected object.";
    return nullptr;
  }

  return new RoamingZone();
}
