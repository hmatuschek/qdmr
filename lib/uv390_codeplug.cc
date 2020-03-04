#include "uv390_codeplug.hh"
#include "config.hh"
#include "utils.hh"
#include "channel.hh"
#include "gpssystem.hh"
#include "userdatabase.hh"
#include "config.h"
#include <QTimeZone>


#define NCHAN            3000
#define NCONTACTS        10000
#define NZONES           250
#define NEMERGENCY       32
#define NGLISTS          250
#define NSCANL           250
#define NMESSAGES        50
#define NGPSSYSTEMS      16

// ---- first segment ----
#define OFFSET_TIMESTMP  0x002800
#define OFFSET_SETTINGS  0x002840
#define OFFSET_MENU      0x0028f0
#define OFFSET_BUTTONS   0x002900
#define OFFSET_MSG       0x002980
#define OFFSET_PRIVACY   0x0061c0
#define OFFSET_EMERGENCY 0x006250
#define OFFSET_GLISTS    0x00f420
#define OFFSET_ZONES     0x0151e0
#define OFFSET_SCANL     0x019060
#define OFFSET_VFO_A     0x02f700
#define OFFSET_VFO_B     0x02f740
#define OFFSET_ZONEXT    0x031800
#define OFFSET_GPS_SYS   0x03f440
// ---- second segment ----
#define OFFSET_CHANNELS  0x110800
#define OFFSET_CONTACTS  0x140800
// ---- thrid segement ----
#define CALLSIGN_START   0x00200000  // Start of callsign database
#define CALLSIGN_END     0x01000000  // End of callsign database
#define CALLSIGN_OFFSET  0x00204003  // Start of callsign entries


/* ******************************************************************************************** *
 * Implementation of UV390Codeplug::channel_t
 * ******************************************************************************************** */
bool
UV390Codeplug::channel_t::isValid() const {
  return (name[0] != 0x0000) && (name[0] != 0xffff);
}

UV390Codeplug::channel_t::channel_t() {
  clear();
}

void
UV390Codeplug::channel_t::clear() {
  channel_mode = MODE_ANALOG;
  bandwidth = BW_12_5_KHZ;
  autoscan = 0;
  _unused0_1 = _unused0_2 = 1;
  lone_worker = 0;

  allow_talkaround = 1;
  rx_only = 0;
  time_slot = 1;
  color_code = 1;

  privacy_no = 0;
  privacy = PRIV_NONE;
  private_call_conf = 0;
  data_call_conf = 0;

  rx_ref_frequency = REF_LOW;
  _unused3_2 = 0;
  emergency_alarm_ack = 0;
  _unused3_4 = 0b110;
  display_pttid_dis = 1;

  tx_ref_frequency = REF_LOW;
  _unused4_2 = 0b01;
  vox = 0;
  _unused4_5 = 1;
  admit_criteria = ADMIT_ALWAYS;

  _unused5_0 = 0;
  in_call_criteria = INCALL_ALWAYS;
  turn_off_freq = TURNOFF_NONE;

  contact_name_index = 0;

  tot = 0;
  _unused8_6 = 0;
  tot_rekey_delay = 0;

  emergency_system_index = 0;
  scan_list_index = 0;

  group_list_index = 0;
  gps_system = 0;

  dtmf_decode1 = dtmf_decode2 = dtmf_decode3 = dtmf_decode4 = dtmf_decode5 = dtmf_decode6 =
      dtmf_decode6 = dtmf_decode7 = dtmf_decode8 = 0;

  squelch = 1;

  rx_frequency = 0x00000040;
  tx_frequency = 0x00000040;

  ctcss_dcs_receive = 0xffff;
  ctcss_dcs_transmit = 0xffff;

  rx_signaling_syst = 0;
  tx_signaling_syst = 0;

  power = POWER_HIGH;
  _unused30_2 = 0b111111;

  send_gps_info = 1;
  recv_gps_info = 1;
  allow_interrupt = 1;
  dcdm_switch_dis = 1;
  leader_ms = 1;
  _unused31_5 = 0b111;

  memset(name, 0x00, sizeof(name));
}

double
UV390Codeplug::channel_t::getRXFrequency() const {
  return decode_frequency(rx_frequency);
}

void
UV390Codeplug::channel_t::setRXFrequency(double freq) {
  rx_frequency = encode_frequency(freq);
}

double
UV390Codeplug::channel_t::getTXFrequency() const {
  return decode_frequency(tx_frequency);
}

void
UV390Codeplug::channel_t::setTXFrequency(double freq) {
  tx_frequency = encode_frequency(freq);
}

QString
UV390Codeplug::channel_t::getName() const {
  return decode_unicode(name, 16);
}

void
UV390Codeplug::channel_t::setName(const QString &n) {
  encode_unicode(name, n, 16);
}

float
UV390Codeplug::channel_t::getRXTone() const {
  return decode_ctcss_tone_table(ctcss_dcs_receive);
}

void
UV390Codeplug::channel_t::setRXTone(float freq) {
  ctcss_dcs_receive = encode_ctcss_tone_table(freq);
}

float
UV390Codeplug::channel_t::getTXTone() const {
  return decode_ctcss_tone_table(ctcss_dcs_transmit);
}

void
UV390Codeplug::channel_t::setTXTone(float freq)  {
  ctcss_dcs_transmit = encode_ctcss_tone_table(freq);
}

Channel *
UV390Codeplug::channel_t::toChannelObj() const {
  if (! isValid())
    return nullptr;

  if (MODE_ANALOG == channel_mode) {
    Channel::Power pwr =
        (POWER_HIGH == power) ? Channel::HighPower : Channel::LowPower;

    AnalogChannel::Admit admit_crit;
    switch(admit_criteria) {
      case ADMIT_ALWAYS: admit_crit = AnalogChannel::AdmitNone; break;
      case ADMIT_TONE: admit_crit = AnalogChannel::AdmitTone; break;
      case ADMIT_CH_FREE: admit_crit = AnalogChannel::AdmitFree; break;
      default: admit_crit = AnalogChannel::AdmitFree; break;
    }

    AnalogChannel::Bandwidth bw =
        (BW_12_5_KHZ == bandwidth) ? AnalogChannel::BWNarrow : AnalogChannel::BWWide;

    return new AnalogChannel(getName(), getRXFrequency(), getTXFrequency(), pwr, (tot*15), rx_only,
                             admit_crit, squelch, getRXTone(), getTXTone(), bw, nullptr);
  } else if (MODE_DIGITAL == channel_mode) {
    Channel::Power pwr =
        (POWER_HIGH == power) ? Channel::HighPower : Channel::LowPower;

    DigitalChannel::Admit admit_crit;
    switch(admit_criteria) {
      case ADMIT_ALWAYS: admit_crit = DigitalChannel::AdmitNone; break;
      case ADMIT_CH_FREE: admit_crit = DigitalChannel::AdmitFree; break;
      case ADMIT_COLOR: admit_crit = DigitalChannel::AdmitColorCode; break;
      default: admit_crit = DigitalChannel::AdmitFree; break;
    }

    DigitalChannel::TimeSlot slot =
        (1 == time_slot) ? DigitalChannel::TimeSlot1 : DigitalChannel::TimeSlot2;

    return new DigitalChannel(getName(), getRXFrequency(), getTXFrequency(), pwr, (tot*15),
                              rx_only, admit_crit, color_code, slot, nullptr, nullptr, nullptr, nullptr);
  }

  return nullptr;
}

