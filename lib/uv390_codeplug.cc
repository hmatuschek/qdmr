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
  setInCallCriteria(TyTChannelExtension::InCallCriterion::Always);
  setTurnOffFreq(TyTChannelExtension::KillTone::Off);
  setSquelch(1);
  setPower(Channel::Power::High);
  enableAllowInterrupt(true);
  enableDualCapacityDirectMode(false);
  enableDCDMLeader(true);
}

TyTChannelExtension::InCallCriterion UV390Codeplug::ChannelElement::inCallCriteria() const {
  return TyTChannelExtension::InCallCriterion(getUInt2(5,4));
}
void
UV390Codeplug::ChannelElement::setInCallCriteria(TyTChannelExtension::InCallCriterion crit) {
  setUInt2(5,4, uint8_t(crit));
}

TyTChannelExtension::KillTone UV390Codeplug::ChannelElement::turnOffFreq() const {
  return TyTChannelExtension::KillTone(getUInt2(5,6));
}
void
UV390Codeplug::ChannelElement::setTurnOffFreq(TyTChannelExtension::KillTone freq) {
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
UV390Codeplug::ChannelElement::dcdmLeader() const {
  return !getBit(31, 4);
}
void
UV390Codeplug::ChannelElement::enableDCDMLeader(bool enable) {
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

  // assemble extension
  if (TyTChannelExtension *ex = ch->tytChannelExtension()) {
    ex->setKillTone(turnOffFreq());
    ex->setInCallCriterion(inCallCriteria());
    ex->enableAllowInterrupt(allowInterrupt());
    ex->enableDCDM(dualCapacityDirectMode());
    ex->enableDCDMLeader(dcdmLeader());
  }

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

  // apply extensions
  if (TyTChannelExtension *ex = chan->tytChannelExtension()) {
    setTurnOffFreq(ex->killTone());
    setInCallCriteria(ex->inCallCriterion());
    enableAllowInterrupt(ex->allowInterrupt());
    enableDualCapacityDirectMode(ex->dcdm());
    enableDCDMLeader(ex->dcdmLeader());
  }
}


/* ******************************************************************************************** *
 * Implementation of UV390Codeplug::VFOChannelElement
 * ******************************************************************************************** */
UV390Codeplug::VFOChannelElement::VFOChannelElement(uint8_t *ptr, size_t size)
  : ChannelElement(ptr, size)
{
  // pass...
}

UV390Codeplug::VFOChannelElement::VFOChannelElement(uint8_t *ptr)
  : ChannelElement(ptr, CHANNEL_SIZE)
{
  // pass...
}

UV390Codeplug::VFOChannelElement::~VFOChannelElement() {
  // pass...
}

QString
UV390Codeplug::VFOChannelElement::name() const {
  return "";
}
void
UV390Codeplug::VFOChannelElement::setName(const QString &txt) {
  Q_UNUSED(txt)
  // pass...
}

unsigned
UV390Codeplug::VFOChannelElement::stepSize() const {
  return (getUInt8(32)+1)*2500;
}
void
UV390Codeplug::VFOChannelElement::setStepSize(unsigned ss_Hz) {
  ss_Hz = std::min(50000U, std::max(ss_Hz, 2500U));
  setUInt8(32, ss_Hz/2500-1);
  setUInt8(33, 0xff);
}


/* ******************************************************************************************** *
 * Implementation of UV390Codeplug::GeneralSettingsElement
 * ******************************************************************************************** */
UV390Codeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr, size_t size)
  : DM1701Codeplug::GeneralSettingsElement(ptr, size)
{
  // pass...
}

UV390Codeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr)
  : DM1701Codeplug::GeneralSettingsElement(ptr, SETTINGS_SIZE)
{
  // pass...
}

void
UV390Codeplug::GeneralSettingsElement::clear() {
  TyTCodeplug::GeneralSettingsElement::clear();

  setTransmitMode(DESIGNED_AND_HAND_CH);
  enableChannelVoiceAnnounce(false);
  setBit(0x43,0, 1); setBit(0x43,1, 1);
  setUInt4(0x43,3, 0xf);

  setBit(0x6b, 2, 1);

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
  if (! DM1701Codeplug::GeneralSettingsElement::fromConfig(config))
    return false;

  setTimeZone(QTimeZone::systemTimeZone());
  setMICLevel(config->settings()->micLevel());
  enableChannelVoiceAnnounce(config->settings()->speech());
  return true;
}

