#include "roaming.hh"
#include "channel.hh"
#include <QSet>
#include "config.hh"

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

RoamingChannel*
RoamingZone::channel(int idx) const {
  if ((idx < 0) || (idx >= count()))
    return nullptr;
  return _channel.get(idx)->as<RoamingChannel>();
}

int
RoamingZone::addChannel(RoamingChannel* ch, int row) {
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
RoamingZone::remChannel(RoamingChannel* ch) {
  return _channel.del(ch);
}

const RoamingChannelRefList *
RoamingZone::channels() const {
  return &_channel;
}

RoamingChannelRefList*
RoamingZone::channels() {
  return &_channel;
}

bool
RoamingZone::link(const YAML::Node &node, const Context &ctx, const ErrorStack &err) {
  // First, run default link
  if (! ConfigObject::link(node, ctx, err))
    return false;

  // Handle channel references separately

  if (! node["channels"])
    return true;

  // check type
  if (! node["channels"].IsSequence()) {
    errMsg(err) << node["channels"].Mark().line << ":" << node["channels"].Mark().column
                << ": Cannot link 'channels' of 'RoamingZone': Expected sequence.";
    return false;
  }
  YAML::Node lst = node["channels"];
  for (YAML::const_iterator it=lst.begin(); it!=lst.end(); it++) {
    if (! it->IsScalar()) {
      errMsg(err) << it->Mark().line << ":" << it->Mark().column
                  << ": Cannot link 'channels' of 'RoamingZone': Expected ID string.";
      return false;
    }
    QString id = QString::fromStdString(it->as<std::string>());
    if (! ctx.contains(id)) {
      errMsg(err) << it->Mark().line << ":" << it->Mark().column
                  << ": Cannot link 'channels' of 'RoamingZone': Reference '"
                  << id << "' not defined.";
      return false;
    }
    ConfigObject *obj = ctx.getObj(id);
    if (obj->is<DMRChannel>()) {
      RoamingChannel *rch = RoamingChannel::fromDMRChannel(obj->as<DMRChannel>());
      config()->roamingChannels()->add(rch);
      addChannel(rch);
    } else if (obj->is<RoamingChannel>()) {
      addChannel(obj->as<RoamingChannel>());
    } else {
      errMsg(err) << it->Mark().line << ":" << it->Mark().column
                  << ": Cannot link 'channels' of 'RoamingZone': "
                  << "Cannot add reference to '" << id << "' to list. "
                  << "Not a roaming channel.";
      return false;
    }
  }

  return true;
}

bool
RoamingZone::populate(YAML::Node &node, const Context &context, const ErrorStack &err) {
  if (! ConfigObject::populate(node, context, err))
    return false;

  // Serialize list of channel references.
  for (int i=0; i<count(); i++) {
    if (! context.contains(channel(i))) {
      errMsg(err) << "Cannot store reference to roaming channel '" << channel(i)->name()
                  << "': No ID assigned.";
      return false;
    }
    node["channels"].push_back(context.getId(channel(i)).toStdString());
  }

  return true;
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
