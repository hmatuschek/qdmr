#include "radioddity_codeplug.hh"
#include "utils.hh"
#include "scanlist.hh"
#include "radioid.hh"
#include "contact.hh"
#include "rxgrouplist.hh"

/* ********************************************************************************************* *
 * Implementation of RadioddityCodeplug::ChannelElement
 * ********************************************************************************************* */
RadioddityCodeplug::ChannelElement::ChannelElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

RadioddityCodeplug::ChannelElement::ChannelElement(uint8_t *ptr)
  : Element(ptr, 0x38)
{
  // pass...
}

RadioddityCodeplug::ChannelElement::~ChannelElement() {
  // pass...
}

void
RadioddityCodeplug::ChannelElement::clear() {
  setName("");
  setRXFrequency(0);
  setTXFrequency(0);
  setMode(MODE_ANALOG);
  setUInt8(0x0019, 0x00); setUInt8(0x001a, 0x00);
  setTXTimeOut(0);
  setTXTimeOutRekeyDelay(0);
  setAdmitCriterion(ADMIT_ALWAYS);
  setUInt8(0x001e, 0x50);
  setScanListIndex(0x00);
  setRXTone(Signaling::SIGNALING_NONE);
  setTXTone(Signaling::SIGNALING_NONE);
  setUInt8(0x0024, 0x00);
  setTXSignalingIndex(0);
  setUInt8(0x0026, 0x00);
  setRXSignalingIndex(0);
  setUInt8(0x0028, 0x16);
  setPrivacyGroup(PRIVGR_NONE);
  setTXColorCode(0);
  setGroupListIndex(0);
  setRXColorCode(0);
  setEmergencySystemIndex(0);
  setUInt8(0x0034, 0); setUInt8(0x0035, 0); setUInt8(0x0036, 0);
}

QString
RadioddityCodeplug::ChannelElement::name() const {
  return readASCII(0x0000, 16, 0xff);
}
void
RadioddityCodeplug::ChannelElement::setName(const QString &n) {
  writeASCII(0x0000, n, 16, 0xff);
}

uint32_t
RadioddityCodeplug::ChannelElement::rxFrequency() const {
  return getBCD8_be(0x0010);
}
void
RadioddityCodeplug::ChannelElement::setRXFrequency(uint32_t freq) {
  setBCD8_be(0x0010, freq);
}
uint32_t
RadioddityCodeplug::ChannelElement::txFrequency() const {
  return getBCD8_be(0x0014);
}
void
RadioddityCodeplug::ChannelElement::setTXFrequency(uint32_t freq) {
  setBCD8_be(0x0014, freq);
}

RadioddityCodeplug::ChannelElement::Mode
RadioddityCodeplug::ChannelElement::mode() const {
  return (Mode)getUInt8(0x0018);
}
void
RadioddityCodeplug::ChannelElement::setMode(Mode mode) {
  setUInt8(0x0018, (uint)mode);
}

uint
RadioddityCodeplug::ChannelElement::txTimeOut() const {
  return getUInt8(0x001b)*15;
}
void
RadioddityCodeplug::ChannelElement::setTXTimeOut(uint tot) {
  setUInt8(0x001b, tot/15);
}
uint
RadioddityCodeplug::ChannelElement::txTimeOutRekeyDelay() const {
  return getUInt8(0x001c);
}
void
RadioddityCodeplug::ChannelElement::setTXTimeOutRekeyDelay(uint delay) {
  setUInt8(0x001c, delay);
}

RadioddityCodeplug::ChannelElement::Admit
RadioddityCodeplug::ChannelElement::admitCriterion() const {
  return (Admit) getUInt8(0x001d);
}
void
RadioddityCodeplug::ChannelElement::setAdmitCriterion(Admit admit) {
  setUInt8(0x001d, (uint)admit);
}

uint
RadioddityCodeplug::ChannelElement::scanListIndex() const {
  return getUInt8(0x001f);
}
void
RadioddityCodeplug::ChannelElement::setScanListIndex(uint index) {
  setUInt8(0x001f, index);
}

