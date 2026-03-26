#include "gpssystem.hh"
#include "contact.hh"
#include "channel.hh"
#include "logger.hh"
#include <QRegularExpressionMatch>


/* ********************************************************************************************* *
 * Implementation of PositionReportingSystem
 * ********************************************************************************************* */
PositionReportingSystem::PositionReportingSystem(QObject *parent)
  : ConfigObject(parent), _period(Interval::infinity())
{
  // pass...
}

PositionReportingSystem::PositionReportingSystem(const QString &name, const Interval &period, QObject *parent)
  : ConfigObject(name, parent), _period(period)
{
  // pass...
}

PositionReportingSystem::~PositionReportingSystem() {
  // pass...
}


bool
PositionReportingSystem::periodDisabled() const {
  return !_period.isFinite();
}

Interval
PositionReportingSystem::period() const {
  return _period;
}

void
PositionReportingSystem::setPeriod(const Interval &period) {
  if (_period == period)
    return;
  _period = period;
  emit modified(this);
}

void
PositionReportingSystem::disablePeriod() {
  _period = Interval::infinity();
}

bool
PositionReportingSystem::populate(YAML::Node &node, const ConfigItem::Context &context, const ErrorStack &err) {
  if (! ConfigObject::populate(node, context, err))
    return false;

  if (! periodDisabled())
    node["period"] = _period.format().toStdString();

  return true;
}

bool
PositionReportingSystem::parse(const YAML::Node &node, Context &ctx, const ErrorStack &err) {
  if (! node)
    return false;

  if ((! node.IsMap()) || (1 != node.size())) {
    errMsg(err) << node.Mark().line << ":" << node.Mark().column
                << ": Cannot parse positioning system: Expected object with one child.";
    return false;
  }

  YAML::Node pos = node.begin()->second;
  if (pos && (! pos["period"])) {
    logWarn() << pos.Mark().line << ":" << pos.Mark().column
              << ": Positioning system has no period.";
  } else if (pos && pos["period"].IsScalar()) {
    Interval period;
    if (! period.parse(QString::fromStdString(pos["period"].as<std::string>()), Interval::Format::Seconds))
      disablePeriod();
    else
      setPeriod(period);
  }

  return ConfigObject::parse(pos, ctx, err);
}

bool
PositionReportingSystem::link(const YAML::Node &node, const Context &ctx, const ErrorStack &err) {
  return ConfigObject::link(node.begin()->second, ctx, err);
}

void
PositionReportingSystem::onReferenceModified() {
  emit modified(this);
}


/* ********************************************************************************************* *
 * Implementation of DMRAPRSSystem
 * ********************************************************************************************* */
DMRAPRSSystem::DMRAPRSSystem(QObject *parent)
  : PositionReportingSystem(parent), _contact(), _revertChannel()
{
  // Allow revert channel to take a reference to the SelectedChannel singleton
  _revertChannel.allow(SelectedChannel::get()->metaObject());
  // Register '!selected' tag for revert channel
  Context::setTag(staticMetaObject.className(), "revert",
                  "!selected", QVariant::fromValue(SelectedChannel::get()));
  // By default, selected channel is revert channel
  resetRevertChannel();

  // Connect signals
  connect(&_contact, SIGNAL(modified()), this, SLOT(onReferenceModified()));
  connect(&_revertChannel, SIGNAL(modified()), this, SLOT(onReferenceModified()));
}

DMRAPRSSystem::DMRAPRSSystem(const QString &name, DMRContact *contact,
                     DMRChannel *revertChannel, const Interval &period,
                     QObject *parent)
  : PositionReportingSystem(name, period, parent), _contact(), _revertChannel()
{
  // Allow revert channel to take a reference to the SelectedChannel singleton
  _revertChannel.allow(SelectedChannel::get()->metaObject());
  // Register '!selected' tag for revert channel
  Context::setTag(staticMetaObject.className(), "revert",
                  "!selected", QVariant::fromValue(SelectedChannel::get()));

  // Set references.
  _contact.set(contact);
  setRevertChannel(revertChannel);

  // Connect signals
  connect(&_contact, SIGNAL(modified()), this, SLOT(onReferenceModified()));
  connect(&_revertChannel, SIGNAL(modified()), this, SLOT(onReferenceModified()));
}

ConfigItem *
DMRAPRSSystem::clone() const {
  DMRAPRSSystem *sys = new DMRAPRSSystem();
  if (! sys->copy(*this)) {
    sys->deleteLater();
    return nullptr;
  }
  return sys;
}

bool
DMRAPRSSystem::hasContact() const {
  return ! _contact.isNull();
}

DMRContact *
DMRAPRSSystem::contact() const {
  return _contact.as<DMRContact>();
}

void
DMRAPRSSystem::setContact(DMRContact *contact) {
  _contact.set(contact);
}


const DMRContactReference *
DMRAPRSSystem::contactRef() const {
  return &_contact;
}

