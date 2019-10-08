#include "rd5r_codeplug.hh"
#include "config.hh"
#include "channel.hh"
#include <QDebug>


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
#define GET_SETTINGS()      ((general_settings_t*) &_radio_mem[OFFSET_SETTINGS])
#define GET_INTRO()         ((intro_text_t*) &_radio_mem[OFFSET_INTRO])
#define GET_ZONETAB()       ((zonetab_t*) &_radio_mem[OFFSET_ZONETAB])
#define GET_SCANTAB(i)      ((scantab_t*) &_radio_mem[OFFSET_SCANTAB])
#define GET_CONTACT(i)      ((contact_t*) &_radio_mem[OFFSET_CONTACTS + (i)*24])
#define GET_GROUPTAB()      ((grouptab_t*) &_radio_mem[OFFSET_GROUPTAB])
#define GET_MSGTAB()        ((msgtab_t*) &_radio_mem[OFFSET_MSGTAB])

#define VALID_TEXT(txt)     (*(txt) != 0 && *(txt) != 0xff)
#define VALID_CONTACT(ct)   VALID_TEXT((ct)->name)


inline unsigned mhz_to_abcdefgh(double mhz) {
  unsigned hz = unsigned(mhz * 1000000.0);
  unsigned a  = (hz / 100000000) % 10;
  unsigned b  = (hz / 10000000)  % 10;
  unsigned c  = (hz / 1000000)   % 10;
  unsigned d  = (hz / 100000)    % 10;
  unsigned e  = (hz / 10000)     % 10;
  unsigned f  = (hz / 1000)      % 10;
  unsigned g  = (hz / 100)       % 10;
  unsigned h  = (hz / 10)        % 10;

  return a << 28 | b << 24 | c << 20 | d << 16 | e << 12 | f << 8 | g << 4 | h;
}

#define NCTCSS  50

static const unsigned CTCSS_TONES [NCTCSS] = {
     670,  693,  719,  744,  770,  797,  825,  854,  885,  915,
     948,  974, 1000, 1035, 1072, 1109, 1148, 1188, 1230, 1273,
    1318, 1365, 1413, 1462, 1514, 1567, 1598, 1622, 1655, 1679,
    1713, 1738, 1773, 1799, 1835, 1862, 1899, 1928, 1966, 1995,
    2035, 2065, 2107, 2181, 2257, 2291, 2336, 2418, 2503, 2541,
};

inline int encode_tone(double hz)
{
    unsigned val, tag, a, b, c, d;

    // Disabled
    if (0 == hz)
      return 0xffff;

    //
    // CTCSS tone
    //
    // Round to integer.
    val = hz * 10.0 + 0.5;

    // Find a valid index in CTCSS table.
    int i;
    for (i=0; i<NCTCSS; i++)
      if (CTCSS_TONES[i] == val)
        break;
    if (i >= NCTCSS)
      return -1;

    a = val / 1000;
    b = val / 100 % 10;
    c = val / 10 % 10;
    d = val % 10;
    tag = 0;

    return (a << 12) | (b << 8) | (c << 4) | d | (tag << 14);
}

RD5RCodePlug::RD5RCodePlug(QObject *parent)
  : CodePlug(parent)
{
  // pass...
}

