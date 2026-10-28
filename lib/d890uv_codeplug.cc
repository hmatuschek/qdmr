#include "d890uv_codeplug.hh"
#include "config.hh"
#include "logger.hh"
#include "utils.hh"
#include "zone.hh"
#include "scanlist.hh"
#include "rxgrouplist.hh"
#include "radioid.hh"
#include <QtEndian>
#include <algorithm>


/* ********************************************************************************************* *
 * Implementation of D890UVCodeplug::ChannelElement
 * ********************************************************************************************* */
D890UVCodeplug::ChannelElement::ChannelElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}

void
D890UVCodeplug::ChannelElement::clear() {
  // Template: a factory channel with frequencies, mode and name cleared. Among others, this
  // holds CTCSS 131.8 Hz (index 0x15) and DCS 023N (0x0013) as unused defaults and color code 1.
  static const uint8_t tmpl[size()] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x1c, 0x00, 0x15, 0x15, 0x13, 0x00, 0x13, 0x00,
    0x26, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0d,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  };
  memcpy(_data, tmpl, size());
}

Frequency
D890UVCodeplug::ChannelElement::rxFrequency() const {
  return Frequency::fromHz(((unsigned long long)getBCD8_be(Offset::rxFrequency()))*10);
}

Frequency
D890UVCodeplug::ChannelElement::txFrequency() const {
  unsigned long long rx = getBCD8_be(Offset::rxFrequency()), off = getBCD8_be(Offset::txOffset());
  switch ((RepeaterMode)getUInt2(Offset::repeaterMode())) {
  case RepeaterMode::Positive: return Frequency::fromHz((rx+off)*10);
  case RepeaterMode::Negative: return Frequency::fromHz((rx-off)*10);
  default: break;
  }
  return Frequency::fromHz(rx*10);
}

void
D890UVCodeplug::ChannelElement::setFrequencies(const Frequency &rx, const Frequency &tx) {
  unsigned long long rxf = rx.inHz()/10, txf = tx.inHz()/10;
  setBCD8_be(Offset::rxFrequency(), rxf);
  if (rxf == txf) {
    setBCD8_be(Offset::txOffset(), 0);
    setUInt2(Offset::repeaterMode(), (unsigned)RepeaterMode::Simplex);
  } else if (rxf < txf) {
    setBCD8_be(Offset::txOffset(), txf-rxf);
    setUInt2(Offset::repeaterMode(), (unsigned)RepeaterMode::Positive);
  } else {
    setBCD8_be(Offset::txOffset(), rxf-txf);
    setUInt2(Offset::repeaterMode(), (unsigned)RepeaterMode::Negative);
  }
}

D890UVCodeplug::ChannelElement::Mode
D890UVCodeplug::ChannelElement::mode() const {
  return (Mode)getUInt2(Offset::mode());
}
void
D890UVCodeplug::ChannelElement::setMode(Mode mode) {
  setUInt2(Offset::mode(), (unsigned)mode);
}

Channel::Power
D890UVCodeplug::ChannelElement::power() const {
  switch (getUInt2(Offset::power())) {
  case 0: return Channel::Power::Low;
  case 1: return Channel::Power::Mid;
  case 2: return Channel::Power::High;
  default: break;
  }
  return Channel::Power::Max;
}
void
D890UVCodeplug::ChannelElement::setPower(Channel::Power power) {
  switch (power) {
  case Channel::Power::Min:
  case Channel::Power::Low: setUInt2(Offset::power(), 0); break;
  case Channel::Power::Mid: setUInt2(Offset::power(), 1); break;
  case Channel::Power::High: setUInt2(Offset::power(), 2); break;
  case Channel::Power::Max: setUInt2(Offset::power(), 3); break;
  }
}

bool
D890UVCodeplug::ChannelElement::wide() const {
  return getBit(Offset::bandwidth());
}
void
D890UVCodeplug::ChannelElement::setWide(bool wide) {
  setBit(Offset::bandwidth(), wide);
}

bool
D890UVCodeplug::ChannelElement::rxOnly() const {
  return getBit(Offset::rxOnly());
}
void
D890UVCodeplug::ChannelElement::setRXOnly(bool enable) {
  setBit(Offset::rxOnly(), enable);
}

static SelectiveCall
decodeTone(unsigned mode, uint8_t ctcss, uint16_t dcs) {
  if (1 == mode)
    return AnytoneCodeplugGen2::CTCSS::decode(ctcss);
  if (2 == mode)
    return SelectiveCall::fromBinaryDCS(dcs % 512, dcs >= 512);
  return SelectiveCall();
}

SelectiveCall
D890UVCodeplug::ChannelElement::rxTone() const {
  return decodeTone(getUInt2(Offset::rxSignaling()), getUInt8(Offset::rxCTCSS()),
                    getUInt16_le(Offset::rxDCS()));
}
void
D890UVCodeplug::ChannelElement::setRXTone(const SelectiveCall &tone) {
  // Squelch mode 0 = carrier, 1 = sub tone.
  if (tone.isCTCSS()) {
    setUInt2(Offset::rxSignaling(), (unsigned)SignalingMode::CTCSS);
    setUInt8(Offset::rxCTCSS(), CTCSS::encode(tone));
    setUInt3(Offset::squelchMode(), 1);
  } else if (tone.isDCS()) {
    setUInt2(Offset::rxSignaling(), (unsigned)SignalingMode::DCS);
    setUInt16_le(Offset::rxDCS(), tone.binCode() + (tone.isInverted() ? 512 : 0));
    setUInt3(Offset::squelchMode(), 1);
  } else {
    setUInt2(Offset::rxSignaling(), (unsigned)SignalingMode::None);
    setUInt3(Offset::squelchMode(), 0);
  }
}

