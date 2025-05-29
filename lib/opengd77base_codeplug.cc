#include "opengd77base_codeplug.hh"
#include "opengd77_extension.hh"
#include "radioid.hh"
#include "config.hh"
#include "logger.hh"
#include "intermediaterepresentation.hh"
#include "satellitedatabase.hh"


/* ********************************************************************************************* *
 * Implementation of some helper functions
 * ********************************************************************************************* */
uint32_t
OpenGD77BaseCodeplug::encodeAngle(double angle) {
  uint32_t sign = (angle < 0) ? 1 : 0;
  uint32_t decimals = std::abs(int(angle * 10000));
  uint32_t deg  = decimals/10000; decimals = decimals % 10000;
  return (sign << 23) | (deg << 15) | decimals;
}

double
OpenGD77BaseCodeplug::decodeAngle(uint32_t code) {
  return (((code >> 23) & 1) ? -1 : 1) * (
        ((code >> 15) & 0xff) + double(code & 0x7fff)/10000
        );
}


uint16_t
OpenGD77BaseCodeplug::encodeSelectiveCall(const SelectiveCall &call) {
  if (call.isInvalid())
    return 0xffff;

  uint16_t dcs = 0, inverted = 0, toneCode = 0;
  if (call.isDCS()) {
    dcs = 1;
    inverted = call.isInverted() ? 1 : 0;
    toneCode = call.octalCode();
  } else {
    dcs = inverted = 0;
    toneCode = call.mHz()/100;
  }

  uint16_t bcd = (((toneCode/1000) % 10 ) << 12) |
      (((toneCode/100) % 10 ) << 8) |
      (((toneCode/10) % 10 ) << 4) |
      (((toneCode/1) % 10 )  << 0);
  return (dcs<<15) | (inverted << 14) | (bcd & 0x3fff);
}

SelectiveCall
OpenGD77BaseCodeplug::decodeSelectiveCall(uint16_t code) {
  if (0xffff == code)
    return SelectiveCall();

  bool dcs = ((code >> 15) & 1),
      inverted = ((code >> 14) & 1);

  uint16_t bcd = (code & 0x3fff);
  code = 1000 * ((bcd >> 12) & 0xf)
      + 100 * ((bcd >> 8) & 0xf)
      + 10 * ((bcd >> 4) & 0xf)
      + 1 * ((bcd >> 0) & 0xf);

  if (! dcs)
    return SelectiveCall(double(code)/10);

  return SelectiveCall(code, inverted);
}



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
  setRXFrequency(Frequency());
  setTXFrequency(Frequency());
  setMode(MODE_ANALOG);
  setPower(Channel::Power::High);
  clearFixedPosition();
  setRXTone(SelectiveCall());
  setTXTone(SelectiveCall());
  enableSimplex(false);
  enablePowerSave(false);
  enableBeep(false);
  clearDMRId();
  clearGroupListIndex();
  setColorCode(0);
  clearAPRSIndex();
  clearTXContact();
  setAliasTimeSlot1(OpenGD77ChannelExtension::TalkerAlias::None);
  setAliasTimeSlot2(OpenGD77ChannelExtension::TalkerAlias::None);
  setTimeSlot(DMRChannel::TimeSlot::TS1);
  setBandwidth(FMChannel::Bandwidth::Narrow);
  enableRXOnly(false);
  enableSkipScan(false);
  enableSkipZoneScan(false);
  enableVOX(false);
  setSquelch(SquelchMode::Global, 0);
}


QString
OpenGD77BaseCodeplug::ChannelElement::name() const {
  return readASCII(Offset::name(), Limit::nameLength(), 0xff);
}

void
OpenGD77BaseCodeplug::ChannelElement::setName(const QString &n) {
  writeASCII(Offset::name(), n, Limit::nameLength(), 0xff);
}


Frequency
OpenGD77BaseCodeplug::ChannelElement::rxFrequency() const {
  return Frequency::fromHz(((unsigned long long)getBCD8_le(Offset::rxFrequency()))*10);
}

void
OpenGD77BaseCodeplug::ChannelElement::setRXFrequency(const Frequency &freq) {
  setBCD8_le(Offset::rxFrequency(), freq.inHz()/10);
}


Frequency
OpenGD77BaseCodeplug::ChannelElement::txFrequency() const {
  return Frequency::fromHz(((unsigned long long)getBCD8_le(Offset::txFrequency()))*10);
}

void
OpenGD77BaseCodeplug::ChannelElement::setTXFrequency(const Frequency &freq) {
  setBCD8_le(Offset::txFrequency(), freq.inHz()/10);
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
  uint32_t latCode = (((uint32_t)getUInt8(Offset::latitude2())) << 16) +
      (((uint32_t)getUInt8(Offset::latitude1())) << 8) +
      ((uint32_t)getUInt8(Offset::latitude0()));
  uint32_t lonCode = (((uint32_t)getUInt8(Offset::longitude2())) << 16) +
      (((uint32_t)getUInt8(Offset::longitude1())) << 8) +
      ((uint32_t)getUInt8(Offset::longitude0()));

  return QGeoCoordinate(decodeAngle(latCode), decodeAngle(lonCode));
}

void
OpenGD77BaseCodeplug::ChannelElement::setFixedPosition(const QGeoCoordinate &coordinate) {
  if (! coordinate.isValid()) {
    clearFixedPosition();
    return;
  }

  uint32_t latCode = encodeAngle(coordinate.latitude());
  uint32_t lonCode = encodeAngle(coordinate.longitude());

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
  return decodeSelectiveCall(getUInt16_le(Offset::rxTone()));
}

void
OpenGD77BaseCodeplug::ChannelElement::setRXTone(const SelectiveCall &code) {
  setUInt16_le(Offset::rxTone(), encodeSelectiveCall(code));
}

SelectiveCall
OpenGD77BaseCodeplug::ChannelElement::txTone() const {
  return decodeSelectiveCall(getUInt16_le(Offset::txTone()));
}

