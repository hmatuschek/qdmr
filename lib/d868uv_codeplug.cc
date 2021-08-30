#include "d868uv_codeplug.hh"
#include "config.hh"
#include "utils.hh"
#include "channel.hh"
#include "gpssystem.hh"
#include "userdatabase.hh"
#include "config.h"
#include "logger.hh"
#include <cmath>

#include <QTimeZone>
#include <QtEndian>
#include <QSet>


#define NUM_CHANNELS              4000
#define NUM_CHANNEL_BANKS         32
#define CHANNEL_BANK_0            0x00800000
#define CHANNEL_BANK_SIZE         0x00002000
#define CHANNEL_BANK_31           0x00fc0000
#define CHANNEL_BANK_31_SIZE      0x00000800
#define CHANNEL_BANK_OFFSET       0x00040000
#define CHANNEL_BITMAP            0x024c1500
#define CHANNEL_BITMAP_SIZE       0x00000200

#define VFO_A_ADDR                0x00fc0800 // Address of VFO A settings (channel_t)
#define VFO_B_ADDR                0x00fc0840 // Address of VFO B settings (channel_t)
#define VFO_SIZE                  0x00000040 // Size of each VFO settings.

#define NUM_CONTACTS              10000      // Total number of contacts
#define NUM_CONTACT_BANKS         2500       // Number of contact banks
#define CONTACTS_PER_BANK         4
#define CONTACT_BANK_0            0x02680000 // First bank of 4 contacts
#define CONTACT_BANK_SIZE         0x00000190 // Size of 4 contacts
#define CONTACT_INDEX_LIST        0x02600000 // Address of contact index list
#define CONTACTS_BITMAP           0x02640000 // Address of contact bitmap
#define CONTACTS_BITMAP_SIZE      0x00000500 // Size of contact bitmap
#define CONTACT_ID_MAP            0x04340000 // Address of ID->Contact index map
#define CONTACT_ID_ENTRY_SIZE     sizeof(contact_map_t) // Size of each map entry


#define NUM_ANALOGCONTACTS        128
#define NUM_ANALOGCONTACT_BANKS   64
#define ANALOGCONTACTS_PER_BANK   2
#define ANALOGCONTACT_BANK_0      0x02940000
#define ANALOGCONTACT_BANK_SIZE   0x00000030
#define ANALOGCONTACT_INDEX_LIST  0x02900000 // Address of analog contact index list
#define ANALOGCONTACT_LIST_SIZE   0x00000080 // Size of analog contact index list
#define ANALOGCONTACT_BYTEMAP     0x02900100 // Address of contact bytemap
#define ANALOGCONTACT_BYTEMAP_SIZE 0x00000080 // Size of contact bytemap
#define ANALOGCONTACT_SIZE        0x00000018 // Size of analog contact
static_assert(
  ANALOGCONTACT_SIZE == sizeof(D868UVCodeplug::analog_contact_t),
  "D868UVCodeplug::analog_contact_t size check failed.");

#define NUM_RXGRP                 250        // Total number of RX group lists
#define ADDR_RXGRP_0              0x02980000 // Address of the first RX group list.
#define RXGRP_SIZE                0x00000120 // Size of each RX group list.
#define RXGRP_OFFSET              0x00000200 // Offset between group lists.
#define RXGRP_BITMAP              0x025C0B10 // Address of RX group list bitmap.
#define RXGRP_BITMAP_SIZE         0x00000020 // Storage size of RX group list bitmap.
static_assert(
  RXGRP_SIZE == sizeof(D868UVCodeplug::grouplist_t),
  "D868UVCodeplug::grouplist_t size check failed.");

#define NUM_ZONES                 250        // Maximum number of zones
#define NUM_CH_PER_ZONE           250        // Maximum number of channels per zone
#define ADDR_ZONE                 0x01000000 // Address of zone-channel lists, see zone_t
#define ZONE_SIZE                 0x00000200 // Size of each zone-channel list
#define ZONE_OFFSET               0x00000200 // Offset between zone-channel lists
#define ADDR_ZONE_NAME            0x02540000 // Address of zone names.
#define ZONE_NAME_SIZE            0x00000010 // Size of zone names
#define ZONE_NAME_OFFSET          0x00000020 // Offset between zone names.
#define ZONE_BITMAPS              0x024c1300 // Bitmap of all enabled zones
#define ZONE_BITMAPS_SIZE         0x00000020 // Size of the zone bitmap

#define NUM_RADIOIDS              250
#define ADDR_RADIOIDS             0x02580000
#define RADIOID_SIZE              0x00000020
#define RADIOID_BITMAP            0x024c1320
#define RADIOID_BITMAP_SIZE       0x00000020
static_assert(
  RADIOID_SIZE == sizeof(D868UVCodeplug::radioid_t),
  "D868UVCodeplug::radioid_t size check failed.");

#define NUM_SCAN_LISTS            250
#define NUM_SCANLISTS_PER_BANK    16
#define SCAN_LIST_BANK_0          0x01080000 // First scanlist bank
#define SCAN_LIST_OFFSET          0x00000200 // Offset to next list.
#define SCAN_LIST_SIZE            0x00000090 // Size of scan-list.
#define SCAN_LIST_BANK_OFFSET     0x00040000 // Offset to next bank
#define SCAN_BITMAP               0x024c1340 // Address of scan-list bitmap.
#define SCAN_BITMAP_SIZE          0x00000020 // Size of scan-list bitmap.
static_assert(
  SCAN_LIST_SIZE == sizeof(D868UVCodeplug::scanlist_t),
  "D868UVCodeplug::scanlist_t size check failed.");

#define ADDR_GENERAL_CONFIG       0x02500000
#define GENERAL_CONFIG_SIZE       0x000000d0
static_assert(
  GENERAL_CONFIG_SIZE == sizeof(D868UVCodeplug::general_settings_base_t),
  "D868UVCodeplug::general_settings_base_t size check failed.");

#define ADDR_ZONE_CHANNELS        0x02500100
#define ZONE_CHANNELS_SIZE        0x00000400
static_assert(
  ZONE_CHANNELS_SIZE == sizeof(D868UVCodeplug::zone_channels_t),
  "D868UVCodeplug::zone_channels_t size check failed.");

#define ADDR_DTMF_NUMBERS         0x02500500
#define DTMF_NUMBERS_SIZE         0x00000100
static_assert(
  DTMF_NUMBERS_SIZE == sizeof(D868UVCodeplug::dtmf_numbers_t),
  "D868UVCodeplug::dtmf_numbers_t size check failed.");

#define ADDR_BOOT_SETTINGS        0x02500600
#define BOOT_SETTINGS_SIZE        0x00000030
static_assert(
  BOOT_SETTINGS_SIZE == sizeof(D868UVCodeplug::boot_settings_t),
  "D868UVCodeplug::boot_settings_t size check failed.");

#define ADDR_GPS_SETTINGS         0x02501000
#define GPS_SETTINGS_SIZE         0x00000030
static_assert(
  GPS_SETTINGS_SIZE == sizeof(D868UVCodeplug::gps_settings_t),
  "D868UVCodeplug::gps_settings_t size check failed.");

#define ADDR_GPS_MESSAGE          0x02501100
#define GPS_MESSAGE_SIZE          0x00000030

#define NUM_MESSAGES              100
#define NUM_MESSAGES_PER_BANK     8
#define MESSAGE_SIZE              0x00000100
#define MESSAGE_BANK_0            0x02140000
#define MESSAGE_BANK_SIZE         0x00000800
#define MESSAGE_BANK_OFFSET       0x00040000
#define MESSAGE_INDEX_LIST        0x01640000
#define MESSAGE_BYTEMAP           0x01640800
#define MESSAGE_BYTEMAP_SIZE      0x00000090
static_assert(
  MESSAGE_SIZE == sizeof(D868UVCodeplug::message_t),
  "D868UVCodeplug::grouplist_t size check failed.");

#define ADDR_HOTKEY               0x025C0000
#define HOTKEY_SIZE               0x00000860
#define STATUSMESSAGE_BITMAP      0x025C0B00
#define STATUSMESSAGE_BITMAP_SIZE 0x00000010

#define ADDR_OFFSET_FREQ          0x024C2000
#define OFFSET_FREQ_SIZE          0x000003F0

#define ADDR_ALARM_SETTING        0x024C1400
#define ALARM_SETTING_SIZE        0x00000020
static_assert(
  ALARM_SETTING_SIZE == sizeof(D868UVCodeplug::alarm_settings_t),
  "D868UVCodeplug::alarm_settings_t size check failed.");

#define ADDR_ALARM_SETTING_EXT    0x024c1440
#define ALARM_SETTING_EXT_SIZE    0x00000030
static_assert(
  ALARM_SETTING_EXT_SIZE == sizeof(D868UVCodeplug::digital_alarm_settings_ext_t),
  "D868UVCodeplug::digital_alarm_settings_ext_t size check failed.");

#define FMBC_BITMAP               0x02480210
#define FMBC_BITMAP_SIZE          0x00000020
#define ADDR_FMBC                 0x02480000
#define FMBC_SIZE                 0x00000200
#define ADDR_FMBC_VFO             0x02480200
#define FMBC_VFO_SIZE             0x00000010

#define FIVE_TONE_ID_BITMAP       0x024C0C80
#define FIVE_TONE_ID_BITMAP_SIZE  0x00000010
#define NUM_FIVE_TONE_IDS         100
#define ADDR_FIVE_TONE_ID_LIST    0x024C0000
#define FIVE_TONE_ID_SIZE         0x00000020
#define FIVE_TONE_ID_LIST_SIZE    0x00000c80
static_assert(
  FIVE_TONE_ID_SIZE == sizeof(D868UVCodeplug::five_tone_id_t),
  "D868UVCodeplug::five_tone_id_t size check failed.");
static_assert(
  FIVE_TONE_ID_LIST_SIZE == (NUM_FIVE_TONE_IDS*sizeof(D868UVCodeplug::five_tone_id_t)),
  "D868UVCodeplug::five_tone_function_t list size check failed.");
#define NUM_FIVE_TONE_FUNCTIONS   16
#define ADDR_FIVE_TONE_FUNCTIONS  0x024C0D00
#define FIVE_TONE_FUNCTION_SIZE   0x00000020
#define FIVE_TONE_FUNCTIONS_SIZE  0x00000200
static_assert(
  FIVE_TONE_FUNCTION_SIZE == sizeof(D868UVCodeplug::five_tone_function_t),
  "D868UVCodeplug::five_tone_function_t size check failed.");
static_assert(
  FIVE_TONE_FUNCTIONS_SIZE == (NUM_FIVE_TONE_FUNCTIONS*sizeof(D868UVCodeplug::five_tone_function_t)),
  "D868UVCodeplug::five_tone_function_t list size check failed.");
#define ADDR_FIVE_TONE_SETTINGS   0x024C1000
#define FIVE_TONE_SETTINGS_SIZE   0x00000080
static_assert(
  FIVE_TONE_SETTINGS_SIZE == sizeof(D868UVCodeplug::five_tone_settings_t),
  "D868UVCodeplug::five_tone_settings_t size check failed.");

