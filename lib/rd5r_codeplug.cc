#include "rd5r_codeplug.hh"
#include "config.hh"
#include "channel.hh"
#include "utils.hh"
#include "logger.hh"
#include <QDateTime>

#define ADDR_TIMESTMP             0x000088
#define ADDR_SETTINGS             0x0000e0
#define ADDR_BUTTONS              0x000108
#define ADDR_MESSAGES             0x000128

#define NUM_CONTACTS                   256
#define ADDR_CONTACTS             0x001788
#define CONTACT_SIZE              0x000018
#define NUM_DTMF_CONTACTS               32
#define ADDR_DTMF_CONTACTS        0x002f88
#define DTMF_CONTACT_SIZE         0x000020

#define NUM_CHANNELS                  1024
#define NUM_CHANNEL_BANKS                8
#define NUM_CHANNELS_PER_BANK          128
#define ADDR_CHANNEL_BANK_0       0x003780 // Channels 1-128
#define ADDR_CHANNEL_BANK_1       0x00b1b0 // Channels 129-1024
#define CHANNEL_SIZE              0x000038
#define CHANNEL_BANK_SIZE         0x001c10

#define ADDR_BOOTSETTINGS         0x007518
#define ADDR_MENU_SETTINGS        0x007538
#define ADDR_BOOT_TEXT            0x007540
#define ADDR_VFO_A                0x007590
#define ADDR_VFO_B                0x0075c8

#define NUM_ZONES                      250
#define ADDR_ZONE_BANK            0x008010
#define ZONE_SIZE                 0x000030

#define NUM_SCAN_LISTS                 250
#define ADDR_SCAN_LIST_BANK       0x017620
#define SCAN_LIST_SIZE            0x000058

#define ADDR_GROUP_LIST_BANK      0x01d620
#define NUM_GROUP_LISTS                 64


/* ******************************************************************************************** *
 * Implementation of RD5RCodeplug::ChannelElement
 * ******************************************************************************************** */
RD5RCodeplug::ChannelElement::ChannelElement(uint8_t *ptr, size_t size)
  : RadioddityCodeplug::ChannelElement(ptr, size)
{
  // pass...
}

RD5RCodeplug::ChannelElement::ChannelElement(uint8_t *ptr)
  : RadioddityCodeplug::ChannelElement(ptr)
{
  // pass...
}

void
RD5RCodeplug::ChannelElement::clear() {
  RadioddityCodeplug::ChannelElement::clear();
  setSquelch(0);
}

unsigned
RD5RCodeplug::ChannelElement::squelch() const {
  return std::min(getUInt8(0x0037), uint8_t(9))+1;
}
void
RD5RCodeplug::ChannelElement::setSquelch(unsigned level) {
  level = std::max(std::min(10u, level), 1u);
  setUInt8(0x0037, level-1);
}

bool
RD5RCodeplug::ChannelElement::fromChannelObj(const Channel *c, Context &ctx) {
  if (! RadioddityCodeplug::ChannelElement::fromChannelObj(c, ctx))
    return false;

  if (c->is<AnalogChannel>()) {
    const AnalogChannel *ac = c->as<AnalogChannel>();
    setSquelch(ac->squelch());
  }

  return true;
}

Channel *
RD5RCodeplug::ChannelElement::toChannelObj(Context &ctx) const {
  Channel *ch = RadioddityCodeplug::ChannelElement::toChannelObj(ctx);
  if (nullptr == ch)
    return nullptr;

  if (ch->is<AnalogChannel>()) {
    AnalogChannel *ac = ch->as<AnalogChannel>();
    ac->setSquelch(squelch());
  }

  return ch;
}


/* ********************************************************************************************* *
 * Implementation of RD5RCodeplug::TimestampElement
 * ********************************************************************************************* */
RD5RCodeplug::TimestampElement::TimestampElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

RD5RCodeplug::TimestampElement::TimestampElement(uint8_t *ptr)
  : Element(ptr, 0x0006)
{
  // pass...
}

RD5RCodeplug::TimestampElement::~TimestampElement() {
  // pass...
}

void
RD5RCodeplug::TimestampElement::clear() {
  set();
}

