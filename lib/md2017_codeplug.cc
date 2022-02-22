#include "md2017_codeplug.hh"
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



MD2017Codeplug::MD2017Codeplug(QObject *parent)
  : TyTCodeplug(parent)
{
  addImage("TYT MD-2017 Codeplug");
  image(0).addElement(0x002000, 0x3e000);
  image(0).addElement(0x110000, 0x90000);

  // Clear entire codeplug
  clear();
}

MD2017Codeplug::~MD2017Codeplug() {
  // pass...
}

void
MD2017Codeplug::clearTimestamp() {
  TimestampElement(data(ADDR_TIMESTAMP)).clear();
}

bool
MD2017Codeplug::encodeTimestamp() {
  TimestampElement ts(data(ADDR_TIMESTAMP));
  ts.setTimestamp(QDateTime::currentDateTime());
  return true;
}

void
MD2017Codeplug::clearGeneralSettings() {
  GeneralSettingsElement(data(ADDR_SETTINGS)).clear();
}

bool
MD2017Codeplug::encodeGeneralSettings(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(ctx); Q_UNUSED(err)
  return GeneralSettingsElement(data(ADDR_SETTINGS)).fromConfig(config);
}

bool
MD2017Codeplug::decodeGeneralSettings(Config *config, const ErrorStack &err) {
  Q_UNUSED(err)
  return GeneralSettingsElement(data(ADDR_SETTINGS)).updateConfig(config);
}

void
MD2017Codeplug::clearChannels() {
  // Clear channels
  for (int i=0; i<NUM_CHANNELS; i++)
    ChannelElement(data(ADDR_CHANNELS+i*CHANNEL_SIZE)).clear();
}

