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
  : TyTCodeplug::ChannelElement::ChannelElement(ptr, size)
{
  // pass...
}

MD390Codeplug::ChannelElement::ChannelElement(uint8_t *ptr)
  : TyTCodeplug::ChannelElement(ptr, CHANNEL_SIZE)
{
  // pass...
}

void
MD390Codeplug::ChannelElement::clear() {
  TyTCodeplug::ChannelElement::clear();

  enableTightSquelch(false);
  enableCompressedUDPHeader(false);
  enableReverseBurst(true);
  setPower(Channel::Power::High);
  setUInt8(0x0005, 0xc3);
  setUInt8(0x000f, 0xff);
}

bool
MD390Codeplug::ChannelElement::tightSquelchEnabled() const {
  return !getBit(0x0000, 5);
}
void
MD390Codeplug::ChannelElement::enableTightSquelch(bool enable) {
  setBit(0x0000, 5, !enable);
}

bool
MD390Codeplug::ChannelElement::compressedUDPHeader() const {
  return !getBit(0x0003, 6);
}
void
MD390Codeplug::ChannelElement::enableCompressedUDPHeader(bool enable) {
  setBit(0x0003, 6, !enable);
}

bool
MD390Codeplug::ChannelElement::reverseBurst() const {
  return getBit(0x0004, 2);
}
void
MD390Codeplug::ChannelElement::enableReverseBurst(bool enable) {
  setBit(0x0004, 2, enable);
}

