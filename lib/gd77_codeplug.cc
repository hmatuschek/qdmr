#include "gd77_codeplug.hh"
#include "config.hh"
#include "channel.hh"
#include "utils.hh"
#include "logger.hh"
#include <QDateTime>


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
  setUInt2(0x0030, 0, (unsigned)mode);
}

GD77Codeplug::ChannelElement::STEAngle
GD77Codeplug::ChannelElement::steAngle() const {
  return STEAngle(getUInt2(0x0032,6));
}
void
GD77Codeplug::ChannelElement::setSTEAngle(STEAngle angle) {
  setUInt2(0x0032, 6, (unsigned)angle);
}

GD77Codeplug::ChannelElement::PTTId
GD77Codeplug::ChannelElement::pttIDMode() const {
  return PTTId(getUInt2(0x0032, 2));
}
void
GD77Codeplug::ChannelElement::setPTTIDMode(PTTId mode) {
  setUInt2(0x0032, 2, (unsigned)mode);
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
GD77Codeplug::ContactElement::ContactElement(uint8_t *ptr, unsigned size)
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
  uint8_t validFlag = getUInt8(Offset::validFlag());
  return RadioddityCodeplug::ContactElement::isValid() && (0x00 != validFlag);
}
void
GD77Codeplug::ContactElement::markValid(bool valid) {
  setUInt8(Offset::validFlag(), valid ? 0xff : 0x00);
}
bool
GD77Codeplug::ContactElement::fromContactObj(const DMRContact *obj, Context &ctx, const ErrorStack &err) {
  if (! RadioddityCodeplug::ContactElement::fromContactObj(obj, ctx, err))
    return false;
  markValid();
  return true;
}


/* ******************************************************************************************** *
 * Implementation of GD77Codeplug::ScanListElement
 * ******************************************************************************************** */
GD77Codeplug::ScanListElement::ScanListElement(uint8_t *ptr, unsigned size)
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
GD77Codeplug::ScanListBankElement::ScanListBankElement(uint8_t *ptr, unsigned size)
  : RadioddityCodeplug::ScanListBankElement(ptr, size)
{
  // pass...
}

GD77Codeplug::ScanListBankElement::ScanListBankElement(uint8_t *ptr)
  : RadioddityCodeplug::ScanListBankElement(ptr, size())
{
  // pass...
}

void
GD77Codeplug::ScanListBankElement::clear() {
  memset(_data, 0, Limit::scanListCount());
}

uint8_t *
GD77Codeplug::ScanListBankElement::get(unsigned n) const {
  return _data+Offset::scanLists() + n*ScanListElement::size();
}


/* ******************************************************************************************** *
 * Implementation of GD77Codeplug::GroupListElement
 * ******************************************************************************************** */
GD77Codeplug::GroupListElement::GroupListElement(uint8_t *ptr, unsigned size)
  : RadioddityCodeplug::GroupListElement(ptr, size)
{
  // pass...
}

GD77Codeplug::GroupListElement::GroupListElement(uint8_t *ptr)
  : RadioddityCodeplug::GroupListElement(ptr, size())
{
  // pass...
}

bool
GD77Codeplug::GroupListElement::linkRXGroupListObj(unsigned int ncnt, RXGroupList *lst, Context &ctx, const ErrorStack &err) const {
  for (unsigned int i=0; (i<Limit::memberCount()) && (i<ncnt); i++) {
    if (ctx.has<DMRContact>(member(i))) {
      lst->addContact(ctx.get<DMRContact>(member(i)));
    } else {
      errMsg(err) << "Cannot link group list '" << lst->name()
                  << "': Member index " << member(i) << " does not refer to a digital contact.";
      return false;
    }
  }

  return true;
}

