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

#define ADDR_GENERAL_CONFIG       0x02500000
#define GENERAL_CONFIG_SIZE       0x00000100
static_assert(
  GENERAL_CONFIG_SIZE == sizeof(D878UVCodeplug::general_settings_base_t),
  "D878UVCodeplug::general_settings_base_t size check failed.");

#define ADDR_APRS_SETTING         0x02501000 // Address of APRS settings
#define APRS_SETTING_SIZE         0x00000040 // Size of the APRS settings
#define ADDR_APRS_MESSAGE         0x02501200 // Address of APRS messages
#define APRS_MESSAGE_SIZE         0x00000040 // Size of APRS messages
static_assert(
  APRS_SETTING_SIZE == sizeof(D878UVCodeplug::aprs_setting_t),
  "D878UVCodeplug::aprs_setting_t size check failed.");

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

    sms_format = SMS_FMT_DMR;

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
  // round up to multiples of 30
  auto_tx_interval = (sec+29)/30;
}

int
D878UVCodeplug::aprs_setting_t::getManualTXInterval() const {
  return manual_tx_interval;
}
void
D878UVCodeplug::aprs_setting_t::setManualTxInterval(int sec) {
  // round up to multiples of 30
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
  case POWER_LOW: return Channel::LowPower;
  case POWER_MID: return Channel::MidPower;
  case POWER_HIGH: return Channel::HighPower;
  case POWER_TURBO: return Channel::MaxPower;
  default: break;
  }
  return Channel::HighPower;
}

void
D878UVCodeplug::aprs_setting_t::setPower(Channel::Power pwr) {
  switch (pwr) {
  case Channel::MinPower:
  case Channel::LowPower:
    power = aprs_setting_t::POWER_LOW;
    break;
  case Channel::MidPower:
    power = aprs_setting_t::POWER_MID;
    break;
  case Channel::HighPower:
    power = aprs_setting_t::POWER_HIGH;
    break;
  case Channel::MaxPower:
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
  setFrequency(sys->channel()->txFrequency());
  tx_delay = 0x03;
  setSignaling(sys->channel()->txTone());
  setManualTxInterval(sys->period());
  setAutoTxInterval(sys->period());
  tx_tone_enable = 0;

  setDestination(sys->destination(), sys->destSSID());
  setSource(sys->source(), sys->srcSSID());
  setPath(sys->path());
  _pad56 = 0;
  setIcon(sys->icon());
  setPower(sys->channel()->power());
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
                           0, false, AnalogChannel::AdmitFree, 1, Signaling::SIGNALING_NONE,
                           getSignaling(), AnalogChannel::BWWide, nullptr);
    logInfo() << "No matching APRS chanel found for TX frequency " << getFrequency()
              << ", create one as 'APRS Channel'";
    ctx.config()->channelList()->addChannel(ch);
  }
  sys->setChannel(ch);
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::roaming_channel_t
 * ******************************************************************************************** */
double
D878UVCodeplug::roaming_channel_t::getRXFrequency() const {
  return decode_frequency(qFromBigEndian(rx_frequency));
}
void
D878UVCodeplug::roaming_channel_t::setRXFrequency(double f) {
  rx_frequency = qToBigEndian(encode_frequency(f));
}

double
D878UVCodeplug::roaming_channel_t::getTXFrequency() const {
  return decode_frequency(qFromBigEndian(tx_frequency));
}
void
D878UVCodeplug::roaming_channel_t::setTXFrequency(double f) {
  tx_frequency = qToBigEndian(encode_frequency(f));
}

DigitalChannel::TimeSlot
D878UVCodeplug::roaming_channel_t::getTimeslot() const {
  if (0 == timeslot)
    return DigitalChannel::TimeSlot1;
  return DigitalChannel::TimeSlot2;
}
void
D878UVCodeplug::roaming_channel_t::setTimeslot(DigitalChannel::TimeSlot ts) {
  if (DigitalChannel::TimeSlot1 == ts)
    timeslot = 0;
  else
    timeslot = 1;
}

