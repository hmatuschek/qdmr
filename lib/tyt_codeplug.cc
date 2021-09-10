#include "tyt_codeplug.hh"
#include "codeplugcontext.hh"
#include "config.hh"
#include "utils.hh"
#include "channel.hh"
#include "gpssystem.hh"
#include "config.h"
#include "logger.hh"
#include "tyt_extensions.hh"
#include <QTimeZone>
#include <QtEndian>


/* ******************************************************************************************** *
 * Implementation of TyTCodeplug::ChannelElement
 * ******************************************************************************************** */
TyTCodeplug::ChannelElement::ChannelElement(uint8_t *ptr, size_t size)
  : CodePlug::Element(ptr, size)
{
  // pass...
}

TyTCodeplug::ChannelElement::ChannelElement(uint8_t *ptr)
  : CodePlug::Element(ptr, 0x0040)
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

  setMode(MODE_ANALOG);
  setBandwidth(AnalogChannel::Bandwidth::Narrow);
  enableAutoScan(0);
  setBit(0, 1); setBit(0,2);
  enableLoneWorker(false);
  enableTalkaround(false);
  enableRXOnly(false);
  setTimeSlot(DigitalChannel::TimeSlot::TS1);
  setColorCode(1);
  setPrivacyIndex(0);
  setPrivacyType(PRIV_NONE);
  enablePrivateCallConfirm(false);
  enableDataCallConfirm(false);
  setRXRefFrequency(REF_LOW);
  clearBit(3,2);
  enableEmergencyAlarmACK(false);
  clearBit(3,4); setBit(3,5); setBit(3,6);
  enableDisplayPTTId(true);
  setTXRefFrequency(REF_LOW);
  setBit(4,2); clearBit(4,3);
  enableVOX(false);
  setBit(4,5);
  setAdmitCriterion(ADMIT_ALWAYS);
  clearBit(5,0);
  setInCallCriteria(INCALL_ALWAYS);
  setTurnOffFreq(TURNOFF_NONE);
  setContactIndex(0);
  setTXTimeOut(0);
  clearBit(8,6);
  setTXTimeOutRekeyDelay(0);
  setEmergencySystemIndex(0);
  setScanListIndex(0);
  setGroupListIndex(0);
  setPositioningSystemIndex(0);
  for (uint8_t i=0; i<8; i++)
    setDTMFDecode(i, false);
  setSquelch(1);
  setRXFrequency(400000000UL);
  setTXFrequency(400000000UL);
  setRXSignaling(Signaling::SIGNALING_NONE);
  setTXSignaling(Signaling::SIGNALING_NONE);
  setRXSignalingSystemIndex(0);
  setTXSignalingSystemIndex(0);
  setPower(Channel::Power::High);
  setBit(30,2); setBit(30,3); setBit(30,4); setBit(30,5); setBit(30,6); setBit(30,7);
  enableTXGPSInfo(true);
  enableRXGPSInfo(true);
  enableAllowInterrupt(true);
  enableDualCapacityDirectMode(false);
  enableLeaderOrMS(true);
  setBit(31, 5); setBit(31, 6); setBit(31, 7);
  memset((_data+32), 0x00, sizeof(32));
}


TyTCodeplug::ChannelElement::Mode
TyTCodeplug::ChannelElement::mode() const {
  return TyTCodeplug::ChannelElement::Mode(getUInt2(0, 0));
}
void
TyTCodeplug::ChannelElement::setMode(Mode mode) {
  setUInt2(0,0, uint8_t(mode));
}

AnalogChannel::Bandwidth
TyTCodeplug::ChannelElement::bandwidth() const {
  if (0 == getUInt2(0, 2))
    return AnalogChannel::Bandwidth::Narrow;
  return AnalogChannel::Bandwidth::Wide;
}
void
TyTCodeplug::ChannelElement::setBandwidth(AnalogChannel::Bandwidth bw) {
  if (AnalogChannel::Bandwidth::Narrow == bw)
    setUInt2(0, 2, BW_12_5_KHZ);
  else
    setUInt2(0, 2, BW_25_KHZ);
}

bool
TyTCodeplug::ChannelElement::autoScan() const {
  return getBit(0, 4);
}
void
TyTCodeplug::ChannelElement::enableAutoScan(bool enable) {
  setBit(0, 4, enable);
}

bool
TyTCodeplug::ChannelElement::loneWorker() const {
  return getBit(0, 7);
}
void
TyTCodeplug::ChannelElement::enableLoneWorker(bool enable) {
  setBit(0, 7, enable);
}

bool
TyTCodeplug::ChannelElement::talkaround() const {
  return ! getBit(1, 0);
}
void
TyTCodeplug::ChannelElement::enableTalkaround(bool enable) {
  setBit(1, 0, !enable);
}

bool
TyTCodeplug::ChannelElement::rxOnly() const {
  return getBit(1, 1);
}
void
TyTCodeplug::ChannelElement::enableRXOnly(bool enable) {
  setBit(1, 1, enable);
}

DigitalChannel::TimeSlot
TyTCodeplug::ChannelElement::timeSlot() const {
  if (2 == getUInt2(1,2))
    return DigitalChannel::TimeSlot::TS2;
  return DigitalChannel::TimeSlot::TS1;
}
void
TyTCodeplug::ChannelElement::setTimeSlot(DigitalChannel::TimeSlot ts) {
  if (DigitalChannel::TimeSlot::TS1 == ts)
    setUInt2(1,2,1);
  else
    setUInt2(1,2,2);
}

uint8_t
TyTCodeplug::ChannelElement::colorCode() const {
  return getUInt4(1,4);
}
void
TyTCodeplug::ChannelElement::setColorCode(uint8_t cc) {
  setUInt4(1,4, std::min(uint8_t(16), cc));
}

uint8_t
TyTCodeplug::ChannelElement::privacyIndex() const {
  return getUInt4(2,0);
}
void
TyTCodeplug::ChannelElement::setPrivacyIndex(uint8_t idx) {
  setUInt4(2,0, idx);
}

TyTCodeplug::ChannelElement::PrivacyType
TyTCodeplug::ChannelElement::privacyType() const {
  return TyTCodeplug::ChannelElement::PrivacyType(getUInt2(2,4));
}
void
TyTCodeplug::ChannelElement::setPrivacyType(TyTCodeplug::ChannelElement::PrivacyType type) {
  setUInt2(2,4, uint8_t(type));
}

bool
TyTCodeplug::ChannelElement::privateCallConfirm() const {
  return getBit(2, 6);
}
void
TyTCodeplug::ChannelElement::enablePrivateCallConfirm(bool enable) {
  setBit(2, 6, enable);
}

bool
TyTCodeplug::ChannelElement::dataCallConfirm() const {
  return getBit(2, 7);
}
void
TyTCodeplug::ChannelElement::enableDataCallConfirm(bool enable) {
  setBit(2, 7, enable);
}

TyTCodeplug::ChannelElement::RefFrequency
TyTCodeplug::ChannelElement::rxRefFrequency() const {
  return TyTCodeplug::ChannelElement::RefFrequency(getUInt2(3,0));
}
void
TyTCodeplug::ChannelElement::setRXRefFrequency(TyTCodeplug::ChannelElement::RefFrequency ref) {
  setUInt2(3,0, uint8_t(ref));
}

bool
TyTCodeplug::ChannelElement::emergencyAlarmACK() const {
  return getBit(3,3);
}
void
TyTCodeplug::ChannelElement::enableEmergencyAlarmACK(bool enable) {
  setBit(3,3, enable);
}

bool
TyTCodeplug::ChannelElement::displayPTTId() const {
  return ! getBit(3,7);
}
void
TyTCodeplug::ChannelElement::enableDisplayPTTId(bool enable) {
  setBit(3,7, !enable);
}

TyTCodeplug::ChannelElement::RefFrequency
TyTCodeplug::ChannelElement::txRefFrequency() const {
  return TyTCodeplug::ChannelElement::RefFrequency(getUInt2(4,0));
}
void
TyTCodeplug::ChannelElement::setTXRefFrequency(TyTCodeplug::ChannelElement::RefFrequency ref) {
  setUInt2(4,0, uint8_t(ref));
}

bool
TyTCodeplug::ChannelElement::vox() const {
  return getBit(4,4);
}
void
TyTCodeplug::ChannelElement::enableVOX(bool enable) {
  setBit(4,4, enable);
}

TyTCodeplug::ChannelElement::Admit
TyTCodeplug::ChannelElement::admitCriterion() const {
  return TyTCodeplug::ChannelElement::Admit(getUInt2(4,6));
}
void
TyTCodeplug::ChannelElement::setAdmitCriterion(TyTCodeplug::ChannelElement::Admit admit) {
  setUInt2(4,6, uint8_t(admit));
}

TyTCodeplug::ChannelElement::InCall
TyTCodeplug::ChannelElement::inCallCriteria() const {
  return InCall(getUInt2(5,4));
}
void
TyTCodeplug::ChannelElement::setInCallCriteria(InCall crit) {
  setUInt2(5,4, uint8_t(crit));
}

TyTCodeplug::ChannelElement::TurnOffFreq
TyTCodeplug::ChannelElement::turnOffFreq() const {
  return TurnOffFreq(getUInt2(5,6));
}
void
TyTCodeplug::ChannelElement::setTurnOffFreq(TurnOffFreq freq) {
  setUInt2(5,6, uint8_t(freq));
}

uint16_t
TyTCodeplug::ChannelElement::contactIndex() const {
  return getUInt16_le(6);
}
void
TyTCodeplug::ChannelElement::setContactIndex(uint16_t idx) {
  setUInt16_le(6, idx);
}

uint TyTCodeplug::ChannelElement::txTimeOut() const {
  return getUInt6(8, 0)*15;
}
void
TyTCodeplug::ChannelElement::setTXTimeOut(uint tot) {
  return setUInt6(8, 0, tot/15);
}

uint8_t
TyTCodeplug::ChannelElement::txTimeOutRekeyDelay() const {
  return getUInt8(9);
}
void
TyTCodeplug::ChannelElement::setTXTimeOutRekeyDelay(uint8_t delay) {
  return setUInt8(9, delay);
}

uint8_t
TyTCodeplug::ChannelElement::emergencySystemIndex() const {
  return getUInt8(10);
}
void
TyTCodeplug::ChannelElement::setEmergencySystemIndex(uint8_t delay) {
  return setUInt8(10, delay);
}

