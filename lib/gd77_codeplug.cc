#include "gd77_codeplug.hh"
#include "config.hh"
#include "channel.hh"
#include "utils.hh"
#include "logger.hh"
#include <QDateTime>


#define ADDR_SETTINGS             0x0000e0
#define ADDR_BUTTONS              0x000108
#define ADDR_MESSAGE_BANK         0x000128

#define NUM_SCAN_LISTS                  64
#define ADDR_SCAN_LIST_BANK       0x001790
#define SCAN_LIST_SIZE            0x000058
#define SCAN_LIST_BANK_SIZE       0x001640

#define NUM_DTMF_CONTACTS               32
#define ADDR_DTMF_CONTACTS        0x002f88
#define DTMF_CONTACT_SIZE         0x000020

#define NUM_CHANNELS                  1024
#define NUM_CHANNEL_BANKS                8
#define NUM_CHANNELS_PER_BANK          128
#define ADDR_CHANNEL_BANK_0       0x003780 // Channels 1-128
#define ADDR_CHANNEL_BANK_1       0x00b1b0 // Channels 129-1024
#define CHANNEL_SIZE              0x000038
#define CHANNEL_BANK_SIZE         0x001c10

#define ADDR_BOOTSETTINGS         0x007518
#define ADDR_MENU_SETTINGS        0x007538
#define ADDR_BOOT_TEXT            0x007540

#define NUM_ZONES                      250
#define ADDR_ZONE_BANK            0x008010

#define NUM_CONTACTS                  1024
#define ADDR_CONTACTS             0x017620
#define CONTACT_SIZE              0x000018

#define NUM_GROUP_LISTS                 76
#define ADDR_GROUP_LIST_BANK      0x01d620
#define GROUPLIST_SIZE            0x000050
#define GROUP_LIST_BANK_SIZE      0x001840

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
  memset(this, 0, sizeof(channel_t));
  memset(name, 0xff, 16);
  _unused0019            = 0;
  _unused001e            = 0x50;
  _unused0024            = 0;
  tx_signaling_syst      = 0;
  _unused0026            = 0;
  rx_signaling_syst      = 0;
  _unused0028            = 0x16;
  privacy_group          = PRIVGR_NONE;
  emergency_system_index = 0;
  arts                   = ARTS_OFF;
  _unused0030_2          = 0;
  emergency_alarm_ack    = 0;
  data_call_conf         = 0;
  private_call_conf      = 0;
  _unused0031_1          = 0;
  privacy                = 0;
  _unused0031_5          = 0;
  _unused0031_7          = 0;
  dcdm                   = 0;
  _unused0032_1          = 0;
  pttid                  = PTTID_OFF;
  _unused0032_4          = 0;
  squelch                = SQ_NORMAL;
  bandwidth              = BW_12_5_KHZ;
  talkaround             = 0;
  _unused0033_4          = 0;
  vox                    = 0;
  _unused0034            = 0;
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

Signaling::Code
GD77Codeplug::channel_t::getRXTone() const {
  return decode_ctcss_tone_table(ctcss_dcs_receive);
}
void
GD77Codeplug::channel_t::setRXTone(Signaling::Code tone) {
  ctcss_dcs_receive = encode_ctcss_tone_table(tone);
}

Signaling::Code
GD77Codeplug::channel_t::getTXTone() const {
  return decode_ctcss_tone_table(ctcss_dcs_transmit);
}
void
GD77Codeplug::channel_t::setTXTone(Signaling::Code tone) {
  ctcss_dcs_transmit = encode_ctcss_tone_table(tone);
}

