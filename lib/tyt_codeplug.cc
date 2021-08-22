#include "tyt_codeplug.hh"
#include "codeplugcontext.hh"
#include "config.hh"
#include "utils.hh"
#include "channel.hh"
#include "gpssystem.hh"
#include "config.h"
#include "logger.hh"
#include <QTimeZone>
#include <QtEndian>


/* ******************************************************************************************** *
 * Implementation of TyTCodeplug::ChannelElement
 * ******************************************************************************************** */
TyTCodeplug::ChannelElement::ChannelElement(uint8_t *ptr)
  : CodePlug::Element(ptr)
{
  // pass...
}

TyTCodeplug::ChannelElement::~ChannelElement() {
  // pass...
}

bool
TyTCodeplug::ChannelElement::isValid() const {
  return Element::isValid() && (0x0000 != getUInt16_be(0x20)) && (0xffff != getUInt16_be(0x20));
}

void
TyTCodeplug::ChannelElement::clear() {
  Element::clear();

  mode(MODE_ANALOG);
  bandwidth(AnalogChannel::BWNarrow);
  autoScan(0);
  setBit(0, 1); setBit(0,2);
  loneWorker(false);
  talkaround(false);
  rxOnly(false);
  timeSlot(DigitalChannel::TimeSlot1);
  colorCode(1);
  privacyIndex(0);
  privacyType(PRIV_NONE);
  privateCallConfirm(false);
  dataCallConfirm(false);
  rxRefFrequency(REF_LOW);
  clearBit(3,2);
  emergencyAlarmACK(false);
  clearBit(3,4); setBit(3,5); setBit(3,6);
  displayPTTId(true);
  txRefFrequency(REF_LOW);
  setBit(4,2); clearBit(4,3);
  vox(false);
  setBit(4,5);
  admitCriterion(ADMIT_ALWAYS);
  clearBit(5,0);
  inCallCriteria(INCALL_ALWAYS);
  turnOffFreq(TURNOFF_NONE);
  contactIndex(0);
  txTimeOut(0);
  clearBit(8,6);
  txTimeOutRekeyDelay(0);
  emergencySystemIndex(0);
  scanListIndex(0);
  groupListIndex(0);
  positioningSystemIndex(0);
  for (uint8_t i=0; i<8; i++)
    dtmfDecode(i, false);
  squelch(1);
  rxFrequency(400000000UL);
  txFrequency(400000000UL);
  rxSignaling(Signaling::SIGNALING_NONE);
  txSignaling(Signaling::SIGNALING_NONE);
  rxSignalingSystemIndex(0);
  txSignalingSystemIndex(0);
  power(Channel::HighPower);
  setBit(30,2); setBit(30,3); setBit(30,4); setBit(30,5); setBit(30,6); setBit(30,7);
  txGPSInfo(true);
  rxGPSInfo(true);
  allowInterrupt(true);
  dualCapacityDirectMode(false);
  leaderOrMS(true);
  setBit(31, 5); setBit(31, 6); setBit(31, 7);
  memset((_data+32), 0x00, sizeof(32));
}


TyTCodeplug::ChannelElement::Mode
TyTCodeplug::ChannelElement::mode() const {
  return TyTCodeplug::ChannelElement::Mode(getUInt2(0, 0));
}
void
TyTCodeplug::ChannelElement::mode(Mode mode) {
  setUInt2(0,0, uint8_t(mode));
}

AnalogChannel::Bandwidth
TyTCodeplug::ChannelElement::bandwidth() const {
  if (0 == getUInt2(0, 2))
    return AnalogChannel::BWNarrow;
  return AnalogChannel::BWWide;
}
void
TyTCodeplug::ChannelElement::bandwidth(AnalogChannel::Bandwidth bw) {
  if (AnalogChannel::BWNarrow == bw)
    setUInt2(0, 2, BW_12_5_KHZ);
  else
    setUInt2(0, 2, BW_25_KHZ);
}

bool
TyTCodeplug::ChannelElement::autoScan() const {
  return getBit(0, 4);
}
void
TyTCodeplug::ChannelElement::autoScan(bool enable) {
  setBit(0, 4, enable);
}

bool
TyTCodeplug::ChannelElement::loneWorker() const {
  return getBit(0, 7);
}
void
TyTCodeplug::ChannelElement::loneWorker(bool enable) {
  setBit(0, 7, enable);
}

bool
TyTCodeplug::ChannelElement::talkaround() const {
  return getBit(1, 0);
}
void
TyTCodeplug::ChannelElement::talkaround(bool enable) {
  setBit(1, 0, enable);
}

bool
TyTCodeplug::ChannelElement::rxOnly() const {
  return getBit(1, 1);
}
void
TyTCodeplug::ChannelElement::rxOnly(bool enable) {
  setBit(1, 1, enable);
}

DigitalChannel::TimeSlot
TyTCodeplug::ChannelElement::timeSlot() const {
  if (2 == getUInt2(1,2))
    return DigitalChannel::TimeSlot2;
  return DigitalChannel::TimeSlot2;
}
void
TyTCodeplug::ChannelElement::timeSlot(DigitalChannel::TimeSlot ts) {
  if (DigitalChannel::TimeSlot1 == ts)
    setUInt2(1,2,1);
  else
    setUInt2(1,2,2);
}

uint8_t
TyTCodeplug::ChannelElement::colorCode() const {
  return getUInt4(1,4);
}
void
TyTCodeplug::ChannelElement::colorCode(uint8_t cc) {
  setUInt4(1,4, std::min(uint8_t(16), cc));
}

uint8_t
TyTCodeplug::ChannelElement::privacyIndex() const {
  return getUInt4(2,0);
}
void
TyTCodeplug::ChannelElement::privacyIndex(uint8_t idx) {
  setUInt4(2,0, idx);
}

TyTCodeplug::ChannelElement::PrivacyType
TyTCodeplug::ChannelElement::privacyType() const {
  return TyTCodeplug::ChannelElement::PrivacyType(getUInt2(2,4));
}
void
TyTCodeplug::ChannelElement::privacyType(TyTCodeplug::ChannelElement::PrivacyType type) {
  setUInt2(2,4, uint8_t(type));
}

bool
TyTCodeplug::ChannelElement::privateCallConfirm() const {
  return getBit(2, 6);
}
void
TyTCodeplug::ChannelElement::privateCallConfirm(bool enable) {
  setBit(2, 6, enable);
}

bool
TyTCodeplug::ChannelElement::dataCallConfirm() const {
  return getBit(2, 7);
}
void
TyTCodeplug::ChannelElement::dataCallConfirm(bool enable) {
  setBit(2, 7, enable);
}

TyTCodeplug::ChannelElement::RefFrequency
TyTCodeplug::ChannelElement::rxRefFrequency() const {
  return TyTCodeplug::ChannelElement::RefFrequency(getUInt2(3,0));
}
void
TyTCodeplug::ChannelElement::rxRefFrequency(TyTCodeplug::ChannelElement::RefFrequency ref) {
  setUInt2(3,0, uint8_t(ref));
}

bool
TyTCodeplug::ChannelElement::emergencyAlarmACK() const {
  return getBit(3,3);
}
void
TyTCodeplug::ChannelElement::emergencyAlarmACK(bool enable) {
  setBit(3,3, enable);
}

bool
TyTCodeplug::ChannelElement::displayPTTId() const {
  return getBit(3,7);
}
void
TyTCodeplug::ChannelElement::displayPTTId(bool enable) {
  setBit(3,7, enable);
}

TyTCodeplug::ChannelElement::RefFrequency
TyTCodeplug::ChannelElement::txRefFrequency() const {
  return TyTCodeplug::ChannelElement::RefFrequency(getUInt2(4,0));
}
void
TyTCodeplug::ChannelElement::txRefFrequency(TyTCodeplug::ChannelElement::RefFrequency ref) {
  setUInt2(4,0, uint8_t(ref));
}

