#include "opengd77base_codeplug.hh"
#include "config.hh"


/* ********************************************************************************************* *
 * Implementation of OpenGD77BaseCodeplug::ChannelElement
 * ********************************************************************************************* */
OpenGD77BaseCodeplug::ChannelElement::ChannelElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

OpenGD77BaseCodeplug::ChannelElement::ChannelElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}

OpenGD77BaseCodeplug::ChannelElement::~ChannelElement() {
  // pass...
}

void
OpenGD77BaseCodeplug::ChannelElement::clear() {
  setName("");
  setRXFrequency(0);
  setTXFrequency(0);
  setMode(MODE_ANALOG);
  setRXTone(SelectiveCall());
  setTXTone(SelectiveCall());
  setColorCode(0);
  setGroupListIndex(0);
}


QString
OpenGD77BaseCodeplug::ChannelElement::name() const {
  return readASCII(Offset::name(), Limit::nameLength(), 0xff);
}

void
OpenGD77BaseCodeplug::ChannelElement::setName(const QString &n) {
  writeASCII(Offset::name(), n, Limit::nameLength(), 0xff);
}


uint32_t
OpenGD77BaseCodeplug::ChannelElement::rxFrequency() const {
  return getBCD8_le(Offset::rxFrequency())*10;
}

void
OpenGD77BaseCodeplug::ChannelElement::setRXFrequency(uint32_t freq) {
  setBCD8_le(Offset::rxFrequency(), freq/10);
}


uint32_t
OpenGD77BaseCodeplug::ChannelElement::txFrequency() const {
  return getBCD8_le(Offset::txFrequency())*10;
}

void
OpenGD77BaseCodeplug::ChannelElement::setTXFrequency(uint32_t freq) {
  setBCD8_le(Offset::txFrequency(), freq/10);
}


OpenGD77BaseCodeplug::ChannelElement::Mode
OpenGD77BaseCodeplug::ChannelElement::mode() const {
  return (Mode)getUInt8(Offset::mode());
}
void
OpenGD77BaseCodeplug::ChannelElement::setMode(Mode mode) {
  setUInt8(Offset::mode(), (unsigned)mode);
}


bool
OpenGD77BaseCodeplug::ChannelElement::globalPower() const {
  return 0 == getUInt8(Offset::power());
}

Channel::Power
OpenGD77BaseCodeplug::ChannelElement::power() const {
  switch (getUInt8(Offset::power())) {
  case 0:
  case 1: return Channel::Power::Min;
  case 2:
  case 3: return Channel::Power::Low;
  case 4:
  case 5:
  case 6: return Channel::Power::Mid;
  case 7:
  case 8:
  case 9: return Channel::Power::High;
  case 10: return Channel::Power::Max;
  default:
  break;
  }
  return Channel::Power::Min;
}

void
OpenGD77BaseCodeplug::ChannelElement::setPower(Channel::Power pwr) {
  switch (pwr) {
  case Channel::Power::Min:  setUInt8(Offset::power(),  1); break;
  case Channel::Power::Low:  setUInt8(Offset::power(),  3); break;
  case Channel::Power::Mid:  setUInt8(Offset::power(),  6); break;
  case Channel::Power::High: setUInt8(Offset::power(),  9); break;
  case Channel::Power::Max:  setUInt8(Offset::power(), 10); break;
  }
}

void
OpenGD77BaseCodeplug::ChannelElement::clearPower() {
  setUInt8(Offset::power(), 0);
}


bool
OpenGD77BaseCodeplug::ChannelElement::hasFixedPosition() const {
  return getBit(Offset::useFixedLocation());
}

