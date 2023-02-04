#include "rd5r_codeplug.hh"
#include "config.hh"
#include "channel.hh"
#include "utils.hh"
#include "logger.hh"
#include <QDateTime>


/* ******************************************************************************************** *
 * Implementation of RD5RCodeplug::ChannelElement
 * ******************************************************************************************** */
RD5RCodeplug::ChannelElement::ChannelElement(uint8_t *ptr, size_t size)
  : RadioddityCodeplug::ChannelElement(ptr, size)
{
  // pass...
}

RD5RCodeplug::ChannelElement::ChannelElement(uint8_t *ptr)
  : RadioddityCodeplug::ChannelElement(ptr)
{
  // pass...
}

void
RD5RCodeplug::ChannelElement::clear() {
  RadioddityCodeplug::ChannelElement::clear();
  setSquelch(0);
}

unsigned
RD5RCodeplug::ChannelElement::squelch() const {
  return getUInt8(Offset::squelch());
}
void
RD5RCodeplug::ChannelElement::setSquelch(unsigned level) {
  level = std::min(9u, level);
  setUInt8(Offset::squelch(), level);
}

bool
RD5RCodeplug::ChannelElement::fromChannelObj(const Channel *c, Context &ctx, const ErrorStack& err) {
  if (! RadioddityCodeplug::ChannelElement::fromChannelObj(c, ctx, err))
    return false;

  if (c->is<FMChannel>()) {
    const FMChannel *ac = c->as<FMChannel>();
    if (ac->defaultSquelch())
      setSquelch(ctx.config()->settings()->squelch());
    else if (ac->squelchDisabled())
      setSquelch(0);
    else
      setSquelch(ac->squelch());
  } else {
    // If digital channel, reuse global quelch setting
    setSquelch(ctx.config()->settings()->squelch());
  }

  return true;
}

Channel *
RD5RCodeplug::ChannelElement::toChannelObj(Context &ctx, const ErrorStack& err) const {
  Channel *ch = RadioddityCodeplug::ChannelElement::toChannelObj(ctx, err);
  if (nullptr == ch)
    return nullptr;

  if (ch->is<FMChannel>()) {
    FMChannel *ac = ch->as<FMChannel>();
    ac->setSquelch(squelch());
  }

  return ch;
}

bool
RD5RCodeplug::ChannelElement::linkChannelObj(Channel *c, Context &ctx, const ErrorStack& err) const {
  if (! RadioddityCodeplug::ChannelElement::linkChannelObj(c, ctx, err))
    return false;
  /*
  if (c->is<AnalogChannel>()) {
    AnalogChannel *ac = c->as<AnalogChannel>();
    if (ctx.config()->settings()->squelch() == ac->squelch()) {
      ac->setSquelchDefault();
    }
  }
  */
  return true;
}

/* ********************************************************************************************* *
 * Implementation of RD5RCodeplug::TimestampElement
 * ********************************************************************************************* */
RD5RCodeplug::TimestampElement::TimestampElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

RD5RCodeplug::TimestampElement::TimestampElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}

RD5RCodeplug::TimestampElement::~TimestampElement() {
  // pass...
}

void
RD5RCodeplug::TimestampElement::clear() {
  set();
}

QDateTime
RD5RCodeplug::TimestampElement::get() const {
  return QDateTime(QDate(getBCD4_be(Offset::year()), getBCD2(Offset::month()), getBCD2(Offset::day())),
                   QTime(getBCD2(Offset::hour()), getBCD2(Offset::minute())));
}
void
RD5RCodeplug::TimestampElement::set(const QDateTime &ts) {
  setBCD4_be(Offset::year(), ts.date().year());
  setBCD2(Offset::month(), ts.date().month());
  setBCD2(Offset::day(), ts.date().day());
  setBCD2(Offset::hour(), ts.time().hour());
  setBCD2(Offset::minute(), ts.time().minute());
}


/* ******************************************************************************************** *
 * Implementation of RD5RCodeplug::EncryptionElement
 * ******************************************************************************************** */
RD5RCodeplug::EncryptionElement::EncryptionElement(uint8_t *ptr)
  : RadioddityCodeplug::EncryptionElement(ptr)
{
  // pass...
}

