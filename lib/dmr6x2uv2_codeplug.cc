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
DMR6X2UV2Codeplug::ExtendedSettingsElement::bluetoothEnabled() const {
  return 0 != getUInt8(Offset::bluetoothEnable());
}

void
DMR6X2UV2Codeplug::ExtendedSettingsElement::enableBluetooth(bool enable) {
  setUInt8(Offset::bluetoothEnable(), enable ? 0x01 : 0x00);
}


bool
DMR6X2UV2Codeplug::ExtendedSettingsElement::internalMicEnabled() const {
  return 0 != getUInt8(Offset::internalMicEnable());
}

void
DMR6X2UV2Codeplug::ExtendedSettingsElement::enableInternalMic(bool enable) {
  setUInt8(Offset::internalMicEnable(), enable ? 0x01 : 0x00);
}


bool
DMR6X2UV2Codeplug::ExtendedSettingsElement::internalSpeakerEnabled() const {
  return 0 != getUInt8(Offset::internalSpeakerEnable());
}

void
DMR6X2UV2Codeplug::ExtendedSettingsElement::enableInternalSpeaker(bool enable) {
  setUInt8(Offset::internalSpeakerEnable(), enable ? 0x01 : 0x00);
}

unsigned int
DMR6X2UV2Codeplug::ExtendedSettingsElement::bluetoothMicGain() const {
  return getUInt8(Offset::bluetoothMicGain());
}

void
DMR6X2UV2Codeplug::ExtendedSettingsElement::setBluetoothMicGain(unsigned int gain) {
  gain = std::min(4U, gain);
  setUInt8(Offset::bluetoothMicGain(), gain);
}

unsigned int
DMR6X2UV2Codeplug::ExtendedSettingsElement::bluetoothSpeakerGain() const {
  return getUInt8(Offset::bluetoothSpeakerGain());
}

void
DMR6X2UV2Codeplug::ExtendedSettingsElement::setBluetoothSpeakerGain(unsigned int gain) {
  gain = std::min(4U, gain);
  setUInt8(Offset::bluetoothSpeakerGain(), gain);
}

Interval
DMR6X2UV2Codeplug::ExtendedSettingsElement::bluetoothHoldDuration() const {
  auto num = getUInt8(Offset::bluetoothHoldDuration());
  if (num <= 30)
    return Interval::fromSeconds(num);
  else if (31 == num)
    return Interval::fromMinutes(1);
  else if (32 == num)
    return Interval::fromMinutes(2);
  /// @todo Implement interval=infinite.
  return Interval();
}

void
DMR6X2UV2Codeplug::ExtendedSettingsElement::setBluetoothHoldDuration(const Interval &dur) {
  if (dur.seconds() <=30)
    setUInt8(Offset::bluetoothHoldDuration(), dur.seconds());
  else if (dur.seconds() <=60)
    setUInt8(Offset::bluetoothHoldDuration(), 31);
  else if (dur.seconds() <=120)
    setUInt8(Offset::bluetoothHoldDuration(), 32);
  else
    setUInt8(Offset::bluetoothHoldDuration(), 33);
}


Interval
DMR6X2UV2Codeplug::ExtendedSettingsElement::bluetoothHoldDelay() const {
  auto num = getUInt8(Offset::bluetoothHoldDelay());
  if (0 == num)
    return Interval::fromMilliseconds(30);
  return Interval::fromMilliseconds(500*num);
}

void
DMR6X2UV2Codeplug::ExtendedSettingsElement::setBluetoothHoldDelay(const Interval &dur) {
  if (dur.milliseconds() <=30)
    setUInt8(Offset::bluetoothHoldDelay(), 0);
  else
    setUInt8(Offset::bluetoothHoldDelay(), dur.milliseconds()/500);
}


bool
DMR6X2UV2Codeplug::ExtendedSettingsElement::bluetoothPTTLatchEnabled() const {
  return 0 != getUInt8(Offset::bluetoothPTTLatch());
}

void
DMR6X2UV2Codeplug::ExtendedSettingsElement::enableBluetoothPTTLatch(bool enable) {
  setUInt8(Offset::bluetoothPTTLatch(), enable ? 0x01 : 0x00);
}