QGeoCoordinate
OpenGD77BaseCodeplug::ChannelElement::fixedPosition() const {
  uint32_t latCode = (((uint32_t)getUInt8(Offset::latitude2())) << 24) +
      (((uint32_t)getUInt8(Offset::latitude1())) << 16) +
      ((uint32_t)getUInt8(Offset::latitude0()));
  uint32_t lonCode = (((uint32_t)getUInt8(Offset::longitude2())) << 24) +
      (((uint32_t)getUInt8(Offset::longitude1())) << 16) +
      ((uint32_t)getUInt8(Offset::longitude0()));

  double lat = (((latCode >> 23) & 1) ? -1 : 1) * (
        ((latCode >> 15) & 0xff) + double(latCode & 0x7ff)/10000
        );
  double lon = (((lonCode >> 23) & 1) ? -1 : 1) * (
        ((lonCode >> 15) & 0xff) + double(lonCode & 0x7ff)/10000
        );

  return QGeoCoordinate(lat, lon);
}

void
OpenGD77BaseCodeplug::ChannelElement::setFixedPosition(const QGeoCoordinate &coordinate) {
  if (!coordinate.isValid()) {
    clearFixedPosition();
    return;
  }

  uint32_t latSign = (coordinate.latitude()<0) ? 1 : 0;
  uint32_t decimals = std::abs(int(coordinate.latitude()*10000));
  uint32_t latDeg  = decimals/10000, latDec = decimals % 10000;
  uint32_t lonSign = (coordinate.longitude()<0) ? 1 : 0;
  decimals = std::abs(int(coordinate.longitude()*10000));
  uint32_t lonDeg  = decimals/10000, lonDec = decimals % 10000;

  uint32_t latCode = (latSign<<23) | (latDeg<<15) | latDec;
  uint32_t lonCode = (lonSign<<23) | (lonDeg<<15) | lonDec;

  setBit(Offset::useFixedLocation(), true);
  setUInt8(Offset::latitude0(), ((latCode >>  0) & 0xff));
  setUInt8(Offset::latitude1(), ((latCode >>  8) & 0xff));
  setUInt8(Offset::latitude2(), ((latCode >> 16) & 0xff));
  setUInt8(Offset::longitude0(), ((lonCode >>  0) & 0xff));
  setUInt8(Offset::longitude1(), ((lonCode >>  8) & 0xff));
  setUInt8(Offset::longitude2(), ((lonCode >> 16) & 0xff));
}

void
OpenGD77BaseCodeplug::ChannelElement::clearFixedPosition() {
  setBit(Offset::useFixedLocation(), false);
}


SelectiveCall
OpenGD77BaseCodeplug::ChannelElement::rxTone() const {
  uint16_t toneCode = getUInt16_le(Offset::rxTone());
  if (0xffff == toneCode)
    return SelectiveCall();

  bool dcs = ((toneCode >> 15) & 1),
      inverted = ((toneCode >> 14) & 1);
  toneCode &= 0x3fff;

  if (! dcs)
    return SelectiveCall(double(toneCode)/10);

  return SelectiveCall(toneCode, inverted);
}

void
OpenGD77BaseCodeplug::ChannelElement::setRXTone(const SelectiveCall &code) {
  if (code.isInvalid())
    setUInt16_le(Offset::rxTone(), 0xffff);

  uint16_t dcs = 0, inverted = 0, toneCode = 0;
  if (code.isDCS()) {
    dcs = 1;
    inverted = code.isInverted() ? 1 : 0;
    toneCode = code.octalCode();
  } else {
    dcs = inverted = 0;
    toneCode = code.mHz()/100;
  }

  setUInt16_le(Offset::rxTone(), (dcs<<15) | (inverted << 14) | (toneCode & 0x3fff));
}

SelectiveCall
OpenGD77BaseCodeplug::ChannelElement::txTone() const {
  uint16_t toneCode = getUInt16_le(Offset::txTone());
  if (0xffff == toneCode)
    return SelectiveCall();

  bool dcs = ((toneCode >> 15) & 1),
      inverted = ((toneCode >> 14) & 1);
  toneCode &= 0x3fff;

  if (! dcs)
    return SelectiveCall(double(toneCode)/10);

  return SelectiveCall(toneCode, inverted);
}

