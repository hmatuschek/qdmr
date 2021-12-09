#include "dm1701_codeplug.hh"
#include "codeplugcontext.hh"
#include "logger.hh"

#define NUM_CHANNELS                3000
#define ADDR_CHANNELS           0x110000
#define CHANNEL_SIZE            0x000040

#define NUM_CONTACTS               10000
#define ADDR_CONTACTS           0x140000
#define CONTACT_SIZE            0x000024

#define NUM_ZONES                    250
#define ADDR_ZONES              0x0149e0
#define ZONE_SIZE               0x000040
#define ADDR_ZONEEXTS           0x031000
#define ZONEEXT_SIZE            0x0000e0

#define NUM_GROUPLISTS               250
#define ADDR_GROUPLISTS         0x00ec20
#define GROUPLIST_SIZE          0x000060

#define NUM_SCANLISTS                250
#define ADDR_SCANLISTS          0x018860
#define SCANLIST_SIZE           0x000068

#define ADDR_TIMESTAMP          0x002000
#define ADDR_SETTINGS           0x002040
#define ADDR_BOOTSETTINGS       0x02f000
#define ADDR_MENUSETTINGS       0x0020f0
#define ADDR_BUTTONSETTINGS     0x002100
#define ADDR_PRIVACY_KEYS       0x0059c0

#define NUM_GPSSYSTEMS                16
#define ADDR_GPSSYSTEMS         0x03ec40
#define GPSSYSTEM_SIZE          0x000010

#define NUM_TEXTMESSAGES              50
#define ADDR_TEXTMESSAGES       0x002180
#define TEXTMESSAGE_SIZE        0x000120

#define ADDR_EMERGENCY_SETTINGS 0x005a50
#define NUM_EMERGENCY_SYSTEMS         32
#define ADDR_EMERGENCY_SYSTEMS  0x005a60
#define EMERGENCY_SYSTEM_SIZE   0x000028

#define ADDR_VFO_CHANNEL_A      0x02ef00
#define ADDR_VFO_CHANNEL_B      0x02ef40



/* ********************************************************************************************* *
 * Implementation of DM1701Codeplug::ChannelElement
 * ********************************************************************************************* */
DM1701Codeplug::ChannelElement::ChannelElement(uint8_t *ptr, size_t size)
  : TyTCodeplug::ChannelElement::ChannelElement(ptr, size)
{
  // pass...
}

DM1701Codeplug::ChannelElement::ChannelElement(uint8_t *ptr)
  : TyTCodeplug::ChannelElement(ptr, CHANNEL_SIZE)
{
  // pass...
}

void
DM1701Codeplug::ChannelElement::clear() {
  TyTCodeplug::ChannelElement::clear();

  enableTightSquelch(false);
  enableReverseBurst(true);
  setPower(Channel::Power::High);
  setBit(0x0003, 6, true);
  setUInt8(0x0005, 0xc3);
  setUInt8(0x000f, 0xff);
}

bool
DM1701Codeplug::ChannelElement::tightSquelchEnabled() const {
  return !getBit(0x0000, 5);
}
void
DM1701Codeplug::ChannelElement::enableTightSquelch(bool enable) {
  setBit(0x0000, 5, !enable);
}

bool
DM1701Codeplug::ChannelElement::reverseBurst() const {
  return getBit(0x0004, 2);
}
void
DM1701Codeplug::ChannelElement::enableReverseBurst(bool enable) {
  setBit(0x0004, 2, enable);
}

Channel::Power
DM1701Codeplug::ChannelElement::power() const {
  if (getBit(0x0004, 5))
    return Channel::Power::High;
  return Channel::Power::Low;
}
void
DM1701Codeplug::ChannelElement::setPower(Channel::Power pwr) {
  switch (pwr) {
  case Channel::Power::Min:
  case Channel::Power::Low:
  case Channel::Power::Mid:
    setBit(0x0004, 5, false);
    break;
  case Channel::Power::High:
  case Channel::Power::Max:
    setBit(0x0004, 5, true);
  }
}

Channel *
DM1701Codeplug::ChannelElement::toChannelObj() const {
  Channel *ch = TyTCodeplug::ChannelElement::toChannelObj();
  if (nullptr == ch)
    return ch;

  ch->setPower(power());

  // Apply extension
  if (ch->tytChannelExtension()) {
    ch->tytChannelExtension()->enableTightSquelch(tightSquelchEnabled());
    ch->tytChannelExtension()->enableReverseBurst(reverseBurst());
  }
  return ch;
}