void
OpenGD77BaseCodeplug::ChannelElement::setTXTone(const SelectiveCall &code) {
  setUInt16_le(Offset::txTone(), encodeSelectiveCall(code));
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
  setUInt24_be(Offset::dmrId(), 0x001600);
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


bool
OpenGD77BaseCodeplug::ChannelElement::hasTXContact() const {
  return 0 != getUInt16_le(Offset::txContact());
}

unsigned int
OpenGD77BaseCodeplug::ChannelElement::txContactIndex() const {
  return getUInt16_le(Offset::txContact()) - 1;
}

void
OpenGD77BaseCodeplug::ChannelElement::setTXContactIndex(unsigned int index) {
  setUInt16_le(Offset::txContact(), index+1);
}

void
OpenGD77BaseCodeplug::ChannelElement::clearTXContact() {
  setUInt16_le(Offset::txContact(), 0);
}


OpenGD77ChannelExtension::TalkerAlias
OpenGD77BaseCodeplug::ChannelElement::aliasTimeSlot1() const {
  switch ((Alias) getUInt2(Offset::aliasTimeSlot1())) {
  case Alias::None: return OpenGD77ChannelExtension::TalkerAlias::None;
  case Alias::APRS: return OpenGD77ChannelExtension::TalkerAlias::APRS;
  case Alias::Text: return OpenGD77ChannelExtension::TalkerAlias::Text;
  case Alias::Both: return OpenGD77ChannelExtension::TalkerAlias::Both;
  }

  return OpenGD77ChannelExtension::TalkerAlias::None;
}

void
OpenGD77BaseCodeplug::ChannelElement::setAliasTimeSlot1(OpenGD77ChannelExtension::TalkerAlias alias) {
  switch (alias) {
  case OpenGD77ChannelExtension::TalkerAlias::None: setUInt2(Offset::aliasTimeSlot1(), (unsigned int)Alias::None); break;
  case OpenGD77ChannelExtension::TalkerAlias::APRS: setUInt2(Offset::aliasTimeSlot1(), (unsigned int)Alias::APRS); break;
  case OpenGD77ChannelExtension::TalkerAlias::Text: setUInt2(Offset::aliasTimeSlot1(), (unsigned int)Alias::Text); break;
  case OpenGD77ChannelExtension::TalkerAlias::Both: setUInt2(Offset::aliasTimeSlot1(), (unsigned int)Alias::Both); break;
  }
}


OpenGD77ChannelExtension::TalkerAlias
OpenGD77BaseCodeplug::ChannelElement::aliasTimeSlot2() const {
  switch ((Alias) getUInt2(Offset::aliasTimeSlot2())) {
  case Alias::None: return OpenGD77ChannelExtension::TalkerAlias::None;
  case Alias::APRS: return OpenGD77ChannelExtension::TalkerAlias::APRS;
  case Alias::Text: return OpenGD77ChannelExtension::TalkerAlias::Text;
  case Alias::Both: return OpenGD77ChannelExtension::TalkerAlias::Both;
  }

  return OpenGD77ChannelExtension::TalkerAlias::None;
}

void
OpenGD77BaseCodeplug::ChannelElement::setAliasTimeSlot2(OpenGD77ChannelExtension::TalkerAlias alias) {
  switch (alias) {
  case OpenGD77ChannelExtension::TalkerAlias::None: setUInt2(Offset::aliasTimeSlot2(), (unsigned int)Alias::None); break;
  case OpenGD77ChannelExtension::TalkerAlias::APRS: setUInt2(Offset::aliasTimeSlot2(), (unsigned int)Alias::APRS); break;
  case OpenGD77ChannelExtension::TalkerAlias::Text: setUInt2(Offset::aliasTimeSlot2(), (unsigned int)Alias::Text); break;
  case OpenGD77ChannelExtension::TalkerAlias::Both: setUInt2(Offset::aliasTimeSlot2(), (unsigned int)Alias::Both); break;
  }
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
OpenGD77BaseCodeplug::ChannelElement::decode(Codeplug::Context &ctx, const ErrorStack& err) const {
  Q_UNUSED(err); Q_UNUSED(ctx);
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
  ch->setRXFrequency(rxFrequency());
  if (isSimplex())
    ch->setTXFrequency(rxFrequency());
  else
    ch->setTXFrequency(txFrequency());
  if (globalPower())
    ch->setDefaultPower();
  else
    ch->setPower(power());
  ch->setRXOnly(rxOnly());
  if (vox())
    ch->setVOXDefault();
  else
    ch->disableVOX();

  ch->setOpenGD77ChannelExtension(new OpenGD77ChannelExtension());
  ch->openGD77ChannelExtension()->enableScanZoneSkip(skipZoneScan());
  ch->openGD77ChannelExtension()->enableScanAllSkip(skipScan());
  ch->openGD77ChannelExtension()->enableBeep(beep());
  ch->openGD77ChannelExtension()->enablePowerSave(powerSave());
  if (hasFixedPosition())
    ch->openGD77ChannelExtension()->setLocation(fixedPosition());
  else
    ch->openGD77ChannelExtension()->setLocation(QGeoCoordinate());
  ch->openGD77ChannelExtension()->setTalkerAliasTS1(aliasTimeSlot1());
  ch->openGD77ChannelExtension()->setTalkerAliasTS2(aliasTimeSlot2());

  // done.
  return ch;
}


bool
OpenGD77BaseCodeplug::ChannelElement::link(Channel *c, Context &ctx, const ErrorStack& err) const {
  Q_UNUSED(err)

  // Link common

  if (c->is<DMRChannel>()) {
    // Link DMR channel
    DMRChannel *dc = c->as<DMRChannel>();
    if (hasGroupList() && ctx.has<RXGroupList>(groupListIndex()))
      dc->setGroupListObj(ctx.get<RXGroupList>(groupListIndex()));
    if (hasTXContact() && ctx.has<DMRContact>(txContactIndex()))
      dc->setTXContactObj(ctx.get<DMRContact>(txContactIndex()));
    // Testing dmrId() == 0 fixes a bug in the OpenGD77 firmware. May change in future.
    if (hasDMRId() && (0 != dmrId())) {
      logDebug() << "Channel '" << c->name() << "' overrides default DMR id with "
                 << dmrId() << ".";
      auto id = ctx.config()->radioIDs()->find(dmrId());
      if (nullptr == id) {
        logDebug() << "DMR Id " << dmrId() << " is not defined yet, create one as 'Unknown ID'.";
        id = new DMRRadioID(QString("Unknown ID"), dmrId());
        ctx.config()->radioIDs()->add(id);
      }
      dc->setRadioIdObj(id);
    }
  } else if (c->is<FMChannel>()) {
    // Link FM channel
    auto fm = c->as<FMChannel>();
    if (hasAPRSIndex()) {
      if (! ctx.has<APRSSystem>(aprsIndex())) {
        logWarn() << "Cannot link APRS system index " << aprsIndex() << ": Unkown index. (ignored)";
      } else {
        fm->setAPRSSystem(ctx.get<APRSSystem>(aprsIndex()));
      }
    }
  }

  return true;
}


bool
OpenGD77BaseCodeplug::ChannelElement::encode(const Channel *c, Context &ctx, const ErrorStack& err) {
  clear();

  setName(c->name());

  setRXFrequency(c->rxFrequency());
  setTXFrequency(c->txFrequency());
  enableSimplex(false);

  clearPower();
  if (! c->defaultPower())
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
    if (ac->aprsSystem() && (0<=ctx.index(ac->aprsSystem())))
      setAPRSIndex(ctx.index(ac->aprsSystem()));
  } else if (c->is<DMRChannel>()) {
    const DMRChannel *dc = c->as<const DMRChannel>();
    setMode(MODE_DIGITAL);
    setTimeSlot(dc->timeSlot());
    setColorCode(dc->colorCode());
    // OpenGD77 does not allow for both TX contact and group list, select one, prefer group list
    if (dc->groupListObj())
      setGroupListIndex(ctx.index(dc->groupListObj()));
    else if (dc->txContactObj())
      setTXContactIndex(ctx.index(dc->txContactObj()));
    if (dc->radioIdObj() != ctx.config()->settings()->defaultId())
      setDMRId(dc->radioIdObj()->number());
  } else {
    errMsg(err) << "Cannot encode channel of type '" << c->metaObject()->className()
                << "': Not supported by the radio.";
    return false;
  }

  if (nullptr == c->openGD77ChannelExtension())
    return true;

  // apply extension
  enableSkipZoneScan(c->openGD77ChannelExtension()->scanZoneSkip());
  enableSkipScan(c->openGD77ChannelExtension()->scanAllSkip());
  enableBeep(c->openGD77ChannelExtension()->beep());
  enablePowerSave(c->openGD77ChannelExtension()->powerSave());
  if (c->openGD77ChannelExtension()->location().isValid())
    setFixedPosition(c->openGD77ChannelExtension()->location());
  else
    clearFixedPosition();

  setAliasTimeSlot1(c->openGD77ChannelExtension()->talkerAliasTS1());
  setAliasTimeSlot2(c->openGD77ChannelExtension()->talkerAliasTS2());

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



/* ******************************************************************************************** *
 * Implementation of OpenGD77BaseCodeplug::GeneralSettingsElement
 * ******************************************************************************************** */
OpenGD77BaseCodeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

OpenGD77BaseCodeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}


void
OpenGD77BaseCodeplug::GeneralSettingsElement::clear() {
  setCall("");
  setRadioId(0);
}


Frequency
OpenGD77BaseCodeplug::GeneralSettingsElement::uhfMinFrequency() const {
  return Frequency::fromMHz(getBCD4_le(Offset::uhfMinFrequency()));
}

void
OpenGD77BaseCodeplug::GeneralSettingsElement::setUHFMinFrequency(const Frequency &f) {
  setBCD4_le(Offset::uhfMinFrequency(), f.inMHz());
}

Frequency
OpenGD77BaseCodeplug::GeneralSettingsElement::uhfMaxFrequency() const {
  return Frequency::fromMHz(getBCD4_le(Offset::uhfMaxFrequency()));
}

void
OpenGD77BaseCodeplug::GeneralSettingsElement::setUHFMaxFrequency(const Frequency &f) {
  setBCD4_le(Offset::uhfMaxFrequency(), f.inMHz());
}


Frequency
OpenGD77BaseCodeplug::GeneralSettingsElement::vhfMinFrequency() const {
  return Frequency::fromMHz(getBCD4_le(Offset::vhfMinFrequency()));
}

void
OpenGD77BaseCodeplug::GeneralSettingsElement::setVHFMinFrequency(const Frequency &f) {
  setBCD4_le(Offset::vhfMinFrequency(), f.inMHz());
}


Frequency
OpenGD77BaseCodeplug::GeneralSettingsElement::vhfMaxFrequency() const {
  return Frequency::fromMHz(getBCD4_le(Offset::vhfMaxFrequency()));
}

void
OpenGD77BaseCodeplug::GeneralSettingsElement::setVHFMaxFrequency(const Frequency &f) {
  setBCD4_le(Offset::vhfMaxFrequency(), f.inMHz());
}


QString
OpenGD77BaseCodeplug::GeneralSettingsElement::call() const {
  return readASCII(Offset::call(), Limit::callLength(), 0xff);
}

void
OpenGD77BaseCodeplug::GeneralSettingsElement::setCall(const QString &call) {
  writeASCII(Offset::call(), call, Limit::callLength(), 0xff);
}


unsigned int
OpenGD77BaseCodeplug::GeneralSettingsElement::radioId() const {
  return getBCD8_be(Offset::dmrId());
}

void
OpenGD77BaseCodeplug::GeneralSettingsElement::setRadioId(unsigned int id) {
  setBCD8_be(Offset::dmrId(), id);
}


bool
OpenGD77BaseCodeplug::GeneralSettingsElement::encode(const Context &ctx, const ErrorStack &err) {
  DMRRadioID *id = ctx.config()->settings()->defaultId();
  if (nullptr == id) {
    errMsg(err) << "Cannot encode DMR ID. No default ID defined.";
    return false;
  }

  setCall(id->name());
  setRadioId(id->number());
  return true;
}


bool
OpenGD77BaseCodeplug::GeneralSettingsElement::decode(const Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  DMRRadioID *id = new DMRRadioID(call(), radioId());
  ctx.config()->radioIDs()->add(id);
  ctx.config()->settings()->setDefaultId(id);

  return true;
}



/* ******************************************************************************************** *
 * Implementation of OpenGD77BaseCodeplug::APRSSettingsElement
 * ******************************************************************************************** */
OpenGD77BaseCodeplug::APRSSettingsElement::APRSSettingsElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

OpenGD77BaseCodeplug::APRSSettingsElement::APRSSettingsElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}