bool
UV390Codeplug::GeneralSettingsElement::updateConfig(Config *config) {
  if (! DM1701Codeplug::GeneralSettingsElement::updateConfig(config))
    return false;
  config->settings()->setMicLevel(micLevel());
  config->settings()->enableSpeech(channelVoiceAnnounce());
  return true;
}


/* ******************************************************************************************** *
 * Implementation of UV390Codeplug::BootSettingsElement
 * ******************************************************************************************** */
UV390Codeplug::BootSettingsElement::BootSettingsElement(uint8_t *ptr, size_t size)
  : Codeplug::Element(ptr, size)
{
  // pass...
}

UV390Codeplug::BootSettingsElement::BootSettingsElement(uint8_t *ptr)
  : Codeplug::Element(ptr, 0x0010)
{
  // pass...
}

UV390Codeplug::BootSettingsElement::~BootSettingsElement() {
  // pass...
}

void
UV390Codeplug::BootSettingsElement::clear() {
  setUInt24_le(0, 0xffffff);
  setZoneIndex(1);
  setChannelIndexA(1);
  setUInt8(0x05, 0xff);
  setChannelIndexB(1);
  setUInt16_le(0x07, 0xffff);
  setUInt16_le(0x09, 0x0001);
  setUInt8(0x0b, 0xff);
  setUInt32_le(0x0c, 0xffffffff);
}

unsigned
UV390Codeplug::BootSettingsElement::zoneIndex() const {
  return getUInt8(0x03);
}
void
UV390Codeplug::BootSettingsElement::setZoneIndex(unsigned idx) {
  setUInt8(0x03, idx);
}

unsigned
UV390Codeplug::BootSettingsElement::channelIndexA() const {
  return getUInt8(0x04);
}
void
UV390Codeplug::BootSettingsElement::setChannelIndexA(unsigned idx) {
  setUInt8(0x04, idx);
}

unsigned
UV390Codeplug::BootSettingsElement::channelIndexB() const {
  return getUInt8(0x06);
}
void
UV390Codeplug::BootSettingsElement::setChannelIndexB(unsigned idx) {
  setUInt8(0x06, idx);
}


/* ******************************************************************************************** *
 * Implementation of UV390Codeplug::MenuSettingsElement
 * ******************************************************************************************** */
UV390Codeplug::MenuSettingsElement::MenuSettingsElement(uint8_t *ptr, size_t size)
  : TyTCodeplug::MenuSettingsElement(ptr, size)
{
  // pass...
}

UV390Codeplug::MenuSettingsElement::MenuSettingsElement(uint8_t *ptr)
  : TyTCodeplug::MenuSettingsElement(ptr)
{
  // pass...
}

void
UV390Codeplug::MenuSettingsElement::clear() {
  TyTCodeplug::MenuSettingsElement::clear();

  enableGPSSettings(true);
  enableRecording(true);

  enableGroupCallMatch(true);
  enablePrivateCallMatch(true);
  enableMenuHangtimeItem(true);
  enableTXMode(true);
  enableZoneSettings(true);
  enableNewZone(true);

  enableEditZone(true);
  enableNewScanList(true);

  setBit(0x05, 0, true);
  setBit(0x05, 1, true);
  enableGroupCallMatch(true);
  enablePrivateCallMatch(true);
  enableMenuHangtimeItem(true);
  enableTXMode(true);
  enableZoneSettings(true);
  enableNewZone(true);
  enableEditZone(true);
  enableNewScanList(true);
}

bool
UV390Codeplug::MenuSettingsElement::gpsSettings() const {
  return !getBit(0x04, 3);
}
void
UV390Codeplug::MenuSettingsElement::enableGPSSettings(bool enable) {
  setBit(0x04, 3, !enable);
}

bool
UV390Codeplug::MenuSettingsElement::recording() const {
  return getBit(0x04, 5);
}
void
UV390Codeplug::MenuSettingsElement::enableRecording(bool enable) {
  setBit(0x04, 5, enable);
}

bool
UV390Codeplug::MenuSettingsElement::groupCallMatch() const {
  return getBit(0x05, 2);
}
void
UV390Codeplug::MenuSettingsElement::enableGroupCallMatch(bool enable) {
  setBit(0x05, 2, enable);
}