bool
RD5RCodeplug::EncryptionElement::isBasicKeySet(unsigned n) const {
  if (n>0)
    return false;
  return RadioddityCodeplug::EncryptionElement::isBasicKeySet(n);
}

QByteArray
RD5RCodeplug::EncryptionElement::basicKey(unsigned n) const {
  if (n>0)
    return QByteArray();
  return QByteArray("\x53\x47\x4c\x39");
}

void
RD5RCodeplug::EncryptionElement::setBasicKey(unsigned n, const QByteArray &key) {
  if ((0 != n) || (key != "\x53\x47\x4c\x39")){
    logError() << "The RD5R only supports a single fixed DMR basic key '53474c39'.";
    return;
  }
  RD5RCodeplug::EncryptionElement::setBasicKey(n, key);
}


/* ******************************************************************************************** *
 * Implementation of RD5RCodeplug
 * ******************************************************************************************** */
RD5RCodeplug::RD5RCodeplug(QObject *parent)
  : RadioddityCodeplug(parent)
{
  addImage("Radioddity RD5R Codeplug");
  image(0).addElement(0x00080, 0x07b80);
  image(0).addElement(0x08000, 0x16300);
}

void
RD5RCodeplug::clear() {
  RadioddityCodeplug::clear();
  this->clearTimestamp();
}

bool
RD5RCodeplug::encodeElements(const Flags &flags, Context &ctx, const ErrorStack &err) {
  if (! RadioddityCodeplug::encodeElements(flags, ctx, err))
    return false;

  // Set timestamp
  if (! this->encodeTimestamp(err)) {
    errMsg(err) << "Cannot encode time-stamp.";
    return false;
  }

  return true;
}

bool
RD5RCodeplug::decodeElements(Context &ctx, const ErrorStack &err) {
  if (! RadioddityCodeplug::decodeElements(ctx, err))
    return false;
  return true;
}

void
RD5RCodeplug::clearTimestamp() {
  encodeTimestamp();
}

bool
RD5RCodeplug::encodeTimestamp(const ErrorStack &err) {
  Q_UNUSED(err)
  TimestampElement(data(Offset::timestamp())).set();
  return true;
}

void
RD5RCodeplug::clearGeneralSettings() {
  TimestampElement(data(Offset::settings())).clear();
}

bool
RD5RCodeplug::encodeGeneralSettings(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)
  GeneralSettingsElement el(data(Offset::settings()));
  if (! flags.updateCodePlug)
    el.clear();
  return el.fromConfig(config, ctx);
}

bool
RD5RCodeplug::decodeGeneralSettings(Config *config, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)
  return GeneralSettingsElement(data(Offset::settings())).updateConfig(config, ctx);
}

void
RD5RCodeplug::clearButtonSettings() {
  ButtonSettingsElement(data(Offset::buttons())).clear();
}

void
RD5RCodeplug::clearMessages() {
  MessageBankElement(data(Offset::messages())).clear();
}

void
RD5RCodeplug::clearContacts() {
  for (unsigned int i=0; i<Limit::contactCount(); i++)
    ContactElement(data(Offset::contacts() + i*ContactElement::size())).clear();
}

bool
RD5RCodeplug::encodeContacts(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)
  for (unsigned int i=0; i<Limit::contactCount(); i++) {
    ContactElement el(data(Offset::contacts() + i*ContactElement::size()));
    el.clear();
    if (i >= (unsigned int) config->contacts()->digitalCount())
      continue;
    if (! el.fromContactObj(config->contacts()->digitalContact(i), ctx, err)) {
      errMsg(err) << "Cannot encode contact '" << config->contacts()->digitalContact(i)->name()
                  << "' at index " << i  << ".";
      return false;
    }
  }
  return true;
}

bool
RD5RCodeplug::createContacts(Config *config, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)
  /* Unpack Contacts */
  for (unsigned int i=0; i<Limit::contactCount(); i++) {
    ContactElement el(data(Offset::contacts() + i*ContactElement::size()));
    if (! el.isValid())
      continue;

    DMRContact *cont = el.toContactObj(ctx);
    ctx.add(cont, i+1); config->contacts()->add(cont);
  }
  return true;
}

void
RD5RCodeplug::clearDTMFContacts() {
  for (unsigned int i=0; i<Limit::dtmfContactCount(); i++)
    DTMFContactElement(data(Offset::dtmfContacts() + i*DTMFContactElement::size())).clear();
}