Interval
DMR6X2UV2Codeplug::ExtendedSettingsElement::bluetoothPTTSleepTimeout() const {
  auto num = getUInt8(Offset::bluetoothPTTSleepTimeout());
  return Interval::fromMinutes(num);
}

void
DMR6X2UV2Codeplug::ExtendedSettingsElement::setBluetoothPTTSleepTimeout(const Interval &dur) {
  if (dur.isNull() || (4 < dur.minutes()))
    setUInt8(Offset::bluetoothPTTSleepTimeout(), 0);
  else
    setUInt8(Offset::bluetoothPTTSleepTimeout(), dur.minutes());
}


bool
DMR6X2UV2Codeplug::ExtendedSettingsElement::fmIdleToneEnabled() const {
  return 0x00 != getUInt8(Offset::fmIdleTone());
}

void
DMR6X2UV2Codeplug::ExtendedSettingsElement::enableFMIdleTone(bool enable) {
  setUInt8(Offset::fmIdleTone(), enable ? 0x01 : 0x00);
}


unsigned int
DMR6X2UV2Codeplug::ExtendedSettingsElement::fmMicGain() const {
  return 2*(1 + getUInt8(Offset::fmMicGain()));
}

void
DMR6X2UV2Codeplug::ExtendedSettingsElement::setFMMicGain(unsigned int gain) {
  gain = std::max(1U, std::min(10U, gain));
  setUInt8(Offset::fmMicGain(), (gain-1)/2);
}


bool
DMR6X2UV2Codeplug::ExtendedSettingsElement::totWarningToneEnabled() const {
  return 0x00 != getUInt8(Offset::totWarningTone());
}

void
DMR6X2UV2Codeplug::ExtendedSettingsElement::enableTOTWarningTone(bool enable) {
  setUInt8(Offset::totWarningTone(), enable ? 0x01 : 0x00);
}


bool
DMR6X2UV2Codeplug::ExtendedSettingsElement::atpcEnabled() const {
  return 0x00 != getUInt8(Offset::atpc());
}

void
DMR6X2UV2Codeplug::ExtendedSettingsElement::enableATPC(bool enable) {
  setUInt8(Offset::atpc(), enable ? 0x01 : 0x00);
}


AnytoneGPSSettingsExtension::GPSMode
DMR6X2UV2Codeplug::ExtendedSettingsElement::gnss() const {
  switch ((GNSS) getUInt8(Offset::gnss())) {
  case GNSS::GPS: return AnytoneGPSSettingsExtension::GPSMode::GPS;
  case GNSS::BeiDou: return AnytoneGPSSettingsExtension::GPSMode::Beidou;
  case GNSS::Both: return AnytoneGPSSettingsExtension::GPSMode::GPS_Beidou;
  }

  return AnytoneGPSSettingsExtension::GPSMode::GPS;
}

void
DMR6X2UV2Codeplug::ExtendedSettingsElement::setGNSS(AnytoneGPSSettingsExtension::GPSMode gnss) {
  switch (gnss) {
  case AnytoneGPSSettingsExtension::GPSMode::GPS:
    setUInt8(Offset::gnss(), (unsigned int)GNSS::GPS);
    break;
  case AnytoneGPSSettingsExtension::GPSMode::Beidou:
  case AnytoneGPSSettingsExtension::GPSMode::Glonass:
  case AnytoneGPSSettingsExtension::GPSMode::Beidou_Glonass:
    setUInt8(Offset::gnss(), (unsigned int)GNSS::BeiDou);
    break;
  case AnytoneGPSSettingsExtension::GPSMode::GPS_Beidou:
  case AnytoneGPSSettingsExtension::GPSMode::GPS_Glonas:
  case AnytoneGPSSettingsExtension::GPSMode::All:
    setUInt8(Offset::gnss(), (unsigned int)GNSS::Both);
    break;
  }
}


DMR6X2UV2Codeplug::ExtendedSettingsElement::ChannelIndexDisplay
DMR6X2UV2Codeplug::ExtendedSettingsElement::channelIndexDisplay() const {
  return (ChannelIndexDisplay)getUInt8(Offset::displayChannelIndex());
}