bool
TyTCodeplug::ChannelElement::vox() const {
  return getBit(4,4);
}
void
TyTCodeplug::ChannelElement::vox(bool enable) {
  setBit(4,4, enable);
}

TyTCodeplug::ChannelElement::Admit
TyTCodeplug::ChannelElement::admitCriterion() const {
  return TyTCodeplug::ChannelElement::Admit(getUInt2(4,6));
}
void
TyTCodeplug::ChannelElement::admitCriterion(TyTCodeplug::ChannelElement::Admit admit) {
  setUInt2(4,6, uint8_t(admit));
}

TyTCodeplug::ChannelElement::InCall
TyTCodeplug::ChannelElement::inCallCriteria() const {
  return InCall(getUInt2(5,4));
}
void
TyTCodeplug::ChannelElement::inCallCriteria(InCall crit) {
  setUInt2(5,4, uint8_t(crit));
}

TyTCodeplug::ChannelElement::TurnOffFreq
TyTCodeplug::ChannelElement::turnOffFreq() const {
  return TurnOffFreq(getUInt2(5,6));
}
void
TyTCodeplug::ChannelElement::turnOffFreq(TurnOffFreq freq) {
  setUInt2(5,6, uint8_t(freq));
}

uint16_t
TyTCodeplug::ChannelElement::contactIndex() const {
  return getUInt16_le(6);
}
void
TyTCodeplug::ChannelElement::contactIndex(uint16_t idx) {
  setUInt16_le(6, idx);
}

uint TyTCodeplug::ChannelElement::txTimeOut() const {
  return getUInt6(8, 0)*15;
}
void
TyTCodeplug::ChannelElement::txTimeOut(uint tot) {
  return setUInt6(8, 0, tot/15);
}

uint8_t
TyTCodeplug::ChannelElement::txTimeOutRekeyDelay() const {
  return getUInt8(9);
}
void
TyTCodeplug::ChannelElement::txTimeOutRekeyDelay(uint8_t delay) {
  return setUInt8(9, delay);
}

uint8_t
TyTCodeplug::ChannelElement::emergencySystemIndex() const {
  return getUInt8(10);
}
void
TyTCodeplug::ChannelElement::emergencySystemIndex(uint8_t delay) {
  return setUInt8(10, delay);
}

uint8_t
TyTCodeplug::ChannelElement::scanListIndex() const {
  return getUInt8(11);
}
void
TyTCodeplug::ChannelElement::scanListIndex(uint8_t idx) {
  return setUInt8(11, idx);
}

uint8_t
TyTCodeplug::ChannelElement::groupListIndex() const {
  return getUInt8(12);
}
void
TyTCodeplug::ChannelElement::groupListIndex(uint8_t idx) {
  return setUInt8(12, idx);
}

uint8_t
TyTCodeplug::ChannelElement::positioningSystemIndex() const {
  return getUInt8(13);
}
void
TyTCodeplug::ChannelElement::positioningSystemIndex(uint8_t idx) {
  return setUInt8(13, idx);
}

bool
TyTCodeplug::ChannelElement::dtmfDecode(uint8_t idx) const {
  return getBit(14, idx);
}
void
TyTCodeplug::ChannelElement::dtmfDecode(uint8_t idx, bool enable) {
  setBit(14, idx, enable);
}

uint TyTCodeplug::ChannelElement::squelch() const {
  return getUInt8(15);
}
void
TyTCodeplug::ChannelElement::squelch(uint value) {
  value = std::min(uint(10), value);
  return setUInt8(15, value);
}

uint32_t
TyTCodeplug::ChannelElement::rxFrequency() const {
  return getBCD8_le(16)*10;
}
void
TyTCodeplug::ChannelElement::rxFrequency(uint32_t freq_Hz) {
  return setBCD8_le(16, freq_Hz/10);
}

uint32_t
TyTCodeplug::ChannelElement::txFrequency() const {
  return getBCD8_le(20)*10;
}
void
TyTCodeplug::ChannelElement::txFrequency(uint32_t freq_Hz) {
  return setBCD8_le(20, freq_Hz/10);
}

Signaling::Code
TyTCodeplug::ChannelElement::rxSignaling() const {
  return decode_ctcss_tone_table(getUInt16_le(24));
}
void
TyTCodeplug::ChannelElement::rxSignaling(Signaling::Code code) {
  setUInt16_le(24, encode_ctcss_tone_table(code));
}

Signaling::Code
TyTCodeplug::ChannelElement::txSignaling() const {
  return decode_ctcss_tone_table(getUInt16_le(26));
}
void
TyTCodeplug::ChannelElement::txSignaling(Signaling::Code code) {
  setUInt16_le(26, encode_ctcss_tone_table(code));
}

uint8_t
TyTCodeplug::ChannelElement::rxSignalingSystemIndex() const {
  return getUInt8(28);
}
void
TyTCodeplug::ChannelElement::rxSignalingSystemIndex(uint8_t idx) {
  setUInt8(28, idx);
}

uint8_t
TyTCodeplug::ChannelElement::txSignalingSystemIndex() const {
  return getUInt8(29);
}
void
TyTCodeplug::ChannelElement::txSignalingSystemIndex(uint8_t idx) {
  setUInt8(29, idx);
}

Channel::Power
TyTCodeplug::ChannelElement::power() const {
  switch (getUInt2(30, 0)) {
  case 0: return Channel::LowPower;
  case 2: return Channel::MidPower;
  case 3: return Channel::HighPower;
  default: break;
  }
  return Channel::LowPower;
}
void
TyTCodeplug::ChannelElement::power(Channel::Power pwr) {
  switch (pwr) {
  case Channel::MinPower:
  case Channel::LowPower:
    setUInt2(30,0, 0);
    break;
  case Channel::MidPower:
    setUInt2(30,0, 2);
    break;
  case Channel::HighPower:
  case Channel::MaxPower:
    setUInt2(30,0, 3);
  }
}

bool
TyTCodeplug::ChannelElement::txGPSInfo() const {
  return ! getBit(31, 0);
}
void
TyTCodeplug::ChannelElement::txGPSInfo(bool enable) {
  setBit(31,0, !enable);
}

bool
TyTCodeplug::ChannelElement::rxGPSInfo() const {
  return !getBit(31, 1);
}
void
TyTCodeplug::ChannelElement::rxGPSInfo(bool enable) {
  setBit(31,1, !enable);
}
bool
TyTCodeplug::ChannelElement::allowInterrupt() const {
  return !getBit(31, 2);
}
void
TyTCodeplug::ChannelElement::allowInterrupt(bool enable) {
  setBit(31,2, !enable);
}
bool
TyTCodeplug::ChannelElement::dualCapacityDirectMode() const {
  return !getBit(31, 3);
}
void
TyTCodeplug::ChannelElement::dualCapacityDirectMode(bool enable) {
  setBit(31,3, !enable);
}
bool
TyTCodeplug::ChannelElement::leaderOrMS() const {
  return !getBit(31, 4);
}
void
TyTCodeplug::ChannelElement::leaderOrMS(bool enable) {
  setBit(31,4, !enable);
}

QString
TyTCodeplug::ChannelElement::name() const {
  return readUnicode(32, 16, 0x0000);
}
void
TyTCodeplug::ChannelElement::name(const QString &name) {
  return writeUnicode(32, name, 16, 0x0000);
}

