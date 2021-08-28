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
    _rxOnly(rxOnly), _scanlist(scanlist)
{
  // pass..
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
  emit modified();
  return true;
}

double Channel::rxFrequency() const {
  return _rxFreq;
}
bool
Channel::setRXFrequency(double freq) {
  _rxFreq = freq;
  emit modified();
  return true;
}

double
Channel::txFrequency() const {
  return _txFreq;
}
bool
Channel::setTXFrequency(double freq) {
  _txFreq = freq;
  emit modified();
  return true;
}

Channel::Power
Channel::power() const {
  return _power;
}
void
Channel::setPower(Power power) {
  _power = power;
  emit modified();
}

uint
Channel::txTimeout() const {
  return _txTimeOut;
}
bool
Channel::setTimeout(uint dur) {
  _txTimeOut = dur;
  emit modified();
  return true;
}

bool
Channel::rxOnly() const {
  return _rxOnly;
}
bool
Channel::setRXOnly(bool enable) {
  _rxOnly = enable;
  emit modified();
  return true;
}

ScanList *
Channel::scanList() const {
  return _scanlist;
}
bool
Channel::setScanList(ScanList *list) {
  if (nullptr == list)
    return false;
  _scanlist = list;
  if (_scanlist)
    connect(_scanlist, SIGNAL(destroyed(QObject *)), this, SLOT(onScanListDeleted(QObject *)));
  emit modified();
  return true;
}

void
Channel::onScanListDeleted(QObject *obj) {
  ScanList *scanlist = reinterpret_cast<ScanList *>(obj);
  if (_scanlist == scanlist)
    _scanlist = nullptr;
}


/* ********************************************************************************************* *
 * Implementation of AnalogChannel
 * ********************************************************************************************* */
AnalogChannel::AnalogChannel(const QString &name, double rxFreq, double txFreq, Power power,
                             uint txTimeout, bool rxOnly, Admit admit, uint squelch,
                             Signaling::Code rxTone, Signaling::Code txTone, Bandwidth bw,
                             ScanList *list, APRSSystem *aprsSys, QObject *parent)
  : Channel(name, rxFreq, txFreq, power, txTimeout, rxOnly, list, parent),
    _admit(admit), _squelch(squelch), _rxTone(rxTone), _txTone(txTone), _bw(bw), _aprsSystem(aprsSys)
{
  // pass...
}

AnalogChannel::Admit
AnalogChannel::admit() const {
  return _admit;
}
void
AnalogChannel::setAdmit(Admit admit) {
  _admit = admit;
  emit modified();
}

uint
AnalogChannel::squelch() const {
  return _squelch;
}
bool
AnalogChannel::setSquelch(uint val) {
  _squelch = val;
  emit modified();
  return true;
}

Signaling::Code
AnalogChannel::rxTone() const {
  return _rxTone;
}
bool
AnalogChannel::setRXTone(Signaling::Code code) {
  _rxTone = code;
  emit modified();
  return true;
}

Signaling::Code
AnalogChannel::txTone() const {
  return _txTone;
}
bool
AnalogChannel::setTXTone(Signaling::Code code) {
  _txTone = code;
  emit modified();
  return true;
}

AnalogChannel::Bandwidth
AnalogChannel::bandwidth() const {
  return _bw;
}
bool
AnalogChannel::setBandwidth(Bandwidth bw) {
  _bw = bw;
  emit modified();
  return true;
}

APRSSystem *
AnalogChannel::aprsSystem() const {
  return _aprsSystem;
}
void
AnalogChannel::setAPRSSystem(APRSSystem *sys) {
  if (_aprsSystem)
    disconnect(_aprsSystem, SIGNAL(destroyed(QObject*)), this, SLOT(onAPRSSystemDeleted()));
  _aprsSystem = sys;
  if (_aprsSystem)
    connect(_aprsSystem, SIGNAL(destroyed(QObject*)), this, SLOT(onAPRSSystemDeleted()));
}

void
AnalogChannel::onAPRSSystemDeleted() {
  _aprsSystem = nullptr;
}


/* ********************************************************************************************* *
 * Implementation of DigitalChannel
 * ********************************************************************************************* */
DigitalChannel::DigitalChannel(const QString &name, double rxFreq, double txFreq, Power power,
                               uint txto, bool rxOnly, Admit admit, uint colorCode,
                               TimeSlot timeslot, RXGroupList *rxGroup, DigitalContact *txContact,
                               PositioningSystem *posSystem, ScanList *list, RoamingZone *roaming, RadioID *radioID, QObject *parent)
  : Channel(name, rxFreq, txFreq, power, txto, rxOnly, list, parent), _admit(admit),
    _colorCode(colorCode), _timeSlot(timeslot), _rxGroup(rxGroup), _txContact(txContact),
    _posSystem(posSystem), _roaming(roaming), _radioId(radioID)
{
  if (_rxGroup)
    connect(_rxGroup, SIGNAL(destroyed()), this, SLOT(onRxGroupDeleted()));
  if (_txContact)
    connect(_txContact, SIGNAL(destroyed()), this, SLOT(onTxContactDeleted()));
  if (_posSystem)
    connect(_posSystem, SIGNAL(destroyed()), this, SLOT(onPosSystemDeleted()));
  if (_roaming)
    connect(_roaming, SIGNAL(destroyed()), this, SLOT(onRoamingZoneDeleted()));
  if (_radioId)
    connect(_radioId, SIGNAL(destroyed(QObject*)), this, SLOT(onRadioIdDeleted()));
}

