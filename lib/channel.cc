#include "channel.hh"
#include "contact.hh"
#include "rxgrouplist.hh"
#include "scanlist.hh"
#include "logger.hh"
#include "radioid.hh"
#include "gpssystem.hh"
#include "roamingzone.hh"
#include <cmath>
#include <QAbstractProxyModel>
#include <QMetaEnum>
#include <QRegularExpression>

#include "opengd77_extension.hh"


/* ********************************************************************************************* *
 * Implementation of Channel
 * ********************************************************************************************* */
Channel::Channel(QObject *parent)
  : ConfigObject("ch", parent), _rxFreq(Frequency::fromHz(0)), _txFreq(Frequency::fromHz(0)),
    _defaultPower(true), _power(Power::Low), _txTimeOut(Interval::null()),
    _rxOnly(false), _vox(), _scanlist(),
    _openGD77ChannelExtension(nullptr),
    _tytChannelExtension(nullptr)
{
  Context::setTag(staticMetaObject.className(), "timeout",
                  "!default", QVariant::fromValue(Interval::null()));
  Context::setTag(staticMetaObject.className(), "vox",
                  "!default", QVariant::fromValue(Level::invalid()));

  // Link scan list modification event (e.g., scan list gets deleted).
  connect(&_scanlist, SIGNAL(modified()), this, SLOT(onReferenceModified()));
}

Channel::Channel(const Channel &other, QObject *parent)
  : ConfigObject("ch", parent), _scanlist(), _openGD77ChannelExtension(nullptr),
    _tytChannelExtension(nullptr)
{
  Channel::copy(other);

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

  setRXFrequency(Frequency::fromHz(0));
  setTXFrequency(Frequency::fromHz(0));
  setDefaultPower();
  setDefaultTimeout();
  setRXOnly(false);
  setVOXDefault();

  _scanlist.clear();

  setOpenGD77ChannelExtension(nullptr);
  setTyTChannelExtension(nullptr);
}


Frequency Channel::rxFrequency() const {
  return _rxFreq;
}

bool
Channel::setRXFrequency(Frequency freq) {
  if (freq == _rxFreq)
    return true;

  _rxFreq = freq;
  emit modified(this);

  return true;
}

Frequency
Channel::txFrequency() const {
  return _txFreq;
}

bool
Channel::setTXFrequency(Frequency freq) {
  if (freq == _txFreq)
    return true;

  _txFreq = freq;
  emit modified(this);

  return true;
}

FrequencyOffset
Channel::offsetFrequency() const {

    return _txFreq - _rxFreq;
}

Channel::OffsetShift
Channel::offsetShift() const {
    if (_rxFreq > _txFreq) {
        return OffsetShift::Negative;
    } else if (_txFreq > _rxFreq) {
        return OffsetShift::Positive;
    } else {
        return OffsetShift::None;
    }
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
  if ((power == _power) && (! _defaultPower))
    return;
  _power = power;
  _defaultPower = false;
  emit modified(this);
}

void
Channel::setDefaultPower() {
  if (defaultPower())
    return;

  _defaultPower = true;
  emit modified(this);
}


bool
Channel::defaultTimeout() const {
  return timeout().isNull();
}

bool
Channel::timeoutDisabled() const {
  return timeout().isInfinite();
}

Interval
Channel::timeout() const {
  return _txTimeOut;
}

bool
Channel::setTimeout(const Interval& dur) {
  if (dur == _txTimeOut)
    return true;

  _txTimeOut = dur;
  emit modified(this);

  return true;
}

void
Channel::setDefaultTimeout() {
  if (defaultTimeout())
    return;

  setTimeout(Interval::null());
  emit modified(this);
}

void
Channel::disableTimeout() {
  setTimeout(Interval::infinity());
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
  return _vox.isNull();
}
bool
Channel::defaultVOX() const {
  return _vox.isInvalid();
}
Level
Channel::vox() const {
  return _vox;
}
void
Channel::setVOX(Level level) {
  if (_vox == level)
    return;
  _vox = level;
  emit modified(this);
}
void
Channel::setVOXDefault() {
  setVOX(Level::invalid());
}
void
Channel::disableVOX() {
  setVOX(Level::null());
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

  return true;
}

