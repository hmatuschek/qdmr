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

#define ADDR_GENERAL_CONFIG       0x02500000
#define GENERAL_CONFIG_SIZE       0x00000100
static_assert(
  GENERAL_CONFIG_SIZE == sizeof(D878UVCodeplug::general_settings_base_t),
  "D878UVCodeplug::general_settings_base_t size check failed.");

#define ADDR_GENERAL_CONFIG_EXT1  0x02501280
#define GENERAL_CONFIG_EXT1_SIZE  0x00000030
static_assert(
  GENERAL_CONFIG_EXT1_SIZE == sizeof(D878UVCodeplug::general_settings_ext1_t),
  "D878UVCodeplug::general_settings_ext1_t size check failed.");

#define ADDR_GENERAL_CONFIG_EXT2  0x02501400
#define GENERAL_CONFIG_EXT2_SIZE  0x00000100
static_assert(
  GENERAL_CONFIG_EXT2_SIZE == sizeof(D878UVCodeplug::general_settings_ext2_t),
  "D878UVCodeplug::general_settings_ext2_t size check failed.");

#define ADDR_APRS_SETTING         0x02501000 // Address of APRS settings
#define APRS_SETTING_SIZE         0x00000040 // Size of the APRS settings

#define ADDR_APRS_SET_EXT         0x025010A0 // Address of APRS settings extension
#define APRS_SET_EXT_SIZE         0x00000060 // Size of APRS settings extension
static_assert(
  APRS_SET_EXT_SIZE == sizeof(D878UVCodeplug::aprs_setting_ext_t),
  "D878UVCodeplug::aprs_setting_ext_t size check failed.");

#define ADDR_APRS_MESSAGE         0x02501200 // Address of APRS messages
#define APRS_MESSAGE_SIZE         0x00000040 // Size of APRS messages
static_assert(
  APRS_SETTING_SIZE == sizeof(D878UVCodeplug::aprs_setting_t),
  "D878UVCodeplug::aprs_setting_t size check failed.");

#define NUM_APRS_RX_ENTRY         32
#define ADDR_APRS_RX_ENTRY        0x02501800 // Address of APRS RX list
#define APRS_RX_ENTRY_SIZE        0x00000008 // Size of each APRS RX entry
static_assert(
  APRS_RX_ENTRY_SIZE == sizeof(D878UVCodeplug::aprs_rx_entry_t),
  "D878UVCodeplug::aprs_rx_entry_t size check failed.");

#define NUM_GPS_SYSTEMS           8
#define ADDR_GPS_SETTING          0x02501040 // Address of GPS settings
#define GPS_SETTING_SIZE          0x00000060 // Size of the GPS settings
static_assert(
  GPS_SETTING_SIZE == sizeof(D878UVCodeplug::gps_systems_t),
  "D878UVCodeplug::gps_systems_t size check failed.");

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
 * Implementation of D878UVCodeplug::general_settings_t
 * ******************************************************************************************** */
D878UVCodeplug::general_settings_base_t::general_settings_base_t() {
  clear();
}

void
D878UVCodeplug::general_settings_base_t::clear() {
  mic_gain = 2;
}

uint
D878UVCodeplug::general_settings_base_t::getMicGain() const {
  return (mic_gain+1)*2;
}
void
D878UVCodeplug::general_settings_base_t::setMicGain(uint gain) {
  mic_gain = (gain-1)/2;
}

void
D878UVCodeplug::general_settings_base_t::fromConfig(const Config *config, const Flags &flags) {
  setMicGain(config->micLevel());

  // Set SMS format
  sms_format = SMS_FMT_DMR;
  // Enable "amateur mode", this enables all menu items.
  enable_pro_mode = 0x00;

  // If auto-enable roaming is enabled
  if (flags.autoEnableRoaming) {
    // Check if roaming is required -> configure & enable
    if (config->requiresRoaming()) {
      repchk_enable     = 0x01;
      repchk_interval   = 0x05; // 0x05 == 30s
      repchk_recon      = 0x02; // 0x02 == 3 times
      repchk_notify     = 0x00; // 0x00 == no notification
      roam_enable       = 0x01;
      roam_default_zone = 0x00; // Default roaming zone index
      roam_start_cond   = 0x01; // Start condition == out-of-range
    } else {
      // If roaming is not required -> disable repeater check and roaming
      repchk_enable = 0x00;
      roam_enable   = 0x00;
    }
  }

  // If auto-enable GPS is enabled
  if (flags.autoEnableGPS) {
    // Check if GPS is required -> enable
    if (config->requiresGPS()) {
      gps_enable = 0x01;
      // Set time zone based on system time zone.
      int offset = QTimeZone::systemTimeZone().offsetFromUtc(QDateTime::currentDateTime());
      timezone = 12 + offset/3600;
      gps_sms_enable = 0x00;
      gps_message_enable = 0x00;
      gps_sms_interval = 0x05;
      // Set measurement system based on system locale (0x00==Metric)
      gps_unit = (QLocale::MetricSystem == QLocale::system().measurementSystem()) ? 0x00 : 0x01;
    } else {
      gps_enable = 0x00;
    }
  }
}