Signaling::Code
RadioddityCodeplug::ChannelElement::rxTone() const {
  return decode_ctcss_tone_table(getUInt16_be(0x0020));
}
void
RadioddityCodeplug::ChannelElement::setRXTone(Signaling::Code code) {
  setUInt16_be(0x0020, encode_ctcss_tone_table(code));
}
Signaling::Code
RadioddityCodeplug::ChannelElement::txTone() const {
  return decode_ctcss_tone_table(getUInt16_be(0x0022));
}
void
RadioddityCodeplug::ChannelElement::setTXTone(Signaling::Code code) {
  setUInt16_be(0x0022, encode_ctcss_tone_table(code));
}

uint
RadioddityCodeplug::ChannelElement::txSignalingIndex() const {
  return getUInt8(0x0025);
}
void
RadioddityCodeplug::ChannelElement::setTXSignalingIndex(uint index) {
  setUInt8(0x0025, index);
}
uint
RadioddityCodeplug::ChannelElement::rxSignalingIndex() const {
  return getUInt8(0x0027);
}
void
RadioddityCodeplug::ChannelElement::setRXSignalingIndex(uint index) {
  setUInt8(0x0027, index);
}

RadioddityCodeplug::ChannelElement::PrivacyGroup
RadioddityCodeplug::ChannelElement::privacyGroup() const {
  return (PrivacyGroup) getUInt8(0x0029);
}
void
RadioddityCodeplug::ChannelElement::setPrivacyGroup(PrivacyGroup grp) {
  setUInt8(0x0029, (uint)grp);
}

uint
RadioddityCodeplug::ChannelElement::txColorCode() const {
  return getUInt8(0x002a);
}
void
RadioddityCodeplug::ChannelElement::setTXColorCode(uint cc) {
  setUInt8(0x002a, cc);
}

uint
RadioddityCodeplug::ChannelElement::groupListIndex() const {
  return getUInt8(0x002b);
}
void
RadioddityCodeplug::ChannelElement::setGroupListIndex(uint index) {
  setUInt8(0x002b, index);
}

uint
RadioddityCodeplug::ChannelElement::rxColorCode() const {
  return getUInt8(0x002c);
}
void
RadioddityCodeplug::ChannelElement::setRXColorCode(uint cc) {
  setUInt8(0x002c, cc);
}

uint
RadioddityCodeplug::ChannelElement::emergencySystemIndex() const {
  return getUInt8(0x002d);
}
void
RadioddityCodeplug::ChannelElement::setEmergencySystemIndex(uint index) {
  setUInt8(0x002d, index);
}

uint
RadioddityCodeplug::ChannelElement::contactIndex() const {
  return getUInt8(0x002e);
}
void
RadioddityCodeplug::ChannelElement::setContactIndex(uint index) {
  setUInt8(0x002e, index);
}

bool
RadioddityCodeplug::ChannelElement::dataCallConfirm() const {
  return getBit(0x0030, 6);
}
void
RadioddityCodeplug::ChannelElement::enableDataCallConfirm(bool enable) {
  setBit(0x0030, 6, enable);
}
bool
RadioddityCodeplug::ChannelElement::emergencyAlarmACK() const {
  return getBit(0x0030, 7);
}
void
RadioddityCodeplug::ChannelElement::enableEmergencyAlarmACK(bool enable) {
  setBit(0x0030, 7, enable);
}
bool
RadioddityCodeplug::ChannelElement::privateCallConfirm() const {
  return getBit(0x0031, 0);
}
void
RadioddityCodeplug::ChannelElement::enablePrivateCallConfirm(bool enable) {
  setBit(0x0031, 0, enable);
}
bool
RadioddityCodeplug::ChannelElement::privacyEnabled() const {
  return getBit(0x0031, 4);
}
void
RadioddityCodeplug::ChannelElement::enablePrivacy(bool enable) {
  setBit(0x00031, 4, enable);
}

DigitalChannel::TimeSlot
RadioddityCodeplug::ChannelElement::timeSlot() const {
  return (getBit(0x0031, 6) ? DigitalChannel::TimeSlot::TS2 : DigitalChannel::TimeSlot::TS1);
}
void
RadioddityCodeplug::ChannelElement::setTimeSlot(DigitalChannel::TimeSlot ts) {
  setBit(0x0031, 6, DigitalChannel::TimeSlot::TS2 == ts);
}

