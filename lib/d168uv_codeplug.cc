#include "d168uv_codeplug.hh"
#include "config.hh"
#include "logger.hh"
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
 * Implementation of D168UVCodeplug::GeneralSettingsElement::KeyFunction
 * ******************************************************************************************** */
uint8_t
D168UVCodeplug::GeneralSettingsElement::KeyFunction::encode(AnytoneKeySettingsExtension::KeyFunction func) {
  switch (func) {
  case AnytoneKeySettingsExtension::KeyFunction::Off:               return (uint8_t)KeyFunction::Off;
  case AnytoneKeySettingsExtension::KeyFunction::Voltage:           return (uint8_t)KeyFunction::Voltage;
  case AnytoneKeySettingsExtension::KeyFunction::Power:             return (uint8_t)KeyFunction::Power;
  case AnytoneKeySettingsExtension::KeyFunction::Repeater:          return (uint8_t)KeyFunction::Repeater;
  case AnytoneKeySettingsExtension::KeyFunction::Reverse:           return (uint8_t)KeyFunction::Reverse;
  case AnytoneKeySettingsExtension::KeyFunction::Encryption:        return (uint8_t)KeyFunction::Encryption;
  case AnytoneKeySettingsExtension::KeyFunction::Call:              return (uint8_t)KeyFunction::Call;
  case AnytoneKeySettingsExtension::KeyFunction::VOX:               return (uint8_t)KeyFunction::VOX;
  case AnytoneKeySettingsExtension::KeyFunction::ToggleVFO:         return (uint8_t)KeyFunction::ToggleVFO;
  case AnytoneKeySettingsExtension::KeyFunction::SubPTT:            return (uint8_t)KeyFunction::SubPTT;
  case AnytoneKeySettingsExtension::KeyFunction::Scan:              return (uint8_t)KeyFunction::Scan;
  case AnytoneKeySettingsExtension::KeyFunction::WFM:               return (uint8_t)KeyFunction::WFM;
  case AnytoneKeySettingsExtension::KeyFunction::Alarm:             return (uint8_t)KeyFunction::Alarm;
  case AnytoneKeySettingsExtension::KeyFunction::RecordSwitch:      return (uint8_t)KeyFunction::RecordSwitch;
  case AnytoneKeySettingsExtension::KeyFunction::Record:            return (uint8_t)KeyFunction::Record;
  case AnytoneKeySettingsExtension::KeyFunction::SMS:               return (uint8_t)KeyFunction::SMS;
  case AnytoneKeySettingsExtension::KeyFunction::Dial:              return (uint8_t)KeyFunction::Dial;
  case AnytoneKeySettingsExtension::KeyFunction::GPSInformation:    return (uint8_t)KeyFunction::GPSInformation;
  case AnytoneKeySettingsExtension::KeyFunction::Monitor:           return (uint8_t)KeyFunction::Monitor;
  case AnytoneKeySettingsExtension::KeyFunction::ToggleMainChannel: return (uint8_t)KeyFunction::ToggleMainChannel;
  case AnytoneKeySettingsExtension::KeyFunction::HotKey1:           return (uint8_t)KeyFunction::HotKey1;
  case AnytoneKeySettingsExtension::KeyFunction::HotKey2:           return (uint8_t)KeyFunction::HotKey2;
  case AnytoneKeySettingsExtension::KeyFunction::HotKey3:           return (uint8_t)KeyFunction::HotKey3;
  case AnytoneKeySettingsExtension::KeyFunction::HotKey4:           return (uint8_t)KeyFunction::HotKey4;
  case AnytoneKeySettingsExtension::KeyFunction::HotKey5:           return (uint8_t)KeyFunction::HotKey5;
  case AnytoneKeySettingsExtension::KeyFunction::HotKey6:           return (uint8_t)KeyFunction::HotKey6;
  case AnytoneKeySettingsExtension::KeyFunction::WorkAlone:         return (uint8_t)KeyFunction::WorkAlone;
  case AnytoneKeySettingsExtension::KeyFunction::SkipChannel:       return (uint8_t)KeyFunction::SkipChannel;
  case AnytoneKeySettingsExtension::KeyFunction::DMRMonitor:        return (uint8_t)KeyFunction::DMRMonitor;
  case AnytoneKeySettingsExtension::KeyFunction::SubChannel:        return (uint8_t)KeyFunction::SubChannel;
  case AnytoneKeySettingsExtension::KeyFunction::PriorityZone:      return (uint8_t)KeyFunction::PriorityZone;
  case AnytoneKeySettingsExtension::KeyFunction::VFOScan:           return (uint8_t)KeyFunction::VFOScan;
  case AnytoneKeySettingsExtension::KeyFunction::MICSoundQuality:   return (uint8_t)KeyFunction::MICSoundQuality;
  case AnytoneKeySettingsExtension::KeyFunction::LastCallReply:     return (uint8_t)KeyFunction::LastCallReply;
  case AnytoneKeySettingsExtension::KeyFunction::ChannelType:       return (uint8_t)KeyFunction::ChannelType;
  case AnytoneKeySettingsExtension::KeyFunction::Ranging:           return (uint8_t)KeyFunction::Ranging;
  case AnytoneKeySettingsExtension::KeyFunction::Roaming:           return (uint8_t)KeyFunction::Roaming;
  case AnytoneKeySettingsExtension::KeyFunction::ChannelRanging:    return (uint8_t)KeyFunction::ChannelRanging;
  case AnytoneKeySettingsExtension::KeyFunction::MaxVolume:         return (uint8_t)KeyFunction::MaxVolume;
  case AnytoneKeySettingsExtension::KeyFunction::Slot:              return (uint8_t)KeyFunction::Slot;
  case AnytoneKeySettingsExtension::KeyFunction::APRSTypeSwitch:    return (uint8_t)KeyFunction::APRSType;
  case AnytoneKeySettingsExtension::KeyFunction::Zone:              return (uint8_t)KeyFunction::Zone;
  case AnytoneKeySettingsExtension::KeyFunction::RoamingSet:        return (uint8_t)KeyFunction::RoamingSet;
  case AnytoneKeySettingsExtension::KeyFunction::APRSSet:           return (uint8_t)KeyFunction::APRSSet;
  case AnytoneKeySettingsExtension::KeyFunction::Mute:              return (uint8_t)KeyFunction::Mute;
  case AnytoneKeySettingsExtension::KeyFunction::CtcssDcsSet:       return (uint8_t)KeyFunction::CtcssDcsSet;
  case AnytoneKeySettingsExtension::KeyFunction::TBSTSend:          return (uint8_t)KeyFunction::TBSTSend;
  case AnytoneKeySettingsExtension::KeyFunction::Bluetooth:         return (uint8_t)KeyFunction::Bluetooth;
  case AnytoneKeySettingsExtension::KeyFunction::GPS:               return (uint8_t)KeyFunction::GPS;
  case AnytoneKeySettingsExtension::KeyFunction::ChannelName:       return (uint8_t)KeyFunction::ChannelName;
  case AnytoneKeySettingsExtension::KeyFunction::CDTScan:           return (uint8_t)KeyFunction::CDTScan;
  case AnytoneKeySettingsExtension::KeyFunction::APRSSend:          return (uint8_t)KeyFunction::APRSSend;
  case AnytoneKeySettingsExtension::KeyFunction::DIMShut:           return (uint8_t)KeyFunction::DIMShut;
  case AnytoneKeySettingsExtension::KeyFunction::GPSRoaming:        return (uint8_t)KeyFunction::GPSRoaming;
  case AnytoneKeySettingsExtension::KeyFunction::WXAlarm:           return (uint8_t)KeyFunction::WXAlarm;
  case AnytoneKeySettingsExtension::KeyFunction::Squelch:           return (uint8_t)KeyFunction::Squelch;
  case AnytoneKeySettingsExtension::KeyFunction::ChannelSettings:   return (uint8_t)KeyFunction::ChannelSettings;
  case AnytoneKeySettingsExtension::KeyFunction::SatPredict:        return (uint8_t)KeyFunction::SatPredict;
  default:                                                          return (uint8_t)KeyFunction::Off;
  }
}

