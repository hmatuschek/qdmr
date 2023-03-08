#include "opengd77_codeplug.hh"
#include "config.hh"
#include "channel.hh"
#include "utils.hh"
#include "logger.hh"
#include <QDateTime>
#include <QtEndian>
#include "opengd77_extension.hh"


// Stored in EEPROM
#define IMAGE_SETTINGS                   0
#define ADDR_SETTINGS             0x0000e0

#define IMAGE_MESSAGE_BANK               0
#define ADDR_MESSAGE_BANK         0x000128

//#define ADDR_SCANTAB      0x01790
#define NUM_DTMF_CONTACTS               32
#define IMAGE_DTMF_CONTACTS              0
#define ADDR_DTMF_CONTACTS        0x002f88
#define DTMF_CONTACT_SIZE         0x000020

#define NUM_CHANNEL_BANKS                8
#define NUM_CHANNELS_PER_BANK          128
#define NUM_CHANNELS                  1024
#define IMAGE_CHANNEL_BANK_0             0
#define ADDR_CHANNEL_BANK_0       0x003780 // Channels 1-128
#define IMAGE_CHANNEL_BANK_1             1
#define ADDR_CHANNEL_BANK_1       0x07b1b0 // Channels 129-1024
#define CHANNEL_SIZE              0x000038
#define CHANNEL_BANK_SIZE         0x001c10

#define IMAGE_BOOT_SETTINGS              0
#define ADDR_BOOT_SETTINGS        0x007518
#define IMAGE_MENU_SETTINGS              0
#define ADDR_MENU_SETTINGS        0x007538
#define IMAGE_BOOT_TEXT                  0
#define ADDR_BOOT_TEXT            0x007540
#define IMAGE_VFO_A                      0
#define ADDR_VFO_A                0x007590
#define IMAGE_VFO_B                      0
#define ADDR_VFO_B                0x0075c8

#define NUM_ZONES                       68
#define IMAGE_ZONE_BANK                  0
#define ADDR_ZONE_BANK            0x008010
#define ZONE_SIZE                 0x0000b0
#define ZONE_BANK_SIZE            (0x20+NUM_ZONES*ZONE_SIZE)

#define NUM_CONTACTS                  1024
#define IMAGE_CONTACTS                   1
#define ADDR_CONTACTS             0x087620
#define CONTACT_SIZE              0x000018

#define NUM_GROUP_LISTS                 76
#define IMAGE_GROUP_LIST_BANK            1
#define ADDR_GROUP_LIST_BANK      0x08d620


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
  return (Power)getUInt8(0x0019);
}
void
OpenGD77Codeplug::ChannelElement::setExtendedPower(Power power) {
  setUInt8(0x0019, (unsigned)(power));
}

bool
OpenGD77Codeplug::ChannelElement::squelchIsDefault() const {
  return 0 == getUInt8(0x0037);
}
unsigned
OpenGD77Codeplug::ChannelElement::squelch() const {
  return (getUInt8(0x0037)-1)/2;
}
void
OpenGD77Codeplug::ChannelElement::setSquelch(unsigned squelch) {
  setUInt8(0x0037, (squelch*2)+1);
}
void
OpenGD77Codeplug::ChannelElement::setSquelchDefault() {
  setUInt8(0x0037, 0);
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
  return getUInt24_be(0x0027);
}
void
OpenGD77Codeplug::ChannelElement::setRadioId(unsigned id) {
  setUInt24_be(0x0027, id);
  setBit(0x0026, 7);
}
void
OpenGD77Codeplug::ChannelElement::clearRadioId() {
  setUInt24_be(0x0027, 0);
  clearBit(0x0026, 7);
}

