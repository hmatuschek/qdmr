#include "openuv380_codeplug.hh"
#include "config.hh"
#include "channel.hh"
#include "utils.hh"
#include "logger.hh"
#include <QDateTime>
#include <QtEndian>
#include "opengd77_extension.hh"


/* ******************************************************************************************** *
 * Implementation of OpenUV380Codeplug
 * ******************************************************************************************** */
OpenUV380Codeplug::OpenUV380Codeplug(QObject *parent)
  : OpenGD77BaseCodeplug(parent)
{
  addImage("OpenGD77 Codeplug EEPROM");
  addImage("OpenGD77 Codeplug FLASH");
  image(FLASH).addElement(0x00000080, 0x00005fe0);
  image(FLASH).addElement(0x00007500, 0x00003b00);
  image(FLASH).addElement(0x00020000, 0x000011a0);
  image(FLASH).addElement(0x0009b000, 0x00013e60);
}

void
OpenUV380Codeplug::clearGeneralSettings() {
  GeneralSettingsElement(data(Offset::settings(), ImageIndex::settings())).clear();
}

bool
OpenUV380Codeplug::encodeGeneralSettings(const Flags &flags, Context &ctx, const ErrorStack &err) {
  GeneralSettingsElement el(data(Offset::settings(), ImageIndex::settings()));
  if (! flags.updateCodePlug)
    el.clear();

  return el.encode(ctx, err);
}

bool
OpenUV380Codeplug::decodeGeneralSettings(Context &ctx, const ErrorStack &err) {
  return GeneralSettingsElement(data(Offset::settings(), ImageIndex::settings())).decode(ctx, err);
}


void
OpenUV380Codeplug::clearDTMFSettings() {
  //DTMFSettingsElement(data(Offset::settings(), ImageIndex::settings())).clear();
}

bool
OpenUV380Codeplug::encodeDTMFSettings(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(ctx); Q_UNUSED(err);
  return true;
}

bool
OpenUV380Codeplug::decodeDTMFSettings(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(err);
  return true;
}


void
OpenUV380Codeplug::clearAPRSSettings() {
  APRSSettingsBankElement(data(Offset::aprsSettings(), ImageIndex::aprsSettings())).clear();
}

bool
OpenUV380Codeplug::encodeAPRSSettings(const Flags &flags, Context &ctx, const ErrorStack &err) {
  APRSSettingsBankElement el(data(Offset::aprsSettings(), ImageIndex::aprsSettings()));
  if (! flags.updateCodePlug)
    el.clear();

  return el.encode(ctx, err);
}

bool
OpenUV380Codeplug::decodeAPRSSettings(Context &ctx, const ErrorStack &err) {
  return APRSSettingsBankElement(data(Offset::aprsSettings(), ImageIndex::aprsSettings()))
      .decode(ctx, err);
}

bool
OpenUV380Codeplug::linkAPRSSettings(Context &ctx, const ErrorStack &err) {
  return APRSSettingsBankElement(data(Offset::aprsSettings(), ImageIndex::aprsSettings()))
      .link(ctx, err);
}


bool
OpenUV380Codeplug::encodeBootSettings(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags);
  return BootSettingsElement(data(Offset::bootSettings(), ImageIndex::bootSettings()))
      .encode(ctx, err);
}

bool
OpenUV380Codeplug::decodeBootSettings(Context &ctx, const ErrorStack &err) {
  return BootSettingsElement(data(Offset::bootSettings(), ImageIndex::bootSettings()))
      .decode(ctx, err);
}


void
OpenUV380Codeplug::clearContacts() {
  ContactBankElement(data(Offset::contacts(), ImageIndex::contacts())).clear();
}

bool
OpenUV380Codeplug::encodeContacts(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags);
  return ContactBankElement(data(Offset::contacts(), ImageIndex::contacts())).encode(ctx, err);
}

bool
OpenUV380Codeplug::createContacts(Context &ctx, const ErrorStack &err) {
  return ContactBankElement(data(Offset::contacts(), ImageIndex::contacts())).decode(ctx, err);
}


void
OpenUV380Codeplug::clearDTMFContacts() {
  DTMFContactBankElement(data(Offset::dtmfContacts(), ImageIndex::dtmfContacts())).clear();
}

bool
OpenUV380Codeplug::encodeDTMFContacts(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags);
  return DTMFContactBankElement(data(Offset::dtmfContacts(), ImageIndex::dtmfContacts()))
      .encode(ctx, err);
}

bool
OpenUV380Codeplug::createDTMFContacts(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)
  return DTMFContactBankElement(data(Offset::dtmfContacts(), ImageIndex::dtmfContacts()))
      .decode(ctx, err);
}


void
OpenUV380Codeplug::clearChannels() {
  for (unsigned int b=0; b<Limit::channelBanks(); b++) {
    if (0 == b)
      ChannelBankElement(data(Offset::channelBank0(), ImageIndex::channelBank0())).clear();
    else
      ChannelBankElement(data(Offset::channelBank1() + (b-1)*ChannelBankElement::size(),
                              ImageIndex::channelBank1())).clear();
  }
}