bool
GD77Codeplug::GroupListElement::fromRXGroupListObj(const RXGroupList *lst, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  setName(lst->name());

  int j = 0;
  // Iterate over all entries in the codeplug
  for (unsigned int i=0; i<Limit::memberCount(); i++) {
    if (lst->count() > j) {
      // Skip non-private-call entries
      while((lst->count() > j) && (DMRContact::GroupCall != lst->contact(j)->type())) {
        logWarn() << "Contact '" << lst->contact(i)->name() << "' in group list '" << lst->name()
                  << "' is not a group call. Skip entry.";
        j++;
      }
      setMember(i, ctx.index(lst->contact(j))); j++;
    } else {
      // Clear entry.
      clearMember(i);
    }
  }

  return false;
}


/* ******************************************************************************************** *
 * Implementation of GD77Codeplug::GroupListBankElement
 * ******************************************************************************************** */
GD77Codeplug::GroupListBankElement::GroupListBankElement(uint8_t *ptr, unsigned size)
  : RadioddityCodeplug::GroupListBankElement(ptr, size)
{
  // pass...
}

GD77Codeplug::GroupListBankElement::GroupListBankElement(uint8_t *ptr)
  : RadioddityCodeplug::GroupListBankElement(ptr, size())
{
  // pass...
}

uint8_t *
GD77Codeplug::GroupListBankElement::get(unsigned n) const {
  if ((Offset::groupLists() + (n+1)*GroupListElement::size())>_size) {
    logFatal() << "Cannot resolve group list at index " << n << ": Overflow.";
    return nullptr;
  }
  return _data + Offset::groupLists() + n*GroupListElement::size();
}


/* ******************************************************************************************** *
 * Implementation of GD77Codeplug
 * ******************************************************************************************** */
GD77Codeplug::GD77Codeplug(QObject *parent)
  : RadioddityCodeplug(parent)
{
  setRadioId(RadioInfo::GD77);

  addImage("Radioddity GD77 Codeplug");
  image(0).addElement(0x00080, 0x07b80);
  image(0).addElement(0x08000, 0x16b00);
}

void
GD77Codeplug::clearGeneralSettings() {
  GeneralSettingsElement(data(Offset::settings())).clear();
}

bool
GD77Codeplug::encodeGeneralSettings(const Flags &flags, Context &ctx, const ErrorStack &err) {
  GeneralSettingsElement el(data(Offset::settings()));
  if (! flags.updateCodeplug())
    el.clear();
  return el.fromConfig(ctx, err);
}

bool
GD77Codeplug::decodeGeneralSettings(Context &ctx, const ErrorStack &err) {
  return GeneralSettingsElement(data(Offset::settings())).updateConfig(ctx, err);
}

void
GD77Codeplug::clearButtonSettings() {
  ButtonSettingsElement(data(Offset::buttons())).clear();
}
bool
GD77Codeplug::encodeButtonSettings(Context &ctx, const Flags &flags, const ErrorStack &err) {
  Q_UNUSED(flags);
  return ButtonSettingsElement(data(Offset::buttons())).encode(ctx, err);
}
bool
GD77Codeplug::decodeButtonSettings(Context &ctx, const ErrorStack &err) {
  return ButtonSettingsElement(data(Offset::buttons())).decode(ctx, err);
}

void
GD77Codeplug::clearMessages() {
  MessageBankElement(data(Offset::messages())).clear();
}
bool
GD77Codeplug::encodeMessages(Context &ctx, const Flags &flags, const ErrorStack &err) {
  return MessageBankElement(data(Offset::messages())).encode(ctx, flags, err);
}
bool
GD77Codeplug::decodeMessages(Context &ctx, const ErrorStack &err) {
  return MessageBankElement(data(Offset::messages())).decode(ctx, err);
}

void
GD77Codeplug::clearScanLists() {
  ScanListBankElement bank(data(Offset::scanListBank())); bank.clear();
  for (unsigned int i=0; i<ScanListBankElement::Limit::scanListCount(); i++)
    ScanListElement(bank.get(i)).clear();
}