bool
UV390Codeplug::channel_t::linkChannelObj(Channel *c, Config *conf) const {
  if (! isValid())
    return false;

  if (MODE_ANALOG == channel_mode) {
    AnalogChannel *ac = dynamic_cast<AnalogChannel *>(c);
    if (! ac) return false;
    if (scan_list_index) {
      if ((scan_list_index-1) >= conf->scanlists()->count())
        return false;
      ac->setScanList(conf->scanlists()->scanlist(scan_list_index-1));
    }
    return true;
  } else if (MODE_DIGITAL == channel_mode) {
    DigitalChannel *dc = dynamic_cast<DigitalChannel *>(c);
    if (! dc) return false;
    if (scan_list_index) {
      if ((scan_list_index-1) >= conf->scanlists()->count())
        return false;
      dc->setScanList(conf->scanlists()->scanlist(scan_list_index-1));
    }
    if (contact_name_index) {
      if ((contact_name_index-1) >= conf->contacts()->digitalCount())
        return false;
      dc->setTXContact(conf->contacts()->digitalContact(contact_name_index-1));
    }
    if (group_list_index) {
      if ((group_list_index-1) >= conf->rxGroupLists()->count())
        return false;
      dc->setRXGroupList(conf->rxGroupLists()->list(group_list_index-1));
    }
    if (gps_system) {
      if ((gps_system-1) >= conf->gpsSystems()->count())
        return false;
      dc->setGPSSystem(conf->gpsSystems()->gpsSystem(gps_system-1));
    }
    return true;
  }

  return false;
}

void
UV390Codeplug::channel_t::fromChannelObj(const Channel *chan, const Config *conf) {
  setName(chan->name());
  setRXFrequency(chan->rxFrequency());
  setTXFrequency(chan->txFrequency());
  rx_only = chan->rxOnly() ? 1 : 0;
  tot     = chan->txTimeout()/15;
  scan_list_index = conf->scanlists()->indexOf(chan->scanList())+1;
  power = (Channel::HighPower == chan->power()) ? POWER_HIGH : POWER_LOW;

  if (chan->is<const DigitalChannel>()) {
    const DigitalChannel *dchan = chan->as<const DigitalChannel>();
    channel_mode = MODE_DIGITAL;
    switch (dchan->admit()) {
    case DigitalChannel::AdmitNone: admit_criteria = ADMIT_ALWAYS; break;
    case DigitalChannel::AdmitFree: admit_criteria = ADMIT_CH_FREE; break;
    case DigitalChannel::AdmitColorCode: admit_criteria = ADMIT_COLOR; break;
    }
    color_code = dchan->colorCode();
    time_slot = (DigitalChannel::TimeSlot1 == dchan->timeslot()) ? 1 : 2;
    group_list_index = conf->rxGroupLists()->indexOf(dchan->rxGroupList()) + 1;
    contact_name_index = conf->contacts()->indexOfDigital(dchan->txContact()) + 1;
    squelch = 0;
    bandwidth = BW_12_5_KHZ;
    ctcss_dcs_receive = 0xffff;
    ctcss_dcs_transmit = 0xffff;
    if (dchan->gpsSystem()) {
      gps_system = conf->gpsSystems()->indexOf(dchan->gpsSystem())+1;
      send_gps_info = 0;
      recv_gps_info = 0;
    }
  } else if (chan->is<AnalogChannel>()) {
    const AnalogChannel *achan = chan->as<const AnalogChannel>();
    channel_mode = MODE_ANALOG;
    bandwidth = ((AnalogChannel::BWNarrow == achan->bandwidth()) ? BW_12_5_KHZ : BW_25_KHZ);
    squelch = achan->squelch();
    switch (achan->admit()) {
    case AnalogChannel::AdmitNone: admit_criteria = ADMIT_ALWAYS; break;
    case AnalogChannel::AdmitFree: admit_criteria = ADMIT_CH_FREE; break;
    case AnalogChannel::AdmitTone: admit_criteria = ADMIT_TONE; break;
    }
    ctcss_dcs_receive = encode_ctcss_tone_table(achan->rxTone());
    ctcss_dcs_transmit = encode_ctcss_tone_table(achan->txTone());
    group_list_index = 0;
    contact_name_index = 0;
  }
}


/* ******************************************************************************************** *
 * Implementation of UV390Codeplug::contact_t
 * ******************************************************************************************** */
UV390Codeplug::contact_t::contact_t() {
  clear();
}

bool
UV390Codeplug::contact_t::isValid() const {
  return (0 != type) && (name[0] != 0x0000) && (name[0] != 0xffff);
}

void
UV390Codeplug::contact_t::clear() {
  memset(id, 0xff, sizeof(id));
  type = 0;
  receive_tone = 0;
  _unused2 = 3;
  memset(name, 0x00, sizeof(name));
}

uint32_t
UV390Codeplug::contact_t::getId() const {
  return decode_dmr_id_bin(id);
}

void
UV390Codeplug::contact_t::setId(uint32_t num) {
  encode_dmr_id_bin(id, num);
}

QString
UV390Codeplug::contact_t::getName() const {
  return decode_unicode(name, 16);
}

void
UV390Codeplug::contact_t::setName(const QString &n) {
  encode_unicode(name, n, 16);
}

DigitalContact *
UV390Codeplug::contact_t::toContactObj() const {
  DigitalContact::Type ctype;
  switch (type) {
    case CALL_GROUP: ctype = DigitalContact::GroupCall; break;
    case CALL_PRIVATE: ctype = DigitalContact::PrivateCall; break;
    case CALL_ALL: ctype = DigitalContact::AllCall; break;
    default: ctype = DigitalContact::PrivateCall; break;
  }
  return new DigitalContact(ctype, getName(), getId(), receive_tone);
}