bool
Channel::parse(const YAML::Node &node, ConfigItem::Context &ctx, const ErrorStack &err) {
  if (! node) {
    errMsg(err) << "YAML node is null!";
    return false;
  }

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
  : Channel(parent), _squelch(Level::invalid())
{
  Context::setTag(staticMetaObject.className(), "squelch",
                  "!default", QVariant::fromValue(Level::invalid()));
}

AnalogChannel::AnalogChannel(const AnalogChannel &other, QObject *parent)
  : Channel(other, parent), _squelch(Level::invalid())
{
  Context::setTag(staticMetaObject.className(), "squelch",
                  "!default", QVariant::fromValue(Level::invalid()));
}

bool
AnalogChannel::defaultSquelch() const {
  return _squelch.isInvalid();
}
bool
AnalogChannel::squelchDisabled() const {
  return _squelch.isNull();
}
Level
AnalogChannel::squelch() const {
  return _squelch;
}
bool
AnalogChannel::setSquelch(Level level) {
  if (_squelch == level)
    return true;
  _squelch = level;
  emit modified(this);
  return true;
}
void
AnalogChannel::disableSquelch() {
  setSquelch(Level::null());
}
void
AnalogChannel::setSquelchDefault() {
  setSquelch(Level::invalid());
}




/* ********************************************************************************************* *
 * Implementation of FMChannel
 * ********************************************************************************************* */
FMChannel::FMChannel(QObject *parent)
  : AnalogChannel(parent),
    _admit(Admit::Always), _rxTone(), _txTone(), _bw(Bandwidth::Narrow),
    _aprsSystem(), _extended(new FMChannelExtension(this)), _anytoneExtension(nullptr)
{
  // Link APRS system reference
  connect(&_aprsSystem, &FMAPRSSystemReference::modified, this, &FMChannel::onReferenceModified);
  // Link extended settings
  connect(_extended, &FMChannelExtension::modified, this, &FMChannel::modified);
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
  setRXTone(SelectiveCall());
  setTXTone(SelectiveCall());
  setBandwidth(Bandwidth::Narrow);
  setAPRS(nullptr);
  setAnytoneChannelExtension(nullptr);
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

SelectiveCall
FMChannel::rxTone() const {
  return _rxTone;
}
bool
FMChannel::setRXTone(SelectiveCall code) {
  _rxTone = code;
  emit modified(this);
  return true;
}

SelectiveCall
FMChannel::txTone() const {
  return _txTone;
}
bool
FMChannel::setTXTone(SelectiveCall code) {
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

const FMAPRSSystemReference *
FMChannel::aprsRef() const {
  return &_aprsSystem;
}

FMAPRSSystemReference *
FMChannel::aprsRef() {
  return &_aprsSystem;
}

FMAPRSSystem *
FMChannel::aprs() const {
  return _aprsSystem.as<FMAPRSSystem>();
}
void
FMChannel::setAPRS(FMAPRSSystem *sys) {
  _aprsSystem.set(sys);
}


FMChannelExtension *
FMChannel::extended() const {
  return _extended;
}


AnytoneFMChannelExtension *
FMChannel::anytoneChannelExtension() const {
  return _anytoneExtension;
}
void
FMChannel::setAnytoneChannelExtension(AnytoneFMChannelExtension *ext) {
  if (_anytoneExtension == ext)
    return;
  if (_anytoneExtension)
    _anytoneExtension->deleteLater();
  _anytoneExtension = ext;
  if (_anytoneExtension) {
    _anytoneExtension->setParent(this);
    connect(_anytoneExtension, SIGNAL(modified(ConfigItem*)), this, SLOT(onReferenceModified()));
  }
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
FMChannel::parse(const YAML::Node &node, Context &ctx, const ErrorStack &err) {
  if (! node)
    return false;

  if ((! node.IsMap()) || (1 != node.size())) {
    errMsg(err) << node.Mark().line << ":" << node.Mark().column
                << ": Cannot parse analog channel: Expected object with one child.";
    return false;
  }

  YAML::Node ch = node.begin()->second;

  return AnalogChannel::parse(node, ctx, err);
}



/* ********************************************************************************************* *
 * Implementation of AMChannel
 * ********************************************************************************************* */
AMChannel::AMChannel(QObject *parent)
  : AnalogChannel(parent)
{
  // pass...
}


bool
AMChannel::copy(const ConfigItem &other) {
  auto c = other.as<AMChannel>();
  if ((nullptr==c) || (! AnalogChannel::copy(other)))
    return false;
  return true;
}

ConfigItem *
AMChannel::clone() const {
  auto c = new AMChannel();
  if (! c->copy(*this)) {
    c->deleteLater();
    return nullptr;
  }
  return c;
}

void
AMChannel::clear() {
  AnalogChannel::clear();
  setSquelchDefault();
}

YAML::Node
AMChannel::serialize(const Context &context, const ErrorStack &err) {
  YAML::Node node = AnalogChannel::serialize(context, err);
  if (node.IsNull())
    return node;

  YAML::Node type;
  type["am"] = node;
  return type;
}

bool
AMChannel::parse(const YAML::Node &node, Context &ctx, const ErrorStack &err) {
  if (! node)
    return false;

  if ((! node.IsMap()) || (1 != node.size())) {
    errMsg(err) << node.Mark().line << ":" << node.Mark().column
                << ": Cannot parse analog channel: Expected object with one child.";
    return false;
  }

  YAML::Node ch = node.begin()->second;

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
    _rxGroup(), _txContact(), _posSystem(), _roaming(), _radioId(),
    _extended(new DMRChannelExtension(this)),
    _commercialExtension(nullptr), _anytoneExtension(nullptr)
{
  // Register default tags
  if (! ConfigItem::Context::tagIsSet(staticMetaObject.className(), "roaming", "!default"))
    ConfigItem::Context::setTag(staticMetaObject.className(), "roaming", "!default", QVariant::fromValue(DefaultRoamingZone::get()));
  if (! ConfigItem::Context::tagIsSet(staticMetaObject.className(), "radioId", "!default"))
    ConfigItem::Context::setTag(staticMetaObject.className(), "radioId", "!default", QVariant::fromValue(DefaultRadioID::get()));

  // Set default DMR Id
  _radioId.set(DefaultRadioID::get());

  // Connect signals of references
  connect(&_rxGroup, SIGNAL(modified()), this, SLOT(onReferenceModified()));
  connect(&_txContact, SIGNAL(modified()), this, SLOT(onReferenceModified()));
  connect(&_posSystem, SIGNAL(modified()), this, SLOT(onReferenceModified()));
  connect(&_roaming, SIGNAL(modified()), this, SLOT(onReferenceModified()));
  connect(&_radioId, SIGNAL(modified()), this, SLOT(onReferenceModified()));
  connect(_extended, &DMRChannelExtension::modified, this, &DMRChannel::modified);
}

void
DMRChannel::clear() {
  DigitalChannel::clear();
  setColorCode(1);
  setTimeSlot(TimeSlot::TS1);
  setGroupList(nullptr);
  setContact(nullptr);
  setAPRS(nullptr);
  setRoaming(nullptr);
  setRadioId(DefaultRadioID::get());
  setCommercialExtension(nullptr);
  setAnytoneChannelExtension(nullptr);
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
DMRChannel::groupListRef() const {
  return &_rxGroup;
}

GroupListReference *
DMRChannel::groupListRef() {
  return &_rxGroup;
}

RXGroupList *
DMRChannel::groupList() const {
  return _rxGroup.as<RXGroupList>();
}

bool
DMRChannel::setGroupList(RXGroupList *g) {
  if(! _rxGroup.set(g))
    return false;
  emit modified(this);
  return true;
}

const DMRContactReference *
DMRChannel::contactRef() const {
  return &_txContact;
}

DMRContactReference *
DMRChannel::contactRef() {
  return &_txContact;
}

DMRContact *
DMRChannel::contact() const {
  return _txContact.as<DMRContact>();
}

bool
DMRChannel::setContact(DMRContact *c) {
  if(! _txContact.set(c))
    return false;
  emit modified(this);
  return true;
}

const PositioningSystemReference *
DMRChannel::aprsRef() const {
  return &_posSystem;
}

PositioningSystemReference *
DMRChannel::aprsRef() {
  return &_posSystem;
}

PositionReportingSystem *
DMRChannel::aprs() const {
  return _posSystem.as<PositionReportingSystem>();
}

bool
DMRChannel::setAPRS(PositionReportingSystem *sys) {
  if (! _posSystem.set(sys))
    return false;
  emit modified(this);
  return true;
}

const RoamingZoneReference *
DMRChannel::roamingRef() const {
  return &_roaming;
}

RoamingZoneReference *
DMRChannel::roamingRef() {
  return &_roaming;
}

RoamingZone *
DMRChannel::roaming() const {
  return _roaming.as<RoamingZone>();
}

bool
DMRChannel::setRoaming(RoamingZone *zone) {
  _roaming.set(zone);
  emit modified(this);
  return true;
}

const DMRRadioIDReference *
DMRChannel::radioIdRef() const {
  return &_radioId;
}

DMRRadioIDReference *
DMRChannel::radioIdRef() {
  return &_radioId;
}

DMRRadioID *
DMRChannel::radioId() const {
  return _radioId.as<DMRRadioID>();
}

bool
DMRChannel::setRadioId(DMRRadioID *id) {
  if (! _radioId.set(id))
    return false;
  emit modified(this);
  return true;
}


DMRChannelExtension *
DMRChannel::extended() const {
  return _extended;
}


CommercialChannelExtension *
DMRChannel::commercialExtension() const {
  return _commercialExtension;
}
void
DMRChannel::setCommercialExtension(CommercialChannelExtension *ext) {
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

AnytoneDMRChannelExtension *
DMRChannel::anytoneChannelExtension() const {
  return _anytoneExtension;
}
void
DMRChannel::setAnytoneChannelExtension(AnytoneDMRChannelExtension *ext) {
  if (_anytoneExtension == ext)
    return;
  if (_anytoneExtension)
    _anytoneExtension->deleteLater();
  _anytoneExtension = ext;
  if (_anytoneExtension) {
    _anytoneExtension->setParent(this);
    connect(_anytoneExtension, SIGNAL(modified(ConfigItem*)), this, SLOT(onReferenceModified()));
  }
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
ChannelList::add(ConfigObject *obj, int row, bool unique) {
  if ((nullptr == obj) || (! obj->is<Channel>())) {
    logError() << "Cannot add nullptr or non-channel objects to channel list.";
    return -1;
  }
  return ConfigObjectList::add(obj, row, unique);
}

Channel *
ChannelList::channel(int idx) const {
  if (ConfigItem *obj = get(idx))
    return obj->as<Channel>();
  return nullptr;
}

DMRChannel *
ChannelList::findDMRChannel(Frequency rx, Frequency tx, DMRChannel::TimeSlot ts, unsigned cc) const {
  for (int i=0; i<count(); i++) {
    if (! _items[i]->is<DMRChannel>())
      continue;
    /// @bug I should certainly change the frequency handling to integer values!
    if ((channel(i)->txFrequency()!=tx) || (channel(i)->rxFrequency()!=rx))
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
ChannelList::findFMChannelByTxFreq(Frequency freq) const {
  for (int i=0; i<count(); i++) {
    if (! channel(i)->is<FMChannel>())
      continue;
    if (channel(i)->txFrequency() == freq)
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
  } else if ("am" == type) {
    return new AMChannel();
  }

  errMsg(err) << node.Mark().line << ":" << node.Mark().column
              << ": Cannot create channel: Unknown type '" << type << "'.";

  return nullptr;
}

