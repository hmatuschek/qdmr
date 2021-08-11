#include "rd5r_codeplug.hh"
#include "config.hh"
#include "channel.hh"
#include "utils.hh"
#include "logger.hh"
#include <QDateTime>


#define OFFSET_TIMESTMP     0x00088
#define OFFSET_SETTINGS     0x000e0
#define OFFSET_BUTTONS      0x00108
#define OFFSET_MSGTAB       0x00128
#define OFFSET_CONTACTS     0x01788
#define OFFSET_DTMF         0x02f88
#define OFFSET_BANK_0       0x03780 // Channels 1-128
#define OFFSET_BOOT         0x07518
#define OFFSET_MENU         0x07538
#define OFFSET_INTRO        0x07540
#define OFFSET_VFO          0x07590
#define OFFSET_ZONETAB      0x08010
#define OFFSET_BANK_1       0x0b1b0 // Channels 129-1024
#define OFFSET_SCANTAB      0x17620
#define OFFSET_GROUPTAB     0x1d620

#define GET_MSGTAB()        ((msgtab_t*) &_radio_mem[OFFSET_MSGTAB])


/* ******************************************************************************************** *
 * Implementation of RD5RCodeplug::channel_t
 * ******************************************************************************************** */
RD5RCodeplug::channel_t::channel_t() {
  clear();
}

bool
RD5RCodeplug::channel_t::isValid() const {
  // Channel is enabled/disabled at channel bank
  return true;
}

void
RD5RCodeplug::channel_t::clear() {
  memset(name, 0xff, 16);
  _unused25              = 0;
  _unused30              = 0x50;
  ctcss_dcs_receive      = 0xffff;
  ctcss_dcs_transmit     = 0xffff;
  _unused36              = 0;
  tx_signaling_syst      = 0;
  _unused38              = 0;
  rx_signaling_syst      = 0;
  _unused40              = 0x16;
  privacy_group          = PRIVGR_NONE;
  emergency_system_index = 0;
  contact_name_index     = 0;
  _unused48_0            = 0;
  emergency_alarm_ack    = 0;
  data_call_conf         = 0;
  private_call_conf      = 0;
  _unused49_1            = 0;
  privacy                = 0;
  _unused49_5            = 0;
  _unused49_7            = 0;
  dcdm                   = 0;
  _unused50_1            = 0;
  non_ste_frequency      = 0;
  _unused50_6            = 0;
  _unused51_0            = 0;
  talkaround             = 0;
  _unused51_4            = 0;
  vox                    = 0;
  memset(_unused52, 0, 3);
}

double
RD5RCodeplug::channel_t::getRXFrequency() const {
  return decode_frequency(rx_frequency);
}
void
RD5RCodeplug::channel_t::setRXFrequency(double freq) {
  rx_frequency = encode_frequency(freq);
}

double
RD5RCodeplug::channel_t::getTXFrequency() const {
  return decode_frequency(tx_frequency);
}
void
RD5RCodeplug::channel_t::setTXFrequency(double freq) {
  tx_frequency = encode_frequency(freq);
}

QString
RD5RCodeplug::channel_t::getName() const {
  return decode_ascii(name, 16, 0xff);
}
void
RD5RCodeplug::channel_t::setName(const QString &n) {
  encode_ascii(name, n, 16, 0xff);
}

Signaling::Code
RD5RCodeplug::channel_t::getRXTone() const {
  return decode_ctcss_tone_table(ctcss_dcs_receive);
}
void
RD5RCodeplug::channel_t::setRXTone(Signaling::Code code) {
  ctcss_dcs_receive = encode_ctcss_tone_table(code);
}

Signaling::Code
RD5RCodeplug::channel_t::getTXTone() const {
  return decode_ctcss_tone_table(ctcss_dcs_transmit);
}
void
RD5RCodeplug::channel_t::setTXTone(Signaling::Code code) {
  ctcss_dcs_transmit = encode_ctcss_tone_table(code);
}

Channel *
RD5RCodeplug::channel_t::toChannelObj() const {
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
        admit = AnalogChannel::AdmitFree;
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
      case ADMIT_COLOR:
      default:
        admit = DigitalChannel::AdmitColorCode; break;
    }
    DigitalChannel::TimeSlot slot = repeater_slot2 ?
          DigitalChannel::TimeSlot2 : DigitalChannel::TimeSlot1;
    return new DigitalChannel(
          name, rxF, txF, pwr, timeout, rxOnly, admit, colorcode_rx, slot,
          nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
  }

  return nullptr;
}

bool
RD5RCodeplug::channel_t::linkChannelObj(Channel *c, const CodeplugContext &ctx) const
{
  if (c->is<AnalogChannel>()) {
    AnalogChannel *ac = c->as<AnalogChannel>();
    if (scan_list_index && ctx.hasScanList(scan_list_index))
      ac->setScanList(ctx.getScanList(scan_list_index));
  } else {
    DigitalChannel *dc = c->as<DigitalChannel>();
    if (scan_list_index && ctx.hasScanList(scan_list_index))
      dc->setScanList(ctx.getScanList(scan_list_index));
    if (group_list_index && ctx.hasGroupList(group_list_index))
      dc->setRXGroupList(ctx.getGroupList(group_list_index));
    if (contact_name_index && ctx.hasDigitalContact(contact_name_index))
      dc->setTXContact(ctx.getDigitalContact(contact_name_index));
  }
  return true;
}