AnytoneKeySettingsExtension::KeyFunction
D168UVCodeplug::GeneralSettingsElement::KeyFunction::decode(uint8_t code) {
  switch ((KeyFunctionCode)code) {
  case KeyFunction::Off:               return AnytoneKeySettingsExtension::KeyFunction::Off;
  case KeyFunction::Voltage:           return AnytoneKeySettingsExtension::KeyFunction::Voltage;
  case KeyFunction::Power:             return AnytoneKeySettingsExtension::KeyFunction::Power;
  case KeyFunction::Repeater:          return AnytoneKeySettingsExtension::KeyFunction::Repeater;
  case KeyFunction::Reverse:           return AnytoneKeySettingsExtension::KeyFunction::Reverse;
  case KeyFunction::Encryption:        return AnytoneKeySettingsExtension::KeyFunction::Encryption;
  case KeyFunction::Call:              return AnytoneKeySettingsExtension::KeyFunction::Call;
  case KeyFunction::VOX:               return AnytoneKeySettingsExtension::KeyFunction::VOX;
  case KeyFunction::ToggleVFO:         return AnytoneKeySettingsExtension::KeyFunction::ToggleVFO;
  case KeyFunction::SubPTT:            return AnytoneKeySettingsExtension::KeyFunction::SubPTT;
  case KeyFunction::Scan:              return AnytoneKeySettingsExtension::KeyFunction::Scan;
  case KeyFunction::WFM:               return AnytoneKeySettingsExtension::KeyFunction::WFM;
  case KeyFunction::Alarm:             return AnytoneKeySettingsExtension::KeyFunction::Alarm;
  case KeyFunction::RecordSwitch:      return AnytoneKeySettingsExtension::KeyFunction::RecordSwitch;
  case KeyFunction::Record:            return AnytoneKeySettingsExtension::KeyFunction::Record;
  case KeyFunction::SMS:               return AnytoneKeySettingsExtension::KeyFunction::SMS;
  case KeyFunction::Dial:              return AnytoneKeySettingsExtension::KeyFunction::Dial;
  case KeyFunction::GPSInformation:    return AnytoneKeySettingsExtension::KeyFunction::GPSInformation;
  case KeyFunction::Monitor:           return AnytoneKeySettingsExtension::KeyFunction::Monitor;
  case KeyFunction::ToggleMainChannel: return AnytoneKeySettingsExtension::KeyFunction::ToggleMainChannel;
  case KeyFunction::HotKey1:           return AnytoneKeySettingsExtension::KeyFunction::HotKey1;
  case KeyFunction::HotKey2:           return AnytoneKeySettingsExtension::KeyFunction::HotKey2;
  case KeyFunction::HotKey3:           return AnytoneKeySettingsExtension::KeyFunction::HotKey3;
  case KeyFunction::HotKey4:           return AnytoneKeySettingsExtension::KeyFunction::HotKey4;
  case KeyFunction::HotKey5:           return AnytoneKeySettingsExtension::KeyFunction::HotKey5;
  case KeyFunction::HotKey6:           return AnytoneKeySettingsExtension::KeyFunction::HotKey6;
  case KeyFunction::WorkAlone:         return AnytoneKeySettingsExtension::KeyFunction::WorkAlone;
  case KeyFunction::SkipChannel:       return AnytoneKeySettingsExtension::KeyFunction::SkipChannel;
  case KeyFunction::DMRMonitor:        return AnytoneKeySettingsExtension::KeyFunction::DMRMonitor;
  case KeyFunction::SubChannel:        return AnytoneKeySettingsExtension::KeyFunction::SubChannel;
  case KeyFunction::PriorityZone:      return AnytoneKeySettingsExtension::KeyFunction::PriorityZone;
  case KeyFunction::VFOScan:           return AnytoneKeySettingsExtension::KeyFunction::VFOScan;
  case KeyFunction::MICSoundQuality:   return AnytoneKeySettingsExtension::KeyFunction::MICSoundQuality;
  case KeyFunction::LastCallReply:     return AnytoneKeySettingsExtension::KeyFunction::LastCallReply;
  case KeyFunction::ChannelType:       return AnytoneKeySettingsExtension::KeyFunction::ChannelType;
  case KeyFunction::Ranging:           return AnytoneKeySettingsExtension::KeyFunction::Ranging;
  case KeyFunction::Roaming:           return AnytoneKeySettingsExtension::KeyFunction::Roaming;
  case KeyFunction::ChannelRanging:    return AnytoneKeySettingsExtension::KeyFunction::ChannelRanging;
  case KeyFunction::MaxVolume:         return AnytoneKeySettingsExtension::KeyFunction::MaxVolume;
  case KeyFunction::Slot:              return AnytoneKeySettingsExtension::KeyFunction::Slot;
  case KeyFunction::APRSType:          return AnytoneKeySettingsExtension::KeyFunction::APRSTypeSwitch;
  case KeyFunction::Zone:              return AnytoneKeySettingsExtension::KeyFunction::Zone;
  case KeyFunction::RoamingSet:        return AnytoneKeySettingsExtension::KeyFunction::RoamingSet;
  case KeyFunction::APRSSet:           return AnytoneKeySettingsExtension::KeyFunction::APRSSet;
  case KeyFunction::Mute:              return AnytoneKeySettingsExtension::KeyFunction::Mute;
  case KeyFunction::CtcssDcsSet:       return AnytoneKeySettingsExtension::KeyFunction::CtcssDcsSet;
  case KeyFunction::TBSTSend:          return AnytoneKeySettingsExtension::KeyFunction::TBSTSend;
  case KeyFunction::Bluetooth:         return AnytoneKeySettingsExtension::KeyFunction::Bluetooth;
  case KeyFunction::GPS:               return AnytoneKeySettingsExtension::KeyFunction::GPS;
  case KeyFunction::ChannelName:       return AnytoneKeySettingsExtension::KeyFunction::ChannelName;
  case KeyFunction::CDTScan:           return AnytoneKeySettingsExtension::KeyFunction::CDTScan;
  case KeyFunction::APRSSend:          return AnytoneKeySettingsExtension::KeyFunction::APRSSend;
  case KeyFunction::DIMShut:           return AnytoneKeySettingsExtension::KeyFunction::DIMShut;
  case KeyFunction::GPSRoaming:        return AnytoneKeySettingsExtension::KeyFunction::GPSRoaming;
  case KeyFunction::WXAlarm:           return AnytoneKeySettingsExtension::KeyFunction::WXAlarm;
  case KeyFunction::Squelch:           return AnytoneKeySettingsExtension::KeyFunction::Squelch;
  case KeyFunction::ChannelSettings:   return AnytoneKeySettingsExtension::KeyFunction::ChannelSettings;
  case KeyFunction::SatPredict:        return AnytoneKeySettingsExtension::KeyFunction::SatPredict;
  default: {
    logWarn() << "Unmapped Key Code: 0x" << QString::number(code, 16);
    return AnytoneKeySettingsExtension::KeyFunction::Off;
  }
  }
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

AnytoneKeySettingsExtension::KeyFunction
D168UVCodeplug::GeneralSettingsElement::funcKeyCShort() const {
  return AnytoneKeySettingsExtension::KeyFunction::Off;
}

void
D168UVCodeplug::GeneralSettingsElement::setFuncKeyCShort(AnytoneKeySettingsExtension::KeyFunction func) {
  Q_UNUSED(func);
}

AnytoneKeySettingsExtension::KeyFunction
D168UVCodeplug::GeneralSettingsElement::funcKeyCLong() const {
  return AnytoneKeySettingsExtension::KeyFunction::Off;
}

void
D168UVCodeplug::GeneralSettingsElement::setFuncKeyCLong(AnytoneKeySettingsExtension::KeyFunction func) {
  Q_UNUSED(func);
}

AnytoneKeySettingsExtension::KeyFunction
D168UVCodeplug::GeneralSettingsElement::funcKey1Short() const {
  return AnytoneKeySettingsExtension::KeyFunction::Off;
}

void
D168UVCodeplug::GeneralSettingsElement::setFuncKey1Short(AnytoneKeySettingsExtension::KeyFunction func) {
  Q_UNUSED(func);
}

AnytoneKeySettingsExtension::KeyFunction
D168UVCodeplug::GeneralSettingsElement::funcKey2Short() const {
  return AnytoneKeySettingsExtension::KeyFunction::Off;
}

void
D168UVCodeplug::GeneralSettingsElement::setFuncKey2Short(AnytoneKeySettingsExtension::KeyFunction func) {
  Q_UNUSED(func);
}

AnytoneKeySettingsExtension::KeyFunction
D168UVCodeplug::GeneralSettingsElement::funcKey1Long() const {
  return AnytoneKeySettingsExtension::KeyFunction::Off;
}

void
D168UVCodeplug::GeneralSettingsElement::setFuncKey1Long(AnytoneKeySettingsExtension::KeyFunction func) {
  Q_UNUSED(func);
}

AnytoneKeySettingsExtension::KeyFunction
D168UVCodeplug::GeneralSettingsElement::funcKey2Long() const {
  return AnytoneKeySettingsExtension::KeyFunction::Off;
}

void
D168UVCodeplug::GeneralSettingsElement::setFuncKey2Long(
  AnytoneKeySettingsExtension::KeyFunction func) {
  Q_UNUSED(func);
}

AnytoneKeySettingsExtension::KeyFunction
D168UVCodeplug::GeneralSettingsElement::funcKeyALong() const {
  return static_cast<AnytoneKeySettingsExtension::KeyFunction>(
    getUInt8(Offset::progFuncKeyALong()));
}

void
D168UVCodeplug::GeneralSettingsElement::setFuncKeyALong(
  AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKeyALong(), static_cast<unsigned int>(func));
}