QDateTime
RD5RCodeplug::TimestampElement::get() const {
  return QDateTime(QDate(getBCD4_be(0x0000), getBCD2(0x0002), getBCD2(0x0003)),
                   QTime(getBCD2(0x0004), getBCD2(0x0005)));
}
void
RD5RCodeplug::TimestampElement::set(const QDateTime &ts) {
  setBCD4_be(0x0000, ts.date().year());
  setBCD2(0x0002, ts.date().month());
  setBCD2(0x0003, ts.date().day());
  setBCD2(0x0004, ts.time().hour());
  setBCD2(0x0005, ts.time().minute());
}


/* ******************************************************************************************** *
 * Implementation of RD5RCodeplug
 * ******************************************************************************************** */
RD5RCodeplug::RD5RCodeplug(QObject *parent)
  : RadioddityCodeplug(parent)
{
  addImage("Radioddity RD5R Codeplug");
  image(0).addElement(0x00080, 0x07b80);
  image(0).addElement(0x08000, 0x16300);
}

void
RD5RCodeplug::clear() {
  RadioddityCodeplug::clear();
  this->clearTimestamp();
}

bool
RD5RCodeplug::encodeElements(const Flags &flags, Context &ctx) {
  if (! RadioddityCodeplug::encodeElements(flags, ctx))
    return false;

  // Set timestamp
  if (! this->encodeTimestamp()) {
    errMsg() << "Cannot encode time-stamp.";
    return false;
  }

  return true;
}

bool
RD5RCodeplug::decodeElements(Context &ctx) {
  if (! RadioddityCodeplug::decodeElements(ctx))
    return false;
  return true;
}

void
RD5RCodeplug::clearTimestamp() {
  encodeTimestamp();
}

bool
RD5RCodeplug::encodeTimestamp() {
  TimestampElement(data(ADDR_TIMESTMP)).set();
  return true;
}

void
RD5RCodeplug::clearGeneralSettings() {
  TimestampElement(data(ADDR_SETTINGS)).clear();
}

bool
RD5RCodeplug::encodeGeneralSettings(Config *config, const Flags &flags, Context &ctx) {
  GeneralSettingsElement el(data(ADDR_SETTINGS));
  if (! flags.updateCodePlug)
    el.clear();
  return el.fromConfig(config, ctx);
}

bool
RD5RCodeplug::decodeGeneralSettings(Config *config, Context &ctx) {
  return GeneralSettingsElement(data(ADDR_SETTINGS)).updateConfig(config, ctx);
}

void
RD5RCodeplug::clearButtonSettings() {
  ButtonSettingsElement(data(ADDR_BUTTONS)).clear();
}

void
RD5RCodeplug::clearMessages() {
  MessageBankElement(data(ADDR_MESSAGES)).clear();
}

void
RD5RCodeplug::clearContacts() {
  for (int i=0; i<NUM_CONTACTS; i++)
    ContactElement(data(ADDR_CONTACTS + i*CONTACT_SIZE)).clear();
}

bool
RD5RCodeplug::encodeContacts(Config *config, const Flags &flags, Context &ctx) {
  Q_UNUSED(flags)
  for (int i=0; i<NUM_CONTACTS; i++) {
    ContactElement el(data(ADDR_CONTACTS + i*CONTACT_SIZE));
    el.clear();
    if (i >= config->contacts()->digitalCount())
      continue;
    el.fromContactObj(config->contacts()->digitalContact(i), ctx);
  }
  return true;
}

bool
RD5RCodeplug::createContacts(Config *config, Context &ctx) {
  /* Unpack Contacts */
  for (int i=0; i<NUM_CONTACTS; i++) {
    ContactElement el(data(ADDR_CONTACTS + i*CONTACT_SIZE));
    if (!el.isValid())
      continue;

    DigitalContact *cont = el.toContactObj(ctx);
    ctx.add(cont, i+1); config->contacts()->add(cont);
  }
  return true;
}

void
RD5RCodeplug::clearDTMFContacts() {
  for (int i=0; i<NUM_DTMF_CONTACTS; i++)
    DTMFContactElement(data(ADDR_DTMF_CONTACTS + i*DTMF_CONTACT_SIZE)).clear();
}

