#include "md390_codeplug.hh"
#include "logger.hh"


#define NUM_CHANNELS                1000
#define ADDR_CHANNELS           0x01ee00
#define CHANNEL_SIZE            0x000040

#define NUM_CONTACTS                1000
#define ADDR_CONTACTS           0x005f80
#define CONTACT_SIZE            0x000024

#define NUM_ZONES                    250
#define ADDR_ZONES              0x0149e0
#define ZONE_SIZE               0x000040

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
#define MENUSETTINGS_SIZE       0x000010
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


/* ********************************************************************************************* *
 * Implementation of MD390Codeplug::ChannelElement
 * ********************************************************************************************* */
MD390Codeplug::ChannelElement::ChannelElement(uint8_t *ptr, size_t size)
  : DM1701Codeplug::ChannelElement::ChannelElement(ptr, size)
{
  // pass...
}

MD390Codeplug::ChannelElement::ChannelElement(uint8_t *ptr)
  : DM1701Codeplug::ChannelElement(ptr, CHANNEL_SIZE)
{
  // pass...
}

void
MD390Codeplug::ChannelElement::clear() {
  DM1701Codeplug::ChannelElement::clear();

  enableCompressedUDPHeader(false);
}

bool
MD390Codeplug::ChannelElement::compressedUDPHeader() const {
  return !getBit(0x0003, 6);
}
void
MD390Codeplug::ChannelElement::enableCompressedUDPHeader(bool enable) {
  setBit(0x0003, 6, !enable);
}

Channel *
MD390Codeplug::ChannelElement::toChannelObj() const {
  Channel *ch = DM1701Codeplug::ChannelElement::toChannelObj();
  if (nullptr == ch)
    return ch;

  // Apply extension
  if (ch->tytChannelExtension()) {
    ch->tytChannelExtension()->enableCompressedUDPHeader(compressedUDPHeader());
  }
  return ch;
}

void
MD390Codeplug::ChannelElement::fromChannelObj(const Channel *c, Context &ctx) {
  DM1701Codeplug::ChannelElement::fromChannelObj(c, ctx);

  // apply extensions (extension will be created in TyTCodeplug::ChannelElement::fromChannelObj)
  if (TyTChannelExtension *ex = c->tytChannelExtension()) {
    enableCompressedUDPHeader(ex->compressedUDPHeader());
  }
}


/* ******************************************************************************************** *
 * Implementation of MD390Codeplug::MenuSettingsElement
 * ******************************************************************************************** */
MD390Codeplug::MenuSettingsElement::MenuSettingsElement(uint8_t *ptr, size_t size)
  : TyTCodeplug::MenuSettingsElement(ptr, size)
{
  // pass...
}

MD390Codeplug::MenuSettingsElement::MenuSettingsElement(uint8_t *ptr)
  : TyTCodeplug::MenuSettingsElement(ptr, MENUSETTINGS_SIZE)
{
  // pass...
}

void
MD390Codeplug::MenuSettingsElement::clear() {
  TyTCodeplug::MenuSettingsElement::clear();

  enableGPSInformation(true);
}

bool
MD390Codeplug::MenuSettingsElement::gpsInformation() const {
  return !getBit(0x04, 4);
}
void
MD390Codeplug::MenuSettingsElement::enableGPSInformation(bool enable) {
  setBit(0x04, 4, !enable);
}

bool
MD390Codeplug::MenuSettingsElement::fromConfig(const Config *config) {
  if (! TyTCodeplug::MenuSettingsElement::fromConfig(config))
    return false;

  if (TyTConfigExtension *ex = config->tytExtension()) {
     enableGPSInformation(ex->menuSettings()->gpsInformation());
  }

  return true;
}