void
UV390Codeplug::contact_t::fromContactObj(const DigitalContact *cont, const Config *conf) {
  Q_UNUSED(conf);
  if (nullptr == cont)
    return;
  setId(cont->number());
  setName(cont->name());
  switch (cont->type()) {
    case DigitalContact::PrivateCall: type = CALL_PRIVATE; break;
    case DigitalContact::GroupCall: type = CALL_GROUP; break;
    case DigitalContact::AllCall: type = CALL_ALL; break;
  }
  receive_tone = cont->rxTone() ? 1 : 0;
}


/* ******************************************************************************************** *
 * Implementation of UV390Codeplug::zone_t
 * ******************************************************************************************** */
bool
UV390Codeplug::zone_t::isValid() const {
  return ((0 != name[0]) && (0xffff != name[0]));
}

UV390Codeplug::zone_t::zone_t() {
  clear();
}

void
UV390Codeplug::zone_t::clear() {
  memset(name, 0, sizeof(name));
  memset(member_a, 0, sizeof(member_a));
}

QString
UV390Codeplug::zone_t::getName() const {
  return decode_unicode(name, 16);
}

void
UV390Codeplug::zone_t::setName(const QString &n) {
  encode_unicode(name, n, 16);
}

Zone *
UV390Codeplug::zone_t::toZoneObj() const {
  if (!isValid())
    return nullptr;

  return new Zone(getName());
}

bool
UV390Codeplug::zone_t::linkZone(Zone *zone, Config *conf) const {
  if (! isValid())
    return false;

  for (int i=0; ((i<16) && member_a[i]); i++) {
    if (member_a[i]>conf->channelList()->count())
      return false;
    zone->A()->addChannel(conf->channelList()->channel(member_a[i]-1));
  }

  return true;
}

void
UV390Codeplug::zone_t::fromZoneObj(const Zone *zone, const Config *conf) {
  setName(zone->name());
  for (int i=0; i<16; i++) {
    if (i < zone->A()->count())
      member_a[i] = conf->channelList()->indexOf(zone->A()->channel(i))+1;
    else
      member_a[i] = 0;
  }
}


/* ******************************************************************************************** *
 * Implementation of UV390Codeplug::zone_ext_t
 * ******************************************************************************************** */
UV390Codeplug::zone_ext_t::zone_ext_t() {
  clear();
}

void
UV390Codeplug::zone_ext_t::clear() {
  memset(ext_a, 0, sizeof(ext_a));
  memset(member_b, 0, sizeof(member_b));
}

bool
UV390Codeplug::zone_ext_t::linkZone(Zone *zone, Config *conf) const {
  for (int i=0; (i<48) && ext_a[i]; i++) {
    if ((ext_a[i]-1)>=conf->channelList()->count())
      return false;
    zone->A()->addChannel(conf->channelList()->channel(ext_a[i]-1));
  }
  for (int i=0; (i<64) && member_b[i]; i++) {
    if ((member_b[i]-1)>=conf->channelList()->count())
      return false;
    zone->B()->addChannel(conf->channelList()->channel(member_b[i]-1));
  }

  return true;
}

void
UV390Codeplug::zone_ext_t::fromZoneObj(const Zone *zone, const Config *config) {
  for (int i=16; i<64; i++) {
    if (i < zone->A()->count())
      ext_a[i-16] = config->channelList()->indexOf(zone->A()->channel(i))+1;
    else
      ext_a[i-16] = 0;
  }
  for (int i=0; i<64; i++) {
    if (i < zone->B()->count())
      member_b[i] = config->channelList()->indexOf(zone->B()->channel(i))+1;
    else
      member_b[i] = 0;
  }
}


/* ******************************************************************************************** *
 * Implementation of UV390Codeplug::grouplist_t
 * ******************************************************************************************** */
UV390Codeplug::grouplist_t::grouplist_t() {
  clear();
}

bool
UV390Codeplug::grouplist_t::isValid() const {
  return (0 != name[0]) && (0xffff != name[0]);
}

void
UV390Codeplug::grouplist_t::clear() {
  memset(name, 0, sizeof(name));
  memset(member, 0, sizeof(member));
}

QString
UV390Codeplug::grouplist_t::getName() const {
  return decode_unicode(name, 16);
}

void
UV390Codeplug::grouplist_t::setName(const QString &n) {
  encode_unicode(name, n, 16);
}

RXGroupList *
UV390Codeplug::grouplist_t::toRXGroupListObj() const {
  if (! isValid())
    return nullptr;
  return new RXGroupList(getName());
}

bool
UV390Codeplug::grouplist_t::linkRXGroupList(RXGroupList *grp, Config *conf) const {
  if (! isValid())
    return false;

  for (int i=0; (i<32)&&(member[i]); i++) {
    if ((member[i]-1) >= conf->contacts()->digitalCount())
      return false;
    grp->addContact(conf->contacts()->digitalContact(member[i]-1));
  }

  return true;
}

void
UV390Codeplug::grouplist_t::fromRXGroupListObj(const RXGroupList *grp, const Config *conf) {
  setName(grp->name());
  for (int i=0; i<32; i++) {
    if (i<grp->count())
      member[i] = conf->contacts()->indexOfDigital(grp->contact(i)) + 1;
    else
      member[i] = 0;
  }
}


/* ******************************************************************************************** *
 * Implementation of UV390Codeplug::scanlist_t
 * ******************************************************************************************** */
UV390Codeplug::scanlist_t::scanlist_t() {
  clear();
}

bool
UV390Codeplug::scanlist_t::isValid() const {
  return (0 != name[0]) && (0xffff != name[0]);
}

void
UV390Codeplug::scanlist_t::clear() {
  memset(name, 0, sizeof(name));
  priority_ch1 = 0xffff;
  priority_ch2 = 0xffff;
  tx_designated_ch = 0xffff;
  _unused38 = 0xf1;
  sign_hold_time = 0x14;
  prio_sample_time = 0x08;
  _unused41 = 0xff;
  memset(member, 0, sizeof(member));
}

QString
UV390Codeplug::scanlist_t::getName() const {
  return decode_unicode(name, 16);
}

void
UV390Codeplug::scanlist_t::setName(const QString &n) {
  encode_unicode(name, n, 16);
}

ScanList *
UV390Codeplug::scanlist_t::toScanListObj() const {
  if (! isValid())
    return nullptr;
  return new ScanList(getName());
}

bool
UV390Codeplug::scanlist_t::linkScanListObj(ScanList *l, Config *conf) const {
  if (! isValid())
    return false;
  for (int i=0; (i<31)&&(member[i]); i++) {
    if ((member[i]-1) >= conf->channelList()->count())
      return false;
    l->addChannel(conf->channelList()->channel(member[i]-1));
  }
  return false;
}

