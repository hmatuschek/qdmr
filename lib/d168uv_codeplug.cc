#include "d168uv_codeplug.hh"
#include "config.hh"

#include <QTimeZone>
#include <QtEndian>

/* ******************************************************************************************** *
 * Implementation of D168UVCodeplug::ChannelElement
 * ******************************************************************************************** */
D168UVCodeplug::ChannelElement::ChannelElement(uint8_t *ptr)
  : D878UVCodeplug::ChannelElement(ptr)
{
  // pass...
}

bool
D168UVCodeplug::ChannelElement::dmrCRCDisabled() const {
  return getBit(Offset::disableDMRCRC());
}

void
D168UVCodeplug::ChannelElement::disableDMRCRC(bool disable) {
  setBit(Offset::disableDMRCRC(), disable);
}


Channel *
D168UVCodeplug::ChannelElement::toChannelObj(Context &ctx) const {
  auto ch = D878UVCodeplug::ChannelElement::toChannelObj(ctx);
  if (nullptr == ch)
    return ch;

  if (ch->is<DMRChannel>()) {
    auto ext = ch->as<DMRChannel>()->anytoneChannelExtension();
    if (nullptr == ext)
      ch->as<DMRChannel>()->setAnytoneChannelExtension(
        ext = new AnytoneDMRChannelExtension());
    ext->enableCRC(! dmrCRCDisabled());
  }

  return ch;
}

bool D168UVCodeplug::ChannelElement::fromChannelObj(const Channel *c, Context &ctx) {
  if (! D878UVCodeplug::ChannelElement::fromChannelObj(c, ctx))
    return false;

  if (c->is<DMRChannel>() && c->as<DMRChannel>()->anytoneChannelExtension()) {
    auto ext = c->as<DMRChannel>()->anytoneChannelExtension();
    disableDMRCRC(! ext->crcEnabled());
  }

  return true;
}



/* ******************************************************************************************** *
 * Implementation of D168UVCodeplug::GeneralSettingsElement::TimeZone
 * ******************************************************************************************** */
QVector<QTimeZone>
D168UVCodeplug::GeneralSettingsElement::TimeZone::_timeZones = {
  QTimeZone(-43200), QTimeZone(-39600), QTimeZone(-36000), QTimeZone(-32400), QTimeZone(-28800),
  QTimeZone(-25200), QTimeZone(-21600), QTimeZone(-18000), QTimeZone(-14400), QTimeZone(-12600),
  QTimeZone(-10800), QTimeZone(- 7200), QTimeZone(- 3600), QTimeZone(     0), QTimeZone(  3600),
  QTimeZone(  7200), QTimeZone( 10800), QTimeZone( 12600), QTimeZone( 14400), QTimeZone( 16200),
  QTimeZone( 18000), QTimeZone( 19800), QTimeZone( 20700), QTimeZone( 21600), QTimeZone( 25200),
  QTimeZone( 28600), QTimeZone( 30600), QTimeZone( 32400), QTimeZone( 36000), QTimeZone( 39600),
  QTimeZone( 43200), QTimeZone( 46800) };

QTimeZone
D168UVCodeplug::GeneralSettingsElement::TimeZone::decode(uint8_t code) {
  if (code >= _timeZones.size())
    return _timeZones.back();
  return _timeZones.at(code);
}
uint8_t
D168UVCodeplug::GeneralSettingsElement::TimeZone::encode(const QTimeZone &zone) {
  if (! _timeZones.contains(zone))
    return 13; //<- UTC
  return _timeZones.indexOf(zone);
}


/* ******************************************************************************************** *
 * Implementation of D168UVCodeplug::GeneralSettingsElement
 * ******************************************************************************************** */
D168UVCodeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr)
  : D878UVCodeplug::GeneralSettingsElement(ptr, GeneralSettingsElement::size())
{
  // pass...
}


QTimeZone
D168UVCodeplug::GeneralSettingsElement::gpsTimeZone() const {
  return TimeZone::decode(getUInt8(Offset::gpsTimeZone()));
}

void
D168UVCodeplug::GeneralSettingsElement::setGPSTimeZone(const QTimeZone &zone) {
  setUInt8(Offset::gpsTimeZone(), TimeZone::encode(zone)); // <- Set to UTC
}



