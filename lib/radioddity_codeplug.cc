#include "radioddity_codeplug.hh"
#include "utils.hh"
#include "logger.hh"
#include "scanlist.hh"
#include "radioid.hh"
#include "contact.hh"
#include "rxgrouplist.hh"
#include "zone.hh"
#include "config.hh"


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
  setContactIndex(0);
  setUInt32_be(0x0030, 0); // clear all bitfields at once.
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
  return getBCD8_le(0x0010)*10;
}
void
RadioddityCodeplug::ChannelElement::setRXFrequency(uint32_t freq) {
  setBCD8_le(0x0010, freq/10);
}
uint32_t
RadioddityCodeplug::ChannelElement::txFrequency() const {
  return getBCD8_le(0x0014)*10;
}
void
RadioddityCodeplug::ChannelElement::setTXFrequency(uint32_t freq) {
  setBCD8_le(0x0014, freq/10);
}

RadioddityCodeplug::ChannelElement::Mode
RadioddityCodeplug::ChannelElement::mode() const {
  return (Mode)getUInt8(0x0018);
}
void
RadioddityCodeplug::ChannelElement::setMode(Mode mode) {
  setUInt8(0x0018, (unsigned)mode);
}

unsigned
RadioddityCodeplug::ChannelElement::txTimeOut() const {
  return getUInt8(0x001b)*15;
}
void
RadioddityCodeplug::ChannelElement::setTXTimeOut(unsigned tot) {
  setUInt8(0x001b, tot/15);
}
unsigned
RadioddityCodeplug::ChannelElement::txTimeOutRekeyDelay() const {
  return getUInt8(0x001c);
}
void
RadioddityCodeplug::ChannelElement::setTXTimeOutRekeyDelay(unsigned delay) {
  setUInt8(0x001c, delay);
}

RadioddityCodeplug::ChannelElement::Admit
RadioddityCodeplug::ChannelElement::admitCriterion() const {
  return (Admit) getUInt8(0x001d);
}
void
RadioddityCodeplug::ChannelElement::setAdmitCriterion(Admit admit) {
  setUInt8(0x001d, (unsigned)admit);
}

bool
RadioddityCodeplug::ChannelElement::hasScanList() const {
  return 0!=scanListIndex();
}
unsigned
RadioddityCodeplug::ChannelElement::scanListIndex() const {
  return getUInt8(0x001f);
}
void
RadioddityCodeplug::ChannelElement::setScanListIndex(unsigned index) {
  setUInt8(0x001f, index);
}

Signaling::Code
RadioddityCodeplug::ChannelElement::rxTone() const {
  return decode_ctcss_tone_table(getUInt16_le(0x0020));
}
void
RadioddityCodeplug::ChannelElement::setRXTone(Signaling::Code code) {
  setUInt16_le(0x0020, encode_ctcss_tone_table(code));
}
Signaling::Code
RadioddityCodeplug::ChannelElement::txTone() const {
  return decode_ctcss_tone_table(getUInt16_le(0x0022));
}
void
RadioddityCodeplug::ChannelElement::setTXTone(Signaling::Code code) {
  setUInt16_le(0x0022, encode_ctcss_tone_table(code));
}

unsigned
RadioddityCodeplug::ChannelElement::txSignalingIndex() const {
  return getUInt8(0x0025);
}
void
RadioddityCodeplug::ChannelElement::setTXSignalingIndex(unsigned index) {
  setUInt8(0x0025, index);
}
unsigned
RadioddityCodeplug::ChannelElement::rxSignalingIndex() const {
  return getUInt8(0x0027);
}
void
RadioddityCodeplug::ChannelElement::setRXSignalingIndex(unsigned index) {
  setUInt8(0x0027, index);
}

RadioddityCodeplug::ChannelElement::PrivacyGroup
RadioddityCodeplug::ChannelElement::privacyGroup() const {
  return (PrivacyGroup) getUInt8(0x0029);
}
void
RadioddityCodeplug::ChannelElement::setPrivacyGroup(PrivacyGroup grp) {
  setUInt8(0x0029, (unsigned)grp);
}

unsigned
RadioddityCodeplug::ChannelElement::txColorCode() const {
  return getUInt8(0x002a);
}
void
RadioddityCodeplug::ChannelElement::setTXColorCode(unsigned cc) {
  setUInt8(0x002a, cc);
}

bool
RadioddityCodeplug::ChannelElement::hasGroupList() const {
  return 0!=groupListIndex();
}
unsigned
RadioddityCodeplug::ChannelElement::groupListIndex() const {
  return getUInt8(0x002b);
}
void
RadioddityCodeplug::ChannelElement::setGroupListIndex(unsigned index) {
  setUInt8(0x002b, index);
}

unsigned
RadioddityCodeplug::ChannelElement::rxColorCode() const {
  return getUInt8(0x002c);
}
void
RadioddityCodeplug::ChannelElement::setRXColorCode(unsigned cc) {
  setUInt8(0x002c, cc);
}

bool
RadioddityCodeplug::ChannelElement::hasEmergencySystem() const {
  return 0!=emergencySystemIndex();
}
unsigned
RadioddityCodeplug::ChannelElement::emergencySystemIndex() const {
  return getUInt8(0x002d);
}
void
RadioddityCodeplug::ChannelElement::setEmergencySystemIndex(unsigned index) {
  setUInt8(0x002d, index);
}

bool
RadioddityCodeplug::ChannelElement::hasContact() const {
  return 0!=contactIndex();
}
unsigned
RadioddityCodeplug::ChannelElement::contactIndex() const {
  return getUInt16_le(0x002e);
}
void
RadioddityCodeplug::ChannelElement::setContactIndex(unsigned index) {
  setUInt16_le(0x002e, index);
}

