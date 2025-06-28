#include "gpssystem.hh"
#include "roamingchannel.hh"
#include "config.hh"
#include "intermediaterepresentation.hh"
#include "dmr6x2uv2_codeplug.hh"
#include "utils.hh"
#include "logger.hh"


/* ********************************************************************************************* *
 * Implementation of DMR6X2UV2Codeplug::GeneralSettingsElement::KeyFunction
 * ********************************************************************************************* */
uint8_t
DMR6X2UV2Codeplug::GeneralSettingsElement::KeyFunction::encode(AnytoneKeySettingsExtension::KeyFunction func) {
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
  case AnytoneKeySettingsExtension::KeyFunction::SimplexRepeater:   return (uint8_t)KeyFunction::SimplexRepeater;
  case AnytoneKeySettingsExtension::KeyFunction::Ranging:           return (uint8_t)KeyFunction::Ranging;
  case AnytoneKeySettingsExtension::KeyFunction::ChannelRanging:    return (uint8_t)KeyFunction::ChannelRanging;
  case AnytoneKeySettingsExtension::KeyFunction::MaxVolume:         return (uint8_t)KeyFunction::MaxVolume;
  case AnytoneKeySettingsExtension::KeyFunction::Slot:              return (uint8_t)KeyFunction::Slot;
  case AnytoneKeySettingsExtension::KeyFunction::Squelch:           return (uint8_t)KeyFunction::Squelch;
  case AnytoneKeySettingsExtension::KeyFunction::Roaming:           return (uint8_t)KeyFunction::Roaming;
  case AnytoneKeySettingsExtension::KeyFunction::Zone:              return (uint8_t)KeyFunction::Zone;
  case AnytoneKeySettingsExtension::KeyFunction::RoamingSet:        return (uint8_t)KeyFunction::RoamingSet;
  case AnytoneKeySettingsExtension::KeyFunction::Mute:              return (uint8_t)KeyFunction::Mute;
  case AnytoneKeySettingsExtension::KeyFunction::CtcssDcsSet:       return (uint8_t)KeyFunction::CtcssDcsSet;
  case AnytoneKeySettingsExtension::KeyFunction::APRSTypeSwitch:    return (uint8_t)KeyFunction::APRSType;
  case AnytoneKeySettingsExtension::KeyFunction::APRSSet:           return (uint8_t)KeyFunction::APRSSet;
  case AnytoneKeySettingsExtension::KeyFunction::TBSTSend:          return (uint8_t)KeyFunction::TBSTSend;
  case AnytoneKeySettingsExtension::KeyFunction::Bluetooth:         return (uint8_t)KeyFunction::BluetoothToggle;
  case AnytoneKeySettingsExtension::KeyFunction::GPS:               return (uint8_t)KeyFunction::GPSToggle;
  case AnytoneKeySettingsExtension::KeyFunction::ChannelName:       return (uint8_t)KeyFunction::ChannelName;
  case AnytoneKeySettingsExtension::KeyFunction::APRSSend:          return (uint8_t)KeyFunction::APRSSend;
  case AnytoneKeySettingsExtension::KeyFunction::APRSInfo:          return (uint8_t)KeyFunction::APRSInfo;
  case AnytoneKeySettingsExtension::KeyFunction::GPSRoaming:        return (uint8_t)KeyFunction::GPSRoaming;
  case AnytoneKeySettingsExtension::KeyFunction::CDTScan:           return (uint8_t)KeyFunction::CTCSSScan;
  case AnytoneKeySettingsExtension::KeyFunction::DIMShut:           return (uint8_t)KeyFunction::DIMShut;
  case AnytoneKeySettingsExtension::KeyFunction::SatPredict:        return (uint8_t)KeyFunction::SatellitePredict;
  default:                                                          return (uint8_t)KeyFunction::Off;
  }
}