#define ADDR_DTMF_SETTINGS        0x024C1080
#define DTMF_SETTINGS_SIZE        0x00000050
static_assert(
  DTMF_SETTINGS_SIZE == sizeof(D868UVCodeplug::dtmf_settings_t),
  "D868UVCodeplug::dtmf_settings_t size check failed.");

#define NUM_TWO_TONE_IDS          24
#define TWO_TONE_IDS_BITMAP       0x024C1280
#define TWO_TONE_IDS_BITMAP_SIZE  0x00000010
#define ADDR_TWO_TONE_IDS         0x024C1100
#define TWO_TONE_ID_SIZE          0x00000010
static_assert(
  TWO_TONE_ID_SIZE == sizeof(D868UVCodeplug::two_tone_id_t),
  "D868UVCodeplug::two_tone_settings_t size check failed.");

#define NUM_TWO_TONE_FUNCTIONS    16
#define TWO_TONE_FUNCTIONS_BITMAP 0x024c2600
#define TWO_TONE_FUNC_BITMAP_SIZE 0x00000010
#define ADDR_TWO_TONE_FUNCTIONS   0x024c2400
#define TWO_TONE_FUNCTION_SIZE    0x00000020
static_assert(
  TWO_TONE_FUNCTION_SIZE == sizeof(D868UVCodeplug::two_tone_function_t),
  "D868UVCodeplug::two_tone_settings_t size check failed.");

#define ADDR_TWO_TONE_SETTINGS    0x024C1290
#define TWO_TONE_SETTINGS_SIZE    0x00000010
static_assert(
  TWO_TONE_SETTINGS_SIZE == sizeof(D868UVCodeplug::two_tone_settings_t),
  "D868UVCodeplug::two_tone_settings_t size check failed.");

#define ADDR_DMR_ENCRYPTION_LIST  0x024C1700
#define DMR_ENCRYPTION_LIST_SIZE  0x00000040
#define ADDR_DMR_ENCRYPTION_KEYS  0x024C1800
#define DMR_ENCRYPTION_KEYS_SIZE  0x00000500

using namespace Signaling;

Code _ctcss_num2code[52] = {
  SIGNALING_NONE, // 62.5 not supported
  CTCSS_67_0Hz,  SIGNALING_NONE, // 69.3 not supported
  CTCSS_71_9Hz,  CTCSS_74_4Hz,  CTCSS_77_0Hz,  CTCSS_79_7Hz,  CTCSS_82_5Hz,
  CTCSS_85_4Hz,  CTCSS_88_5Hz,  CTCSS_91_5Hz,  CTCSS_94_8Hz,  CTCSS_97_4Hz,  CTCSS_100_0Hz,
  CTCSS_103_5Hz, CTCSS_107_2Hz, CTCSS_110_9Hz, CTCSS_114_8Hz, CTCSS_118_8Hz, CTCSS_123_0Hz,
  CTCSS_127_3Hz, CTCSS_131_8Hz, CTCSS_136_5Hz, CTCSS_141_3Hz, CTCSS_146_2Hz, CTCSS_151_4Hz,
  CTCSS_156_7Hz,
  SIGNALING_NONE, // 159.8 not supported
  CTCSS_162_2Hz,
  SIGNALING_NONE, // 165.5 not supported
  CTCSS_167_9Hz,
  SIGNALING_NONE, // 171.3 not supported
  CTCSS_173_8Hz,
  SIGNALING_NONE, // 177.3 not supported
  CTCSS_179_9Hz,
  SIGNALING_NONE, // 183.5 not supported
  CTCSS_186_2Hz,
  SIGNALING_NONE, // 189.9 not supported
  CTCSS_192_8Hz,
  SIGNALING_NONE, SIGNALING_NONE, // 196.6 & 199.5 not supported
  CTCSS_203_5Hz,
  SIGNALING_NONE, // 206.5 not supported
  CTCSS_210_7Hz, CTCSS_218_1Hz, CTCSS_225_7Hz,
  SIGNALING_NONE, // 229.1 not supported
  CTCSS_233_6Hz, CTCSS_241_8Hz, CTCSS_250_3Hz,
  SIGNALING_NONE, SIGNALING_NONE // 254.1 and custom CTCSS not supported.
};

uint8_t
D868UVCodeplug::ctcss_code2num(Signaling::Code code) {
  for (uint8_t i=0; i<52; i++) {
    if (code == _ctcss_num2code[i])
      return i;
  }
  return 0;
}

Signaling::Code
D868UVCodeplug::ctcss_num2code(uint8_t num) {
  if (num >= 52)
    return Signaling::SIGNALING_NONE;
  return _ctcss_num2code[num];
}


/* ******************************************************************************************** *
 * Implementation of D868UVCodeplug::channel_t
 * ******************************************************************************************** */
D868UVCodeplug::channel_t::channel_t() {
  clear();
}

void
D868UVCodeplug::channel_t::clear() {
  memset(this, 0, sizeof(D868UVCodeplug::channel_t));
  custom_ctcss = qToLittleEndian(0x09cf); // some value
  scan_list_index  = 0xff; // None
  group_list_index = 0xff; // None
  id_index = 0;
  squelch_mode = SQ_CARRIER;
  tx_permit = ADMIT_ALWAYS;

}

bool
D868UVCodeplug::channel_t::isValid() const {
  return (0 != name[0]) && (0xff != name[0]);
}

double
D868UVCodeplug::channel_t::getRXFrequency() const {
  return decode_frequency(qFromBigEndian(rx_frequency));
}

void
D868UVCodeplug::channel_t::setRXFrequency(double f) {
  rx_frequency = qToBigEndian(encode_frequency(f));
}

double
D868UVCodeplug::channel_t::getTXFrequency() const {
  double f = decode_frequency(qFromBigEndian(rx_frequency));
  switch ((RepeaterMode) repeater_mode) {
  case RM_SIMPLEX:
    break;
  case RM_TXNEG:
    f -= decode_frequency(qFromBigEndian(tx_offset));
    break;
  case RM_TXPOS:
    f += decode_frequency(qFromBigEndian(tx_offset));
    break;
  }
  return f;
}

void
D868UVCodeplug::channel_t::setTXFrequency(double f) {
  if (getRXFrequency() == f) {
    tx_offset = encode_frequency(0);
    repeater_mode = RM_SIMPLEX;
  } else if (getRXFrequency() > f) {
    tx_offset = qToBigEndian(encode_frequency(getRXFrequency()-f));
    repeater_mode = RM_TXNEG;
  } else {
    tx_offset = qToBigEndian(encode_frequency(f-getRXFrequency()));
    repeater_mode = RM_TXPOS;
  }
}

QString
D868UVCodeplug::channel_t::getName() const {
  return decode_ascii(name, 16, 0);
}

void
D868UVCodeplug::channel_t::setName(const QString &name) {
  encode_ascii(this->name, name, 16, 0);
}

Signaling::Code
D868UVCodeplug::channel_t::getRXTone() const {
  if (rx_ctcss && (ctcss_receive < 52))
    return ctcss_num2code(ctcss_receive);
  else if (rx_dcs && (qFromLittleEndian(dcs_receive) < 512))
    return Signaling::fromDCSNumber(dec_to_oct(qFromLittleEndian(dcs_receive)), false);
  else if (rx_dcs && (qFromLittleEndian(dcs_receive) >= 512))
    return Signaling::fromDCSNumber(dec_to_oct(qFromLittleEndian(dcs_receive)-512), true);
 return Signaling::SIGNALING_NONE;
}

void
D868UVCodeplug::channel_t::setRXTone(Code code) {
  if (Signaling::SIGNALING_NONE == code) {
    rx_ctcss = rx_dcs = 0;
    ctcss_receive = dcs_receive = 0;
  } else if (Signaling::isCTCSS(code)) {
    rx_ctcss = 1;
    rx_dcs = 0;
    ctcss_receive = ctcss_code2num(code);
    dcs_receive = 0;
  } else if (Signaling::isDCSNormal(code)) {
    rx_ctcss = 0;
    rx_dcs = 1;
    ctcss_receive = 0;
    dcs_receive = qToLittleEndian(oct_to_dec(Signaling::toDCSNumber(code)));
  } else if (Signaling::isDCSInverted(code)) {
    rx_ctcss = 0;
    rx_dcs = 1;
    ctcss_receive = 0;
    dcs_receive = qToLittleEndian(oct_to_dec(Signaling::toDCSNumber(code))+512);
  }
}

Signaling::Code
D868UVCodeplug::channel_t::getTXTone() const {
  if (tx_ctcss && (ctcss_transmit < 52))
    return ctcss_num2code(ctcss_transmit);
  else if (tx_dcs && (qFromLittleEndian(dcs_transmit) < 512))
    return Signaling::fromDCSNumber(dec_to_oct(qFromLittleEndian(dcs_transmit)), false);
  else if (tx_dcs && (qFromLittleEndian(dcs_transmit) >= 512))
    return Signaling::fromDCSNumber(dec_to_oct(qFromLittleEndian(dcs_transmit)-512), true);
 return Signaling::SIGNALING_NONE;
}

void
D868UVCodeplug::channel_t::setTXTone(Code code) {
  if (Signaling::SIGNALING_NONE == code) {
    tx_ctcss = tx_dcs = 0;
    ctcss_transmit = dcs_transmit = 0;
  } else if (Signaling::isCTCSS(code)) {
    tx_ctcss = 1;
    tx_dcs = 0;
    ctcss_transmit = ctcss_code2num(code);
    dcs_transmit = 0;
  } else if (Signaling::isDCSNormal(code)) {
    tx_ctcss = 0;
    tx_dcs = 1;
    ctcss_transmit = 0;
    dcs_transmit = qToLittleEndian(oct_to_dec(Signaling::toDCSNumber(code)));
  } else if (Signaling::isDCSInverted(code)) {
    tx_ctcss = 0;
    tx_dcs = 1;
    ctcss_transmit = 0;
    dcs_transmit = qToLittleEndian(oct_to_dec(Signaling::toDCSNumber(code))+512);
  }
}

