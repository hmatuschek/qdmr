#include "d878uv_codeplug.hh"
#include "config.hh"
#include "utils.hh"
#include "channel.hh"
#include "gpssystem.hh"
#include "userdatabase.hh"
#include "config.h"
#include "logger.hh"

#include <QTimeZone>
#include <QtEndian>

#define NUM_CHANNELS              4000
#define NUM_CHANNEL_BANKS         32
#define CHANNEL_BANK_0            0x00800000
#define CHANNEL_BANK_SIZE         0x00002000
#define CHANNEL_BANK_31           0x00fc0000
#define CHANNEL_BANK_31_SIZE      0x00000800
#define CHANNEL_BANK_OFFSET       0x00040000
#define CHANNEL_SIZE              0x00000040
#define CHANNEL_BITMAP            0x024c1500
#define CHANNEL_BITMAP_SIZE       0x00000200

#define ADDR_HIDDEN_ZONE_MAP      0x024c1360
#define HIDDEN_ZONE_MAP_SIZE      0x00000020

#define ADDR_GENERAL_CONFIG       0x02500000
#define GENERAL_CONFIG_SIZE       0x00000100

#define ADDR_GENERAL_CONFIG_EXT1  0x02501280
#define GENERAL_CONFIG_EXT1_SIZE  0x00000030

#define ADDR_GENERAL_CONFIG_EXT2  0x02501400
#define GENERAL_CONFIG_EXT2_SIZE  0x00000100

#define ADDR_APRS_SETTING         0x02501000 // Address of APRS settings
#define APRS_SETTING_SIZE         0x00000040 // Size of the APRS settings

#define ADDR_APRS_SET_EXT         0x025010A0 // Address of APRS settings extension
#define APRS_SET_EXT_SIZE         0x00000060 // Size of APRS settings extension

#define ADDR_APRS_MESSAGE         0x02501200 // Address of APRS messages
#define APRS_MESSAGE_SIZE         0x00000040 // Size of APRS messages

#define NUM_APRS_RX_ENTRY         32
#define ADDR_APRS_RX_ENTRY        0x02501800 // Address of APRS RX list
#define APRS_RX_ENTRY_SIZE        0x00000008 // Size of each APRS RX entry

#define NUM_GPS_SYSTEMS           8
#define ADDR_GPS_SETTING          0x02501040 // Address of GPS settings
#define GPS_SETTING_SIZE          0x00000060 // Size of the GPS settings

#define NUM_ROAMING_CHANNEL         250
#define ADDR_ROAMING_CHANNEL_BITMAP 0x01042000
#define ROAMING_CHANNEL_BITMAP_SIZE 0x00000020
#define ADDR_ROAMING_CHANNEL_0      0x01040000
#define ROAMING_CHANNEL_SIZE        0x00000020
#define ROAMING_CHANNEL_OFFSET      0x00000020

#define NUM_ROAMING_ZONES           64
#define NUM_CH_PER_ROAMINGZONE      64
#define ADDR_ROAMING_ZONE_BITMAP    0x01042080
#define ROAMING_ZONE_BITMAP_SIZE    0x00000010
#define ADDR_ROAMING_ZONE_0         0x01043000
#define ROAMING_ZONE_SIZE           0x00000080
#define ROAMING_ZONE_OFFSET         0x00000080

#define NUM_ENCRYPTION_KEYS       256
#define ADDR_ENCRYPTION_KEYS      0x024C4000
#define ENCRYPTION_KEY_SIZE       0x00000040
#define ENCRYPTION_KEYS_SIZE      0x00004000


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::ChannelElement
 * ******************************************************************************************** */
D878UVCodeplug::ChannelElement::ChannelElement(uint8_t *ptr, unsigned size)
  : D868UVCodeplug::ChannelElement(ptr, size)
{
  // pass...
}

D878UVCodeplug::ChannelElement::ChannelElement(uint8_t *ptr)
  : D868UVCodeplug::ChannelElement(ptr, 0x0040)
{
  // pass...
}

void
D878UVCodeplug::ChannelElement::clear() {
  D868UVCodeplug::ChannelElement::clear();
  setPTTIDSetting(PTTId::Off);
}

D878UVCodeplug::ChannelElement::PTTId
D878UVCodeplug::ChannelElement::pttIDSetting() const {
  return (PTTId)getUInt2(0x0019, 0);
}
void
D878UVCodeplug::ChannelElement::setPTTIDSetting(PTTId ptt) {
  setUInt2(0x0019, 0, (unsigned)ptt);
}

bool
D878UVCodeplug::ChannelElement::roamingEnabled() const {
  // inverted
  return !getBit(0x0034, 2);
}
void
D878UVCodeplug::ChannelElement::enableRoaming(bool enable) {
  // inverted
  setBit(0x0034, 2, !enable);
}
bool
D878UVCodeplug::ChannelElement::dataACK() const {
  // inverted
  return !getBit(0x0034, 3);
}
void
D878UVCodeplug::ChannelElement::enableDataACK(bool enable) {
  // inverted
  setBit(0x0034, 3, !enable);
}

bool
D878UVCodeplug::ChannelElement::txDigitalAPRS() const {
  return 2 == getUInt2(0x0035, 0);
}
void
D878UVCodeplug::ChannelElement::enableTXDigitalAPRS(bool enable) {
  setUInt2(0x0035, 0, (enable ? 0x02 : 0x00));
}
bool
D878UVCodeplug::ChannelElement::txAnalogAPRS() const {
  return 1 == getUInt2(0x0035, 0);
}
void
D878UVCodeplug::ChannelElement::enableTXAnalogAPRS(bool enable) {
  setUInt2(0x0035, 0, (enable ? 0x01 : 0x00));
}

D878UVCodeplug::ChannelElement::APRSPTT
D878UVCodeplug::ChannelElement::analogAPRSPTTSetting() const {
  return (APRSPTT)getUInt8(0x0036);
}
void
D878UVCodeplug::ChannelElement::setAnalogAPRSPTTSetting(APRSPTT ptt) {
  setUInt8(0x0036, (unsigned)ptt);
}

D878UVCodeplug::ChannelElement::APRSPTT
D878UVCodeplug::ChannelElement::digitalAPRSPTTSetting() const {
  return (APRSPTT)getUInt8(0x0037);
}
void
D878UVCodeplug::ChannelElement::setDigitalAPRSPTTSetting(APRSPTT ptt) {
  setUInt8(0x0037, (unsigned)ptt);
}

unsigned
D878UVCodeplug::ChannelElement::digitalAPRSSystemIndex() const {
  return getUInt8(0x0038);
}
void
D878UVCodeplug::ChannelElement::setDigitalAPRSSystemIndex(unsigned idx) {
  setUInt8(0x0038, idx);
}

int
D878UVCodeplug::ChannelElement::frequenyCorrection() const {
  return ((int)getInt8(0x0039))*10;
}
void
D878UVCodeplug::ChannelElement::setFrequencyCorrection(int corr) {
  setInt8(0x0039, corr/10);
}

Channel *
D878UVCodeplug::ChannelElement::toChannelObj(Context &ctx) const {
  Channel *ch = D868UVCodeplug::ChannelElement::toChannelObj(ctx);

  if (nullptr == ch)
    return nullptr;

  // Get extensions
  AnytoneChannelExtension *ext = nullptr;
  if (DMRChannel *dch = ch->as<DMRChannel>()) {
    ext = dch->anytoneChannelExtension();
  } else if (FMChannel *fch = ch->as<FMChannel>()){
    ext = fch->anytoneChannelExtension();
  }

  // If extension is present, update
  if (nullptr != ext) {
    ext->setFrequencyCorrection(frequenyCorrection());
  }

  return ch;
}

bool
D878UVCodeplug::ChannelElement::linkChannelObj(Channel *c, Context &ctx) const {
  if (! D868UVCodeplug::ChannelElement::linkChannelObj(c, ctx))
    return false;

  if (c->is<DMRChannel>()) {
    DMRChannel *dc = c->as<DMRChannel>();
    // Link to GPS system
    if (txDigitalAPRS() && ctx.has<GPSSystem>(digitalAPRSSystemIndex()))
      dc->setAPRSObj(ctx.get<GPSSystem>(digitalAPRSSystemIndex()));
    // Link APRS system if one is defined
    //  There can only be one active APRS system, hence the index is fixed to one.
    if (txAnalogAPRS() && ctx.has<APRSSystem>(0))
      dc->setAPRSObj(ctx.get<APRSSystem>(0));
    // If roaming is not disabled -> link to default roaming zone
    if (roamingEnabled())
      dc->setRoamingZone(DefaultRoamingZone::get());
  } else if (c->is<FMChannel>()) {
    FMChannel *ac = c->as<FMChannel>();
    // Link APRS system if one is defined
    //  There can only be one active APRS system, hence the index is fixed to one.
    if (txAnalogAPRS() && ctx.has<APRSSystem>(0))
      ac->setAPRSSystem(ctx.get<APRSSystem>(0));
  }

  return true;
}