AnytoneKeySettingsExtension::KeyFunction
DMR6X2UV2Codeplug::GeneralSettingsElement::KeyFunction::decode(uint8_t code) {
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
  case KeyFunction::SimplexRepeater:   return AnytoneKeySettingsExtension::KeyFunction::SimplexRepeater;
  case KeyFunction::Ranging:           return AnytoneKeySettingsExtension::KeyFunction::Ranging;
  case KeyFunction::ChannelRanging:    return AnytoneKeySettingsExtension::KeyFunction::ChannelRanging;
  case KeyFunction::MaxVolume:         return AnytoneKeySettingsExtension::KeyFunction::MaxVolume;
  case KeyFunction::Slot:              return AnytoneKeySettingsExtension::KeyFunction::Slot;
  case KeyFunction::Squelch:           return AnytoneKeySettingsExtension::KeyFunction::Squelch;
  case KeyFunction::Roaming:           return AnytoneKeySettingsExtension::KeyFunction::Roaming;
  case KeyFunction::Zone:              return AnytoneKeySettingsExtension::KeyFunction::Zone;
  case KeyFunction::RoamingSet:        return AnytoneKeySettingsExtension::KeyFunction::RoamingSet;
  case KeyFunction::Mute:              return AnytoneKeySettingsExtension::KeyFunction::Mute;
  case KeyFunction::CtcssDcsSet:       return AnytoneKeySettingsExtension::KeyFunction::CtcssDcsSet;
  case KeyFunction::APRSType:          return AnytoneKeySettingsExtension::KeyFunction::APRSTypeSwitch;
  case KeyFunction::APRSSet:           return AnytoneKeySettingsExtension::KeyFunction::APRSSet;
  case KeyFunction::TBSTSend:          return AnytoneKeySettingsExtension::KeyFunction::TBSTSend;
  case KeyFunction::BluetoothToggle:   return AnytoneKeySettingsExtension::KeyFunction::Bluetooth;
  case KeyFunction::GPSToggle:         return AnytoneKeySettingsExtension::KeyFunction::GPS;
  case KeyFunction::ChannelName:       return AnytoneKeySettingsExtension::KeyFunction::ChannelName;
  case KeyFunction::APRSSend:          return AnytoneKeySettingsExtension::KeyFunction::APRSSend;
  case KeyFunction::APRSInfo:          return AnytoneKeySettingsExtension::KeyFunction::APRSInfo;
  case KeyFunction::GPSRoaming:        return AnytoneKeySettingsExtension::KeyFunction::GPSRoaming;
  case KeyFunction::CTCSSScan:         return AnytoneKeySettingsExtension::KeyFunction::CDTScan;
  case KeyFunction::DIMShut:           return AnytoneKeySettingsExtension::KeyFunction::DIMShut;
  case KeyFunction::SatellitePredict:  return AnytoneKeySettingsExtension::KeyFunction::SatPredict;
  default:                             return AnytoneKeySettingsExtension::KeyFunction::Off;
  }
}

/* ********************************************************************************************* *
 * Implementation of DMR6X2UV2Codeplug::GeneralSettingsElement
 * ********************************************************************************************* */
DMR6X2UV2Codeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr, unsigned size)
  : DMR6X2UVCodeplug::GeneralSettingsElement(ptr, size)
{
  // pass...
}

DMR6X2UV2Codeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr)
  : DMR6X2UVCodeplug::GeneralSettingsElement(ptr, GeneralSettingsElement::size())
{
  // pass...
}

AnytoneKeySettingsExtension::KeyFunction
DMR6X2UV2Codeplug::GeneralSettingsElement::funcKeyAShort() const {
  return KeyFunction::decode(getUInt8(Offset::progFuncKeyAShort()));
}
void
DMR6X2UV2Codeplug::GeneralSettingsElement::setFuncKeyAShort(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKeyAShort(), KeyFunction::encode(func));
}

AnytoneKeySettingsExtension::KeyFunction
DMR6X2UV2Codeplug::GeneralSettingsElement::funcKeyBShort() const {
  return KeyFunction::decode(getUInt8(Offset::progFuncKeyBShort()));
}
void
DMR6X2UV2Codeplug::GeneralSettingsElement::setFuncKeyBShort(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKeyBShort(), KeyFunction::encode(func));
}

AnytoneKeySettingsExtension::KeyFunction
DMR6X2UV2Codeplug::GeneralSettingsElement::funcKeyCShort() const {
  return KeyFunction::decode(getUInt8(Offset::progFuncKeyCShort()));
}
void
DMR6X2UV2Codeplug::GeneralSettingsElement::setFuncKeyCShort(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKeyCShort(), KeyFunction::encode(func));
}