bool
MD390Codeplug::MenuSettingsElement::updateConfig(Config *config) {
  if (! TyTCodeplug::MenuSettingsElement::updateConfig(config))
    return false;

  if (TyTConfigExtension *ex = config->tytExtension()) {
    ex->menuSettings()->enableGPSInformation(gpsInformation());
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of MD390Codeplug
 * ********************************************************************************************* */
MD390Codeplug::MD390Codeplug(QObject *parent)
  : TyTCodeplug(parent)
{
  addImage("TYT MD-390 Codeplug");
  image(0).addElement(0x002000, 0x3e000);
  // Clear entire codeplug
  clear();
}

bool
MD390Codeplug::decodeElements(Context &ctx, const ErrorStack &err) {
  logDebug() << "Decode MD390 codeplug, programmed with CPS version "
             << TimestampElement(data(ADDR_TIMESTAMP)).cpsVersion() << ".";
  return TyTCodeplug::decodeElements(ctx, err);
}

void
MD390Codeplug::clearTimestamp() {
  TimestampElement(data(ADDR_TIMESTAMP)).clear();
}

bool
MD390Codeplug::encodeTimestamp() {
  TimestampElement ts(data(ADDR_TIMESTAMP));
  ts.setTimestamp(QDateTime::currentDateTime());
  return true;
}

void
MD390Codeplug::clearGeneralSettings() {
  GeneralSettingsElement(data(ADDR_SETTINGS)).clear();
}

bool
MD390Codeplug::encodeGeneralSettings(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(ctx); Q_UNUSED(err)
  return GeneralSettingsElement(data(ADDR_SETTINGS)).fromConfig(config);
}

bool
MD390Codeplug::decodeGeneralSettings(Config *config, const ErrorStack &err) {
  Q_UNUSED(err)
  return GeneralSettingsElement(data(ADDR_SETTINGS)).updateConfig(config);
}

void
MD390Codeplug::clearChannels() {
  // Clear channels
  for (int i=0; i<NUM_CHANNELS; i++)
    ChannelElement(data(ADDR_CHANNELS+i*CHANNEL_SIZE)).clear();
}

bool
MD390Codeplug::encodeChannels(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
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
MD390Codeplug::createChannels(Config *config, Context &ctx, const ErrorStack &err) {
  for (int i=0; i<NUM_CHANNELS; i++) {
    ChannelElement chan(data(ADDR_CHANNELS+i*CHANNEL_SIZE));
    if (! chan.isValid())
      continue;
    if (Channel *obj = chan.toChannelObj()) {
      config->channelList()->add(obj); ctx.add(obj, i+1);
    } else {
      errMsg(err) << "Invalid channel at index " << i << ".";
      return false;
    }
  }
  return true;
}

bool
MD390Codeplug::linkChannels(Context &ctx, const ErrorStack &err) {
  for (int i=0; i<NUM_CHANNELS; i++) {
    ChannelElement chan(data(ADDR_CHANNELS+i*CHANNEL_SIZE));
    if (! chan.isValid())
      continue;
    if (! chan.linkChannelObj(ctx.get<Channel>(i+1), ctx)) {
      errMsg(err) << "Cannot link channel at index " << i << ".";
      return false;
    }
  }
  return true;
}

void
MD390Codeplug::clearContacts() {
  // Clear contacts
  for (int i=0; i<NUM_CONTACTS; i++)
    ContactElement(data(ADDR_CONTACTS+i*CONTACT_SIZE)).clear();
}

bool
MD390Codeplug::encodeContacts(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
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
MD390Codeplug::createContacts(Config *config, Context &ctx, const ErrorStack &err) {
  for (int i=0; i<NUM_CONTACTS; i++) {
    ContactElement cont(data(ADDR_CONTACTS+i*CONTACT_SIZE));
    if (! cont.isValid())
      continue;
    if (DMRContact *obj = cont.toContactObj()) {
      config->contacts()->add(obj); ctx.add(obj, i+1);
    } else {
      errMsg(err) << "Invalid contact at index " << i << ".";
      return false;
    }
  }
  return true;
}

void
MD390Codeplug::clearZones() {
  // Clear zones & zone extensions
  for (int i=0; i<NUM_ZONES; i++) {
    ZoneElement(data(ADDR_ZONES+i*ZONE_SIZE)).clear();
  }
}

bool
MD390Codeplug::encodeZones(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)
  for (int i=0,z=0; i<NUM_ZONES; i++, z++) {
    ZoneElement zone(data(ADDR_ZONES + i*ZONE_SIZE));
    zone.clear();

    if (z < config->zones()->count()) {
      // handle A list
      Zone *obj = config->zones()->zone(z);
      bool needs_ext = obj->B()->count();
      // set name
      if (needs_ext)
        zone.setName(obj->name() + " A");
      else
        zone.setName(obj->name());
      // fill channels
      for (int c=0; c<16; c++) {
        if (c < obj->A()->count())
          zone.setMemberIndex(c, ctx.index(obj->A()->get(c)));
      }
      // If there is a B list, add a zone more
      if (needs_ext) {
        i++;
        ZoneElement zone(data(ADDR_ZONES + i*ZONE_SIZE));
        zone.clear();
        zone.setName(obj->name() + " B");
        for (int c=0; c<16; c++) {
          if (c < obj->B()->count())
            zone.setMemberIndex(c, ctx.index(obj->B()->get(c)));
        }
      }
    }
  }

  return true;
}

bool
MD390Codeplug::createZones(Config *config, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)
  Zone *last_zone = nullptr;
  for (int i=0; i<NUM_ZONES; i++) {
    ZoneElement zone(data(ADDR_ZONES+i*ZONE_SIZE));
    if (! zone.isValid())
      continue;
    bool is_ext = (nullptr != last_zone) && (zone.name().endsWith(" B")) &&
        (zone.name().startsWith(last_zone->name()));
    Zone *obj = last_zone;
    if (! is_ext) {
      last_zone = obj = new Zone(zone.name());
      if (zone.name().endsWith(" A"))
        obj->setName(zone.name().chopped(2));
      config->zones()->add(obj); ctx.add(obj, i+1);
    }
  }

  return true;
}

bool
MD390Codeplug::linkZones(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)
  Zone *last_zone = nullptr;
  for (int i=0, z=0; i<NUM_ZONES; i++, z++) {
    ZoneElement zone(data(ADDR_ZONES+i*ZONE_SIZE));
    if (! zone.isValid())
      continue;
    if (ctx.has<Zone>(i+1)) {
      Zone *obj = last_zone = ctx.get<Zone>(i+1);
      for (int i=0; ((i<16) && zone.memberIndex(i)); i++) {
        if (! ctx.has<Channel>(zone.memberIndex(i))) {
          logWarn() << "Cannot link channel with index " << zone.memberIndex(i)
                    << " channel not defined.";
          continue;
        }
        obj->A()->add(ctx.get<Channel>(zone.memberIndex(i)));
      }
    } else {
      Zone *obj = last_zone; last_zone = nullptr;
      for (int i=0; ((i<16) && zone.memberIndex(i)); i++) {
        if (! ctx.has<Channel>(zone.memberIndex(i))) {
          logWarn() << "Cannot link channel with index " << zone.memberIndex(i)
                    << " channel not defined.";
          continue;
        }
        obj->B()->add(ctx.get<Channel>(zone.memberIndex(i)));
      }
    }
  }

  return true;
}

void
MD390Codeplug::clearGroupLists() {
  for (int i=0; i<NUM_GROUPLISTS; i++)
    GroupListElement(data(ADDR_GROUPLISTS+i*GROUPLIST_SIZE)).clear();
}

bool
MD390Codeplug::encodeGroupLists(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
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
MD390Codeplug::createGroupLists(Config *config, Context &ctx, const ErrorStack &err) {
  for (int i=0; i<NUM_GROUPLISTS; i++) {
    GroupListElement glist(data(ADDR_GROUPLISTS+i*GROUPLIST_SIZE));
    if (! glist.isValid())
      continue;
    if (RXGroupList *obj = glist.toGroupListObj(ctx)) {
      config->rxGroupLists()->add(obj); ctx.add(obj, i+1);
    } else {
      errMsg(err) << "Invalid group list at index " << i << ".";
      return false;
    }
  }
  return true;
}

bool
MD390Codeplug::linkGroupLists(Context &ctx, const ErrorStack &err) {
  for (int i=0; i<NUM_GROUPLISTS; i++) {
    GroupListElement glist(data(ADDR_GROUPLISTS+i*GROUPLIST_SIZE));
    if (! glist.isValid())
      continue;
    if (! glist.linkGroupListObj(ctx.get<RXGroupList>(i+1), ctx)) {
      errMsg(err) << "Cannot link group list at index " << i << ".";
      return false;
    }
  }
  return true;
}

void
MD390Codeplug::clearScanLists() {
  // Clear scan lists
  for (int i=0; i<NUM_SCANLISTS; i++)
    ScanListElement(data(ADDR_SCANLISTS + i*SCANLIST_SIZE)).clear();
}

bool
MD390Codeplug::encodeScanLists(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
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
MD390Codeplug::createScanLists(Config *config, Context &ctx, const ErrorStack &err) {
  for (int i=0; i<NUM_SCANLISTS; i++) {
    ScanListElement scan(data(ADDR_SCANLISTS + i*SCANLIST_SIZE));
    if (! scan.isValid())
      continue;
    if (ScanList *obj = scan.toScanListObj(ctx)) {
      config->scanlists()->add(obj); ctx.add(obj, i+1);
    } else {
      errMsg(err) << "Invalid scanlist at index " << i << ".";
      return false;
    }
  }
  return true;
}

bool
MD390Codeplug::linkScanLists(Context &ctx, const ErrorStack &err) {
  for (int i=0; i<NUM_SCANLISTS; i++) {
    ScanListElement scan(data(ADDR_SCANLISTS + i*SCANLIST_SIZE));
    if (! scan.isValid())
      continue;
    if (! scan.linkScanListObj(ctx.get<ScanList>(i+1), ctx)) {
      errMsg(err) << "Cannot link scan list at index " << i << ".";
      return false;
    }
  }

  return true;
}

void
MD390Codeplug::clearPositioningSystems() {
  // Clear GPS systems
  for (int i=0; i<NUM_GPSSYSTEMS; i++)
    GPSSystemElement(data(ADDR_GPSSYSTEMS+i*GPSSYSTEM_SIZE)).clear();
}

bool
MD390Codeplug::encodePositioningSystems(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)
  for (int i=0; i<NUM_GPSSYSTEMS; i++) {
    GPSSystemElement gps(data(ADDR_GPSSYSTEMS+i*GPSSYSTEM_SIZE));
    if (i < config->posSystems()->gpsCount()) {
      logDebug() << "Encode GPS system #" << i << " '" <<
                    config->posSystems()->gpsSystem(i)->name() << "'.";
      gps.fromGPSSystemObj(config->posSystems()->gpsSystem(i), ctx);
    } else {
      gps.clear();
    }
  }
  return true;
}

bool
MD390Codeplug::createPositioningSystems(Config *config, Context &ctx, const ErrorStack &err) {
  for (int i=0; i<NUM_GPSSYSTEMS; i++) {
    GPSSystemElement gps(data(ADDR_GPSSYSTEMS+i*GPSSYSTEM_SIZE));
    if (! gps.isValid())
      continue;
    if (GPSSystem *obj = gps.toGPSSystemObj()) {
      config->posSystems()->add(obj); ctx.add(obj, i+1);
    } else {
      errMsg(err) << "Invalid GPS system at index " << i << ".";
      return false;
    }
  }

  return true;
}

bool
MD390Codeplug::linkPositioningSystems(Context &ctx, const ErrorStack &err) {
  for (int i=0; i<NUM_GPSSYSTEMS; i++) {
    GPSSystemElement gps(data(ADDR_GPSSYSTEMS+i*GPSSYSTEM_SIZE));
    if (! gps.isValid())
      continue;
    if (! gps.linkGPSSystemObj(ctx.get<GPSSystem>(i+1), ctx)) {
      errMsg(err) << "Cannot link GPS system at index " << i << ".";
      return false;
    }
  }

  return true;
}

void
MD390Codeplug::clearButtonSettings() {
  ButtonSettingsElement(data(ADDR_BUTTONSETTINGS)).clear();
}

bool
MD390Codeplug::encodeButtonSettings(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(flags); Q_UNUSED(err)
  // Encode settings
  return ButtonSettingsElement(data(ADDR_BUTTONSETTINGS)).fromConfig(config);
}

bool
MD390Codeplug::decodeButtonSetttings(Config *config, const ErrorStack &err) {
  Q_UNUSED(err)
  return ButtonSettingsElement(data(ADDR_BUTTONSETTINGS)).updateConfig(config);
}


void
MD390Codeplug::clearPrivacyKeys() {
  EncryptionElement(data(ADDR_PRIVACY_KEYS)).clear();
}

bool
MD390Codeplug::encodePrivacyKeys(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err);
  // First, reset keys
  clearPrivacyKeys();
  // Get keys
  EncryptionElement keys(data(ADDR_PRIVACY_KEYS));
  // Encode keys
  return keys.fromCommercialExt(config->commercialExtension(), ctx);
}

bool
MD390Codeplug::decodePrivacyKeys(Config *config, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(config)
  // Get keys
  EncryptionElement keys(data(ADDR_PRIVACY_KEYS));
  // Decode element
  if (! keys.updateCommercialExt(ctx)) {
    errMsg(err) << "Cannot create encryption extension.";
    return false;
  }
  return true;
}


void
MD390Codeplug::clearMenuSettings() {
  MenuSettingsElement(data(ADDR_MENUSETTINGS)).clear();
}

void
MD390Codeplug::clearTextMessages() {
  memset(data(ADDR_TEXTMESSAGES), 0, NUM_TEXTMESSAGES*TEXTMESSAGE_SIZE);
}

void
MD390Codeplug::clearEmergencySystems() {
  EmergencySettingsElement(data(ADDR_EMERGENCY_SETTINGS)).clear();
  for (int i=0; i<NUM_EMERGENCY_SYSTEMS; i++)
    EmergencySystemElement(data(ADDR_EMERGENCY_SYSTEMS + i*EMERGENCY_SYSTEM_SIZE)).clear();
}