bool
RadioddityCodeplug::ChannelElement::dualCapacityDirectMode() const {
  return getBit(0x0032, 0);
}
void
RadioddityCodeplug::ChannelElement::enableDualCapacityDirectMode(bool enable) {
  setBit(0x0032, 0, enable);
}
bool
RadioddityCodeplug::ChannelElement::nonSTEFrequency() const {
  return getBit(0x0032, 5);
}
void
RadioddityCodeplug::ChannelElement::enableNonSTEFrequency(bool enable) {
  setBit(0x0032, 5, enable);
}

AnalogChannel::Bandwidth
RadioddityCodeplug::ChannelElement::bandwidth() const {
  return (getBit(0x0033, 1) ? AnalogChannel::Bandwidth::Wide : AnalogChannel::Bandwidth::Narrow);
}
void
RadioddityCodeplug::ChannelElement::setBandwidth(AnalogChannel::Bandwidth bw) {
  setBit(0x0033, 1, AnalogChannel::Bandwidth::Wide == bw);
}

bool
RadioddityCodeplug::ChannelElement::rxOnly() const {
  return getBit(0x0033, 2);
}
void
RadioddityCodeplug::ChannelElement::enableRXOnly(bool enable) {
  setBit(0x0033, 2, enable);
}
bool
RadioddityCodeplug::ChannelElement::talkaround() const {
  return getBit(0x0033, 3);
}
void
RadioddityCodeplug::ChannelElement::enableTalkaround(bool enable) {
  setBit(0x0033, 3, enable);
}
bool
RadioddityCodeplug::ChannelElement::vox() const {
  return getBit(0x0033, 6);
}
void
RadioddityCodeplug::ChannelElement::enableVOX(bool enable) {
  setBit(0x0033, 6, enable);
}

Channel::Power
RadioddityCodeplug::ChannelElement::power() const {
  return (getBit(0x0033, 7) ? Channel::Power::High : Channel::Power::Low);
}
void
RadioddityCodeplug::ChannelElement::setPower(Channel::Power pwr) {
  switch (pwr) {
  case Channel::Power::Min:
  case Channel::Power::Low:
    clearBit(0x0033, 7);
    break;
  case Channel::Power::Mid:
  case Channel::Power::High:
  case Channel::Power::Max:
    setBit(0x0033, 7);
    break;
  }
}

uint
RadioddityCodeplug::ChannelElement::squelch() const {
  return std::min(getUInt8(0x0037), uint8_t(9))+1;
}
void
RadioddityCodeplug::ChannelElement::setSquelch(uint level) {
  level = std::max(std::min(10u, level), 1u);
  setUInt8(0x0037, level-1);
}

Channel *
RadioddityCodeplug::ChannelElement::toChannelObj(CodePlug::Context &ctx) const {
  Channel *ch = nullptr;
  if (MODE_ANALOG == mode()) {
    AnalogChannel *ach = new AnalogChannel(
          "", 0, 0, Channel::Power::Low, 0, false, AnalogChannel::Admit::Always, 0,
          Signaling::SIGNALING_NONE, Signaling::SIGNALING_NONE, AnalogChannel::Bandwidth::Wide,
          nullptr, nullptr, nullptr); ch = ach;
    switch (admitCriterion()) {
    case ADMIT_ALWAYS: ach->setAdmit(AnalogChannel::Admit::Always); break;
    case ADMIT_CH_FREE: ach->setAdmit(AnalogChannel::Admit::Free); break;
    default: ach->setAdmit(AnalogChannel::Admit::Always); break;
    }
    ach->setBandwidth(bandwidth());
  } else {
    DigitalChannel *dch = new DigitalChannel(
          "", 0, 0, Channel::Power::Low, 0, false, DigitalChannel::Admit::Always, 0,
          DigitalChannel::TimeSlot::TS1, nullptr, nullptr, nullptr, nullptr, nullptr,
          DefaultRadioID::get(), nullptr); ch = dch;
    switch (admitCriterion()) {
    case ADMIT_ALWAYS: dch->setAdmit(DigitalChannel::Admit::Always); break;
    case ADMIT_CH_FREE: dch->setAdmit(DigitalChannel::Admit::Free); break;
    case ADMIT_COLOR: dch->setAdmit(DigitalChannel::Admit::ColorCode); break;
    default: dch->setAdmit(DigitalChannel::Admit::Always); break;
    }
    dch->setTimeSlot(timeSlot());
  }

  // Apply common settings
  ch->setName(name());
  ch->setRXFrequency(rxFrequency());
  ch->setTXFrequency(txFrequency());
  ch->setPower(power());
  ch->setTimeout(txTimeOut());
  ch->setRXOnly(rxOnly());
  if (0 < scanListIndex()) {
    ch->setScanListObj(ctx.get<ScanList>(scanListIndex()));
  }
  // done.
  return ch;
}