Channel *
TyTCodeplug::ChannelElement::toChannelObj() const {
  if (! isValid())
    return nullptr;

  // decode power setting
  if (MODE_ANALOG == mode()) {
    AnalogChannel::Admit admit_crit;
    switch(admitCriterion()) {
      case ADMIT_ALWAYS: admit_crit = AnalogChannel::AdmitNone; break;
      case ADMIT_TONE: admit_crit = AnalogChannel::AdmitTone; break;
      case ADMIT_CH_FREE: admit_crit = AnalogChannel::AdmitFree; break;
      default: admit_crit = AnalogChannel::AdmitFree; break;
    }

    return new AnalogChannel(name(), double(rxFrequency())/1e6, double(txFrequency())/1e6,
                             power(), txTimeOut(), rxOnly(), admit_crit, squelch(), rxSignaling(),
                             txSignaling(), bandwidth(), nullptr);
  } else if (MODE_DIGITAL == mode()) {
    DigitalChannel::Admit admit_crit;
    switch(admitCriterion()) {
      case ADMIT_ALWAYS: admit_crit = DigitalChannel::AdmitNone; break;
      case ADMIT_CH_FREE: admit_crit = DigitalChannel::AdmitFree; break;
      case ADMIT_COLOR: admit_crit = DigitalChannel::AdmitColorCode; break;
      default: admit_crit = DigitalChannel::AdmitFree; break;
    }

    return new DigitalChannel(name(), double(rxFrequency())/1e6, double(txFrequency())/1e6,
                              power(), txTimeOut(), rxOnly(), admit_crit, colorCode(), timeSlot(),
                              nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
  }

  return nullptr;
}

bool
TyTCodeplug::ChannelElement::linkChannelObj(Channel *c, const CodeplugContext &ctx) const
{
  if (! isValid())
    return false;

  if (scanListIndex() && ctx.hasScanList(scanListIndex())) {
    c->setScanList(ctx.getScanList(scanListIndex()));
  }

  if (MODE_ANALOG == mode()) {
    return true;
  } else if ((MODE_DIGITAL == mode()) && (c->is<DigitalChannel>())){
    DigitalChannel *dc = c->as<DigitalChannel>();
    if (contactIndex() && ctx.hasDigitalContact(contactIndex())) {
      dc->setTXContact(ctx.getDigitalContact(contactIndex()));
    }
    if (groupListIndex() && ctx.hasGroupList(groupListIndex())) {
      dc->setRXGroupList(ctx.getGroupList(groupListIndex()));
    }
    if (positioningSystemIndex() && ctx.hasGPSSystem(positioningSystemIndex())) {
      dc->setPosSystem(ctx.getGPSSystem(positioningSystemIndex()));
    }
    return true;
  }

  return false;
}

void
TyTCodeplug::ChannelElement::fromChannelObj(const Channel *chan, const CodeplugContext &ctx) {
  name(chan->name());
  rxFrequency(chan->rxFrequency()*1e6);
  txFrequency(chan->txFrequency()*1e6);
  rxOnly(chan->rxOnly());
  txTimeOut(chan->txTimeout());
  if (chan->scanList())
    scanListIndex(ctx.config()->scanlists()->indexOf(chan->scanList())+1);
  else
    scanListIndex(0);

  // encode power setting
  power(chan->power());

  if (chan->is<const DigitalChannel>()) {
    const DigitalChannel *dchan = chan->as<const DigitalChannel>();
    mode(MODE_DIGITAL);
    switch (dchan->admit()) {
    case DigitalChannel::AdmitNone: admitCriterion(ADMIT_ALWAYS); break;
    case DigitalChannel::AdmitFree: admitCriterion(ADMIT_CH_FREE); break;
    case DigitalChannel::AdmitColorCode: admitCriterion(ADMIT_COLOR); break;
    }
    colorCode(dchan->colorCode());
    timeSlot(dchan->timeslot());
    if (dchan->rxGroupList())
      groupListIndex(ctx.config()->rxGroupLists()->indexOf(dchan->rxGroupList())+1);
    else
      groupListIndex(0);
    if (dchan->txContact())
      contactIndex(ctx.config()->contacts()->indexOfDigital(dchan->txContact())+1);
    squelch(0);
    bandwidth(AnalogChannel::BWNarrow);
    rxSignaling(Signaling::SIGNALING_NONE);
    txSignaling(Signaling::SIGNALING_NONE);
    if (dchan->posSystem() && dchan->posSystem()->is<GPSSystem>()) {
      positioningSystemIndex(ctx.config()->posSystems()->indexOfGPSSys(dchan->posSystem()->as<GPSSystem>())+1);
      txGPSInfo(true); rxGPSInfo(false);
    }
  } else if (chan->is<AnalogChannel>()) {
    const AnalogChannel *achan = chan->as<const AnalogChannel>();
    mode(MODE_ANALOG);
    bandwidth(achan->bandwidth());
    squelch(achan->squelch());
    switch (achan->admit()) {
    case AnalogChannel::AdmitNone: admitCriterion(ADMIT_ALWAYS); break;
    case AnalogChannel::AdmitFree: admitCriterion(ADMIT_CH_FREE); break;
    case AnalogChannel::AdmitTone: admitCriterion(ADMIT_TONE); break;
    }
    rxSignaling(achan->rxTone());
    txSignaling(achan->txTone());
    groupListIndex(0);
    contactIndex(0);
  }
}


/* ******************************************************************************************** *
 * Implementation of TyTCodeplug::VFOChannelElement
 * ******************************************************************************************** */
TyTCodeplug::VFOChannelElement::VFOChannelElement(uint8_t *ptr)
  : ChannelElement(ptr)
{
  // pass...
}

TyTCodeplug::VFOChannelElement::~VFOChannelElement() {
  // pass...
}

QString
TyTCodeplug::VFOChannelElement::name() const {
  return "";
}
void
TyTCodeplug::VFOChannelElement::name(const QString &txt) {
  // pass...
}

uint
TyTCodeplug::VFOChannelElement::stepSize() const {
  return (getUInt8(32)+1)*2500;
}
void
TyTCodeplug::VFOChannelElement::stepSize(uint ss_Hz) {
  ss_Hz = std::min(50000U, std::max(ss_Hz, 2500U));
  setUInt8(32, ss_Hz/2500-1);
  setUInt8(33, 0xff);
}


/* ******************************************************************************************** *
 * Implementation of TyTCodeplug::ContactElement
 * ******************************************************************************************** */
TyTCodeplug::ContactElement::ContactElement(uint8_t *ptr, uint size)
  : CodePlug::Element(ptr)
{
  // pass...
}

TyTCodeplug::ContactElement::~ContactElement() {
  // pass...
}

bool
TyTCodeplug::ContactElement::isValid() const {
  return Element::isValid() && (0 != getUInt2(3, 0))
      && (0x0000 != getUInt16_be(4)) && (0xffff != getUInt16_be(4));
}

void
TyTCodeplug::ContactElement::clear() {
  memset(_data, 0xff, 3); // clear DMR ID
  setUInt2(3, 0, 0);      // type=0
  setBit(3,2, 0); setBit(3,3, 0); setBit(3,4, 0); // unused = 0
  ringTone(false);
  setBit(3,6, 1); setBit(3,7, 1); // unknown = 1
  memset(_data+0x04, 0x00, 2*16);
}

uint32_t
TyTCodeplug::ContactElement::dmrId() const {
  return getUInt24_le(0);
}

void
TyTCodeplug::ContactElement::dmrId(uint32_t id) {
  setUInt24_le(0, id);
}

bool
TyTCodeplug::ContactElement::ringTone() const {
  return getBit(3, 5);
}
void
TyTCodeplug::ContactElement::ringTone(bool enable) {
  setBit(3, 5, enable);
}

DigitalContact::Type TyTCodeplug::ContactElement::callType() const {
  switch(getUInt2(3,0)) {
  case 1: return DigitalContact::GroupCall;
  case 2: return DigitalContact::PrivateCall;
  case 3: return DigitalContact::AllCall;
  default:
    break;
  }
  return DigitalContact::PrivateCall;
}
void
TyTCodeplug::ContactElement::callType(DigitalContact::Type type) {
  switch (type) {
  case DigitalContact::GroupCall:   setUInt2(3,0, 1); break;
  case DigitalContact::PrivateCall: setUInt2(3,0, 2); break;
  case DigitalContact::AllCall:     setUInt2(3,0, 3); break;
  }
}

QString
TyTCodeplug::ContactElement::name() const {
  return readUnicode(4, 16, 0x0000);
}
void
TyTCodeplug::ContactElement::name(const QString &nm) {
  writeUnicode(4, nm, 16, 0x0000);
}

DigitalContact *
TyTCodeplug::ContactElement::toContactObj() const {
  return new DigitalContact(callType(), name(), dmrId(), ringTone());
}

bool
TyTCodeplug::ContactElement::fromContactObj(const DigitalContact *cont) {
  if (nullptr == cont)
    return false;

  dmrId(cont->number());
  name(cont->name());
  callType(cont->type());
  ringTone(cont->rxTone());

  return true;
}


/* ******************************************************************************************** *
 * Implementation of TyTCodeplug::ZoneElement
 * ******************************************************************************************** */
TyTCodeplug::ZoneElement::ZoneElement(uint8_t *ptr, uint size)
  : CodePlug::Element(ptr)
{
  // pass...
}

TyTCodeplug::ZoneElement::~ZoneElement() {
  // pass...
}

bool
TyTCodeplug::ZoneElement::isValid() const {
  return Element::isValid() && (0x0000 != getUInt16_be(0)) && (0xffff != getUInt16_be(0));
}

void
TyTCodeplug::ZoneElement::clear() {
  memset(_data, 0x00, 0x40);
}

QString
TyTCodeplug::ZoneElement::name() const {
  return readUnicode(0, 16);
}
void
TyTCodeplug::ZoneElement::name(const QString &name) {
  writeUnicode(0, name, 16);
}

uint16_t
TyTCodeplug::ZoneElement::memberIndex(uint n) const {
  return getUInt16_le(0x20 + n*2);
}
void
TyTCodeplug::ZoneElement::memberIndex(uint n, uint16_t idx) {
  setUInt16_le(0x20 + n*2, idx);
}

bool
TyTCodeplug::ZoneElement::fromZoneObj(const Zone *zone, const CodeplugContext &ctx) {
  name(zone->name());
  for (int i=0; i<16; i++) {
    if (i < zone->A()->count())
      memberIndex(i, ctx.config()->channelList()->indexOf(zone->A()->channel(i))+1);
    else
      memberIndex(i, 0);
  }
  return true;
}

Zone *
TyTCodeplug::ZoneElement::toZoneObj() const {
  if (!isValid())
    return nullptr;
  return new Zone(name());
}

bool
TyTCodeplug::ZoneElement::linkZone(Zone *zone, const CodeplugContext &ctx) const {
  if (! isValid())
    return false;

  for (int i=0; ((i<16) && memberIndex(i)); i++) {
    if (! ctx.hasChannel(memberIndex(i))) {
      logWarn() << "Cannot link channel with index " << memberIndex(i) << " channel not defined.";
      continue;
    }
    zone->A()->addChannel(ctx.getChannel(memberIndex(i)));
  }

  return true;
}


/* ******************************************************************************************** *
 * Implementation of TyTCodeplug::ZoneElement
 * ******************************************************************************************** */
TyTCodeplug::ZoneExtElement::ZoneExtElement(uint8_t *ptr, uint size)
  : CodePlug::Element(ptr)
{
  // pass...
}

TyTCodeplug::ZoneExtElement::~ZoneExtElement() {
  // pass...
}

void
TyTCodeplug::ZoneExtElement::clear() {
  memset(_data, 0x00, 0xe0);
}

uint16_t
TyTCodeplug::ZoneExtElement::memberIndexA(uint n) const {
  return getUInt16_le(0x00 + 2*n);
}

void
TyTCodeplug::ZoneExtElement::memberIndexA(uint n, uint16_t idx) {
  setUInt16_le(0x00 + 2*n, idx);
}

uint16_t
TyTCodeplug::ZoneExtElement::memberIndexB(uint n) const {
  return getUInt16_le(0x60 + 2*n);
}

void
TyTCodeplug::ZoneExtElement::memberIndexB(uint n, uint16_t idx) {
  setUInt16_le(0x60 + 2*n, idx);
}

bool
TyTCodeplug::ZoneExtElement::fromZoneObj(const Zone *zone, const CodeplugContext &ctx) {
  // Store remaining channels from list A
  for (int i=16; i<64; i++) {
    if (i < zone->A()->count())
      memberIndexA(i-16, ctx.config()->channelList()->indexOf(zone->A()->channel(i))+1);
    else
      memberIndexA(i-16, 0);
  }
  // Store channel from list B
  for (int i=0; i<64; i++) {
    if (i < zone->B()->count())
      memberIndexB(i, ctx.config()->channelList()->indexOf(zone->B()->channel(i))+1);
    else
      memberIndexB(i, 0);
  }

  return true;
}

bool
TyTCodeplug::ZoneExtElement::linkZoneObj(Zone *zone, const CodeplugContext &ctx) {
  for (int i=0; (i<48) && memberIndexA(i); i++) {
    if (! ctx.hasChannel(memberIndexA(i))) {
      logWarn() << "Cannot link zone: Channel index " << memberIndexA(i) << " not defined.";
      return false;
    }
    zone->A()->addChannel(ctx.getChannel(memberIndexA(i)));
  }

  for (int i=0; (i<64) && memberIndexB(i); i++) {
    if (! ctx.hasChannel(memberIndexB(i))) {
      logWarn() << "Cannot link zone: Channel index " << memberIndexB(i) << " not defined.";
      return false;
    }
    zone->B()->addChannel(ctx.getChannel(memberIndexB(i)));
  }

  return true;
}


/* ******************************************************************************************** *
 * Implementation of TyTCodeplug::GroupListElement
 * ******************************************************************************************** */
TyTCodeplug::GroupListElement::GroupListElement(uint8_t *ptr, uint size)
  : CodePlug::Element(ptr)
{
  // pass...
}

TyTCodeplug::GroupListElement::~GroupListElement() {
  // pass...
}

bool
TyTCodeplug::GroupListElement::isValid() const {
  return Element::isValid() && (0x0000 != getUInt16_be(0));
}

void
TyTCodeplug::GroupListElement::clear() {
  memset(_data, 0x00, 0x60);
}

QString
TyTCodeplug::GroupListElement::name() const {
  return readUnicode(0, 16);
}

void
TyTCodeplug::GroupListElement::name(const QString &nm) {
  writeUnicode(0, nm, 16);
}

uint16_t
TyTCodeplug::GroupListElement::memberIndex(uint n) const {
  return getUInt16_le(0x20 + n*2);
}

void
TyTCodeplug::GroupListElement::memberIndex(uint n, uint16_t idx) {
  setUInt16_le(0x20 + 2*n, idx);
}

bool
TyTCodeplug::GroupListElement::fromGroupListObj(const RXGroupList *lst, const CodeplugContext &ctx) {
  name(lst->name());
  for (int i=0; i<32; i++) {
    if (i<lst->count())
      memberIndex(i, ctx.config()->contacts()->indexOfDigital(lst->contact(i)) + 1);
    else
      memberIndex(i, 0);
  }
  return true;
}

RXGroupList *
TyTCodeplug::GroupListElement::toGroupListObj(const CodeplugContext &ctx) {
  if (! isValid())
    return nullptr;
  return new RXGroupList(name());
}

bool
TyTCodeplug::GroupListElement::linkGroupListObj(RXGroupList *lst, const CodeplugContext &ctx) {
  if (! isValid())
    return false;

  for (int i=0; (i<32) && memberIndex(i); i++) {
    if (! ctx.hasDigitalContact(memberIndex(i))) {
      logWarn() << "Cannot link contact " << memberIndex(i) << " to group list '"
                << name() << "': Invalid contact index. Ignored.";
      continue;
    }
    lst->addContact(ctx.getDigitalContact(memberIndex(i)));
  }

  return true;
}


/* ******************************************************************************************** *
 * Implementation of TyTCodeplug::ScanListElement
 * ******************************************************************************************** */
TyTCodeplug::ScanListElement::ScanListElement(uint8_t *ptr, uint size)
  : Element(ptr)
{
  // pass...
}

TyTCodeplug::ScanListElement::~ScanListElement() {
  // pass...
}

bool
TyTCodeplug::ScanListElement::isValid() const {
  return Element::isValid() && (0x0000 != getUInt16_be(0));
}

void
TyTCodeplug::ScanListElement::clear() {
  memset(_data, 0, 2*16);
  priorityChannel1Index(0xffff);
  priorityChannel2Index(0xffff);
  txChannelIndex(0xffff);
  setUInt8(0x26, 0xf1);
  holdTime(500);
  prioritySampleTime(2000);
  setUInt8(0x29, 0xff);
  memset(_data+0x2a, 0, 2*31);
}

QString
TyTCodeplug::ScanListElement::name() const {
  return readUnicode(0, 16);
}

void
TyTCodeplug::ScanListElement::name(const QString &name) {
  writeUnicode(0, name, 16);
}

uint16_t
TyTCodeplug::ScanListElement::priorityChannel1Index() const {
  return getUInt16_le(0x20);
}

void
TyTCodeplug::ScanListElement::priorityChannel1Index(uint16_t idx) {
  setUInt16_le(0x20, idx);
}

uint16_t
TyTCodeplug::ScanListElement::priorityChannel2Index() const {
  return getUInt16_le(0x22);
}

void
TyTCodeplug::ScanListElement::priorityChannel2Index(uint16_t idx) {
  setUInt16_le(0x22, idx);
}

uint16_t
TyTCodeplug::ScanListElement::txChannelIndex() const {
  return getUInt16_le(0x24);
}

void
TyTCodeplug::ScanListElement::txChannelIndex(uint16_t idx) {
  setUInt16_le(0x24, idx);
}

uint
TyTCodeplug::ScanListElement::holdTime() const {
  return uint(getUInt8(0x27))*25;
}

void
TyTCodeplug::ScanListElement::holdTime(uint time) {
  setUInt8(0x27, time/25);
}

uint
TyTCodeplug::ScanListElement::prioritySampleTime() const {
  return uint(getUInt8(0x27))*250;
}

void
TyTCodeplug::ScanListElement::prioritySampleTime(uint time) {
  setUInt8(0x27, time/250);
}

uint16_t
TyTCodeplug::ScanListElement::memberIndex(uint n) const {
  return getUInt16_le(0x2a + 2*n);
}

void
TyTCodeplug::ScanListElement::memberIndex(uint n, uint16_t idx) {
  setUInt16_le(0x2a + 2*n, idx);
}

bool
TyTCodeplug::ScanListElement::fromScanListObj(const ScanList *lst, const CodeplugContext &ctx) {
  // Set name
  name(lst->name());

  // Set priority channel 1
  if (lst->priorityChannel() && (SelectedChannel::get() == lst->priorityChannel()))
    priorityChannel1Index(0);
  else if (lst->priorityChannel())
    priorityChannel1Index(ctx.config()->channelList()->indexOf(lst->priorityChannel())+1);

  // Set priority channel 2
  if (lst->secPriorityChannel() && (SelectedChannel::get() == lst->secPriorityChannel()))
    priorityChannel2Index(0);
  else if (lst->secPriorityChannel())
    priorityChannel2Index(ctx.config()->channelList()->indexOf(lst->secPriorityChannel())+1);

  // Set transmit channel
  if (lst->txChannel() && (SelectedChannel::get() == lst->txChannel()))
    txChannelIndex(0);
  else if (lst->txChannel())
    txChannelIndex(ctx.config()->channelList()->indexOf(lst->txChannel())+1);

  for (int i=0, j=0; i<31;) {
    if (j >= lst->count()) {
      memberIndex(i++, 0);
    } else if (SelectedChannel::get() == lst->channel(j)) {
      logInfo() << "Cannot encode '" << lst->channel(j) << "' for UV390: skip.";
      j++;
    } else {
      memberIndex(i++, ctx.config()->channelList()->indexOf(lst->channel(j++))+1);
    }
  }

  return true;
}

ScanList *
TyTCodeplug::ScanListElement::toScanListObj(const CodeplugContext &ctx) {
  if (! isValid())
    return nullptr;
  return new ScanList(name());
}

bool
TyTCodeplug::ScanListElement::linkScanListObj(ScanList *lst, const CodeplugContext &ctx) {
  if (! isValid()) {
    logDebug() << "Cannot link invalid scanlist.";
    return false;
  }

  if (0 == priorityChannel1Index())
    lst->setPriorityChannel(SelectedChannel::get());
  else if (ctx.hasChannel(priorityChannel1Index()))
    lst->setPriorityChannel(ctx.getChannel(priorityChannel1Index()));
  else if (0xffff == priorityChannel1Index())
    lst->setPriorityChannel(nullptr);
  else
    logWarn() << "Cannot deocde reference to priority channel index " << priorityChannel1Index()
                 << " in scan list '" << name() << "'.";

  if (0 == priorityChannel2Index())
    lst->setSecPriorityChannel(SelectedChannel::get());
  else if (ctx.hasChannel(priorityChannel2Index()))
    lst->setSecPriorityChannel(ctx.getChannel(priorityChannel2Index()));
  else if (0xffff == priorityChannel2Index())
    lst->setSecPriorityChannel(nullptr);
  else
    logWarn() << "Cannot deocde reference to secondary priority channel index " << priorityChannel2Index()
              << " in scan list '" << name() << "'.";

  if (0 == txChannelIndex())
    lst->setTXChannel(SelectedChannel::get());
  else if (ctx.hasChannel(txChannelIndex()))
    lst->setTXChannel(ctx.getChannel(txChannelIndex()));
  else
    logWarn() << "Cannot deocde reference to secondary priority channel index " << txChannelIndex()
                << " in scan list '" << name() << "'.";

  for (int i=0; ((i<31) && memberIndex(i)); i++) {
    if (! ctx.hasChannel(memberIndex(i))) {
      logDebug() << "Cannot link scanlist to channel idx " << memberIndex(i)
                    << ". Uknown channel index.";
      return false;
    }
    lst->addChannel(ctx.getChannel(memberIndex(i)));
  }

  return true;

}


/* ******************************************************************************************** *
 * Implementation of TyTCodeplug::GeneralSettingsElement
 * ******************************************************************************************** */
TyTCodeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr, uint size)
  : Element(ptr)
{
  // pass...
}

TyTCodeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr)
  : Element(ptr)
{
  // pass...
}

TyTCodeplug::GeneralSettingsElement::~GeneralSettingsElement() {
  // pass...
}

void
TyTCodeplug::GeneralSettingsElement::clear() {
  introLine1("");
  introLine2("");
  memset(_data+0x26, 0xff, 0x1a);

  setBit(0x40,0, 0); setBit(0,1, 1);
  disableAllLEDs(false);
  setBit(0x40,3, 1);
  monitorType(MONITOR_OPEN_SQUELCH);
  setBit(0x40,5, 1);
  transmitMode(DESIGNED_AND_HAND_CH);

  savePreamble(true);
  saveModeRX(true);
  disableAllTones(false);
  setBit(0x41,3, 1);
  chFreeIndicationTone(true);
  passwdAndLock(true);
  talkPermitDigital(false);
  talkPermitAnalog(false);

  setBit(0x42,0, 0);
  channelVoiceAnnounce(false);
  setBit(0x42,2, 0); setBit(0x42,3, 1);
  introPicture(true);
  keypadTones(false);
  setBit(0x42,6, 1); setBit(0x42,7, 1);

  setBit(0x43,0, 1); setBit(0x43,1, 1);
  channelModeA(true);
  setUInt4(0x43,3, 0xf);
  channelModeB(true);

  dmrID(0); setUInt8(0x47,0);

  txPreambleDuration(600);
  groupCallHangTime(3000);
  privateCallHangTime(4000);
  voxSesitivity(3);
  setUInt8(0x4c, 0x00); setUInt8(0x4d, 0x00);
  lowBatteryInterval(120);
  callAlertToneDuration(0);
  loneWorkerResponseTime(1);
  loneWorkerReminderTime(10);
  setUInt8(0x52, 0x00);
  scanDigitalHangTime(1000);
  scanAnalogHangTime(1000);

  backlightTime(10);
  setUInt6(0x55, 2, 0);
  keypadLockTimeSetManual();
  channelMode(true);

  powerOnPassword(0);
  radioProgPasswordDisable();
  pcProgPasswordDisable();
  setUInt24_le(0x68, 0xffffff);

  groupCallMatch(false);
  privateCallMatch(false);
  setBit(0x6b, 2, 1);
  timeZone(QTimeZone::systemTimeZone());
  setUInt32_le(0x6c, 0xffffffff);

  radioName("");

  channelHangTime(3000);
  setUInt8(0x91, 0xff); setUInt2(0x92, 0, 0x03);
  publicZone(true);
  setUInt5(0x92, 3, 0x1f);
  setUInt8(0x93, 0xff);
  additionalDMRId(0, 1); setUInt8(0x97, 0);
  additionalDMRId(1, 2); setUInt8(0x9b, 0);
  additionalDMRId(2, 3); setUInt8(0x9f, 0);
  setUInt3(0xa0, 0, 0b111);
  micLevel(2);
  editRadioID(true);
  setBit(0xa0, 7, true);
  memset(_data+0xa1, 0xff, 15);
}