void
OpenGD77BaseCodeplug::ChannelElement::setTXTone(const SelectiveCall &code) {
  if (code.isInvalid())
    setUInt16_le(Offset::txTone(), 0xffff);

  uint16_t dcs = 0, inverted = 0, toneCode = 0;
  if (code.isDCS()) {
    dcs = 1;
    inverted = code.isInverted() ? 1 : 0;
    toneCode = code.octalCode();
  } else {
    dcs = inverted = 0;
    toneCode = code.mHz()/100;
  }

  setUInt16_le(Offset::txTone(), (dcs<<15) | (inverted << 14) | (toneCode & 0x3fff));
}


bool
OpenGD77BaseCodeplug::ChannelElement::isSimplex() const {
  return getBit(Offset::simplex());
}

void
OpenGD77BaseCodeplug::ChannelElement::enableSimplex(bool enable) {
  setBit(Offset::simplex(), enable);
}


bool
OpenGD77BaseCodeplug::ChannelElement::powerSave() const {
  return ! getBit(Offset::disablePowerSave());
}

void
OpenGD77BaseCodeplug::ChannelElement::enablePowerSave(bool enable) {
  setBit(Offset::disablePowerSave(), !enable);
}


bool
OpenGD77BaseCodeplug::ChannelElement::beep() const {
  return ! getBit(Offset::disableBeep());
}

void
OpenGD77BaseCodeplug::ChannelElement::enableBeep(bool enable) {
  setBit(Offset::disableBeep(), !enable);
}


bool
OpenGD77BaseCodeplug::ChannelElement::hasDMRId() const {
  return getBit(Offset::overrideDMRID());
}

unsigned int
OpenGD77BaseCodeplug::ChannelElement::dmrId() const {
  return getUInt24_be(Offset::dmrId());
}

void
OpenGD77BaseCodeplug::ChannelElement::setDMRId(unsigned int dmrId) {
  setBit(Offset::overrideDMRID());
  setUInt24_be(Offset::dmrId(), dmrId);
}

void
OpenGD77BaseCodeplug::ChannelElement::clearDMRId() {
  clearBit(Offset::overrideDMRID());
}


bool
OpenGD77BaseCodeplug::ChannelElement::hasGroupList() const {
  return 0 != getUInt8(Offset::groupList());
}

unsigned
OpenGD77BaseCodeplug::ChannelElement::groupListIndex() const {
  return getUInt8(Offset::groupList())-1;
}

void
OpenGD77BaseCodeplug::ChannelElement::setGroupListIndex(unsigned index) {
  setUInt8(Offset::groupList(), index+1);
}

void
OpenGD77BaseCodeplug::ChannelElement::clearGroupListIndex() {
  setUInt8(Offset::groupList(), 0);
}


unsigned
OpenGD77BaseCodeplug::ChannelElement::colorCode() const {
  return getUInt8(Offset::colorCode());
}
void
OpenGD77BaseCodeplug::ChannelElement::setColorCode(unsigned cc) {
  setUInt8(Offset::colorCode(), cc);
}


bool
OpenGD77BaseCodeplug::ChannelElement::hasAPRSIndex() const {
  return 0 != getUInt8(Offset::aprsIndex());
}

unsigned int
OpenGD77BaseCodeplug::ChannelElement::aprsIndex() const {
  return getUInt8(Offset::aprsIndex())-1;
}

void
OpenGD77BaseCodeplug::ChannelElement::setAPRSIndex(unsigned int index) {
  setUInt8(Offset::aprsIndex(), index+1);
}

void
OpenGD77BaseCodeplug::ChannelElement::clearAPRSIndex() {
  setUInt8(Offset::aprsIndex(), 0);
}


OpenGD77BaseCodeplug::ChannelElement::Alias
OpenGD77BaseCodeplug::ChannelElement::aliasTimeSlot1() const {
  return (Alias) getUInt2(Offset::aliasTimeSlot1());
}

