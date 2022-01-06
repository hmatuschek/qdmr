#include "channel.hh"
#include "contact.hh"
#include "ctcssbox.hh"
#include "rxgrouplist.hh"
#include "config.hh"
#include "scanlist.hh"
#include "logger.hh"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QDoubleValidator>
#include <QIntValidator>
#include <cmath>
#include "application.hh"
#include <QCompleter>
#include <QAbstractProxyModel>
#include <QMetaEnum>

#include "opengd77_extension.hh"


/* ********************************************************************************************* *
 * Implementation of Channel
 * ********************************************************************************************* */
Channel::Channel(QObject *parent)
  : ConfigObject("ch", parent), _rxFreq(0), _txFreq(0), _defaultPower(true),
    _power(Power::Low), _txTimeOut(std::numeric_limits<unsigned>::max()), _rxOnly(false),
    _vox(std::numeric_limits<unsigned>::max()), _scanlist(), _openGD77ChannelExtension(nullptr),
    _tytChannelExtension(nullptr)
{
  // Link scan list modification event (e.g., scan list gets deleted).
  connect(&_scanlist, SIGNAL(modified()), this, SLOT(onReferenceModified()));
}

Channel::Channel(const Channel &other, QObject *parent)
  : ConfigObject("ch", parent), _scanlist(), _openGD77ChannelExtension(nullptr),
    _tytChannelExtension(nullptr)
{
  copy(other);

  // Link scan list modification event (e.g., scan list gets deleted).
  connect(&_scanlist, SIGNAL(modified()), this, SLOT(onReferenceModified()));
}

bool
Channel::copy(const ConfigItem &other) {
  const Channel *c = other.as<Channel>();
  if ((nullptr == c) || (! ConfigObject::copy(other)))
    return false;

  if (c->defaultPower())
    setDefaultPower();
  if (c->defaultTimeout())
    setDefaultTimeout();
  if (c->defaultVOX())
    setVOXDefault();

  return true;
}

void
Channel::clear() {
  ConfigObject::clear();
  _rxFreq = 0; _txFreq = 0;
  setDefaultPower();
  setDefaultTimeout();
  _rxOnly = false;
  setVOXDefault();
  _scanlist.clear();
  if (_openGD77ChannelExtension)
    _openGD77ChannelExtension->deleteLater();
  _openGD77ChannelExtension = nullptr;
  if (_tytChannelExtension)
    _tytChannelExtension->deleteLater();
  _tytChannelExtension = nullptr;
}

double
Channel::rxFrequency() const {
  return _rxFreq;
}
bool
Channel::setRXFrequency(double freq) {
  _rxFreq = freq;
  emit modified(this);
  return true;
}

double
Channel::txFrequency() const {
  return _txFreq;
}
bool
Channel::setTXFrequency(double freq) {
  _txFreq = freq;
  emit modified(this);
  return true;
}

bool
Channel::defaultPower() const {
  return _defaultPower;
}
Channel::Power
Channel::power() const {
  return _power;
}
void
Channel::setPower(Power power) {
  _power = power;
  _defaultPower = false;
  emit modified(this);
}
void
Channel::setDefaultPower() {
  _defaultPower = true;
}

bool
Channel::defaultTimeout() const {
  return std::numeric_limits<unsigned>::max() == timeout();
}
bool
Channel::timeoutDisabled() const {
  return 0 == timeout();
}
unsigned
Channel::timeout() const {
  return _txTimeOut;
}
bool
Channel::setTimeout(unsigned dur) {
  _txTimeOut = dur;
  emit modified(this);
  return true;
}
void
Channel::setDefaultTimeout() {
  setTimeout(std::numeric_limits<unsigned>::max());
}
void
Channel::disableTimeout() {
  setTimeout(0);
}

bool
Channel::rxOnly() const {
  return _rxOnly;
}
bool
Channel::setRXOnly(bool enable) {
  _rxOnly = enable;
  emit modified(this);
  return true;
}