AnytoneKeySettingsExtension::KeyFunction
D168UVCodeplug::GeneralSettingsElement::funcKeyBLong() const {
  return static_cast<AnytoneKeySettingsExtension::KeyFunction>(
    getUInt8(Offset::progFuncKeyBLong()));
}

void
D168UVCodeplug::GeneralSettingsElement::setFuncKeyBLong(
  AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKeyBLong(), static_cast<unsigned int>(func));
}

AnytoneKeySettingsExtension::KeyFunction
D168UVCodeplug::GeneralSettingsElement::funcKeyAVeryLong() const {
  return static_cast<AnytoneKeySettingsExtension::KeyFunction>(
    getUInt8(Offset::progFuncKeyAVeryLong()));
}

void
D168UVCodeplug::GeneralSettingsElement::setFuncKeyAVeryLong(
  AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKeyAVeryLong(), static_cast<unsigned int>(func));
}

AnytoneKeySettingsExtension::KeyFunction
D168UVCodeplug::GeneralSettingsElement::funcKeyBVeryLong() const {
  return static_cast<AnytoneKeySettingsExtension::KeyFunction>(
    getUInt8(Offset::progFuncKeyBVeryLong()));
}

void
D168UVCodeplug::GeneralSettingsElement::setFuncKeyBVeryLong(
  AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKeyBLong(), static_cast<unsigned int>(func));
}