bool
RadioddityCodeplug::ChannelElement::dataCallConfirm() const {
  return getBit(0x0030, 7);
}
void
RadioddityCodeplug::ChannelElement::enableDataCallConfirm(bool enable) {
  setBit(0x0030, 7, enable);
}
bool
RadioddityCodeplug::ChannelElement::emergencyAlarmACK() const {
  return getBit(0x0030, 6);
}
void
RadioddityCodeplug::ChannelElement::enableEmergencyAlarmACK(bool enable) {
  setBit(0x0030, 6, enable);
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

Channel *
RadioddityCodeplug::ChannelElement::toChannelObj(Codeplug::Context &ctx) const {
  Q_UNUSED(ctx)
  Channel *ch = nullptr;
  if (MODE_ANALOG == mode()) {
    AnalogChannel *ach = new AnalogChannel(); ch = ach;
    switch (admitCriterion()) {
    case ADMIT_ALWAYS: ach->setAdmit(AnalogChannel::Admit::Always); break;
    case ADMIT_CH_FREE: ach->setAdmit(AnalogChannel::Admit::Free); break;
    default: ach->setAdmit(AnalogChannel::Admit::Always); break;
    }
    ach->setBandwidth(bandwidth());
    ach->setRXTone(rxTone());
    ach->setTXTone(txTone());
    ach->setSquelchDefault(); // There is no per-channel squelch setting
  } else {
    DigitalChannel *dch = new DigitalChannel(); ch = dch;
    switch (admitCriterion()) {
    case ADMIT_ALWAYS: dch->setAdmit(DigitalChannel::Admit::Always); break;
    case ADMIT_CH_FREE: dch->setAdmit(DigitalChannel::Admit::Free); break;
    case ADMIT_COLOR: dch->setAdmit(DigitalChannel::Admit::ColorCode); break;
    default: dch->setAdmit(DigitalChannel::Admit::Always); break;
    }
    dch->setTimeSlot(timeSlot());
    dch->setColorCode(txColorCode());
  }

  // Apply common settings
  ch->setName(name());
  ch->setRXFrequency(double(rxFrequency())/1e6);
  ch->setTXFrequency(double(txFrequency())/1e6);
  ch->setPower(power());
  ch->setTimeout(txTimeOut());
  ch->setRXOnly(rxOnly());
  if (vox())
    ch->setVOXDefault();
  else
    ch->disableVOX();
  // done.
  return ch;
}

bool
RadioddityCodeplug::ChannelElement::linkChannelObj(Channel *c, Context &ctx) const {
  // Link common
  if (hasScanList() && ctx.has<ScanList>(scanListIndex()))
    c->setScanListObj(ctx.get<ScanList>(scanListIndex()));
  // Link digital channel
  if (c->is<DigitalChannel>()) {
    DigitalChannel *dc = c->as<DigitalChannel>();
    if (hasGroupList() && ctx.has<RXGroupList>(groupListIndex()))
      dc->setGroupListObj(ctx.get<RXGroupList>(groupListIndex()));
    if (hasContact() && ctx.has<DigitalContact>(contactIndex()))
      dc->setTXContactObj(ctx.get<DigitalContact>(contactIndex()));
  }
  return true;

}

bool
RadioddityCodeplug::ChannelElement::fromChannelObj(const Channel *c, Context &ctx) {
  clear();

  setName(c->name());
  setRXFrequency(c->rxFrequency()*1e6);
  setTXFrequency(c->txFrequency()*1e6);
  if (c->defaultPower())
    setPower(ctx.config()->settings()->power());
  else
    setPower(c->power());
  if (c->defaultTimeout())
    setTXTimeOut(ctx.config()->settings()->tot());
  else
    setTXTimeOut(c->timeout());
  enableRXOnly(c->rxOnly());
  // Enable vox
  bool defaultVOXEnabled = (c->defaultVOX() && (!ctx.config()->settings()->voxDisabled()));
  bool channelVOXEnabled = (! (c->voxDisabled()||c->defaultVOX()));
  enableVOX(defaultVOXEnabled || channelVOXEnabled);

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
    setRXTone(ac->rxTone());
    setTXTone(ac->txTone());
    // no per channel squelch setting
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
 * Implementation of RadioddityCodeplug::ChannelBankElement
 * ********************************************************************************************* */
RadioddityCodeplug::ChannelBankElement::ChannelBankElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

RadioddityCodeplug::ChannelBankElement::ChannelBankElement(uint8_t *ptr)
  : Element(ptr, 0x1c10)
{
  // pass...
}

RadioddityCodeplug::ChannelBankElement::~ChannelBankElement() {
  // pass...
}

void
RadioddityCodeplug::ChannelBankElement::clear() {
  memset(_data, 0, 0x10);
}

bool
RadioddityCodeplug::ChannelBankElement::isEnabled(unsigned idx) const {
  unsigned byte = idx/8, bit = idx%8;
  return getBit(byte, bit);
}
void
RadioddityCodeplug::ChannelBankElement::enable(unsigned idx, bool enabled) {
  unsigned byte = idx/8, bit = idx%8;
  return setBit(byte, bit, enabled);
}

uint8_t *
RadioddityCodeplug::ChannelBankElement::get(unsigned idx) const {
  return (_data+0x10)+idx*0x38;
}


/* ******************************************************************************************** *
 * Implementation of RadioddityCodeplug::VFOChannelElement
 * ******************************************************************************************** */
RadioddityCodeplug::VFOChannelElement::VFOChannelElement(uint8_t *ptr, unsigned size)
  : ChannelElement(ptr, size)
{
  // pass...
}

RadioddityCodeplug::VFOChannelElement::VFOChannelElement(uint8_t *ptr)
  : ChannelElement(ptr)
{
  // pass...
}

void
RadioddityCodeplug::VFOChannelElement::clear() {
  ChannelElement::clear();
  setStepSize(12.5);
  setOffsetMode(OffsetMode::Off);
  setTXOffset(10.0);
}

QString
RadioddityCodeplug::VFOChannelElement::name() const {
  return QString();
}
void
RadioddityCodeplug::VFOChannelElement::setName(const QString &name) {
  Q_UNUSED(name);
  ChannelElement::setName("");
}

double
RadioddityCodeplug::VFOChannelElement::stepSize() const {
  switch (StepSize(getUInt4(0x0036, 4))) {
  case StepSize::SS2_5kHz: return 2.5;
  case StepSize::SS5kHz: return 5;
  case StepSize::SS6_25kHz: return 6.25;
  case StepSize::SS10kHz: return 10.0;
  case StepSize::SS12_5kHz: return 12.5;
  case StepSize::SS20kHz: return 20;
  case StepSize::SS30kHz: return 30;
  case StepSize::SS50kHz: return 50;
  }
  return 12.5;
}

void
RadioddityCodeplug::VFOChannelElement::setStepSize(double kHz) {
  if (2.5 >= kHz)
    setUInt4(0x0036, 4, (unsigned)StepSize::SS2_5kHz);
  else if (5.0 >= kHz)
    setUInt4(0x0036, 4, (unsigned)StepSize::SS5kHz);
  else if (6.25 >= kHz)
    setUInt4(0x0036, 4, (unsigned)StepSize::SS6_25kHz);
  else if (10.0 >= kHz)
    setUInt4(0x0036, 4, (unsigned)StepSize::SS10kHz);
  else if (12.5 >= kHz)
    setUInt4(0x0036, 4, (unsigned)StepSize::SS12_5kHz);
  else if (20.0 >= kHz)
    setUInt4(0x0036, 4, (unsigned)StepSize::SS20kHz);
  else if (30.0 >= kHz)
    setUInt4(0x0036, 4, (unsigned)StepSize::SS30kHz);
  else
    setUInt4(0x0036, 4, (unsigned)StepSize::SS50kHz);
}

RadioddityCodeplug::VFOChannelElement::OffsetMode
RadioddityCodeplug::VFOChannelElement::offsetMode() const {
  return (OffsetMode)getUInt2(0x0036, 2);
}
void
RadioddityCodeplug::VFOChannelElement::setOffsetMode(OffsetMode mode) {
  setUInt2(0x0036, 2, (unsigned)mode);
}
double
RadioddityCodeplug::VFOChannelElement::txOffset() const {
  return ((double)getBCD4_le(0x0034))/100;
}
void
RadioddityCodeplug::VFOChannelElement::setTXOffset(double f) {
  setBCD4_le(0x0034, (f*100));
}


/* ********************************************************************************************* *
 * Implementation of RadioddityCodeplug::ContactElement
 * ********************************************************************************************* */
RadioddityCodeplug::ContactElement::ContactElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

RadioddityCodeplug::ContactElement::ContactElement(uint8_t *ptr)
  : Element(ptr, 0x0018)
{
  // pass...
}

RadioddityCodeplug::ContactElement::~ContactElement() {
  // pass...
}

void
RadioddityCodeplug::ContactElement::clear() {
  setName("");
  setNumber(0);
  setType(DigitalContact::GroupCall);
  enableRing(0);
  setRingStyle(0);
  setUInt8(0x017, 0x00);
}

bool
RadioddityCodeplug::ContactElement::isValid() const {
  return (! name().isEmpty());
}

QString
RadioddityCodeplug::ContactElement::name() const {
  return readASCII(0x0000, 16, 0xff);
}
void
RadioddityCodeplug::ContactElement::setName(const QString name) {
  writeASCII(0x0000, name, 16, 0xff);
}

unsigned
RadioddityCodeplug::ContactElement::number() const {
  return getBCD8_be(0x0010);
}
void
RadioddityCodeplug::ContactElement::setNumber(unsigned id) {
  setBCD8_be(0x0010, id);
}

DigitalContact::Type
RadioddityCodeplug::ContactElement::type() const {
  switch (getUInt8(0x0014)) {
  case 0: return DigitalContact::GroupCall;
  case 1: return DigitalContact::PrivateCall;
  case 2: return DigitalContact::AllCall;
  default: break;
  }
  return DigitalContact::PrivateCall;
}
void
RadioddityCodeplug::ContactElement::setType(DigitalContact::Type type) {
  switch (type) {
  case DigitalContact::GroupCall: setUInt8(0x0014, 0); break;
  case DigitalContact::PrivateCall: setUInt8(0x0014, 1); break;
  case DigitalContact::AllCall: setUInt8(0x0014, 2); break;
  }
}

bool
RadioddityCodeplug::ContactElement::ring() const {
  return 0x00 != getUInt8(0x0015);
}
void
RadioddityCodeplug::ContactElement::enableRing(bool enable) {
  if (enable)
    setUInt8(0x0015, 1);
  else
    setUInt8(0x0015, 0);
}

unsigned
RadioddityCodeplug::ContactElement::ringStyle() const {
  return getUInt8(0x0016);
}
void
RadioddityCodeplug::ContactElement::setRingStyle(unsigned style) {
  style = std::min(style, 10u);
  setUInt8(0x0016, style);
}

DigitalContact *
RadioddityCodeplug::ContactElement::toContactObj(Context &ctx) const {
  Q_UNUSED(ctx)
  if (! isValid())
    return nullptr;
  return new DigitalContact(type(), name(), number(), ring());
}

void
RadioddityCodeplug::ContactElement::fromContactObj(const DigitalContact *cont, Context &ctx) {
  Q_UNUSED(ctx)
  setName(cont->name());
  setNumber(cont->number());
  setType(cont->type());
  if (cont->ring()) {
    enableRing(true);
    setRingStyle(1);
  } else {
    enableRing(false);
  }
}


/* ********************************************************************************************* *
 * Implementation of RadioddityCodeplug::DTMFContactElement
 * ********************************************************************************************* */
RadioddityCodeplug::DTMFContactElement::DTMFContactElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

RadioddityCodeplug::DTMFContactElement::DTMFContactElement(uint8_t *ptr)
  : Element(ptr, 0x0020)
{
  // pass...
}

RadioddityCodeplug::DTMFContactElement::~DTMFContactElement() {
  // pass...
}

void
RadioddityCodeplug::DTMFContactElement::clear() {
  memset(_data, 0xff, 0x20);
}
bool
RadioddityCodeplug::DTMFContactElement::isValid() const {
  return (! name().isEmpty());
}

QString
RadioddityCodeplug::DTMFContactElement::name() const {
  return readASCII(0x0000, 16, 0xff);
}
void
RadioddityCodeplug::DTMFContactElement::setName(const QString &name) {
  writeASCII(0x0000, name, 16, 0xff);
}

QString
RadioddityCodeplug::DTMFContactElement::number() const {
  return readASCII(0x0010, 16, 0xff);
}
void
RadioddityCodeplug::DTMFContactElement::setNumber(const QString &number) {
  writeASCII(0x0010, number, 16, 0xff);
}

DTMFContact *
RadioddityCodeplug::DTMFContactElement::toContactObj(Context &ctx) const {
  Q_UNUSED(ctx)
  if (! isValid())
    return nullptr;
  return new DTMFContact(name(), number());
}

void
RadioddityCodeplug::DTMFContactElement::fromContactObj(const DTMFContact *cont, Context &ctx) {
  Q_UNUSED(ctx)
  setName(cont->name());
  setNumber(cont->number());
}


/* ********************************************************************************************* *
 * Implementation of RadioddityCodeplug::ZoneElement
 * ********************************************************************************************* */
RadioddityCodeplug::ZoneElement::ZoneElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

RadioddityCodeplug::ZoneElement::ZoneElement(uint8_t *ptr)
  : Element(ptr, 0x0030)
{
  // pass...
}

RadioddityCodeplug::ZoneElement::~ZoneElement() {
  // pass...
}

void
RadioddityCodeplug::ZoneElement::clear() {
  memset(_data+0x0000, 0xff, 0x0010);
  memset(_data+0x0010, 0x00, 0x0020);
}
bool
RadioddityCodeplug::ZoneElement::isValid() const {
  return (! name().isEmpty());
}

QString
RadioddityCodeplug::ZoneElement::name() const {
  return readASCII(0x0000, 16, 0xff);
}
void
RadioddityCodeplug::ZoneElement::setName(const QString &name) {
  writeASCII(0x0000, name, 16, 0xff);
}

bool
RadioddityCodeplug::ZoneElement::hasMember(unsigned n) const {
  return (0 != member(n));
}
unsigned
RadioddityCodeplug::ZoneElement::member(unsigned n) const {
  return getUInt16_le(0x0010+2*n);
}
void
RadioddityCodeplug::ZoneElement::setMember(unsigned n, unsigned idx) {
  setUInt16_le(0x0010+n*2, idx);
}
void
RadioddityCodeplug::ZoneElement::clearMember(unsigned n) {
  setMember(n, 0);
}

Zone *
RadioddityCodeplug::ZoneElement::toZoneObj(Context &ctx) const {
  Q_UNUSED(ctx)
  if (! isValid())
    return nullptr;
  return new Zone(name());
}

bool
RadioddityCodeplug::ZoneElement::linkZoneObj(Zone *zone, Context &ctx, bool putInB) const {
  if (! isValid()) {
    logWarn() << "Cannot link zone: Zone is invalid.";
    return false;
  }

  for (int i=0; (i<16) && hasMember(i); i++) {
    if (ctx.has<Channel>(member(i))) {
      if (! putInB)
        zone->A()->add(ctx.get<Channel>(member(i)));
      else
        zone->B()->add(ctx.get<Channel>(member(i)));
    } else {
      logWarn() << "While linking zone '" << zone->name() << "': " << i <<"-th channel index "
                << member(i) << " out of bounds.";
    }
  }
  return true;
}

void
RadioddityCodeplug::ZoneElement::fromZoneObjA(const Zone *zone, Context &ctx) {
  if (zone->A()->count() && zone->B()->count())
    setName(zone->name() + " A");
  else
    setName(zone->name());

  for (int i=0; i<16; i++) {
    if (i < zone->A()->count())
      setMember(i, ctx.index(zone->A()->get(i)));
    else
      clearMember(i);
  }
}

void
RadioddityCodeplug::ZoneElement::fromZoneObjB(const Zone *zone, Context &ctx) {
  if (zone->A()->count() && zone->B()->count())
    setName(zone->name() + " B");
  else
    setName(zone->name());

  for (int i=0; i<16; i++) {
    if (i < zone->B()->count())
      setMember(i, ctx.index(zone->B()->get(i)));
    else
      clearMember(i);
  }
}

/* ********************************************************************************************* *
 * Implementation of RadioddityCodeplug::ZoneBankElement
 * ********************************************************************************************* */
RadioddityCodeplug::ZoneBankElement::ZoneBankElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

RadioddityCodeplug::ZoneBankElement::ZoneBankElement(uint8_t *ptr)
  : Element(ptr, 0x2f00)
{
  // pass...
}

RadioddityCodeplug::ZoneBankElement::~ZoneBankElement() {
  // pass...
}

void
RadioddityCodeplug::ZoneBankElement::clear() {
  memset(_data, 0, 0x0020);
}

bool
RadioddityCodeplug::ZoneBankElement::isEnabled(unsigned idx) const {
  unsigned byte=idx/8, bit = idx%8;
  return getBit(byte, bit);
}
void
RadioddityCodeplug::ZoneBankElement::enable(unsigned idx, bool enabled) {
  unsigned byte=idx/8, bit = idx%8;
  setBit(byte, bit, enabled);
}
uint8_t *
RadioddityCodeplug::ZoneBankElement::get(unsigned idx) const {
  return _data + 0x0020 + idx*0x0030;
}


/* ********************************************************************************************* *
 * Implementation of RadioddityCodeplug::GroupListElement
 * ********************************************************************************************* */
RadioddityCodeplug::GroupListElement::GroupListElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

RadioddityCodeplug::GroupListElement::GroupListElement(uint8_t *ptr)
  : Element(ptr, 0x0030)
{
  // pass...
}

RadioddityCodeplug::GroupListElement::~GroupListElement() {
  // pass...
}

void
RadioddityCodeplug::GroupListElement::clear() {
  setName("");
  memset(_data+0x0010, 0, 2*16);
}

QString
RadioddityCodeplug::GroupListElement::name() const {
  return readASCII(0x0000, 16, 0xff);
}
void
RadioddityCodeplug::GroupListElement::setName(const QString &name) {
  writeASCII(0x0000, name, 16, 0xff);
}

bool
RadioddityCodeplug::GroupListElement::hasMember(unsigned n) const {
  return 0!=member(n);
}
unsigned
RadioddityCodeplug::GroupListElement::member(unsigned n) const {
  return getUInt16_le(0x0010 + 2*n);
}
void
RadioddityCodeplug::GroupListElement::setMember(unsigned n, unsigned idx) {
  return setUInt16_le(0x0010+2*n, idx);
}
void
RadioddityCodeplug::GroupListElement::clearMember(unsigned n) {
  setMember(n,0);
}

RXGroupList *
RadioddityCodeplug::GroupListElement::toRXGroupListObj(Context &ctx) {
  Q_UNUSED(ctx)
  return new RXGroupList(name());
}

bool
RadioddityCodeplug::GroupListElement::linkRXGroupListObj(int ncnt, RXGroupList *lst, Context &ctx) const {
  for (int i=0; (i<16) && (i<ncnt); i++) {
    if (ctx.has<DigitalContact>(member(i))) {
      lst->addContact(ctx.get<DigitalContact>(member(i)));
    } else {
      logError() << "Cannot link group list '" << lst->name()
                << "': Member index " << member(i) << " does not refer to a digital contact.";
      return false;
    }
  }
  return true;
}

void
RadioddityCodeplug::GroupListElement::fromRXGroupListObj(const RXGroupList *lst, Context &ctx) {
  setName(lst->name());
  for (int i=0; i<15; i++) {
    if (lst->count() > i) {
      setMember(i, ctx.index(lst->contact(i)));
    } else {
      clearMember(i);
    }
  }
}


/* ********************************************************************************************* *
 * Implementation of RadioddityCodeplug::GroupListBankElement
 * ********************************************************************************************* */
RadioddityCodeplug::GroupListBankElement::GroupListBankElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

RadioddityCodeplug::GroupListBankElement::GroupListBankElement(uint8_t *ptr)
  : Element(ptr, 0x0c80)
{
  // pass...
}

RadioddityCodeplug::GroupListBankElement::~GroupListBankElement() {
  // pass...
}

void
RadioddityCodeplug::GroupListBankElement::clear() {
  memset(_data, 0, 128);
}

bool
RadioddityCodeplug::GroupListBankElement::isEnabled(unsigned n) const {
  return 0 != getUInt8(n);
}
unsigned
RadioddityCodeplug::GroupListBankElement::contactCount(unsigned n) const {
  return getUInt8(n)-1;
}
void
RadioddityCodeplug::GroupListBankElement::setContactCount(unsigned n, unsigned size) {
  setUInt8(n, size+1);
}
void
RadioddityCodeplug::GroupListBankElement::disable(unsigned n) {
  setUInt8(n, 0);
}

uint8_t *
RadioddityCodeplug::GroupListBankElement::get(unsigned n) const {
  return _data + 0x80 + n*0x30;
}


/* ********************************************************************************************* *
 * Implementation of RadioddityCodeplug::ScanListElement
 * ********************************************************************************************* */
RadioddityCodeplug::ScanListElement::ScanListElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

RadioddityCodeplug::ScanListElement::ScanListElement(uint8_t *ptr)
  : Element(ptr, 0x0058)
{
  // pass...
}

RadioddityCodeplug::ScanListElement::~ScanListElement() {
  // pass...
}

void
RadioddityCodeplug::ScanListElement::clear() {
  setName("");
  setUInt8(0x000f, 0xff);
  // Clear member
  memset(_data+0x0010, 0x00, 2*32);
  clearPrimary();
  clearSecondary();
  clearRevert();
  setHoldTime(1000);
  setPrioritySampleTime(2000);
}

QString
RadioddityCodeplug::ScanListElement::name() const {
  return readASCII(0x0000, 15, 0xff);
}
void
RadioddityCodeplug::ScanListElement::setName(const QString &name) {
  writeASCII(0x0000, name, 15, 0xff);
}

bool
RadioddityCodeplug::ScanListElement::channelMark() const {
  return getBit(0x000f, 4);
}
void
RadioddityCodeplug::ScanListElement::enableChannelMark(bool enable) {
  setBit(0x000f, 4, enable);
}

RadioddityCodeplug::ScanListElement::Mode
RadioddityCodeplug::ScanListElement::mode() const {
  return (Mode) getUInt2(0x000f, 5);
}
void
RadioddityCodeplug::ScanListElement::setMode(Mode mode) {
  setUInt2(0x000f, 5, (unsigned)mode);
}

bool
RadioddityCodeplug::ScanListElement::talkback() const {
  return getBit(0x000f, 7);
}
void
RadioddityCodeplug::ScanListElement::enableTalkback(bool enable) {
  setBit(0x000f, 7, enable);
}

bool
RadioddityCodeplug::ScanListElement::hasMember(unsigned n) const {
  return 0 != getUInt16_le(0x0010+2*n);
}
bool
RadioddityCodeplug::ScanListElement::isSelected(unsigned n) const {
  return 1 == getUInt16_le(0x0010+2*n);
}
unsigned
RadioddityCodeplug::ScanListElement::member(unsigned n) const {
  return getUInt16_le(0x0010 + 2*n)-1;
}
void
RadioddityCodeplug::ScanListElement::setMember(unsigned n, unsigned idx) {
  setUInt16_le(0x0010+2*n, idx+1);
}
void
RadioddityCodeplug::ScanListElement::setSelected(unsigned n) {
  setUInt16_le(0x0010+2*n, 1);
}
void
RadioddityCodeplug::ScanListElement::clearMember(unsigned n) {
  setUInt16_le(0x0010+2*n, 0);
}

bool
RadioddityCodeplug::ScanListElement::hasPrimary() const {
  return 0 != getUInt16_le(0x0050);
}
bool
RadioddityCodeplug::ScanListElement::primaryIsSelected() const {
  return 1 == getUInt16_le(0x0050);
}
unsigned
RadioddityCodeplug::ScanListElement::primary() const {
  return getUInt16_le(0x0050)-1;
}
void
RadioddityCodeplug::ScanListElement::setPrimary(unsigned idx) {
  setUInt16_le(0x0050, idx+1);
}
void
RadioddityCodeplug::ScanListElement::setPrimarySelected() {
  setUInt16_le(0x0050, 1);
}
void
RadioddityCodeplug::ScanListElement::clearPrimary() {
  setUInt16_le(0x0050, 0);
}

bool
RadioddityCodeplug::ScanListElement::hasSecondary() const {
  return 0 != getUInt16_le(0x0052);
}
bool
RadioddityCodeplug::ScanListElement::secondaryIsSelected() const {
  return 1 == getUInt16_le(0x0052);
}
unsigned
RadioddityCodeplug::ScanListElement::secondary() const {
  return getUInt16_le(0x0052)-1;
}
void
RadioddityCodeplug::ScanListElement::setSecondary(unsigned idx) {
  setUInt16_le(0x0052, idx+1);
}
void
RadioddityCodeplug::ScanListElement::setSecondarySelected() {
  setUInt16_le(0x0052, 1);
}
void
RadioddityCodeplug::ScanListElement::clearSecondary() {
  setUInt16_le(0x0052, 0);
}

bool
RadioddityCodeplug::ScanListElement::hasRevert() const {
  return 0 != getUInt16_le(0x0054);
}
bool
RadioddityCodeplug::ScanListElement::revertIsSelected() const {
  return 1 == getUInt16_le(0x0054);
}
unsigned
RadioddityCodeplug::ScanListElement::revert() const {
  return getUInt16_le(0x0054)-1;
}
void
RadioddityCodeplug::ScanListElement::setRevert(unsigned idx) {
  setUInt16_le(0x0054, idx+1);
}
void
RadioddityCodeplug::ScanListElement::setRevertSelected() {
  setUInt16_le(0x0054, 1);
}
void
RadioddityCodeplug::ScanListElement::clearRevert() {
  setUInt16_le(0x0054, 0);
}

unsigned
RadioddityCodeplug::ScanListElement::holdTime() const {
  return unsigned(getUInt8(0x0056))*25;
}
void
RadioddityCodeplug::ScanListElement::setHoldTime(unsigned ms) {
  setUInt8(0x0056, ms/25);
}

unsigned
RadioddityCodeplug::ScanListElement::prioritySampleTime() const {
  return unsigned(getUInt8(0x0057))*250;
}
void
RadioddityCodeplug::ScanListElement::setPrioritySampleTime(unsigned ms) {
  setUInt8(0x0057, ms/250);
}

ScanList *
RadioddityCodeplug::ScanListElement::toScanListObj(Context &ctx) const {
  Q_UNUSED(ctx)
  return new ScanList(name());
}

bool
RadioddityCodeplug::ScanListElement::linkScanListObj(ScanList *lst, Context &ctx) const {
  if (primaryIsSelected())
    lst->setPrimaryChannel(SelectedChannel::get());
  else if (hasPrimary())
    lst->setPrimaryChannel(ctx.get<Channel>(primary()));

  if (secondaryIsSelected())
    lst->setSecondaryChannel(SelectedChannel::get());
  else if (hasSecondary())
    lst->setSecondaryChannel(ctx.get<Channel>(secondary()));

  if (revertIsSelected())
    lst->setRevertChannel(SelectedChannel::get());
  else if (hasRevert())
    lst->setRevertChannel(ctx.get<Channel>(revert()));

  for (int i=0; (i<32) && hasMember(i); i++) {
    if (isSelected(i))
      lst->addChannel(SelectedChannel::get());
    else if (hasMember(i))
      lst->addChannel(ctx.get<Channel>(member(i)));
  }
  return true;
}

void
RadioddityCodeplug::ScanListElement::fromScanListObj(const ScanList *lst, Context &ctx) {
  clear();
  setName(lst->name());
  if (lst->primaryChannel() && (SelectedChannel::get() == lst->primaryChannel()))
    setPrimarySelected();
  else if (lst->primaryChannel())
    setPrimary(ctx.index(lst->primaryChannel()));

  if (lst->secondaryChannel() && (SelectedChannel::get() == lst->secondaryChannel()))
    setSecondarySelected();
  else if (lst->secondaryChannel())
    setSecondary(ctx.index(lst->secondaryChannel()));

  if (lst->revertChannel() && (SelectedChannel::get() == lst->revertChannel()))
    setRevertSelected();
  else if (lst->revertChannel())
    setRevert(ctx.index(lst->revertChannel()));

  for (int i=0; i<32; i++) {
    if (i >= lst->count())
      clearMember(i);
    else if (SelectedChannel::get() == lst->channel(i))
      setSelected(i);
    else
      setMember(i, ctx.index(lst->channel(i)));
  }
}


/* ********************************************************************************************* *
 * Implementation of RadioddityCodeplug::ScanListBankElement
 * ********************************************************************************************* */
RadioddityCodeplug::ScanListBankElement::ScanListBankElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

RadioddityCodeplug::ScanListBankElement::ScanListBankElement(uint8_t *ptr)
  : Element(ptr, 0x56f0)
{
  // pass...
}

RadioddityCodeplug::ScanListBankElement::~ScanListBankElement() {
  // pass...
}

void
RadioddityCodeplug::ScanListBankElement::clear() {
  memset(_data, 0, 256);
}

bool
RadioddityCodeplug::ScanListBankElement::isEnabled(unsigned n) const {
  return 0x00 != getUInt8(n);
}
void
RadioddityCodeplug::ScanListBankElement::enable(unsigned n, bool enabled) {
  if (enabled)
    setUInt8(n, 0x01);
  else
    setUInt8(n, 0x00);
}

uint8_t *
RadioddityCodeplug::ScanListBankElement::get(unsigned n) const {
  return _data+0x0100 + n*0x0058;
}


/* ********************************************************************************************* *
 * Implementation of RadioddityCodeplug::GeneralSettingsElement
 * ********************************************************************************************* */
RadioddityCodeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pas...
}

RadioddityCodeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr)
  : Element(ptr, 0x0028)
{
  // pass...
}