DMRContactReference *
DMRAPRSSystem::contactRef() {
  return &_contact;
}


bool
DMRAPRSSystem::hasRevertChannel() const {
  return _revertChannel.is<DMRChannel>();
}

DMRChannel *
DMRAPRSSystem::revertChannel() const {
  return _revertChannel.as<DMRChannel>();
}

void
DMRAPRSSystem::setRevertChannel(DMRChannel *channel) {
  if (nullptr == channel)
    resetRevertChannel();
  else
    _revertChannel.set(channel);
}

void
DMRAPRSSystem::resetRevertChannel() {
  _revertChannel.set(SelectedChannel::get());
}


const DMRChannelReference*
DMRAPRSSystem::revertChannelRef() const {
  return &_revertChannel;
}

DMRChannelReference*
DMRAPRSSystem::revertChannelRef() {
  return &_revertChannel;
}

YAML::Node
DMRAPRSSystem::serialize(const Context &context, const ErrorStack &err) {
  YAML::Node node = PositionReportingSystem::serialize(context, err);
  if (node.IsNull())
    return node;
  YAML::Node type; type["dmr"] = node;
  return type;
}



/* ********************************************************************************************* *
 * Implementation of FMAPRSSystem
 * ********************************************************************************************* */
FMAPRSSystem::FMAPRSSystem(QObject *parent)
  : PositionReportingSystem(parent), _channel(), _destination(), _destSSID(0),
    _source(), _srcSSID(0), _path(), _icon(Icon::None), _message(),
    _anytone(nullptr)
{
  // Allow revert channel to take a reference to the SelectedChannel singleton
  _channel.allow(SelectedChannel::get()->metaObject());
  // Register '!selected' tag for revert channel
  Context::setTag(staticMetaObject.className(), "revert",
                  "!selected", QVariant::fromValue(SelectedChannel::get()));
  // By default, selected channel is revert channel
  resetRevertChannel();

  // Connect to channel reference
  connect(&_channel, SIGNAL(modified()), this, SLOT(onReferenceModified()));
}

FMAPRSSystem::FMAPRSSystem(const QString &name, FMChannel *channel, const QString &dest, unsigned destSSID,
                       const QString &src, unsigned srcSSID, const QString &path, Icon icon, const QString &message,
                       const Interval &period, QObject *parent)
  : PositionReportingSystem(name, period, parent), _channel(), _destination(dest), _destSSID(destSSID),
    _source(src), _srcSSID(srcSSID), _path(path), _icon(icon), _message(message),
    _anytone(nullptr)
{
  // Allow revert channel to take a reference to the SelectedChannel singleton
  _channel.allow(SelectedChannel::get()->metaObject());
  // Register '!selected' tag for revert channel
  Context::setTag(staticMetaObject.className(), "revert",
                  "!selected", QVariant::fromValue(SelectedChannel::get()));

  // Set revert channel
  setRevertChannel(channel);

  // Connect to channel reference
  connect(&_channel, SIGNAL(modified()), this, SLOT(onReferenceModified()));
}

bool
FMAPRSSystem::copy(const ConfigItem &other) {
  const FMAPRSSystem *sys = other.as<FMAPRSSystem>();
  if ((nullptr == sys) || (! PositionReportingSystem::copy(other)))
    return false;
  _destination = sys->destination();
  _destSSID = sys->_destSSID;
  _source = sys->_source;
  _srcSSID = sys->_srcSSID;
  _path = sys->_path;
  return true;
}

ConfigItem *
FMAPRSSystem::clone() const {
  FMAPRSSystem *sys = new FMAPRSSystem();
  if (! sys->copy(*this)) {
    sys->deleteLater();
    return nullptr;
  }
  return sys;
}


bool
FMAPRSSystem::hasRevertChannel() const {
  return _channel.is<FMChannel>();
}

FMChannel *
FMAPRSSystem::revertChannel() const {
  return _channel.as<FMChannel>();
}

void
FMAPRSSystem::setRevertChannel(FMChannel *channel) {
  if (nullptr == channel)
    resetRevertChannel();
  else
    _channel.set(channel);
}

void
FMAPRSSystem::resetRevertChannel() {
  _channel.set(SelectedChannel::get());
}


const FMChannelReference *
FMAPRSSystem::revertChannelRef() const {
  return &_channel;
}

FMChannelReference *
FMAPRSSystem::revertChannelRef() {
  return &_channel;
}


const QString &
FMAPRSSystem::destination() const {
  return _destination;
}

unsigned
FMAPRSSystem::destSSID() const {
  return _destSSID;
}

void
FMAPRSSystem::setDestination(const QString &call, unsigned ssid) {
  _destination = call;
  _destSSID = ssid;
}

void
FMAPRSSystem::setDestination(const QString &call) {
  _destination = call;
}