bool
GD77Codeplug::encodeScanLists(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags);

  ScanListBankElement bank(data(Offset::scanListBank()));
  for (unsigned int i=0; i<ScanListBankElement::Limit::scanListCount(); i++) {
    if (i >= ctx.count<ScanList>()) {
      bank.enable(i, false); continue;
    }
    if (! ScanListElement(bank.get(i)).fromScanListObj(ctx.get<ScanList>(i+1), ctx, err))
      return false;
    bank.enable(i, true);
  }

  return true;
}

bool
GD77Codeplug::createScanLists(Context &ctx, const ErrorStack &err) {
  ScanListBankElement bank(data(Offset::scanListBank()));
  for (unsigned int i=0; i<ScanListBankElement::Limit::scanListCount(); i++) {
    if (! bank.isEnabled(i))
      continue;
    ScanListElement el(bank.get(i));
    ScanList *scan = el.toScanListObj(ctx, err);
    if (nullptr == scan) {
      errMsg(err) << "Cannot decode scan list at index " << i << ".";
      return false;
    }
    ctx.config()->scanlists()->add(scan);
    ctx.add(scan, i+1);
  }

  return true;
}

bool
GD77Codeplug::linkScanLists(Context &ctx, const ErrorStack &err) {
  ScanListBankElement bank(data(Offset::scanListBank()));
  for (unsigned int i=0; i<ScanListBankElement::Limit::scanListCount(); i++) {
    if (! bank.isEnabled(i))
      continue;
    if (! ScanListElement(bank.get(i)).linkScanListObj(ctx.get<ScanList>(i+1), ctx, err))
      return false;
  }

  return true;
}

void
GD77Codeplug::clearChannels() {
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
GD77Codeplug::encodeChannels(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)

  for (unsigned int b=0,c=0; b<Limit::channelBankCount(); b++) {
    uint8_t *ptr = nullptr;
    if (0 == b) ptr = data(Offset::channelBank0());
    else ptr = data(Offset::channelBank1() + (b-1)*ChannelBankElement::size());
    ChannelBankElement bank(ptr); bank.clear();
    for (unsigned int i=0; (i<ChannelBankElement::Limit::channelCount())&&(c<Limit::channelCount()); i++, c++) {
      ChannelElement el(bank.get(i));
      if (c < ctx.count<Channel>()) {
        if (! el.fromChannelObj(ctx.get<Channel>(c+1), ctx, err)) {
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
GD77Codeplug::createChannels(Context &ctx, const ErrorStack &err) {
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
        errMsg(err) << "Cannot decode channel at index " << i << ".";
        return false;
      }
      ctx.config()->channelList()->add(ch); ctx.add(ch, c+1);
    }
  }
  return true;
}

bool
GD77Codeplug::linkChannels(Context &ctx, const ErrorStack &err) {
  for (unsigned int b=0,c=0; b<Limit::channelBankCount(); b++) {
    uint8_t *ptr = nullptr;
    if (0 == b) ptr = data(Offset::channelBank0());
    else ptr = data(Offset::channelBank1() + (b-1)*ChannelBankElement::size());
    ChannelBankElement bank(ptr);
    for (unsigned int i=0; (i<ChannelBankElement::Limit::channelCount())&&(c<Limit::channelCount()); i++, c++) {
      if (! bank.isEnabled(i))
        continue;
      if (!ChannelElement(bank.get(i)).linkChannelObj(ctx.get<Channel>(c+1), ctx, err)) {
        errMsg(err) << "Cannot link channel '" << ctx.get<Channel>(c+1)->name()
                    << "' at index " << i << ".";
        return false;
      }
    }
  }
  return true;
}

void
GD77Codeplug::clearBootSettings() {
  BootSettingsElement(data(Offset::bootSettings())).clear();
}

void
GD77Codeplug::clearMenuSettings() {
  MenuSettingsElement(data(Offset::menuSettings())).clear();
}

void
GD77Codeplug::clearBootText() {
  BootTextElement(data(Offset::bootText())).clear();
}

bool
GD77Codeplug::encodeBootText(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags);
  BootTextElement(data(Offset::bootText())).fromConfig(ctx, err);
  return true;
}

bool
GD77Codeplug::decodeBootText(Context &ctx, const ErrorStack &err) {
  BootTextElement(data(Offset::bootText())).updateConfig(ctx, err);
  return true;
}

void
GD77Codeplug::clearVFOSettings() {
  VFOChannelElement(data(Offset::vfoA())).clear();
  VFOChannelElement(data(Offset::vfoB())).clear();
}

void
GD77Codeplug::clearZones() {
  ZoneBankElement bank(data(Offset::zoneBank()));
  bank.clear();
  for (unsigned int i=0; i<ZoneBankElement::Limit::zoneCount(); i++)
    ZoneElement(bank.get(i)).clear();
}

bool
GD77Codeplug::encodeZones(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)

  ZoneBankElement bank(data(Offset::zoneBank()));

  // Pack Zones
  for (unsigned int i=0; i<ZoneBankElement::Limit::zoneCount(); i++) {
    ZoneElement z(bank.get(i));
    if (! ctx.has<Zone>(i+1)) {
      bank.enable(i, false);
      continue;
    }

    // Construct from Zone obj
    Zone *zone = ctx.get<Zone>(i+1);
    z.fromZoneObjA(zone, ctx);
    bank.enable(i, true);
  }
  return true;
}

