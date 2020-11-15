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
#define CHANNEL_BANK_SIZE         0x00020000
#define CHANNEL_BANK_31           0x00fc0000
#define CHANNEL_BANK_31_SIZE      0x00000880
#define CHANNEL_BANK_OFFSET       0x00040000
#define CHANNEL_BITMAP            0x024c1500
#define CHANNEL_BITMAP_SIZE       0x00000240

#define NUM_CONTACTS              10000      // total number of contacts
#define NUM_CONTACT_BANKS         313        // number of contact banks
#define CONTACT_BANK_0            0x02680000 // first bank of 32 contacts
#define CONTACT_BANK_SIZE         0x00000c80 // size of 32 contacts
#define CONTACT_BANK_312          0x02772f80 // last bank of 16 contacts
#define CONTACT_BANK_312_SIZE     0x00000640 // size of last contact bank
#define CONTACTS_BITMAP           0x02640000 // address of contact bitmap
#define CONTACTS_BITMAP_SIZE      0x00000500 // size of contact bitmap

#define NUM_RXGRP                 250
#define ADDR_RXGRP_0              0x02980000
#define RXGRP_SIZE                0x00000140
#define RXGRP_OFFSET              0x00000200

#define NUM_ZONES                 250
#define NUM_CH_PER_ZONE           250
#define ADDR_ZONE                 0x01000000
#define ZONE_SIZE                 0x00000200
#define ZONE_OFFSET               0x00000200
#define ADDR_ZONE_NAME            0x02540000
#define ZONE_NAME_SIZE            0x00000010
#define ZONE_NAME_OFFSET          0x00000020
#define ZONE_BITMAPS              0x024c1300
#define ZONE_BITMAPS_SIZE         0x00000040
#define ZONE_A_BITMAP             0x024c1300
#define ZONE_A_BITMAP_SIZE        0x00000020
#define ZONE_B_BITMAP             0x024c1320
#define ZONE_B_BITMAP_SIZE        0x00000020

#define NUM_RADIOIDS              250
#define ADDR_RADIOIDS             0x02580000
#define RADIOIDS_SIZE             0x00001f40

#define NUM_SCAN_LISTS            250
#define SCAN_BANK_0               0x01080000 // Scanlist 0-31.
#define SCAN_BANK_OFFSET          0x00080000 // Offset to next bank.
#define SCAN_LIST_SIZE            0x000000c0 // Size of scan-list.
#define SCAN_LIST_OFFSET          0x00000200 // Offset to next scan-list within bank.
#define SCAN_BITMAP               0x024c1340 // Address of scan-list bitmap.
#define SCAN_BITMAP_SIZE          0x00000040 // Size of scan-list bitmap.

#define ADDR_GENERAL_CONFIG       0x02500000
#define GENERAL_CONFIG_SIZE       0x00000640

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
  memset(this, 0xff, sizeof(D878UVCodeplug::channel_t));
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
  Channel::Power power = Channel::LowPower;
  switch ((channel_t::Power) this->power) {
  case POWER_LOW:
  case POWER_MIDDLE:
    power = Channel::LowPower;
    break;
  case POWER_HIGH:
  case POWER_TURBO:
    power = Channel::HighPower;
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
  if (MODE_DIGITAL == channel_mode) {
    DigitalChannel *dc = c->as<DigitalChannel>();
    if (nullptr == dc)
      return false;

    uint16_t conIdx = qFromLittleEndian(contact_index);
    if (ctx.hasDigitalContact(conIdx))
      dc->setTXContact(ctx.getDigitalContact(conIdx));

    if (ctx.hasGroupList(group_list_index))
      dc->setRXGroupList(ctx.getGroupList(group_list_index));
  }

  /// @bug Complete D878UV channel linking.
  return true;
}

void
D878UVCodeplug::channel_t::fromChannelObj(const Channel *c, const Config *conf) {
  // Pack common channel config
  // set channel name
  setName(c->name());
  // set rx and tx frequencies
  setRXFrequency(c->rxFrequency());
  setTXFrequency(c->txFrequency());
  // set power
  power = (Channel::LowPower == c->power()) ? POWER_LOW : POWER_HIGH;
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
    channel_mode = MODE_DIGITAL;
    // pack digital channel config.
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
    if (nullptr == dc->txContact())
      contact_index = 0xffff;
    else
      contact_index = conf->contacts()->indexOfDigital(dc->txContact());
    // link RX group list
    if (nullptr == dc->rxGroupList())
      group_list_index = 0xff;
    else
      group_list_index = conf->rxGroupLists()->indexOf(dc->rxGroupList());
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
  memset(this, 0xff, sizeof(D878UVCodeplug::grouplist_t));
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
    if ((0xffffffff == idx) || (! ctx.hasDigitalContact(idx)))
      continue;
    lst->addContact(ctx.getDigitalContact(idx));
  }
  return true;
}