bool
RD5RCodePlug::decode(Config *config)
{
  memset(&_radio_mem[0], 0xff, 128);
  memset(&_radio_mem[966*128], 0xff, MEMSZ - 966*128);
  memset(&_radio_mem[248*128], 0xff, 8*128);

  /*
   * Unpack general config
   */
  general_settings_t *gs = GET_SETTINGS();
  config->setId(GET_ID(gs->radio_id));
  if (VALID_TEXT(gs->radio_name))
    config->setName(_unpack_ascii(gs->radio_name, 8));
  intro_text_t *it = GET_INTRO();
  if (VALID_TEXT(it->intro_line1))
    config->setIntroLine1(_unpack_ascii(it->intro_line1, 16));
  if (VALID_TEXT(it->intro_line2))
    config->setIntroLine2(_unpack_ascii(it->intro_line2, 16));

  /*
   * Unpack Contacts
   */
  for (int i=0; i<NCONTACTS; i++) {
    contact_t *ct = GET_CONTACT(i);

    // If contact is disabled
    if (! VALID_CONTACT(ct))
      continue;

    QString name = _unpack_ascii(ct->name, 16);
    uint number = CONTACT_ID(ct);
    bool rxtone = (0 != ct->receive_tone);
    DigitalContact::Type type = DigitalContact::PrivateCall;
    if (0 == (ct->type & 3))
      type = DigitalContact::GroupCall;
    else if (1 == (ct->type & 3))
      type = DigitalContact::PrivateCall;
    else if (2 == (ct->type & 3))
      type = DigitalContact::AllCall;
    config->contacts()->addContact(new DigitalContact(type, name, number, rxtone));
  }

  /*
   * Unpack RX Group Lists
   */
  for (int i=0; i<NGLISTS; i++) {
    grouplist_t *gl = _unpack_get_grouplist(i);
    // If group list is disabled.
    if (!gl)
      continue;
    QString name = _unpack_ascii(gl->name, 16);
    RXGroupList *list = new RXGroupList(name);
    for (int i=0; (i<16) && gl->member[i]; i++)
      list->addContact(config->contacts()->contact(gl->member[i]-1)->as<DigitalContact>());
    config->rxGroupLists()->addList(list);
  }

  /*
   * Unpack Channels
   */
  for (int i=0; i<NCHAN; i++) {
    channel_t *ch = _unpack_get_channel(i);
    if (! ch)
      continue;

    QString name = _unpack_ascii(ch->name, 16);
    double rx = _unpack_frequency(ch->rx_frequency);
    double tx = _unpack_frequency(ch->tx_frequency);
    Channel::Power power = (0==ch->power) ? Channel::LowPower : Channel::HighPower;
    uint timeout = 15*ch->tot;
    bool rxOnly = (1 == ch->rx_only);

    if (ch->channel_mode == MODE_DIGITAL) {
      DigitalChannel::Admit admit = DigitalChannel::AdmitNone;
      if (0 == ch->admit_criteria)
        admit = DigitalChannel::AdmitNone;
      else if (1 == ch->admit_criteria)
        admit = DigitalChannel::AdmitFree;
      else if (2 == ch->admit_criteria)
        admit = DigitalChannel::AdmitColorCode;
      uint colorcode = ch->colorcode_tx;
      DigitalChannel::TimeSlot slot = (0 == ch->repeater_slot2) ?
            DigitalChannel::TimeSlot1 : DigitalChannel::TimeSlot2;
      RXGroupList *rxlst = nullptr;
      if (ch->group_list_index)
        rxlst = config->rxGroupLists()->list(ch->group_list_index-1);
      DigitalContact *contact = nullptr;
      if (ch->contact_name_index)
        contact = config->contacts()->contact(ch->contact_name_index-1)->as<DigitalContact>();
      qDebug() << "Add digital channel" << name;
      config->channelList()->addChannel(
            new DigitalChannel(name, rx, tx, power, timeout, rxOnly, admit, colorcode, slot,
                               rxlst, contact, nullptr));
    } else {
      AnalogChannel::Admit admit = AnalogChannel::AdmitNone;
      if (0 == ch->admit_criteria)
        admit = AnalogChannel::AdmitNone;
      else if (1 == ch->admit_criteria)
        admit = AnalogChannel::AdmitFree;
      uint squelch = ch->squelch <= 9 ? ch->squelch : 5;
      double ctcss_rx = _unpack_tone(ch->ctcss_dcs_receive);
      double ctcss_tx = _unpack_tone(ch->ctcss_dcs_transmit);
      AnalogChannel::Bandwidth bw = (0 == ch->bandwidth) ?
            AnalogChannel::BWNarrow : AnalogChannel::BWWide;
      config->channelList()->addChannel(
            new AnalogChannel(name, rx, tx, power, timeout, rxOnly,  admit, squelch,
                              ctcss_rx, ctcss_tx, bw, nullptr));
      qDebug() << "Add analog channel" << name << "at idx" << (config->channelList()->count()-1);
    }
  }

  //
  // Unpack Zones
  //
  for (int i=0; i<NZONES; i++) {
    zone_t *z = nullptr;
    zonetab_t *zt = GET_ZONETAB();
    if (zt->bitmap[i / 8] >> (i & 7) & 1)
        z = &zt->zone[i];
    else
      continue;
    QString name = _unpack_ascii(z->name, 16);
    Zone *zone = new Zone(name);
    for (int i=0; (i<16) && z->member[i]; i++) {
      qDebug() << "Add channel" << (z->member[i]-1) << "to zone" << name;
      zone->addChannel(config->channelList()->channel(z->member[i]-1));
    }
    config->zones()->addZone(zone);
  }

  //
  // Unpack Scan lists
  //
  for (int i=0; i<NSCANL; i++) {
    scanlist_t *sl = nullptr;
    scantab_t *st = GET_SCANTAB();
    if (st->valid[i])
      sl = &st->scanlist[i];
    else
      continue;
    QString name = _unpack_ascii(sl->name, 15);
    ScanList *scan = new ScanList(name);
    if (sl->member[1]) {
      for (int i=1; (i<31) && sl->member[i]; i++) {
        qDebug() << "Add channel" << (sl->member[i]-2) << "to scanlist" << name;
        scan->addChannel(config->channelList()->channel(sl->member[i]-2));
      }
    }
    config->scanlists()->addScanList(scan);
  }

  /*
   * Link Channels -> ScanLists
   */
  for (int i=0, j=0; i<NCHAN; i++) {
    channel_t *ch = _unpack_get_channel(i);
    if (! ch)
      continue;
    if (ch->scan_list_index)
      config->channelList()->channel(j)->setScanList(
            config->scanlists()->scanlist(ch->scan_list_index-1));
    j++;
  }

  return true;
}