bool
D168UVCodeplug::GeneralSettingsElement::fromConfig(const Flags &flags,
                                                   Context &ctx,
                                                   const ErrorStack &err)
{
  if (! D878UVCodeplug::GeneralSettingsElement::fromConfig(flags, ctx, err)) {
    errMsg(err) << "Cannot encode general settings.";
    return false;
  }

  if (nullptr == ctx.config()->settings()->anytoneExtension())
    return true;

  auto ext = ctx.config()->settings()->anytoneExtension();
  setFuncKeyAVeryLong(ext->keySettings()->funcKeyAVeryLong());
  setFuncKeyBVeryLong(ext->keySettings()->funcKeyBVeryLong());

  return true;
}


bool
D168UVCodeplug::GeneralSettingsElement::updateConfig(Context &ctx, const ErrorStack &err) {
  if (! D878UVCodeplug::GeneralSettingsElement::updateConfig(ctx, err)) {
    errMsg(err) << "Cannot update general settings.";
    return false;
  }

  if (! ctx.config()->settings()->anytoneExtension())
    ctx.config()->settings()->setAnytoneExtension(new AnytoneSettingsExtension());
  auto ext = ctx.config()->settings()->anytoneExtension();

  ext->keySettings()->setFuncKeyAVeryLong(funcKeyAVeryLong());
  ext->keySettings()->setFuncKeyBVeryLong(funcKeyBVeryLong());

  return true;
}


