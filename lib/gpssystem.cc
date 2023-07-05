#include "gpssystem.hh"
#include "contact.hh"
#include "channel.hh"
#include "logger.hh"
#include "utils.hh"


/* ********************************************************************************************* *
 * Implementation of PositioningSystem
 * ********************************************************************************************* */
PositioningSystem::PositioningSystem(QObject *parent)
  : ConfigObject(parent), _period(0)
{
  // pass...
}

PositioningSystem::PositioningSystem(const QString &name, unsigned period, QObject *parent)
  : ConfigObject(name, parent), _period(period)
{
  // pass...
}

PositioningSystem::~PositioningSystem() {
  // pass...
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
PositioningSystem::populate(YAML::Node &node, const ConfigItem::Context &context, const ErrorStack &err) {
  if (! ConfigObject::populate(node, context, err))
    return false;
  return true;
}

bool
PositioningSystem::parse(const YAML::Node &node, Context &ctx, const ErrorStack &err) {
  if (! node)
    return false;

  if ((! node.IsMap()) || (1 != node.size())) {
    errMsg(err) << node.Mark().line << ":" << node.Mark().column
                << ": Cannot parse positioning system: Expected object with one child.";
    return false;
  }

  YAML::Node pos = node.begin()->second;
  if (pos && (!pos["period"])) {
    logWarn() << pos.Mark().line << ":" << pos.Mark().column
              << ": Positioning system has no period.";
  }

  return ConfigObject::parse(pos, ctx);
}

bool
PositioningSystem::link(const YAML::Node &node, const Context &ctx, const ErrorStack &err) {
  return ConfigObject::link(node.begin()->second, ctx, err);
}

void
PositioningSystem::onReferenceModified() {
  emit modified(this);
}


/* ********************************************************************************************* *
 * Implementation of GPSSystem
 * ********************************************************************************************* */
GPSSystem::GPSSystem(QObject *parent)
  : PositioningSystem(parent), _contact(), _revertChannel()
{
  // Register '!selected' tag for revert channel
  Context::setTag(staticMetaObject.className(), "revert", "!selected", SelectedChannel::get());

  // Allow revert channel to take a reference to the SelectedChannel singleton
  _revertChannel.allow(SelectedChannel::get()->metaObject());

  // Connect signals
  connect(&_contact, SIGNAL(modified()), this, SLOT(onReferenceModified()));
  connect(&_revertChannel, SIGNAL(modified()), this, SLOT(onReferenceModified()));
}

GPSSystem::GPSSystem(const QString &name, DMRContact *contact,
                     DMRChannel *revertChannel, unsigned period,
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

ConfigItem *
GPSSystem::clone() const {
  GPSSystem *sys = new GPSSystem();
  if (! sys->copy(*this)) {
    sys->deleteLater();
    return nullptr;
  }
  return sys;
}

bool
GPSSystem::hasContact() const {
  return ! _contact.isNull();
}

DMRContact *
GPSSystem::contactObj() const {
  return _contact.as<DMRContact>();
}

void
GPSSystem::setContactObj(DMRContact *contact) {
  _contact.set(contact);
}

void
GPSSystem::setContact(DMRContactReference *contact) {
  _contact.copy(contact);
}

const DMRContactReference *
GPSSystem::contact() const {
  return &_contact;
}

DMRContactReference *
GPSSystem::contact() {
  return &_contact;
}

bool
GPSSystem::hasRevertChannel() const {
  return ! _revertChannel.isNull();
}

DMRChannel *
GPSSystem::revertChannel() const {
  return _revertChannel.as<DMRChannel>();
}

void
GPSSystem::setRevertChannel(DMRChannel *channel) {
  _revertChannel.set(channel);
}

const DMRChannelReference*
GPSSystem::revert() const {
  return &_revertChannel;
}

DMRChannelReference*
GPSSystem::revert() {
  return &_revertChannel;
}

void
GPSSystem::setRevert(DMRChannelReference *channel) {
  _revertChannel.copy(channel);
}

YAML::Node
GPSSystem::serialize(const Context &context, const ErrorStack &err) {
  YAML::Node node = PositioningSystem::serialize(context, err);
  if (node.IsNull())
    return node;
  YAML::Node type; type["dmr"] = node;
  return type;
}


/* ********************************************************************************************* *
 * Implementation of APRSSystem
 * ********************************************************************************************* */
APRSSystem::APRSSystem(QObject *parent)
  : PositioningSystem(parent), _channel(), _destination(), _destSSID(0),
    _source(), _srcSSID(0), _path(), _icon(Icon::None), _message(), _anytone(nullptr)
{
  // Connect to channel reference
  connect(&_channel, SIGNAL(modified()), this, SLOT(onReferenceModified()));
}

APRSSystem::APRSSystem(const QString &name, FMChannel *channel, const QString &dest, unsigned destSSID,
                       const QString &src, unsigned srcSSID, const QString &path, Icon icon, const QString &message,
                       unsigned period, QObject *parent)
  : PositioningSystem(name, period, parent), _channel(), _destination(dest), _destSSID(destSSID),
    _source(src), _srcSSID(srcSSID), _path(path), _icon(icon), _message(message), _anytone(nullptr)
{
  // Set channel reference
  _channel.set(channel);
  // Connect to channel reference
  connect(&_channel, SIGNAL(modified()), this, SLOT(onReferenceModified()));
}

bool
APRSSystem::copy(const ConfigItem &other) {
  const APRSSystem *sys = other.as<APRSSystem>();
  if ((nullptr == sys) || (! PositioningSystem::copy(other)))
    return false;
  _destination = sys->destination();
  _destSSID = sys->_destSSID;
  _source = sys->_source;
  _srcSSID = sys->_srcSSID;
  _path = sys->_path;
  return true;
}

ConfigItem *
APRSSystem::clone() const {
  APRSSystem *sys = new APRSSystem();
  if (! sys->copy(*this)) {
    sys->deleteLater();
    return nullptr;
  }
  return sys;
}

FMChannel *
APRSSystem::revertChannel() const {
  return _channel.as<FMChannel>();
}

void
APRSSystem::setRevertChannel(FMChannel *channel) {
  _channel.set(channel);
}

const FMChannelReference *
APRSSystem::revert() const {
  return &_channel;
}

FMChannelReference *
APRSSystem::revert() {
  return &_channel;
}

void
APRSSystem::setRevert(FMChannelReference *ref) {
  _channel.copy(ref);
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

AnytoneFMAPRSSettingsExtension *
APRSSystem::anytoneExtension() const {
  return _anytone;
}
void
APRSSystem::setAnytoneExtension(AnytoneFMAPRSSettingsExtension *ext) {
  if (_anytone) {
    _anytone->deleteLater();
    _anytone = nullptr;
  }
  if (ext) {
    _anytone = ext;
    ext->setParent(this);
    connect(ext, SIGNAL(modified(ConfigItem *)), this, SIGNAL(modified(ConfigItem *)));
  }
}

YAML::Node
APRSSystem::serialize(const Context &context, const ErrorStack &err) {
  YAML::Node node = PositioningSystem::serialize(context, err);
  if (node.IsNull())
    return node;
  YAML::Node type; type["aprs"] = node;
  return type;
}

bool
APRSSystem::populate(YAML::Node &node, const Context &context, const ErrorStack &err) {
  if (! PositioningSystem::populate(node, context, err))
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


bool
APRSSystem::parse(const YAML::Node &node, Context &ctx, const ErrorStack &err) {
  if (! node)
    return false;

  if ((! node.IsMap()) || (1 != node.size())) {
    errMsg(err) << node.Mark().line << ":" << node.Mark().column
                << ": Cannot parse APRS system: Expected object with one child.";
    return false;
  }

  YAML::Node sys = node.begin()->second;
  if (sys["source"] && sys["source"].IsScalar()) {
    QString source = QString::fromStdString(sys["source"].as<std::string>());
    QRegExp pattern("^([A-Z0-9]+)-(1?[0-9])$");
    if (! pattern.exactMatch(source)) {
      errMsg(err) << sys.Mark().line << ":" << sys.Mark().column
                  << ": Cannot parse APRS system: '" << source << "' not a valid source call and SSID.";
      return false;
    }
    setSource(pattern.cap(1), pattern.cap(2).toUInt());
  } else {
    errMsg(err) << sys.Mark().line << ":" << sys.Mark().column
                << ": Cannot parse APRS system: No source call+SSID specified.";
    return false;
  }

  if (sys["destination"] && sys["destination"].IsScalar()) {
    QString dest = QString::fromStdString(sys["destination"].as<std::string>());
    QRegExp pattern("^([A-Z0-9]+)-(1?[0-9])$");
    if (! pattern.exactMatch(dest)) {
      errMsg(err) << sys.Mark().line << ":" << sys.Mark().column
                  << ": Cannot parse APRS system: '" << dest << "' not a valid destination call and SSID.";
      return false;
    }
    setDestination(pattern.cap(1), pattern.cap(2).toUInt());
  } else {
    errMsg(err) << sys.Mark().line << ":" << sys.Mark().column
                << ": Cannot parse APRS system: No destination call+SSID specified.";
    return false;
  }

  if (sys["path"] && sys["path"].IsSequence()) {
    QStringList path;
    for (YAML::const_iterator it=sys["path"].begin(); it!=sys["path"].end(); it++) {
      if (it->IsScalar())
        path.append(QString::fromStdString(it->as<std::string>()));
    }
    setPath(path.join(""));
  }

  return PositioningSystem::parse(node, ctx, err);
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
  if (ConfigItem *obj = get(idx))
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
  if (! _items.contains((GPSSystem *)gps))
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

ConfigItem *
PositioningSystems::allocateChild(const YAML::Node &node, ConfigItem::Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx)
  if (! node)
    return nullptr;

  if ((! node.IsMap()) || (1 != node.size())) {
    errMsg(err) << node.Mark().line << ":" << node.Mark().column
                << ": Cannot create positioning system: Expected object with one child.";
    return nullptr;
  }

  QString type = QString::fromStdString(node.begin()->first.as<std::string>());
  if ("dmr" == type) {
    return new GPSSystem();
  } else if ("aprs"==type) {
    return new APRSSystem();
  }

  errMsg(err) << node.Mark().line << ":" << node.Mark().column
              << ": Cannot create positioning system: Unknown type '" << type << "'.";

  return nullptr;
}


