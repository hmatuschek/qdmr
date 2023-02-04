#include "opengd77_codeplug.hh"
#include "config.hh"
#include "channel.hh"
#include "utils.hh"
#include "logger.hh"
#include <QDateTime>
#include <QtEndian>
#include "opengd77_extension.hh"


/* ******************************************************************************************** *
 * Implementation of OpenGD77Codeplug::ChannelElement
 * ******************************************************************************************** */
OpenGD77Codeplug::ChannelElement::ChannelElement(uint8_t *ptr)
  : GD77Codeplug::ChannelElement(ptr)
{
  // pass...
}

void
OpenGD77Codeplug::ChannelElement::clear() {
  GD77Codeplug::ChannelElement::clear();
  setExtendedPower(Power::Global);
  setSquelchDefault();
  enableScanZoneSkip(false);
  enableScanAllSkip(false);
  clearRadioId();
}

Channel::Power
OpenGD77Codeplug::ChannelElement::power() const {
  switch (extendedPower()) {
  case Power::Global:
    return Channel::Power::Low;
  case Power::P50mW:
    return Channel::Power::Min;
  case Power::P250mW:
  case Power::P500mW:
  case Power::P750mW:
  case Power::P1W:
    return Channel::Power::Low;
  case Power::P2W:
  case Power::P3W:
    return Channel::Power::Mid;
  case Power::P4W:
  case Power::P5W:
    return Channel::Power::High;
  case Power::Max:
    return Channel::Power::Max;
  }
  return Channel::Power::Low;
}

void
OpenGD77Codeplug::ChannelElement::setPower(Channel::Power power) {
  switch (power) {
  case Channel::Power::Max:  setExtendedPower(Power::Max); break;
  case Channel::Power::High: setExtendedPower(Power::P5W); break;
  case Channel::Power::Mid:  setExtendedPower(Power::P3W); break;
  case Channel::Power::Low:  setExtendedPower(Power::P1W); break;
  case Channel::Power::Min:  setExtendedPower(Power::P50mW); break;
  }
}

OpenGD77Codeplug::ChannelElement::Power
OpenGD77Codeplug::ChannelElement::extendedPower() const {
  return (Power)getUInt8(Offset::extendedPower());
}
void
OpenGD77Codeplug::ChannelElement::setExtendedPower(Power power) {
  setUInt8(Offset::extendedPower(), (unsigned)(power));
}

bool
OpenGD77Codeplug::ChannelElement::squelchIsDefault() const {
  return 0 == getUInt8(Offset::squelch());
}
unsigned
OpenGD77Codeplug::ChannelElement::squelch() const {
  return (getUInt8(Offset::squelch())-1)/2;
}
void
OpenGD77Codeplug::ChannelElement::setSquelch(unsigned squelch) {
  setUInt8(Offset::squelch(), (squelch*2)+1);
}
void
OpenGD77Codeplug::ChannelElement::setSquelchDefault() {
  setUInt8(Offset::squelch(), 0);
}

bool
OpenGD77Codeplug::ChannelElement::scanZoneSkip() const {
  return getBit(0x0033, 5);
}
void
OpenGD77Codeplug::ChannelElement::enableScanZoneSkip(bool enable) {
  setBit(0x0033, 5, enable);
}

bool
OpenGD77Codeplug::ChannelElement::scanAllSkip() const {
  return getBit(0x0033, 4);
}
void
OpenGD77Codeplug::ChannelElement::enableScanAllSkip(bool enable) {
  setBit(0x0033, 4, enable);
}

bool
OpenGD77Codeplug::ChannelElement::hasRadioId() const {
  return getBit(0x0026, 7);
}
unsigned
OpenGD77Codeplug::ChannelElement::radioId() const {
  return getUInt24_be(Offset::radioID());
}
void
OpenGD77Codeplug::ChannelElement::setRadioId(unsigned id) {
  setUInt24_be(Offset::radioID(), id);
  setBit(0x0026, 7);
}
void
OpenGD77Codeplug::ChannelElement::clearRadioId() {
  setUInt24_be(Offset::radioID(), 0);
  clearBit(0x0026, 7);
}

