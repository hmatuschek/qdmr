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

#include <QDebug>

/* ********************************************************************************************* *
 * Implementation of Channel
 * ********************************************************************************************* */
Channel::Channel(const QString &name, double rx, double tx, Power power, uint txTimeout,
                 bool rxOnly, ScanList *scanlist, QObject *parent)
  : ConfigObject("ch", parent), _name(name), _rxFreq(rx), _txFreq(tx), _power(power), _txTimeOut(txTimeout),
    _rxOnly(rxOnly), _scanlist()
{
  // Set reference to scan list
  _scanlist.set(scanlist);
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

Channel::Power
Channel::power() const {
  return _power;
}
void
Channel::setPower(Power power) {
  _power = power;
  emit modified(this);
}

uint
Channel::timeout() const {
  return _txTimeOut;
}
bool
Channel::setTimeout(uint dur) {
  _txTimeOut = dur;
  emit modified(this);
  return true;
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


/* ********************************************************************************************* *
 * Implementation of AnalogChannel
 * ********************************************************************************************* */
AnalogChannel::AnalogChannel(const QString &name, double rxFreq, double txFreq, Power power,
                             uint txTimeout, bool rxOnly, Admit admit, uint squelch,
                             Signaling::Code rxTone, Signaling::Code txTone, Bandwidth bw,
                             ScanList *list, APRSSystem *aprsSys, QObject *parent)
  : Channel(name, rxFreq, txFreq, power, txTimeout, rxOnly, list, parent),
    _admit(admit), _squelch(squelch), _rxTone(rxTone), _txTone(txTone), _bw(bw), _aprsSystem()
{
  // Reference APRS system
  _aprsSystem.set(aprsSys);
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

uint
AnalogChannel::squelch() const {
  return _squelch;
}
bool
AnalogChannel::setSquelch(uint val) {
  _squelch = val;
  emit modified(this);
  return true;
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

  return true;
}


/* ********************************************************************************************* *
 * Implementation of DigitalChannel
 * ********************************************************************************************* */
DigitalChannel::DigitalChannel(const QString &name, double rxFreq, double txFreq, Power power,
                               uint txto, bool rxOnly, Admit admit, uint colorCode,
                               TimeSlot timeslot, RXGroupList *rxGroup, DigitalContact *txContact,
                               PositioningSystem *aprs, ScanList *list, RoamingZone *roaming,
                               RadioID *radioID, QObject *parent)
  : Channel(name, rxFreq, txFreq, power, txto, rxOnly, list, parent), _admit(admit),
    _colorCode(colorCode), _timeSlot(timeslot),
    _rxGroup(), _txContact(), _posSystem(), _roaming(), _radioId()
{
  // Register default tags
  if (! ConfigObject::Context::hasTag(metaObject()->className(), "roaming", "!default"))
    ConfigObject::Context::setTag(metaObject()->className(), "roaming", "!default", DefaultRoamingZone::get());
  if (! ConfigObject::Context::hasTag(metaObject()->className(), "radioID", "!default"))
    ConfigObject::Context::setTag(metaObject()->className(), "radioID", "!default", DefaultRadioID::get());

  // Set references
  _rxGroup.set(rxGroup);
  _txContact.set(txContact);
  _posSystem.set(aprs);
  _roaming.set(roaming);
  _radioId.set(radioID);

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

uint
DigitalChannel::colorCode() const {
  return _colorCode;
}
bool
DigitalChannel::setColorCode(uint cc) {
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
DigitalChannel::aprsObj(PositioningSystem *sys) {
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
  : Channel("[Selected]", 0, 0, Channel::Power::Low, 0, true, nullptr, nullptr)
{
  // pass...
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
ChannelList::findDigitalChannel(double rx, double tx, DigitalChannel::TimeSlot ts, uint cc) const {
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
    if (1e-6 > std::abs(channel(i)->txFrequency()-freq))
      return channel(i)->as<AnalogChannel>();
  }
  return nullptr;
}

