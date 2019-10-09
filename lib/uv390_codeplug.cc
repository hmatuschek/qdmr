#include "uv390_codeplug.hh"
#include "config.hh"
#include "utils.hh"
#include "channel.hh"
#include <QDebug>


#define NCHAN           3000
#define NCONTACTS       10000
#define NZONES          250
#define NGLISTS         250
#define NSCANL          250
#define NMESSAGES       50
#define NGPSSYSTEMS     16

#define MEMSZ           0xd0000
#define OFFSET_TIMESTMP 0x02001
#define OFFSET_SETTINGS 0x02040
#define OFFSET_MSG      0x02180
#define OFFSET_GLISTS   0x0ec20
#define OFFSET_ZONES    0x149e0
#define OFFSET_SCANL    0x18860
#define OFFSET_ZONEXT   0x31060
#define OFFSET_CHANNELS 0x40000
#define OFFSET_CONTACTS 0x70000
#define OFFSET_GPS_SYS  0x02080

#define CALLSIGN_START  0x00200000  // Start of callsign database
#define CALLSIGN_FINISH 0x01000000  // End of callsign database
#define CALLSIGN_OFFSET 0x4003

#define GET_TIMESTAMP()     (&radio_mem[OFFSET_TIMESTMP])
#define GET_SETTINGS()      ((general_settings_t*) &radio_mem[OFFSET_SETTINGS])
#define GET_CHANNEL(i)      ((channel_t*) &radio_mem[OFFSET_CHANNELS + (i)*64])
#define GET_ZONE(i)         ((zone_t*) &radio_mem[OFFSET_ZONES + (i)*64])
#define GET_ZONEXT(i)       ((zone_ext_t*) &radio_mem[OFFSET_ZONEXT + (i)*224])
#define GET_SCANLIST(i)     ((scanlist_t*) &radio_mem[OFFSET_SCANL + (i)*104])
#define GET_CONTACT(i)      ((contact_t*) &radio_mem[OFFSET_CONTACTS + (i)*36])
#define GET_GROUPLIST(i)    ((grouplist_t*) &radio_mem[OFFSET_GLISTS + (i)*96])
#define GET_MESSAGE(i)      ((uint16_t*) &radio_mem[OFFSET_MSG + (i)*288])
#define GET_CALLSIGN(m,i)   ((callsign_t*) ((m) + CALLSIGN_OFFSET + (i)*120))

#define VALID_TEXT(txt)     (*(txt) != 0 && *(txt) != 0xffff)
#define VALID_CHANNEL(ch)   VALID_TEXT((ch)->name)
#define VALID_ZONE(z)       VALID_TEXT((z)->name)
#define VALID_SCANLIST(sl)  VALID_TEXT((sl)->name)
#define VALID_GROUPLIST(gl) VALID_TEXT((gl)->name)
#define VALID_CONTACT(ct)   ((ct)->type != 0 && VALID_TEXT((ct)->name))

/* ******************************************************************************************** *
 * Implementation of UV390Codeplug::channel_t
 * ******************************************************************************************** */
bool
UV390Codeplug::channel_t::isValid() const {
  return (rx_frequency!=0x40000000) && (tx_frequency!=0x40000000);
}

void
UV390Codeplug::channel_t::clear() {
  channel_mode = MODE_ANALOG;
  bandwidth = BW_12_5_KHZ;
  autoscan = 0;
  _unused0_1 = _unused0_2 = 0;
  lone_worker = 0;

  allow_talkaround = 0;
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

  tot = 4;
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
  tx_signaling_syst = 1;

  power = POWER_HIGH;
  _unused30_2 = 0b111111;

  send_gps_info = 1;
  recv_gsp_info = 1;
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
  return 0;
}

void
UV390Codeplug::channel_t::setRXTone(float freq) {
  Q_UNUSED(freq);
  ctcss_dcs_receive = 0xffff;
}

float
UV390Codeplug::channel_t::getTXTone() const {
  return 0;
}

void
UV390Codeplug::channel_t::setTXTone(float freq)  {
  Q_UNUSED(freq);
  ctcss_dcs_transmit = 0xffff;
}

