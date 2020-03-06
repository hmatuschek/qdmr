#include "gd77_codeplug.hh"
#include "config.hh"
#include "channel.hh"
#include "utils.hh"
#include <QDateTime>


#define OFFSET_TIMESTMP     0x00088
#define OFFSET_SETTINGS     0x000e0
#define OFFSET_MSGTAB       0x00128
#define OFFSET_SCANTAB      0x01790
#define OFFSET_BANK_0       0x03780 // Channels 1-128
#define OFFSET_INTRO        0x07540
#define OFFSET_ZONETAB      0x08010
#define OFFSET_BANK_1       0x0b1b0 // Channels 129-1024
#define OFFSET_CONTACTS     0x17620
#define OFFSET_GROUPTAB     0x1d620

#define GET_MSGTAB()        ((msgtab_t*) &radio_mem[OFFSET_MSGTAB])

/* ******************************************************************************************** *
 * Implementation of GD77Codeplug::channel_t
 * ******************************************************************************************** */
bool
GD77Codeplug::channel_t::isValid() const {
  // Channel is enabled/disabled at channel bank
  return true;
}

void
GD77Codeplug::channel_t::clear() {
  memset(name, 0xff, 16);
  _unused25              = 0;
  _unused30              = 0x50;
  _unused36              = 0;
  tx_signaling_syst      = 0;
  _unused38              = 0;
  rx_signaling_syst      = 0;
  _unused40              = 0x16;
  privacy_group          = PRIVGR_NONE;
  emergency_system_index = 0;
  _unused48              = 0;
  emergency_alarm_ack    = 0;
  data_call_conf         = 0;
  private_call_conf      = 0;
  _unused49_1            = 0;
  privacy                = 0;
  _unused49_5            = 0;
  _unused49_7            = 0;
  dcdm                   = 0;
  _unused50_1            = 0;
  _unused50_6            = 0;
  squelch                = SQ_NORMAL;
  bandwidth              = BW_12_5_KHZ;
  talkaround             = 0;
  _unused51_4            = 0;
  vox                    = 0;
  _unused52              = 0;
}

double
GD77Codeplug::channel_t::getRXFrequency() const {
  return decode_frequency(rx_frequency);
}
void
GD77Codeplug::channel_t::setRXFrequency(double freq) {
  rx_frequency = encode_frequency(freq);
}

double
GD77Codeplug::channel_t::getTXFrequency() const {
  return decode_frequency(tx_frequency);
}
void
GD77Codeplug::channel_t::setTXFrequency(double freq) {
  tx_frequency = encode_frequency(freq);
}

QString
GD77Codeplug::channel_t::getName() const {
  return decode_ascii(name, 16, 0xff);
}
void
GD77Codeplug::channel_t::setName(const QString &n) {
  encode_ascii(name, n, 16, 0xff);
}

float
GD77Codeplug::channel_t::getRXTone() const {
  return decode_ctcss_tone_table(ctcss_dcs_receive);
}
void
GD77Codeplug::channel_t::setRXTone(float tone) {
  ctcss_dcs_receive = encode_ctcss_tone_table(tone);
}

float
GD77Codeplug::channel_t::getTXTone() const {
  return decode_ctcss_tone_table(ctcss_dcs_transmit);
}
void
GD77Codeplug::channel_t::setTXTone(float tone) {
  ctcss_dcs_transmit = encode_ctcss_tone_table(tone);
}

Channel *
GD77Codeplug::channel_t::toChannelObj() const {
  QString name = getName();
  double rxF = getRXFrequency();
  double txF = getTXFrequency();
  Channel::Power pwr = (POWER_HIGH == power) ? Channel::HighPower : Channel::LowPower;
  uint timeout = tot*15;
  bool rxOnly = rx_only;
  if (MODE_ANALOG == channel_mode) {
    AnalogChannel::Admit admit;
    switch (admit_criteria) {
      case ADMIT_ALWAYS: admit = AnalogChannel::AdmitNone; break;
      case ADMIT_CH_FREE: admit = AnalogChannel::AdmitFree; break;
      default:
        return nullptr;
    }
    AnalogChannel::Bandwidth bw = (BW_25_KHZ == bandwidth) ? AnalogChannel::BWWide : AnalogChannel::BWNarrow;
    return new AnalogChannel(
          name, rxF, txF, pwr, timeout, rxOnly, admit, squelch,  getRXTone(), getTXTone(),
          bw, nullptr);
  } else if(MODE_DIGITAL == channel_mode) {
    DigitalChannel::Admit admit;
    switch (admit_criteria) {
      case ADMIT_ALWAYS: admit = DigitalChannel::AdmitNone; break;
      case ADMIT_CH_FREE: admit = DigitalChannel::AdmitFree; break;
      case ADMIT_COLOR: admit = DigitalChannel::AdmitColorCode; break;
      default:
        return nullptr;
    }
    DigitalChannel::TimeSlot slot = repeater_slot2 ?
          DigitalChannel::TimeSlot2 : DigitalChannel::TimeSlot1;
    return new DigitalChannel(
          name, rxF, txF, pwr, timeout, rxOnly, admit, colorcode_rx, slot,
          nullptr, nullptr, nullptr, nullptr);
  }

  return nullptr;
}