bool
OpenUV380Codeplug::encodeChannels(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags);

  for (unsigned int b=0,c=0; b<Limit::channelBanks(); b++) {
    ChannelBankElement bank(nullptr);
    if (0 == b)
      bank = ChannelBankElement(data(Offset::channelBank0(), ImageIndex::channelBank0()));
    else
      bank = ChannelBankElement(data(Offset::channelBank1() + (b-1)*ChannelBankElement::size(), ImageIndex::channelBank1()));

    for (unsigned int i=0; i<ChannelBankElement::Limit::channelCount(); i++, c++) {
      if (ctx.has<Channel>(c)) {
        if (! bank.channel(i).encode(ctx.get<Channel>(c), ctx, err)) {
          errMsg(err) << "Cannot encode channel '" << ctx.get<Channel>(c)->name()
                      << "' at index " << i << " of bank " << b << ".";
          return false;
        }
        bank.enable(i, true);
      } else {
        bank.enable(i, false);
      }
    }
  }

  return true;
}

bool
OpenUV380Codeplug::createChannels(Context &ctx, const ErrorStack &err) {
  for (unsigned int b=0,c=0; b<Limit::channelBanks(); b++) {
    ChannelBankElement bank(nullptr);
    if (0 == b)
      bank = ChannelBankElement(data(Offset::channelBank0(), ImageIndex::channelBank0()));
    else
      bank = ChannelBankElement(data(Offset::channelBank1() + (b-1)*ChannelBankElement::size(), ImageIndex::channelBank1()));

    for (unsigned int i=0; i < ChannelBankElement::Limit::channelCount(); i++) {
      if (! bank.isEnabled(i))
        continue;

      Channel *obj = bank.channel(i).decode(ctx, err);
      if (nullptr == obj) {
        errMsg(err) << "Cannot create channel from index " << i << " in bank " << b << ".";
        return false;
      }
      ctx.config()->channelList()->add(obj);
      ctx.add(obj, c++);
    }
  }

  return true;
}

bool
OpenUV380Codeplug::linkChannels(Context &ctx, const ErrorStack &err) {
  for (unsigned int b=0,c=0; b<Limit::channelBanks(); b++) {
    ChannelBankElement bank(nullptr);
    if (0 == b)
      bank = ChannelBankElement(data(Offset::channelBank0(), ImageIndex::channelBank0()));
    else
      bank = ChannelBankElement(data(Offset::channelBank1() + (b-1)*ChannelBankElement::size(), ImageIndex::channelBank1()));

    for (unsigned int i=0; i < ChannelBankElement::Limit::channelCount(); i++) {
      if (! bank.isEnabled(i))
        continue;

      Channel *obj = ctx.get<Channel>(c);
      ChannelElement element = bank.channel(i);

      assert(obj->name() == element.name());

      if (! element.link(obj, ctx, err)) {
        errMsg(err) << "Cannot link channel '" << obj->name()
                    << "' from index " << i << " in bank " << b << ".";
        return false;
      }
      c++;
    }
  }

  return true;
}

void
OpenUV380Codeplug::clearBootSettings() {
  BootSettingsElement(data(Offset::bootSettings(), ImageIndex::bootSettings())).clear();
}


void
OpenUV380Codeplug::clearVFOSettings() {
  VFOChannelElement(data(Offset::vfoA(), ImageIndex::vfoA())).clear();
  VFOChannelElement(data(Offset::vfoB(), ImageIndex::vfoB())).clear();
}

void
OpenUV380Codeplug::clearZones() {
  ZoneBankElement(data(Offset::zoneBank(), ImageIndex::zoneBank())).clear();
}

bool
OpenUV380Codeplug::encodeZones(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags);
  return ZoneBankElement(data(Offset::zoneBank(), ImageIndex::zoneBank())).encode(ctx, err);
}

bool
OpenUV380Codeplug::createZones(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)
  return ZoneBankElement(data(Offset::zoneBank(), ImageIndex::zoneBank())).decode(ctx, err);
}

bool
OpenUV380Codeplug::linkZones(Context &ctx, const ErrorStack &err) {
  return ZoneBankElement(data(Offset::zoneBank(), ImageIndex::zoneBank())).link(ctx, err);
}

void
OpenUV380Codeplug::clearGroupLists() {
  GroupListBankElement(data(Offset::groupLists(), ImageIndex::groupLists())).clear();
}

bool
OpenUV380Codeplug::encodeGroupLists(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags);
  return GroupListBankElement(data(Offset::groupLists(), ImageIndex::groupLists())).encode(ctx, err);
}

bool
OpenUV380Codeplug::createGroupLists(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)
  return GroupListBankElement(data(Offset::groupLists(), ImageIndex::groupLists())).decode(ctx, err);
}

bool
OpenUV380Codeplug::linkGroupLists(Context &ctx, const ErrorStack &err) {
  return GroupListBankElement(data(Offset::groupLists(), ImageIndex::groupLists())).link(ctx, err);
}