bool
Channel::voxDisabled() const {
  return 0==vox();
}
bool
Channel::defaultVOX() const {
  return std::numeric_limits<unsigned>::max() == vox();
}
unsigned
Channel::vox() const {
  return _vox;
}
void
Channel::setVOX(unsigned level) {
  _vox = level;
  emit modified(this);
}
void
Channel::setVOXDefault() {
  setVOX(std::numeric_limits<unsigned>::max());
}
void
Channel::disableVOX() {
  setVOX(0);
}

const ScanListReference *
Channel::scanList() const {
  return &_scanlist;
}

ScanListReference *
Channel::scanList() {
  return &_scanlist;
}

void
Channel::setScanList(ScanListReference *ref) {
  if (nullptr == ref)
    _scanlist.clear();
  else
    _scanlist.copy(ref);
}

ScanList *
Channel::scanListObj() const {
  return _scanlist.as<ScanList>();
}
bool
Channel::setScanListObj(ScanList *list) {
  return _scanlist.set(list);
}

void
Channel::onReferenceModified() {
  emit modified(this);
}

OpenGD77ChannelExtension *
Channel::openGD77ChannelExtension() const {
  return _openGD77ChannelExtension;
}
void
Channel::setOpenGD77ChannelExtension(OpenGD77ChannelExtension *ext) {
  if (_openGD77ChannelExtension == ext)
    return;
  if (_openGD77ChannelExtension)
    _openGD77ChannelExtension->deleteLater();
  _openGD77ChannelExtension = ext;
  if (_openGD77ChannelExtension) {
    _openGD77ChannelExtension->setParent(this);
    connect(_openGD77ChannelExtension, SIGNAL(modified(ConfigItem*)), this, SLOT(onReferenceModified()));
  }
}

TyTChannelExtension *
Channel::tytChannelExtension() const {
  return _tytChannelExtension;
}
void
Channel::setTyTChannelExtension(TyTChannelExtension *ext) {
  if (_tytChannelExtension == ext)
    return;
  if (_tytChannelExtension)
    _tytChannelExtension->deleteLater();
  _tytChannelExtension = ext;
  if (_tytChannelExtension) {
    _tytChannelExtension->setParent(this);
    connect(_tytChannelExtension, SIGNAL(modified(ConfigItem*)), this, SLOT(onReferenceModified()));
  }
}

bool
Channel::populate(YAML::Node &node, const Context &context) {
  if (! ConfigObject::populate(node, context))
    return false;

  if (defaultPower()) {
    YAML::Node def = YAML::Node(YAML::NodeType::Scalar); def.SetTag("!default");
    node["power"] = def;
  } else {
    QMetaEnum metaEnum = QMetaEnum::fromType<Power>();
    node["power"] = metaEnum.valueToKey((unsigned)power());
  }

  if (defaultTimeout()) {
    YAML::Node def = YAML::Node(YAML::NodeType::Scalar); def.SetTag("!default");
    node["timeout"] = def;
  } else {
    node["timeout"] = timeout();
  }

  if (defaultVOX()) {
    YAML::Node def = YAML::Node(YAML::NodeType::Scalar); def.SetTag("!default");
    node["vox"] = def;
  } else {
    node["vox"] = vox();
  }

  return true;
}

ConfigItem *
Channel::allocateChild(QMetaProperty &prop, const YAML::Node &node, const Context &ctx, const ErrorStack &err) {
  Q_UNUSED(node); Q_UNUSED(ctx)
  if (0 == strcmp("openGD77", prop.name())) {
    return new OpenGD77ChannelExtension();
  } else if (0 == strcmp("tyt", prop.name())) {
    return new TyTChannelExtension();
  }

  errMsg(err) << "Cannot allocate instance for unknown child '" << prop.name() << "'.";
  return nullptr;
}