bool
RD5RCodePlug::encode(Config *config)
{
  // Clear header and footer.
  memset(&_radio_mem[0], 0xff, 128);
  memset(&_radio_mem[966*128], 0xff, MEMSZ - 966*128);
  memset(&_radio_mem[248*128], 0xff, 8*128);
  memcpy(&_radio_mem[0], "BF-5R", 5);

  // Pack channels
  for (int i=0; i<NCHAN; i++) {
    if (i<config->channelList()->count())
      _pack_channel(config, i, config->channelList()->channel(i));
    else
      _pack_channel(config, i, nullptr);
  }

  // Pack Zones
  for (int i=0; i<NZONES; i++) {
    if (i < config->zones()->count())
      _pack_zone(config, i, config->zones()->zone(i));
    else
      _pack_zone(config, i, nullptr);
  }

  // Pack Scanlists
  for (int i=0; i<NSCANL; i++) {
    if (i < config->scanlists()->count())
      _pack_scanlist(config, i, config->scanlists()->scanlist(i));
    else
      _pack_scanlist(config, i, nullptr);
  }

  // Pack contacts
  for (int i=0, j=0; i<config->contacts()->count(); i++) {
    if (config->contacts()->contact(i)->is<AnalogChannel>())
        continue;
    _pack_contact(config, j, config->contacts()->contact(i)->as<DigitalContact>());
    j++;
  }

  // Pack Grouplists:
  for (int i=0; i<config->rxGroupLists()->count(); i++) {
    _pack_grouplist(config, i, config->rxGroupLists()->list(i));
  }

  // Pack basic config
  general_settings_t *gs = GET_SETTINGS();
  memset(gs->radio_name, 0xff, sizeof(gs->radio_name));
  memcpy(gs->radio_name, config->name().toLocal8Bit().constData(), std::min(int(sizeof(gs->radio_name)), config->name().size()));

  gs->radio_id[0] = ((config->id() / 10000000) << 4) | ((config->id() / 1000000) % 10);
  gs->radio_id[1] = ((config->id() / 100000 % 10) << 4) | ((config->id() / 10000) % 10);
  gs->radio_id[2] = ((config->id() / 1000 % 10) << 4) | ((config->id() / 100) % 10);
  gs->radio_id[3] = ((config->id() / 10 % 10) << 4) | (config->id() % 10);

  intro_text_t *it = GET_INTRO();
  memset(it->intro_line1, 0xff, sizeof(it->intro_line1));
  memcpy(it->intro_line1, config->introLine1().toLocal8Bit().constData(), std::min(int(sizeof(it->intro_line1)), config->introLine1().size()));
  memset(it->intro_line2, 0xff, sizeof(it->intro_line2));
  memcpy(it->intro_line2, config->introLine2().toLocal8Bit().constData(), std::min(int(sizeof(it->intro_line2)), config->introLine2().size()));

  return true;
}