RadioddityCodeplug::GeneralSettingsElement::~GeneralSettingsElement() {
  // pass...
}

void
RadioddityCodeplug::GeneralSettingsElement::clear() {
  memset(_data+0x0000, 0xff, 8);
  memset(_data+0x0008, 0x00, 4);
  setUInt32_be(0x000c, 0);
  setUInt8(0x0010, 0);

  setPreambleDuration(360);
  setMonitorType(SILENT_MONITOR);
  setVOXSensitivity(3);
  setLowBatteryWarnInterval(30);
  setCallAlertDuration(120);
  setLoneWorkerResponsePeriod(1);
  setLoneWorkerReminderPeriod(10);
  setGroupCallHangTime(3000);
  setPrivateCallHangTime(3000);

  enableDownChannelModeVFO(false);
  enableUpChannelModeVFO(false);
  enableResetTone(false);
  enableUnknownNumberTone(false);
  setARTSToneMode(ARTS_ONCE);

  enableDigitalTalkPermitTone(false);
  enableAnalogTalkPermitTone(false);
  enableSelftestTone(true);
  enableFrequencyIndicationTone(false);
  setBit(0x001b, 4, false);
  disableAllTones(false);
  enableBatsaveRX(true);
  enableBatsavePreamble(true);

  setUInt5(0x001c, 0, 0);
  disableAllLEDs(false);
  inhibitQuickKeyOverride(false);
  setBit(0x001c, 7, true);

  setUInt3(0x001d, 0, 0);
  enableTXExitTone(false);
  enableTXOnActiveChannel(true);
  enableAnimation(false);
  setScanMode(SCANMODE_TIME);

  setRepeaterEndDelay(0);
  setRepeaterSTE(0);

  setUInt8(0x001f, 0);
  memset(_data+0x0020, 0xff, 8);
}

