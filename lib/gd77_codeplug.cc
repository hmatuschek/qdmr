#include "gd77_codeplug.hh"
#include "config.hh"
#include "channel.hh"
#include "utils.hh"
#include "logger.hh"
#include <QDateTime>

#define ADDR_SETTINGS             0x0000e0
#define ADDR_BUTTONS              0x000108
#define ADDR_MESSAGE_BANK         0x000128

#define NUM_SCAN_LISTS                  64
#define ADDR_SCAN_LIST_BANK       0x001790
#define SCAN_LIST_SIZE            0x000058
#define SCAN_LIST_BANK_SIZE       0x001640

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

#define NUM_CONTACTS                  1024
#define ADDR_CONTACTS             0x017620
#define CONTACT_SIZE              0x000018

#define NUM_GROUP_LISTS                 76
#define ADDR_GROUP_LIST_BANK      0x01d620
#define GROUPLIST_SIZE            0x000050
#define GROUP_LIST_BANK_SIZE      0x001840


/* ******************************************************************************************** *
 * Implementation of GD77Codeplug::ChannelElement
 * ******************************************************************************************** */
GD77Codeplug::ChannelElement::ChannelElement(uint8_t *ptr, size_t size)
  : RadioddityCodeplug::ChannelElement(ptr, size)
{
  // pass...
}

GD77Codeplug::ChannelElement::ChannelElement(uint8_t *ptr)
  : RadioddityCodeplug::ChannelElement(ptr)
{
  // pass...
}

void
GD77Codeplug::ChannelElement::clear() {
  RadioddityCodeplug::ChannelElement::clear();
  setUInt8(0x0028, 0x00);
  setARTSMode(ARTS_OFF);
  setSTEAngle(STE_FREQUENCY);
}

GD77Codeplug::ChannelElement::ARTSMode
GD77Codeplug::ChannelElement::artsMode() const {
  return ARTSMode(getUInt2(0x0030,0));
}
void
GD77Codeplug::ChannelElement::setARTSMode(ARTSMode mode) {
  setUInt2(0x0030, 0, (uint)mode);
}

GD77Codeplug::ChannelElement::STEAngle
GD77Codeplug::ChannelElement::steAngle() const {
  return STEAngle(getUInt2(0x0032,6));
}
void
GD77Codeplug::ChannelElement::setSTEAngle(STEAngle angle) {
  setUInt2(0x0032, 6, (uint)angle);
}

GD77Codeplug::ChannelElement::PTTId
GD77Codeplug::ChannelElement::pttIDMode() const {
  return PTTId(getUInt2(0x0032, 2));
}
void
GD77Codeplug::ChannelElement::setPTTIDMode(PTTId mode) {
  setUInt2(0x0032, 2, (uint)mode);
}

bool
GD77Codeplug::ChannelElement::squelchIsTight() const {
  return getBit(0x0033, 0);
}
void
GD77Codeplug::ChannelElement::enableTightSquelch(bool enable) {
  setBit(0x0033, 0, enable);
}

bool
GD77Codeplug::ChannelElement::loneWorker() const {
  return getBit(0x0033, 4);
}
void
GD77Codeplug::ChannelElement::enableLoneWorker(bool enable) {
  setBit(0x0033, 4, enable);
}

bool
GD77Codeplug::ChannelElement::autoscan() const {
  return getBit(0x0033, 5);
}
void
GD77Codeplug::ChannelElement::enableAutoscan(bool enable) {
  setBit(0x0033, 5, enable);
}


/* ******************************************************************************************** *
 * Implementation of GD77Codeplug::VFOChannelElement
 * ******************************************************************************************** */
GD77Codeplug::VFOChannelElement::VFOChannelElement(uint8_t *ptr, uint size)
  : ChannelElement(ptr, size)
{
  // pass...
}

GD77Codeplug::VFOChannelElement::VFOChannelElement(uint8_t *ptr)
  : ChannelElement(ptr)
{
  // pass...
}