SelectiveCall
D890UVCodeplug::ChannelElement::txTone() const {
  return decodeTone(getUInt2(Offset::txSignaling()), getUInt8(Offset::txCTCSS()),
                    getUInt16_le(Offset::txDCS()));
}
void
D890UVCodeplug::ChannelElement::setTXTone(const SelectiveCall &tone) {
  if (tone.isCTCSS()) {
    setUInt2(Offset::txSignaling(), (unsigned)SignalingMode::CTCSS);
    setUInt8(Offset::txCTCSS(), CTCSS::encode(tone));
  } else if (tone.isDCS()) {
    setUInt2(Offset::txSignaling(), (unsigned)SignalingMode::DCS);
    setUInt16_le(Offset::txDCS(), tone.binCode() + (tone.isInverted() ? 512 : 0));
  } else {
    setUInt2(Offset::txSignaling(), (unsigned)SignalingMode::None);
  }
}

unsigned
D890UVCodeplug::ChannelElement::admit() const {
  return getUInt2(Offset::admit());
}
void
D890UVCodeplug::ChannelElement::setAdmit(unsigned admit) {
  setUInt2(Offset::admit(), admit);
}

unsigned
D890UVCodeplug::ChannelElement::contactIndex() const {
  return getUInt32_le(Offset::contactIndex());
}
void
D890UVCodeplug::ChannelElement::setContactIndex(unsigned idx) {
  setUInt32_le(Offset::contactIndex(), idx);
}

unsigned
D890UVCodeplug::ChannelElement::radioIDIndex() const {
  return getUInt8(Offset::radioIDIndex());
}
void
D890UVCodeplug::ChannelElement::setRadioIDIndex(unsigned idx) {
  setUInt8(Offset::radioIDIndex(), idx);
}

bool
D890UVCodeplug::ChannelElement::hasScanListIndex() const {
  return 0xff != scanListIndex();
}
unsigned
D890UVCodeplug::ChannelElement::scanListIndex() const {
  return getUInt8(Offset::scanListIndex());
}
void
D890UVCodeplug::ChannelElement::setScanListIndex(unsigned idx) {
  setUInt8(Offset::scanListIndex(), idx);
}

bool
D890UVCodeplug::ChannelElement::hasGroupListIndex() const {
  return 0xff != groupListIndex();
}
unsigned
D890UVCodeplug::ChannelElement::groupListIndex() const {
  return getUInt8(Offset::groupListIndex());
}
void
D890UVCodeplug::ChannelElement::setGroupListIndex(unsigned idx) {
  setUInt8(Offset::groupListIndex(), idx);
}

unsigned
D890UVCodeplug::ChannelElement::colorCode() const {
  return getUInt8(Offset::colorCode());
}
void
D890UVCodeplug::ChannelElement::setColorCode(unsigned cc) {
  setUInt8(Offset::colorCode(), cc);
  setUInt8(Offset::txColorCode(), cc);
}

DMRChannel::TimeSlot
D890UVCodeplug::ChannelElement::timeSlot() const {
  return getBit(Offset::timeSlot()) ? DMRChannel::TimeSlot::TS2 : DMRChannel::TimeSlot::TS1;
}
void
D890UVCodeplug::ChannelElement::setTimeSlot(DMRChannel::TimeSlot ts) {
  setBit(Offset::timeSlot(), DMRChannel::TimeSlot::TS2 == ts);
}

QString
D890UVCodeplug::ChannelElement::name() const {
  return readUnicode(Offset::name(), Limit::nameLength(), 0x0000);
}
void
D890UVCodeplug::ChannelElement::setName(const QString &name) {
  writeUnicode(Offset::name(), name, Limit::nameLength(), 0x0000);
}

Channel *
D890UVCodeplug::ChannelElement::toChannelObj() const {
  Channel *ch = nullptr;
  if ((Mode::Analog == mode()) || (Mode::MixedAnalog == mode())) {
    if (Mode::MixedAnalog == mode())
      logWarn() << "Mixed mode channel '" << name() << "' is treated as an FM channel.";
    FMChannel *fm = new FMChannel();
    // Admit: 0 = always, 1 = tone, 2 = busy (channel free).
    switch (admit()) {
    case 1: fm->setAdmit(FMChannel::Admit::Tone); break;
    case 2: fm->setAdmit(FMChannel::Admit::Free); break;
    default: fm->setAdmit(FMChannel::Admit::Always); break;
    }
    fm->setRXTone(rxTone());
    fm->setTXTone(txTone());
    fm->setBandwidth(wide() ? FMChannel::Bandwidth::Wide : FMChannel::Bandwidth::Narrow);
    fm->setSquelchDefault();
    ch = fm;
  } else {
    if (Mode::MixedDigital == mode())
      logWarn() << "Mixed mode channel '" << name() << "' is treated as a DMR channel.";
    DMRChannel *dmr = new DMRChannel();
    // Admit: 0 = always, 1 = channel free, 2/3 = (different/same) color code.
    switch (admit()) {
    case 1: dmr->setAdmit(DMRChannel::Admit::Free); break;
    case 2:
    case 3: dmr->setAdmit(DMRChannel::Admit::ColorCode); break;
    default: dmr->setAdmit(DMRChannel::Admit::Always); break;
    }
    dmr->setColorCode(colorCode());
    dmr->setTimeSlot(timeSlot());
    ch = dmr;
  }

  ch->setName(name());
  ch->setRXFrequency(rxFrequency());
  ch->setTXFrequency(txFrequency());
  ch->setPower(power());
  ch->setRXOnly(rxOnly());
  ch->setVOXDefault();
  ch->setDefaultTimeout();
  return ch;
}

bool
D890UVCodeplug::ChannelElement::linkChannelObj(Channel *ch, Context &ctx) const {
  if (DMRChannel *dmr = ch->as<DMRChannel>()) {
    if (ctx.has<DMRContact>(contactIndex()))
      dmr->setContact(ctx.get<DMRContact>(contactIndex()));
    if (hasGroupListIndex() && ctx.has<RXGroupList>(groupListIndex()))
      dmr->setGroupList(ctx.get<RXGroupList>(groupListIndex()));
    if (ctx.has<DMRRadioID>(radioIDIndex())) {
      DMRRadioID *id = ctx.get<DMRRadioID>(radioIDIndex());
      if (id == ctx.config()->settings()->defaultIdRef()->as<DMRRadioID>())
        dmr->setRadioId(DefaultRadioID::get());
      else
        dmr->setRadioId(id);
    }
  }
  if (hasScanListIndex() && ctx.has<ScanList>(scanListIndex()))
    ch->setScanList(ctx.get<ScanList>(scanListIndex()));
  return true;
}