void
OpenGD77BaseCodeplug::APRSSettingsElement::clear() {
  Element::clear();

  setName("");
  clearFixedPosition();
  setUInt32_le(Offset::fmFrequency(), 0);

  // Some random data, appears to be important
  writeASCII(Offset::unknownBytes(), "RA", 2);
}


bool
OpenGD77BaseCodeplug::APRSSettingsElement::isValid() const {
  return ! name().isEmpty();
}


QString
OpenGD77BaseCodeplug::APRSSettingsElement::name() const {
  return readASCII(Offset::name(), Limit::nameLength(), 0xff);
}

void
OpenGD77BaseCodeplug::APRSSettingsElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::nameLength(), 0xff);
}


unsigned int
OpenGD77BaseCodeplug::APRSSettingsElement::sourceSSID() const {
  return getUInt8(Offset::sourceSSID());
}

void
OpenGD77BaseCodeplug::APRSSettingsElement::setSourceSSID(unsigned int ssid) {
  setUInt8(Offset::sourceSSID(), ssid);
}


bool
OpenGD77BaseCodeplug::APRSSettingsElement::hasFixedPosition() const {
  return getBit(Offset::useFixedPosition());
}

QGeoCoordinate
OpenGD77BaseCodeplug::APRSSettingsElement::fixedPosition() const {
  uint32_t latCode = getUInt24_le(Offset::latitude());
  uint32_t lonCode = getUInt24_le(Offset::longitude());
  return QGeoCoordinate(decodeAngle(latCode), decodeAngle(lonCode));
}

void
OpenGD77BaseCodeplug::APRSSettingsElement::setFixedPosition(const QGeoCoordinate &coor) {
  setUInt24_le(Offset::latitude(), encodeAngle(coor.latitude()));
  setUInt24_le(Offset::longitude(), encodeAngle(coor.longitude()));
  setBit(Offset::useFixedPosition());
}

void
OpenGD77BaseCodeplug::APRSSettingsElement::clearFixedPosition() {
  clearBit(Offset::useFixedPosition());
}

OpenGD77BaseCodeplug::APRSSettingsElement::PositionPrecision
OpenGD77BaseCodeplug::APRSSettingsElement::positionPrecision() const {
  return (PositionPrecision)getUInt4(Offset::positionPrecision());
}

void
OpenGD77BaseCodeplug::APRSSettingsElement::setPositionPrecision(PositionPrecision prec) {
  setUInt4(Offset::positionPrecision(), (unsigned int) prec);
}


bool
OpenGD77BaseCodeplug::APRSSettingsElement::hasVia1() const {
  return ! via1Call().isEmpty();
}

QString
OpenGD77BaseCodeplug::APRSSettingsElement::via1Call() const {
  return readASCII(Offset::via1Call(), 6, 0x00);
}

unsigned int
OpenGD77BaseCodeplug::APRSSettingsElement::via1SSID() const {
  return getUInt8(Offset::via1SSID());
}

void
OpenGD77BaseCodeplug::APRSSettingsElement::setVia1(const QString &call, unsigned int ssid) {
  writeASCII(Offset::via1Call(), call, 6, 0x00);
  setUInt8(Offset::via1SSID(), ssid);
}

void
OpenGD77BaseCodeplug::APRSSettingsElement::clearVia1() {
  setVia1("", 0);
}


bool
OpenGD77BaseCodeplug::APRSSettingsElement::hasVia2() const {
  return ! via2Call().isEmpty();
}

QString
OpenGD77BaseCodeplug::APRSSettingsElement::via2Call() const {
  return readASCII(Offset::via2Call(), 6, 0x00);
}

unsigned int
OpenGD77BaseCodeplug::APRSSettingsElement::via2SSID() const {
  return getUInt8(Offset::via2SSID());
}

void
OpenGD77BaseCodeplug::APRSSettingsElement::setVia2(const QString &call, unsigned int ssid) {
  writeASCII(Offset::via2Call(), call, 6, 0x00);
  setUInt8(Offset::via2SSID(), ssid);
}

void
OpenGD77BaseCodeplug::APRSSettingsElement::clearVia2() {
  setVia2("", 0);
}


APRSSystem::Icon
OpenGD77BaseCodeplug::APRSSettingsElement::icon() const {
  return (APRSSystem::Icon)getUInt8(Offset::iconIndex());
}
void
OpenGD77BaseCodeplug::APRSSettingsElement::setIcon(APRSSystem::Icon icon) {
  setUInt8(Offset::iconTable(), (APRSSystem::SECONDARY_TABLE & (unsigned int)icon) ? 1 : 0);
  setUInt8(Offset::iconIndex(), APRSSystem::ICON_MASK & (unsigned int)icon);
}