bool
Channel::parse(const YAML::Node &node, ConfigItem::Context &ctx, const ErrorStack &err) {
  if (! node)
    return false;

  if ((! node.IsMap()) || (1 != node.size())) {
    errMsg(err) << node.Mark().line << ":" << node.Mark().column
                << ": Cannot parse channel: Expected object with one child.";
    return false;
  }

  YAML::Node ch = node.begin()->second;
  if ((!ch["power"]) || ("!default" == ch["power"].Tag())) {
    setDefaultPower();
  } else if (ch["power"] && ch["power"].IsScalar()) {
    QMetaEnum meta = QMetaEnum::fromType<Channel::Power>();
    setPower((Channel::Power)meta.keyToValue(ch["power"].as<std::string>().c_str()));
  }

  if ((!ch["timeout"]) || ("!default" == ch["timeout"].Tag())) {
    setDefaultTimeout();
  } else if (ch["timeout"] && ch["timeout"].IsScalar()) {
    setTimeout(ch["timeout"].as<unsigned>());
  }

  if ((!ch["vox"]) || ("!default" == ch["vox"].Tag())) {
    setVOXDefault();
  } else if (ch["vox"] && ch["vox"].IsScalar()) {
    setVOX(ch["vox"].as<unsigned>());
  }

  return ConfigObject::parse(ch, ctx, err);
}

bool
Channel::link(const YAML::Node &node, const ConfigItem::Context &ctx, const ErrorStack &err) {
  if (! node)
    return false;

  if ((! node.IsMap()) || (1 != node.size())) {
    errMsg(err) << node.Mark().line << ":" << node.Mark().column
                << ": Cannot link channel: Expected object with one child.";
    return false;
  }

  return ConfigObject::link(node.begin()->second, ctx, err);
}


/* ********************************************************************************************* *
 * Implementation of AnalogChannel
 * ********************************************************************************************* */
AnalogChannel::AnalogChannel(QObject *parent)
  : Channel(parent),
    _admit(Admit::Always), _squelch(std::numeric_limits<unsigned>::max()),
    _rxTone(Signaling::SIGNALING_NONE), _txTone(Signaling::SIGNALING_NONE), _bw(Bandwidth::Narrow),
    _aprsSystem()
{
  // Link APRS system reference
  connect(&_aprsSystem, SIGNAL(modified()), this, SLOT(onReferenceModified()));
}

AnalogChannel::AnalogChannel(const AnalogChannel &other, QObject *parent)
  : Channel(parent), _aprsSystem()
{
  copy(other);
  // Link APRS system reference
  connect(&_aprsSystem, SIGNAL(modified()), this, SLOT(onReferenceModified()));
}

bool
AnalogChannel::copy(const ConfigItem &other) {
  const AnalogChannel *c = other.as<AnalogChannel>();
  if ((nullptr==c) || (! Channel::copy(other)))
    return false;

  setRXTone(c->rxTone());
  setTXTone(c->txTone());

  return true;
}

ConfigItem *
AnalogChannel::clone() const {
  AnalogChannel *c = new AnalogChannel();
  if (! c->copy(*this)) {
    c->deleteLater();
    return nullptr;
  }
  return c;
}

void
AnalogChannel::clear() {
  Channel::clear();
  setAdmit(Admit::Always);
  setSquelchDefault();
  setRXTone(Signaling::SIGNALING_NONE);
  setTXTone(Signaling::SIGNALING_NONE);
  setBandwidth(Bandwidth::Narrow);
  setAPRSSystem(nullptr);
}

AnalogChannel::Admit
AnalogChannel::admit() const {
  return _admit;
}
void
AnalogChannel::setAdmit(Admit admit) {
  _admit = admit;
  emit modified(this);
}

bool
AnalogChannel::defaultSquelch() const {
  return std::numeric_limits<unsigned>::max()==squelch();
}
bool
AnalogChannel::squelchDisabled() const {
  return 0==squelch();
}
unsigned
AnalogChannel::squelch() const {
  return _squelch;
}
bool
AnalogChannel::setSquelch(unsigned val) {
  _squelch = val;
  emit modified(this);
  return true;
}
void
AnalogChannel::disableSquelch() {
  setSquelch(0);
}
void
AnalogChannel::setSquelchDefault() {
  setSquelch(std::numeric_limits<unsigned>::max());
}