uint8_t
TyTCodeplug::ChannelElement::scanListIndex() const {
  return getUInt8(11);
}
void
TyTCodeplug::ChannelElement::setScanListIndex(uint8_t idx) {
  return setUInt8(11, idx);
}

uint8_t
TyTCodeplug::ChannelElement::groupListIndex() const {
  return getUInt8(12);
}
void
TyTCodeplug::ChannelElement::setGroupListIndex(uint8_t idx) {
  return setUInt8(12, idx);
}

uint8_t
TyTCodeplug::ChannelElement::positioningSystemIndex() const {
  return getUInt8(13);
}
void
TyTCodeplug::ChannelElement::setPositioningSystemIndex(uint8_t idx) {
  return setUInt8(13, idx);
}

bool
TyTCodeplug::ChannelElement::dtmfDecode(uint8_t idx) const {
  return getBit(14, idx);
}
void
TyTCodeplug::ChannelElement::setDTMFDecode(uint8_t idx, bool enable) {
  setBit(14, idx, enable);
}

uint TyTCodeplug::ChannelElement::squelch() const {
  return getUInt8(15);
}
void
TyTCodeplug::ChannelElement::setSquelch(uint value) {
  value = std::min(uint(10), value);
  return setUInt8(15, value);
}

uint32_t
TyTCodeplug::ChannelElement::rxFrequency() const {
  return getBCD8_le(16)*10;
}
void
TyTCodeplug::ChannelElement::setRXFrequency(uint32_t freq_Hz) {
  return setBCD8_le(16, freq_Hz/10);
}

uint32_t
TyTCodeplug::ChannelElement::txFrequency() const {
  return getBCD8_le(20)*10;
}
void
TyTCodeplug::ChannelElement::setTXFrequency(uint32_t freq_Hz) {
  return setBCD8_le(20, freq_Hz/10);
}

Signaling::Code
TyTCodeplug::ChannelElement::rxSignaling() const {
  return decode_ctcss_tone_table(getUInt16_le(24));
}
void
TyTCodeplug::ChannelElement::setRXSignaling(Signaling::Code code) {
  setUInt16_le(24, encode_ctcss_tone_table(code));
}

Signaling::Code
TyTCodeplug::ChannelElement::txSignaling() const {
  return decode_ctcss_tone_table(getUInt16_le(26));
}
void
TyTCodeplug::ChannelElement::setTXSignaling(Signaling::Code code) {
  setUInt16_le(26, encode_ctcss_tone_table(code));
}

uint8_t
TyTCodeplug::ChannelElement::rxSignalingSystemIndex() const {
  return getUInt8(28);
}
void
TyTCodeplug::ChannelElement::setRXSignalingSystemIndex(uint8_t idx) {
  setUInt8(28, idx);
}

uint8_t
TyTCodeplug::ChannelElement::txSignalingSystemIndex() const {
  return getUInt8(29);
}
void
TyTCodeplug::ChannelElement::setTXSignalingSystemIndex(uint8_t idx) {
  setUInt8(29, idx);
}

Channel::Power
TyTCodeplug::ChannelElement::power() const {
  switch (getUInt2(30, 0)) {
  case 0: return Channel::Power::Low;
  case 2: return Channel::Power::Mid;
  case 3: return Channel::Power::High;
  default: break;
  }
  return Channel::Power::Low;
}
void
TyTCodeplug::ChannelElement::setPower(Channel::Power pwr) {
  switch (pwr) {
  case Channel::Power::Min:
  case Channel::Power::Low:
    setUInt2(30,0, 0);
    break;
  case Channel::Power::Mid:
    setUInt2(30,0, 2);
    break;
  case Channel::Power::High:
  case Channel::Power::Max:
    setUInt2(30,0, 3);
  }
}

bool
TyTCodeplug::ChannelElement::txGPSInfo() const {
  return ! getBit(31, 0);
}
void
TyTCodeplug::ChannelElement::enableTXGPSInfo(bool enable) {
  setBit(31,0, !enable);
}

bool
TyTCodeplug::ChannelElement::rxGPSInfo() const {
  return !getBit(31, 1);
}
void
TyTCodeplug::ChannelElement::enableRXGPSInfo(bool enable) {
  setBit(31,1, !enable);
}
bool
TyTCodeplug::ChannelElement::allowInterrupt() const {
  return !getBit(31, 2);
}
void
TyTCodeplug::ChannelElement::enableAllowInterrupt(bool enable) {
  setBit(31,2, !enable);
}
bool
TyTCodeplug::ChannelElement::dualCapacityDirectMode() const {
  return !getBit(31, 3);
}
void
TyTCodeplug::ChannelElement::enableDualCapacityDirectMode(bool enable) {
  setBit(31,3, !enable);
}
bool
TyTCodeplug::ChannelElement::leaderOrMS() const {
  return !getBit(31, 4);
}
void
TyTCodeplug::ChannelElement::enableLeaderOrMS(bool enable) {
  setBit(31,4, !enable);
}

QString
TyTCodeplug::ChannelElement::name() const {
  return readUnicode(32, 16, 0x0000);
}
void
TyTCodeplug::ChannelElement::setName(const QString &name) {
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
      case ADMIT_ALWAYS: admit_crit = AnalogChannel::Admit::Always; break;
      case ADMIT_TONE: admit_crit = AnalogChannel::Admit::Tone; break;
      case ADMIT_CH_FREE: admit_crit = AnalogChannel::Admit::Free; break;
      default: admit_crit = AnalogChannel::Admit::Free; break;
    }

    return new AnalogChannel(name(), double(rxFrequency())/1e6, double(txFrequency())/1e6,
                             power(), txTimeOut(), rxOnly(), admit_crit, squelch(), rxSignaling(),
                             txSignaling(), bandwidth(), nullptr);
  } else if (MODE_DIGITAL == mode()) {
    DigitalChannel::Admit admit_crit;
    switch(admitCriterion()) {
      case ADMIT_ALWAYS: admit_crit = DigitalChannel::Admit::Always; break;
      case ADMIT_CH_FREE: admit_crit = DigitalChannel::Admit::Free; break;
      case ADMIT_COLOR: admit_crit = DigitalChannel::Admit::ColorCode; break;
      default: admit_crit = DigitalChannel::Admit::Free; break;
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
    c->setScanListObj(ctx.getScanList(scanListIndex()));
  }

  if (MODE_ANALOG == mode()) {
    return true;
  } else if ((MODE_DIGITAL == mode()) && (c->is<DigitalChannel>())){
    DigitalChannel *dc = c->as<DigitalChannel>();
    if (contactIndex() && ctx.hasDigitalContact(contactIndex())) {
      dc->setTXContactObj(ctx.getDigitalContact(contactIndex()));
    }
    if (groupListIndex() && ctx.hasGroupList(groupListIndex())) {
      dc->setGroupListObj(ctx.getGroupList(groupListIndex()));
    }
    if (positioningSystemIndex() && ctx.hasGPSSystem(positioningSystemIndex())) {
      dc->aprsObj(ctx.getGPSSystem(positioningSystemIndex()));
    }
    return true;
  }

  return false;
}

void
TyTCodeplug::ChannelElement::fromChannelObj(const Channel *chan, const CodeplugContext &ctx) {
  setName(chan->name());
  setRXFrequency(chan->rxFrequency()*1e6);
  setTXFrequency(chan->txFrequency()*1e6);
  enableRXOnly(chan->rxOnly());
  setTXTimeOut(chan->timeout());
  if (chan->scanListObj())
    setScanListIndex(ctx.config()->scanlists()->indexOf(chan->scanListObj())+1);
  else
    setScanListIndex(0);

  // encode power setting
  setPower(chan->power());

  if (chan->is<const DigitalChannel>()) {
    const DigitalChannel *dchan = chan->as<const DigitalChannel>();
    setMode(MODE_DIGITAL);
    switch (dchan->admit()) {
    case DigitalChannel::Admit::Always: setAdmitCriterion(ADMIT_ALWAYS); break;
    case DigitalChannel::Admit::Free: setAdmitCriterion(ADMIT_CH_FREE); break;
    case DigitalChannel::Admit::ColorCode: setAdmitCriterion(ADMIT_COLOR); break;
    }
    setColorCode(dchan->colorCode());
    setTimeSlot(dchan->timeSlot());
    if (dchan->groupListObj())
      setGroupListIndex(ctx.config()->rxGroupLists()->indexOf(dchan->groupListObj())+1);
    else
      setGroupListIndex(0);
    if (dchan->txContactObj())
      setContactIndex(ctx.config()->contacts()->indexOfDigital(dchan->txContactObj())+1);
    setSquelch(0);
    setBandwidth(AnalogChannel::Bandwidth::Narrow);
    setRXSignaling(Signaling::SIGNALING_NONE);
    setTXSignaling(Signaling::SIGNALING_NONE);
    if (dchan->aprsObj() && dchan->aprsObj()->is<GPSSystem>()) {
      setPositioningSystemIndex(ctx.config()->posSystems()->indexOfGPSSys(dchan->aprsObj()->as<GPSSystem>())+1);
      enableTXGPSInfo(true); enableRXGPSInfo(false);
    }
  } else if (chan->is<AnalogChannel>()) {
    const AnalogChannel *achan = chan->as<const AnalogChannel>();
    setMode(MODE_ANALOG);
    setBandwidth(achan->bandwidth());
    setSquelch(achan->squelch());
    switch (achan->admit()) {
    case AnalogChannel::Admit::Always: setAdmitCriterion(ADMIT_ALWAYS); break;
    case AnalogChannel::Admit::Free: setAdmitCriterion(ADMIT_CH_FREE); break;
    case AnalogChannel::Admit::Tone: setAdmitCriterion(ADMIT_TONE); break;
    }
    setRXSignaling(achan->rxTone());
    setTXSignaling(achan->txTone());
    setGroupListIndex(0);
    setContactIndex(0);
  }
}


/* ******************************************************************************************** *
 * Implementation of TyTCodeplug::VFOChannelElement
 * ******************************************************************************************** */
TyTCodeplug::VFOChannelElement::VFOChannelElement(uint8_t *ptr, size_t size)
  : ChannelElement(ptr, size)
{
  // pass...
}

TyTCodeplug::VFOChannelElement::VFOChannelElement(uint8_t *ptr)
  : ChannelElement(ptr, 0x0040)
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
TyTCodeplug::VFOChannelElement::setName(const QString &txt) {
  // pass...
}

uint
TyTCodeplug::VFOChannelElement::stepSize() const {
  return (getUInt8(32)+1)*2500;
}
void
TyTCodeplug::VFOChannelElement::setStepSize(uint ss_Hz) {
  ss_Hz = std::min(50000U, std::max(ss_Hz, 2500U));
  setUInt8(32, ss_Hz/2500-1);
  setUInt8(33, 0xff);
}