QString
OpenGD77BaseCodeplug::APRSSettingsElement::comment() const {
  return readASCII(Offset::comment(), Limit::commentLength(), 0x00);
}
void
OpenGD77BaseCodeplug::APRSSettingsElement::setComment(const QString &comment) {
  writeASCII(Offset::comment(), comment, Limit::commentLength(), 0x00);
}

OpenGD77BaseCodeplug::APRSSettingsElement::BaudRate
OpenGD77BaseCodeplug::APRSSettingsElement::baudRate() const {
  return getBit(Offset::baudRate()) ? BaudRate::Baud300 : BaudRate::Baud1200;
}
void
OpenGD77BaseCodeplug::APRSSettingsElement::setBaudRate(BaudRate rate) {
  setBit(Offset::baudRate(), BaudRate::Baud300 == rate);
}


Frequency
OpenGD77BaseCodeplug::APRSSettingsElement::fmFrequency() const {
  return Frequency::fromHz(getUInt32_le(Offset::fmFrequency())*10);
}

void
OpenGD77BaseCodeplug::APRSSettingsElement::setFMFrequency(Frequency f) {
  setUInt32_le(Offset::fmFrequency(), f.inHz()/10);
}

bool
OpenGD77BaseCodeplug::APRSSettingsElement::encode(const APRSSystem *sys, const Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(err);
  clear();

  setName(sys->name());
  setSourceSSID(sys->srcSSID());

  QStringList vias = sys->path().split(",");
  unsigned int viaCount = 0;
  for (auto via: vias) {
    QRegExp pattern("^([A-Z0-9]+)-(1?[0-9])$");
    if (! pattern.exactMatch(via))
      continue;
    if (0 == viaCount)
      setVia1(pattern.cap(1), pattern.cap(2).toUInt());
    else if (1 == viaCount)
      setVia2(pattern.cap(1), pattern.cap(2).toUInt());
    else
      break;
    viaCount++;
  }

  setIcon(sys->icon());
  setComment(sys->message());

  clearFixedPosition();
  setBaudRate(BaudRate::Baud1200);
  setPositionPrecision(PositionPrecision::Max);

  if(sys->hasRevertChannel()) {
    setFMFrequency(sys->revertChannel()->txFrequency());
  }

  if (nullptr == sys->openGD77Extension())
    return true;

  if (sys->openGD77Extension()->location().isValid())
    setFixedPosition(sys->openGD77Extension()->location());

  return true;
}


APRSSystem *
OpenGD77BaseCodeplug::APRSSettingsElement::decode(const Context &ctx, const ErrorStack &err) const {
  Q_UNUSED(ctx); Q_UNUSED(err);

  if (! isValid()) {
    errMsg(err) << "Cannot decode invalid APRS settings.";
    return nullptr;
  }

  APRSSystem *sys = new APRSSystem();
  sys->setName(name());
  sys->setDestination("APN000", 0);
  sys->setSrcSSID(sourceSSID());
  QStringList path;
  if (hasVia1()) path.append(QString("%1-%2").arg(via1Call()).arg(via1SSID()));
  if (hasVia2()) path.append(QString("%1-%2").arg(via2Call()).arg(via2SSID()));
  sys->setPath(path.join(","));

  sys->setIcon(icon());
  sys->setMessage(comment());

  auto ext = new OpenGD77APRSSystemExtension();
  sys->setOpenGD77Extension(ext);

  if (hasFixedPosition())
    ext->setLocation(fixedPosition());

  return sys;
}

bool
OpenGD77BaseCodeplug::APRSSettingsElement::link(APRSSystem *sys, const Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err);

  if(fmFrequency().inHz() == 0) {
    sys->resetRevertChannel();
  } else {
    // First, try to find a matching analog channel in list
    FMChannel *ch = ctx.config()->channelList()->findFMChannelByTxFreq(fmFrequency());
    if (! ch) {
      // If no channel is found, create one with the settings from APRS channel:
      ch = new FMChannel();
      ch->setName("APRS Channel");
      ch->setRXFrequency(fmFrequency());
      ch->setTXFrequency(fmFrequency());
      ch->setBandwidth(FMChannel::Bandwidth::Narrow);
      logInfo() << "No matching APRS channel found for TX frequency " << double(fmFrequency().inHz())/1e6
                << "MHz, create one as 'APRS Channel'";
      ctx.config()->channelList()->add(ch);
    }
    sys->setRevertChannel(ch);
  }

  if (ctx.config()->settings()->defaultId())
    sys->setSource(ctx.config()->settings()->defaultId()->name());

  return true;
}



/* ******************************************************************************************** *
 * Implementation of OpenGD77BaseCodeplug::APRSSettingsBank
 * ******************************************************************************************** */
OpenGD77BaseCodeplug::APRSSettingsBankElement::APRSSettingsBankElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

OpenGD77BaseCodeplug::APRSSettingsBankElement::APRSSettingsBankElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}


void
OpenGD77BaseCodeplug::APRSSettingsBankElement::clear() {
  for (unsigned int i=0; i<Limit::systems(); i++)
    system(i).clear();
}


OpenGD77BaseCodeplug::APRSSettingsElement
OpenGD77BaseCodeplug::APRSSettingsBankElement::system(unsigned int idx) const {
  return APRSSettingsElement(_data + Offset::systems() + idx*Offset::betweenSystems());
}


bool
OpenGD77BaseCodeplug::APRSSettingsBankElement::encode(Context &ctx, const ErrorStack &err) {
  for (unsigned int i=0; i<Limit::systems(); i++) {
    if (ctx.has<APRSSystem>(i)) {
      if (! system(i).encode(ctx.get<APRSSystem>(i), ctx, err)) {
        errMsg(err) << "Cannot encode APRS system '" << ctx.get<APRSSystem>(i)->name()
                    << " at index " << i << ".";
        return false;
      }
    } else {
      system(i).clear();
    }
  }

  return true;
}


bool
OpenGD77BaseCodeplug::APRSSettingsBankElement::decode(Context &ctx, const ErrorStack &err) {
  for (unsigned int i=0; i<Limit::systems(); i++) {
    if (system(i).isValid()) {
      APRSSystem *sys = system(i).decode(ctx, err);
      if (nullptr == sys) {
        errMsg(err) << "Cannot decode APRS system at index " << i << ".";
        return false;
      }
      ctx.config()->posSystems()->add(sys);
      ctx.add(sys, i);
    }
  }

  return true;
}

bool
OpenGD77BaseCodeplug::APRSSettingsBankElement::link(Context &ctx, const ErrorStack &err) {
  for (unsigned int i=0; i<Limit::systems(); i++) {
    if (system(i).isValid()) {
      if (! ctx.has<APRSSystem>(i)) {
        errMsg(err) << "Cannot link APRS system at index " << i << ": Not found in context.";
        return false;
      }
      if (! system(i).link(ctx.get<APRSSystem>(i), ctx, err)) {
        errMsg(err) << "Cannot link APRS system '" << ctx.get<APRSSystem>(i)->name()
                    << "' at index " << i << ".";
        return false;
      }
    }
  }
  return true;
}



/* ******************************************************************************************** *
 * Implementation of OpenGD77BaseCodeplug::DTMFContactElement
 * ******************************************************************************************** */
OpenGD77BaseCodeplug::DTMFContactElement::DTMFContactElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

OpenGD77BaseCodeplug::DTMFContactElement::DTMFContactElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}


bool
OpenGD77BaseCodeplug::DTMFContactElement::isValid() const {
  return !name().isEmpty();
}


void
OpenGD77BaseCodeplug::DTMFContactElement::clear() {
  setName("");
  setNumber("");
}


QString
OpenGD77BaseCodeplug::DTMFContactElement::name() const {
  return readASCII(Offset::name(), Limit::nameLength(), 0xff);
}