bool
D890UVCodeplug::ChannelElement::fromChannelObj(const Channel *ch, Context &ctx, const ErrorStack &err) {
  clear();

  setName(ch->name());
  setFrequencies(ch->rxFrequency(), ch->txFrequency());
  setPower(ch->defaultPower() ? ctx.config()->settings()->power() : ch->power());
  setRXOnly(ch->rxOnly());
  setScanListIndex(ch->scanList() ? ctx.index(ch->scanList()) : 0xff);

  if (const FMChannel *fm = ch->as<const FMChannel>()) {
    setMode(Mode::Analog);
    switch (fm->admit()) {
    case FMChannel::Admit::Always: setAdmit(0); break;
    case FMChannel::Admit::Tone: setAdmit(1); break;
    case FMChannel::Admit::Free: setAdmit(2); break;
    }
    if (fm->rxTone().isCTCSS() && (CTCSS::customIndex() == CTCSS::encode(fm->rxTone()))) {
      errMsg(err) << "Channel '" << ch->name() << "': Non-standard CTCSS tone "
                  << fm->rxTone().format() << " is not supported yet.";
      return false;
    }
    if (fm->txTone().isCTCSS() && (CTCSS::customIndex() == CTCSS::encode(fm->txTone()))) {
      errMsg(err) << "Channel '" << ch->name() << "': Non-standard CTCSS tone "
                  << fm->txTone().format() << " is not supported yet.";
      return false;
    }
    setRXTone(fm->rxTone());
    setTXTone(fm->txTone());
    setWide(FMChannel::Bandwidth::Wide == fm->bandwidth());
  } else if (const DMRChannel *dmr = ch->as<const DMRChannel>()) {
    setMode(Mode::Digital);
    switch (dmr->admit()) {
    case DMRChannel::Admit::Always: setAdmit(0); break;
    case DMRChannel::Admit::Free: setAdmit(1); break;
    case DMRChannel::Admit::ColorCode: setAdmit(2); break;
    }
    setColorCode(dmr->colorCode());
    setTimeSlot(dmr->timeSlot());
    // The vendor CPS has no "none" for the TX contact, index 0 is the first contact.
    setContactIndex(dmr->contact() ? ctx.index(dmr->contact()) : 0);
    setGroupListIndex(dmr->groupList() ? ctx.index(dmr->groupList()) : 0xff);
    DMRRadioID *id = dmr->radioId();
    if ((nullptr == id) || (DefaultRadioID::get() == id))
      id = ctx.config()->settings()->defaultIdRef()->as<DMRRadioID>();
    setRadioIDIndex(id ? ctx.index(id) : 0);
  } else {
    errMsg(err) << "Channel '" << ch->name() << "' of type " << ch->metaObject()->className()
                << " is not supported.";
    return false;
  }

  return true;
}



/* ********************************************************************************************* *
 * Implementation of D890UVCodeplug::ScanListElement
 * ********************************************************************************************* */
D890UVCodeplug::ScanListElement::ScanListElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}

void
D890UVCodeplug::ScanListElement::clear() {
  // Template: the factory scan list with name and members cleared. Priority channels off,
  // look back times 1.5 s and 2.5 s, drop out delay and dwell time 2.9 s. The two bytes at 0x2e
  // and the single 0x00 at 0x94 are of unknown meaning, the remaining record is 0xff.
  memset(_data, 0xff, size());
  static const uint8_t header[0x30] = {
    0x00, 0x00, 0x01, 0x00, 0x02, 0x00, 0x0f, 0x00, 0x19, 0x00, 0x1d, 0x00, 0x1d, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x23
  };
  memcpy(_data, header, sizeof(header));
  setUInt8(0x0094, 0x00);
}

QString
D890UVCodeplug::ScanListElement::name() const {
  return readUnicode(Offset::name(), Limit::nameLength(), 0x0000);
}
void
D890UVCodeplug::ScanListElement::setName(const QString &name) {
  writeUnicode(Offset::name(), name, Limit::nameLength(), 0x0000);
}

unsigned int
D890UVCodeplug::ScanListElement::count() const {
  unsigned int n = 0;
  while ((n < Limit::members()) && (0xffff != member(n)))
    n++;
  return n;
}
unsigned int
D890UVCodeplug::ScanListElement::member(unsigned int n) const {
  return getUInt16_le(Offset::members() + 2*n);
}
void
D890UVCodeplug::ScanListElement::setMember(unsigned int n, unsigned int idx) {
  setUInt16_le(Offset::members() + 2*n, idx);
}

unsigned int
D890UVCodeplug::ScanListElement::priorityChannel(unsigned int n) const {
  // Bit 0 enables the primary, bit 1 the secondary priority channel.
  if (! getBit(Offset::priorityChannels(), n))
    return 0xffff;
  return getUInt16_le(n ? Offset::secondary() : Offset::primary());
}
void
D890UVCodeplug::ScanListElement::setPriorityChannel(unsigned int n, unsigned int code) {
  setBit(Offset::priorityChannels(), n, true);
  setUInt16_le(n ? Offset::secondary() : Offset::primary(), code);
}

ScanList *
D890UVCodeplug::ScanListElement::toScanListObj() const {
  return new ScanList(name());
}