AnytoneKeySettingsExtension::KeyFunction
DMR6X2UV2Codeplug::GeneralSettingsElement::funcKey1Short() const {
  return KeyFunction::decode(getUInt8(Offset::progFuncKey1Short()));
}
void
DMR6X2UV2Codeplug::GeneralSettingsElement::setFuncKey1Short(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKey1Short(), KeyFunction::encode(func));
}

AnytoneKeySettingsExtension::KeyFunction
DMR6X2UV2Codeplug::GeneralSettingsElement::funcKey2Short() const {
  return KeyFunction::decode(getUInt8(Offset::progFuncKey2Short()));
}
void
DMR6X2UV2Codeplug::GeneralSettingsElement::setFuncKey2Short(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKey2Short(), KeyFunction::encode(func));
}

AnytoneKeySettingsExtension::KeyFunction
DMR6X2UV2Codeplug::GeneralSettingsElement::funcKeyALong() const {
  return KeyFunction::decode(getUInt8(Offset::progFuncKeyALong()));
}
void
DMR6X2UV2Codeplug::GeneralSettingsElement::setFuncKeyALong(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKeyALong(), KeyFunction::encode(func));
}

AnytoneKeySettingsExtension::KeyFunction
DMR6X2UV2Codeplug::GeneralSettingsElement::funcKeyBLong() const {
  return KeyFunction::decode(getUInt8(Offset::progFuncKeyBLong()));
}
void
DMR6X2UV2Codeplug::GeneralSettingsElement::setFuncKeyBLong(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKeyBLong(), KeyFunction::encode(func));
}

AnytoneKeySettingsExtension::KeyFunction
DMR6X2UV2Codeplug::GeneralSettingsElement::funcKeyCLong() const {
  return KeyFunction::decode(getUInt8(Offset::progFuncKeyCLong()));
}
void
DMR6X2UV2Codeplug::GeneralSettingsElement::setFuncKeyCLong(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKeyCLong(), KeyFunction::encode(func));
}

AnytoneKeySettingsExtension::KeyFunction
DMR6X2UV2Codeplug::GeneralSettingsElement::funcKey1Long() const {
  return KeyFunction::decode(getUInt8(Offset::progFuncKey1Long()));
}
void
DMR6X2UV2Codeplug::GeneralSettingsElement::setFuncKey1Long(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKey1Long(), KeyFunction::encode(func));
}

AnytoneKeySettingsExtension::KeyFunction
DMR6X2UV2Codeplug::GeneralSettingsElement::funcKey2Long() const {
  return KeyFunction::decode(getUInt8(Offset::progFuncKey2Long()));
}
void
DMR6X2UV2Codeplug::GeneralSettingsElement::setFuncKey2Long(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKey2Long(), KeyFunction::encode(func));
}


bool
DMR6X2UV2Codeplug::GeneralSettingsElement::fromConfig(const Flags &flags, Context &ctx)
{
  if (! DMR6X2UVCodeplug::GeneralSettingsElement::fromConfig(flags, ctx))
    return false;

  // apply DMR-6X2UV Pro specific settings.
  AnytoneSettingsExtension *ext = ctx.config()->settings()->anytoneExtension();
  if (nullptr == ext)
    return true;

  return true;
}

bool
DMR6X2UV2Codeplug::GeneralSettingsElement::updateConfig(Context &ctx)
{
  if (! DMR6X2UVCodeplug::GeneralSettingsElement::updateConfig(ctx))
    return false;

  // Get or add settings extension
  AnytoneSettingsExtension *ext = nullptr;
  if (ctx.config()->settings()->anytoneExtension()) {
    ext = ctx.config()->settings()->anytoneExtension();
  } else {
    ext = new AnytoneSettingsExtension();
    ctx.config()->settings()->setAnytoneExtension(ext);
  }

  return true;
}



/* ********************************************************************************************* *
 * Implementation of DMR6X2UV2Codeplug::ExtendedSettingsElement
 * ********************************************************************************************* */
