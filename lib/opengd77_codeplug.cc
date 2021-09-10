#include "opengd77_codeplug.hh"
#include "config.hh"
#include "channel.hh"
#include "utils.hh"
#include "logger.hh"
#include <QDateTime>
#include <QtEndian>

// Stored in EEPROM
#define OFFSET_SETTINGS     0x000e0
#define OFFSET_MSGTAB       0x00128
#define OFFSET_SCANTAB      0x01790
#define OFFSET_BANK_0       0x03780 // Channels 1-128
#define OFFSET_INTRO        0x07540
#define OFFSET_ZONETAB      0x08010
// Stored in flash
#define OFFSET_BANK_1       0x7b1b0 // Channels 129-1024
#define OFFSET_CONTACTS     0x87620
#define OFFSET_GROUPTAB     0x8d620


/* ******************************************************************************************** *
 * Implementation of OpenGD77Codeplug::channel_t
 * ******************************************************************************************** */
bool
OpenGD77Codeplug::channel_t::isValid() const {
  // Channel is enabled/disabled at channel bank
  return true;
}

void
OpenGD77Codeplug::channel_t::clear() {
  memset(name, 0xff, 16);
  power                  = POWER_GLOBAL;
  _unused26              = 0;
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
OpenGD77Codeplug::channel_t::getRXFrequency() const {
  return decode_frequency(rx_frequency);
}
void
OpenGD77Codeplug::channel_t::setRXFrequency(double freq) {
  rx_frequency = encode_frequency(freq);
}

double
OpenGD77Codeplug::channel_t::getTXFrequency() const {
  return decode_frequency(tx_frequency);
}
void
OpenGD77Codeplug::channel_t::setTXFrequency(double freq) {
  tx_frequency = encode_frequency(freq);
}

QString
OpenGD77Codeplug::channel_t::getName() const {
  return decode_ascii(name, 16, 0xff);
}
void
OpenGD77Codeplug::channel_t::setName(const QString &n) {
  encode_ascii(name, n, 16, 0xff);
}

Signaling::Code
OpenGD77Codeplug::channel_t::getRXTone() const {
  return decode_ctcss_tone_table(ctcss_dcs_receive);
}
void
OpenGD77Codeplug::channel_t::setRXTone(Signaling::Code tone) {
  ctcss_dcs_receive = encode_ctcss_tone_table(tone);
}

Signaling::Code
OpenGD77Codeplug::channel_t::getTXTone() const {
  return decode_ctcss_tone_table(ctcss_dcs_transmit);
}
void
OpenGD77Codeplug::channel_t::setTXTone(Signaling::Code tone) {
  ctcss_dcs_transmit = encode_ctcss_tone_table(tone);
}

Channel *
OpenGD77Codeplug::channel_t::toChannelObj() const {
  QString name = getName();
  double rxF = getRXFrequency();
  double txF = getTXFrequency();
  // decode power setting
  Channel::Power pwr = Channel::Power::Low;
  if (POWER_GLOBAL == power) {
    pwr = Channel::Power::Low;
  } else if (POWER_50mW == power) {
    pwr = Channel::Power::Min;
  } else if ((POWER_250mW == power) || (POWER_500mW == power) || (POWER_750mW == power) || (POWER_1W == power)){
    pwr = Channel::Power::Low;
  } else if ((POWER_2W == power) || (POWER_3W == power)) {
    pwr = Channel::Power::Mid;
  } else if ((POWER_4W == power) || (POWER_5W == power)) {
    pwr = Channel::Power::High;
  } else if (POWER_MAX == power) {
    pwr = Channel::Power::Max;
  }

  uint timeout = tot*15;
  bool rxOnly = rx_only;
  if (MODE_ANALOG == channel_mode) {
    AnalogChannel::Admit admit;
    switch (admit_criteria) {
    case ADMIT_ALWAYS: admit = AnalogChannel::Admit::Always; break;
    case ADMIT_CH_FREE: admit = AnalogChannel::Admit::Free; break;
    default:
      logError() << "Unknwon admit criterion " << int(admit_criteria);
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
      logError() << "Unknwon admit criterion " << int(admit_criteria);
      return nullptr;
    }
    DigitalChannel::TimeSlot slot = repeater_slot2 ?
          DigitalChannel::TimeSlot::TS2 : DigitalChannel::TimeSlot::TS1;
    return new DigitalChannel(
          name, rxF, txF, pwr, timeout, rxOnly, admit, colorcode_rx, slot,
          nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
  }

  logError() << "Unknwon channel type " << int(channel_mode);

  return nullptr;
}

bool
OpenGD77Codeplug::channel_t::linkChannelObj(Channel *c, const CodeplugContext &ctx) const {
  if (c->is<AnalogChannel>()) {
    AnalogChannel *ac = c->as<AnalogChannel>();
    if (scan_list_index && ctx.hasScanList(scan_list_index))
      ac->setScanListObj(ctx.getScanList(scan_list_index));
  } else {
    DigitalChannel *dc = c->as<DigitalChannel>();
    if (scan_list_index && ctx.getScanList(scan_list_index))
      dc->setScanListObj(ctx.getScanList(scan_list_index));
    if (group_list_index && ctx.getGroupList(group_list_index))
      dc->setGroupListObj(ctx.getGroupList(group_list_index));
    if (contact_name_index && ctx.hasDigitalContact(contact_name_index))
      dc->setTXContactObj(ctx.getDigitalContact(contact_name_index));
  }
  return true;
}

void
OpenGD77Codeplug::channel_t::fromChannelObj(const Channel *c, const Config *conf) {
  clear();

  setName(c->name());
  setRXFrequency(c->rxFrequency());
  setTXFrequency(c->txFrequency());

  // encode power setting
  switch (c->power()) {
  case Channel::Power::Max:
    power = POWER_MAX;
  case Channel::Power::High:
    power = POWER_5W;
    break;
  case Channel::Power::Mid:
    power = POWER_3W;
  case Channel::Power::Low:
    power = POWER_1W;
  case Channel::Power::Min:
    power = POWER_50mW;
    break;
  }

  tot = c->timeout()/15;
  rx_only = c->rxOnly() ? 1 : 0;
  bandwidth = BW_12_5_KHZ;

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
    scan_list_index = conf->scanlists()->indexOf(ac->scanListObj())+1;
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
    scan_list_index = conf->scanlists()->indexOf(dc->scanListObj()) + 1;
    group_list_index = conf->rxGroupLists()->indexOf(dc->groupListObj()) + 1;
    contact_name_index = conf->contacts()->indexOfDigital(dc->txContactObj()) + 1;
  }
}


/* ******************************************************************************************** *
 * Implementation of OpenGD77Codeplug::contact_t
 * ******************************************************************************************** */
OpenGD77Codeplug::contact_t::contact_t() {
  clear();
}

void
OpenGD77Codeplug::contact_t::clear() {
  memset(name, 0xff, 16);
  memset(id, 0x00, 4);
  type = CALL_GROUP;
  receive_tone = ring_style = 0;
  timeslot_override = TSO_NONE;
}

bool
OpenGD77Codeplug::contact_t::isValid() const {
  return (0x00 != name[0]) && (0xff != name[0]);
}

uint32_t
OpenGD77Codeplug::contact_t::getId() const {
  return decode_dmr_id_bcd(id);
}
void
OpenGD77Codeplug::contact_t::setId(uint32_t num) {
  encode_dmr_id_bcd(id, num);
}

QString
OpenGD77Codeplug::contact_t::getName() const {
  return decode_ascii(name, 16, 0xff);
}
void
OpenGD77Codeplug::contact_t::setName(const QString &n) {
  encode_ascii(name, n, 16, 0xff);
}

DigitalContact *
OpenGD77Codeplug::contact_t::toContactObj() const {
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
OpenGD77Codeplug::contact_t::fromContactObj(const DigitalContact *cont, const Config *conf) {
  Q_UNUSED(conf);
  timeslot_override = TSO_NONE;
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
 * Implementation of OpenGD77Codeplug::zone_t
 * ******************************************************************************************** */
OpenGD77Codeplug::zone_t::zone_t() {
  clear();
}

bool
OpenGD77Codeplug::zone_t::isValid() const {
  return 0xff != name[0];
}
void
OpenGD77Codeplug::zone_t::clear() {
  memset(name, 0xff, sizeof(name));
  memset(member, 0x00, sizeof(member));
}

QString
OpenGD77Codeplug::zone_t::getName() const {
  return decode_ascii(name, 16, 0xff);
}
void
OpenGD77Codeplug::zone_t::setName(const QString &n) {
  encode_ascii(name, n, 16, 0xff);
}

Zone *
OpenGD77Codeplug::zone_t::toZoneObj() const {
  if (! isValid())
    return nullptr;
  return new Zone(getName());
}

bool
OpenGD77Codeplug::zone_t::linkZoneObj(Zone *zone, const CodeplugContext &ctx, bool putInB) const {
  if (! isValid()) {
    logDebug() << "Cannot link zone: Zone is invalid.";
    return false;
  }

  for (int i=0; (i<80) && member[i]; i++) {
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
OpenGD77Codeplug::zone_t::fromZoneObjA(const Zone *zone, const Config *conf) {
  if (zone->A()->count() && zone->B()->count())
    setName(zone->name() + " A");
  else
    setName(zone->name());

  for (int i=0; i<80; i++) {
    if (i < zone->A()->count())
      member[i] = conf->channelList()->indexOf(zone->A()->get(i))+1;
    else
      member[i] = 0;
  }
}

void
OpenGD77Codeplug::zone_t::fromZoneObjB(const Zone *zone, const Config *conf) {
  if (zone->A()->count() && zone->B()->count())
    setName(zone->name() + " B");
  else
    setName(zone->name());

  for (int i=0; i<80; i++) {
    if (i < zone->B()->count())
      member[i] = conf->channelList()->indexOf(zone->B()->get(i))+1;
    else
      member[i] = 0;
  }
}


/* ******************************************************************************************** *
 * Implementation of OpenGD77Codeplug
 * ******************************************************************************************** */
OpenGD77Codeplug::OpenGD77Codeplug(QObject *parent)
  : GD77Codeplug(parent)
{
  remImage(0);

  addImage("OpenGD77 Codeplug EEPROM");
  image(0).addElement(0x000e0, 0x05f20);
  image(0).addElement(0x07500, 0x0bb00);
  addImage("OpenGD77 Codeplug FLASH");
  image(1).addElement(0x00000, 0x011a0);
  image(1).addElement(0x7b000, 0x13e60);
}

bool
OpenGD77Codeplug::encode(Config *config, const Flags &flags) {
  // pack basic config
  general_settings_t *gs = (general_settings_t*) data(OFFSET_SETTINGS, EEPROM);
  if (! flags.updateCodePlug)
    gs->initDefault();

  if (nullptr == config->radioIDs()->defaultId()) {
    _errorMessage = tr("Cannot encode OpenGD77 codeplug: No default radio ID specified.");
    return false;
  }
  gs->setName(config->radioIDs()->defaultId()->name());
  gs->setRadioId(config->radioIDs()->defaultId()->number());

  intro_text_t *it = (intro_text_t*) data(OFFSET_INTRO, EEPROM);
  it->setIntroLine1(config->introLine1());
  it->setIntroLine2(config->introLine2());

  // Pack channels
  for (int i=0; i<NCHAN; i++) {
    // First, get bank
    bank_t *b;
    if ((i>>7) == 0)
      b = (bank_t*) data(OFFSET_BANK_0, EEPROM);
    else
      b = (((i>>7)-1) + (bank_t *) data(OFFSET_BANK_1, FLASH));
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
    zonetab_t *zt = (zonetab_t*) data(OFFSET_ZONETAB, EEPROM);
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
  /*
   * Scan lists are not supported by OpenGD77 right now, hence encoding is disabled for now.
  for (int i=0; i<NSCANL; i++) {
    scantab_t *st = (scantab_t*) data(OFFSET_SCANTAB, EEPROM);
    scanlist_t *sl = &st->scanlist[i];

    if (i >= config->scanlists()->count()) {
      // Clear valid bit.
      st->valid[i] = 0;
      continue;
    }

    sl->fromScanListObj(config->scanlists()->scanlist(i), config);
    st->valid[i] = 1;
  } */

  // Pack contacts
  for (int i=0; i<NCONTACTS; i++) {
    contact_t *ct = (contact_t*) data(OFFSET_CONTACTS + (i)*sizeof(contact_t), FLASH);
    ct->clear();
    if (i >= config->contacts()->digitalCount())
      continue;
    ct->fromContactObj(config->contacts()->digitalContact(i), config);
  }

  // Pack Grouplists:
  for (int i=0; i<NGLISTS; i++) {
    grouptab_t *gt = (grouptab_t*) data(OFFSET_GROUPTAB, FLASH);
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

bool
OpenGD77Codeplug::decode(Config *config) {
  // Clear config object
  config->reset();

  /* Unpack general config */
  general_settings_t *gs = (general_settings_t*) data(OFFSET_SETTINGS, EEPROM);
  if (nullptr == gs) {
    _errorMessage = QString("%1(): Cannot access general settings memory!")
        .arg(__func__);
    return false;
  }

  int idx = config->radioIDs()->addId(gs->getName(), gs->getRadioId());
  config->radioIDs()->setDefaultId(idx);

  intro_text_t *it = (intro_text_t*) data(OFFSET_INTRO, EEPROM);
  config->setIntroLine1(it->getIntroLine1());
  config->setIntroLine2(it->getIntroLine2());

  CodeplugContext ctx(config);

  /* Unpack Contacts */
  for (int i=0; i<NCONTACTS; i++) {
    contact_t *ct = (contact_t *) data(OFFSET_CONTACTS+i*sizeof(contact_t), FLASH);
    if (nullptr == ct) {
      _errorMessage = QString("%1(): Cannot access contact memory at index %2!")
          .arg(__func__).arg(i);
      return false;
    }
    // If contact is disabled
    if (! ct->isValid())
      continue;
    if (DigitalContact *cont = ct->toContactObj()) {
      logDebug() << "Contact " << i+1 << " enabled, mapped to index "
                 << config->contacts()->digitalCount();
      // Store in index table & config
      ctx.addDigitalContact(cont, i+1);
    } else {
      _errorMessage = QString("%1(): Cannot decode codeplug: Invalid contact at index %2.")
          .arg(__func__).arg(i);
      return false;
    }
  }

  /* Unpack RX Group Lists */
  for (int i=0; i<NGLISTS; i++) {
    grouptab_t *gt = (grouptab_t*) data(OFFSET_GROUPTAB, FLASH);
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
    if(! gl->linkRXGroupListObj(list, ctx)) {
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
      b = (bank_t*) data(OFFSET_BANK_0, EEPROM);
    else
      b = (((i>>7)-1) + (bank_t*) data(OFFSET_BANK_1, FLASH));
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
      logDebug() << "Mapped channel index " << i+1 << " to " << config->channelList()->count();
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
    zonetab_t *zt = (zonetab_t*) data(OFFSET_ZONETAB, EEPROM);
    if (! zt){
      _errorMessage = QString("%1(): Cannot access zone table memory.")
          .arg(__func__);
      logError() << _errorMessage;
      return false;
    }

    // if zone is disabled -> skip
    if (! ((zt->bitmap[i / 8] >> (i & 7)) & 1) )
      continue;
    logDebug() << "Unpack zone " << i+1 << "...";

    // get zone_t
    zone_t *z = &zt->zone[i];
    if (! z){
      _errorMessage = QString("%1(): Cannot access zone at index %2")
          .arg(__func__).arg(i);
      logError() << _errorMessage;
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
        logError() << _errorMessage;
        return false;
      }
      config->zones()->add(last_zone);
    } else {
      // when extending the last zone, chop its name to remove the "... A" part.
      last_zone->setName(last_zonebasename);
    }

    // Link zone
    if (! z->linkZoneObj(last_zone, ctx, extend_last_zone)) {
      _errorMessage = QString("%1(): Cannot unpack codeplug: Cannot link zone at index %2")
          .arg(__func__).arg(i);
      logError() << _errorMessage;
      return false;
    }
  }

  /* Unpack Scan lists
   *
   * OpenGD77 does not support any scan lists. Decoding/encoding is disabled for now.
  for (int i=0; i<NSCANL; i++) {
    scantab_t *st = (scantab_t*) data(OFFSET_SCANTAB, EEPROM);
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

    if (! sl->linkScanListObj(scan, config, channel_table)) {
      _errorMessage = QString("%1(): Cannot unpack codeplug: Cannot link scanlist at index %2")
          .arg(__func__).arg(i);
      scan->deleteLater();
      return false;
    }

    logDebug() << "Map scan-list at index " << i+1 << " to " << config->scanlists()->count();
    scan_table[i+1] = config->scanlists()->count();
    config->scanlists()->addScanList(scan);
  } */

  /*
   * Link Channels -> contacts, group lists, scan lists.
   */
  for (int i=0; i<NCHAN; i++) {
    // First, get bank
    bank_t *b;
    if ((i>>7) == 0)
      b = (bank_t*) data(OFFSET_BANK_0, EEPROM);
    else
      b = (((i>>7)-1) + (bank_t*) data(OFFSET_BANK_1, FLASH));
    // If channel is disabled
    if (! ((b->bitmap[i % 128 / 8] >> (i & 7)) & 1) )
      continue;
    // finally, get channel
    channel_t *ch = &b->chan[i % 128];

    if (! ch->linkChannelObj(ctx.getChannel(i+1), ctx))
    {
      _errorMessage = QString("%1(): Cannot unpack codeplug: Cannot link channel at index %2")
          .arg(__func__).arg(i);
      return false;
    }
  }

  return true;
}
