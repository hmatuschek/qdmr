#include "gpssystem.hh"
#include "contact.hh"
#include "channel.hh"
#include "logger.hh"
#include "utils.hh"


/* ********************************************************************************************* *
 * Implementation of PositioningSystem
 * ********************************************************************************************* */
PositioningSystem::PositioningSystem(const QString &name, unsigned period, QObject *parent)
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
  emit modified(this);
}

unsigned
PositioningSystem::period() const {
  return _period;
}

void
PositioningSystem::setPeriod(unsigned period) {
  _period = period;
  emit modified(this);
}

bool
PositioningSystem::populate(YAML::Node &node, const ConfigObject::Context &context) {
  if (! ConfigObject::populate(node, context))
    return false;
  return true;
}

void
PositioningSystem::onReferenceModified() {
  emit modified(this);
}


/* ********************************************************************************************* *
 * Implementation of GPSSystem
 * ********************************************************************************************* */
GPSSystem::GPSSystem(const QString &name, DigitalContact *contact,
                     DigitalChannel *revertChannel, unsigned period,
                     QObject *parent)
  : PositioningSystem(name, period, parent), _contact(), _revertChannel()
{
  // Register '!selected' tag for revert channel
  Context::setTag(staticMetaObject.className(), "revert", "!selected", SelectedChannel::get());

  // Set references.
  _contact.set(contact);
  _revertChannel.set(revertChannel);

  // Allow revert channel to take a reference to the SelectedChannel singleton
  _revertChannel.allow(SelectedChannel::get()->metaObject());

  // Connect signals
  connect(&_contact, SIGNAL(modified()), this, SLOT(onReferenceModified()));
  connect(&_revertChannel, SIGNAL(modified()), this, SLOT(onReferenceModified()));
}

YAML::Node
GPSSystem::serialize(const Context &context) {
  YAML::Node node = PositioningSystem::serialize(context);
  if (node.IsNull())
    return node;
  YAML::Node type; type["dmr"] = node;
  return type;
}

bool
GPSSystem::hasContact() const {
  return ! _contact.isNull();
}

DigitalContact *
GPSSystem::contactObj() const {
  return _contact.as<DigitalContact>();
}

void
GPSSystem::setContact(DigitalContact *contact) {
  _contact.set(contact);
}

const DigitalContactReference *
GPSSystem::contact() const {
  return &_contact;
}

DigitalContactReference *
GPSSystem::contact() {
  return &_contact;
}

bool
GPSSystem::hasRevertChannel() const {
  return ! _revertChannel.isNull();
}

DigitalChannel *
GPSSystem::revertChannel() const {
  return _revertChannel.as<DigitalChannel>();
}

void
GPSSystem::setRevertChannel(DigitalChannel *channel) {
  _revertChannel.set(channel);
}

const DigitalChannelReference*
GPSSystem::revert() const {
  return &_revertChannel;
}

DigitalChannelReference*
GPSSystem::revert() {
  return &_revertChannel;
}


/* ********************************************************************************************* *
 * Implementation of APRSSystem
 * ********************************************************************************************* */
APRSSystem::APRSSystem(const QString &name, AnalogChannel *channel, const QString &dest, unsigned destSSID,
                       const QString &src, unsigned srcSSID, const QString &path, Icon icon, const QString &message,
                       unsigned period, QObject *parent)
  : PositioningSystem(name, period, parent), _channel(), _destination(dest), _destSSID(destSSID),
    _source(src), _srcSSID(srcSSID), _path(path), _icon(icon), _message(message)
{
  // Set channel reference
  _channel.set(channel);
  // Connect to channel reference
  connect(&_channel, SIGNAL(modified()), this, SLOT(onReferenceModified()));
}

YAML::Node
APRSSystem::serialize(const Context &context) {
  YAML::Node node = PositioningSystem::serialize(context);
  if (node.IsNull())
    return node;
  YAML::Node type; type["aprs"] = node;
  return type;
}

AnalogChannel *
APRSSystem::revertChannel() const {
  return _channel.as<AnalogChannel>();
}

void
APRSSystem::setRevertChannel(AnalogChannel *channel) {
  _channel.set(channel);
}

const AnalogChannelReference *
APRSSystem::revert() const {
  return &_channel;
}

AnalogChannelReference *
APRSSystem::revert() {
  return &_channel;
}

const QString &
APRSSystem::destination() const {
  return _destination;
}
unsigned
APRSSystem::destSSID() const {
  return _destSSID;
}
void
APRSSystem::setDestination(const QString &call, unsigned ssid) {
  _destination = call;
  _destSSID = ssid;
}

const QString &
APRSSystem::source() const {
  return _source;
}
unsigned
APRSSystem::srcSSID() const {
  return _srcSSID;
}
void
APRSSystem::setSource(const QString &call, unsigned ssid) {
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
  emit modified(this);
}

bool
APRSSystem::populate(YAML::Node &node, const Context &context) {
  if (! PositioningSystem::populate(node, context))
    return false;

  node["destination"] = QString("%1-%2").arg(_destination).arg(_destSSID).toStdString();
  node["source"] = QString("%1-%2").arg(_source).arg(_srcSSID).toStdString();

  QStringList path;
  QRegExp pattern("([A-Za-z0-9]+-[0-9]+)");
  int idx = 0;
  while (0 <= (idx = pattern.indexIn(_path, idx))) {
    path.append(pattern.cap(1));
    idx += pattern.matchedLength();
  }

  if (path.count()) {
    YAML::Node list = YAML::Node(YAML::NodeType::Sequence);
    list.SetStyle(YAML::EmitterStyle::Flow);
    foreach (QString call, path) {
      list.push_back(call.toStdString());
    }
    node["path"] = list;
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of GPSSystems table
 * ********************************************************************************************* */
PositioningSystems::PositioningSystems(QObject *parent)
  : ConfigObjectList(PositioningSystem::staticMetaObject, parent)
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
  if (! _items.contains(gps))
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



