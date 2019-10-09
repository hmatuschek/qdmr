#include "rd5r_codeplug.hh"
#include "config.hh"
#include "channel.hh"
#include "utils.hh"
#include <QDebug>
#include <QDateTime>


#define MEMSZ           0x20000
#define OFFSET_TIMESTMP 0x00088
#define OFFSET_SETTINGS 0x000e0
#define OFFSET_MSGTAB   0x00128
#define OFFSET_CONTACTS 0x01788
#define OFFSET_BANK_0   0x03780 // Channels 1-128
#define OFFSET_INTRO    0x07540
#define OFFSET_ZONETAB  0x08010
#define OFFSET_BANK_1   0x0b1b0 // Channels 129-1024
#define OFFSET_SCANTAB  0x17620
#define OFFSET_GROUPTAB 0x1d620

#define GET_TIMESTAMP()     (&_radio_mem[OFFSET_TIMESTMP])
#define GET_MSGTAB()        ((msgtab_t*) &_radio_mem[OFFSET_MSGTAB])


/* ******************************************************************************************** *
 * Implementation of RD5RCodeplug::channel_t
 * ******************************************************************************************** */
bool
RD5RCodeplug::channel_t::isValid() const {
  // Channel is enabled/disabled at channel bank
  return true;
}

void
RD5RCodeplug::channel_t::clear() {
  memset(name, 0xff, 16);
  _unused25[0] = _unused25[1] = 0;
  _unused30 = 0x50;
  _unused36 = 0;
  tx_signaling_syst = 0;
  _unused38 = 0;
  rx_signaling_syst = 0;
  _unused40 = 0x16;
  privacy_group = 0;
  emergency_system_index = 0;
  _unused48 = 0;
  emergency_alarm_ack = 0;
  data_call_conf = 0;
  private_call_conf = 1;
  _unused49_1 = 0;
  privacy = 0;
  _unused49_5 = 0;
  _unused49_7 = 0;
  dcdm = 0;
  _unused50_1 = 0;
  _unused50_6 = 0;
  _unused51_0 = 0;
  talkaround = 1;
  _unused51_4 = 0;
  vox = 0;
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

float
RD5RCodeplug::channel_t::getRXTone() const {
  return decode_ctcss_tone(ctcss_dcs_receive);
}
void
RD5RCodeplug::channel_t::setRXTone(float tone) {
  ctcss_dcs_receive = encode_ctcss_tone(tone);
}

float
RD5RCodeplug::channel_t::getTXTone() const {
  return decode_ctcss_tone(ctcss_dcs_transmit);
}
void
RD5RCodeplug::channel_t::setTXTone(float tone) {
  ctcss_dcs_transmit = encode_ctcss_tone(tone);
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
          nullptr, nullptr, nullptr);
  }

  return nullptr;
}

bool
RD5RCodeplug::channel_t::linkChannelObj(Channel *c, Config *conf) const {
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
    if (contact_name_index && (conf->contacts()->count() >= contact_name_index))
      dc->setTXContact(conf->contacts()->contact(contact_name_index-1)->as<DigitalContact>());
  }
  return true;
}

void
RD5RCodeplug::channel_t::fromChannelObj(const Channel *c, const Config *conf) {
  clear();
  setName(c->name());
  setRXFrequency(c->rxFrequency());
  setTXFrequency(c->txFrequency());
  power = (Channel::HighPower == c->power()) ? POWER_HIGH : POWER_LOW;
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
    contact_name_index = conf->contacts()->indexOf(dc->txContact()) + 1;
  }
}


/* ******************************************************************************************** *
 * Implementation of RD5RCodeplug::contact_t
 * ******************************************************************************************** */
void
RD5RCodeplug::contact_t::clear() {
  is_valid = 0x00;
}
bool
RD5RCodeplug::contact_t::isValid() const {
  return 0xff == is_valid;
}

uint32_t
RD5RCodeplug::contact_t::getId() const {
  return decode_dmr_id(id);
}
void
RD5RCodeplug::contact_t::setId(uint32_t num) {
  encode_dmr_id(id, num);
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
  bool rxTone = receive_tone;
  return new DigitalContact(ctype, name, id, rxTone);
}

void
RD5RCodeplug::contact_t::fromContactObj(const DigitalContact *cont, const Config *conf) {
  Q_UNUSED(conf);
  clear();
  setName(cont->name());
  setId(cont->number());
  switch (cont->type()) {
    case DigitalContact::PrivateCall: type = CALL_PRIVATE;
    case DigitalContact::GroupCall: type = CALL_GROUP;
    case DigitalContact::AllCall: type = CALL_ALL;
  }
  receive_tone = (cont->rxTone()) ? 1 : 0;
  is_valid = 0xff;
}


/* ******************************************************************************************** *
 * Implementation of RD5RCodeplug::zone_t
 * ******************************************************************************************** */