void
OpenGD77BaseCodeplug::ChannelElement::setAliasTimeSlot1(Alias alias) {
  setUInt2(Offset::aliasTimeSlot1(), (unsigned int)alias);
}

OpenGD77BaseCodeplug::ChannelElement::Alias
OpenGD77BaseCodeplug::ChannelElement::aliasTimeSlot2() const {
  return (Alias) getUInt2(Offset::aliasTimeSlot2());
}

void
OpenGD77BaseCodeplug::ChannelElement::setAliasTimeSlot2(Alias alias) {
  setUInt2(Offset::aliasTimeSlot2(), (unsigned int)alias);
}


DMRChannel::TimeSlot
OpenGD77BaseCodeplug::ChannelElement::timeSlot() const {
  return (getBit(Offset::timeSlot()) ? DMRChannel::TimeSlot::TS2 : DMRChannel::TimeSlot::TS1);
}
void
OpenGD77BaseCodeplug::ChannelElement::setTimeSlot(DMRChannel::TimeSlot ts) {
  setBit(Offset::timeSlot(), DMRChannel::TimeSlot::TS2 == ts);
}


FMChannel::Bandwidth
OpenGD77BaseCodeplug::ChannelElement::bandwidth() const {
  return (getBit(Offset::bandwidth()) ? FMChannel::Bandwidth::Wide : FMChannel::Bandwidth::Narrow);
}
void
OpenGD77BaseCodeplug::ChannelElement::setBandwidth(FMChannel::Bandwidth bw) {
  setBit(Offset::bandwidth(), FMChannel::Bandwidth::Wide == bw);
}

bool
OpenGD77BaseCodeplug::ChannelElement::rxOnly() const {
  return getBit(Offset::rxOnly());
}
void
OpenGD77BaseCodeplug::ChannelElement::enableRXOnly(bool enable) {
  setBit(Offset::rxOnly(), enable);
}

bool
OpenGD77BaseCodeplug::ChannelElement::skipScan() const {
  return getBit(Offset::skipScan());
}
void
OpenGD77BaseCodeplug::ChannelElement::enableSkipScan(bool enable) {
  setBit(Offset::skipScan(), enable);
}

bool
OpenGD77BaseCodeplug::ChannelElement::skipZoneScan() const {
  return getBit(Offset::skipZoneScan());
}
void
OpenGD77BaseCodeplug::ChannelElement::enableSkipZoneScan(bool enable) {
  setBit(Offset::skipZoneScan(), enable);
}

bool
OpenGD77BaseCodeplug::ChannelElement::vox() const {
  return getBit(Offset::vox());
}
void
OpenGD77BaseCodeplug::ChannelElement::enableVOX(bool enable) {
  setBit(Offset::vox(), enable);
}


OpenGD77BaseCodeplug::ChannelElement::SquelchMode
OpenGD77BaseCodeplug::ChannelElement::squelchMode() const {
  switch (getUInt8(Offset::squelch())) {
  case 0: return SquelchMode::Global;
  case 1: return SquelchMode::Open;
  case 15: return SquelchMode::Closed;
  default: return SquelchMode::Normal;
  }
}

unsigned int
OpenGD77BaseCodeplug::ChannelElement::squelchLevel() const {
  int level = (10*(getUInt8(Offset::squelch())-1))/14;
  return std::min(level, 10);
}

void
OpenGD77BaseCodeplug::ChannelElement::setSquelch(SquelchMode mode, unsigned int level) {
  level = std::min(level, 10U);
  switch (mode) {
  case SquelchMode::Global: level = 0; break;
  case SquelchMode::Open:   level = 1; break;
  case SquelchMode::Closed: level = 15; break;
  case SquelchMode::Normal: level = 1 + (14*level)/10; break;
  }
  setUInt8(Offset::squelch(), level);
}


