#include "uv390_codeplug.hh"
#include "codeplugcontext.hh"
#include "logger.hh"
#include "tyt_extensions.hh"
#include <QTimeZone>


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
#define SETTINGS_SIZE           0x0000b0
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



/* ******************************************************************************************** *
 * Implementation of UV390Codeplug::ChannelElement
 * ******************************************************************************************** */
UV390Codeplug::ChannelElement::ChannelElement(uint8_t *ptr, size_t size)
  : TyTCodeplug::ChannelElement(ptr, size)
{
  // pass...
}

UV390Codeplug::ChannelElement::ChannelElement(uint8_t *ptr)
  : TyTCodeplug::ChannelElement(ptr, CHANNEL_SIZE)
{
  // pass...
}

void
UV390Codeplug::ChannelElement::clear() {
  Element::clear();

  clearBit(5,0);
  setInCallCriteria(INCALL_ALWAYS);
  setTurnOffFreq(TURNOFF_NONE);
  setSquelch(1);
  setPower(Channel::Power::High);
  enableAllowInterrupt(true);
  enableDualCapacityDirectMode(false);
  enableLeaderOrMS(true);
}

UV390Codeplug::ChannelElement::InCall
UV390Codeplug::ChannelElement::inCallCriteria() const {
  return InCall(getUInt2(5,4));
}
void
UV390Codeplug::ChannelElement::setInCallCriteria(InCall crit) {
  setUInt2(5,4, uint8_t(crit));
}

UV390Codeplug::ChannelElement::TurnOffFreq
UV390Codeplug::ChannelElement::turnOffFreq() const {
  return TurnOffFreq(getUInt2(5,6));
}
void
UV390Codeplug::ChannelElement::setTurnOffFreq(TurnOffFreq freq) {
  setUInt2(5,6, uint8_t(freq));
}

unsigned
UV390Codeplug::ChannelElement::squelch() const {
  return getUInt8(15);
}
void
UV390Codeplug::ChannelElement::setSquelch(unsigned value) {
  value = std::min(unsigned(10), value);
  return setUInt8(15, value);
}

Channel::Power
UV390Codeplug::ChannelElement::power() const {
  switch (getUInt2(30, 0)) {
  case 0: return Channel::Power::Low;
  case 2: return Channel::Power::Mid;
  case 3: return Channel::Power::High;
  default: break;
  }
  return Channel::Power::Low;
}
void
UV390Codeplug::ChannelElement::setPower(Channel::Power pwr) {
  switch (pwr) {
  case Channel::Power::Min:
  case Channel::Power::Low:
    setUInt2(30,0, 0);
    break;
  case Channel::Power::Mid:
    setUInt2(30,0, 2);
    break;
  case Channel::Power::High:
  case Channel::Power::Max:
    setUInt2(30,0, 3);
  }
}

bool
UV390Codeplug::ChannelElement::allowInterrupt() const {
  return !getBit(31, 2);
}
void
UV390Codeplug::ChannelElement::enableAllowInterrupt(bool enable) {
  setBit(31,2, !enable);
}

bool
UV390Codeplug::ChannelElement::dualCapacityDirectMode() const {
  return !getBit(31, 3);
}
void
UV390Codeplug::ChannelElement::enableDualCapacityDirectMode(bool enable) {
  setBit(31,3, !enable);
}

bool
UV390Codeplug::ChannelElement::leaderOrMS() const {
  return !getBit(31, 4);
}
void
UV390Codeplug::ChannelElement::enableLeaderOrMS(bool enable) {
  setBit(31,4, !enable);
}


Channel *
UV390Codeplug::ChannelElement::toChannelObj() const {
  if (! isValid())
    return nullptr;

  Channel *ch = TyTCodeplug::ChannelElement::toChannelObj();
  if (nullptr == ch)
    return nullptr;

  // decode squelch setting
  if (ch->is<AnalogChannel>()) {
    AnalogChannel *ach = ch->as<AnalogChannel>();
    ach->setSquelch(squelch());
  }
  // Common settings
  ch->setPower(power());
  return ch;
}

void
UV390Codeplug::ChannelElement::fromChannelObj(const Channel *chan, Context &ctx) {
  TyTCodeplug::ChannelElement::fromChannelObj(chan, ctx);
  // encode power setting
  if (chan->defaultPower())
    setPower(ctx.config()->settings()->power());
  else
    setPower(chan->power());
  setSquelch(0);

  if (chan->is<AnalogChannel>()) {
    const AnalogChannel *achan = chan->as<const AnalogChannel>();
    if (achan->defaultSquelch())
      setSquelch(ctx.config()->settings()->squelch());
    else
      setSquelch(achan->squelch());
  }
}


/* ******************************************************************************************** *
 * Implementation of UV390Codeplug::GeneralSettingsElement
 * ******************************************************************************************** */
UV390Codeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr, size_t size)
  : TyTCodeplug::GeneralSettingsElement(ptr, size)
{
  // pass...
}