bool
D890UVCodeplug::ScanListElement::linkScanListObj(ScanList *lst, Context &ctx) const {
  for (unsigned int n=0; n<2; n++) {
    unsigned int code = priorityChannel(n);
    Channel *ch = nullptr;
    if (0 == code)
      ch = SelectedChannel::get();
    else if ((0xffff != code) && ctx.has<Channel>(code-1))
      ch = ctx.get<Channel>(code-1);
    if (ch && (0 == n))
      lst->setPrimaryChannel(ch);
    else if (ch)
      lst->setSecondaryChannel(ch);
  }
  for (unsigned int i=0; i<count(); i++) {
    if (! ctx.has<Channel>(member(i))) {
      logWarn() << "Scan list '" << name() << "': Unknown channel index " << member(i) << ".";
      continue;
    }
    lst->addChannel(ctx.get<Channel>(member(i)));
  }
  return true;
}

bool
D890UVCodeplug::ScanListElement::fromScanListObj(const ScanList *lst, Context &ctx, const ErrorStack &err) {
  clear();
  setName(lst->name());
  Channel *priority[2] = {lst->primaryChannel(), lst->secondaryChannel()};
  for (unsigned int n=0; n<2; n++) {
    if (SelectedChannel::get() == priority[n])
      setPriorityChannel(n, 0);
    else if (priority[n])
      setPriorityChannel(n, ctx.index(priority[n])+1);
  }
  if (lst->revertChannel())
    logWarn() << "Scan list '" << lst->name() << "': The revert channel is not supported yet "
                 "and is ignored.";
  unsigned int n = 0;
  for (int i=0; i<lst->count(); i++) {
    if (SelectedChannel::get() == lst->channel(i))
      continue;
    if (n >= Limit::members()) {
      errMsg(err) << "Scan list '" << lst->name() << "' has more than " << Limit::members()
                  << " members.";
      return false;
    }
    setMember(n++, ctx.index(lst->channel(i)));
  }
  return true;
}



/* ********************************************************************************************* *
 * Implementation of D890UVCodeplug::ContactElement
 * ********************************************************************************************* */
D890UVCodeplug::ContactElement::ContactElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}

void
D890UVCodeplug::ContactElement::clear() {
  memset(_data, 0x00, size());
}

DMRContact *
D890UVCodeplug::ContactElement::toContactObj() const {
  DMRContact::Type type = DMRContact::PrivateCall;
  switch (getUInt8(Offset::type())) {
  case 1: type = DMRContact::GroupCall; break;
  case 2: type = DMRContact::AllCall; break;
  default: break;
  }
  return new DMRContact(type, readUnicode(Offset::name(), Limit::nameLength(), 0x0000),
                        getBCD8_be(Offset::number()));
}

bool
D890UVCodeplug::ContactElement::fromContactObj(const DMRContact *contact) {
  clear();
  switch (contact->type()) {
  case DMRContact::PrivateCall: setUInt8(Offset::type(), 0); break;
  case DMRContact::GroupCall: setUInt8(Offset::type(), 1); break;
  case DMRContact::AllCall: setUInt8(Offset::type(), 2); break;
  }
  setBCD8_be(Offset::number(), contact->number());
  writeUnicode(Offset::name(), contact->name(), Limit::nameLength(), 0x0000);
  return true;
}



/* ********************************************************************************************* *
 * Implementation of D890UVCodeplug::GroupListElement
 * ********************************************************************************************* */
D890UVCodeplug::GroupListElement::GroupListElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}

void
D890UVCodeplug::GroupListElement::clear() {
  // Template: the factory group list with name and members cleared. The single 0x00 at 0x22,
  // within member 8, is of unknown meaning.
  memset(_data, 0xff, Offset::name());
  memset(_data + Offset::name(), 0x00, size() - Offset::name());
  setUInt8(0x0022, 0x00);
}

RXGroupList *
D890UVCodeplug::GroupListElement::toGroupListObj() const {
  return new RXGroupList(readUnicode(Offset::name(), Limit::nameLength(), 0x0000));
}

bool
D890UVCodeplug::GroupListElement::linkGroupListObj(RXGroupList *lst, Context &ctx) const {
  for (unsigned int i=0; i<Limit::members(); i++) {
    uint32_t idx = getUInt32_le(Offset::members() + 4*i);
    // Unused members are 0xffffffff. Anything else out of range (e.g., the stray 0x00 at 0x22 of
    // the template) is not a member either.
    if (idx >= D890UVCodeplug::Limit::contacts())
      continue;
    if (! ctx.has<DMRContact>(idx)) {
      logWarn() << "Group list '" << lst->name() << "': Unknown contact index " << idx << ".";
      continue;
    }
    lst->addContact(ctx.get<DMRContact>(idx));
  }
  return true;
}

bool
D890UVCodeplug::GroupListElement::fromGroupListObj(const RXGroupList *lst, Context &ctx, const ErrorStack &err) {
  clear();
  writeUnicode(Offset::name(), lst->name(), Limit::nameLength(), 0x0000);
  if (lst->count() > int(Limit::members())) {
    errMsg(err) << "Group list '" << lst->name() << "' has more than " << Limit::members()
                << " members.";
    return false;
  }
  for (int i=0; i<lst->count(); i++)
    setUInt32_le(Offset::members() + 4*i, ctx.index(lst->contact(i)));
  return true;
}



/* ********************************************************************************************* *
 * Implementation of D890UVCodeplug::RadioIDElement
 * ********************************************************************************************* */
D890UVCodeplug::RadioIDElement::RadioIDElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}

void
D890UVCodeplug::RadioIDElement::clear() {
  // Template: the factory radio ID with ID and name cleared.
  memset(_data, 0x00, 0x28);
  memset(_data + 0x28, 0xff, size() - 0x28);
}

DMRRadioID *
D890UVCodeplug::RadioIDElement::toRadioIDObj() const {
  return new DMRRadioID(readUnicode(Offset::name(), Limit::nameLength(), 0x0000),
                        getBCD8_be(Offset::number()));
}

bool
D890UVCodeplug::RadioIDElement::fromRadioIDObj(const DMRRadioID *id) {
  clear();
  setBCD8_be(Offset::number(), id->number());
  writeUnicode(Offset::name(), id->name(), Limit::nameLength(), 0x0000);
  return true;
}