void
GD77Codeplug::VFOChannelElement::clear() {
  ChannelElement::clear();
  setStepSize(12.5);
  setOffsetMode(OffsetMode::Off);
  setTXOffset(10.0);
}

QString
GD77Codeplug::VFOChannelElement::name() const {
  return QString();
}
void
GD77Codeplug::VFOChannelElement::setName(const QString &name) {
  Q_UNUSED(name);
  GD77Codeplug::ChannelElement::setName("");
}

double
GD77Codeplug::VFOChannelElement::stepSize() const {
  switch (StepSize(getUInt4(0x0036, 4))) {
  case StepSize::SS2_5kHz: return 2.5;
  case StepSize::SS5kHz: return 5;
  case StepSize::SS6_25kHz: return 6.25;
  case StepSize::SS10kHz: return 10.0;
  case StepSize::SS12_5kHz: return 12.5;
  case StepSize::SS20kHz: return 20;
  case StepSize::SS30kHz: return 30;
  case StepSize::SS50kHz: return 50;
  }
  return 12.5;
}

void
GD77Codeplug::VFOChannelElement::setStepSize(double kHz) {
  if (2.5 >= kHz)
    setUInt4(0x0036, 4, (uint)StepSize::SS2_5kHz);
  else if (5.0 >= kHz)
    setUInt4(0x0036, 4, (uint)StepSize::SS5kHz);
  else if (6.25 >= kHz)
    setUInt4(0x0036, 4, (uint)StepSize::SS6_25kHz);
  else if (10.0 >= kHz)
    setUInt4(0x0036, 4, (uint)StepSize::SS10kHz);
  else if (12.5 >= kHz)
    setUInt4(0x0036, 4, (uint)StepSize::SS12_5kHz);
  else if (20.0 >= kHz)
    setUInt4(0x0036, 4, (uint)StepSize::SS20kHz);
  else if (30.0 >= kHz)
    setUInt4(0x0036, 4, (uint)StepSize::SS30kHz);
  else
    setUInt4(0x0036, 4, (uint)StepSize::SS50kHz);
}

GD77Codeplug::VFOChannelElement::OffsetMode
GD77Codeplug::VFOChannelElement::offsetMode() const {
  return (OffsetMode)getUInt2(0x0036, 2);
}
void
GD77Codeplug::VFOChannelElement::setOffsetMode(OffsetMode mode) {
  setUInt2(0x0036, 2, (uint)mode);
}
double
GD77Codeplug::VFOChannelElement::txOffset() const {
  return ((double)getBCD4_le(0x0034))/100;
}
void
GD77Codeplug::VFOChannelElement::setTXOffset(double f) {
  setBCD4_le(0x0034, (f*100));
}


/* ******************************************************************************************** *
 * Implementation of GD77Codeplug::ContactElement
 * ******************************************************************************************** */
GD77Codeplug::ContactElement::ContactElement(uint8_t *ptr, uint size)
  : RadioddityCodeplug::ContactElement(ptr, size)
{
  // pass...
}

GD77Codeplug::ContactElement::ContactElement(uint8_t *ptr)
  : RadioddityCodeplug::ContactElement(ptr)
{
  // pass...
}

void
GD77Codeplug::ContactElement::clear() {
  markValid(false);
}

bool
GD77Codeplug::ContactElement::isValid() const {
  uint8_t validFlag = getUInt8(0x0017);
  return RadioddityCodeplug::ContactElement::isValid() && (0x00 != validFlag);
}
void
GD77Codeplug::ContactElement::markValid(bool valid) {
  if (valid)
    setUInt8(0x0017, 0xff);
  else
    setUInt8(0x0017, 0x00);
}
void
GD77Codeplug::ContactElement::fromContactObj(const DigitalContact *obj, Context &ctx) {
  RadioddityCodeplug::ContactElement::fromContactObj(obj, ctx);
  markValid();
}


/* ******************************************************************************************** *
 * Implementation of GD77Codeplug::ScanListElement
 * ******************************************************************************************** */