void
UV390Codeplug::scanlist_t::fromScanListObj(const ScanList *lst, const Config *conf) {
  setName(lst->name());
  priority_ch1 = conf->channelList()->indexOf(lst->priorityChannel())+1;
  priority_ch2 = conf->channelList()->indexOf(lst->secPriorityChannel())+1;

  for (int i=0; i<31; i++) {
    if (i < lst->count())
      member[i] = conf->channelList()->indexOf(lst->channel(i))+1;
    else
      member[i] = 0;
  }
}


/* ******************************************************************************************** *
 * Implementation of UV390Codeplug::general_settings_t
 * ******************************************************************************************** */
UV390Codeplug::general_settings_t::general_settings_t() {
  clear();
}

void
UV390Codeplug::general_settings_t::clear() {
  memset(intro_line1, 0, sizeof(intro_line1));
  memset(intro_line2, 0, sizeof(intro_line2));
  memset(_unused40, 0xff, sizeof(_unused40));

  _unused64_0 = 1;
  disable_all_leds_inv = 1;
  _unused64_3 = 1;
  monitor_type = 1;
  _unused64_6 = 1;
  tx_mode = 3;

  save_preamble = 1;
  save_mode_receive = 1;
  disable_all_tones_inv = 1;
  _unused65_3 = 1;
  ch_free_indication_tone = 1;
  pw_and_lock_enable = 1;
  talk_permit_tone = 1;

  _unused66_0 = 0;
  channel_voice_announce = 0;
  _unused66_2 = 0;
  _unused66_3 = 1;
  intro_picture = 1;
  keypad_tones = 0;
  _unused66_6 = 0b11;

  _unused67_0 = 0b11;
  mode_select_a = 1;
  _unused67_1 = 0b1111;
  mode_select_b = 1;

  memset(radio_id, 0, sizeof(radio_id));
  _unused71 = 0;

  tx_preamble_duration = 0x0a;
  group_call_hang_time = 0x28;
  private_call_hang_time = 0x28;
  vox_sensitivity = 0x03;
  _unused76 = 0x00;
  _unused77 = 0x00;
  rx_low_battery_interval = 0x18;
  call_alert_tone_duration = 0x00;
  lone_worker_response_time = 0x01;
  lone_worker_reminder_time = 0x0a;
  _unused82 = 0x00;
  scan_digital_hang_time = 0x0a;
  scan_analog_hang_time = 0x0a;

  backlight_time = 0x02;
  _unused85_2 = 0;

  set_keypad_lock_time = 0xff;
  mode = 0xff;

  power_on_password = 0;
  radio_prog_password = 0xffffffff;

  memset(pc_prog_password, 0xff, sizeof(pc_prog_password));
  memset(_unused104, 0xff, sizeof(_unused104));

  group_call_match = 1;
  private_call_match = 1;
  _unused107_4 = 1;
  setTimeZone();

  _unused108 = 0xffffffff;

  memset(radio_name, 0x00, sizeof(radio_name));

  channel_hang_time = 0x1e;

  _unused145 = 0xff;

  _unused146_0 = 0b11;
  public_zone = 1;
  _unused146_4 = 0b11111;

  _unused147 = 0xff;

  radio_id1[0] = 0x01; radio_id1[1] = radio_id1[2] = 0x00;
  _pad151 = 0x00;
  radio_id2[0] = 0x01; radio_id2[1] = radio_id2[2] = 0x00;
  _pad155 = 0x00;
  radio_id3[0] = 0x01; radio_id3[1] = radio_id3[2] = 0x00;
  _pad159 = 0x00;

  _unused_160_0 = 0b111;
  mic_level = 0;
  edit_radio_id = 0;
  _unused_160_7 = 1;

  memset(_reserved_161, 0xff, sizeof(_reserved_161));
}

QTimeZone
UV390Codeplug::general_settings_t::getTimeZone() const {
  return QTimeZone((int(time_zone)-12)*3600);
}

void
UV390Codeplug::general_settings_t::setTimeZone() {
  setTimeZone(QTimeZone::systemTimeZone());
}

void
UV390Codeplug::general_settings_t::setTimeZone(const QTimeZone &ts) {
  time_zone = 12 + ts.offsetFromUtc(QDateTime::currentDateTime())/3600;
}

uint32_t
UV390Codeplug::general_settings_t::getRadioId() const {
  return decode_dmr_id_bin(radio_id);
}
void
UV390Codeplug::general_settings_t::setRadioId(uint32_t num) {
  encode_dmr_id_bin(radio_id, num);
}

uint32_t
UV390Codeplug::general_settings_t::getRadioId1() const {
  return decode_dmr_id_bin(radio_id1);
}
void
UV390Codeplug::general_settings_t::setRadioId1(uint32_t num) {
  encode_dmr_id_bin(radio_id1, num);
}

uint32_t
UV390Codeplug::general_settings_t::getRadioId2() const {
  return decode_dmr_id_bin(radio_id2);
}
void
UV390Codeplug::general_settings_t::setRadioId2(uint32_t num) {
  encode_dmr_id_bin(radio_id2, num);
}

uint32_t
UV390Codeplug::general_settings_t::getRadioId3() const {
  return decode_dmr_id_bin(radio_id3);
}
void
UV390Codeplug::general_settings_t::setRadioId3(uint32_t num) {
  encode_dmr_id_bin(radio_id3, num);
}

QString
UV390Codeplug::general_settings_t::getName() const {
  return decode_unicode(radio_name, 16);
}
void
UV390Codeplug::general_settings_t::setName(const QString &name) {
  encode_unicode(radio_name, name, 16);
}

QString
UV390Codeplug::general_settings_t::getIntroLine1() const {
  return decode_unicode(intro_line1, 10);
}
void
UV390Codeplug::general_settings_t::setIntroLine1(const QString &txt) {
  encode_unicode(intro_line1, txt, 10);
}

QString
UV390Codeplug::general_settings_t::getIntroLine2() const {
  return decode_unicode(intro_line2, 10);
}
void
UV390Codeplug::general_settings_t::setIntroLine2(const QString &txt) {
  encode_unicode(intro_line2, txt, 10);
}

bool
UV390Codeplug::general_settings_t::updateConfigObj(Config *conf) const {
  conf->setId(getRadioId());
  conf->setName(getName());
  conf->setIntroLine1(getIntroLine1());
  conf->setIntroLine2(getIntroLine2());
  conf->setMicLevel(1+mic_level*2);
  conf->setSpeech(1 == channel_voice_announce);
  return true;
}

void
UV390Codeplug::general_settings_t::fromConfigObj(const Config *conf) {
  setName(conf->name());
  setRadioId(conf->id());
  setIntroLine1(conf->introLine1());
  setIntroLine2(conf->introLine2());
  setTimeZone();
  mic_level = conf->micLevel()/2;
  channel_voice_announce = (conf->speech() ? 1 : 0);
}