/* ******************************************************************************************** *
 * Implementation of D168UVCodeplug
 * ******************************************************************************************** */
D168UVCodeplug::D168UVCodeplug(const QString &label, QObject *parent)
  : D878UVCodeplug(label, parent)
{
  // pass...
}

D168UVCodeplug::D168UVCodeplug(QObject *parent)
  : D878UVCodeplug("AnyTone AT-D168UV Codeplug", parent)
{
  // pass...
}

bool
D168UVCodeplug::encodeChannels(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)
  // Encode channels
  for (unsigned int i=0; i<ctx.count<Channel>(); i++) {
    // enable channel
    uint16_t bank = i/Limit::channelsPerBank(), idx = i%Limit::channelsPerBank();
    uint32_t addr = Offset::channelBanks() + bank*Offset::betweenChannelBanks()
        + idx*ChannelElement::size();

    ChannelElement ch(data(addr));
    ch.fromChannelObj(ctx.get<Channel>(i), ctx);
    ChannelExtensionElement ext(data(addr + Offset::toChannelExtension()));
    ext.fromChannelObj(ctx.get<Channel>(i), ctx);
  }
  return true;
}

bool
D168UVCodeplug::createChannels(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  // Create channels
  ChannelBitmapElement channel_bitmap(data(Offset::channelBitmap()));
  for (uint16_t i=0; i<Limit::numChannels(); i++) {
    // Check if channel is enabled:
    uint16_t bank = i/Limit::channelsPerBank(), idx = i%Limit::channelsPerBank();
    uint32_t addr = Offset::channelBanks() + bank*Offset::betweenChannelBanks()
        + idx*ChannelElement::size();

    if (! channel_bitmap.isEncoded(i))
      continue;

    ChannelElement ch(data(addr));
    ChannelExtensionElement ext(data(addr + Offset::toChannelExtension()));

    if (Channel *obj = ch.toChannelObj(ctx)) {
      ctx.config()->channelList()->add(obj); ctx.add(obj, i);
      ext.updateChannelObj(obj, ctx);
    }
  }
  return true;
}

bool
D168UVCodeplug::linkChannels(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)
  ChannelBitmapElement channel_bitmap(data(Offset::channelBitmap()));

  // Link channel objects
  for (uint16_t i=0; i<Limit::numChannels(); i++) {
    // Check if channel is enabled:
    uint16_t bank = i/Limit::channelsPerBank(), idx = i%Limit::channelsPerBank();
    uint32_t addr = Offset::channelBanks() + bank*Offset::betweenChannelBanks()
        + idx*ChannelElement::size();
    if (! channel_bitmap.isEncoded(i))
      continue;
    ChannelElement ch(data(addr));
    ChannelExtensionElement ext(data(addr + Offset::toChannelExtension()));

    if (ctx.has<Channel>(i)) {
      ch.linkChannelObj(ctx.get<Channel>(i), ctx);
      ext.linkChannelObj(ctx.get<Channel>(i), ctx);
    }
  }
  return true;
}


bool
D168UVCodeplug::encodeGeneralSettings(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  GeneralSettingsElement(data(Offset::settings())).fromConfig(flags, ctx);
  DMRAPRSMessageElement(data(Offset::dmrAPRSMessage())).fromConfig(flags, ctx);
  ExtendedSettingsElement(data(Offset::settingsExtension())).fromConfig(flags, ctx);
  return true;
}
bool
D168UVCodeplug::decodeGeneralSettings(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  GeneralSettingsElement(data(Offset::settings())).updateConfig(ctx);
  DMRAPRSMessageElement(data(Offset::dmrAPRSMessage())).updateConfig(ctx);
  ExtendedSettingsElement(data(Offset::settingsExtension())).updateConfig(ctx);
  return true;
}
bool
D168UVCodeplug::linkGeneralSettings(Context &ctx, const ErrorStack &err) {
  if (! GeneralSettingsElement(data(Offset::settings())).linkSettings(ctx.config()->settings(), ctx, err)) {
    errMsg(err) << "Cannot link general settings extension.";
    return false;
  }

  if (! ExtendedSettingsElement(data(Offset::settingsExtension())).linkConfig(ctx, err)) {
    errMsg(err) << "Cannot link general settings extension.";
    return false;
  }

  return true;
}