Signaling::Code
AnalogChannel::rxTone() const {
  return _rxTone;
}
bool
AnalogChannel::setRXTone(Signaling::Code code) {
  _rxTone = code;
  emit modified(this);
  return true;
}

Signaling::Code
AnalogChannel::txTone() const {
  return _txTone;
}
bool
AnalogChannel::setTXTone(Signaling::Code code) {
  _txTone = code;
  emit modified(this);
  return true;
}

AnalogChannel::Bandwidth
AnalogChannel::bandwidth() const {
  return _bw;
}
bool
AnalogChannel::setBandwidth(Bandwidth bw) {
  _bw = bw;
  emit modified(this);
  return true;
}

const APRSSystemReference *
AnalogChannel::aprs() const {
  return &_aprsSystem;
}

APRSSystemReference *
AnalogChannel::aprs() {
  return &_aprsSystem;
}

void
AnalogChannel::setAPRS(APRSSystemReference *ref) {
  if (nullptr == ref)
    return _aprsSystem.clear();
  _aprsSystem.copy(ref);
}

APRSSystem *
AnalogChannel::aprsSystem() const {
  return _aprsSystem.as<APRSSystem>();
}
void
AnalogChannel::setAPRSSystem(APRSSystem *sys) {
  _aprsSystem.set(sys);
}

YAML::Node
AnalogChannel::serialize(const Context &context) {
  YAML::Node node = Channel::serialize(context);
  if (node.IsNull())
    return node;
  YAML::Node type;
  type["analog"] = node;
  return type;
}

bool
AnalogChannel::populate(YAML::Node &node, const Context &context) {
  if (! Channel::populate(node, context))
    return false;

  if (Signaling::SIGNALING_NONE != _rxTone) {
    YAML::Node tone;
    if (Signaling::isCTCSS(_rxTone))
      tone["ctcss"] = Signaling::toCTCSSFrequency(_rxTone);
    else if (Signaling::isDCSNormal(_rxTone))
      tone["dcs"] = Signaling::toDCSNumber(_rxTone);
    else if (Signaling::isDCSInverted(_rxTone))
      tone["dcs"] = -Signaling::toDCSNumber(_rxTone);
    tone.SetStyle(YAML::EmitterStyle::Flow);
    node["rxTone"] = tone;
  }

  if (Signaling::SIGNALING_NONE != _txTone) {
    YAML::Node tone;
    if (Signaling::isCTCSS(_txTone))
      tone["ctcss"] = Signaling::toCTCSSFrequency(_txTone);
    else if (Signaling::isDCSNormal(_txTone))
      tone["dcs"] = Signaling::toDCSNumber(_txTone);
    else if (Signaling::isDCSInverted(_txTone))
      tone["dcs"] = -Signaling::toDCSNumber(_txTone);
    tone.SetStyle(YAML::EmitterStyle::Flow);
    node["txTone"] = tone;
  }

  if (defaultSquelch()) {
    YAML::Node def = YAML::Node(YAML::NodeType::Scalar); def.SetTag("!default");
    node["squelch"] = def;
  } else {
    node["squelch"] = squelch();
  }

  return true;
}