Channel *
OpenGD77Codeplug::ChannelElement::toChannelObj(Context &ctx, const ErrorStack& err) const {
  Channel *ch = GD77Codeplug::ChannelElement::toChannelObj(ctx, err);
  if (nullptr == ch)
    return nullptr;

  if (ch->is<FMChannel>()) {
    FMChannel *ac = ch->as<FMChannel>();
    if (squelchIsDefault())
      ac->setSquelchDefault();
    else
      ac->setSquelch(squelch());
  }

  if (Power::Global == extendedPower())
    ch->setDefaultPower();

  OpenGD77ChannelExtension *ext = new OpenGD77ChannelExtension(ch);
  ext->setPower(extendedPower());
  ext->enableScanZoneSkip(scanZoneSkip());
  ext->enableScanAllSkip(scanAllSkip());
  ch->setOpenGD77ChannelExtension(ext);

  return ch;
}

bool
OpenGD77Codeplug::ChannelElement::linkChannelObj(Channel *c, Context &ctx, const ErrorStack& err) const {
  if (! GD77Codeplug::ChannelElement::linkChannelObj(c, ctx, err))
    return false;

  if (c->is<DMRChannel>()) {
    DMRChannel *dc = c->as<DMRChannel>();
    // Link radio ID
    if (hasRadioId()) {
      DMRRadioID *id = ctx.config()->radioIDs()->find(radioId());
      if (nullptr == id) {
        id = new DMRRadioID(QString("ID%1").arg(radioId()), radioId());
        ctx.config()->radioIDs()->add(id);
      }
      dc->setRadioIdObj(id);
    } else {
      // If no radio ID is set, set default.
      dc->setRadioIdObj(DefaultRadioID::get());
    }
  }

  return true;
}

bool
OpenGD77Codeplug::ChannelElement::fromChannelObj(const Channel *c, Context &ctx, const ErrorStack& err) {
  if (! GD77Codeplug::ChannelElement::fromChannelObj(c, ctx, err))
    return false;

  if (c->is<FMChannel>()) {
    const FMChannel *ac = c->as<FMChannel>();
    if (ac->defaultSquelch())
      setSquelchDefault();
    else
      setSquelch(ac->squelch());
  } else if (c->is<DMRChannel>()) {
    const DMRChannel *dc = c->as<DMRChannel>();
    // Encode radio ID
    if (DefaultRadioID::get() == dc->radioIdObj()) {
      clearRadioId();
    } else {
      setRadioId(dc->radioIdObj()->number());
    }
  }

  if (c->defaultPower())
    setExtendedPower(Power::Global);

  if (nullptr == c->openGD77ChannelExtension())
    return true;

  setExtendedPower(c->openGD77ChannelExtension()->power());
  enableScanZoneSkip(c->openGD77ChannelExtension()->scanZoneSkip());
  enableScanAllSkip(c->openGD77ChannelExtension()->scanAllSkip());

  return true;
}

bool
OpenGD77Codeplug::ChannelElement::autoscan() const {
  return false;
}
void
OpenGD77Codeplug::ChannelElement::enableAutoscan(bool enable) {
  Q_UNUSED(enable)
}

bool
OpenGD77Codeplug::ChannelElement::loneWorker() const {
  return false;
}
void
OpenGD77Codeplug::ChannelElement::enableLoneWorker(bool enable) {
  Q_UNUSED(enable)
}

unsigned
OpenGD77Codeplug::ChannelElement::rxSignalingIndex() const {
  return 0;
}
void
OpenGD77Codeplug::ChannelElement::setRXSignalingIndex(unsigned idx) {
  Q_UNUSED(idx);
}

OpenGD77Codeplug::ChannelElement::PrivacyGroup
OpenGD77Codeplug::ChannelElement::privacyGroup() const {
  return PRIVGR_NONE;
}
void
OpenGD77Codeplug::ChannelElement::setPrivacyGroup(PrivacyGroup grp) {
  Q_UNUSED(grp);
}