uint
D878UVCodeplug::roaming_channel_t::getColorCode() const {
  return std::min(uint8_t(15), colorcode);
}
void
D878UVCodeplug::roaming_channel_t::setColorCode(uint8_t cc) {
  colorcode = std::min(uint8_t(15), cc);
}

QString
D878UVCodeplug::roaming_channel_t::getName() const {
  return decode_ascii(name, 16, 0x00);
}
void
D878UVCodeplug::roaming_channel_t::setName(const QString &name) {
  encode_ascii(this->name, name, 16, 0x00);
}

void
D878UVCodeplug::roaming_channel_t::fromChannel(DigitalChannel *ch) {
  setName(ch->name());
  setRXFrequency(ch->rxFrequency());
  setTXFrequency(ch->txFrequency());
  setColorCode(ch->colorCode());
  setTimeslot(ch->timeslot());
}

DigitalChannel *
D878UVCodeplug::roaming_channel_t::toChannel(CodeplugContext &ctx) {
  // Find matching channel for RX, TX frequency, TS and CC
  double rx = getRXFrequency(), tx = getTXFrequency();
  DigitalChannel *digi = ctx.config()->channelList()->findDigitalChannel(
        rx, tx, getTimeslot(), getColorCode());
  if (nullptr == digi) {
    // If no matching channel can be found -> create one
    digi = new DigitalChannel(getName(), getRXFrequency(), getTXFrequency(),
                              Channel::LowPower, 0, false, DigitalChannel::AdmitColorCode,
                              getColorCode(), getTimeslot(), nullptr, nullptr, nullptr,
                              nullptr, nullptr);
    logDebug() << "Create channel '" << digi->name() << "' as roaming channel.";
    ctx.config()->channelList()->addChannel(digi);
  }
  return digi;
}

/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::roaming_zone_t
 * ******************************************************************************************** */
QString
D878UVCodeplug::roaming_zone_t::getName() const {
  return decode_ascii(name, 16, 0x00);
}
void
D878UVCodeplug::roaming_zone_t::setName(const QString &name) {
  encode_ascii(this->name, name, 16, 0x00);
}

void
D878UVCodeplug::roaming_zone_t::fromRoamingZone(RoamingZone *zone, const QHash<DigitalChannel *,int> &map) {
  memset(channels, 0xff, sizeof(channels));
  setName(zone->name());
  memset(_unused80, 0x00, sizeof(_unused80));
  for (int i=0; i<std::min(64, zone->count()); i++) {
    channels[i] = map.value(zone->channel(i), 0xff);
  }
}

RoamingZone *
D878UVCodeplug::roaming_zone_t::toRoamingZone() {
  return new RoamingZone(getName());
}