bool
GD77Codeplug::channel_t::linkChannelObj(Channel *c, Config *conf) const {
  if (c->is<AnalogChannel>()) {
    AnalogChannel *ac = c->as<AnalogChannel>();
    if (scan_list_index && (conf->scanlists()->count() >= scan_list_index))
      ac->setScanList(conf->scanlists()->scanlist(scan_list_index-1));
  } else {
    DigitalChannel *dc = c->as<DigitalChannel>();
    if (scan_list_index && (conf->scanlists()->count() >= scan_list_index))
      dc->setScanList(conf->scanlists()->scanlist(scan_list_index-1));
    if (group_list_index && (conf->rxGroupLists()->count() >= group_list_index))
      dc->setRXGroupList(conf->rxGroupLists()->list(group_list_index-1));
    if (contact_name_index && (conf->contacts()->digitalCount() >= contact_name_index))
      dc->setTXContact(conf->contacts()->digitalContact(contact_name_index-1));
  }
  return true;
}

void
GD77Codeplug::channel_t::fromChannelObj(const Channel *c, const Config *conf) {
  clear();

  setName(c->name());
  setRXFrequency(c->rxFrequency());
  setTXFrequency(c->txFrequency());

  power = (Channel::HighPower == c->power()) ? POWER_HIGH : POWER_LOW;
  tot = c->txTimeout()/15;
  rx_only = c->rxOnly() ? 1 : 0;
  bandwidth = BW_12_5_KHZ;

  if (c->is<AnalogChannel>()) {
    const AnalogChannel *ac = c->as<const AnalogChannel>();
    channel_mode = MODE_ANALOG;
    switch (ac->admit()) {
      case AnalogChannel::AdmitNone: admit_criteria = ADMIT_ALWAYS; break;
      case AnalogChannel::AdmitFree: admit_criteria = ADMIT_CH_FREE; break;
      default: admit_criteria = ADMIT_CH_FREE; break;
    }
    bandwidth = (AnalogChannel::BWWide == ac->bandwidth()) ? BW_25_KHZ : BW_12_5_KHZ;
    squelch = SQ_NORMAL; //ac->squelch();
    setRXTone(ac->rxTone());
    setTXTone(ac->txTone());
    scan_list_index = conf->scanlists()->indexOf(ac->scanList())+1;
  } else if (c->is<DigitalChannel>()) {
    const DigitalChannel *dc = c->as<const DigitalChannel>();
    channel_mode = MODE_DIGITAL;
    switch (dc->admit()) {
      case DigitalChannel::AdmitNone: admit_criteria = ADMIT_ALWAYS; break;
      case DigitalChannel::AdmitFree: admit_criteria = ADMIT_CH_FREE; break;
      case DigitalChannel::AdmitColorCode: admit_criteria = ADMIT_COLOR; break;
    }
    repeater_slot2 = (DigitalChannel::TimeSlot1 == dc->timeslot()) ? 0 : 1;
    colorcode_rx = colorcode_tx = dc->colorCode();
    scan_list_index = conf->scanlists()->indexOf(dc->scanList()) + 1;
    group_list_index = conf->rxGroupLists()->indexOf(dc->rxGroupList()) + 1;
    contact_name_index = conf->contacts()->indexOfDigital(dc->txContact()) + 1;
  }
}


/* ******************************************************************************************** *
 * Implementation of GD77Codeplug::grouplist_t
 * ******************************************************************************************** */
QString
GD77Codeplug::grouplist_t::getName() const {
  return decode_ascii(name, 16, 0xff);
}
void
GD77Codeplug::grouplist_t::setName(const QString &n) {
  encode_ascii(name, n, 16, 0xff);
}

RXGroupList *
GD77Codeplug::grouplist_t::toRXGroupListObj() {
  return new RXGroupList(getName());
}

bool
GD77Codeplug::grouplist_t::linkRXGroupListObj(RXGroupList *lst, const Config *conf) const {
  for (int i=0; (i<32) && member[i]; i++)
    lst->addContact(conf->contacts()->digitalContact(member[i]-1));
  return false;
}

void
GD77Codeplug::grouplist_t::fromRXGroupListObj(const RXGroupList *lst, const Config *conf) {
  setName(lst->name());
  for (int i=0; i<32; i++) {
    if (i < lst->count())
      member[i] = conf->contacts()->indexOfDigital(lst->contact(i))+1;
    else
      member[i] = 0;
  }
}


/* ******************************************************************************************** *
 * Implementation of GD77Codeplug
 * ******************************************************************************************** */
GD77Codeplug::GD77Codeplug(QObject *parent)
  : CodePlug(parent)
{
  addImage("Radioddity GD77 Codeplug");
  image(0).addElement(0x00080, 0x07b80);
  image(0).addElement(0x08000, 0x16300);
}

bool
GD77Codeplug::encode(Config *config) {
  /// @bug Implement!
  return false;
}

bool
GD77Codeplug::decode(Config *config) {
  // Clear config object
  config->reset();

  /// @bug Implement!

  return false;
}