RD5RCodePlug::bank_t *
RD5RCodePlug::_unpack_get_bank(int i)
{
  if (i == 0)
    return (bank_t*) &_radio_mem[OFFSET_BANK_0];
  else
    return (i - 1) + (bank_t*) &_radio_mem[OFFSET_BANK_1];
}

RD5RCodePlug::channel_t *
RD5RCodePlug::_unpack_get_channel(int i)
{
  bank_t *b = _unpack_get_bank(i >> 7);
  if ((b->bitmap[i % 128 / 8] >> (i & 7)) & 1)
    return &b->chan[i % 128];
  else
    return nullptr;
}

RD5RCodePlug::grouplist_t *
RD5RCodePlug::_unpack_get_grouplist(int index)
{
    grouptab_t *gt = GET_GROUPTAB();

    if (gt->nitems1[index] > 0)
        return &gt->grouplist[index];
    else
        return 0;
}
double
RD5RCodePlug::_unpack_frequency(unsigned data) {
  double freq = 100.0*((data >> 28) & 15) + 10.0*((data >> 24) & 15) + 1.0*((data >> 20) & 15)
      + 0.1*((data >> 16) & 15) + 0.01*((data >> 12) & 15) + 0.001*((data >> 8) & 15);
  if ((data & 0xff) != 0) {
    freq += 0.0001*((data >> 4) & 15);
    if ((data & 0x0f) != 0)
      freq += 0.00001*(data & 15);
  }
  return freq;
}

double
RD5RCodePlug::_unpack_tone(unsigned data) {
  if (data == 0xffff)
    return 0.0;

  unsigned tag = data >> 14;
  unsigned a = (data >> 12) & 3;
  unsigned b = (data >> 8) & 15;
  unsigned c = (data >> 4) & 15;
  unsigned d = data & 15;

  switch (tag) {
    case 2:
    case 3:
      return 0;
    default:
      return 100.0*a+10.0*b+1.0*c+0.1*d;
  }
}

QString
RD5RCodePlug::_unpack_ascii(const unsigned char *text, unsigned nchars) {
  QString out; out.reserve(nchars);

  for (unsigned i=0; i<nchars && *text != 0xff && *text != 0; i++) {
    char ch = *text++;
    if (ch == '\t')
      ch = ' ';
    out.append(ch);
  }

  return out;
}

