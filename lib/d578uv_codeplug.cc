#include "d578uv_codeplug.hh"
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


#define NUM_CONTACTS              10000      // Total number of contacts
#define NUM_CONTACT_BANKS         2500       // Number of contact banks
#define CONTACTS_PER_BANK         4
#define CONTACT_BANK_0            0x02680000 // First bank of 4 contacts
#define CONTACT_BANK_SIZE         0x00000190 // Size of 4 contacts
#define CONTACT_INDEX_LIST        0x02600000 // Address of contact index list
#define CONTACTS_BITMAP           0x02640000 // Address of contact bitmap
#define CONTACTS_BITMAP_SIZE      0x00000500 // Size of contact bitmap
#define CONTACT_ID_MAP            0x04800000 // Address of ID->Contact index map
#define CONTACT_ID_ENTRY_SIZE     sizeof(contact_map_t) // Size of each map entry

#define ADDR_HOTKEY               0x025C0000 // Same address as D868UV::hotkey_settings_t
#define HOTKEY_SIZE               0x00000970 // Different size.

#define ADDR_UNKNOWN_SETTING_1    0x02BC0000 // Address of unknown settings
#define UNKNOWN_SETTING_1_SIZE    0x00000020 // Size of unknown settings.
#define ADDR_UNKNOWN_SETTING_2    0x02BC0C60 // Address of unknown settings
#define UNKNOWN_SETTING_2_SIZE    0x00000020 // Size of unknown settings.
#define ADDR_UNKNOWN_SETTING_3    0x02BC1000 // Address of unknown settings
#define UNKNOWN_SETTING_3_SIZE    0x00000060 // Size of unknown settings.


/* ******************************************************************************************** *
 * Implementation of D578UVCodeplug::channel_t
 * ******************************************************************************************** */
D578UVCodeplug::channel_t::channel_t() {
  clear();
}

void
D578UVCodeplug::channel_t::clear() {
  memset(this, 0, sizeof(D578UVCodeplug::channel_t));
  custom_ctcss = qToLittleEndian(0x09cf); // some value
  scan_list_index  = 0xff; // None
  group_list_index = 0xff; // None
  id_index = 0;
  squelch_mode = SQ_CARRIER;
  tx_permit = ADMIT_ALWAYS;
  // Enable BT hands free by default.
  bt_hands_free = 1;
}

bool
D578UVCodeplug::channel_t::isValid() const {
  return (0 != name[0]) && (0xff != name[0]);
}

double
D578UVCodeplug::channel_t::getRXFrequency() const {
  return decode_frequency(qFromBigEndian(rx_frequency));
}

void
D578UVCodeplug::channel_t::setRXFrequency(double f) {
  rx_frequency = qToBigEndian(encode_frequency(f));
}