Channel *
D868UVCodeplug::channel_t::toChannelObj() const {
  // Decode power setting
  Channel::Power power = Channel::LowPower;
  switch ((channel_t::Power) this->power) {
  case POWER_LOW:
    power = Channel::LowPower;
    break;
  case POWER_MIDDLE:
    power = Channel::MidPower;
    break;
  case POWER_HIGH:
    power = Channel::HighPower;
    break;
  case POWER_TURBO:
    power = Channel::MaxPower;
    break;
  }
  bool rxOnly = (1 == this->rx_only);

  Channel *ch;
  if ((MODE_ANALOG == channel_mode) || (MODE_MIXED_A_D == channel_mode)) {
    if (MODE_MIXED_A_D == channel_mode)
      logWarn() << "Mixed mode channels are not supported (for now). Treat ch '"
                << getName() <<"' as analog channel.";
    AnalogChannel::Admit admit = AnalogChannel::AdmitNone;
    switch ((channel_t::Admit) tx_permit) {
    case ADMIT_ALWAYS:
      admit = AnalogChannel::AdmitNone;
      break;
    case ADMIT_CH_FREE:
      admit = AnalogChannel::AdmitFree;
      break;
    default:
      break;
    }
    AnalogChannel::Bandwidth bw = AnalogChannel::BWNarrow;
    if (BW_12_5_KHZ == bandwidth)
      bw = AnalogChannel::BWNarrow;
    else
      bw = AnalogChannel::BWWide;
    ch = new AnalogChannel(
          getName(), getRXFrequency(), getTXFrequency(), power, 0.0, rxOnly, admit,
          1, getRXTone(), getTXTone(), bw, nullptr);
  } else if ((MODE_DIGITAL == channel_mode) || (MODE_MIXED_D_A == channel_mode)) {
    if (MODE_MIXED_A_D == channel_mode)
      logWarn() << "Mixed mode channels are not supported (for now). Treat ch '"
                << getName() <<"' as digital channel.";
    DigitalChannel::Admit admit = DigitalChannel::AdmitNone;
    switch ((channel_t::Admit) tx_permit) {
    case ADMIT_ALWAYS:
      admit = DigitalChannel::AdmitNone;
      break;
    case ADMIT_CH_FREE:
      admit = DigitalChannel::AdmitFree;
      break;
    case ADMIT_COLORCODE:
      admit = DigitalChannel::AdmitColorCode;
      break;
    }
    DigitalChannel::TimeSlot ts = (slot2 ? DigitalChannel::TimeSlot2 : DigitalChannel::TimeSlot1);
    ch = new DigitalChannel(
          getName(), getRXFrequency(), getTXFrequency(), power, 0.0, rxOnly, admit,
          color_code, ts, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
  } else {
    logError() << "Cannot create channel '" << getName()
               << "': Channel type " << channel_mode << "not supported.";
    return nullptr;
  }

  return ch;
}

bool
D868UVCodeplug::channel_t::linkChannelObj(Channel *c, const CodeplugContext &ctx) const {
  if (MODE_DIGITAL == channel_mode) {
    // If channel is digital
    DigitalChannel *dc = c->as<DigitalChannel>();
    if (nullptr == dc)
      return false;

    // Check if default contact is set, in fact a valid contact index is mandatory.
    uint32_t conIdx = qFromLittleEndian(contact_index);
    if ((0xffffffff != conIdx) && ctx.hasDigitalContact(conIdx))
      dc->setTXContact(ctx.getDigitalContact(conIdx));

    // Set if RX group list is set
    if ((0xff != group_list_index) && ctx.hasGroupList(group_list_index))
      dc->setRXGroupList(ctx.getGroupList(group_list_index));

    // Link to GPS system
    if (aprs_report && (!ctx.hasGPSSystem(gps_system)))
      logWarn() << "Cannot link to GPS system index " << gps_system << ": undefined GPS system.";
    else if (ctx.hasGPSSystem(gps_system))
      dc->setPosSystem(ctx.getGPSSystem(gps_system));

    // Link radio ID
    RadioID *rid = ctx.getRadioId(id_index);
    if (rid == ctx.config()->radioIDs()->defaultId())
      dc->setRadioId(nullptr);
    else
      dc->setRadioId(rid);
  } else if (MODE_ANALOG == channel_mode) {
    // If channel is analog
    AnalogChannel *ac = c->as<AnalogChannel>();
    if (nullptr == ac)
      return false;
  }

  // For both, analog and digital channels:

  // If channel has scan list
  if ((0xff != scan_list_index) && ctx.hasScanList(scan_list_index))
    c->setScanList(ctx.getScanList(scan_list_index));

  return true;
}

void
D868UVCodeplug::channel_t::fromChannelObj(const Channel *c, const Config *conf) {
  clear();

  // set channel name
  setName(c->name());

  // set rx and tx frequencies
  setRXFrequency(c->rxFrequency());
  setTXFrequency(c->txFrequency());

  // encode power setting
  switch (c->power()) {
  case Channel::MaxPower:
    power = POWER_TURBO;
    break;
  case Channel::HighPower:
    power = POWER_HIGH;
    break;
  case Channel::MidPower:
    power = POWER_MIDDLE;
    break;
  case Channel::LowPower:
  case Channel::MinPower:
    power = POWER_LOW;
    break;
  }

  // set tx-enable
  rx_only = c->rxOnly() ? 1 : 0;

  // Link scan list if set
  if (nullptr == c->scanList())
    scan_list_index = 0xff;
  else
    scan_list_index = conf->scanlists()->indexOf(c->scanList());

  // Dispatch by channel type
  if (c->is<AnalogChannel>()) {
    const AnalogChannel *ac = c->as<const AnalogChannel>();
    channel_mode = MODE_ANALOG;
    // pack analog channel config
    // set admit criterion
    switch (ac->admit()) {
    case AnalogChannel::AdmitNone: tx_permit = ADMIT_ALWAYS; break;
    case AnalogChannel::AdmitFree: tx_permit = ADMIT_CH_FREE; break;
    case AnalogChannel::AdmitTone: tx_permit = ADMIT_ALWAYS; break;
    }
    // squelch mode
    setRXTone(ac->rxTone());
    setTXTone(ac->txTone());
    // set bandwidth
    bandwidth = (AnalogChannel::BWNarrow == ac->bandwidth()) ? BW_12_5_KHZ : BW_25_KHZ;
  } else if (c->is<DigitalChannel>()) {
    const DigitalChannel *dc = c->as<const DigitalChannel>();
    // pack digital channel config.
    channel_mode = MODE_DIGITAL;
    // set admit criterion
    switch(dc->admit()) {
    case DigitalChannel::AdmitNone: tx_permit = ADMIT_ALWAYS; break;
    case DigitalChannel::AdmitFree: tx_permit = ADMIT_CH_FREE; break;
    case DigitalChannel::AdmitColorCode: tx_permit = ADMIT_COLORCODE; break;
    }
    // set color code
    color_code = dc->colorCode();
    // set time-slot
    slot2 = (DigitalChannel::TimeSlot2 == dc->timeslot()) ? 1 : 0;
    // link transmit contact
    if (nullptr == dc->txContact()) {
      contact_index = 0;
    } else {
      contact_index = qToLittleEndian(
            uint32_t(conf->contacts()->indexOfDigital(dc->txContact())));
    }
    // link RX group list
    if (nullptr == dc->rxGroupList())
      group_list_index = 0xff;
    else
      group_list_index = conf->rxGroupLists()->indexOf(dc->rxGroupList());
    // Set GPS system index
    if (dc->posSystem() && dc->posSystem()->is<GPSSystem>()) {
      aprs_report = 1;
      gps_system = conf->posSystems()->indexOfGPSSys(dc->posSystem()->as<GPSSystem>());
    }
    // Set radio ID
    if (nullptr != dc->radioId())
      id_index = conf->radioIDs()->indexOf(dc->radioId());
    else
      id_index = 0;
  }
}


/* ******************************************************************************************** *
 * Implementation of D868UVCodeplug::contact_t
 * ******************************************************************************************** */
D868UVCodeplug::contact_t::contact_t() {
  clear();
}

void
D868UVCodeplug::contact_t::clear() {
  memset(this, 0, sizeof(D868UVCodeplug::contact_t));
}

bool
D868UVCodeplug::contact_t::isValid() const {
  return (0 != name[0]) && (0xff != name[0]);
}

QString
D868UVCodeplug::contact_t::getName() const {
  return decode_ascii(name, 16, 0x00);
}

void
D868UVCodeplug::contact_t::setName(const QString &name) {
  encode_ascii(this->name, name, 16, 0);
}

DigitalContact::Type
D868UVCodeplug::contact_t::getType() const {
  switch ((CallType) type) {
  case CALL_PRIVATE: return DigitalContact::PrivateCall;
  case CALL_GROUP: return DigitalContact::GroupCall;
  case CALL_ALL: return DigitalContact::AllCall;
  }
}

void
D868UVCodeplug::contact_t::setType(DigitalContact::Type type) {
  switch (type) {
  case DigitalContact::PrivateCall: this->type = CALL_PRIVATE; break;
  case DigitalContact::GroupCall: this->type = CALL_GROUP; break;
  case DigitalContact::AllCall:
    this->type = CALL_ALL;
    this->id = qToBigEndian(16777215);
    break;
  }
}

uint32_t
D868UVCodeplug::contact_t::getId() const {
  return decode_dmr_id_bcd((const uint8_t *)&id);
}

void
D868UVCodeplug::contact_t::setId(uint32_t id) {
  encode_dmr_id_bcd((uint8_t *)&(this->id), id);
}

bool
D868UVCodeplug::contact_t::getAlert() const {
  return (ALERT_NONE != call_alert);
}

void
D868UVCodeplug::contact_t::setAlert(bool enable) {
  call_alert = enable ? ALERT_RING : ALERT_NONE;
}

DigitalContact *
D868UVCodeplug::contact_t::toContactObj() const {
  return new DigitalContact(getType(), getName(), getId(), getAlert());
}

void
D868UVCodeplug::contact_t::fromContactObj(const DigitalContact *contact) {
  clear();
  setType(contact->type());
  setName(contact->name());
  setId(contact->number());
  setAlert(contact->rxTone());
}


/* ******************************************************************************************** *
 * Implementation of D868UVCodeplug::analog_contact_t
 * ******************************************************************************************** */
void
D868UVCodeplug::analog_contact_t::clear() {
  memset(number, 0, sizeof(number));
  digits = 0;
  memset(name, 0, sizeof(name));
  pad47 = 0;
}

QString
D868UVCodeplug::analog_contact_t::getNumber() const {
  return decode_dtmf_bcd_be(number, digits);
}

bool
D868UVCodeplug::analog_contact_t::setNumber(const QString &num) {
  if (! validDTMFNumber(num))
    return false;
  digits = num.length();
  return encode_dtmf_bcd_be(num, number, sizeof(number), 0);
}

QString
D868UVCodeplug::analog_contact_t::getName() const {
  return decode_ascii(name, sizeof(name), 0);
}

void
D868UVCodeplug::analog_contact_t::setName(const QString &name) {
  encode_ascii(this->name, name, sizeof(this->name), 0);
}

void
D868UVCodeplug::analog_contact_t::fromContact(const DTMFContact *contact) {
  setNumber(contact->number());
  setName(contact->name());
}

DTMFContact *
D868UVCodeplug::analog_contact_t::toContact() const {
  return new DTMFContact(getName(), getNumber());
}


/* ******************************************************************************************** *
 * Implementation of D868UVCodeplug::grouplist_t
 * ******************************************************************************************** */
D868UVCodeplug::grouplist_t::grouplist_t() {
  clear();
}

void
D868UVCodeplug::grouplist_t::clear() {
  memset(member, 0xff, sizeof(member));
  memset(name, 0x00, sizeof(name));
  memset(unused, 0x00, sizeof(unused));
}

bool
D868UVCodeplug::grouplist_t::isValid() const {
  return (0 != name[0]) && (0xff != name[0]);
}

QString
D868UVCodeplug::grouplist_t::getName() const {
  return decode_ascii(name, 16, 0x00);
}

void
D868UVCodeplug::grouplist_t::setName(const QString &name) {
  encode_ascii(this->name, name, 16, 0x00);
}

RXGroupList *
D868UVCodeplug::grouplist_t::toGroupListObj() const {
  return new RXGroupList(getName());
}

bool
D868UVCodeplug::grouplist_t::linkGroupList(RXGroupList *lst, const CodeplugContext &ctx) {
  for (uint8_t i=0; i<64; i++) {
    uint32_t idx = qFromLittleEndian(member[i]);
    // Disabled contact -> continue
    if (0xffffffff == idx)
      continue;
    // Missing contact ignore.
    if (! ctx.hasDigitalContact(idx)) {
      logWarn() << "Cannot link contact " << member[i] << " to group list '"
                << this->getName() << "': Invalid contact index. Ignored.";
      continue;
    }

    lst->addContact(ctx.getDigitalContact(idx));
  }
  return true;
}

void
D868UVCodeplug::grouplist_t::fromGroupListObj(const RXGroupList *lst, const Config *conf) {
  clear();
  // set name of group list
  setName(lst->name());
  // set members
  for (uint8_t i=0; i<64; i++) {
    if (i < lst->count())
      member[i] = qToLittleEndian(conf->contacts()->indexOfDigital(lst->contact(i)));
    else
      member[i] = 0xffffffff;
  }
}


/* ******************************************************************************************** *
 * Implementation of D868UVCodeplug::scanlist_t
 * ******************************************************************************************** */
D868UVCodeplug::scanlist_t::scanlist_t() {
  clear();
}

void
D868UVCodeplug::scanlist_t::clear() {
  _unused0000 = 0;
  prio_ch_select = PRIO_CHAN_OFF;
  priority_ch1 = 0xffff;
  priority_ch2 = 0xffff;
  look_back_a = qToLittleEndian(0x000f);
  look_back_b = qToLittleEndian(0x0019);
  dropout_delay = qToLittleEndian(0x001d);
  dwell = qToLittleEndian(0x001d);
  revert_channel = REVCH_SELECTED;
  memset(name, 0, sizeof(name));
  _pad001e = 0;
  memset(member, 0xff, sizeof(member));
  memset(_unused0084, 0, sizeof(_unused0084));
}

QString
D868UVCodeplug::scanlist_t::getName() const {
  return decode_ascii(name, sizeof(name), 0);
}

void
D868UVCodeplug::scanlist_t::setName(const QString &name) {
  encode_ascii(this->name, name, 16, 0);
}

ScanList *
D868UVCodeplug::scanlist_t::toScanListObj() {
  return new ScanList(getName());
}

void
D868UVCodeplug::scanlist_t::linkScanListObj(ScanList *lst, CodeplugContext &ctx) {
  if (prio_ch_select & PRIO_CHAN_SEL1) {
    uint16_t idx = qFromLittleEndian(priority_ch1);
    if (! ctx.hasChannel(idx)) {
      logError() << "Cannot link scanlist '" << getName()
                 << "', priority channel 1 index " << idx << " unknown.";
      // Ignore error, continue decoding
    } else {
      lst->setPriorityChannel(ctx.getChannel(idx));
    }
  }

  if (prio_ch_select & PRIO_CHAN_SEL2) {
    uint16_t idx = qFromLittleEndian(priority_ch2);
    if (! ctx.hasChannel(idx)) {
      logError() << "Cannot link scanlist '" << getName()
                 << "', priority channel 2 index " << idx << " unknown.";
      // Ignore error, continue decoding
    } else {
      lst->setSecPriorityChannel(ctx.getChannel(idx));
    }
  }

  for (uint16_t i=0; i<50; i++) {
    if (0xffff == member[i])
      continue;
    uint16_t idx = qFromLittleEndian(member[i]);
    if (! ctx.hasChannel(idx)) {
      logError() << "Cannot link scanlist '" << getName() << "', channel index " << idx << " unknown.";
      continue;
    }
    lst->addChannel(ctx.getChannel(idx));
  }
}

bool
D868UVCodeplug::scanlist_t::fromScanListObj(ScanList *lst, Config *config) {
  clear();

  setName(lst->name());
  prio_ch_select = PRIO_CHAN_OFF;

  if (lst->priorityChannel()) {
    prio_ch_select |= PRIO_CHAN_SEL1;
    if (SelectedChannel::get() == lst->priorityChannel())
      priority_ch1 = 0x0000;
    else
      priority_ch1 = qToLittleEndian(
            config->channelList()->indexOf(lst->priorityChannel())+1);
  }

  if (lst->secPriorityChannel()) {
    prio_ch_select |= PRIO_CHAN_SEL2;
    if (SelectedChannel::get() == lst->secPriorityChannel())
      priority_ch2 = 0x0000;
    else
      priority_ch2 = qToLittleEndian(
            config->channelList()->indexOf(lst->secPriorityChannel())+1);
  }

  for (int i=0; i<std::min(50, lst->count()); i++) {
    if (SelectedChannel::get() == lst->channel(i))
      continue;
    member[i] = qToLittleEndian(
          config->channelList()->indexOf(lst->channel(i)));
  }

  return false;
}


/* ******************************************************************************************** *
 * Implementation of D868UVCodeplug::radioid_t
 * ******************************************************************************************** */
D868UVCodeplug::radioid_t::radioid_t() {
  clear();
}

void
D868UVCodeplug::radioid_t::clear() {
  memset(this, 0, sizeof(D868UVCodeplug::radioid_t));
}

bool
D868UVCodeplug::radioid_t::isValid() const {
  return ((0x00 != name[0]) && (0xff != name[0]));
}

QString
D868UVCodeplug::radioid_t::getName() const {
  return decode_ascii(name, 16, 0);
}

void
D868UVCodeplug::radioid_t::setName(const QString name) {
  encode_ascii(this->name, name, 16, 0);
}

uint32_t
D868UVCodeplug::radioid_t::getId() const {
  uint32_t id_bcd = qFromLittleEndian(this->id);
  return decode_dmr_id_bcd((const uint8_t *) &id_bcd);
}

void
D868UVCodeplug::radioid_t::setId(uint32_t num) {
  uint32_t id_bcd;
  encode_dmr_id_bcd((uint8_t *)&id_bcd, num);
  this->id = qToLittleEndian(id_bcd);
}



/* ******************************************************************************************** *
 * Implementation of D868UVCodeplug::general_settings_base_t
 * ******************************************************************************************** */
void
D868UVCodeplug::general_settings_base_t::fromConfig(Config *config, const Flags &flags) {
  // Set microphone gain
  mic_gain = std::min(uint(4), config->micLevel()/10);

  // If auto-enable GPS is enabled
  if (flags.autoEnableGPS) {
    // Check if GPS is required -> enable
    if (config->requiresGPS()) {
      enable_gps = 0x01;
      // Set time zone based on system time zone.
      int offset = QTimeZone::systemTimeZone().offsetFromUtc(QDateTime::currentDateTime());
      gps_timezone = 12 + offset/3600;
      enable_get_gps_pos = 0x00;
      gps_update_period = 0x05;
      // Set measurement system based on system locale (0x00==Metric)
      gps_units = (QLocale::MetricSystem == QLocale::system().measurementSystem()) ? GPS_METRIC : GPS_IMPERIAL;
    } else {
      enable_gps = 0x00;
    }
  }
}

void
D868UVCodeplug::general_settings_base_t::updateConfig(Config *config) {
  // get microphone gain
  config->setMicLevel(2*mic_gain+1);
  // D868UV does not support speech synthesis?
  config->setSpeech(false);
}



/* ******************************************************************************************** *
 * Implementation of D868UVCodeplug::boot_settings_t
 * ******************************************************************************************** */
void
D868UVCodeplug::boot_settings_t::clear() {
  memset(intro_line1, 0, sizeof(intro_line2));
  memset(intro_line2, 0, sizeof(intro_line2));
}

QString
D868UVCodeplug::boot_settings_t::getIntroLine1() const {
  return decode_ascii(intro_line1, 14, 0);
}
void
D868UVCodeplug::boot_settings_t::setIntroLine1(const QString line) {
  encode_ascii(intro_line1, line, 14, 0);
}

QString
D868UVCodeplug::boot_settings_t::getIntroLine2() const {
  return decode_ascii(intro_line2, 14, 0);
}
void
D868UVCodeplug::boot_settings_t::setIntroLine2(const QString line) {
  encode_ascii(intro_line2, line, 14, 0);
}

void
D868UVCodeplug::boot_settings_t::fromConfig(const Config *config, const Flags &flags) {
  Q_UNUSED(flags)
  setIntroLine1(config->introLine1());
  setIntroLine2(config->introLine2());
}

void
D868UVCodeplug::boot_settings_t::updateConfig(Config *config) {
  config->setIntroLine1(getIntroLine1());
  config->setIntroLine2(getIntroLine2());
}



/* ******************************************************************************************** *
 * Implementation of D868UVCodeplug::gps_settings_t
 * ******************************************************************************************** */
void
D868UVCodeplug::gps_settings_t::clear() {
  manual_tx_intervall = 0;
  auto_tx_intervall = 0;
  enable_fixed_location = 0;
  transmit_power = POWER_LOW;
  for (uint8_t i=0; i<8; i++)
    channel_idxs[i] = qToLittleEndian(0x0fa1);
  target_id = 1;
  call_type = PRIVATE_CALL;
  timeslot = TIMESLOT_SAME;
}

uint8_t
D868UVCodeplug::gps_settings_t::getManualTXIntervall() const {
  return manual_tx_intervall;
}
void
D868UVCodeplug::gps_settings_t::setManualTXIntervall(uint8_t period) {
  manual_tx_intervall = period;
}

uint16_t
D868UVCodeplug::gps_settings_t::getAutomaticTXIntervall() const {
  if (0 == auto_tx_intervall)
    return 0;
  return 45 + 15*auto_tx_intervall;
}
void
D868UVCodeplug::gps_settings_t::setAutomaticTXIntervall(uint16_t period) {
  if (0 == period)
    auto_tx_intervall = 0;
  else if (60 > period)
    auto_tx_intervall = 1;
  else
    auto_tx_intervall = (period-45)/15;
}

Channel::Power
D868UVCodeplug::gps_settings_t::getTransmitPower() const {
  switch (transmit_power) {
  case POWER_LOW: return Channel::LowPower;
  case POWER_MID: return Channel::MidPower;
  case POWER_HIGH: return Channel::HighPower;
  case POWER_TURBO: return Channel::MaxPower;
  }
}
void
D868UVCodeplug::gps_settings_t::setTransmitPower(Channel::Power power) {
  switch(power) {
  case Channel::MinPower:
  case Channel::LowPower:
    transmit_power = POWER_LOW;
    break;
  case Channel::MidPower:
    transmit_power = POWER_MID;
    break;
  case Channel::HighPower:
    transmit_power = POWER_HIGH;
    break;
  case Channel::MaxPower:
    transmit_power = POWER_TURBO;
    break;
  }
}

bool
D868UVCodeplug::gps_settings_t::isFixedLocation() const {
  return enable_fixed_location;
}

double
D868UVCodeplug::gps_settings_t::getFixedLat() const {
  return ((1 == north_south) ? -1 : 1) * (lat_deg + double(lat_min)/60 + double(lat_sec)/3600);
}

double
D868UVCodeplug::gps_settings_t::getFixedLon() const {
  return ((1 == east_west) ? -1 : 1) * (lon_deg + double(lon_min)/60 + double(lon_sec)/3600);
}

void
D868UVCodeplug::gps_settings_t::setFixedLocation(double lat, double lon) {
  enable_fixed_location = 1;
  north_south = (lat < 0) ? 1 : 0; lat = std::abs(lat);
  lat_deg = uint(lat); lat -= uint(lat); lat *= 60;
  lat_min = uint(lat); lat -= uint(lat); lat *= 60;
  lat_sec = uint(lat);
  east_west = (lon < 0) ? 1 : 0; lon = std::abs(lon);
  lon_deg = uint(lon); lon -= uint(lon); lon *= 60;
  lon_min = uint(lon); lon -= uint(lon); lon *= 60;
  lon_sec = uint(lon);
}

bool
D868UVCodeplug::gps_settings_t::isChannelSelected(uint8_t i) const {
  return 0x0fa2 == qFromLittleEndian(channel_idxs[i]);
}

bool
D868UVCodeplug::gps_settings_t::isChannelVFOA(uint8_t i) const {
  return 0x0fa0 == qFromLittleEndian(channel_idxs[i]);
}

bool
D868UVCodeplug::gps_settings_t::isChannelVFOB(uint8_t i) const {
  return 0x0fa1 == qFromLittleEndian(channel_idxs[i]);
}

uint16_t
D868UVCodeplug::gps_settings_t::getChannelIndex(uint8_t i) const {
  return qFromLittleEndian(channel_idxs[i]);
}

void
D868UVCodeplug::gps_settings_t::setChannelIndex(uint8_t i, uint16_t idx) {
  channel_idxs[i] = qToLittleEndian(idx);
}

void
D868UVCodeplug::gps_settings_t::setChannelSelected(uint8_t i) {
  channel_idxs[i] = qToLittleEndian(0x0fa2);
}

void
D868UVCodeplug::gps_settings_t::setChannelVFOA(uint8_t i) {
  channel_idxs[i] = qToLittleEndian(0x0fa0);
}

void
D868UVCodeplug::gps_settings_t::setChannelVFOB(uint8_t i) {
  channel_idxs[i] = qToLittleEndian(0x0fa1);
}

uint32_t
D868UVCodeplug::gps_settings_t::getTargetID() const {
  return decode_dmr_id_bcd((uint8_t *)&target_id);
}
void
D868UVCodeplug::gps_settings_t::setTargetID(uint32_t id) {
  encode_dmr_id_bcd((uint8_t *)&target_id, id);
}

DigitalContact::Type
D868UVCodeplug::gps_settings_t::getTargetType() const {
  switch (call_type) {
  case PRIVATE_CALL: return DigitalContact::PrivateCall;
  case GROUP_CALL: return DigitalContact::GroupCall;
  case ALL_CALL: return DigitalContact::AllCall;
  }
}
void
D868UVCodeplug::gps_settings_t::setTargetType(DigitalContact::Type type) {
  switch(type) {
  case DigitalContact::PrivateCall: call_type = PRIVATE_CALL; break;
  case DigitalContact::GroupCall: call_type = GROUP_CALL; break;
  case DigitalContact::AllCall: call_type = ALL_CALL; break;
  }
}

void
D868UVCodeplug::gps_settings_t::fromConfig(Config *config, const Flags &flags) {
  if (1 < config->posSystems()->gpsCount()) {
    logDebug() << "D868UV only supports a single independent GPS positioning system.";
  } else if (0 == config->posSystems()->gpsCount()) {
    return;
  }

  GPSSystem *sys = config->posSystems()->gpsSystem(0);
  setTargetID(sys->contact()->number());
  setTargetType(sys->contact()->type());
  setManualTXIntervall(sys->period());
  setAutomaticTXIntervall(sys->period());
  if (SelectedChannel::get() == sys->revertChannel()->as<Channel>()) {
    setChannelSelected(0);
    timeslot = TIMESLOT_SAME;
  } else {
    setChannelIndex(0, config->channelList()->indexOf(sys->revertChannel()));
    timeslot = TIMESLOT_SAME;
  }
}

bool
D868UVCodeplug::gps_settings_t::createGPSSystem(uint8_t i, Config *config, CodeplugContext &ctx) {
  ctx.addGPSSystem(new GPSSystem(QString("GPS sys %1").arg(i+1), nullptr, nullptr, getAutomaticTXIntervall()), i);
  return true;
}

bool
D868UVCodeplug::gps_settings_t::linkGPSSystem(uint8_t i, Config *config, CodeplugContext &ctx) {
  DigitalContact *cont = nullptr;
  // Find matching contact, if not found -> create one.
  if (nullptr == (cont = config->contacts()->findDigitalContact(getTargetID()))) {
    cont = new DigitalContact(getTargetType(), QString("GPS target"), getTargetID());
    config->contacts()->add(cont);
  }
  ctx.getGPSSystem(i)->setContact(cont);

  // Check if there is a revert channel set
  if ((! isChannelSelected(i)) && (ctx.hasChannel(getChannelIndex(i))) && (ctx.getChannel(getChannelIndex(i)))->is<DigitalChannel>()) {
    DigitalChannel *ch = ctx.getChannel(getChannelIndex(i))->as<DigitalChannel>();
    ctx.getGPSSystem(i)->setRevertChannel(ch);
  }
  return true;
}


/* ******************************************************************************************** *
 * Implementation of D868UVCodeplug::contact_map_t
 * ******************************************************************************************** */
D868UVCodeplug::contact_map_t::contact_map_t() {
  clear();
}

void
D868UVCodeplug::contact_map_t::clear() {
  memset(this, 0xff, sizeof(contact_map_t));
}

bool
D868UVCodeplug::contact_map_t::isValid() const {
  return (0xffffffff != id_group) && (0xffffffff != contact_index);
}

bool
D868UVCodeplug::contact_map_t::isGroup() const {
  uint32_t tmp = qFromLittleEndian(id_group);
  return tmp & 0x01;
}
uint32_t
D868UVCodeplug::contact_map_t::ID() const {
  uint32_t tmp = qFromLittleEndian(id_group);
  tmp = tmp>>1;
  return decode_dmr_id_bcd_le((uint8_t *)&tmp);
}
void
D868UVCodeplug::contact_map_t::setID(uint32_t id, bool group) {
  uint32_t tmp; encode_dmr_id_bcd_le((uint8_t *)&tmp, id);
  tmp = ( (tmp << 1) | (group ? 1 : 0) );
  id_group = qToLittleEndian(tmp);
}


uint32_t
D868UVCodeplug::contact_map_t::index() const {
  return qFromLittleEndian(contact_index);
}
void
D868UVCodeplug::contact_map_t::setIndex(uint32_t index) {
  contact_index = qToLittleEndian(index);
}


/* ******************************************************************************************** *
 * Implementation of D868UVCodeplug
 * ******************************************************************************************** */
D868UVCodeplug::D868UVCodeplug(QObject *parent)
  : AnytoneCodeplug(parent)
{
  addImage("Anytone AT-D868UV Codeplug");

  // Channel bitmap
  image(0).addElement(CHANNEL_BITMAP, CHANNEL_BITMAP_SIZE);
  // Zone bitmap
  image(0).addElement(ZONE_BITMAPS, ZONE_BITMAPS_SIZE);
  // Contacts bitmap
  image(0).addElement(CONTACTS_BITMAP, CONTACTS_BITMAP_SIZE);
  // Analog contacts bytemap
  image(0).addElement(ANALOGCONTACT_BYTEMAP, ANALOGCONTACT_BYTEMAP_SIZE);
  // RX group list bitmaps
  image(0).addElement(RXGRP_BITMAP, RXGRP_BITMAP_SIZE);
  // Scan list bitmaps
  image(0).addElement(SCAN_BITMAP, SCAN_BITMAP_SIZE);
  // Radio IDs bitmaps
  image(0).addElement(RADIOID_BITMAP, RADIOID_BITMAP_SIZE);
  // Messag bitmaps
  image(0).addElement(MESSAGE_BYTEMAP, MESSAGE_BYTEMAP_SIZE);
  // Status messages
  image(0).addElement(STATUSMESSAGE_BITMAP, STATUSMESSAGE_BITMAP_SIZE);
  // FM Broadcast bitmaps
  image(0).addElement(FMBC_BITMAP, FMBC_BITMAP_SIZE);
  // 5-Tone function bitmaps
  image(0).addElement(FIVE_TONE_ID_BITMAP, FIVE_TONE_ID_BITMAP_SIZE);
  // 2-Tone function bitmaps
  image(0).addElement(TWO_TONE_IDS_BITMAP, TWO_TONE_IDS_BITMAP_SIZE);
  image(0).addElement(TWO_TONE_FUNCTIONS_BITMAP, TWO_TONE_FUNC_BITMAP_SIZE);
}

void
D868UVCodeplug::clear() {
  // NOOP
}

void
D868UVCodeplug::allocateUpdated() {
  this->allocateVFOSettings();

  // General config
  this->allocateGeneralSettings();
  this->allocateZoneChannelList();
  this->allocateDTMFNumbers();
  this->allocateBootSettings();

  this->allocateGPSSystems();

  this->allocateSMSMessages();
  this->allocateHotKeySettings();
  this->allocateRepeaterOffsetSettings();
  this->allocateAlarmSettings();
  this->allocateFMBroadcastSettings();

  this->allocate5ToneIDs();
  this->allocate5ToneFunctions();
  this->allocate5ToneSettings();

  this->allocate2ToneIDs();
  this->allocate2ToneFunctions();
  this->allocate2ToneSettings();

  this->allocateDTMFSettings();

  image(0).addElement(ADDR_DMR_ENCRYPTION_LIST, DMR_ENCRYPTION_LIST_SIZE);
  image(0).addElement(ADDR_DMR_ENCRYPTION_KEYS, DMR_ENCRYPTION_KEYS_SIZE);
}

void
D868UVCodeplug::allocateForEncoding() {
  this->allocateChannels();
  this->allocateZones();
  this->allocateContacts();
  this->allocateAnalogContacts();
  this->allocateRXGroupLists();
  this->allocateScanLists();
  this->allocateRadioIDs();
}

void
D868UVCodeplug::allocateForDecoding() {
  this->allocateRadioIDs();
  this->allocateChannels();
  this->allocateZones();
  this->allocateContacts();
  this->allocateAnalogContacts();
  this->allocateRXGroupLists();
  this->allocateScanLists();

  // General config
  this->allocateGeneralSettings();
  this->allocateZoneChannelList();
  this->allocateBootSettings();

  // GPS settings
  this->allocateGPSSystems();
}


void
D868UVCodeplug::setBitmaps(Config *config)
{
  // Mark first radio ID as valid
  uint8_t *radioid_bitmap = data(RADIOID_BITMAP);
  memset(radioid_bitmap, 0, RADIOID_BITMAP_SIZE);
  for (int i=0; i<std::min(NUM_RADIOIDS, config->radioIDs()->count()); i++)
    radioid_bitmap[i/8] |= (1 << (i%8));

  // Mark valid channels (set bit)
  uint8_t *channel_bitmap = data(CHANNEL_BITMAP);
  memset(channel_bitmap, 0, CHANNEL_BITMAP_SIZE);
  for (int i=0; i<std::min(NUM_CHANNELS, config->channelList()->count()); i++) {
    channel_bitmap[i/8] |= (1 << (i%8));
  }

  // Mark valid contacts (clear bit)
  uint8_t *contact_bitmap = data(CONTACTS_BITMAP);
  memset(contact_bitmap, 0x00, CONTACTS_BITMAP_SIZE);
  memset(contact_bitmap, 0xff, NUM_CONTACTS/8+1);
  for (int i=0; i<std::min(NUM_CONTACTS, config->contacts()->digitalCount()); i++) {
    contact_bitmap[i/8] &= ~(1 << (i%8));
  }

  // Mark valid analog contacts (clear bytes)
  uint8_t *analog_contact_bitmap = data(ANALOGCONTACT_BYTEMAP);
  memset(analog_contact_bitmap, 0xff, ANALOGCONTACT_BYTEMAP_SIZE);
  for (int i=0; i<std::min(NUM_ANALOGCONTACTS, config->contacts()->dtmfCount()); i++) {
    analog_contact_bitmap[i] = 0x00;
  }

  // Mark valid zones (set bits)
  uint8_t *zone_bitmap = data(ZONE_BITMAPS);
  memset(zone_bitmap, 0x00, ZONE_BITMAPS_SIZE);
  for (int i=0,z=0; i<std::min(NUM_ZONES, config->zones()->count()); i++) {
    zone_bitmap[z/8] |= (1 << (z%8)); z++;
    if (config->zones()->zone(i)->B()->count()) {
      zone_bitmap[z/8] |= (1 << (z%8)); z++;
    }
  }

  // Mark group lists
  uint8_t *group_bitmap = data(RXGRP_BITMAP);
  memset(group_bitmap, 0x00, RXGRP_BITMAP_SIZE);
  for (int i=0; i<std::min(NUM_RXGRP, config->rxGroupLists()->count()); i++)
    group_bitmap[i/8] |= (1 << (i%8));

  // Mark scan lists
  uint8_t *scan_bitmap = data(SCAN_BITMAP);
  memset(scan_bitmap, 0x00, SCAN_BITMAP_SIZE);
  for (int i=0; i<std::min(NUM_SCAN_LISTS, config->scanlists()->count()); i++) {
    scan_bitmap[i/8] |= (1<<(i%8));
  }
}


bool
D868UVCodeplug::encode(Config *config, const Flags &flags)
{
  if (! this->encodeRadioID(config, flags))
    return false;

  // Encode general config
  if (! this->encodeGeneralSettings(config, flags))
    return false;
  if (! this->encodeBootSettings(config, flags))
    return false;

  if (! this->encodeChannels(config, flags))
    return false;

  if (! this->encodeContacts(config, flags))
    return false;
  if (! this->encodeAnalogContacts(config, flags))
    return false;

  if (! this->encodeRXGroupLists(config, flags))
    return false;

  if (! this->encodeZones(config, flags))
    return false;

  if (! this->encodeScanLists(config, flags))
    return false;

  if (! this->encodeGPSSystems(config, flags))
    return false;

  return true;
}

bool D868UVCodeplug::decode(Config *config) {
  // Maps code-plug indices to objects
  CodeplugContext ctx(config);
  return decode(config, ctx);
}

bool
D868UVCodeplug::decode(Config *config, CodeplugContext &ctx)
{
  if (! this->setRadioID(config, ctx))
    return false;

  if (! this->decodeGeneralSettings(config))
    return false;
  if (! this->decodeBootSettings(config))
    return false;

  if (! this->createChannels(config, ctx))
    return false;

  if (! this->createContacts(config, ctx))
    return false;
  if (! this->createAnalogContacts(config, ctx))
    return false;

  if (! this->createRXGroupLists(config, ctx))
    return false;
  if (! this->linkRXGroupLists(config, ctx))
    return false;

  if (! this->createZones(config, ctx))
    return false;
  if (! this->linkZones(config, ctx))
    return false;

  if (! this->createScanLists(config, ctx))
    return false;
  if (! this->linkScanLists(config, ctx))
    return false;

  if (! this->createGPSSystems(config, ctx))
    return false;

  if (! this->linkChannels(config, ctx))
    return false;

  if (! this->linkGPSSystems(config, ctx))
    return false;

  return true;
}


void
D868UVCodeplug::allocateChannels() {
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
        + idx*sizeof(channel_t);
    if (nullptr == data(addr, 0)) {
      image(0).addElement(addr, sizeof(channel_t));
    }
  }
}