DMR6X2UV2Codeplug::ExtendedSettingsElement::ExtendedSettingsElement(uint8_t *ptr, unsigned size)
  : DMR6X2UVCodeplug::ExtendedSettingsElement(ptr, size)
{
  // pass...
}

DMR6X2UV2Codeplug::ExtendedSettingsElement::ExtendedSettingsElement(uint8_t *ptr)
  : DMR6X2UVCodeplug::ExtendedSettingsElement(ptr, ExtendedSettingsElement::size())
{
  // pass...
}

bool
DMR6X2UV2Codeplug::ExtendedSettingsElement::fromConfig(const Flags &flags, Context &ctx, const ErrorStack &err)
{
  if (! DMR6X2UVCodeplug::ExtendedSettingsElement::fromConfig(flags, ctx, err)) {
    errMsg(err) << "Cannot encode extended settings for DMR-6X2UV codeplug.";
    return false;
  }

  // Encode device specific settings
  AnytoneSettingsExtension *ext = ctx.config()->settings()->anytoneExtension();
  if (nullptr == ext)
    return true;

  return true;
}

bool
DMR6X2UV2Codeplug::ExtendedSettingsElement::updateConfig(Context &ctx, const ErrorStack &err) {
  if (! DMR6X2UVCodeplug::ExtendedSettingsElement::updateConfig(ctx, err)) {
    errMsg(err) << "Cannot decode extended settings for DMR-6X2UV codeplug";
    return false;
  }

  AnytoneSettingsExtension *ext = ctx.config()->settings()->anytoneExtension();
  if (nullptr == ext) {
    ext = new AnytoneSettingsExtension();
    ctx.config()->settings()->setAnytoneExtension(ext);
  }

  return true;
}

bool
DMR6X2UV2Codeplug::ExtendedSettingsElement::linkConfig(Context &ctx, const ErrorStack &err) {
  if (! DMR6X2UVCodeplug::ExtendedSettingsElement::linkConfig(ctx, err)) {
    errMsg(err) << "Cannot link extended settings for DMR-6X2UV codeplug";
    return false;
  }

  AnytoneSettingsExtension *ext = ctx.config()->settings()->anytoneExtension();
  if (nullptr == ext)
    return false;

  return true;
}



/* ********************************************************************************************* *
 * Implementation of DMR6X2UV2Codeplug
 * ********************************************************************************************* */
DMR6X2UV2Codeplug::DMR6X2UV2Codeplug(const QString &label, QObject *parent)
  : DMR6X2UVCodeplug(label, parent)
{
  // pass...
}

DMR6X2UV2Codeplug::DMR6X2UV2Codeplug(QObject *parent)
  : DMR6X2UVCodeplug("BTECH DMR-6X2UV PRO", parent)
{
  // pass...
}


void
DMR6X2UV2Codeplug::allocateGeneralSettings() {
  image(0).addElement(Offset::settings(), GeneralSettingsElement::size());
  image(0).addElement(Offset::settingsExtension(), ExtendedSettingsElement::size());
}

bool
DMR6X2UV2Codeplug::encodeGeneralSettings(const Flags &flags, Context &ctx, const ErrorStack &err) {
  if (! GeneralSettingsElement(data(Offset::settings())).fromConfig(flags, ctx)) {
    errMsg(err) << "Cannot encode general settings element.";
    return false;
  }

  if (! ExtendedSettingsElement(data(Offset::settingsExtension())).fromConfig(flags, ctx)) {
    errMsg(err) << "Cannot encode extended settings element.";
    return false;
  }

  return true;
}

bool
DMR6X2UV2Codeplug::decodeGeneralSettings(Context &ctx, const ErrorStack &err) {
  if (! GeneralSettingsElement(data(Offset::settings())).updateConfig(ctx)) {
    errMsg(err) << "Cannot decode general settings element.";
    return false;
  }

  if (! ExtendedSettingsElement(data(Offset::settingsExtension())).updateConfig(ctx)) {
    errMsg(err) << "Cannot decode extended settings element.";
    return false;
  }

  return true;
}
bool
DMR6X2UV2Codeplug::linkGeneralSettings(Context &ctx, const ErrorStack &err) {
  return GeneralSettingsElement(data(Offset::settings())).linkSettings(ctx.config()->settings(), ctx, err);
}