DigitalChannel::Admit
DigitalChannel::admit() const {
  return _admit;
}
void
DigitalChannel::setAdmit(Admit admit) {
  _admit = admit;
  emit modified();
}

uint
DigitalChannel::colorCode() const {
  return _colorCode;
}
bool
DigitalChannel::setColorCode(uint cc) {
  _colorCode = cc;
  emit modified();
  return true;
}

DigitalChannel::TimeSlot
DigitalChannel::timeslot() const {
  return _timeSlot;
}
bool
DigitalChannel::setTimeSlot(TimeSlot slot) {
  _timeSlot = slot;
  emit modified();
  return true;
}

RXGroupList *
DigitalChannel::rxGroupList() const {
  return _rxGroup;
}

bool
DigitalChannel::setRXGroupList(RXGroupList *g) {
  if (_rxGroup)
    disconnect(_rxGroup, SIGNAL(destroyed()), this, SLOT(onRxGroupDeleted()));
  _rxGroup = g;
  if (_rxGroup)
    connect(_rxGroup, SIGNAL(destroyed()), this, SLOT(onRxGroupDeleted()));
  emit modified();
  return true;
}

DigitalContact *
DigitalChannel::txContact() const {
  return _txContact;
}

bool
DigitalChannel::setTXContact(DigitalContact *c) {
  if (_txContact)
    disconnect(_txContact, SIGNAL(destroyed()), this, SLOT(onTxContactDeleted()));
  _txContact = c;
  if (_txContact)
    connect(_txContact, SIGNAL(destroyed()), this, SLOT(onTxContactDeleted()));
  emit modified();
  return true;
}

PositioningSystem *
DigitalChannel::posSystem() const {
  return _posSystem;
}

bool
DigitalChannel::setPosSystem(PositioningSystem *sys) {
  if (_posSystem)
    disconnect(_posSystem, SIGNAL(destroyed()), this, SLOT(onPosSystemDeleted()));
  _posSystem = sys;
  if (_posSystem)
    connect(_posSystem, SIGNAL(destroyed()), this, SLOT(onPosSystemDeleted()));
  emit modified();
  return true;
}


RoamingZone *
DigitalChannel::roaming() const {
  return _roaming;
}

bool
DigitalChannel::setRoaming(RoamingZone *zone) {
  if (_roaming)
    disconnect(_roaming, SIGNAL(destroyed(QObject*)), this, SLOT(onRoamingZoneDeleted()));
  _roaming = zone;
  if (_roaming)
    connect(_roaming, SIGNAL(destroyed(QObject*)), this, SLOT(onRoamingZoneDeleted()));
  emit modified();
  return true;
}

RadioID *
DigitalChannel::radioId() const {
  return _radioId;
}

bool
DigitalChannel::setRadioId(RadioID *id) {
  if (_radioId)
    disconnect(_radioId, SIGNAL(destroyed(QObject*)), this, SLOT(onRadioIdDeleted()));
  _radioId = id;
  if (_radioId)
    connect(_radioId, SIGNAL(destroyed(QObject*)), this, SLOT(onRadioIdDeleted()));
  emit modified();
  return true;
}


void
DigitalChannel::onRxGroupDeleted() {
  setRXGroupList(nullptr);
}

void
DigitalChannel::onTxContactDeleted() {
  setTXContact(nullptr);
}

void
DigitalChannel::onPosSystemDeleted() {
  setPosSystem(nullptr);
}

void
DigitalChannel::onRoamingZoneDeleted() {
  setRoaming(nullptr);
}

void
DigitalChannel::onRadioIdDeleted() {
  setRadioId(nullptr);
}

/* ********************************************************************************************* *
 * Implementation of SelectedChannel
 * ********************************************************************************************* */
SelectedChannel *SelectedChannel::_instance = nullptr;

SelectedChannel::SelectedChannel()
  : Channel("[Selected]", 0, 0, Channel::LowPower, 0, true, nullptr, nullptr)
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
  : ConfigObjectList(parent)
{

}

bool
ChannelList::add(ConfigObject *obj, int row) {
  if ((nullptr == obj) || (obj->is<Channel>()))
    return false;
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
    if (digi->timeslot() != ts)
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