void
D878UVCodeplug::grouplist_t::fromGroupListObj(const RXGroupList *lst, const Config *conf) {
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
  power_on = PWON_DEFAULT;
  _unused7 = 0;
  memset(intro_line1, 0, sizeof(intro_line2));
  memset(intro_line2, 0, sizeof(intro_line2));
  memset(password, 0, sizeof(password));
  memset(_unused630, 0, sizeof(_unused630));
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
 * Implementation of D878UVCodeplug
 * ******************************************************************************************** */
D878UVCodeplug::D878UVCodeplug(QObject *parent)
  : CodePlug(parent)
{
  addImage("Anytone AT-D878UV Codeplug");

  // Channel bitmap
  image(0).addElement(CHANNEL_BITMAP, CHANNEL_BITMAP_SIZE);
  // Contacts bitmap
  image(0).addElement(CONTACTS_BITMAP, CONTACTS_BITMAP_SIZE);
  // Zone bitmap
  image(0).addElement(ZONE_BITMAPS, ZONE_BITMAPS_SIZE);
  // All readio IDs
  image(0).addElement(ADDR_RADIOIDS, RADIOIDS_SIZE);
  // General config
  image(0).addElement(ADDR_GENERAL_CONFIG, GENERAL_CONFIG_SIZE);

  clear();
}

void
D878UVCodeplug::clear() {
  for (int i=0; i<image(0).numElements(); i++) {
    memset(image(0).element(i).data().data(), 0, image(0).element(i).size());
  }
}

void
D878UVCodeplug::allocateFromBitmaps() {
  // Check channel bitmap
  uint8_t *channel_bitmap = data(CHANNEL_BITMAP);
  for (uint16_t i=0; i<NUM_CHANNELS; i++) {
    // Get byte and bit for channel, as well as bank of channel
    uint16_t bit = i%8, byte = i/8, bank = i/128;
    if (0 == ((channel_bitmap[byte]>>bit) & 0x01))
      continue;
    uint32_t addr = CHANNEL_BANK_0+bank*CHANNEL_BANK_OFFSET;
    if (nullptr == data(addr, 0)) {
      if (31 == bank)
        image(0).addElement(addr, CHANNEL_BANK_31_SIZE);
      else
        image(0).addElement(addr, CHANNEL_BANK_SIZE);
    }
  }

  // Check contacts bitmap
  uint8_t *contact_bitmap = data(CONTACTS_BITMAP);
  for (uint16_t i=0; i<NUM_CONTACTS; i++) {
    // Get byte and bit for contact, as well as bank of contact
    uint16_t bit = i%8, byte = i/8, bank = i/128;
    if (1 == ((contact_bitmap[byte]>>bit) & 0x01))
      continue;
    uint32_t addr = CONTACT_BANK_0+bank*CONTACT_BANK_SIZE;
    if (nullptr == data(addr, 0)) {
      if (312 == bank)
        image(0).addElement(addr, CONTACT_BANK_312_SIZE);
      else
        image(0).addElement(addr, CONTACT_BANK_SIZE);
    }
  }

  // Allocate all group lists (no index table for RX groups! WTF!)
  for (uint16_t i=0; i<NUM_RXGRP; i++) {
    image(0).addElement(ADDR_RXGRP_0+i*RXGRP_OFFSET, RXGRP_SIZE);
  }

  // Allocate only valid zones
  uint8_t *zone_bitmap = data(ZONE_BITMAPS);
  for (uint16_t i=0; i<NUM_ZONES; i++) {
    // Get byte and bit for zone
    uint16_t bit = i%8, byte = i/8;
    // If valid ...
    if (0 == ((zone_bitmap[byte]>>bit) & 0x01))
      continue;
    // Allocate zone name and zone itself
    image(0).addElement(ADDR_ZONE+i*ZONE_OFFSET, ZONE_SIZE);
  }
  // but allocate all zone names (only 8k)
  image(0).addElement(ADDR_ZONE_NAME, NUM_ZONES*ZONE_NAME_OFFSET);
}

void
D878UVCodeplug::setBitmaps(Config *config)
{
  // Mark valid channels (set bit)
  uint8_t *channel_bitmap = data(CHANNEL_BITMAP);
  memset(channel_bitmap, 0x00, CHANNEL_BITMAP_SIZE);
  for (int i=0; i<config->channelList()->count(); i++) {
    uint16_t bit = i%8, byte = i/8;
    channel_bitmap[byte] |= (1 << bit);
  }

  // Mark valid contacts (clear bit)
  uint8_t *contact_bitmap = data(CONTACTS_BITMAP);
  memset(contact_bitmap, 0xff, CONTACTS_BITMAP_SIZE);
  for (int i=0; i<config->contacts()->digitalCount(); i++) {
    uint16_t bit = i%8, byte = i/8;
    contact_bitmap[byte] &= ~(1 << bit);
  }

  // Mark valid zones (set bits)
  uint8_t *zone_a_bitmap = data(ZONE_A_BITMAP);
  uint8_t *zone_b_bitmap = data(ZONE_A_BITMAP);
  memset(zone_a_bitmap, 0, ZONE_BITMAPS_SIZE);
  for (int i=0; i<config->zones()->count(); i++) {
    uint16_t bit = i%8, byte = i/8;
    zone_a_bitmap[byte] |= (1 << bit);
    zone_b_bitmap[byte] |= (1 << bit);
  }
}


bool
D878UVCodeplug::encode(Config *config)
{
  // Encode radio IDs
  radioid_t *radio_ids = (radioid_t *)data(ADDR_RADIOIDS);
  for (int i=1; i<NUM_RADIOIDS; i++)
    radio_ids[i].clear();
  radio_ids[0].setId(config->id());
  radio_ids[0].setName(config->name());

  // Encode general config
  general_settings_t *settings = (general_settings_t *)data(ADDR_GENERAL_CONFIG);
  settings->setIntroLine1(config->introLine1());
  settings->setIntroLine2(config->introLine2());

  // Encode channels
  for (int i=0; i<config->channelList()->count(); i++) {
    uint16_t bank = i/128, idx = i%128;
    channel_t *ch = (channel_t *)data(CHANNEL_BANK_0
                                      +bank*CHANNEL_BANK_OFFSET
                                      +idx*sizeof(channel_t));
    ch->fromChannelObj(config->channelList()->channel(i), config);
  }

  // Encode contacts
  for (int i=0; i<config->contacts()->digitalCount(); i++) {
    contact_t *con = (contact_t *)data(CONTACT_BANK_0+i*sizeof(contact_t));
    con->fromContactObj(config->contacts()->digitalContact(i));
  }

  // Encode RX group-lists
  for (int i=0; i<NUM_RXGRP; i++) {
    grouplist_t *grp = (grouplist_t *)data(ADDR_RXGRP_0+i*RXGRP_OFFSET);
    grp->clear();
    if (i<config->rxGroupLists()->count())
      grp->fromGroupListObj(config->rxGroupLists()->list(i), config);
  }

  for (int i=0; i<config->zones()->count(); i++) {
    uint8_t  *name     = (uint8_t *)data(ADDR_ZONE_NAME+i*ZONE_NAME_OFFSET);
    uint16_t *channels = (uint16_t *)data(ADDR_ZONE+i*ZONE_OFFSET);
    encode_ascii(name, config->zones()->zone(i)->name(), 16, 0);
    int nch_a = config->zones()->zone(i)->A()->count();
    int nch_b = config->zones()->zone(i)->B()->count();
    for (int j=0; j<NUM_CH_PER_ZONE; j++) {
      if (j < nch_a) {
        channels[j] = qToLittleEndian(
              config->channelList()->indexOf(
                config->zones()->zone(i)->A()->channel(j)));
      } else if (j < (nch_a+nch_b)) {
        channels[j] = qToLittleEndian(
              config->channelList()->indexOf(
                config->zones()->zone(i)->B()->channel(j-nch_a)));
      } else {
        channels[j] = 0xffff;
      }
    }
  }

  /// @bug Implement scan-list D878UV code-plug encoding.
  /// @bug Implement analog contact D878UV code-plug encoding.
  /// @bug Implement GPS D878UV code-plug encoding.
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

  // Create contacts
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
  for (uint16_t i=0; i<NUM_RXGRP; i++) {
    grouplist_t *grp = (grouplist_t *)data(ADDR_RXGRP_0+i*RXGRP_OFFSET);
    if (! grp->isValid())
      continue;
    if (RXGroupList *obj = grp->toGroupListObj()) {
      ctx.addGroupList(obj, i);
      grp->linkGroupList(obj, ctx);
    }
  }

  // Create zones
  uint8_t *zone_a_bitmap = data(ZONE_A_BITMAP);
  //uint8_t *zone_b_bitmap = data(ZONE_A_BITMAP);
  for (uint16_t i=0; i<NUM_ZONES; i++) {
    // Check if zone is enabled:
    uint16_t bit = i%8, byte = i/8;
    bool has_a = ((zone_a_bitmap[byte]>>bit)&0x01);
    //bool has_b = ((zone_b_bitmap[byte]>>bit)&0x01);
    if (! has_a)
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

  // Apply general settings
  general_settings_t *settings = (general_settings_t *)data(ADDR_GENERAL_CONFIG);
  config->setIntroLine1(settings->getIntroLine1());
  config->setIntroLine2(settings->getIntroLine2());

  /// @bug Implement scan-list D878UV code-plug decoding.
  /// @bug Implement analog contact D878UV code-plug decoding.
  /// @bug Implement GPS D878UV code-plug decoding.

  return true;
}