bool
RD5RCodeplug::encodeDTMFContacts(Config *config, const Flags &flags, Context &ctx) {
  Q_UNUSED(flags)
  for (int i=0; i<NUM_DTMF_CONTACTS; i++) {
    DTMFContactElement el(data(ADDR_DTMF_CONTACTS + i*DTMF_CONTACT_SIZE));
    el.clear();
    if (i >= config->contacts()->dtmfCount())
      continue;
    el.fromContactObj(config->contacts()->dtmfContact(i), ctx);
  }
  return true;
}

bool
RD5RCodeplug::createDTMFContacts(Config *config, Context &ctx) {
  for (int i=0; i<NUM_DTMF_CONTACTS; i++) {
    DTMFContactElement el(data(ADDR_DTMF_CONTACTS+i*DTMF_CONTACT_SIZE));
    // If contact is disabled
    if (! el.isValid())
      continue;
    DTMFContact *cont = el.toContactObj(ctx);
    ctx.add(cont, i+1); config->contacts()->add(cont);
  }
  return true;
}

void
RD5RCodeplug::clearChannels() {
  for (int b=0,c=0; b<NUM_CHANNEL_BANKS; b++) {
    uint8_t *ptr = nullptr;
    if (0 == b) ptr = data(ADDR_CHANNEL_BANK_0);
    else ptr = data(ADDR_CHANNEL_BANK_1 + (b-1)*CHANNEL_BANK_SIZE);
    ChannelBankElement bank(ptr); bank.clear();
    for (int i=0; (i<NUM_CHANNELS_PER_BANK)&&(c<NUM_CHANNELS); i++, c++)
      ChannelElement(bank.get(i)).clear();
  }
}