/* ********************************************************************************************* *
 * Implementation of D890UVCodeplug::RecordElement
 * ********************************************************************************************* */
D890UVCodeplug::RecordElement::RecordElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}



/* ********************************************************************************************* *
 * Implementation of D890UVCodeplug
 * ********************************************************************************************* */
D890UVCodeplug::D890UVCodeplug(QObject *parent)
  : AnytoneCodeplugGen2("AnyTone AT-D890UV Codeplug", parent)
{
  // pass...
}

uint32_t
D890UVCodeplug::channelAddress(unsigned int n) {
  return Offset::channelBanks() + (n/Limit::channelsPerBank())*Offset::betweenChannelBanks()
      + (n%Limit::channelsPerBank())*ChannelElement::size();
}

uint32_t
D890UVCodeplug::contactAddress(unsigned int n) {
  return Offset::contactBanks() + (n/Limit::contactsPerBank())*Offset::betweenContactBanks()
      + (n%Limit::contactsPerBank())*ContactElement::size();
}

uint32_t
D890UVCodeplug::contactBlockAddress(unsigned int n) {
  return Offset::contactBanks() + (n/Limit::contactsPerBank())*Offset::betweenContactBanks()
      + ((n%Limit::contactsPerBank())/Limit::contactsPerBlock())
      * Limit::contactsPerBlock()*ContactElement::size();
}

void
D890UVCodeplug::clear() {
  AnytoneCodeplugGen2::clear();
  allocateBitmaps();
  allocatePreserved();
}

void
D890UVCodeplug::allocateBitmaps() {
  image(0).addElement(Offset::channelBitmap(), Size::channelBitmap());
  image(0).addElement(Offset::zoneBitmap(), Size::zoneBitmap());
  image(0).addElement(Offset::radioIDBitmap(), Size::radioIDBitmap());
  image(0).addElement(Offset::scanListBitmap(), Size::scanListBitmap());
  image(0).addElement(Offset::groupListBitmap(), Size::groupListBitmap());
  image(0).addElement(Offset::contactBitmap(), Size::contactBitmap());
  // A freshly created codeplug holds no contacts. The contact bitmap is inverted.
  memset(data(Offset::contactBitmap()), 0xff, Size::contactBitmap());
}

/** Flash erase unit of the radio. */
static constexpr uint32_t eraseUnit = 0x00040000;

/** The regions, that are not modelled, but share an erase unit with a modelled region. Taken from
 * USB captures of the vendor CPS, which reads and writes all of them. Writing a modelled region
 * without these erases them: Confirmed on hardware for all regions in the erase units 0x03480000,
 * 0x03680000 and 0x03700000. */
static const struct { uint32_t address, size; } preservedRegions[] = {
  { 0x01f81000, 0x0100 },  // VFO A and B, i.e., channels 4000 and 4001 in the last channel bank
  { 0x03480000, 0x1900 },  // 5-tone IDs
  { 0x03481900, 0x0010 },  // 5-tone ID bitmap
  { 0x03481910, 0x0060 },  // 5-tone settings
  { 0x03481a00, 0x0400 },  // 5-tone functions
  { 0x03481e00, 0x0050 },  // DTMF settings
  { 0x03482000, 0x0300 },  // 2-tone IDs
  { 0x03482400, 0x0400 },  // 2-tone functions
  { 0x03482800, 0x0010 },  // 2-tone ID bitmap
  { 0x03482810, 0x0010 },  // 2-tone function bitmap
  { 0x03482820, 0x0020 },  // 2-tone settings
  { 0x03482c20, 0x0020 },  // hidden zone bitmap
  { 0x03482e00, 0x0030 },  // alarm settings
  { 0x03483000, 0x0030 },  // alarm settings extension
  { 0x03483200, 0x03f0 },  // auto repeater offset frequencies
  { 0x03684000, 0x0020 },  // primary radio ID
  { 0x03684020, 0x0020 },  // unknown
  { 0x03700000, 0x0100 },  // analog quick calls
  { 0x03700100, 0x0800 },  // status messages
  { 0x03700900, 0x0700 },  // unknown
  { 0x03701000, 0x0360 },  // hot key settings
  { 0x03701360, 0x01a0 },  // unknown
  { 0x03701500, 0x0010 },  // unknown
  { 0x03703900, 0x0020 }   // unknown
};

/** Returns @c true, if the content of the given erase unit is known, i.e., it holds modelled
 * regions and all other known regions in it are listed in preservedRegions. */
static bool
isKnownEraseUnit(uint32_t unit) {
  if ((unit >= 0x01000000) && (unit < 0x02000000) && (0 == (unit % 0x00080000)))
    return true; // channel banks
  if ((unit >= 0x03a00000) && (unit < 0x03f00000) && (0 == (unit % 0x00080000)))
    return true; // contact banks
  switch (unit) {
  case 0x02000000: // zone channels
  case 0x02100000: // scan lists
  case 0x03480000: // bitmaps
  case 0x03600000: // zone names
  case 0x03680000: // radio IDs
  case 0x03700000: // group list bitmap
  case 0x03780000: // group lists
  case 0x03980000: // contact bitmap
  case 0x03f00000: // contact ID map
    return true;
  default:
    break;
  }
  return false;
}

void
D890UVCodeplug::allocatePreserved() {
  for (auto &region : preservedRegions) {
    if (isAllocated(region.address, 0))
      continue;
    image(0).addElement(region.address, region.size);
    memset(data(region.address), 0xff, region.size);
  }
}

void
D890UVCodeplug::allocateUpdated() {
  allocatePreserved();
}