Channel *
OpenGD77BaseCodeplug::ChannelElement::toChannelObj(Codeplug::Context &ctx, const ErrorStack& err) const {
  Q_UNUSED(ctx); Q_UNUSED(err)
  Channel *ch = nullptr;
  if (MODE_ANALOG == mode()) {
    FMChannel *ach = new FMChannel(); ch = ach;
    ach->setBandwidth(bandwidth());
    ach->setRXTone(rxTone());
    ach->setTXTone(txTone());
    ach->setSquelchDefault(); // There is no per-channel squelch setting
  } else {
    DMRChannel *dch = new DMRChannel(); ch = dch;
    dch->setTimeSlot(timeSlot());
    dch->setColorCode(colorCode());
  }

  // Apply common settings
  ch->setName(name());
  ch->setRXFrequency(Frequency::fromHz(rxFrequency()));
  ch->setTXFrequency(Frequency::fromHz(txFrequency()));
  ch->setPower(power());
  ch->setRXOnly(rxOnly());
  if (vox())
    ch->setVOXDefault();
  else
    ch->disableVOX();

  // done.
  return ch;
}


bool
OpenGD77BaseCodeplug::ChannelElement::linkChannelObj(Channel *c, Context &ctx, const ErrorStack& err) const {
  Q_UNUSED(err)

  // Link common
  // Link digital channel
  if (c->is<DMRChannel>()) {
    DMRChannel *dc = c->as<DMRChannel>();
    if (hasGroupList() && ctx.has<RXGroupList>(groupListIndex()))
      dc->setGroupListObj(ctx.get<RXGroupList>(groupListIndex()));
  }

  return true;
}


bool
OpenGD77BaseCodeplug::ChannelElement::fromChannelObj(const Channel *c, Context &ctx, const ErrorStack& err) {
  clear();

  setName(c->name());
  setRXFrequency(c->rxFrequency().inHz());
  setTXFrequency(c->txFrequency().inHz());
  if (c->defaultPower())
    setPower(ctx.config()->settings()->power());
  else
    setPower(c->power());
  enableRXOnly(c->rxOnly());

  // Enable vox
  bool defaultVOXEnabled = (c->defaultVOX() && (!ctx.config()->settings()->voxDisabled()));
  bool channelVOXEnabled = (! (c->voxDisabled()||c->defaultVOX()));
  enableVOX(defaultVOXEnabled || channelVOXEnabled);


  if (c->is<FMChannel>()) {
    const FMChannel *ac = c->as<const FMChannel>();
    setMode(MODE_ANALOG);
    setBandwidth(ac->bandwidth());
    setRXTone(ac->rxTone());
    setTXTone(ac->txTone());
    // no per channel squelch setting
  } else if (c->is<DMRChannel>()) {
    const DMRChannel *dc = c->as<const DMRChannel>();
    setMode(MODE_DIGITAL);
    setTimeSlot(dc->timeSlot());
    setColorCode(dc->colorCode());
    if (dc->groupListObj())
      setGroupListIndex(ctx.index(dc->groupListObj()));
  } else {
    errMsg(err) << "Cannot encode channel of type '" << c->metaObject()->className()
                << "': Not supported by the radio.";
    return false;
  }

  return true;
}



/* ********************************************************************************************* *
 * Implementation of OpenGD77BaseCodeplug::ChannelBankElement
 * ********************************************************************************************* */
OpenGD77BaseCodeplug::ChannelBankElement::ChannelBankElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

OpenGD77BaseCodeplug::ChannelBankElement::ChannelBankElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}

OpenGD77BaseCodeplug::ChannelBankElement::~ChannelBankElement() {
  // pass...
}

void
OpenGD77BaseCodeplug::ChannelBankElement::clear() {
  memset(_data, 0, size());
}

bool
OpenGD77BaseCodeplug::ChannelBankElement::isEnabled(unsigned idx) const {
  unsigned byte = Offset::bitmask() + idx/8, bit = idx%8;
  return getBit(byte, bit);
}
void
OpenGD77BaseCodeplug::ChannelBankElement::enable(unsigned idx, bool enabled) {
  unsigned byte = Offset::bitmask() + idx/8, bit = idx%8;
  return setBit(byte, bit, enabled);
}