bool
RD5RCodeplug::encodeChannels(Config *config, const Flags &flags, Context &ctx) {
  Q_UNUSED(flags)
  for (int b=0,c=0; b<NUM_CHANNEL_BANKS; b++) {
    uint8_t *ptr = nullptr;
    if (0 == b) ptr = data(ADDR_CHANNEL_BANK_0);
    else ptr = data(ADDR_CHANNEL_BANK_1 + (b-1)*CHANNEL_BANK_SIZE);
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
RD5RCodeplug::createChannels(Config *config, Context &ctx) {
  for (int b=0,c=0; b<NUM_CHANNEL_BANKS; b++) {
    uint8_t *ptr = nullptr;
    if (0 == b) ptr = data(ADDR_CHANNEL_BANK_0);
    else ptr = data(ADDR_CHANNEL_BANK_1 + (b-1)*CHANNEL_BANK_SIZE);
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
RD5RCodeplug::linkChannels(Config *config, Context &ctx) {
  Q_UNUSED(config)
  for (int b=0,c=0; b<NUM_CHANNEL_BANKS; b++) {
    uint8_t *ptr = nullptr;
    if (0 == b) ptr = data(ADDR_CHANNEL_BANK_0);
    else ptr = data(ADDR_CHANNEL_BANK_1 + (b-1)*CHANNEL_BANK_SIZE);
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
RD5RCodeplug::clearBootSettings() {
  BootSettingsElement(data(ADDR_BOOTSETTINGS)).clear();
}

void
RD5RCodeplug::clearMenuSettings() {
  MenuSettingsElement(data(ADDR_MENU_SETTINGS)).clear();
}

void
RD5RCodeplug::clearBootText() {
  BootTextElement(data(ADDR_BOOT_TEXT)).clear();
}

bool
RD5RCodeplug::encodeBootText(Config *config, const Flags &flags, Context &ctx) {
  Q_UNUSED(flags)
  Q_UNUSED(ctx)
  BootTextElement(data(ADDR_BOOT_TEXT)).fromConfig(config);
  return true;
}

bool
RD5RCodeplug::decodeBootText(Config *config, Context &ctx) {
  Q_UNUSED(ctx)
  BootTextElement(data(ADDR_BOOT_TEXT)).updateConfig(config);
  return true;
}

void
RD5RCodeplug::clearVFOSettings() {
  ChannelElement(data(ADDR_VFO_A)).clear();
  ChannelElement(data(ADDR_VFO_B)).clear();
}

void
RD5RCodeplug::clearZones() {
  ZoneBankElement bank(data(ADDR_ZONE_BANK));
  bank.clear();
  for (int i=0; i<NUM_ZONES; i++)
    ZoneElement(bank.get(i)).clear();
}

bool
RD5RCodeplug::encodeZones(Config *config, const Flags &flags, Context &ctx) {
  Q_UNUSED(flags)

  ZoneBankElement bank(data(ADDR_ZONE_BANK));

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
RD5RCodeplug::createZones(Config *config, Context &ctx) {
  QString last_zonename, last_zonebasename; Zone *last_zone = nullptr;
  bool extend_last_zone = false;
  ZoneBankElement bank(data(ADDR_ZONE_BANK));

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
RD5RCodeplug::linkZones(Config *config, Context &ctx) {
  Q_UNUSED(config)

  QString last_zonename, last_zonebasename; Zone *last_zone = nullptr;
  bool extend_last_zone = false;
  ZoneBankElement bank(data(ADDR_ZONE_BANK));

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
      errMsg() << "Cannot link zone at index " << i << ".";
      return false;
    }
  }

  return true;
}

void
RD5RCodeplug::clearScanLists() {
  ScanListBankElement bank(data(ADDR_SCAN_LIST_BANK)); bank.clear();
  for (int i=0; i<NUM_SCAN_LISTS; i++)
    ScanListElement(bank.get(i)).clear();
}

bool
RD5RCodeplug::encodeScanLists(Config *config, const Flags &flags, Context &ctx) {
  Q_UNUSED(flags)

  ScanListBankElement bank(data(ADDR_SCAN_LIST_BANK));
  for (int i=0; i<NUM_SCAN_LISTS; i++) {
    if (i >= config->scanlists()->count()) {
      bank.enable(i, false); continue;
    }
    ScanListElement(bank.get(i)).fromScanListObj(config->scanlists()->scanlist(i), ctx);
    bank.enable(i, true);
  }
  return true;
}

bool
RD5RCodeplug::createScanLists(Config *config, Context &ctx) {
  ScanListBankElement bank(data(ADDR_SCAN_LIST_BANK));
  for (int i=0; i<NUM_SCAN_LISTS; i++) {
    if (! bank.isEnabled(i))
      continue;
    ScanListElement el(bank.get(i));
    ScanList *scan = el.toScanListObj(ctx);
    config->scanlists()->add(scan); ctx.add(scan, i+1);
  }
  return true;
}

bool
RD5RCodeplug::linkScanLists(Config *config, Context &ctx) {
  Q_UNUSED(config)

  ScanListBankElement bank(data(ADDR_SCAN_LIST_BANK));
  for (int i=0; i<NUM_SCAN_LISTS; i++) {
    if (! bank.isEnabled(i))
      continue;
    if (! ScanListElement(bank.get(i)).linkScanListObj(ctx.get<ScanList>(i+1), ctx))
      return false;
  }
  return true;
}

void
RD5RCodeplug::clearGroupLists() {
  GroupListBankElement bank(data(ADDR_GROUP_LIST_BANK)); bank.clear();
  for (int i=0; i<NUM_GROUP_LISTS; i++)
    GroupListElement(bank.get(i)).clear();
}

bool
RD5RCodeplug::encodeGroupLists(Config *config, const Flags &flags, Context &ctx) {
  Q_UNUSED(flags)

  GroupListBankElement bank(data(ADDR_GROUP_LIST_BANK)); bank.clear();
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
RD5RCodeplug::createGroupLists(Config *config, Context &ctx) {
  GroupListBankElement bank(data(ADDR_GROUP_LIST_BANK));
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
RD5RCodeplug::linkGroupLists(Config *config, Context &ctx) {
  Q_UNUSED(config)

  GroupListBankElement bank(data(ADDR_GROUP_LIST_BANK));
  for (int i=0; i<NUM_GROUP_LISTS; i++) {
    if (! bank.isEnabled(i))
      continue;
    GroupListElement el(bank.get(i));
    /*logDebug() << "Link " << bank.contactCount(i) << " members of group list '"
               << ctx.get<RXGroupList>(i+1)->name() << "'.";*/
    if (! el.linkRXGroupListObj(bank.contactCount(i), ctx.get<RXGroupList>(i+1), ctx)) {
      errMsg() << "Cannot link group list '" << ctx.get<RXGroupList>(i+1)->name() << "'.";
      return false;
    }
  }
  return true;
}