void
FMAPRSSystem::setDestSSID(unsigned int ssid) {
  _destSSID = ssid;
}


const QString &
FMAPRSSystem::source() const {
  return _source;
}

unsigned
FMAPRSSystem::srcSSID() const {
  return _srcSSID;
}

void
FMAPRSSystem::setSource(const QString &call, unsigned ssid) {
  _source = call;
  _srcSSID = ssid;
}

void
FMAPRSSystem::setSource(const QString &call) {
  _source = call;
}

void
FMAPRSSystem::setSrcSSID(unsigned ssid) {
  _srcSSID = ssid;
}


const QString &
FMAPRSSystem::path() const {
  return _path;
}
void
FMAPRSSystem::setPath(const QString &path) {
  _path = path.toUpper();
  _path.replace(" ","");
}

FMAPRSSystem::Icon
FMAPRSSystem::icon() const {
  return _icon;
}
void
FMAPRSSystem::setIcon(Icon icon) {
  _icon = icon;
}

const QString &
FMAPRSSystem::message() const {
  return _message;
}
void
FMAPRSSystem::setMessage(const QString &msg) {
  _message = msg;
  emit modified(this);
}

AnytoneFMAPRSSettingsExtension *
FMAPRSSystem::anytoneExtension() const {
  return _anytone;
}
void
FMAPRSSystem::setAnytoneExtension(AnytoneFMAPRSSettingsExtension *ext) {
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
FMAPRSSystem::serialize(const Context &context, const ErrorStack &err) {
  YAML::Node node = PositionReportingSystem::serialize(context, err);
  if (node.IsNull())
    return node;
  YAML::Node type; type["aprs"] = node;
  return type;
}

bool
FMAPRSSystem::populate(YAML::Node &node, const Context &context, const ErrorStack &err) {
  if (! PositionReportingSystem::populate(node, context, err))
    return false;

  node["destination"] = QString("%1-%2").arg(_destination).arg(_destSSID).toStdString();
  node["source"] = QString("%1-%2").arg(_source).arg(_srcSSID).toStdString();

  QStringList path;
  QRegularExpression pattern("([A-Za-z0-9]+-[0-9]+)");
  int idx = 0;
  auto match = pattern.match(_path, idx);
  while (match.hasMatch()) {
    path.append(match.captured(1));
    idx += match.capturedLength(1);
    match = pattern.match(_path, idx);
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
FMAPRSSystem::parse(const YAML::Node &node, Context &ctx, const ErrorStack &err) {
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
    QRegularExpression pattern("^([A-Z0-9]+)-(1?[0-9])$");
    auto match = pattern.match(source);
    if (! match.hasMatch()) {
      errMsg(err) << sys.Mark().line << ":" << sys.Mark().column
                  << ": Cannot parse APRS system: '" << source << "' not a valid source call and SSID.";
      return false;
    }
    setSource(match.captured(1), match.captured(2).toUInt());
  } else {
    errMsg(err) << sys.Mark().line << ":" << sys.Mark().column
                << ": Cannot parse APRS system: No source call+SSID specified.";
    return false;
  }

  if (sys["destination"] && sys["destination"].IsScalar()) {
    QString dest = QString::fromStdString(sys["destination"].as<std::string>());
    QRegularExpression pattern("^([A-Z0-9]+)-(1?[0-9])$");
    auto match = pattern.match(dest);
    if (! match.hasMatch()) {
      errMsg(err) << sys.Mark().line << ":" << sys.Mark().column
                  << ": Cannot parse APRS system: '" << dest << "' not a valid destination call and SSID.";
      return false;
    }
    setDestination(match.captured(1), match.captured(2).toUInt());
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
    setPath(path.join(","));
  }

  return PositionReportingSystem::parse(node, ctx, err);
}


/* ********************************************************************************************* *
 * Implementation of GPSSystems table
 * ********************************************************************************************* */
PositionReportingSystems::PositionReportingSystems(QObject *parent)
  : ConfigObjectList(PositionReportingSystem::staticMetaObject, parent)
{
  // pass...
}

PositionReportingSystem *
PositionReportingSystems::system(int idx) const {
  if (ConfigItem *obj = get(idx))
    return obj->as<PositionReportingSystem>();
  return nullptr;
}

int
PositionReportingSystems::add(ConfigObject *obj, int row, bool unique) {
  if (obj && obj->is<PositionReportingSystem>())
    return ConfigObjectList::add(obj, row, unique);
  return -1;
}

ConfigItem *
PositionReportingSystems::allocateChild(const YAML::Node &node, ConfigItem::Context &ctx, const ErrorStack &err) {
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
    return new DMRAPRSSystem();
  } else if ("aprs"==type) {
    return new FMAPRSSystem();
  }

  errMsg(err) << node.Mark().line << ":" << node.Mark().column
              << ": Cannot create positioning system: Unknown type '" << type << "'.";

  return nullptr;
}


