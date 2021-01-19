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
#define ANALOGCONTACT_LIST_SIZE   0x00000080 // Address of analog contact index list
#define ANALOGCONTACT_BITMAP      0x02900100 // Address of contact bytemap
#define ANALOGCONTACT_BITMAP_SIZE 0x00000080 // Size of contact bytemap

#define NUM_RXGRP                 250        // Total number of RX group lists
#define ADDR_RXGRP_0              0x02980000 // Address of the first RX group list.
#define RXGRP_SIZE                0x00000120 // Size of each RX group list.
#define RXGRP_OFFSET              0x00000200 // Offset between group lists.
#define RXGRP_BITMAP              0x025C0B10 // Address of RX group list bitmap.
#define RXGRP_BITMAP_SIZE         0x00000020 // Storage size of RX group list bitmap.

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

#define NUM_SCAN_LISTS            250
#define NUM_SCANLISTS_PER_BANK    16
#define SCAN_LIST_BANK_0          0x01080000 // First scanlist bank
#define SCAN_LIST_OFFSET          0x00000200 // Offset to next list.
#define SCAN_LIST_SIZE            0x000000c0 // Size of scan-list.
#define SCAN_LIST_BANK_OFFSET     0x00040000 // Offset to next bank
#define SCAN_BITMAP               0x024c1340 // Address of scan-list bitmap.
#define SCAN_BITMAP_SIZE          0x00000020 // Size of scan-list bitmap.

#define ADDR_GENERAL_CONFIG       0x02500000
#define GENERAL_CONFIG_SIZE       0x00000630

#define ADDR_APRS_SETTING         0x02501000 // Address of APRS settings
#define APRS_SETTING_SIZE         0x00000040 // Size of the APRS settings
#define ADDR_APRS_MESSAGES        0x02501200 // Address of APRS messages
#define APRS_MESSAGES_SIZE        0x00000040 // Size of APRS messages

#define NUM_GPS_SYSTEMS           8
#define ADDR_GPS_SETTING          0x02501040 // Address of GPS settings
#define GPS_SETTING_SIZE          0x00000060 // Size of the GPS settings

#define NUM_MESSAGES              100
#define NUM_MESSAGES_PER_BANK     8
#define MESSAGE_SIZE              0x00000100
#define MESSAGE_BANK_0            0x02140000
#define MESSAGE_BANK_SIZE         0x00000800
#define MESSAGE_BANK_OFFSET       0x00040000
#define MESSAGE_INDEX_LIST        0x01640000
#define MESSAGE_BYTEMAP           0x01640800
#define MESSAGE_BYTEMAP_SIZE      0x00000090

#define ADDR_HOTKEY               0x025C0000
#define HOTKEY_SIZE               0x00000860
#define STATUSMESSAGE_BITMAP      0x025C0B00
#define STATUSMESSAGE_BITMAP_SIZE 0x00000010

#define NUM_ENCRYPTION_KEYS       256
#define ADDR_ENCRYPTION_KEYS      0x024C4000
#define ENCRYPTION_KEY_SIZE       0x00000040
#define ENCRYPTION_KEYS_SIZE      0x00004000

#define ADDR_OFFSET_FREQ          0x024C2000
#define OFFSET_FREQ_SIZE          0x000003F0

#define ADDR_TALKER_ALIAS         0x02501400
#define TALKER_ALIAS_SIZE         0x00000100

#define ADDR_ALARM_SETTING        0x024C1400
#define ALARM_SETTING_SIZE        0x00000020

#define FMBC_BITMAP               0x02480210
#define FMBC_BITMAP_SIZE          0x00000020
#define ADDR_FMBC                 0x02480000
#define FMBC_SIZE                 0x00000200
#define ADDR_FMBC_VFO             0x02480200
#define FMBC_VFO_SIZE             0x00000010


using namespace Signaling;