bool
RD5RCodeplug::encodeDTMFContacts(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)
  for (unsigned int i=0; i<Limit::dtmfContactCount(); i++) {
    DTMFContactElement el(data(Offset::dtmfContacts() + i*DTMFContactElement::size()));
    el.clear();
    if (i >= (unsigned int) config->contacts()->dtmfCount())
      continue;
    if (! el.fromContactObj(config->contacts()->dtmfContact(i), ctx, err)) {
      errMsg(err) << "Cannot encode DTMF contact '" << config->contacts()->dtmfContact(i)->name()
                  << "' at index " << i << ".";
      return false;
    }
  }
  return true;
}

bool
RD5RCodeplug::createDTMFContacts(Config *config, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)
  for (unsigned int i=0; i<Limit::dtmfContactCount(); i++) {
    DTMFContactElement el(data(Offset::dtmfContacts()+i*DTMFContactElement::size()));
    // If contact is disabled
    if (! el.isValid())
      continue;
    DTMFContact *cont = el.toContactObj(ctx, err);
    if (nullptr == cont) {
      errMsg(err) << "Cannot decode DTMF contact at index " << i << ".";
      return false;
    }
    ctx.add(cont, i+1); config->contacts()->add(cont);
  }
  return true;
}

void
RD5RCodeplug::clearChannels() {
  for (unsigned int b=0,c=0; b<Limit::channelBankCount(); b++) {
    uint8_t *ptr = nullptr;
    if (0 == b) ptr = data(Offset::channelBank0());
    else ptr = data(Offset::channelBank1() + (b-1)*ChannelBankElement::size());
    ChannelBankElement bank(ptr); bank.clear();
    for (unsigned int i=0; (i<ChannelBankElement::Limit::channelCount())&&(c<Limit::channelCount()); i++, c++)
      ChannelElement(bank.get(i)).clear();
  }
}