QString
RadioddityCodeplug::GeneralSettingsElement::name() const {
  return readASCII(0x0000, 8, 0xff);
}
void
RadioddityCodeplug::GeneralSettingsElement::setName(const QString &name) {
  writeASCII(0x0000, name, 8, 0xff);
}

unsigned
RadioddityCodeplug::GeneralSettingsElement::radioID() const {
  return getBCD8_be(0x0008);
}
void
RadioddityCodeplug::GeneralSettingsElement::setRadioID(unsigned id) {
  setBCD8_be(0x0008, id);
}

unsigned
RadioddityCodeplug::GeneralSettingsElement::preambleDuration() const {
  return unsigned(getUInt8(0x0011)*60);
}
void
RadioddityCodeplug::GeneralSettingsElement::setPreambleDuration(unsigned ms) {
  setUInt8(0x0011, ms/60);
}

RadioddityCodeplug::GeneralSettingsElement::MonitorType
RadioddityCodeplug::GeneralSettingsElement::monitorType() const {
  return MonitorType(getUInt8(0x0012));
}
void
RadioddityCodeplug::GeneralSettingsElement::setMonitorType(MonitorType type) {
  setUInt8(0x0012, (unsigned)type);
}

unsigned
RadioddityCodeplug::GeneralSettingsElement::voxSensitivity() const {
  return getUInt8(0x0013);
}
void
RadioddityCodeplug::GeneralSettingsElement::setVOXSensitivity(unsigned value) {
  value = std::min(10u, std::max(1u, value));
  setUInt8(0x0013, value);
}