/* ******************************************************************************************** *
 * Implementation of OpenGD77Codeplug::ZoneElement
 * ******************************************************************************************** */
OpenGD77Codeplug::ZoneElement::ZoneElement(uint8_t *ptr)
  : RadioddityCodeplug::ZoneElement(ptr, ZoneElement::size())
{
  // pass...
}

void
OpenGD77Codeplug::ZoneElement::clear() {
  RadioddityCodeplug::ZoneElement::clear();
  memset(_data, 0x00, size());
}

bool
OpenGD77Codeplug::ZoneElement::linkZoneObj(Zone *zone, Context &ctx, bool putInB, const ErrorStack &err) const {
  if (! isValid()) {
    errMsg(err) << "Cannot link invalid zone.";
    return false;
  }

  for (int i=0; (i<80) && hasMember(i); i++) {
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

bool
OpenGD77Codeplug::ZoneElement::fromZoneObjA(const Zone *zone, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  if (zone->A()->count() && zone->B()->count())
    setName(zone->name() + " A");
  else
    setName(zone->name());

  for (unsigned int i=0; i<Limit::channelCount(); i++) {
    if (i < (unsigned int)zone->A()->count())
      setMember(i, ctx.index(zone->A()->get(i)));
    else
      clearMember(i);
  }

  return true;
}

bool
OpenGD77Codeplug::ZoneElement::fromZoneObjB(const Zone *zone, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  if (zone->A()->count() && zone->B()->count())
    setName(zone->name() + " B");
  else
    setName(zone->name());

  for (int i=0; i<80; i++) {
    if (i < zone->B()->count())
      setMember(i, ctx.index(zone->B()->get(i)));
    else
      clearMember(i);
  }

  return true;
}


/* ******************************************************************************************** *
 * Implementation of OpenGD77Codeplug::ZoneBankElement
 * ******************************************************************************************** */
OpenGD77Codeplug::ZoneBankElement::ZoneBankElement(uint8_t *ptr)
  : RadioddityCodeplug::ZoneBankElement(ptr, ZoneBankElement::size())
{
  // pass...
}

uint8_t *
OpenGD77Codeplug::ZoneBankElement::get(unsigned n) const {
  return (_data + Offset::zones() + n*ZoneElement::size());
}

OpenGD77Codeplug::ZoneElement
OpenGD77Codeplug::ZoneBankElement::zone(unsigned int n) const {
  return ZoneElement(_data + Offset::zones() + n*ZoneElement::size());
}


/* ******************************************************************************************** *
 * Implementation of OpenGD77Codeplug::ContactElement
 * ******************************************************************************************** */
OpenGD77Codeplug::ContactElement::ContactElement(uint8_t *ptr)
  : GD77Codeplug::ContactElement(ptr)
{
  // pass...
}

void
OpenGD77Codeplug::ContactElement::clear() {
  GD77Codeplug::ContactElement::clear();
  disableTimeSlotOverride();
}

bool
OpenGD77Codeplug::ContactElement::isValid() const {
  // The GD77 uses byte 0x17 as a valid flag, the OpenGD77 uses it to override the time-slot.
  return RadioddityCodeplug::ContactElement::isValid();
}
void
OpenGD77Codeplug::ContactElement::markValid(bool valid) {
  // The valid flag of the GD77 codeplug is reused
  if (! valid)
    setName("");
}

bool
OpenGD77Codeplug::ContactElement::overridesTimeSlot() const {
  return TimeSlotOverride::None != (TimeSlotOverride)getUInt8(Offset::timslotOverride());
}
DMRChannel::TimeSlot
OpenGD77Codeplug::ContactElement::timeSlot() const {
  switch ((TimeSlotOverride)getUInt8(Offset::timslotOverride())) {
  case TimeSlotOverride::TS1: return DMRChannel::TimeSlot::TS1;
  case TimeSlotOverride::TS2: return DMRChannel::TimeSlot::TS2;
  default: break;
  }
  return DMRChannel::TimeSlot::TS1;
}
void
OpenGD77Codeplug::ContactElement::setTimeSlot(DMRChannel::TimeSlot ts) {
  if (DMRChannel::TimeSlot::TS1 == ts)
    setUInt8(Offset::timslotOverride(), (unsigned)TimeSlotOverride::TS1);
  else
    setUInt8(Offset::timslotOverride(), (unsigned)TimeSlotOverride::TS2);
}
void
OpenGD77Codeplug::ContactElement::disableTimeSlotOverride() {
  setUInt8(Offset::timslotOverride(), (unsigned)TimeSlotOverride::None);
}

DMRContact *
OpenGD77Codeplug::ContactElement::toContactObj(Context &ctx, const ErrorStack &err) const {
  DMRContact *c = GD77Codeplug::ContactElement::toContactObj(ctx, err);
  if (nullptr == c)
    return nullptr;

  OpenGD77ContactExtension *ext = new OpenGD77ContactExtension(c);
  if (overridesTimeSlot()) {
    if (DMRChannel::TimeSlot::TS1 == timeSlot())
      ext->setTimeSlotOverride(OpenGD77ContactExtension::TimeSlotOverride::TS1);
    else
      ext->setTimeSlotOverride(OpenGD77ContactExtension::TimeSlotOverride::TS2);
  } else {
    ext->setTimeSlotOverride(OpenGD77ContactExtension::TimeSlotOverride::None);
  }
  c->setOpenGD77ContactExtension(ext);

  return c;
}

bool
OpenGD77Codeplug::ContactElement::fromContactObj(const DMRContact *c, Context &ctx, const ErrorStack &err) {
  if (! GD77Codeplug::ContactElement::fromContactObj(c, ctx, err))
    return false;

  if(const OpenGD77ContactExtension *ext = c->openGD77ContactExtension()) {
    if (OpenGD77ContactExtension::TimeSlotOverride::None != ext->timeSlotOverride()) {
      if (OpenGD77ContactExtension::TimeSlotOverride::TS1 == ext->timeSlotOverride())
        setTimeSlot(DMRChannel::TimeSlot::TS1);
      else
        setTimeSlot(DMRChannel::TimeSlot::TS2);
    } else {
      disableTimeSlotOverride();
    }
  }

  return true;
}


/* ******************************************************************************************** *
 * Implementation of OpenGD77Codeplug::GroupListElement
 * ******************************************************************************************** */
OpenGD77Codeplug::GroupListElement::GroupListElement(uint8_t *ptr, unsigned size)
  : GD77Codeplug::GroupListElement(ptr, size)
{
  // pass...
}

OpenGD77Codeplug::GroupListElement::GroupListElement(uint8_t *ptr)
  : GD77Codeplug::GroupListElement(ptr)
{
  // pass...
}

bool
OpenGD77Codeplug::GroupListElement::fromRXGroupListObj(const RXGroupList *lst, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  setName(lst->name());

  // Iterate over all 32 entries in the codeplug
  for (unsigned int i=0; i<Limit::memberCount(); i++) {
    if ((unsigned)lst->count() > i) {
      setMember(i, ctx.index(lst->contact(i)));
    } else {
      // Clear entry.
      clearMember(i);
    }
  }

  return true;
}


/* ******************************************************************************************** *
 * Implementation of OpenGD77Codeplug
 * ******************************************************************************************** */
OpenGD77Codeplug::OpenGD77Codeplug(QObject *parent)
  : GD77Codeplug(parent)
{
  // Delete allocated image by GD77 codeplug
  remImage(0);

  addImage("OpenGD77 Codeplug EEPROM");
  image(EEPROM).addElement(0x000e0, 0x05f20);
  image(EEPROM).addElement(0x07500, 0x0bb00);

  addImage("OpenGD77 Codeplug FLASH");
  image(FLASH).addElement(0x00000, 0x011a0);
  image(FLASH).addElement(0x7b000, 0x13e60);
}

void
OpenGD77Codeplug::clearGeneralSettings() {
  GeneralSettingsElement(data(Offset::settings(), ImageIndex::settings())).clear();
}

bool
OpenGD77Codeplug::encodeGeneralSettings(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)
  GeneralSettingsElement el(data(Offset::settings(), ImageIndex::settings()));
  if (! flags.updateCodePlug)
    el.clear();
  return el.fromConfig(config, ctx);
}

bool
OpenGD77Codeplug::decodeGeneralSettings(Config *config, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)
  return GeneralSettingsElement(data(Offset::settings(), ImageIndex::settings())).updateConfig(config, ctx);
}