/* ******************************************************************************************** *
 * Implementation of UV390Codeplug::message_t
 * ******************************************************************************************** */
UV390Codeplug::message_t::message_t() {
  clear();
}

void
UV390Codeplug::message_t::clear() {
  memset(text, 0, sizeof(text));
}

bool
UV390Codeplug::message_t::isValid() const {
  return 0 != text[0];
}

QString
UV390Codeplug::message_t::getText() const {
  QString txt; txt.reserve(144);
  for (int i=0; (i<144) && (0!=text[i]); i++)
    txt.append(QChar(text[i]));
  return txt;
}

void
UV390Codeplug::message_t::setText(const QString text) {
  memset(this->text, 0, 288);
  for (int i=0; (i<128) && (i<text.size()); i++)
    this->text[i] = text.at(i).unicode();
}

/* ******************************************************************************************** *
 * Implementation of UV390Codeplug::timestamp_t
 * ******************************************************************************************** */
UV390Codeplug::timestamp_t::timestamp_t() {
  set();
}

void
UV390Codeplug::timestamp_t::set() {
  static const char *vtable = "0123456789:;<=>?";
  _pad0 = 0xff;
  setTimestamp();
  cps_version[0] = vtable[std::min(9,VERSION_MAJOR/10)];
  cps_version[1] = vtable[VERSION_MAJOR%10];
  cps_version[2] = vtable[std::min(9,VERSION_MINOR/10)];
  cps_version[3] = vtable[VERSION_MINOR%10];
}

void
UV390Codeplug::timestamp_t::setTimestamp(const QDateTime &dt) {
  date[0] = ((dt.date().year()/1000) << 4)     | ((dt.date().year()%1000)/100);
  date[1] = (((dt.date().year()%100)/10) << 4) | (dt.date().year()%10);
  date[2] = ((dt.date().month()/10) << 4)      | (dt.date().month()%10);
  date[3] = ((dt.date().day()/10) << 4)        | (dt.date().day()%10);
  date[4] = ((dt.time().hour()/10) << 4)       | (dt.time().hour()%10);
  date[5] = ((dt.time().minute()/10) << 4)     | (dt.time().minute()%10);
  date[6] = ((dt.time().second()/10) << 4)     | (dt.time().second()%10);
}

QDateTime
UV390Codeplug::timestamp_t::getTimestamp() const {
  int year   = (date[0]>>4)*1000 + (date[0]&0xf)*100 + (date[1]>>4)*10 + (date[1]&0xf);
  int month  = (date[2]>>4)*10 + (date[2]&0xf);
  int day    = (date[3]>>4)*10 + (date[3]&0xf);
  int hour   = (date[4]>>4)*10 + (date[4]&0xf);
  int minute = (date[5]>>4)*10 + (date[5]&0xf);
  int second = (date[6]>>4)*10 + (date[6]&0xf);
  return QDateTime(QDate(year, month, day), QTime(hour, minute, second), Qt::UTC);
}


/* ******************************************************************************************** *
 * Implementation of UV390Codeplug::gpssystem_t
 * ******************************************************************************************** */
UV390Codeplug::gpssystem_t::gpssystem_t() {
  clear();
}

void
UV390Codeplug::gpssystem_t::clear() {
  revert_channel = 0;
  repeat_interval = 0;
  destination = 0;
  _unused_3 = 0xff;
  memset(_unused_6, 0xff, 10);
}

bool
UV390Codeplug::gpssystem_t::isValid() const {
  return repeat_interval != 0;
}

uint
UV390Codeplug::gpssystem_t::repeatInterval() const {
  return uint(repeat_interval)*30;
}

void
UV390Codeplug::gpssystem_t::setRepeatInterval(uint interval) {
  interval = std::min(interval, 7200u);
  repeat_interval = interval/30;
}

GPSSystem *
UV390Codeplug::gpssystem_t::toGPSSystemObj() const {
  return new GPSSystem("GPS System", nullptr, nullptr, repeatInterval());
}

bool
UV390Codeplug::gpssystem_t::linkGPSSystemObj(GPSSystem *gs, Config *conf) const {
  if ((! destination) || ((destination-1)>=conf->contacts()->digitalCount()))
    return false;
  gs->setContact(conf->contacts()->digitalContact(destination-1));

  if (0 < revert_channel) {
    if ( ((revert_channel-1)>=conf->channelList()->count()) ||
         (conf->channelList()->channel(revert_channel-1)->is<DigitalChannel>()))
      return false;
    gs->setRevertChannel(conf->channelList()->channel(revert_channel-1)->as<DigitalChannel>());
  }

  return true;
}

void
UV390Codeplug::gpssystem_t::fromGPSSystemObj(const GPSSystem *l, const Config *conf) {
  clear();

  if (! l->hasContact())
    return;

  if (l->hasRevertChannel())
    revert_channel = conf->channelList()->indexOf(l->revertChannel())+1;
  else
    revert_channel = 0;

  destination = conf->contacts()->indexOfDigital(l->contact())+1;
  setRepeatInterval(l->period());
}


/* ******************************************************************************************** *
 * Implementation of UV390Codeplug::buttons_t
 * ******************************************************************************************** */
UV390Codeplug::buttons_t::buttons_t()
{
  clear();
}

void
UV390Codeplug::buttons_t::clear() {
  _reserved_0 = 0x0000;
  side_button_1 = Disabled;
  side_button_1_long = Tone1750Hz;
  side_button_2 = MonitorToggle;
  side_button_2_long = Disabled;
  memset(_unused_6, 0x00, sizeof(_unused_6));
  _unknown_16 = 0x01;
  long_press_dur = 0x04;
  _unused_18 = 0xffff;
  for (int i=0; i<6; i++)
    one_touch[i].clear();
  memset(_unused_42, 0x00, sizeof(_unused_42));
}


/* ******************************************************************************************** *
 * Implementation of UV390Codeplug::emergency_t
 * ******************************************************************************************** */
UV390Codeplug::emergency_t::emergency_t() {
  clear();
}

void
UV390Codeplug::emergency_t::clear() {
  radio_dis_dec = 1;
  remote_mon_decode = 0;
  em_remote_mon_decode = 1;
  _unknown_0_3 = 0b11111;

  remote_mon_dur = 1;
  tx_sync_wakeup_tot = 5;
  tx_wakeup_msg_limit = 2;
  memset(_unused_4, 0xff, sizeof(_unused_4));

  for (int i=0; i<32; i++)
    systems[i].clear();
}

/* ******************************************************************************************** *
 * Implementation of UV390Codeplug::emergency_t::system_t
 * ******************************************************************************************** */
UV390Codeplug::emergency_t::system_t::system_t() {
  clear();
}