double
D578UVCodeplug::channel_t::getTXFrequency() const {
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
D578UVCodeplug::channel_t::setTXFrequency(double f) {
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
D578UVCodeplug::channel_t::getName() const {
  return decode_ascii(name, 16, 0);
}

void
D578UVCodeplug::channel_t::setName(const QString &name) {
  encode_ascii(this->name, name, 16, 0);
}

Signaling::Code
D578UVCodeplug::channel_t::getRXTone() const {
  // If squelch is not SQ_TONE -> RX tone is ignored
  if (SQ_TONE != squelch_mode)
    return Signaling::SIGNALING_NONE;

  if (rx_ctcss && (ctcss_receive < 52))
    return ctcss_num2code(ctcss_receive);
  else if (rx_dcs && (qFromLittleEndian(dcs_receive) < 512))
    return Signaling::fromDCSNumber(dec_to_oct(qFromLittleEndian(dcs_receive)), false);
  else if (rx_dcs && (qFromLittleEndian(dcs_receive) >= 512))
    return Signaling::fromDCSNumber(dec_to_oct(qFromLittleEndian(dcs_receive)-512), true);
  return Signaling::SIGNALING_NONE;
}

void
D578UVCodeplug::channel_t::setRXTone(Signaling::Code code) {
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
D578UVCodeplug::channel_t::getTXTone() const {
  if (tx_ctcss && (ctcss_transmit < 52))
    return ctcss_num2code(ctcss_transmit);
  else if (tx_dcs && (qFromLittleEndian(dcs_transmit) < 512))
    return Signaling::fromDCSNumber(dec_to_oct(qFromLittleEndian(dcs_transmit)), false);
  else if (tx_dcs && (qFromLittleEndian(dcs_transmit) >= 512))
    return Signaling::fromDCSNumber(dec_to_oct(qFromLittleEndian(dcs_transmit)-512), true);
  return Signaling::SIGNALING_NONE;
}

void
D578UVCodeplug::channel_t::setTXTone(Signaling::Code code) {
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
D578UVCodeplug::channel_t::toChannelObj() const {
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
    if (MODE_MIXED_D_A == channel_mode)
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
    case ADMIT_CC_SAME:
    case ADMIT_CC_DIFF:
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
D578UVCodeplug::channel_t::linkChannelObj(Channel *c, const CodeplugContext &ctx) const {
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
    if ((APRS_REPORT_DIGITAL == aprs_report) && ctx.hasGPSSystem(gps_system))
      dc->setPosSystem(ctx.getGPSSystem(gps_system));
    // Link APRS system if one is defined
    //  There can only be one active APRS system, hence the index is fixed to one.
    if ((APRS_REPORT_ANALOG == aprs_report) && ctx.hasAPRSSystem(0))
      dc->setPosSystem(ctx.getAPRSSystem(0));

    // If roaming is not disabled -> link to default roaming zone
    if (0 == excl_from_roaming)
      dc->setRoaming(DefaultRoamingZone::get());

    // Link radio ID
    dc->setRadioId(ctx.getRadioId(id_index));

  } else if (MODE_ANALOG == channel_mode) {
    // If channel is analog
    AnalogChannel *ac = c->as<AnalogChannel>();
    if (nullptr == ac)
      return false;

    // Link APRS system if one is defined
    //  There can only be one active APRS system, hence the index is fixed to one.
    if ((APRS_REPORT_ANALOG == aprs_report) && ctx.hasAPRSSystem(0))
      ac->setAPRSSystem(ctx.getAPRSSystem(0));
  }

  // For both, analog and digital channels:

  // If channel has scan list
  if ((0xff != scan_list_index) && ctx.hasScanList(scan_list_index))
    c->setScanList(ctx.getScanList(scan_list_index));

  return true;
}

void
D578UVCodeplug::channel_t::fromChannelObj(const Channel *c, const Config *conf) {
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
    squelch_mode = (Signaling::SIGNALING_NONE == ac->rxTone()) ? SQ_CARRIER : SQ_TONE;
    setRXTone(ac->rxTone());
    setTXTone(ac->txTone());
    // set bandwidth
    bandwidth = (AnalogChannel::BWNarrow == ac->bandwidth()) ? BW_12_5_KHZ : BW_25_KHZ;
    // Set APRS system
    rx_gps = 0;
    if (nullptr != ac->aprsSystem()) {
      aprs_report = APRS_REPORT_ANALOG;
      rx_gps = 1;
    }
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
    rx_gps = 0;
    if (dc->posSystem() && dc->posSystem()->is<GPSSystem>()) {
      aprs_report = APRS_REPORT_DIGITAL;
      gps_system = conf->posSystems()->indexOfGPSSys(dc->posSystem()->as<GPSSystem>());
      rx_gps = 1;
    } else if (dc->posSystem() && dc->posSystem()->is<APRSSystem>()) {
      aprs_report = APRS_REPORT_ANALOG;
      rx_gps = 1;
    }
    // Set radio ID
    if (nullptr != dc->radioId())
      id_index = conf->radioIDs()->indexOf(dc->radioId());
    else
      id_index = 0;
  }
}


/* ******************************************************************************************** *
 * Implementation of D578UVCodeplug
 * ******************************************************************************************** */
D578UVCodeplug::D578UVCodeplug(QObject *parent)
  : D878UVCodeplug(parent)
{
  // pass...
}

void
D578UVCodeplug::allocateUpdated() {
  D868UVCodeplug::allocateUpdated();

  image(0).addElement(ADDR_UNKNOWN_SETTING_1, UNKNOWN_SETTING_1_SIZE);
  image(0).addElement(ADDR_UNKNOWN_SETTING_2, UNKNOWN_SETTING_2_SIZE);
  image(0).addElement(ADDR_UNKNOWN_SETTING_3, UNKNOWN_SETTING_3_SIZE);
}

void
D578UVCodeplug::allocateHotKeySettings() {
  image(0).addElement(ADDR_HOTKEY, HOTKEY_SIZE);
}

bool
D578UVCodeplug::encodeChannels(Config *config, const Flags &flags) {
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


void
D578UVCodeplug::allocateContacts() {
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
D578UVCodeplug::encodeContacts(Config *config, const Flags &flags) {
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