bool
GD77Codeplug::createZones(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  ZoneBankElement bank(data(Offset::zoneBank()));
  for (unsigned int i=0; i<ZoneBankElement::Limit::zoneCount(); i++) {
    if (! bank.isEnabled(i))
      continue;
    ZoneElement z(bank.get(i));
    Zone *zone = z.toZoneObj(ctx);
    ctx.config()->zones()->add(zone);
    ctx.add(zone, i+1);
  }
  return true;
}

bool
GD77Codeplug::linkZones(Context &ctx, const ErrorStack &err) {
  ZoneBankElement bank(data(Offset::zoneBank()));
  for (unsigned int i=0; i<ZoneBankElement::Limit::zoneCount(); i++) {
    if (! bank.isEnabled(i))
      continue;
    ZoneElement z(bank.get(i));
    Zone *zone = ctx.get<Zone>(i+1);
    if (! z.linkZoneObj(zone, ctx)) {
      errMsg(err) << "Cannot link zone at index " << i << ".";
      return false;
    }
  }

  return true;
}

void
GD77Codeplug::clearContacts() {
  for (unsigned int i=0; i<Limit::contactCount(); i++)
    ContactElement(data(Offset::contacts() + i*ContactElement::size())).clear();
}

bool
GD77Codeplug::encodeContacts(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags);

  for (unsigned int i=0; i<Limit::contactCount(); i++) {
    ContactElement el(data(Offset::contacts() + i*ContactElement::size()));
    el.clear();
    if (i >= ctx.count<DMRContact>())
      continue;
    if (! el.fromContactObj(ctx.get<DMRContact>(i+1), ctx, err)) {
      errMsg(err) << "Cannot encode contact at index " << i << ".";
      return false;
    }
  }

  return true;
}

bool
GD77Codeplug::createContacts(Context &ctx, const ErrorStack &err) {
  for (unsigned int i=0; i<Limit::contactCount(); i++) {
    ContactElement el(data(Offset::contacts() + i*ContactElement::size()));
    if (! el.isValid())
      continue;

    DMRContact *cont = el.toContactObj(ctx, err);
    if (nullptr == cont) {
      errMsg(err) << "Cannot decode contact at index " << i << ".";
      return false;
    }
    ctx.add(cont, i+1);
    ctx.config()->contacts()->add(cont);
  }

  return true;
}

void
GD77Codeplug::clearDTMFContacts() {
  for (unsigned int i=0; i<Limit::dtmfContactCount(); i++)
    DTMFContactElement(data(Offset::dtmfContacts() + i*DTMFContactElement::size())).clear();
}