/* ******************************************************************************************** *
 * Implementation of TyTCodeplug::ContactElement
 * ******************************************************************************************** */
TyTCodeplug::ContactElement::ContactElement(uint8_t *ptr, size_t size)
  : CodePlug::Element(ptr, size)
{
  // pass...
}

TyTCodeplug::ContactElement::ContactElement(uint8_t *ptr)
  : CodePlug::Element(ptr, 0x0024)
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
  enableRingTone(false);
  setBit(3,6, 1); setBit(3,7, 1); // unknown = 1
  memset(_data+0x04, 0x00, 2*16);
}

uint32_t
TyTCodeplug::ContactElement::dmrId() const {
  return getUInt24_le(0);
}

void
TyTCodeplug::ContactElement::setDMRId(uint32_t id) {
  setUInt24_le(0, id);
}

bool
TyTCodeplug::ContactElement::ringTone() const {
  return getBit(3, 5);
}
void
TyTCodeplug::ContactElement::enableRingTone(bool enable) {
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
TyTCodeplug::ContactElement::setCallType(DigitalContact::Type type) {
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
TyTCodeplug::ContactElement::setName(const QString &nm) {
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

  setDMRId(cont->number());
  setName(cont->name());
  setCallType(cont->type());
  enableRingTone(cont->ring());

  return true;
}


/* ******************************************************************************************** *
 * Implementation of TyTCodeplug::ZoneElement
 * ******************************************************************************************** */
TyTCodeplug::ZoneElement::ZoneElement(uint8_t *ptr, size_t size)
  : CodePlug::Element(ptr, size)
{
  // pass...
}

TyTCodeplug::ZoneElement::ZoneElement(uint8_t *ptr)
  : CodePlug::Element(ptr, 0x0040)
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
TyTCodeplug::ZoneElement::setName(const QString &name) {
  writeUnicode(0, name, 16);
}

uint16_t
TyTCodeplug::ZoneElement::memberIndex(uint n) const {
  return getUInt16_le(0x20 + n*2);
}
void
TyTCodeplug::ZoneElement::setMemberIndex(uint n, uint16_t idx) {
  setUInt16_le(0x20 + n*2, idx);
}

bool
TyTCodeplug::ZoneElement::fromZoneObj(const Zone *zone, const CodeplugContext &ctx) {
  setName(zone->name());
  for (int i=0; i<16; i++) {
    if (i < zone->A()->count())
      setMemberIndex(i, ctx.config()->channelList()->indexOf(zone->A()->get(i))+1);
    else
      setMemberIndex(i, 0);
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
    zone->A()->add(ctx.getChannel(memberIndex(i)));
  }

  return true;
}


/* ******************************************************************************************** *
 * Implementation of TyTCodeplug::ZoneElement
 * ******************************************************************************************** */
TyTCodeplug::ZoneExtElement::ZoneExtElement(uint8_t *ptr, size_t size)
  : CodePlug::Element(ptr, size)
{
  // pass...
}

TyTCodeplug::ZoneExtElement::ZoneExtElement(uint8_t *ptr)
  : CodePlug::Element(ptr, 0x00e0)
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
TyTCodeplug::ZoneExtElement::setMemberIndexA(uint n, uint16_t idx) {
  setUInt16_le(0x00 + 2*n, idx);
}

uint16_t
TyTCodeplug::ZoneExtElement::memberIndexB(uint n) const {
  return getUInt16_le(0x60 + 2*n);
}

void
TyTCodeplug::ZoneExtElement::setMemberIndexB(uint n, uint16_t idx) {
  setUInt16_le(0x60 + 2*n, idx);
}

bool
TyTCodeplug::ZoneExtElement::fromZoneObj(const Zone *zone, const CodeplugContext &ctx) {
  // Store remaining channels from list A
  for (int i=16; i<64; i++) {
    if (i < zone->A()->count())
      setMemberIndexA(i-16, ctx.config()->channelList()->indexOf(zone->A()->get(i))+1);
    else
      setMemberIndexA(i-16, 0);
  }
  // Store channel from list B
  for (int i=0; i<64; i++) {
    if (i < zone->B()->count())
      setMemberIndexB(i, ctx.config()->channelList()->indexOf(zone->B()->get(i))+1);
    else
      setMemberIndexB(i, 0);
  }

  return true;
}

bool
TyTCodeplug::ZoneExtElement::linkZoneObj(Zone *zone, const CodeplugContext &ctx) {
  for (int i=0; (i<48) && memberIndexA(i); i++) {
    if (! ctx.hasChannel(memberIndexA(i))) {
      logWarn() << "Cannot link zone extension: Channel index " << memberIndexA(i) << " not defined.";
      return false;
    }
    zone->A()->add(ctx.getChannel(memberIndexA(i)));
  }

  for (int i=0; (i<64) && memberIndexB(i); i++) {
    if (! ctx.hasChannel(memberIndexB(i))) {
      logWarn() << "Cannot link zone extension: Channel index " << memberIndexB(i) << " not defined.";
      return false;
    }
    zone->B()->add(ctx.getChannel(memberIndexB(i)));
  }

  return true;
}


/* ******************************************************************************************** *
 * Implementation of TyTCodeplug::GroupListElement
 * ******************************************************************************************** */
TyTCodeplug::GroupListElement::GroupListElement(uint8_t *ptr, size_t size)
  : CodePlug::Element(ptr, size)
{
  // pass...
}

TyTCodeplug::GroupListElement::GroupListElement(uint8_t *ptr)
  : CodePlug::Element(ptr, 0x0060)
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
TyTCodeplug::GroupListElement::setName(const QString &nm) {
  writeUnicode(0, nm, 16);
}

uint16_t
TyTCodeplug::GroupListElement::memberIndex(uint n) const {
  return getUInt16_le(0x20 + n*2);
}

void
TyTCodeplug::GroupListElement::setMemberIndex(uint n, uint16_t idx) {
  setUInt16_le(0x20 + 2*n, idx);
}

bool
TyTCodeplug::GroupListElement::fromGroupListObj(const RXGroupList *lst, const CodeplugContext &ctx) {
  logDebug() << "Encode group list '" << lst->name() << "' with " << lst->count() << " members.";
  setName(lst->name());
  for (int i=0; i<32; i++) {
    if (i<lst->count()) {
      logDebug() << "Add contact " << lst->contact(i)->name() << " to list.";
      setMemberIndex(i, ctx.config()->contacts()->indexOfDigital(lst->contact(i)) + 1);
    } else {
      setMemberIndex(i, 0);
    }
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
    logDebug() << "Add contact idx=" << memberIndex(i) << " to group list " << lst->name() << ".";
    if (0 > lst->addContact(ctx.getDigitalContact(memberIndex(i)))) {
      logWarn() << "Cannot add contact '" << ctx.getDigitalContact(memberIndex(i))->name()
                << "' at idx=" << memberIndex(i) << ".";
      continue;
    }
  }

  return true;
}


/* ******************************************************************************************** *
 * Implementation of TyTCodeplug::ScanListElement
 * ******************************************************************************************** */
TyTCodeplug::ScanListElement::ScanListElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

TyTCodeplug::ScanListElement::ScanListElement(uint8_t *ptr)
  : Element(ptr, 0x0068)
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
  setPriorityChannel1Index(0xffff);
  setPriorityChannel2Index(0xffff);
  setTXChannelIndex(0xffff);
  setUInt8(0x26, 0xf1);
  setHoldTime(500);
  setPrioritySampleTime(2000);
  setUInt8(0x29, 0xff);
  memset(_data+0x2a, 0, 2*31);
}

QString
TyTCodeplug::ScanListElement::name() const {
  return readUnicode(0, 16);
}

void
TyTCodeplug::ScanListElement::setName(const QString &name) {
  writeUnicode(0, name, 16);
}

uint16_t
TyTCodeplug::ScanListElement::priorityChannel1Index() const {
  return getUInt16_le(0x20);
}

void
TyTCodeplug::ScanListElement::setPriorityChannel1Index(uint16_t idx) {
  setUInt16_le(0x20, idx);
}

uint16_t
TyTCodeplug::ScanListElement::priorityChannel2Index() const {
  return getUInt16_le(0x22);
}

void
TyTCodeplug::ScanListElement::setPriorityChannel2Index(uint16_t idx) {
  setUInt16_le(0x22, idx);
}

uint16_t
TyTCodeplug::ScanListElement::txChannelIndex() const {
  return getUInt16_le(0x24);
}

void
TyTCodeplug::ScanListElement::setTXChannelIndex(uint16_t idx) {
  setUInt16_le(0x24, idx);
}

uint
TyTCodeplug::ScanListElement::holdTime() const {
  return uint(getUInt8(0x27))*25;
}

void
TyTCodeplug::ScanListElement::setHoldTime(uint time) {
  setUInt8(0x27, time/25);
}

uint
TyTCodeplug::ScanListElement::prioritySampleTime() const {
  return uint(getUInt8(0x27))*250;
}

void
TyTCodeplug::ScanListElement::setPrioritySampleTime(uint time) {
  setUInt8(0x27, time/250);
}

uint16_t
TyTCodeplug::ScanListElement::memberIndex(uint n) const {
  return getUInt16_le(0x2a + 2*n);
}

void
TyTCodeplug::ScanListElement::setMemberIndex(uint n, uint16_t idx) {
  setUInt16_le(0x2a + 2*n, idx);
}

bool
TyTCodeplug::ScanListElement::fromScanListObj(const ScanList *lst, const CodeplugContext &ctx) {
  // Set name
  setName(lst->name());

  // Set priority channel 1
  if (lst->primaryChannel() && (SelectedChannel::get() == lst->primaryChannel()))
    setPriorityChannel1Index(0);
  else if (lst->primaryChannel())
    setPriorityChannel1Index(ctx.config()->channelList()->indexOf(lst->primaryChannel())+1);

  // Set priority channel 2
  if (lst->secondaryChannel() && (SelectedChannel::get() == lst->secondaryChannel()))
    setPriorityChannel2Index(0);
  else if (lst->secondaryChannel())
    setPriorityChannel2Index(ctx.config()->channelList()->indexOf(lst->secondaryChannel())+1);

  // Set transmit channel
  if (lst->revertChannel() && (SelectedChannel::get() == lst->revertChannel()))
    setTXChannelIndex(0);
  else if (lst->revertChannel())
    setTXChannelIndex(ctx.config()->channelList()->indexOf(lst->revertChannel())+1);

  for (int i=0, j=0; i<31;) {
    if (j >= lst->count()) {
      setMemberIndex(i++, 0);
    } else if (SelectedChannel::get() == lst->channel(j)) {
      logInfo() << "Cannot encode '" << lst->channel(j) << "' for UV390: skip.";
      j++;
    } else {
      setMemberIndex(i++, ctx.config()->channelList()->indexOf(lst->channel(j++))+1);
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
    lst->setPrimaryChannel(SelectedChannel::get());
  else if (ctx.hasChannel(priorityChannel1Index()))
    lst->setPrimaryChannel(ctx.getChannel(priorityChannel1Index()));
  else if (0xffff == priorityChannel1Index())
    lst->setPrimaryChannel(nullptr);
  else
    logWarn() << "Cannot deocde reference to priority channel index " << priorityChannel1Index()
                 << " in scan list '" << name() << "'.";

  if (0 == priorityChannel2Index())
    lst->setSecondaryChannel(SelectedChannel::get());
  else if (ctx.hasChannel(priorityChannel2Index()))
    lst->setSecondaryChannel(ctx.getChannel(priorityChannel2Index()));
  else if (0xffff == priorityChannel2Index())
    lst->setSecondaryChannel(nullptr);
  else
    logWarn() << "Cannot deocde reference to secondary priority channel index " << priorityChannel2Index()
              << " in scan list '" << name() << "'.";

  if (0 == txChannelIndex())
    lst->setRevertChannel(SelectedChannel::get());
  else if (ctx.hasChannel(txChannelIndex()))
    lst->setRevertChannel(ctx.getChannel(txChannelIndex()));
  else if (0xffff == priorityChannel2Index())
    lst->setSecondaryChannel(nullptr);
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
TyTCodeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

TyTCodeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr)
  : Element(ptr, 0x00b0)
{
  // pass...
}

TyTCodeplug::GeneralSettingsElement::~GeneralSettingsElement() {
  // pass...
}

void
TyTCodeplug::GeneralSettingsElement::clear() {
  setIntroLine1("");
  setIntroLine2("");
  memset(_data+0x26, 0xff, 0x1a);

  setBit(0x40,0, 0); setBit(0,1, 1);
  disableAllLEDs(false);
  setBit(0x40,3, 1);
  setMonitorType(MONITOR_OPEN_SQUELCH);
  setBit(0x40,5, 1);
  setTransmitMode(DESIGNED_AND_HAND_CH);

  setSavePreamble(true);
  setSaveModeRX(true);
  disableAllTones(false);
  setBit(0x41,3, 1);
  setChFreeIndicationTone(true);
  enablePasswdAndLock(true);
  enableTalkPermitToneDigital(false);
  enableTalkPermitToneAnalog(false);

  setBit(0x42,0, 0);
  enableChannelVoiceAnnounce(false);
  setBit(0x42,2, 0); setBit(0x42,3, 1);
  enableIntroPicture(true);
  enableKeypadTones(false);
  setBit(0x42,6, 1); setBit(0x42,7, 1);

  setBit(0x43,0, 1); setBit(0x43,1, 1);
  enableChannelModeA(true);
  setUInt4(0x43,3, 0xf);
  enableChannelModeB(true);

  setDMRId(0); setUInt8(0x47,0);

  setTXPreambleDuration(600);
  setGroupCallHangTime(3000);
  setPrivateCallHangTime(4000);
  setVOXSesitivity(3);
  setUInt8(0x4c, 0x00); setUInt8(0x4d, 0x00);
  setLowBatteryInterval(120);
  setCallAlertToneDuration(0);
  setLoneWorkerResponseTime(1);
  setLoneWorkerReminderTime(10);
  setUInt8(0x52, 0x00);
  setCcanDigitalHangTime(1000);
  setScanAnalogHangTime(1000);

  setBacklightTime(10);
  setUInt6(0x55, 2, 0);
  keypadLockTimeSetManual();
  enableChannelMode(true);

  setPowerOnPassword(0);
  radioProgPasswordDisable();
  pcProgPasswordDisable();
  setUInt24_le(0x68, 0xffffff);

  enableGroupCallMatch(false);
  enablePrivateCallMatch(false);
  setBit(0x6b, 2, 1);
  setTimeZone(QTimeZone::systemTimeZone());
  setUInt32_le(0x6c, 0xffffffff);

  setRadioName("");

  setChannelHangTime(3000);
  setUInt8(0x91, 0xff); setUInt2(0x92, 0, 0x03);
  enablePublicZone(true);
  setUInt5(0x92, 3, 0x1f);
  setUInt8(0x93, 0xff);
  setAdditionalDMRId(0, 1); setUInt8(0x97, 0);
  setAdditionalDMRId(1, 2); setUInt8(0x9b, 0);
  setAdditionalDMRId(2, 3); setUInt8(0x9f, 0);
  setUInt3(0xa0, 0, 0b111);
  setMICLevel(2);
  enableEditRadioID(true);
  setBit(0xa0, 7, true);
  memset(_data+0xa1, 0xff, 15);
}

QString
TyTCodeplug::GeneralSettingsElement::introLine1() const {
  return readUnicode(0, 10);
}
void
TyTCodeplug::GeneralSettingsElement::setIntroLine1(const QString line) {
  writeUnicode(0, line, 10);
}

QString
TyTCodeplug::GeneralSettingsElement::introLine2() const {
  return readUnicode(0x14, 10);
}
void
TyTCodeplug::GeneralSettingsElement::setIntroLine2(const QString line) {
  writeUnicode(0x14, line, 10);
}

TyTCodeplug::GeneralSettingsElement::TransmitMode
TyTCodeplug::GeneralSettingsElement::transmitMode() const {
  return TransmitMode(getUInt2(0x40,6));
}
void
TyTCodeplug::GeneralSettingsElement::setTransmitMode(TransmitMode mode) {
  setUInt2(0x40,6, mode);
}

TyTCodeplug::GeneralSettingsElement::MonitorType
TyTCodeplug::GeneralSettingsElement::monitorType() const {
  return getBit(0x40,4) ? MONITOR_OPEN_SQUELCH : MONITOR_SILENT;
}
void
TyTCodeplug::GeneralSettingsElement::setMonitorType(MonitorType type) {
  setBit(0x40,4, MONITOR_OPEN_SQUELCH==type);
}

bool
TyTCodeplug::GeneralSettingsElement::allLEDsDisabled() const {
  return ! getBit(0x40,2);
}
void
TyTCodeplug::GeneralSettingsElement::disableAllLEDs(bool disable) {
  setBit(0x40,2, !disable);
}

bool
TyTCodeplug::GeneralSettingsElement::savePreamble() const {
  return getBit(0x41,0);
}
void
TyTCodeplug::GeneralSettingsElement::setSavePreamble(bool enable) {
  setBit(0x41,0, enable);
}

bool
TyTCodeplug::GeneralSettingsElement::saveModeRX() const {
  return getBit(0x41,1);
}
void
TyTCodeplug::GeneralSettingsElement::setSaveModeRX(bool enable) {
  setBit(0x41,1, enable);
}

bool
TyTCodeplug::GeneralSettingsElement::allTonesDisabled() const {
  return ! getBit(0x41,2);
}
void
TyTCodeplug::GeneralSettingsElement::disableAllTones(bool disable) {
  setBit(0x41,2, !disable);
}

bool
TyTCodeplug::GeneralSettingsElement::chFreeIndicationTone() const {
  return !getBit(0x41,4);
}
void
TyTCodeplug::GeneralSettingsElement::setChFreeIndicationTone(bool enable) {
  setBit(0x41,4, !enable);
}

bool
TyTCodeplug::GeneralSettingsElement::passwdAndLock() const {
  return ! getBit(0x41,5);
}
void
TyTCodeplug::GeneralSettingsElement::enablePasswdAndLock(bool enable) {
  setBit(0x41,5, !enable);
}

bool
TyTCodeplug::GeneralSettingsElement::talkPermitToneDigital() const {
  return getBit(0x41,6);
}
void
TyTCodeplug::GeneralSettingsElement::enableTalkPermitToneDigital(bool enable) {
  setBit(0x41,6, enable);
}

bool
TyTCodeplug::GeneralSettingsElement::talkPermitToneAnalog() const {
  return getBit(0x41,7);
}
void
TyTCodeplug::GeneralSettingsElement::enableTalkPermitToneAnalog(bool enable) {
  setBit(0x41,7, enable);
}

bool
TyTCodeplug::GeneralSettingsElement::channelVoiceAnnounce() const {
  return getBit(0x42,1);
}
void
TyTCodeplug::GeneralSettingsElement::enableChannelVoiceAnnounce(bool enable) {
  setBit(0x42,1, enable);
}

bool
TyTCodeplug::GeneralSettingsElement::introPicture() const {
  return getBit(0x42,4);
}
void
TyTCodeplug::GeneralSettingsElement::enableIntroPicture(bool enable) {
  setBit(0x42,4, enable);
}

bool
TyTCodeplug::GeneralSettingsElement::keypadTones() const {
  return getBit(0x42,5);
}
void
TyTCodeplug::GeneralSettingsElement::enableKeypadTones(bool enable) {
  setBit(0x42,5, enable);
}

bool
TyTCodeplug::GeneralSettingsElement::channelModeA() const {
  return getBit(0x43,3);
}
void
TyTCodeplug::GeneralSettingsElement::enableChannelModeA(bool enable) {
  setBit(0x43,3, enable);
}

bool
TyTCodeplug::GeneralSettingsElement::channelModeB() const {
  return getBit(0x43,7);
}
void
TyTCodeplug::GeneralSettingsElement::enableChannelModeB(bool enable) {
  setBit(0x43,7, enable);
}

uint32_t
TyTCodeplug::GeneralSettingsElement::dmrId() const {
  return getUInt24_le(0x44);
}
void
TyTCodeplug::GeneralSettingsElement::setDMRId(uint32_t id) {
  setUInt24_le(0x44, id);
}

uint
TyTCodeplug::GeneralSettingsElement::txPreambleDuration() const {
  return uint(getUInt8(0x48))*60;
}
void
TyTCodeplug::GeneralSettingsElement::setTXPreambleDuration(uint dur) {
  dur = std::min(8640U, dur);
  setUInt8(0x48, dur/60);
}

uint
TyTCodeplug::GeneralSettingsElement::groupCallHangTime() const {
  return uint(getUInt8(0x49))*100;
}
void
TyTCodeplug::GeneralSettingsElement::setGroupCallHangTime(uint dur) {
  dur = std::min(7000U, dur);
  setUInt8(0x49, dur/100);
}

uint
TyTCodeplug::GeneralSettingsElement::privateCallHangTime() const {
  return uint(getUInt8(0x4a))*100;
}
void
TyTCodeplug::GeneralSettingsElement::setPrivateCallHangTime(uint dur) {
  dur = std::min(7000U, dur);
  setUInt8(0x4a, dur/100);
}

uint
TyTCodeplug::GeneralSettingsElement::voxSesitivity() const {
  return getUInt8(0x4b);
}
void
TyTCodeplug::GeneralSettingsElement::setVOXSesitivity(uint level) {
  level = std::min(10U, std::max(1U, level));
  setUInt8(0x4b, level);
}

uint
TyTCodeplug::GeneralSettingsElement::lowBatteryInterval() const {
  return uint(getUInt8(0x4e))*5;
}
void
TyTCodeplug::GeneralSettingsElement::setLowBatteryInterval(uint dur) {
  dur = std::min(635U, dur);
  setUInt8(0x4e, dur/5);
}

bool
TyTCodeplug::GeneralSettingsElement::callAlertToneIsContinuous() const {
  return 0==getUInt8(0x4f);
}
uint
TyTCodeplug::GeneralSettingsElement::callAlertToneDuration() const {
  return uint(getUInt8(0x4f))*5;
}
void
TyTCodeplug::GeneralSettingsElement::setCallAlertToneDuration(uint dur) {
  dur = std::min(1200U, dur);
  setUInt8(0x4f, dur/5);
}
void
TyTCodeplug::GeneralSettingsElement::setCallAlertToneContinuous() {
  setUInt8(0x4f, 0);
}

uint
TyTCodeplug::GeneralSettingsElement::loneWorkerResponseTime() const {
  return getUInt8(0x50);
}
void
TyTCodeplug::GeneralSettingsElement::setLoneWorkerResponseTime(uint dur) {
  setUInt8(0x50, dur);
}

uint
TyTCodeplug::GeneralSettingsElement::loneWorkerReminderTime() const {
  return getUInt8(0x51);
}
void
TyTCodeplug::GeneralSettingsElement::setLoneWorkerReminderTime(uint dur) {
  setUInt8(0x51, dur);
}

uint
TyTCodeplug::GeneralSettingsElement::scanDigitalHangTime() const {
  return uint(getUInt8(0x53))*100;
}
void
TyTCodeplug::GeneralSettingsElement::setCcanDigitalHangTime(uint dur) {
  dur = std::min(10000U, dur);
  setUInt8(0x53, dur/100);
}

uint
TyTCodeplug::GeneralSettingsElement::scanAnalogHangTime() const {
  return uint(getUInt8(0x54))*100;
}
void
TyTCodeplug::GeneralSettingsElement::setScanAnalogHangTime(uint dur) {
  dur = std::min(10000U, dur);
  setUInt8(0x54, dur/100);
}

bool
TyTCodeplug::GeneralSettingsElement::backlightIsAlways() const {
  return 0 == getUInt2(0x55, 0);
}
uint
TyTCodeplug::GeneralSettingsElement::backlightTime() const {
  return getUInt2(0x55, 0)*5;
}
void
TyTCodeplug::GeneralSettingsElement::setBacklightTime(uint sec) {
  sec = std::min(15U, sec);
  setUInt2(0x55, 0, sec/5);
}
void
TyTCodeplug::GeneralSettingsElement::backlightTimeSetAlways() {
  setUInt2(0x55, 0, 0);
}

bool
TyTCodeplug::GeneralSettingsElement::keypadLockIsManual() const {
  return 0xff == getUInt8(0x56);
}
uint
TyTCodeplug::GeneralSettingsElement::keypadLockTime() const {
  return uint(getUInt8(0x56))*5;
}
void
TyTCodeplug::GeneralSettingsElement::setKeypadLockTime(uint sec) {
  sec = std::max(1U, std::min(15U, sec));
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
TyTCodeplug::GeneralSettingsElement::enableChannelMode(bool enable) {
  setUInt8(0x57, enable ? 0xff : 0x00);
}

uint32_t
TyTCodeplug::GeneralSettingsElement::powerOnPassword() const {
  return getBCD8_le(0x58);
}
void
TyTCodeplug::GeneralSettingsElement::setPowerOnPassword(uint32_t pass) {
  setBCD8_le(0x58, pass);
}

bool
TyTCodeplug::GeneralSettingsElement::radioProgPasswordEnabled() const {
  return 0xffffffff != getUInt32_le(0x5c);
}
uint32_t
TyTCodeplug::GeneralSettingsElement::radioProgPassword() const {
  return getBCD8_le(0x5c);
}
void
TyTCodeplug::GeneralSettingsElement::setRadioProgPassword(uint32_t passwd) {
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
TyTCodeplug::GeneralSettingsElement::setPCProgPassword(const QString &pass) {
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
TyTCodeplug::GeneralSettingsElement::enableGroupCallMatch(bool enable) {
  setBit(0x6b, 0, enable);
}
bool
TyTCodeplug::GeneralSettingsElement::privateCallMatch() const {
  return getBit(0x6b, 1);
}
void
TyTCodeplug::GeneralSettingsElement::enablePrivateCallMatch(bool enable) {
  setBit(0x6b, 1, enable);
}

QTimeZone
TyTCodeplug::GeneralSettingsElement::timeZone() const {
  return QTimeZone((int(getUInt5(0x6b, 3))-12)*3600);
}
void
TyTCodeplug::GeneralSettingsElement::setTimeZone(const QTimeZone &zone) {
  int idx = (zone.standardTimeOffset(QDateTime::currentDateTime())/3600)+12;
  setUInt5(0x6b, 3, uint8_t(idx));
}

QString
TyTCodeplug::GeneralSettingsElement::radioName() const {
  return readUnicode(0x70, 16);
}
void
TyTCodeplug::GeneralSettingsElement::setRadioName(const QString &name) {
  writeUnicode(0x70, name, 16);
}

uint
TyTCodeplug::GeneralSettingsElement::channelHangTime() const {
  return uint(getUInt8(0x90))*100;
}
void
TyTCodeplug::GeneralSettingsElement::setChannelHangTime(uint dur) {
  setUInt8(0x90, dur/100);
}

bool
TyTCodeplug::GeneralSettingsElement::publicZone() const {
  return getBit(0x92, 2);
}
void
TyTCodeplug::GeneralSettingsElement::enablePublicZone(bool enable) {
  setBit(0x92, 2, enable);
}

uint32_t
TyTCodeplug::GeneralSettingsElement::additionalDMRId(uint n) const {
  return getUInt24_le(0x94+4*n);
}
void
TyTCodeplug::GeneralSettingsElement::setAdditionalDMRId(uint n, uint32_t id) {
  setUInt24_le(0x94+4*n, id);
}

uint
TyTCodeplug::GeneralSettingsElement::micLevel() const {
  return (uint(getUInt3(0xa0,3)+1)*100)/60;
}
void
TyTCodeplug::GeneralSettingsElement::setMICLevel(uint level) {
  setUInt3(0xa0,3, ((level-1)*60)/100);
}

bool
TyTCodeplug::GeneralSettingsElement::editRadioID() const {
  return !getBit(0xa0, 6);
}
void
TyTCodeplug::GeneralSettingsElement::enableEditRadioID(bool enable) {
  setBit(0xa0,6, !enable);
}

bool
TyTCodeplug::GeneralSettingsElement::fromConfig(const Config *config) {
  if (nullptr == config->radioIDs()->defaultId())
    return false;
  setRadioName(config->radioIDs()->defaultId()->name());
  setDMRId(config->radioIDs()->defaultId()->number());

  setIntroLine1(config->introLine1());
  setIntroLine2(config->introLine2());
  setTimeZone(QTimeZone::systemTimeZone());
  setMICLevel(config->micLevel());
  enableChannelVoiceAnnounce(config->speech());

  return true;
}

bool
TyTCodeplug::GeneralSettingsElement::updateConfig(Config *config) {
  int idx = config->radioIDs()->addId(radioName(),dmrId());
  config->radioIDs()->setDefaultId(idx);
  config->setIntroLine1(introLine1());
  config->setIntroLine2(introLine2());
  config->setMicLevel(micLevel());
  config->setSpeech(channelVoiceAnnounce());
  return true;
}


/* ******************************************************************************************** *
 * Implementation of TyTCodeplug::BootSettingsElement
 * ******************************************************************************************** */
TyTCodeplug::BootSettingsElement::BootSettingsElement(uint8_t *ptr, size_t size)
  : CodePlug::Element(ptr, size)
{
  // pass...
}

TyTCodeplug::BootSettingsElement::BootSettingsElement(uint8_t *ptr)
  : CodePlug::Element(ptr, 0x0010)
{
  // pass...
}

TyTCodeplug::BootSettingsElement::~BootSettingsElement() {
  // pass...
}

void
TyTCodeplug::BootSettingsElement::clear() {
  setUInt24_le(0, 0xffffff);
  setZoneIndex(1);
  setChannelIndexA(1);
  setUInt8(0x05, 0xff);
  setChannelIndexB(1);
  setUInt16_le(0x07, 0xffff);
  setUInt16_le(0x09, 0x0001);
  setUInt8(0x0b, 0xff);
  setUInt32_le(0x0c, 0xffffffff);
}

uint
TyTCodeplug::BootSettingsElement::zoneIndex() const {
  return getUInt8(0x03);
}
void
TyTCodeplug::BootSettingsElement::setZoneIndex(uint idx) {
  setUInt8(0x03, idx);
}

uint
TyTCodeplug::BootSettingsElement::channelIndexA() const {
  return getUInt8(0x04);
}
void
TyTCodeplug::BootSettingsElement::setChannelIndexA(uint idx) {
  setUInt8(0x04, idx);
}

uint
TyTCodeplug::BootSettingsElement::channelIndexB() const {
  return getUInt8(0x06);
}
void
TyTCodeplug::BootSettingsElement::setChannelIndexB(uint idx) {
  setUInt8(0x06, idx);
}


/* ******************************************************************************************** *
 * Implementation of TyTCodeplug::TimestampElement
 * ******************************************************************************************** */
TyTCodeplug::TimestampElement::TimestampElement(uint8_t *ptr, size_t size)
  : CodePlug::Element(ptr, size)
{
  // pass...
}

TyTCodeplug::TimestampElement::TimestampElement(uint8_t *ptr)
  : CodePlug::Element(ptr, 0x0c)
{
  // pass...
}

TyTCodeplug::TimestampElement::~TimestampElement() {
  // pass...
}

void
TyTCodeplug::TimestampElement::clear() {
  setUInt8(0x00, 0);
  setTimestamp(QDateTime::currentDateTime());
  setUInt32_be(0x08, 0x00010300);
}

QDateTime
TyTCodeplug::TimestampElement::timestamp() const {
  return QDateTime(QDate(getBCD4_le(0x01), getBCD2(0x03), getBCD2(0x04)),
                   QTime(getBCD2(0x05), getBCD2(0x06), getBCD2(0x07)));
}
void
TyTCodeplug::TimestampElement::setTimestamp(const QDateTime &ts) {
  setBCD4_le(0x01, ts.date().year());
  setBCD2(0x03, ts.date().month());
  setBCD2(0x04, ts.date().day());
  setBCD2(0x05, ts.time().hour());
  setBCD2(0x06, ts.time().minute());
  setBCD2(0x07, ts.time().second());
}

QString
TyTCodeplug::TimestampElement::cpsVersion() const {
  const char table[] = "0123456789:;<=>?";

  QString v;
  v.append(table[std::min(uint8_t(15), getUInt8(0x08))]);
  v.append(table[std::min(uint8_t(15), getUInt8(0x09))]);
  v.append(".");
  v.append(table[std::min(uint8_t(15), getUInt8(0x0a))]);
  v.append(table[std::min(uint8_t(15), getUInt8(0x0b))]);

  return v;
}


/* ******************************************************************************************** *
 * Implementation of TyTCodeplug::GPSSystemElement
 * ******************************************************************************************** */
TyTCodeplug::GPSSystemElement::GPSSystemElement(uint8_t *ptr, size_t size)
  : CodePlug::Element(ptr, size)
{
  // pass...
}

TyTCodeplug::GPSSystemElement::GPSSystemElement(uint8_t *ptr)
  : CodePlug::Element(ptr, 0x10)
{
  // pass...
}

TyTCodeplug::GPSSystemElement::~GPSSystemElement() {
  // pass...
}

bool
TyTCodeplug::GPSSystemElement::isValid() const {
  return Element::isValid() && (! repeatIntervalDisabled()) &&
      (0xffff != revertChannelIndex());
}

void
TyTCodeplug::GPSSystemElement::clear() {
  setRevertChannelIndex(0xffff);
  disableRepeatInterval();
  setUInt8(0x03, 0xff);
  disableDestinationContact();
  memset(_data + 0x06, 0xff, 10);
}

bool
TyTCodeplug::GPSSystemElement::revertChannelIsSelected() const {
  return 0 == getUInt16_le(0x00);
}
uint16_t
TyTCodeplug::GPSSystemElement::revertChannelIndex() const {
  return getUInt16_le(0x00);
}
void
TyTCodeplug::GPSSystemElement::setRevertChannelIndex(uint16_t idx) {
  setUInt16_le(0x00, idx);
}
void
TyTCodeplug::GPSSystemElement::setRevertChannelSelected() {
  setUInt16_le(0x00, 0);
}

bool
TyTCodeplug::GPSSystemElement::repeatIntervalDisabled() const {
  return 0 == getUInt8(0x02);
}
uint
TyTCodeplug::GPSSystemElement::repeatInterval() const {
  return uint(getUInt8(0x02))*30;
}
void
TyTCodeplug::GPSSystemElement::setRepeatInterval(uint dur) {
  setUInt8(0x02, dur/30);
}
void
TyTCodeplug::GPSSystemElement::disableRepeatInterval() {
  setUInt8(0x02, 0);
}

bool
TyTCodeplug::GPSSystemElement::destinationContactDisabled() const {
  return 0 == getUInt16_le(0x04);
}
uint16_t
TyTCodeplug::GPSSystemElement::destinationContactIndex() const {
  return getUInt16_le(0x04);
}
void
TyTCodeplug::GPSSystemElement::setDestinationContactIndex(uint16_t idx) {
  setUInt16_le(0x04, idx);
}
void
TyTCodeplug::GPSSystemElement::disableDestinationContact() {
  setUInt16_le(0x04, 0);
}

bool
TyTCodeplug::GPSSystemElement::fromGPSSystemObj(const GPSSystem *sys, const CodeplugContext &ctx) {
  clear();
  if (sys->hasContact())
    setDestinationContactIndex(ctx.config()->posSystems()->indexOfGPSSys(sys)+1);
  if (sys->hasRevertChannel())
    setRevertChannelIndex(ctx.config()->channelList()->indexOf(sys->revertChannel())+1);
  setRepeatInterval(sys->period());
  return true;
}

GPSSystem *
TyTCodeplug::GPSSystemElement::toGPSSystemObj() {
  return new GPSSystem("GPS System", nullptr, nullptr, repeatInterval());
}

bool
TyTCodeplug::GPSSystemElement::linkGPSSystemObj(GPSSystem *sys, const CodeplugContext &ctx) {
  if (! isValid())
    return false;

  if ((! destinationContactDisabled()) && (ctx.hasDigitalContact(destinationContactIndex())))
    sys->setContact(ctx.getDigitalContact(destinationContactIndex()));

  if (revertChannelIsSelected())
    sys->setRevertChannel(nullptr);
  else if (ctx.hasChannel(revertChannelIndex()) && ctx.getChannel(revertChannelIndex())->is<DigitalChannel>())
    sys->setRevertChannel(ctx.getChannel(revertChannelIndex())->as<DigitalChannel>());

  return true;
}


/* ******************************************************************************************** *
 * Implementation of TyTCodeplug::MenuSettingsElement
 * ******************************************************************************************** */
TyTCodeplug::MenuSettingsElement::MenuSettingsElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

TyTCodeplug::MenuSettingsElement::MenuSettingsElement(uint8_t *ptr)
  : Element(ptr, 0x10)
{
  // pass...
}

TyTCodeplug::MenuSettingsElement::~MenuSettingsElement() {
  // pass...
}

void
TyTCodeplug::MenuSettingsElement::clear() {
  setMenuHangtime(0);

  enableTextMessage(true);
  enableCallAlert(true);
  enableContactEditing(true);
  enableManualDial(true);
  enableContactRadioCheck(false);
  enableRemoteMonitor(false);
  enableRadioEnable(false);
  enableRadioDisable(false);

  setBit(0x02, 0, false);
  enableScan(true);
  enableEditScanlist(true);
  enableCallLogMissed(true);
  enableCallLogAnswered(true);
  enableCallLogOutgoing(true);
  enableTalkaround(false);
  enableToneAlert(true);

  enablePower(true);
  enableBacklight(true);
  enableIntroScreen(true);
  enableKeypadLock(true);
  enableLEDIndicator(true);
  enableSquelch(true);
  setBit(0x03, 6, false);
  enableVOX(true);

  enablePassword(false);
  enableDisplayMode(true);
  enableProgramRadio(true);
  enableGPSSettings(true);
  enableGPSInformation(true);
  enableRecording(true);
  setBit(0x04, 6, true);
  setBit(0x04, 7, true);

  setBit(0x05, 0, true);
  setBit(0x05, 1, true);
  enableGroupCallMatch(true);
  enablePrivateCallMatch(true);
  enableMenuHangtimeItem(true);
  enableTXMode(true);
  enableZoneSettings(true);
  enableNewZone(true);

  enableEditZone(true);
  enableNewScanList(true);
  setUInt6(6, 2, 0b111111);

  memset(_data+0x07, 0xff, 9);
}

bool
TyTCodeplug::MenuSettingsElement::menuHangtimeIsInfinite() const {
  return 0 == menuHangtime();
}
uint
TyTCodeplug::MenuSettingsElement::menuHangtime() const {
  return getUInt8(0x00);
}
void
TyTCodeplug::MenuSettingsElement::setMenuHangtime(uint sec) {
  setUInt8(0x00, sec);
}
void
TyTCodeplug::MenuSettingsElement::infiniteMenuHangtime() {
  setUInt8(0x00, 0x00);
}

bool
TyTCodeplug::MenuSettingsElement::textMessage() const {
  return getBit(0x01, 0);
}
void
TyTCodeplug::MenuSettingsElement::enableTextMessage(bool enable) {
  setBit(0x01, 0, enable);
}

bool
TyTCodeplug::MenuSettingsElement::callAlert() const {
  return getBit(0x01, 1);
}
void
TyTCodeplug::MenuSettingsElement::enableCallAlert(bool enable) {
  setBit(0x01, 1, enable);
}

bool
TyTCodeplug::MenuSettingsElement::contactEditing() const {
  return getBit(0x01, 2);
}
void
TyTCodeplug::MenuSettingsElement::enableContactEditing(bool enable) {
  setBit(0x01, 2, enable);
}

bool
TyTCodeplug::MenuSettingsElement::manualDial() const {
  return getBit(0x01, 3);
}
void
TyTCodeplug::MenuSettingsElement::enableManualDial(bool enable) {
  setBit(0x01, 3, enable);
}

bool
TyTCodeplug::MenuSettingsElement::contactRadioCheck() const {
  return getBit(0x01, 4);
}
void
TyTCodeplug::MenuSettingsElement::enableContactRadioCheck(bool enable) {
  setBit(0x01, 4, enable);
}

bool
TyTCodeplug::MenuSettingsElement::remoteMonitor() const {
  return getBit(0x01, 5);
}
void
TyTCodeplug::MenuSettingsElement::enableRemoteMonitor(bool enable) {
  setBit(0x01, 5, enable);
}

bool
TyTCodeplug::MenuSettingsElement::radioEnable() const {
  return getBit(0x01, 6);
}
void
TyTCodeplug::MenuSettingsElement::enableRadioEnable(bool enable) {
  setBit(0x01, 6, enable);
}

bool
TyTCodeplug::MenuSettingsElement::radioDisable() const {
  return getBit(0x01, 7);
}
void
TyTCodeplug::MenuSettingsElement::enableRadioDisable(bool enable) {
  setBit(0x01, 7, enable);
}

bool
TyTCodeplug::MenuSettingsElement::scan() const {
  return getBit(0x02, 1);
}
void
TyTCodeplug::MenuSettingsElement::enableScan(bool enable) {
  setBit(0x02, 1, enable);
}

bool
TyTCodeplug::MenuSettingsElement::editScanlist() const {
  return getBit(0x02, 2);
}
void
TyTCodeplug::MenuSettingsElement::enableEditScanlist(bool enable) {
  setBit(0x02, 2, enable);
}

bool
TyTCodeplug::MenuSettingsElement::callLogMissed() const {
  return getBit(0x02, 3);
}
void
TyTCodeplug::MenuSettingsElement::enableCallLogMissed(bool enable) {
  setBit(0x02, 3, enable);
}

bool
TyTCodeplug::MenuSettingsElement::callLogAnswered() const {
  return getBit(0x02, 4);
}
void
TyTCodeplug::MenuSettingsElement::enableCallLogAnswered(bool enable) {
  setBit(0x02, 4, enable);
}

bool
TyTCodeplug::MenuSettingsElement::callLogOutgoing() const {
  return getBit(0x02, 5);
}
void
TyTCodeplug::MenuSettingsElement::enableCallLogOutgoing(bool enable) {
  setBit(0x02, 5, enable);
}

bool
TyTCodeplug::MenuSettingsElement::talkaround() const {
  return getBit(0x02, 6);
}
void
TyTCodeplug::MenuSettingsElement::enableTalkaround(bool enable) {
  setBit(0x02, 6, enable);
}

bool
TyTCodeplug::MenuSettingsElement::toneAlert() const {
  return getBit(0x02, 7);
}
void
TyTCodeplug::MenuSettingsElement::enableToneAlert(bool enable) {
  setBit(0x02, 7, enable);
}

bool
TyTCodeplug::MenuSettingsElement::power() const {
  return getBit(0x03, 0);
}
void
TyTCodeplug::MenuSettingsElement::enablePower(bool enable) {
  setBit(0x03, 0, enable);
}

bool
TyTCodeplug::MenuSettingsElement::backlight() const {
  return getBit(0x03, 1);
}
void
TyTCodeplug::MenuSettingsElement::enableBacklight(bool enable) {
  setBit(0x03, 1, enable);
}

bool
TyTCodeplug::MenuSettingsElement::introScreen() const {
  return getBit(0x03, 2);
}
void
TyTCodeplug::MenuSettingsElement::enableIntroScreen(bool enable) {
  setBit(0x03, 2, enable);
}

bool
TyTCodeplug::MenuSettingsElement::keypadLock() const {
  return getBit(0x03, 3);
}
void
TyTCodeplug::MenuSettingsElement::enableKeypadLock(bool enable) {
  setBit(0x03, 3, enable);
}

bool
TyTCodeplug::MenuSettingsElement::ledIndicator() const {
  return getBit(0x03, 4);
}
void
TyTCodeplug::MenuSettingsElement::enableLEDIndicator(bool enable) {
  setBit(0x03, 4, enable);
}

bool
TyTCodeplug::MenuSettingsElement::squelch() const {
  return getBit(0x03, 5);
}
void
TyTCodeplug::MenuSettingsElement::enableSquelch(bool enable) {
  setBit(0x03, 5, enable);
}

bool
TyTCodeplug::MenuSettingsElement::vox() const {
  return getBit(0x03, 7);
}
void
TyTCodeplug::MenuSettingsElement::enableVOX(bool enable) {
  setBit(0x03, 7, enable);
}

bool
TyTCodeplug::MenuSettingsElement::password() const {
  return getBit(0x04, 0);
}
void
TyTCodeplug::MenuSettingsElement::enablePassword(bool enable) {
  setBit(0x04, 0, enable);
}

bool
TyTCodeplug::MenuSettingsElement::displayMode() const {
  return getBit(0x04, 1);
}
void
TyTCodeplug::MenuSettingsElement::enableDisplayMode(bool enable) {
  setBit(0x04, 1, enable);
}

bool
TyTCodeplug::MenuSettingsElement::programRadio() const {
  return ! getBit(0x04, 2);
}
void
TyTCodeplug::MenuSettingsElement::enableProgramRadio(bool enable) {
  setBit(0x04, 2, !enable);
}

bool
TyTCodeplug::MenuSettingsElement::gpsSettings() const {
  return !getBit(0x04, 3);
}
void
TyTCodeplug::MenuSettingsElement::enableGPSSettings(bool enable) {
  setBit(0x04, 3, !enable);
}

bool
TyTCodeplug::MenuSettingsElement::gpsInformation() const {
  return !getBit(0x04, 4);
}
void
TyTCodeplug::MenuSettingsElement::enableGPSInformation(bool enable) {
  setBit(0x04, 4, !enable);
}

bool
TyTCodeplug::MenuSettingsElement::recording() const {
  return getBit(0x04, 5);
}
void
TyTCodeplug::MenuSettingsElement::enableRecording(bool enable) {
  setBit(0x04, 5, enable);
}

bool
TyTCodeplug::MenuSettingsElement::groupCallMatch() const {
  return getBit(0x05, 2);
}
void
TyTCodeplug::MenuSettingsElement::enableGroupCallMatch(bool enable) {
  setBit(0x05, 2, enable);
}

bool
TyTCodeplug::MenuSettingsElement::privateCallMatch() const {
  return getBit(0x05, 3);
}
void
TyTCodeplug::MenuSettingsElement::enablePrivateCallMatch(bool enable) {
  setBit(0x05, 3, enable);
}

bool
TyTCodeplug::MenuSettingsElement::menuHangtimeItem() const {
  return getBit(0x05, 4);
}
void
TyTCodeplug::MenuSettingsElement::enableMenuHangtimeItem(bool enable) {
  setBit(0x05, 4, enable);
}

bool
TyTCodeplug::MenuSettingsElement::txMode() const {
  return getBit(0x05, 5);
}
void
TyTCodeplug::MenuSettingsElement::enableTXMode(bool enable) {
  setBit(0x05, 5, enable);
}

bool
TyTCodeplug::MenuSettingsElement::zoneSettings() const {
  return getBit(0x05, 6);
}
void
TyTCodeplug::MenuSettingsElement::enableZoneSettings(bool enable) {
  setBit(0x05, 6, enable);
}

bool
TyTCodeplug::MenuSettingsElement::newZone() const {
  return getBit(0x05, 7);
}
void
TyTCodeplug::MenuSettingsElement::enableNewZone(bool enable) {
  setBit(0x05, 7, enable);
}

bool
TyTCodeplug::MenuSettingsElement::editZone() const {
  return getBit(0x06, 0);
}
void
TyTCodeplug::MenuSettingsElement::enableEditZone(bool enable) {
  setBit(0x06, 0, enable);
}

bool
TyTCodeplug::MenuSettingsElement::newScanList() const {
  return getBit(0x06, 1);
}
void
TyTCodeplug::MenuSettingsElement::enableNewScanList(bool enable) {
  setBit(0x06, 1, enable);
}


/* ******************************************************************************************** *
 * Implementation of TyTCodeplug::ButtonSettingsElement
 * ******************************************************************************************** */
TyTCodeplug::ButtonSettingsElement::ButtonSettingsElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

TyTCodeplug::ButtonSettingsElement::ButtonSettingsElement(uint8_t *ptr)
  : Element(ptr, 0x14) // <- size excluding one-touch settings
{
  // pass...
}

TyTCodeplug::ButtonSettingsElement::~ButtonSettingsElement() {
  // pass...
}

void
TyTCodeplug::ButtonSettingsElement::clear() {
  setUInt16_le(0x00, 0);
  setSideButton1Short(ButtonAction::Disabled);
  setSideButton1Long(ButtonAction::Tone1750Hz);
  setSideButton2Short(ButtonAction::MonitorToggle);
  setSideButton2Long(ButtonAction::Disabled);
  memset(_data+0x06, 0x00, 10);
  setUInt8(0x10, 1);
  setLongPressDuration(1000);
  setUInt16_le(0x12, 0xffff);
}

TyTCodeplug::ButtonSettingsElement::ButtonAction
TyTCodeplug::ButtonSettingsElement::sideButton1Short() const {
  return ButtonAction(getUInt8(0x02));
}
void
TyTCodeplug::ButtonSettingsElement::setSideButton1Short(ButtonAction action) {
  setUInt8(0x02, action);
}

TyTCodeplug::ButtonSettingsElement::ButtonAction
TyTCodeplug::ButtonSettingsElement::sideButton1Long() const {
  return ButtonAction(getUInt8(0x03));
}
void
TyTCodeplug::ButtonSettingsElement::setSideButton1Long(ButtonAction action) {
  setUInt8(0x03, action);
}

TyTCodeplug::ButtonSettingsElement::ButtonAction
TyTCodeplug::ButtonSettingsElement::sideButton2Short() const {
  return ButtonAction(getUInt8(0x04));
}
void
TyTCodeplug::ButtonSettingsElement::setSideButton2Short(ButtonAction action) {
  setUInt8(0x04, action);
}

TyTCodeplug::ButtonSettingsElement::ButtonAction
TyTCodeplug::ButtonSettingsElement::sideButton2Long() const {
  return ButtonAction(getUInt8(0x05));
}
void
TyTCodeplug::ButtonSettingsElement::setSideButton2Long(ButtonAction action) {
  setUInt8(0x05, action);
}

uint
TyTCodeplug::ButtonSettingsElement::longPressDuration() const {
  return uint(getUInt8(0x11))*250;
}
void
TyTCodeplug::ButtonSettingsElement::setLongPressDuration(uint ms) {
  setUInt8(0x11, ms/250);
}

bool
TyTCodeplug::ButtonSettingsElement::fromConfig(const Config *config) {
  // Skip if not defined
  if (! config->hasExtension(TyTButtonSettingsExtension::staticMetaObject.className()))
    return true;
  // Check type
  const TyTButtonSettingsExtension *ext =
      config->extension(TyTButtonSettingsExtension::staticMetaObject.className())->as<TyTButtonSettingsExtension>();
  if (nullptr == ext)
    return false;

  setSideButton1Short(ext->sideButton1Short());
  setSideButton1Long(ext->sideButton1Long());
  setSideButton2Short(ext->sideButton2Short());
  setSideButton2Long(ext->sideButton2Long());
  setLongPressDuration(ext->longPressDuration());

  return true;
}

bool
TyTCodeplug::ButtonSettingsElement::updateConfig(Config *config) {
  TyTButtonSettingsExtension *ext = new TyTButtonSettingsExtension(config);
  config->addExtension(TyTButtonSettingsExtension::staticMetaObject.className(), ext);

  ext->setSideButton1Short(sideButton1Short());
  ext->setSideButton1Long(sideButton1Long());
  ext->setSideButton2Short(sideButton2Short());
  ext->setSideButton2Long(sideButton2Long());
  ext->setLongPressDuration(longPressDuration());
  return true;
}


/* ******************************************************************************************** *
 * Implementation of TyTCodeplug::OneTouchSettingElement
 * ******************************************************************************************** */
TyTCodeplug::OneTouchSettingElement::OneTouchSettingElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

TyTCodeplug::OneTouchSettingElement::OneTouchSettingElement(uint8_t *ptr)
  : Element(ptr, 0x04)
{
  // pass....
}

TyTCodeplug::OneTouchSettingElement::~OneTouchSettingElement() {
  // pass...
}

bool
TyTCodeplug::OneTouchSettingElement::isValid() const {
  return Element::isValid() && (Disabled != actionType());
}

void
TyTCodeplug::OneTouchSettingElement::clear() {
  setAction(CALL);
  setActionType(Disabled);
  setUInt2(0x00, 6, 3);
  setMessageIndex(0);
  setContactIndex(0);
}

TyTCodeplug::OneTouchSettingElement::Action
TyTCodeplug::OneTouchSettingElement::action() const {
  return Action(getUInt4(0x00, 0));
}
void
TyTCodeplug::OneTouchSettingElement::setAction(Action action) {
  setUInt4(0x00, 0, action);
}

TyTCodeplug::OneTouchSettingElement::Type
TyTCodeplug::OneTouchSettingElement::actionType() const {
  return Type(getUInt2(0x00, 4));
}
void
TyTCodeplug::OneTouchSettingElement::setActionType(Type type) {
  setUInt2(0x00, 4, type);
}

uint8_t
TyTCodeplug::OneTouchSettingElement::messageIndex() const {
  return getUInt8(0x01);
}
void
TyTCodeplug::OneTouchSettingElement::setMessageIndex(uint8_t idx) {
  setUInt8(0x01, idx);
}

uint16_t
TyTCodeplug::OneTouchSettingElement::contactIndex() const {
  return getUInt16_le(0x02);
}
void
TyTCodeplug::OneTouchSettingElement::setContactIndex(uint16_t idx) {
  setUInt16_le(0x02, idx);
}


/* ******************************************************************************************** *
 * Implementation of TyTCodeplug::EmergencySettingsElement
 * ******************************************************************************************** */
TyTCodeplug::EmergencySettingsElement::EmergencySettingsElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

TyTCodeplug::EmergencySettingsElement::EmergencySettingsElement(uint8_t *ptr)
  : Element(ptr, 0x10)
{
  // pass...
}

TyTCodeplug::EmergencySettingsElement::~EmergencySettingsElement() {
  // pass...
}

void
TyTCodeplug::EmergencySettingsElement::clear() {
  setUInt5(0x00, 3, 0b11111);
  enableRadioDisable(true);
  enableRemoteMonitor(false);
  enableEmergencyRemoteMonitor(true);
  setRemoteMonitorDuration(10);
  setTXTimeOut(125);
  setMessageLimit(2);
  memset(_data+0x04, 0xff, 12);
}

bool
TyTCodeplug::EmergencySettingsElement::emergencyRemoteMonitor() const {
  return getBit(0x00, 2);
}
void
TyTCodeplug::EmergencySettingsElement::enableEmergencyRemoteMonitor(bool enable) {
  setBit(0x00, 2, enable);
}

bool
TyTCodeplug::EmergencySettingsElement::remoteMonitor() const {
  return getBit(0x00, 1);
}
void
TyTCodeplug::EmergencySettingsElement::enableRemoteMonitor(bool enable) {
  setBit(0x00, 1, enable);
}

bool
TyTCodeplug::EmergencySettingsElement::radioDisable() const {
  return getBit(0x00, 0);
}
void
TyTCodeplug::EmergencySettingsElement::enableRadioDisable(bool enable) {
  setBit(0x00, 0, enable);
}

uint
TyTCodeplug::EmergencySettingsElement::remoteMonitorDuration() const {
  return uint(getUInt8(0x01))*10;
}
void
TyTCodeplug::EmergencySettingsElement::setRemoteMonitorDuration(uint sec) {
  setUInt8(0x01, sec/10);
}

uint
TyTCodeplug::EmergencySettingsElement::txTimeOut() const {
  return uint(getUInt8(0x02))*25;
}
void
TyTCodeplug::EmergencySettingsElement::setTXTimeOut(uint ms) {
  setUInt8(0x02, ms/25);
}

uint
TyTCodeplug::EmergencySettingsElement::messageLimit() const {
  return uint(getUInt8(0x03));
}
void
TyTCodeplug::EmergencySettingsElement::setMessageLimit(uint limit) {
  setUInt8(0x03, limit);
}


/* ******************************************************************************************** *
 * Implementation of TyTCodeplug::EmergencySystemElement
 * ******************************************************************************************** */
TyTCodeplug::EmergencySystemElement::EmergencySystemElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

TyTCodeplug::EmergencySystemElement::EmergencySystemElement(uint8_t *ptr)
  : Element(ptr, 0x28)
{
  // pass...
}

TyTCodeplug::EmergencySystemElement::~EmergencySystemElement() {
  // pass...
}

bool
TyTCodeplug::EmergencySystemElement::isValid() const {
  return Element::isValid() && (0 != revertChannelIndex());
}

void
TyTCodeplug::EmergencySystemElement::clear() {
  setName("");
  setAlarmType(DISABLED);
  setUInt2(0x00, 2, 3);
  setAlarmMode(ALARM);
  setUInt2(0x00, 6, 1);
  setImpoliteRetries(15);
  setPoliteRetries(5);
  setHotMICDuration(100);
  setRevertChannelIndex(0);
  setUInt16_le(0x26, 0xffff);
}

QString
TyTCodeplug::EmergencySystemElement::name() const {
  return readUnicode(0x00, 16);
}
void
TyTCodeplug::EmergencySystemElement::setName(const QString &nm) {
  writeUnicode(0x00, nm, 16);
}

TyTCodeplug::EmergencySystemElement::AlarmType
TyTCodeplug::EmergencySystemElement::alarmType() const {
  return AlarmType(getUInt2(0x20, 0));
}
void
TyTCodeplug::EmergencySystemElement::setAlarmType(AlarmType type) {
  setUInt2(0x20, 0, type);
}

TyTCodeplug::EmergencySystemElement::AlarmMode
TyTCodeplug::EmergencySystemElement::alarmMode() const {
  return AlarmMode(getUInt2(0x20, 4));
}
void
TyTCodeplug::EmergencySystemElement::setAlarmMode(AlarmMode mode) {
  setUInt2(0x20, 4, mode);
}

uint
TyTCodeplug::EmergencySystemElement::impoliteRetries() const {
  return getUInt8(0x21);
}
void
TyTCodeplug::EmergencySystemElement::setImpoliteRetries(uint num) {
  setUInt8(0x21, num);
}

uint
TyTCodeplug::EmergencySystemElement::politeRetries() const {
  return getUInt8(0x22);
}
void
TyTCodeplug::EmergencySystemElement::setPoliteRetries(uint num) {
  setUInt8(0x22, num);
}

uint
TyTCodeplug::EmergencySystemElement::hotMICDuration() const {
  return uint(getUInt8(0x23))*10;
}
void
TyTCodeplug::EmergencySystemElement::setHotMICDuration(uint dur) {
  setUInt8(0x23, dur/10);
}

bool
TyTCodeplug::EmergencySystemElement::revertChannelIsSelected() const {
  return 0xffff == getUInt16_le(0x24);
}
uint16_t
TyTCodeplug::EmergencySystemElement::revertChannelIndex() const {
  return getUInt16_le(0x24);
}
void
TyTCodeplug::EmergencySystemElement::setRevertChannelIndex(uint16_t idx) {
  setUInt16_le(0x24, idx);
}
void
TyTCodeplug::EmergencySystemElement::revertChannelSelected() {
  setUInt16_le(0x24, 0xffff);
}


/* ******************************************************************************************** *
 * Implementation of TyTCodeplug::EncryptionElement
 * ******************************************************************************************** */
TyTCodeplug::EncryptionElement::EncryptionElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

TyTCodeplug::EncryptionElement::EncryptionElement(uint8_t *ptr)
  : Element(ptr, 0xb0)
{
  // pass...
}

TyTCodeplug::EncryptionElement::~EncryptionElement() {
  // pass...
}

void
TyTCodeplug::EncryptionElement::clear() {
  memset(_data, 0xff, 0xb0);
}

QByteArray
TyTCodeplug::EncryptionElement::enhancedKey(uint n) {
  return QByteArray((char *)(_data+n*16), 16);
}
void
TyTCodeplug::EncryptionElement::setEnhancedKey(uint n, const QByteArray &key) {
  if (16 != key.size())
    return;
  memcpy(_data+n*16, key.constData(), 16);
}

QByteArray
TyTCodeplug::EncryptionElement::basicKey(uint n) {
  return QByteArray((char *)(_data+0x90+n*2), 2);
}
void
TyTCodeplug::EncryptionElement::setBasicKey(uint n, const QByteArray &key) {
  if (2 != key.size())
    return;
  memcpy(_data+0x90+n*16, key.constData(), 2);
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
  // Clear boot settings
  this->clearBootSettings();
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
  // Check if default DMR id is set.
  if (nullptr == config->radioIDs()->defaultId()) {
    _errorMessage = tr("Cannot encode TyT codeplug: No default radio ID specified.");
    return false;
  }

  // Set timestamp
  if (! this->encodeTimestamp()) {
    _errorMessage = tr("Cannot encode time-stamp: %1").arg(_errorMessage);
    return false;
  }
  // General config
  if (! this->encodeGeneralSettings(config, flags)) {
    _errorMessage = tr("Cannot encode general settings: %1").arg(_errorMessage);
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

  // Encode button settings
  if (! this->encodeButtonSettings(config, flags)) {
    _errorMessage = tr("Cannot encode button settings: %1").arg(_errorMessage);
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

  // Decode button settings
  if (! this->decodeButtonSetttings(config)) {
    _errorMessage = tr("Cannot decode button settings: %1").arg(_errorMessage);
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
TyTCodeplug::encodeTimestamp() {
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
TyTCodeplug::clearButtonSettings() {
  // pass...
}

bool
TyTCodeplug::encodeButtonSettings(Config *config, const Flags &flags) {
  // pass...
  return true;
}

bool
TyTCodeplug::decodeButtonSetttings(Config *config) {
  // pass...
  return true;
}


void
TyTCodeplug::clearBootSettings() {
  // pass...
}

void
TyTCodeplug::clearMenuSettings() {
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