void
DMR6X2UV2Codeplug::ExtendedSettingsElement::setChannelIndexDisplay(ChannelIndexDisplay mode) {
  setUInt8(Offset::displayChannelIndex(), (unsigned int)mode);
}


bool
DMR6X2UV2Codeplug::ExtendedSettingsElement::wxAlarmEnabled() const {
  return 0x00 != getUInt8(Offset::wxAlarm());
}

void
DMR6X2UV2Codeplug::ExtendedSettingsElement::enableWXAlarm(bool enable) {
  setUInt8(Offset::wxAlarm(), enable ? 0x01 : 0x00);
}


bool
DMR6X2UV2Codeplug::ExtendedSettingsElement::locationSourceGNSS() const {
  return 0 == getUInt8(Offset::fixedLocationIndex());
}

unsigned int
DMR6X2UV2Codeplug::ExtendedSettingsElement::fixedLocationIndex() const {
  return getUInt8(Offset::fixedLocationIndex())-1;
}

void
DMR6X2UV2Codeplug::ExtendedSettingsElement::setFixedLocationIndex(unsigned int idx) {
  setUInt8(Offset::fixedLocationIndex(), idx+1);
}

void
DMR6X2UV2Codeplug::ExtendedSettingsElement::setLocationSourceGNSS() {
  setUInt8(Offset::fixedLocationIndex(), 0);
}


Channel::Power
DMR6X2UV2Codeplug::ExtendedSettingsElement::satPower() const {
  switch ((Power)getUInt8(Offset::satPower())) {
  case Power::Low: return Channel::Power::Low;
  case Power::Medium: return Channel::Power::Mid;
  case Power::High: return Channel::Power::High;
  case Power::Turbo: return Channel::Power::Max;
  }
  return Channel::Power::Low;
}

void
DMR6X2UV2Codeplug::ExtendedSettingsElement::setSatPower(Channel::Power power) {
  switch (power) {
  case Channel::Power::Min:
  case Channel::Power::Low:
    setUInt8(Offset::satPower(), (unsigned int)Power::Low);
  break;
  case Channel::Power::Mid:
    setUInt8(Offset::satPower(), (unsigned int)Power::Medium);
  break;
  case Channel::Power::High:
    setUInt8(Offset::satPower(), (unsigned int)Power::High);
  break;
  case Channel::Power::Max:
    setUInt8(Offset::satPower(), (unsigned int)Power::Turbo);
  break;
  }
}