void
UV390Codeplug::emergency_t::system_t::clear() {
  memset(name, 0x00, sizeof(name));
  alarm_type = DISABLED;
  _unknown_32_2 = 0b11;
  alarm_mode = ALARM;
  _unknown_32_6 = 0b01;
  impolite_retires = 15;
  polite_retries = 5;
  hot_mic_dur = 10;
  revert_channel = 0x0000;
  _unused_37 = 0xffff;
}

bool
UV390Codeplug::emergency_t::system_t::isValid() const {
  return 0x0000 != revert_channel;
}


/* ******************************************************************************************** *
 * Implementation of UV390Codeplug::privacy_t
 * ******************************************************************************************** */
UV390Codeplug::privacy_t::privacy_t() {
  clear();
}

void
UV390Codeplug::privacy_t::clear() {
  memset(enhanced_keys, 0xff, sizeof(enhanced_keys));
  memset(_reserved, 0xff, sizeof(_reserved));
  memset(basic_keys, 0xff, sizeof(basic_keys));
}


/* ******************************************************************************************** *
 * Implementation of UV390Codeplug::buttons_t::one_touch_t
 * ******************************************************************************************** */
void
UV390Codeplug::buttons_t::one_touch_t::clear() {
  action        = CALL;
  type          = Disabled;
  _reserved_0_6 = 0b11;
  message       = 0;
  contact       = 0;
}


/* ******************************************************************************************** *
 * Implementation of UV390Codeplug::menu_t
 * ******************************************************************************************** */
UV390Codeplug::menu_t::menu_t() {
  clear();
}

void
UV390Codeplug::menu_t::clear() {
  hangtime           = 0;

  text_message       = 1;
  call_alert         = 1;
  contacts_edit      = 1;
  manual_dial        = 1;
  radio_check        = 0;
  remote_monitor     = 0;
  radio_disable      = 0;
  radio_enable       = 0;

  _reserved_2_0      = 1;
  scan               = 1;
  edit_scan_list     = 1;
  calllog_missed     = 1;
  calllog_answered   = 1;
  calllog_outgoing   = 1;
  talkaround         = 0;
  tone_or_alert      = 1;

  power              = 1;
  backlight          = 1;
  intro_screen       = 1;
  keypad_lock        = 1;
  led_indicator      = 1;
  squelch            = 1;
  _unknown_3_6       = 0;
  vox                = 1;

  password           = 0;
  display_mode       = 1;
  hide_prog_radio    = 0;
  _unknown_4_3       = 1;
  hide_gps           = 0;
  record_switch      = 1;
  _unknown_4_6       = 0b11;

  _unknown_5_0       = 0b11;
  group_call_match   = 1;
  private_call_match = 1;
  menu_hangtime      = 1;
  tx_mode            = 1;
  zone               = 1;
  new_zone           = 1;

  edit_zone          = 1;
  new_scan_list      = 1;
  _unknown_6_2       = 0b111111;

  memset(_reserved_7, 0xff, sizeof(_reserved_7));
}

/* ******************************************************************************************** *
 * Implementation of UV390Codeplug::callsign_db_t
 * ******************************************************************************************** */
UV390Codeplug::callsign_db_t::entry_t::entry_t() {
  clear();
}

void
UV390Codeplug::callsign_db_t::entry_t::clear() {
  id_high = 0xfff;
  index   = 0xfffff;
}

bool
UV390Codeplug::callsign_db_t::entry_t::isValid() const {
  return (0xfff != id_high) && (0xfffff != index);
}

UV390Codeplug::callsign_db_t::callsign_t::callsign_t() {
  clear();
}

void
UV390Codeplug::callsign_db_t::callsign_t::clear() {
  memset(dmrid, 0xff, sizeof(dmrid));
  _unused = 0xff;
  memset(callsign, 0xff, sizeof(callsign));
  memset(name, 0xff, sizeof(name));
}

bool
UV390Codeplug::callsign_db_t::callsign_t::isValid() const {
  return (0xff != dmrid[0]) && (0xff != dmrid[1]) && (0xff != dmrid[2]);
}

void
UV390Codeplug::callsign_db_t::callsign_t::setID(uint32_t dmrid) {
  encode_dmr_id_bcd(this->dmrid, dmrid);
}

void
UV390Codeplug::callsign_db_t::callsign_t::setCall(const QString &call) {
  memset(callsign, 0xff, sizeof(callsign));
  strncpy(callsign, call.toStdString().c_str(), sizeof(callsign));
}

void
UV390Codeplug::callsign_db_t::callsign_t::setName(const QString &name) {
  memset(this->name, 0xff, sizeof(this->name));
  strncpy(this->name, name.toStdString().c_str(), sizeof(this->name));
}

void
UV390Codeplug::callsign_db_t::callsign_t::fromUser(const UserDatabase::User &user) {
  setID(user.id);
  setCall(user.call);
  QString name = user.name;
  if (! user.surname.isEmpty())
    name += " " + user.surname;
  if (! user.country.isEmpty())
    name += ", " + user.country;
  setName(name);
}

UV390Codeplug::callsign_db_t::callsign_db_t() {
  clear();
}

void
UV390Codeplug::callsign_db_t::clear() {
  n = 0;
  for (int i=0; i<4096; i++)
    index[i].clear();
  for (int i=0; i<122197; i++)
    db[i].clear();
}

void
UV390Codeplug::callsign_db_t::fromUserDB(const UserDatabase *db) {
  // Clear database and index
  clear();
  // Limit users to 122197 entries
  n = std::min(122197ll, db->count());
  // If there are no users -> done.
  if (0 == n)
    return;

  // Select n users and sort them in ascending order of their IDs
  QVector<UserDatabase::User> users;
  for (int i=0; i<n; i++)
    users.append(db->user(i));
  std::sort(users.begin(), users.end(),
            [](const UserDatabase::User &a, const UserDatabase::User &b) { return a.id < b.id; });

  // First index entry
  int  j = 0;
  uint cidh = (users[0].id >> 12);
  this->index[j].id_high = cidh;
  this->index[j].index = 1;
  j++;

  // Store users and update index
  for (int i=0; i<n; i++) {
    this->db[i].fromUser(users[i]);
    uint idh = (users[i].id >> 12);
    if (idh != cidh) {
      this->index[j].id_high = idh;
      this->index[j].index = i+1;
      cidh = idh; j++;
    }
  }
}


/* ******************************************************************************************** *
 * Implementation of UV390Codeplug
 * ******************************************************************************************** */
UV390Codeplug::UV390Codeplug(QObject *parent)
  : CodePlug(parent)
{
  addImage("TYT MD-UV390 Codeplug");
  image(0).addElement(0x002800, 0x3e000);
  image(0).addElement(0x110800, 0x90000);

  // Clear entire codeplug
  clear();
}