/* ******************************************************************************************** *
 * Implementation of D168UVCodeplug::ExtendedSettingsElement
 * ******************************************************************************************** */
D168UVCodeplug::ExtendedSettingsElement::ExtendedSettingsElement(uint8_t *ptr)
  : D878UVCodeplug::ExtendedSettingsElement(ptr, size())
{
  // pass...
}

bool
D168UVCodeplug::ExtendedSettingsElement::fromConfig(const Flags &flags, Context &ctx, const ErrorStack &err) {
  if (! D878UVCodeplug::ExtendedSettingsElement::fromConfig(flags, ctx, err))
    return false;

  setUInt8(Offset::talkerAliasType(), (uint8_t)TalkerAliasType::RadioName);

  return true;
}



/* ******************************************************************************************** *
 * Implementation of D168UVCodeplug::FixedLocationNamesElement
 * ******************************************************************************************** */
D168UVCodeplug::FixedLocationNamesElement::FixedLocationNamesElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}

void
D168UVCodeplug::FixedLocationNamesElement::clear() {
  memset(_data, 0x00, size());
}


QString
D168UVCodeplug::FixedLocationNamesElement::name(unsigned int n) const {
  if (n >= Limit::nameCount())
    return {};
  return readASCII(Offset::names() + n*Offset::betweenNames(), Limit::nameLength(), 0x00);
}