QString
TyTCodeplug::GeneralSettingsElement::introLine1() const {
  return readUnicode(0, 10);
}
void
TyTCodeplug::GeneralSettingsElement::introLine1(const QString line) {
  writeUnicode(0, line, 10);
}

QString
TyTCodeplug::GeneralSettingsElement::introLine2() const {
  return readUnicode(0x14, 10);
}
void
TyTCodeplug::GeneralSettingsElement::introLine2(const QString line) {
  writeUnicode(0x14, line, 10);
}

TyTCodeplug::GeneralSettingsElement::TransmitMode
TyTCodeplug::GeneralSettingsElement::transmitMode() const {
  return TransmitMode(getUInt2(0x40,6));
}
void
TyTCodeplug::GeneralSettingsElement::transmitMode(TransmitMode mode) {
  setUInt2(0x40,6, mode);
}

TyTCodeplug::GeneralSettingsElement::MonitorType
TyTCodeplug::GeneralSettingsElement::monitorType() const {
  return getBit(0x40,4) ? MONITOR_OPEN_SQUELCH : MONITOR_SILENT;
}
void
TyTCodeplug::GeneralSettingsElement::monitorType(MonitorType type) {
  setBit(0x40,4, MONITOR_OPEN_SQUELCH==type);
}

bool
TyTCodeplug::GeneralSettingsElement::allLEDsDisabled() const {
  return getBit(0x40,2);
}
void
TyTCodeplug::GeneralSettingsElement::disableAllLEDs(bool disable) {
  setBit(0x40,2, disable);
}

