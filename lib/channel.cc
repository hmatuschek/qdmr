#include "channel.hh"
#include "contact.hh"
#include "ctcssbox.hh"
#include "rxgrouplist.hh"
#include "config.hh"
#include "scanlist.hh"
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

#include <QDebug>

/* ********************************************************************************************* *
 * Implementation of Channel
 * ********************************************************************************************* */
Channel::Channel(QObject *parent)
  : ConfigObject("ch", parent), _name(""), _rxFreq(0), _txFreq(0), _defaultPower(true),
    _power(Power::Low), _txTimeOut(std::numeric_limits<unsigned>::max()), _rxOnly(false),
    _vox(std::numeric_limits<unsigned>::max()), _scanlist()
{
  // Link scan list modification event (e.g., scan list gets deleted).
  connect(&_scanlist, SIGNAL(modified()), this, SLOT(onReferenceModified()));
}

Channel::Channel(const Channel &other, QObject *parent)
  : ConfigObject("ch", parent), _scanlist()
{
  setName(other.name());
  setRXFrequency(other.rxFrequency());
  setTXFrequency(other.txFrequency());
  if (other.defaultPower())
    setDefaultPower();
  else
    setPower(other.power());
  if (other.defaultTimeout())
    setDefaultTimeout();
  else
    setTimeout(other.timeout());
  setRXOnly(other.rxOnly());
  if (other.defaultVOX())
    setVOXDefault();
  setScanListObj(other.scanListObj());

  // Link scan list modification event (e.g., scan list gets deleted).
  connect(&_scanlist, SIGNAL(modified()), this, SLOT(onReferenceModified()));
}

const QString &
Channel::name() const {
  return _name;
}
bool
Channel::setName(const QString &name) {
  if (name.simplified().isEmpty())
    return false;
  _name = name;
  emit modified(this);
  return true;
}

double Channel::rxFrequency() const {
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

bool
Channel::populate(YAML::Node &node, const Context &context) {
  if (!ConfigObject::populate(node, context))
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
  setAdmit(other.admit());
  if (other.defaultSquelch())
    setSquelchDefault();
  else
    setSquelch(other.squelch());
  setRXTone(other.rxTone());
  setTXTone(other.txTone());
  setBandwidth(other.bandwidth());
  setAPRSSystem(other.aprsSystem());

  // Link APRS system reference
  connect(&_aprsSystem, SIGNAL(modified()), this, SLOT(onReferenceModified()));
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

APRSSystem *
AnalogChannel::aprsSystem() const {
  return _aprsSystem.as<APRSSystem>();
}
void
AnalogChannel::setAPRSSystem(APRSSystem *sys) {
  _aprsSystem.set(sys);
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


/* ********************************************************************************************* *
 * Implementation of DigitalChannel
 * ********************************************************************************************* */
DigitalChannel::DigitalChannel(QObject *parent)
  : Channel(parent), _admit(Admit::Always),
    _colorCode(1), _timeSlot(TimeSlot::TS1),
    _rxGroup(), _txContact(), _posSystem(), _roaming(), _radioId()
{
  // Register default tags
  if (! ConfigObject::Context::hasTag(metaObject()->className(), "roaming", "!default"))
    ConfigObject::Context::setTag(metaObject()->className(), "roaming", "!default", DefaultRoamingZone::get());
  if (! ConfigObject::Context::hasTag(metaObject()->className(), "radioID", "!default"))
    ConfigObject::Context::setTag(metaObject()->className(), "radioID", "!default", DefaultRadioID::get());

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
  if (! ConfigObject::Context::hasTag(metaObject()->className(), "roaming", "!default"))
    ConfigObject::Context::setTag(metaObject()->className(), "roaming", "!default", DefaultRoamingZone::get());
  if (! ConfigObject::Context::hasTag(metaObject()->className(), "radioID", "!default"))
    ConfigObject::Context::setTag(metaObject()->className(), "radioID", "!default", DefaultRadioID::get());

  setColorCode(other.colorCode());
  setTimeSlot(other.timeSlot());
  setGroupListObj(other.groupListObj());
  setTXContactObj(other.txContactObj());
  setAPRSObj(other.aprsObj());
  setRoamingZone(other.roamingZone());
  setRadioIdObj(other.radioIdObj());

  // Connect signals of references
  connect(&_rxGroup, SIGNAL(modified()), this, SLOT(onReferenceModified()));
  connect(&_txContact, SIGNAL(modified()), this, SLOT(onReferenceModified()));
  connect(&_posSystem, SIGNAL(modified()), this, SLOT(onReferenceModified()));
  connect(&_roaming, SIGNAL(modified()), this, SLOT(onReferenceModified()));
  connect(&_radioId, SIGNAL(modified()), this, SLOT(onReferenceModified()));
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
DigitalChannel::radioID() const {
  return &_radioId;
}

RadioIDReference *
DigitalChannel::radioID() {
  return &_radioId;
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
  if ((nullptr == obj) || (! obj->is<Channel>()))
    return -1;
  return ConfigObjectList::add(obj, row);
}

Channel *
ChannelList::channel(int idx) const {
  if (ConfigObject *obj = get(idx))
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