unsigned
RadioddityCodeplug::GeneralSettingsElement::lowBatteryWarnInterval() const {
  return unsigned(getUInt8(0x0014))*5;
}
void
RadioddityCodeplug::GeneralSettingsElement::setLowBatteryWarnInterval(unsigned sec) {
  setUInt8(0x0014, sec/5);
}

unsigned
RadioddityCodeplug::GeneralSettingsElement::callAlertDuration() const {
  return unsigned(getUInt8(0x0015))*5;
}
void
RadioddityCodeplug::GeneralSettingsElement::setCallAlertDuration(unsigned sec) {
  setUInt8(0x0015, sec/5);
}

unsigned
RadioddityCodeplug::GeneralSettingsElement::loneWorkerResponsePeriod() const {
  return getUInt8(0x0016);
}
void
RadioddityCodeplug::GeneralSettingsElement::setLoneWorkerResponsePeriod(unsigned min) {
  setUInt8(0x0016, min);
}
unsigned
RadioddityCodeplug::GeneralSettingsElement::loneWorkerReminderPeriod() const {
  return getUInt8(0x0017);
}
void
RadioddityCodeplug::GeneralSettingsElement::setLoneWorkerReminderPeriod(unsigned sec) {
  setUInt8(0x0017, sec);
}

unsigned
RadioddityCodeplug::GeneralSettingsElement::groupCallHangTime() const {
  return unsigned(getUInt8(0x0018))*500;
}
void
RadioddityCodeplug::GeneralSettingsElement::setGroupCallHangTime(unsigned ms) {
  setUInt8(0x0018, ms/500);
}
unsigned
RadioddityCodeplug::GeneralSettingsElement::privateCallHangTime() const {
  return unsigned(getUInt8(0x0019))*500;
}
void
RadioddityCodeplug::GeneralSettingsElement::setPrivateCallHangTime(unsigned ms) {
  setUInt8(0x0019, ms/500);
}

bool
RadioddityCodeplug::GeneralSettingsElement::downChannelModeVFO() const {
  return getBit(0x001a, 0);
}
void
RadioddityCodeplug::GeneralSettingsElement::enableDownChannelModeVFO(bool enable) {
  setBit(0x001a, 0, enable);
}
bool
RadioddityCodeplug::GeneralSettingsElement::upChannelModeVFO() const {
  return getBit(0x001a, 1);
}
void
RadioddityCodeplug::GeneralSettingsElement::enableUpChannelModeVFO(bool enable) {
  setBit(0x001a, 1, enable);
}

bool
RadioddityCodeplug::GeneralSettingsElement::resetTone() const {
  return getBit(0x001a, 2);
}
void
RadioddityCodeplug::GeneralSettingsElement::enableResetTone(bool enable) {
  setBit(0x001a, 2, enable);
}
bool
RadioddityCodeplug::GeneralSettingsElement::unknownNumberTone() const {
  return getBit(0x001a, 3);
}
void
RadioddityCodeplug::GeneralSettingsElement::enableUnknownNumberTone(bool enable) {
  setBit(0x001a, 3, enable);
}

RadioddityCodeplug::GeneralSettingsElement::ARTSTone
RadioddityCodeplug::GeneralSettingsElement::artsToneMode() const {
  return ARTSTone(getUInt4(0x001a, 4));
}
void
RadioddityCodeplug::GeneralSettingsElement::setARTSToneMode(ARTSTone mode) {
  setUInt4(0x001a, 4, (unsigned) mode);
}

bool
RadioddityCodeplug::GeneralSettingsElement::digitalTalkPermitTone() const {
  return getBit(0x001b, 0);
}
void
RadioddityCodeplug::GeneralSettingsElement::enableDigitalTalkPermitTone(bool enable) {
  setBit(0x001b, 0, enable);
}
bool
RadioddityCodeplug::GeneralSettingsElement::analogTalkPermitTone() const {
  return getBit(0x001b, 1);
}
void
RadioddityCodeplug::GeneralSettingsElement::enableAnalogTalkPermitTone(bool enable) {
  setBit(0x001b, 1, enable);
}
bool
RadioddityCodeplug::GeneralSettingsElement::selftestTone() const {
  return getBit(0x001b, 2);
}
void
RadioddityCodeplug::GeneralSettingsElement::enableSelftestTone(bool enable) {
  setBit(0x001b, 2, enable);
}
bool
RadioddityCodeplug::GeneralSettingsElement::frequencyIndicationTone() const {
  return getBit(0x001b, 3);
}
void
RadioddityCodeplug::GeneralSettingsElement::enableFrequencyIndicationTone(bool enable) {
  setBit(0x001b, 3, enable);
}
bool
RadioddityCodeplug::GeneralSettingsElement::allTonesDisabled() const {
  return getBit(0x001b, 5);
}
void
RadioddityCodeplug::GeneralSettingsElement::disableAllTones(bool disable) {
  setBit(0x001b, 5, disable);
}
bool
RadioddityCodeplug::GeneralSettingsElement::batsaveRX() const {
  return getBit(0x001b, 6);
}
void
RadioddityCodeplug::GeneralSettingsElement::enableBatsaveRX(bool enable) {
  setBit(0x001b, 6, enable);
}
bool
RadioddityCodeplug::GeneralSettingsElement::batsavePreamble() const {
  return getBit(0x001b, 7);
}
void
RadioddityCodeplug::GeneralSettingsElement::enableBatsavePreamble(bool enable) {
  setBit(0x001b, 7, enable);
}

bool
RadioddityCodeplug::GeneralSettingsElement::allLEDsDisabled() const {
  return getBit(0x001c, 5);
}
void
RadioddityCodeplug::GeneralSettingsElement::disableAllLEDs(bool disable) {
  setBit(0x001c, 5, disable);
}
bool
RadioddityCodeplug::GeneralSettingsElement::quickKeyOverrideInhibited() const {
  return getBit(0x001c, 6);
}
void
RadioddityCodeplug::GeneralSettingsElement::inhibitQuickKeyOverride(bool inhibit) {
  setBit(0x001c, 6, inhibit);
}

bool
RadioddityCodeplug::GeneralSettingsElement::txExitTone() const {
  return getBit(0x001d, 3);
}
void
RadioddityCodeplug::GeneralSettingsElement::enableTXExitTone(bool enable) {
  setBit(0x001d, 3, enable);
}
bool
RadioddityCodeplug::GeneralSettingsElement::txOnActiveChannel() const {
  return getBit(0x001d, 4);
}
void
RadioddityCodeplug::GeneralSettingsElement::enableTXOnActiveChannel(bool enable) {
  setBit(0x001d, 4, enable);
}
bool
RadioddityCodeplug::GeneralSettingsElement::animation() const {
  return getBit(0x001d, 5);
}
void
RadioddityCodeplug::GeneralSettingsElement::enableAnimation(bool enable) {
  setBit(0x001d, 5, enable);
}
RadioddityCodeplug::GeneralSettingsElement::ScanMode
RadioddityCodeplug::GeneralSettingsElement::scanMode() const {
  return ScanMode(getUInt2(0x001d, 6));
}
void
RadioddityCodeplug::GeneralSettingsElement::setScanMode(ScanMode mode) {
  setUInt2(0x001d, 6, unsigned(mode));
}

unsigned
RadioddityCodeplug::GeneralSettingsElement::repeaterEndDelay() const {
  return getUInt4(0x001e, 0);
}
void
RadioddityCodeplug::GeneralSettingsElement::setRepeaterEndDelay(unsigned delay) {
  setUInt4(0x001e, 0, delay);
}
unsigned
RadioddityCodeplug::GeneralSettingsElement::repeaterSTE() const {
  return getUInt4(0x001e, 4);
}
void
RadioddityCodeplug::GeneralSettingsElement::setRepeaterSTE(unsigned ste) {
  setUInt4(0x001e, 4, ste);
}

QString
RadioddityCodeplug::GeneralSettingsElement::progPassword() const {
  return readASCII(0x0020, 8, 0xff);
}
void
RadioddityCodeplug::GeneralSettingsElement::setProgPassword(const QString &pwd) {
  writeASCII(0x0020, pwd, 8, 0xff);
}

bool
RadioddityCodeplug::GeneralSettingsElement::fromConfig(const Config *conf, Context &ctx) {
  if (conf->radioIDs()->defaultId()) {
    setName(conf->radioIDs()->defaultId()->name());
    setRadioID(conf->radioIDs()->defaultId()->number());
  } else if (conf->radioIDs()->count()) {
    setName(conf->radioIDs()->getId(0)->name());
    setRadioID(conf->radioIDs()->getId(0)->number());
  } else {
    logError() << "Cannot encode radioddity codeplug: No radio ID defined.";
    return false;
  }

  setVOXSensitivity(ctx.config()->settings()->vox());
  // There is no global squelch settings either...
  return true;
}