void
OpenGD77BaseCodeplug::DTMFContactElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::nameLength(), 0xff);
}


QString
OpenGD77BaseCodeplug::DTMFContactElement::number() const {
  QString number;
  uint8_t *ptr = _data + Offset::number();
  const QVector<char> lut = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','*','#'};
  for (unsigned int i=0; (i<Limit::numberLength()) && (lut.size() < *ptr); i++, ptr++)
    number.append(lut[*ptr]);
  return number;
}

void
OpenGD77BaseCodeplug::DTMFContactElement::setNumber(const QString &number) {
  uint8_t *ptr = _data + Offset::number();
  unsigned int n = std::min(Limit::numberLength(), (unsigned int)number.length());
  const QVector<QChar> lut = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','*','#'};
  for (unsigned int i=0; (i<n) && (lut.contains(number.at(i))); i++, ptr++)
    *ptr = lut.indexOf(number.at(i));
}


bool
OpenGD77BaseCodeplug::DTMFContactElement::encode(const DTMFContact *contact, const Context &ctx, const ErrorStack &err)
{
  Q_UNUSED(ctx); Q_UNUSED(err);

  setName(contact->name());
  setNumber(contact->number());

  return true;
}


DTMFContact *
OpenGD77BaseCodeplug::DTMFContactElement::decode(const Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx);

  if (! isValid()) {
    errMsg(err) << "Cannot decode invalid DTMF contact.";
    return nullptr;
  }

  return new DTMFContact(name(), number());
}



/* ******************************************************************************************** *
 * Implementation of OpenGD77BaseCodeplug::DTMFContactBankElement
 * ******************************************************************************************** */
OpenGD77BaseCodeplug::DTMFContactBankElement::DTMFContactBankElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

OpenGD77BaseCodeplug::DTMFContactBankElement::DTMFContactBankElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}


void
OpenGD77BaseCodeplug::DTMFContactBankElement::clear() {
  for (unsigned int i=0; i<Limit::contacts(); i++)
    contact(i).clear();
}


OpenGD77BaseCodeplug::DTMFContactElement
OpenGD77BaseCodeplug::DTMFContactBankElement::contact(unsigned int n) const {
  return DTMFContactElement(_data + Offset::contacts() + n*Offset::betweenContacts());
}


bool
OpenGD77BaseCodeplug::DTMFContactBankElement::encode(Context &ctx, const ErrorStack &err) {
  for (unsigned int i=0; i<Limit::contacts(); i++) {
    if (ctx.has<DTMFContact>(i)) {
      if (! contact(i).encode(ctx.get<DTMFContact>(i), ctx, err)) {
        errMsg(err) << "Cannot encode DTMF contact " << ctx.get<DTMFContact>(i)->name()
                    << " at index " << i << ".";
        return false;
      }
    } else {
      contact(i).clear();
    }
  }
  return true;
}


bool
OpenGD77BaseCodeplug::DTMFContactBankElement::decode(Context &ctx, const ErrorStack &err) {
  for (unsigned int i=0; i<Limit::contacts(); i++) {
    if (! contact(i).isValid())
      continue;
    DTMFContact *cnt = contact(i).decode(ctx, err);
    if (nullptr == cnt) {
      errMsg(err) << "Cannot decode DTMF contact at index " << i << ".";
      return false;
    }
    ctx.config()->contacts()->add(cnt);
    ctx.add(cnt, i);
  }

  return true;
}



/* ********************************************************************************************* *
 * Implementation of OpenGD77BaseCodeplug::BootSettingsElement
 * ********************************************************************************************* */
OpenGD77BaseCodeplug::BootSettingsElement::BootSettingsElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

OpenGD77BaseCodeplug::BootSettingsElement::BootSettingsElement(uint8_t *ptr)
  : Element(ptr, 0x20)
{
  // pass...
}

OpenGD77BaseCodeplug::BootSettingsElement::~BootSettingsElement() {
  // pass...
}

void
OpenGD77BaseCodeplug::BootSettingsElement::clear() {
  enableBootText(true);
  clearBootPassword();
  setLine1("");
  setLine2("");
}


bool
OpenGD77BaseCodeplug::BootSettingsElement::bootText() const {
  return (1 == getUInt8(Offset::bootText()));
}

void
OpenGD77BaseCodeplug::BootSettingsElement::enableBootText(bool enable) {
  setUInt8(Offset::bootText(), (enable ? 1 :0));
}


bool
OpenGD77BaseCodeplug::BootSettingsElement::bootPasswordEnabled() const {
  return (1 == getUInt8(Offset::bootPasswdEnable()));
}

unsigned
OpenGD77BaseCodeplug::BootSettingsElement::bootPassword() const {
  return getBCD8_be(Offset::bootPasswd());
}

void
OpenGD77BaseCodeplug::BootSettingsElement::setBootPassword(unsigned passwd) {
  setBCD8_be(Offset::bootPasswd(), passwd);
  setUInt8(Offset::bootPasswdEnable(), 1);
}

void
OpenGD77BaseCodeplug::BootSettingsElement::clearBootPassword() {
  setUInt8(Offset::bootPasswdEnable(), 0);
}


QString
OpenGD77BaseCodeplug::BootSettingsElement::line1() const {
  return readASCII(Offset::line1(), Limit::lineLength(), 0xff);
}
void
OpenGD77BaseCodeplug::BootSettingsElement::setLine1(const QString &text) {
  writeASCII(Offset::line1(), text, Limit::lineLength(), 0xff);
}

QString
OpenGD77BaseCodeplug::BootSettingsElement::line2() const {
  return readASCII(Offset::line2(), Limit::lineLength(), 0xff);
}
void
OpenGD77BaseCodeplug::BootSettingsElement::setLine2(const QString &text) {
  writeASCII(Offset::line2(), text, Limit::lineLength(), 0xff);
}


bool
OpenGD77BaseCodeplug::BootSettingsElement::encode(const Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)
  setLine1(ctx.config()->settings()->introLine1());
  setLine2(ctx.config()->settings()->introLine2());
  return true;
}

bool
OpenGD77BaseCodeplug::BootSettingsElement::decode(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)
  ctx.config()->settings()->setIntroLine1(line1());
  ctx.config()->settings()->setIntroLine2(line2());
  return true;
}



/* ********************************************************************************************* *
 * Implementation of OpenGD77BaseCodeplug::ZoneElement
 * ********************************************************************************************* */
OpenGD77BaseCodeplug::ZoneElement::ZoneElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

OpenGD77BaseCodeplug::ZoneElement::ZoneElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}

OpenGD77BaseCodeplug::ZoneElement::~ZoneElement() {
  // pass...
}

void
OpenGD77BaseCodeplug::ZoneElement::clear() {
  memset(_data+Offset::name(), 0xff, Limit::nameLength());
  memset(_data+Offset::channels(), 0x00, Offset::betweenChannels()*Limit::memberCount());
}
bool
OpenGD77BaseCodeplug::ZoneElement::isValid() const {
  return (! name().isEmpty());
}

QString
OpenGD77BaseCodeplug::ZoneElement::name() const {
  return readASCII(Offset::name(), Limit::nameLength(), 0xff);
}
void
OpenGD77BaseCodeplug::ZoneElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::nameLength(), 0xff);
}


bool
OpenGD77BaseCodeplug::ZoneElement::hasMember(unsigned n) const {
  if (n >= Limit::memberCount())
    return false;
  return 0 != getUInt16_le(Offset::channels() + Offset::betweenChannels()*n);
}

unsigned
OpenGD77BaseCodeplug::ZoneElement::member(unsigned n) const {
  if (n >= Limit::memberCount())
    return 0;
  return getUInt16_le(Offset::channels() + Offset::betweenChannels()*n)-1;
}

