#include "uv390_codeplug.hh"
#include "codeplugcontext.hh"
#include "logger.hh"
#include "tyt_extensions.hh"


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



UV390Codeplug::UV390Codeplug(QObject *parent)
  : TyTCodeplug(parent)
{
  addImage("TYT MD-UV390 Codeplug");
  image(0).addElement(0x002000, 0x3e000);
  image(0).addElement(0x110000, 0x90000);

  // Clear entire codeplug
  clear();
}

UV390Codeplug::~UV390Codeplug() {
  // pass...
}

void
UV390Codeplug::clearTimestamp() {
  TimestampElement(data(ADDR_TIMESTAMP)).clear();
}

bool
UV390Codeplug::encodeTimestamp() {
  TimestampElement ts(data(ADDR_TIMESTAMP));
  ts.setTimestamp(QDateTime::currentDateTime());
  return true;
}

void
UV390Codeplug::clearGeneralSettings() {
  GeneralSettingsElement(data(ADDR_SETTINGS)).clear();
}

bool
UV390Codeplug::encodeGeneralSettings(Config *config, const Flags &flags) {
  return GeneralSettingsElement(data(ADDR_SETTINGS)).fromConfig(config);
}

bool
UV390Codeplug::decodeGeneralSettings(Config *config) {
  return GeneralSettingsElement(data(ADDR_SETTINGS)).updateConfig(config);
}

void
UV390Codeplug::clearChannels() {
  // Clear channels
  for (int i=0; i<NUM_CHANNELS; i++)
    ChannelElement(data(ADDR_CHANNELS+i*CHANNEL_SIZE)).clear();
}