bool
D868UVCodeplug::encodeChannels(Config *config, const Flags &flags) {
  // Encode channels
  for (int i=0; i<config->channelList()->count(); i++) {
    // enable channel
    uint16_t bank = i/128, idx = i%128;
    channel_t *ch = (channel_t *)data(CHANNEL_BANK_0
                                      + bank*CHANNEL_BANK_OFFSET
                                      + idx*sizeof(channel_t));
    ch->fromChannelObj(config->channelList()->channel(i), config);
  }
  return true;
}

bool
D868UVCodeplug::createChannels(Config *config, CodeplugContext &ctx) {
  // Create channels
  uint8_t *channel_bitmap = data(CHANNEL_BITMAP);
  for (uint16_t i=0; i<NUM_CHANNELS; i++) {
    // Check if channel is enabled:
    uint16_t  bit = i%8, byte = i/8, bank = i/128, idx = i%128;
    if (0 == ((channel_bitmap[byte]>>bit) & 0x01))
      continue;
    channel_t *ch = (channel_t *)data(CHANNEL_BANK_0
                                      +bank*CHANNEL_BANK_OFFSET
                                      +idx*sizeof(channel_t));
    if (Channel *obj = ch->toChannelObj())
      ctx.addChannel(obj, i);
  }
  return true;
}

