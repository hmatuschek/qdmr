#include "opengd77_codeplug.hh"
#include "config.hh"
#include "channel.hh"
#include <QDateTime>
#include <QtEndian>


/* ******************************************************************************************** *
 * Implementation of OpenGD77Codeplug
 * ******************************************************************************************** */
OpenGD77Codeplug::OpenGD77Codeplug(QObject *parent)
  : OpenGD77BaseCodeplug(parent)
{
  // Delete allocated image by GD77 codeplug
  addImage("OpenGD77 Codeplug EEPROM");
  image(EEPROM).addElement(0x00080, 0x05fe0);
  image(EEPROM).addElement(0x07500, 0x03b00);

  addImage("OpenGD77 Codeplug FLASH");
  image(FLASH).addElement(0x00000, 0x011a0);
  image(FLASH).addElement(0x7b000, 0x13e60);
}

void
OpenGD77Codeplug::clearGeneralSettings() {
  GeneralSettingsElement(data(Offset::settings(), ImageIndex::settings())).clear();
}

bool
OpenGD77Codeplug::encodeGeneralSettings(const Flags &flags, Context &ctx, const ErrorStack &err) {
  GeneralSettingsElement el(data(Offset::settings(), ImageIndex::settings()));
  if (! flags.updateCodePlug)
    el.clear();

  return el.encode(ctx, err);
}

bool
OpenGD77Codeplug::decodeGeneralSettings(Context &ctx, const ErrorStack &err) {
  return GeneralSettingsElement(data(Offset::settings(), ImageIndex::settings())).decode(ctx, err);
}


void
OpenGD77Codeplug::clearDTMFSettings() {
  //DTMFSettingsElement(data(Offset::settings(), ImageIndex::settings())).clear();
}

bool
OpenGD77Codeplug::encodeDTMFSettings(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(ctx); Q_UNUSED(err);
  return true;
}

bool
OpenGD77Codeplug::decodeDTMFSettings(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(err);
  return true;
}


void
OpenGD77Codeplug::clearAPRSSettings() {
  APRSSettingsBankElement(data(Offset::aprsSettings(), ImageIndex::aprsSettings())).clear();
}

bool
OpenGD77Codeplug::encodeAPRSSettings(const Flags &flags, Context &ctx, const ErrorStack &err) {
  APRSSettingsBankElement el(data(Offset::aprsSettings(), ImageIndex::aprsSettings()));
  if (! flags.updateCodePlug)
    el.clear();

  return el.encode(ctx, err);
}

bool
OpenGD77Codeplug::decodeAPRSSettings(Context &ctx, const ErrorStack &err) {
  return APRSSettingsBankElement(data(Offset::aprsSettings(), ImageIndex::aprsSettings()))
      .decode(ctx, err);
}

bool
OpenGD77Codeplug::linkAPRSSettings(Context &ctx, const ErrorStack &err) {
  return APRSSettingsBankElement(data(Offset::aprsSettings(), ImageIndex::aprsSettings()))
      .link(ctx, err);
}


bool
OpenGD77Codeplug::encodeBootSettings(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags);
  return BootSettingsElement(data(Offset::bootSettings(), ImageIndex::bootSettings()))
      .encode(ctx, err);
}

bool
OpenGD77Codeplug::decodeBootSettings(Context &ctx, const ErrorStack &err) {
  return BootSettingsElement(data(Offset::bootSettings(), ImageIndex::bootSettings()))
      .decode(ctx, err);
}


void
OpenGD77Codeplug::clearContacts() {
  ContactBankElement(data(Offset::contacts(), ImageIndex::contacts())).clear();
}

bool
OpenGD77Codeplug::encodeContacts(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags);
  return ContactBankElement(data(Offset::contacts(), ImageIndex::contacts())).encode(ctx, err);
}

bool
OpenGD77Codeplug::createContacts(Context &ctx, const ErrorStack &err) {
  return ContactBankElement(data(Offset::contacts(), ImageIndex::contacts())).decode(ctx, err);
}


void
OpenGD77Codeplug::clearDTMFContacts() {
  DTMFContactBankElement(data(Offset::dtmfContacts(), ImageIndex::dtmfContacts())).clear();
}

bool
OpenGD77Codeplug::encodeDTMFContacts(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags);
  return DTMFContactBankElement(data(Offset::dtmfContacts(), ImageIndex::dtmfContacts()))
      .encode(ctx, err);
}

bool
OpenGD77Codeplug::createDTMFContacts(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)
  return DTMFContactBankElement(data(Offset::dtmfContacts(), ImageIndex::dtmfContacts()))
      .decode(ctx, err);
}


void
OpenGD77Codeplug::clearChannels() {
  for (unsigned int b=0; b<Limit::channelBanks(); b++) {
    if (0 == b)
      ChannelBankElement(data(Offset::channelBank0(), ImageIndex::channelBank0())).clear();
    else
      ChannelBankElement(data(Offset::channelBank1() + (b-1)*ChannelBankElement::size(),
                              ImageIndex::channelBank1())).clear();
  }
}

bool
OpenGD77Codeplug::encodeChannels(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags);

  for (unsigned int b=0, c=0; b<Limit::channelBanks(); b++) {
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
OpenGD77Codeplug::createChannels(Context &ctx, const ErrorStack &err) {
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
OpenGD77Codeplug::linkChannels(Context &ctx, const ErrorStack &err) {
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
      if (! bank.channel(i).link(obj, ctx, err)) {
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
OpenGD77Codeplug::clearBootSettings() {
  BootSettingsElement(data(Offset::bootSettings(), ImageIndex::bootSettings())).clear();
}


void
OpenGD77Codeplug::clearVFOSettings() {
  VFOChannelElement(data(Offset::vfoA(), ImageIndex::vfoA())).clear();
  VFOChannelElement(data(Offset::vfoB(), ImageIndex::vfoB())).clear();
}

void
OpenGD77Codeplug::clearZones() {
  ZoneBankElement(data(Offset::zoneBank(), ImageIndex::zoneBank())).clear();
}

bool
OpenGD77Codeplug::encodeZones(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags);
  return ZoneBankElement(data(Offset::zoneBank(), ImageIndex::zoneBank())).encode(ctx, err);
}

bool
OpenGD77Codeplug::createZones(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)
  return ZoneBankElement(data(Offset::zoneBank(), ImageIndex::zoneBank())).decode(ctx, err);
}

bool
OpenGD77Codeplug::linkZones(Context &ctx, const ErrorStack &err) {
  return ZoneBankElement(data(Offset::zoneBank(), ImageIndex::zoneBank())).link(ctx, err);
}

void
OpenGD77Codeplug::clearGroupLists() {
  GroupListBankElement(data(Offset::groupLists(), ImageIndex::groupLists())).clear();
}

bool
OpenGD77Codeplug::encodeGroupLists(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags);
  return GroupListBankElement(data(Offset::groupLists(), ImageIndex::groupLists())).encode(ctx, err);
}

bool
OpenGD77Codeplug::createGroupLists(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)
  return GroupListBankElement(data(Offset::groupLists(), ImageIndex::groupLists())).decode(ctx, err);
}

bool
OpenGD77Codeplug::linkGroupLists(Context &ctx, const ErrorStack &err) {
  return GroupListBankElement(data(Offset::groupLists(), ImageIndex::groupLists())).link(ctx, err);
}