bool
UV390Codeplug::encodeChannels(Config *config, const Flags &flags) {
  CodeplugContext ctx(config);

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
UV390Codeplug::createChannels(CodeplugContext &ctx) {
  for (int i=0; i<NUM_CHANNELS; i++) {
    ChannelElement chan(data(ADDR_CHANNELS+i*CHANNEL_SIZE));
    if (! chan.isValid())
      break;
    if (Channel *obj = chan.toChannelObj())
      ctx.addChannel(obj, i+1);
    else {
      _errorMessage = QString("Cannot decode codeplug: Invlaid channel at index %1.").arg(i);
      logError() << _errorMessage;
      return false;
    }
  }
  return true;
}

bool
UV390Codeplug::linkChannels(CodeplugContext &ctx) {
  for (int i=0; i<NUM_CHANNELS; i++) {
    ChannelElement chan(data(ADDR_CHANNELS+i*CHANNEL_SIZE));
    if (! chan.isValid())
      break;
    if (! chan.linkChannelObj(ctx.getChannel(i+1), ctx)) {
      _errorMessage = QString("Cannot decode TyT codeplug: Cannot link channel at index %1.").arg(i);
      logError() << _errorMessage;
      return false;
    }
  }
  return true;
}

void
UV390Codeplug::clearContacts() {
  // Clear contacts
  for (int i=0; i<NUM_CONTACTS; i++)
    ContactElement(data(ADDR_CONTACTS+i*CONTACT_SIZE)).clear();
}

bool
UV390Codeplug::encodeContacts(Config *config, const Flags &flags) {
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
UV390Codeplug::createContacts(CodeplugContext &ctx) {
  for (int i=0; i<NUM_CONTACTS; i++) {
    ContactElement cont(data(ADDR_CONTACTS+i*CONTACT_SIZE));
    if (! cont.isValid())
      break;
    if (DigitalContact *obj = cont.toContactObj())
      ctx.addDigitalContact(obj, i+1);
    else {
      _errorMessage = QString("Cannot decode TyT codeplug: Invlaid contact at index %1.").arg(i);
      logError() << _errorMessage;
      return false;
    }
  }
  return true;
}

void
UV390Codeplug::clearZones() {
  // Clear zones & zone extensions
  for (int i=0; i<NUM_ZONES; i++) {
    ZoneElement(data(ADDR_ZONES+i*ZONE_SIZE)).clear();
    ZoneExtElement(data(ADDR_ZONEEXTS+i*ZONEEXT_SIZE)).clear();
  }
}

bool
UV390Codeplug::encodeZones(Config *config, const Flags &flags) {
  for (int i=0; i<NUM_ZONES; i++) {
    ZoneElement zone(data(ADDR_ZONES + i*ZONE_SIZE));
    ZoneExtElement ext(data(ADDR_ZONEEXTS + i*ZONEEXT_SIZE));
    zone.clear();
    ext.clear();
    if (i < config->zones()->count()) {
      zone.fromZoneObj(config->zones()->zone(i), config);
      if (config->zones()->zone(i)->B()->count() || (16 < config->zones()->zone(i)->A()->count()))
        ext.fromZoneObj(config->zones()->zone(i), config);
    }
  }
  return true;
}

bool
UV390Codeplug::createZones(CodeplugContext &ctx) {
  for (int i=0; i<NUM_ZONES; i++) {
    ZoneElement zone(data(ADDR_ZONES+i*ZONE_SIZE));
    if (! zone.isValid())
      break;
    if (Zone *obj = zone.toZoneObj()) {
      ctx.config()->zones()->add(obj);
    } else {
      _errorMessage = QString("%1(): Cannot decode codeplug: Invlaid zone at index %2.")
          .arg(__func__).arg(i);
      logError() << _errorMessage;
      return false;
    }
  }

  return true;
}

bool
UV390Codeplug::linkZones(CodeplugContext &ctx) {
  for (int i=0; i<NUM_ZONES; i++) {
    ZoneElement zone(data(ADDR_ZONES+i*ZONE_SIZE));
    if (! zone.isValid())
      break;
    if (! zone.linkZone(ctx.config()->zones()->zone(i), ctx)) {
      _errorMessage = QString("Cannot decode TyT codeplug: Cannot link zone at index %1.").arg(i);
      logError() << _errorMessage;
      return false;
    }
    ZoneExtElement zoneext(data(ADDR_ZONEEXTS + i*ZONEEXT_SIZE));
    if (! zoneext.linkZoneObj(ctx.config()->zones()->zone(i), ctx)) {
      _errorMessage = QString("%1(): Cannot decode codeplug: Cannot link zone extension at index %2.")
          .arg(__func__).arg(i);
      logError() << _errorMessage;
      return false;
    }
  }

  return true;
}

void
UV390Codeplug::clearGroupLists() {
  for (int i=0; i<NUM_GROUPLISTS; i++)
    GroupListElement(data(ADDR_GROUPLISTS+i*GROUPLIST_SIZE)).clear();
}

bool
UV390Codeplug::encodeGroupLists(Config *config, const Flags &flags) {
  for (int i=0; i<NUM_GROUPLISTS; i++) {
    GroupListElement glist(data(ADDR_GROUPLISTS+i*GROUPLIST_SIZE));
    if (i < config->rxGroupLists()->count())
      glist.fromGroupListObj(config->rxGroupLists()->list(i), config);
    else
      glist.clear();
  }
  return true;
}

bool
UV390Codeplug::createGroupLists(CodeplugContext &ctx) {
  for (int i=0; i<NUM_GROUPLISTS; i++) {
    GroupListElement glist(data(ADDR_GROUPLISTS+i*GROUPLIST_SIZE));
    if (! glist.isValid())
      break;
    if (RXGroupList *obj = glist.toGroupListObj(ctx))
      ctx.addGroupList(obj, i+1);
    else {
      _errorMessage = QString("Cannot decode codeplug: Invlaid RX group list at index %1.").arg(i);
      logError() << _errorMessage;
      return false;
    }
  }
  return true;
}

bool
UV390Codeplug::linkGroupLists(CodeplugContext &ctx) {
  for (int i=0; i<NUM_GROUPLISTS; i++) {
    GroupListElement glist(data(ADDR_GROUPLISTS+i*GROUPLIST_SIZE));
    if (! glist.isValid())
      break;
    if (! glist.linkGroupListObj(ctx.getGroupList(i+1), ctx)) {
      _errorMessage = QString("Cannot decode codeplug: Cannot link group-list at index %1.").arg(i);
      logError() << _errorMessage;
      return false;
    }
  }
  return true;
}

void
UV390Codeplug::clearScanLists() {
  // Clear scan lists
  for (int i=0; i<NUM_SCANLISTS; i++)
    ScanListElement(data(ADDR_SCANLISTS + i*SCANLIST_SIZE)).clear();
}

bool
UV390Codeplug::encodeScanLists(Config *config, const Flags &flags) {
  // Define Scanlists
  for (int i=0; i<NUM_SCANLISTS; i++) {
    ScanListElement scan(data(ADDR_SCANLISTS + i*SCANLIST_SIZE));
    if (i < config->scanlists()->count())
      scan.fromScanListObj(config->scanlists()->scanlist(i), config);
    else
      scan.clear();
  }
  return true;
}

bool
UV390Codeplug::createScanLists(CodeplugContext &ctx) {
  for (int i=0; i<NUM_SCANLISTS; i++) {
    ScanListElement scan(data(ADDR_SCANLISTS + i*SCANLIST_SIZE));
    if (! scan.isValid())
      break;
    if (ScanList *obj = scan.toScanListObj(ctx))
      ctx.addScanList(obj, i+1);
    else {
      _errorMessage = QString("Cannot decode TyT codeplug: Invlaid scanlist at index %1.").arg(i);
      logError() << _errorMessage;
      return false;
    }
  }
  return true;
}

bool
UV390Codeplug::linkScanLists(CodeplugContext &ctx) {
  for (int i=0; i<NUM_SCANLISTS; i++) {
    ScanListElement scan(data(ADDR_SCANLISTS + i*SCANLIST_SIZE));
    if (! scan.isValid())
      break;

    if (! scan.linkScanListObj(ctx.getScanList(i+1), ctx)) {
      _errorMessage = QString("Cannot decode codeplug: Cannot link scan-list at index %1.").arg(i);
      logError() << _errorMessage;
      return false;
    }
  }

  return true;
}

void
UV390Codeplug::clearPositioningSystems() {
  // Clear GPS systems
  for (int i=0; i<NUM_GPSSYSTEMS; i++)
    GPSSystemElement(data(ADDR_GPSSYSTEMS+i*GPSSYSTEM_SIZE)).clear();
}

bool
UV390Codeplug::encodePositioningSystems(Config *config, const Flags &flags) {
  for (int i=0; i<NUM_GPSSYSTEMS; i++) {
    GPSSystemElement gps(data(ADDR_GPSSYSTEMS+i*GPSSYSTEM_SIZE));
    if (i < config->posSystems()->gpsCount())
      gps.fromGPSSystemObj(config->posSystems()->gpsSystem(i), config);
    else
      gps.clear();
  }
  return true;
}

bool
UV390Codeplug::createPositioningSystems(CodeplugContext &ctx) {
  for (int i=0; i<NUM_GPSSYSTEMS; i++) {
    GPSSystemElement gps(data(ADDR_GPSSYSTEMS+i*GPSSYSTEM_SIZE));
    if (! gps.isValid())
      break;
    if (GPSSystem *obj = gps.toGPSSystemObj()) {
      ctx.addGPSSystem(obj, i+1);
    } else {
      _errorMessage = QString("Cannot decode codeplug: Invlaid GPS system at index %1.").arg(i);
      logError() << _errorMessage;
      return false;
    }
  }

  return true;
}

bool
UV390Codeplug::linkPositioningSystems(CodeplugContext &ctx) {
  for (int i=0; i<NUM_GPSSYSTEMS; i++) {
    GPSSystemElement gps(data(ADDR_GPSSYSTEMS+i*GPSSYSTEM_SIZE));
    if (! gps.isValid())
      break;
    if (! gps.linkGPSSystemObj(ctx.config()->posSystems()->gpsSystem(i), ctx)) {
      _errorMessage = QString("Cannot decode codeplug: Cannot link GPS system at index %1.").arg(i);
      logError() << _errorMessage;
      return false;
    }
  }

  return true;
}

void
UV390Codeplug::clearButtonSettings() {
  ButtonSettingsElement(data(ADDR_BUTTONSETTINGS)).clear();
}

bool
UV390Codeplug::encodeButtonSettings(Config *config, const Flags &flags) {
  // Encode settings
  return ButtonSettingsElement(data(ADDR_BUTTONSETTINGS)).fromConfig(config);
}

bool
UV390Codeplug::decodeButtonSetttings(Config *config) {
  return ButtonSettingsElement(data(ADDR_BUTTONSETTINGS)).updateConfig(config);
}

void
UV390Codeplug::clearBootSettings() {
  BootSettingsElement(data(ADDR_BOOTSETTINGS)).clear();
}

void
UV390Codeplug::clearMenuSettings() {
  MenuSettingsElement(data(ADDR_MENUSETTINGS)).clear();
}

void
UV390Codeplug::clearTextMessages() {
  memset(data(ADDR_TEXTMESSAGES), 0, NUM_TEXTMESSAGES*TEXTMESSAGE_SIZE);
}

void
UV390Codeplug::clearPrivacyKeys() {
  EncryptionElement(data(ADDR_PRIVACY_KEYS)).clear();

}

void
UV390Codeplug::clearEmergencySystems() {
  EmergencySettingsElement(data(ADDR_EMERGENCY_SETTINGS)).clear();
  for (int i=0; i<NUM_EMERGENCY_SYSTEMS; i++)
    EmergencySystemElement(data(ADDR_EMERGENCY_SYSTEMS + i*EMERGENCY_SYSTEM_SIZE)).clear();
}

void
UV390Codeplug::clearVFOSettings() {
  VFOChannelElement(data(ADDR_VFO_CHANNEL_A)).clear();
  VFOChannelElement(data(ADDR_VFO_CHANNEL_B)).clear();
}