Channel *
UV390Codeplug::channel_t::toChannelObj() const {
  if (! isValid())
    return nullptr;
  qDebug() << rx_frequency;

  if (MODE_ANALOG == channel_mode) {
    Channel::Power pwr =
        (POWER_HIGH == power) ? Channel::HighPower : Channel::LowPower;

    AnalogChannel::Admit admit_crit;
    switch(admit_criteria) {
      case ADMIT_ALWAYS: admit_crit = AnalogChannel::AdmitNone; break;
      case ADMIT_TONE: admit_crit = AnalogChannel::AdmitTone; break;
      case ADMIT_CH_FREE: admit_crit = AnalogChannel::AdmitFree; break;
      default: admit_crit = AnalogChannel::AdmitNone; break;
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
      default: admit_crit = DigitalChannel::AdmitNone; break;
    }

    DigitalChannel::TimeSlot slot =
        (1 == time_slot) ? DigitalChannel::TimeSlot1 : DigitalChannel::TimeSlot2;

    return new DigitalChannel(getName(), getRXFrequency(), getTXFrequency(), pwr, (tot*15),
                              rx_only, admit_crit, color_code, slot, nullptr, nullptr, nullptr);
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
      if ((contact_name_index-1) >= conf->contacts()->count())
        return false;
      ///@bug This method fails if there are some non-digital contacts!
      dc->setTXContact(conf->contacts()->contact(contact_name_index-1)->as<DigitalContact>());
    }
    if (group_list_index) {
      if ((group_list_index-1) >= conf->rxGroupLists()->count())
        return false;
      dc->setRXGroupList(conf->rxGroupLists()->list(group_list_index-1));
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
      case DigitalChannel::AdmitNone: admit_criteria = ADMIT_ALWAYS;
      case DigitalChannel::AdmitFree: admit_criteria = ADMIT_CH_FREE;
      case DigitalChannel::AdmitColorCode: admit_criteria = ADMIT_COLOR;
    }
    color_code = dchan->colorCode();
    time_slot = (DigitalChannel::TimeSlot1 == dchan->timeslot()) ? 1 : 2;
    group_list_index = conf->rxGroupLists()->indexOf(dchan->rxGroupList())+1;
    contact_name_index = conf->contacts()->indexOf(dchan->txContact())+2;
  }
}


/* ******************************************************************************************** *
 * Implementation of UV390Codeplug::contact_t
 * ******************************************************************************************** */
bool
UV390Codeplug::contact_t::isValid() const {
  return 0 != type;
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
  return decode_dmr_id(id);
}

void
UV390Codeplug::contact_t::setId(uint32_t num) {
  encode_dmr_id(id, num);
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
  clear();
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
  return (0 != member_a[0]);
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
    zone->addChannel(conf->channelList()->channel(member_a[i]-1));
  }

  return true;
}

void
UV390Codeplug::zone_t::fromZoneObj(const Zone *zone, const Config *conf) {
  clear();
  setName(zone->name());
  for (int i=0; i<16; i++) {
    if (i < zone->count())
      member_a[i] = conf->channelList()->indexOf(zone->channel(i))+1;
    else
      member_a[i] = 0;
  }
}


/* ******************************************************************************************** *
 * Implementation of UV390Codeplug::zone_ext_t
 * ******************************************************************************************** */
bool
UV390Codeplug::zone_ext_t::isValid() const {
  return ((0 != ext_a[0]) && (0 != member_b[0]));
}

void
UV390Codeplug::zone_ext_t::clear() {
  memset(ext_a, 0, sizeof(ext_a));
  memset(member_b, 0, sizeof(member_b));
}

bool
UV390Codeplug::zone_ext_t::linkZone(Zone *zone, Config *conf) const {
  if (! isValid())
    return false;

  for (int i=0; (i<48) && ext_a[i]; i++) {
    if ((ext_a[i]-1)>=conf->channelList()->count())
      return false;
    zone->addChannel(conf->channelList()->channel(ext_a[i]-1));
  }

  ///@todo Implement ChannelList B for Zone!
  return false;
}