bool
D868UVCodeplug::linkChannels(Config *config, CodeplugContext &ctx) {
  // Link channel objects
  for (uint16_t i=0; i<NUM_CHANNELS; i++) {
    // Check if channel is enabled:
    uint16_t  bit = i%8, byte = i/8, bank = i/128, idx = i%128;
    if (0 == (((*data(CHANNEL_BITMAP+byte))>>bit) & 0x01))
      continue;
    channel_t *ch = (channel_t *)data(CHANNEL_BANK_0
                                      +bank*CHANNEL_BANK_OFFSET
                                      +idx*sizeof(channel_t));

    if (ctx.hasChannel(i))
      ch->linkChannelObj(ctx.getChannel(i), ctx);
  }
  return true;
}


void
D868UVCodeplug::allocateVFOSettings() {
  // Allocate VFO channels
  image(0).addElement(VFO_A_ADDR, sizeof(channel_t));
  image(0).addElement(VFO_A_ADDR+0x2000, sizeof(channel_t));
  image(0).addElement(VFO_B_ADDR, sizeof(channel_t));
  image(0).addElement(VFO_B_ADDR+0x2000, sizeof(channel_t));
}

void
D868UVCodeplug::allocateContacts() {
  /* Allocate contacts */
  uint8_t *contact_bitmap = data(CONTACTS_BITMAP);
  uint contactCount=0;
  for (uint16_t i=0; i<NUM_CONTACTS; i++) {
    // enabled if false (ass hole)
    if (1 == ((contact_bitmap[i/8]>>(i%8)) & 0x01))
      continue;
    contactCount++;
    uint32_t addr = CONTACT_BANK_0+(i/CONTACTS_PER_BANK)*CONTACT_BANK_SIZE;
    if (nullptr == data(addr, 0)) {
      image(0).addElement(addr, CONTACT_BANK_SIZE);
      memset(data(addr), 0x00, CONTACT_BANK_SIZE);
    }
  }
  if (contactCount) {
    image(0).addElement(CONTACT_INDEX_LIST, align_size(4*contactCount, 16));
    memset(data(CONTACT_INDEX_LIST), 0xff, align_size(4*contactCount, 16));
    image(0).addElement(CONTACT_ID_MAP, align_size(CONTACT_ID_ENTRY_SIZE*(1+contactCount), 16));
    memset(data(CONTACT_ID_MAP), 0xff, align_size(CONTACT_ID_ENTRY_SIZE*(1+contactCount), 16));
  }
}