void
OpenGD77Codeplug::clearButtonSettings() {
  /// @bug Find button settings within OpenGD77 codeplug.
}

void
OpenGD77Codeplug::clearMessages() {
  MessageBankElement(data(Offset::messages(), ImageIndex::messages())).clear();
}

void
OpenGD77Codeplug::clearScanLists() {
  // Scan lists are not touched with OpenGD77 codeplug
}
bool
OpenGD77Codeplug::encodeScanLists(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(config); Q_UNUSED(flags); Q_UNUSED(ctx); Q_UNUSED(err)
  // Scan lists are not touched with OpenGD77 codeplug
  return true;
}
bool
OpenGD77Codeplug::createScanLists(Config *config, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(config); Q_UNUSED(ctx); Q_UNUSED(err)
  // Scan lists are not touched with OpenGD77 codeplug
  return true;
}
bool
OpenGD77Codeplug::linkScanLists(Config *config, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(config); Q_UNUSED(ctx); Q_UNUSED(err)
  // Scan lists are not touched with OpenGD77 codeplug
  return true;
}

void
OpenGD77Codeplug::clearContacts() {
  for (unsigned int i=0; i<Limit::contactCount(); i++)
    ContactElement(data(Offset::contacts() + i*ContactElement::size(), ImageIndex::contacts())).clear();
}