bool
RD5RCodePlug::_pack_channel(Config *config, int i, Channel *channel) {
  bank_t *b = _unpack_get_bank(i >> 7);
  channel_t *ch = &b->chan[i % 128];

  if (nullptr == channel) {
    // Disable channel
    b->bitmap[i % 128 / 8] &= ~(1 << (i & 7));
    return true;
  }

  if (channel->is<DigitalChannel>()) {
    DigitalChannel *digi   = channel->as<DigitalChannel>();
    ch->channel_mode       = MODE_DIGITAL;
    ch->squelch            = 3;
    ch->repeater_slot2     = (DigitalChannel::TimeSlot2 == digi->timeslot()) ? 1 : 0;
    ch->colorcode_tx       = digi->colorCode();
    ch->colorcode_rx       = digi->colorCode();
    ch->admit_criteria     = (DigitalChannel::AdmitNone == digi->admit() ?
                                ADMIT_ALWAYS : (DigitalChannel::AdmitFree == digi->admit() ?
                                                  ADMIT_CH_FREE : ADMIT_COLOR) );
    ch->group_list_index   = config->rxGroupLists()->indexOf(digi->rxGroupList())+1;
    ch->contact_name_index = config->contacts()->indexOf(digi->txContact())+1;
    ch->ctcss_dcs_receive  = 0xffff;
    ch->ctcss_dcs_transmit = 0xffff;
    ch->bandwidth          = BW_12_5_KHZ;
  } else {
    AnalogChannel *analog  = channel->as<AnalogChannel>();
    ch->channel_mode       = MODE_ANALOG;
    ch->squelch            = analog->squelch();
    ch->repeater_slot2     = 0;
    ch->colorcode_tx       = 0;
    ch->colorcode_rx       = 0;
    ch->admit_criteria     = (AnalogChannel::AdmitNone == analog->admit() ?
                                ADMIT_ALWAYS : (AnalogChannel::AdmitFree == analog->admit() ?
                                                  ADMIT_CH_FREE : ADMIT_COLOR) );
    ch->group_list_index   = 0;
    ch->contact_name_index = 0;
    ch->ctcss_dcs_receive  = encode_tone(analog->rxTone());
    ch->ctcss_dcs_transmit = encode_tone(analog->txTone());
    ch->bandwidth          = (AnalogChannel::BWNarrow == analog->bandwidth() ? BW_12_5_KHZ : BW_25_KHZ);
  }
  ch->rx_only             = (channel->rxOnly() ? 1 : 0);
  //ch->data_call_conf      = 1;        // Always ask for SMS acknowledge
  ch->power               = (Channel::HighPower == channel->power() ? POWER_HIGH : POWER_LOW);
  ch->tot                 = channel->txTimeout()/15;
  ch->scan_list_index     = (nullptr != channel->scanList() ?
        config->scanlists()->indexOf(channel->scanList()) : 0);
  ch->rx_frequency        = mhz_to_abcdefgh(channel->rxFrequency());
  ch->tx_frequency        = mhz_to_abcdefgh(channel->txFrequency());
  ch->vox                 = 0;
  ch->dcdm                = 0;

  memset(ch->name, 0xff, sizeof(ch->name));
	memcpy(ch->name, channel->name().toLocal8Bit().constData(), std::min(16, channel->name().size()));

  // Set valid bit.
  b->bitmap[i % 128 / 8] |= 1 << (i & 7);

  return true;
}


bool
RD5RCodePlug::_pack_zone(Config *config, int i, Zone *zone) {
  Q_UNUSED(config)

  zonetab_t *zt = GET_ZONETAB();
  zone_t *z = &zt->zone[i];

  if (nullptr == zone) {
    // Clear valid bit.
    zt->bitmap[i / 8] &= ~(1 << (i & 7));
    return true;
  }

  memset(z->name, 0xff, sizeof (z->name));
  memcpy(z->name, zone->name().toLocal8Bit().constData(), std::min(int(sizeof(z->name)), zone->name().size()));
  memset(z->member, 0, sizeof(z->member));

  // Set valid bit.
  zt->bitmap[i / 8] |= 1 << (i & 7);
  for (int i=0; i<16; i++) {
    if (i < zone->count())
      z->member[i] = config->channelList()->indexOf(zone->channel(i))+1;
    else
      z->member[i] = 0;
  }

  return true;
}