void
DM1701Codeplug::ChannelElement::fromChannelObj(const Channel *c, Context &ctx) {
  TyTCodeplug::ChannelElement::fromChannelObj(c, ctx);

  setPower(c->power());

  // apply extensions (extension will be created in TyTCodeplug::ChannelElement::fromChannelObj)
  if (TyTChannelExtension *ex = c->tytChannelExtension()) {
    enableTightSquelch(ex->tightSquelch());
    enableReverseBurst(ex->reverseBurst());
  }
}



/* ********************************************************************************************* *
 * Implementation of DM1701Codeplug
 * ********************************************************************************************* */
DM1701Codeplug::DM1701Codeplug(QObject *parent)
  : TyTCodeplug(parent)
{
  addImage("Baofeng DM-1701 Codeplug");
  image(0).addElement(0x002000, 0x3e000);
  image(0).addElement(0x110000, 0x90000);

  // Clear entire codeplug
  clear();
}

DM1701Codeplug::~DM1701Codeplug() {
  // pass...
}

void
DM1701Codeplug::clearTimestamp() {
  TimestampElement(data(ADDR_TIMESTAMP)).clear();
}

bool
DM1701Codeplug::encodeTimestamp() {
  TimestampElement ts(data(ADDR_TIMESTAMP));
  ts.setTimestamp(QDateTime::currentDateTime());
  return true;
}

void
DM1701Codeplug::clearGeneralSettings() {
  GeneralSettingsElement(data(ADDR_SETTINGS)).clear();
}

bool
DM1701Codeplug::encodeGeneralSettings(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(ctx); Q_UNUSED(err)
  return GeneralSettingsElement(data(ADDR_SETTINGS)).fromConfig(config);
}

bool
DM1701Codeplug::decodeGeneralSettings(Config *config, const ErrorStack &err) {
  Q_UNUSED(err)
  return GeneralSettingsElement(data(ADDR_SETTINGS)).updateConfig(config);
}

void
DM1701Codeplug::clearChannels() {
  // Clear channels
  for (int i=0; i<NUM_CHANNELS; i++)
    ChannelElement(data(ADDR_CHANNELS+i*CHANNEL_SIZE)).clear();
}

bool
DM1701Codeplug::encodeChannels(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)
  // Define Channels
  for (int i=0; i<NUM_CHANNELS; i++) {
    ChannelElement chan(data(ADDR_CHANNELS+i*CHANNEL_SIZE));
    if (i < config->channelList()->count()) {
      chan.fromChannelObj(config->channelList()->channel(i), ctx);
    } else {
      chan.clear();
    }
  }
  return true;
}

bool
DM1701Codeplug::createChannels(Config *config, Context &ctx, const ErrorStack &err) {
  for (int i=0; i<NUM_CHANNELS; i++) {
    ChannelElement chan(data(ADDR_CHANNELS+i*CHANNEL_SIZE));
    if (! chan.isValid())
      break;
    if (Channel *obj = chan.toChannelObj()) {
      config->channelList()->add(obj); ctx.add(obj, i+1);
    } else {
      errMsg(err) << "Invlaid channel at index %" << i << ".";
      return false;
    }
  }
  return true;
}

bool
DM1701Codeplug::linkChannels(Context &ctx, const ErrorStack &err) {
  for (int i=0; i<NUM_CHANNELS; i++) {
    ChannelElement chan(data(ADDR_CHANNELS+i*CHANNEL_SIZE));
    if (! chan.isValid())
      break;
    if (! chan.linkChannelObj(ctx.get<Channel>(i+1), ctx)) {
      errMsg(err) << "Cannot link channel at index " << i << ".";
      return false;
    }
  }
  return true;
}

void
DM1701Codeplug::clearContacts() {
  // Clear contacts
  for (int i=0; i<NUM_CONTACTS; i++)
    ContactElement(data(ADDR_CONTACTS+i*CONTACT_SIZE)).clear();
}

bool
DM1701Codeplug::encodeContacts(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(ctx); Q_UNUSED(err)
  // Encode contacts
  for (int i=0; i<NUM_CONTACTS; i++) {
    ContactElement cont(data(ADDR_CONTACTS+i*CONTACT_SIZE));
    if (i < config->contacts()->digitalCount())
      cont.fromContactObj(config->contacts()->digitalContact(i));
    else
      cont.clear();
  }
  return true;
}