bool
UV390Codeplug::MenuSettingsElement::privateCallMatch() const {
  return getBit(0x05, 3);
}
void
UV390Codeplug::MenuSettingsElement::enablePrivateCallMatch(bool enable) {
  setBit(0x05, 3, enable);
}

bool
UV390Codeplug::MenuSettingsElement::menuHangtimeItem() const {
  return getBit(0x05, 4);
}
void
UV390Codeplug::MenuSettingsElement::enableMenuHangtimeItem(bool enable) {
  setBit(0x05, 4, enable);
}

bool
UV390Codeplug::MenuSettingsElement::txMode() const {
  return getBit(0x05, 5);
}
void
UV390Codeplug::MenuSettingsElement::enableTXMode(bool enable) {
  setBit(0x05, 5, enable);
}

bool
UV390Codeplug::MenuSettingsElement::zoneSettings() const {
  return getBit(0x05, 6);
}
void
UV390Codeplug::MenuSettingsElement::enableZoneSettings(bool enable) {
  setBit(0x05, 6, enable);
}

bool
UV390Codeplug::MenuSettingsElement::newZone() const {
  return getBit(0x05, 7);
}
void
UV390Codeplug::MenuSettingsElement::enableNewZone(bool enable) {
  setBit(0x05, 7, enable);
}

bool
UV390Codeplug::MenuSettingsElement::editZone() const {
  return getBit(0x06, 0);
}
void
UV390Codeplug::MenuSettingsElement::enableEditZone(bool enable) {
  setBit(0x06, 0, enable);
}