bool
RadioddityCodeplug::GeneralSettingsElement::updateConfig(Config *conf, Context &ctx) {
  Q_UNUSED(ctx)
  if (! conf->radioIDs()->defaultId()) {
    int idx = conf->radioIDs()->add(new RadioID(name(), radioID()));
    conf->radioIDs()->setDefaultId(idx);
  }
  conf->radioIDs()->defaultId()->setName(name());
  conf->radioIDs()->defaultId()->setNumber(radioID());
  conf->settings()->setVOX(voxSensitivity());
  return true;
}


/* ********************************************************************************************* *
 * Implementation of RadioddityCodeplug::ButtonSettingsElement
 * ********************************************************************************************* */
RadioddityCodeplug::ButtonSettingsElement::ButtonSettingsElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

RadioddityCodeplug::ButtonSettingsElement::ButtonSettingsElement(uint8_t *ptr)
  : Element(ptr, 0x0020)
{
  // pass...
}

RadioddityCodeplug::ButtonSettingsElement::~ButtonSettingsElement() {
  // pass...
}

void
RadioddityCodeplug::ButtonSettingsElement::clear() {
  setUInt8(0x0000, 0x01);
  setLongPressDuration(1500);
  setSK1ShortPress(Action::ZoneSelect);
  setSK1LongPress(Action::ToggleFMRadio);
  setSK2ShortPress(Action::ToggleMonitor);
  setSK2LongPress(Action::ToggleFlashLight);
  setTKShortPress(Action::BatteryIndicator);
  setTKLongPress(Action::ToggleVox);
  memset(_data+0x0008, 0xff, 6*4);
}

unsigned
RadioddityCodeplug::ButtonSettingsElement::longPressDuration() const {
  return unsigned(getUInt8(0x0001))*250;
}
void
RadioddityCodeplug::ButtonSettingsElement::setLongPressDuration(unsigned ms) {
  setUInt8(0x0001, ms/250);
}

RadioddityCodeplug::ButtonSettingsElement::Action
RadioddityCodeplug::ButtonSettingsElement::sk1ShortPress() const {
  return (Action) getUInt8(0x0002);
}
void
RadioddityCodeplug::ButtonSettingsElement::setSK1ShortPress(Action action) {
  setUInt8(0x0002, (unsigned)action);
}
RadioddityCodeplug::ButtonSettingsElement::Action
RadioddityCodeplug::ButtonSettingsElement::sk1LongPress() const {
  return (Action) getUInt8(0x0003);
}
void
RadioddityCodeplug::ButtonSettingsElement::setSK1LongPress(Action action) {
  setUInt8(0x0003, (unsigned)action);
}

RadioddityCodeplug::ButtonSettingsElement::Action
RadioddityCodeplug::ButtonSettingsElement::sk2ShortPress() const {
  return (Action) getUInt8(0x0004);
}
void
RadioddityCodeplug::ButtonSettingsElement::setSK2ShortPress(Action action) {
  setUInt8(0x0004, (unsigned)action);
}
RadioddityCodeplug::ButtonSettingsElement::Action
RadioddityCodeplug::ButtonSettingsElement::sk2LongPress() const {
  return (Action) getUInt8(0x0005);
}
void
RadioddityCodeplug::ButtonSettingsElement::setSK2LongPress(Action action) {
  setUInt8(0x0005, (unsigned)action);
}

RadioddityCodeplug::ButtonSettingsElement::Action
RadioddityCodeplug::ButtonSettingsElement::tkShortPress() const {
  return (Action) getUInt8(0x0006);
}
void
RadioddityCodeplug::ButtonSettingsElement::setTKShortPress(Action action) {
  setUInt8(0x0006, (unsigned)action);
}
RadioddityCodeplug::ButtonSettingsElement::Action
RadioddityCodeplug::ButtonSettingsElement::tkLongPress() const {
  return (Action) getUInt8(0x0007);
}
void
RadioddityCodeplug::ButtonSettingsElement::setTKLongPress(Action action) {
  setUInt8(0x0007, (unsigned)action);
}

RadioddityCodeplug::ButtonSettingsElement::OneTouchAction
RadioddityCodeplug::ButtonSettingsElement::oneTouchAction(unsigned n) const {
  return OneTouchAction(getUInt8(0x0008 + n*4 + 0));
}
unsigned
RadioddityCodeplug::ButtonSettingsElement::oneTouchContact(unsigned n) const {
  return getUInt16_be(0x0008 + n*4 + 1);
}
unsigned
RadioddityCodeplug::ButtonSettingsElement::oneTouchMessage(unsigned n) const {
  return getUInt16_be(0x0008 + n*4 + 3);
}
void
RadioddityCodeplug::ButtonSettingsElement::disableOneTouch(unsigned n) {
  setUInt8(0x0008 + n*4 + 0, (unsigned)OneTouchAction::None);
}
void
RadioddityCodeplug::ButtonSettingsElement::setOneTouchDigitalCall(unsigned n, unsigned index) {
  setUInt8(0x0008 + n*4 + 0, (unsigned)OneTouchAction::DigitalCall);
  setUInt16_be(0x0008 + n*4 + 1, index);
  setUInt16_be(0x0008 + n*4 + 3, 0);
}
void
RadioddityCodeplug::ButtonSettingsElement::setOneTouchDigitalMessage(unsigned n, unsigned index) {
  setUInt8(0x0008 + n*4 + 0, (unsigned)OneTouchAction::DigitalMessage);
  setUInt16_be(0x0008 + n*4 + 1, 0);
  setUInt16_be(0x0008 + n*4 + 3, index);
}
void
RadioddityCodeplug::ButtonSettingsElement::setOneTouchAnalogCall(unsigned n) {
  setUInt8(0x0008 + n*4 + 0, (unsigned)OneTouchAction::AnalogCall);
  setUInt16_be(0x0008 + n*4 + 1, 0);
  setUInt16_be(0x0008 + n*4 + 3, 0);
}


/* ********************************************************************************************* *
 * Implementation of RadioddityCodeplug::MenuSettingsElement
 * ********************************************************************************************* */
RadioddityCodeplug::MenuSettingsElement::MenuSettingsElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

RadioddityCodeplug::MenuSettingsElement::MenuSettingsElement(uint8_t *ptr)
  : Element(ptr, 0x0008)
{
  // pass...
}

RadioddityCodeplug::MenuSettingsElement::~MenuSettingsElement() {
  // pass...
}

void
RadioddityCodeplug::MenuSettingsElement::clear() {
  setMenuHangTime(10);

  enableMessage(true);
  enableScanStart(true);
  enableCallAlert(true);
  enableEditContact(true);
  enableManualDial(true);
  enableRadioCheck(true);
  enableRemoteMonitor(true);

  enableRadioEnable(true);
  enableRadioDisable(true);
  enableProgPassword(true);
  enableTalkaround(true);
  enableTone(true);
  enablePower(true);
  enableBacklight(true);
  enableIntroScreen(true);

  enableKeypadLock(true);
  enableLEDIndicator(true);
  enableSquelch(true);
  enablePrivacy(true);
  enableVOX(true);
  enablePasswordLock(true);
  enableMissedCalls(true);
  enableAnsweredCalls(true);

  enableOutgoingCalls(true);
  enableChannelDisplay(true);
  enableDualWatch(true);
  setBit(0x0004, 3, 0); setBit(0x0004, 4, 0); setBit(0x0004, 5, 1); setBit(0x0004, 6, 1);
  setBit(0x0004, 7, 1);

  setUInt8(0x0005, 0xff);

  setKeypadLockTime(0);
  setBacklightTime(15);
  setUInt2(0x0006, 4, 0);
  setChannelDisplayMode(ChannelDisplayMode::Name);

  setUInt4(0x0007, 0, 0);
  setBit(0x0007, 4, 1);
  enableKeyTone(true);
  setDualWatchMode(DualWatchMode::DualDual);
}

unsigned
RadioddityCodeplug::MenuSettingsElement::menuHangTime() const {
  return getUInt8(0x0000);
}
void
RadioddityCodeplug::MenuSettingsElement::setMenuHangTime(unsigned sec) {
  sec = std::min(10u, sec);
  setUInt8(0x0000, sec);
}

bool
RadioddityCodeplug::MenuSettingsElement::message() const {
  return getBit(0x0001, 0);
}
void
RadioddityCodeplug::MenuSettingsElement::enableMessage(bool enable) {
  setBit(0x0001, 0, enable);
}
bool
RadioddityCodeplug::MenuSettingsElement::scanStart() const {
  return getBit(0x0001, 1);
}
void
RadioddityCodeplug::MenuSettingsElement::enableScanStart(bool enable) {
  setBit(0x0001, 1, enable);
}
bool
RadioddityCodeplug::MenuSettingsElement::editScanList() const {
  return getBit(0x0001, 2);
}
void
RadioddityCodeplug::MenuSettingsElement::enableEditScanList(bool enable) {
  setBit(0x0001, 2, enable);
}
bool
RadioddityCodeplug::MenuSettingsElement::callAlert() const {
  return getBit(0x0001, 3);
}
void
RadioddityCodeplug::MenuSettingsElement::enableCallAlert(bool enable) {
  setBit(0x0001, 3, enable);
}
bool
RadioddityCodeplug::MenuSettingsElement::editContact() const {
  return getBit(0x0001, 4);
}
void
RadioddityCodeplug::MenuSettingsElement::enableEditContact(bool enable) {
  setBit(0x0001, 4, enable);
}
bool
RadioddityCodeplug::MenuSettingsElement::manualDial() const {
  return getBit(0x0001, 5);
}
void
RadioddityCodeplug::MenuSettingsElement::enableManualDial(bool enable) {
  setBit(0x0001, 5, enable);
}
bool
RadioddityCodeplug::MenuSettingsElement::radioCheck() const {
  return getBit(0x0001, 6);
}
void
RadioddityCodeplug::MenuSettingsElement::enableRadioCheck(bool enable) {
  setBit(0x0001, 6, enable);
}
bool
RadioddityCodeplug::MenuSettingsElement::remoteMonitor() const {
  return getBit(0x0001, 7);
}
void
RadioddityCodeplug::MenuSettingsElement::enableRemoteMonitor(bool enable) {
  setBit(0x0001, 7, enable);
}