bool
OpenGD77Codeplug::encodeContacts(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags);

  for (unsigned int i=0; i<Limit::contactCount(); i++) {
    ContactElement el(data(Offset::contacts() + i*ContactElement::size(), ImageIndex::contacts()));
    el.clear();
    if (i >= (unsigned int)config->contacts()->digitalCount())
      continue;
    if (! el.fromContactObj(config->contacts()->digitalContact(i), ctx, err)) {
      errMsg(err) << "Cannot encode contact at index " << i << ".";
      return false;
    }
  }
  return true;
}

bool
OpenGD77Codeplug::createContacts(Config *config, Context &ctx, const ErrorStack &err) {
  for (unsigned int i=0; i<Limit::contactCount(); i++) {
    ContactElement el(data(Offset::contacts() + i*ContactElement::size(), ImageIndex::contacts()));
    if (! el.isValid())
      continue;
    DMRContact *cont = el.toContactObj(ctx, err);
    if (nullptr == cont) {
      errMsg(err) << "Cannot decode contact at index " << i << ".";
      return false;
    }
    ctx.add(cont, i+1); ctx.config()->contacts()->add(cont);
  }
  return true;
}

void
OpenGD77Codeplug::clearDTMFContacts() {
  for (unsigned int i=0; i<Limit::dtmfContactCount(); i++)
    DTMFContactElement(data(Offset::dtmfContacts() + i*DTMFContactElement::size(), ImageIndex::dtmfContacts())).clear();
}

bool
OpenGD77Codeplug::encodeDTMFContacts(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags);

  for (unsigned int i=0; i<Limit::dtmfContactCount(); i++) {
    DTMFContactElement el(data(Offset::dtmfContacts() + i*DTMFContactElement::size(), ImageIndex::dtmfContacts()));
    el.clear();
    if (i >= (unsigned int)config->contacts()->dtmfCount())
      continue;
    if (! el.fromContactObj(config->contacts()->dtmfContact(i), ctx, err)) {
      errMsg(err) << "Cannot encode DTMF contact at index " << i << ".";
      return false;
    }
  }
  return true;
}

