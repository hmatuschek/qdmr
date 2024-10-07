#include "radioddity_codeplug.hh"
#include "utils.hh"
#include "logger.hh"
#include "scanlist.hh"
#include "radioid.hh"
#include "contact.hh"
#include "rxgrouplist.hh"
#include "zone.hh"
#include "config.hh"
#include "commercial_extension.hh"
#include "intermediaterepresentation.hh"


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
  setRXTone(SelectiveCall());
  setTXTone(SelectiveCall());
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

SelectiveCall
RadioddityCodeplug::ChannelElement::rxTone() const {
  return decode_ctcss_tone_table(getUInt16_le(0x0020));
}
void
RadioddityCodeplug::ChannelElement::setRXTone(const SelectiveCall &code) {
  setUInt16_le(0x0020, encode_ctcss_tone_table(code));
}
SelectiveCall
RadioddityCodeplug::ChannelElement::txTone() const {
  return decode_ctcss_tone_table(getUInt16_le(0x0022));
}
void
RadioddityCodeplug::ChannelElement::setTXTone(const SelectiveCall &code) {
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

DMRChannel::TimeSlot
RadioddityCodeplug::ChannelElement::timeSlot() const {
  return (getBit(0x0031, 6) ? DMRChannel::TimeSlot::TS2 : DMRChannel::TimeSlot::TS1);
}
void
RadioddityCodeplug::ChannelElement::setTimeSlot(DMRChannel::TimeSlot ts) {
  setBit(0x0031, 6, DMRChannel::TimeSlot::TS2 == ts);
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

FMChannel::Bandwidth
RadioddityCodeplug::ChannelElement::bandwidth() const {
  return (getBit(0x0033, 1) ? FMChannel::Bandwidth::Wide : FMChannel::Bandwidth::Narrow);
}
void
RadioddityCodeplug::ChannelElement::setBandwidth(FMChannel::Bandwidth bw) {
  setBit(0x0033, 1, FMChannel::Bandwidth::Wide == bw);
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
    FMChannel *ach = new FMChannel(); ch = ach;
    switch (admitCriterion()) {
    case ADMIT_ALWAYS: ach->setAdmit(FMChannel::Admit::Always); break;
    case ADMIT_CH_FREE: ach->setAdmit(FMChannel::Admit::Free); break;
    default: ach->setAdmit(FMChannel::Admit::Always); break;
    }
    ach->setBandwidth(bandwidth());
    ach->setRXTone(rxTone());
    ach->setTXTone(txTone());
    ach->setSquelchDefault(); // There is no per-channel squelch setting
  } else {
    DMRChannel *dch = new DMRChannel(); ch = dch;
    switch (admitCriterion()) {
    case ADMIT_ALWAYS: dch->setAdmit(DMRChannel::Admit::Always); break;
    case ADMIT_CH_FREE: dch->setAdmit(DMRChannel::Admit::Free); break;
    case ADMIT_COLOR: dch->setAdmit(DMRChannel::Admit::ColorCode); break;
    default: dch->setAdmit(DMRChannel::Admit::Always); break;
    }
    dch->setTimeSlot(timeSlot());
    dch->setColorCode(txColorCode());
  }

  // Apply common settings
  ch->setName(name());
  ch->setRXFrequency(Frequency::fromHz(rxFrequency()));
  ch->setTXFrequency(Frequency::fromHz(txFrequency()));
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
    c->setScanList(ctx.get<ScanList>(scanListIndex()));
  // Link digital channel
  if (c->is<DMRChannel>()) {
    DMRChannel *dc = c->as<DMRChannel>();
    if (hasGroupList() && ctx.has<RXGroupList>(groupListIndex()))
      dc->setGroupListObj(ctx.get<RXGroupList>(groupListIndex()));
    if (hasContact() && ctx.has<DMRContact>(contactIndex()))
      dc->setTXContactObj(ctx.get<DMRContact>(contactIndex()));
  }
  return true;

}

bool
RadioddityCodeplug::ChannelElement::fromChannelObj(const Channel *c, Context &ctx) {
  clear();

  setName(c->name());
  setRXFrequency(c->rxFrequency().inHz());
  setTXFrequency(c->txFrequency().inHz());
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

  if (c->scanList())
    setScanListIndex(ctx.index(c->scanList()));

  if (c->is<FMChannel>()) {
    const FMChannel *ac = c->as<const FMChannel>();
    setMode(MODE_ANALOG);
    switch (ac->admit()) {
    case FMChannel::Admit::Always: setAdmitCriterion(ADMIT_ALWAYS); break;
    case FMChannel::Admit::Free: setAdmitCriterion(ADMIT_CH_FREE); break;
    default: setAdmitCriterion(ADMIT_ALWAYS);
    }
    setBandwidth(ac->bandwidth());
    setRXTone(ac->rxTone());
    setTXTone(ac->txTone());
    // no per channel squelch setting
  } else if (c->is<DMRChannel>()) {
    const DMRChannel *dc = c->as<const DMRChannel>();
    setMode(MODE_DIGITAL);
    switch (dc->admit()) {
    case DMRChannel::Admit::Always: setAdmitCriterion(ADMIT_ALWAYS); break;
    case DMRChannel::Admit::Free: setAdmitCriterion(ADMIT_CH_FREE); break;
    case DMRChannel::Admit::ColorCode: setAdmitCriterion(ADMIT_COLOR); break;
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
  setType(DMRContact::GroupCall);
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

DMRContact::Type
RadioddityCodeplug::ContactElement::type() const {
  switch (getUInt8(0x0014)) {
  case 0: return DMRContact::GroupCall;
  case 1: return DMRContact::PrivateCall;
  case 2: return DMRContact::AllCall;
  default: break;
  }
  return DMRContact::PrivateCall;
}
void
RadioddityCodeplug::ContactElement::setType(DMRContact::Type type) {
  switch (type) {
  case DMRContact::GroupCall: setUInt8(0x0014, 0); break;
  case DMRContact::PrivateCall: setUInt8(0x0014, 1); break;
  case DMRContact::AllCall: setUInt8(0x0014, 2); break;
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

DMRContact *
RadioddityCodeplug::ContactElement::toContactObj(Context &ctx) const {
  Q_UNUSED(ctx)
  if (! isValid())
    return nullptr;
  return new DMRContact(type(), name(), number(), ring());
}

void
RadioddityCodeplug::ContactElement::fromContactObj(const DMRContact *cont, Context &ctx) {
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
RadioddityCodeplug::ZoneElement::linkZoneObj(Zone *zone, Context &ctx) const {
  if (! isValid()) {
    logWarn() << "Cannot link zone: Zone is invalid.";
    return false;
  }

  for (int i=0; (i<16) && hasMember(i); i++) {
    if (ctx.has<Channel>(member(i))) {
      zone->A()->add(ctx.get<Channel>(member(i)));
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
  : Element(ptr, size())
{
  // pass...
}

RadioddityCodeplug::GroupListElement::~GroupListElement() {
  // pass...
}

void
RadioddityCodeplug::GroupListElement::clear() {
  setName("");
  memset(_data+Offset::members(), 0, Offset::betweenMembers()*Limit::memberCount());
}

QString
RadioddityCodeplug::GroupListElement::name() const {
  return readASCII(Offset::name(), Limit::nameLength(), 0xff);
}
void
RadioddityCodeplug::GroupListElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::nameLength(), 0xff);
}

bool
RadioddityCodeplug::GroupListElement::hasMember(unsigned n) const {
  if (n >= Limit::memberCount())
    return false;
  return 0 != member(n);
}
unsigned
RadioddityCodeplug::GroupListElement::member(unsigned n) const {
  return getUInt16_le(Offset::members() + n*Offset::betweenMembers());
}
void
RadioddityCodeplug::GroupListElement::setMember(unsigned n, unsigned idx) {
  return setUInt16_le(Offset::members() + n*Offset::betweenMembers(), idx);
}
void
RadioddityCodeplug::GroupListElement::clearMember(unsigned n) {
  setMember(n,0);
}

RXGroupList *
RadioddityCodeplug::GroupListElement::toRXGroupListObj(Context &ctx, const ErrorStack& err) {
  Q_UNUSED(ctx); Q_UNUSED(err)
  return new RXGroupList(name());
}

bool
RadioddityCodeplug::GroupListElement::linkRXGroupListObj(int ncnt, RXGroupList *lst, Context &ctx, const ErrorStack& err) const {
  for (int i=0; (i<16) && (i<ncnt); i++) {
    if (ctx.has<DMRContact>(member(i))) {
      lst->addContact(ctx.get<DMRContact>(member(i)));
    } else {
      errMsg(err) << "Cannot link group list '" << lst->name()
                  << "': Member index " << member(i) << " does not refer to a digital contact.";
      return false;
    }
  }
  return true;
}

void
RadioddityCodeplug::GroupListElement::fromRXGroupListObj(const RXGroupList *lst, Context &ctx, const ErrorStack& err) {
  Q_UNUSED(err)
  setName(lst->name());
  int j = 0;
  // Iterate over all 15 entries in the codeplug
  for (int i=0; i<15; i++) {
    if (lst->count() > j) {
      // Skip non-group-call entries
      while((lst->count() > j) && (DMRContact::GroupCall != lst->contact(j)->type())) {
        logWarn() << "Contact '" << lst->contact(i)->name() << "' in group list '" << lst->name()
                  << "' is not a group call. Skip entry.";
        j++;
      }
      setMember(i, ctx.index(lst->contact(j))); j++;
    } else {
      // Clear entry.
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
  : Element(ptr, size())
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
  return _data + Offset::groupLists() + n*GroupListElement::size();
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
  setMonitorType(MonitorType::Silent);
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
  setARTSToneMode(ARTSTone::Once);

  enableDigitalTalkPermitTone(false);
  enableAnalogTalkPermitTone(false);
  enableSelftestTone(true);
  enableChannelFreeIndicationTone(false);
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
  setScanMode(ScanMode::Time);

  setRepeaterEndDelay(0);
  setRepeaterSTE(0);

  setUInt8(0x001f, 0);
  clearProgPassword();
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
RadioddityCodeplug::GeneralSettingsElement::channelFreeIndicationTone() const {
  return getBit(0x001b, 3);
}
void
RadioddityCodeplug::GeneralSettingsElement::enableChannelFreeIndicationTone(bool enable) {
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

bool
RadioddityCodeplug::GeneralSettingsElement::hasProgPassword() const {
  return (0xff != _data[0x0020]) && (0x00 != _data[0x0020]);
}
QString
RadioddityCodeplug::GeneralSettingsElement::progPassword() const {
  return readASCII(0x0020, 8, 0xff);
}
void
RadioddityCodeplug::GeneralSettingsElement::setProgPassword(const QString &pwd) {
  writeASCII(0x0020, pwd, 8, 0xff);
}
void
RadioddityCodeplug::GeneralSettingsElement::clearProgPassword() {
  memset(_data+0x0020, 0xff, 8);
}

bool
RadioddityCodeplug::GeneralSettingsElement::fromConfig(const Config *conf, Context &ctx) {
  if (! conf->settings()->defaultIdRef()->isNull()) {
    setName(conf->settings()->defaultIdRef()->as<DMRRadioID>()->name());
    setRadioID(conf->settings()->defaultIdRef()->as<DMRRadioID>()->number());
  } else if (conf->radioIDs()->count()) {
    setName(conf->radioIDs()->getId(0)->name());
    setRadioID(conf->radioIDs()->getId(0)->number());
  } else {
    logError() << "Cannot encode radioddity codeplug: No radio ID defined.";
    return false;
  }

  setVOXSensitivity(ctx.config()->settings()->vox());
  // There is no global squelch settings either

  // Handle Radioddity extension
  if (RadiodditySettingsExtension *ext = conf->settings()->radioddityExtension()) {
    setPreambleDuration(ext->preambleDuration().milliseconds());
    setMonitorType(ext->monitorType());
    setLowBatteryWarnInterval(ext->tone()->lowBatteryWarnInterval().seconds());
    setCallAlertDuration(ext->tone()->callAlertDuration().seconds());
    setLoneWorkerResponsePeriod(ext->loneWorkerResponseTime().minutes());
    setLoneWorkerReminderPeriod(ext->loneWorkerReminderPeriod().seconds());
    setGroupCallHangTime(ext->groupCallHangTime().milliseconds());
    setPrivateCallHangTime(ext->privateCallHangTime().milliseconds());
    enableDownChannelModeVFO(ext->downChannelModeVFO());
    enableUpChannelModeVFO(ext->upChannelModeVFO());
    enableResetTone(ext->tone()->resetTone());
    enableUnknownNumberTone(ext->tone()->unknownNumberTone());
    setARTSToneMode(ext->tone()->artsToneMode());
    enableDigitalTalkPermitTone(ext->tone()->digitalTalkPermitTone());
    enableAnalogTalkPermitTone(ext->tone()->analogTalkPermitTone());
    enableSelftestTone(ext->tone()->selftestTone());
    enableChannelFreeIndicationTone(ext->tone()->channelFreeIndicationTone());
    disableAllTones(ext->tone()->allTonesDisabled());
    enableBatsaveRX(ext->powerSaveMode());
    enableBatsavePreamble(ext->wakeupPreamble());
    disableAllLEDs(ext->allLEDsDisabled());
    inhibitQuickKeyOverride(ext->quickKeyOverrideInhibited());
    enableTXExitTone(ext->tone()->txExitTone());
    enableTXOnActiveChannel(ext->txOnActiveChannel());
    enableAnimation(RadioddityBootSettingsExtension::DisplayMode::Image == ext->boot()->display());
    setScanMode(ext->scanMode());
    setRepeaterEndDelay(ext->repeaterEndDelay().seconds());
    setRepeaterSTE(ext->repeaterSTE().seconds());
    if (ext->boot()->progPassword().isEmpty())
      clearProgPassword();
    else
      setProgPassword(ext->boot()->progPassword());
  }

  return true;
}

bool
RadioddityCodeplug::GeneralSettingsElement::updateConfig(Config *conf, Context &ctx) {
  Q_UNUSED(ctx)
  if (conf->settings()->defaultIdRef()->isNull()) {
    int idx = conf->radioIDs()->add(new DMRRadioID(name(), radioID()));
    conf->settings()->defaultIdRef()->set(conf->radioIDs()->getId(idx));
  } else {
    conf->settings()->defaultIdRef()->as<DMRRadioID>()->setName(name());
    conf->settings()->defaultIdRef()->as<DMRRadioID>()->setNumber(radioID());
  }
  conf->settings()->setVOX(voxSensitivity());
  // There is no global squelch settings either, so set it to 1
  conf->settings()->setSquelch(1);

  // Allocate Radioddity extension if needed
  RadiodditySettingsExtension *ext = conf->settings()->radioddityExtension();
  if (nullptr == ext) {
    ext = new RadiodditySettingsExtension();
    conf->settings()->setRadioddityExtension(ext);
  }
  // Update settings extension
  ext->setPreambleDuration(Interval::fromMilliseconds(preambleDuration()));
  ext->setMonitorType(monitorType());
  ext->tone()->setLowBatteryWarnInterval(Interval::fromSeconds(lowBatteryWarnInterval()));
  ext->tone()->setCallAlertDuration(Interval::fromSeconds(callAlertDuration()));
  ext->setLoneWorkerResponseTime(Interval::fromMinutes(loneWorkerResponsePeriod()));
  ext->setLoneWorkerReminderPeriod(Interval::fromSeconds(loneWorkerReminderPeriod()));
  ext->setGroupCallHangTime(Interval::fromMilliseconds(groupCallHangTime()));
  ext->setPrivateCallHangTime(Interval::fromMilliseconds(privateCallHangTime()));
  ext->enableDownChannelModeVFO(downChannelModeVFO());
  ext->enableUpChannelModeVFO(upChannelModeVFO());
  ext->tone()->enableResetTone(resetTone());
  ext->tone()->enableUnknownNumberTone(unknownNumberTone());
  ext->tone()->setARTSToneMode(artsToneMode());
  ext->tone()->enableDigitalTalkPermitTone(digitalTalkPermitTone());
  ext->tone()->enableAnalogTalkPermitTone(analogTalkPermitTone());
  ext->tone()->enableSelftestTone(selftestTone());
  ext->tone()->enableChannelFreeIndicationTone(channelFreeIndicationTone());
  ext->tone()->disableAllTones(allTonesDisabled());
  ext->enablePowerSaveMode(batsaveRX());
  ext->enableWakeupPreamble(batsavePreamble());
  ext->disableAllLEDs(allLEDsDisabled());
  ext->inhibitQuickKeyOverride(quickKeyOverrideInhibited());
  ext->tone()->enableTXExitTone(txExitTone());
  ext->enableTXOnActiveChannel(txOnActiveChannel());
  ext->boot()->setDisplay(animation() ? RadioddityBootSettingsExtension::DisplayMode::Image:
                                            RadioddityBootSettingsExtension::DisplayMode::Text);
  ext->setScanMode(scanMode());
  ext->setRepeaterEndDelay(Interval::fromSeconds(repeaterEndDelay()));
  ext->setRepeaterSTE(Interval::fromSeconds(repeaterSTE()));
  if (hasProgPassword())
    ext->boot()->setProgPassword(progPassword());
  else
    ext->boot()->setProgPassword("");

  return true;
}


/* ********************************************************************************************* *
 * Implementation of RadioddityCodeplug::ButtonSettingsElement
 * ********************************************************************************************* */
uint8_t
RadioddityCodeplug::ButtonSettingsElement::KeyFunction::encode(RadioddityButtonSettingsExtension::Function func) {
  switch (func) {
  case RadioddityButtonSettingsExtension::Function::None: return None;
  case RadioddityButtonSettingsExtension::Function::ToggleAllAlertTones: return ToggleAllAlertTones;
  case RadioddityButtonSettingsExtension::Function::EmergencyOn: return EmergencyOn;
  case RadioddityButtonSettingsExtension::Function::EmergencyOff: return EmergencyOff;
  case RadioddityButtonSettingsExtension::Function::ToggleMonitor: return ToggleMonitor;
  case RadioddityButtonSettingsExtension::Function::RadioDisable: return NuiaceDelete;
  case RadioddityButtonSettingsExtension::Function::OneTouch1: return OneTouch1;
  case RadioddityButtonSettingsExtension::Function::OneTouch2: return OneTouch2;
  case RadioddityButtonSettingsExtension::Function::OneTouch3: return OneTouch3;
  case RadioddityButtonSettingsExtension::Function::OneTouch4: return OneTouch4;
  case RadioddityButtonSettingsExtension::Function::OneTouch5: return OneTouch5;
  case RadioddityButtonSettingsExtension::Function::OneTouch6: return OneTouch6;
  case RadioddityButtonSettingsExtension::Function::ToggleTalkaround: return ToggleRepeatTalkaround;
  case RadioddityButtonSettingsExtension::Function::ToggleScan: return ToggleScan;
  case RadioddityButtonSettingsExtension::Function::ToggleEncryption: return TogglePrivacy;
  case RadioddityButtonSettingsExtension::Function::ToggleVox: return ToggleVox;
  case RadioddityButtonSettingsExtension::Function::ZoneSelect: return ZoneSelect;
  case RadioddityButtonSettingsExtension::Function::BatteryIndicator: return BatteryIndicator;
  case RadioddityButtonSettingsExtension::Function::ToggleLoneWorker: return ToggleLoneWorker;
  case RadioddityButtonSettingsExtension::Function::PhoneExit: return PhoneExit;
  case RadioddityButtonSettingsExtension::Function::ToggleFlashLight: return ToggleFlashLight;
  case RadioddityButtonSettingsExtension::Function::ToggleFMRadio: return ToggleFMRadio;
  default: break;
  }
  return Action::None;
}

RadioddityButtonSettingsExtension::Function
RadioddityCodeplug::ButtonSettingsElement::KeyFunction::decode(uint8_t action) {
  switch ((Action) action) {
  case None: return RadioddityButtonSettingsExtension::Function::None;
  case ToggleAllAlertTones: return RadioddityButtonSettingsExtension::Function::ToggleAllAlertTones;
  case EmergencyOn: return RadioddityButtonSettingsExtension::Function::EmergencyOn;
  case EmergencyOff: return RadioddityButtonSettingsExtension::Function::EmergencyOff;
  case ToggleMonitor: return RadioddityButtonSettingsExtension::Function::ToggleMonitor;
  case NuiaceDelete: return RadioddityButtonSettingsExtension::Function::RadioDisable;
  case OneTouch1: return RadioddityButtonSettingsExtension::Function::OneTouch1;
  case OneTouch2: return RadioddityButtonSettingsExtension::Function::OneTouch2;
  case OneTouch3: return RadioddityButtonSettingsExtension::Function::OneTouch3;
  case OneTouch4: return RadioddityButtonSettingsExtension::Function::OneTouch4;
  case OneTouch5: return RadioddityButtonSettingsExtension::Function::OneTouch5;
  case OneTouch6: return RadioddityButtonSettingsExtension::Function::OneTouch6;
  case ToggleRepeatTalkaround: return RadioddityButtonSettingsExtension::Function::ToggleTalkaround;
  case ToggleScan: return RadioddityButtonSettingsExtension::Function::ToggleScan;
  case TogglePrivacy: return RadioddityButtonSettingsExtension::Function::ToggleEncryption;
  case ToggleVox: return RadioddityButtonSettingsExtension::Function::ToggleVox;
  case ZoneSelect: return RadioddityButtonSettingsExtension::Function::ZoneSelect;
  case BatteryIndicator: return RadioddityButtonSettingsExtension::Function::BatteryIndicator;
  case ToggleLoneWorker: return RadioddityButtonSettingsExtension::Function::ToggleLoneWorker;
  case PhoneExit: return RadioddityButtonSettingsExtension::Function::PhoneExit;
  case ToggleFlashLight: return RadioddityButtonSettingsExtension::Function::ToggleFlashLight;
  case ToggleFMRadio: return RadioddityButtonSettingsExtension::Function::ToggleFMRadio;
  }

  return RadioddityButtonSettingsExtension::Function::None;
}


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
  setLongPressDuration(Interval::fromMilliseconds(1500));
  setSK1ShortPress(RadioddityButtonSettingsExtension::Function::ZoneSelect);
  setSK1LongPress(RadioddityButtonSettingsExtension::Function::ToggleFMRadio);
  setSK2ShortPress(RadioddityButtonSettingsExtension::Function::ToggleMonitor);
  setSK2LongPress(RadioddityButtonSettingsExtension::Function::ToggleFlashLight);
  setTKShortPress(RadioddityButtonSettingsExtension::Function::BatteryIndicator);
  setTKLongPress(RadioddityButtonSettingsExtension::Function::ToggleVox);
  memset(_data+0x0008, 0xff, 6*4);
}

Interval
RadioddityCodeplug::ButtonSettingsElement::longPressDuration() const {
  return Interval::fromMilliseconds(getUInt8(Offset::longPressDuration())*250);
}
void
RadioddityCodeplug::ButtonSettingsElement::setLongPressDuration(Interval ms) {
  setUInt8(Offset::longPressDuration(), ms.milliseconds()/250);
}

RadioddityButtonSettingsExtension::Function
RadioddityCodeplug::ButtonSettingsElement::sk1ShortPress() const {
  return KeyFunction::decode(getUInt8(Offset::sk1ShortPress()));
}
void
RadioddityCodeplug::ButtonSettingsElement::setSK1ShortPress(RadioddityButtonSettingsExtension::Function action) {
  setUInt8(Offset::sk1ShortPress(), KeyFunction::encode(action));
}
RadioddityButtonSettingsExtension::Function
RadioddityCodeplug::ButtonSettingsElement::sk1LongPress() const {
  return KeyFunction::decode(getUInt8(0x0003));
}
void
RadioddityCodeplug::ButtonSettingsElement::setSK1LongPress(RadioddityButtonSettingsExtension::Function action) {
  setUInt8(Offset::sk1LongPress(), KeyFunction::encode(action));
}

RadioddityButtonSettingsExtension::Function
RadioddityCodeplug::ButtonSettingsElement::sk2ShortPress() const {
  return KeyFunction::decode(getUInt8(Offset::sk2ShortPress()));
}
void
RadioddityCodeplug::ButtonSettingsElement::setSK2ShortPress(RadioddityButtonSettingsExtension::Function action) {
  setUInt8(Offset::sk2ShortPress(), KeyFunction::encode(action));
}
RadioddityButtonSettingsExtension::Function
RadioddityCodeplug::ButtonSettingsElement::sk2LongPress() const {
  return KeyFunction::decode(getUInt8(Offset::sk2LongPress()));
}
void
RadioddityCodeplug::ButtonSettingsElement::setSK2LongPress(RadioddityButtonSettingsExtension::Function action) {
  setUInt8(Offset::sk2LongPress(), KeyFunction::encode(action));
}

RadioddityButtonSettingsExtension::Function
RadioddityCodeplug::ButtonSettingsElement::tkShortPress() const {
  return KeyFunction::decode(getUInt8(Offset::tkShortPress()));
}
void
RadioddityCodeplug::ButtonSettingsElement::setTKShortPress(RadioddityButtonSettingsExtension::Function action) {
  setUInt8(Offset::tkShortPress(), KeyFunction::encode(action));
}
RadioddityButtonSettingsExtension::Function
RadioddityCodeplug::ButtonSettingsElement::tkLongPress() const {
  return KeyFunction::decode(getUInt8(Offset::tkLongPress()));
}
void
RadioddityCodeplug::ButtonSettingsElement::setTKLongPress(RadioddityButtonSettingsExtension::Function action) {
  setUInt8(Offset::tkLongPress(), KeyFunction::encode(action));
}

RadioddityCodeplug::ButtonSettingsElement::OneTouchAction
RadioddityCodeplug::ButtonSettingsElement::oneTouchAction(unsigned n) const {
  return OneTouchAction(getUInt8(Offset::oneTouchActions() + n*Offset::betweenOneTouchActions() + 0));
}
unsigned
RadioddityCodeplug::ButtonSettingsElement::oneTouchContact(unsigned n) const {
  return getUInt16_be(Offset::oneTouchActions() + n*Offset::betweenOneTouchActions() + 1);
}
unsigned
RadioddityCodeplug::ButtonSettingsElement::oneTouchMessage(unsigned n) const {
  return getUInt16_be(Offset::oneTouchActions() + n*Offset::betweenOneTouchActions() + 3);
}
void
RadioddityCodeplug::ButtonSettingsElement::disableOneTouch(unsigned n) {
  setUInt8(Offset::oneTouchActions() + n*Offset::betweenOneTouchActions() + 0, (unsigned)OneTouchAction::None);
}
void
RadioddityCodeplug::ButtonSettingsElement::setOneTouchDigitalCall(unsigned n, unsigned index) {
  setUInt8(Offset::oneTouchActions() + n*Offset::betweenOneTouchActions() + 0, (unsigned)OneTouchAction::DigitalCall);
  setUInt16_be(Offset::oneTouchActions() + n*Offset::betweenOneTouchActions() + 1, index);
  setUInt16_be(Offset::oneTouchActions() + n*Offset::betweenOneTouchActions() + 3, 0);
}
void
RadioddityCodeplug::ButtonSettingsElement::setOneTouchDigitalMessage(unsigned n, unsigned index) {
  setUInt8(Offset::oneTouchActions() + n*Offset::betweenOneTouchActions() + 0, (unsigned)OneTouchAction::DigitalMessage);
  setUInt16_be(Offset::oneTouchActions() + n*Offset::betweenOneTouchActions() + 1, 0);
  setUInt16_be(Offset::oneTouchActions() + n*Offset::betweenOneTouchActions() + 3, index);
}
void
RadioddityCodeplug::ButtonSettingsElement::setOneTouchAnalogCall(unsigned n) {
  setUInt8(Offset::oneTouchActions() + n*Offset::betweenOneTouchActions() + 0, (unsigned)OneTouchAction::AnalogCall);
  setUInt16_be(Offset::oneTouchActions() + n*Offset::betweenOneTouchActions() + 1, 0);
  setUInt16_be(Offset::oneTouchActions() + n*Offset::betweenOneTouchActions() + 3, 0);
}

bool
RadioddityCodeplug::ButtonSettingsElement::encode(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  if (! ctx.config()->settings()->radioddityExtension())
    return true;
  RadiodditySettingsExtension *ext = ctx.config()->settings()->radioddityExtension();

  setLongPressDuration(ext->buttons()->longPressDuration());
  setSK1ShortPress(ext->buttons()->funcKey1Short());
  setSK1LongPress(ext->buttons()->funcKey1Long());
  setSK2ShortPress(ext->buttons()->funcKey2Short());
  setSK2LongPress(ext->buttons()->funcKey2Long());
  setTKShortPress(ext->buttons()->funcKey3Short());
  setTKLongPress(ext->buttons()->funcKey3Long());

  return true;
}

bool
RadioddityCodeplug::ButtonSettingsElement::decode(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  if (! ctx.config()->settings()->radioddityExtension())
    ctx.config()->settings()->setRadioddityExtension(new RadiodditySettingsExtension());
  RadiodditySettingsExtension *ext = ctx.config()->settings()->radioddityExtension();

  ext->buttons()->setLongPressDuration(longPressDuration());
  ext->buttons()->setFuncKey1Short(sk1ShortPress());
  ext->buttons()->setFuncKey1Long(sk1LongPress());
  ext->buttons()->setFuncKey2Short(sk2ShortPress());
  ext->buttons()->setFuncKey2Long(sk2LongPress());
  ext->buttons()->setFuncKey3Short(tkShortPress());
  ext->buttons()->setFuncKey3Long(tkLongPress());

  return true;
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
  setUInt8(Offset::messageConut(), 0); // set count to 0
  memset(_data+0x0001, 0x00, 7); // Fill unused
  memset(_data+Offset::messageLengths(), 0x00, Limit::messages()); // Set message lengths to 0
  memset(_data+0x0028, 0x00, 32); // Fill unused
  memset(_data+Offset::messages(), 0xff, Limit::messages()*Limit::messageLength()); // Clear all messages
}

unsigned
RadioddityCodeplug::MessageBankElement::numMessages() const {
  return getUInt8(Offset::messageConut());
}
QString
RadioddityCodeplug::MessageBankElement::message(unsigned n) const {
  if (n >= numMessages())
    return QString();
  return readASCII(Offset::messages()+n*Offset::betweenMessages(), Limit::messageLength(), 0xff);
}
void
RadioddityCodeplug::MessageBankElement::appendMessage(const QString msg) {
  unsigned idx = numMessages();
  if (idx >= Limit::messages())
    return;
  unsigned int len = std::min((unsigned int)msg.size(), Limit::messageLength());
  // increment counter
  setUInt8(Offset::messageConut(), idx+1);
  // store length
  setUInt8(Offset::messageLengths()+idx, len);
  // store string
  writeASCII(Offset::messages()+Offset::betweenMessages()*idx, msg, Limit::messageLength(), 0xff);
}

bool
RadioddityCodeplug::MessageBankElement::encode(Context &ctx, const Flags &flags, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)
  clear();
  unsigned int count = std::min(
        Limit::messages(), (unsigned int)ctx.config()->smsExtension()->smsTemplates()->count());
  for (unsigned int i=0; i<count; i++)
    appendMessage(ctx.config()->smsExtension()->smsTemplates()->message(i)->message());
  return true;
}

bool
RadioddityCodeplug::MessageBankElement::decode(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err);

  for (unsigned int i=0; i<numMessages(); i++) {
    SMSTemplate *sms = new SMSTemplate();
    sms->setName(QString("Message %1").arg(i+1));
    sms->setMessage(message(i));
    ctx.config()->smsExtension()->smsTemplates()->add(sms);
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of RadioddityCodeplug::EncryptionElement
 * ********************************************************************************************* */
RadioddityCodeplug::EncryptionElement::EncryptionElement(uint8_t *ptr, size_t size)
  : Codeplug::Element(ptr, size)
{
  // pass...
}

RadioddityCodeplug::EncryptionElement::EncryptionElement(uint8_t *ptr)
  : Element(ptr, 0x88)
{
  // pass...
}

RadioddityCodeplug::EncryptionElement::~EncryptionElement() {
  // pass...
}

void
RadioddityCodeplug::EncryptionElement::clear() {
  setPrivacyType(PrivacyType::None);
  for (int i=0; i<16; i++) {
    clearBasicKey(i);
  }
}

RadioddityCodeplug::EncryptionElement::PrivacyType
RadioddityCodeplug::EncryptionElement::privacyType() const {
  return PrivacyType(getUInt8(0x0000));
}
void
RadioddityCodeplug::EncryptionElement::setPrivacyType(PrivacyType type) {
  setUInt8(0x0000, (uint8_t)type);
}

bool
RadioddityCodeplug::EncryptionElement::isBasicKeySet(unsigned n) const {
  if (n>=16)
    return false;
  unsigned byte=n/8, bit =n%8;
  return getBit(0x0002+byte, bit);
}
QByteArray
RadioddityCodeplug::EncryptionElement::basicKey(unsigned n) const {
  if (n >= 16)
    return QByteArray();
  return QByteArray((const char *)_data+0x0008+0x08*n, 4);
}
void
RadioddityCodeplug::EncryptionElement::setBasicKey(unsigned n, const QByteArray &key) {
  if ((n>=16) || (4 != key.size()))
    return;
  unsigned byte=n/8, bit =n%8;
  memcpy(_data+0x0008 + 0x08*n, key.data(), 4);
  memcpy(_data+0x0008 + 0x08*n + 0x04, key.data(), 4);
  setBit(0x0002+byte, bit);
  setPrivacyType(PrivacyType::Basic);
}
void
RadioddityCodeplug::EncryptionElement::clearBasicKey(unsigned n) {
  if (n>=16)
    return;
  unsigned byte=n/8, bit =n%8;
  memset(_data+0x0008 + 0x08*n, 0xff, 4);
  memset(_data+0x0008 + 0x08*n + 0x04, 0xff, 4);
  clearBit(0x0002+byte, bit);
}

bool
RadioddityCodeplug::EncryptionElement::fromCommercialExt(CommercialExtension *ext, Context &ctx) {
  clear();

  if (ext->encryptionKeys()->count() > (int)Limit::basicEncryptionKeys()) {
    logError() << "Cannot encode encryption extension. Can only encode "
               << Limit::basicEncryptionKeys() << " keys.";
    return false;
  }

  for (int i=0; i<ext->encryptionKeys()->count(); i++) {
    if (! ext->encryptionKeys()->get(i)->is<BasicEncryptionKey>()) {
      logError() << "Can only encode basic encryption keys.";
      return false;
    }
    BasicEncryptionKey *key = ext->encryptionKeys()->get(i)->as<BasicEncryptionKey>();
    if (key->key().size() != 4) {
      logError() << "Can only encode 32bit basic encryption keys.";
      return false;
    }
    setBasicKey(i, key->key());
    ctx.add(key, i+1);
  }

  return true;
}

bool RadioddityCodeplug::EncryptionElement::updateCommercialExt(Context &ctx) {
  if (PrivacyType::None == privacyType())
    return false;

  CommercialExtension *ext = ctx.config()->commercialExtension();
  for (unsigned int i=0; i<Limit::basicEncryptionKeys(); i++) {
    if (! isBasicKeySet(i))
      continue;
    // Assemble key
    BasicEncryptionKey *key = new BasicEncryptionKey();
    key->setName(QString("Basic Key %1").arg(i+1));
    key->fromHex(basicKey(i).toHex());
    // add key to extension
    ext->encryptionKeys()->add(key);
    // register key index
    ctx.add(key, i+1);
  }

  return ext;
}

bool
RadioddityCodeplug::EncryptionElement::linkCommercialExt(CommercialExtension *ext, Context &ctx) {
  Q_UNUSED(ext); Q_UNUSED(ctx);
  // Keys do not need any linking step
  return true;
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
  // Clear messages
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
  // clear encryption keys
  clearEncryption();
}

bool
RadioddityCodeplug::index(Config *config, Context &ctx, const ErrorStack &err) const {
  Q_UNUSED(err)
  // All indices as 1-based. That is, the first channel gets index 1.

  // Map radio IDs
  for (int i=0; i<config->radioIDs()->count(); i++)
    ctx.add(config->radioIDs()->getId(i), i+1);

  // Map digital and DTMF contacts
  for (int i=0, d=0, a=0; i<config->contacts()->count(); i++) {
    if (config->contacts()->contact(i)->is<DMRContact>()) {
      ctx.add(config->contacts()->contact(i)->as<DMRContact>(), d+1); d++;
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
  for (int i=0; i<config->roamingZones()->count(); i++)
    ctx.add(config->roamingZones()->zone(i), i+1);

  return true;
}

Config *
RadioddityCodeplug::preprocess(Config *config, const ErrorStack &err) const {
  Config *intermediate = Codeplug::preprocess(config, err);
  if (nullptr == intermediate) {
    errMsg(err) << "Cannot pre-process Radioddity codeplug.";
    return nullptr;
  }

  ZoneSplitVisitor splitter;
  if (! splitter.process(intermediate, err)) {
    errMsg(err) << "Cannot split zone for Radioddity codeplug.";
    delete intermediate;
    return nullptr;
  }

  return intermediate;
}


bool
RadioddityCodeplug::encode(Config *config, const Flags &flags, const ErrorStack &err) {
  // Check if default DMR id is set.
  if (config->settings()->defaultIdRef()->isNull()) {
    errMsg(err) << "No default radio ID specified.";
    return false;
  }

  // Create index<->object table.
  Context ctx(config);
  if (! index(config, ctx, err)) {
    errMsg(err) << "Cannot index configuration objects.";
    return false;
  }

  return this->encodeElements(flags, ctx);
}

bool
RadioddityCodeplug::encodeElements(const Flags &flags, Context &ctx, const ErrorStack &err) {
  // General config
  if (! this->encodeGeneralSettings(ctx.config(), flags, ctx, err)) {
    errMsg(err) << "Cannot encode general settings.";
    return false;
  }

  if (! this->encodeButtonSettings(ctx, flags, err)) {
    errMsg(err) << "Cannot encode button settings.";
    return false;
  }

  if (! this->encodeMessages(ctx, flags, err)) {
    errMsg(err) << "Cannot encode preset messages.";
    return false;
  }

  // Define Contacts
  if (! this->encodeContacts(ctx.config(), flags, ctx, err)) {
    errMsg(err) << "Cannot encode contacts.";
    return false;
  }

  if (! this->encodeDTMFContacts(ctx.config(), flags, ctx, err)) {
    errMsg(err) << "Cannot encode DTMF contacts.";
    return false;
  }

  if (! this->encodeChannels(ctx.config(), flags, ctx, err)) {
    errMsg(err) << "Cannot encode channels";
    return false;
  }

  if (! this->encodeBootText(ctx.config(), flags, ctx, err)) {
    errMsg(err) << "Cannot encode boot text.";
    return false;
  }

  if (! this->encodeZones(ctx.config(), flags, ctx, err)) {
    errMsg(err) << "Cannot encode zones.";
    return false;
  }

  if (! this->encodeScanLists(ctx.config(), flags, ctx, err)) {
    errMsg(err) << "Cannot encode scan lists.";
    return false;
  }

  if (! this->encodeGroupLists(ctx.config(), flags, ctx, err)) {
    errMsg(err) << "Cannot encode group lists.";
    return false;
  }

  if (! this->encodeEncryption(ctx.config(), flags, ctx, err)) {
    errMsg(err) << "Cannot encode encryption keys.";
    return true;
  }

  return true;
}


bool
RadioddityCodeplug::decode(Config *config, const ErrorStack &err) {
  // Clear config object
  config->clear();

  // Create index<->object table.
  Context ctx(config);

  return this->decodeElements(ctx, err);
}

bool
RadioddityCodeplug::postprocess(Config *config, const ErrorStack &err) const {
  if (! Codeplug::postprocess(config, err)) {
    errMsg(err) << "Cannot post-process Radioddy codeplug.";
    return false;
  }

  ZoneMergeVisitor merger;
  if (! merger.process(config, err)) {
    errMsg(err) << "Cannot merg zones in decoded Radioddity codeplug.";
    return false;
  }

  return true;
}

bool
RadioddityCodeplug::decodeElements(Context &ctx, const ErrorStack &err) {
  if (! this->decodeGeneralSettings(ctx.config(), ctx, err)) {
    errMsg(err) << "Cannot decode general settings.";
    return false;
  }

  if (! this->decodeButtonSettings(ctx, err)) {
    errMsg(err) << "Cannot decode button settings.";
    return false;
  }

  if (! this->decodeMessages(ctx, err)) {
    errMsg(err) << "Cannot decode preset messages.";
    return false;
  }

  if (! this->createContacts(ctx.config(), ctx, err)) {
    errMsg(err) << "Cannot create contacts.";
    return false;
  }

  if (! this->createDTMFContacts(ctx.config(), ctx, err)) {
    errMsg(err) << "Cannot create DTMF contacts";
    return false;
  }

  if (! this->createChannels(ctx.config(), ctx, err)) {
    errMsg(err) << "Cannot create channels.";
    return false;
  }

  if (! this->decodeBootText(ctx.config(), ctx, err)) {
    errMsg(err) << "Cannot decode boot text.";
    return false;
  }

  if (! this->createEncryption(ctx.config(), ctx, err)) {
    errMsg(err) << "Cannot decode encryption keys.";
    return false;
  }

  if (! this->createZones(ctx.config(), ctx, err)) {
    errMsg(err) << "Cannot create zones.";
    return false;
  }

  if (! this->createScanLists(ctx.config(), ctx, err)) {
    errMsg(err) << "Cannot create scan lists.";
    return false;
  }

  if (! this->createGroupLists(ctx.config(), ctx, err)) {
    errMsg(err) << "Cannot create group lists.";
    return false;
  }

  if (! this->linkChannels(ctx.config(), ctx, err)) {
    errMsg(err) << "Cannot link channels.";
    return false;
  }

  if (! this->linkZones(ctx.config(), ctx, err)) {
    errMsg(err) << "Cannot link zones.";
    return false;
  }

  if (! this->linkScanLists(ctx.config(), ctx, err)) {
    errMsg(err) << "Cannot link scan lists.";
    return false;
  }

  if (! this->linkGroupLists(ctx.config(), ctx, err)) {
    errMsg(err) << "Cannot link group lists.";
    return false;
  }

  if (! this->linkEncryption(ctx.config(), ctx, err)) {
    errMsg(err) << "Cannot link encryption keys.";
    return false;
  }

  return true;
}