bool
DM1701Codeplug::createContacts(Config *config, Context &ctx, const ErrorStack &err) {
  for (int i=0; i<NUM_CONTACTS; i++) {
    ContactElement cont(data(ADDR_CONTACTS+i*CONTACT_SIZE));
    if (! cont.isValid())
      break;
    if (DigitalContact *obj = cont.toContactObj()) {
      config->contacts()->add(obj); ctx.add(obj, i+1);
    } else {
      errMsg(err) << "Invlaid contact at index " << i << ".";
      return false;
    }
  }
  return true;
}

void
DM1701Codeplug::clearZones() {
  // Clear zones & zone extensions
  for (int i=0; i<NUM_ZONES; i++) {
    ZoneElement(data(ADDR_ZONES+i*ZONE_SIZE)).clear();
    ZoneExtElement(data(ADDR_ZONEEXTS+i*ZONEEXT_SIZE)).clear();
  }
}

bool
DM1701Codeplug::encodeZones(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)
  for (int i=0; i<NUM_ZONES; i++) {
    ZoneElement zone(data(ADDR_ZONES + i*ZONE_SIZE));
    ZoneExtElement ext(data(ADDR_ZONEEXTS + i*ZONEEXT_SIZE));
    zone.clear();
    ext.clear();
    if (i < config->zones()->count()) {
      zone.fromZoneObj(config->zones()->zone(i), ctx);
      if (config->zones()->zone(i)->B()->count() || (16 < config->zones()->zone(i)->A()->count()))
        ext.fromZoneObj(config->zones()->zone(i), ctx);
    }
  }
  return true;
}

bool
DM1701Codeplug::createZones(Config *config, Context &ctx, const ErrorStack &err) {
  for (int i=0; i<NUM_ZONES; i++) {
    ZoneElement zone(data(ADDR_ZONES+i*ZONE_SIZE));
    if (! zone.isValid())
      break;
    if (Zone *obj = zone.toZoneObj()) {
      config->zones()->add(obj); ctx.add(obj, i+1);
    } else {
      errMsg(err) << "Invlaid zone at index " << i << ".";
      return false;
    }
  }

  return true;
}

bool
DM1701Codeplug::linkZones(Context &ctx, const ErrorStack &err) {
  for (int i=0; i<NUM_ZONES; i++) {
    ZoneElement zone(data(ADDR_ZONES+i*ZONE_SIZE));
    if (! zone.isValid())
      break;
    if (! zone.linkZone(ctx.get<Zone>(i+1), ctx)) {
      errMsg(err) << "Cannot link zone at index " << i << ".";
      return false;
    }
    ZoneExtElement zoneext(data(ADDR_ZONEEXTS + i*ZONEEXT_SIZE));
    if (! zoneext.linkZoneObj(ctx.get<Zone>(i+1), ctx)) {
      errMsg(err) << "Cannot link zone extension at index " << i << ".";
      return false;
    }
  }

  return true;
}

void
DM1701Codeplug::clearGroupLists() {
  for (int i=0; i<NUM_GROUPLISTS; i++)
    GroupListElement(data(ADDR_GROUPLISTS+i*GROUPLIST_SIZE)).clear();
}

bool
DM1701Codeplug::encodeGroupLists(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)
  for (int i=0; i<NUM_GROUPLISTS; i++) {
    GroupListElement glist(data(ADDR_GROUPLISTS+i*GROUPLIST_SIZE));
    if (i < config->rxGroupLists()->count())
      glist.fromGroupListObj(config->rxGroupLists()->list(i), ctx);
    else
      glist.clear();
  }
  return true;
}

bool
DM1701Codeplug::createGroupLists(Config *config, Context &ctx, const ErrorStack &err) {
  for (int i=0; i<NUM_GROUPLISTS; i++) {
    GroupListElement glist(data(ADDR_GROUPLISTS+i*GROUPLIST_SIZE));
    if (! glist.isValid())
      break;
    if (RXGroupList *obj = glist.toGroupListObj(ctx)) {
      config->rxGroupLists()->add(obj); ctx.add(obj, i+1);
    } else {
      errMsg(err) << "Invlaid group list at index " << i << ".";
      return false;
    }
  }
  return true;
}