bool
RD5RCodeplug::zone_t::isValid() const {
  return true;
}
void
RD5RCodeplug::zone_t::clear() {
  // pass...
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
RD5RCodeplug::zone_t::linkZoneObj(Zone *zone, const Config *conf) const {
  if (! isValid())
    return false;
  for (int i=0; (i<16) && member[i]; i++)
    zone->addChannel(conf->channelList()->channel(member[i]-1));
  return true;
}

void
RD5RCodeplug::zone_t::fromZoneObj(const Zone *zone, const Config *conf) {
  clear();
  setName(zone->name());
  for (int i=0; i<16; i++) {
    if (i < zone->count())
      member[i] = conf->channelList()->indexOf(zone->channel(i))+1;
    else
      member[i] = 0;
  }
}


/* ******************************************************************************************** *
 * Implementation of RD5RCodeplug::grouplist_t
 * ******************************************************************************************** */
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
RD5RCodeplug::grouplist_t::linkRXGroupListObj(RXGroupList *lst, const Config *conf) const {
  for (int i=0; (i<16)&&member[i]; i++) {
    lst->addContact(conf->contacts()->contact(member[i]-1)->as<DigitalContact>());
  }
  return false;
}

void
RD5RCodeplug::grouplist_t::fromRXGroupListObj(const RXGroupList *lst, const Config *conf) {
  setName(lst->name());
  for (int i=0; i<16; i++) {
    if (i < lst->count())
      member[i] = conf->contacts()->indexOf(lst->contact(i))+1;
    else
      member[i] = 0;
  }
}


/* ******************************************************************************************** *
 * Implementation of RD5RCodeplug::scanlist_t
 * ******************************************************************************************** */
void
RD5RCodeplug::scanlist_t::clear() {
  memset(name, 0, 15);
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
RD5RCodeplug::scanlist_t::linkScanListObj(ScanList *lst, const Config *conf) const {
  if (1<priority_ch1)
    lst->setPriorityChannel(conf->channelList()->channel(priority_ch1-2));
  if (1<priority_ch2)
    lst->setSecPriorityChannel(conf->channelList()->channel(priority_ch2-2));
  for (int i=0; (i<32) && member[i]; i++)
      lst->addChannel(conf->channelList()->channel(member[i]-1));
  return false;
}

void
RD5RCodeplug::scanlist_t::fromScanListObj(const ScanList *lst, const Config *conf) {
  clear();
  if (lst->priorityChannel())
    priority_ch1 = conf->channelList()->indexOf(lst->priorityChannel())+2;
  if (lst->secPriorityChannel())
    priority_ch2 = conf->channelList()->indexOf(lst->secPriorityChannel())+2;
  tx_designated_ch = 0;
}


/* ******************************************************************************************** *
 * Implementation of RD5RCodeplug::general_settings_t
 * ******************************************************************************************** */
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
  return decode_dmr_id(radio_id);
}
void
RD5RCodeplug::general_settings_t::setRadioId(uint32_t num) {
  encode_dmr_id(radio_id, num);
}


/* ******************************************************************************************** *
 * Implementation of RD5RCodeplug::intro_text_t
 * ******************************************************************************************** */
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
  image(0).addElement(0, MEMSZ);
}

size_t
RD5RCodeplug::size() const {
  return MEMSZ;
}