UV390Codeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr)
  : TyTCodeplug::GeneralSettingsElement(ptr, SETTINGS_SIZE)
{
  // pass...
}

void
UV390Codeplug::GeneralSettingsElement::clear() {
  TyTCodeplug::GeneralSettingsElement::clear();

  setTransmitMode(DESIGNED_AND_HAND_CH);
  enableChannelVoiceAnnounce(false);
  setBit(0x43,0, 1); setBit(0x43,1, 1);
  enableChannelModeA(true);
  setUInt4(0x43,3, 0xf);
  enableChannelModeB(true);

  enableChannelMode(true);

  enableGroupCallMatch(false);
  enablePrivateCallMatch(false);
  setBit(0x6b, 2, 1);
  setTimeZone(QTimeZone::systemTimeZone());

  setChannelHangTime(3000);
  setUInt8(0x91, 0xff); setUInt2(0x92, 0, 0x03);
  enablePublicZone(true);
  setUInt5(0x92, 3, 0x1f);
  setUInt8(0x93, 0xff);
  setAdditionalDMRId(0, 1); setUInt8(0x97, 0);
  setAdditionalDMRId(1, 2); setUInt8(0x9b, 0);
  setAdditionalDMRId(2, 3); setUInt8(0x9f, 0);
  setUInt3(0xa0, 0, 0b111);
  setMICLevel(2);
  enableEditRadioID(true);
  setBit(0xa0, 7, true);
  memset(_data+0xa1, 0xff, 15);
}

UV390Codeplug::GeneralSettingsElement::TransmitMode
UV390Codeplug::GeneralSettingsElement::transmitMode() const {
  return TransmitMode(getUInt2(0x40,6));
}
void
UV390Codeplug::GeneralSettingsElement::setTransmitMode(TransmitMode mode) {
  setUInt2(0x40,6, mode);
}

bool
UV390Codeplug::GeneralSettingsElement::channelVoiceAnnounce() const {
  return getBit(0x42,1);
}
void
UV390Codeplug::GeneralSettingsElement::enableChannelVoiceAnnounce(bool enable) {
  setBit(0x42,1, enable);
}

bool
UV390Codeplug::GeneralSettingsElement::keypadTones() const {
  return getBit(0x42,5);
}
void
UV390Codeplug::GeneralSettingsElement::enableKeypadTones(bool enable) {
  setBit(0x42,5, enable);
}

bool
UV390Codeplug::GeneralSettingsElement::channelModeA() const {
  return getBit(0x43,3);
}
void
UV390Codeplug::GeneralSettingsElement::enableChannelModeA(bool enable) {
  setBit(0x43,3, enable);
}

bool
UV390Codeplug::GeneralSettingsElement::channelModeB() const {
  return getBit(0x43,7);
}
void
UV390Codeplug::GeneralSettingsElement::enableChannelModeB(bool enable) {
  setBit(0x43,7, enable);
}

bool
UV390Codeplug::GeneralSettingsElement::channelMode() const {
  return 0xff == getUInt8(0x57);
}
void
UV390Codeplug::GeneralSettingsElement::enableChannelMode(bool enable) {
  setUInt8(0x57, enable ? 0xff : 0x00);
}

bool
UV390Codeplug::GeneralSettingsElement::groupCallMatch() const {
  return getBit(0x6b, 0);
}
void
UV390Codeplug::GeneralSettingsElement::enableGroupCallMatch(bool enable) {
  setBit(0x6b, 0, enable);
}
bool
UV390Codeplug::GeneralSettingsElement::privateCallMatch() const {
  return getBit(0x6b, 1);
}
void
UV390Codeplug::GeneralSettingsElement::enablePrivateCallMatch(bool enable) {
  setBit(0x6b, 1, enable);
}

QTimeZone
UV390Codeplug::GeneralSettingsElement::timeZone() const {
  return QTimeZone((int(getUInt5(0x6b, 3))-12)*3600);
}
void
UV390Codeplug::GeneralSettingsElement::setTimeZone(const QTimeZone &zone) {
  int idx = (zone.standardTimeOffset(QDateTime::currentDateTime())/3600)+12;
  setUInt5(0x6b, 3, uint8_t(idx));
}

unsigned
UV390Codeplug::GeneralSettingsElement::channelHangTime() const {
  return unsigned(getUInt8(0x90))*100;
}
void
UV390Codeplug::GeneralSettingsElement::setChannelHangTime(unsigned dur) {
  setUInt8(0x90, dur/100);
}

bool
UV390Codeplug::GeneralSettingsElement::publicZone() const {
  return getBit(0x92, 2);
}
void
UV390Codeplug::GeneralSettingsElement::enablePublicZone(bool enable) {
  setBit(0x92, 2, enable);
}

uint32_t
UV390Codeplug::GeneralSettingsElement::additionalDMRId(unsigned n) const {
  return getUInt24_le(0x94+4*n);
}
void
UV390Codeplug::GeneralSettingsElement::setAdditionalDMRId(unsigned n, uint32_t id) {
  setUInt24_le(0x94+4*n, id);
}