void
D168UVCodeplug::FixedLocationNamesElement::setName(unsigned int n, const QString &name) {
  if (n >= Limit::nameCount())
    return;
  writeASCII(Offset::names() + n*Offset::betweenNames(), name, Limit::nameLength(), 0x00);
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


void
D168UVCodeplug::allocateUpdated() {
  D878UVCodeplug::allocateUpdated();

  // Fixed location names, not yet touched by dmrconf
  image(0).addElement(Offset::fixedLocationNames(), FixedLocationNamesElement::size());

  // allocate unknown segments
  image(0).addElement(Offset::unknown_25c2000(), 0x0020);
  image(0).addElement(Offset::unknown_25c2c80(), 0x0020);
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

  GeneralSettingsElement(data(Offset::settings())).fromConfig(flags, ctx, err);
  DMRAPRSMessageElement(data(Offset::dmrAPRSMessage())).fromConfig(flags, ctx);
  ExtendedSettingsElement(data(Offset::settingsExtension())).fromConfig(flags, ctx);
  return true;
}

bool
D168UVCodeplug::decodeGeneralSettings(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  GeneralSettingsElement(data(Offset::settings())).updateConfig(ctx, err);
  DMRAPRSMessageElement(data(Offset::dmrAPRSMessage())).updateConfig(ctx);
  ExtendedSettingsElement(data(Offset::settingsExtension())).updateConfig(ctx, err);
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