void
UV390Codeplug::clear()
{
  // Clear entire images
  memset(data(0x002800), 0xff, 0x3e000);
  memset(data(0x110800), 0xff, 0x90000);

  // Clear timestamp
  ((timestamp_t *)(data(OFFSET_TIMESTMP)))->set();

  // Clear general config
  ((general_settings_t *)(data(OFFSET_SETTINGS)))->clear();

  // Clear menu settings
  ((menu_t *)(data(OFFSET_MENU)))->clear();

  // Clear button settings
  ((buttons_t *)(data(OFFSET_BUTTONS)))->clear();

  // Clear text messages
  for (int i=0; i<NMESSAGES; i++)
    ((message_t *)(data(OFFSET_MSG+i*sizeof(message_t))))->clear();

  // Clear privacy keys
  ((privacy_t *)(data(OFFSET_PRIVACY)))->clear();

  // Clear emergency systems
  ((emergency_t *)(data(OFFSET_EMERGENCY)))->clear();

  // Clear RX group lists
  for (int i=0; i<NGLISTS; i++)
    ((grouplist_t *)(data(OFFSET_GLISTS+i*sizeof(grouplist_t))))->clear();

  // Clear zones & zone extensions
  for (int i=0; i<NZONES; i++) {
    ((zone_t *)(data(OFFSET_ZONES+i*sizeof(zone_t))))->clear();
    ((zone_ext_t*)(data(OFFSET_ZONEXT+i*sizeof(zone_ext_t))))->clear();
  }

  // Clear scan lists
  for (int i=0; i<NSCANL; i++)
    ((scanlist_t *)(data(OFFSET_SCANL+i*sizeof(scanlist_t))))->clear();

  // Clear VFO A & B settings.
  ((channel_t *)(data(OFFSET_VFO_A)))->clear();
  ((channel_t *)(data(OFFSET_VFO_B)))->clear();

  // Clear GPS systems
  for (int i=0; i<NGPSSYSTEMS; i++)
    ((gpssystem_t *)(data(OFFSET_GPS_SYS+i*sizeof(gpssystem_t))))->clear();

  // Clear channels
  for (int i=0; i<NCHAN; i++)
    ((channel_t *)(data(OFFSET_CHANNELS+i*sizeof(channel_t))))->clear();

  // Clear contacts
  for (int i=0; i<NCONTACTS; i++)
    ((contact_t *)(data(OFFSET_CONTACTS+i*sizeof(contact_t))))->clear();
}

bool
UV390Codeplug::encode(Config *config, UserDatabase *users) {
  // Set timestamp
  ((timestamp_t *)(data(OFFSET_TIMESTMP)))->set();

  // General config
  general_settings_t *genset = (general_settings_t *)(data(OFFSET_SETTINGS));
  genset->fromConfigObj(config);

  // Define Contacts
  for (int i=0; i<NCONTACTS; i++) {
    contact_t *cont = (contact_t *)(data(OFFSET_CONTACTS+i*sizeof(contact_t)));
    if (i < config->contacts()->digitalCount())
      cont->fromContactObj(config->contacts()->digitalContact(i), config);
    else
      cont->clear();
  }

  // Define RX GroupLists
  for (int i=0; i<NGLISTS; i++) {
    grouplist_t *glist = (grouplist_t *)(data(OFFSET_GLISTS+i*sizeof(grouplist_t)));
    if (i < config->rxGroupLists()->count())
      glist->fromRXGroupListObj(config->rxGroupLists()->list(i), config);
    else
      glist->clear();
  }

  // Define Channels
  for (int i=0; i<NCHAN; i++) {
    channel_t *chan = (channel_t *)(data(OFFSET_CHANNELS+i*sizeof(channel_t)));
    if (i < config->channelList()->count())
      chan->fromChannelObj(config->channelList()->channel(i), config);
    else
      chan->clear();
  }

  // Define Zones
  for (int i=0; i<NZONES; i++) {
    zone_t *zone = (zone_t *)(data(OFFSET_ZONES+i*sizeof(zone_t)));
    zone_ext_t *ext = (zone_ext_t*)(data(OFFSET_ZONEXT+i*sizeof(zone_ext_t)));
    zone->clear();
    ext->clear();
    if (i < config->zones()->count()) {
      zone->fromZoneObj(config->zones()->zone(i), config);
      if (config->zones()->zone(i)->B()->count() || (16 < config->zones()->zone(i)->A()->count()))
        ext->fromZoneObj(config->zones()->zone(i), config);
    }
  }

  // Define Scanlists
  for (int i=0; i<NSCANL; i++) {
    scanlist_t *scan = (scanlist_t *)(data(OFFSET_SCANL+i*sizeof(scanlist_t)));
    if (i < config->scanlists()->count())
      scan->fromScanListObj(config->scanlists()->scanlist(i), config);
    else
      scan->clear();
  }

  // Define GPS systems
  for (int i=0; i<NGPSSYSTEMS; i++) {
    gpssystem_t *gps = (gpssystem_t *)(data(OFFSET_GPS_SYS+i*sizeof(gpssystem_t)));
    if (i < config->gpsSystems()->count())
      gps->fromGPSSystemObj(config->gpsSystems()->gpsSystem(i), config);
    else
      gps->clear();
  }

  // Define User DataBase (ContactCSV)
  // If userdatabase is given and not defined yet -> allocate user DB
  if (users && (2 == this->image(0).numElements()))
    this->image(0).addElement(CALLSIGN_START, CALLSIGN_END-CALLSIGN_START);
  // If user database is givan and defined -> reset and fill user DB.
  if (users && (3 == this->image(0).numElements())) {
    // clear DB memory
    memset(data(CALLSIGN_START), 0xff, CALLSIGN_END-CALLSIGN_START);
    ((callsign_db_t *)data(CALLSIGN_START))->fromUserDB(users);
  }

  return true;
}