bool
D868UVCodeplug::encodeContacts(Config *config, const Flags &flags) {
  // Encode contacts
  QVector<contact_map_t> contact_id_map;
  contact_id_map.reserve(config->contacts()->digitalCount());
  for (int i=0; i<config->contacts()->digitalCount(); i++) {
    contact_t *con = (contact_t *)data(CONTACT_BANK_0+i*sizeof(contact_t));
    con->fromContactObj(config->contacts()->digitalContact(i));
    ((uint32_t *)data(CONTACT_INDEX_LIST))[i] = qToLittleEndian(i);
    contact_map_t entry;
    entry.setID(config->contacts()->digitalContact(i)->number(),
                DigitalContact::GroupCall == config->contacts()->digitalContact(i)->type());
    entry.setIndex(i);
    contact_id_map.append(entry);
  }
  // encode index map for contacts
  std::sort(contact_id_map.begin(), contact_id_map.end(),
            [](const contact_map_t &a, const contact_map_t &b) {
    return a.ID() < b.ID();
  });
  for (int i=0; i<contact_id_map.size(); i++) {
    ((contact_map_t *)data(CONTACT_ID_MAP))[i] = contact_id_map[i];
  }
  return true;
}

bool
D868UVCodeplug::createContacts(Config *config, CodeplugContext &ctx) {
  // Create digital contacts
  uint8_t *contact_bitmap = data(CONTACTS_BITMAP);
  for (uint16_t i=0; i<NUM_CONTACTS; i++) {
    // Check if contact is enabled:
    uint16_t  bit = i%8, byte = i/8;
    if (1 == ((contact_bitmap[byte]>>bit) & 0x01))
      continue;
    contact_t *con = (contact_t *)data(CONTACT_BANK_0+i*sizeof(contact_t));
    if (DigitalContact *obj = con->toContactObj())
      ctx.addDigitalContact(obj, i);
  }
  return true;
}