void
D890UVCodeplug::allocateForDecoding() {
  BitmapElement channels(data(Offset::channelBitmap()), Size::channelBitmap());
  for (unsigned int i=0; i<Limit::channels(); i++) {
    if (channels.isEncoded(i))
      image(0).addElement(channelAddress(i), ChannelElement::size());
  }

  BitmapElement zones(data(Offset::zoneBitmap()), Size::zoneBitmap());
  for (unsigned int i=0; i<Limit::zones(); i++) {
    if (! zones.isEncoded(i))
      continue;
    image(0).addElement(Offset::zoneChannels() + i*Offset::betweenZoneChannels(), Size::zoneChannels());
    image(0).addElement(Offset::zoneNames() + i*Offset::betweenZoneNames(), Size::zoneName());
  }

  BitmapElement radioIDs(data(Offset::radioIDBitmap()), Size::radioIDBitmap());
  for (unsigned int i=0; i<Limit::radioIDs(); i++) {
    if (radioIDs.isEncoded(i))
      image(0).addElement(Offset::radioIDs() + i*RadioIDElement::size(), RadioIDElement::size());
  }

  BitmapElement scanLists(data(Offset::scanListBitmap()), Size::scanListBitmap());
  for (unsigned int i=0; i<Limit::scanLists(); i++) {
    if (scanLists.isEncoded(i))
      image(0).addElement(Offset::scanLists() + i*ScanListElement::size(), ScanListElement::size());
  }

  BitmapElement groupLists(data(Offset::groupListBitmap()), Size::groupListBitmap());
  for (unsigned int i=0; i<Limit::groupLists(); i++) {
    if (groupLists.isEncoded(i))
      image(0).addElement(Offset::groupLists() + i*Offset::betweenGroupLists(), GroupListElement::size());
  }

  InvertedBitmapElement contacts(data(Offset::contactBitmap()), Size::contactBitmap());
  for (unsigned int i=0; i<Limit::contacts(); i++) {
    if (contacts.isEncoded(i) && (! isAllocated(contactBlockAddress(i), 0)))
      image(0).addElement(contactBlockAddress(i), Limit::contactsPerBlock()*ContactElement::size());
  }
}

void
D890UVCodeplug::setBitmaps(Context &ctx) {
  // Only the bits of valid records are touched. Bits beyond, e.g., those of the VFO channels
  // 4000 and 4001 in the channel bitmap, are kept as read from the radio.
  BitmapElement channels(data(Offset::channelBitmap()), Size::channelBitmap());
  for (unsigned int i=0; i<Limit::channels(); i++)
    channels.setEncoded(i, i < ctx.count<Channel>());

  BitmapElement zones(data(Offset::zoneBitmap()), Size::zoneBitmap());
  for (unsigned int i=0; i<Limit::zones(); i++)
    zones.setEncoded(i, i < ctx.count<Zone>());

  BitmapElement radioIDs(data(Offset::radioIDBitmap()), Size::radioIDBitmap());
  for (unsigned int i=0; i<Limit::radioIDs(); i++)
    radioIDs.setEncoded(i, i < ctx.count<DMRRadioID>());

  BitmapElement scanLists(data(Offset::scanListBitmap()), Size::scanListBitmap());
  for (unsigned int i=0; i<Limit::scanLists(); i++)
    scanLists.setEncoded(i, i < ctx.count<ScanList>());

  BitmapElement groupLists(data(Offset::groupListBitmap()), Size::groupListBitmap());
  for (unsigned int i=0; i<Limit::groupLists(); i++)
    groupLists.setEncoded(i, i < ctx.count<RXGroupList>());

  InvertedBitmapElement contacts(data(Offset::contactBitmap()), Size::contactBitmap());
  for (unsigned int i=0; i<Limit::contacts(); i++)
    contacts.setEncoded(i, i < ctx.count<DigitalContact>());
}

void
D890UVCodeplug::allocateForEncoding(Context &ctx) {
  for (unsigned int i=0; i<ctx.count<Channel>(); i++) {
    if (! isAllocated(channelAddress(i), 0))
      image(0).addElement(channelAddress(i), ChannelElement::size());
  }

  for (unsigned int i=0; i<ctx.count<Zone>(); i++) {
    uint32_t addr = Offset::zoneChannels() + i*Offset::betweenZoneChannels();
    if (! isAllocated(addr, 0))
      image(0).addElement(addr, Size::zoneChannels());
    addr = Offset::zoneNames() + i*Offset::betweenZoneNames();
    if (! isAllocated(addr, 0))
      image(0).addElement(addr, Size::zoneName());
  }

  for (unsigned int i=0; i<ctx.count<DMRRadioID>(); i++) {
    uint32_t addr = Offset::radioIDs() + i*RadioIDElement::size();
    if (! isAllocated(addr, 0))
      image(0).addElement(addr, RadioIDElement::size());
  }

  for (unsigned int i=0; i<ctx.count<ScanList>(); i++) {
    uint32_t addr = Offset::scanLists() + i*ScanListElement::size();
    if (! isAllocated(addr, 0))
      image(0).addElement(addr, ScanListElement::size());
  }

  for (unsigned int i=0; i<ctx.count<RXGroupList>(); i++) {
    uint32_t addr = Offset::groupLists() + i*Offset::betweenGroupLists();
    if (! isAllocated(addr, 0))
      image(0).addElement(addr, GroupListElement::size());
  }

  for (unsigned int i=0; i<ctx.count<DigitalContact>(); i++) {
    uint32_t addr = contactBlockAddress(i);
    if (isAllocated(addr, 0))
      continue;
    // Unused records within a block read 0xff on the radio.
    image(0).addElement(addr, Limit::contactsPerBlock()*ContactElement::size());
    memset(data(addr), 0xff, Limit::contactsPerBlock()*ContactElement::size());
  }

  // The ID map, one 0xff entry terminates it. Its size depends on the number of contacts, so an
  // existing map (e.g., one read from the radio) is replaced rather than reused.
  unsigned int mapSize = align_size(Size::contactIDMapEntry()*(1+ctx.count<DigitalContact>()), 16);
  for (int i=0; i<image(0).numElements(); i++) {
    if (Offset::contactIDMap() == image(0).element(i).address()) {
      image(0).remElement(i);
      break;
    }
  }
  image(0).addElement(Offset::contactIDMap(), mapSize);
}