bool
D878UVCodeplug::roaming_zone_t::linkRoamingZone(RoamingZone *zone, CodeplugContext &ctx) {
  for (uint8_t i=0; (i<NUM_CH_PER_ROAMINGZONE)&&(0xff!=channels[i]); i++) {
    DigitalChannel *digi = nullptr;
    if (ctx.hasRoamingChannel(channels[i])) {
      // If matching index is known -> resolve directly
      digi = ctx.getRoamingChannel(channels[i]);
    } else {
      logError() << "Cannot link roaming zone '" << zone->name()
                 << "', unknown roaming channel index " << channels[i];
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
D878UVCodeplug::allocateUntouched() {
  // First allocate everything common between D868UV and D878UV codeplugs.
  D868UVCodeplug::allocateUntouched();

  // APRS settings
  image(0).addElement(ADDR_APRS_SETTING, APRS_SETTING_SIZE);
  image(0).addElement(ADDR_APRS_MESSAGE, APRS_MESSAGE_SIZE);
}

void
D878UVCodeplug::allocateForEncoding() {
  // First allocate everything common between D868UV and D878UV codeplugs.
  D868UVCodeplug::allocateForEncoding();

  /*
   * Allocate roaming channel and zones
   */
  uint8_t *roaming_channel_bitmap = data(ADDR_ROAMING_CHANNEL_BITMAP);
  for (uint8_t i=0; i<NUM_ROAMING_CHANNEL; i++) {
    // Get byte and bit for radio ID
    uint16_t bit = i%8, byte = i/8;
    // if disabled -> skip
    if (0 == ((roaming_channel_bitmap[byte]>>bit) & 0x01))
      continue;
    // Allocate roaming channel
    uint32_t addr = ADDR_ROAMING_CHANNEL_0 + i*ROAMING_CHANNEL_OFFSET;
    if (nullptr == data(addr, 0)) {
      logDebug() << "Allocate roaming channel at " << hex << addr;
      image(0).addElement(addr, ROAMING_CHANNEL_SIZE);
    }
  }
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

void
D878UVCodeplug::allocateForDecoding() {
  // First allocate everything common between D868UV and D878UV codeplugs.
  D868UVCodeplug::allocateForDecoding();

  /*
   * Allocate roaming channel and zones
   */
  uint8_t *roaming_channel_bitmap = data(ADDR_ROAMING_CHANNEL_BITMAP);
  for (uint8_t i=0; i<NUM_ROAMING_CHANNEL; i++) {
    // Get byte and bit for radio ID
    uint16_t bit = i%8, byte = i/8;
    // if disabled -> skip
    if (0 == ((roaming_channel_bitmap[byte]>>bit) & 0x01))
      continue;
    // Allocate roaming channel
    uint32_t addr = ADDR_ROAMING_CHANNEL_0 + i*ROAMING_CHANNEL_OFFSET;
    if (nullptr == data(addr, 0)) {
      image(0).addElement(addr, ROAMING_CHANNEL_SIZE);
    }
  }
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
      image(0).addElement(addr, ROAMING_ZONE_SIZE);
    }
  }

  // APRS settings and messages
  image(0).addElement(ADDR_APRS_SETTING, APRS_SETTING_SIZE);
  image(0).addElement(ADDR_APRS_MESSAGE, APRS_MESSAGE_SIZE);
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
D878UVCodeplug::encode(Config *config, const Flags &flags)
{
  // Encode everything common between d868uv and d878uv radios.
  D868UVCodeplug::encode(config, flags);

  // Encode APRS system (there can only be one)
  if (0 < config->posSystems()->aprsCount()) {
    ((aprs_setting_t *)data(ADDR_APRS_SETTING))->fromAPRSSystem(config->posSystems()->aprsSystem(0));
    uint8_t *aprsmsg = (uint8_t *)data(ADDR_APRS_MESSAGE);
    encode_ascii(aprsmsg, config->posSystems()->aprsSystem(0)->message(), 60, 0x00);
  }

  // Encode roaming channels
  QHash<DigitalChannel *, int> roaming_ch_map;
  {
    // Get set of unique roaming channels
    QSet<DigitalChannel*> roaming_channels;
    config->roaming()->uniqueChannels(roaming_channels);
    // Encode channels and store in index<->channel map
    int i=0; QSet<DigitalChannel*>::iterator ch=roaming_channels.begin();
    for(; ch != roaming_channels.end(); ch++, i++) {
      roaming_ch_map[*ch] = i;
      uint32_t addr = ADDR_ROAMING_CHANNEL_0+i*ROAMING_CHANNEL_OFFSET;
      roaming_channel_t *rch = (roaming_channel_t *)data(addr);
      rch->fromChannel(*ch);
      logDebug() << "Encode roaming channel " << (*ch)->name() << " (" << i
                 << ") at " << hex << addr;
    }
  }
  // Encode roaming zones
  for (int i=0; i<config->roaming()->count(); i++){
    uint32_t addr = ADDR_ROAMING_ZONE_0+i*ROAMING_ZONE_OFFSET;
    roaming_zone_t *zone = (roaming_zone_t *)data(addr);
    logDebug() << "Encode roaming zone " << config->roaming()->zone(i)->name() << " (" << (i+1)
               << ") at " << hex << addr;
    zone->fromRoamingZone(config->roaming()->zone(i), roaming_ch_map);
  }

  return true;
}

bool
D878UVCodeplug::decode(Config *config)
{
  // Maps code-plug indices to objects
  CodeplugContext ctx(config);
  return this->decode(config, ctx);
}

bool
D878UVCodeplug::decode(Config *config, CodeplugContext &ctx)
{
  // Decode everything commong between d868uv and d878uv codeplugs.
  D868UVCodeplug::decode(config, ctx);

  // Before creating any GPS/APRS systems, get global auto TX intervall
  uint pos_intervall = ((aprs_setting_t *)data(ADDR_APRS_SETTING))->getAutoTXInterval();

  // Create APRS system (if enabled)
  aprs_setting_t *aprs = (aprs_setting_t *)data(ADDR_APRS_SETTING);
  uint8_t *aprsmsg = (uint8_t *)data(ADDR_APRS_MESSAGE);
  if (aprs->isValid()) {
    APRSSystem *sys = aprs->toAPRSSystem();
    sys->setMessage(decode_ascii(aprsmsg, 60, 0x00));
    ctx.addAPRSSystem(sys,0);
  }

  // Create or find roaming channels
  uint8_t *roaming_channel_bitmap = data(ADDR_ROAMING_CHANNEL_BITMAP);
  for (int i=0; i<NUM_ROAMING_CHANNEL; i++) {
    uint8_t byte=i/8, bit=i%8;
    if (0 == ((roaming_channel_bitmap[byte]>>bit) & 0x01))
      continue;
    uint32_t addr = ADDR_ROAMING_CHANNEL_0 + i*ROAMING_CHANNEL_OFFSET;
    roaming_channel_t *ch = (roaming_channel_t *)data(addr);
    DigitalChannel *digi = ch->toChannel(ctx);
    if (digi) {
      logDebug() << "Register channel '" << digi->name() << "' as roaming channel " << i+1;
      ctx.addRoamingChannel(digi, i);
    }
  }

  // Create and link roaming zones
  uint8_t *roaming_zone_bitmap = data(ADDR_ROAMING_ZONE_BITMAP);
  for (int i=0; i<NUM_ROAMING_ZONES; i++) {
    uint8_t byte=i/8, bit=i%8;
    if (0 == ((roaming_zone_bitmap[byte]>>bit) & 0x01))
      continue;
    uint32_t addr = ADDR_ROAMING_ZONE_0 + i*ROAMING_ZONE_OFFSET;
    roaming_zone_t *z = (roaming_zone_t *)data(addr);
    RoamingZone *zone = z->toRoamingZone();
    ctx.addRoamingZone(zone, i+1);
    z->linkRoamingZone(zone, ctx);
  }

  // Link APRS system
  if (aprs->isValid()) {
    aprs->linkAPRSSystem(ctx.config()->posSystems()->aprsSystem(0), ctx);
  }

  return true;
}

void
D878UVCodeplug::allocateGeneralSettings() {
  // override allocation of general settings for D878UV code-plug. General settings are larger!
  image(0).addElement(ADDR_GENERAL_CONFIG, GENERAL_CONFIG_SIZE);
}
bool
D878UVCodeplug::encodeGeneralSettings(Config *config, const Flags &flags) {
  ((D878UVCodeplug::general_settings_base_t *)data(ADDR_GENERAL_CONFIG))->fromConfig(config, flags);
  return true;
}
bool
D878UVCodeplug::decodeGeneralSettings(Config *config) {
  ((D878UVCodeplug::general_settings_base_t *)data(ADDR_GENERAL_CONFIG))->updateConfig(config);
  return true;
}