void
D878UVCodeplug::general_settings_base_t::updateConfig(Config *config) {
  config->setMicLevel(getMicGain());
  // Check if default roaming zone is defined
  if (roam_default_zone >= config->roaming()->count()) {
    roam_default_zone = 0;
  }
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::general_settings_ext1_t
 * ******************************************************************************************** */
void
D878UVCodeplug::general_settings_ext1_t::fromConfig(const Config *conf, const Flags &flags) {
  Q_UNUSED(conf)
  Q_UNUSED(flags)
  memset(gps_message, 0, sizeof(gps_message));
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::general_settings_ext1_t
 * ******************************************************************************************** */
void
D878UVCodeplug::general_settings_ext2_t::fromConfig(const Config *conf, const Flags &flags) {
  Q_UNUSED(conf)
  Q_UNUSED(flags)
  // Do not send talker alias
  send_alias = 0x00;
  // Enable only GPS here.
  gps_mode = 0x00;
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::aprs_system_t
 * ******************************************************************************************** */
bool
D878UVCodeplug::aprs_setting_t::isValid() const {
  return getFrequency() && (! getDestination().isEmpty()) && (! getSource().isEmpty());
}

double
D878UVCodeplug::aprs_setting_t::getFrequency() const {
  return decode_frequency(qFromBigEndian(frequency));
}
void
D878UVCodeplug::aprs_setting_t::setFrequency(double freq) {
  frequency = qToBigEndian(encode_frequency(freq));
}

int
D878UVCodeplug::aprs_setting_t::getAutoTXInterval() const {
  return int(auto_tx_interval)*30;
}
void
D878UVCodeplug::aprs_setting_t::setAutoTxInterval(int sec) {
  if (0 == sec)
    auto_tx_interval = 0;
  else if (30 >= sec)
    auto_tx_interval = 1;
  else
    auto_tx_interval = (sec-16)/15;
}

int
D878UVCodeplug::aprs_setting_t::getManualTXInterval() const {
  return manual_tx_interval;
}
void
D878UVCodeplug::aprs_setting_t::setManualTxInterval(int sec) {
  manual_tx_interval = sec;
}

QString
D878UVCodeplug::aprs_setting_t::getDestination() const {
  return decode_ascii(to_call, 6, 0x20);
}
void
D878UVCodeplug::aprs_setting_t::setDestination(const QString &call, uint8_t ssid) {
  encode_ascii(to_call, call, 6, 0x20);
  to_ssid = std::min(uint8_t(16), ssid);
}

QString
D878UVCodeplug::aprs_setting_t::getSource() const {
  return decode_ascii(from_call, 6, 0x20);
}
void
D878UVCodeplug::aprs_setting_t::setSource(const QString &call, uint8_t ssid) {
  encode_ascii(from_call, call, 6, 0x20);
  from_ssid = ssid;
}

QString
D878UVCodeplug::aprs_setting_t::getPath() const {
  return decode_ascii(path, 20, 0x00);
}
void
D878UVCodeplug::aprs_setting_t::setPath(const QString &path) {
  encode_ascii(this->path, path, 20, 0x00);
}

void
D878UVCodeplug::aprs_setting_t::setSignaling(Signaling::Code code) {
  if (Signaling::SIGNALING_NONE == code) {
    sig_type = SIG_OFF;
    ctcss = dcs = 0;
  } else if (Signaling::isCTCSS(code)) {
    sig_type = SIG_CTCSS;
    ctcss = ctcss_code2num(code);
    dcs = 0;
  } else if (Signaling::isDCSNormal(code)) {
    sig_type = SIG_DCS;
    ctcss = 0;
    dcs = qToLittleEndian(oct_to_dec(Signaling::toDCSNumber(code)));
  } else if (Signaling::isDCSInverted(code)) {
    sig_type = SIG_DCS;
    dcs = qToLittleEndian(oct_to_dec(Signaling::toDCSNumber(code))+512);
  }
}
Signaling::Code
D878UVCodeplug::aprs_setting_t::getSignaling() const {
  if (SIG_CTCSS == sig_type) {
    return ctcss_num2code((ctcss<52) ? ctcss : 0);
  } else if (SIG_DCS == sig_type) {
    uint16_t dcsnum = dec_to_oct(qFromLittleEndian(dcs));
    if (512 <= dcsnum)
      return Signaling::fromDCSNumber(dcsnum-512, true);
    return Signaling::fromDCSNumber(dcsnum, false);
  }
  return Signaling::SIGNALING_NONE;
}

Channel::Power
D878UVCodeplug::aprs_setting_t::getPower() const {
  switch (power) {
  case POWER_LOW: return Channel::Power::Low;
  case POWER_MID: return Channel::Power::Mid;
  case POWER_HIGH: return Channel::Power::High;
  case POWER_TURBO: return Channel::Power::Max;
  default: break;
  }
  return Channel::Power::High;
}

void
D878UVCodeplug::aprs_setting_t::setPower(Channel::Power pwr) {
  switch (pwr) {
  case Channel::Power::Min:
  case Channel::Power::Low:
    power = aprs_setting_t::POWER_LOW;
    break;
  case Channel::Power::Mid:
    power = aprs_setting_t::POWER_MID;
    break;
  case Channel::Power::High:
    power = aprs_setting_t::POWER_HIGH;
    break;
  case Channel::Power::Max:
    power = aprs_setting_t::POWER_TURBO;
    break;
  }
}

APRSSystem::Icon
D878UVCodeplug::aprs_setting_t::getIcon() const {
  return code2aprsicon(table, icon);
}
void
D878UVCodeplug::aprs_setting_t::setIcon(APRSSystem::Icon icon) {
  this->table = aprsicon2tablecode(icon);
  this->icon = aprsicon2iconcode(icon);
}

void
D878UVCodeplug::aprs_setting_t::fromAPRSSystem(APRSSystem *sys) {
  _unknown0 = 0xff;
  setFrequency(sys->revertChannel()->txFrequency());
  tx_delay = 0x03;
  setSignaling(sys->revertChannel()->txTone());
  setManualTxInterval(sys->period());
  setAutoTxInterval(sys->period());
  tx_tone_enable = 0;

  setDestination(sys->destination(), sys->destSSID());
  setSource(sys->source(), sys->srcSSID());
  setPath(sys->path());
  _pad56 = 0;
  setIcon(sys->icon());
  setPower(sys->revertChannel()->power());
  prewave_delay = 0;
  _unknown61 = 0x01;
  _unknown62 = 0x03;
  _unknown63 = 0xff;
}

APRSSystem *
D878UVCodeplug::aprs_setting_t::toAPRSSystem() {
  return new APRSSystem(
        tr("APRS %1").arg(getDestination()), nullptr,
        getDestination(), to_ssid, getSource(), from_ssid,
        getPath(), getIcon(), "", getAutoTXInterval());
}

void
D878UVCodeplug::aprs_setting_t::linkAPRSSystem(APRSSystem *sys, CodeplugContext &ctx) {
  // First, try to find a matching analog channel in list
  AnalogChannel *ch = ctx.config()->channelList()->findAnalogChannelByTxFreq(getFrequency());
  if (! ch) {
    // If no channel is found, create one with the settings from APRS channel:
    ch = new AnalogChannel("APRS Channel", getFrequency(), getFrequency(), getPower(),
                           0, false, AnalogChannel::Admit::Free, 1, Signaling::SIGNALING_NONE,
                           getSignaling(), AnalogChannel::Bandwidth::Wide, nullptr);
    logInfo() << "No matching APRS chanel found for TX frequency " << getFrequency()
              << ", create one as 'APRS Channel'";
    ctx.config()->channelList()->add(ch);
  }
  sys->setRevertChannel(ch);
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::gps_systems_t
 * ******************************************************************************************** */
D878UVCodeplug::gps_systems_t::gps_systems_t() {
  clear();
}

void
D878UVCodeplug::gps_systems_t::clear() {
  //memset(this, 0, sizeof(gps_systems_t));
  for (int i=0; i<8; i++)
    setChannelIndex(i, 4002);
}

bool
D878UVCodeplug::gps_systems_t::isValid(int idx) const {
  if ((idx<0)||(idx>7))
    return false;
  return 0 != this->getContactId(idx);
}

uint32_t
D878UVCodeplug::gps_systems_t::getContactId(int idx) const {
  return decode_dmr_id_bcd((uint8_t *)&(talkgroups[idx]));
}
void
D878UVCodeplug::gps_systems_t::setContactId(int idx, uint32_t number) {
  encode_dmr_id_bcd((uint8_t *)&(talkgroups[idx]), number);
}

DigitalContact::Type
D878UVCodeplug::gps_systems_t::getContactType(int idx) const {
  switch (calltypes[idx]) {
  case 1: return DigitalContact::GroupCall;
  case 2: return DigitalContact::AllCall;
  default:
    return DigitalContact::PrivateCall;
  }
}
void
D878UVCodeplug::gps_systems_t::setContactType(int idx, DigitalContact::Type type) {
  switch (type) {
  case DigitalContact::PrivateCall: calltypes[idx]  = 0; break;
  case DigitalContact::GroupCall: calltypes[idx]  = 1; break;
  case DigitalContact::AllCall: calltypes[idx]  = 2; break;
  }
}

uint16_t
D878UVCodeplug::gps_systems_t::getChannelIndex(int idx) const {
  return qFromLittleEndian(digi_channels[idx]);
}
void
D878UVCodeplug::gps_systems_t::setChannelIndex(int idx, uint16_t ch_index) {
  digi_channels[idx] = qToLittleEndian(ch_index);
}

void
D878UVCodeplug::gps_systems_t::fromGPSSystemObj(GPSSystem *sys, const Config *conf) {
  int idx = conf->posSystems()->indexOfGPSSys(sys);
  if ((idx < 0) || idx > 7)
    return;
  if (sys->hasContact()) {
    setContactId(idx, sys->contactObj()->number());
    setContactType(idx, sys->contactObj()->type());
  }
  if (sys->hasRevertChannel() && (SelectedChannel::get() != (Channel *)sys->revertChannel())) {
    digi_channels[idx] = conf->channelList()->indexOf(sys->revertChannel());
    timeslots[idx] = 0; // Use TS of channel
  }
}

void
D878UVCodeplug::gps_systems_t::fromGPSSystems(const Config *conf) {
  if (conf->posSystems()->gpsCount() > 8)
    return;
  for (int i=0; i<conf->posSystems()->gpsCount(); i++)
    fromGPSSystemObj(conf->posSystems()->gpsSystem(i), conf);
}

GPSSystem *
D878UVCodeplug::gps_systems_t::toGPSSystemObj(int idx) const {
  if (! isValid(idx))
    return nullptr;
  return new GPSSystem(tr("GPS Sys #%1").arg(idx+1));
}

bool
D878UVCodeplug::gps_systems_t::linkGPSSystem(int idx, GPSSystem *sys, const CodeplugContext &ctx) const {
  // Clear revert channel from GPS system
  sys->setRevertChannel(nullptr);
  // if a revert channel is defined -> link to it
  if (ctx.hasChannel(getChannelIndex(idx)) && ctx.getChannel(getChannelIndex(idx))->is<DigitalChannel>())
    sys->setRevertChannel(ctx.getChannel(getChannelIndex(idx))->as<DigitalChannel>());

  // Search for a matching contact in contacts
  DigitalContact *cont = ctx.config()->contacts()->findDigitalContact(getContactId(idx));
  // If no matching contact is found, create one
  if (nullptr == cont) {
    cont = new DigitalContact(getContactType(idx), tr("GPS #%1 Contact").arg(idx+1),
                              getContactId(idx), false);
    ctx.config()->contacts()->add(cont);
  }
  // link contact to GPS system.
  sys->setContact(cont);

  return true;
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::ChannelElement
 * ******************************************************************************************** */
D878UVCodeplug::ChannelElement::ChannelElement(uint8_t *ptr, uint size)
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
  setUInt2(0x0019, 0, (uint)ptt);
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
  return 1 == getUInt2(0x0035, 0);
}
void
D878UVCodeplug::ChannelElement::enableTXDigitalAPRS(bool enable) {
  setUInt2(0x0035, 0, (enable ? 0x01 : 0x00));
}
bool
D878UVCodeplug::ChannelElement::txAnalogAPRS() const {
  return 2 == getUInt2(0x0035, 0);
}
void
D878UVCodeplug::ChannelElement::enableTXAnalogAPRS(bool enable) {
  setUInt2(0x0035, 0, (enable ? 0x02 : 0x00));
}

D878UVCodeplug::ChannelElement::APRSPTT
D878UVCodeplug::ChannelElement::analogAPRSPTTSetting() const {
  return (APRSPTT)getUInt8(0x0036);
}
void
D878UVCodeplug::ChannelElement::setAnalogAPRSPTTSetting(APRSPTT ptt) {
  setUInt8(0x0036, (uint)ptt);
}

D878UVCodeplug::ChannelElement::APRSPTT
D878UVCodeplug::ChannelElement::digitalAPRSPTTSetting() const {
  return (APRSPTT)getUInt8(0x0037);
}
void
D878UVCodeplug::ChannelElement::setDigitalAPRSPTTSetting(APRSPTT ptt) {
  setUInt8(0x0037, (uint)ptt);
}

uint
D878UVCodeplug::ChannelElement::digitalAPRSSystemIndex() const {
  return getUInt8(0x0038);
}
void
D878UVCodeplug::ChannelElement::setDigitalAPRSSystemIndex(uint idx) {
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
  Channel *ch = AnytoneCodeplug::ChannelElement::toChannelObj(ctx);
  if (nullptr == ch)
    return nullptr;
  // Nothing else to do
  return ch;
}

bool
D878UVCodeplug::ChannelElement::linkChannelObj(Channel *c, Context &ctx) const {
  if (! AnytoneCodeplug::ChannelElement::linkChannelObj(c, ctx))
    return false;

  if (c->is<DigitalChannel>()) {
    DigitalChannel *dc = c->as<DigitalChannel>();
    // Link to GPS system
    if (txDigitalAPRS() && ctx.has<GPSSystem>(digitalAPRSSystemIndex()))
      dc->aprsObj(ctx.get<GPSSystem>(digitalAPRSSystemIndex()));
    // Link APRS system if one is defined
    //  There can only be one active APRS system, hence the index is fixed to one.
    if (txAnalogAPRS() && ctx.has<APRSSystem>(0))
      dc->aprsObj(ctx.get<APRSSystem>(0));
    // If roaming is not disabled -> link to default roaming zone
    if (roamingEnabled())
      dc->setRoamingZone(DefaultRoamingZone::get());
  } else if (c->is<AnalogChannel>()) {
    AnalogChannel *ac = c->as<AnalogChannel>();
    // Link APRS system if one is defined
    //  There can only be one active APRS system, hence the index is fixed to one.
    if (txAnalogAPRS() && ctx.has<APRSSystem>(0))
      ac->setAPRSSystem(ctx.get<APRSSystem>(0));
  }

  return true;
}

bool
D878UVCodeplug::ChannelElement::fromChannelObj(const Channel *c, Context &ctx) {
  if (! AnytoneCodeplug::ChannelElement::fromChannelObj(c, ctx))
    return false;

  if (const DigitalChannel *dc = c->as<DigitalChannel>()) {
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
  } else if (const AnalogChannel *ac = c->as<AnalogChannel>()) {
    // Set APRS system
    enableRXAPRS(false);
    if (nullptr != ac->aprsSystem()) {
      enableTXAnalogAPRS(true);
      enableRXAPRS(true);
    }
  }

  return true;
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::RoamingChannelElement
 * ******************************************************************************************** */
D878UVCodeplug::RoamingChannelElement::RoamingChannelElement(uint8_t *ptr, uint size)
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

uint
D878UVCodeplug::RoamingChannelElement::rxFrequency() const {
  return getBCD8_le(0x0000)*10;
}
void
D878UVCodeplug::RoamingChannelElement::setRXFrequency(uint hz) {
  setBCD8_le(0x0000, hz/10);
}
uint
D878UVCodeplug::RoamingChannelElement::txFrequency() const {
  return getBCD8_le(0x0004)*10;
}
void
D878UVCodeplug::RoamingChannelElement::setTXFrequency(uint hz) {
  setBCD8_le(0x0004, hz/10);
}
uint
D878UVCodeplug::RoamingChannelElement::colorCode() const {
  return getUInt8(0x0008);
}
void
D878UVCodeplug::RoamingChannelElement::setColorCode(uint cc) {
  setUInt8(0x0008, cc);
}

DigitalChannel::TimeSlot
D878UVCodeplug::RoamingChannelElement::timeSlot() const {
  switch (getUInt8(0x0009)) {
  case 0x00: return DigitalChannel::TimeSlot::TS1;
  case 0x01: return DigitalChannel::TimeSlot::TS2;
  }
  return DigitalChannel::TimeSlot::TS1;
}
void
D878UVCodeplug::RoamingChannelElement::setTimeSlot(DigitalChannel::TimeSlot ts) {
  switch (ts) {
  case DigitalChannel::TimeSlot::TS1: setUInt8(0x0009, 0x00); break;
  case DigitalChannel::TimeSlot::TS2: setUInt8(0x0009, 0x01); break;
  }
}

QString
D878UVCodeplug::RoamingChannelElement::name() const {
  return readASCII(0x000a, 16, 0x00);
}
void
D878UVCodeplug::RoamingChannelElement::setName(const QString &name) {
  writeASCII(0x000a, name, 16, 0x00);
}

bool
D878UVCodeplug::RoamingChannelElement::fromChannel(const DigitalChannel *ch) {
  setName(ch->name());
  setRXFrequency(ch->rxFrequency());
  setTXFrequency(ch->txFrequency());
  setColorCode(ch->colorCode());
  setTimeSlot(ch->timeSlot());
  return true;
}

DigitalChannel *
D878UVCodeplug::RoamingChannelElement::toChannel(Context &ctx) {
  // Find matching channel for RX, TX frequency, TS and CC
  double rx = rxFrequency(), tx = txFrequency();
  DigitalChannel *digi = ctx.config()->channelList()->findDigitalChannel(
        rx, tx, timeSlot(), colorCode());
  if (nullptr == digi) {
    // If no matching channel can be found -> create one
    digi = new DigitalChannel(name(), rxFrequency(), txFrequency(),
                              Channel::Power::Low, 0, false, DigitalChannel::Admit::ColorCode,
                              colorCode(), timeSlot(), nullptr, nullptr, nullptr,
                              nullptr, nullptr, nullptr);
    logDebug() << "Create channel '" << digi->name() << "' as roaming channel.";
    ctx.config()->channelList()->add(digi);
  }
  return digi;
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::RoamingZoneElement
 * ******************************************************************************************** */
D878UVCodeplug::RoamingZoneElement::RoamingZoneElement(uint8_t *ptr, uint size)
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
D878UVCodeplug::RoamingZoneElement::hasMember(uint n) const {
  return (0xff != member(n));
}
uint
D878UVCodeplug::RoamingZoneElement::member(uint n) const {
  return getUInt8(0x0000 + n);
}
void
D878UVCodeplug::RoamingZoneElement::setMember(uint n, uint idx) {
  setUInt8(0x0000 + n, idx);
}
void
D878UVCodeplug::RoamingZoneElement::clearMember(uint n) {
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
    RoamingZone *zone, const QHash<DigitalChannel *, uint> &map)
{
  clear();
  setName(zone->name());
  for (int i=0; i<std::min(NUM_CH_PER_ROAMINGZONE, zone->count()); i++) {
    setMember(i, map.value(zone->channel(i), 0xff));
  }
  return true;
}

RoamingZone *
D878UVCodeplug::RoamingZoneElement::toRoamingZone() const {
  return new RoamingZone(name());
}

bool
D878UVCodeplug::RoamingZoneElement::linkRoamingZone(
    RoamingZone *zone, const QHash<uint, DigitalChannel*> &map)
{
  for (uint8_t i=0; (i<NUM_CH_PER_ROAMINGZONE)&&hasMember(i); i++) {
    DigitalChannel *digi = nullptr;
    if (map.contains(member(i))) {
      // If matching index is known -> resolve directly
      digi = map.value(member(i));
    } else {
      logError() << "Cannot link roaming zone '" << zone->name()
                 << "', unknown roaming channel index " << member(i);
      return false;
    }
    zone->addChannel(digi);
  }
  return true;
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug
 * ******************************************************************************************** */
D878UVCodeplug::D878UVCodeplug(QObject *parent)
  : D868UVCodeplug(parent)
{
  // Rename image
  image(0).setName("Anytone AT-D878UV Codeplug");

  // Roaming channel bitmaps
  image(0).addElement(ADDR_ROAMING_CHANNEL_BITMAP, ROAMING_CHANNEL_BITMAP_SIZE);
  // Roaming zone bitmaps
  image(0).addElement(ADDR_ROAMING_ZONE_BITMAP, ROAMING_ZONE_BITMAP_SIZE);
}

void
D878UVCodeplug::clear() {
  D868UVCodeplug::clear();
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
  for (int i=0; i<config->roaming()->count(); i++)
    roaming_zone_bitmap[i/8] |= (1<<(i%8));

  // Mark roaming channels
  uint8_t *roaming_ch_bitmap = data(ADDR_ROAMING_CHANNEL_BITMAP);
  memset(roaming_ch_bitmap, 0x00, ROAMING_CHANNEL_BITMAP_SIZE);
  // Get all (unique) channels used in roaming
  QSet<DigitalChannel*> roaming_channels;
  config->roaming()->uniqueChannels(roaming_channels);
  for (int i=0; i<std::min(NUM_ROAMING_CHANNEL,roaming_channels.count()); i++)
    roaming_ch_bitmap[i/8] |= (1<<(i%8));
}


bool
D878UVCodeplug::encodeElements(const Flags &flags, Context &ctx)
{
  // Encode everything common between d868uv and d878uv radios.
  if (! D868UVCodeplug::encodeElements(flags, ctx))
    return false;

  if (! this->encodeRoaming(flags, ctx))
    return false;

  return true;
}


bool
D878UVCodeplug::decodeElements(Context &ctx)
{
  // Decode everything commong between d868uv and d878uv codeplugs.
  if (! D868UVCodeplug::decodeElements(ctx))
    return false;

  if (! this->createRoaming(ctx))
    return false;

  if (! this->linkRoaming(ctx))
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
D878UVCodeplug::encodeChannels(const Flags &flags, Context &ctx) {
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
D878UVCodeplug::createChannels(Context &ctx) {
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
D878UVCodeplug::linkChannels(Context &ctx) {
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
D878UVCodeplug::allocateGeneralSettings() {
  // override allocation of general settings for D878UV code-plug. General settings are larger!
  image(0).addElement(ADDR_GENERAL_CONFIG, GENERAL_CONFIG_SIZE);
  image(0).addElement(ADDR_GENERAL_CONFIG_EXT1, GENERAL_CONFIG_EXT1_SIZE);
  image(0).addElement(ADDR_GENERAL_CONFIG_EXT2, GENERAL_CONFIG_EXT2_SIZE);

}
bool
D878UVCodeplug::encodeGeneralSettings(Config *config, const Flags &flags) {
  ((D878UVCodeplug::general_settings_base_t *)data(ADDR_GENERAL_CONFIG))->fromConfig(config, flags);
  ((general_settings_ext1_t *)data(ADDR_GENERAL_CONFIG_EXT1))->fromConfig(config, flags);
  ((general_settings_ext2_t *)data(ADDR_GENERAL_CONFIG_EXT2))->fromConfig(config, flags);
  return true;
}
bool
D878UVCodeplug::decodeGeneralSettings(Config *config) {
  ((D878UVCodeplug::general_settings_base_t *)data(ADDR_GENERAL_CONFIG))->updateConfig(config);
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
D878UVCodeplug::encodeGPSSystems(Config *config, const Flags &flags) {
  // replaces D868UVCodeplug::encodeGPSSystems

  // Encode APRS system (there can only be one)
  if (0 < config->posSystems()->aprsCount()) {
    ((aprs_setting_t *)data(ADDR_APRS_SETTING))->fromAPRSSystem(config->posSystems()->aprsSystem(0));
    uint8_t *aprsmsg = (uint8_t *)data(ADDR_APRS_MESSAGE);
    encode_ascii(aprsmsg, config->posSystems()->aprsSystem(0)->message(), 60, 0x00);
  }

  // Encode GPS systems
  gps_systems_t *gps = (gps_systems_t *)data(ADDR_GPS_SETTING);
  gps->fromGPSSystems(config);
  if (0 < config->posSystems()->gpsCount()) {
    // If there is at least one GPS system defined -> set auto TX interval.
    //  This setting might be overridden by any analog APRS system below
    aprs_setting_t *aprs = (aprs_setting_t *)data(ADDR_APRS_SETTING);
    aprs->setAutoTxInterval(config->posSystems()->gpsSystem(0)->period());
    aprs->setManualTxInterval(config->posSystems()->gpsSystem(0)->period());
  }
  return true;
}

bool
D878UVCodeplug::createGPSSystems(Config *config, CodeplugContext &ctx) {
  // replaces D868UVCodeplug::createGPSSystems

  // Before creating any GPS/APRS systems, get global auto TX intervall
  uint pos_intervall = ((aprs_setting_t *)data(ADDR_APRS_SETTING))->getAutoTXInterval();

  // Create APRS system (if enabled)
  aprs_setting_t *aprs = (aprs_setting_t *)data(ADDR_APRS_SETTING);
  uint8_t *aprsmsg = (uint8_t *)data(ADDR_APRS_MESSAGE);
  if (aprs->isValid()) {
    APRSSystem *sys = aprs->toAPRSSystem();
    sys->setPeriod(pos_intervall);
    sys->setMessage(decode_ascii(aprsmsg, 60, 0x00));
    ctx.addAPRSSystem(sys,0);
  }

  // Create GPS systems
  gps_systems_t *gps_systems = (gps_systems_t *)data(ADDR_GPS_SETTING);
  for (int i=0; i<NUM_GPS_SYSTEMS; i++) {
    if (! gps_systems->isValid(i))
      continue;
    GPSSystem *sys = gps_systems->toGPSSystemObj(i);
    if (sys)
      logDebug() << "Create GPS sys '" << sys->name() << "' at idx " << i << ".";
    sys->setPeriod(pos_intervall);
    ctx.addGPSSystem(sys, i);
  }
  return true;
}

bool
D878UVCodeplug::linkGPSSystems(Config *config, CodeplugContext &ctx) {
  // replaces D868UVCodeplug::linkGPSSystems

  // Link APRS system
  aprs_setting_t *aprs = (aprs_setting_t *)data(ADDR_APRS_SETTING);
  if (aprs->isValid()) {
    aprs->linkAPRSSystem(ctx.config()->posSystems()->aprsSystem(0), ctx);
  }

  // Link GPS systems
  gps_systems_t *gps_systems = (gps_systems_t *)data(ADDR_GPS_SETTING);
  for (int i=0; i<NUM_GPS_SYSTEMS; i++) {
    if (! gps_systems->isValid(i))
      continue;
    gps_systems->linkGPSSystem(i, ctx.getGPSSystem(i), ctx);
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
      logDebug() << "Allocate roaming zone at " << hex << addr;
      image(0).addElement(addr, ROAMING_ZONE_SIZE);
    }
  }
}

bool
D878UVCodeplug::encodeRoaming(const Flags &flags, Context &ctx) {
  // Encode roaming channels
  QHash<DigitalChannel *, uint> roaming_ch_map;
  {
    // Get set of unique roaming channels
    QSet<DigitalChannel*> roaming_channels;
    ctx.config()->roaming()->uniqueChannels(roaming_channels);
    // Encode channels and store in index<->channel map
    int i=0; QSet<DigitalChannel*>::iterator ch=roaming_channels.begin();
    for(; ch != roaming_channels.end(); ch++, i++) {
      roaming_ch_map[*ch] = i;
      uint32_t addr = ADDR_ROAMING_CHANNEL_0+i*ROAMING_CHANNEL_OFFSET;
      RoamingChannelElement rch(data(addr));
      rch.fromChannel(*ch);
    }
  }

  // Encode roaming zones
  for (int i=0; i<ctx.config()->roaming()->count(); i++){
    uint32_t addr = ADDR_ROAMING_ZONE_0+i*ROAMING_ZONE_OFFSET;
    RoamingZoneElement zone(data(addr));
    logDebug() << "Encode roaming zone " << ctx.config()->roaming()->zone(i)->name()
               << " (" << (i+1) << ") at " << QString::number(addr, 16);
    zone.fromRoamingZone(ctx.config()->roaming()->zone(i), roaming_ch_map);
  }

  return true;
}

bool
D878UVCodeplug::createRoaming(Context &ctx) {
  QHash<uint, DigitalChannel*> map;
  // Create or find roaming channels
  uint8_t *roaming_channel_bitmap = data(ADDR_ROAMING_CHANNEL_BITMAP);
  for (int i=0; i<NUM_ROAMING_CHANNEL; i++) {
    uint8_t byte=i/8, bit=i%8;
    if (0 == ((roaming_channel_bitmap[byte]>>bit) & 0x01))
      continue;
    uint32_t addr = ADDR_ROAMING_CHANNEL_0 + i*ROAMING_CHANNEL_OFFSET;
    RoamingChannelElement ch(data(addr));
    if (DigitalChannel *digi = ch.toChannel(ctx))
      map.insert(i, digi);
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
    ctx.config()->roaming()->add(zone); ctx.add(zone, i);
    z.linkRoamingZone(zone, map);
  }

  return true;
}

bool
D878UVCodeplug::linkRoaming(Context &ctx) {
  // Pass, no need to link roaming channels.
  return true;
}