bool
OpenGD77Codeplug::createDTMFContacts(Config *config, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  for (unsigned int i=0; i<Limit::dtmfContactCount(); i++) {
    DTMFContactElement el(data(Offset::dtmfContacts()+i*DTMFContactElement::size(), ImageIndex::dtmfContacts()));
    // If contact is disabled
    if (! el.isValid())
      continue;
    DTMFContact *cont = el.toContactObj(ctx);
    if (nullptr == cont) {
      errMsg(err) << "Cannot decode DTMF contact at index " << i << ".";
      return false;
    }
    ctx.add(cont, i+1); config->contacts()->add(cont);
  }
  return true;
}

void
OpenGD77Codeplug::clearChannels() {
  for (unsigned int b=0,c=0; b<Limit::channelBankCount(); b++) {
    uint8_t *ptr = nullptr;
    if (0 == b) ptr = data(Offset::channelBank0(), ImageIndex::channelBank0());
    else ptr = data(Offset::channelBank1() + (b-1)*ChannelBankElement::size(), ImageIndex::channelBank1());
    ChannelBankElement bank(ptr); bank.clear();
    for (unsigned int i=0; (i<ChannelBankElement::Limit::channelCount())&&(c<Limit::channelCount()); i++, c++)
      ChannelElement(bank.get(i)).clear();
  }
}

bool
OpenGD77Codeplug::encodeChannels(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)

  for (unsigned int b=0,c=0; b<Limit::channelBankCount(); b++) {
    uint8_t *ptr = nullptr;
    if (0 == b) ptr = data(Offset::channelBank0(), ImageIndex::channelBank0());
    else ptr = data(Offset::channelBank1() + (b-1)*ChannelBankElement::size(), ImageIndex::channelBank1());
    ChannelBankElement bank(ptr); bank.clear();
    for (unsigned int i=0; (i<ChannelBankElement::Limit::channelCount())&&(c<Limit::channelCount()); i++, c++) {
      ChannelElement el(bank.get(i));
      if (c < (unsigned int)config->channelList()->count()) {
        if (! el.fromChannelObj(config->channelList()->channel(c), ctx, err)) {
          errMsg(err) << "Cannot encode channel " << c << " (" << i << " of bank " << b <<").";
          return false;
        }
        bank.enable(i,true);
      } else {
        el.clear();
        bank.enable(i, false);
      }
    }
  }
  return true;
}

bool
OpenGD77Codeplug::createChannels(Config *config, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  for (unsigned int b=0,c=0; b<Limit::channelBankCount(); b++) {
    uint8_t *ptr = nullptr;
    if (0 == b) ptr = data(Offset::channelBank0(), ImageIndex::channelBank0());
    else ptr = data(Offset::channelBank1() + (b-1)*ChannelBankElement::size(), ImageIndex::channelBank1());
    ChannelBankElement bank(ptr);
    for (unsigned int i=0; (i<ChannelBankElement::Limit::channelCount())&&(c<Limit::channelCount()); i++, c++) {
      if (! bank.isEnabled(i))
        continue;
      Channel *ch = ChannelElement(bank.get(i)).toChannelObj(ctx, err);
      if (nullptr == ch) {
        errMsg(err) << "Cannot decode channel at index " << i
                    << " of bank " << b << ".";
        return false;
      }
      config->channelList()->add(ch); ctx.add(ch, c+1);
    }
  }
  return true;
}

bool
OpenGD77Codeplug::linkChannels(Config *config, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(config); Q_UNUSED(err)

  for (unsigned int b=0,c=0; b<Limit::channelBankCount(); b++) {
    uint8_t *ptr = nullptr;
    if (0 == b) ptr = data(Offset::channelBank0(), ImageIndex::channelBank0());
    else ptr = data(Offset::channelBank1() + (b-1)*ChannelBankElement::size(), ImageIndex::channelBank1());
    ChannelBankElement bank(ptr);
    for (unsigned int i=0; (i<ChannelBankElement::Limit::channelCount())&&(c<Limit::channelCount()); i++, c++) {
      if (! bank.isEnabled(i))
        continue;
      if (! ChannelElement(bank.get(i)).linkChannelObj(ctx.get<Channel>(c+1), ctx, err))
        return false;
    }
  }
  return true;
}