bool
RadioddityCodeplug::MenuSettingsElement::radioEnable() const {
  return getBit(0x0002, 0);
}
void
RadioddityCodeplug::MenuSettingsElement::enableRadioEnable(bool enable) {
  setBit(0x0002, 0, enable);
}
bool
RadioddityCodeplug::MenuSettingsElement::radioDisable() const {
  return getBit(0x0002, 1);
}
void
RadioddityCodeplug::MenuSettingsElement::enableRadioDisable(bool enable) {
  setBit(0x0002, 1, enable);
}
bool
RadioddityCodeplug::MenuSettingsElement::progPassword() const {
  return getBit(0x0002, 2);
}
void
RadioddityCodeplug::MenuSettingsElement::enableProgPassword(bool enable) {
  setBit(0x0002, 2, enable);
}
bool
RadioddityCodeplug::MenuSettingsElement::talkaround() const {
  return getBit(0x0002, 3);
}
void
RadioddityCodeplug::MenuSettingsElement::enableTalkaround(bool enable) {
  setBit(0x0002, 3, enable);
}
bool
RadioddityCodeplug::MenuSettingsElement::tone() const {
  return getBit(0x0002, 4);
}
void
RadioddityCodeplug::MenuSettingsElement::enableTone(bool enable) {
  setBit(0x0002, 4, enable);
}
bool
RadioddityCodeplug::MenuSettingsElement::power() const {
  return getBit(0x0002, 5);
}
void
RadioddityCodeplug::MenuSettingsElement::enablePower(bool enable) {
  setBit(0x0002, 5, enable);
}
bool
RadioddityCodeplug::MenuSettingsElement::backlight() const {
  return getBit(0x0002, 6);
}
void
RadioddityCodeplug::MenuSettingsElement::enableBacklight(bool enable) {
  setBit(0x0002, 6, enable);
}
bool
RadioddityCodeplug::MenuSettingsElement::introScreen() const {
  return getBit(0x0002, 7);
}
void
RadioddityCodeplug::MenuSettingsElement::enableIntroScreen(bool enable) {
  setBit(0x0002, 7, enable);
}

bool
RadioddityCodeplug::MenuSettingsElement::keypadLock() const {
  return getBit(0x0003, 0);
}
void
RadioddityCodeplug::MenuSettingsElement::enableKeypadLock(bool enable) {
  setBit(0x0003, 0, enable);
}
bool
RadioddityCodeplug::MenuSettingsElement::ledIndicator() const {
  return getBit(0x0003, 1);
}
void
RadioddityCodeplug::MenuSettingsElement::enableLEDIndicator(bool enable) {
  setBit(0x0003, 1, enable);
}
bool
RadioddityCodeplug::MenuSettingsElement::squelch() const {
  return getBit(0x0003, 2);
}
void
RadioddityCodeplug::MenuSettingsElement::enableSquelch(bool enable) {
  setBit(0x0003, 2, enable);
}
bool
RadioddityCodeplug::MenuSettingsElement::privacy() const {
  return getBit(0x0003, 3);
}
void
RadioddityCodeplug::MenuSettingsElement::enablePrivacy(bool enable) {
  setBit(0x0003, 3, enable);
}
bool
RadioddityCodeplug::MenuSettingsElement::vox() const {
  return getBit(0x0003, 4);
}
void
RadioddityCodeplug::MenuSettingsElement::enableVOX(bool enable) {
  setBit(0x0003, 4, enable);
}
bool
RadioddityCodeplug::MenuSettingsElement::passwordLock() const {
  return getBit(0x0003, 5);
}
void
RadioddityCodeplug::MenuSettingsElement::enablePasswordLock(bool enable) {
  setBit(0x0003, 5, enable);
}
bool
RadioddityCodeplug::MenuSettingsElement::missedCalls() const {
  return getBit(0x0003, 6);
}
void
RadioddityCodeplug::MenuSettingsElement::enableMissedCalls(bool enable) {
  setBit(0x0003, 6, enable);
}
bool
RadioddityCodeplug::MenuSettingsElement::answeredCalls() const {
  return getBit(0x0003, 7);
}
void
RadioddityCodeplug::MenuSettingsElement::enableAnsweredCalls(bool enable) {
  setBit(0x0003, 7, enable);
}

bool
RadioddityCodeplug::MenuSettingsElement::outgoingCalls() const {
  return getBit(0x0004, 0);
}
void
RadioddityCodeplug::MenuSettingsElement::enableOutgoingCalls(bool enable) {
  setBit(0x0004, 0, enable);
}

bool
RadioddityCodeplug::MenuSettingsElement::channelDisplay() const {
  return getBit(0x0004, 1);
}
void
RadioddityCodeplug::MenuSettingsElement::enableChannelDisplay(bool enable) {
  setBit(0x0004, 1, enable);
}
bool
RadioddityCodeplug::MenuSettingsElement::dualWatch() const {
  return getBit(0x0004, 2);
}
void
RadioddityCodeplug::MenuSettingsElement::enableDualWatch(bool enable) {
  setBit(0x0004, 2, enable);
}

unsigned
RadioddityCodeplug::MenuSettingsElement::keypadLockTime() const {
  return getUInt2(0x0006, 0)*5;
}
void
RadioddityCodeplug::MenuSettingsElement::setKeypadLockTime(unsigned sec) {
  setUInt2(0x0006, 0, sec/5);
}

unsigned
RadioddityCodeplug::MenuSettingsElement::backlightTime() const {
  return getUInt2(0x0006, 2)*5;
}
void
RadioddityCodeplug::MenuSettingsElement::setBacklightTime(unsigned sec) {
  setUInt2(0x0006, 2, sec/5);
}

RadioddityCodeplug::MenuSettingsElement::ChannelDisplayMode
RadioddityCodeplug::MenuSettingsElement::channelDisplayMode() const {
  return (ChannelDisplayMode)getUInt2(0x0006, 6);
}
void
RadioddityCodeplug::MenuSettingsElement::setChannelDisplayMode(ChannelDisplayMode mode) {
  setUInt2(0x0006, 6, (unsigned)mode);
}

bool
RadioddityCodeplug::MenuSettingsElement::keyTone() const {
  return getBit(0x0007, 5);
}
void
RadioddityCodeplug::MenuSettingsElement::enableKeyTone(bool enable) {
  setBit(0x0007, 5, enable);
}

RadioddityCodeplug::MenuSettingsElement::DualWatchMode
RadioddityCodeplug::MenuSettingsElement::dualWatchMode() const {
  return DualWatchMode(getUInt2(0x0007, 6));
}
void
RadioddityCodeplug::MenuSettingsElement::setDualWatchMode(DualWatchMode mode) {
  setUInt2(0x0007, 6, (unsigned)mode);
}


/* ********************************************************************************************* *
 * Implementation of RadioddityCodeplug::BootSettingsElement
 * ********************************************************************************************* */
RadioddityCodeplug::BootSettingsElement::BootSettingsElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

RadioddityCodeplug::BootSettingsElement::BootSettingsElement(uint8_t *ptr)
  : Element(ptr, 0x20)
{
  // pass...
}

RadioddityCodeplug::BootSettingsElement::~BootSettingsElement() {
  // pass...
}

void
RadioddityCodeplug::BootSettingsElement::clear() {
  enableBootText(true);
  enableBootPassword(false);
  setBCD8_be(0x0002, 0);
  setUInt8(0x0007, 0);
  memset(_data+0x0008, 0, 24);
}

bool
RadioddityCodeplug::BootSettingsElement::bootText() const {
  return (1 == getUInt8(0x0000));
}
void
RadioddityCodeplug::BootSettingsElement::enableBootText(bool enable) {
  setUInt8(0x0000, (enable ? 1 :0));
}

bool
RadioddityCodeplug::BootSettingsElement::bootPasswordEnabled() const {
  return (1 == getUInt8(0x0001));
}
void
RadioddityCodeplug::BootSettingsElement::enableBootPassword(bool enable) {
  setUInt8(0x0001, (enable ? 1 : 0));
}

unsigned
RadioddityCodeplug::BootSettingsElement::bootPassword() const {
  return getBCD8_be(0x0002);
}
void
RadioddityCodeplug::BootSettingsElement::setBootPassword(unsigned passwd) {
  setBCD8_be(0x0002, passwd);
}

/* ********************************************************************************************* *
 * Implementation of RadioddityCodeplug::BootTextElement
 * ********************************************************************************************* */
RadioddityCodeplug::BootTextElement::BootTextElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

RadioddityCodeplug::BootTextElement::BootTextElement(uint8_t *ptr)
  : Element(ptr, 0x0020)
{
  // pass...
}

RadioddityCodeplug::BootTextElement::~BootTextElement() {
  // pass...
}

void
RadioddityCodeplug::BootTextElement::clear() {
  setLine1("");
  setLine2("");
}

QString
RadioddityCodeplug::BootTextElement::line1() const {
  return readASCII(0x0000, 16, 0xff);
}
void
RadioddityCodeplug::BootTextElement::setLine1(const QString &text) {
  writeASCII(0x0000, text, 16, 0xff);
}