bool
GD77Codeplug::encodeDTMFContacts(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags);
  for (unsigned int i=0; i<Limit::dtmfContactCount(); i++) {
    DTMFContactElement el(data(Offset::dtmfContacts() + i*DTMFContactElement::size()));
    el.clear();
    if (i >= ctx.count<DTMFContact>())
      continue;
    el.fromContactObj(ctx.get<DTMFContact>(i), ctx, err);
  }
  return true;
}

bool
GD77Codeplug::createDTMFContacts(Context &ctx, const ErrorStack &err) {
  for (unsigned int i=0; i<Limit::dtmfContactCount(); i++) {
    DTMFContactElement el(data(Offset::dtmfContacts()+i*DTMFContactElement::size()));
    // If contact is disabled
    if (! el.isValid())
      continue;
    DTMFContact *cont = el.toContactObj(ctx, err);
    if (nullptr == cont ) {
      errMsg(err) << "Cannot decode DTMF contact at index " << i << ".";
      return false;
    }
    ctx.add(cont, i+1); ctx.config()->contacts()->add(cont);
  }
  return true;
}

void
GD77Codeplug::clearGroupLists() {
  GroupListBankElement bank(data(Offset::groupListBank())); bank.clear();
  for (unsigned int i=0; i<GroupListBankElement::Limit::groupListCount(); i++)
    GroupListElement(bank.get(0)).clear();
}

bool
GD77Codeplug::encodeGroupLists(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)

  GroupListBankElement bank(data(Offset::groupListBank())); bank.clear();
  for (unsigned int i=0; i<GroupListBankElement::Limit::groupListCount(); i++) {
    if (i >= ctx.count<RXGroupList>())
      continue;
    GroupListElement el(bank.get(i));
    el.fromRXGroupListObj(ctx.get<RXGroupList>(i+1), ctx, err);
    bank.setContactCount(i, ctx.get<RXGroupList>(i+1)->count());
  }
  return true;
}

bool
GD77Codeplug::createGroupLists(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  GroupListBankElement bank(data(Offset::groupListBank()));
  for (unsigned int i=0; i<GroupListBankElement::Limit::groupListCount(); i++) {
    if (! bank.isEnabled(i))
      continue;
    GroupListElement el(bank.get(i));
    RXGroupList *list = el.toRXGroupListObj(ctx, err);
    ctx.config()->rxGroupLists()->add(list);
    ctx.add(list, i+1);
  }
  return true;
}

bool
GD77Codeplug::linkGroupLists(Context &ctx, const ErrorStack &err) {
  GroupListBankElement bank(data(Offset::groupListBank()));
  for (unsigned int i=0; i<GroupListBankElement::Limit::groupListCount(); i++) {
    if (! bank.isEnabled(i))
      continue;
    GroupListElement el(bank.get(i));
    if (! el.linkRXGroupListObj(bank.contactCount(i), ctx.get<RXGroupList>(i+1), ctx, err)) {
      errMsg(err) << "Cannot link group list '" << ctx.get<RXGroupList>(i+1)->name() << "'.";
      return false;
    }
  }
  return true;
}


void
GD77Codeplug::clearEncryption() {
  EncryptionElement enc(data(Offset::encryption()));
  enc.clear();
}

bool
GD77Codeplug::encodeEncryption(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags);
  clearEncryption();
  EncryptionElement enc(data(Offset::encryption()));
  return enc.fromCommercialExt(ctx.config()->commercialExtension(), ctx, err);
}

bool
GD77Codeplug::createEncryption(Context &ctx, const ErrorStack &err) {
  EncryptionElement enc(data(Offset::encryption()));
  if (EncryptionElement::PrivacyType::None == enc.privacyType())
    return true;
  return enc.updateCommercialExt(ctx, err);
}

bool
GD77Codeplug::linkEncryption(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(err);
  return true;
}