bool
TyTCodeplug::GeneralSettingsElement::savePreamble() const {
  return getBit(0x41,0);
}
void
TyTCodeplug::GeneralSettingsElement::savePreamble(bool enable) {
  setBit(0x41,0, enable);
}

bool
TyTCodeplug::GeneralSettingsElement::saveModeRX() const {
  return getBit(0x41,1);
}
void
TyTCodeplug::GeneralSettingsElement::saveModeRX(bool enable) {
  setBit(0x41,1, enable);
}

bool
TyTCodeplug::GeneralSettingsElement::disableAllTones() const {
  return getBit(0x41,2);
}
void
TyTCodeplug::GeneralSettingsElement::disableAllTones(bool disable) {
  setBit(0x41,2, disable);
}

bool
TyTCodeplug::GeneralSettingsElement::chFreeIndicationTone() const {
  return getBit(0x41,4);
}
void
TyTCodeplug::GeneralSettingsElement::chFreeIndicationTone(bool enable) {
  setBit(0x41,4, enable);
}

bool
TyTCodeplug::GeneralSettingsElement::passwdAndLock() const {
  return getBit(0x41,5);
}
void
TyTCodeplug::GeneralSettingsElement::passwdAndLock(bool enable) {
  setBit(0x41,5, enable);
}

bool
TyTCodeplug::GeneralSettingsElement::talkPermitDigital() const {
  return getBit(0x41,6);
}
void
TyTCodeplug::GeneralSettingsElement::talkPermitDigital(bool enable) {
  setBit(0x41,6, enable);
}

bool
TyTCodeplug::GeneralSettingsElement::talkPermitAnalog() const {
  return getBit(0x41,7);
}
void
TyTCodeplug::GeneralSettingsElement::talkPermitAnalog(bool enable) {
  setBit(0x41,7, enable);
}

bool
TyTCodeplug::GeneralSettingsElement::channelVoiceAnnounce() const {
  return getBit(0x42,1);
}
void
TyTCodeplug::GeneralSettingsElement::channelVoiceAnnounce(bool enable) {
  setBit(0x42,1, enable);
}

bool
TyTCodeplug::GeneralSettingsElement::introPicture() const {
  return getBit(0x42,4);
}
void
TyTCodeplug::GeneralSettingsElement::introPicture(bool enable) {
  setBit(0x42,4, enable);
}

bool
TyTCodeplug::GeneralSettingsElement::keypadTones() const {
  return getBit(0x42,5);
}
void
TyTCodeplug::GeneralSettingsElement::keypadTones(bool enable) {
  setBit(0x42,5, enable);
}

bool
TyTCodeplug::GeneralSettingsElement::channelModeA() const {
  return getBit(0x43,3);
}
void
TyTCodeplug::GeneralSettingsElement::channelModeA(bool enable) {
  setBit(0x43,3, enable);
}

bool
TyTCodeplug::GeneralSettingsElement::channelModeB() const {
  return getBit(0x43,7);
}
void
TyTCodeplug::GeneralSettingsElement::channelModeB(bool enable) {
  setBit(0x43,7, enable);
}

uint32_t
TyTCodeplug::GeneralSettingsElement::dmrID() const {
  return getUInt24_le(0x44);
}
void
TyTCodeplug::GeneralSettingsElement::dmrID(uint32_t id) {
  setUInt24_le(0x44, id);
}

uint
TyTCodeplug::GeneralSettingsElement::txPreambleDuration() const {
  return uint(getUInt8(0x48))*60;
}
void
TyTCodeplug::GeneralSettingsElement::txPreambleDuration(uint dur) {
  setUInt8(0x48, dur/60);
}

uint
TyTCodeplug::GeneralSettingsElement::groupCallHangTime() const {
  return uint(getUInt8(0x49))*100;
}
void
TyTCodeplug::GeneralSettingsElement::groupCallHangTime(uint dur) {
  setUInt8(0x49, dur/100);
}

uint
TyTCodeplug::GeneralSettingsElement::privateCallHangTime() const {
  return uint(getUInt8(0x4a))*100;
}
void
TyTCodeplug::GeneralSettingsElement::privateCallHangTime(uint dur) {
  setUInt8(0x4a, dur/100);
}

uint
TyTCodeplug::GeneralSettingsElement::voxSesitivity() const {
  return getUInt8(0x4b);
}
void
TyTCodeplug::GeneralSettingsElement::voxSesitivity(uint level) {
  setUInt8(0x4b, level);
}

uint
TyTCodeplug::GeneralSettingsElement::lowBatteryInterval() const {
  return uint(getUInt8(0x4e))*5;
}
void
TyTCodeplug::GeneralSettingsElement::lowBatteryInterval(uint dur) {
  setUInt8(0x4e, dur/5);
}