bool
AnalogChannel::parse(const YAML::Node &node, Context &ctx, const ErrorStack &err) {
  if (! node)
    return false;

  if ((! node.IsMap()) || (1 != node.size())) {
    errMsg(err) << node.Mark().line << ":" << node.Mark().column
                << ": Cannot parse analog channel: Expected object with one child.";
    return false;
  }

  YAML::Node ch = node.begin()->second;

  setRXTone(Signaling::SIGNALING_NONE);
  if (ch["rxTone"] && ch["rxTone"].IsMap()) {
    if (ch["rxTone"]["ctcss"] && ch["rxTone"]["ctcss"].IsScalar()) {
      setRXTone(Signaling::fromCTCSSFrequency(ch["rxTone"]["ctcss"].as<double>()));
    } else if (ch["rxTone"]["dcs"] && ch["rxTone"]["dcs"].IsScalar()) {
      int code = ch["rxTone"]["dcs"].as<int>();
      bool inverted = (code < 0); code = std::abs(code);
      setRXTone(Signaling::fromDCSNumber(code, inverted));
    }
  }

  setTXTone(Signaling::SIGNALING_NONE);
  if (ch["txTone"] && ch["txTone"].IsMap()) {
    if (ch["txTone"]["ctcss"] && ch["txTone"]["ctcss"].IsScalar()) {
      setTXTone(Signaling::fromCTCSSFrequency(ch["txTone"]["ctcss"].as<double>()));
    } else if (ch["txTone"]["dcs"] && ch["txTone"]["dcs"].IsScalar()) {
      int code = ch["txTone"]["dcs"].as<int>();
      bool inverted = (code < 0); code = std::abs(code);
      setTXTone(Signaling::fromDCSNumber(code, inverted));
    }
  }

  if ((!ch["squelch"]) || ("!default" == ch["squelch"].Tag())) {
    setSquelchDefault();
  } else if (ch["squelch"].IsDefined() && ch["squelch"].IsScalar()) {
    setSquelch(ch["squelch"].as<unsigned>());
  }

  return Channel::parse(node, ctx, err);
}


/* ********************************************************************************************* *
 * Implementation of DigitalChannel
 * ********************************************************************************************* */
DigitalChannel::DigitalChannel(QObject *parent)
  : Channel(parent), _admit(Admit::Always),
    _colorCode(1), _timeSlot(TimeSlot::TS1),
    _rxGroup(), _txContact(), _posSystem(), _roaming(), _radioId()
{
  // Register default tags
  if (! ConfigItem::Context::hasTag(metaObject()->className(), "roaming", "!default"))
    ConfigItem::Context::setTag(metaObject()->className(), "roaming", "!default", DefaultRoamingZone::get());
  if (! ConfigItem::Context::hasTag(metaObject()->className(), "radioId", "!default"))
    ConfigItem::Context::setTag(metaObject()->className(), "radioId", "!default", DefaultRadioID::get());

  // Connect signals of references
  connect(&_rxGroup, SIGNAL(modified()), this, SLOT(onReferenceModified()));
  connect(&_txContact, SIGNAL(modified()), this, SLOT(onReferenceModified()));
  connect(&_posSystem, SIGNAL(modified()), this, SLOT(onReferenceModified()));
  connect(&_roaming, SIGNAL(modified()), this, SLOT(onReferenceModified()));
  connect(&_radioId, SIGNAL(modified()), this, SLOT(onReferenceModified()));
}

DigitalChannel::DigitalChannel(const DigitalChannel &other, QObject *parent)
  : Channel(parent), _rxGroup(), _txContact(), _posSystem(), _roaming(), _radioId()
{
  // Register default tags
  if (! ConfigItem::Context::hasTag(metaObject()->className(), "roaming", "!default"))
    ConfigItem::Context::setTag(metaObject()->className(), "roaming", "!default", DefaultRoamingZone::get());
  if (! ConfigItem::Context::hasTag(metaObject()->className(), "radioId", "!default"))
    ConfigItem::Context::setTag(metaObject()->className(), "radioId", "!default", DefaultRadioID::get());

  copy(other);

  // Connect signals of references
  connect(&_rxGroup, SIGNAL(modified()), this, SLOT(onReferenceModified()));
  connect(&_txContact, SIGNAL(modified()), this, SLOT(onReferenceModified()));
  connect(&_posSystem, SIGNAL(modified()), this, SLOT(onReferenceModified()));
  connect(&_roaming, SIGNAL(modified()), this, SLOT(onReferenceModified()));
  connect(&_radioId, SIGNAL(modified()), this, SLOT(onReferenceModified()));
}