Channel *
GD77Codeplug::channel_t::toChannelObj() const {
  QString name = getName();
  double rxF = getRXFrequency();
  double txF = getTXFrequency();
  Channel::Power pwr = (POWER_HIGH == power) ? Channel::Power::High : Channel::Power::Low;
  uint timeout = tot*15;
  bool rxOnly = rx_only;
  if (MODE_ANALOG == channel_mode) {
    AnalogChannel::Admit admit;
    switch (admit_criteria) {
      case ADMIT_ALWAYS: admit = AnalogChannel::Admit::Always; break;
      case ADMIT_CH_FREE: admit = AnalogChannel::Admit::Free; break;
      default:
        return nullptr;
    }
    AnalogChannel::Bandwidth bw = (BW_25_KHZ == bandwidth) ? AnalogChannel::Bandwidth::Wide : AnalogChannel::Bandwidth::Narrow;
    return new AnalogChannel(
          name, rxF, txF, pwr, timeout, rxOnly, admit, squelch,  getRXTone(), getTXTone(),
          bw, nullptr);
  } else if(MODE_DIGITAL == channel_mode) {
    DigitalChannel::Admit admit;
    switch (admit_criteria) {
      case ADMIT_ALWAYS: admit = DigitalChannel::Admit::Always; break;
      case ADMIT_CH_FREE: admit = DigitalChannel::Admit::Free; break;
      case ADMIT_COLOR: admit = DigitalChannel::Admit::ColorCode; break;
      default:
        return nullptr;
    }
    DigitalChannel::TimeSlot slot = repeater_slot2 ?
          DigitalChannel::TimeSlot::TS2 : DigitalChannel::TimeSlot::TS1;
    return new DigitalChannel(
          name, rxF, txF, pwr, timeout, rxOnly, admit, colorcode_rx, slot,
          nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
  }

  return nullptr;
}

bool
GD77Codeplug::channel_t::linkChannelObj(Channel *c, const CodeplugContext &ctx) const {
  if (c->is<AnalogChannel>()) {
    AnalogChannel *ac = c->as<AnalogChannel>();
    if (scan_list_index && ctx.hasScanList(scan_list_index))
      ac->setScanListObj(ctx.getScanList(scan_list_index));
  } else {
    DigitalChannel *dc = c->as<DigitalChannel>();
    if (scan_list_index && ctx.hasScanList(scan_list_index))
      dc->setScanListObj(ctx.getScanList(scan_list_index));
    if (group_list_index && ctx.hasGroupList(group_list_index))
      dc->setGroupListObj(ctx.getGroupList(group_list_index));
    if (contact_name_index && ctx.hasDigitalContact(contact_name_index))
      dc->setTXContactObj(ctx.getDigitalContact(contact_name_index));
  }
  return true;
}

void
GD77Codeplug::channel_t::fromChannelObj(const Channel *c, const Config *conf) {
  clear();

  setName(c->name());
  setRXFrequency(c->rxFrequency());
  setTXFrequency(c->txFrequency());

  // encode power setting
  switch (c->power()) {
  case Channel::Power::Max:
  case Channel::Power::High:
    power = POWER_HIGH;
    break;
  case Channel::Power::Mid:
  case Channel::Power::Low:
  case Channel::Power::Min:
    power = POWER_LOW;
    break;
  }

  tot = c->timeout()/15;
  rx_only = c->rxOnly() ? 1 : 0;
  bandwidth = BW_12_5_KHZ;
  if (c->scanListObj())
    scan_list_index = conf->scanlists()->indexOf(c->scanListObj())+1;

  if (c->is<AnalogChannel>()) {
    const AnalogChannel *ac = c->as<const AnalogChannel>();
    channel_mode = MODE_ANALOG;
    switch (ac->admit()) {
      case AnalogChannel::Admit::Always: admit_criteria = ADMIT_ALWAYS; break;
      case AnalogChannel::Admit::Free: admit_criteria = ADMIT_CH_FREE; break;
      default: admit_criteria = ADMIT_CH_FREE; break;
    }
    bandwidth = (AnalogChannel::Bandwidth::Wide == ac->bandwidth()) ? BW_25_KHZ : BW_12_5_KHZ;
    squelch = SQ_NORMAL; //ac->squelch();
    setRXTone(ac->rxTone());
    setTXTone(ac->txTone());
  } else if (c->is<DigitalChannel>()) {
    const DigitalChannel *dc = c->as<const DigitalChannel>();
    channel_mode = MODE_DIGITAL;
    switch (dc->admit()) {
      case DigitalChannel::Admit::Always: admit_criteria = ADMIT_ALWAYS; break;
      case DigitalChannel::Admit::Free: admit_criteria = ADMIT_CH_FREE; break;
      case DigitalChannel::Admit::ColorCode: admit_criteria = ADMIT_COLOR; break;
    }
    repeater_slot2 = (DigitalChannel::TimeSlot::TS1 == dc->timeSlot()) ? 0 : 1;
    colorcode_rx = colorcode_tx = dc->colorCode();
    group_list_index = conf->rxGroupLists()->indexOf(dc->groupListObj()) + 1;
    contact_name_index = conf->contacts()->indexOfDigital(dc->txContactObj()) + 1;
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
GD77Codeplug::grouplist_t::linkRXGroupListObj(RXGroupList *lst, const CodeplugContext &ctx) const {
  for (int i=0; (i<32) && member[i]; i++) {
    if (! ctx.hasDigitalContact(member[i])) {
      logWarn() << "Cannot link RX group list '"<< lst->name() <<
                    ": contact #" << member[i] << " is not defined.";
      continue;
    }
    lst->addContact(ctx.getDigitalContact(member[i]));
  }
  return true;
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
 * Implementation of GD77Codeplug::zone_t
 * ******************************************************************************************** */
GD77Codeplug::zone_t::zone_t() {
  clear();
}

bool
GD77Codeplug::zone_t::isValid() const {
  return 0xff != name[0];
}
void
GD77Codeplug::zone_t::clear() {
  memset(name, 0xff, sizeof(name));
  memset(member, 0x00, sizeof(member));
}

QString
GD77Codeplug::zone_t::getName() const {
  return decode_ascii(name, 16, 0xff);
}
void
GD77Codeplug::zone_t::setName(const QString &n) {
  encode_ascii(name, n, 16, 0xff);
}

Zone *
GD77Codeplug::zone_t::toZoneObj() const {
  if (! isValid())
    return nullptr;
  return new Zone(getName());
}

bool
GD77Codeplug::zone_t::linkZoneObj(Zone *zone, const CodeplugContext &ctx, bool putInB) const {
  if (! isValid()) {
    logDebug() << "Cannot link zone: Zone is invalid.";
    return false;
  }

  for (int i=0; (i<16) && member[i]; i++) {
    if (ctx.hasChannel(member[i])) {
      if (! putInB)
        zone->A()->add(ctx.getChannel(member[i]));
      else
        zone->B()->add(ctx.getChannel(member[i]));
    } else {
      logWarn() << "While linking zone '" << zone->name() << "': " << i <<"-th channel index "
                << member[i] << " out of bounds.";
    }
  }
  return true;
}

void
GD77Codeplug::zone_t::fromZoneObjA(const Zone *zone, const Config *conf) {
  if (zone->A()->count() && zone->B()->count())
    setName(zone->name() + " A");
  else
    setName(zone->name());

  for (int i=0; i<16; i++) {
    if (i < zone->A()->count())
      member[i] = conf->channelList()->indexOf(zone->A()->get(i))+1;
    else
      member[i] = 0;
  }
}

void
GD77Codeplug::zone_t::fromZoneObjB(const Zone *zone, const Config *conf) {
  if (zone->A()->count() && zone->B()->count())
    setName(zone->name() + " B");
  else
    setName(zone->name());

  for (int i=0; i<16; i++) {
    if (i < zone->B()->count())
      member[i] = conf->channelList()->indexOf(zone->B()->get(i))+1;
    else
      member[i] = 0;
  }
}


/* ******************************************************************************************** *
 * Implementation of GD77Codeplug::scanlist_t
 * ******************************************************************************************** */
GD77Codeplug::scanlist_t::scanlist_t() {
  clear();
}

void
GD77Codeplug::scanlist_t::clear() {
  memset(name, 0xff, sizeof(name));
  _unused = 1;
  channel_mark = 1;
  pl_type = PL_PRI_NONPRI;
  talkback = 1;
  memset(member, 0x00, sizeof(member));
  sign_hold_time = 40;
  prio_sample_time = 8;
  tx_designated_ch = 0;
}

QString
GD77Codeplug::scanlist_t::getName() const {
  return decode_ascii(name, 15, 0xff);
}

void
GD77Codeplug::scanlist_t::setName(const QString &n) {
  encode_ascii(name, n, 15, 0xff);
}

ScanList *
GD77Codeplug::scanlist_t::toScanListObj() const {
  return new ScanList(getName());
}

bool
GD77Codeplug::scanlist_t::linkScanListObj(ScanList *lst, const CodeplugContext &ctx) const {
  if (0 == priority_ch1)
    lst->setPrimaryChannel(nullptr);
  else if (1 == priority_ch1)
    lst->setPrimaryChannel(SelectedChannel::get());
  else if ((1<priority_ch1) && ctx.hasChannel(priority_ch1-1))
    lst->setPrimaryChannel(ctx.getChannel(priority_ch1-1));
  else
    logWarn() << "Cannot deocde reference to priority channel index " << priority_ch1
                 << " in scan list '" << getName() << "'.";
  if (0 == priority_ch2)
    lst->setSecondaryChannel(nullptr);
  else if (1 == priority_ch2)
    lst->setSecondaryChannel(SelectedChannel::get());
  else if ((1<priority_ch2) && ctx.hasChannel(priority_ch2-1))
    lst->setSecondaryChannel(ctx.getChannel(priority_ch2-1));
  else
    logWarn() << "Cannot deocde reference to secondary priority channel index " << priority_ch2
              << " in scan list '" << getName() << "'.";

  if (0 == tx_designated_ch)
    lst->setRevertChannel(SelectedChannel::get());
  else if ((1<priority_ch2) && ctx.hasChannel(tx_designated_ch-1))
    lst->setRevertChannel(ctx.getChannel(tx_designated_ch-1));
  else
    logWarn() << "Cannot deocde reference to secondary priority channel index " << tx_designated_ch
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
GD77Codeplug::scanlist_t::fromScanListObj(const ScanList *lst, const Config *conf) {
  setName(lst->name());
  if (lst->primaryChannel() && (SelectedChannel::get() == lst->primaryChannel()))
    priority_ch1 = 1;
  else if (lst->primaryChannel())
    priority_ch1 = conf->channelList()->indexOf(lst->primaryChannel())+2;
  if (lst->secondaryChannel() && (SelectedChannel::get() == lst->secondaryChannel()))
    priority_ch2 = 1;
  else if (lst->secondaryChannel())
    priority_ch2 = conf->channelList()->indexOf(lst->secondaryChannel())+2;
  if (lst->revertChannel() && (SelectedChannel::get() == lst->revertChannel()))
    tx_designated_ch = 1;
  else if (lst->revertChannel())
    tx_designated_ch = conf->channelList()->indexOf(lst->revertChannel())+2;

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
 * Implementation of GD77Codeplug::contact_t
 * ******************************************************************************************** */
GD77Codeplug::contact_t::contact_t() {
  clear();
}

void
GD77Codeplug::contact_t::clear() {
  memset(name, 0xff, 16);
  memset(id, 0x00, 4);
  type = receive_tone = ring_style = valid = 0;
}

bool
GD77Codeplug::contact_t::isValid() const {
  return (0x00 != name[0]) && (0xff != name[0]);
}

uint32_t
GD77Codeplug::contact_t::getId() const {
  return decode_dmr_id_bcd(id);
}
void
GD77Codeplug::contact_t::setId(uint32_t num) {
  encode_dmr_id_bcd(id, num);
}

QString
GD77Codeplug::contact_t::getName() const {
  return decode_ascii(name, 16, 0xff);
}
void
GD77Codeplug::contact_t::setName(const QString &n) {
  encode_ascii(name, n, 16, 0xff);
}

DigitalContact *
GD77Codeplug::contact_t::toContactObj() const {
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
GD77Codeplug::contact_t::fromContactObj(const DigitalContact *cont, const Config *conf) {
  Q_UNUSED(conf);
  valid = 0xff;
  setName(cont->name());
  setId(cont->number());
  switch (cont->type()) {
    case DigitalContact::PrivateCall: type = CALL_PRIVATE; break;
    case DigitalContact::GroupCall:   type = CALL_GROUP; break;
    case DigitalContact::AllCall:     type = CALL_ALL; break;
  }
  if (cont->ring())
    receive_tone = ring_style = 1;
}


/* ******************************************************************************************** *
 * Implementation of GD77Codeplug::dtmf_contact_t
 * ******************************************************************************************** */
GD77Codeplug::dtmf_contact_t::dtmf_contact_t() {
  clear();
}

void
GD77Codeplug::dtmf_contact_t::clear() {
  memset(name, 0xff, 16);
  memset(number, 0xff, 16);
}

bool
GD77Codeplug::dtmf_contact_t::isValid() const {
  return 0xff != name[0];
}

QString
GD77Codeplug::dtmf_contact_t::getNumber() const {
  return decode_dtmf_bin(number, 16, 0xff);
}
bool
GD77Codeplug::dtmf_contact_t::setNumber(const QString &num) {
  return encode_dtmf_bin(num, number, 16, 0xff);
}

QString
GD77Codeplug::dtmf_contact_t::getName() const {
  return decode_ascii(name, 16, 0xff);
}
void
GD77Codeplug::dtmf_contact_t::setName(const QString &n) {
  encode_ascii(name, n, 16, 0xff);
}

DTMFContact *
GD77Codeplug::dtmf_contact_t::toContactObj() const {
  if (! isValid())
    return nullptr;
  return new DTMFContact(getName(), getNumber());
}

void
GD77Codeplug::dtmf_contact_t::fromContactObj(const DTMFContact *cont, const Config *conf) {
  Q_UNUSED(conf);
  setName(cont->name());
  setNumber(cont->number());
}


/* ******************************************************************************************** *
 * Implementation of GD77Codeplug::general_settings_t
 * ******************************************************************************************** */
GD77Codeplug::general_settings_t::general_settings_t() {
  clear();
  initDefault();
}

void
GD77Codeplug::general_settings_t::clear() {
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
GD77Codeplug::general_settings_t::initDefault() {
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
GD77Codeplug::general_settings_t::getName() const {
  return decode_ascii(radio_name, 8, 0xff);
}
void
GD77Codeplug::general_settings_t::setName(const QString &n) {
  return encode_ascii(radio_name, n, 8, 0xff);
}

uint32_t
GD77Codeplug::general_settings_t::getRadioId() const {
  return decode_dmr_id_bcd(radio_id);
}
void
GD77Codeplug::general_settings_t::setRadioId(uint32_t num) {
  encode_dmr_id_bcd(radio_id, num);
}


/* ******************************************************************************************** *
 * Implementation of GD77Codeplug::button_settings_t
 * ******************************************************************************************** */
GD77Codeplug::button_settings_t::button_settings_t() {
  clear();
  initDefault();
}

void
GD77Codeplug::button_settings_t::clear() {
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
GD77Codeplug::button_settings_t::initDefault() {
  clear();
  long_press_dur = 0x06;
  sk1_short      = ZoneSelect;
  sk1_long       = ToggleFMRadio;
  sk2_short      = ToggleMonitor;
  sk2_long       = ToggleFlashLight;
}


/* ******************************************************************************************** *
 * Implementation of GD77Codeplug::intro_text_t
 * ******************************************************************************************** */
GD77Codeplug::intro_text_t::intro_text_t() {
  clear();
}

void
GD77Codeplug::intro_text_t::clear() {
  memset(intro_line1, 0xff, 16);
  memset(intro_line2, 0xff, 16);
}

QString
GD77Codeplug::intro_text_t::getIntroLine1() const {
  return decode_ascii(intro_line1, 16, 0xff);
}
void
GD77Codeplug::intro_text_t::setIntroLine1(const QString &text) {
  encode_ascii(intro_line1, text, 16, 0xff);
}

QString
GD77Codeplug::intro_text_t::getIntroLine2() const {
  return decode_ascii(intro_line2, 16, 0xff);
}
void
GD77Codeplug::intro_text_t::setIntroLine2(const QString &text) {
  encode_ascii(intro_line2, text, 16, 0xff);
}


/* ******************************************************************************************** *
 * Implementation of GD77Codeplug::msgtab_t
 * ******************************************************************************************** */
GD77Codeplug::msgtab_t::msgtab_t() {
  clear();
}

void
GD77Codeplug::msgtab_t::clear() {
  count = 0;
  memset(_unused1, 0, 7);
  memset(len, 0, NMESSAGES);
  memset(_unused2, 0, NMESSAGES);
  memset(message, 0, NMESSAGES*144);
}

QString
GD77Codeplug::msgtab_t::getMessage(int i) const {
  if (i >= count)
    return "";
  return decode_ascii(message[i], len[i], 0xff);
}

bool
GD77Codeplug::msgtab_t::addMessage(const QString &msg) {
  if (count == 144)
    return false;
  len[count] = std::min(msg.size(), 144);
  encode_ascii(message[count], msg, 144, 0xff);
  count++;
  return true;
}


/* ******************************************************************************************** *
 * Implementation of GD77Codeplug::ChannelElement
 * ******************************************************************************************** */
GD77Codeplug::ChannelElement::ChannelElement(uint8_t *ptr, size_t size)
  : RadioddityCodeplug::ChannelElement(ptr, size)
{
  // pass...
}

GD77Codeplug::ChannelElement::ChannelElement(uint8_t *ptr)
  : RadioddityCodeplug::ChannelElement(ptr)
{
  // pass...
}

void
GD77Codeplug::ChannelElement::clear() {
  RadioddityCodeplug::ChannelElement::clear();
  setUInt8(0x0028, 0x00);
  setARTSMode(ARTS_OFF);
  setSTEAngle(STE_FREQUENCY);
}

GD77Codeplug::ChannelElement::ARTSMode
GD77Codeplug::ChannelElement::artsMode() const {
  return ARTSMode(getUInt2(0x0030,0));
}
void
GD77Codeplug::ChannelElement::setARTSMode(ARTSMode mode) {
  setUInt2(0x0030, 0, (uint)mode);
}

GD77Codeplug::ChannelElement::STEAngle
GD77Codeplug::ChannelElement::steAngle() const {
  return STEAngle(getUInt2(0x0032,6));
}
void
GD77Codeplug::ChannelElement::setSTEAngle(STEAngle angle) {
  setUInt2(0x0032, 6, (uint)angle);
}

GD77Codeplug::ChannelElement::PTTId
GD77Codeplug::ChannelElement::pttIDMode() const {
  return PTTId(getUInt2(0x0032, 2));
}
void
GD77Codeplug::ChannelElement::setPTTIDMode(PTTId mode) {
  setUInt2(0x0032, 2, (uint)mode);
}

bool
GD77Codeplug::ChannelElement::squelchIsTight() const {
  return getBit(0x0033, 0);
}
void
GD77Codeplug::ChannelElement::enableTightSquelch(bool enable) {
  setBit(0x0033, 0, enable);
}

bool
GD77Codeplug::ChannelElement::loneWorker() const {
  return getBit(0x0033, 4);
}
void
GD77Codeplug::ChannelElement::enableLoneWorker(bool enable) {
  setBit(0x0033, 4, enable);
}

bool
GD77Codeplug::ChannelElement::autoscan() const {
  return getBit(0x0033, 5);
}
void
GD77Codeplug::ChannelElement::enableAutoscan(bool enable) {
  setBit(0x0033, 5, enable);
}


/* ******************************************************************************************** *
 * Implementation of GD77Codeplug::ContactElement
 * ******************************************************************************************** */
GD77Codeplug::ContactElement::ContactElement(uint8_t *ptr, uint size)
  : RadioddityCodeplug::ContactElement(ptr, size)
{
  // pass...
}

GD77Codeplug::ContactElement::ContactElement(uint8_t *ptr)
  : RadioddityCodeplug::ContactElement(ptr)
{
  // pass...
}

void
GD77Codeplug::ContactElement::clear() {
  markValid(false);
}

bool
GD77Codeplug::ContactElement::isValid() const {
  return RadioddityCodeplug::ContactElement::isValid() && (0x00 != getUInt8(0x0017));
}
void
GD77Codeplug::ContactElement::markValid(bool valid) {
  if (valid)
    setUInt8(0x0017, 0xff);
  else
    setUInt8(0x0017, 0x00);
}
void
GD77Codeplug::ContactElement::fromContactObj(const DigitalContact *obj, Context &ctx) {
  RadioddityCodeplug::ContactElement::fromContactObj(obj, ctx);
  markValid();
}


/* ******************************************************************************************** *
 * Implementation of GD77Codeplug::ScanListElement
 * ******************************************************************************************** */
GD77Codeplug::ScanListElement::ScanListElement(uint8_t *ptr, uint size)
  : RadioddityCodeplug::ScanListElement(ptr, size)
{
  // pass...
}

GD77Codeplug::ScanListElement::ScanListElement(uint8_t *ptr)
  : RadioddityCodeplug::ScanListElement(ptr)
{
  // pass...
}

void
GD77Codeplug::ScanListElement::clear() {
  RadioddityCodeplug::ScanListElement::clear();
  setBit(0x0f, 0, true);
}


/* ******************************************************************************************** *
 * Implementation of GD77Codeplug::ScanListBankElement
 * ******************************************************************************************** */
GD77Codeplug::ScanListBankElement::ScanListBankElement(uint8_t *ptr, uint size)
  : RadioddityCodeplug::ScanListBankElement(ptr, size)
{
  // pass...
}

GD77Codeplug::ScanListBankElement::ScanListBankElement(uint8_t *ptr)
  : RadioddityCodeplug::ScanListBankElement(ptr, 0x1640)
{
  // pass...
}

void
GD77Codeplug::ScanListBankElement::clear() {
  memset(_data, 0, 0x0040);
}

uint8_t *
GD77Codeplug::ScanListBankElement::get(uint n) const {
  return _data+0x0040 + n*0x0058;
}


/* ******************************************************************************************** *
 * Implementation of GD77Codeplug::GroupListElement
 * ******************************************************************************************** */
GD77Codeplug::GroupListElement::GroupListElement(uint8_t *ptr, uint size)
  : RadioddityCodeplug::GroupListElement(ptr, size)
{
  // pass...
}

GD77Codeplug::GroupListElement::GroupListElement(uint8_t *ptr)
  : RadioddityCodeplug::GroupListElement(ptr, 0x0050)
{
  // pass...
}


/* ******************************************************************************************** *
 * Implementation of GD77Codeplug::GroupListBankElement
 * ******************************************************************************************** */
GD77Codeplug::GroupListBankElement::GroupListBankElement(uint8_t *ptr, uint size)
  : RadioddityCodeplug::GroupListBankElement(ptr, size)
{
  // pass...
}

GD77Codeplug::GroupListBankElement::GroupListBankElement(uint8_t *ptr)
  : RadioddityCodeplug::GroupListBankElement(ptr, 0x1840)
{
  // pass...
}

uint8_t *
GD77Codeplug::GroupListBankElement::get(uint n) const {
  return _data + 0x80 + n*GROUPLIST_SIZE;
}


/* ******************************************************************************************** *
 * Implementation of GD77Codeplug
 * ******************************************************************************************** */
GD77Codeplug::GD77Codeplug(QObject *parent)
  : RadioddityCodeplug(parent)
{
  addImage("Radioddity GD77 Codeplug");
  image(0).addElement(0x00080, 0x07b80);
  image(0).addElement(0x08000, 0x16300);
}

void
GD77Codeplug::clearGeneralSettings() {
  GeneralSettingsElement(data(ADDR_SETTINGS)).clear();
}

bool
GD77Codeplug::encodeGeneralSettings(Config *config, const Flags &flags, Context &ctx) {
  GeneralSettingsElement el(data(ADDR_SETTINGS));
  if (! flags.updateCodePlug)
    el.clear();
  return el.fromConfig(config, ctx);
}

bool
GD77Codeplug::decodeGeneralSettings(Config *config, Context &ctx) {
  return GeneralSettingsElement(data(ADDR_SETTINGS)).updateConfig(config, ctx);
}

void
GD77Codeplug::clearButtonSettings() {
  ButtonSettingsElement(data(ADDR_BUTTONS)).clear();
}

void
GD77Codeplug::clearMessages() {
  MessageBankElement(data(ADDR_MESSAGE_BANK)).clear();
}

void
GD77Codeplug::clearScanLists() {
  ScanListBankElement bank(data(ADDR_SCAN_LIST_BANK)); bank.clear();
  for (int i=0; i<NUM_SCAN_LISTS; i++)
    ScanListElement(bank.get(i)).clear();
}

bool
GD77Codeplug::encodeScanLists(Config *config, const Flags &flags, Context &ctx) {
  ScanListBankElement bank(data(ADDR_SCAN_LIST_BANK));
  for (int i=0; i<NUM_SCAN_LISTS; i++) {
    if (i >= config->scanlists()->count()) {
      bank.enable(i, false); continue;
    }
    ScanListElement(bank.get(i)).fromScanListObj(config->scanlists()->scanlist(i), ctx);
    bank.enable(i, true);
  }
  return true;
}

bool
GD77Codeplug::createScanLists(Config *config, Context &ctx) {
  ScanListBankElement bank(data(ADDR_SCAN_LIST_BANK));
  for (int i=0; i<NUM_SCAN_LISTS; i++) {
    if (! bank.isEnabled(i))
      continue;
    ScanListElement el(bank.get(i));
    ScanList *scan = el.toScanListObj(ctx);
    config->scanlists()->add(scan); ctx.add(scan, i+1);
  }
  return true;
}

bool
GD77Codeplug::linkScanLists(Config *config, Context &ctx) {
  ScanListBankElement bank(data(ADDR_SCAN_LIST_BANK));
  for (int i=0; i<NUM_SCAN_LISTS; i++) {
    if (! bank.isEnabled(i))
      continue;
    if (! ScanListElement(bank.get(i)).linkScanListObj(ctx.get<ScanList>(i+1), ctx))
      return false;
  }
  return true;
}

void
GD77Codeplug::clearChannels() {
  for (int b=0,c=0; b<NUM_CHANNEL_BANKS; b++) {
    uint8_t *ptr = nullptr;
    if (0 == b) ptr = data(ADDR_CHANNEL_BANK_0);
    else ptr = data(ADDR_CHANNEL_BANK_1 + (b-1)*CHANNEL_BANK_SIZE);
    ChannelBankElement bank(ptr); bank.clear();
    for (int i=0; (i<NUM_CHANNELS_PER_BANK)&&(c<NUM_CHANNELS); i++, c++)
      ChannelElement(bank.get(i)).clear();
  }
}

bool
GD77Codeplug::encodeChannels(Config *config, const Flags &flags, Context &ctx) {
  for (int b=0,c=0; b<NUM_CHANNEL_BANKS; b++) {
    uint8_t *ptr = nullptr;
    if (0 == b) ptr = data(ADDR_CHANNEL_BANK_0);
    else ptr = data(ADDR_CHANNEL_BANK_1 + (b-1)*CHANNEL_BANK_SIZE);
    ChannelBankElement bank(ptr); bank.clear();
    for (int i=0; (i<NUM_CHANNELS_PER_BANK)&&(c<NUM_CHANNELS); i++, c++) {
      ChannelElement el(bank.get(i));
      if (c < config->channelList()->count()) {
        if (! el.fromChannelObj(config->channelList()->channel(c), ctx)) {
          logError() << "Cannot encode channel " << c << " (" << i << " of bank " << b <<").";
          return false;
        }
        bank.enable(i,true);
      } else {
        el.clear();
        bank.enable(i, false);
      }
    }
  }
  return true;
}

bool
GD77Codeplug::createChannels(Config *config, Context &ctx) {
  for (int b=0,c=0; b<NUM_CHANNEL_BANKS; b++) {
    uint8_t *ptr = nullptr;
    if (0 == b) ptr = data(ADDR_CHANNEL_BANK_0);
    else ptr = data(ADDR_CHANNEL_BANK_1 + (b-1)*CHANNEL_BANK_SIZE);
    ChannelBankElement bank(ptr);
    for (int i=0; (i<NUM_CHANNELS_PER_BANK)&&(c<NUM_CHANNELS); i++, c++) {
      if (! bank.isEnabled(i))
        continue;
      Channel *ch = ChannelElement(bank.get(i)).toChannelObj(ctx);
      config->channelList()->add(ch); ctx.add(ch, c+1);
    }
  }
  return true;
}

bool
GD77Codeplug::linkChannels(Config *config, Context &ctx) {
  for (int b=0,c=0; b<NUM_CHANNEL_BANKS; b++) {
    uint8_t *ptr = nullptr;
    if (0 == b) ptr = data(ADDR_CHANNEL_BANK_0);
    else ptr = data(ADDR_CHANNEL_BANK_1 + (b-1)*CHANNEL_BANK_SIZE);
    ChannelBankElement bank(ptr);
    for (int i=0; (i<NUM_CHANNELS_PER_BANK)&&(c<NUM_CHANNELS); i++, c++) {
      if (! bank.isEnabled(i))
        continue;
      if (!ChannelElement(bank.get(i)).linkChannelObj(ctx.get<Channel>(c+1), ctx))
        return false;
    }
  }
  return true;
}

void
GD77Codeplug::clearBootSettings() {
  BootSettingsElement(data(ADDR_BOOTSETTINGS)).clear();
}

void
GD77Codeplug::clearMenuSettings() {
  MenuSettingsElement(data(ADDR_MENU_SETTINGS)).clear();
}

void
GD77Codeplug::clearBootText() {
  BootTextElement(data(ADDR_BOOT_TEXT)).clear();
}

bool
GD77Codeplug::encodeBootText(Config *config, const Flags &flags, Context &ctx) {
  BootTextElement(data(ADDR_BOOT_TEXT)).fromConfig(config);
  return true;
}

bool
GD77Codeplug::decodeBootText(Config *config, Context &ctx) {
  BootTextElement(data(ADDR_BOOT_TEXT)).updateConfig(config);
  return true;
}

void
GD77Codeplug::clearVFOSettings() {
  /// @bug Search for VFO channels in GD77 codeplug!
}

void
GD77Codeplug::clearZones() {
  ZoneBankElement bank(data(ADDR_ZONE_BANK));
  bank.clear();
  for (int i=0; i<NUM_ZONES; i++)
    ZoneElement(bank.get(i)).clear();
}

bool
GD77Codeplug::encodeZones(Config *config, const Flags &flags, Context &ctx) {
  ZoneBankElement bank(data(ADDR_ZONE_BANK));

  // Pack Zones
  bool pack_zone_a = true;
  for (int i=0, j=0; i<NUM_ZONES; i++) {
    ZoneElement z(bank.get(i));
next:
    if (j >= config->zones()->count()) {
      bank.enable(i, false);
      continue;
    }

    // Construct from Zone obj
    Zone *zone = config->zones()->zone(j);
    if (pack_zone_a) {
      pack_zone_a = false;
      if (zone->A()->count())
        z.fromZoneObjA(zone, ctx);
      else
        goto next;
    } else {
      pack_zone_a = true;
      j++;
      if (zone->B()->count())
        z.fromZoneObjB(zone, ctx);
      else
        goto next;
    }
    bank.enable(i, true);
  }
  return true;
}

bool
GD77Codeplug::createZones(Config *config, Context &ctx) {
  QString last_zonename, last_zonebasename; Zone *last_zone = nullptr;
  bool extend_last_zone = false;
  ZoneBankElement bank(data(ADDR_ZONE_BANK));

  for (int i=0; i<NUM_ZONES; i++) {
    if (! bank.isEnabled(i))
      continue;
    ZoneElement z(bank.get(i));

    // Determine whether this zone should be combined with the previous one
    QString zonename = z.name();
    QString zonebasename = zonename; zonebasename.chop(2);
    extend_last_zone = ( zonename.endsWith(" B") && last_zonename.endsWith(" A")
                         && (zonebasename == last_zonebasename)
                         && (nullptr != last_zone) && (0 == last_zone->B()->count()) );
    last_zonename = zonename;
    last_zonebasename = zonebasename;

    // Create zone obj
    if (! extend_last_zone) {
      last_zone = z.toZoneObj(ctx);
      config->zones()->add(last_zone);
      ctx.add(last_zone, i+1);
    } else {
      // when extending the last zone, chop its name to remove the "... A" part.
      last_zone->setName(last_zonebasename);
    }
  }
  return true;
}

bool
GD77Codeplug::linkZones(Config *config, Context &ctx) {
  QString last_zonename, last_zonebasename; Zone *last_zone = nullptr;
  bool extend_last_zone = false;
  ZoneBankElement bank(data(ADDR_ZONE_BANK));

  for (int i=0; i<NUM_ZONES; i++) {
    if (! bank.isEnabled(i))
      continue;
    ZoneElement z(bank.get(i));

    // Determine whether this zone should be combined with the previous one
    QString zonename = z.name();
    QString zonebasename = zonename; zonebasename.chop(2);
    extend_last_zone = ( zonename.endsWith(" B") && last_zonename.endsWith(" A")
                         && (zonebasename == last_zonebasename)
                         && (nullptr != last_zone) && (0 == last_zone->B()->count()) );
    last_zonename = zonename;
    last_zonebasename = zonebasename;

    // Create zone obj
    if (! extend_last_zone) {
      last_zone = ctx.get<Zone>(i+1);
    }
    if (! z.linkZoneObj(last_zone, ctx, extend_last_zone)) {
      _errorMessage = QString("%1(): Cannot unpack codeplug: Cannot link zone at index %2")
          .arg(__func__).arg(i);
      return false;
    }
  }

  return true;
}

void
GD77Codeplug::clearContacts() {
  for (int i=0; i<NUM_CONTACTS; i++)
    ContactElement(data(ADDR_CONTACTS + i*CONTACT_SIZE)).clear();
}

bool
GD77Codeplug::encodeContacts(Config *config, const Flags &flags, Context &ctx) {
  for (int i=0; i<NUM_CONTACTS; i++) {
    ContactElement el(data(ADDR_CONTACTS + i*CONTACT_SIZE));
    el.clear();
    if (i >= config->contacts()->digitalCount())
      continue;
    el.fromContactObj(config->contacts()->digitalContact(i), ctx);
  }
  return true;
}

bool
GD77Codeplug::createContacts(Config *config, Context &ctx) {
  /* Unpack Contacts */
  for (int i=0; i<NUM_CONTACTS; i++) {
    ContactElement el(data(ADDR_CONTACTS + i*CONTACT_SIZE));
    if (!el.isValid())
      continue;

    DigitalContact *cont = el.toContactObj(ctx);
    ctx.add(cont, i+1); config->contacts()->add(cont);
  }
  return true;
}

void
GD77Codeplug::clearDTMFContacts() {
  for (int i=0; i<NUM_DTMF_CONTACTS; i++)
    DTMFContactElement(data(ADDR_DTMF_CONTACTS + i*DTMF_CONTACT_SIZE)).clear();
}

bool
GD77Codeplug::encodeDTMFContacts(Config *config, const Flags &flags, Context &ctx) {
  for (int i=0; i<NUM_DTMF_CONTACTS; i++) {
    DTMFContactElement el(data(ADDR_DTMF_CONTACTS + i*DTMF_CONTACT_SIZE));
    el.clear();
    if (i >= config->contacts()->dtmfCount())
      continue;
    el.fromContactObj(config->contacts()->dtmfContact(i), ctx);
  }
  return true;
}

bool
GD77Codeplug::createDTMFContacts(Config *config, Context &ctx) {
  for (int i=0; i<NUM_DTMF_CONTACTS; i++) {
    DTMFContactElement el(data(ADDR_DTMF_CONTACTS+i*DTMF_CONTACT_SIZE));
    // If contact is disabled
    if (! el.isValid())
      continue;
    DTMFContact *cont = el.toContactObj(ctx);
    ctx.add(cont, i+1); config->contacts()->add(cont);
  }
  return true;
}

void
GD77Codeplug::clearGroupLists() {
  GroupListBankElement bank(data(ADDR_GROUP_LIST_BANK)); bank.clear();
  for (int i=0; i<NUM_GROUP_LISTS; i++)
    GroupListElement(bank.get(i)).clear();
}

bool
GD77Codeplug::encodeGroupLists(Config *config, const Flags &flags, Context &ctx) {
  GroupListBankElement bank(data(ADDR_GROUP_LIST_BANK)); bank.clear();
  for (int i=0; i<NUM_GROUP_LISTS; i++) {
    if (i >= config->rxGroupLists()->count())
      continue;
    GroupListElement el(bank.get(i));
    el.fromRXGroupListObj(config->rxGroupLists()->list(i), ctx);
    bank.setContactCount(i, config->rxGroupLists()->list(i)->count());
  }
  return true;
}

bool
GD77Codeplug::createGroupLists(Config *config, Context &ctx) {
  GroupListBankElement bank(data(ADDR_GROUP_LIST_BANK));
  for (int i=0; i<NUM_GROUP_LISTS; i++) {
    if (! bank.isEnabled(i))
      continue;
    GroupListElement el(bank.get(i));
    RXGroupList *list = el.toRXGroupListObj(ctx);
    config->rxGroupLists()->add(list); ctx.add(list, i+1);
  }
  return true;
}

bool
GD77Codeplug::linkGroupLists(Config *config, Context &ctx) {
  GroupListBankElement bank(data(ADDR_GROUP_LIST_BANK));
  for (int i=0; i<NUM_GROUP_LISTS; i++) {
    if (! bank.isEnabled(i))
      continue;
    GroupListElement el(bank.get(i));
    /*logDebug() << "Link " << bank.contactCount(i) << " members of group list '"
               << ctx.get<RXGroupList>(i+1)->name() << "'.";*/
    if (! el.linkRXGroupListObj(bank.contactCount(i), ctx.get<RXGroupList>(i+1), ctx)) {
      _errorMessage = tr("Cannot link group list '%1'.").arg(ctx.get<RXGroupList>(i+1)->name());
      return false;
    }
  }
  return true;
}