GD77Codeplug::ScanListElement::ScanListElement(uint8_t *ptr, uint size)
  : RadioddityCodeplug::ScanListElement(ptr, size)
{
  // pass...
}

GD77Codeplug::ScanListElement::ScanListElement(uint8_t *ptr)
  : RadioddityCodeplug::ScanListElement(ptr)
{
  // pass...
}

void
GD77Codeplug::ScanListElement::clear() {
  RadioddityCodeplug::ScanListElement::clear();
  setBit(0x0f, 0, true);
}


/* ******************************************************************************************** *
 * Implementation of GD77Codeplug::ScanListBankElement
 * ******************************************************************************************** */
GD77Codeplug::ScanListBankElement::ScanListBankElement(uint8_t *ptr, uint size)
  : RadioddityCodeplug::ScanListBankElement(ptr, size)
{
  // pass...
}

GD77Codeplug::ScanListBankElement::ScanListBankElement(uint8_t *ptr)
  : RadioddityCodeplug::ScanListBankElement(ptr, 0x1640)
{
  // pass...
}

void
GD77Codeplug::ScanListBankElement::clear() {
  memset(_data, 0, 0x0040);
}

uint8_t *
GD77Codeplug::ScanListBankElement::get(uint n) const {
  return _data+0x0040 + n*0x0058;
}


/* ******************************************************************************************** *
 * Implementation of GD77Codeplug::GroupListElement
 * ******************************************************************************************** */
GD77Codeplug::GroupListElement::GroupListElement(uint8_t *ptr, uint size)
  : RadioddityCodeplug::GroupListElement(ptr, size)
{
  // pass...
}

GD77Codeplug::GroupListElement::GroupListElement(uint8_t *ptr)
  : RadioddityCodeplug::GroupListElement(ptr, 0x0050)
{
  // pass...
}


/* ******************************************************************************************** *
 * Implementation of GD77Codeplug::GroupListBankElement
 * ******************************************************************************************** */
GD77Codeplug::GroupListBankElement::GroupListBankElement(uint8_t *ptr, uint size)
  : RadioddityCodeplug::GroupListBankElement(ptr, size)
{
  // pass...
}

GD77Codeplug::GroupListBankElement::GroupListBankElement(uint8_t *ptr)
  : RadioddityCodeplug::GroupListBankElement(ptr, 0x1840)
{
  // pass...
}

uint8_t *
GD77Codeplug::GroupListBankElement::get(uint n) const {
  return _data + 0x80 + n*GROUPLIST_SIZE;
}


/* ******************************************************************************************** *
 * Implementation of GD77Codeplug
 * ******************************************************************************************** */
GD77Codeplug::GD77Codeplug(QObject *parent)
  : RadioddityCodeplug(parent)
{
  addImage("Radioddity GD77 Codeplug");
  image(0).addElement(0x00080, 0x07b80);
  image(0).addElement(0x08000, 0x16300);
}

void
GD77Codeplug::clearGeneralSettings() {
  GeneralSettingsElement(data(ADDR_SETTINGS)).clear();
}

bool
GD77Codeplug::encodeGeneralSettings(Config *config, const Flags &flags, Context &ctx) {
  GeneralSettingsElement el(data(ADDR_SETTINGS));
  if (! flags.updateCodePlug)
    el.clear();
  return el.fromConfig(config, ctx);
}

bool
GD77Codeplug::decodeGeneralSettings(Config *config, Context &ctx) {
  return GeneralSettingsElement(data(ADDR_SETTINGS)).updateConfig(config, ctx);
}

void
GD77Codeplug::clearButtonSettings() {
  ButtonSettingsElement(data(ADDR_BUTTONS)).clear();
}

void
GD77Codeplug::clearMessages() {
  MessageBankElement(data(ADDR_MESSAGE_BANK)).clear();
}