bool
MD2017Codeplug::encodeChannels(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
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
MD2017Codeplug::createChannels(Config *config, Context &ctx, const ErrorStack &err) {
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
MD2017Codeplug::linkChannels(Context &ctx, const ErrorStack &err) {
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
MD2017Codeplug::clearContacts() {
  // Clear contacts
  for (int i=0; i<NUM_CONTACTS; i++)
    ContactElement(data(ADDR_CONTACTS+i*CONTACT_SIZE)).clear();
}

bool
MD2017Codeplug::encodeContacts(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
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
MD2017Codeplug::createContacts(Config *config, Context &ctx, const ErrorStack &err) {
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
MD2017Codeplug::clearZones() {
  // Clear zones & zone extensions
  for (int i=0; i<NUM_ZONES; i++) {
    ZoneElement(data(ADDR_ZONES+i*ZONE_SIZE)).clear();
    ZoneExtElement(data(ADDR_ZONEEXTS+i*ZONEEXT_SIZE)).clear();
  }
}

bool
MD2017Codeplug::encodeZones(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
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
MD2017Codeplug::createZones(Config *config, Context &ctx, const ErrorStack &err) {
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
MD2017Codeplug::linkZones(Context &ctx, const ErrorStack &err) {
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
MD2017Codeplug::clearGroupLists() {
  for (int i=0; i<NUM_GROUPLISTS; i++)
    GroupListElement(data(ADDR_GROUPLISTS+i*GROUPLIST_SIZE)).clear();
}

bool
MD2017Codeplug::encodeGroupLists(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
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
MD2017Codeplug::createGroupLists(Config *config, Context &ctx, const ErrorStack &err) {
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
MD2017Codeplug::linkGroupLists(Context &ctx, const ErrorStack &err) {
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
MD2017Codeplug::clearScanLists() {
  // Clear scan lists
  for (int i=0; i<NUM_SCANLISTS; i++)
    ScanListElement(data(ADDR_SCANLISTS + i*SCANLIST_SIZE)).clear();
}

bool
MD2017Codeplug::encodeScanLists(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
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
MD2017Codeplug::createScanLists(Config *config, Context &ctx, const ErrorStack &err) {

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
MD2017Codeplug::linkScanLists(Context &ctx, const ErrorStack &err) {
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
MD2017Codeplug::clearPositioningSystems() {
  // Clear GPS systems
  for (int i=0; i<NUM_GPSSYSTEMS; i++)
    GPSSystemElement(data(ADDR_GPSSYSTEMS+i*GPSSYSTEM_SIZE)).clear();
}

bool
MD2017Codeplug::encodePositioningSystems(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)
  for (int i=0; i<NUM_GPSSYSTEMS; i++) {
    GPSSystemElement gps(data(ADDR_GPSSYSTEMS+i*GPSSYSTEM_SIZE));
    if (i < config->posSystems()->gpsCount())
      gps.fromGPSSystemObj(config->posSystems()->gpsSystem(i), ctx);
    else
      gps.clear();
  }
  return true;
}

bool
MD2017Codeplug::createPositioningSystems(Config *config, Context &ctx, const ErrorStack &err) {
  for (int i=0; i<NUM_GPSSYSTEMS; i++) {
    GPSSystemElement gps(data(ADDR_GPSSYSTEMS+i*GPSSYSTEM_SIZE));
    if (! gps.isValid())
      break;
    if (GPSSystem *obj = gps.toGPSSystemObj()) {
      config->posSystems()->add(obj); ctx.add(obj, i+1);
    } else {
      errMsg(err) << "Invlaid GPS system at index " << i << ".";
      return false;
    }
  }

  return true;
}

bool
MD2017Codeplug::linkPositioningSystems(Context &ctx, const ErrorStack &err) {
  for (int i=0; i<NUM_GPSSYSTEMS; i++) {
    GPSSystemElement gps(data(ADDR_GPSSYSTEMS+i*GPSSYSTEM_SIZE));
    if (! gps.isValid())
      break;
    if (! gps.linkGPSSystemObj(ctx.get<GPSSystem>(i+1), ctx)) {
      errMsg(err) << "Cannot link GPS system at index " << i << ".";
      return false;
    }
  }

  return true;
}

void
MD2017Codeplug::clearBootSettings() {
  BootSettingsElement(data(ADDR_BOOTSETTINGS)).clear();
}

void
MD2017Codeplug::clearMenuSettings() {
  MenuSettingsElement(data(ADDR_MENUSETTINGS)).clear();
}

void
MD2017Codeplug::clearButtonSettings() {
  ButtonSettingsElement(data(ADDR_BUTTONSETTINGS)).clear();
}

bool
MD2017Codeplug::encodeButtonSettings(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(ctx); Q_UNUSED(err)
  // Encode settings
  return ButtonSettingsElement(data(ADDR_BUTTONSETTINGS)).fromConfig(config);
}

bool
MD2017Codeplug::decodeButtonSetttings(Config *config, const ErrorStack &err) {
  Q_UNUSED(err)
  return ButtonSettingsElement(data(ADDR_BUTTONSETTINGS)).updateConfig(config);
}


void
MD2017Codeplug::clearPrivacyKeys() {
  EncryptionElement(data(ADDR_PRIVACY_KEYS)).clear();
}

bool
MD2017Codeplug::encodePrivacyKeys(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err);
  // First, reset keys
  clearPrivacyKeys();
  // Get keys
  EncryptionElement keys(data(ADDR_PRIVACY_KEYS));
  // If there is no encryption extension -> done.
  if (! config->encryptionExtension())
    return true;
  // Otherwise encode
  return keys.fromEncryptionExt(config->encryptionExtension(), ctx);
}

bool
MD2017Codeplug::decodePrivacyKeys(Config *config, Context &ctx, const ErrorStack &err) {
  // Get keys
  EncryptionElement keys(data(ADDR_PRIVACY_KEYS));
  // Decode element
  EncryptionExtension *ext = keys.toEncryptionExt(ctx);
  // Handle errors
  if (nullptr == ext) {
    errMsg(err) << "Cannot create encryption extension.";
    return false;
  }

  // Add encryption extension from codeplug
  config->setEncryptionExtension(ext);
  return true;
}


void
MD2017Codeplug::clearTextMessages() {
  memset(data(ADDR_TEXTMESSAGES), 0, NUM_TEXTMESSAGES*TEXTMESSAGE_SIZE);
}

void
MD2017Codeplug::clearEmergencySystems() {
  EmergencySettingsElement(data(ADDR_EMERGENCY_SETTINGS)).clear();
  for (int i=0; i<NUM_EMERGENCY_SYSTEMS; i++)
    EmergencySystemElement(data(ADDR_EMERGENCY_SYSTEMS + i*EMERGENCY_SYSTEM_SIZE)).clear();
}

void
MD2017Codeplug::clearVFOSettings() {
  VFOChannelElement(data(ADDR_VFO_CHANNEL_A)).clear();
  VFOChannelElement(data(ADDR_VFO_CHANNEL_B)).clear();
}