Channel *
OpenGD77Codeplug::ChannelElement::toChannelObj(Context &ctx) const {
  Channel *ch = GD77Codeplug::ChannelElement::toChannelObj(ctx);
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
OpenGD77Codeplug::ChannelElement::linkChannelObj(Channel *c, Context &ctx) const {
  if (! GD77Codeplug::ChannelElement::linkChannelObj(c, ctx))
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
OpenGD77Codeplug::ChannelElement::fromChannelObj(const Channel *c, Context &ctx) {
  if (! GD77Codeplug::ChannelElement::fromChannelObj(c, ctx))
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
  : RadioddityCodeplug::ZoneElement(ptr, ZONE_SIZE)
{
  // pass...
}

void
OpenGD77Codeplug::ZoneElement::clear() {
  RadioddityCodeplug::ZoneElement::clear();
  memset(_data+0x0010, 0x00, 0xa0);
}

bool
OpenGD77Codeplug::ZoneElement::linkZoneObj(Zone *zone, Context &ctx, bool putInB) const {
  if (! isValid()) {
    logWarn() << "Cannot link zone: Zone is invalid.";
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

void
OpenGD77Codeplug::ZoneElement::fromZoneObjA(const Zone *zone, Context &ctx) {
  if (zone->A()->count() && zone->B()->count())
    setName(zone->name() + " A");
  else
    setName(zone->name());

  for (int i=0; i<80; i++) {
    if (i < zone->A()->count())
      setMember(i, ctx.index(zone->A()->get(i)));
    else
      clearMember(i);
  }
}

void
OpenGD77Codeplug::ZoneElement::fromZoneObjB(const Zone *zone, Context &ctx) {
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
}


/* ******************************************************************************************** *
 * Implementation of OpenGD77Codeplug::ZoneBankElement
 * ******************************************************************************************** */
OpenGD77Codeplug::ZoneBankElement::ZoneBankElement(uint8_t *ptr)
  : RadioddityCodeplug::ZoneBankElement(ptr, ZONE_BANK_SIZE)
{
  // pass...
}

uint8_t *
OpenGD77Codeplug::ZoneBankElement::get(unsigned n) const {
  return (_data+0x20 + n*ZONE_SIZE);
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
  return TimeSlotOverride::None != (TimeSlotOverride)getUInt8(Offset::TimeSlotOverride);
}
DMRChannel::TimeSlot
OpenGD77Codeplug::ContactElement::timeSlot() const {
  switch ((TimeSlotOverride)getUInt8(Offset::TimeSlotOverride)) {
  case TimeSlotOverride::TS1: return DMRChannel::TimeSlot::TS1;
  case TimeSlotOverride::TS2: return DMRChannel::TimeSlot::TS2;
  default: break;
  }
  return DMRChannel::TimeSlot::TS1;
}
void
OpenGD77Codeplug::ContactElement::setTimeSlot(DMRChannel::TimeSlot ts) {
  if (DMRChannel::TimeSlot::TS1 == ts)
    setUInt8(Offset::TimeSlotOverride, (unsigned)TimeSlotOverride::TS1);
  else
    setUInt8(Offset::TimeSlotOverride, (unsigned)TimeSlotOverride::TS2);
}
void
OpenGD77Codeplug::ContactElement::disableTimeSlotOverride() {
  setUInt8(Offset::TimeSlotOverride, (unsigned)TimeSlotOverride::None);
}

DMRContact *
OpenGD77Codeplug::ContactElement::toContactObj(Context &ctx) const {
  DMRContact *c = GD77Codeplug::ContactElement::toContactObj(ctx);
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

void
OpenGD77Codeplug::ContactElement::fromContactObj(const DMRContact *c, Context &ctx) {
  GD77Codeplug::ContactElement::fromContactObj(c, ctx);

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

void
OpenGD77Codeplug::GroupListElement::fromRXGroupListObj(const RXGroupList *lst, Context &ctx, const ErrorStack& err) {
  Q_UNUSED(err)
  setName(lst->name());
  // Iterate over all 32 entries in the codeplug
  for (int i=0; i<32; i++) {
    if (lst->count() > i) {
      /*logDebug() << "Store member '" << lst->contact(i)->name()
                 << "' at index " << ctx.index(lst->contact(i))
                 << " in group list '" << lst->name() << "'.";*/
      setMember(i, ctx.index(lst->contact(i)));
    } else {
      // Clear entry.
      clearMember(i);
    }
  }
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
  image(0).addElement(0x000e0, 0x05f20);
  image(0).addElement(0x07500, 0x0bb00);

  addImage("OpenGD77 Codeplug FLASH");
  image(1).addElement(0x00000, 0x011a0);
  image(1).addElement(0x7b000, 0x13e60);
}

void
OpenGD77Codeplug::clearGeneralSettings() {
  GeneralSettingsElement(data(ADDR_SETTINGS, IMAGE_SETTINGS)).clear();
}

bool
OpenGD77Codeplug::encodeGeneralSettings(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)
  GeneralSettingsElement el(data(ADDR_SETTINGS, IMAGE_SETTINGS));
  if (! flags.updateCodePlug)
    el.clear();
  return el.fromConfig(config, ctx);
}

bool
OpenGD77Codeplug::decodeGeneralSettings(Config *config, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)
  return GeneralSettingsElement(data(ADDR_SETTINGS, IMAGE_SETTINGS)).updateConfig(config, ctx);
}

void
OpenGD77Codeplug::clearButtonSettings() {
  /// @bug Find button settings within OpenGD77 codeplug.
}

void
OpenGD77Codeplug::clearMessages() {
  MessageBankElement(data(ADDR_MESSAGE_BANK, IMAGE_MESSAGE_BANK)).clear();
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
  for (int i=0; i<NUM_CONTACTS; i++)
    ContactElement(data(ADDR_CONTACTS + i*CONTACT_SIZE, IMAGE_CONTACTS)).clear();
}

bool
OpenGD77Codeplug::encodeContacts(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)

  for (int i=0; i<NUM_CONTACTS; i++) {
    ContactElement el(data(ADDR_CONTACTS + i*CONTACT_SIZE, IMAGE_CONTACTS));
    el.clear();
    if (i >= config->contacts()->digitalCount())
      continue;
    el.fromContactObj(config->contacts()->digitalContact(i), ctx);
  }
  return true;
}

bool
OpenGD77Codeplug::createContacts(Config *config, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  /* Unpack Contacts */
  for (int i=0; i<NUM_CONTACTS; i++) {
    ContactElement el(data(ADDR_CONTACTS + i*CONTACT_SIZE, IMAGE_CONTACTS));
    if (! el.isValid())
      continue;
    DMRContact *cont = el.toContactObj(ctx);
    ctx.add(cont, i+1); config->contacts()->add(cont);
  }
  return true;
}

void
OpenGD77Codeplug::clearDTMFContacts() {
  for (int i=0; i<NUM_DTMF_CONTACTS; i++)
    DTMFContactElement(data(ADDR_DTMF_CONTACTS + i*DTMF_CONTACT_SIZE, IMAGE_DTMF_CONTACTS)).clear();
}

bool
OpenGD77Codeplug::encodeDTMFContacts(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)

  for (int i=0; i<NUM_DTMF_CONTACTS; i++) {
    DTMFContactElement el(data(ADDR_DTMF_CONTACTS + i*DTMF_CONTACT_SIZE, IMAGE_DTMF_CONTACTS));
    el.clear();
    if (i >= config->contacts()->dtmfCount())
      continue;
    el.fromContactObj(config->contacts()->dtmfContact(i), ctx);
  }
  return true;
}

bool
OpenGD77Codeplug::createDTMFContacts(Config *config, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  for (int i=0; i<NUM_DTMF_CONTACTS; i++) {
    DTMFContactElement el(data(ADDR_DTMF_CONTACTS+i*DTMF_CONTACT_SIZE, IMAGE_DTMF_CONTACTS));
    // If contact is disabled
    if (! el.isValid())
      continue;
    DTMFContact *cont = el.toContactObj(ctx);
    ctx.add(cont, i+1); config->contacts()->add(cont);
  }
  return true;
}

void
OpenGD77Codeplug::clearChannels() {
  for (int b=0,c=0; b<NUM_CHANNEL_BANKS; b++) {
    uint8_t *ptr = nullptr;
    if (0 == b) ptr = data(ADDR_CHANNEL_BANK_0, IMAGE_CHANNEL_BANK_0);
    else ptr = data(ADDR_CHANNEL_BANK_1 + (b-1)*CHANNEL_BANK_SIZE, IMAGE_CHANNEL_BANK_1);
    ChannelBankElement bank(ptr); bank.clear();
    for (int i=0; (i<NUM_CHANNELS_PER_BANK)&&(c<NUM_CHANNELS); i++, c++)
      ChannelElement(bank.get(i)).clear();
  }
}

bool
OpenGD77Codeplug::encodeChannels(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)

  for (int b=0,c=0; b<NUM_CHANNEL_BANKS; b++) {
    uint8_t *ptr = nullptr;
    if (0 == b) ptr = data(ADDR_CHANNEL_BANK_0, IMAGE_CHANNEL_BANK_0);
    else ptr = data(ADDR_CHANNEL_BANK_1 + (b-1)*CHANNEL_BANK_SIZE, IMAGE_CHANNEL_BANK_1);
    ChannelBankElement bank(ptr); bank.clear();
    for (int i=0; (i<NUM_CHANNELS_PER_BANK)&&(c<NUM_CHANNELS); i++, c++) {
      ChannelElement el(bank.get(i));
      if (c < config->channelList()->count()) {
        if (! el.fromChannelObj(config->channelList()->channel(c), ctx)) {
          logError() << "Cannot encode channel " << c << " (" << i << " of bank " << b <<").";
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

  for (int b=0,c=0; b<NUM_CHANNEL_BANKS; b++) {
    uint8_t *ptr = nullptr;
    if (0 == b) ptr = data(ADDR_CHANNEL_BANK_0, IMAGE_CHANNEL_BANK_0);
    else ptr = data(ADDR_CHANNEL_BANK_1 + (b-1)*CHANNEL_BANK_SIZE, IMAGE_CHANNEL_BANK_1);
    ChannelBankElement bank(ptr);
    for (int i=0; (i<NUM_CHANNELS_PER_BANK)&&(c<NUM_CHANNELS); i++, c++) {
      if (! bank.isEnabled(i))
        continue;
      Channel *ch = ChannelElement(bank.get(i)).toChannelObj(ctx);
      config->channelList()->add(ch); ctx.add(ch, c+1);
    }
  }
  return true;
}

bool
OpenGD77Codeplug::linkChannels(Config *config, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(config); Q_UNUSED(err)

  for (int b=0,c=0; b<NUM_CHANNEL_BANKS; b++) {
    uint8_t *ptr = nullptr;
    if (0 == b) ptr = data(ADDR_CHANNEL_BANK_0, IMAGE_CHANNEL_BANK_0);
    else ptr = data(ADDR_CHANNEL_BANK_1 + (b-1)*CHANNEL_BANK_SIZE, IMAGE_CHANNEL_BANK_1);
    ChannelBankElement bank(ptr);
    for (int i=0; (i<NUM_CHANNELS_PER_BANK)&&(c<NUM_CHANNELS); i++, c++) {
      if (! bank.isEnabled(i))
        continue;
      if (!ChannelElement(bank.get(i)).linkChannelObj(ctx.get<Channel>(c+1), ctx))
        return false;
    }
  }
  return true;
}

void
OpenGD77Codeplug::clearBootSettings() {
  BootSettingsElement(data(ADDR_BOOT_SETTINGS, IMAGE_BOOT_SETTINGS)).clear();
}

void
OpenGD77Codeplug::clearMenuSettings() {
  MenuSettingsElement(data(ADDR_MENU_SETTINGS, IMAGE_MENU_SETTINGS)).clear();
}

void
OpenGD77Codeplug::clearBootText() {
  BootTextElement(data(ADDR_BOOT_TEXT, IMAGE_BOOT_TEXT)).clear();
}

bool
OpenGD77Codeplug::encodeBootText(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(ctx); Q_UNUSED(err)

  BootTextElement(data(ADDR_BOOT_TEXT, IMAGE_BOOT_TEXT)).fromConfig(config);
  return true;
}

bool
OpenGD77Codeplug::decodeBootText(Config *config, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(err)

  BootTextElement(data(ADDR_BOOT_TEXT, IMAGE_BOOT_TEXT)).updateConfig(config);
  return true;
}

void
OpenGD77Codeplug::clearVFOSettings() {
  VFOChannelElement(data(ADDR_VFO_A, IMAGE_VFO_A)).clear();
  VFOChannelElement(data(ADDR_VFO_B, IMAGE_VFO_B)).clear();
}

void
OpenGD77Codeplug::clearZones() {
  ZoneBankElement bank(data(ADDR_ZONE_BANK, IMAGE_ZONE_BANK));
  bank.clear();
  for (int i=0; i<NUM_ZONES; i++)
    ZoneElement(bank.get(i)).clear();
}

bool
OpenGD77Codeplug::encodeZones(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)

  ZoneBankElement bank(data(ADDR_ZONE_BANK, IMAGE_ZONE_BANK));

  // Pack Zones
  bool pack_zone_a = true;
  for (int i=0, j=0; i<NUM_ZONES; i++) {
    ZoneElement z(bank.get(i));
next:
    if (j >= config->zones()->count()) {
      bank.enable(i, false);
      continue;
    }

    // Construct from Zone obj
    Zone *zone = config->zones()->zone(j);
    if (pack_zone_a) {
      pack_zone_a = false;
      if (zone->A()->count())
        z.fromZoneObjA(zone, ctx);
      else
        goto next;
    } else {
      pack_zone_a = true;
      j++;
      if (zone->B()->count())
        z.fromZoneObjB(zone, ctx);
      else
        goto next;
    }
    bank.enable(i, true);
  }
  return true;
}

bool
OpenGD77Codeplug::createZones(Config *config, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  QString last_zonename, last_zonebasename; Zone *last_zone = nullptr;
  bool extend_last_zone = false;
  ZoneBankElement bank(data(ADDR_ZONE_BANK, IMAGE_ZONE_BANK));

  for (int i=0; i<NUM_ZONES; i++) {
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
      last_zone = z.toZoneObj(ctx);
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
  ZoneBankElement bank(data(ADDR_ZONE_BANK, IMAGE_ZONE_BANK));

  for (int i=0; i<NUM_ZONES; i++) {
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
    if (! z.linkZoneObj(last_zone, ctx, extend_last_zone)) {
      errMsg(err) << "Cannot link zone at index " << i << ".";
      return false;
    }
  }

  return true;
}

void
OpenGD77Codeplug::clearGroupLists() {
  GroupListBankElement bank(data(ADDR_GROUP_LIST_BANK, IMAGE_GROUP_LIST_BANK)); bank.clear();
  for (int i=0; i<NUM_GROUP_LISTS; i++)
    GroupListElement(bank.get(i)).clear();
}

bool
OpenGD77Codeplug::encodeGroupLists(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags);

  GroupListBankElement bank(data(ADDR_GROUP_LIST_BANK, IMAGE_GROUP_LIST_BANK)); bank.clear();
  for (int i=0; i<NUM_GROUP_LISTS; i++) {
    GroupListElement el(bank.get(i));
    if (i < config->rxGroupLists()->count()) {
      el.fromRXGroupListObj(config->rxGroupLists()->list(i), ctx, err);
      bank.setContactCount(i, config->rxGroupLists()->list(i)->count());
    }
  }
  return true;
}

bool
OpenGD77Codeplug::createGroupLists(Config *config, Context &ctx, const ErrorStack &err) {
  GroupListBankElement bank(data(ADDR_GROUP_LIST_BANK, IMAGE_GROUP_LIST_BANK));
  for (int i=0; i<NUM_GROUP_LISTS; i++) {
    if (! bank.isEnabled(i))
      continue;
    GroupListElement el(bank.get(i));
    RXGroupList *list = el.toRXGroupListObj(ctx, err);
    config->rxGroupLists()->add(list); ctx.add(list, i+1);
  }
  return true;
}

bool
OpenGD77Codeplug::linkGroupLists(Config *config, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(config)

  GroupListBankElement bank(data(ADDR_GROUP_LIST_BANK, IMAGE_GROUP_LIST_BANK));
  for (int i=0; i<NUM_GROUP_LISTS; i++) {
    if (! bank.isEnabled(i))
      continue;
    GroupListElement el(bank.get(i));
    /*logDebug() << "Link " << bank.contactCount(i) << " members of group list '"
               << ctx.get<RXGroupList>(i+1)->name() << "'.";*/
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