void
DigitalChannel::clear() {
  Channel::clear();
  setColorCode(1);
  setTimeSlot(TimeSlot::TS1);
  setGroupListObj(nullptr);
  setTXContactObj(nullptr);
  setAPRSObj(nullptr);
  setRoamingZone(nullptr);
  setRadioIdObj(nullptr);
}

ConfigItem *
DigitalChannel::clone() const {
  DigitalChannel *c = new DigitalChannel();
  if (! c->copy(*this)) {
    c->deleteLater();
    return nullptr;
  }
  return c;
}

DigitalChannel::Admit
DigitalChannel::admit() const {
  return _admit;
}
void
DigitalChannel::setAdmit(Admit admit) {
  _admit = admit;
  emit modified(this);
}

unsigned
DigitalChannel::colorCode() const {
  return _colorCode;
}
bool
DigitalChannel::setColorCode(unsigned cc) {
  _colorCode = cc;
  emit modified(this);
  return true;
}

DigitalChannel::TimeSlot
DigitalChannel::timeSlot() const {
  return _timeSlot;
}
bool
DigitalChannel::setTimeSlot(TimeSlot slot) {
  _timeSlot = slot;
  emit modified(this);
  return true;
}

const GroupListReference *
DigitalChannel::groupList() const {
  return &_rxGroup;
}

GroupListReference *
DigitalChannel::groupList() {
  return &_rxGroup;
}

void
DigitalChannel::setGroupList(GroupListReference *ref) {
  if (nullptr == ref)
    _rxGroup.clear();
  else
    _rxGroup.copy(ref);
}

RXGroupList *
DigitalChannel::groupListObj() const {
  return _rxGroup.as<RXGroupList>();
}

bool
DigitalChannel::setGroupListObj(RXGroupList *g) {
  if(! _rxGroup.set(g))
    return false;
  emit modified(this);
  return true;
}

const DigitalContactReference *
DigitalChannel::contact() const {
  return &_txContact;
}

DigitalContactReference *
DigitalChannel::contact() {
  return &_txContact;
}

void
DigitalChannel::setContact(DigitalContactReference *ref) {
  if (nullptr == ref)
    _txContact.clear();
  else
    _txContact.copy(ref);
}

DigitalContact *
DigitalChannel::txContactObj() const {
  return _txContact.as<DigitalContact>();
}

bool
DigitalChannel::setTXContactObj(DigitalContact *c) {
  if(! _txContact.set(c))
    return false;
  emit modified(this);
  return true;
}

const PositioningSystemReference *
DigitalChannel::aprs() const {
  return &_posSystem;
}

PositioningSystemReference *
DigitalChannel::aprs() {
  return &_posSystem;
}

void
DigitalChannel::setAPRS(PositioningSystemReference *ref) {
  if (nullptr == ref)
    _posSystem.clear();
  else
    _posSystem.copy(ref);
}

PositioningSystem *
DigitalChannel::aprsObj() const {
  return _posSystem.as<PositioningSystem>();
}

bool
DigitalChannel::setAPRSObj(PositioningSystem *sys) {
  if (! _posSystem.set(sys))
    return false;
  emit modified(this);
  return true;
}

const RoamingZoneReference *
DigitalChannel::roaming() const {
  return &_roaming;
}

RoamingZoneReference *
DigitalChannel::roaming() {
  return &_roaming;
}

void
DigitalChannel::setRoaming(RoamingZoneReference *ref) {
  if (nullptr == ref)
    _roaming.clear();
  else
    _roaming.copy(ref);
}

RoamingZone *
DigitalChannel::roamingZone() const {
  return _roaming.as<RoamingZone>();
}

bool
DigitalChannel::setRoamingZone(RoamingZone *zone) {
  _roaming.set(zone);
  emit modified(this);
  return true;
}

const RadioIDReference *
DigitalChannel::radioId() const {
  return &_radioId;
}

RadioIDReference *
DigitalChannel::radioId() {
  return &_radioId;
}