bool
DM1701Codeplug::linkGroupLists(Context &ctx, const ErrorStack &err) {
  for (int i=0; i<NUM_GROUPLISTS; i++) {
    GroupListElement glist(data(ADDR_GROUPLISTS+i*GROUPLIST_SIZE));
    if (! glist.isValid())
      break;
    if (! glist.linkGroupListObj(ctx.get<RXGroupList>(i+1), ctx)) {
      errMsg(err) << "Cannot link group list at index " << i << ".";
      return false;
    }
  }
  return true;
}

void
DM1701Codeplug::clearScanLists() {
  // Clear scan lists
  for (int i=0; i<NUM_SCANLISTS; i++)
    ScanListElement(data(ADDR_SCANLISTS + i*SCANLIST_SIZE)).clear();
}

bool
DM1701Codeplug::encodeScanLists(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)
  // Define Scanlists
  for (int i=0; i<NUM_SCANLISTS; i++) {
    ScanListElement scan(data(ADDR_SCANLISTS + i*SCANLIST_SIZE));
    if (i < config->scanlists()->count())
      scan.fromScanListObj(config->scanlists()->scanlist(i), ctx);
    else
      scan.clear();
  }
  return true;
}

bool
DM1701Codeplug::createScanLists(Config *config, Context &ctx, const ErrorStack &err) {
  for (int i=0; i<NUM_SCANLISTS; i++) {
    ScanListElement scan(data(ADDR_SCANLISTS + i*SCANLIST_SIZE));
    if (! scan.isValid())
      break;
    if (ScanList *obj = scan.toScanListObj(ctx)) {
      config->scanlists()->add(obj); ctx.add(obj, i+1);
    } else {
      errMsg(err) << "Invlaid scanlist at index " << i << ".";
      return false;
    }
  }
  return true;
}

bool
DM1701Codeplug::linkScanLists(Context &ctx, const ErrorStack &err) {
  for (int i=0; i<NUM_SCANLISTS; i++) {
    ScanListElement scan(data(ADDR_SCANLISTS + i*SCANLIST_SIZE));
    if (! scan.isValid())
      break;

    if (! scan.linkScanListObj(ctx.get<ScanList>(i+1), ctx)) {
      errMsg(err) << "Cannot link scan list at index" << i << ".";
      return false;
    }
  }

  return true;
}

void
DM1701Codeplug::clearMenuSettings() {
  MenuSettingsElement(data(ADDR_MENUSETTINGS)).clear();
}

void
DM1701Codeplug::clearButtonSettings() {
  ButtonSettingsElement(data(ADDR_BUTTONSETTINGS)).clear();
}

bool
DM1701Codeplug::encodeButtonSettings(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(ctx); Q_UNUSED(err)
  // Encode settings
  return ButtonSettingsElement(data(ADDR_BUTTONSETTINGS)).fromConfig(config);
}

bool
DM1701Codeplug::decodeButtonSetttings(Config *config, const ErrorStack &err) {
  Q_UNUSED(err)
  return ButtonSettingsElement(data(ADDR_BUTTONSETTINGS)).updateConfig(config);
}

void
DM1701Codeplug::clearTextMessages() {
  memset(data(ADDR_TEXTMESSAGES), 0, NUM_TEXTMESSAGES*TEXTMESSAGE_SIZE);
}

void
DM1701Codeplug::clearPrivacyKeys() {
  EncryptionElement(data(ADDR_PRIVACY_KEYS)).clear();

}

void
DM1701Codeplug::clearEmergencySystems() {
  EmergencySettingsElement(data(ADDR_EMERGENCY_SETTINGS)).clear();
  for (int i=0; i<NUM_EMERGENCY_SYSTEMS; i++)
    EmergencySystemElement(data(ADDR_EMERGENCY_SYSTEMS + i*EMERGENCY_SYSTEM_SIZE)).clear();
}

void
DM1701Codeplug::clearVFOSettings() {
  VFOChannelElement(data(ADDR_VFO_CHANNEL_A)).clear();
  VFOChannelElement(data(ADDR_VFO_CHANNEL_B)).clear();
}


void
DM1701Codeplug::clearPositioningSystems() {
  // pass, nothing to do.
}

bool
DM1701Codeplug::encodePositioningSystems(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(config); Q_UNUSED(flags); Q_UNUSED(ctx); Q_UNUSED(err)
  // Pass, nothing to do
  return true;
}

bool
DM1701Codeplug::createPositioningSystems(Config *config, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(config); Q_UNUSED(ctx); Q_UNUSED(err)
  // Pass, nothing to do
  return true;
}

bool
DM1701Codeplug::linkPositioningSystems(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(err);
  // Nothing to do
  return true;
}