void
OpenGD77BaseCodeplug::ZoneElement::setMember(unsigned n, unsigned idx) {
  if (n >= Limit::memberCount())
    return;
  setUInt16_le(Offset::channels() + Offset::betweenChannels()*n, idx+1);
}

void
OpenGD77BaseCodeplug::ZoneElement::clearMember(unsigned n) {
  setUInt16_le(Offset::channels() + Offset::betweenChannels()*n, 0);
}


Zone *
OpenGD77BaseCodeplug::ZoneElement::decode(const Context &ctx, const ErrorStack &err) const {
  Q_UNUSED(ctx)
  if (! isValid()) {
    errMsg(err) << "Cannot decode an invalid zone.";
    return nullptr;
  }
  return new Zone(name());
}


bool
OpenGD77BaseCodeplug::ZoneElement::link(Zone *zone, Context &ctx, const ErrorStack &err) const {
  if (! isValid()) {
    errMsg(err) << "Cannot link invalid zone.";
    return false;
  }

  for (unsigned int i=0; (i<Limit::memberCount()) && hasMember(i); i++) {
    if (ctx.has<Channel>(member(i))) {
      zone->A()->add(ctx.get<Channel>(member(i)));
    } else {
      logWarn() << "While linking zone '" << zone->name() << "': " << i <<"-th channel index "
                << member(i) << " out of bounds.";
    }
  }

  return true;
}


bool
OpenGD77BaseCodeplug::ZoneElement::encode(const Zone *zone, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  if (zone->A()->count() && zone->B()->count())
    setName(zone->name() + " A");
  else
    setName(zone->name());

  for (unsigned int i=0; i<Limit::memberCount(); i++) {
    if (i < (unsigned int)zone->A()->count())
      setMember(i, ctx.index(zone->A()->get(i)));
    else
      clearMember(i);
  }

  return true;
}



/* ********************************************************************************************* *
 * Implementation of OpenGD77BaseCodeplug::ZoneBankElement
 * ********************************************************************************************* */
OpenGD77BaseCodeplug::ZoneBankElement::ZoneBankElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

OpenGD77BaseCodeplug::ZoneBankElement::ZoneBankElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}

OpenGD77BaseCodeplug::ZoneBankElement::~ZoneBankElement() {
  // pass...
}

void
OpenGD77BaseCodeplug::ZoneBankElement::clear() {
  memset(_data, 0, size());
}

bool
OpenGD77BaseCodeplug::ZoneBankElement::isEnabled(unsigned idx) const {
  unsigned byte= Offset::bitmap() + idx/8, bit = idx%8;
  return getBit(byte, bit);
}
void
OpenGD77BaseCodeplug::ZoneBankElement::enable(unsigned idx, bool enabled) {
  unsigned byte=Offset::bitmap() + idx/8, bit = idx%8;
  setBit(byte, bit, enabled);
}


OpenGD77BaseCodeplug::ZoneElement
OpenGD77BaseCodeplug::ZoneBankElement::zone(unsigned int idx) {
  return ZoneElement(_data + Offset::zones() + idx*Offset::betweenZones());
}


bool
OpenGD77BaseCodeplug::ZoneBankElement::encode(Context &ctx, const ErrorStack &err) {
  for (unsigned int i=0; i<Limit::zoneCount(); i++) {
    if (ctx.has<Zone>(i)) {
      if (! zone(i).encode(ctx.get<Zone>(i), ctx, err)) {
        errMsg(err) << "Cannot encode zone '" << ctx.get<Zone>(i)->name()
                    << "' at index " << i << ".";
        return false;
      }
      enable(i, true);
    } else {
      zone(i).clear();
      enable(i, false);
    }
  }
  return true;
}


bool
OpenGD77BaseCodeplug::ZoneBankElement::decode(Context &ctx, const ErrorStack &err) {
  for (unsigned int i=0; i<Limit::zoneCount(); i++ ) {
    if (! isEnabled(i))
      continue;

    Zone *obj = zone(i).decode(ctx, err);
    if (nullptr == obj) {
      errMsg(err) << "Cannot decode zone at index " << i << ".";
      return false;
    }

    ctx.config()->zones()->add(obj);
    ctx.add(obj, i);
  }

  return true;
}


bool
OpenGD77BaseCodeplug::ZoneBankElement::link(Context &ctx, const ErrorStack &err) {
  for (unsigned int i=0; i<Limit::zoneCount(); i++ ) {
    if (! isEnabled(i))
      continue;

    Zone *obj = ctx.get<Zone>(i);
    if (! zone(i).link(obj, ctx, err)) {
      errMsg(err) << "Cannot link zone '" << obj->name()
                  << "' at index " << i << ".";
      return false;
    }
  }

  return true;
}



/* ********************************************************************************************* *
 * Implementation of OpenGD77BaseCodeplug::ContactElement
 * ********************************************************************************************* */
OpenGD77BaseCodeplug::ContactElement::ContactElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

OpenGD77BaseCodeplug::ContactElement::ContactElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}

OpenGD77BaseCodeplug::ContactElement::~ContactElement() {
  // pass...
}


void
OpenGD77BaseCodeplug::ContactElement::clear() {
  setName("");
  setNumber(0);
  setType(DMRContact::GroupCall);
  setTimeSlotOverride(TimeSlotOverride::None);
}


bool
OpenGD77BaseCodeplug::ContactElement::isValid() const {
  return (! name().isEmpty());
}


QString
OpenGD77BaseCodeplug::ContactElement::name() const {
  return readASCII(Offset::name(), Limit::nameLength(), 0xff);
}

void
OpenGD77BaseCodeplug::ContactElement::setName(const QString name) {
  writeASCII(Offset::name(), name, Limit::nameLength(), 0xff);
}


unsigned
OpenGD77BaseCodeplug::ContactElement::number() const {
  return getBCD8_be(Offset::number());
}

void
OpenGD77BaseCodeplug::ContactElement::setNumber(unsigned id) {
  setBCD8_be(Offset::number(), id);
}


DMRContact::Type
OpenGD77BaseCodeplug::ContactElement::type() const {
  switch (getUInt8(Offset::type())) {
  case 0: return DMRContact::GroupCall;
  case 1: return DMRContact::PrivateCall;
  case 2: return DMRContact::AllCall;
  default: break;
  }
  return DMRContact::PrivateCall;
}

void
OpenGD77BaseCodeplug::ContactElement::setType(DMRContact::Type type) {
  switch (type) {
  case DMRContact::GroupCall: setUInt8(Offset::type(), 0); break;
  case DMRContact::PrivateCall: setUInt8(Offset::type(), 1); break;
  case DMRContact::AllCall: setUInt8(Offset::type(), 2); break;
  }
}


OpenGD77BaseCodeplug::ContactElement::TimeSlotOverride
OpenGD77BaseCodeplug::ContactElement::timeSlotOverride() const {
  return (TimeSlotOverride)getUInt8(Offset::timeSlotOverride());
}
void
OpenGD77BaseCodeplug::ContactElement::setTimeSlotOverride(TimeSlotOverride ts) {
  setUInt8(Offset::timeSlotOverride(), (unsigned int) ts);
}


DMRContact *
OpenGD77BaseCodeplug::ContactElement::decode(Context &ctx, const ErrorStack &err) const {
  Q_UNUSED(ctx)
  if (! isValid()) {
    errMsg(err) << "Cannot create contact from an invalid element.";
    return nullptr;
  }

  auto contact = new DMRContact(type(), name(), number(), false);
  contact->setOpenGD77ContactExtension(new OpenGD77ContactExtension());

  switch (timeSlotOverride()) {
  case TimeSlotOverride::None:
    contact->openGD77ContactExtension()->setTimeSlotOverride(
          OpenGD77ContactExtension::TimeSlotOverride::None);
    break;
  case TimeSlotOverride::TS1:
    contact->openGD77ContactExtension()->setTimeSlotOverride(
          OpenGD77ContactExtension::TimeSlotOverride::TS1);
    break;
  case TimeSlotOverride::TS2:
    contact->openGD77ContactExtension()->setTimeSlotOverride(
          OpenGD77ContactExtension::TimeSlotOverride::TS2);
    break;
  }

  return contact;
}