QString
RadioddityCodeplug::BootTextElement::line2() const {
  return readASCII(0x0010, 16, 0xff);
}
void
RadioddityCodeplug::BootTextElement::setLine2(const QString &text) {
  writeASCII(0x0010, text, 16, 0xff);
}

void
RadioddityCodeplug::BootTextElement::fromConfig(Config *conf) {
  setLine1(conf->settings()->introLine1());
  setLine2(conf->settings()->introLine2());
}

void
RadioddityCodeplug::BootTextElement::updateConfig(Config *conf) {
  conf->settings()->setIntroLine1(line1());
  conf->settings()->setIntroLine2(line2());
}


/* ********************************************************************************************* *
 * Implementation of RadioddityCodeplug::MessageBankElement
 * ********************************************************************************************* */
RadioddityCodeplug::MessageBankElement::MessageBankElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

RadioddityCodeplug::MessageBankElement::MessageBankElement(uint8_t *ptr)
  : Element(ptr, 0x1248)
{
  // pass...
}

RadioddityCodeplug::MessageBankElement::~MessageBankElement() {
  // pass...
}

void
RadioddityCodeplug::MessageBankElement::clear() {
  setUInt8(0x0000, 0); // set count to 0
  memset(_data+0x0001, 0x00, 7); // Fill unused
  memset(_data+0x0008, 0x00, 32); // Set message lengths to 0
  memset(_data+0x0028, 0x00, 32); // Fill unused
  memset(_data+0x0048, 0xff, 32*144); // Clear all messages
}

unsigned
RadioddityCodeplug::MessageBankElement::numMessages() const {
  return getUInt8(0x0000);
}
QString
RadioddityCodeplug::MessageBankElement::message(unsigned n) const {
  if (n >= numMessages())
    return QString();
  return readASCII(0x0048+n*144, 144, 0xff);
}
void
RadioddityCodeplug::MessageBankElement::appendMessage(const QString msg) {
  unsigned idx = numMessages();
  if (idx >= 32)
    return;
  unsigned len = std::min(msg.size(), 144);
  // increment counter
  setUInt8(0x0000, idx+1);
  // store length
  setUInt8(0x0008+idx, len);
  // store string
  writeASCII(0x0048+144*len, msg, 144, 0xff);
}


/* ********************************************************************************************* *
 * Implementation of RadioddityCodeplug
 * ********************************************************************************************* */
RadioddityCodeplug::RadioddityCodeplug(QObject *parent)
  : Codeplug(parent)
{
  // pass...
}

RadioddityCodeplug::~RadioddityCodeplug() {
  // pass...
}

void
RadioddityCodeplug::clear() {
  // Clear general config
  clearGeneralSettings();
  // Clear button settings
  clearButtonSettings();
  // Clear messsages
  clearMessages();
  // Clear emergency systems
  //clearEmergencySystems();
  // Clear contacts
  clearContacts();
  // clear DTMF contacts
  clearDTMFContacts();
  // clear boot settings
  clearBootSettings();
  // clear menu settings
  clearMenuSettings();
  // clear boot text
  clearBootText();
  // clear VFO settings
  clearVFOSettings();
  // clear zones
  clearZones();
  // clear scan lists
  clearScanLists();
  // clear group lists
  clearGroupLists();
}

bool
RadioddityCodeplug::index(Config *config, Context &ctx) const {
  // All indices as 1-based. That is, the first channel gets index 1.

  // Map radio IDs
  for (int i=0; i<config->radioIDs()->count(); i++)
    ctx.add(config->radioIDs()->getId(i), i+1);

  // Map digital and DTMF contacts
  for (int i=0, d=0, a=0; i<config->contacts()->count(); i++) {
    if (config->contacts()->contact(i)->is<DigitalContact>()) {
      ctx.add(config->contacts()->contact(i)->as<DigitalContact>(), d+1); d++;
    } else if (config->contacts()->contact(i)->is<DTMFContact>()) {
      ctx.add(config->contacts()->contact(i)->as<DTMFContact>(), a+1); a++;
    }
  }

  // Map rx group lists
  for (int i=0; i<config->rxGroupLists()->count(); i++)
    ctx.add(config->rxGroupLists()->list(i), i+1);

  // Map channels
  for (int i=0; i<config->channelList()->count(); i++)
    ctx.add(config->channelList()->channel(i), i+1);

  // Map zones
  for (int i=0; i<config->zones()->count(); i++)
    ctx.add(config->zones()->zone(i), i+1);

  // Map scan lists
  for (int i=0; i<config->scanlists()->count(); i++)
    ctx.add(config->scanlists()->scanlist(i), i+1);

  // Map DMR APRS systems
  for (int i=0,a=0,d=0; i<config->posSystems()->count(); i++) {
    if (config->posSystems()->system(i)->is<GPSSystem>()) {
      ctx.add(config->posSystems()->system(i)->as<GPSSystem>(), d+1); d++;
    } else if (config->posSystems()->system(i)->is<APRSSystem>()) {
      ctx.add(config->posSystems()->system(i)->as<APRSSystem>(), a+1); a++;
    }
  }

  // Map roaming
  for (int i=0; i<config->roaming()->count(); i++)
    ctx.add(config->roaming()->zone(i), i+1);

  return true;
}

bool
RadioddityCodeplug::encode(Config *config, const Flags &flags) {
  // Check if default DMR id is set.
  if (nullptr == config->radioIDs()->defaultId()) {
    _errorMessage = tr("Cannot encode TyT codeplug: No default radio ID specified.");
    return false;
  }

  // Create index<->object table.
  Context ctx(config);
  if (! index(config, ctx))
    return false;

  return this->encodeElements(flags, ctx);
}

bool
RadioddityCodeplug::encodeElements(const Flags &flags, Context &ctx) {
  // General config
  if (! this->encodeGeneralSettings(ctx.config(), flags, ctx)) {
    _errorMessage = tr("Cannot encode general settings: %1").arg(_errorMessage);
    return false;
  }

  // Define Contacts
  if (! this->encodeContacts(ctx.config(), flags, ctx)) {
    _errorMessage = tr("Cannot encode contacts: %1").arg(_errorMessage);
    return false;
  }

  if (! this->encodeDTMFContacts(ctx.config(), flags, ctx)) {
    _errorMessage = tr("Cannot encode DTMF contacts: %1").arg(_errorMessage);
    return false;
  }

  if (! this->encodeChannels(ctx.config(), flags, ctx)) {
    _errorMessage = tr("Cannot encode channels: %1").arg(_errorMessage);
    return false;
  }

  if (! this->encodeBootText(ctx.config(), flags, ctx)) {
    _errorMessage = tr("Cannot encode boot text: %1").arg(_errorMessage);
    return false;
  }

  if (! this->encodeZones(ctx.config(), flags, ctx)) {
    _errorMessage = tr("Cannot encode zones: %1").arg(_errorMessage);
    return false;
  }

  if (! this->encodeScanLists(ctx.config(), flags, ctx)) {
    _errorMessage = tr("Cannot encode scan lists: %1").arg(_errorMessage);
    return false;
  }

  if (! this->encodeGroupLists(ctx.config(), flags, ctx)) {
    _errorMessage = tr("Cannot encode group lists: %1").arg(_errorMessage);
    return false;
  }

  return true;
}

bool
RadioddityCodeplug::decode(Config *config) {
  // Clear config object
  config->clear();

  // Create index<->object table.
  Context ctx(config);

  return this->decodeElements(ctx);
}

bool
RadioddityCodeplug::decodeElements(Context &ctx) {
  if (! this->decodeGeneralSettings(ctx.config(), ctx)) {
    _errorMessage = tr("Cannot decode general settings: %1").arg(_errorMessage);
    return false;
  }

  if (! this->createContacts(ctx.config(), ctx)) {
    _errorMessage = tr("Cannot create contacts: %1").arg(_errorMessage);
    return false;
  }

  if (! this->createDTMFContacts(ctx.config(), ctx)) {
    _errorMessage = tr("Cannot create DTMF contacts: %1").arg(_errorMessage);
    return false;
  }

  if (! this->createChannels(ctx.config(), ctx)) {
    _errorMessage = tr("Cannot create channels: %1").arg(_errorMessage);
    return false;
  }

  if (! this->decodeBootText(ctx.config(), ctx)) {
    _errorMessage = tr("Cannot decode boot text: %1").arg(_errorMessage);
    return false;
  }

  if (! this->createZones(ctx.config(), ctx)) {
    _errorMessage = tr("Cannot create zones: %1").arg(_errorMessage);
    return false;
  }

  if (! this->createScanLists(ctx.config(), ctx)) {
    _errorMessage = tr("Cannot create scan lists: %1").arg(_errorMessage);
    return false;
  }

  if (! this->createGroupLists(ctx.config(), ctx)) {
    _errorMessage = tr("Cannot create group lists: %1").arg(_errorMessage);
    return false;
  }

  if (! this->linkChannels(ctx.config(), ctx)) {
    _errorMessage = tr("Cannot link channels: %1").arg(_errorMessage);
    return false;
  }

  if (! this->linkZones(ctx.config(), ctx)) {
    _errorMessage = tr("Cannot link zones: %1").arg(_errorMessage);
    return false;
  }

  if (! this->linkScanLists(ctx.config(), ctx)) {
    _errorMessage = tr("Cannot link scan lists: %1").arg(_errorMessage);
    return false;
  }

  if (! this->linkGroupLists(ctx.config(), ctx)) {
    _errorMessage = tr("Cannot link group lists: %1").arg(_errorMessage);
    return false;
  }

  return true;
}