uint
TyTCodeplug::GeneralSettingsElement::callAlertToneDuration() const {
  return uint(getUInt8(0x4f))*5;
}
void
TyTCodeplug::GeneralSettingsElement::callAlertToneDuration(uint dur) {
  setUInt8(0x4f, dur/5);
}

uint
TyTCodeplug::GeneralSettingsElement::loneWorkerResponseTime() const {
  return getUInt8(0x50);
}
void
TyTCodeplug::GeneralSettingsElement::loneWorkerResponseTime(uint dur) {
  setUInt8(0x50, dur);
}

uint
TyTCodeplug::GeneralSettingsElement::loneWorkerReminderTime() const {
  return getUInt8(0x51);
}
void
TyTCodeplug::GeneralSettingsElement::loneWorkerReminderTime(uint dur) {
  setUInt8(0x51, dur);
}

uint
TyTCodeplug::GeneralSettingsElement::scanDigitalHangTime() const {
  return uint(getUInt8(0x53))*100;
}
void
TyTCodeplug::GeneralSettingsElement::scanDigitalHangTime(uint dur) {
  setUInt8(0x53, dur/100);
}

uint
TyTCodeplug::GeneralSettingsElement::scanAnalogHangTime() const {
  return uint(getUInt8(0x54))*100;
}
void
TyTCodeplug::GeneralSettingsElement::scanAnalogHangTime(uint dur) {
  setUInt8(0x54, dur/100);
}

uint
TyTCodeplug::GeneralSettingsElement::backlightTime() const {
  return getUInt2(0x55, 0)*5;
}
void
TyTCodeplug::GeneralSettingsElement::backlightTime(uint sec) {
  setUInt2(0x55, 0, sec/5);
}
void
TyTCodeplug::GeneralSettingsElement::backlightTimeSetAlways() {
  setUInt2(0x55, 0, 0);
}

uint
TyTCodeplug::GeneralSettingsElement::keypadLockTime() const {
  return uint(getUInt8(0x56))*5;
}
void
TyTCodeplug::GeneralSettingsElement::keypadLockTime(uint sec) {
  setUInt8(0x56, sec/5);
}
void
TyTCodeplug::GeneralSettingsElement::keypadLockTimeSetManual() {
  setUInt8(0x56, 0xff);
}

bool
TyTCodeplug::GeneralSettingsElement::channelMode() const {
  return 0xff == getUInt8(0x57);
}
void
TyTCodeplug::GeneralSettingsElement::channelMode(bool enable) {
  setUInt8(0x57, enable ? 0xff : 0x00);
}

uint32_t
TyTCodeplug::GeneralSettingsElement::powerOnPassword() const {
  return getBCD8_le(0x58);
}
void
TyTCodeplug::GeneralSettingsElement::powerOnPassword(uint32_t pass) {
  setBCD8_le(0x58, pass);
}
uint32_t
TyTCodeplug::GeneralSettingsElement::radioProgPassword() const {
  return getBCD8_le(0x5c);
}
void
TyTCodeplug::GeneralSettingsElement::radioProgPassword(uint32_t passwd) {
  setBCD8_le(0x5c, passwd);
}
void
TyTCodeplug::GeneralSettingsElement::radioProgPasswordDisable() {
  setUInt32_le(0x5c, 0xffffffff);
}

bool
TyTCodeplug::GeneralSettingsElement::pcProgPasswordEnabled() const {
  return (0xff != getUInt8(0x60));
}
QString
TyTCodeplug::GeneralSettingsElement::pcProgPassword() const {
  return readASCII(0x60, 8);
}
void
TyTCodeplug::GeneralSettingsElement::pcProgPassword(const QString &pass) {
  writeASCII(0x60, pass, 8);
}
void
TyTCodeplug::GeneralSettingsElement::pcProgPasswordDisable() {
  memset(_data+0x60, 0xff, 8);
}

bool
TyTCodeplug::GeneralSettingsElement::groupCallMatch() const {
  return getBit(0x6b, 0);
}
void
TyTCodeplug::GeneralSettingsElement::groupCallMatch(bool enable) {
  setBit(0x6b, 0, enable);
}
bool
TyTCodeplug::GeneralSettingsElement::privateCallMatch() const {
  return getBit(0x6b, 1);
}
void
TyTCodeplug::GeneralSettingsElement::privateCallMatch(bool enable) {
  setBit(0x6b, 1, enable);
}

QTimeZone
TyTCodeplug::GeneralSettingsElement::timeZone() const {
  return QTimeZone((int(getUInt5(0x6b, 3))-12)*3600);
}
void
TyTCodeplug::GeneralSettingsElement::timeZone(const QTimeZone &zone) {
  int idx = (zone.standardTimeOffset(QDateTime::currentDateTime())/3600)+12;
  setUInt5(0x6b, 3, uint8_t(idx));
}

QString
TyTCodeplug::GeneralSettingsElement::radioName() const {
  return readUnicode(0x70, 16);
}
void
TyTCodeplug::GeneralSettingsElement::radioName(const QString &name) {
  writeUnicode(0x70, name, 16);
}

uint
TyTCodeplug::GeneralSettingsElement::channelHangTime() const {
  return uint(getUInt8(0x90))*100;
}
void
TyTCodeplug::GeneralSettingsElement::channelHangTime(uint dur) {
  setUInt8(0x90, dur/100);
}

bool
TyTCodeplug::GeneralSettingsElement::publicZone() const {
  return getBit(0x92, 2);
}
void
TyTCodeplug::GeneralSettingsElement::publicZone(bool enable) {
  setBit(0x92, 2, enable);
}

uint32_t
TyTCodeplug::GeneralSettingsElement::additionalDMRId(uint n) const {
  return getUInt24_le(0x94+4*n);
}
void
TyTCodeplug::GeneralSettingsElement::additionalDMRId(uint n, uint32_t id) {
  setUInt24_le(0x94+4*n, id);
}

uint
TyTCodeplug::GeneralSettingsElement::micLevel() const {
  return (uint(getUInt3(0xa0,3)+1)*100)/60;
}
void
TyTCodeplug::GeneralSettingsElement::micLevel(uint level) {
  setUInt3(0xa0,3, ((level-1)*60)/100);
}

bool
TyTCodeplug::GeneralSettingsElement::editRadioID() const {
  return !getBit(0xa0, 6);
}
void
TyTCodeplug::GeneralSettingsElement::editRadioID(bool enable) {
  setBit(0xa0,6, !enable);
}

bool
TyTCodeplug::GeneralSettingsElement::fromConfig(const Config *config) {
  radioName(config->name());
  dmrID(config->radioIDs()->getDefaultId()->id());
  introLine1(config->introLine1());
  introLine2(config->introLine2());
  timeZone(QTimeZone::systemTimeZone());
  micLevel(config->micLevel());
  channelVoiceAnnounce(config->speech());
  return true;
}

bool
TyTCodeplug::GeneralSettingsElement::updateConfig(Config *config) {
  config->radioIDs()->getDefaultId()->setId(dmrID());
  config->setName(radioName());
  config->setIntroLine1(introLine1());
  config->setIntroLine2(introLine2());
  config->setMicLevel(micLevel());
  config->setSpeech(channelVoiceAnnounce());
  return true;
}


/* ******************************************************************************************** *
 * Implementation of TyTCodeplug
 * ******************************************************************************************** */
TyTCodeplug::TyTCodeplug(QObject *parent)
  : CodePlug(parent)
{
  // pass...
}

TyTCodeplug::~TyTCodeplug() {
  // pass...
}