Code ctcss_num2code[52] = {
  SIGNALING_NONE, // 62.5 not supported
  CTCSS_67_0Hz,  CTCSS_71_0Hz,  CTCSS_74_4Hz,  CTCSS_77_0Hz,  CTCSS_79_9Hz,  CTCSS_82_5Hz,
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

uint8_t ctcss_code2num(Signaling::Code code) {
  for (uint8_t i=0; i<52; i++) {
    if (code == ctcss_num2code[i])
      return i;
  }
  return 0;
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::channel_t
 * ******************************************************************************************** */
D878UVCodeplug::channel_t::channel_t() {
  clear();
}

void
D878UVCodeplug::channel_t::clear() {
  memset(this, 0, sizeof(D878UVCodeplug::channel_t));
  custom_ctcss = qToLittleEndian(0x09cf); // some value
  scan_list_index  = 0xff; // None
  group_list_index = 0xff; // None
  id_index = 0;
  squelch_mode = SQ_CARRIER;
  tx_permit = ADMIT_ALWAYS;

}

bool
D878UVCodeplug::channel_t::isValid() const {
  return (0 != name[0]) && (0xff != name[0]);
}

double
D878UVCodeplug::channel_t::getRXFrequency() const {
  return decode_frequency(qFromBigEndian(rx_frequency));
}

void
D878UVCodeplug::channel_t::setRXFrequency(double f) {
  rx_frequency = qToBigEndian(encode_frequency(f));
}

double
D878UVCodeplug::channel_t::getTXFrequency() const {
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
D878UVCodeplug::channel_t::setTXFrequency(double f) {
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
D878UVCodeplug::channel_t::getName() const {
  return decode_ascii(name, 16, 0);
}

void
D878UVCodeplug::channel_t::setName(const QString &name) {
  encode_ascii(this->name, name, 16, 0);
}

Signaling::Code
D878UVCodeplug::channel_t::getRXTone() const {
  // If squelch is not SQ_TONE -> RX tone is ignored
  if (SQ_TONE != squelch_mode)
    return Signaling::SIGNALING_NONE;

  if (rx_ctcss && (ctcss_receive < 52))
    return ctcss_num2code[ctcss_receive];
  else if (rx_dcs && (qFromLittleEndian(dcs_receive) < 512))
    return Signaling::fromDCSNumber(dec_to_oct(qFromLittleEndian(dcs_receive)), false);
  else if (rx_dcs && (qFromLittleEndian(dcs_receive) >= 512))
    return Signaling::fromDCSNumber(dec_to_oct(qFromLittleEndian(dcs_receive)-512), true);
 return Signaling::SIGNALING_NONE;
}

void
D878UVCodeplug::channel_t::setRXTone(Code code) {
  if (Signaling::SIGNALING_NONE == code) {
    squelch_mode = SQ_CARRIER;
    rx_ctcss = rx_dcs = 0;
    ctcss_receive = dcs_receive = 0;
  } else if (Signaling::isCTCSS(code)) {
    squelch_mode = SQ_TONE;
    rx_ctcss = 1;
    rx_dcs = 0;
    ctcss_receive = ctcss_code2num(code);
    dcs_receive = 0;
  } else if (Signaling::isDCSNormal(code)) {
    squelch_mode = SQ_TONE;
    rx_ctcss = 0;
    rx_dcs = 1;
    ctcss_receive = 0;
    dcs_receive = qToLittleEndian(oct_to_dec(Signaling::toDCSNumber(code)));
  } else if (Signaling::isDCSInverted(code)) {
    squelch_mode = SQ_TONE;
    rx_ctcss = 0;
    rx_dcs = 1;
    ctcss_receive = 0;
    dcs_receive = qToLittleEndian(oct_to_dec(Signaling::toDCSNumber(code))+512);
  }
}

Signaling::Code
D878UVCodeplug::channel_t::getTXTone() const {
  if (tx_ctcss && (ctcss_transmit < 52))
    return ctcss_num2code[ctcss_transmit];
  else if (tx_dcs && (qFromLittleEndian(dcs_transmit) < 512))
    return Signaling::fromDCSNumber(dec_to_oct(qFromLittleEndian(dcs_transmit)), false);
  else if (tx_dcs && (qFromLittleEndian(dcs_transmit) >= 512))
    return Signaling::fromDCSNumber(dec_to_oct(qFromLittleEndian(dcs_transmit)-512), true);
 return Signaling::SIGNALING_NONE;
}

void
D878UVCodeplug::channel_t::setTXTone(Code code) {
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
D878UVCodeplug::channel_t::toChannelObj() const {
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
  case POWER_TURBO:
    power = Channel::MaxPower;
    break;
  }
  bool rxOnly = (1 == this->rx_only);

  Channel *ch;
  if (MODE_ANALOG == channel_mode) {
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
  } else if (MODE_DIGITAL == channel_mode) {
    DigitalChannel::Admit admit = DigitalChannel::AdmitNone;
    switch ((channel_t::Admit) tx_permit) {
    case ADMIT_ALWAYS:
      admit = DigitalChannel::AdmitNone;
      break;
    case ADMIT_CH_FREE:
      admit = DigitalChannel::AdmitFree;
      break;
    case ADMIT_CC_SAME:
    case ADMIT_CC_DIFF:
      admit = DigitalChannel::AdmitColorCode;
      break;
    }
    DigitalChannel::TimeSlot ts = (slot2 ? DigitalChannel::TimeSlot2 : DigitalChannel::TimeSlot1);
    ch = new DigitalChannel(
          getName(), getRXFrequency(), getTXFrequency(), power, 0.0, rxOnly, admit,
          color_code, ts, nullptr, nullptr, nullptr, nullptr);
  } else {
    logError() << "Cannot create channel '" << getName()
               << "': Mixed channel types not supported.";
    return nullptr;
  }

  return ch;
}

bool
D878UVCodeplug::channel_t::linkChannelObj(Channel *c, const CodeplugContext &ctx) const {
  // If channel is digital
  if (MODE_DIGITAL == channel_mode) {
    DigitalChannel *dc = c->as<DigitalChannel>();
    if (nullptr == dc)
      return false;

    // Check if default contact is set
    uint32_t conIdx = qFromLittleEndian(contact_index);
    if ((0xffffffff != conIdx) && ctx.hasDigitalContact(conIdx))
      dc->setTXContact(ctx.getDigitalContact(conIdx));

    // Set if RX group list is set
    if ((0xff != group_list_index) && ctx.hasGroupList(group_list_index))
      dc->setRXGroupList(ctx.getGroupList(group_list_index));

    // Link to GPS system
    //if ((APRS_REPORT_DIGITAL == aprs_report) && ctx.hasGPSSystem(gps_system))
    //  dc->setGPSSystem(ctx.getGPSSystem(gps_system));

  }

  // For both, analog and digital channels:

  // If channel has scan list
  if ((0xff != scan_list_index) && ctx.hasScanList(scan_list_index))
    c->setScanList(ctx.getScanList(scan_list_index));

  return true;
}

void
D878UVCodeplug::channel_t::fromChannelObj(const Channel *c, const Config *conf) {
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
    squelch_mode = (Signaling::SIGNALING_NONE == ac->rxTone()) ? SQ_CARRIER : SQ_TONE;
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
    case DigitalChannel::AdmitColorCode: tx_permit = ADMIT_CC_SAME; break;
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
    /*if (nullptr != dc->gpsSystem()) {
      aprs_report = APRS_REPORT_DIGITAL;
      gps_system = conf->gpsSystems()->indexOf(dc->gpsSystem());
    }*/
  }
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::contact_t
 * ******************************************************************************************** */
D878UVCodeplug::contact_t::contact_t() {
  clear();
}

void
D878UVCodeplug::contact_t::clear() {
  memset(this, 0, sizeof(D878UVCodeplug::contact_t));
}

bool
D878UVCodeplug::contact_t::isValid() const {
  return (0 != name[0]) && (0xff != name[0]);
}

QString
D878UVCodeplug::contact_t::getName() const {
  return decode_ascii(name, 16, 0x00);
}

void
D878UVCodeplug::contact_t::setName(const QString &name) {
  encode_ascii(this->name, name, 16, 0);
}

DigitalContact::Type
D878UVCodeplug::contact_t::getType() const {
  switch ((CallType) type) {
  case CALL_PRIVATE: return DigitalContact::PrivateCall;
  case CALL_GROUP: return DigitalContact::GroupCall;
  case CALL_ALL: return DigitalContact::AllCall;
  }
}

void
D878UVCodeplug::contact_t::setType(DigitalContact::Type type) {
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
D878UVCodeplug::contact_t::getId() const {
  uint32_t tmp = qFromLittleEndian(id);
  return decode_dmr_id_bcd((const uint8_t *)&tmp);
}

void
D878UVCodeplug::contact_t::setId(uint32_t id) {
  uint32_t tmp;
  encode_dmr_id_bcd((uint8_t *)&tmp, id);
  this->id = qToLittleEndian(tmp);
}

bool
D878UVCodeplug::contact_t::getAlert() const {
  return (ALERT_NONE != call_alert);
}

void
D878UVCodeplug::contact_t::setAlert(bool enable) {
  call_alert = enable ? ALERT_RING : ALERT_NONE;
}

DigitalContact *
D878UVCodeplug::contact_t::toContactObj() const {
  return new DigitalContact(getType(), getName(), getId(), getAlert());
}

void
D878UVCodeplug::contact_t::fromContactObj(const DigitalContact *contact) {
  clear();
  setType(contact->type());
  setName(contact->name());
  setId(contact->number());
  setAlert(contact->rxTone());
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::grouplist_t
 * ******************************************************************************************** */
D878UVCodeplug::grouplist_t::grouplist_t() {
  clear();
}

void
D878UVCodeplug::grouplist_t::clear() {
  memset(member, 0xff, sizeof(member));
  memset(name, 0x00, sizeof(name));
  memset(unused, 0x00, sizeof(unused));
}

bool
D878UVCodeplug::grouplist_t::isValid() const {
  return (0 != name[0]) && (0xff != name[0]);
}

QString
D878UVCodeplug::grouplist_t::getName() const {
  return decode_ascii(name, 16, 0x00);
}

void
D878UVCodeplug::grouplist_t::setName(const QString &name) {
  encode_ascii(this->name, name, 16, 0x00);
}

RXGroupList *
D878UVCodeplug::grouplist_t::toGroupListObj() const {
  return new RXGroupList(getName());
}

bool
D878UVCodeplug::grouplist_t::linkGroupList(RXGroupList *lst, const CodeplugContext &ctx) {
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
D878UVCodeplug::grouplist_t::fromGroupListObj(const RXGroupList *lst, const Config *conf) {
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
 * Implementation of D878UVCodeplug::scanlist_t
 * ******************************************************************************************** */
D878UVCodeplug::scanlist_t::scanlist_t() {
  clear();
}

void
D878UVCodeplug::scanlist_t::clear() {
  _unused0 = 0;
  prio_ch_select = PRIO_CHAN_OFF;
  priority_ch1 = 0xffff;
  priority_ch2 = 0xffff;
  look_back_a = qToLittleEndian(0x000f);
  look_back_b = qToLittleEndian(0x0019);
  dropout_delay = qToLittleEndian(0x001d);
  dwell = qToLittleEndian(0x001d);
  revert_channel = REVCH_SELECTED;
  memset(name, 0, sizeof(name));
  _pad31 = 0;
  memset(member, 0xff, sizeof(member));
  memset(_unused132, 0, sizeof(_unused132));
}

QString
D878UVCodeplug::scanlist_t::getName() const {
  return decode_ascii(name, sizeof(name), 0);
}

void
D878UVCodeplug::scanlist_t::setName(const QString &name) {
  encode_ascii(this->name, name, 16, 0);
}

ScanList *
D878UVCodeplug::scanlist_t::toScanListObj() {
  return new ScanList(getName());
}

void
D878UVCodeplug::scanlist_t::linkScanListObj(ScanList *lst, CodeplugContext &ctx) {
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
D878UVCodeplug::scanlist_t::fromScanListObj(ScanList *lst, Config *config) {
  clear();

  setName(lst->name());

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
 * Implementation of D878UVCodeplug::radioid_t
 * ******************************************************************************************** */
D878UVCodeplug::radioid_t::radioid_t() {
  clear();
}

void
D878UVCodeplug::radioid_t::clear() {
  memset(this, 0, sizeof(D878UVCodeplug::radioid_t));
}

bool
D878UVCodeplug::radioid_t::isValid() const {
  return ((0x00 != name[0]) && (0xff != name[0]));
}

QString
D878UVCodeplug::radioid_t::getName() const {
  return decode_ascii(name, 16, 0);
}

void
D878UVCodeplug::radioid_t::setName(const QString name) {
  encode_ascii(this->name, name, 16, 0);
}

uint32_t
D878UVCodeplug::radioid_t::getId() const {
  uint32_t id_bcd = qFromLittleEndian(this->id);
  return decode_dmr_id_bcd((const uint8_t *) &id_bcd);
}

void
D878UVCodeplug::radioid_t::setId(uint32_t num) {
  uint32_t id_bcd;
  encode_dmr_id_bcd((uint8_t *)&id_bcd, num);
  this->id = qToLittleEndian(id_bcd);
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::general_settings_t
 * ******************************************************************************************** */
D878UVCodeplug::general_settings_t::general_settings_t() {
  clear();
}

void
D878UVCodeplug::general_settings_t::clear() {
  memset(intro_line1, 0, sizeof(intro_line2));
  memset(intro_line2, 0, sizeof(intro_line2));
}

QString
D878UVCodeplug::general_settings_t::getIntroLine1() const {
  return decode_ascii(intro_line1, 14, 0);
}

void
D878UVCodeplug::general_settings_t::setIntroLine1(const QString line) {
  encode_ascii(intro_line1, line, 14, 0);
}

QString
D878UVCodeplug::general_settings_t::getIntroLine2() const {
  return decode_ascii(intro_line2, 14, 0);
}

void
D878UVCodeplug::general_settings_t::setIntroLine2(const QString line) {
  encode_ascii(intro_line2, line, 14, 0);
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::aprs_system_t
 * ******************************************************************************************** */
void
D878UVCodeplug::aprs_setting_t::setAutoTxInterval(int sec) {
  // round up to multiples of 30
  auto_tx_interval = (sec+29)/30;
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
  int idx = conf->gpsSystems()->indexOf(sys);
  if ((idx < 0) || idx > 7)
    return;
  if (sys->hasContact()) {
    setContactId(idx, sys->contact()->number());
    setContactType(idx, sys->contact()->type());
  }
  if (sys->hasRevertChannel() && (SelectedChannel::get() != (Channel *)sys->revertChannel())) {
    digi_channels[idx] = conf->channelList()->indexOf(sys->revertChannel());
    timeslots[idx] = 0; // Use TS of channel
  }
}

void
D878UVCodeplug::gps_systems_t::fromGPSSystems(const Config *conf) {
  if (conf->gpsSystems()->count() > 8)
    return;
  for (int i=0; i<conf->gpsSystems()->count(); i++)
    fromGPSSystemObj(conf->gpsSystems()->gpsSystem(i), conf);
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
    ctx.config()->contacts()->addContact(cont);
  }
  // link contact to GPS system.
  sys->setContact(cont);

  return true;
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::contact_map_t
 * ******************************************************************************************** */
D878UVCodeplug::contact_map_t::contact_map_t() {
  clear();
}

void
D878UVCodeplug::contact_map_t::clear() {
  memset(this, 0xff, sizeof(contact_map_t));
}

bool
D878UVCodeplug::contact_map_t::isValid() const {
  return (0xffffffff != id_group) && (0xffffffff != contact_index);
}

bool
D878UVCodeplug::contact_map_t::isGroup() const {
  uint32_t tmp = qFromLittleEndian(id_group);
  return tmp & 0x01;
}
uint32_t
D878UVCodeplug::contact_map_t::ID() const {
  uint32_t tmp = qFromLittleEndian(id_group);
  tmp = tmp>>1;
  return decode_dmr_id_bcd_le((uint8_t *)&tmp);
}
void
D878UVCodeplug::contact_map_t::setID(uint32_t id, bool group) {
  uint32_t tmp; encode_dmr_id_bcd_le((uint8_t *)&tmp, id);
  tmp = ( (tmp << 1) | (group ? 1 : 0) );
  id_group = qToLittleEndian(tmp);
}


uint32_t
D878UVCodeplug::contact_map_t::index() const {
  return qFromLittleEndian(contact_index);
}
void
D878UVCodeplug::contact_map_t::setIndex(uint32_t index) {
  contact_index = qToLittleEndian(index);
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug
 * ******************************************************************************************** */
D878UVCodeplug::D878UVCodeplug(QObject *parent)
  : CodePlug(parent)
{
  addImage("Anytone AT-D878UV Codeplug");

  // Channel bitmap
  image(0).addElement(CHANNEL_BITMAP, CHANNEL_BITMAP_SIZE);
  // Zone bitmap
  image(0).addElement(ZONE_BITMAPS, ZONE_BITMAPS_SIZE);
  // Contacts bitmap
  image(0).addElement(CONTACTS_BITMAP, CONTACTS_BITMAP_SIZE);
  // Analog contacts bitmap
  image(0).addElement(ANALOGCONTACT_BITMAP, ANALOGCONTACT_BITMAP_SIZE);
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
}

void
D878UVCodeplug::clear() {
}

void
D878UVCodeplug::allocateUntouched() {
  // Allocate VFO channels
  image(0).addElement(VFO_A_ADDR, sizeof(channel_t));
  image(0).addElement(VFO_A_ADDR+0x2000, sizeof(channel_t));
  image(0).addElement(VFO_B_ADDR, sizeof(channel_t));
  image(0).addElement(VFO_B_ADDR+0x2000, sizeof(channel_t));

  // General config
  image(0).addElement(ADDR_GENERAL_CONFIG, GENERAL_CONFIG_SIZE);
  // GPS settings
  //image(0).addElement(ADDR_GPS_SETTING, GPS_SETTING_SIZE);

  /*
   * Kept but untouched memory regions.
   */

  /*
   * Allocate analog contacts
   */
  uint8_t *analog_contact_bytemap = data(ANALOGCONTACT_BITMAP);
  uint contactCount = 0;
  for (uint8_t i=0; i<NUM_ANALOGCONTACTS; i++) {
    // if disabled -> skip
    if (0 == analog_contact_bytemap[i])
      continue;
    contactCount++;
    uint32_t addr = ANALOGCONTACT_BANK_0 + (i/ANALOGCONTACTS_PER_BANK)*ANALOGCONTACT_BANK_SIZE;
    if (nullptr == data(addr, 0)) {
      image(0).addElement(addr, ANALOGCONTACT_BANK_SIZE);
      memset(data(addr), 0x00, ANALOGCONTACT_BANK_SIZE);
    }
  }
  image(0).addElement(ANALOGCONTACT_INDEX_LIST, 0xff, ANALOGCONTACT_LIST_SIZE);

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

  // Allocate Hot Keys
  image(0).addElement(ADDR_HOTKEY, HOTKEY_SIZE);
  // Encryption keys
  image(0).addElement(ADDR_ENCRYPTION_KEYS, ENCRYPTION_KEYS_SIZE);
  // Offset frequencies
  image(0).addElement(ADDR_OFFSET_FREQ, OFFSET_FREQ_SIZE);
  // Talker alias settings
  image(0).addElement(ADDR_TALKER_ALIAS, TALKER_ALIAS_SIZE);
  // Alarm settings
  image(0).addElement(ADDR_ALARM_SETTING, ALARM_SETTING_SIZE);
  // FM broad-cast settings
  image(0).addElement(ADDR_FMBC, FMBC_SIZE+FMBC_VFO_SIZE);
  // APRS settings
  //image(0).addElement(ADDR_APRS_SETTING, APRS_SETTING_SIZE);
  //image(0).addElement(ADDR_APRS_MESSAGES, APRS_MESSAGES_SIZE);

  // Unknown memory region
  image(0).addElement(0x01042000, 0x020);
  image(0).addElement(0x01042080, 0x010);
  image(0).addElement(0x024C0C80, 0x010);
  image(0).addElement(0x024C0D00, 0x200);
  image(0).addElement(0x024C0000, 0x020);
  image(0).addElement(0x024C1000, 0x0D0);
  image(0).addElement(0x024C1100, 0x010);
  image(0).addElement(0x024C1280, 0x020);
  image(0).addElement(0x024C1440, 0x030);
  image(0).addElement(0x024C1700, 0x040);
  image(0).addElement(0x024C1800, 0x500);
  image(0).addElement(0x024C2400, 0x030);
  image(0).addElement(0x024C2600, 0x010);
  //image(0).addElement(0x02501280, 0x030); // <- related to GPS/APRS?
}

void
D878UVCodeplug::allocateForEncoding() {
  /*
   * Allocate channels
   */
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
    if (nullptr == data(addr+0x2000, 0)) {
      image(0).addElement(addr+0x2000, sizeof(channel_t));
      memset(data(addr+0x2000), 0x00, sizeof(channel_t));
    }
  }

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

  /*
   * Allocate contacts
   */
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

  /*
   * Allocate radio IDs
   */
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

void
D878UVCodeplug::allocateForDecoding() {
  /*
   * Allocate channels
   */
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
    if (nullptr == data(addr, 0))
      image(0).addElement(addr, sizeof(channel_t));
  }

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

  /*
   * Allocate contacts
   */
  uint8_t *contact_bitmap = data(CONTACTS_BITMAP);
  for (uint16_t i=0; i<NUM_CONTACTS; i++) {
    // Get byte and bit for contact, as well as bank of contact
    uint16_t bit = i%8, byte = i/8;
    // enabled if false (ass hole)
    if (1 == ((contact_bitmap[byte]>>bit) & 0x01))
      continue;
    uint32_t addr = CONTACT_BANK_0+(i/CONTACTS_PER_BANK)*CONTACT_BANK_SIZE;
    if (nullptr == data(addr, 0)) {
      image(0).addElement(addr, CONTACT_BANK_SIZE);
    }
  }

  /*
   * Allocate analog contacts
   */
  uint8_t *analog_contact_bytemap = data(ANALOGCONTACT_BITMAP);
  for (uint8_t i=0; i<NUM_ANALOGCONTACTS; i++) {
    // if disabled -> skip
    if (0 == analog_contact_bytemap[i])
      continue;
    uint32_t addr = ANALOGCONTACT_BANK_0 + (i/ANALOGCONTACTS_PER_BANK)*ANALOGCONTACT_BANK_SIZE;
    if (nullptr == data(addr, 0)) {
      image(0).addElement(addr, ANALOGCONTACT_BANK_SIZE);
    }
  }

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
    }
  }

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
    }
  }

  /*
   * Allocate radio IDs
   */
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

  // General config
  image(0).addElement(ADDR_GENERAL_CONFIG, GENERAL_CONFIG_SIZE);
  // GPS settings
  image(0).addElement(ADDR_GPS_SETTING, GPS_SETTING_SIZE);
}


void
D878UVCodeplug::setBitmaps(Config *config)
{
  // Mark first radio ID as valid
  uint8_t *radioid_bitmap = data(RADIOID_BITMAP);
  radioid_bitmap[0] |= 1;

  // Mark valid channels (set bit)
  uint8_t *channel_bitmap = data(CHANNEL_BITMAP);
  memset(channel_bitmap, 0, CHANNEL_BITMAP_SIZE);
  for (int i=0; i<config->channelList()->count(); i++) {
    channel_bitmap[i/8] |= (1 << (i%8));
  }

  // Mark valid contacts (clear bit)
  uint8_t *contact_bitmap = data(CONTACTS_BITMAP);
  memset(contact_bitmap, 0x00, CONTACTS_BITMAP_SIZE);
  memset(contact_bitmap, 0xff, NUM_CONTACTS/8+1);
  for (int i=0; i<config->contacts()->digitalCount(); i++) {
    contact_bitmap[i/8] &= ~(1 << (i%8));
  }

  // Mark valid zones (set bits)
  uint8_t *zone_bitmap = data(ZONE_BITMAPS);
  memset(zone_bitmap, 0x00, ZONE_BITMAPS_SIZE);
  for (int i=0,z=0; i<config->zones()->count(); i++) {
    zone_bitmap[z/8] |= (1 << (z%8)); z++;
    if (config->zones()->zone(i)->B()->count()) {
      zone_bitmap[z/8] |= (1 << (z%8)); z++;
    }
  }

  // Mark group lists
  uint8_t *group_bitmap = data(RXGRP_BITMAP);
  memset(group_bitmap, 0x00, RXGRP_BITMAP_SIZE);
  for (int i=0; i<config->rxGroupLists()->count(); i++)
    group_bitmap[i/8] |= (1 << (i%8));

  // Mark scan lists
  uint8_t *scan_bitmap = data(SCAN_BITMAP);
  memset(scan_bitmap, 0x00, SCAN_BITMAP_SIZE);
  for (int i=0; i<config->scanlists()->count(); i++) {
    scan_bitmap[i/8] |= (1<<(i%8));
  }
}


bool
D878UVCodeplug::encode(Config *config, bool update)
{
  // Encode radio IDs
  radioid_t *radio_ids = (radioid_t *)data(ADDR_RADIOIDS);
  radio_ids[0].setId(config->id());
  radio_ids[0].setName(config->name());

  // Encode general config
  general_settings_t *settings = (general_settings_t *)data(ADDR_GENERAL_CONFIG);
  settings->setIntroLine1(config->introLine1());
  settings->setIntroLine2(config->introLine2());

  // Encode channels
  for (int i=0; i<config->channelList()->count(); i++) {
    // enable channel
    uint16_t bank = i/128, idx = i%128;
    channel_t *ch = (channel_t *)data(CHANNEL_BANK_0
                                      + bank*CHANNEL_BANK_OFFSET
                                      + idx*sizeof(channel_t));
    ch->fromChannelObj(config->channelList()->channel(i), config);
  }

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
  std::sort(contact_id_map.begin(), contact_id_map.end(),
            [](const contact_map_t &a, const contact_map_t &b) {
    return a.ID() < b.ID();
  });
  for (int i=0; i<contact_id_map.size(); i++) {
    ((contact_map_t *)data(CONTACT_ID_MAP))[i] = contact_id_map[i];
  }

  // Encode RX group-lists
  for (int i=0; i<config->rxGroupLists()->count(); i++) {
    grouplist_t *grp = (grouplist_t *)data(ADDR_RXGRP_0 + i*RXGRP_OFFSET);
    grp->fromGroupListObj(config->rxGroupLists()->list(i), config);
  }

  // Encode zones
  uint zidx = 0;
  for (int i=0; i<config->zones()->count(); i++) {
    // Clear name and channel list
    uint8_t  *name     = (uint8_t *)data(ADDR_ZONE_NAME + zidx*ZONE_NAME_OFFSET);
    uint16_t *channels = (uint16_t *)data(ADDR_ZONE + zidx*ZONE_OFFSET);
    memset(name, 0, ZONE_NAME_OFFSET);
    memset(channels, 0xff, ZONE_OFFSET);
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
    memset(name, 0, ZONE_NAME_OFFSET);
    memset(channels, 0xff, ZONE_OFFSET);
    encode_ascii(name, config->zones()->zone(i)->name()+" B", 16, 0);
    // Handle list B
    for (int j=0; j<config->zones()->zone(i)->B()->count(); j++) {
      channels[j] = qToLittleEndian(
            config->channelList()->indexOf(
              config->zones()->zone(i)->B()->channel(j)));
    }
    zidx++;
  }

  // Encode scan lists
  for (int i=0; i<config->scanlists()->count(); i++) {
    uint8_t bank = i/NUM_SCANLISTS_PER_BANK, idx = i%NUM_SCANLISTS_PER_BANK;
    scanlist_t *scan = (scanlist_t *)data(
          SCAN_LIST_BANK_0 + bank*SCAN_LIST_BANK_OFFSET + idx*SCAN_LIST_OFFSET);
    scan->fromScanListObj(config->scanlists()->scanlist(i), config);
  }

  // Encode GPS systems
  /*
  gps_systems_t *gps = (gps_systems_t *)data(ADDR_GPS_SETTING);
  gps->fromGPSSystems(config);
  if (0 < config->gpsSystems()->count()) {
    aprs_setting_t *aprs = (aprs_setting_t *)data(ADDR_APRS_SETTING);
    aprs->setAutoTxInterval(config->gpsSystems()->gpsSystem(0)->period());
  }*/

  return true;
}

bool
D878UVCodeplug::decode(Config *config)
{
  // Maps code-plug indices to objects
  CodeplugContext ctx(config);

  // Find a valid RadioID
  uint8_t *radio_ids = data(ADDR_RADIOIDS);
  for (uint16_t i=0; i<NUM_RADIOIDS; i++) {
    radioid_t *id = (radioid_t *)(radio_ids+i*sizeof(radioid_t));
    if (id->isValid()) {
      config->setId(id->getId());
      config->setName(id->getName());
      break;
    }
  }

  // Get intro lines
  general_settings_t *settings = (general_settings_t *)data(ADDR_GENERAL_CONFIG);
  config->setIntroLine1(settings->getIntroLine1());
  config->setIntroLine2(settings->getIntroLine2());

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

  // Create zones
  uint8_t *zone_bitmap = data(ZONE_BITMAPS);
  for (uint16_t i=0; i<NUM_ZONES; i++) {
    // Check if zone is enabled:
    uint16_t bit = i%8, byte = i/8;
    if (0 == (zone_bitmap[byte]>>bit))
      continue;
    // If enabled, create zone with name
    Zone *zone = new Zone(decode_ascii(data(ADDR_ZONE_NAME+i*ZONE_NAME_OFFSET), 16, 0));
    // add to config
    config->zones()->addZone(zone);
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
      zone->A()->addChannel(ctx.getChannel(cidx));
    }
  }

  // Create scan lists
  uint8_t *scanlist_bitmap = data(SCAN_BITMAP);
  for (uint i=0; i<NUM_SCAN_LISTS; i++) {
    uint8_t byte=i/8, bit=i%8;
    if (0 == (scanlist_bitmap[byte]>>bit))
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

  // Create GPS systems
  gps_systems_t *gps_systems = (gps_systems_t *)data(ADDR_GPS_SETTING);
  for (int i=0; i<NUM_GPS_SYSTEMS; i++) {
    if (! gps_systems->isValid(i))
      continue;
    GPSSystem *sys = gps_systems->toGPSSystemObj(i);
    if (sys)
      logDebug() << "Create GPS sys '" << sys->name() << "' at idx " << i << ".";
    ctx.addGPSSystem(sys, i);
  }

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

  // Link GPS systems
  for (int i=0; i<NUM_GPS_SYSTEMS; i++) {
    if (! gps_systems->isValid(i))
      continue;
    gps_systems->linkGPSSystem(i, ctx.getGPSSystem(i), ctx);
  }

  return true;
}