bool
D878UVCodeplug::ChannelElement::fromChannelObj(const Channel *c, Context &ctx) {
  if (! D868UVCodeplug::ChannelElement::fromChannelObj(c, ctx))
    return false;

  if (const DMRChannel *dc = c->as<DMRChannel>()) {
    // Set GPS system index
    enableRXAPRS(false);
    if (dc->aprsObj() && dc->aprsObj()->is<GPSSystem>()) {
      enableRXAPRS(true);
      enableTXDigitalAPRS(true);
      setDigitalAPRSSystemIndex(ctx.index(dc->aprsObj()->as<GPSSystem>()));
    } else if (dc->aprsObj() && dc->aprsObj()->is<APRSSystem>()) {
      enableRXAPRS(true);
      enableTXAnalogAPRS(true);
    }
    // Enable roaming
    if (dc->roaming())
      enableRoaming(true);
    // Apply extension settings, if present
    if (AnytoneDMRChannelExtension *ext = dc->anytoneChannelExtension()) {
      setFrequencyCorrection(ext->frequencyCorrection());
      /// Handles bug in AnyTone firmware.
      /// @todo Remove once fixed by AnyTone.
      enableRXAPRS(! ext->sms());
    }
  } else if (const FMChannel *ac = c->as<FMChannel>()) {
    // Set APRS system
    enableRXAPRS(false);
    if (nullptr != ac->aprsSystem()) {
      enableTXAnalogAPRS(true);
      enableRXAPRS(true);
    }
    // Apply extension settings
    if (AnytoneFMChannelExtension *ext = ac->anytoneChannelExtension()) {
      setFrequencyCorrection(ext->frequencyCorrection());
    }
  }

  return true;
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::RoamingChannelElement
 * ******************************************************************************************** */
D878UVCodeplug::RoamingChannelElement::RoamingChannelElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

D878UVCodeplug::RoamingChannelElement::RoamingChannelElement(uint8_t *ptr)
  : Element(ptr, 0x0020)
{
  // pass...
}

void
D878UVCodeplug::RoamingChannelElement::clear() {
  memset(_data, 0x00, _size);
}

unsigned
D878UVCodeplug::RoamingChannelElement::rxFrequency() const {
  return getBCD8_be(Offsets::RXFrequency)*10;
}
void
D878UVCodeplug::RoamingChannelElement::setRXFrequency(unsigned hz) {
  setBCD8_be(Offsets::RXFrequency, hz/10);
}
unsigned
D878UVCodeplug::RoamingChannelElement::txFrequency() const {
  return getBCD8_be(Offsets::TXFrequency)*10;
}
void
D878UVCodeplug::RoamingChannelElement::setTXFrequency(unsigned hz) {
  setBCD8_be(Offsets::TXFrequency, hz/10);
}

bool
D878UVCodeplug::RoamingChannelElement::hasColorCode() const {
  return ColorCodeValue::Disabled == getUInt8(Offsets::ColorCode);
}
unsigned
D878UVCodeplug::RoamingChannelElement::colorCode() const {
  return std::min(15u, (unsigned)getUInt8(Offsets::ColorCode));
}
void
D878UVCodeplug::RoamingChannelElement::setColorCode(unsigned cc) {
  setUInt8(Offsets::ColorCode, cc);
}
void
D878UVCodeplug::RoamingChannelElement::disableColorCode() {
  setUInt8(Offsets::ColorCode, ColorCodeValue::Disabled);
}

DMRChannel::TimeSlot
D878UVCodeplug::RoamingChannelElement::timeSlot() const {
  switch (getUInt8(Offsets::TimeSlot)) {
  case TimeSlotValue::TS1: return DMRChannel::TimeSlot::TS1;
  case TimeSlotValue::TS2: return DMRChannel::TimeSlot::TS2;
  }
  return DMRChannel::TimeSlot::TS1;
}
void
D878UVCodeplug::RoamingChannelElement::setTimeSlot(DMRChannel::TimeSlot ts) {
  switch (ts) {
  case DMRChannel::TimeSlot::TS1: setUInt8(Offsets::TimeSlot, TimeSlotValue::TS1); break;
  case DMRChannel::TimeSlot::TS2: setUInt8(Offsets::TimeSlot, TimeSlotValue::TS2); break;
  }
}

QString
D878UVCodeplug::RoamingChannelElement::name() const {
  return readASCII(Offsets::Name, Offsets::NameLength, 0x00);
}
void
D878UVCodeplug::RoamingChannelElement::setName(const QString &name) {
  writeASCII(Offsets::Name, name, Offsets::NameLength, 0x00);
}

bool
D878UVCodeplug::RoamingChannelElement::fromChannel(const RoamingChannel* ch) {
  setName(ch->name());
  setRXFrequency(ch->rxFrequency()*1e6);
  setTXFrequency(ch->txFrequency()*1e6);
  if (ch->colorCodeOverridden())
    setColorCode(ch->colorCode());
  else
    disableColorCode();
  setTimeSlot(ch->timeSlot());
  return true;
}

RoamingChannel *
D878UVCodeplug::RoamingChannelElement::toChannel(Context &ctx) {
  RoamingChannel *roam = new RoamingChannel();
  roam->setName(name());
  roam->setRXFrequency(rxFrequency()/1e6);
  roam->setTXFrequency(txFrequency()/1e6);
  if (hasColorCode())
    roam->setColorCode(colorCode());
  else
    roam->overrideColorCode(false);
  roam->overrideTimeSlot(true);
  roam->setTimeSlot(timeSlot());

  ctx.config()->roamingChannels()->add(roam);

  return roam;
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::RoamingZoneElement
 * ******************************************************************************************** */
D878UVCodeplug::RoamingZoneElement::RoamingZoneElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

D878UVCodeplug::RoamingZoneElement::RoamingZoneElement(uint8_t *ptr)
  : Element(ptr, 0x0080)
{
  // pass...
}

void
D878UVCodeplug::RoamingZoneElement::clear() {
  memset(_data, 0x00, _size);
  memset(_data+0x000, 0xff, NUM_CH_PER_ROAMINGZONE);
}

bool
D878UVCodeplug::RoamingZoneElement::hasMember(unsigned n) const {
  return (0xff != member(n));
}
unsigned
D878UVCodeplug::RoamingZoneElement::member(unsigned n) const {
  return getUInt8(0x0000 + n);
}
void
D878UVCodeplug::RoamingZoneElement::setMember(unsigned n, unsigned idx) {
  setUInt8(0x0000 + n, idx);
}
void
D878UVCodeplug::RoamingZoneElement::clearMember(unsigned n) {
  setMember(n, 0xff);
}

QString
D878UVCodeplug::RoamingZoneElement::name() const {
  return readASCII(0x0040, 16, 0x00);
}
void
D878UVCodeplug::RoamingZoneElement::setName(const QString &name) {
  writeASCII(0x0040, name, 16, 0x00);
}

bool
D878UVCodeplug::RoamingZoneElement::fromRoamingZone(
    RoamingZone *zone, Context &ctx)
{
  clear();
  setName(zone->name());
  for (int i=0; i<std::min(NUM_CH_PER_ROAMINGZONE, zone->count()); i++) {
    setMember(i, ctx.index(zone->channel(i)));
  }
  return true;
}

RoamingZone *
D878UVCodeplug::RoamingZoneElement::toRoamingZone() const {
  return new RoamingZone(name());
}

bool
D878UVCodeplug::RoamingZoneElement::linkRoamingZone(RoamingZone *zone, Context &ctx)
{
  for (uint8_t i=0; (i<NUM_CH_PER_ROAMINGZONE)&&hasMember(i); i++) {
    if (ctx.has<RoamingChannel>(i))
      zone->addChannel(ctx.get<RoamingChannel>(i));
  }
  return true;
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::GeneralSettingsElement
 * ******************************************************************************************** */
D878UVCodeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr, unsigned size)
  : AnytoneCodeplug::GeneralSettingsElement(ptr, size)
{
  // pass...
}

D878UVCodeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr)
  : AnytoneCodeplug::GeneralSettingsElement(ptr, 0x0100)
{
  // pass...
}

void
D878UVCodeplug::GeneralSettingsElement::clear() {
  AnytoneCodeplug::GeneralSettingsElement::clear();
}

unsigned
D878UVCodeplug::GeneralSettingsElement::transmitTimeout() const {
  return ((unsigned)getUInt8(0x0004))*30;
}
void
D878UVCodeplug::GeneralSettingsElement::setTransmitTimeout(unsigned tot) {
  setUInt8(0x0004, tot/30);
}

D878UVCodeplug::GeneralSettingsElement::Language
D878UVCodeplug::GeneralSettingsElement::language() const {
  return (Language)getUInt8(0x0005);
}
void
D878UVCodeplug::GeneralSettingsElement::setLanguage(Language lang) {
  setUInt8(0x0005, (unsigned)lang);
}

double
D878UVCodeplug::GeneralSettingsElement::vfoFrequencyStep() const {
  switch (getUInt8(0x0008)) {
  case FREQ_STEP_2_5kHz: return 2.5;
  case FREQ_STEP_5kHz: return 5;
  case FREQ_STEP_6_25kHz: return 6.25;
  case FREQ_STEP_10kHz: return 10;
  case FREQ_STEP_12_5kHz: return 12.5;
  case FREQ_STEP_20kHz: return 20;
  case FREQ_STEP_25kHz: return 25;
  case FREQ_STEP_50kHz: return 50;
  }
  return 2.5;
}
void
D878UVCodeplug::GeneralSettingsElement::setVFOFrequencyStep(double kHz) {
  if (kHz <= 2.5)
    setUInt8(0x0008, FREQ_STEP_2_5kHz);
  else if (kHz <= 5)
    setUInt8(0x0008, FREQ_STEP_5kHz);
  else if (kHz <= 6.25)
    setUInt8(0x0008, FREQ_STEP_6_25kHz);
  else if (kHz <= 10)
    setUInt8(0x0008, FREQ_STEP_10kHz);
  else if (kHz <= 12.5)
    setUInt8(0x0008, FREQ_STEP_12_5kHz);
  else if (kHz <= 20)
    setUInt8(0x0008, FREQ_STEP_20kHz);
  else if (kHz <= 25)
    setUInt8(0x0008, FREQ_STEP_25kHz);
  else
    setUInt8(0x0008, FREQ_STEP_50kHz);
}

D878UVCodeplug::GeneralSettingsElement::STEType
D878UVCodeplug::GeneralSettingsElement::steType() const {
  return (STEType)getUInt8(0x0017);
}
void
D878UVCodeplug::GeneralSettingsElement::setSTEType(STEType type) {
  setUInt8(0x0017, (unsigned)type);
}
D878UVCodeplug::GeneralSettingsElement::STEFrequency
D878UVCodeplug::GeneralSettingsElement::steFrequency() const {
  return (STEFrequency)getUInt8(0x0018);
}
void
D878UVCodeplug::GeneralSettingsElement::setSTEFrequency(STEFrequency freq) {
  setUInt8(0x0018, (unsigned)freq);
}

unsigned
D878UVCodeplug::GeneralSettingsElement::groupCallHangTime() const {
  return getUInt8(0x0019);
}
void
D878UVCodeplug::GeneralSettingsElement::setGroupCallHangTime(unsigned sec) {
  setUInt8(0x0019, sec);
}
unsigned
D878UVCodeplug::GeneralSettingsElement::privateCallHangTime() const {
  return getUInt8(0x001a);
}
void
D878UVCodeplug::GeneralSettingsElement::setPrivateCallHangTime(unsigned sec) {
  setUInt8(0x001a, sec);
}
unsigned
D878UVCodeplug::GeneralSettingsElement::preWaveDelay() const {
  return ((unsigned)getUInt8(0x001b))*20;
}
void
D878UVCodeplug::GeneralSettingsElement::setPreWaveDelay(unsigned ms) {
  setUInt8(0x001b, ms/20);
}
unsigned
D878UVCodeplug::GeneralSettingsElement::wakeHeadPeriod() const {
  return ((unsigned)getUInt8(0x001c))*20;
}
void
D878UVCodeplug::GeneralSettingsElement::setWakeHeadPeriod(unsigned ms) {
  setUInt8(0x001c, ms/20);
}

unsigned
D878UVCodeplug::GeneralSettingsElement::wfmChannelIndex() const {
  return getUInt8(0x001d);
}
void
D878UVCodeplug::GeneralSettingsElement::setWFMChannelIndex(unsigned idx) {
  setUInt8(0x001d, idx);
}
bool
D878UVCodeplug::GeneralSettingsElement::wfmVFOEnabled() const {
  return getUInt8(0x001e);
}
void
D878UVCodeplug::GeneralSettingsElement::enableWFMVFO(bool enable) {
  setUInt8(0x001e, (enable ? 0x01 : 0x00));
}

unsigned
D878UVCodeplug::GeneralSettingsElement::dtmfToneDuration() const {
  switch (getUInt8(0x0023)) {
  case DTMF_DUR_50ms:  return 50;
  case DTMF_DUR_100ms: return 100;
  case DTMF_DUR_200ms: return 200;
  case DTMF_DUR_300ms: return 300;
  case DTMF_DUR_500ms: return 500;
  }
  return 50;
}
void
D878UVCodeplug::GeneralSettingsElement::setDTMFToneDuration(unsigned ms) {
  if (ms<=50) {
    setUInt8(0x0023, DTMF_DUR_50ms);
  } else if (ms<=100) {
    setUInt8(0x0023, DTMF_DUR_100ms);
  } else if (ms<=200) {
    setUInt8(0x0023, DTMF_DUR_200ms);
  } else if (ms<=300) {
    setUInt8(0x0023, DTMF_DUR_300ms);
  } else {
    setUInt8(0x0023, DTMF_DUR_500ms);
  }
}

bool
D878UVCodeplug::GeneralSettingsElement::manDown() const {
  return getUInt8(0x0024);
}
void
D878UVCodeplug::GeneralSettingsElement::enableManDown(bool enable) {
  setUInt8(0x0024, (enable ? 0x01 : 0x00));
}

bool
D878UVCodeplug::GeneralSettingsElement::wfmMonitor() const {
  return getUInt8(0x002b);
}
void
D878UVCodeplug::GeneralSettingsElement::enableWFMMonitor(bool enable) {
  setUInt8(0x002b, (enable ? 0x01 : 0x00));
}

D878UVCodeplug::GeneralSettingsElement::TBSTFrequency
D878UVCodeplug::GeneralSettingsElement::tbstFrequency() const {
  return (TBSTFrequency)getUInt8(0x002e);
}
void
D878UVCodeplug::GeneralSettingsElement::setTBSTFrequency(TBSTFrequency freq) {
  setUInt8(0x002e, (unsigned)freq);
}

bool
D878UVCodeplug::GeneralSettingsElement::proMode() const {
  return getUInt8(0x0034);
}
void
D878UVCodeplug::GeneralSettingsElement::enableProMode(bool enable) {
  setUInt8(0x0034, (enable ? 0x01 : 0x00));
}

bool
D878UVCodeplug::GeneralSettingsElement::filterOwnID() const {
  return getUInt8(0x0038);
}
void
D878UVCodeplug::GeneralSettingsElement::enableFilterOwnID(bool enable) {
  setUInt8(0x0038, (enable ? 0x01 : 0x00));
}

bool
D878UVCodeplug::GeneralSettingsElement::remoteStunKill() const {
  return getUInt8(0x003c);
}
void
D878UVCodeplug::GeneralSettingsElement::enableRemoteStunKill(bool enable) {
  setUInt8(0x003c, (enable ? 0x01 : 0x00));
}

bool
D878UVCodeplug::GeneralSettingsElement::remoteMonitor() const {
  return getUInt8(0x003e);
}
void
D878UVCodeplug::GeneralSettingsElement::enableRemoteMonitor(bool enable) {
  setUInt8(0x003e, (enable ? 0x01 : 0x00));
}

D878UVCodeplug::GeneralSettingsElement::SlotMatch
D878UVCodeplug::GeneralSettingsElement::monitorSlotMatch() const {
  return (SlotMatch)getUInt8(0x0049);
}
void
D878UVCodeplug::GeneralSettingsElement::setMonitorSlotMatch(SlotMatch match) {
  setUInt8(0x0049, (unsigned)match);
}

bool
D878UVCodeplug::GeneralSettingsElement::monitorColorCodeMatch() const {
  return getUInt8(0x004a);
}
void
D878UVCodeplug::GeneralSettingsElement::enableMonitorColorCodeMatch(bool enable) {
  setUInt8(0x004a, (enable ? 0x01 : 0x00));
}

bool
D878UVCodeplug::GeneralSettingsElement::monitorIDMatch() const {
  return getUInt8(0x004b);
}
void
D878UVCodeplug::GeneralSettingsElement::enableMonitorIDMatch(bool enable) {
  setUInt8(0x004b, (enable ? 0x01 : 0x00));
}

bool
D878UVCodeplug::GeneralSettingsElement::monitorTimeSlotHold() const {
  return getUInt8(0x004c);
}
void
D878UVCodeplug::GeneralSettingsElement::enableMonitorTimeSlotHold(bool enable) {
  setUInt8(0x004c, (enable ? 0x01 : 0x00));
}

unsigned
D878UVCodeplug::GeneralSettingsElement::manDownDelay() const {
  return getUInt8(0x004f);
}
void
D878UVCodeplug::GeneralSettingsElement::setManDownDelay(unsigned sec) {
  setUInt8(0x004f, sec);
}

unsigned
D878UVCodeplug::GeneralSettingsElement::analogCallHold() const {
  return getUInt8(0x0050);
}
void
D878UVCodeplug::GeneralSettingsElement::setAnalogCallHold(unsigned sec) {
  setUInt8(0x0050, sec);
}

bool
D878UVCodeplug::GeneralSettingsElement::gpsRangReporting() const {
  return getUInt8(0x0053);
}
void
D878UVCodeplug::GeneralSettingsElement::enableGPSRangeReporting(bool enable) {
  setUInt8(0x0053, (enable ? 0x01 : 0x00));
}

bool
D878UVCodeplug::GeneralSettingsElement::maintainCallChannel() const {
  return getUInt8(0x006e);
}
void
D878UVCodeplug::GeneralSettingsElement::enableMaintainCalLChannel(bool enable) {
  setUInt8(0x0063, (enable ? 0x01 : 0x00));
}

unsigned
D878UVCodeplug::GeneralSettingsElement::priorityZoneAIndex() const {
  return getUInt8(0x006f);
}
void
D878UVCodeplug::GeneralSettingsElement::setPriorityZoneAIndex(unsigned idx) {
  setUInt8(0x006f, idx);
}
unsigned
D878UVCodeplug::GeneralSettingsElement::priorityZoneBIndex() const {
  return getUInt8(0x0070);
}
void
D878UVCodeplug::GeneralSettingsElement::setPriorityZoneBIndex(unsigned idx) {
  setUInt8(0x0070, idx);
}

unsigned
D878UVCodeplug::GeneralSettingsElement::gpsRangingInterval() const {
  return getUInt8(0x00b5);
}
void
D878UVCodeplug::GeneralSettingsElement::setGPSRangingInterval(unsigned sec) {
  setUInt8(0x00b5, sec);
}

bool
D878UVCodeplug::GeneralSettingsElement::displayChannelNumber() const {
  return getUInt8(0x00b8);
}
void
D878UVCodeplug::GeneralSettingsElement::enableDisplayChannelNumber(bool enable) {
  setUInt8(0x00b8, (enable ? 0x01 : 0x00));
}

bool
D878UVCodeplug::GeneralSettingsElement::displayContact() const {
  return getUInt8(0x00b9);
}
void
D878UVCodeplug::GeneralSettingsElement::enableDisplayContact(bool enable) {
  setUInt8(0x00b8, (enable ? 0x01 : 0x00));
}

unsigned
D878UVCodeplug::GeneralSettingsElement::autoRoamPeriod() const {
  return getUInt8(0x00ba);
}
void
D878UVCodeplug::GeneralSettingsElement::setAutoRoamPeriod(unsigned min) {
  setUInt8(0x00ba, min);
}

bool
D878UVCodeplug::GeneralSettingsElement::keyToneLevelAdjustable() const {
  return 0 == keyToneLevel();
}
unsigned
D878UVCodeplug::GeneralSettingsElement::keyToneLevel() const {
  return ((unsigned)getUInt8(0x00bb))*10/15;
}
void
D878UVCodeplug::GeneralSettingsElement::setKeyToneLevel(unsigned level) {
  setUInt8(0x00bb, level*10/15);
}
void
D878UVCodeplug::GeneralSettingsElement::setKeyToneLevelAdjustable() {
  setUInt8(0x00bb, 0);
}

D878UVCodeplug::GeneralSettingsElement::Color
D878UVCodeplug::GeneralSettingsElement::callDisplayColor() const {
  return (Color)getUInt8(0x00bc);
}
void
D878UVCodeplug::GeneralSettingsElement::setCallDisplayColor(Color color) {
  setUInt8(0x00bc, (unsigned)color);
}

bool
D878UVCodeplug::GeneralSettingsElement::gpsUnitsImperial() const {
  return getUInt8(0x00bd);
}
void
D878UVCodeplug::GeneralSettingsElement::enableGPSUnitsImperial(bool enable) {
  setUInt8(0x00bd, (enable ? 0x01 : 0x00));
}

bool
D878UVCodeplug::GeneralSettingsElement::knobLock() const {
  return getBit(0x00be, 0);
}
void
D878UVCodeplug::GeneralSettingsElement::enableKnobLock(bool enable) {
  setBit(0x00be, 0, enable);
}
bool
D878UVCodeplug::GeneralSettingsElement::keypadLock() const {
  return getBit(0x00be, 1);
}
void
D878UVCodeplug::GeneralSettingsElement::enableKeypadLock(bool enable) {
  setBit(0x00be, 1, enable);
}
bool
D878UVCodeplug::GeneralSettingsElement::sidekeysLock() const {
  return getBit(0x00be, 3);
}
void
D878UVCodeplug::GeneralSettingsElement::enableSidekeysLock(bool enable) {
  setBit(0x00be, 3, enable);
}
bool
D878UVCodeplug::GeneralSettingsElement::keyLockForced() const {
  return getBit(0x00be, 4);
}
void
D878UVCodeplug::GeneralSettingsElement::enableKeyLockForced(bool enable) {
  setBit(0x00be, 4, enable);
}

unsigned
D878UVCodeplug::GeneralSettingsElement::autoRoamDelay() const {
  return getUInt8(0x00bf);
}
void
D878UVCodeplug::GeneralSettingsElement::setAutoRoamDelay(unsigned sec) {
  setUInt8(0x00bf, sec);
}

D878UVCodeplug::GeneralSettingsElement::Color
D878UVCodeplug::GeneralSettingsElement::standbyTextColor() const {
  return (Color)getUInt8(0x00c0);
}
void
D878UVCodeplug::GeneralSettingsElement::setStandbyTextColor(Color color) {
  setUInt8(0x00c0, (unsigned)color);
}

D878UVCodeplug::GeneralSettingsElement::Color
D878UVCodeplug::GeneralSettingsElement::standbyImageColor() const {
  return (Color)getUInt8(0x00c1);
}
void
D878UVCodeplug::GeneralSettingsElement::setStandbyImageColor(Color color) {
  setUInt8(0x00c1, (unsigned)color);
}

bool
D878UVCodeplug::GeneralSettingsElement::showLastHeard() const {
  return getUInt8(0x00c2);
}
void
D878UVCodeplug::GeneralSettingsElement::enableShowLastHeard(bool enable) {
  setUInt8(0x00c2, (enable ? 0x01 : 0x00));
}

D878UVCodeplug::GeneralSettingsElement::SMSFormat
D878UVCodeplug::GeneralSettingsElement::smsFormat() const {
  return (SMSFormat) getUInt8(0x00c3);
}
void
D878UVCodeplug::GeneralSettingsElement::setSMSFormat(SMSFormat fmt) {
  setUInt8(0x00c3, (unsigned)fmt);
}

unsigned
D878UVCodeplug::GeneralSettingsElement::autoRepeaterMinFrequencyVHF() const {
  return getBCD8_be(0x00c4)*10;
}
void
D878UVCodeplug::GeneralSettingsElement::setAutoRepeaterMinFrequencyVHF(unsigned Hz) {
  setBCD8_be(0x00c4, Hz/10);
}
unsigned
D878UVCodeplug::GeneralSettingsElement::autoRepeaterMaxFrequencyVHF() const {
  return getBCD8_be(0x00c8)*10;
}
void
D878UVCodeplug::GeneralSettingsElement::setAutoRepeaterMaxFrequencyVHF(unsigned Hz) {
  setBCD8_be(0x00c8, Hz/10);
}

unsigned
D878UVCodeplug::GeneralSettingsElement::autoRepeaterMinFrequencyUHF() const {
  return getBCD8_be(0x00cc)*10;
}
void
D878UVCodeplug::GeneralSettingsElement::setAutoRepeaterMinFrequencyUHF(unsigned Hz) {
  setBCD8_be(0x00cc, Hz/10);
}
unsigned
D878UVCodeplug::GeneralSettingsElement::autoRepeaterMaxFrequencyUHF() const {
  return getBCD8_be(0x00d0)*10;
}
void
D878UVCodeplug::GeneralSettingsElement::setAutoRepeaterMaxFrequencyUHF(unsigned Hz) {
  setBCD8_be(0x00d0, Hz/10);
}

D878UVCodeplug::GeneralSettingsElement::AutoRepDir
D878UVCodeplug::GeneralSettingsElement::autoRepeaterDirectionB() const {
  return (AutoRepDir)getUInt8(0x00d4);
}
void
D878UVCodeplug::GeneralSettingsElement::setAutoRepeaterDirectionB(AutoRepDir dir) {
  setUInt8(0x00d4, (unsigned)dir);
}

bool
D878UVCodeplug::GeneralSettingsElement::defaultChannel() const {
  return getUInt8(0x00d7);
}
void
D878UVCodeplug::GeneralSettingsElement::enableDefaultChannel(bool enable) {
  setUInt8(0x00d7, (enable ? 0x01 : 0x00));
}

unsigned
D878UVCodeplug::GeneralSettingsElement::defaultZoneIndexA() const {
  return getUInt8(0x00d8);
}
void
D878UVCodeplug::GeneralSettingsElement::setDefaultZoneIndexA(unsigned idx) {
  setUInt8(0x00d8, idx);
}

unsigned
D878UVCodeplug::GeneralSettingsElement::defaultZoneIndexB() const {
  return getUInt8(0x00d9);
}
void
D878UVCodeplug::GeneralSettingsElement::setDefaultZoneIndexB(unsigned idx) {
  setUInt8(0x00d9, idx);
}

bool
D878UVCodeplug::GeneralSettingsElement::defaultChannelAIsVFO() const {
  return 0xff == defaultChannelAIndex();
}
unsigned
D878UVCodeplug::GeneralSettingsElement::defaultChannelAIndex() const {
  return getUInt8(0x00da);
}
void
D878UVCodeplug::GeneralSettingsElement::setDefaultChannelAIndex(unsigned idx) {
  setUInt8(0x00da, idx);
}
void
D878UVCodeplug::GeneralSettingsElement::setDefaultChannelAToVFO() {
  setDefaultChannelAIndex(0xff);
}

bool
D878UVCodeplug::GeneralSettingsElement::defaultChannelBIsVFO() const {
  return 0xff == defaultChannelBIndex();
}
unsigned
D878UVCodeplug::GeneralSettingsElement::defaultChannelBIndex() const {
  return getUInt8(0x00db);
}
void
D878UVCodeplug::GeneralSettingsElement::setDefaultChannelBIndex(unsigned idx) {
  setUInt8(0x00db, idx);
}
void
D878UVCodeplug::GeneralSettingsElement::setDefaultChannelBToVFO() {
  setDefaultChannelBIndex(0xff);
}

unsigned
D878UVCodeplug::GeneralSettingsElement::defaultRoamingZoneIndex() const {
  return getUInt8(0x00dc);
}
void
D878UVCodeplug::GeneralSettingsElement::setDefaultRoamingZoneIndex(unsigned idx) {
  setUInt8(0x00dc, idx);
}

bool
D878UVCodeplug::GeneralSettingsElement::repeaterRangeCheck() const {
  return getUInt8(0x00dd);
}
void
D878UVCodeplug::GeneralSettingsElement::enableRepeaterRangeCheck(bool enable) {
  setUInt8(0x00dd, (enable ? 0x01 : 0x00));
}

unsigned
D878UVCodeplug::GeneralSettingsElement::repeaterRangeCheckInterval() const {
  return ((unsigned)getUInt8(0x00de))*5;
}
void
D878UVCodeplug::GeneralSettingsElement::setRepeaterRangeCheckInterval(unsigned sec) {
  setUInt8(0x00de, sec/5);
}

unsigned
D878UVCodeplug::GeneralSettingsElement::repeaterRangeCheckCount() const {
  return getUInt8(0x00df);
}
void
D878UVCodeplug::GeneralSettingsElement::setRepeaterRangeCheckCount(unsigned n) {
  setUInt8(0x00df, n);
}

D878UVCodeplug::GeneralSettingsElement::RoamStart
D878UVCodeplug::GeneralSettingsElement::roamingStartCondition() const {
  return (RoamStart)getUInt8(0x00e0);
}
void
D878UVCodeplug::GeneralSettingsElement::setRoamingStartCondition(RoamStart cond) {
  setUInt8(0x00e0, (unsigned)cond);
}

unsigned
D878UVCodeplug::GeneralSettingsElement::backlightTXDuration() const {
  return getUInt8(0x00e1);
}
void
D878UVCodeplug::GeneralSettingsElement::setBacklightTXDuration(unsigned sec) {
  setUInt8(0x00e1, sec);
}

bool
D878UVCodeplug::GeneralSettingsElement::separateDisplay() const {
  return getUInt8(0x00e2);
}
void
D878UVCodeplug::GeneralSettingsElement::enableSeparateDisplay(bool enable) {
  setUInt8(0x00e2, (enable ? 0x01 : 0x00));
}

bool
D878UVCodeplug::GeneralSettingsElement::keepCaller() const {
  return getUInt8(0x00e3);
}
void
D878UVCodeplug::GeneralSettingsElement::enableKeepCaller(bool enable) {
  setUInt8(0x00e3, (enable ? 0x01 : 0x00));
}

D878UVCodeplug::GeneralSettingsElement::Color
D878UVCodeplug::GeneralSettingsElement::channelNameColor() const {
  return (Color) getUInt8(0x00e4);
}
void
D878UVCodeplug::GeneralSettingsElement::setChannelNameColor(Color color) {
  setUInt8(0x00e4, (unsigned)color);
}

bool
D878UVCodeplug::GeneralSettingsElement::repeaterCheckNotification() const {
  return getUInt8(0x00e5);
}
void
D878UVCodeplug::GeneralSettingsElement::enableRepeaterCheckNotification(bool enable) {
  setUInt8(0x00e5, (enable ? 0x01 : 0x00));
}


unsigned
D878UVCodeplug::GeneralSettingsElement::backlightRXDuration() const {
  return getUInt8(0x00e6);
}
void
D878UVCodeplug::GeneralSettingsElement::setBacklightRXDuration(unsigned sec) {
  setUInt8(0x00e6, sec);
}

bool
D878UVCodeplug::GeneralSettingsElement::roaming() const {
  return getUInt8(0x00e7);
}
void
D878UVCodeplug::GeneralSettingsElement::enableRoaming(bool enable) {
  setUInt8(0x00e7, (enable ? 0x01 : 0x00));
}

unsigned
D878UVCodeplug::GeneralSettingsElement::muteDelay() const {
  return getUInt8(0x00e9)+1;
}
void
D878UVCodeplug::GeneralSettingsElement::setMuteDelay(unsigned min) {
  if (1>min) min = 1;
  setUInt8(0x00e9, min-1);
}

unsigned
D878UVCodeplug::GeneralSettingsElement::repeaterCheckNumNotifications() const {
  return getUInt8(0x00ea);
}
void
D878UVCodeplug::GeneralSettingsElement::setRepeaterCheckNumNotifications(unsigned num) {
  setUInt8(0x00ea, num);
}

bool
D878UVCodeplug::GeneralSettingsElement::bootGPSCheck() const {
  return getUInt8(0x00eb);
}
void
D878UVCodeplug::GeneralSettingsElement::enableBootGPSCheck(bool enable) {
  setUInt8(0x00eb, (enable ? 0x01 : 0x00));
}

bool
D878UVCodeplug::GeneralSettingsElement::bootReset() const {
  return getUInt8(0x00ec);
}
void
D878UVCodeplug::GeneralSettingsElement::enableBootReset(bool enable) {
  setUInt8(0x00ec, (enable ? 0x01 : 0x00));
}

bool
D878UVCodeplug::GeneralSettingsElement::fromConfig(const Flags &flags, Context &ctx) {
  if (! AnytoneCodeplug::GeneralSettingsElement::fromConfig(flags, ctx))
    return false;

  // Set measurement system based on system locale (0x00==Metric)
  enableGPSUnitsImperial(QLocale::ImperialSystem == QLocale::system().measurementSystem());
  // Set transmit timeout
  setTransmitTimeout(ctx.config()->settings()->tot());

  return true;
}

bool
D878UVCodeplug::GeneralSettingsElement::updateConfig(Context &ctx) {
  if (! AnytoneCodeplug::GeneralSettingsElement::updateConfig(ctx))
    return false;
  ctx.config()->settings()->setTOT(transmitTimeout());
  return true;
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::GPSMessageElement
 * ******************************************************************************************** */
D878UVCodeplug::GPSMessageElement::GPSMessageElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

D878UVCodeplug::GPSMessageElement::GPSMessageElement(uint8_t *ptr)
  : Element(ptr, 0x0030)
{
  // pass...
}

void
D878UVCodeplug::GPSMessageElement::clear() {
  memset(_data, 0x00, _size);
}

QString
D878UVCodeplug::GPSMessageElement::message() const {
  return readASCII(0x0000, 32, 0x00);
}
void
D878UVCodeplug::GPSMessageElement::setMessage(const QString &message) {
  writeASCII(0x0000, message, 32, 0x00);
}

bool
D878UVCodeplug::GPSMessageElement::fromConfig(const Flags &flags, Context &ctx) {
  Q_UNUSED(flags); Q_UNUSED(ctx)
  return true;
}

bool
D878UVCodeplug::GPSMessageElement::updateConfig(Context &ctx) const {
  Q_UNUSED(ctx)
  return true;
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::GeneralSettingsExtensionElement
 * ******************************************************************************************** */
D878UVCodeplug::GeneralSettingsExtensionElement::GeneralSettingsExtensionElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

D878UVCodeplug::GeneralSettingsExtensionElement::GeneralSettingsExtensionElement(uint8_t *ptr)
  : Element(ptr, 0x0100)
{
  // pass...
}

void
D878UVCodeplug::GeneralSettingsExtensionElement::clear() {
  memset(_data, 0x00, _size);
}

bool
D878UVCodeplug::GeneralSettingsExtensionElement::sendTalkerAlias() const {
  return getUInt8(0x0000);
}
void
D878UVCodeplug::GeneralSettingsExtensionElement::enableSendTalkerAlias(bool enable) {
  setUInt8(0x0000, (enable ? 0x01 : 0x00));
}

D878UVCodeplug::GeneralSettingsExtensionElement::TalkerAliasDisplay
D878UVCodeplug::GeneralSettingsExtensionElement::talkerAliasDisplay() const {
  return (TalkerAliasDisplay)getUInt8(0x001e);
}
void
D878UVCodeplug::GeneralSettingsExtensionElement::setTalkerAliasDisplay(TalkerAliasDisplay mode) {
  setUInt8(0x001e, (unsigned)mode);
}

D878UVCodeplug::GeneralSettingsExtensionElement::TalkerAliasEncoding
D878UVCodeplug::GeneralSettingsExtensionElement::talkerAliasEncoding() const {
  return (TalkerAliasEncoding)getUInt8(0x001f);
}
void
D878UVCodeplug::GeneralSettingsExtensionElement::setTalkerAliasEncoding(TalkerAliasEncoding enc) {
  setUInt8(0x001f, (unsigned)enc);
}

bool
D878UVCodeplug::GeneralSettingsExtensionElement::hasAutoRepeaterUHF2OffsetIndex() const {
  return 0xff != autoRepeaterUHF2OffsetIndex();
}
unsigned
D878UVCodeplug::GeneralSettingsExtensionElement::autoRepeaterUHF2OffsetIndex() const {
  return getUInt8(0x0022);
}
void
D878UVCodeplug::GeneralSettingsExtensionElement::setAutoRepeaterUHF2OffsetIndex(unsigned idx) {
  setUInt8(0x0022, idx);
}
void
D878UVCodeplug::GeneralSettingsExtensionElement::clearAutoRepeaterUHF2OffsetIndex() {
  setAutoRepeaterUHF2OffsetIndex(0xff);
}

bool
D878UVCodeplug::GeneralSettingsExtensionElement::hasAutoRepeaterVHF2OffsetIndex() const {
  return 0xff != autoRepeaterVHF2OffsetIndex();
}
unsigned
D878UVCodeplug::GeneralSettingsExtensionElement::autoRepeaterVHF2OffsetIndex() const {
  return getUInt8(0x0023);
}
void
D878UVCodeplug::GeneralSettingsExtensionElement::setAutoRepeaterVHF2OffsetIndex(unsigned idx) {
  setUInt8(0x0023, idx);
}
void
D878UVCodeplug::GeneralSettingsExtensionElement::clearAutoRepeaterVHF2OffsetIndex() {
  setAutoRepeaterVHF2OffsetIndex(0xff);
}

unsigned
D878UVCodeplug::GeneralSettingsExtensionElement::autoRepeaterVHF2MinFrequency() const {
  return ((unsigned)getBCD8_be(0x0024))*10;
}
void
D878UVCodeplug::GeneralSettingsExtensionElement::setAutoRepeaterVHF2MinFrequency(unsigned hz) {
  setBCD8_be(0x0024, hz/10);
}
unsigned
D878UVCodeplug::GeneralSettingsExtensionElement::autoRepeaterVHF2MaxFrequency() const {
  return ((unsigned)getBCD8_be(0x0028))*10;
}
void
D878UVCodeplug::GeneralSettingsExtensionElement::setAutoRepeaterVHF2MaxFrequency(unsigned hz) {
  setBCD8_be(0x0028, hz/10);
}
unsigned
D878UVCodeplug::GeneralSettingsExtensionElement::autoRepeaterUHF2MinFrequency() const {
  return ((unsigned)getBCD8_be(0x002c))*10;
}
void
D878UVCodeplug::GeneralSettingsExtensionElement::setAutoRepeaterUHF2MinFrequency(unsigned hz) {
  setBCD8_be(0x002c, hz/10);
}
unsigned
D878UVCodeplug::GeneralSettingsExtensionElement::autoRepeaterUHF2MaxFrequency() const {
  return ((unsigned)getBCD8_be(0x0030))*10;
}
void
D878UVCodeplug::GeneralSettingsExtensionElement::setAutoRepeaterUHF2MaxFrequency(unsigned hz) {
  setBCD8_be(0x0030, hz/10);
}

D878UVCodeplug::GeneralSettingsExtensionElement::GPSMode
D878UVCodeplug::GeneralSettingsExtensionElement::gpsMode() const {
  return (GPSMode)getUInt8(0x0035);
}
void
D878UVCodeplug::GeneralSettingsExtensionElement::setGPSMode(GPSMode mode) {
  setUInt8(0x0035, (unsigned)mode);
}

bool
D878UVCodeplug::GeneralSettingsExtensionElement::fromConfig(const Flags &flags, Context &ctx) {
  Q_UNUSED(flags); Q_UNUSED(ctx)
  return true;
}

bool
D878UVCodeplug::GeneralSettingsExtensionElement::updateConfig(Context &ctx) {
  Q_UNUSED(ctx)
  return true;
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::AnalogAPRSSettingsElement
 * ******************************************************************************************** */
D878UVCodeplug::AnalogAPRSSettingsElement::AnalogAPRSSettingsElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

D878UVCodeplug::AnalogAPRSSettingsElement::AnalogAPRSSettingsElement(uint8_t *ptr)
  : Element(ptr, 0x0040)
{
  // pass...
}

void
D878UVCodeplug::AnalogAPRSSettingsElement::clear() {
  memset(_data, 0x00, _size);
  setUInt8(0x0000, 0xff);
  setTXDelay(60);
  setUInt8(0x003d, 0x01); setUInt8(0x003e, 0x03); setUInt8(0x003f, 0xff);
}

bool
D878UVCodeplug::AnalogAPRSSettingsElement::isValid() const {
  if (! Codeplug::Element::isValid())
    return false;
  return (0 != frequency()) && (! destination().simplified().isEmpty())
      && (! source().simplified().isEmpty());
}

unsigned
D878UVCodeplug::AnalogAPRSSettingsElement::frequency() const {
  return ((unsigned)getBCD8_be(0x0001))*10;
}
void
D878UVCodeplug::AnalogAPRSSettingsElement::setFrequency(unsigned hz) {
  setBCD8_be(0x0001, hz/10);
}

unsigned
D878UVCodeplug::AnalogAPRSSettingsElement::txDelay() const {
  return ((unsigned)getUInt8(0x0005))*20;
}
void
D878UVCodeplug::AnalogAPRSSettingsElement::setTXDelay(unsigned ms) {
  setUInt8(0x0005, ms/20);
}

Signaling::Code
D878UVCodeplug::AnalogAPRSSettingsElement::txTone() const {
  if (0 == getUInt8(0x0006)) { // none
    return Signaling::SIGNALING_NONE;
  } else if (1 == getUInt8(0x0006)) { // CTCSS
    return ctcss_num2code(getUInt8(0x0007));
  } else if (2 == getUInt8(0x0006)) { // DCS
    uint16_t code = getUInt16_le(0x0008);
    if (512 < code)
      return Signaling::fromDCSNumber(dec_to_oct(code), false);
    return Signaling::fromDCSNumber(dec_to_oct(code-512), true);
  }

  return Signaling::SIGNALING_NONE;
}
void
D878UVCodeplug::AnalogAPRSSettingsElement::setTXTone(Signaling::Code code) {
  if (Signaling::SIGNALING_NONE == code) {
    setUInt8(0x0006, 0x00);
  } else if (Signaling::isCTCSS(code)) {
    setUInt8(0x0006, 0x01);
    setUInt8(0x0007, ctcss_code2num(code));
  } else if (Signaling::isDCSNormal(code)) {
    setUInt8(0x0006, 0x02);
    setUInt16_le(0x0008, oct_to_dec(Signaling::toDCSNumber(code)));
  } else if (Signaling::isDCSInverted(code)) {
    setUInt8(0x0006, 0x02);
    setUInt16_le(0x0008, oct_to_dec(Signaling::toDCSNumber(code))+512);
  }
}

unsigned
D878UVCodeplug::AnalogAPRSSettingsElement::manualTXInterval() const {
  return getUInt8(0x000a);
}
void
D878UVCodeplug::AnalogAPRSSettingsElement::setManualTXInterval(unsigned sec) {
  setUInt8(0x000a, sec);
}

bool
D878UVCodeplug::AnalogAPRSSettingsElement::autoTX() const {
  return 0!=autoTXInterval();
}
unsigned
D878UVCodeplug::AnalogAPRSSettingsElement::autoTXInterval() const {
  return ((unsigned)getUInt8(0x000b))*30;
}
void
D878UVCodeplug::AnalogAPRSSettingsElement::setAutoTXInterval(unsigned sec) {
  setUInt8(0x000b, sec/30);
}
void
D878UVCodeplug::AnalogAPRSSettingsElement::disableAutoTX() {
  setAutoTXInterval(0);
}

bool
D878UVCodeplug::AnalogAPRSSettingsElement::fixedLocationEnabled() const {
  return getUInt8(0x000d);
}
QGeoCoordinate
D878UVCodeplug::AnalogAPRSSettingsElement::fixedLocation() const {
  double latitude  = getUInt8(0x000e) + double(getUInt8(0x000f))/60 + double(getUInt8(0x0010))/3600;
  if (getUInt8(0x0011)) latitude *= -1;
  double longitude = getUInt8(0x0012) + double(getUInt8(0x0013))/60 + double(getUInt8(0x0014))/3600;
  if (getUInt8(0x0015)) longitude *= -1;
  return QGeoCoordinate(latitude, longitude);
}
void
D878UVCodeplug::AnalogAPRSSettingsElement::setFixedLocation(QGeoCoordinate &loc) {
  double latitude = loc.latitude();
  bool south = (0 > latitude); latitude = std::abs(latitude);
  unsigned lat_deg = int(latitude); latitude -= lat_deg; latitude *= 60;
  unsigned lat_min = int(latitude); latitude -= lat_min; latitude *= 60;
  unsigned lat_sec = int(latitude);
  double longitude = loc.longitude();
  bool west = (0 > longitude); longitude = std::abs(longitude);
  unsigned lon_deg = int(longitude); longitude -= lon_deg; longitude *= 60;
  unsigned lon_min = int(longitude); longitude -= lon_min; longitude *= 60;
  unsigned lon_sec = int(longitude);
  setUInt8(0x000e, lat_deg); setUInt8(0x000f, lat_min); setUInt8(0x0010, lat_sec); setUInt8(0x0011, (south ? 0x01 : 0x00));
  setUInt8(0x0012, lon_deg); setUInt8(0x0013, lon_min); setUInt8(0x0014, lon_sec); setUInt8(0x0015, (west ? 0x01 : 0x00));
  // enable fixed location.
  setUInt8(0x000d, 0x01);
}
void
D878UVCodeplug::AnalogAPRSSettingsElement::disableFixedLocation() {
  setUInt8(0x000d, 0x00);
}

QString
D878UVCodeplug::AnalogAPRSSettingsElement::destination() const {
  // Terminated/padded with space
  return readASCII(0x0016, 6, ' ');
}
unsigned
D878UVCodeplug::AnalogAPRSSettingsElement::destinationSSID() const {
  return getUInt8(0x001c);
}
void
D878UVCodeplug::AnalogAPRSSettingsElement::setDestination(const QString &call, unsigned ssid) {
  // Terminated/padded with space
  writeASCII(0x0016, call, 6, ' ');
  setUInt8(0x001c, ssid);
}
QString
D878UVCodeplug::AnalogAPRSSettingsElement::source() const {
  // Terminated/padded with space
  return readASCII(0x001d, 6, ' ');
}
unsigned
D878UVCodeplug::AnalogAPRSSettingsElement::sourceSSID() const {
  return getUInt8(0x0023);
}
void
D878UVCodeplug::AnalogAPRSSettingsElement::setSource(const QString &call, unsigned ssid) {
  // Terminated/padded with space
  writeASCII(0x001d, call, 6, ' ');
  setUInt8(0x0023, ssid);
}

QString
D878UVCodeplug::AnalogAPRSSettingsElement::path() const {
  return readASCII(0x0024, 20, 0x00);
}
void
D878UVCodeplug::AnalogAPRSSettingsElement::setPath(const QString &path) {
  writeASCII(0x0024, path, 20, 0x00);
}

APRSSystem::Icon
D878UVCodeplug::AnalogAPRSSettingsElement::icon() const {
  return code2aprsicon(getUInt8(0x0039), getUInt8(0x003a));
}
void
D878UVCodeplug::AnalogAPRSSettingsElement::setIcon(APRSSystem::Icon icon) {
  setUInt8(0x0039, aprsicon2tablecode(icon));
  setUInt8(0x003a, aprsicon2iconcode(icon));
}

Channel::Power
D878UVCodeplug::AnalogAPRSSettingsElement::power() const {
  switch (getUInt8(0x003b)) {
  case 0: return Channel::Power::Low;
  case 1: return Channel::Power::Mid;
  case 2: return Channel::Power::High;
  case 3: return Channel::Power::Max;
  }
  return Channel::Power::Low;
}
void
D878UVCodeplug::AnalogAPRSSettingsElement::setPower(Channel::Power power) {
  switch (power) {
  case Channel::Power::Min:
  case Channel::Power::Low:  setUInt8(0x003b, 0x00); break;
  case Channel::Power::Mid:  setUInt8(0x003b, 0x01); break;
  case Channel::Power::High: setUInt8(0x003b, 0x02); break;
  case Channel::Power::Max:  setUInt8(0x003b, 0x03); break;
  }
}

unsigned
D878UVCodeplug::AnalogAPRSSettingsElement::preWaveDelay() const {
  return ((unsigned)getUInt8(0x003c))*10;
}
void
D878UVCodeplug::AnalogAPRSSettingsElement::setPreWaveDelay(unsigned ms) {
  setUInt8(0x003c, ms/10);
}

bool
D878UVCodeplug::AnalogAPRSSettingsElement::fromAPRSSystem(const APRSSystem *sys, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx)
  clear();
  if (! sys->revertChannel()) {
    errMsg(err) << "Cannot encode APRS settings: "
                << "No revert channel defined for APRS system '" << sys->name() <<"'.";
    return false;
  }
  setFrequency(sys->revertChannel()->txFrequency()*1e6);
  setTXTone(sys->revertChannel()->txTone());
  setPower(sys->revertChannel()->power());
  setManualTXInterval(sys->period());
  setAutoTXInterval(sys->period());
  setDestination(sys->destination(), sys->destSSID());
  setSource(sys->source(), sys->srcSSID());
  setPath(sys->path());
  setIcon(sys->icon());
  setPreWaveDelay(0);
  return true;
}

APRSSystem *
D878UVCodeplug::AnalogAPRSSettingsElement::toAPRSSystem() {
  return new APRSSystem(
        tr("APRS %1").arg(destination()), nullptr,
        destination(), destinationSSID(), source(), sourceSSID(),
        path(), icon(), "", autoTXInterval());
}

bool
D878UVCodeplug::AnalogAPRSSettingsElement::linkAPRSSystem(APRSSystem *sys, Context &ctx) {
  // First, try to find a matching analog channel in list
  FMChannel *ch = ctx.config()->channelList()->findFMChannelByTxFreq(frequency()/1e6);
  if (! ch) {
    // If no channel is found, create one with the settings from APRS channel:
    ch = new FMChannel();
    ch->setName("APRS Channel");
    ch->setRXFrequency(frequency()/1e6);
    ch->setTXFrequency(frequency()/1e6);
    ch->setPower(power());
    ch->setTXTone(txTone());
    ch->setBandwidth(FMChannel::Bandwidth::Wide);
    logInfo() << "No matching APRS channel found for TX frequency " << frequency()/1e6
              << "MHz, create one as 'APRS Channel'";
    ctx.config()->channelList()->add(ch);
  }
  sys->setRevertChannel(ch);
  return true;
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::AnalogAPRSSettingsExtensionElement
 * ******************************************************************************************** */
D878UVCodeplug::AnalogAPRSSettingsExtensionElement::AnalogAPRSSettingsExtensionElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

D878UVCodeplug::AnalogAPRSSettingsExtensionElement::AnalogAPRSSettingsExtensionElement(uint8_t *ptr)
  : Element(ptr, 0x0060)
{
  // pass...
}

void
D878UVCodeplug::AnalogAPRSSettingsExtensionElement::clear() {
  memset(_data, 0x00, _size);
}

unsigned
D878UVCodeplug::AnalogAPRSSettingsExtensionElement::fixedAltitude() const {
  return getUInt16_le(0x0006)/3.28;
}
void
D878UVCodeplug::AnalogAPRSSettingsExtensionElement::setFixedAltitude(unsigned m) {
  setUInt16_le(0x0006, m*3.28);
}

bool
D878UVCodeplug::AnalogAPRSSettingsExtensionElement::reportPosition() const {
  return getBit(0x0008, 0);
}
void
D878UVCodeplug::AnalogAPRSSettingsExtensionElement::enableReportPosition(bool enable) {
  setBit(0x0008, 0, enable);
}

bool
D878UVCodeplug::AnalogAPRSSettingsExtensionElement::reportMicE() const {
  return getBit(0x0008, 1);
}
void
D878UVCodeplug::AnalogAPRSSettingsExtensionElement::enableReportMicE(bool enable) {
  setBit(0x0008, 1, enable);
}

bool
D878UVCodeplug::AnalogAPRSSettingsExtensionElement::reportObject() const {
  return getBit(0x0008, 2);
}
void
D878UVCodeplug::AnalogAPRSSettingsExtensionElement::enableReportObject(bool enable) {
  setBit(0x0008, 2, enable);
}

bool
D878UVCodeplug::AnalogAPRSSettingsExtensionElement::reportItem() const {
  return getBit(0x0008, 3);
}
void
D878UVCodeplug::AnalogAPRSSettingsExtensionElement::enableReportItem(bool enable) {
  setBit(0x0008, 3, enable);
}

bool
D878UVCodeplug::AnalogAPRSSettingsExtensionElement::reportMessage() const {
  return getBit(0x0008, 4);
}
void
D878UVCodeplug::AnalogAPRSSettingsExtensionElement::enableReportMessage(bool enable) {
  setBit(0x0008, 4, enable);
}

bool
D878UVCodeplug::AnalogAPRSSettingsExtensionElement::reportWeather() const {
  return getBit(0x0008, 5);
}
void
D878UVCodeplug::AnalogAPRSSettingsExtensionElement::enableReportWeather(bool enable) {
  setBit(0x0008, 5, enable);
}

bool
D878UVCodeplug::AnalogAPRSSettingsExtensionElement::reportNMEA() const {
  return getBit(0x0008, 6);
}
void
D878UVCodeplug::AnalogAPRSSettingsExtensionElement::enableReportNMEA(bool enable) {
  setBit(0x0008, 6, enable);
}

bool
D878UVCodeplug::AnalogAPRSSettingsExtensionElement::reportStatus() const {
  return getBit(0x0008, 7);
}
void
D878UVCodeplug::AnalogAPRSSettingsExtensionElement::enableReportStatus(bool enable) {
  setBit(0x0008, 7, enable);
}

bool
D878UVCodeplug::AnalogAPRSSettingsExtensionElement::reportOther() const {
  return getBit(0x0009, 0);
}
void
D878UVCodeplug::AnalogAPRSSettingsExtensionElement::enableReportOther(bool enable) {
  setBit(0x0009, 0, enable);
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::AnalogAPRSRXEntryElement
 * ******************************************************************************************** */
D878UVCodeplug::AnalogAPRSRXEntryElement::AnalogAPRSRXEntryElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

D878UVCodeplug::AnalogAPRSRXEntryElement::AnalogAPRSRXEntryElement(uint8_t *ptr)
  : Element(ptr, 0x0008)
{
  // pass...
}

void
D878UVCodeplug::AnalogAPRSRXEntryElement::clear() {
  memset(_data, 0x00, _size);
}

bool
D878UVCodeplug::AnalogAPRSRXEntryElement::isValid() const {
  return Element::isValid() && (0 != getUInt8(0x0000));
}

QString
D878UVCodeplug::AnalogAPRSRXEntryElement::call() const {
  return readASCII(0x0001, 6, 0x00);
}
unsigned
D878UVCodeplug::AnalogAPRSRXEntryElement::ssid() const {
  return getUInt8(0x0007);
}
void
D878UVCodeplug::AnalogAPRSRXEntryElement::setCall(const QString &call, unsigned ssid) {
  writeASCII(0x0001, call, 6, 0x00); // Store call
  setUInt8(0x0007, ssid);            // Store SSID
  setUInt8(0x0000, 0x01);            // Enable entry.
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::DMRAPRSSystemsElement
 * ******************************************************************************************** */
D878UVCodeplug::DMRAPRSSystemsElement::DMRAPRSSystemsElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

D878UVCodeplug::DMRAPRSSystemsElement::DMRAPRSSystemsElement(uint8_t *ptr)
  : Element(ptr, 0x0060)
{
  // pass...
}

void
D878UVCodeplug::DMRAPRSSystemsElement::clear() {
  memset(_data, 0x00, _size);
}

bool
D878UVCodeplug::DMRAPRSSystemsElement::channelIsSelected(unsigned n) const {
  return 0xfa2 == channelIndex(n);
}
unsigned
D878UVCodeplug::DMRAPRSSystemsElement::channelIndex(unsigned n) const {
  return getUInt16_le(0x0000 + n*2);
}
void
D878UVCodeplug::DMRAPRSSystemsElement::setChannelIndex(unsigned n, unsigned idx) {
  setUInt16_le(0x0000 + 2*n, idx);
}
void
D878UVCodeplug::DMRAPRSSystemsElement::setChannelSelected(unsigned n) {
  setChannelIndex(n, 0xfa2);
}

unsigned
D878UVCodeplug::DMRAPRSSystemsElement::destination(unsigned n) const {
  return getBCD8_be(0x0010 + 4*n);
}
void
D878UVCodeplug::DMRAPRSSystemsElement::setDestination(unsigned n, unsigned idx) {
  setBCD8_be(0x0010 + 4*n, idx);
}

DMRContact::Type
D878UVCodeplug::DMRAPRSSystemsElement::callType(unsigned n) const {
  switch(getUInt8(0x0030 + n)) {
  case 0: return DMRContact::PrivateCall;
  case 1: return DMRContact::GroupCall;
  case 2: return DMRContact::AllCall;
  }
  return DMRContact::PrivateCall;
}
void
D878UVCodeplug::DMRAPRSSystemsElement::setCallType(unsigned n, DMRContact::Type type) {
  switch(type) {
  case DMRContact::PrivateCall: setUInt8(0x0030+n, 0x00); break;
  case DMRContact::GroupCall: setUInt8(0x0030+n, 0x01); break;
  case DMRContact::AllCall: setUInt8(0x0030+n, 0x02); break;
  }
}

bool
D878UVCodeplug::DMRAPRSSystemsElement::timeSlotOverride(unsigned n) {
  return 0x00 != getUInt8(0x0039 + n);
}
DMRChannel::TimeSlot
D878UVCodeplug::DMRAPRSSystemsElement::timeSlot(unsigned n) const {
  switch (getUInt8(0x0039 + n)) {
  case 1: return DMRChannel::TimeSlot::TS1;
  case 2: return DMRChannel::TimeSlot::TS2;
  }
  return DMRChannel::TimeSlot::TS1;
}
void
D878UVCodeplug::DMRAPRSSystemsElement::setTimeSlot(unsigned n, DMRChannel::TimeSlot ts) {
  switch (ts) {
  case DMRChannel::TimeSlot::TS1: setUInt8(0x0039+n, 0x01); break;
  case DMRChannel::TimeSlot::TS2: setUInt8(0x0039+n, 0x02); break;
  }
}
void
D878UVCodeplug::DMRAPRSSystemsElement::clearTimeSlotOverride(unsigned n) {
  setUInt8(0x0039+n, 0);
}

bool
D878UVCodeplug::DMRAPRSSystemsElement::roaming() const {
  return getUInt8(0x0038);
}
void
D878UVCodeplug::DMRAPRSSystemsElement::enableRoaming(bool enable) {
  setUInt8(0x0038, (enable ? 0x01 : 0x00));
}

unsigned
D878UVCodeplug::DMRAPRSSystemsElement::repeaterActivationDelay() const {
  return ((unsigned)getUInt8(0x0041))*100;
}
void
D878UVCodeplug::DMRAPRSSystemsElement::setRepeaterActivationDelay(unsigned ms) {
  setUInt8(0x0041, ms/100);
}

bool
D878UVCodeplug::DMRAPRSSystemsElement::fromGPSSystems(Context &ctx) {
  if (ctx.config()->posSystems()->gpsCount() > 8)
    return false;
  for (int i=0; i<ctx.config()->posSystems()->gpsCount(); i++)
    fromGPSSystemObj(ctx.config()->posSystems()->gpsSystem(i), ctx);
  return true;
}

bool
D878UVCodeplug::DMRAPRSSystemsElement::fromGPSSystemObj(GPSSystem *sys, Context &ctx) {
  int idx = ctx.config()->posSystems()->indexOfGPSSys(sys);
  if ((idx < 0) || idx > 7)
    return false;
  if (sys->hasContact()) {
    setDestination(idx, sys->contactObj()->number());
    setCallType(idx, sys->contactObj()->type());
  }
  if (sys->hasRevertChannel() && (SelectedChannel::get() != (Channel *)sys->revertChannel())) {
    setChannelIndex(idx, ctx.index(sys->revertChannel()));
    clearTimeSlotOverride(idx);
  } else { // no revert channel specified or "selected channel":
    setChannelSelected(idx);
  }
  return true;
}

GPSSystem *
D878UVCodeplug::DMRAPRSSystemsElement::toGPSSystemObj(int idx) const {
  if (0 == destination(idx))
    return nullptr;
  return new GPSSystem(tr("GPS Sys #%1").arg(idx+1));
}

bool
D878UVCodeplug::DMRAPRSSystemsElement::linkGPSSystem(int idx, GPSSystem *sys, Context &ctx) const {
  // Clear revert channel from GPS system
  sys->setRevertChannel(nullptr);

  // if a revert channel is defined -> link to it
  if (channelIsSelected(idx))
    sys->setRevertChannel(nullptr);
  else if (ctx.has<Channel>(channelIndex(idx)) && ctx.get<Channel>(channelIndex(idx))->is<DMRChannel>())
    sys->setRevertChannel(ctx.get<Channel>(channelIndex(idx))->as<DMRChannel>());

  // Search for a matching contact in contacts
  DMRContact *cont = ctx.config()->contacts()->findDigitalContact(destination(idx));
  // If no matching contact is found, create one
  if (nullptr == cont) {
    cont = new DMRContact(callType(idx), tr("GPS #%1 Contact").arg(idx+1),
                              destination(idx), false);
    ctx.config()->contacts()->add(cont);
  }
  // link contact to GPS system.
  sys->setContactObj(cont);

  return true;
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::AESEncryptionKeyElement
 * ******************************************************************************************** */
D878UVCodeplug::AESEncryptionKeyElement::AESEncryptionKeyElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

D878UVCodeplug::AESEncryptionKeyElement::AESEncryptionKeyElement(uint8_t *ptr)
  : Element(ptr, 0x0040)
{
  // pass...
}

void
D878UVCodeplug::AESEncryptionKeyElement::clear() {
  memset(_data, 0x00, _size);
  setIndex(0xff);
  setUInt8(0x0022, 0x40);
}

bool
D878UVCodeplug::AESEncryptionKeyElement::isValid() const {
  return Element::isValid() && (0xff != index());
}

unsigned
D878UVCodeplug::AESEncryptionKeyElement::index() const {
  return getUInt8(0x0000);
}
void
D878UVCodeplug::AESEncryptionKeyElement::setIndex(unsigned idx) {
  setUInt8(0x0000, idx);
}

QByteArray
D878UVCodeplug::AESEncryptionKeyElement::key() const {
  QByteArray ar(32, 0); memcpy(ar.data(), _data+0x0001, 32);
  return ar;
}

void
D878UVCodeplug::AESEncryptionKeyElement::setKey(const QByteArray &key) {
  if (32 != key.size())
    return;
  memcpy(_data+0x0001, key.constData(), 32);
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::RadioInfoElement
 * ******************************************************************************************** */
D878UVCodeplug::RadioInfoElement::RadioInfoElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

D878UVCodeplug::RadioInfoElement::RadioInfoElement(uint8_t *ptr)
  : Element(ptr, 0x0100)
{
  // pass...
}

void
D878UVCodeplug::RadioInfoElement::clear() {
  setBandSelectPassword("");
  setProgramPassword("");
}

bool
D878UVCodeplug::RadioInfoElement::fullTest() const {
  return getUInt8(0x0002);
}

D878UVCodeplug::RadioInfoElement::FrequencyRange
D878UVCodeplug::RadioInfoElement::frequencyRange() const {
  return (FrequencyRange)getUInt8(0x0003);
}
void
D878UVCodeplug::RadioInfoElement::setFrequencyRange(FrequencyRange range) {
  setUInt8(0x0003, (unsigned)range);
}

bool
D878UVCodeplug::RadioInfoElement::international() const {
  return getUInt8(0x0004);
}
void
D878UVCodeplug::RadioInfoElement::enableInternational(bool enable) {
  setUInt8(0x0004, (enable ? 0x01 : 0x00));
}

bool
D878UVCodeplug::RadioInfoElement::bandSelect() const {
  return getUInt8(0x0006);
}
void
D878UVCodeplug::RadioInfoElement::enableBandSelect(bool enable) {
  setUInt8(0x0006, (enable ? 0x01 : 0x00));
}

QString
D878UVCodeplug::RadioInfoElement::bandSelectPassword() const {
  return readASCII(0x000b, 4, 0x00);
}
void
D878UVCodeplug::RadioInfoElement::setBandSelectPassword(const QString &passwd) {
  writeASCII(0x000b, passwd, 4, 0x00);
}

QString
D878UVCodeplug::RadioInfoElement::radioType() const {
  return readASCII(0x0010, 7, 0x00);
}

QString
D878UVCodeplug::RadioInfoElement::programPassword() const {
  return readASCII(0x0028, 4, 0x00);
}
void
D878UVCodeplug::RadioInfoElement::setProgramPassword(const QString &passwd) {
  writeASCII(0x0028, passwd, 4, 0x00);
}

QString
D878UVCodeplug::RadioInfoElement::areaCode() const {
  return readASCII(0x002c, 4, 0x00);
}

QString
D878UVCodeplug::RadioInfoElement::serialNumber() const {
  return readASCII(0x0030, 16, 0x00);
}

QString
D878UVCodeplug::RadioInfoElement::productionDate() const {
  return readASCII(0x0040, 10, 0x00);
}

QString
D878UVCodeplug::RadioInfoElement::manufacturerCode() const {
  return readASCII(0x0050, 8, 0x00);
}

QString
D878UVCodeplug::RadioInfoElement::maintainedDate() const {
  return readASCII(0x0060, 16, 0x00);
}

QString
D878UVCodeplug::RadioInfoElement::dealerCode() const {
  return readASCII(0x0070, 16, 0x00);
}

QString
D878UVCodeplug::RadioInfoElement::stockDate() const {
  return readASCII(0x0080, 16, 0x00);
}

QString
D878UVCodeplug::RadioInfoElement::sellDate() const {
  return readASCII(0x0090, 16, 0x00);
}

QString
D878UVCodeplug::RadioInfoElement::seller() const {
  return readASCII(0x00a0, 16, 0x00);
}

QString
D878UVCodeplug::RadioInfoElement::maintainerNote() const {
  return readASCII(0x00b0, 128, 0x00);
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug
 * ******************************************************************************************** */
D878UVCodeplug::D878UVCodeplug(const QString &label, QObject *parent)
  : D868UVCodeplug(label, parent)
{
  // pass...
}

D878UVCodeplug::D878UVCodeplug(QObject *parent)
  : D868UVCodeplug("Anytone AT-D878UV Codeplug", parent)
{
  // pass...
}

bool
D878UVCodeplug::allocateBitmaps() {
  if (! D868UVCodeplug::allocateBitmaps())
    return false;

  // Roaming channel bitmaps
  image(0).addElement(ADDR_ROAMING_CHANNEL_BITMAP, ROAMING_CHANNEL_BITMAP_SIZE);
  // Roaming zone bitmaps
  image(0).addElement(ADDR_ROAMING_ZONE_BITMAP, ROAMING_ZONE_BITMAP_SIZE);

  return true;
}

void
D878UVCodeplug::allocateUpdated() {
  // First allocate everything common between D868UV and D878UV codeplugs.
  D868UVCodeplug::allocateUpdated();

  // Encryption keys
  image(0).addElement(ADDR_ENCRYPTION_KEYS, ENCRYPTION_KEYS_SIZE);

  // allocate APRS settings extension
  image(0).addElement(ADDR_APRS_SET_EXT, APRS_SET_EXT_SIZE);

  // allocate APRS RX list
  image(0).addElement(ADDR_APRS_RX_ENTRY, NUM_APRS_RX_ENTRY*APRS_RX_ENTRY_SIZE);
}

void
D878UVCodeplug::allocateForEncoding() {
  // First allocate everything common between D868UV and D878UV codeplugs.
  D868UVCodeplug::allocateForEncoding();
  this->allocateRoaming();
}

void
D878UVCodeplug::allocateForDecoding() {
  // First allocate everything common between D868UV and D878UV codeplugs.
  D868UVCodeplug::allocateForDecoding();
  this->allocateRoaming();
}


void
D878UVCodeplug::setBitmaps(Config *config)
{
  // First set everything common between D868UV and D878UV codeplugs.
  D868UVCodeplug::setBitmaps(config);

  // Mark roaming zones
  uint8_t *roaming_zone_bitmap = data(ADDR_ROAMING_ZONE_BITMAP);
  memset(roaming_zone_bitmap, 0x00, ROAMING_ZONE_BITMAP_SIZE);
  for (int i=0; i<config->roamingZones()->count(); i++)
    roaming_zone_bitmap[i/8] |= (1<<(i%8));

  // Mark roaming channels
  uint8_t *roaming_ch_bitmap = data(ADDR_ROAMING_CHANNEL_BITMAP);
  memset(roaming_ch_bitmap, 0x00, ROAMING_CHANNEL_BITMAP_SIZE);
  // Get all (unique) channels used in roaming
  for (int i=0; i<std::min(NUM_ROAMING_CHANNEL,config->roamingChannels()->count()); i++)
    roaming_ch_bitmap[i/8] |= (1<<(i%8));
}


bool
D878UVCodeplug::encodeElements(const Flags &flags, Context &ctx, const ErrorStack &err)
{
  // Encode everything common between d868uv and d878uv radios.
  if (! D868UVCodeplug::encodeElements(flags, ctx, err))
    return false;

  if (! this->encodeRoaming(flags, ctx, err))
    return false;

  return true;
}


bool
D878UVCodeplug::decodeElements(Context &ctx, const ErrorStack &err)
{
  // Decode everything commong between d868uv and d878uv codeplugs.
  if (! D868UVCodeplug::decodeElements(ctx, err))
    return false;

  if (! this->createRoaming(ctx, err))
    return false;

  if (! this->linkRoaming(ctx, err))
    return false;

  return true;
}

void
D878UVCodeplug::allocateChannels() {
  /* Allocate channels */
  uint8_t *channel_bitmap = data(CHANNEL_BITMAP);
  for (uint16_t i=0; i<NUM_CHANNELS; i++) {
    // Get byte and bit for channel, as well as bank of channel
    uint16_t bit = i%8, byte = i/8, bank = i/128, idx=i%128;
    // if disabled -> skip
    if (0 == ((channel_bitmap[byte]>>bit) & 0x01))
      continue;
    // compute address for channel
    uint32_t addr = CHANNEL_BANK_0
        + bank*CHANNEL_BANK_OFFSET
        + idx*CHANNEL_SIZE;
    if (nullptr == data(addr, 0)) {
      image(0).addElement(addr, CHANNEL_SIZE);
    }
    if (nullptr == data(addr+0x2000, 0)) {
      image(0).addElement(addr+0x2000, CHANNEL_SIZE);
      memset(data(addr+0x2000), 0x00, CHANNEL_SIZE);
    }
  }
}

bool
D878UVCodeplug::encodeChannels(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)
  // Encode channels
  for (int i=0; i<ctx.config()->channelList()->count(); i++) {
    // enable channel
    uint16_t bank = i/128, idx = i%128;
    ChannelElement ch(data(CHANNEL_BANK_0 + bank*CHANNEL_BANK_OFFSET + idx*CHANNEL_SIZE));
    ch.fromChannelObj(ctx.config()->channelList()->channel(i), ctx);
  }
  return true;
}

bool
D878UVCodeplug::createChannels(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  // Create channels
  uint8_t *channel_bitmap = data(CHANNEL_BITMAP);
  for (uint16_t i=0; i<NUM_CHANNELS; i++) {
    // Check if channel is enabled:
    uint16_t  bit = i%8, byte = i/8, bank = i/128, idx = i%128;
    if (0 == ((channel_bitmap[byte]>>bit) & 0x01))
      continue;
    ChannelElement ch(data(CHANNEL_BANK_0 + bank*CHANNEL_BANK_OFFSET + idx*CHANNEL_SIZE));
    if (Channel *obj = ch.toChannelObj(ctx)) {
      ctx.config()->channelList()->add(obj); ctx.add(obj, i);
    }
  }
  return true;
}

bool
D878UVCodeplug::linkChannels(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  // Link channel objects
  for (uint16_t i=0; i<NUM_CHANNELS; i++) {
    // Check if channel is enabled:
    uint16_t  bit = i%8, byte = i/8, bank = i/128, idx = i%128;
    if (0 == (((*data(CHANNEL_BITMAP+byte))>>bit) & 0x01))
      continue;
    ChannelElement ch(data(CHANNEL_BANK_0 + bank*CHANNEL_BANK_OFFSET + idx*CHANNEL_SIZE));
    if (ctx.has<Channel>(i))
      ch.linkChannelObj(ctx.get<Channel>(i), ctx);
  }
  return true;
}


void
D878UVCodeplug::allocateZones() {
  D868UVCodeplug::allocateZones();
  // Hidden zone map
  image(0).addElement(ADDR_HIDDEN_ZONE_MAP, HIDDEN_ZONE_MAP_SIZE);
}

bool
D878UVCodeplug::encodeZone(int i, Zone *zone, bool isB, const Flags &flags, Context &ctx, const ErrorStack &err) {
  if (! D868UVCodeplug::encodeZone(i, zone, isB, flags, ctx, err))
    return false;

  AnytoneZoneExtension *ext = zone->anytoneExtension();
  if (nullptr == ext)
      return true;

  if (ext->hidden()) {
    // set bit
    data(ADDR_HIDDEN_ZONE_MAP)[i/8] |= (1<<(i%8));
  } else {
    // clear bit
    data(ADDR_HIDDEN_ZONE_MAP)[i/8] &= ~(1<<(i%8));
  }

  return true;
}

bool
D878UVCodeplug::decodeZone(int i, Zone *zone, bool isB, Context &ctx, const ErrorStack &err) {
  if (! D868UVCodeplug::decodeZone(i, zone, isB, ctx, err))
    return false;
  AnytoneZoneExtension *ext = zone->anytoneExtension();
  if (nullptr == ext) {
    ext = new AnytoneZoneExtension();
    zone->setAnytoneExtension(ext);
  }

  if ((! isB) && (data(ADDR_HIDDEN_ZONE_MAP)[i/8] & (1<<(i%8))))
    ext->enableHidden(true);
  else
    ext->enableHidden(false);

  return true;
}


void
D878UVCodeplug::allocateGeneralSettings() {
  // override allocation of general settings for D878UV code-plug. General settings are larger!
  image(0).addElement(ADDR_GENERAL_CONFIG, GENERAL_CONFIG_SIZE);
  image(0).addElement(ADDR_GENERAL_CONFIG_EXT1, GENERAL_CONFIG_EXT1_SIZE);
  image(0).addElement(ADDR_GENERAL_CONFIG_EXT2, GENERAL_CONFIG_EXT2_SIZE);

}
bool
D878UVCodeplug::encodeGeneralSettings(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  GeneralSettingsElement(data(ADDR_GENERAL_CONFIG)).fromConfig(flags, ctx);
  GPSMessageElement(data(ADDR_GENERAL_CONFIG_EXT1)).fromConfig(flags, ctx);
  GeneralSettingsExtensionElement(data(ADDR_GENERAL_CONFIG_EXT2)).fromConfig(flags, ctx);
  return true;
}
bool
D878UVCodeplug::decodeGeneralSettings(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  GeneralSettingsElement(data(ADDR_GENERAL_CONFIG)).updateConfig(ctx);
  GPSMessageElement(data(ADDR_GENERAL_CONFIG_EXT1)).updateConfig(ctx);
  GeneralSettingsExtensionElement(data(ADDR_GENERAL_CONFIG_EXT2)).updateConfig(ctx);
  return true;
}

void
D878UVCodeplug::allocateGPSSystems() {
  // replaces D868UVCodeplug::allocateGPSSystems

  // APRS settings
  image(0).addElement(ADDR_APRS_SETTING, APRS_SETTING_SIZE);
  image(0).addElement(ADDR_APRS_MESSAGE, APRS_MESSAGE_SIZE);
  image(0).addElement(ADDR_GPS_SETTING, GPS_SETTING_SIZE);
}

bool
D878UVCodeplug::encodeGPSSystems(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)
  // replaces D868UVCodeplug::encodeGPSSystems

  // Encode APRS system (there can only be one)
  if (0 < ctx.config()->posSystems()->aprsCount()) {
    AnalogAPRSSettingsElement(data(ADDR_APRS_SETTING))
        .fromAPRSSystem(ctx.config()->posSystems()->aprsSystem(0), ctx);
    uint8_t *aprsmsg = (uint8_t *)data(ADDR_APRS_MESSAGE);
    encode_ascii(aprsmsg, ctx.config()->posSystems()->aprsSystem(0)->message(), 60, 0x00);
  }

  // Encode GPS systems
  DMRAPRSSystemsElement gps(data(ADDR_GPS_SETTING));
  if (! gps.fromGPSSystems(ctx))
    return false;
  if (0 < ctx.config()->posSystems()->gpsCount()) {
    // If there is at least one GPS system defined -> set auto TX interval.
    //  This setting might be overridden by any analog APRS system below
    AnalogAPRSSettingsElement aprs(data(ADDR_APRS_SETTING));
    aprs.setAutoTXInterval(ctx.config()->posSystems()->gpsSystem(0)->period());
    aprs.setManualTXInterval(ctx.config()->posSystems()->gpsSystem(0)->period());
  }
  return true;
}

bool
D878UVCodeplug::createGPSSystems(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  // replaces D868UVCodeplug::createGPSSystems

  // Before creating any GPS/APRS systems, get global auto TX interval
  AnalogAPRSSettingsElement aprs(data(ADDR_APRS_SETTING));
  unsigned pos_intervall = aprs.autoTXInterval();

  // Create APRS system (if enabled)
  uint8_t *aprsmsg = (uint8_t *)data(ADDR_APRS_MESSAGE);
  if (aprs.isValid()) {
    APRSSystem *sys = aprs.toAPRSSystem();
    sys->setPeriod(pos_intervall);
    sys->setMessage(decode_ascii(aprsmsg, 60, 0x00));
    ctx.config()->posSystems()->add(sys); ctx.add(sys,0);
  }

  // Create GPS systems
  DMRAPRSSystemsElement gps_systems(data(ADDR_GPS_SETTING));
  for (int i=0; i<NUM_GPS_SYSTEMS; i++) {
    if (0 == gps_systems.destination(i))
      continue;
    if (GPSSystem *sys = gps_systems.toGPSSystemObj(i)) {
      logDebug() << "Create GPS sys '" << sys->name() << "' at idx " << i << ".";
      sys->setPeriod(pos_intervall);
      ctx.config()->posSystems()->add(sys); ctx.add(sys, i);
    } else {
      return false;
    }
  }
  return true;
}

bool
D878UVCodeplug::linkGPSSystems(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)
  // replaces D868UVCodeplug::linkGPSSystems

  // Link APRS system
  AnalogAPRSSettingsElement aprs(data(ADDR_APRS_SETTING));
  if (aprs.isValid()) {
    aprs.linkAPRSSystem(ctx.config()->posSystems()->aprsSystem(0), ctx);
  }

  // Link GPS systems
  DMRAPRSSystemsElement gps_systems(data(ADDR_GPS_SETTING));
  for (int i=0; i<NUM_GPS_SYSTEMS; i++) {
    if (0 == gps_systems.destination(i))
      continue;
    gps_systems.linkGPSSystem(i, ctx.get<GPSSystem>(i), ctx);
  }

  return true;
}


void
D878UVCodeplug::allocateRoaming() {
  /* Allocate roaming channels */
  uint8_t *roaming_channel_bitmap = data(ADDR_ROAMING_CHANNEL_BITMAP);
  for (uint8_t i=0; i<NUM_ROAMING_CHANNEL; i++) {
    // Get byte and bit for roaming channel
    uint16_t bit = i%8, byte = i/8;
    // if disabled -> skip
    if (0 == ((roaming_channel_bitmap[byte]>>bit) & 0x01))
      continue;
    // Allocate roaming channel
    uint32_t addr = ADDR_ROAMING_CHANNEL_0 + i*ROAMING_CHANNEL_OFFSET;
    if (nullptr == data(addr, 0))
      image(0).addElement(addr, ROAMING_CHANNEL_SIZE);
  }

  /* Allocate roaming zones. */
  uint8_t *roaming_zone_bitmap = data(ADDR_ROAMING_ZONE_BITMAP);
  for (uint8_t i=0; i<NUM_ROAMING_ZONES; i++) {
    // Get byte and bit for radio ID
    uint16_t bit = i%8, byte = i/8;
    // if disabled -> skip
    if (0 == ((roaming_zone_bitmap[byte]>>bit) & 0x01))
      continue;
    // Allocate roaming zone
    uint32_t addr = ADDR_ROAMING_ZONE_0 + i*ROAMING_ZONE_OFFSET;
    if (nullptr == data(addr, 0)) {
      logDebug() << "Allocate roaming zone at " << QString::number(addr, 16);
      image(0).addElement(addr, ROAMING_ZONE_SIZE);
    }
  }
}

bool
D878UVCodeplug::encodeRoaming(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)

  // Encode roaming channels
  for (uint8_t i=0; i<std::min(NUM_ROAMING_CHANNEL, ctx.config()->roamingChannels()->count()); i++) {
    // Encode roaming channel
    uint32_t addr = ADDR_ROAMING_CHANNEL_0 + i*ROAMING_CHANNEL_OFFSET;
    RoamingChannelElement rch_elm(data(addr));
    RoamingChannel *rch = ctx.config()->roamingChannels()->get(i)->as<RoamingChannel>();
    rch_elm.clear();
    rch_elm.fromChannel(rch);
    if (! ctx.add(rch, i)) {
      errMsg(err) << "Cannot add index " << i << " for roaming channel '"
                  << rch->name() << "' to codeplug context.";
      return false;
    }
  }

  // Encode roaming zones
  for (int i=0; i<ctx.config()->roamingZones()->count(); i++){
    uint32_t addr = ADDR_ROAMING_ZONE_0+i*ROAMING_ZONE_OFFSET;
    RoamingZoneElement zone(data(addr));
    logDebug() << "Encode roaming zone " << ctx.config()->roamingZones()->zone(i)->name()
               << " (" << (i+1) << ") at " << QString::number(addr, 16)
               << " with " << ctx.config()->roamingZones()->zone(i)->count() << " elements.";
    zone.fromRoamingZone(ctx.config()->roamingZones()->zone(i), ctx);
  }

  return true;
}

bool
D878UVCodeplug::createRoaming(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  // Create or find roaming channels
  uint8_t *roaming_channel_bitmap = data(ADDR_ROAMING_CHANNEL_BITMAP);
  for (int i=0; i<NUM_ROAMING_CHANNEL; i++) {
    uint8_t byte=i/8, bit=i%8;
    if (0 == ((roaming_channel_bitmap[byte]>>bit) & 0x01))
      continue;
    uint32_t addr = ADDR_ROAMING_CHANNEL_0 + i*ROAMING_CHANNEL_OFFSET;
    RoamingChannelElement ch(data(addr));
    RoamingChannel *digi = ch.toChannel(ctx);
    ctx.add(digi, i);
  }

  // Create and link roaming zones
  uint8_t *roaming_zone_bitmap = data(ADDR_ROAMING_ZONE_BITMAP);
  for (int i=0; i<NUM_ROAMING_ZONES; i++) {
    uint8_t byte=i/8, bit=i%8;
    if (0 == ((roaming_zone_bitmap[byte]>>bit) & 0x01))
      continue;
    uint32_t addr = ADDR_ROAMING_ZONE_0 + i*ROAMING_ZONE_OFFSET;
    RoamingZoneElement z(data(addr));
    RoamingZone *zone = z.toRoamingZone();
    ctx.config()->roamingZones()->add(zone); ctx.add(zone, i);
    z.linkRoamingZone(zone, ctx);
  }

  return true;
}

bool
D878UVCodeplug::linkRoaming(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(err)
  // Pass, no need to link roaming channels.
  return true;
}