void
OpenGD77Codeplug::clearBootSettings() {
  BootSettingsElement(data(Offset::bootSettings(), ImageIndex::bootSettings())).clear();
}

void
OpenGD77Codeplug::clearMenuSettings() {
  MenuSettingsElement(data(Offset::menuSettings(), ImageIndex::menuSettings())).clear();
}

void
OpenGD77Codeplug::clearBootText() {
  BootTextElement(data(Offset::bootText(), ImageIndex::bootText())).clear();
}

bool
OpenGD77Codeplug::encodeBootText(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(config);
  return BootTextElement(data(Offset::bootText(), ImageIndex::bootText())).fromConfig(ctx, err);
}

bool
OpenGD77Codeplug::decodeBootText(Config *config, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(config); Q_UNUSED(err)
  return BootTextElement(data(Offset::bootText(), ImageIndex::bootText())).updateConfig(ctx, err);
}

void
OpenGD77Codeplug::clearVFOSettings() {
  VFOChannelElement(data(Offset::vfoA(), ImageIndex::vfoA())).clear();
  VFOChannelElement(data(Offset::vfoB(), ImageIndex::vfoB())).clear();
}

void
OpenGD77Codeplug::clearZones() {
  ZoneBankElement bank(data(Offset::zoneBank(), ImageIndex::zoneBank()));
  bank.clear();
  for (unsigned int i=0; i<ZoneBankElement::Limit::zoneCount(); i++)
    ZoneElement(bank.get(i)).clear();
}

bool
OpenGD77Codeplug::encodeZones(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags);

  ZoneBankElement bank(data(Offset::zoneBank(), ImageIndex::zoneBank()));

  // Pack Zones
  bool pack_zone_a = true;
  for (unsigned int i=0, j=0; i<ZoneBankElement::Limit::zoneCount(); i++) {
    ZoneElement z(bank.get(i));
next:
    if (j >= (unsigned int)config->zones()->count()) {
      bank.enable(i, false);
      continue;
    }

    // Construct from Zone obj
    Zone *zone = config->zones()->zone(j);
    if (pack_zone_a) {
      pack_zone_a = false;
      if (zone->A()->count())
        z.fromZoneObjA(zone, ctx, err);
      else
        goto next;
    } else {
      pack_zone_a = true;
      j++;
      if (zone->B()->count())
        z.fromZoneObjB(zone, ctx, err);
      else
        goto next;
    }
    bank.enable(i, true);
  }
  return true;
}

bool
OpenGD77Codeplug::createZones(Config *config, Context &ctx, const ErrorStack &err) {
  QString last_zonename, last_zonebasename; Zone *last_zone = nullptr;
  bool extend_last_zone = false;
  ZoneBankElement bank(data(Offset::zoneBank(), ImageIndex::zoneBank()));

  for (unsigned int i=0; i<ZoneBankElement::Limit::zoneCount(); i++) {
    if (! bank.isEnabled(i))
      continue;
    ZoneElement z = bank.zone(i);

    // Determine whether this zone should be combined with the previous one
    QString zonename = z.name();
    QString zonebasename = zonename; zonebasename.chop(2);
    extend_last_zone = ( zonename.endsWith(" B") && last_zonename.endsWith(" A")
                         && (zonebasename == last_zonebasename)
                         && (nullptr != last_zone) && (0 == last_zone->B()->count()) );
    last_zonename = zonename;
    last_zonebasename = zonebasename;

    // Create zone obj
    if (! extend_last_zone) {
      last_zone = z.toZoneObj(ctx, err);
      if (nullptr == last_zone) {
        errMsg(err) << "Cannot decode zone at index " << i << ".";
        return false;
      }
      config->zones()->add(last_zone);
      ctx.add(last_zone, i+1);
    } else {
      // when extending the last zone, chop its name to remove the "... A" part.
      last_zone->setName(last_zonebasename);
    }
  }
  return true;
}