/* ******************************************************************************************** *
 * Implementation of UV390Codeplug::grouplist_t
 * ******************************************************************************************** */
bool
UV390Codeplug::grouplist_t::isValid() const {
  return (0 != member[0]);
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
    if ((member[i]-1) >= conf->contacts()->count())
      return false;
    grp->addContact(conf->contacts()->contact(member[i]-1)->as<DigitalContact>());
  }

  return true;
}

void
UV390Codeplug::grouplist_t::fromRXGroupListObj(const RXGroupList *grp, const Config *conf) {
  clear();
  setName(grp->name());
  for (int i=0; i<32; i++) {
    if (i<grp->count())
      member[i] = conf->contacts()->indexOf(grp->contact(i))+1;
    else
      member[i] = 0;
  }
}


/* ******************************************************************************************** *
 * Implementation of UV390Codeplug::scanlist_t
 * ******************************************************************************************** */
bool
UV390Codeplug::scanlist_t::isValid() const {
  return (0xffff != priority_ch1) && (0xffff != priority_ch2) && (0 != member[0]);
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
  clear();

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
void
UV390Codeplug::general_settings_t::clear() {
  memset(intro_line1, 0, sizeof(intro_line1));
  memset(intro_line2, 0, sizeof(intro_line2));

  memset(_unused40, 0xff, sizeof(_unused40));

  _unused64_0 = 1;
  disable_all_leds = 1;
  _unused64_3 = 1;
  monitor_type = 1;
  _unused64_6 = 1;
  tx_mode = 3;

  save_preamble = 1;
  save_mode_receive = 1;
  disable_all_tones = 0;
  _unused65_3 = 1;
  ch_free_indication_tone = 1;
  pw_and_lock_enable = 1;
  talk_permit_tone = 0;

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
  group_call_hang_time = 0x1e;
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
  time_zone = 0x0c;

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
}

uint32_t
UV390Codeplug::general_settings_t::getRadioId() const {
  return decode_dmr_id(radio_id);
}
void
UV390Codeplug::general_settings_t::setRadioId(uint32_t num) {
  encode_dmr_id(radio_id, num);
}

uint32_t
UV390Codeplug::general_settings_t::getRadioId1() const {
  return decode_dmr_id(radio_id1);
}
void
UV390Codeplug::general_settings_t::setRadioId1(uint32_t num) {
  encode_dmr_id(radio_id1, num);
}

uint32_t
UV390Codeplug::general_settings_t::getRadioId2() const {
  return decode_dmr_id(radio_id2);
}
void
UV390Codeplug::general_settings_t::setRadioId2(uint32_t num) {
  encode_dmr_id(radio_id2, num);
}

uint32_t
UV390Codeplug::general_settings_t::getRadioId3() const {
  return decode_dmr_id(radio_id3);
}
void
UV390Codeplug::general_settings_t::setRadioId3(uint32_t num) {
  encode_dmr_id(radio_id3, num);
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
  conf->setVoxLevel(vox_sensitivity);
  conf->setMicLevel(1+mic_level*2);
  return true;
}

void
UV390Codeplug::general_settings_t::fromConfigObj(const Config *conf) {
  setName(conf->name());
  setRadioId(conf->id());
  setIntroLine1(conf->introLine1());
  setIntroLine2(conf->introLine2());
  vox_sensitivity = conf->voxLevel();
  mic_level = conf->micLevel()/2;
}


/* ******************************************************************************************** *
 * Implementation of UV390Codeplug
 * ******************************************************************************************** */
UV390Codeplug::UV390Codeplug(QObject *parent)
  : CodePlug(parent)
{
  addImage("TYT UV390 Codeplug");
  image(0).addElement(0, MEMSZ);
}

bool
UV390Codeplug::encode(Config *config) {
  // General config
  general_settings_t *genset = (general_settings_t *)(data(OFFSET_SETTINGS));
  genset->fromConfigObj(config);

  // Define Contacts
  for (int i=0; i<NCONTACTS; i++) {
    contact_t *cont = (contact_t *)(data(OFFSET_CONTACTS+i*sizeof(contact_t)));
    if (i < config->contacts()->count())
      cont->fromContactObj(config->contacts()->contact(i)->as<DigitalContact>(), config);
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
    if (i < config->zones()->count())
      zone->fromZoneObj(config->zones()->zone(i), config);
    else
      zone->clear();
  }

  // Define Scanlists
  for (int i=0; i<NSCANL; i++) {
    scanlist_t *scan = (scanlist_t *)(data(OFFSET_SCANL+i*sizeof(scanlist_t)));
    if (i < config->scanlists()->count())
      scan->fromScanListObj(config->scanlists()->scanlist(i), config);
    else
      scan->clear();
  }

  return true;
}

bool
UV390Codeplug::decode(Config *config) {
  // Clear config object
  config->reset();

  // General config
  general_settings_t *genset = (general_settings_t *)data(OFFSET_SETTINGS);
  if (! genset->updateConfigObj(config))
    return false;

  // Define Contacts
  for (int i=0; i<NCONTACTS; i++) {
    contact_t *cont = (contact_t *)(data(OFFSET_CONTACTS+i*sizeof(contact_t)));
    if (! cont->isValid())
      break;
    if (Contact *obj = cont->toContactObj())
      config->contacts()->addContact(obj);
    else
      return false;
  }

  // Define RX GroupLists
  for (int i=0; i<NGLISTS; i++) {
    grouplist_t *glist = (grouplist_t *)(data(OFFSET_GLISTS+i*sizeof(grouplist_t)));
    if (! glist->isValid())
      break;
    if (RXGroupList *obj = glist->toRXGroupListObj())
      config->rxGroupLists()->addList(obj);
    else
      return false;
  }

  // Define Channels
  for (int i=0; i<NCHAN; i++) {
    channel_t *chan = (channel_t *)(data(OFFSET_CHANNELS+i*sizeof(channel_t)));
    if (! chan->isValid())
      break;
    if (Channel *obj = chan->toChannelObj())
      config->channelList()->addChannel(obj);
    else
      return false;
  }

  // Define Zones
  for (int i=0; i<NZONES; i++) {
    zone_t *zone = (zone_t *)(data(OFFSET_ZONES+i*sizeof(zone_t)));
    if (! zone->isValid())
      break;
    if (Zone *obj = zone->toZoneObj())
      config->zones()->addZone(obj);
    else
      return false;
  }

  // Define Scanlists
  for (int i=0; i<NSCANL; i++) {
    scanlist_t *scan = (scanlist_t *)(data(OFFSET_SCANL+i*sizeof(scanlist_t)));
    if (! scan->isValid())
      break;
    if (ScanList *obj = scan->toScanListObj())
      config->scanlists()->addScanList(obj);
    else
      return false;
  }

  // Link RX GroupLists
  for (int i=0; i<config->rxGroupLists()->count(); i++) {
    grouplist_t *glist = (grouplist_t *)(data(OFFSET_GLISTS+i*sizeof(grouplist_t)));
    if (! glist->linkRXGroupList(config->rxGroupLists()->list(i), config))
      return false;
  }

  // Link Channels
  for (int i=0; i<config->channelList()->count(); i++) {
    channel_t *chan = (channel_t *)(data(OFFSET_CHANNELS+i*sizeof(channel_t)));
    if (! chan->linkChannelObj(config->channelList()->channel(i), config))
      return false;
  }

  // Link Zones
  for (int i=0; i<config->zones()->count(); i++) {
    zone_t *zone = (zone_t *)(data(OFFSET_ZONES+i*sizeof(zone_t)));
    if (! zone->linkZone(config->zones()->zone(i), config))
      return false;
  }

  // Link Scanlists
  for (int i=0; i<config->scanlists()->count(); i++) {
    scanlist_t *scan = (scanlist_t *)(data(OFFSET_SCANL+i*sizeof(scanlist_t)));
    if (! scan->linkScanListObj(config->scanlists()->scanlist(i), config))
      return false;
  }

  return true;
}

size_t
UV390Codeplug::size() const {
  return 0xd0000;
}