bool
OpenGD77BaseCodeplug::ContactElement::encode(const DMRContact *cont, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(err)
  setName(cont->name());
  setNumber(cont->number());
  setType(cont->type());

  if (nullptr == cont->openGD77ContactExtension())
    return true;

  switch (cont->openGD77ContactExtension()->timeSlotOverride()) {
  case OpenGD77ContactExtension::TimeSlotOverride::None:
    setTimeSlotOverride(TimeSlotOverride::None);
    break;
  case OpenGD77ContactExtension::TimeSlotOverride::TS1:
    setTimeSlotOverride(TimeSlotOverride::TS1);
    break;
  case OpenGD77ContactExtension::TimeSlotOverride::TS2:
    setTimeSlotOverride(TimeSlotOverride::TS2);
    break;
  }

  return true;
}



/* ********************************************************************************************* *
 * Implementation of OpenGD77BaseCodeplug::ContactBankElement
 * ********************************************************************************************* */
OpenGD77BaseCodeplug::ContactBankElement::ContactBankElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

OpenGD77BaseCodeplug::ContactBankElement::ContactBankElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}


void
OpenGD77BaseCodeplug::ContactBankElement::clear() {
  for (unsigned int i=0; i<Limit::contactCount(); i++)
    contact(i).clear();
}


OpenGD77BaseCodeplug::ContactElement
OpenGD77BaseCodeplug::ContactBankElement::contact(unsigned int idx) const {
  return ContactElement(_data + Offset::contacts() + idx*Offset::betweenContacts());
}


bool
OpenGD77BaseCodeplug::ContactBankElement::encode(Context &ctx, const ErrorStack &err) {
  for (unsigned int i=0; i<Limit::contactCount(); i++) {
    if (ctx.has<DMRContact>(i)) {
      if (! contact(i).encode(ctx.get<DMRContact>(i), ctx, err)) {
        errMsg(err) << "Cannot encode DMR contact " << ctx.get<DMRContact>(i)->name()
                    << " at index " << i << ".";
        return false;
      }
    } else {
      contact(i).clear();
    }
  }

  return true;
}


bool
OpenGD77BaseCodeplug::ContactBankElement::decode(Context &ctx, const ErrorStack &err) {
  for (unsigned int i=0; i<Limit::contactCount(); i++) {
    if (! contact(i).isValid())
      continue;
    DMRContact *cnt = contact(i).decode(ctx, err);
    if (nullptr == cnt) {
      errMsg(err) << "Cannot decode contact at index " << i << ".";
      return false;
    }
    ctx.config()->contacts()->add(cnt);
    ctx.add(cnt, i);
  }
  return true;
}



/* ********************************************************************************************* *
 * Implementation of OpenGD77BaseCodeplug::GroupListElement
 * ********************************************************************************************* */
OpenGD77BaseCodeplug::GroupListElement::GroupListElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

OpenGD77BaseCodeplug::GroupListElement::GroupListElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}


void
OpenGD77BaseCodeplug::GroupListElement::clear() {
  memset(_data + Offset::name(), 0xff, Limit::nameLength()+1);
  memset(_data + Offset::contacts(), 0, Limit::contactCount()*2);
}


QString
OpenGD77BaseCodeplug::GroupListElement::name() const {
  return readASCII(Offset::name(), Limit::nameLength(), 0xff);
}

void
OpenGD77BaseCodeplug::GroupListElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::nameLength(), 0xff);
}


bool
OpenGD77BaseCodeplug::GroupListElement::hasContactIndex(unsigned int i) const {
  return 0 != getUInt16_le(Offset::contacts() + i*Offset::betweenContacts());
}

unsigned int
OpenGD77BaseCodeplug::GroupListElement::contactIndex(unsigned int i) const {
  return getUInt16_le(Offset::contacts() + i*Offset::betweenContacts()) - 1;
}

void
OpenGD77BaseCodeplug::GroupListElement::setContactIndex(unsigned int i, unsigned int contactIdx) {
  setUInt16_le(Offset::contacts() + i*Offset::betweenContacts(), contactIdx + 1);
}

void
OpenGD77BaseCodeplug::GroupListElement::clearContactIndex(unsigned int i) {
  setUInt16_le(Offset::contacts() + i*Offset::betweenContacts(), 0);
}


bool
OpenGD77BaseCodeplug::GroupListElement::encode(RXGroupList *lst, Context &ctx, const ErrorStack &err) {
  setName(lst->name());

  for (unsigned int i=0; i<Limit::contactCount(); i++) {
    if (i < (unsigned int)lst->count()) {
      int idx = ctx.index(lst->contact(i));
      if (0 > idx) {
        errMsg(err) << "Cannot encode group list '" << lst->name()
                    << "', contact '" << lst->contact(i)->name() << "' not indexed.";
        return false;
      }
      setContactIndex(i, idx);
    } else {
      clearContactIndex(i);
    }
  }
  return true;
}


RXGroupList *
OpenGD77BaseCodeplug::GroupListElement::decode(Context &ctx, const ErrorStack &err) const {
  Q_UNUSED(ctx); Q_UNUSED(err);
  return new RXGroupList(name());
}


bool
OpenGD77BaseCodeplug::GroupListElement::link(RXGroupList *lst, Context &ctx, const ErrorStack &err) const {
  for (unsigned int i=0; i<Limit::contactCount(); i++) {
    if (! hasContactIndex(i))
      continue;

    if (! ctx.has<DMRContact>(contactIndex(i))) {
      errMsg(err) << "Cannot resolve contact index " << contactIndex(i) << ".";
      return false;
    }

    lst->addContact(ctx.get<DMRContact>(contactIndex(i)));
  }

  return true;
}



/* ********************************************************************************************* *
 * Implementation of OpenGD77BaseCodeplug::GroupListBankElement
 * ********************************************************************************************* */
OpenGD77BaseCodeplug::GroupListBankElement::GroupListBankElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

OpenGD77BaseCodeplug::GroupListBankElement::GroupListBankElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}


void
OpenGD77BaseCodeplug::GroupListBankElement::clear() {
  for (unsigned int i=0; i<Limit::groupListCount(); i++)
    clearGroupList(i);
}


bool
OpenGD77BaseCodeplug::GroupListBankElement::hasGroupList(unsigned int i) const {
  return 0 != getUInt8(Offset::length() + i);
}

unsigned int
OpenGD77BaseCodeplug::GroupListBankElement::groupListContactCount(unsigned int i) const {
  return getUInt8(Offset::length() + i) - 1;
}

void
OpenGD77BaseCodeplug::GroupListBankElement::setGroupListContactCount(unsigned int i, unsigned int count) {
  setUInt8(Offset::length() + i, count+1);
}

OpenGD77BaseCodeplug::GroupListElement
OpenGD77BaseCodeplug::GroupListBankElement::groupList(unsigned int i) const {
  return GroupListElement(_data + Offset::groupLists()  + i*Offset::betweenGroupLists());
}

void
OpenGD77BaseCodeplug::GroupListBankElement::clearGroupList(unsigned int i) {
  setUInt8(Offset::length() + i, 0);
  groupList(i).clear();
}


bool
OpenGD77BaseCodeplug::GroupListBankElement::encode(Context &ctx, const ErrorStack &err) {
  for (unsigned int i=0; i<Limit::groupListCount(); i++) {
    if (ctx.has<RXGroupList>(i)) {
      auto obj = ctx.get<RXGroupList>(i);
      setGroupListContactCount(i, obj->count());
      if (! groupList(i).encode(obj, ctx, err)) {
        clearGroupList(i);
        errMsg(err) << "Cannot encode group list '" << obj->name() << "' at index " << i << ".";
        return false;
      }
    } else {
      clearGroupList(i);
    }
  }
  return true;
}