void
D868UVCodeplug::allocateAnalogContacts() {
  /* Allocate analog contacts */
  uint8_t *analog_contact_bytemap = data(ANALOGCONTACT_BYTEMAP);
  for (uint8_t i=0; i<NUM_ANALOGCONTACTS; i+=2) {
    // if disabled -> skip
    if (0xff == analog_contact_bytemap[i])
      continue;
    uint32_t addr = ANALOGCONTACT_BANK_0 + (i/ANALOGCONTACTS_PER_BANK)*ANALOGCONTACT_BANK_SIZE;
    if (nullptr == data(addr, 0)) {
      image(0).addElement(addr, ANALOGCONTACT_BANK_SIZE);
    }
  }
  image(0).addElement(ANALOGCONTACT_INDEX_LIST, ANALOGCONTACT_LIST_SIZE);
}

bool
D868UVCodeplug::encodeAnalogContacts(Config *config, const Flags &flags) {
  uint8_t *idxlst = data(ANALOGCONTACT_INDEX_LIST);
  memset(idxlst, 0xff, ANALOGCONTACT_LIST_SIZE);
  for (int i=0; i<config->contacts()->dtmfCount(); i++) {
    uint32_t addr = ANALOGCONTACT_BANK_0 + (i/ANALOGCONTACTS_PER_BANK)*ANALOGCONTACT_BANK_SIZE
        + (i%ANALOGCONTACTS_PER_BANK)*ANALOGCONTACT_SIZE;
    analog_contact_t *cont = (analog_contact_t *)data(addr);
    cont->fromContact(config->contacts()->dtmfContact(i));
    idxlst[i] = i;
  }
  return true;
}

bool
D868UVCodeplug::createAnalogContacts(Config *config, CodeplugContext &ctx) {
  uint8_t *analog_contact_bytemap = data(ANALOGCONTACT_BYTEMAP);
  for (uint8_t i=0; i<NUM_ANALOGCONTACTS; i++) {
    // if disabled -> skip
    if (0xff == analog_contact_bytemap[i])
      continue;
    uint32_t addr = ANALOGCONTACT_BANK_0 + (i/ANALOGCONTACTS_PER_BANK)*ANALOGCONTACT_BANK_SIZE
        + (i%ANALOGCONTACTS_PER_BANK)*ANALOGCONTACT_SIZE;
    analog_contact_t *cont = (analog_contact_t *)data(addr);
    ctx.addAnalogContact(cont->toContact(), i);
  }
  return true;
}


void
D868UVCodeplug::allocateRadioIDs() {
  /* Allocate radio IDs */
  uint8_t *radioid_bitmap = data(RADIOID_BITMAP);
  for (uint8_t i=0; i<NUM_RADIOIDS; i++) {
    // Get byte and bit for radio ID
    uint16_t bit = i%8, byte = i/8;
    // if disabled -> skip
    if (0 == ((radioid_bitmap[byte]>>bit) & 0x01))
      continue;
    // Allocate radio IDs individually
    uint32_t addr = ADDR_RADIOIDS + i*RADIOID_SIZE;
    if (nullptr == data(addr, 0)) {
      image(0).addElement(addr, RADIOID_SIZE);
    }
  }
}

bool
D868UVCodeplug::encodeRadioID(Config *config, const Flags &flags) {
  // Encode radio IDs
  for (int i=0; i<config->radioIDs()->count(); i++) {
    radioid_t *radio_id = (radioid_t *)data(ADDR_RADIOIDS + i*RADIOID_SIZE);
    radio_id->setName(config->radioIDs()->getId(i)->name());
    radio_id->setId(config->radioIDs()->getId(i)->id());
  }
  return true;
}

bool
D868UVCodeplug::setRadioID(Config *config, CodeplugContext &ctx) {
  // Find a valid RadioID
  uint8_t *radio_id_bitmap = data(RADIOID_BITMAP);
  for (uint16_t i=0; i<NUM_RADIOIDS; i++) {
    if (0 == (radio_id_bitmap[i/8] & (1 << (i%8))))
      continue;
    radioid_t *id = (radioid_t *)data(ADDR_RADIOIDS + i*RADIOID_SIZE);
    logDebug() << "Store id " << id->getId() << " at idx " << i << ".";
    ctx.addRadioId(id->getId(), i, id->getName());
  }
  return true;
}


void
D868UVCodeplug::allocateRXGroupLists() {
  /*
   * Allocate group lists
   */
  uint8_t *grouplist_bitmap = data(RXGRP_BITMAP);
  for (uint16_t i=0; i<NUM_RXGRP; i++) {
    // Get byte and bit for group list
    uint16_t bit = i%8, byte = i/8;
    // if disabled -> skip
    if (0 == ((grouplist_bitmap[byte]>>bit) & 0x01))
      continue;
    // Allocate RX group lists indivitually
    uint32_t addr = ADDR_RXGRP_0 + i*RXGRP_OFFSET;
    if (nullptr == data(addr, 0)) {
      image(0).addElement(addr, RXGRP_SIZE);
      memset(data(addr), 0xff, RXGRP_SIZE);
    }
  }

}

bool
D868UVCodeplug::encodeRXGroupLists(Config *config, const Flags &flags) {
  // Encode RX group-lists
  for (int i=0; i<config->rxGroupLists()->count(); i++) {
    grouplist_t *grp = (grouplist_t *)data(ADDR_RXGRP_0 + i*RXGRP_OFFSET);
    grp->fromGroupListObj(config->rxGroupLists()->list(i), config);
  }
  return true;
}

bool
D868UVCodeplug::createRXGroupLists(Config *config, CodeplugContext &ctx) {
  // Create RX group lists
  uint8_t *grouplist_bitmap = data(RXGRP_BITMAP);
  for (uint16_t i=0; i<NUM_RXGRP; i++) {
    // check if group list is enabled
    uint16_t  bit = i%8, byte = i/8;
    if (0 == ((grouplist_bitmap[byte]>>bit) & 0x01))
      continue;
    // construct RXGroupList from definition
    grouplist_t *grp = (grouplist_t *)data(ADDR_RXGRP_0+i*RXGRP_OFFSET);
    if (RXGroupList *obj = grp->toGroupListObj()) {
      ctx.addGroupList(obj, i);
      // Link group list immediately, all contacts are defined allready.
      grp->linkGroupList(obj, ctx);
    }
  }
  return true;
}

bool
D868UVCodeplug::linkRXGroupLists(Config *config, CodeplugContext &ctx) {
  // pass, already linked during creation.
  return true;
}


void
D868UVCodeplug::allocateZones() {
  /*
   *  Allocate zones
   */
  uint8_t *zone_bitmap = data(ZONE_BITMAPS);
  for (uint16_t i=0; i<NUM_ZONES; i++) {
    // Get byte and bit for zone
    uint16_t bit = i%8, byte = i/8;
    // if invalid -> skip
    if (0 == ((zone_bitmap[byte]>>bit) & 0x01))
      continue;
    // Allocate zone itself
    image(0).addElement(ADDR_ZONE+i*ZONE_OFFSET, ZONE_SIZE);
    image(0).addElement(ADDR_ZONE_NAME+i*ZONE_NAME_OFFSET, ZONE_NAME_SIZE);
  }
}

bool
D868UVCodeplug::encodeZones(Config *config, const Flags &flags) {
  // Encode zones
  uint zidx = 0;
  for (int i=0; i<config->zones()->count(); i++) {
    // Clear name and channel list
    uint8_t  *name     = (uint8_t *)data(ADDR_ZONE_NAME + zidx*ZONE_NAME_OFFSET);
    uint16_t *channels = (uint16_t *)data(ADDR_ZONE + zidx*ZONE_OFFSET);
    memset(name, 0, ZONE_NAME_SIZE);
    memset(channels, 0xff, ZONE_SIZE);
    if (config->zones()->zone(i)->B()->count())
      encode_ascii(name, config->zones()->zone(i)->name()+" A", 16, 0);
    else
      encode_ascii(name, config->zones()->zone(i)->name(), 16, 0);
    // Handle list A
    for (int j=0; j<config->zones()->zone(i)->A()->count(); j++) {
      channels[j] = qToLittleEndian(
            config->channelList()->indexOf(
              config->zones()->zone(i)->A()->channel(j)));
    }
    zidx++;
    if (! config->zones()->zone(i)->B()->count())
      continue;

    // Process list B if present
    name     = (uint8_t *)data(ADDR_ZONE_NAME+zidx*ZONE_NAME_OFFSET);
    channels = (uint16_t *)data(ADDR_ZONE+zidx*ZONE_OFFSET);
    memset(name, 0, ZONE_NAME_SIZE);
    memset(channels, 0xff, ZONE_SIZE);
    encode_ascii(name, config->zones()->zone(i)->name()+" B", 16, 0);
    // Handle list B
    for (int j=0; j<config->zones()->zone(i)->B()->count(); j++) {
      channels[j] = qToLittleEndian(
            config->channelList()->indexOf(
              config->zones()->zone(i)->B()->channel(j)));
    }
    zidx++;
  }
  return true;
}

bool
D868UVCodeplug::createZones(Config *config, CodeplugContext &ctx) {
  // Create zones
  uint8_t *zone_bitmap = data(ZONE_BITMAPS);
  QString last_zonename, last_zonebasename; Zone *last_zone = nullptr;
  bool extend_last_zone = false;
  for (uint16_t i=0; i<NUM_ZONES; i++) {
    // Check if zone is enabled:
    uint16_t bit = i%8, byte = i/8;
    if (0 == ((zone_bitmap[byte]>>bit) & 0x01))
      continue;
    // Determine whether this zone should be combined with the previous one
    QString zonename = decode_ascii(data(ADDR_ZONE_NAME+i*ZONE_NAME_OFFSET), 16, 0);
    QString zonebasename = zonename; zonebasename.chop(2);
    extend_last_zone = ( zonename.endsWith(" B") && last_zonename.endsWith(" A")
                         && (zonebasename == last_zonebasename)
                         && (nullptr != last_zone) && (0 == last_zone->B()->count()) );
    last_zonename = zonename;
    last_zonebasename = zonebasename;

    // If enabled, create zone with name
    if (! extend_last_zone) {
      last_zone = new Zone(zonename);
      // add to config
      config->zones()->add(last_zone);
    } else {
      // when extending the last zone, chop its name to remove the "... A" part.
      last_zone->setName(last_zonebasename);
    }

    // link zone
    uint16_t *channels = (uint16_t *)data(ADDR_ZONE+i*ZONE_OFFSET);
    for (uint8_t i=0; i<NUM_CH_PER_ZONE; i++, channels++) {
      // If not enabled -> continue
      if (0xffff == *channels)
        continue;
      // Get channel index and check if defined
      uint16_t cidx = qFromLittleEndian(*channels);
      if (! ctx.hasChannel(cidx))
        continue;
      // If defined -> add channel to zone obj
      if (extend_last_zone)
        last_zone->B()->addChannel(ctx.getChannel(cidx));
      else
        last_zone->A()->addChannel(ctx.getChannel(cidx));
    }
  }
  return true;
}