void
DigitalChannel::setRadioId(RadioIDReference *ref) {
  if (nullptr == ref)
    _radioId.clear();
  else
    _radioId.copy(ref);
}

RadioID *
DigitalChannel::radioIdObj() const {
  return _radioId.as<RadioID>();
}

bool
DigitalChannel::setRadioIdObj(RadioID *id) {
  if (! _radioId.set(id))
    return false;
  emit modified(this);
  return true;
}

YAML::Node
DigitalChannel::serialize(const Context &context) {
  YAML::Node node = Channel::serialize(context);
  if (node.IsNull())
    return node;
  YAML::Node type;
  type["digital"] = node;
  return type;
}


/* ********************************************************************************************* *
 * Implementation of SelectedChannel
 * ********************************************************************************************* */
SelectedChannel *SelectedChannel::_instance = nullptr;

SelectedChannel::SelectedChannel()
  : Channel()
{
  setName("[Selected]");
}

SelectedChannel::~SelectedChannel() {
  SelectedChannel::_instance = nullptr;
}

bool
SelectedChannel::copy(const ConfigItem &other) {
  Q_UNUSED(other);
  return false;
}
ConfigItem *
SelectedChannel::clone() const {
  return nullptr;
}

SelectedChannel *
SelectedChannel::get() {
  if (nullptr == SelectedChannel::_instance)
    SelectedChannel::_instance = new SelectedChannel();
  return SelectedChannel::_instance;
}


/* ********************************************************************************************* *
 * Implementation of ChannelList
 * ********************************************************************************************* */
ChannelList::ChannelList(QObject *parent)
  : ConfigObjectList(Channel::staticMetaObject, parent)
{
  // pass...
}

int
ChannelList::add(ConfigObject *obj, int row) {
  if ((nullptr == obj) || (! obj->is<Channel>())) {
    logError() << "Cannot add nullptr or non-channel objects to channel list.";
    return -1;
  }
  return ConfigObjectList::add(obj, row);
}

Channel *
ChannelList::channel(int idx) const {
  if (ConfigItem *obj = get(idx))
    return obj->as<Channel>();
  return nullptr;
}

DigitalChannel *
ChannelList::findDigitalChannel(double rx, double tx, DigitalChannel::TimeSlot ts, unsigned cc) const {
  for (int i=0; i<count(); i++) {
    if (! _items[i]->is<DigitalChannel>())
      continue;
    /// @bug I should certainly change the frequency handling to integer values!
    if ( (1e-6<std::abs(channel(i)->txFrequency()-tx)) ||
         (1e-6<std::abs(channel(i)->rxFrequency()-rx)) )
      continue;
    DigitalChannel *digi = channel(i)->as<DigitalChannel>();
    if (digi->timeSlot() != ts)
      continue;
    if (digi->colorCode() != cc)
      continue;
    return digi;
  }
  return nullptr;
}

AnalogChannel *
ChannelList::findAnalogChannelByTxFreq(double freq) const {
  for (int i=0; i<count(); i++) {
    if (! channel(i)->is<AnalogChannel>())
      continue;
    if (1e-5 > std::abs(channel(i)->txFrequency()-freq))
      return channel(i)->as<AnalogChannel>();
  }
  return nullptr;
}

ConfigItem *
ChannelList::allocateChild(const YAML::Node &node, ConfigItem::Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx)
  if (! node)
    return nullptr;

  if ((! node.IsMap()) || (1 != node.size())) {
    errMsg(err) << node.Mark().line << ":" << node.Mark().column
                << ": Cannot create channel: Expected object with one child.";
    return nullptr;
  }

  QString type = QString::fromStdString(node.begin()->first.as<std::string>());
  if (("digital" == type) || ("dmr" == type)) {
    return new DigitalChannel();
  } else if (("analog" == type) || ("fm"==type)) {
    return new AnalogChannel();
  }

  errMsg(err) << node.Mark().line << ":" << node.Mark().column
              << ": Cannot create channel: Unknown type '" << type << "'.";

  return nullptr;
}