uint8_t *
OpenGD77BaseCodeplug::ChannelBankElement::get(unsigned idx) const {
  return (_data+Offset::channels())+idx*ChannelElement::size();
}

OpenGD77BaseCodeplug::ChannelElement
OpenGD77BaseCodeplug::ChannelBankElement::channel(unsigned int n) {
  return ChannelElement((_data+Offset::channels())+n*ChannelElement::size());
}


/* ******************************************************************************************** *
 * Implementation of OpenGD77BaseCodeplug::VFOChannelElement
 * ******************************************************************************************** */
OpenGD77BaseCodeplug::VFOChannelElement::VFOChannelElement(uint8_t *ptr, unsigned size)
  : ChannelElement(ptr, size)
{
  // pass...
}

OpenGD77BaseCodeplug::VFOChannelElement::VFOChannelElement(uint8_t *ptr)
  : ChannelElement(ptr)
{
  // pass...
}

void
OpenGD77BaseCodeplug::VFOChannelElement::clear() {
  ChannelElement::clear();
  setStepSize(12.5);
  setOffsetMode(OffsetMode::Off);
  setTXOffset(10.0);
}

QString
OpenGD77BaseCodeplug::VFOChannelElement::name() const {
  return QString();
}
void
OpenGD77BaseCodeplug::VFOChannelElement::setName(const QString &name) {
  Q_UNUSED(name);
  ChannelElement::setName("");
}

double
OpenGD77BaseCodeplug::VFOChannelElement::stepSize() const {
  switch (StepSize(getUInt4(Offset::stepSize()))) {
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
OpenGD77BaseCodeplug::VFOChannelElement::setStepSize(double kHz) {
  if (2.5 >= kHz)
    setUInt4(Offset::stepSize(), (unsigned)StepSize::SS2_5kHz);
  else if (5.0 >= kHz)
    setUInt4(Offset::stepSize(), (unsigned)StepSize::SS5kHz);
  else if (6.25 >= kHz)
    setUInt4(Offset::stepSize(), (unsigned)StepSize::SS6_25kHz);
  else if (10.0 >= kHz)
    setUInt4(Offset::stepSize(), (unsigned)StepSize::SS10kHz);
  else if (12.5 >= kHz)
    setUInt4(Offset::stepSize(), (unsigned)StepSize::SS12_5kHz);
  else if (20.0 >= kHz)
    setUInt4(Offset::stepSize(), (unsigned)StepSize::SS20kHz);
  else if (30.0 >= kHz)
    setUInt4(Offset::stepSize(), (unsigned)StepSize::SS30kHz);
  else
    setUInt4(Offset::stepSize(), (unsigned)StepSize::SS50kHz);
}

OpenGD77BaseCodeplug::VFOChannelElement::OffsetMode
OpenGD77BaseCodeplug::VFOChannelElement::offsetMode() const {
  return (OffsetMode)getUInt2(Offset::offsetMode());
}
void
OpenGD77BaseCodeplug::VFOChannelElement::setOffsetMode(OffsetMode mode) {
  setUInt2(Offset::offsetMode(), (unsigned)mode);
}
double
OpenGD77BaseCodeplug::VFOChannelElement::txOffset() const {
  return ((double)getBCD4_le(Offset::txOffset()))/100;
}
void
OpenGD77BaseCodeplug::VFOChannelElement::setTXOffset(double f) {
  setBCD4_le(Offset::txOffset(), (f*100));
}



/* ********************************************************************************************* *
 * Implementation of OpenGD77BaseCodeplug
 * ********************************************************************************************* */
OpenGD77BaseCodeplug::OpenGD77BaseCodeplug(QObject *parent)
  : Codeplug{parent}
{

}