bool
D868UVCodeplug::linkZones(Config *config, CodeplugContext &ctx) {
  // Zones are linked during creation.
  return true;
}


void
D868UVCodeplug::allocateScanLists() {
  /*
   * Allocate scan lists
   */
  uint8_t *scanlist_bitmap = data(SCAN_BITMAP);
  for (uint8_t i=0; i<NUM_SCAN_LISTS; i++) {
    // Get byte and bit for scan list, bank and bank_idx
    uint16_t bit = i%8, byte = i/8;
    uint8_t bank = (i/NUM_SCANLISTS_PER_BANK), bank_idx = (i%NUM_SCANLISTS_PER_BANK);
    // if disabled -> skip
    if (0 == ((scanlist_bitmap[byte]>>bit) & 0x01))
      continue;
    // Allocate scan lists indivitually
    uint32_t addr = SCAN_LIST_BANK_0 + bank*SCAN_LIST_BANK_OFFSET + bank_idx*SCAN_LIST_OFFSET;
    if (nullptr == data(addr, 0)) {
      image(0).addElement(addr, SCAN_LIST_SIZE);
      memset(data(addr), 0xff, SCAN_LIST_SIZE);
    }
  }
}

bool
D868UVCodeplug::encodeScanLists(Config *config, const Flags &flags) {
  // Encode scan lists
  for (int i=0; i<config->scanlists()->count(); i++) {
    uint8_t bank = i/NUM_SCANLISTS_PER_BANK, idx = i%NUM_SCANLISTS_PER_BANK;
    scanlist_t *scan = (scanlist_t *)data(
          SCAN_LIST_BANK_0 + bank*SCAN_LIST_BANK_OFFSET + idx*SCAN_LIST_OFFSET);
    scan->fromScanListObj(config->scanlists()->scanlist(i), config);
  }
  return true;
}

bool
D868UVCodeplug::createScanLists(Config *config, CodeplugContext &ctx) {
  // Create scan lists
  uint8_t *scanlist_bitmap = data(SCAN_BITMAP);
  for (uint i=0; i<NUM_SCAN_LISTS; i++) {
    uint8_t byte=i/8, bit=i%8;
    if (0 == ((scanlist_bitmap[byte]>>bit) & 0x01))
      continue;
    uint8_t bank = i/NUM_SCANLISTS_PER_BANK, bank_idx = i%NUM_SCANLISTS_PER_BANK;
    uint32_t addr = SCAN_LIST_BANK_0 + bank*SCAN_LIST_BANK_OFFSET + bank_idx*SCAN_LIST_OFFSET;
    scanlist_t *scanl = (scanlist_t *)data(addr);
    // Create scanlist
    ScanList *obj = scanl->toScanListObj();
    ctx.addScanList(obj, i);
    // Link scanlists immediately, all channels are defined allready
    scanl->linkScanListObj(obj, ctx);
  }
  return true;
}

bool
D868UVCodeplug::linkScanLists(Config *config, CodeplugContext &ctx) {
  // already linked during creation
  return true;
}


void
D868UVCodeplug::allocateGeneralSettings() {
  image(0).addElement(ADDR_GENERAL_CONFIG, GENERAL_CONFIG_SIZE);
}

bool
D868UVCodeplug::encodeGeneralSettings(Config *config, const Flags &flags) {
  ((general_settings_base_t *)data(ADDR_GENERAL_CONFIG))->fromConfig(config, flags);
  return true;
}

bool
D868UVCodeplug::decodeGeneralSettings(Config *config) {
  ((general_settings_base_t *)data(ADDR_GENERAL_CONFIG))->updateConfig(config);
  return true;
}


void
D868UVCodeplug::allocateZoneChannelList() {
  image(0).addElement(ADDR_ZONE_CHANNELS, ZONE_CHANNELS_SIZE);
}


void
D868UVCodeplug::allocateDTMFNumbers() {
  image(0).addElement(ADDR_DTMF_NUMBERS, DTMF_NUMBERS_SIZE);
}


void
D868UVCodeplug::allocateBootSettings() {
  image(0).addElement(ADDR_BOOT_SETTINGS, BOOT_SETTINGS_SIZE);
}

bool
D868UVCodeplug::encodeBootSettings(Config *config, const Flags &flags) {
  ((boot_settings_t *)data(ADDR_BOOT_SETTINGS))->fromConfig(config, flags);
  return true;
}

bool
D868UVCodeplug::decodeBootSettings(Config *config) {
  ((boot_settings_t *)data(ADDR_BOOT_SETTINGS))->updateConfig(config);
  return true;
}


void
D868UVCodeplug::allocateGPSSystems() {
  image(0).addElement(ADDR_GPS_SETTINGS, GPS_SETTINGS_SIZE);
  image(0).addElement(ADDR_GPS_MESSAGE, GPS_MESSAGE_SIZE);
}

bool
D868UVCodeplug::encodeGPSSystems(Config *config, const Flags &flags) {
  gps_settings_t *gps = (gps_settings_t *)data(ADDR_GPS_SETTINGS);
  gps->fromConfig(config, flags);
  return true;
}

bool
D868UVCodeplug::createGPSSystems(Config *config, CodeplugContext &ctx) {
  QSet<uint8_t> systems;
  // First find all GPS systems linked, that is referenced by any channel
  // Create channels
  uint8_t *channel_bitmap = data(CHANNEL_BITMAP);
  for (uint16_t i=0; i<NUM_CHANNELS; i++) {
    // Check if channel is enabled:
    uint16_t  bit = i%8, byte = i/8, bank = i/128, idx = i%128;
    if (0 == ((channel_bitmap[byte]>>bit) & 0x01))
      continue;
    if (ctx.getChannel(i)->is<AnalogChannel>())
      continue;
    channel_t *ch = (channel_t *)data(CHANNEL_BANK_0
                                      +bank*CHANNEL_BANK_OFFSET
                                      +idx*sizeof(channel_t));
    if (ch->aprs_report)
      systems.insert(ch->gps_system);
  }
  // Then create all referenced GPS systems
  gps_settings_t *gps = (gps_settings_t *)data(ADDR_GPS_SETTINGS);
  for (QSet<uint8_t>::iterator idx=systems.begin(); idx!=systems.end(); idx++)
    gps->createGPSSystem(*idx, config, ctx);
  return true;
}

bool
D868UVCodeplug::linkGPSSystems(Config *config, CodeplugContext &ctx) {
  gps_settings_t *gps = (gps_settings_t *)data(ADDR_GPS_SETTINGS);
  // Then link all referenced GPS systems
  for (uint8_t i=0; i<8; i++) {
    if (! ctx.hasGPSSystem(i))
      continue;
    gps->linkGPSSystem(i, config, ctx);
  }
  return true;
}


void
D868UVCodeplug::allocateSMSMessages() {
  // Prefab. SMS messages
  uint8_t *messages_bytemap = data(MESSAGE_BYTEMAP);
  uint message_count = 0;
  for (uint8_t i=0; i<NUM_MESSAGES; i++) {
    uint8_t bank = i/NUM_MESSAGES_PER_BANK;
    if (0xff == messages_bytemap[i])
      continue;
    message_count++;
    uint32_t addr = MESSAGE_BANK_0 + bank*MESSAGE_BANK_SIZE;
    if (nullptr == data(addr, 0)) {
      image(0).addElement(addr, MESSAGE_BANK_SIZE);
    }
  }
  if (message_count) {
    image(0).addElement(MESSAGE_INDEX_LIST, 0x10*message_count);
  }
}

void
D868UVCodeplug::allocateHotKeySettings() {
  // Allocate Hot Keys
  image(0).addElement(ADDR_HOTKEY, HOTKEY_SIZE);
}

void
D868UVCodeplug::allocateRepeaterOffsetSettings() {
  // Offset frequencies
  image(0).addElement(ADDR_OFFSET_FREQ, OFFSET_FREQ_SIZE);
}

void
D868UVCodeplug::allocateAlarmSettings() {
  // Alarm settings
  image(0).addElement(ADDR_ALARM_SETTING, ALARM_SETTING_SIZE);
  image(0).addElement(ADDR_ALARM_SETTING_EXT, ALARM_SETTING_EXT_SIZE);
}

void
D868UVCodeplug::allocateFMBroadcastSettings() {
  // FM broad-cast settings
  image(0).addElement(ADDR_FMBC, FMBC_SIZE+FMBC_VFO_SIZE);
}

void
D868UVCodeplug::allocate5ToneIDs() {
  // Allocate 5-tone functions
  uint8_t *bitmap = data(FIVE_TONE_ID_BITMAP);
  for (uint8_t i=0; i<NUM_FIVE_TONE_IDS; i++) {
    uint16_t  bit = i%8, byte = i/8;
    if (0 == (bitmap[byte] & (1<<bit)))
      continue;
    image(0).addElement(ADDR_FIVE_TONE_ID_LIST + i*FIVE_TONE_ID_SIZE, FIVE_TONE_ID_SIZE);
  }
}

void
D868UVCodeplug::allocate5ToneFunctions() {
  image(0).addElement(ADDR_FIVE_TONE_FUNCTIONS, FIVE_TONE_FUNCTIONS_SIZE);
}

void
D868UVCodeplug::allocate5ToneSettings() {
  image(0).addElement(ADDR_FIVE_TONE_SETTINGS, FIVE_TONE_SETTINGS_SIZE);
}

void
D868UVCodeplug::allocate2ToneIDs() {
  // Allocate 2-tone encoding
  uint8_t *enc_bitmap = data(TWO_TONE_IDS_BITMAP);
  for (uint8_t i=0; i<NUM_TWO_TONE_IDS; i++) {
    uint16_t  bit = i%8, byte = i/8;
    if (0 == (enc_bitmap[byte] & (1<<bit)))
      continue;
    image(0).addElement(ADDR_TWO_TONE_IDS + i*TWO_TONE_ID_SIZE, TWO_TONE_ID_SIZE);
  }
}


void
D868UVCodeplug::allocate2ToneFunctions() {
  // Allocate 2-tone decoding
  uint8_t *dec_bitmap = data(TWO_TONE_FUNCTIONS_BITMAP);
  for (uint8_t i=0; i<NUM_TWO_TONE_FUNCTIONS; i++) {
    uint16_t  bit = i%8, byte = i/8;
    if (0 == (dec_bitmap[byte] & (1<<bit)))
      continue;
    image(0).addElement(ADDR_TWO_TONE_FUNCTIONS + i*TWO_TONE_FUNCTION_SIZE, TWO_TONE_FUNCTION_SIZE);
  }
}

void
D868UVCodeplug::allocate2ToneSettings() {
  image(0).addElement(ADDR_TWO_TONE_SETTINGS, TWO_TONE_SETTINGS_SIZE);
}


void
D868UVCodeplug::allocateDTMFSettings() {
  image(0).addElement(ADDR_DTMF_SETTINGS, DTMF_SETTINGS_SIZE);
}