void
TyTCodeplug::clear()
{
  // Clear timestamp
  this->clearTimestamp();
  // Clear general config
  this->clearGeneralSettings();
  // Clear menu settings
  this->clearMenuSettings();
  // Clear button settings
  this->clearButtonSettings();
  // Clear text messages
  this->clearTextMessages();
  // Clear privacy keys
  this->clearPrivacyKeys();
  // Clear emergency systems
  this->clearEmergencySystems();
  // Clear RX group lists
  this->clearGroupLists();
  // Clear zones & zone extensions
  this->clearZones();
  // Clear scan lists;
  this->clearScanLists();
  // Clear VFO A & B settings.
  this->clearVFOSettings();
  // Clear GPS systems
  this->clearPositioningSystems();
  // Clear channels
  this->clearChannels();
  // Clear contacts
  this->clearContacts();
}

bool
TyTCodeplug::encode(Config *config, const Flags &flags) {
  // Set timestamp
  if (! this->encodeTimestamp(config, flags)) {
    _errorMessage = tr("Cannot encode time-stamp: %1").arg(_errorMessage);
    return false;
  }
  // General config
  if (! this->encodeGeneralSettings(config, flags)) {
    _errorMessage = tr("Cannot encode general settings: %1").arg(_errorMessage);
    return false;
  }

  // Boot time settings
  if (! this->encodeBootSettings(config, flags)) {
    _errorMessage = tr("Cannot encode boot settings: %1").arg(_errorMessage);
    return false;
  }

  // Define Contacts
  if (! this->encodeContacts(config, flags)) {
    _errorMessage = tr("Cannot encode contacts: %1").arg(_errorMessage);
    return false;
  }

  // Define RX GroupLists
  if (! this->encodeGroupLists(config, flags)) {
    _errorMessage = tr("Cannot encode group lists: %1").arg(_errorMessage);
    return false;
  }

  // Define Channels
  if (! this->encodeChannels(config, flags)) {
    _errorMessage = tr("Cannot encode channels: %1").arg(_errorMessage);
    return false;
  }

  // Define Zones
  if (! this->encodeZones(config, flags)) {
    _errorMessage = tr("Cannot encode zones: %1").arg(_errorMessage);
    return false;
  }

  // Define Scanlists
  if (! this->encodeScanLists(config, flags)) {
    _errorMessage = tr("Cannot encode scan lists: %1").arg(_errorMessage);
    return false;
  }

  // Define GPS systems
  if (! this->encodePositioningSystems(config, flags)) {
    _errorMessage = tr("Cannot encode positioning systems: %1").arg(_errorMessage);
    return false;
  }

  return true;
}

bool
TyTCodeplug::decode(Config *config) {
  // Create index<->object table.
  CodeplugContext ctx(config);

  // Clear config object
  config->reset();

  // General config
  if (! this->decodeGeneralSettings(config)) {
    _errorMessage = tr("Cannot decode general settings: %1").arg(_errorMessage);
    return false;
  }

  // Define Contacts
  if (! this->createContacts(ctx)) {
    _errorMessage = tr("Cannot create contacts: %1").arg(_errorMessage);
    return false;
  }

  // Define RX GroupLists
  if (! this->createGroupLists(ctx)) {
    _errorMessage = tr("Cannot create group lists: %1").arg(_errorMessage);
    return false;
  }

  // Define Channels
  if (! this->createChannels(ctx)) {
    _errorMessage = tr("Cannot create channels: %1").arg(_errorMessage);
    return false;
  }

  // Define Zones
  if (! this->createZones(ctx)) {
    _errorMessage = tr("Cannot create zones: %1").arg(_errorMessage);
    return false;
  }

  // Define Scanlists
  if (! this->createScanLists(ctx)) {
    _errorMessage = tr("Cannot create scan lists: %1").arg(_errorMessage);
    return false;
  }

  // Define GPS systems
  if (! this->createPositioningSystems(ctx)) {
    _errorMessage = tr("Cannot create positioning systems: %1").arg(_errorMessage);
    return false;
  }

  // Link RX GroupLists
  if (! this->linkGroupLists(ctx)) {
    _errorMessage = tr("Cannot link group lists: %1").arg(_errorMessage);
    return false;
  }

  // Link Channels
  if (! this->linkChannels(ctx)) {
    _errorMessage = tr("Cannot link channels: %1").arg(_errorMessage);
    return false;
  }

  // Link Zones
  if (! this->linkZones(ctx)) {
    _errorMessage = tr("Cannot link zones: %1").arg(_errorMessage);
    return false;
  }

  // Link Scanlists
  if (! this->linkScanLists(ctx)) {
    _errorMessage = tr("Cannot link scan lists: %1").arg(_errorMessage);
    return false;
  }

  // Link GPS systems
  if (! this->linkPositioningSystems(ctx)) {
    _errorMessage = tr("Cannot link positioning systems: %1").arg(_errorMessage);
    return false;
  }

  return true;
}


void
TyTCodeplug::clearTimestamp() {
  // pass...
}

bool
TyTCodeplug::encodeTimestamp(Config *config, const Flags &flags) {
  return true;
}

bool
TyTCodeplug::decodeTimestamp(Config *config) {
  return true;
}


void
TyTCodeplug::clearGeneralSettings() {
  // pass...
}

bool
TyTCodeplug::encodeGeneralSettings(Config *config, const Flags &flags) {
  return true;
}

bool
TyTCodeplug::decodeGeneralSettings(Config *config) {
  return true;
}


void
TyTCodeplug::clearBootSettings() {
  // pass...
}

bool
TyTCodeplug::encodeBootSettings(Config *config, const Flags &flags) {
  return true;
}

bool
TyTCodeplug::decodeBootSettings(Config *config) {
  return true;
}


void
TyTCodeplug::clearContacts() {
  // pass...
}

bool
TyTCodeplug::encodeContacts(Config *config, const Flags &flags) {
  return true;
}

bool
TyTCodeplug::createContacts(CodeplugContext &ctx) {
  return true;
}


void
TyTCodeplug::clearGroupLists() {
  // pass...
}

bool
TyTCodeplug::encodeGroupLists(Config *config, const Flags &flags) {
  return true;
}

bool
TyTCodeplug::createGroupLists(CodeplugContext &ctx) {
  return true;
}

bool
TyTCodeplug::linkGroupLists(CodeplugContext &ctx) {
  return true;
}


void
TyTCodeplug::clearChannels() {
  // pass...
}

bool
TyTCodeplug::encodeChannels(Config *config, const Flags &flags) {
  return true;
}

bool
TyTCodeplug::createChannels(CodeplugContext &ctx) {
  return true;
}

bool
TyTCodeplug::linkChannels(CodeplugContext &ctx) {
  return true;
}


void
TyTCodeplug::clearZones() {
  // pass...
}

bool
TyTCodeplug::encodeZones(Config *config, const Flags &flags) {
  return true;
}

bool
TyTCodeplug::createZones(CodeplugContext &ctx) {
  return true;
}

bool
TyTCodeplug::linkZones(CodeplugContext &ctx) {
  return true;
}


void
TyTCodeplug::clearScanLists() {
  // pass...
}

bool
TyTCodeplug::encodeScanLists(Config *config, const Flags &flags) {
  return true;
}

bool
TyTCodeplug::createScanLists(CodeplugContext &ctx) {
  return true;
}

bool
TyTCodeplug::linkScanLists(CodeplugContext &ctx) {
  return true;
}


void
TyTCodeplug::clearPositioningSystems() {
  // pass...
}

bool
TyTCodeplug::encodePositioningSystems(Config *config, const Flags &flags) {
  return true;
}

bool
TyTCodeplug::createPositioningSystems(CodeplugContext &ctx) {
  return true;
}

bool
TyTCodeplug::linkPositioningSystems(CodeplugContext &ctx) {
  return true;
}


void
TyTCodeplug::clearMenuSettings() {
  // pass...
}

void
TyTCodeplug::clearButtonSettings() {
  // pass...
}

void
TyTCodeplug::clearTextMessages() {
  // pass...
}

void
TyTCodeplug::clearPrivacyKeys() {
  // pass...
}

void
TyTCodeplug::clearEmergencySystems() {
  // pass...
}

void
TyTCodeplug::clearVFOSettings() {
  // pass...
}
