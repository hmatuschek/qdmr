#include "md2017_codeplug.hh"
#include "codeplugcontext.hh"
#include "logger.hh"

#define NUM_CHANNELS         3000
#define ADDR_CHANNELS    0x110000
#define CHANNEL_SIZE     0x000040

#define NUM_CONTACTS        10000
#define ADDR_CONTACTS    0x140000
#define CONTACT_SIZE     0x000024

#define NUM_ZONES             250
#define ADDR_ZONES       0x0149e0
#define ZONE_SIZE        0x000040
#define ADDR_ZONEEXTS    0x031000
#define ZONEEXT_SIZE     0x0000e0

#define NUM_GROUPLISTS        250
#define ADDR_GROUPLISTS  0x00ec20
#define GROUPLIST_SIZE   0x000060

#define NUM_SCANLISTS         250
#define ADDR_SCANLISTS   0x018860
#define SCANLIST_SIZE    0x000068

#define ADDR_SETTINGS    0x002040



MD2017Codeplug::MD2017Codeplug(QObject *parent)
  : TyTCodeplug(parent)
{
  addImage("TYT MD-UV390 Codeplug");
  image(0).addElement(0x002000, 0x3e000);
  image(0).addElement(0x110000, 0x90000);

  // Clear entire codeplug
  clear();
}

MD2017Codeplug::~MD2017Codeplug() {
  // pass...
}

void
MD2017Codeplug::clearGeneralSettings() {
  GeneralSettingsElement(data(ADDR_SETTINGS)).clear();
}

bool
MD2017Codeplug::encodeGeneralSettings(Config *config, const Flags &flags) {
  return GeneralSettingsElement(data(ADDR_SETTINGS)).fromConfig(config);
}

bool
MD2017Codeplug::decodeGeneralSettings(Config *config) {
  return GeneralSettingsElement(data(ADDR_SETTINGS)).updateConfig(config);
}

void
MD2017Codeplug::clearChannels() {
  // Clear channels
  for (int i=0; i<NUM_CHANNELS; i++)
    ChannelElement(data(ADDR_CHANNELS+i*CHANNEL_SIZE)).clear();
}

bool
MD2017Codeplug::encodeChannels(Config *config, const Flags &flags) {
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
MD2017Codeplug::createChannels(CodeplugContext &ctx) {
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
MD2017Codeplug::linkChannels(CodeplugContext &ctx) {
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
MD2017Codeplug::clearContacts() {
  // Clear contacts
  for (int i=0; i<NUM_CONTACTS; i++)
    ContactElement(data(ADDR_CONTACTS+i*CONTACT_SIZE)).clear();
}

bool
MD2017Codeplug::encodeContacts(Config *config, const Flags &flags) {
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
MD2017Codeplug::createContacts(CodeplugContext &ctx) {
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
MD2017Codeplug::clearZones() {
  // Clear zones & zone extensions
  for (int i=0; i<NUM_ZONES; i++) {
    ZoneElement(data(ADDR_ZONES+i*ZONE_SIZE)).clear();
    ZoneExtElement(data(ADDR_ZONEEXTS+i*ZONEEXT_SIZE)).clear();
  }
}

bool
MD2017Codeplug::encodeZones(Config *config, const Flags &flags) {
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
MD2017Codeplug::createZones(CodeplugContext &ctx) {
  for (int i=0; i<NUM_ZONES; i++) {
    ZoneElement zone(data(ADDR_ZONES+i*ZONE_SIZE));
    if (! zone.isValid())
      break;
    if (Zone *obj = zone.toZoneObj()) {
      ctx.config()->zones()->addZone(obj);
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
MD2017Codeplug::linkZones(CodeplugContext &ctx) {
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
MD2017Codeplug::clearGroupLists() {
  for (int i=0; i<NUM_GROUPLISTS; i++)
    GroupListElement(data(ADDR_GROUPLISTS+i*GROUPLIST_SIZE)).clear();
}

bool
MD2017Codeplug::encodeGroupLists(Config *config, const Flags &flags) {
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
MD2017Codeplug::createGroupLists(CodeplugContext &ctx) {
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
MD2017Codeplug::linkGroupLists(CodeplugContext &ctx) {
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
MD2017Codeplug::clearScanLists() {
  // Clear scan lists
  for (int i=0; i<NUM_SCANLISTS; i++)
    ScanListElement(data(ADDR_SCANLISTS + i*SCANLIST_SIZE)).clear();
}

bool
MD2017Codeplug::encodeScanLists(Config *config, const Flags &flags) {
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
MD2017Codeplug::createScanLists(CodeplugContext &ctx) {
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
MD2017Codeplug::linkScanLists(CodeplugContext &ctx) {
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