bool
RD5RCodeplug::encodeChannels(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)
  for (unsigned int b=0,c=0; b<Limit::channelBankCount(); b++) {
    uint8_t *ptr = nullptr;
    if (0 == b) ptr = data(Offset::channelBank0());
    else ptr = data(Offset::channelBank1() + (b-1)*ChannelBankElement::size());
    ChannelBankElement bank(ptr); bank.clear();
    for (unsigned int i=0; (i<ChannelBankElement::Limit::channelCount())&&(c<Limit::channelCount()); i++, c++) {
      ChannelElement el(bank.get(i));
      if (c < (unsigned int) config->channelList()->count()) {
        if (! el.fromChannelObj(config->channelList()->channel(c), ctx, err)) {
          errMsg(err) << "Cannot encode channel " << c << " (" << i << " of bank " << b <<").";
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
RD5RCodeplug::createChannels(Config *config, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)
  for (unsigned int b=0,c=0; b<Limit::channelBankCount(); b++) {
    uint8_t *ptr = nullptr;
    if (0 == b) ptr = data(Offset::channelBank0());
    else ptr = data(Offset::channelBank1() + (b-1)*ChannelBankElement::size());
    ChannelBankElement bank(ptr);
    for (unsigned int i=0; (i<ChannelBankElement::Limit::channelCount())&&(c<Limit::channelCount()); i++, c++) {
      if (! bank.isEnabled(i))
        continue;
      Channel *ch = ChannelElement(bank.get(i)).toChannelObj(ctx, err);
      if (nullptr == ch) {
        errMsg(err) << "Cannot create channel at index " << i << ".";
        return false;
      }
      config->channelList()->add(ch); ctx.add(ch, c+1);
    }
  }
  return true;
}

bool
RD5RCodeplug::linkChannels(Config *config, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(config); Q_UNUSED(err)
  for (unsigned int b=0,c=0; b<Limit::channelBankCount(); b++) {
    uint8_t *ptr = nullptr;
    if (0 == b) ptr = data(Offset::channelBank0());
    else ptr = data(Offset::channelBank1() + (b-1)*ChannelBankElement::size());
    ChannelBankElement bank(ptr);
    for (unsigned int i=0; (i<ChannelBankElement::Limit::channelCount())&&(c<Limit::channelCount()); i++, c++) {
      if (! bank.isEnabled(i))
        continue;
      if (!ChannelElement(bank.get(i)).linkChannelObj(ctx.get<Channel>(c+1), ctx, err))
        return false;
    }
  }
  return true;
}

void
RD5RCodeplug::clearBootSettings() {
  BootSettingsElement(data(Offset::bootSettings())).clear();
}

void
RD5RCodeplug::clearMenuSettings() {
  MenuSettingsElement(data(Offset::menuSettings())).clear();
}

void
RD5RCodeplug::clearBootText() {
  BootTextElement(data(Offset::bootText())).clear();
}

bool
RD5RCodeplug::encodeBootText(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(config);
  BootTextElement(data(Offset::bootText())).fromConfig(ctx, err);
  return true;
}

bool
RD5RCodeplug::decodeBootText(Config *config, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(config)
  BootTextElement(data(Offset::bootText())).updateConfig(ctx, err);
  return true;
}

void
RD5RCodeplug::clearVFOSettings() {
  ChannelElement(data(Offset::vfoA())).clear();
  ChannelElement(data(Offset::vfoB())).clear();
}

void
RD5RCodeplug::clearZones() {
  ZoneBankElement bank(data(Offset::zoneBank()));
  bank.clear();
  for (unsigned int i=0; i<ZoneBankElement::Limit::zoneCount(); i++)
    ZoneElement(bank.get(i)).clear();
}

bool
RD5RCodeplug::encodeZones(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags)

  ZoneBankElement bank(data(Offset::zoneBank()));

  // Pack Zones
  bool pack_zone_a = true;
  for (unsigned int i=0, j=0; i<ZoneBankElement::Limit::zoneCount(); i++) {
    ZoneElement z(bank.get(i));
next:
    if (j >= (unsigned int)config->zones()->count()) {
      bank.enable(i, false);
      continue;
    }

    // Construct from Zone obj
    Zone *zone = config->zones()->zone(j);
    if (pack_zone_a) {
      pack_zone_a = false;
      if (zone->A()->count())
        z.fromZoneObjA(zone, ctx, err);
      else
        goto next;
    } else {
      pack_zone_a = true;
      j++;
      if (zone->B()->count())
        z.fromZoneObjB(zone, ctx, err);
      else
        goto next;
    }
    bank.enable(i, true);
  }

  return true;
}

bool
RD5RCodeplug::createZones(Config *config, Context &ctx, const ErrorStack &err) {
  QString last_zonename, last_zonebasename; Zone *last_zone = nullptr;
  bool extend_last_zone = false;
  ZoneBankElement bank(data(Offset::zoneBank()));

  for (unsigned int i=0; i<ZoneBankElement::Limit::zoneCount(); i++) {
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
      last_zone = z.toZoneObj(ctx, err);
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
RD5RCodeplug::linkZones(Config *config, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(config)

  QString last_zonename, last_zonebasename; Zone *last_zone = nullptr;
  bool extend_last_zone = false;
  ZoneBankElement bank(data(Offset::zoneBank()));

  for (unsigned int i=0; i<ZoneBankElement::Limit::zoneCount(); i++) {
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
    if (! z.linkZoneObj(last_zone, ctx, extend_last_zone, err)) {
      errMsg(err) << "Cannot link zone at index " << i << ".";
      return false;
    }
  }

  return true;
}

void
RD5RCodeplug::clearScanLists() {
  ScanListBankElement bank(data(Offset::scanListBank())); bank.clear();
  for (unsigned int i=0; i<ScanListBankElement::Limit::scanListCount(); i++)
    ScanListElement(bank.get(i)).clear();
}

bool
RD5RCodeplug::encodeScanLists(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags);

  ScanListBankElement bank(data(Offset::scanListBank()));
  for (unsigned int i=0; i<ScanListBankElement::Limit::scanListCount(); i++) {
    if (i >= (unsigned int) config->scanlists()->count()) {
      bank.enable(i, false);
      continue;
    }
    if (! ScanListElement(bank.get(i)).fromScanListObj(config->scanlists()->scanlist(i), ctx, err)) {
      errMsg(err) << "Cannot encode scan list at index " << i << ".";
      return false;
    }
    bank.enable(i, true);
  }
  return true;
}

bool
RD5RCodeplug::createScanLists(Config *config, Context &ctx, const ErrorStack &err) {
  ScanListBankElement bank(data(Offset::scanListBank()));
  for (unsigned int i=0; i<ScanListBankElement::Limit::scanListCount(); i++) {
    if (! bank.isEnabled(i))
      continue;
    ScanListElement el(bank.get(i));
    ScanList *scan = el.toScanListObj(ctx);
    if (nullptr == scan) {
      errMsg(err) << "Cannot decode scan list at index " << i << ".";
      return false;
    }
    config->scanlists()->add(scan); ctx.add(scan, i+1);
  }
  return true;
}

bool
RD5RCodeplug::linkScanLists(Config *config, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(config);

  ScanListBankElement bank(data(Offset::scanListBank()));
  for (unsigned int i=0; i<ScanListBankElement::Limit::scanListCount(); i++) {
    if (! bank.isEnabled(i))
      continue;
    if (! ScanListElement(bank.get(i)).linkScanListObj(ctx.get<ScanList>(i+1), ctx, err)) {
      errMsg(err) << "Cannot link scan list '" << ctx.get<ScanList>(i+1)
                  << "' at index " << i << ".";
      return false;
    }
  }

  return true;
}

void
RD5RCodeplug::clearGroupLists() {
  GroupListBankElement bank(data(Offset::groupListBank())); bank.clear();
  for (unsigned int i=0; i<GroupListBankElement::Limit::groupListCount(); i++)
    GroupListElement(bank.get(i)).clear();
}

bool
RD5RCodeplug::encodeGroupLists(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags);

  GroupListBankElement bank(data(Offset::groupListBank())); bank.clear();
  for (unsigned int i=0; i<GroupListBankElement::Limit::groupListCount(); i++) {
    if (i >= (unsigned int)config->rxGroupLists()->count())
      continue;
    GroupListElement el(bank.get(i));
    el.fromRXGroupListObj(config->rxGroupLists()->list(i), ctx, err);
    // Only group calls are encoded
    int count = 0;
    for (int j=0; j<config->rxGroupLists()->list(i)->count(); j++)
      if (DMRContact::GroupCall == config->rxGroupLists()->list(i)->contact(j)->type())
        count++;
    bank.setContactCount(i, count);
  }

  return true;
}

bool
RD5RCodeplug::createGroupLists(Config *config, Context &ctx, const ErrorStack &err) {
  GroupListBankElement bank(data(Offset::groupListBank()));
  for (unsigned int i=0; i<GroupListBankElement::Limit::groupListCount(); i++) {
    if (! bank.isEnabled(i))
      continue;
    GroupListElement el(bank.get(i));
    RXGroupList *list = el.toRXGroupListObj(ctx, err);
    config->rxGroupLists()->add(list); ctx.add(list, i+1);
  }
  return true;
}

bool
RD5RCodeplug::linkGroupLists(Config *config, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(config)

  GroupListBankElement bank(data(Offset::groupListBank()));
  for (unsigned int i=0; i<GroupListBankElement::Limit::groupListCount(); i++) {
    if (! bank.isEnabled(i))
      continue;
    GroupListElement el(bank.get(i));
    /*logDebug() << "Link " << bank.contactCount(i) << " members of group list '"
               << ctx.get<RXGroupList>(i+1)->name() << "'.";*/
    if (! el.linkRXGroupListObj(bank.contactCount(i), ctx.get<RXGroupList>(i+1), ctx, err)) {
      errMsg(err) << "Cannot link group list '" << ctx.get<RXGroupList>(i+1)->name() << "'.";
      return false;
    }
  }
  return true;
}


void
RD5RCodeplug::clearEncryption() {
  EncryptionElement enc(data(Offset::encryption()));
  enc.clear();
}

bool
RD5RCodeplug::encodeEncryption(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags);
  clearEncryption();
  EncryptionElement enc(data(Offset::encryption()));
  return enc.fromCommercialExt(config->commercialExtension(), ctx, err);
}

bool
RD5RCodeplug::createEncryption(Config *config, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(config); Q_UNUSED(err);
  EncryptionElement enc(data(Offset::encryption()));
  if (EncryptionElement::PrivacyType::None == enc.privacyType())
    return true;
  return enc.updateCommercialExt(ctx, err);
}

bool
RD5RCodeplug::linkEncryption(Config *config, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(config); Q_UNUSED(ctx); Q_UNUSED(err);
  return true;
}