bool
OpenGD77BaseCodeplug::GroupListBankElement::decode(Context &ctx, const ErrorStack &err) {
  for (unsigned int i=0; i<Limit::groupListCount(); i++) {
    if (! hasGroupList(i))
      continue;
    auto obj = groupList(i).decode(ctx, err);
    if (nullptr == obj) {
      errMsg(err) << "Cannot decode group list at index " << i << ".";
      return false;
    }
    ctx.config()->rxGroupLists()->add(obj);
    ctx.add(obj, i);
  }

  return true;
}


bool
OpenGD77BaseCodeplug::GroupListBankElement::link(Context &ctx, const ErrorStack &err) {
  for (unsigned int i=0; i<Limit::groupListCount(); i++) {
    if (! hasGroupList(i))
      continue;
    auto obj = ctx.get<RXGroupList>(i);
    if (! groupList(i).link(obj, ctx, err)) {
      errMsg(err) << "Cannot link group list '" << obj->name() << "' at index " << i << ".";
      return false;
    }
  }

  return true;
}



/* ********************************************************************************************* *
 * Implementation of OpenGD77BaseCodeplug
 * ********************************************************************************************* */
OpenGD77BaseCodeplug::OpenGD77BaseCodeplug(QObject *parent)
  : Codeplug{parent}
{
  // pass...
}


void
OpenGD77BaseCodeplug::clear() {
  // Clear general config
  clearGeneralSettings();
  clearDTMFSettings();
  clearAPRSSettings();
  // clear DTMF contacts
  clearDTMFContacts();
  // clear channel
  clearChannels();
  // clear boot settings
  clearBootSettings();
  // clear VFO settings
  clearVFOSettings();
  // clear zones
  clearZones();
  // Clear contacts
  clearContacts();
  // clear group lists
  clearGroupLists();
}

bool
OpenGD77BaseCodeplug::index(Config *config, Context &ctx, const ErrorStack &err) const {
  Q_UNUSED(err)

  // Map radio IDs
  for (int i=0; i<config->radioIDs()->count(); i++)
    ctx.add(config->radioIDs()->getId(i), i+1);

  // Map digital and DTMF contacts
  for (int i=0, d=0, a=0; i<config->contacts()->count(); i++) {
    if (config->contacts()->contact(i)->is<DMRContact>()) {
      ctx.add(config->contacts()->contact(i)->as<DMRContact>(), d); d++;
    } else if (config->contacts()->contact(i)->is<DTMFContact>()) {
      ctx.add(config->contacts()->contact(i)->as<DTMFContact>(), a); a++;
    }
  }

  // Map rx group lists
  for (int i=0; i<config->rxGroupLists()->count(); i++)
    ctx.add(config->rxGroupLists()->list(i), i);

  // Map channels
  for (int i=0; i<config->channelList()->count(); i++)
    ctx.add(config->channelList()->channel(i), i);

  // Map zones
  for (int i=0; i<config->zones()->count(); i++)
    ctx.add(config->zones()->zone(i), i);

  // Map FM APRS systems
  for (int i=0,a=0; i<config->posSystems()->count(); i++) {
    if (config->posSystems()->system(i)->is<APRSSystem>()) {
      ctx.add(config->posSystems()->system(i)->as<APRSSystem>(), a); a++;
    }
  }

  return true;
}

Config *
OpenGD77BaseCodeplug::preprocess(Config *config, const ErrorStack &err) const {
  Config *intermediate = Codeplug::preprocess(config, err);
  if (nullptr == intermediate) {
    errMsg(err) << "Cannot pre-process OpenGD77 codeplug.";
    return nullptr;
  }

  ZoneSplitVisitor splitter;
  if (! splitter.process(intermediate, err)) {
    errMsg(err) << "Cannot split zone for OpenGD77 codeplug.";
    delete intermediate;
    return nullptr;
  }

  return intermediate;
}


bool
OpenGD77BaseCodeplug::encode(Config *config, const Flags &flags, const ErrorStack &err) {
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
OpenGD77BaseCodeplug::encodeElements(const Flags &flags, Context &ctx, const ErrorStack &err) {
  // General config
  if (! this->encodeGeneralSettings(flags, ctx, err)) {
    errMsg(err) << "Cannot encode general settings.";
    return false;
  }

  if (! this->encodeDTMFSettings(flags, ctx, err)) {
    errMsg(err) << "Cannot encode DTMF settings.";
    return false;
  }

  if (! this->encodeAPRSSettings(flags, ctx, err)) {
    errMsg(err) << "Cannot encode APRS settings.";
    return false;
  }

  if (! this->encodeDTMFContacts(flags, ctx, err)) {
    errMsg(err) << "Cannot encode DTMF contacts.";
    return false;
  }

  if (! this->encodeChannels(flags, ctx, err)) {
    errMsg(err) << "Cannot encode channels";
    return false;
  }

  if (! this->encodeBootSettings(flags, ctx, err)) {
    errMsg(err) << "Cannot encode boot text.";
    return false;
  }

  if (! this->encodeZones(flags, ctx, err)) {
    errMsg(err) << "Cannot encode zones.";
    return false;
  }

  // Define Contacts
  if (! this->encodeContacts(flags, ctx, err)) {
    errMsg(err) << "Cannot encode contacts.";
    return false;
  }

  if (! this->encodeGroupLists(flags, ctx, err)) {
    errMsg(err) << "Cannot encode group lists.";
    return false;
  }

  return true;
}


bool
OpenGD77BaseCodeplug::decode(Config *config, const ErrorStack &err) {
  // Clear config object
  config->clear();

  // Create index<->object table.
  Context ctx(config);
  return this->decodeElements(ctx, err);
}

bool
OpenGD77BaseCodeplug::postprocess(Config *config, const ErrorStack &err) const {
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
OpenGD77BaseCodeplug::decodeElements(Context &ctx, const ErrorStack &err) {
  if (! this->decodeGeneralSettings(ctx, err)) {
    errMsg(err) << "Cannot decode general settings.";
    return false;
  }

  if (! this->decodeDTMFSettings(ctx, err)) {
    errMsg(err) << "Cannot decode DTMF settings.";
    return false;
  }

  if (! this->decodeAPRSSettings(ctx, err)) {
    errMsg(err) << "Cannot decode APRS settings.";
    return false;
  }

  if (! this->decodeBootSettings(ctx, err)) {
    errMsg(err) << "Cannot decode boot settings.";
    return false;
  }

  if (! this->createContacts(ctx, err)) {
    errMsg(err) << "Cannot create contacts.";
    return false;
  }

  if (! this->createDTMFContacts(ctx, err)) {
    errMsg(err) << "Cannot create DTMF contacts";
    return false;
  }

  if (! this->createChannels(ctx, err)) {
    errMsg(err) << "Cannot create channels.";
    return false;
  }

  if (! this->createZones(ctx, err)) {
    errMsg(err) << "Cannot create zones.";
    return false;
  }

  if (! this->createGroupLists(ctx, err)) {
    errMsg(err) << "Cannot create group lists.";
    return false;
  }

  if (! this->linkChannels(ctx, err)) {
    errMsg(err) << "Cannot link channels.";
    return false;
  }

  if (! this->linkZones(ctx, err)) {
    errMsg(err) << "Cannot link zones.";
    return false;
  }

  if (! this->linkGroupLists(ctx, err)) {
    errMsg(err) << "Cannot link group lists.";
    return false;
  }

  if (! this->linkAPRSSettings(ctx, err)) {
    errMsg(err) << "Cannot decode APRS settings.";
    return false;
  }

  return true;
}

