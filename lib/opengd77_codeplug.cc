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

#define NUM_CHANNEL_BANKS                8
#define NUM_CHANNELS_PER_BANK          128
#define NUM_CHANNELS                  1024
#define IMAGE_CHANNEL_BANK_0             0
#define ADDR_CHANNEL_BANK_0       0x003780 // Channels 1-128
#define IMAGE_CHANNEL_BANK_1             1
#define ADDR_CHANNEL_BANK_1       0x07b1b0 // Channels 129-1024
#define CHANNEL_SIZE              0x000038
#define CHANNEL_BANK_SIZE         0x001c10

#define IMAGE_BOOT_TEXT                  0
#define ADDR_BOOT_TEXT            0x007540

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
  setExtendedPower(Power::Global);
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
  setUInt8(0x0019, (uint)(power));
}

Channel *
OpenGD77Codeplug::ChannelElement::toChannelObj(Context &ctx) const {
  Channel *ch = GD77Codeplug::ChannelElement::toChannelObj(ctx);
  if (nullptr == ch)
    return nullptr;

  OpenGD77ChannelExtension *ext = new OpenGD77ChannelExtension(ch);
  ext->setPower(extendedPower());

  ch->addExtension("openGD77", ext);
  return ch;
}

bool
OpenGD77Codeplug::ChannelElement::fromChannelObj(const Channel *c, Context &ctx) {
  if (! GD77Codeplug::ChannelElement::fromChannelObj(c, ctx))
    return false;
  if (! c->hasExtension("openGD77"))
    return true;

  const OpenGD77ChannelExtension *ext = c->extension("openGD77")->as<OpenGD77ChannelExtension>();
  setExtendedPower(ext->power());

  return true;
}


/* ******************************************************************************************** *
 * Implementation of OpenGD77Codeplug::ZoneElement
 * ******************************************************************************************** */