bool
RD5RCodePlug::_pack_scanlist(Config *config, int i, ScanList *list) {
  scantab_t *st = GET_SCANTAB();
  scanlist_t *sl = &st->scanlist[i];

  if (nullptr == list) {
    // Clear valid bit.
    st->valid[i] = 0;
    return true;
  }

  memset(sl, 0, sizeof(scanlist_t));

  memset(sl->name, 0xff, sizeof(sl->name));
  memcpy(sl->name, list->name().toLocal8Bit().constData(), std::min(int(sizeof(sl->name)), list->name().size()));

  sl->priority_ch1     = 0;
  sl->priority_ch2     = 0;
  sl->tx_designated_ch = 0;
  sl->talkback         = 1;
  sl->channel_mark     = 1;
  sl->pl_type          = PL_PRI_NONPRI;
  sl->sign_hold_time   = 1000 / 25;   // 1 sec
  sl->prio_sample_time = 2000 / 250;  // 2 sec

  // Set valid bit.
  st->valid[i] = 1;

  // First element is always selected channel.
  if (sl->member[0] == 0)
    sl->member[0] = CHAN_SELECTED;

  for (i=1; i<32; i++) {
    if (i < list->count())
      sl->member[i] = config->channelList()->indexOf(list->channel(i))+2;
    else
      sl->member[i] = 0;
  }
  return true;
}

bool
RD5RCodePlug::_pack_contact(Config *config, int i, DigitalContact *contact) {
  Q_UNUSED(config);
  contact_t *ct = GET_CONTACT(i);

  ct->id[0] = ((contact->number() / 10000000) << 4) | ((contact->number() / 1000000) % 10);
  ct->id[1] = ((contact->number() / 100000 % 10) << 4) | ((contact->number() / 10000) % 10);
  ct->id[2] = ((contact->number() / 1000 % 10) << 4) | ((contact->number() / 100) % 10);
  ct->id[3] = ((contact->number() / 10 % 10) << 4) | (contact->number() % 10);
  switch (contact->type()) {
    case DigitalContact::PrivateCall:
      ct->type = CALL_PRIVATE;
      break;
    case DigitalContact::GroupCall:
      ct->type = CALL_GROUP;
      break;
    case DigitalContact::AllCall:
      ct->type = CALL_ALL;
      break;
  }
  ct->receive_tone = (contact->rxTone() ? 1 : 0);
  ct->ring_style   = 0; // TODO
  ct->_unused23    = (contact->type() != DigitalContact::AllCall) ? 0 : 0xff;
  memset(ct->name, 0xff, sizeof(ct->name));
  memcpy(ct->name, contact->name().toLocal8Bit(), std::min(int(sizeof(ct->name)),contact->name().size()));

  return true;
}


bool
RD5RCodePlug::_pack_grouplist(Config *config, int i, RXGroupList *list) {
  grouptab_t *gt = GET_GROUPTAB();
  grouplist_t *gl = &gt->grouplist[i];

  memset(gl->name, 0xff, sizeof(gl->name));
  memcpy(gl->name, list->name().toLocal8Bit().constData(), std::min(int(sizeof(gl->name)), list->name().size()));

  // Enable grouplist.
  gt->nitems1[i] = 1;

  // RD-5R firmware up to v2.1.6 has a bug:
  // when all 16 entries in RX group list are occupied,
  // it stops functioning and no audio is received.
  // As a workaround, we must use only 15 entries here.
  for (int j=0; j<std::min(15,list->count()); j++)
    gl->member[j] = config->contacts()->indexOf(list->contact(j))+1;
  gt->nitems1[i] = std::min(15,list->count())+1;

  return true;
}