bool
UV390Codeplug::MenuSettingsElement::newScanList() const {
  return getBit(0x06, 1);
}
void
UV390Codeplug::MenuSettingsElement::enableNewScanList(bool enable) {
  setBit(0x06, 1, enable);
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
UV390Codeplug::clear() {
  TyTCodeplug::clear();

  clearBootSettings();
  clearVFOSettings();
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
UV390Codeplug::encodeGeneralSettings(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(ctx); Q_UNUSED(err)
  return GeneralSettingsElement(data(ADDR_SETTINGS)).fromConfig(config);
}

bool
UV390Codeplug::decodeGeneralSettings(Config *config, const ErrorStack &err) {
  Q_UNUSED(err)
  return GeneralSettingsElement(data(ADDR_SETTINGS)).updateConfig(config);
}

void
UV390Codeplug::clearChannels() {
  // Clear channels
  for (int i=0; i<NUM_CHANNELS; i++)
    ChannelElement(data(ADDR_CHANNELS+i*CHANNEL_SIZE)).clear();
}

bool
UV390Codeplug::encodeChannels(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
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
UV390Codeplug::createChannels(Config *config, Context &ctx, const ErrorStack &err) {
  for (int i=0; i<NUM_CHANNELS; i++) {
    ChannelElement chan(data(ADDR_CHANNELS+i*CHANNEL_SIZE));
    if (! chan.isValid())
      continue;
    if (Channel *obj = chan.toChannelObj()) {
      config->channelList()->add(obj); ctx.add(obj, i+1);
    } else {
      errMsg(err) << "Invalid channel at index  " << i << ".";
      return false;
    }
  }
  return true;
}

bool
UV390Codeplug::linkChannels(Context &ctx, const ErrorStack &err) {
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
UV390Codeplug::clearContacts() {
  // Clear contacts
  for (int i=0; i<NUM_CONTACTS; i++)
    ContactElement(data(ADDR_CONTACTS+i*CONTACT_SIZE)).clear();
}

bool
UV390Codeplug::encodeContacts(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
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
UV390Codeplug::createContacts(Config *config, Context &ctx, const ErrorStack &err) {
  for (int i=0; i<NUM_CONTACTS; i++) {
    ContactElement cont(data(ADDR_CONTACTS+i*CONTACT_SIZE));
    if (! cont.isValid())
      continue;
    if (DigitalContact *obj = cont.toContactObj()) {
      config->contacts()->add(obj); ctx.add(obj, i+1);
    } else {
      errMsg(err) << "Invalid contact at index " << i << ".";
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
UV390Codeplug::encodeZones(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
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
UV390Codeplug::createZones(Config *config, Context &ctx, const ErrorStack &err) {
  for (int i=0; i<NUM_ZONES; i++) {
    ZoneElement zone(data(ADDR_ZONES+i*ZONE_SIZE));
    if (! zone.isValid())
      continue;
    if (Zone *obj = zone.toZoneObj()) {
      config->zones()->add(obj); ctx.add(obj, i+1);
    } else {
      errMsg(err) << "Invalid zone at index " << i << ".";
      return false;
    }
  }

  return true;
}

bool
UV390Codeplug::linkZones(Context &ctx, const ErrorStack &err) {
  for (int i=0; i<NUM_ZONES; i++) {
    ZoneElement zone(data(ADDR_ZONES+i*ZONE_SIZE));
    if (! zone.isValid())
      continue;
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
UV390Codeplug::clearGroupLists() {
  for (int i=0; i<NUM_GROUPLISTS; i++)
    GroupListElement(data(ADDR_GROUPLISTS+i*GROUPLIST_SIZE)).clear();
}

bool
UV390Codeplug::encodeGroupLists(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
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
UV390Codeplug::createGroupLists(Config *config, Context &ctx, const ErrorStack &err) {
  for (int i=0; i<NUM_GROUPLISTS; i++) {
    GroupListElement glist(data(ADDR_GROUPLISTS+i*GROUPLIST_SIZE));
    if (! glist.isValid())
      continue;
    if (RXGroupList *obj = glist.toGroupListObj(ctx)) {
      config->rxGroupLists()->add(obj); ctx.add(obj, i+1);
    } else {
      errMsg(err) << "Invalid RX group list at index " << i << ".";
      return false;
    }
  }
  return true;
}

bool
UV390Codeplug::linkGroupLists(Context &ctx, const ErrorStack &err) {
  for (int i=0; i<NUM_GROUPLISTS; i++) {
    GroupListElement glist(data(ADDR_GROUPLISTS+i*GROUPLIST_SIZE));
    if (! glist.isValid())
      continue;
    if (! glist.linkGroupListObj(ctx.get<RXGroupList>(i+1), ctx)) {
      errMsg(err) << "Cannot link group-list at index " << i << ".";
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
UV390Codeplug::encodeScanLists(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
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
UV390Codeplug::createScanLists(Config *config, Context &ctx, const ErrorStack &err) {
  for (int i=0; i<NUM_SCANLISTS; i++) {
    ScanListElement scan(data(ADDR_SCANLISTS + i*SCANLIST_SIZE));
    if (! scan.isValid())
      continue;
    if (ScanList *obj = scan.toScanListObj(ctx)) {
      config->scanlists()->add(obj); ctx.add(obj, i+1);
    } else {
      errMsg(err) << "Invalid scan list at index " << i << ".";
      return false;
    }
  }
  return true;
}

bool
UV390Codeplug::linkScanLists(Context &ctx, const ErrorStack &err) {
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
UV390Codeplug::clearPositioningSystems() {
  // Clear GPS systems
  for (int i=0; i<NUM_GPSSYSTEMS; i++)
    GPSSystemElement(data(ADDR_GPSSYSTEMS+i*GPSSYSTEM_SIZE)).clear();
}

bool
UV390Codeplug::encodePositioningSystems(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
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
UV390Codeplug::createPositioningSystems(Config *config, Context &ctx, const ErrorStack &err) {
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
UV390Codeplug::linkPositioningSystems(Context &ctx, const ErrorStack &err) {
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
UV390Codeplug::clearButtonSettings() {
  ButtonSettingsElement(data(ADDR_BUTTONSETTINGS)).clear();
}

bool
UV390Codeplug::encodeButtonSettings(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(ctx); Q_UNUSED(err)
  // Encode settings
  return ButtonSettingsElement(data(ADDR_BUTTONSETTINGS)).fromConfig(config);
}

bool
UV390Codeplug::decodeButtonSetttings(Config *config, const ErrorStack &err) {
  Q_UNUSED(err)
  return ButtonSettingsElement(data(ADDR_BUTTONSETTINGS)).updateConfig(config);
}


void
UV390Codeplug::clearPrivacyKeys() {
  EncryptionElement(data(ADDR_PRIVACY_KEYS)).clear();
}

bool
UV390Codeplug::encodePrivacyKeys(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err);
  // First, reset keys
  clearPrivacyKeys();
  // Get keys
  EncryptionElement keys(data(ADDR_PRIVACY_KEYS));
  return keys.fromCommercialExt(config->commercialExtension(), ctx);
}

bool
UV390Codeplug::decodePrivacyKeys(Config *config, Context &ctx, const ErrorStack &err) {
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