OpenGD77Codeplug::ZoneElement::ZoneElement(uint8_t *ptr)
  : RadioddityCodeplug::ZoneElement(ptr, ZONE_SIZE)
{
  // pass...
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
OpenGD77Codeplug::ZoneBankElement::get(uint n) const {
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
OpenGD77Codeplug::ContactElement::overridesTimeSlot() const {
  return TimeSlotOverride::None != (TimeSlotOverride)getUInt8(0x0017);
}
DigitalChannel::TimeSlot
OpenGD77Codeplug::ContactElement::timeSlot() const {
  switch ((TimeSlotOverride)getUInt8(0x0017)) {
  case TimeSlotOverride::TS1: return DigitalChannel::TimeSlot::TS1;
  case TimeSlotOverride::TS2: return DigitalChannel::TimeSlot::TS2;
  default: break;
  }
  return DigitalChannel::TimeSlot::TS1;
}
void
OpenGD77Codeplug::ContactElement::setTimeSlot(DigitalChannel::TimeSlot ts) {
  if (DigitalChannel::TimeSlot::TS1 == ts)
    setUInt8(0x0017, (uint)TimeSlotOverride::TS1);
  else
    setUInt8(0x0017, (uint)TimeSlotOverride::TS2);
}
void
OpenGD77Codeplug::ContactElement::disableTimeSlotOverride() {
  setUInt8(0x0017, (uint)TimeSlotOverride::None);
}

DigitalContact *
OpenGD77Codeplug::ContactElement::toContactObj(Context &ctx) const {
  DigitalContact *c = GD77Codeplug::ContactElement::toContactObj(ctx);
  if (nullptr == c)
    return nullptr;

  OpenGD77ContactExtension *ext = new OpenGD77ContactExtension(c);
  if (overridesTimeSlot()) {
    if (DigitalChannel::TimeSlot::TS1 == timeSlot())
      ext->setTimeSlotOverride(OpenGD77ContactExtension::TimeSlotOverride::TS1);
    else
      ext->setTimeSlotOverride(OpenGD77ContactExtension::TimeSlotOverride::TS2);
  } else {
    ext->setTimeSlotOverride(OpenGD77ContactExtension::TimeSlotOverride::None);
  }

  c->addExtension("openGD77", ext);
  return c;
}

void
OpenGD77Codeplug::ContactElement::fromContactObj(const DigitalContact *c, Context &ctx) {
  GD77Codeplug::ContactElement::fromContactObj(c, ctx);
  if (! c->hasExtension("openGD77"))
    return;

  const OpenGD77ContactExtension *ext = c->extension("openGD77")->as<OpenGD77ContactExtension>();
  if (OpenGD77ContactExtension::TimeSlotOverride::None != ext->timeSlotOverride()) {
    if (OpenGD77ContactExtension::TimeSlotOverride::TS1 == ext->timeSlotOverride())
      setTimeSlot(DigitalChannel::TimeSlot::TS1);
    else
      setTimeSlot(DigitalChannel::TimeSlot::TS2);
  } else {
    disableTimeSlotOverride();
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
OpenGD77Codeplug::encodeGeneralSettings(Config *config, const Flags &flags, Context &ctx) {
  GeneralSettingsElement el(data(ADDR_SETTINGS, IMAGE_SETTINGS));
  if (! flags.updateCodePlug)
    el.clear();
  return el.fromConfig(config, ctx);
}

bool
OpenGD77Codeplug::decodeGeneralSettings(Config *config, Context &ctx) {
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
OpenGD77Codeplug::encodeScanLists(Config *config, const Flags &flags, Context &ctx) {
  // Scan lists are not touched with OpenGD77 codeplug
  return true;
}
bool
OpenGD77Codeplug::createScanLists(Config *config, Context &ctx) {
  // Scan lists are not touched with OpenGD77 codeplug
  return true;
}
bool
OpenGD77Codeplug::linkScanLists(Config *config, Context &ctx) {
  // Scan lists are not touched with OpenGD77 codeplug
  return true;
}

void
OpenGD77Codeplug::clearContacts() {
  for (int i=0; i<NUM_CONTACTS; i++)
    ContactElement(data(ADDR_CONTACTS + i*CONTACT_SIZE, IMAGE_CONTACTS)).clear();
}

bool
OpenGD77Codeplug::encodeContacts(Config *config, const Flags &flags, Context &ctx) {
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
OpenGD77Codeplug::createContacts(Config *config, Context &ctx) {
  /* Unpack Contacts */
  for (int i=0; i<NUM_CONTACTS; i++) {
    ContactElement el(data(ADDR_CONTACTS + i*CONTACT_SIZE, IMAGE_CONTACTS));
    if (!el.isValid())
      continue;
    DigitalContact *cont = el.toContactObj(ctx);
    ctx.add(cont, i+1); config->contacts()->add(cont);
  }
  return true;
}

void
OpenGD77Codeplug::clearDTMFContacts() {
  /// @bug Find DTMF contacts in OpenGD77 codeplug.
}
bool
OpenGD77Codeplug::encodeDTMFContacts(Config *config, const Flags &flags, Context &ctx) {
  /// @bug Find DTMF contacts in OpenGD77 codeplug.
  return true;
}
bool
OpenGD77Codeplug::createDTMFContacts(Config *config, Context &ctx) {
  /// @bug Find DTMF contacts in OpenGD77 codeplug.
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
OpenGD77Codeplug::encodeChannels(Config *config, const Flags &flags, Context &ctx) {
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
OpenGD77Codeplug::createChannels(Config *config, Context &ctx) {
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
OpenGD77Codeplug::linkChannels(Config *config, Context &ctx) {
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
  /// @bug Find BootSettings in OpenGD77 codeplug.
}

void
OpenGD77Codeplug::clearMenuSettings() {
  /// @bug Find MenuSettings in OpenGD77 codeplug.
}

void
OpenGD77Codeplug::clearBootText() {
  BootTextElement(data(ADDR_BOOT_TEXT, IMAGE_BOOT_TEXT)).clear();
}

bool
OpenGD77Codeplug::encodeBootText(Config *config, const Flags &flags, Context &ctx) {
  BootTextElement(data(ADDR_BOOT_TEXT, IMAGE_BOOT_TEXT)).fromConfig(config);
  return true;
}

bool
OpenGD77Codeplug::decodeBootText(Config *config, Context &ctx) {
  BootTextElement(data(ADDR_BOOT_TEXT, IMAGE_BOOT_TEXT)).updateConfig(config);
  return true;
}

void
OpenGD77Codeplug::clearVFOSettings() {
  /// @bug Search for VFO channels in OpenGD77 codeplug!
}

void
OpenGD77Codeplug::clearZones() {
  ZoneBankElement bank(data(ADDR_ZONE_BANK, IMAGE_ZONE_BANK));
  bank.clear();
  for (int i=0; i<NUM_ZONES; i++)
    ZoneElement(bank.get(i)).clear();
}

bool
OpenGD77Codeplug::encodeZones(Config *config, const Flags &flags, Context &ctx) {
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
OpenGD77Codeplug::createZones(Config *config, Context &ctx) {
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
OpenGD77Codeplug::linkZones(Config *config, Context &ctx) {
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
      _errorMessage = QString("%1(): Cannot unpack codeplug: Cannot link zone at index %2")
          .arg(__func__).arg(i);
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
OpenGD77Codeplug::encodeGroupLists(Config *config, const Flags &flags, Context &ctx) {
  GroupListBankElement bank(data(ADDR_GROUP_LIST_BANK, IMAGE_GROUP_LIST_BANK)); bank.clear();
  for (int i=0; i<NUM_GROUP_LISTS; i++) {
    if (i >= config->rxGroupLists()->count())
      continue;
    GroupListElement el(bank.get(i));
    el.fromRXGroupListObj(config->rxGroupLists()->list(i), ctx);
    bank.setContactCount(i, config->rxGroupLists()->list(i)->count());
  }
  return true;
}

bool
OpenGD77Codeplug::createGroupLists(Config *config, Context &ctx) {
  GroupListBankElement bank(data(ADDR_GROUP_LIST_BANK, IMAGE_GROUP_LIST_BANK));
  for (int i=0; i<NUM_GROUP_LISTS; i++) {
    if (! bank.isEnabled(i))
      continue;
    GroupListElement el(bank.get(i));
    RXGroupList *list = el.toRXGroupListObj(ctx);
    config->rxGroupLists()->add(list); ctx.add(list, i+1);
  }
  return true;
}

bool
OpenGD77Codeplug::linkGroupLists(Config *config, Context &ctx) {
  GroupListBankElement bank(data(ADDR_GROUP_LIST_BANK, IMAGE_GROUP_LIST_BANK));
  for (int i=0; i<NUM_GROUP_LISTS; i++) {
    if (! bank.isEnabled(i))
      continue;
    GroupListElement el(bank.get(i));
    /*logDebug() << "Link " << bank.contactCount(i) << " members of group list '"
               << ctx.get<RXGroupList>(i+1)->name() << "'.";*/
    if (! el.linkRXGroupListObj(bank.contactCount(i), ctx.get<RXGroupList>(i+1), ctx)) {
      _errorMessage = tr("Cannot link group list '%1'.").arg(ctx.get<RXGroupList>(i+1)->name());
      return false;
    }
  }
  return true;
}