Channel::Power
MD390Codeplug::ChannelElement::power() const {
  if (getBit(0x0004, 5))
    return Channel::Power::High;
  return Channel::Power::Low;
}
void
MD390Codeplug::ChannelElement::setPower(Channel::Power pwr) {
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


/* ********************************************************************************************* *
 * Implementation of MD390Codeplug
 * ********************************************************************************************* */
MD390Codeplug::MD390Codeplug(QObject *parent)
{
  addImage("TYT MD-390 Codeplug");
  image(0).addElement(0x002000, 0x3e000);
  // Clear entire codeplug
  clear();
}

bool
MD390Codeplug::decodeElements(Context &ctx) {
  logDebug() << "Decode MD390 codeplug, programmed with CPS version "
             << TimestampElement(data(ADDR_TIMESTAMP)).cpsVersion() << ".";
  return TyTCodeplug::decodeElements(ctx);
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
MD390Codeplug::encodeGeneralSettings(Config *config, const Flags &flags, Context &ctx) {
  return GeneralSettingsElement(data(ADDR_SETTINGS)).fromConfig(config);
}

bool
MD390Codeplug::decodeGeneralSettings(Config *config) {
  return GeneralSettingsElement(data(ADDR_SETTINGS)).updateConfig(config);
}

void
MD390Codeplug::clearChannels() {
  // Clear channels
  for (int i=0; i<NUM_CHANNELS; i++)
    ChannelElement(data(ADDR_CHANNELS+i*CHANNEL_SIZE)).clear();
}

bool
MD390Codeplug::encodeChannels(Config *config, const Flags &flags, Context &ctx) {
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
MD390Codeplug::createChannels(Config *config, Context &ctx) {
  for (int i=0; i<NUM_CHANNELS; i++) {
    ChannelElement chan(data(ADDR_CHANNELS+i*CHANNEL_SIZE));
    if (! chan.isValid())
      break;
    if (Channel *obj = chan.toChannelObj()) {
      config->channelList()->add(obj); ctx.add(obj, i+1);
    } else {
      _errorMessage = QString("Cannot decode codeplug: Invlaid channel at index %1.").arg(i);
      logError() << _errorMessage;
      return false;
    }
  }
  return true;
}

bool
MD390Codeplug::linkChannels(Context &ctx) {
  for (int i=0; i<NUM_CHANNELS; i++) {
    ChannelElement chan(data(ADDR_CHANNELS+i*CHANNEL_SIZE));
    if (! chan.isValid())
      break;
    if (! chan.linkChannelObj(ctx.get<Channel>(i+1), ctx)) {
      _errorMessage = QString("Cannot decode TyT codeplug: Cannot link channel at index %1.").arg(i);
      logError() << _errorMessage;
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
MD390Codeplug::encodeContacts(Config *config, const Flags &flags, Context &ctx) {
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
MD390Codeplug::createContacts(Config *config, Context &ctx) {
  for (int i=0; i<NUM_CONTACTS; i++) {
    ContactElement cont(data(ADDR_CONTACTS+i*CONTACT_SIZE));
    if (! cont.isValid())
      break;
    if (DigitalContact *obj = cont.toContactObj()) {
      config->contacts()->add(obj); ctx.add(obj, i+1);
    } else {
      _errorMessage = QString("Cannot decode TyT codeplug: Invlaid contact at index %1.").arg(i);
      logError() << _errorMessage;
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
MD390Codeplug::encodeZones(Config *config, const Flags &flags, Context &ctx) {
  for (int i=0; i<NUM_ZONES; i++) {
    ZoneElement zone(data(ADDR_ZONES + i*ZONE_SIZE));
    zone.clear();
    if (i < config->zones()->count()) {
      zone.fromZoneObj(config->zones()->zone(i), ctx);
    }
  }
  return true;
}

bool
MD390Codeplug::createZones(Config *config, Context &ctx) {
  for (int i=0; i<NUM_ZONES; i++) {
    ZoneElement zone(data(ADDR_ZONES+i*ZONE_SIZE));
    if (! zone.isValid())
      break;
    if (Zone *obj = zone.toZoneObj()) {
      config->zones()->add(obj); ctx.add(obj, i+1);
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
MD390Codeplug::linkZones(Context &ctx) {
  for (int i=0; i<NUM_ZONES; i++) {
    ZoneElement zone(data(ADDR_ZONES+i*ZONE_SIZE));
    if (! zone.isValid())
      break;
    if (! zone.linkZone(ctx.get<Zone>(i+1), ctx)) {
      _errorMessage = QString("Cannot decode TyT codeplug: Cannot link zone at index %1.").arg(i);
      logError() << _errorMessage;
      return false;
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
MD390Codeplug::encodeGroupLists(Config *config, const Flags &flags, Context &ctx) {
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
MD390Codeplug::createGroupLists(Config *config, Context &ctx) {
  for (int i=0; i<NUM_GROUPLISTS; i++) {
    GroupListElement glist(data(ADDR_GROUPLISTS+i*GROUPLIST_SIZE));
    if (! glist.isValid())
      break;
    if (RXGroupList *obj = glist.toGroupListObj(ctx)) {
      config->rxGroupLists()->add(obj); ctx.add(obj, i+1);
    } else {
      _errorMessage = QString("Cannot decode codeplug: Invlaid RX group list at index %1.").arg(i);
      logError() << _errorMessage;
      return false;
    }
  }
  return true;
}

bool
MD390Codeplug::linkGroupLists(Context &ctx) {
  for (int i=0; i<NUM_GROUPLISTS; i++) {
    GroupListElement glist(data(ADDR_GROUPLISTS+i*GROUPLIST_SIZE));
    if (! glist.isValid())
      break;
    if (! glist.linkGroupListObj(ctx.get<RXGroupList>(i+1), ctx)) {
      _errorMessage = QString("Cannot decode codeplug: Cannot link group-list at index %1.").arg(i);
      logError() << _errorMessage;
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
MD390Codeplug::encodeScanLists(Config *config, const Flags &flags, Context &ctx) {
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
MD390Codeplug::createScanLists(Config *config, Context &ctx) {
  for (int i=0; i<NUM_SCANLISTS; i++) {
    ScanListElement scan(data(ADDR_SCANLISTS + i*SCANLIST_SIZE));
    if (! scan.isValid())
      break;
    if (ScanList *obj = scan.toScanListObj(ctx)) {
      config->scanlists()->add(obj); ctx.add(obj, i+1);
    } else {
      _errorMessage = QString("Cannot decode TyT codeplug: Invlaid scanlist at index %1.").arg(i);
      logError() << _errorMessage;
      return false;
    }
  }
  return true;
}

bool
MD390Codeplug::linkScanLists(Context &ctx) {
  for (int i=0; i<NUM_SCANLISTS; i++) {
    ScanListElement scan(data(ADDR_SCANLISTS + i*SCANLIST_SIZE));
    if (! scan.isValid())
      break;

    if (! scan.linkScanListObj(ctx.get<ScanList>(i+1), ctx)) {
      _errorMessage = QString("Cannot decode codeplug: Cannot link scan-list at index %1.").arg(i);
      logError() << _errorMessage;
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
MD390Codeplug::encodePositioningSystems(Config *config, const Flags &flags, Context &ctx) {
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
MD390Codeplug::createPositioningSystems(Config *config, Context &ctx) {
  for (int i=0; i<NUM_GPSSYSTEMS; i++) {
    GPSSystemElement gps(data(ADDR_GPSSYSTEMS+i*GPSSYSTEM_SIZE));
    if (! gps.isValid())
      break;
    if (GPSSystem *obj = gps.toGPSSystemObj()) {
      config->posSystems()->add(obj); ctx.add(obj, i+1);
    } else {
      _errorMessage = QString("Cannot decode codeplug: Invlaid GPS system at index %1.").arg(i);
      logError() << _errorMessage;
      return false;
    }
  }

  return true;
}

bool
MD390Codeplug::linkPositioningSystems(Context &ctx) {
  for (int i=0; i<NUM_GPSSYSTEMS; i++) {
    GPSSystemElement gps(data(ADDR_GPSSYSTEMS+i*GPSSYSTEM_SIZE));
    if (! gps.isValid())
      break;
    if (! gps.linkGPSSystemObj(ctx.get<GPSSystem>(i+1), ctx)) {
      _errorMessage = QString("Cannot decode codeplug: Cannot link GPS system at index %1.").arg(i);
      logError() << _errorMessage;
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
MD390Codeplug::encodeButtonSettings(Config *config, const Flags &flags, Context &ctx) {
  // Encode settings
  return ButtonSettingsElement(data(ADDR_BUTTONSETTINGS)).fromConfig(config);
}

bool
MD390Codeplug::decodeButtonSetttings(Config *config) {
  return ButtonSettingsElement(data(ADDR_BUTTONSETTINGS)).updateConfig(config);
}

void
MD390Codeplug::clearBootSettings() {
  BootSettingsElement(data(ADDR_BOOTSETTINGS)).clear();
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
MD390Codeplug::clearPrivacyKeys() {
  EncryptionElement(data(ADDR_PRIVACY_KEYS)).clear();

}

void
MD390Codeplug::clearEmergencySystems() {
  EmergencySettingsElement(data(ADDR_EMERGENCY_SETTINGS)).clear();
  for (int i=0; i<NUM_EMERGENCY_SYSTEMS; i++)
    EmergencySystemElement(data(ADDR_EMERGENCY_SYSTEMS + i*EMERGENCY_SYSTEM_SIZE)).clear();
}

void
MD390Codeplug::clearVFOSettings() {
  // There is no VFO settings in MD-390 codeplugs.
}