unsigned
UV390Codeplug::GeneralSettingsElement::micLevel() const {
  return (unsigned(getUInt3(0xa0,3)+1)*100)/60;
}
void
UV390Codeplug::GeneralSettingsElement::setMICLevel(unsigned level) {
  setUInt3(0xa0,3, ((level-1)*60)/100);
}

bool
UV390Codeplug::GeneralSettingsElement::editRadioID() const {
  return !getBit(0xa0, 6);
}
void
UV390Codeplug::GeneralSettingsElement::enableEditRadioID(bool enable) {
  setBit(0xa0,6, !enable);
}

bool
UV390Codeplug::GeneralSettingsElement::fromConfig(const Config *config) {
  if (! TyTCodeplug::GeneralSettingsElement::fromConfig(config))
    return false;

  setTimeZone(QTimeZone::systemTimeZone());
  setMICLevel(config->settings()->micLevel());
  enableChannelVoiceAnnounce(config->settings()->speech());
  return true;
}

bool
UV390Codeplug::GeneralSettingsElement::updateConfig(Config *config) {
  if (! TyTCodeplug::GeneralSettingsElement::updateConfig(config))
    return false;
  config->settings()->setMicLevel(micLevel());
  config->settings()->enableSpeech(channelVoiceAnnounce());
  return true;
}


/* ******************************************************************************************** *
 * Implementation of UV390Codeplug
 * ******************************************************************************************** */
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
UV390Codeplug::encodeGeneralSettings(Config *config, const Flags &flags, Context &ctx) {
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
UV390Codeplug::encodeChannels(Config *config, const Flags &flags, Context &ctx) {
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
UV390Codeplug::createChannels(Config *config, Context &ctx) {
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
UV390Codeplug::linkChannels(Context &ctx) {
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
UV390Codeplug::clearContacts() {
  // Clear contacts
  for (int i=0; i<NUM_CONTACTS; i++)
    ContactElement(data(ADDR_CONTACTS+i*CONTACT_SIZE)).clear();
}

bool
UV390Codeplug::encodeContacts(Config *config, const Flags &flags, Context &ctx) {
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
UV390Codeplug::createContacts(Config *config, Context &ctx) {
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
UV390Codeplug::clearZones() {
  // Clear zones & zone extensions
  for (int i=0; i<NUM_ZONES; i++) {
    ZoneElement(data(ADDR_ZONES+i*ZONE_SIZE)).clear();
    ZoneExtElement(data(ADDR_ZONEEXTS+i*ZONEEXT_SIZE)).clear();
  }
}

bool
UV390Codeplug::encodeZones(Config *config, const Flags &flags, Context &ctx) {
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
UV390Codeplug::createZones(Config *config, Context &ctx) {
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
UV390Codeplug::linkZones(Context &ctx) {
  for (int i=0; i<NUM_ZONES; i++) {
    ZoneElement zone(data(ADDR_ZONES+i*ZONE_SIZE));
    if (! zone.isValid())
      break;
    if (! zone.linkZone(ctx.get<Zone>(i+1), ctx)) {
      _errorMessage = QString("Cannot decode TyT codeplug: Cannot link zone at index %1.").arg(i);
      logError() << _errorMessage;
      return false;
    }
    ZoneExtElement zoneext(data(ADDR_ZONEEXTS + i*ZONEEXT_SIZE));
    if (! zoneext.linkZoneObj(ctx.get<Zone>(i+1), ctx)) {
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
UV390Codeplug::encodeGroupLists(Config *config, const Flags &flags, Context &ctx) {
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
UV390Codeplug::createGroupLists(Config *config, Context &ctx) {
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
UV390Codeplug::linkGroupLists(Context &ctx) {
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
UV390Codeplug::clearScanLists() {
  // Clear scan lists
  for (int i=0; i<NUM_SCANLISTS; i++)
    ScanListElement(data(ADDR_SCANLISTS + i*SCANLIST_SIZE)).clear();
}

bool
UV390Codeplug::encodeScanLists(Config *config, const Flags &flags, Context &ctx) {
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
UV390Codeplug::createScanLists(Config *config, Context &ctx) {
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
UV390Codeplug::linkScanLists(Context &ctx) {
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
UV390Codeplug::clearPositioningSystems() {
  // Clear GPS systems
  for (int i=0; i<NUM_GPSSYSTEMS; i++)
    GPSSystemElement(data(ADDR_GPSSYSTEMS+i*GPSSYSTEM_SIZE)).clear();
}

bool
UV390Codeplug::encodePositioningSystems(Config *config, const Flags &flags, Context &ctx) {
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
UV390Codeplug::createPositioningSystems(Config *config, Context &ctx) {
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
UV390Codeplug::linkPositioningSystems(Context &ctx) {
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
UV390Codeplug::clearButtonSettings() {
  ButtonSettingsElement(data(ADDR_BUTTONSETTINGS)).clear();
}

bool
UV390Codeplug::encodeButtonSettings(Config *config, const Flags &flags, Context &ctx) {
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