bool
D890UVCodeplug::index(Config *config, Context &ctx, const ErrorStack &err) const {
  for (int i=0, n=0; i<config->radioIDs()->count(); i++) {
    if (config->radioIDs()->get(i)->is<DMRRadioID>())
      ctx.add(config->radioIDs()->get(i)->as<DMRRadioID>(), n++);
  }

  for (int i=0, n=0; i<config->contacts()->count(); i++) {
    if (config->contacts()->contact(i)->is<DMRContact>()) {
      ctx.add(config->contacts()->contact(i)->as<DMRContact>(), n++);
    } else {
      logWarn() << "Contact '" << config->contacts()->contact(i)->name() << "' of type "
                << config->contacts()->contact(i)->metaObject()->className()
                << " is not supported yet and is ignored.";
    }
  }

  for (int i=0; i<config->rxGroupLists()->count(); i++)
    ctx.add(config->rxGroupLists()->list(i), i);

  for (int i=0; i<config->channelList()->count(); i++) {
    Channel *ch = config->channelList()->channel(i);
    if (! (ch->is<FMChannel>() || ch->is<DMRChannel>())) {
      errMsg(err) << "Channel '" << ch->name() << "' of type " << ch->metaObject()->className()
                  << " is not supported.";
      return false;
    }
    ctx.add(ch, i);
  }

  for (int i=0; i<config->zones()->count(); i++)
    ctx.add(config->zones()->zone(i), i);

  for (int i=0; i<config->scanlists()->count(); i++)
    ctx.add(config->scanlists()->scanlist(i), i);

  return true;
}

bool
D890UVCodeplug::encode(Config *config, const Flags &flags, const ErrorStack &err) {
  // Encoding from scratch only makes sense for a codeplug file. Uploading such a codeplug is
  // refused by D890UV::startUpload, as it would not preserve the regions not modelled here.
  if (! flags.updateCodeplug())
    clear();

  Context ctx(config);
  if (! index(config, ctx, err)) {
    errMsg(err) << "Cannot encode AT-D890UV codeplug.";
    return false;
  }

  if ((ctx.count<Channel>() > Limit::channels()) || (ctx.count<Zone>() > Limit::zones()) ||
      (ctx.count<DMRRadioID>() > Limit::radioIDs()) || (ctx.count<ScanList>() > Limit::scanLists()) ||
      (ctx.count<RXGroupList>() > Limit::groupLists()) || (ctx.count<DigitalContact>() > Limit::contacts())) {
    errMsg(err) << "Cannot encode AT-D890UV codeplug: Too many channels, zones, radio IDs, "
                   "scan lists, group lists or contacts.";
    return false;
  }
  if (ctx.count<Channel>() && (0 == ctx.count<Zone>())) {
    // The radio rejects such a codeplug and resets itself to factory defaults.
    errMsg(err) << "Cannot encode AT-D890UV codeplug: At least one zone is needed.";
    return false;
  }

  setBitmaps(ctx);
  allocateForEncoding(ctx);

  for (unsigned int i=0; i<ctx.count<Channel>(); i++) {
    if (! ChannelElement(data(channelAddress(i))).fromChannelObj(ctx.get<Channel>(i), ctx, err))
      return false;
  }

  for (unsigned int i=0; i<ctx.count<Zone>(); i++) {
    Zone *zone = ctx.get<Zone>(i);
    if (zone->A()->count() > int(Limit::zoneMembers())) {
      errMsg(err) << "Zone '" << zone->name() << "' has more than " << Limit::zoneMembers()
                  << " channels.";
      return false;
    }
    RecordElement members(data(Offset::zoneChannels() + i*Offset::betweenZoneChannels()), Size::zoneChannels());
    members.fill(0xff);
    for (int j=0; j<zone->A()->count(); j++)
      members.setUInt16_le(2*j, ctx.index(zone->A()->get(j)->as<Channel>()));
    RecordElement name(data(Offset::zoneNames() + i*Offset::betweenZoneNames()), Size::zoneName());
    name.writeUnicode(0, zone->name(), Limit::zoneNameLength(), 0x0000);
  }

  for (unsigned int i=0; i<ctx.count<DMRRadioID>(); i++)
    RadioIDElement(data(Offset::radioIDs() + i*RadioIDElement::size())).fromRadioIDObj(ctx.get<DMRRadioID>(i));

  for (unsigned int i=0; i<ctx.count<ScanList>(); i++) {
    if (! ScanListElement(data(Offset::scanLists() + i*ScanListElement::size()))
        .fromScanListObj(ctx.get<ScanList>(i), ctx, err))
      return false;
  }

  for (unsigned int i=0; i<ctx.count<RXGroupList>(); i++) {
    if (! GroupListElement(data(Offset::groupLists() + i*Offset::betweenGroupLists()))
        .fromGroupListObj(ctx.get<RXGroupList>(i), ctx, err))
      return false;
  }

  QVector<DMRContact *> contacts;
  for (unsigned int i=0; i<ctx.count<DigitalContact>(); i++) {
    ContactElement(data(contactAddress(i))).fromContactObj(ctx.get<DMRContact>(i));
    contacts.append(ctx.get<DMRContact>(i));
  }
  // The ID map is sorted by the encoded ID: The ID as BCD, shifted left by one, with the lowest
  // bit set for group calls. E.g., group call 12345678 reads 0x2468acf1.
  auto key = [](const DMRContact *c) -> uint32_t {
    uint32_t bcd = 0, number = c->number();
    for (unsigned int shift=0; number; shift+=4, number/=10)
      bcd |= (number % 10) << shift;
    return (bcd << 1) | (DMRContact::GroupCall == c->type() ? 1 : 0);
  };
  std::sort(contacts.begin(), contacts.end(),
            [key](DMRContact *a, DMRContact *b) { return key(a) < key(b); });
  unsigned int mapSize = align_size(Size::contactIDMapEntry()*(1+contacts.size()), 16);
  RecordElement map(data(Offset::contactIDMap()), mapSize);
  map.fill(0xff);
  for (int i=0; i<contacts.size(); i++) {
    map.setUInt32_le(i*Size::contactIDMapEntry(), key(contacts[i]));
    map.setUInt32_le(i*Size::contactIDMapEntry() + 4, ctx.index(contacts[i]));
  }

  // Elements read from a file may lack the preserved regions.
  allocatePreserved();

  return verifyEraseUnits(err) && verifySectorSafety(err);
}