void
GD77Codeplug::clearScanLists() {
  ScanListBankElement bank(data(ADDR_SCAN_LIST_BANK)); bank.clear();
  for (int i=0; i<NUM_SCAN_LISTS; i++)
    ScanListElement(bank.get(i)).clear();
}

bool
GD77Codeplug::encodeScanLists(Config *config, const Flags &flags, Context &ctx) {
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
GD77Codeplug::createScanLists(Config *config, Context &ctx) {
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
GD77Codeplug::linkScanLists(Config *config, Context &ctx) {
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
GD77Codeplug::clearChannels() {
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
GD77Codeplug::encodeChannels(Config *config, const Flags &flags, Context &ctx) {
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
GD77Codeplug::createChannels(Config *config, Context &ctx) {
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
GD77Codeplug::linkChannels(Config *config, Context &ctx) {
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
GD77Codeplug::clearBootSettings() {
  BootSettingsElement(data(ADDR_BOOTSETTINGS)).clear();
}

void
GD77Codeplug::clearMenuSettings() {
  MenuSettingsElement(data(ADDR_MENU_SETTINGS)).clear();
}

void
GD77Codeplug::clearBootText() {
  BootTextElement(data(ADDR_BOOT_TEXT)).clear();
}

bool
GD77Codeplug::encodeBootText(Config *config, const Flags &flags, Context &ctx) {
  BootTextElement(data(ADDR_BOOT_TEXT)).fromConfig(config);
  return true;
}

bool
GD77Codeplug::decodeBootText(Config *config, Context &ctx) {
  BootTextElement(data(ADDR_BOOT_TEXT)).updateConfig(config);
  return true;
}

void
GD77Codeplug::clearVFOSettings() {
  ChannelElement(data(ADDR_VFO_A)).clear();
  ChannelElement(data(ADDR_VFO_B)).clear();
}

void
GD77Codeplug::clearZones() {
  ZoneBankElement bank(data(ADDR_ZONE_BANK));
  bank.clear();
  for (int i=0; i<NUM_ZONES; i++)
    ZoneElement(bank.get(i)).clear();
}

bool
GD77Codeplug::encodeZones(Config *config, const Flags &flags, Context &ctx) {
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
GD77Codeplug::createZones(Config *config, Context &ctx) {
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
GD77Codeplug::linkZones(Config *config, Context &ctx) {
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
      _errorMessage = QString("%1(): Cannot unpack codeplug: Cannot link zone at index %2")
          .arg(__func__).arg(i);
      return false;
    }
  }

  return true;
}

void
GD77Codeplug::clearContacts() {
  for (int i=0; i<NUM_CONTACTS; i++)
    ContactElement(data(ADDR_CONTACTS + i*CONTACT_SIZE)).clear();
}

bool
GD77Codeplug::encodeContacts(Config *config, const Flags &flags, Context &ctx) {
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
GD77Codeplug::createContacts(Config *config, Context &ctx) {
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
GD77Codeplug::clearDTMFContacts() {
  for (int i=0; i<NUM_DTMF_CONTACTS; i++)
    DTMFContactElement(data(ADDR_DTMF_CONTACTS + i*DTMF_CONTACT_SIZE)).clear();
}

bool
GD77Codeplug::encodeDTMFContacts(Config *config, const Flags &flags, Context &ctx) {
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
GD77Codeplug::createDTMFContacts(Config *config, Context &ctx) {
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
GD77Codeplug::clearGroupLists() {
  GroupListBankElement bank(data(ADDR_GROUP_LIST_BANK)); bank.clear();
  for (int i=0; i<NUM_GROUP_LISTS; i++)
    GroupListElement(bank.get(i)).clear();
}

bool
GD77Codeplug::encodeGroupLists(Config *config, const Flags &flags, Context &ctx) {
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
GD77Codeplug::createGroupLists(Config *config, Context &ctx) {
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
GD77Codeplug::linkGroupLists(Config *config, Context &ctx) {
  GroupListBankElement bank(data(ADDR_GROUP_LIST_BANK));
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