bool
RadioddityCodeplug::ChannelElement::linkChannelObj(Channel *c, Context &ctx) const {
  // Link common
  if (scanListIndex() && ctx.has<ScanList>(scanListIndex()))
    c->setScanListObj(ctx.get<ScanList>(scanListIndex()));
  // Link digital channel
  if (c->is<DigitalChannel>()) {
    DigitalChannel *dc = c->as<DigitalChannel>();
    if (groupListIndex() && ctx.has<RXGroupList>(groupListIndex()))
      dc->setGroupListObj(ctx.get<RXGroupList>(groupListIndex()));
    if (contactIndex() && ctx.has<DigitalContact>(contactIndex()))
      dc->setTXContactObj(ctx.get<DigitalContact>(contactIndex()));
  }
  return true;

}

bool
RadioddityCodeplug::ChannelElement::fromChannelObj(const Channel *c, Context &ctx) {
  clear();

  setName(c->name());
  setRXFrequency(c->rxFrequency());
  setTXFrequency(c->txFrequency());
  setPower(c->power());
  setTXTimeOut(c->timeout());
  enableRXOnly(c->rxOnly());
  if (c->scanListObj())
    setScanListIndex(ctx.index(c->scanListObj()));

  if (c->is<AnalogChannel>()) {
    const AnalogChannel *ac = c->as<const AnalogChannel>();
    setMode(MODE_ANALOG);
    switch (ac->admit()) {
    case AnalogChannel::Admit::Always: setAdmitCriterion(ADMIT_ALWAYS); break;
    case AnalogChannel::Admit::Free: setAdmitCriterion(ADMIT_CH_FREE); break;
    default: setAdmitCriterion(ADMIT_ALWAYS);
    }
    setBandwidth(ac->bandwidth());
    setSquelch(ac->squelch());
    setRXTone(ac->rxTone());
    setTXTone(ac->txTone());
  } else if (c->is<DigitalChannel>()) {
    const DigitalChannel *dc = c->as<const DigitalChannel>();
    setMode(MODE_DIGITAL);
    switch (dc->admit()) {
      case DigitalChannel::Admit::Always: setAdmitCriterion(ADMIT_ALWAYS);
      case DigitalChannel::Admit::Free: setAdmitCriterion(ADMIT_CH_FREE);
      case DigitalChannel::Admit::ColorCode: setAdmitCriterion(ADMIT_COLOR);
    }
    setTimeSlot(dc->timeSlot());
    setRXColorCode(dc->colorCode());
    setTXColorCode(dc->colorCode());
    if (dc->groupListObj())
      setGroupListIndex(ctx.index(dc->groupListObj()));
    if (dc->txContactObj())
      setContactIndex(ctx.index(dc->txContactObj()));
  }
  return true;
}


/* ********************************************************************************************* *
 * Implementation of RadioddityCodeplug
 * ********************************************************************************************* */
RadioddityCodeplug::RadioddityCodeplug(QObject *parent)
  : CodePlug(parent)
{
  // pass...
}

RadioddityCodeplug::~RadioddityCodeplug() {
  // pass...
}

void
RadioddityCodeplug::clear() {
  // pass...
}

bool
RadioddityCodeplug::encode(Config *config, const Flags &flags) {
  return false;
}

bool
RadioddityCodeplug::decode(Config *config) {
  return false;
}