bool
D890UVCodeplug::decode(Config *config, const ErrorStack &err) {
  Q_UNUSED(err);
  Context ctx(config);

  BitmapElement radioIDs(data(Offset::radioIDBitmap()), Size::radioIDBitmap());
  for (unsigned int i=0; i<Limit::radioIDs(); i++) {
    if (! radioIDs.isEncoded(i))
      continue;
    DMRRadioID *id = RadioIDElement(data(Offset::radioIDs() + i*RadioIDElement::size())).toRadioIDObj();
    config->radioIDs()->add(id); ctx.add(id, i);
    if (nullptr == config->settings()->defaultIdRef()->as<DMRRadioID>())
      config->settings()->setDefaultId(id);
  }

  InvertedBitmapElement contacts(data(Offset::contactBitmap()), Size::contactBitmap());
  for (unsigned int i=0; i<Limit::contacts(); i++) {
    if (! contacts.isEncoded(i))
      continue;
    DMRContact *contact = ContactElement(data(contactAddress(i))).toContactObj();
    config->contacts()->add(contact); ctx.add(contact, i);
  }

  BitmapElement groupLists(data(Offset::groupListBitmap()), Size::groupListBitmap());
  for (unsigned int i=0; i<Limit::groupLists(); i++) {
    if (! groupLists.isEncoded(i))
      continue;
    GroupListElement el(data(Offset::groupLists() + i*Offset::betweenGroupLists()));
    RXGroupList *lst = el.toGroupListObj();
    config->rxGroupLists()->add(lst); ctx.add(lst, i);
    el.linkGroupListObj(lst, ctx);
  }

  BitmapElement scanLists(data(Offset::scanListBitmap()), Size::scanListBitmap());
  for (unsigned int i=0; i<Limit::scanLists(); i++) {
    if (! scanLists.isEncoded(i))
      continue;
    ScanList *lst = ScanListElement(data(Offset::scanLists() + i*ScanListElement::size())).toScanListObj();
    config->scanlists()->add(lst); ctx.add(lst, i);
  }

  BitmapElement channels(data(Offset::channelBitmap()), Size::channelBitmap());
  for (unsigned int i=0; i<Limit::channels(); i++) {
    if (! channels.isEncoded(i))
      continue;
    Channel *ch = ChannelElement(data(channelAddress(i))).toChannelObj();
    config->channelList()->add(ch); ctx.add(ch, i);
  }

  // Link everything that references channels, once all channels exist.
  for (unsigned int i=0; i<Limit::channels(); i++) {
    if (channels.isEncoded(i))
      ChannelElement(data(channelAddress(i))).linkChannelObj(ctx.get<Channel>(i), ctx);
  }
  for (unsigned int i=0; i<Limit::scanLists(); i++) {
    if (scanLists.isEncoded(i))
      ScanListElement(data(Offset::scanLists() + i*ScanListElement::size()))
          .linkScanListObj(ctx.get<ScanList>(i), ctx);
  }

  BitmapElement zones(data(Offset::zoneBitmap()), Size::zoneBitmap());
  for (unsigned int i=0; i<Limit::zones(); i++) {
    if (! zones.isEncoded(i))
      continue;
    RecordElement name(data(Offset::zoneNames() + i*Offset::betweenZoneNames()), Size::zoneName());
    Zone *zone = new Zone(name.readUnicode(0, Limit::zoneNameLength(), 0x0000));
    RecordElement members(data(Offset::zoneChannels() + i*Offset::betweenZoneChannels()), Size::zoneChannels());
    for (unsigned int j=0; j<Limit::zoneMembers(); j++) {
      uint16_t idx = members.getUInt16_le(2*j);
      if (0xffff == idx)
        break;
      if (ctx.has<Channel>(idx))
        zone->A()->add(ctx.get<Channel>(idx));
      else
        logWarn() << "Zone '" << zone->name() << "': Unknown channel index " << idx << ".";
    }
    config->zones()->add(zone); ctx.add(zone, i);
  }

  return true;
}

bool
D890UVCodeplug::verifyEraseUnits(const ErrorStack &err) const {
  for (int i=0; i<image(0).numElements(); i++) {
    uint32_t start = image(0).element(i).address(), end = start + image(0).element(i).data().size();
    for (uint32_t unit = start & ~(eraseUnit-1); unit < end; unit += eraseUnit) {
      if (! isKnownEraseUnit(unit)) {
        errMsg(err) << "Refuse to write to the erase unit at 0x" << QString::number(unit, 16)
                    << ": Its content is not known, writing would erase it.";
        return false;
      }
    }
  }
  for (auto &region : preservedRegions) {
    if (! isAllocated(region.address, 0)) {
      errMsg(err) << "Region at 0x" << QString::number(region.address, 16) << " is not preserved.";
      return false;
    }
  }
  return true;
}

bool
D890UVCodeplug::verifySectorSafety(const ErrorStack &err) const {
  // Each 0x40000 flash erase unit holds sector management data in the 16 byte blocks at
  // +0x3fbf0 and +0x3fff0. The vendor CPS never writes them. Writing them makes the radio
  // reset itself to factory defaults.
  for (int i=0; i<image(0).numElements(); i++) {
    uint32_t start = image(0).element(i).address(), end = start + image(0).element(i).data().size();
    for (uint32_t unit = start & ~0x3ffffu; unit < end; unit += 0x40000) {
      for (uint32_t block : {unit + 0x3fbf0, unit + 0x3fff0}) {
        if ((block < end) && (start < block+0x10)) {
          errMsg(err) << "Refuse to write to the sector management block at 0x"
                      << QString::number(block, 16) << ".";
          return false;
        }
      }
    }
  }
  return true;
}