bool
OpenGD77Codeplug::linkZones(Config *config, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(config)

  QString last_zonename, last_zonebasename; Zone *last_zone = nullptr;
  bool extend_last_zone = false;
  ZoneBankElement bank(data(Offset::zoneBank(), ImageIndex::zoneBank()));

  for (unsigned int i=0; i<ZoneBankElement::Limit::zoneCount(); i++) {
    if (! bank.isEnabled(i))
      continue;
    ZoneElement z(bank.get(i));

    // Determine whether this zone should be combined with the previous one
    QString zonename = z.name();
    QString zonebasename = zonename; zonebasename.chop(2);
    extend_last_zone = ( zonename.endsWith(" B") && last_zonename.endsWith(" A")
                         && (zonebasename == last_zonebasename)
                         && (nullptr != last_zone) && (0 == last_zone->B()->count()) );
    last_zonename = zonename;
    last_zonebasename = zonebasename;

    // Create zone obj
    if (! extend_last_zone) {
      last_zone = ctx.get<Zone>(i+1);
    }
    if (! z.linkZoneObj(last_zone, ctx, extend_last_zone, err)) {
      errMsg(err) << "Cannot link zone at index " << i << ".";
      return false;
    }
  }

  return true;
}

void
OpenGD77Codeplug::clearGroupLists() {
  GroupListBankElement bank(data(Offset::groupLists(), ImageIndex::groupLists())); bank.clear();
  for (unsigned int i=0; i<GroupListBankElement::Limit::groupListCount(); i++)
    GroupListElement(bank.get(i)).clear();
}

bool
OpenGD77Codeplug::encodeGroupLists(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags);

  GroupListBankElement bank(data(Offset::groupLists(), ImageIndex::groupLists())); bank.clear();
  for (unsigned int i=0; i<GroupListBankElement::Limit::groupListCount(); i++) {
    GroupListElement el(bank.get(i));
    if (i < (unsigned int)config->rxGroupLists()->count()) {
      if (! el.fromRXGroupListObj(config->rxGroupLists()->list(i), ctx, err))
        return false;
      bank.setContactCount(i, config->rxGroupLists()->list(i)->count());
    }
  }

  return true;
}

bool
OpenGD77Codeplug::createGroupLists(Config *config, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  GroupListBankElement bank(data(Offset::groupLists(), ImageIndex::groupLists()));
  for (unsigned int i=0; i<GroupListBankElement::Limit::groupListCount(); i++) {
    if (! bank.isEnabled(i))
      continue;
    GroupListElement el(bank.get(i));
    RXGroupList *list = el.toRXGroupListObj(ctx, err);
    if (nullptr == list)
      return false;
    config->rxGroupLists()->add(list); ctx.add(list, i+1);
  }
  return true;
}

bool
OpenGD77Codeplug::linkGroupLists(Config *config, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(config)

  GroupListBankElement bank(data(Offset::groupLists(), ImageIndex::groupLists()));
  for (unsigned int i=0; i<GroupListBankElement::Limit::groupListCount(); i++) {
    if (! bank.isEnabled(i))
      continue;
    GroupListElement el(bank.get(i));
    if (! el.linkRXGroupListObj(bank.contactCount(i), ctx.get<RXGroupList>(i+1), ctx, err)) {
      errMsg(err) << "Cannot link group list '" << ctx.get<RXGroupList>(i+1)->name() << "'.";
      return false;
    }
  }
  return true;
}

void
OpenGD77Codeplug::clearEncryption() {
  // Encryption not supported in OpenGD77 codeplug
}
bool
OpenGD77Codeplug::encodeEncryption(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(config); Q_UNUSED(flags); Q_UNUSED(ctx); Q_UNUSED(err)
  // Encryption not supported in OpenGD77 codeplug
  return true;
}
bool
OpenGD77Codeplug::createEncryption(Config *config, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(config); Q_UNUSED(ctx); Q_UNUSED(err)
  // Encryption not supported in OpenGD77 codeplug
  return true;
}
bool
OpenGD77Codeplug::linkEncryption(Config *config, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(config); Q_UNUSED(ctx); Q_UNUSED(err)
  // Encryption not supported in OpenGD77 codeplug
  return true;
}