bool
RD5RCodeplug::decode(Config *config)
{
  // Init radio memory
  memset(data(0), 0xff, 128);
  memset(data(966*128), 0xff, MEMSZ - 966*128);
  memset(data(248*128), 0xff, 8*128);

  /*
   * Unpack general config
   */
  general_settings_t *gs = (general_settings_t*) data(OFFSET_SETTINGS);
  config->setId(gs->getRadioId());
  config->setName(gs->getName());
  intro_text_t *it = (intro_text_t*) data(OFFSET_INTRO);
  config->setIntroLine1(it->getIntroLine1());
  config->setIntroLine2(it->getIntroLine2());

  /*
   * Unpack Contacts
   */
  for (int i=0; i<NCONTACTS; i++) {
    contact_t *ct = (contact_t *) data(OFFSET_CONTACTS+i*sizeof(contact_t));
    // If contact is disabled
    if (! ct->isValid())
      continue;
    config->contacts()->addContact(ct->toContactObj());
  }

  /*
   * Unpack RX Group Lists
   */
  for (int i=0; i<NGLISTS; i++) {
    grouptab_t *gt = (grouptab_t*) data(OFFSET_GROUPTAB);
    if (0 == gt->nitems1[i])
      continue;
    grouplist_t *gl = &gt->grouplist[i];
    RXGroupList *list = gl->toRXGroupListObj();
    config->rxGroupLists()->addList(list);
    gl->linkRXGroupListObj(list, config);
  }

  /*
   * Unpack Channels
   */
  for (int i=0; i<NCHAN; i++) {
    // First, get bank
    bank_t *b;
    if ((i>>7) == 0)
      b = (bank_t*) data(OFFSET_BANK_0);
    else
      b = ((i-1) + (bank_t*) data(OFFSET_BANK_1));
    // If channel is disabled
    if (! ((b->bitmap[i % 128 / 8] >> (i & 7)) & 1) )
      continue;
    // finally, get channel
    channel_t *ch = &b->chan[i % 128];
    config->channelList()->addChannel(ch->toChannelObj());
  }

  //
  // Unpack Zones
  //
  for (int i=0; i<NZONES; i++) {
    zonetab_t *zt = (zonetab_t*) data(OFFSET_ZONETAB);
    // if zone is disabled
    if (! (zt->bitmap[i / 8] >> (i & 7) & 1) )
      continue;
    // get zone_t
    zone_t *z = &zt->zone[i];

    // Create & link zone obj
    Zone *zone = z->toZoneObj();
    z->linkZoneObj(zone, config);
    config->zones()->addZone(zone);
  }

  //
  // Unpack Scan lists
  //
  for (int i=0; i<NSCANL; i++) {
    scantab_t *st = (scantab_t*) data(OFFSET_SCANTAB);
    if (! st->valid[i])
      continue;
    scanlist_t *sl = &st->scanlist[i];

    ScanList *scan = sl->toScanListObj();
    sl->linkScanListObj(scan, config);
    config->scanlists()->addScanList(scan);
  }

  /*
   * Link Channels -> ScanLists
   */
  for (int i=0; i<NCHAN; i++) {
    // First, get bank
    bank_t *b;
    if ((i>>7) == 0)
      b = (bank_t*) data(OFFSET_BANK_0);
    else
      b = ((i-1) + (bank_t*) data(OFFSET_BANK_1));
    // If channel is disabled
    if (! ((b->bitmap[i % 128 / 8] >> (i & 7)) & 1) )
      continue;
    // finally, get channel
    channel_t *ch = &b->chan[i % 128];
    ch->linkChannelObj(config->channelList()->channel(i), config);
  }

  return true;
}

bool
RD5RCodeplug::encode(Config *config)
{
  // Clear header and footer.
  memset(data(0), 0xff, 128);
  memset(data(966*128), 0xff, MEMSZ - 966*128);
  memset(data(248*128), 0xff, 8*128);
  memcpy(data(0), "BF-5R", 5);

  // Pack channels
  for (int i=0; i<NCHAN; i++) {
    // First, get bank
    bank_t *b;
    if ((i>>7) == 0)
      b = (bank_t*) data(OFFSET_BANK_0);
    else
      b = ((i-1) + (bank_t *) data(OFFSET_BANK_1));
    channel_t *ch = &b->chan[i % 128];

    // Disable channel if not used
    if (i >= config->channelList()->count()) {
      b->bitmap[i % 128 / 8] &= ~(1 << (i & 7));
      continue;
    }

    // Construct from Channel object
    ch->fromChannelObj(config->channelList()->channel(i), config);

    // Set valid bit.
    b->bitmap[i % 128 / 8] |= 1 << (i & 7);
  }

  // Pack Zones
  for (int i=0; i<NZONES; i++) {
    zonetab_t *zt = (zonetab_t*) data(OFFSET_ZONETAB);
    zone_t *z = &zt->zone[i];

    if (i >= config->zones()->count()) {
      // Clear valid bit.
      zt->bitmap[i / 8] &= ~(1 << (i & 7));
      continue;
    }

    // Construct from Zone obj
    z->fromZoneObj(config->zones()->zone(i), config);

    // Set valid bit.
    zt->bitmap[i / 8] |= 1 << (i & 7);
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
    if (i >= config->contacts()->count())
      continue;
    ct->fromContactObj(config->contacts()->contact(i)->as<const DigitalContact>(), config);
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
    gt->nitems1[i] = 1;

    // RD-5R firmware up to v2.1.6 has a bug:
    // when all 16 entries in RX group list are occupied,
    // it stops functioning and no audio is received.
    // As a workaround, we must use only 15 entries here.
    for (int j=0; j<std::min(15,config->rxGroupLists()->list(i)->count()); j++)
      gl->member[j] = config->contacts()->indexOf(config->rxGroupLists()->list(i)->contact(j))+1;
    gt->nitems1[i] = std::min(15,config->rxGroupLists()->list(i)->count()) + 1;
  }

  // Pack basic config
  general_settings_t *gs = (general_settings_t*) data(OFFSET_SETTINGS);
  gs->setName(config->name());
  gs->setRadioId(config->id());
  intro_text_t *it = (intro_text_t*) data(OFFSET_INTRO);
  it->setIntroLine1(config->introLine1());
  it->setIntroLine2(config->introLine2());

  return true;
}