void
RD5RCodeplug::channel_t::fromChannelObj(const Channel *c, const Config *conf) {
  setName(c->name());
  setRXFrequency(c->rxFrequency());
  setTXFrequency(c->txFrequency());

  // encode power setting
  switch (c->power()) {
  case Channel::MaxPower:
  case Channel::HighPower:
    power = POWER_HIGH;
    break;
  case Channel::MidPower:
  case Channel::LowPower:
  case Channel::MinPower:
    power = POWER_LOW;
    break;
  }

  tot = c->txTimeout()/15;
  rx_only = c->rxOnly() ? 1 : 0;
  if (c->is<AnalogChannel>()) {
    const AnalogChannel *ac = c->as<const AnalogChannel>();
    channel_mode = MODE_ANALOG;
    switch (ac->admit()) {
      case AnalogChannel::AdmitNone: admit_criteria = ADMIT_ALWAYS; break;
      case AnalogChannel::AdmitFree: admit_criteria = ADMIT_CH_FREE; break;
      default: admit_criteria = ADMIT_CH_FREE; break;
    }
    bandwidth = (AnalogChannel::BWWide == ac->bandwidth()) ? BW_25_KHZ : BW_12_5_KHZ;
    squelch = ac->squelch();
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
 * Implementation of RD5RCodeplug::contact_t
 * ******************************************************************************************** */
RD5RCodeplug::contact_t::contact_t() {
  clear();
}

void
RD5RCodeplug::contact_t::clear() {
  memset(name, 0xff, 16);
  memset(id, 0x00, 4);
  type = receive_tone = ring_style = _unused23 = 0;
}

bool
RD5RCodeplug::contact_t::isValid() const {
  return 0xff != name[0];
}

uint32_t
RD5RCodeplug::contact_t::getId() const {
  return decode_dmr_id_bcd(id);
}
void
RD5RCodeplug::contact_t::setId(uint32_t num) {
  encode_dmr_id_bcd(id, num);
}

QString
RD5RCodeplug::contact_t::getName() const {
  return decode_ascii(name, 16, 0xff);
}
void
RD5RCodeplug::contact_t::setName(const QString &n) {
  encode_ascii(name, n, 16, 0xff);
}

DigitalContact *
RD5RCodeplug::contact_t::toContactObj() const {
  if (! isValid())
    return nullptr;
  QString name = getName();
  uint32_t id = getId();
  DigitalContact::Type ctype;
  switch (type) {
    case CALL_PRIVATE: ctype = DigitalContact::PrivateCall; break;
    case CALL_GROUP: ctype = DigitalContact::GroupCall; break;
    case CALL_ALL: ctype = DigitalContact::AllCall; break;
  }
  bool rxTone = (receive_tone && ring_style);
  return new DigitalContact(ctype, name, id, rxTone);
}

void
RD5RCodeplug::contact_t::fromContactObj(const DigitalContact *cont, const Config *conf) {
  Q_UNUSED(conf);
  setName(cont->name());
  setId(cont->number());
  switch (cont->type()) {
    case DigitalContact::PrivateCall: type = CALL_PRIVATE; break;
    case DigitalContact::GroupCall:   type = CALL_GROUP; break;
    case DigitalContact::AllCall:     type = CALL_ALL; break;
  }
  if (cont->rxTone())
    receive_tone = ring_style = 1;
}


/* ******************************************************************************************** *
 * Implementation of RD5RCodeplug::dtmf_contact_t
 * ******************************************************************************************** */
RD5RCodeplug::dtmf_contact_t::dtmf_contact_t() {
  clear();
}

void
RD5RCodeplug::dtmf_contact_t::clear() {
  memset(name, 0xff, 16);
  memset(number, 0xff, 16);
}

bool
RD5RCodeplug::dtmf_contact_t::isValid() const {
  return 0xff != name[0];
}

QString
RD5RCodeplug::dtmf_contact_t::getNumber() const {
  return decode_dtmf_bin(number, 16, 0xff);
}
bool
RD5RCodeplug::dtmf_contact_t::setNumber(const QString &num) {
  return encode_dtmf_bin(num, number, 16, 0xff);
}

QString
RD5RCodeplug::dtmf_contact_t::getName() const {
  return decode_ascii(name, 16, 0xff);
}
void
RD5RCodeplug::dtmf_contact_t::setName(const QString &n) {
  encode_ascii(name, n, 16, 0xff);
}

DTMFContact *
RD5RCodeplug::dtmf_contact_t::toContactObj() const {
  if (! isValid())
    return nullptr;
  return new DTMFContact(getName(), getNumber());
}

void
RD5RCodeplug::dtmf_contact_t::fromContactObj(const DTMFContact *cont, const Config *conf) {
  Q_UNUSED(conf);
  setName(cont->name());
  setNumber(cont->number());
}


/* ******************************************************************************************** *
 * Implementation of RD5RCodeplug::zone_t
 * ******************************************************************************************** */
RD5RCodeplug::zone_t::zone_t() {
  clear();
}

bool
RD5RCodeplug::zone_t::isValid() const {
  return 0xff != name[0];
}
void
RD5RCodeplug::zone_t::clear() {
  memset(name, 0xff, sizeof(name));
  memset(member, 0x00, sizeof(member));
}

QString
RD5RCodeplug::zone_t::getName() const {
  return decode_ascii(name, 16, 0xff);
}
void
RD5RCodeplug::zone_t::setName(const QString &n) {
  encode_ascii(name, n, 16, 0xff);
}

Zone *
RD5RCodeplug::zone_t::toZoneObj() const {
  if (! isValid())
    return nullptr;
  return new Zone(getName());
}

bool
RD5RCodeplug::zone_t::linkZoneObj(Zone *zone, const CodeplugContext &ctx, bool putInB) const {
  if (! isValid()) {
    logDebug() << "Cannot link zone: Zone is invalid.";
    return false;
  }

  for (int i=0; (i<16) && member[i]; i++) {
    if (ctx.hasChannel(member[i])) {
      if (! putInB)
        zone->A()->addChannel(ctx.getChannel(member[i]));
      else
        zone->B()->addChannel(ctx.getChannel(member[i]));
    } else {
      logWarn() << "While linking zone '" << zone->name() << "': " << i <<"-th channel index "
                << member[i] << " out of bounds.";
    }
  }
  return true;
}

void
RD5RCodeplug::zone_t::fromZoneObjA(const Zone *zone, const Config *conf) {
  if (zone->A()->count() && zone->B()->count())
    setName(zone->name() + " A");
  else
    setName(zone->name());

  for (int i=0; i<16; i++) {
    if (i < zone->A()->count())
      member[i] = conf->channelList()->indexOf(zone->A()->channel(i))+1;
    else
      member[i] = 0;
  }
}

void
RD5RCodeplug::zone_t::fromZoneObjB(const Zone *zone, const Config *conf) {
  if (zone->A()->count() && zone->B()->count())
    setName(zone->name() + " B");
  else
    setName(zone->name());

  for (int i=0; i<16; i++) {
    if (i < zone->B()->count())
      member[i] = conf->channelList()->indexOf(zone->B()->channel(i))+1;
    else
      member[i] = 0;
  }
}


/* ******************************************************************************************** *
 * Implementation of RD5RCodeplug::grouplist_t
 * ******************************************************************************************** */
RD5RCodeplug::grouplist_t::grouplist_t() {
  clear();
}

void
RD5RCodeplug::grouplist_t::clear() {
  memset(name, 0xff, sizeof(name));
  memset(member, 0, sizeof(member));
}

QString
RD5RCodeplug::grouplist_t::getName() const {
  return decode_ascii(name, 16, 0xff);
}
void
RD5RCodeplug::grouplist_t::setName(const QString &n) {
  encode_ascii(name, n, 16, 0xff);
}

RXGroupList *
RD5RCodeplug::grouplist_t::toRXGroupListObj() {
  return new RXGroupList(getName());
}

bool
RD5RCodeplug::grouplist_t::linkRXGroupListObj(int ncnt, RXGroupList *lst, const CodeplugContext &ctx) const {
  for (int i=0; (i<16) && (i<ncnt); i++) {
    if (ctx.hasDigitalContact(member[i]))
      lst->addContact(ctx.getDigitalContact(member[i]));
    else
      return false;
  }
  return true;
}

void
RD5RCodeplug::grouplist_t::fromRXGroupListObj(const RXGroupList *lst, const Config *conf) {
  setName(lst->name());
  for (int i=0; i<15; i++) {
    if (lst->count() > i) {
      member[i] = conf->contacts()->indexOfDigital(lst->contact(i))+1;
    } else {
      member[i] = 0;
    }
  }
}


/* ******************************************************************************************** *
 * Implementation of RD5RCodeplug::scanlist_t
 * ******************************************************************************************** */
RD5RCodeplug::scanlist_t::scanlist_t() {
  clear();
}

void
RD5RCodeplug::scanlist_t::clear() {
  memset(name, 0xff, 15);
  _unused = 1;
  channel_mark = 1;
  pl_type = PL_PRI_NONPRI;
  talkback = 1;
  memset(member, 0x00, 64);
  sign_hold_time = 40;
  prio_sample_time = 8;
  tx_designated_ch = 0;
}

QString
RD5RCodeplug::scanlist_t::getName() const {
  return decode_ascii(name, 15, 0xff);
}
void
RD5RCodeplug::scanlist_t::setName(const QString &n) {
  encode_ascii(name, n, 15, 0xff);
}

ScanList *
RD5RCodeplug::scanlist_t::toScanListObj() const {
  return new ScanList(getName());
}

bool
RD5RCodeplug::scanlist_t::linkScanListObj(ScanList *lst, const CodeplugContext &ctx) const {
  if (1 == priority_ch1)
    lst->setPriorityChannel(SelectedChannel::get());
  else if ((1<priority_ch1) && ctx.hasChannel(priority_ch1-1))
    lst->setPriorityChannel(ctx.getChannel(priority_ch1-1));
  else
    logWarn() << "Cannot deocde reference to priority channel index " << priority_ch1
                 << " in scan list '" << getName() << "'.";
  if (1 == priority_ch2)
    lst->setSecPriorityChannel(SelectedChannel::get());
  else if ((1<priority_ch2) && ctx.hasChannel(priority_ch2-1))
    lst->setSecPriorityChannel(ctx.getChannel(priority_ch2-1));
  else
    logWarn() << "Cannot deocde reference to secondary priority channel index " << priority_ch2
              << " in scan list '" << getName() << "'.";

  if (1 == tx_designated_ch)
    lst->setTXChannel(SelectedChannel::get());
  else if ((1<tx_designated_ch) && ctx.hasChannel(tx_designated_ch-1))
    lst->setTXChannel(ctx.getChannel(tx_designated_ch-1));
  else
    logWarn() << "Cannot deocde reference to transmit channel index " << tx_designated_ch
              << " in scan list '" << getName() << "'.";

  for (int i=0; (i<32) && (member[i]>0); i++) {
    if (1 == member[i])
      lst->addChannel(SelectedChannel::get());
    else if (member[i] && ctx.hasChannel(member[i]-1))
      lst->addChannel(ctx.getChannel(member[i]-1));
    else
      logWarn() << "Cannot deocde reference to channel index " << priority_ch2
                << " in scan list '" << getName() << "'.";
  }
  return true;
}

void
RD5RCodeplug::scanlist_t::fromScanListObj(const ScanList *lst, const Config *conf) {
  if (lst->priorityChannel() && (SelectedChannel::get() == lst->priorityChannel()))
    priority_ch1 = 1;
  else if (lst->priorityChannel())
    priority_ch1 = conf->channelList()->indexOf(lst->priorityChannel())+2;
  if (lst->secPriorityChannel() && (SelectedChannel::get() == lst->secPriorityChannel()))
    priority_ch2 = 1;
  else if (lst->secPriorityChannel())
    priority_ch2 = conf->channelList()->indexOf(lst->secPriorityChannel())+2;
  if (lst->txChannel() && (SelectedChannel::get() == lst->txChannel()))
    tx_designated_ch = 1;
  else if (lst->txChannel())
    tx_designated_ch = conf->channelList()->indexOf(lst->txChannel())+2;

  tx_designated_ch = 0;
  for (int i=0; i<32; i++) {
    if (i >= lst->count())
      member[i] = 0;
    else if (SelectedChannel::get() == lst->channel(i))
      member[i] = 1;
    else
      member[i] = conf->channelList()->indexOf(lst->channel(i))+2;
  }
}


/* ******************************************************************************************** *
 * Implementation of RD5RCodeplug::general_settings_t
 * ******************************************************************************************** */
RD5RCodeplug::general_settings_t::general_settings_t() {
  clear();
  initDefault();
}

void
RD5RCodeplug::general_settings_t::clear() {
  memset(radio_name, 0xff, 8);
  memset(radio_id, 0x00, 4);
  _reserved12   = 0;
  _reserved16   = 0;
  _reserved27_4 = 0;
  _reserved28_0 = 0;
  _unknown38_7  = 1;
  _reserved31   = 0;
  memset(prog_password, 0xff, 8);
}

void
RD5RCodeplug::general_settings_t::initDefault() {
  clear();
  tx_preamble          = 6;
  monitor_type         = OPEN_SQUELCH; // 0x00
  vox_sensitivity      = 3;
  lowbat_intervall     = 6;
  call_alert_dur       = 24; // 0x18
  lone_worker_response = 1;
  lone_worker_reminder = 10; //0x0a
  grp_hang  = 6;
  priv_hang = 6;

  // default value 0x40
  downch_mode_vfo      = 0;
  upch_mode_vfo        = 0;
  reset_tone           = 0;
  unknown_number_tone  = 0;
  arts_tone            = 4;

  // default value 0xc4
  permit_digital       = 0;
  permit_ananlog       = 0;
  selftest_tone        = 1;
  freq_ind_tone        = 0;
  disable_all_tones    = 0;
  savebat_receive      = 1;
  savebet_preamble     = 1;

  // default value 0x80
  disable_all_leds     = 0;
  inh_quickkey_ovr     = 0;

  // default value 0x10
  tx_exit_tone         = 0;
  dblwait_tx_active    = 1;
  animation            = 0;
  scan_mode            = SCANMODE_TIME;

  // default value 0x00
  repeater_end_delay   = 0;
  repeater_ste         = 0;
}

QString
RD5RCodeplug::general_settings_t::getName() const {
  return decode_ascii(radio_name, 8, 0xff);
}
void
RD5RCodeplug::general_settings_t::setName(const QString &n) {
  return encode_ascii(radio_name, n, 8, 0xff);
}

uint32_t
RD5RCodeplug::general_settings_t::getRadioId() const {
  return decode_dmr_id_bcd(radio_id);
}
void
RD5RCodeplug::general_settings_t::setRadioId(uint32_t num) {
  encode_dmr_id_bcd(radio_id, num);
}


/* ******************************************************************************************** *
 * Implementation of RD5RCodeplug::button_settings_t
 * ******************************************************************************************** */
RD5RCodeplug::button_settings_t::button_settings_t() {
  clear();
  initDefault();
}

void
RD5RCodeplug::button_settings_t::clear() {
  _unknown0      = 0x01;
  long_press_dur = 0x06;
  sk1_short      = 0x00;
  sk1_long       = 0x00;
  sk2_short      = 0x00;
  sk2_short      = 0x00;
  tk_short       = 0x13;
  tk_long        = 0x11;
  memset(one_touch, 0xff, 6*sizeof(one_touch_t));
}

void
RD5RCodeplug::button_settings_t::initDefault() {
  clear();
  long_press_dur = 0x06;
  sk1_short      = ZoneSelect;
  sk1_long       = ToggleFMRadio;
  sk2_short      = ToggleMonitor;
  sk2_long       = ToggleFlashLight;
}


/* ******************************************************************************************** *
 * Implementation of RD5RCodeplug::intro_text_t
 * ******************************************************************************************** */
RD5RCodeplug::intro_text_t::intro_text_t() {
  clear();
}

void
RD5RCodeplug::intro_text_t::clear() {
  memset(intro_line1, 0xff, 16);
  memset(intro_line2, 0xff, 16);
}

QString
RD5RCodeplug::intro_text_t::getIntroLine1() const {
  return decode_ascii(intro_line1, 16, 0xff);
}
void
RD5RCodeplug::intro_text_t::setIntroLine1(const QString &text) {
  encode_ascii(intro_line1, text, 16, 0xff);
}

QString
RD5RCodeplug::intro_text_t::getIntroLine2() const {
  return decode_ascii(intro_line2, 16, 0xff);
}
void
RD5RCodeplug::intro_text_t::setIntroLine2(const QString &text) {
  encode_ascii(intro_line2, text, 16, 0xff);
}


/* ******************************************************************************************** *
 * Implementation of RD5RCodeplug::msgtab_t
 * ******************************************************************************************** */
RD5RCodeplug::msgtab_t::msgtab_t() {
  clear();
}

void
RD5RCodeplug::msgtab_t::clear() {
  count = 0;
  memset(_unused1, 0, 7);
  memset(len, 0, NMESSAGES);
  memset(_unused2, 0, NMESSAGES);
  memset(message, 0, NMESSAGES*144);
}

QString
RD5RCodeplug::msgtab_t::getMessage(int i) const {
  if (i >= count)
    return "";
  return decode_ascii(message[i], len[i], 0xff);
}

bool
RD5RCodeplug::msgtab_t::addMessage(const QString &msg) {
  if (count == 144)
    return false;
  len[count] = std::min(msg.size(), 144);
  encode_ascii(message[count], msg, 144, 0xff);
  count++;
  return true;
}


/* ******************************************************************************************** *
 * Implementation of RD5RCodeplug::timestamp_t
 * ******************************************************************************************** */
QDateTime
RD5RCodeplug::timestamp_t::get() const {
  uint16_t year = (year_bcd & 0x0f) + 10*((year_bcd>>4) & 0x0f) + 100*((year_bcd>>8) & 0x0f)
      + 1000*((year_bcd>>12) & 0x0f);
  uint8_t month = (month_bcd & 0x0f) + 10*((month_bcd>>4) & 0x0f);
  uint8_t day = (day_bcd & 0x0f) + 10*((day_bcd>>4) & 0x0f);
  uint8_t hour = (hour_bcd & 0x0f) + 10*((hour_bcd>>4) & 0x0f);
  uint8_t minute = (minute_bcd & 0x0f) + 10*((minute_bcd>>4) & 0x0f);
  return QDateTime(QDate(year, month, day), QTime(hour, minute));
}

void
RD5RCodeplug::timestamp_t::setNow() {
  set(QDateTime::currentDateTime());
}

void
RD5RCodeplug::timestamp_t::set(const QDateTime &dt) {
  uint16_t year = dt.date().year();
  uint8_t month = dt.date().month(), day = dt.date().day(), hour = dt.time().hour(), minute = dt.time().minute();
  year_bcd = (year % 10) | ((year/10) % 10)<<4 | ((year/100) % 10) << 8 | ((year/100) % 10) << 12;
  month_bcd = (month % 10) | ((month/10) % 10) << 4;
  day_bcd = (day % 10) | ((day/10) % 10) << 4;
  hour_bcd = (hour % 10) | ((hour/10) % 10) << 4;
  minute_bcd = (minute % 10) | ((minute/10) % 10) << 4;
}


/* ******************************************************************************************** *
 * Implementation of RD5RCodeplug
 * ******************************************************************************************** */
RD5RCodeplug::RD5RCodeplug(QObject *parent)
  : CodePlug(parent)
{
  addImage("Radioddity RD5R Codeplug");
  image(0).addElement(0x00080, 0x07b80);
  image(0).addElement(0x08000, 0x16300);
}

void
RD5RCodeplug::clear()
{
  // Clear codeplug and set to default values
  memset(data(0x00080), 0xff, 0x07b80);
  memset(data(0x08000), 0xff, 0x16300);

  // clear timestamp
  timestamp_t *ts = (timestamp_t *)data(OFFSET_TIMESTMP);
  ts->setNow();

  // Clear basic config and set to default values
  general_settings_t *gs = (general_settings_t*) data(OFFSET_SETTINGS);
  gs->clear();
  gs->initDefault();

  // Clear default button settings
  button_settings_t *bs = (button_settings_t *)data(OFFSET_BUTTONS);
  bs->initDefault();

  // Clear intro text.
  intro_text_t *it = (intro_text_t*) data(OFFSET_INTRO);
  it->clear();

  // Clear channels
  for (int i=0; i<NCHAN; i++) {
    // First, get bank
    bank_t *b;
    if (0 == (i>>7))
      b = (bank_t*) data(OFFSET_BANK_0);
    else
      b = (((i>>7)-1) + (bank_t *) data(OFFSET_BANK_1));
    channel_t *ch = &b->chan[i % 128];
    ch->clear();
    // Disable channel
    b->bitmap[(i % 128) / 8] &= ~(1 << (i & 7));
  }

  // Clear Zones
  for (int i=0; i<NZONES; i++) {
    zonetab_t *zt = (zonetab_t*) data(OFFSET_ZONETAB);
    zone_t *z = &zt->zone[i];
    // Clear valid bit.
    zt->bitmap[i / 8] &= ~(1 << (i & 7));
    z->clear();
  }

  // Clear Scanlists
  for (int i=0; i<NSCANL; i++) {
    scantab_t *st = (scantab_t*) data(OFFSET_SCANTAB);
    scanlist_t *sl = &st->scanlist[i];
    // Clear valid bit.
    st->valid[i] = 0;
    sl->clear();
  }

  // Clear contacts
  for (int i=0; i<NCONTACTS; i++) {
    contact_t *ct = (contact_t*) data(OFFSET_CONTACTS + (i)*sizeof(contact_t));
    ct->clear();
  }

  // Clear DTMF contacts
  for (int i=0; i<NDTMF; i++) {
    dtmf_contact_t *ct = (dtmf_contact_t*) data(OFFSET_DTMF + (i)*sizeof(dtmf_contact_t));
    ct->clear();
  }

  // Clear Grouplists:
  for (int i=0; i<NGLISTS; i++) {
    grouptab_t *gt = (grouptab_t*) data(OFFSET_GROUPTAB);
    grouplist_t *gl = &gt->grouplist[i];
    gt->nitems1[i] = 0;
    gl->clear();
  }
}

bool
RD5RCodeplug::decode(Config *config)
{
  // Clear config
  config->reset();

  /* Unpack general config */
  general_settings_t *gs = (general_settings_t*) data(OFFSET_SETTINGS);
  if (nullptr == gs) {
    _errorMessage = QString("%1(): Cannot access general settings memory!")
        .arg(__func__);
    return false;
  }

  config->radioIDs()->getId(0)->setId(gs->getRadioId());
  config->setName(gs->getName());
  intro_text_t *it = (intro_text_t*) data(OFFSET_INTRO);
  config->setIntroLine1(it->getIntroLine1());
  config->setIntroLine2(it->getIntroLine2());

  CodeplugContext ctx(config);

  /* Unpack Contacts */
  for (int i=0; i<NCONTACTS; i++) {
    contact_t *ct = (contact_t *) data(OFFSET_CONTACTS+i*sizeof(contact_t));
    if (nullptr == ct) {
      _errorMessage = QString("%1(): Cannot access contact memory at index %2!")
          .arg(__func__).arg(i);
      return false;
    }

    // If contact is disabled
    if (! ct->isValid())
      continue;

    if (DigitalContact *cont = ct->toContactObj()) {
      logDebug() << "Contact at index " << i+1 << " mapped to "
                 << config->contacts()->count();
      ctx.addDigitalContact(cont, i+1);
    } else {
      _errorMessage = QString("%1(): Cannot decode codeplug: Invalid contact at index %2.")
          .arg(__func__).arg(i);
      return false;
    }
  }

  /* Unpack DTMF Contacts */
  for (int i=0; i<NDTMF; i++) {
    dtmf_contact_t *ct = (dtmf_contact_t *) data(OFFSET_DTMF+i*sizeof(dtmf_contact_t));
    if (nullptr == ct) {
      _errorMessage = QString("%1(): Cannot access DTMF contact memory at index %2!")
          .arg(__func__).arg(i);
      return false;
    }
    // If contact is disabled
    if (! ct->isValid())
      continue;
    if (DTMFContact *cont = ct->toContactObj()) {
      logDebug() << "DTMF contact at index " << i+1 << " mapped to "
                 << config->contacts()->dtmfCount();
      config->contacts()->addContact(cont);
    } else {
      _errorMessage = QString("%1(): Cannot decode codeplug: Invalid DTMF contact at index %2.")
          .arg(__func__).arg(i);
      return false;
    }
  }

  /* Unpack RX Group Lists */
  for (int i=0; i<NGLISTS; i++) {
    grouptab_t *gt = (grouptab_t*) data(OFFSET_GROUPTAB);
    if (nullptr == gt) {
      _errorMessage = QString("%1(): Cannot access group list table memory!")
          .arg(__func__);
      return false;
    }

    if (0 == gt->nitems1[i])
      continue;

    grouplist_t *gl = &gt->grouplist[i];
    if (nullptr == gl) {
      _errorMessage = QString("%1(): Cannot access group list memory at index %2!")
          .arg(__func__).arg(i);
      return false;
    }

    RXGroupList *list = gl->toRXGroupListObj();
    if (list) {
      logDebug() << "RX group list at index " << i+1 << " mapped to "
                 << config->rxGroupLists()->count();
      ctx.addGroupList(list, i+1);
    } else {
      _errorMessage = QString("%1(): Cannot decode codeplug: Invalid RX group-list at index %2.")
          .arg(__func__).arg(i);
      return false;
    }

    if(! gl->linkRXGroupListObj(gt->nitems1[i]-1, list, ctx)) {
      _errorMessage = QString("%1(): Cannot decode codeplug: Cannot link RX group list at index %2.")
          .arg(__func__).arg(i);
      return false;
    }
  }

  /* Unpack Channels */
  for (int i=0; i<NCHAN; i++) {
    // First, get bank
    bank_t *b;
    if ((i>>7) == 0)
      b = (bank_t*) data(OFFSET_BANK_0);
    else
      b = (((i>>7)-1) + (bank_t*) data(OFFSET_BANK_1));
    if (nullptr == b) {
      _errorMessage = QString("%1(): Cannot access channel bank at index %2!")
          .arg(__func__).arg(i);
      return false;
    }
    // If channel is disabled -> skip
    if (! ((b->bitmap[i % 128 / 8] >> (i & 7)) & 1) )
      continue;

    // finally, get channel
    channel_t *ch = &b->chan[i % 128];
    if (! ch){
      _errorMessage = QString("%1(): Cannot access channel at index %2!")
          .arg(__func__).arg(i);
      return false;
    }

    if (Channel *chan = ch->toChannelObj()) {
      logDebug() << "Map channel index " << i << " to " << config->channelList()->count();
      ctx.addChannel(chan, i+1);
    } else {
      _errorMessage = QString("%1(): Cannot unpack codeplug: Invalid channel at index %2!")
          .arg(__func__).arg(i);
      return false;
    }
  }

  /* Unpack Zones */
  QString last_zonename, last_zonebasename; Zone *last_zone = nullptr;
  bool extend_last_zone = false;
  for (int i=0; i<NZONES; i++) {
    zonetab_t *zt = (zonetab_t*) data(OFFSET_ZONETAB);
    if (! zt){
      _errorMessage = QString("%1(): Cannot access zone table memory.")
          .arg(__func__);
      return false;
    }
    // if zone is disabled
    if (! (zt->bitmap[i / 8] >> (i & 7) & 1) )
      continue;

    // get zone_t
    zone_t *z = &zt->zone[i];
    if (! z){
      _errorMessage = QString("%1(): Cannot access zone at index %2")
          .arg(__func__).arg(i);
      return false;
    }

    // Determine whether this zone should be combined with the previous one
    QString zonename = z->getName();
    QString zonebasename = zonename; zonebasename.chop(2);
    extend_last_zone = ( zonename.endsWith(" B") && last_zonename.endsWith(" A")
                         && (zonebasename == last_zonebasename)
                         && (nullptr != last_zone) && (0 == last_zone->B()->count()) );
    last_zonename = zonename;
    last_zonebasename = zonebasename;

    // Create zone obj
    if (! extend_last_zone) {
      last_zone = z->toZoneObj();
      if (nullptr == last_zone) {
        _errorMessage = QString("%1(): Cannot unpack codeplug: Invalid zone at index %2")
            .arg(__func__).arg(i);
        return false;
      }
      config->zones()->addZone(last_zone);
    } else {
      // when extending the last zone, chop its name to remove the "... A" part.
      last_zone->setName(last_zonebasename);
    }

    // Link zone obj
    if (! z->linkZoneObj(last_zone, ctx, extend_last_zone)) {
      _errorMessage = QString("%1(): Cannot unpack codeplug: Cannot link zone at index %2")
          .arg(__func__).arg(i);
      return false;
    }
  }

  /* Unpack Scan lists */
  for (int i=0; i<NSCANL; i++) {
    scantab_t *st = (scantab_t*) data(OFFSET_SCANTAB);
    if (! st){
      _errorMessage = QString("%1(): Cannot access scanlist table memory!")
          .arg(__func__);
      return false;
    }

    if (! st->valid[i])
      continue;

    scanlist_t *sl = &st->scanlist[i];
    if (! sl){
      _errorMessage = QString("%1(): Cannot access scan list at index %2")
          .arg(__func__).arg(i);
      return false;
    }

    ScanList *scan = sl->toScanListObj();
    if (nullptr == scan) {
      _errorMessage = QString("%1(): Cannot unpack codeplug: Invalid scanlist at index %2")
          .arg(__func__).arg(i);
      return false;
    }

    if (! sl->linkScanListObj(scan, ctx)) {
      _errorMessage = QString("%1(): Cannot unpack codeplug: Cannot link scanlist at index %2")
          .arg(__func__).arg(i);
      return false;
    }

    logDebug() << "Scan at index " << i+1 << " mapped to "
               << config->scanlists()->count();
    ctx.addScanList(scan, i+1);
  }

  /*
   * Link Channels -> ScanLists, etc.
   */
  for (int i=0, j=0; i<NCHAN; i++) {
    // First, get bank
    bank_t *b;
    if ((i>>7) == 0)
      b = (bank_t*) data(OFFSET_BANK_0);
    else
      b = (((i>>7)-1) + (bank_t*) data(OFFSET_BANK_1));
    // If channel is disabled
    if (! ((b->bitmap[i % 128 / 8] >> (i & 7)) & 1) )
      continue;
    // finally, get channel
    channel_t *ch = &b->chan[i % 128];
    if (! ch->linkChannelObj(ctx.getChannel(i+1), ctx)) {
      _errorMessage = QString("%1(): Cannot unpack codeplug: Cannot link channel at index %2")
          .arg(__func__).arg(j);
      return false;
    }
    // advance channel counter
    j++;
  }

  return true;
}

bool
RD5RCodeplug::encode(Config *config, const Flags &flags)
{
  // set timestamp
  timestamp_t *ts = (timestamp_t *)data(OFFSET_TIMESTMP);
  ts->setNow();

  // pack basic config
  general_settings_t *gs = (general_settings_t*) data(OFFSET_SETTINGS);
  if (! flags.updateCodePlug)
    gs->initDefault();
  gs->setName(config->name());
  gs->setRadioId(config->radioIDs()->getId(0)->id());

  intro_text_t *it = (intro_text_t*) data(OFFSET_INTRO);
  it->setIntroLine1(config->introLine1());
  it->setIntroLine2(config->introLine2());

  // Pack channels
  for (int i=0; i<NCHAN; i++) {
    // First, get bank
    bank_t *b;
    if ((i>>7) == 0)
      b = (bank_t*) data(OFFSET_BANK_0);
    else
      b = (((i>>7)-1) + (bank_t *) data(OFFSET_BANK_1));
    channel_t *ch = &b->chan[i % 128];

    // Disable channel if not used
    if (i >= config->channelList()->count()) {
      b->bitmap[(i % 128) / 8] &= ~(1 << (i & 7));
      continue;
    }

    // Construct from Channel object
    ch->clear();
    ch->fromChannelObj(config->channelList()->channel(i), config);

    // Set valid bit.
    b->bitmap[(i % 128) / 8] |= (1 << (i & 7));
  }

  // Pack Zones
  bool pack_zone_a = true;
  for (int i=0, j=0; i<NZONES; i++) {
    zonetab_t *zt = (zonetab_t*) data(OFFSET_ZONETAB);
    zone_t *z = &zt->zone[i];

next:
    if (j >= config->zones()->count()) {
      // Clear valid bit.
      zt->bitmap[i / 8] &= ~(1 << (i & 7));
      continue;
    }

    // Construct from Zone obj
    Zone *zone = config->zones()->zone(j);
    if (pack_zone_a) {
      pack_zone_a = false;
      if (zone->A()->count())
        z->fromZoneObjA(zone, config);
      else
        goto next;
    } else {
      pack_zone_a = true;
      j++;
      if (zone->B()->count())
        z->fromZoneObjB(zone, config);
      else
        goto next;
    }

    // Set valid bit.
    zt->bitmap[i / 8] |= (1 << (i & 7));
  }

  // Pack Scanlists
  for (int i=0; i<NSCANL; i++) {
    scantab_t *st = (scantab_t*) data(OFFSET_SCANTAB);
    scanlist_t *sl = &st->scanlist[i];

    if (i >= config->scanlists()->count()) {
      // Clear valid bit.
      st->valid[i] = 0;
      continue;
    }

    sl->fromScanListObj(config->scanlists()->scanlist(i), config);
    st->valid[i] = 1;
  }

  // Pack contacts
  for (int i=0; i<NCONTACTS; i++) {
    contact_t *ct = (contact_t*) data(OFFSET_CONTACTS + (i)*sizeof(contact_t));
    ct->clear();
    if (i >= config->contacts()->digitalCount())
      continue;
    ct->fromContactObj(config->contacts()->digitalContact(i), config);
  }

  // Pack DTMF contacts
  for (int i=0; i<NDTMF; i++) {
    dtmf_contact_t *ct = (dtmf_contact_t*) data(OFFSET_DTMF + (i)*sizeof(dtmf_contact_t));
    ct->clear();
    if (i >= config->contacts()->dtmfCount())
      continue;
    ct->fromContactObj(config->contacts()->dtmfContact(i), config);
  }

  // Pack Grouplists:
  for (int i=0; i<NGLISTS; i++) {
    grouptab_t *gt = (grouptab_t*) data(OFFSET_GROUPTAB);
    grouplist_t *gl = &gt->grouplist[i];
    if (i >= config->rxGroupLists()->count()) {
      gt->nitems1[i] = 0;
      continue;
    }
    // Enable group list
    gt->nitems1[i] = std::min(15,config->rxGroupLists()->list(i)->count()) + 1;
    gl->fromRXGroupListObj(config->rxGroupLists()->list(i), config);
  }

  return true;
}
