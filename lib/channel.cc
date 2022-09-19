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
    _tytChannelExtension(nullptr), _commercialExtension(nullptr)
{
  // Link scan list modification event (e.g., scan list gets deleted).
  connect(&_scanlist, SIGNAL(modified()), this, SLOT(onReferenceModified()));
}

Channel::Channel(const Channel &other, QObject *parent)
  : ConfigObject("ch", parent), _scanlist(), _openGD77ChannelExtension(nullptr),
    _tytChannelExtension(nullptr), _commercialExtension(nullptr)
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
  if (_commercialExtension)
    _commercialExtension->deleteLater();
  _commercialExtension = nullptr;
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
Channel::scanListRef() const {
  return &_scanlist;
}

ScanListReference *
Channel::scanListRef() {
  return &_scanlist;
}

ScanList *
Channel::scanList() const {
  return _scanlist.as<ScanList>();
}
bool
Channel::setScanList(ScanList *list) {
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

CommercialChannelExtension *
Channel::commercialExtension() const {
  return _commercialExtension;
}
void
Channel::setCommercialExtension(CommercialChannelExtension *ext) {
  if (_commercialExtension == ext)
    return;
  if (_commercialExtension)
    _commercialExtension->deleteLater();
  _commercialExtension = ext;
  if (_commercialExtension) {
    _commercialExtension->setParent(this);
    connect(_commercialExtension, SIGNAL(modified(ConfigItem*)), this, SLOT(onReferenceModified()));
  }
}

bool
Channel::populate(YAML::Node &node, const Context &context, const ErrorStack &err) {
  if (! ConfigObject::populate(node, context, err))
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
  : Channel(parent)
{
  // pass...
}

AnalogChannel::AnalogChannel(const AnalogChannel &other, QObject *parent)
  : Channel(other, parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of FMChannel
 * ********************************************************************************************* */
FMChannel::FMChannel(QObject *parent)
  : AnalogChannel(parent),
    _admit(Admit::Always), _squelch(std::numeric_limits<unsigned>::max()),
    _rxTone(Signaling::SIGNALING_NONE), _txTone(Signaling::SIGNALING_NONE), _bw(Bandwidth::Narrow),
    _aprsSystem()
{
  // Link APRS system reference
  connect(&_aprsSystem, SIGNAL(modified()), this, SLOT(onReferenceModified()));
}

FMChannel::FMChannel(const FMChannel &other, QObject *parent)
  : AnalogChannel(parent), _aprsSystem()
{
  copy(other);
  // Link APRS system reference
  connect(&_aprsSystem, SIGNAL(modified()), this, SLOT(onReferenceModified()));
}

bool
FMChannel::copy(const ConfigItem &other) {
  const FMChannel *c = other.as<FMChannel>();
  if ((nullptr==c) || (! AnalogChannel::copy(other)))
    return false;

  setRXTone(c->rxTone());
  setTXTone(c->txTone());

  return true;
}

ConfigItem *
FMChannel::clone() const {
  FMChannel *c = new FMChannel();
  if (! c->copy(*this)) {
    c->deleteLater();
    return nullptr;
  }
  return c;
}

void
FMChannel::clear() {
  AnalogChannel::clear();
  setAdmit(Admit::Always);
  setSquelchDefault();
  setRXTone(Signaling::SIGNALING_NONE);
  setTXTone(Signaling::SIGNALING_NONE);
  setBandwidth(Bandwidth::Narrow);
  setAPRSSystem(nullptr);
}

FMChannel::Admit
FMChannel::admit() const {
  return _admit;
}
void
FMChannel::setAdmit(Admit admit) {
  _admit = admit;
  emit modified(this);
}

bool
FMChannel::defaultSquelch() const {
  return std::numeric_limits<unsigned>::max()==squelch();
}
bool
FMChannel::squelchDisabled() const {
  return 0==squelch();
}
unsigned
FMChannel::squelch() const {
  return _squelch;
}
bool
FMChannel::setSquelch(unsigned val) {
  _squelch = val;
  emit modified(this);
  return true;
}
void
FMChannel::disableSquelch() {
  setSquelch(0);
}
void
FMChannel::setSquelchDefault() {
  setSquelch(std::numeric_limits<unsigned>::max());
}

Signaling::Code
FMChannel::rxTone() const {
  return _rxTone;
}
bool
FMChannel::setRXTone(Signaling::Code code) {
  _rxTone = code;
  emit modified(this);
  return true;
}

Signaling::Code
FMChannel::txTone() const {
  return _txTone;
}
bool
FMChannel::setTXTone(Signaling::Code code) {
  _txTone = code;
  emit modified(this);
  return true;
}

FMChannel::Bandwidth
FMChannel::bandwidth() const {
  return _bw;
}
bool
FMChannel::setBandwidth(Bandwidth bw) {
  _bw = bw;
  emit modified(this);
  return true;
}

const APRSSystemReference *
FMChannel::aprs() const {
  return &_aprsSystem;
}

APRSSystemReference *
FMChannel::aprs() {
  return &_aprsSystem;
}

void
FMChannel::setAPRS(APRSSystemReference *ref) {
  if (nullptr == ref)
    return _aprsSystem.clear();
  _aprsSystem.copy(ref);
}

APRSSystem *
FMChannel::aprsSystem() const {
  return _aprsSystem.as<APRSSystem>();
}
void
FMChannel::setAPRSSystem(APRSSystem *sys) {
  _aprsSystem.set(sys);
}

YAML::Node
FMChannel::serialize(const Context &context, const ErrorStack &err) {
  YAML::Node node = AnalogChannel::serialize(context, err);
  if (node.IsNull())
    return node;

  YAML::Node type;
  type["fm"] = node;
  return type;
}

bool
FMChannel::populate(YAML::Node &node, const Context &context, const ErrorStack &err) {
  if (! AnalogChannel::populate(node, context, err))
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
FMChannel::parse(const YAML::Node &node, Context &ctx, const ErrorStack &err) {
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

  return AnalogChannel::parse(node, ctx, err);
}


/* ********************************************************************************************* *
 * Implementation of DigitalChannel
 * ********************************************************************************************* */
DigitalChannel::DigitalChannel(QObject *parent)
  : Channel(parent)
{
  // pass...
}

DigitalChannel::DigitalChannel(const DigitalChannel &other, QObject *parent)
  : Channel(other, parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of DMRChannel
 * ********************************************************************************************* */
DMRChannel::DMRChannel(QObject *parent)
  : DigitalChannel(parent), _admit(Admit::Always),
    _colorCode(1), _timeSlot(TimeSlot::TS1),
    _rxGroup(), _txContact(), _posSystem(), _roaming(), _radioId()
{
  // Register default tags
  if (! ConfigItem::Context::hasTag(metaObject()->className(), "roaming", "!default"))
    ConfigItem::Context::setTag(metaObject()->className(), "roaming", "!default", DefaultRoamingZone::get());
  if (! ConfigItem::Context::hasTag(metaObject()->className(), "radioId", "!default"))
    ConfigItem::Context::setTag(metaObject()->className(), "radioId", "!default", DefaultRadioID::get());

  // Set default DMR Id
  _radioId.set(DefaultRadioID::get());

  // Connect signals of references
  connect(&_rxGroup, SIGNAL(modified()), this, SLOT(onReferenceModified()));
  connect(&_txContact, SIGNAL(modified()), this, SLOT(onReferenceModified()));
  connect(&_posSystem, SIGNAL(modified()), this, SLOT(onReferenceModified()));
  connect(&_roaming, SIGNAL(modified()), this, SLOT(onReferenceModified()));
  connect(&_radioId, SIGNAL(modified()), this, SLOT(onReferenceModified()));
}

DMRChannel::DMRChannel(const DMRChannel &other, QObject *parent)
  : DigitalChannel(parent), _rxGroup(), _txContact(), _posSystem(), _roaming(), _radioId()
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
DMRChannel::clear() {
  DigitalChannel::clear();
  setColorCode(1);
  setTimeSlot(TimeSlot::TS1);
  setGroupListObj(nullptr);
  setTXContactObj(nullptr);
  setAPRSObj(nullptr);
  setRoamingZone(nullptr);
  setRadioIdObj(DefaultRadioID::get());
}

ConfigItem *
DMRChannel::clone() const {
  DMRChannel *c = new DMRChannel();
  if (! c->copy(*this)) {
    c->deleteLater();
    return nullptr;
  }
  return c;
}

DMRChannel::Admit
DMRChannel::admit() const {
  return _admit;
}
void
DMRChannel::setAdmit(Admit admit) {
  _admit = admit;
  emit modified(this);
}

unsigned
DMRChannel::colorCode() const {
  return _colorCode;
}
bool
DMRChannel::setColorCode(unsigned cc) {
  if (15 < cc) {
    logWarn() << "Color-code " << cc << " is not in range [0,15], set to 15.";
    cc = 15;
  }
  _colorCode = cc;
  emit modified(this);
  return true;
}

DMRChannel::TimeSlot
DMRChannel::timeSlot() const {
  return _timeSlot;
}
bool
DMRChannel::setTimeSlot(TimeSlot slot) {
  _timeSlot = slot;
  emit modified(this);
  return true;
}

const GroupListReference *
DMRChannel::groupList() const {
  return &_rxGroup;
}

GroupListReference *
DMRChannel::groupList() {
  return &_rxGroup;
}

void
DMRChannel::setGroupList(GroupListReference *ref) {
  if (nullptr == ref)
    _rxGroup.clear();
  else
    _rxGroup.copy(ref);
}

RXGroupList *
DMRChannel::groupListObj() const {
  return _rxGroup.as<RXGroupList>();
}

bool
DMRChannel::setGroupListObj(RXGroupList *g) {
  if(! _rxGroup.set(g))
    return false;
  emit modified(this);
  return true;
}

const DMRContactReference *
DMRChannel::contact() const {
  return &_txContact;
}

DMRContactReference *
DMRChannel::contact() {
  return &_txContact;
}

void
DMRChannel::setContact(DMRContactReference *ref) {
  if (nullptr == ref)
    _txContact.clear();
  else
    _txContact.copy(ref);
}

DMRContact *
DMRChannel::txContactObj() const {
  return _txContact.as<DMRContact>();
}

bool
DMRChannel::setTXContactObj(DMRContact *c) {
  if(! _txContact.set(c))
    return false;
  emit modified(this);
  return true;
}

const PositioningSystemReference *
DMRChannel::aprs() const {
  return &_posSystem;
}

PositioningSystemReference *
DMRChannel::aprs() {
  return &_posSystem;
}

void
DMRChannel::setAPRS(PositioningSystemReference *ref) {
  if (nullptr == ref)
    _posSystem.clear();
  else
    _posSystem.copy(ref);
}

PositioningSystem *
DMRChannel::aprsObj() const {
  return _posSystem.as<PositioningSystem>();
}

bool
DMRChannel::setAPRSObj(PositioningSystem *sys) {
  if (! _posSystem.set(sys))
    return false;
  emit modified(this);
  return true;
}

const RoamingZoneReference *
DMRChannel::roaming() const {
  return &_roaming;
}

RoamingZoneReference *
DMRChannel::roaming() {
  return &_roaming;
}

void
DMRChannel::setRoaming(RoamingZoneReference *ref) {
  if (nullptr == ref)
    _roaming.clear();
  else
    _roaming.copy(ref);
}

RoamingZone *
DMRChannel::roamingZone() const {
  return _roaming.as<RoamingZone>();
}

bool
DMRChannel::setRoamingZone(RoamingZone *zone) {
  _roaming.set(zone);
  emit modified(this);
  return true;
}

const DMRRadioIDReference *
DMRChannel::radioId() const {
  return &_radioId;
}

DMRRadioIDReference *
DMRChannel::radioId() {
  return &_radioId;
}

void
DMRChannel::setRadioId(DMRRadioIDReference *ref) {
  if (nullptr == ref)
    _radioId.clear();
  else
    _radioId.copy(ref);
}

DMRRadioID *
DMRChannel::radioIdObj() const {
  return _radioId.as<DMRRadioID>();
}

bool
DMRChannel::setRadioIdObj(DMRRadioID *id) {
  if (! _radioId.set(id))
    return false;
  emit modified(this);
  return true;
}

YAML::Node
DMRChannel::serialize(const Context &context, const ErrorStack &err) {
  YAML::Node node = DigitalChannel::serialize(context, err);
  if (node.IsNull())
    return node;

  YAML::Node type;
  type["dmr"] = node;
  return type;
}


/* ********************************************************************************************* *
 * Implementation of M17Channel
 * ********************************************************************************************* */
M17Channel::M17Channel(QObject *parent)
  : DigitalChannel(parent), _mode(Mode::Voice), _accessNumber(0), _txContact(),
    _gpsEnabled(false), _encryptionMode(EncryptionMode::None)
{
  // Connect signals of references
  connect(&_txContact, SIGNAL(modified()), this, SLOT(onReferenceModified()));
}

M17Channel::M17Channel(const M17Channel &other, QObject *parent)
  : DigitalChannel(other, parent), _mode(Mode::Voice), _accessNumber(0),
    _txContact(), _gpsEnabled(false),
    _encryptionMode(EncryptionMode::None)
{
  copy(other);

  // Connect signals of references
  connect(&_txContact, SIGNAL(modified()), this, SLOT(onReferenceModified()));
}

void
M17Channel::clear() {
  DigitalChannel::clear();
  setMode(Mode::Voice);
  setAccessNumber(0);
  setContact(nullptr);
  enableGPS(false);
  setEncryptionMode(EncryptionMode::None);
}

ConfigItem *
M17Channel::clone() const {
  M17Channel *c = new M17Channel();
  if (! c->copy(*this)) {
    c->deleteLater();
    return nullptr;
  }
  return c;
}

M17Channel::Mode
M17Channel::mode() const {
  return _mode;
}

void
M17Channel::setMode(Mode mode) {
  if (_mode == mode)
    return;
  _mode = mode;
  emit modified(this);
}

unsigned int
M17Channel::accessNumber() const {
  return _accessNumber;
}

void
M17Channel::setAccessNumber(unsigned int can) {
  can = std::min(15u, can);
  if (_accessNumber == can)
    return;
  _accessNumber = can;
  emit modified(this);
}

const M17ContactReference *
M17Channel::contactRef() const {
  return &_txContact;
}

M17ContactReference *
M17Channel::contactRef() {
  return &_txContact;
}

void
M17Channel::setContactRef(M17ContactReference *ref) {
  if (nullptr == ref)
    _txContact.clear();
  else
    _txContact.copy(ref);
}

M17Contact *
M17Channel::contact() const {
  return _txContact.as<M17Contact>();
}

bool
M17Channel::setContact(M17Contact *c) {
  if(! _txContact.set(c))
    return false;
  emit modified(this);
  return true;
}

bool
M17Channel::gpsEnabled() const {
  return _gpsEnabled;
}

void
M17Channel::enableGPS(bool enabled) {
  if (_gpsEnabled == enabled)
    return;
  _gpsEnabled = enabled;
  emit modified(this);
}

M17Channel::EncryptionMode
M17Channel::encryptionMode() const {
  return _encryptionMode;
}

void
M17Channel::setEncryptionMode(EncryptionMode mode) {
  if (_encryptionMode == mode)
    return;
  _encryptionMode = mode;
  emit modified(this);
}

YAML::Node
M17Channel::serialize(const Context &context, const ErrorStack &err) {
  YAML::Node node = DigitalChannel::serialize(context, err);
  if (node.IsNull())
    return node;

  YAML::Node type;
  type["m17"] = node;
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

DMRChannel *
ChannelList::findDMRChannel(double rx, double tx, DMRChannel::TimeSlot ts, unsigned cc) const {
  for (int i=0; i<count(); i++) {
    if (! _items[i]->is<DMRChannel>())
      continue;
    /// @bug I should certainly change the frequency handling to integer values!
    if ( (1e-6<std::abs(channel(i)->txFrequency()-tx)) ||
         (1e-6<std::abs(channel(i)->rxFrequency()-rx)) )
      continue;
    DMRChannel *digi = channel(i)->as<DMRChannel>();
    if (digi->timeSlot() != ts)
      continue;
    if (digi->colorCode() != cc)
      continue;
    return digi;
  }
  return nullptr;
}

FMChannel *
ChannelList::findFMChannelByTxFreq(double freq) const {
  for (int i=0; i<count(); i++) {
    if (! channel(i)->is<FMChannel>())
      continue;
    if (1e-5 > std::abs(channel(i)->txFrequency()-freq))
      return channel(i)->as<FMChannel>();
  }
  return nullptr;
}

ConfigItem *
ChannelList::allocateChild(const YAML::Node &node, ConfigItem::Context &ctx, const ErrorStack &err) {
  static bool digitalDepricated = true, analogDeprecated = true;
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
    if (("digital" == type) && digitalDepricated) {
      logWarn() << node.Mark().line << ":" << node.Mark().column
                << ": Using 'digital' for DMR channels is deprecated. Please use 'dmr' instead.";
      digitalDepricated = false;
    }
    return new DMRChannel();
  } else if (("analog" == type) || ("fm"==type)) {
    if (("analog" == type) && analogDeprecated) {
      logWarn() << node.Mark().line << ":" << node.Mark().column
                << ": Using 'analog' for FM channels is deprecated. Please use 'fm' instead.";
      analogDeprecated = false;
    }
    return new FMChannel();
  } else if ("m17" == type) {
    return new M17Channel();
  }

  errMsg(err) << node.Mark().line << ":" << node.Mark().column
              << ": Cannot create channel: Unknown type '" << type << "'.";

  return nullptr;
}