unsigned int
DMR6X2UV2Codeplug::ExtendedSettingsElement::satSquelchLevel() const {
  return 2*getUInt8(Offset::satSquelch());
}
void
DMR6X2UV2Codeplug::ExtendedSettingsElement::setSatSquelchLevel(unsigned int level) {
  level = std::min(10U, level);
  if (1 == level) level = 2; // otherwise level=1 => open
  setUInt8(Offset::satSquelch(), level/2);
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

  // Bluetooth settings.
  enableBluetooth(ext->bluetoothSettings()->bluetoothEnabled());
  enableInternalMic(ext->bluetoothSettings()->internalMicEnabled());
  enableInternalSpeaker(ext->bluetoothSettings()->internalSpeakerEnabled());
  setBluetoothMicGain(ext->bluetoothSettings()->micGain());
  setBluetoothSpeakerGain(ext->bluetoothSettings()->speakerGain());
  setBluetoothHoldDuration(ext->bluetoothSettings()->holdDuration());
  setBluetoothHoldDelay(ext->bluetoothSettings()->holdDelay());
  enableBluetoothPTTLatch(ext->bluetoothSettings()->pttLatch());
  setBluetoothPTTSleepTimeout(ext->bluetoothSettings()->pttSleepTimer());

  // Encode audio settings
  enableFMIdleTone(ext->toneSettings()->fmIdleChannelToneEnabled());
  if (ext->audioSettings()->fmMicGainEnabled())
    setFMMicGain(ext->audioSettings()->fmMicGain());
  else
    setFMMicGain(ctx.config()->settings()->micLevel());
  enableTOTWarningTone(ext->toneSettings()->totNotification());
  enableWXAlarm(ext->toneSettings()->wxAlarm());

  // Power settings
  enableATPC(ext->powerSaveSettings()->atpc());

  // GPS settings
  setGNSS(ext->gpsSettings()->mode());

  // Display settings
  setChannelIndexDisplay(ext->displaySettings()->showGlobalChannelNumber() ?
                           ChannelIndexDisplay::GlobalIndex :
                           ChannelIndexDisplay::IndexWithinZone);

  // Sat settings
  setSatPower(ext->satelliteSettings()->power());
  setSatSquelchLevel(ext->satelliteSettings()->squelch());

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

  // Bluetooth settings
  ext->bluetoothSettings()->enableBluetooth(bluetoothEnabled());
  ext->bluetoothSettings()->enableInternalMic(internalMicEnabled());
  ext->bluetoothSettings()->enableInternalSpeaker(internalSpeakerEnabled());
  ext->bluetoothSettings()->setMicGain(bluetoothMicGain());
  ext->bluetoothSettings()->setSpeakerGain(bluetoothSpeakerGain());
  ext->bluetoothSettings()->setHoldDuration(bluetoothHoldDuration());
  ext->bluetoothSettings()->setHoldDelay(bluetoothHoldDelay());
  ext->bluetoothSettings()->enablePTTLatch(bluetoothPTTLatchEnabled());
  ext->bluetoothSettings()->setPTTSleepTimer(bluetoothPTTSleepTimeout());

  // Store FM mic gain separately
  ext->toneSettings()->enableFMIdleChannelTone(fmIdleToneEnabled());
  ext->audioSettings()->setFMMicGain(fmMicGain());
  // Enable separate mic gain, if it differs from the DMR mic gain:
  ext->audioSettings()->enableFMMicGain(
        ctx.config()->settings()->micLevel() != fmMicGain());
  ext->toneSettings()->enableTOTNotification(totWarningToneEnabled());
  ext->toneSettings()->enableWXAlarm(wxAlarmEnabled());

  // Power settings
  ext->powerSaveSettings()->enableATPC(atpcEnabled());

  // Store GPS settings
  ext->gpsSettings()->setMode(gnss());

  // Display settings
  ext->displaySettings()->enableShowGlobalChannelNumber(
        ChannelIndexDisplay::GlobalIndex == channelIndexDisplay());

  // Satellite settings
  ext->satelliteSettings()->setPower(satPower());
  ext->satelliteSettings()->setSquelch(satSquelchLevel());

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
 * Implementation of DMR6X2UV2Codeplug::APRSFilterElement
 * ********************************************************************************************* */
DMR6X2UV2Codeplug::APRSFilterElement::APRSFilterElement(uint8_t *ptr, size_t size)
  : Element{ptr, size}
{
  // pass
}

DMR6X2UV2Codeplug::APRSFilterElement::APRSFilterElement(uint8_t *ptr)
  : Element{ptr, size()}
{
  // pass...
}


void
DMR6X2UV2Codeplug::APRSFilterElement::clear() {
  memset(_data, 0, _size);
}

bool
DMR6X2UV2Codeplug::APRSFilterElement::isValid() const {
  if (! Element::isValid())
    return false;
  return 0 != getUInt8(Offset::valid());
}


QString
DMR6X2UV2Codeplug::APRSFilterElement::call() const {
  return readASCII(Offset::call(), Limit::call(), 0x00);
}

void
DMR6X2UV2Codeplug::APRSFilterElement::setCall(const QString &call) {
  writeASCII(Offset::call(), call, Limit::call(), 0x00);
}


unsigned int
DMR6X2UV2Codeplug::APRSFilterElement::ssid() const {
  return getUInt8(Offset::ssid());
}

void
DMR6X2UV2Codeplug::APRSFilterElement::setSSID(unsigned int ssid) {
  setUInt8(Offset::ssid(), ssid);
}




/* ********************************************************************************************* *
 * Implementation of DMR6X2UV2Codeplug::GPSRoamingZoneElement
 * ********************************************************************************************* */
DMR6X2UV2Codeplug::GPSRoamingZoneElement::GPSRoamingZoneElement(uint8_t *ptr, size_t size)
  : Element{ptr, size}
{
  // pass...
}

DMR6X2UV2Codeplug::GPSRoamingZoneElement::GPSRoamingZoneElement(uint8_t *ptr)
  : Element{ptr, size()}
{
  // pass...
}

void
DMR6X2UV2Codeplug::GPSRoamingZoneElement::clear() {
  memset(_data, 0, size());
}

bool
DMR6X2UV2Codeplug::GPSRoamingZoneElement::isValid() const {
  return Element::isValid() && (0 != getUInt8(Offset::valid()));
}


bool
DMR6X2UV2Codeplug::GPSRoamingZoneElement::hasRoamingZoneIndex() const {
  return 0xff != getUInt8(Offset::zoneIndex());
}

unsigned int
DMR6X2UV2Codeplug::GPSRoamingZoneElement::roamingZoneIndex() const {
  return getUInt8(Offset::zoneIndex());
}

void
DMR6X2UV2Codeplug::GPSRoamingZoneElement::setRoamingZoneIndex(unsigned int idx) {
  setUInt8(Offset::zoneIndex(), idx);
}

void
DMR6X2UV2Codeplug::GPSRoamingZoneElement::clearRoamingZoneIndex() {
  setUInt8(Offset::zoneIndex(), 0xff);
}


QGeoCoordinate
DMR6X2UV2Codeplug::GPSRoamingZoneElement::coordinate() const {
  double latDeg = getUInt8(Offset::latDegrees()), latMin = getUInt8(Offset::latMinutes()),
      latSec = getUInt8(Offset::latSeconds()),
      lonDeg = getUInt8(Offset::lonDegrees()), lonMin = getUInt8(Offset::lonMinutes()),
      lonSec = getUInt8(Offset::lonSeconds());
  double lat = latDeg + (latMin + (latSec/100))/60,
      lon = lonDeg + (lonMin + (lonSec/100))/60;
  if (getUInt8(Offset::latHemisphere())) lat *= -1;
  if (getUInt8(Offset::lonHemisphere())) lon *= -1;
  return QGeoCoordinate(lat, lon);
}

void
DMR6X2UV2Codeplug::GPSRoamingZoneElement::setCoordinate(const QGeoCoordinate &coor) {
  double lat = coor.latitude(), lon = coor.longitude();
  setUInt8(Offset::latHemisphere(), (lat<0) ? 1 : 0); lat = std::abs(lat);
  setUInt8(Offset::lonHemisphere(), (lon<0) ? 1 : 0); lon = std::abs(lon);
  setUInt8(Offset::latDegrees(), lat); lat -= int(lat); lat *=60;
  setUInt8(Offset::lonDegrees(), lon); lon -= int(lon); lon *=60;
  setUInt8(Offset::latMinutes(), lat); lat -= int(lat); lat *=100;
  setUInt8(Offset::lonMinutes(), lon); lon -= int(lon); lon *=100;
  setUInt8(Offset::latSeconds(), lat);
  setUInt8(Offset::lonSeconds(), lon);
}


unsigned int
DMR6X2UV2Codeplug::GPSRoamingZoneElement::radius() const {
  return getUInt16_le(Offset::radius());
}

void
DMR6X2UV2Codeplug::GPSRoamingZoneElement::setRadius(unsigned int radius) {
  setUInt16_le(Offset::radius(), radius);
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
DMR6X2UV2Codeplug::allocateUpdated() {
  image(0).addElement(Offset::aprsFilterBank(), Limit::aprsFilter()*APRSFilterElement::size());
  image(0).addElement(Offset::gpsRoamingZones(), Limit::gpsRoamingZones()*GPSRoamingZoneElement::size());
}

void
DMR6X2UV2Codeplug::allocateForEncoding() {
  DMR6X2UVCodeplug::allocateForEncoding();
}

void
DMR6X2UV2Codeplug::allocateForDecoding() {
  DMR6X2UVCodeplug::allocateForDecoding();
}


bool
DMR6X2UV2Codeplug::encodeElements(const Flags &flags, Context &ctx, const ErrorStack &err) {
  if (! DMR6X2UVCodeplug::encodeElements(flags, ctx, err))
    return false;

  return true;
}


bool
DMR6X2UV2Codeplug::createElements(Context &ctx, const ErrorStack &err) {
  if (! DMR6X2UVCodeplug::createElements(ctx, err))
    return false;

  return true;
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