bool
UV390Codeplug::decode(Config *config) {
  // Clear config object
  config->reset();

  // General config
  general_settings_t *genset = (general_settings_t *)data(OFFSET_SETTINGS);
  if (! genset) {
    _errorMessage = QString("%1(): Cannot access general settings memory!").arg(__func__);
    return false;
  }
  if (! genset->updateConfigObj(config)) {
    _errorMessage = QString("%1(): Invalid general settings!").arg(__func__);
    return false;
  }

  // Define Contacts
  for (int i=0; i<NCONTACTS; i++) {
    contact_t *cont = (contact_t *)(data(OFFSET_CONTACTS+i*sizeof(contact_t)));
    if (! cont) {
      _errorMessage = QString("%1(): Cannot access contact memory at index %2!").arg(__func__).arg(i);
      return false;
    }
    if (! cont->isValid())
      break;
    if (Contact *obj = cont->toContactObj())
      config->contacts()->addContact(obj);
    else {
      _errorMessage = QString("%1(): Cannot decode codeplug: Invlaid contact at index %2.")
          .arg(__func__).arg(i);
      return false;
    }
  }

  // Define RX GroupLists
  for (int i=0; i<NGLISTS; i++) {
    grouplist_t *glist = (grouplist_t *)(data(OFFSET_GLISTS+i*sizeof(grouplist_t)));
    if (! glist) {
      _errorMessage = QString("%1(): Cannot access group-list memory at index %2!").arg(__func__).arg(i);
      return false;
    }
    if (! glist->isValid())
      break;
    if (RXGroupList *obj = glist->toRXGroupListObj())
      config->rxGroupLists()->addList(obj);
    else {
      _errorMessage = QString("%1(): Cannot decode codeplug: Invlaid RX group list at index %2.")
          .arg(__func__).arg(i);
      return false;
    }
  }

  // Define Channels
  for (int i=0; i<NCHAN; i++) {
    channel_t *chan = (channel_t *)(data(OFFSET_CHANNELS+i*sizeof(channel_t)));
    if (! chan) {
      _errorMessage = QString("%1(): Cannot access channel memory at index %2!").arg(__func__).arg(i);
      return false;
    }
    if (! chan->isValid())
      break;
    if (Channel *obj = chan->toChannelObj())
      config->channelList()->addChannel(obj);
    else {
      _errorMessage = QString("%1(): Cannot decode codeplug: Invlaid channel at index %2.")
          .arg(__func__).arg(i);
      return false;
    }
  }

  // Define Zones
  for (int i=0; i<NZONES; i++) {
    zone_t *zone = (zone_t *)(data(OFFSET_ZONES+i*sizeof(zone_t)));
    if (! zone) {
      _errorMessage = QString("%1(): Cannot access zone memory at index %2!").arg(__func__).arg(i);
      return false;
    }
    if (! zone->isValid())
      break;
    if (Zone *obj = zone->toZoneObj()) {
      config->zones()->addZone(obj);
    } else {
      _errorMessage = QString("%1(): Cannot decode codeplug: Invlaid zone at index %2.")
          .arg(__func__).arg(i);
      return false;
    }
  }

  // Define Scanlists
  for (int i=0; i<NSCANL; i++) {
    scanlist_t *scan = (scanlist_t *)(data(OFFSET_SCANL+i*sizeof(scanlist_t)));
    if (! scan) {
      _errorMessage = QString("%1(): Cannot access scan-list memory at index %2!").arg(__func__).arg(i);
      return false;
    }
    if (! scan->isValid())
      break;
    if (ScanList *obj = scan->toScanListObj())
      config->scanlists()->addScanList(obj);
    else {
      _errorMessage = QString("%1(): Cannot decode codeplug: Invlaid scanlist at index %2.")
          .arg(__func__).arg(i);
      return false;
    }
  }

  // Define GPS systems
  for (int i=0; i<NGPSSYSTEMS; i++) {
    gpssystem_t *gps = (gpssystem_t *)(data(OFFSET_GPS_SYS + i*sizeof(gpssystem_t)));
    if (! gps) {
      _errorMessage = QString("%1(): Cannot decode codeplug: Invlaid GPS system at index %2.")
          .arg(__func__).arg(i);
    }
    if (! gps->isValid())
      break;
    if (GPSSystem *obj = gps->toGPSSystemObj()) {
      config->gpsSystems()->addGPSSystem(obj);
    } else {
      _errorMessage = QString("%1(): Cannot decode codeplug: Invlaid GPS system at index %2.")
          .arg(__func__).arg(i);
      return false;
    }
  }

  // Link RX GroupLists
  for (int i=0; i<config->rxGroupLists()->count(); i++) {
    grouplist_t *glist = (grouplist_t *)(data(OFFSET_GLISTS+i*sizeof(grouplist_t)));
    if (! glist->linkRXGroupList(config->rxGroupLists()->list(i), config)) {
      _errorMessage = QString("%1(): Cannot decode codeplug: Cannot link group-list at index %2.")
          .arg(__func__).arg(i);
      return false;
    }
  }

  // Link Channels
  for (int i=0; i<config->channelList()->count(); i++) {
    channel_t *chan = (channel_t *)(data(OFFSET_CHANNELS+i*sizeof(channel_t)));
    if (! chan->linkChannelObj(config->channelList()->channel(i), config)) {
      _errorMessage = QString("%1(): Cannot decode codeplug: Cannot link channel at index %2.")
          .arg(__func__).arg(i);
      return false;
    }
  }

  // Link Zones
  for (int i=0; i<config->zones()->count(); i++) {
    zone_t *zone = (zone_t *)(data(OFFSET_ZONES+i*sizeof(zone_t)));
    if (! zone->linkZone(config->zones()->zone(i), config)) {
      _errorMessage = QString("%1(): Cannot decode codeplug: Cannot link zone at index %2.")
          .arg(__func__).arg(i);
      return false;
    }
    zone_ext_t *zoneext = (zone_ext_t *)(data(OFFSET_ZONEXT+i*sizeof(zone_ext_t)));
    if (! zoneext) {
      _errorMessage = QString("%1(): Cannot access zone extension memory at index %2!").arg(__func__).arg(i);
      return false;
    }
    if (! zoneext->linkZone(config->zones()->zone(i), config)) {
      _errorMessage = QString("%1(): Cannot decode codeplug: Cannot link zone extension at index %2.")
          .arg(__func__).arg(i);
      return false;
    }
  }

  // Link Scanlists
  for (int i=0; i<config->scanlists()->count(); i++) {
    scanlist_t *scan = (scanlist_t *)(data(OFFSET_SCANL+i*sizeof(scanlist_t)));
    if (! scan->linkScanListObj(config->scanlists()->scanlist(i), config)) {
      _errorMessage = QString("%1(): Cannot decode codeplug: Cannot link scan-list at index %2.")
          .arg(__func__).arg(i);
      return false;
    }
  }

  // Link GPS systems
  for (int i=0; i<config->gpsSystems()->count(); i++) {
    gpssystem_t *gps = (gpssystem_t *)(data(OFFSET_GPS_SYS+i*sizeof(gpssystem_t)));
    if (! gps->linkGPSSystemObj(config->gpsSystems()->gpsSystem(i), config)) {
      _errorMessage = QString("%1(): Cannot decode codeplug: Cannot link GPS system at index %2.")
          .arg(__func__).arg(i);
      return false;
    }
  }

  return true;
}
