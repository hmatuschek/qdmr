#include "gpssystem.hh"
#include "roamingchannel.hh"
#include "config.hh"
#include "dmr6x2uv_codeplug.hh"
#include "utils.hh"
#include "logger.hh"


/* ******************************************************************************************** *
 * Implementation of DMR6X2UVCodeplug::Color
 * ******************************************************************************************** */
AnytoneDisplaySettingsExtension::Color
DMR6X2UVCodeplug::Color::decode(uint8_t code) {
  switch((CodedColor) code) {
  case Orange: return AnytoneDisplaySettingsExtension::Color::Orange;
  case Red: return AnytoneDisplaySettingsExtension::Color::Red;
  case Yellow: return AnytoneDisplaySettingsExtension::Color::Yellow;
  case Green: return AnytoneDisplaySettingsExtension::Color::Green;
  case Turquoise: return AnytoneDisplaySettingsExtension::Color::Turquoise;
  case Blue: return AnytoneDisplaySettingsExtension::Color::Blue;
  case White: return AnytoneDisplaySettingsExtension::Color::White;
  case Black: return AnytoneDisplaySettingsExtension::Color::Black;
  default: break;
  }
  return AnytoneDisplaySettingsExtension::Color::White;
}

uint8_t
DMR6X2UVCodeplug::Color::encode(AnytoneDisplaySettingsExtension::Color color) {
  switch(color) {
  case AnytoneDisplaySettingsExtension::Color::Orange: return (uint8_t) Orange;
  case AnytoneDisplaySettingsExtension::Color::Red: return (uint8_t) Red;
  case AnytoneDisplaySettingsExtension::Color::Yellow: return (uint8_t) Yellow;
  case AnytoneDisplaySettingsExtension::Color::Green: return (uint8_t) Green;
  case AnytoneDisplaySettingsExtension::Color::Turquoise: return (uint8_t) Turquoise;
  case AnytoneDisplaySettingsExtension::Color::Blue: return (uint8_t) Blue;
  case AnytoneDisplaySettingsExtension::Color::White: return (uint8_t) White;
  case AnytoneDisplaySettingsExtension::Color::Black: return (uint8_t) Black;
  default: break;
  }
  return (uint8_t) White;
}

/* ******************************************************************************************** *
 * Implementation of DMR6X2UVCodeplug::BackgroundColor
 * ******************************************************************************************** */
AnytoneDisplaySettingsExtension::Color
DMR6X2UVCodeplug::BackgroundColor::decode(uint8_t code) {
  switch((CodedColor) code) {
  case Black: return AnytoneDisplaySettingsExtension::Color::Black;
  case Blue: return AnytoneDisplaySettingsExtension::Color::Blue;
  default: break;
  }
  return AnytoneDisplaySettingsExtension::Color::Black;
}

uint8_t
DMR6X2UVCodeplug::BackgroundColor::encode(AnytoneDisplaySettingsExtension::Color color) {
  switch(color) {
  case AnytoneDisplaySettingsExtension::Color::Black: return (uint8_t) Black;
  case AnytoneDisplaySettingsExtension::Color::Blue: return (uint8_t) Blue;
  default: break;
  }
  return (uint8_t) Black;
}

/* ******************************************************************************************** *
 * Implementation of DMR6X2UVCodeplug::FontColor
 * ******************************************************************************************** */
AnytoneDisplaySettingsExtension::Color
DMR6X2UVCodeplug::FontColor::decode(uint8_t code) {
  switch((CodedColor) code) {
  case Orange: return AnytoneDisplaySettingsExtension::Color::Orange;
  case Red: return AnytoneDisplaySettingsExtension::Color::Red;
  case Yellow: return AnytoneDisplaySettingsExtension::Color::Yellow;
  case Green: return AnytoneDisplaySettingsExtension::Color::Green;
  case Turquoise: return AnytoneDisplaySettingsExtension::Color::Turquoise;
  case Blue: return AnytoneDisplaySettingsExtension::Color::Blue;
  case White: return AnytoneDisplaySettingsExtension::Color::White;
  case Black: return AnytoneDisplaySettingsExtension::Color::Black;
  default: break;
  }
  return AnytoneDisplaySettingsExtension::Color::White;
}

uint8_t
DMR6X2UVCodeplug::FontColor::encode(AnytoneDisplaySettingsExtension::Color color) {
  switch(color) {
  case AnytoneDisplaySettingsExtension::Color::Orange: return (uint8_t) Orange;
  case AnytoneDisplaySettingsExtension::Color::Red: return (uint8_t) Red;
  case AnytoneDisplaySettingsExtension::Color::Yellow: return (uint8_t) Yellow;
  case AnytoneDisplaySettingsExtension::Color::Green: return (uint8_t) Green;
  case AnytoneDisplaySettingsExtension::Color::Turquoise: return (uint8_t) Turquoise;
  case AnytoneDisplaySettingsExtension::Color::Blue: return (uint8_t) Blue;
  case AnytoneDisplaySettingsExtension::Color::White: return (uint8_t) White;
  case AnytoneDisplaySettingsExtension::Color::Black: return (uint8_t) Black;
  default: break;
  }
  return (uint8_t) White;
}


/* ********************************************************************************************* *
 * Implementation of DMR6X2UVCodeplug::GeneralSettingsElement::KeyFunction
 * ********************************************************************************************* */
uint8_t
DMR6X2UVCodeplug::GeneralSettingsElement::KeyFunction::encode(AnytoneKeySettingsExtension::KeyFunction func) {
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
  case AnytoneKeySettingsExtension::KeyFunction::APRSSet:           return (uint8_t)KeyFunction::APRSSet;
  case AnytoneKeySettingsExtension::KeyFunction::APRSSend:          return (uint8_t)KeyFunction::APRSSend;
  default:                                                          return (uint8_t)KeyFunction::Off;
  }
}

AnytoneKeySettingsExtension::KeyFunction
DMR6X2UVCodeplug::GeneralSettingsElement::KeyFunction::decode(uint8_t code) {
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
  case KeyFunction::APRSSet:           return AnytoneKeySettingsExtension::KeyFunction::APRSSet;
  case KeyFunction::APRSSend:          return AnytoneKeySettingsExtension::KeyFunction::APRSSend;
  default:                             return AnytoneKeySettingsExtension::KeyFunction::Off;
  }
}

/* ********************************************************************************************* *
 * Implementation of DMR6X2UVCodeplug::GeneralSettingsElement
 * ********************************************************************************************* */
DMR6X2UVCodeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr, unsigned size)
  : D868UVCodeplug::GeneralSettingsElement(ptr, size)
{
  // pass...
}

DMR6X2UVCodeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr)
  : D868UVCodeplug::GeneralSettingsElement(ptr, GeneralSettingsElement::size())
{
  // pass...
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::idleChannelTone() const {
  return getUInt8(Offset::idleChannelTone());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableIdleChannelTone(bool enable) {
  return setUInt8(Offset::idleChannelTone(), (enable ? 0x01 : 0x00));
}

unsigned
DMR6X2UVCodeplug::GeneralSettingsElement::transmitTimeout() const {
  return ((unsigned)getUInt8(Offset::transmitTimeout()))*30;
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setTransmitTimeout(unsigned tot) {
  setUInt8(Offset::transmitTimeout(), tot/30);
}

AnytoneDisplaySettingsExtension::Language
DMR6X2UVCodeplug::GeneralSettingsElement::language() const {
  return (AnytoneDisplaySettingsExtension::Language)getUInt8(Offset::language());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setLanguage(AnytoneDisplaySettingsExtension::Language lang) {
  setUInt8(Offset::language(), (unsigned)lang);
}

Frequency
DMR6X2UVCodeplug::GeneralSettingsElement::vfoFrequencyStep() const {
  switch (getUInt8(Offset::vfoFrequencyStep())) {
  case FREQ_STEP_2_5kHz: return Frequency::fromkHz(2.5);
  case FREQ_STEP_5kHz: return Frequency::fromkHz(5);
  case FREQ_STEP_6_25kHz: return Frequency::fromkHz(6.25);
  case FREQ_STEP_10kHz: return Frequency::fromkHz(10);
  case FREQ_STEP_12_5kHz: return Frequency::fromkHz(12.5);
  case FREQ_STEP_20kHz: return Frequency::fromkHz(20);
  case FREQ_STEP_25kHz: return Frequency::fromkHz(25);
  case FREQ_STEP_50kHz: return Frequency::fromkHz(50);
  }
  return Frequency::fromkHz(2.5);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setVFOFrequencyStep(Frequency freq) {
  if (freq.inkHz() <= 2.5)
    setUInt8(Offset::vfoFrequencyStep(), FREQ_STEP_2_5kHz);
  else if (freq.inkHz() <= 5)
    setUInt8(Offset::vfoFrequencyStep(), FREQ_STEP_5kHz);
  else if (freq.inkHz() <= 6.25)
    setUInt8(Offset::vfoFrequencyStep(), FREQ_STEP_6_25kHz);
  else if (freq.inkHz() <= 10)
    setUInt8(Offset::vfoFrequencyStep(), FREQ_STEP_10kHz);
  else if (freq.inkHz() <= 12.5)
    setUInt8(Offset::vfoFrequencyStep(), FREQ_STEP_12_5kHz);
  else if (freq.inkHz() <= 20)
    setUInt8(Offset::vfoFrequencyStep(), FREQ_STEP_20kHz);
  else if (freq.inkHz() <= 25)
    setUInt8(Offset::vfoFrequencyStep(), FREQ_STEP_25kHz);
  else
    setUInt8(Offset::vfoFrequencyStep(), FREQ_STEP_50kHz);
}

AnytoneKeySettingsExtension::KeyFunction
DMR6X2UVCodeplug::GeneralSettingsElement::funcKeyAShort() const {
  return KeyFunction::decode(getUInt8(Offset::progFuncKeyAShort()));
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setFuncKeyAShort(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKeyAShort(), KeyFunction::encode(func));
}

AnytoneKeySettingsExtension::KeyFunction
DMR6X2UVCodeplug::GeneralSettingsElement::funcKeyBShort() const {
  return KeyFunction::decode(getUInt8(Offset::progFuncKeyBShort()));
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setFuncKeyBShort(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKeyBShort(), KeyFunction::encode(func));
}

AnytoneKeySettingsExtension::KeyFunction
DMR6X2UVCodeplug::GeneralSettingsElement::funcKeyCShort() const {
  return KeyFunction::decode(getUInt8(Offset::progFuncKeyCShort()));
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setFuncKeyCShort(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKeyCShort(), KeyFunction::encode(func));
}

AnytoneKeySettingsExtension::KeyFunction
DMR6X2UVCodeplug::GeneralSettingsElement::funcKey1Short() const {
  return KeyFunction::decode(getUInt8(Offset::progFuncKey1Short()));
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setFuncKey1Short(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKey1Short(), KeyFunction::encode(func));
}

AnytoneKeySettingsExtension::KeyFunction
DMR6X2UVCodeplug::GeneralSettingsElement::funcKey2Short() const {
  return KeyFunction::decode(getUInt8(Offset::progFuncKey2Short()));
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setFuncKey2Short(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKey2Short(), KeyFunction::encode(func));
}

AnytoneKeySettingsExtension::KeyFunction
DMR6X2UVCodeplug::GeneralSettingsElement::funcKeyALong() const {
  return KeyFunction::decode(getUInt8(Offset::progFuncKeyALong()));
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setFuncKeyALong(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKeyALong(), KeyFunction::encode(func));
}

AnytoneKeySettingsExtension::KeyFunction
DMR6X2UVCodeplug::GeneralSettingsElement::funcKeyBLong() const {
  return KeyFunction::decode(getUInt8(Offset::progFuncKeyBLong()));
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setFuncKeyBLong(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKeyBLong(), KeyFunction::encode(func));
}

AnytoneKeySettingsExtension::KeyFunction
DMR6X2UVCodeplug::GeneralSettingsElement::funcKeyCLong() const {
  return KeyFunction::decode(getUInt8(Offset::progFuncKeyCLong()));
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setFuncKeyCLong(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKeyCLong(), KeyFunction::encode(func));
}

AnytoneKeySettingsExtension::KeyFunction
DMR6X2UVCodeplug::GeneralSettingsElement::funcKey1Long() const {
  return KeyFunction::decode(getUInt8(Offset::progFuncKey1Long()));
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setFuncKey1Long(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKey1Long(), KeyFunction::encode(func));
}

AnytoneKeySettingsExtension::KeyFunction
DMR6X2UVCodeplug::GeneralSettingsElement::funcKey2Long() const {
  return KeyFunction::decode(getUInt8(Offset::progFuncKey2Long()));
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setFuncKey2Long(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKey2Long(), KeyFunction::encode(func));
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::vfoModeA() const {
  return getUInt8(Offset::vfoModeA());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableVFOModeA(bool enable) {
  setUInt8(Offset::vfoModeA(), (enable ? 0x01 : 0x00));
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::vfoModeB() const {
  return getUInt8(Offset::vfoModeB());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableVFOModeB(bool enable) {
  setUInt8(Offset::vfoModeB(), (enable ? 0x01 : 0x00));
}

AnytoneSettingsExtension::STEType
DMR6X2UVCodeplug::GeneralSettingsElement::steType() const {
  return (AnytoneSettingsExtension::STEType)getUInt8(Offset::steType());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setSTEType(AnytoneSettingsExtension::STEType type) {
  setUInt8(Offset::steType(), (unsigned)type);
}
double
DMR6X2UVCodeplug::GeneralSettingsElement::steFrequency() const {
  switch ((STEFrequency)getUInt8(Offset::steFrequency())) {
  case STEFrequency::Off: return 0;
  case STEFrequency::Hz55_2: return 55.2;
  case STEFrequency::Hz259_2: return 259.2;
  }
  return 0;
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setSTEFrequency(double freq) {
  if (0 >= freq) {
    setUInt8(Offset::steFrequency(), (unsigned)STEFrequency::Off);
  } else if (100 > freq) {
    setUInt8(Offset::steFrequency(), (unsigned)STEFrequency::Hz55_2);
  } else {
    setUInt8(Offset::steFrequency(), (unsigned)STEFrequency::Hz259_2);
  }
}

Interval
DMR6X2UVCodeplug::GeneralSettingsElement::groupCallHangTime() const {
  return Interval::fromSeconds(getUInt8(Offset::groupCallHangTime()));
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setGroupCallHangTime(Interval intv) {
  setUInt8(Offset::groupCallHangTime(), intv.seconds());
}
Interval
DMR6X2UVCodeplug::GeneralSettingsElement::privateCallHangTime() const {
  return Interval::fromSeconds(getUInt8(Offset::privateCallHangTime()));
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setPrivateCallHangTime(Interval intv) {
  setUInt8(Offset::privateCallHangTime(), intv.seconds());
}
Interval
DMR6X2UVCodeplug::GeneralSettingsElement::preWaveDelay() const {
  return Interval::fromMilliseconds((unsigned)getUInt8(Offset::preWaveDelay())*20);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setPreWaveDelay(Interval intv) {
  setUInt8(Offset::preWaveDelay(), intv.milliseconds()/20);
}
Interval
DMR6X2UVCodeplug::GeneralSettingsElement::wakeHeadPeriod() const {
  return Interval::fromMilliseconds(((unsigned)getUInt8(Offset::wakeHeadPeriod()))*20);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setWakeHeadPeriod(Interval intv) {
  setUInt8(Offset::wakeHeadPeriod(), intv.milliseconds()/20);
}

unsigned
DMR6X2UVCodeplug::GeneralSettingsElement::wfmChannelIndex() const {
  return getUInt8(Offset::wfmChannelIndex());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setWFMChannelIndex(unsigned idx) {
  setUInt8(Offset::wfmChannelIndex(), idx);
}
bool
DMR6X2UVCodeplug::GeneralSettingsElement::wfmVFOEnabled() const {
  return getUInt8(Offset::wfmVFOEnabled());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableWFMVFO(bool enable) {
  setUInt8(Offset::wfmVFOEnabled(), (enable ? 0x01 : 0x00));
}

unsigned
DMR6X2UVCodeplug::GeneralSettingsElement::dtmfToneDuration() const {
  switch (getUInt8(Offset::dtmfToneDuration())) {
  case DTMF_DUR_50ms:  return 50;
  case DTMF_DUR_100ms: return 100;
  case DTMF_DUR_200ms: return 200;
  case DTMF_DUR_300ms: return 300;
  case DTMF_DUR_500ms: return 500;
  }
  return 50;
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setDTMFToneDuration(unsigned ms) {
  if (ms<=50) {
    setUInt8(Offset::dtmfToneDuration(), DTMF_DUR_50ms);
  } else if (ms<=100) {
    setUInt8(Offset::dtmfToneDuration(), DTMF_DUR_100ms);
  } else if (ms<=200) {
    setUInt8(Offset::dtmfToneDuration(), DTMF_DUR_200ms);
  } else if (ms<=300) {
    setUInt8(Offset::dtmfToneDuration(), DTMF_DUR_300ms);
  } else {
    setUInt8(Offset::dtmfToneDuration(), DTMF_DUR_500ms);
  }
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::manDown() const {
  return getUInt8(Offset::manDown());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableManDown(bool enable) {
  setUInt8(Offset::manDown(), (enable ? 0x01 : 0x00));
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::wfmMonitor() const {
  return getUInt8(Offset::wfmMonitor());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableWFMMonitor(bool enable) {
  setUInt8(Offset::wfmMonitor(), (enable ? 0x01 : 0x00));
}

Frequency
DMR6X2UVCodeplug::GeneralSettingsElement::tbstFrequency() const {
  switch ((TBSTFrequency)getUInt8(Offset::tbstFrequency())) {
  case TBSTFrequency::Hz1000: return Frequency::fromHz(1000);
  case TBSTFrequency::Hz1450: return Frequency::fromHz(1450);
  case TBSTFrequency::Hz1750: return Frequency::fromHz(1750);
  case TBSTFrequency::Hz2100: return Frequency::fromHz(2100);
  }
  return Frequency::fromHz(1750);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setTBSTFrequency(Frequency freq) {
  if (1000 == freq.inHz()) {
    setUInt8(Offset::tbstFrequency(), (unsigned)TBSTFrequency::Hz1000);
  } else if (1450 == freq.inHz()) {
    setUInt8(Offset::tbstFrequency(), (unsigned)TBSTFrequency::Hz1450);
  } else if (1750 == freq.inHz()) {
    setUInt8(Offset::tbstFrequency(), (unsigned)TBSTFrequency::Hz1750);
  } else if (2100 == freq.inHz()) {
    setUInt8(Offset::tbstFrequency(), (unsigned)TBSTFrequency::Hz2100);
  } else {
    setUInt8(Offset::tbstFrequency(), (unsigned)TBSTFrequency::Hz1750);
  }
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::proMode() const {
  return getUInt8(Offset::proMode());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableProMode(bool enable) {
  setUInt8(Offset::proMode(), (enable ? 0x01 : 0x00));
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::filterOwnID() const {
  return getUInt8(Offset::filterOwnID());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableFilterOwnID(bool enable) {
  setUInt8(Offset::filterOwnID(), (enable ? 0x01 : 0x00));
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::keyToneEnabled() const {
  return 0x00 != getUInt8(Offset::enableKeyTone());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableKeyTone(bool enable) {
  setUInt8(Offset::enableKeyTone(), enable ? 0x01 : 0x00);
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::remoteStunKill() const {
  return getUInt8(Offset::remoteStunKill());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableRemoteStunKill(bool enable) {
  setUInt8(Offset::remoteStunKill(), (enable ? 0x01 : 0x00));
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::remoteMonitor() const {
  return getUInt8(Offset::remoteMonitor());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableRemoteMonitor(bool enable) {
  setUInt8(Offset::remoteMonitor(), (enable ? 0x01 : 0x00));
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::selectTXContactEnabled() const {
  return 0x01 == getUInt8(Offset::selectTXContact());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableSelectTXContact(bool enable) {
  setUInt8(Offset::selectTXContact(), enable ? 0x01 : 0x00);
}

AnytoneDMRSettingsExtension::SlotMatch
DMR6X2UVCodeplug::GeneralSettingsElement::monitorSlotMatch() const {
  return (AnytoneDMRSettingsExtension::SlotMatch)getUInt8(Offset::monSlotMatch());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setMonitorSlotMatch(AnytoneDMRSettingsExtension::SlotMatch match) {
  setUInt8(Offset::monSlotMatch(), (unsigned)match);
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::monitorColorCodeMatch() const {
  return getUInt8(Offset::monColorCodeMatch());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableMonitorColorCodeMatch(bool enable) {
  setUInt8(Offset::monColorCodeMatch(), (enable ? 0x01 : 0x00));
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::monitorIDMatch() const {
  return getUInt8(Offset::monIDMatch());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableMonitorIDMatch(bool enable) {
  setUInt8(Offset::monIDMatch(), (enable ? 0x01 : 0x00));
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::monitorTimeSlotHold() const {
  return getUInt8(Offset::monTimeSlotHold());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableMonitorTimeSlotHold(bool enable) {
  setUInt8(Offset::monTimeSlotHold(), (enable ? 0x01 : 0x00));
}

Interval
DMR6X2UVCodeplug::GeneralSettingsElement::manDownDelay() const {
  return Interval::fromSeconds(getUInt8(Offset::manDownDelay()));
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setManDownDelay(Interval sec) {
  setUInt8(Offset::manDownDelay(), sec.seconds());
}

unsigned
DMR6X2UVCodeplug::GeneralSettingsElement::fmCallHold() const {
  return getUInt8(Offset::fmCallHold());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setFMCallHold(unsigned sec) {
  setUInt8(Offset::fmCallHold(), sec);
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::gpsMessageEnabled() const {
  return getUInt8(Offset::enableGPSMessage());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableGPSMessage(bool enable) {
  setUInt8(Offset::enableGPSMessage(), (enable ? 0x01 : 0x00));
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::maintainCallChannel() const {
  return getUInt8(Offset::maintainCallChannel());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableMaintainCallChannel(bool enable) {
  setUInt8(Offset::maintainCallChannel(), (enable ? 0x01 : 0x00));
}

unsigned
DMR6X2UVCodeplug::GeneralSettingsElement::priorityZoneAIndex() const {
  return getUInt8(Offset::priorityZoneA());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setPriorityZoneAIndex(unsigned idx) {
  setUInt8(Offset::priorityZoneA(), idx);
}
unsigned
DMR6X2UVCodeplug::GeneralSettingsElement::priorityZoneBIndex() const {
  return getUInt8(Offset::priorityZoneB());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setPriorityZoneBIndex(unsigned idx) {
  setUInt8(Offset::priorityZoneB(), idx);
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::smsConfirmEnabled() const {
  return 0x01 == getUInt8(Offset::smsConfirm());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableSMSConfirm(bool enable) {
  setUInt8(Offset::smsConfirm(), enable ? 0x01 : 0x00);
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::simplexRepeaterEnabled() const {
  return 0x01 == getUInt8(Offset::simplexRepEnable());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableSimplexRepeater(bool enable) {
  setUInt8(Offset::simplexRepEnable(), enable ? 0x01 : 0x00);
}

Interval
DMR6X2UVCodeplug::GeneralSettingsElement::gpsUpdatePeriod() const {
  return Interval::fromSeconds(getUInt8(Offset::gpsUpdatePeriod()));
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setGPSUpdatePeriod(Interval intv) {
  setUInt8(Offset::gpsUpdatePeriod(), intv.seconds());
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::monitorSimplexRepeaterEnabled() const {
  return 0x01 == getUInt8(Offset::simplxRepSpeaker());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableMonitorSimplexRepeater(bool enable) {
  setUInt8(Offset::simplxRepSpeaker(), enable ? 0x01 : 0x00);
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::showCurrentContact() const {
  return getUInt8(Offset::showContact());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableShowCurrentContact(bool enable) {
  setUInt8(Offset::showContact(), (enable ? 0x01 : 0x00));
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::keyToneLevelAdjustable() const {
  return 0 == keyToneLevel();
}
unsigned
DMR6X2UVCodeplug::GeneralSettingsElement::keyToneLevel() const {
  return ((unsigned)getUInt8(Offset::keyToneLevel()))*10/15;
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setKeyToneLevel(unsigned level) {
  setUInt8(Offset::keyToneLevel(), level*10/15);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setKeyToneLevelAdjustable() {
  setUInt8(Offset::keyToneLevel(), 0);
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::knobLock() const {
  return getBit(Offset::knobLock());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableKnobLock(bool enable) {
  setBit(Offset::knobLock(), enable);
}
bool
DMR6X2UVCodeplug::GeneralSettingsElement::keypadLock() const {
  return getBit(Offset::keypadLock());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableKeypadLock(bool enable) {
  setBit(Offset::keypadLock(), enable);
}
bool
DMR6X2UVCodeplug::GeneralSettingsElement::sidekeysLock() const {
  return getBit(Offset::sideKeyLock());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableSidekeysLock(bool enable) {
  setBit(Offset::sideKeyLock(), enable);
}
bool
DMR6X2UVCodeplug::GeneralSettingsElement::keyLockForced() const {
  return getBit(Offset::forceKeyLock());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableKeyLockForced(bool enable) {
  setBit(Offset::forceKeyLock(), enable);
}

AnytoneSimplexRepeaterSettingsExtension::TimeSlot
DMR6X2UVCodeplug::GeneralSettingsElement::simplexRepeaterTimeslot() const {
  return (AnytoneSimplexRepeaterSettingsExtension::TimeSlot)getUInt8(Offset::simplxRepSlot());
}

void
DMR6X2UVCodeplug::GeneralSettingsElement::setSimplexRepeaterTimeslot(AnytoneSimplexRepeaterSettingsExtension::TimeSlot slot) {
  setUInt8(Offset::simplxRepSlot(), (unsigned int) slot);
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::showLastHeard() const {
  return getUInt8(Offset::showLastHeard());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableShowLastHeard(bool enable) {
  setUInt8(Offset::showLastHeard(), (enable ? 0x01 : 0x00));
}

AnytoneDMRSettingsExtension::SMSFormat
DMR6X2UVCodeplug::GeneralSettingsElement::smsFormat() const {
  return (AnytoneDMRSettingsExtension::SMSFormat) getUInt8(Offset::smsFormat());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setSMSFormat(AnytoneDMRSettingsExtension::SMSFormat format) {
  setUInt8(Offset::smsFormat(), (unsigned int)format);
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::gpsUnitsImperial() const {
  return getUInt8(Offset::gpsUnits());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableGPSUnitsImperial(bool enable) {
  setUInt8(Offset::gpsUnits(), (enable ? 0x01 : 0x00));
}

Frequency
DMR6X2UVCodeplug::GeneralSettingsElement::autoRepeaterMinFrequencyVHF() const {
  return Frequency::fromHz(getUInt32_le(Offset::autoRepMinVHF())*10);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setAutoRepeaterMinFrequencyVHF(Frequency freq) {
  setUInt32_le(Offset::autoRepMinVHF(), freq.inHz()/10);
}
Frequency
DMR6X2UVCodeplug::GeneralSettingsElement::autoRepeaterMaxFrequencyVHF() const {
  return Frequency::fromHz(getUInt32_le(Offset::autoRepMaxVHF())*10);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setAutoRepeaterMaxFrequencyVHF(Frequency freq) {
  setUInt32_le(Offset::autoRepMaxVHF(), freq.inHz()/10);
}

Frequency
DMR6X2UVCodeplug::GeneralSettingsElement::autoRepeaterMinFrequencyUHF() const {
  return Frequency::fromHz(getUInt32_le(Offset::autoRepMinUHF())*10);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setAutoRepeaterMinFrequencyUHF(Frequency freq) {
  setUInt32_le(Offset::autoRepMinUHF(), freq.inHz()/10);
}
Frequency
DMR6X2UVCodeplug::GeneralSettingsElement::autoRepeaterMaxFrequencyUHF() const {
  return Frequency::fromHz(getUInt32_le(Offset::autoRepMaxUHF())*10);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setAutoRepeaterMaxFrequencyUHF(Frequency freq) {
  setUInt32_le(Offset::autoRepMaxUHF(), freq.inHz()/10);
}

AnytoneAutoRepeaterSettingsExtension::Direction
DMR6X2UVCodeplug::GeneralSettingsElement::autoRepeaterDirectionB() const {
  return (AnytoneAutoRepeaterSettingsExtension::Direction)getUInt8(Offset::autoRepeaterDirB());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setAutoRepeaterDirectionB(AnytoneAutoRepeaterSettingsExtension::Direction dir) {
  setUInt8(Offset::autoRepeaterDirB(), (uint8_t)dir);
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::fmSendIDAndContact() const {
  return 0 != getUInt8(Offset::fmSendIDAndContact());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableFMSendIDAndContact(bool enable) {
  setUInt8(Offset::fmSendIDAndContact(), enable ? 0x01 : 0x00);
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::defaultChannel() const {
  return 0x01 == getUInt8(Offset::defaultChannels());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableDefaultChannel(bool enable) {
  setUInt8(Offset::defaultChannels(), enable ? 0x01 : 0x00);
}

unsigned int
DMR6X2UVCodeplug::GeneralSettingsElement::defaultZoneIndexA() const {
  return getUInt8(Offset::defaultZoneA());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setDefaultZoneIndexA(unsigned int index) {
  setUInt8(Offset::defaultZoneA(), index);
}
bool
DMR6X2UVCodeplug::GeneralSettingsElement::defaultChannelAIsVFO() const {
  return 0xff == defaultChannelAIndex();
}
unsigned int
DMR6X2UVCodeplug::GeneralSettingsElement::defaultChannelAIndex() const {
  return getUInt8(Offset::defaultChannelA());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setDefaultChannelAIndex(unsigned int index) {
  return setUInt8(Offset::defaultChannelA(), index);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setDefaultChannelAToVFO() {
  setDefaultChannelAIndex(0xff);
}

unsigned int
DMR6X2UVCodeplug::GeneralSettingsElement::defaultZoneIndexB() const {
  return getUInt8(Offset::defaultZoneB());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setDefaultZoneIndexB(unsigned int index) {
  setUInt8(Offset::defaultZoneB(), index);
}
bool
DMR6X2UVCodeplug::GeneralSettingsElement::defaultChannelBIsVFO() const {
  return 0xff == defaultChannelBIndex();
}
unsigned int
DMR6X2UVCodeplug::GeneralSettingsElement::defaultChannelBIndex() const {
  return getUInt8(Offset::defaultChannelB());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setDefaultChannelBIndex(unsigned int index) {
  return setUInt8(Offset::defaultChannelB(), index);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setDefaultChannelBToVFO() {
  setDefaultChannelBIndex(0xff);
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::keepLastCaller() const {
  return getUInt8(Offset::keepLastCaller());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableKeepLastCaller(bool enable) {
  setUInt8(Offset::keepLastCaller(), (enable ? 0x01 : 0x00));
}

Interval
DMR6X2UVCodeplug::GeneralSettingsElement::rxBacklightDuration() const {
  return Interval::fromSeconds(getUInt8(Offset::rxBacklightDuration()));
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setRXBacklightDuration(Interval dur) {
  unsigned int secs = std::max(30ULL, dur.seconds());
  setUInt8(Offset::rxBacklightDuration(), secs);
}

AnytoneDisplaySettingsExtension::Color
DMR6X2UVCodeplug::GeneralSettingsElement::standbyBackgroundColor() const {
  return BackgroundColor::decode(getUInt8(Offset::standbyBackground()));
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setStandbyBackgroundColor(AnytoneDisplaySettingsExtension::Color color) {
  setUInt8(Offset::standbyBackground(), BackgroundColor::encode(color));
}

unsigned int
DMR6X2UVCodeplug::GeneralSettingsElement::manualDialedGroupCallHangTime() const {
  return getUInt8(Offset::manGrpCallHangTime());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setManualDialedGroupCallHangTime(unsigned int dur) {
  setUInt8(Offset::manGrpCallHangTime(), dur);
}
unsigned int
DMR6X2UVCodeplug::GeneralSettingsElement::manualDialedPrivateCallHangTime() const {
  return getUInt8(Offset::manPrvCallHangTime());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setManualDialedPrivateCallHangTime(unsigned int dur) {
  setUInt8(Offset::manPrvCallHangTime(), dur);
}


bool
DMR6X2UVCodeplug::GeneralSettingsElement::fromConfig(const Flags &flags, Context &ctx)
{
  if (! D868UVCodeplug::GeneralSettingsElement::fromConfig(flags, ctx))
    return false;

  // apply DMR-6X2UV specific settings.
  AnytoneSettingsExtension *ext = ctx.config()->settings()->anytoneExtension();
  if (nullptr == ext)
    return true;

  // Encode boot settings
  if (ext->bootSettings()->priorityZoneA()->isNull())
    setPriorityZoneAIndex(0xff);
  else
    setPriorityZoneAIndex(ctx.index(ext->bootSettings()->priorityZoneA()->as<Zone>()));
  if (ext->bootSettings()->priorityZoneB()->isNull())
    setPriorityZoneBIndex(0xff);
  else
    setPriorityZoneBIndex(ctx.index(ext->bootSettings()->priorityZoneB()->as<Zone>()));

  // Encode key settings
  enableKnobLock(ext->keySettings()->knobLockEnabled());
  enableKeypadLock(ext->keySettings()->keypadLockEnabled());
  enableSidekeysLock(ext->keySettings()->sideKeysLockEnabled());
  enableKeyLockForced(ext->keySettings()->forcedKeyLockEnabled());

  // Encode tone settings
  setKeyToneLevel(ext->toneSettings()->keyToneLevel());

  // Encode display settings
  setCallDisplayColor(ext->displaySettings()->callColor());
  setStandbyBackgroundColor(ext->displaySettings()->standbyBackgroundColor());
  setLanguage(ext->displaySettings()->language());
  enableShowCurrentContact(ext->displaySettings()->showContact());
  enableShowLastHeard(ext->displaySettings()->showLastHeardEnabled());
  setRXBacklightDuration(ext->displaySettings()->backlightDurationRX());

  // Encode auto-repeater settings
  setAutoRepeaterDirectionB(ext->autoRepeaterSettings()->directionB());
  setAutoRepeaterMinFrequencyVHF(ext->autoRepeaterSettings()->vhfMin());
  setAutoRepeaterMaxFrequencyVHF(ext->autoRepeaterSettings()->vhfMax());
  setAutoRepeaterMinFrequencyUHF(ext->autoRepeaterSettings()->uhfMin());
  setAutoRepeaterMaxFrequencyUHF(ext->autoRepeaterSettings()->uhfMax());

  // Encode DMR settings
  setGroupCallHangTime(ext->dmrSettings()->groupCallHangTime());
  setPrivateCallHangTime(ext->dmrSettings()->privateCallHangTime());
  setPreWaveDelay(ext->dmrSettings()->preWaveDelay());
  setWakeHeadPeriod(ext->dmrSettings()->wakeHeadPeriod());
  enableFilterOwnID(ext->dmrSettings()->filterOwnIDEnabled());
  setMonitorSlotMatch(ext->dmrSettings()->monitorSlotMatch());
  enableMonitorColorCodeMatch(ext->dmrSettings()->monitorColorCodeMatchEnabled());
  enableMonitorIDMatch(ext->dmrSettings()->monitorIDMatchEnabled());
  enableMonitorTimeSlotHold(ext->dmrSettings()->monitorTimeSlotHoldEnabled());
  setSMSFormat(ext->dmrSettings()->smsFormat());

  // Encode GPS settings.
  enableGPSUnitsImperial(AnytoneGPSSettingsExtension::Units::Archaic == ext->gpsSettings()->units());
  setGPSTimeZone(ext->gpsSettings()->timeZone());
  enableGPSMessage(ext->gpsSettings()->positionReportingEnabled());
  setGPSUpdatePeriod(ext->gpsSettings()->updatePeriod());

  // Encode other settings
  enableKeepLastCaller(ext->keepLastCallerEnabled());
  setVFOFrequencyStep(ext->vfoStep());
  setSTEType(ext->steType());
  setSTEFrequency(ext->steFrequency());
  setTBSTFrequency(ext->tbstFrequency());
  enableProMode(ext->proModeEnabled());
  enableMaintainCallChannel(ext->maintainCallChannelEnabled());

  // Apply simplex repeater settings
  enableSimplexRepeater(ext->simplexRepeaterSettings()->enabled());
  enableMonitorSimplexRepeater(ext->simplexRepeaterSettings()->monitorEnabled());
  setSimplexRepeaterTimeslot(ext->simplexRepeaterSettings()->timeSlot());

  return true;
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::updateConfig(Context &ctx) {
  if (! D868UVCodeplug::GeneralSettingsElement::updateConfig(ctx))
    return false;

  // Get or add settings extension
  AnytoneSettingsExtension *ext = nullptr;
  if (ctx.config()->settings()->anytoneExtension()) {
    ext = ctx.config()->settings()->anytoneExtension();
  } else {
    ext = new AnytoneSettingsExtension();
    ctx.config()->settings()->setAnytoneExtension(ext);
  }

  // Decode key settings
  ext->keySettings()->enableKnobLock(this->knobLock());
  ext->keySettings()->enableKeypadLock(this->keypadLock());
  ext->keySettings()->enableSideKeysLock(this->sidekeysLock());
  ext->keySettings()->enableForcedKeyLock(this->keyLockForced());

  // Decode tone settings
  ext->toneSettings()->setKeyToneLevel(keyToneLevel());

  // Decode display settings
  ext->displaySettings()->setCallColor(this->callDisplayColor());
  ext->displaySettings()->setLanguage(this->language());
  ext->displaySettings()->enableShowContact(this->showCurrentContact());
  ext->displaySettings()->enableShowLastHeard(this->showLastHeard());
  ext->displaySettings()->setBacklightDurationRX(this->rxBacklightDuration());
  ext->displaySettings()->setStandbyBackgroundColor(this->standbyBackgroundColor());

  // Decode auto-repeater settings
  ext->autoRepeaterSettings()->setDirectionB(autoRepeaterDirectionB());
  ext->autoRepeaterSettings()->setVHFMin(this->autoRepeaterMinFrequencyVHF());
  ext->autoRepeaterSettings()->setVHFMax(this->autoRepeaterMaxFrequencyVHF());
  ext->autoRepeaterSettings()->setUHFMin(this->autoRepeaterMinFrequencyUHF());
  ext->autoRepeaterSettings()->setUHFMax(this->autoRepeaterMaxFrequencyUHF());

  // Encode dmr settings
  ext->dmrSettings()->setGroupCallHangTime(this->groupCallHangTime());
  ext->dmrSettings()->setPrivateCallHangTime(this->privateCallHangTime());
  ext->dmrSettings()->setPreWaveDelay(this->preWaveDelay());
  ext->dmrSettings()->setWakeHeadPeriod(this->wakeHeadPeriod());
  ext->dmrSettings()->enableFilterOwnID(this->filterOwnID());
  ext->dmrSettings()->setMonitorSlotMatch(this->monitorSlotMatch());
  ext->dmrSettings()->enableMonitorColorCodeMatch(this->monitorColorCodeMatch());
  ext->dmrSettings()->enableMonitorIDMatch(this->monitorIDMatch());
  ext->dmrSettings()->enableMonitorTimeSlotHold(this->monitorTimeSlotHold());
  ext->dmrSettings()->setSMSFormat(this->smsFormat());

  // Encode GPS settings
  ext->gpsSettings()->setUnits(this->gpsUnitsImperial() ? AnytoneGPSSettingsExtension::Units::Archaic :
                                                          AnytoneGPSSettingsExtension::Units::Metric);
  ext->gpsSettings()->setTimeZone(this->gpsTimeZone());
  ext->gpsSettings()->enablePositionReporting(this->gpsMessageEnabled());
  ext->gpsSettings()->setUpdatePeriod(this->gpsUpdatePeriod());

  // Decode other settings
  ext->enableKeepLastCaller(this->keepLastCaller());
  ext->setVFOStep(this->vfoFrequencyStep());
  ext->setSTEType(this->steType());
  ext->setSTEFrequency(this->steFrequency());
  ext->setTBSTFrequency(this->tbstFrequency());
  ext->enableProMode(this->proMode());
  ext->enableMaintainCallChannel(this->maintainCallChannel());

  // Decode simplex-repeater feature.
  ext->simplexRepeaterSettings()->enable(this->simplexRepeaterEnabled());
  ext->simplexRepeaterSettings()->enableMonitor(this->monitorSimplexRepeaterEnabled());
  ext->simplexRepeaterSettings()->setTimeSlot(this->simplexRepeaterTimeslot());

  return true;
}


/* ********************************************************************************************* *
 * Implementation of DMR6X2UVCodeplug::ExtendedSettingsElement
 * ********************************************************************************************* */
DMR6X2UVCodeplug::ExtendedSettingsElement::ExtendedSettingsElement(uint8_t *ptr, unsigned size)
  : AnytoneCodeplug::ExtendedSettingsElement(ptr, size)
{
  // pass...
}

DMR6X2UVCodeplug::ExtendedSettingsElement::ExtendedSettingsElement(uint8_t *ptr)
  : AnytoneCodeplug::ExtendedSettingsElement(ptr, ExtendedSettingsElement::size())
{
  // pass...
}

void
DMR6X2UVCodeplug::ExtendedSettingsElement::clear() {
  Codeplug::Element::clear();
}

bool
DMR6X2UVCodeplug::ExtendedSettingsElement::sendTalkerAlias() const {
  return 0x01 == getUInt8(Offset::sendTalkerAlias());
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::enableSendTalkerAlias(bool enable) {
  setUInt8(Offset::sendTalkerAlias(), enable ? 0x01 : 0x00);
}

AnytoneDMRSettingsExtension::TalkerAliasSource
DMR6X2UVCodeplug::ExtendedSettingsElement::talkerAliasSource() const {
  return (AnytoneDMRSettingsExtension::TalkerAliasSource) getUInt8(Offset::talkerAliasDisplay());
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::setTalkerAliasSource(AnytoneDMRSettingsExtension::TalkerAliasSource source) {
  setUInt8(Offset::talkerAliasDisplay(), (uint8_t)source);
}

AnytoneDMRSettingsExtension::TalkerAliasEncoding
DMR6X2UVCodeplug::ExtendedSettingsElement::talkerAliasEncoding() const {
  return (AnytoneDMRSettingsExtension::TalkerAliasEncoding) getUInt8(Offset::talkerAliasEncoding());
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::setTalkerAliasEncoding(AnytoneDMRSettingsExtension::TalkerAliasEncoding encoding) {
  setUInt8(Offset::talkerAliasEncoding(), (uint8_t)encoding);
}

AnytoneDisplaySettingsExtension::Color
DMR6X2UVCodeplug::ExtendedSettingsElement::fontColor() const {
  return FontColor::decode(getUInt8(Offset::fontColor()));
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::setFontColor(AnytoneDisplaySettingsExtension::Color color) {
  setUInt8(Offset::fontColor(), FontColor::encode(color));
}

bool
DMR6X2UVCodeplug::ExtendedSettingsElement::customChannelBackgroundEnabled() const {
  return 0x01 == getUInt8(Offset::customChannelBackground());
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::enableCustomChannelBackground(bool enable) {
  setUInt8(Offset::customChannelBackground(), enable ? 0x01 : 0x00);
}

unsigned int
DMR6X2UVCodeplug::ExtendedSettingsElement::defaultRoamingZoneIndex() const {
  return getUInt8(Offset::defaultRoamingZone());
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::setDefaultRoamingZoneIndex(unsigned int index) {
  setUInt8(Offset::defaultRoamingZone(), index);
}
bool
DMR6X2UVCodeplug::ExtendedSettingsElement::autoRoamingEnabled() const {
  return 0x01 == getUInt8(Offset::roaming());
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::enableAutoRoaming(bool enable) {
  setUInt8(Offset::roaming(), enable ? 0x01 : 0x00);
}
bool
DMR6X2UVCodeplug::ExtendedSettingsElement::repeaterRangeCheckEnabled() const {
  return 0x01 == getUInt8(Offset::repRangeCheck());
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::enableRepeaterRangeCheck(bool enable) {
  setUInt8(Offset::repRangeCheck(), enable ? 0x01 : 0x00);
}
AnytoneRoamingSettingsExtension::OutOfRangeAlert
DMR6X2UVCodeplug::ExtendedSettingsElement::repeaterOutOfRangeAlert() const {
  return (AnytoneRoamingSettingsExtension::OutOfRangeAlert) getUInt8(Offset::repRangeAlert());
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::setRepeaterOutOfRangeAlert(AnytoneRoamingSettingsExtension::OutOfRangeAlert alert) {
  setUInt8(Offset::repRangeAlert(), (uint8_t)alert);
}
unsigned int
DMR6X2UVCodeplug::ExtendedSettingsElement::repeaterCheckNumNotifications() const {
  return getUInt8(Offset::repRangeReminder())+1;
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::setRepeaterCheckNumNotifications(unsigned int n) {
  n = Limit::repRangeReminder().map(n);
  setUInt8(Offset::repRangeReminder(), n-1);
}
Interval
DMR6X2UVCodeplug::ExtendedSettingsElement::repeaterRangeCheckInterval() const {
  return Interval::fromSeconds((1+getUInt8(Offset::rangeCheckInterval()))*5);
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::setRepeaterRangeCheckInterval(Interval intv) {
  intv = Limit::rangeCheckInterval().map(intv);
  setUInt8(Offset::rangeCheckInterval(), intv.seconds()/5-1);
}
unsigned int
DMR6X2UVCodeplug::ExtendedSettingsElement::repeaterRangeCheckCount() const {
  return getUInt8(Offset::rangeCheckCount())+3;
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::setRepeaterRangeCheckCount(unsigned int n) {
  n = Limit::repeaterReconnections().map(n);
  setUInt8(Offset::rangeCheckCount(), n-3);
}

AnytoneRoamingSettingsExtension::RoamStart
DMR6X2UVCodeplug::ExtendedSettingsElement::roamingStartCondition() const {
  return (AnytoneRoamingSettingsExtension::RoamStart) getUInt8(Offset::roamStartCondition());
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::setRoamingStartCondition(AnytoneRoamingSettingsExtension::RoamStart cond) {
  setUInt8(Offset::roamStartCondition(), (uint8_t)cond);
}

Interval
DMR6X2UVCodeplug::ExtendedSettingsElement::autoRoamPeriod() const {
  return Interval::fromMinutes(getUInt8(Offset::autoRoamPeriod()) + 1);
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::setAutoRoamPeriod(Interval minutes) {
  minutes = Limit::autoRoamingInterval().map(minutes);
  setUInt8(Offset::autoRoamPeriod(), minutes.minutes()-1);
}

Interval
DMR6X2UVCodeplug::ExtendedSettingsElement::autoRoamDelay() const {
  return Interval::fromSeconds(getUInt8(Offset::autoRoamDelay()));
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::setAutoRoamDelay(Interval sec) {
  sec = Limit::autoRoamDelay().map(sec);
  setUInt8(Offset::autoRoamDelay(), sec.seconds());
}

AnytoneRoamingSettingsExtension::RoamStart
DMR6X2UVCodeplug::ExtendedSettingsElement::roamingReturnCondition() const {
  return (AnytoneRoamingSettingsExtension::RoamStart) getUInt8(Offset::roamReturnCondition());
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::setRoamingReturnCondition(AnytoneRoamingSettingsExtension::RoamStart cond) {
  setUInt8(Offset::roamReturnCondition(), (uint8_t)cond);
}

Interval
DMR6X2UVCodeplug::ExtendedSettingsElement::muteTimer() const {
  return Interval::fromMinutes(getUInt8(Offset::muteDelay()) + 1);
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::setMuteTimer(Interval minutes) {
  minutes = Limit::muteTimer().map(minutes);
  setUInt8(Offset::muteDelay(), minutes.minutes()-1);
}

AnytoneDMRSettingsExtension::EncryptionType
DMR6X2UVCodeplug::ExtendedSettingsElement::encryptionType() const {

  return (0 == getUInt8(0x0011)) ? AnytoneDMRSettingsExtension::EncryptionType::DMR :
                                   AnytoneDMRSettingsExtension::EncryptionType::AES;
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::setEncryptionType(AnytoneDMRSettingsExtension::EncryptionType type) {
  switch (type) {
  case AnytoneDMRSettingsExtension::EncryptionType::DMR: setUInt8(Offset::encryptionType(), 0x00); break;
  case AnytoneDMRSettingsExtension::EncryptionType::AES: setUInt8(Offset::encryptionType(), 0x01); break;
  }
}

AnytoneDisplaySettingsExtension::Color
DMR6X2UVCodeplug::ExtendedSettingsElement::zoneANameColor() const {
  return Color::decode(getUInt8(Offset::zoneANameColor()));
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::setZoneANameColor(AnytoneDisplaySettingsExtension::Color color) {
  setUInt8(Offset::zoneANameColor(), Color::encode(color));
}
AnytoneDisplaySettingsExtension::Color
DMR6X2UVCodeplug::ExtendedSettingsElement::zoneBNameColor() const {
  return Color::decode(getUInt8(Offset::zoneBNameColor()));
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::setZoneBNameColor(AnytoneDisplaySettingsExtension::Color color) {
  setUInt8(Offset::zoneBNameColor(), Color::encode(color));
}

AnytoneDisplaySettingsExtension::Color
DMR6X2UVCodeplug::ExtendedSettingsElement::channelANameColor() const {
  return Color::decode(getUInt8(Offset::channelANameColor()));
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::setChannelANameColor(AnytoneDisplaySettingsExtension::Color color) {
  setUInt8(Offset::channelANameColor(), Color::encode(color));
}
AnytoneDisplaySettingsExtension::Color
DMR6X2UVCodeplug::ExtendedSettingsElement::channelBNameColor() const {
  return Color::decode(getUInt8(Offset::channelBNameColor()));
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::setChannelBNameColor(AnytoneDisplaySettingsExtension::Color color) {
  setUInt8(Offset::channelBNameColor(), Color::encode(color));
}

bool
DMR6X2UVCodeplug::ExtendedSettingsElement::fromConfig(const Flags &flags, Context &ctx, const ErrorStack &err)
{
  if (! AnytoneCodeplug::ExtendedSettingsElement::fromConfig(flags, ctx, err)) {
    errMsg(err) << "Cannot encode extended settings for DMR-6X2UV codeplug.";
    return false;
  }

  // Encode device specific settings
  AnytoneSettingsExtension *ext = ctx.config()->settings()->anytoneExtension();
  if (nullptr == ext)
    return true;

  // Encode audio settings
  setMuteTimer(ext->audioSettings()->muteDelay());

  // Encode display settings
  setChannelANameColor(ext->displaySettings()->channelNameColor());
  setFontColor(ext->displaySettings()->standbyTextColor());
  enableCustomChannelBackground(ext->displaySettings()->customChannelBackground());

  // Encode DMR settings
  setEncryptionType(ext->dmrSettings()->encryption());

  // Encode ranging/roaming settings.
  enableAutoRoaming(ext->roamingSettings()->autoRoam());
  setAutoRoamPeriod(ext->roamingSettings()->autoRoamPeriod());
  setAutoRoamDelay(ext->roamingSettings()->autoRoamDelay());
  enableRepeaterRangeCheck(ext->roamingSettings()->repeaterRangeCheckEnabled());
  setRepeaterRangeCheckInterval(ext->roamingSettings()->repeaterCheckInterval());
  setRepeaterRangeCheckCount(ext->roamingSettings()->repeaterRangeCheckCount());
  setRepeaterOutOfRangeAlert(ext->roamingSettings()->outOfRangeAlert());
  setRoamingStartCondition(ext->roamingSettings()->roamingStartCondition());
  setRoamingReturnCondition(ext->roamingSettings()->roamingReturnCondition());
  setRepeaterCheckNumNotifications(ext->roamingSettings()->notificationCount());
  if (! ext->roamingSettings()->defaultZone()->isNull())
    setDefaultRoamingZoneIndex(
          ctx.index(ext->roamingSettings()->defaultZone()->as<RoamingZone>()));

  return true;
}

bool
DMR6X2UVCodeplug::ExtendedSettingsElement::updateConfig(Context &ctx, const ErrorStack &err) {
  if (! AnytoneCodeplug::ExtendedSettingsElement::updateConfig(ctx, err)) {
    errMsg(err) << "Cannot decode extended settings for DMR-6X2UV codeplug";
    return false;
  }

  AnytoneSettingsExtension *ext = ctx.config()->settings()->anytoneExtension();
  if (nullptr == ext) {
    ext = new AnytoneSettingsExtension();
    ctx.config()->settings()->setAnytoneExtension(ext);
  }

  // Decode audio settings
  ext->audioSettings()->setMuteDelay(this->muteTimer());

  // Decode display settings
  ext->displaySettings()->setChannelNameColor(this->channelANameColor());
  ext->displaySettings()->setStandbyTextColor(this->fontColor());
  ext->displaySettings()->enableCustomChannelBackground(this->customChannelBackgroundEnabled());

  // Decode DMR settings
  ext->dmrSettings()->setEncryption(this->encryptionType());

  // Decode ranging/roaming settings
  ext->roamingSettings()->enableAutoRoam(this->autoRoamingEnabled());
  ext->roamingSettings()->setAutoRoamPeriod(this->autoRoamPeriod());
  ext->roamingSettings()->setAutoRoamDelay(this->autoRoamDelay());
  ext->roamingSettings()->enableRepeaterRangeCheck(this->repeaterRangeCheckEnabled());
  ext->roamingSettings()->setRepeaterCheckInterval(this->repeaterRangeCheckInterval());
  ext->roamingSettings()->setRepeaterRangeCheckCount(this->repeaterRangeCheckCount());
  ext->roamingSettings()->setOutOfRangeAlert(this->repeaterOutOfRangeAlert());
  ext->roamingSettings()->setRoamingStartCondition(this->roamingStartCondition());
  ext->roamingSettings()->setRoamingReturnCondition(this->roamingReturnCondition());
  ext->roamingSettings()->setNotificationCount(this->repeaterCheckNumNotifications());

  return true;
}

bool
DMR6X2UVCodeplug::ExtendedSettingsElement::linkConfig(Context &ctx, const ErrorStack &err) {
  if (! AnytoneCodeplug::ExtendedSettingsElement::linkConfig(ctx, err)) {
    errMsg(err) << "Cannot link extended settings for DMR-6X2UV codeplug";
    return false;
  }

  AnytoneSettingsExtension *ext = ctx.config()->settings()->anytoneExtension();
  if (nullptr == ext)
    return false;

  if (ctx.has<RoamingZone>(defaultRoamingZoneIndex())) {
    if (! ctx.has<RoamingZone>(this->defaultRoamingZoneIndex())) {
      errMsg(err) << "Cannot link roaming zone, index " << this->defaultRoamingZoneIndex()
                  << " not defined.";
      return false;
    }
    ext->roamingSettings()->defaultZone()->set(ctx.get<RoamingZone>(this->defaultRoamingZoneIndex()));
  }

  return true;
}



/* ********************************************************************************************* *
 * Implementation of DMR6X2UVCodeplug::ChannelElement
 * ********************************************************************************************* */
DMR6X2UVCodeplug::ChannelElement::ChannelElement(uint8_t *ptr, unsigned size)
  : AnytoneCodeplug::ChannelElement(ptr, size)
{
  // pass...
}

DMR6X2UVCodeplug::ChannelElement::ChannelElement(uint8_t *ptr)
  : AnytoneCodeplug::ChannelElement(ptr)
{
  // pass...
}

bool
DMR6X2UVCodeplug::ChannelElement::hasScanListIndex() const {
  return hasScanListIndex(0);
}
unsigned
DMR6X2UVCodeplug::ChannelElement::scanListIndex() const {
  return scanListIndex(0);
}
void
DMR6X2UVCodeplug::ChannelElement::setScanListIndex(unsigned idx) {
  setScanListIndex(0, idx);
}
void
DMR6X2UVCodeplug::ChannelElement::clearScanListIndex() {
  clearScanListIndex(0);
}

bool
DMR6X2UVCodeplug::ChannelElement::hasScanListIndex(unsigned int n) const {
  return 0xff != scanListIndex(n);
}
unsigned int
DMR6X2UVCodeplug::ChannelElement::scanListIndex(unsigned int n) const {
  if (n >= Limit::scanListIndices())
    return 0xff;
  return getUInt8(Offset::scanListIndices() + n*Offset::betweenScanListIndices());
}
void
DMR6X2UVCodeplug::ChannelElement::setScanListIndex(unsigned int n, unsigned idx) {
  if (n >= Limit::scanListIndices())
    return;
  setUInt8(Offset::scanListIndices() + n*Offset::betweenScanListIndices(), idx);
}
void
DMR6X2UVCodeplug::ChannelElement::clearScanListIndex(unsigned int n) {
  setScanListIndex(n, 0xff);
}

bool
DMR6X2UVCodeplug::ChannelElement::roamingEnabled() const {
  // inverted
  return ! getBit(Offset::roaming());
}
void
DMR6X2UVCodeplug::ChannelElement::enableRoaming(bool enable) {
  // inverted
  setBit(Offset::roaming(), !enable);
}

bool
DMR6X2UVCodeplug::ChannelElement::ranging() const {
  return getBit(Offset::ranging());
}
void
DMR6X2UVCodeplug::ChannelElement::enableRanging(bool enable) {
  return setBit(Offset::ranging(), enable);
}

unsigned int
DMR6X2UVCodeplug::ChannelElement::dmrAPRSChannelIndex() const {
  return getUInt8(Offset::dmrAPRSChannelIndex());
}
void
DMR6X2UVCodeplug::ChannelElement::setDMRAPRSChannelIndex(unsigned int idx) {
  setUInt8(Offset::dmrAPRSChannelIndex(), std::min(APRSSettingsElement::Limit::dmrSystems(), idx));
}

bool
DMR6X2UVCodeplug::ChannelElement::dmrAPRSRXEnabled() const {
  return getBit(Offset::dmrAPRSRXEnable());
}
void
DMR6X2UVCodeplug::ChannelElement::enableDMRARPSRX(bool enable) {
  setBit(Offset::dmrAPRSRXEnable(), enable);
}

bool
DMR6X2UVCodeplug::ChannelElement::dmrAPRSPTTEnabled() const {
  return getBit(Offset::dmrAPRSPTTEnable());
}
void
DMR6X2UVCodeplug::ChannelElement::enableDMRAPRSPTT(bool enable) {
  setBit(Offset::dmrAPRSPTTEnable(), enable);
}

DMR6X2UVCodeplug::ChannelElement::FMAPRSPTTMode
DMR6X2UVCodeplug::ChannelElement::fmAPRSPTTMode() const {
  return (FMAPRSPTTMode)getUInt2(Offset::fmAPRSPTTMode());
}
void
DMR6X2UVCodeplug::ChannelElement::setFMAPRSPTTMode(FMAPRSPTTMode mode) {
  setUInt2(Offset::fmAPRSPTTMode(), (unsigned int)mode);
}

DMR6X2UVCodeplug::ChannelElement::APRSType
DMR6X2UVCodeplug::ChannelElement::aprsType() const {
  return (APRSType) getUInt2(Offset::aprsType());
}
void
DMR6X2UVCodeplug::ChannelElement::setAPRSType(APRSType aprstype) {
  setUInt2(Offset::aprsType(), (unsigned int)aprstype);
}

bool
DMR6X2UVCodeplug::ChannelElement::fromChannelObj(const Channel *c, Context &ctx) {
  if (! AnytoneCodeplug::ChannelElement::fromChannelObj(c, ctx))
    return false;

  if (const FMChannel *fm = c->as<FMChannel>()) {
    if (fm->aprsSystem()) {
      setAPRSType(APRSType::FM);
      if (auto ext = fm->anytoneChannelExtension()) {
        switch (ext->aprsPTT()) {
        case AnytoneChannelExtension::APRSPTT::Off: setFMAPRSPTTMode(FMAPRSPTTMode::Off); break;
        case AnytoneChannelExtension::APRSPTT::Start: setFMAPRSPTTMode(FMAPRSPTTMode::Start); break;
        case AnytoneChannelExtension::APRSPTT::End: setFMAPRSPTTMode(FMAPRSPTTMode::End); break;
        }
      }
    }
  } else if (const DMRChannel *dmr = c->as<DMRChannel>()) {
    if (dmr->aprs()) {
      if (dmr->aprs()->is<APRSSystem>()) {
        setAPRSType(APRSType::FM);
        if (auto ext = dmr->anytoneChannelExtension()) {
          switch (ext->aprsPTT()) {
          case AnytoneChannelExtension::APRSPTT::Off: setFMAPRSPTTMode(FMAPRSPTTMode::Off); break;
          case AnytoneChannelExtension::APRSPTT::Start: setFMAPRSPTTMode(FMAPRSPTTMode::Start); break;
          case AnytoneChannelExtension::APRSPTT::End: setFMAPRSPTTMode(FMAPRSPTTMode::End); break;
          }
        }
      } else if (GPSSystem *sys = dmr->aprs()->as<GPSSystem>()){
        if (0 <= ctx.index(sys)) {
          setAPRSType(APRSType::DMR);
          setDMRAPRSChannelIndex(ctx.index(sys));
          if (auto ext = dmr->anytoneChannelExtension())
            enableDMRAPRSPTT(AnytoneChannelExtension::APRSPTT::Off != ext->aprsPTT());
        }
      }
    }
  }

  return true;
}

bool
DMR6X2UVCodeplug::ChannelElement::linkChannelObj(Channel *c, Context &ctx) const {
  if (! AnytoneCodeplug::ChannelElement::linkChannelObj(c, ctx))
    return false;
  if (FMChannel *fm = c->as<FMChannel>()) {
    auto ext = fm->anytoneChannelExtension();
    if (nullptr == ext)
      fm->setAnytoneChannelExtension(ext = new AnytoneFMChannelExtension());
    if ((APRSType::FM == aprsType()) && ctx.count<APRSSystem>()) {
      switch (fmAPRSPTTMode()) {
      case FMAPRSPTTMode::Off: ext->setAPRSPTT(AnytoneChannelExtension::APRSPTT::Off); break;
      case FMAPRSPTTMode::Start: ext->setAPRSPTT(AnytoneChannelExtension::APRSPTT::Start); break;
      case FMAPRSPTTMode::End: ext->setAPRSPTT(AnytoneChannelExtension::APRSPTT::End); break;
      }
      fm->setAPRSSystem(ctx.get<APRSSystem>(0));
    }
  } else if (DMRChannel *dmr = c->as<DMRChannel>()) {
    auto ext = dmr->anytoneChannelExtension();
    if (nullptr == ext)
      dmr->setAnytoneChannelExtension(ext = new AnytoneDMRChannelExtension());
    if ((APRSType::FM == aprsType()) && ctx.count<APRSSystem>()) {
      switch (fmAPRSPTTMode()) {
      case FMAPRSPTTMode::Off: ext->setAPRSPTT(AnytoneChannelExtension::APRSPTT::Off); break;
      case FMAPRSPTTMode::Start: ext->setAPRSPTT(AnytoneChannelExtension::APRSPTT::Start); break;
      case FMAPRSPTTMode::End: ext->setAPRSPTT(AnytoneChannelExtension::APRSPTT::End); break;
      }
      dmr->setAPRSObj(ctx.get<APRSSystem>(0));
    } else if ((APRSType::DMR == aprsType()) && ctx.has<GPSSystem>(dmrAPRSChannelIndex())) {
      ext->setAPRSPTT(dmrAPRSPTTEnabled() ? AnytoneChannelExtension::APRSPTT::Start : AnytoneChannelExtension::APRSPTT::Off);
      dmr->setAPRSObj(ctx.get<GPSSystem>(dmrAPRSChannelIndex()));
    }
  }

  return true;
}


/* ******************************************************************************************** *
 * Implementation of DMR6X2UVCodeplug::AnalogAPRSSettingsElement
 * ******************************************************************************************** */
DMR6X2UVCodeplug::APRSSettingsElement::APRSSettingsElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

DMR6X2UVCodeplug::APRSSettingsElement::APRSSettingsElement(uint8_t *ptr)
  : Element(ptr, APRSSettingsElement::size())
{
  // pass...
}

void
DMR6X2UVCodeplug::APRSSettingsElement::clear() {
  memset(_data, 0x00, _size);
  setUInt8(0x0000, 0xff);
  setFMTXDelay(Interval::fromMilliseconds(60));
  setUInt8(0x003d, 0x01); setUInt8(0x003e, 0x03); setUInt8(0x003f, 0xff);
}

bool
DMR6X2UVCodeplug::APRSSettingsElement::isValid() const {
  if (! Codeplug::Element::isValid())
    return false;
  return (! destination().simplified().isEmpty())
      && (! source().simplified().isEmpty());
}

Frequency
DMR6X2UVCodeplug::APRSSettingsElement::fmFrequency() const {
  return Frequency::fromHz(getBCD8_be(Offset::fmFrequency())*10);
}
void
DMR6X2UVCodeplug::APRSSettingsElement::setFMFrequency(Frequency f) {
  setBCD8_be(Offset::fmFrequency(), f.inHz()/10);
}

Interval DMR6X2UVCodeplug::APRSSettingsElement::fmTXDelay() const {
  return Interval::fromMilliseconds(((unsigned)getUInt8(Offset::fmTXDelay())*20));
}
void
DMR6X2UVCodeplug::APRSSettingsElement::setFMTXDelay(const Interval intv) {
  setUInt8(Offset::fmTXDelay(), intv.milliseconds()/20);
}

SelectiveCall
DMR6X2UVCodeplug::APRSSettingsElement::txTone() const {
  if ((uint8_t)SignalingType::Off ==getUInt8(Offset::fmSigType())) { // none
    return SelectiveCall();
  } else if ((uint8_t)SignalingType::CTCSS == getUInt8(Offset::fmSigType())) { // CTCSS
    return CTCSS::decode(getUInt8(Offset::fmCTCSS()));
  } else if ((uint8_t)SignalingType::DCS == getUInt8(Offset::fmSigType())) { // DCS
    uint16_t code = getUInt16_le(Offset::fmDCS());
    if (512 < code)
      return SelectiveCall::fromBinaryDCS(code, false);
    return SelectiveCall::fromBinaryDCS(code-512, true);
  }

  return SelectiveCall();
}
void
DMR6X2UVCodeplug::APRSSettingsElement::setTXTone(const SelectiveCall &code) {
  if (code.isInvalid()) {
    setUInt8(Offset::fmSigType(), (uint8_t)SignalingType::Off);
  } else if (code.isCTCSS()) {
    setUInt8(Offset::fmSigType(), (uint8_t)SignalingType::CTCSS);
    setUInt8(Offset::fmCTCSS(), CTCSS::encode(code));
  } else if (code.isDCS()) {
    setUInt8(Offset::fmSigType(), (uint8_t)SignalingType::DCS);
    setUInt16_le(Offset::fmDCS(), code.binCode() + (code.isInverted() ? 512 : 0));
  }
}

Interval
DMR6X2UVCodeplug::APRSSettingsElement::manualTXInterval() const {
  return Interval::fromSeconds(getUInt8(Offset::manualTXInterval()));
}
void
DMR6X2UVCodeplug::APRSSettingsElement::setManualTXInterval(Interval sec) {
  setUInt8(Offset::manualTXInterval(), sec.seconds());
}

bool
DMR6X2UVCodeplug::APRSSettingsElement::autoTX() const {
  return ! autoTXInterval().isNull();
}
Interval
DMR6X2UVCodeplug::APRSSettingsElement::autoTXInterval() const {
  return Interval::fromSeconds( ((unsigned)getUInt8(Offset::autoTXInterval()))*30 );
}
void
DMR6X2UVCodeplug::APRSSettingsElement::setAutoTXInterval(Interval sec) {
  setUInt8(Offset::autoTXInterval(), sec.seconds()/30);
}
void
DMR6X2UVCodeplug::APRSSettingsElement::disableAutoTX() {
  setAutoTXInterval(Interval::fromMilliseconds(0));
}

bool
DMR6X2UVCodeplug::APRSSettingsElement::fixedLocationEnabled() const {
  return getUInt8(Offset::fixedLocation());
}
QGeoCoordinate
DMR6X2UVCodeplug::APRSSettingsElement::fixedLocation() const {
  double latitude  = getUInt8(Offset::fixedLatDeg()) + double(getUInt8(Offset::fixedLatMin()))/60
      + double(getUInt8(Offset::fixedLatSec()))/3600;
  if (getUInt8(Offset::fixedLatSouth()))
    latitude *= -1;

  double longitude = getUInt8(Offset::fixedLonDeg()) + double(getUInt8(Offset::fixedLonMin()))/60
      + double(getUInt8(Offset::fixedLonSec()))/3600;
  if (getUInt8(Offset::fixedLonWest()))
    longitude *= -1;

  return QGeoCoordinate(latitude, longitude);
}
void
DMR6X2UVCodeplug::APRSSettingsElement::setFixedLocation(QGeoCoordinate &loc) {
  double latitude = loc.latitude();
  bool south = (0 > latitude); latitude = std::abs(latitude);
  unsigned lat_deg = int(latitude); latitude -= lat_deg; latitude *= 60;
  unsigned lat_min = int(latitude); latitude -= lat_min; latitude *= 60;
  unsigned lat_sec = int(latitude);
  double longitude = loc.longitude();
  bool west = (0 > longitude); longitude = std::abs(longitude);
  unsigned lon_deg = int(longitude); longitude -= lon_deg; longitude *= 60;
  unsigned lon_min = int(longitude); longitude -= lon_min; longitude *= 60;
  unsigned lon_sec = int(longitude);
  setUInt8(Offset::fixedLatDeg(), lat_deg);
  setUInt8(Offset::fixedLatMin(), lat_min);
  setUInt8(Offset::fixedLatSec(), lat_sec);
  setUInt8(Offset::fixedLatSouth(), (south ? 0x01 : 0x00));
  setUInt8(Offset::fixedLonDeg(), lon_deg);
  setUInt8(Offset::fixedLonMin(), lon_min);
  setUInt8(Offset::fixedLonSec(), lon_sec);
  setUInt8(Offset::fixedLonWest(), (west ? 0x01 : 0x00));
  // enable fixed location.
  setUInt8(Offset::fixedLocation(), 0x01);
}
void
DMR6X2UVCodeplug::APRSSettingsElement::disableFixedLocation() {
  setUInt8(Offset::fixedLocation(), 0x00);
}

QString
DMR6X2UVCodeplug::APRSSettingsElement::destination() const {
  // Terminated/padded with space
  return readASCII(Offset::destinationCall(), 6, ' ');
}
unsigned
DMR6X2UVCodeplug::APRSSettingsElement::destinationSSID() const {
  return getUInt8(Offset::destinationSSID());
}
void
DMR6X2UVCodeplug::APRSSettingsElement::setDestination(const QString &call, unsigned ssid) {
  // Terminated/padded with space
  writeASCII(Offset::destinationCall(), call, 6, ' ');
  setUInt8(Offset::destinationSSID(), ssid);
}
QString
DMR6X2UVCodeplug::APRSSettingsElement::source() const {
  // Terminated/padded with space
  return readASCII(Offset::sourceCall(), 6, ' ');
}
unsigned
DMR6X2UVCodeplug::APRSSettingsElement::sourceSSID() const {
  return getUInt8(Offset::sourceSSID());
}
void
DMR6X2UVCodeplug::APRSSettingsElement::setSource(const QString &call, unsigned ssid) {
  // Terminated/padded with space
  writeASCII(Offset::sourceCall(), call, 6, ' ');
  setUInt8(Offset::sourceSSID(), ssid);
}

QString
DMR6X2UVCodeplug::APRSSettingsElement::path() const {
  return readASCII(Offset::path(), 20, 0x00);
}
void
DMR6X2UVCodeplug::APRSSettingsElement::setPath(const QString &path) {
  writeASCII(Offset::path(), path, 20, 0x00);
}

APRSSystem::Icon
DMR6X2UVCodeplug::APRSSettingsElement::icon() const {
  return code2aprsicon(getUInt8(Offset::symbolTable()), getUInt8(Offset::symbol()));
}
void
DMR6X2UVCodeplug::APRSSettingsElement::setIcon(APRSSystem::Icon icon) {
  setUInt8(Offset::symbolTable(), aprsicon2tablecode(icon));
  setUInt8(Offset::symbol(), aprsicon2iconcode(icon));
}

Channel::Power
DMR6X2UVCodeplug::APRSSettingsElement::power() const {
  switch (getUInt8(Offset::fmPower())) {
  case 0: return Channel::Power::Low;
  case 1: return Channel::Power::Mid;
  case 2: return Channel::Power::High;
  case 3: return Channel::Power::Max;
  }
  return Channel::Power::Low;
}
void
DMR6X2UVCodeplug::APRSSettingsElement::setPower(Channel::Power power) {
  switch (power) {
  case Channel::Power::Min:
  case Channel::Power::Low:  setUInt8(Offset::fmPower(), 0x00); break;
  case Channel::Power::Mid:  setUInt8(Offset::fmPower(), 0x01); break;
  case Channel::Power::High: setUInt8(Offset::fmPower(), 0x02); break;
  case Channel::Power::Max:  setUInt8(Offset::fmPower(), 0x03); break;
  }
}

Interval
DMR6X2UVCodeplug::APRSSettingsElement::fmPreWaveDelay() const {
  return Interval::fromMilliseconds(((unsigned)getUInt8(Offset::fmPrewaveDelay()))*10);
}
void
DMR6X2UVCodeplug::APRSSettingsElement::setFMPreWaveDelay(Interval ms) {
  setUInt8(Offset::fmPrewaveDelay(), ms.milliseconds()/10);
}

bool
DMR6X2UVCodeplug::APRSSettingsElement::dmrChannelIsSelected(unsigned n) const {
  return 0xfa2 == dmrChannelIndex(n);
}
unsigned
DMR6X2UVCodeplug::APRSSettingsElement::dmrChannelIndex(unsigned n) const {
  return getUInt16_le(Offset::dmrChannelIndices() + n*Offset::betweenDMRChannelIndices());
}
void
DMR6X2UVCodeplug::APRSSettingsElement::setDMRChannelIndex(unsigned n, unsigned idx) {
  setUInt16_le(Offset::dmrChannelIndices() + n*Offset::betweenDMRChannelIndices(), idx);
}
void
DMR6X2UVCodeplug::APRSSettingsElement::setDMRChannelSelected(unsigned n) {
  setDMRChannelIndex(n, 0xfa2);
}

unsigned
DMR6X2UVCodeplug::APRSSettingsElement::dmrDestination(unsigned n) const {
  return getBCD8_be(Offset::dmrDestinations() + n*Offset::betweenDMRDestinations());
}
void
DMR6X2UVCodeplug::APRSSettingsElement::setDMRDestination(unsigned n, unsigned idx) {
  setBCD8_be(Offset::dmrDestinations() + n*Offset::betweenDMRDestinations(), idx);
}

DMRContact::Type
DMR6X2UVCodeplug::APRSSettingsElement::dmrCallType(unsigned n) const {
  switch(getUInt8(Offset::dmrCallTypes() + n*Offset::betweenDMRCallTypes())) {
  case 0: return DMRContact::PrivateCall;
  case 1: return DMRContact::GroupCall;
  case 2: return DMRContact::AllCall;
  }
  return DMRContact::PrivateCall;
}
void
DMR6X2UVCodeplug::APRSSettingsElement::setDMRCallType(unsigned n, DMRContact::Type type) {
  switch(type) {
  case DMRContact::PrivateCall: setUInt8(Offset::dmrCallTypes() + n*Offset::betweenDMRCallTypes(), 0x00); break;
  case DMRContact::GroupCall: setUInt8(Offset::dmrCallTypes() + n*Offset::betweenDMRCallTypes(), 0x01); break;
  case DMRContact::AllCall: setUInt8(Offset::dmrCallTypes() + n*Offset::betweenDMRCallTypes(), 0x02); break;
  }
}

bool
DMR6X2UVCodeplug::APRSSettingsElement::dmrTimeSlotOverride(unsigned n) {
  return 0x00 != getUInt8(Offset::dmrTimeSlots() + n*Offset::betweenDMRTimeSlots());
}
DMRChannel::TimeSlot
DMR6X2UVCodeplug::APRSSettingsElement::dmrTimeSlot(unsigned n) const {
  switch (getUInt8(Offset::dmrTimeSlots() + n*Offset::betweenDMRTimeSlots())) {
  case 1: return DMRChannel::TimeSlot::TS1;
  case 2: return DMRChannel::TimeSlot::TS2;
  }
  return DMRChannel::TimeSlot::TS1;
}
void
DMR6X2UVCodeplug::APRSSettingsElement::setDMRTimeSlot(unsigned n, DMRChannel::TimeSlot ts) {
  switch (ts) {
  case DMRChannel::TimeSlot::TS1: setUInt8(Offset::dmrTimeSlots() + n*Offset::betweenDMRTimeSlots(), 0x01); break;
  case DMRChannel::TimeSlot::TS2: setUInt8(Offset::dmrTimeSlots() + n*Offset::betweenDMRTimeSlots(), 0x02); break;
  }
}
void
DMR6X2UVCodeplug::APRSSettingsElement::clearDMRTimeSlotOverride(unsigned n) {
  setUInt8(Offset::dmrTimeSlots() + n*Offset::betweenDMRTimeSlots(), 0);
}

bool
DMR6X2UVCodeplug::APRSSettingsElement::dmrRoaming() const {
  return getUInt8(Offset::roamingSupport());
}
void
DMR6X2UVCodeplug::APRSSettingsElement::enableDMRRoaming(bool enable) {
  setUInt8(Offset::roamingSupport(), (enable ? 0x01 : 0x00));
}

Interval
DMR6X2UVCodeplug::APRSSettingsElement::dmrPreWaveDelay() const {
  return Interval::fromMilliseconds(((unsigned)getUInt8(Offset::dmrPrewaveDelay()))*100);
}
void
DMR6X2UVCodeplug::APRSSettingsElement::setDMRPreWaveDelay(Interval ms) {
  setUInt8(Offset::dmrPrewaveDelay(), ms.milliseconds()/100);
}

bool
DMR6X2UVCodeplug::APRSSettingsElement::fromFMAPRSSystem(const APRSSystem *sys, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx)
  clear();
  if (! sys->revertChannel()) {
    errMsg(err) << "Cannot encode APRS settings: "
                << "No revert channel defined for APRS system '" << sys->name() <<"'.";
    return false;
  }
  setFMFrequency(sys->revertChannel()->txFrequency());
  setTXTone(sys->revertChannel()->txTone());
  setPower(sys->revertChannel()->power());
  setManualTXInterval(Interval::fromSeconds(sys->period()));
  setAutoTXInterval(Interval::fromSeconds(sys->period()));
  setDestination(sys->destination(), sys->destSSID());
  setSource(sys->source(), sys->srcSSID());
  setPath(sys->path());
  setIcon(sys->icon());

  AnytoneFMAPRSSettingsExtension *ext = sys->anytoneExtension();
  if (nullptr == ext)
    return true;

  setFMPreWaveDelay(ext->preWaveDelay());
  setFMTXDelay(ext->txDelay());

  return true;
}

APRSSystem *
DMR6X2UVCodeplug::APRSSettingsElement::toFMAPRSSystem() {
  APRSSystem *sys = new APRSSystem(
        tr("APRS %1").arg(destination()), nullptr,
        destination(), destinationSSID(), source(), sourceSSID(),
        path(), icon(), "", autoTXInterval().seconds());

  AnytoneFMAPRSSettingsExtension *ext = new AnytoneFMAPRSSettingsExtension();
  ext->setPreWaveDelay(fmPreWaveDelay());
  ext->setTXDelay(fmTXDelay());
  sys->setAnytoneExtension(ext);

  return sys;
}

bool
DMR6X2UVCodeplug::APRSSettingsElement::linkFMAPRSSystem(APRSSystem *sys, Context &ctx) {
  // First, try to find a matching analog channel in list
  Frequency f = fmFrequency();
  FMChannel *ch = ctx.config()->channelList()->findFMChannelByTxFreq(f);
  if (! ch) {
    // If no channel is found, create one with the settings from APRS channel:
    ch = new FMChannel();
    ch->setName("APRS Channel");
    ch->setRXFrequency(fmFrequency());
    ch->setTXFrequency(fmFrequency());
    ch->setPower(power());
    ch->setTXTone(txTone());
    logInfo() << "No matching APRS channel found for TX frequency " << fmFrequency().format()
              << ", create one as 'APRS Channel'";
    ctx.config()->channelList()->add(ch);
  }
  sys->setRevertChannel(ch);
  return true;
}

bool
DMR6X2UVCodeplug::APRSSettingsElement::fromDMRAPRSSystems(Context &ctx) {
  unsigned int n = std::min(ctx.count<GPSSystem>(), Limit::dmrSystems());
  for (unsigned int idx=0; idx<n; idx++)
    fromDMRAPRSSystemObj(idx, ctx.get<GPSSystem>(idx), ctx);
  return true;
}

bool
DMR6X2UVCodeplug::APRSSettingsElement::fromDMRAPRSSystemObj(unsigned int idx, GPSSystem *sys, Context &ctx) {
  if (sys->hasContact()) {
    setDMRDestination(idx, sys->contactObj()->number());
    setDMRCallType(idx, sys->contactObj()->type());
  }
  if (sys->hasRevertChannel() && (SelectedChannel::get() != (Channel *)sys->revertChannel())) {
    setDMRChannelIndex(idx, ctx.index(sys->revertChannel()));
    clearDMRTimeSlotOverride(idx);
  } else { // no revert channel specified or "selected channel":
    setDMRChannelSelected(idx);
  }
  return true;
}

GPSSystem *
DMR6X2UVCodeplug::APRSSettingsElement::toDMRAPRSSystemObj(int idx) const {
  if (0 == dmrDestination(idx))
    return nullptr;
  return new GPSSystem(tr("GPS Sys #%1").arg(idx+1));
}

bool
DMR6X2UVCodeplug::APRSSettingsElement::linkDMRAPRSSystem(int idx, GPSSystem *sys, Context &ctx) const {
  // Clear revert channel from GPS system
  sys->setRevertChannel(nullptr);

  // if a revert channel is defined -> link to it
  if (dmrChannelIsSelected(idx))
    sys->setRevertChannel(nullptr);
  else if (ctx.has<Channel>(dmrChannelIndex(idx)) && ctx.get<Channel>(dmrChannelIndex(idx))->is<DMRChannel>())
    sys->setRevertChannel(ctx.get<Channel>(dmrChannelIndex(idx))->as<DMRChannel>());

  // Search for a matching contact in contacts
  DMRContact *cont = ctx.config()->contacts()->findDMRContact(dmrDestination(idx));
  // If no matching contact is found, create one
  if (nullptr == cont) {
    cont = new DMRContact(dmrCallType(idx), tr("GPS #%1 Contact").arg(idx+1),
                          dmrDestination(idx), false);
    ctx.config()->contacts()->add(cont);
  }
  // link contact to GPS system.
  sys->setContactObj(cont);

  return true;
}


/* ********************************************************************************************* *
 * Implementation of DMR6X2UVCodeplug
 * ********************************************************************************************* */
DMR6X2UVCodeplug::DMR6X2UVCodeplug(const QString &label, QObject *parent)
  : D868UVCodeplug(label, parent)
{
  // pass...
}

DMR6X2UVCodeplug::DMR6X2UVCodeplug(QObject *parent)
  : D868UVCodeplug("BTECH DMR-6X2UV", parent)
{
  // pass...
}

bool
DMR6X2UVCodeplug::allocateBitmaps() {
  if (! D868UVCodeplug::allocateBitmaps())
    return false;

  // Roaming channel bitmaps
  image(0).addElement(Offset::roamingChannelBitmap(), RoamingChannelBitmapElement::size());
  // Roaming zone bitmaps
  image(0).addElement(Offset::roamingZoneBitmap(), RoamingZoneBitmapElement::size());

  return true;
}

void
DMR6X2UVCodeplug::setBitmaps(Context& ctx)
{
  // First set everything common between D868UV and D878UV codeplugs.
  D868UVCodeplug::setBitmaps(ctx);

  // Mark roaming zones
  RoamingZoneBitmapElement roaming_zone_bitmap(data(Offset::roamingZoneBitmap()));
  unsigned int num_roaming_zones = std::min(Limit::roamingZones(), ctx.count<RoamingZone>());
  roaming_zone_bitmap.clear(); roaming_zone_bitmap.enableFirst(num_roaming_zones);

  // Mark roaming channels
  RoamingChannelBitmapElement roaming_ch_bitmap(data(Offset::roamingChannelBitmap()));
  unsigned int num_roaming_channel = std::min(Limit::roamingChannels(), ctx.count<RoamingChannel>());
  roaming_ch_bitmap.clear(); roaming_ch_bitmap.enableFirst(num_roaming_channel);
}


void
DMR6X2UVCodeplug::allocateForEncoding() {
  // First allocate everything common between D868UV and D878UV codeplugs.
  D868UVCodeplug::allocateForEncoding();

  this->allocateRoaming();
}

void
DMR6X2UVCodeplug::allocateForDecoding() {
  // First allocate everything common between D868UV and D878UV codeplugs.
  D868UVCodeplug::allocateForDecoding();

  this->allocateRoaming();
}

bool
DMR6X2UVCodeplug::encodeElements(const Flags &flags, Context &ctx, const ErrorStack &err)
{
  // Encode everything common between d868uv and d878uv radios.
  if (! D868UVCodeplug::encodeElements(flags, ctx, err))
    return false;

  if (! this->encodeRoaming(flags, ctx, err))
    return false;

  return true;
}


bool
DMR6X2UVCodeplug::decodeElements(Context &ctx, const ErrorStack &err)
{
  // Decode everything commong between d868uv and d878uv codeplugs.
  if (! D868UVCodeplug::decodeElements(ctx, err))
    return false;

  if (! this->createRoaming(ctx, err))
    return false;

  if (! this->linkRoaming(ctx, err))
    return false;

  return true;
}


void
DMR6X2UVCodeplug::allocateGeneralSettings() {
  image(0).addElement(Offset::settings(), GeneralSettingsElement::size());
  image(0).addElement(Offset::settingsExtension(), ExtendedSettingsElement::size());
}

bool
DMR6X2UVCodeplug::encodeGeneralSettings(const Flags &flags, Context &ctx, const ErrorStack &err) {
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
DMR6X2UVCodeplug::decodeGeneralSettings(Context &ctx, const ErrorStack &err) {
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
DMR6X2UVCodeplug::linkGeneralSettings(Context &ctx, const ErrorStack &err) {
  return GeneralSettingsElement(data(Offset::settings())).linkSettings(ctx.config()->settings(), ctx, err);
}


bool
DMR6X2UVCodeplug::encodeChannels(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)

  // Encode channels
  for (int i=0; i<ctx.config()->channelList()->count(); i++) {
    // enable channel
    uint16_t bank = i/Limit::channelsPerBank(), idx = i%Limit::channelsPerBank();
    ChannelElement ch(data(Offset::channelBanks() + bank * Offset::betweenChannelBanks()
                           + idx * ChannelElement::size()));
    if (! ch.fromChannelObj(ctx.config()->channelList()->channel(i), ctx))
      return false;
  }
  return true;
}

bool
DMR6X2UVCodeplug::createChannels(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)
  // Create channels
  ChannelBitmapElement channel_bitmap(data(Offset::channelBitmap()));
  for (uint16_t i=0; i<Limit::numChannels(); i++) {
    // Check if channel is enabled:
    uint16_t bank = i/Limit::channelsPerBank(), idx = i%Limit::channelsPerBank();
    if (! channel_bitmap.isEncoded(i))
      continue;
    ChannelElement ch(data(Offset::channelBanks() + bank*Offset::betweenChannelBanks()
                           + idx*ChannelElement::size()));
    if (Channel *obj = ch.toChannelObj(ctx)) {
      ctx.config()->channelList()->add(obj); ctx.add(obj, i);
    }
  }
  return true;
}

bool
DMR6X2UVCodeplug::linkChannels(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  ChannelBitmapElement channel_bitmap(data(Offset::channelBitmap()));
  // Link channel objects
  for (uint16_t i=0; i<Limit::numChannels(); i++) {
    // Check if channel is enabled:
    if (! channel_bitmap.isEncoded(i))
      continue;
    uint16_t bank = i/Limit::channelsPerBank(), idx = i%Limit::channelsPerBank();
    ChannelElement ch(data(Offset::channelBanks() + bank*Offset::betweenChannelBanks()
                           + idx*ChannelElement::size()));
    if (ctx.has<Channel>(i))
      ch.linkChannelObj(ctx.get<Channel>(i), ctx);
  }

  return true;
}

void
DMR6X2UVCodeplug::allocateGPSSystems() {
  // replaces D868UVCodeplug::allocateGPSSystems

  // APRS settings
  image(0).addElement(Offset::aprsSettings(), APRSSettingsElement::size());
  image(0).addElement(Offset::fmAPRSMessage(), Size::fmAPRSMessage());
}

bool
DMR6X2UVCodeplug::encodeGPSSystems(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)
  // replaces D868UVCodeplug::encodeGPSSystems

  APRSSettingsElement aprs(data(Offset::aprsSettings())); aprs.clear();

  // Encode APRS system (there can only be one)
  if (0 < ctx.count<APRSSystem>()) {
    aprs.fromFMAPRSSystem(ctx.get<APRSSystem>(0), ctx, err);
    uint8_t *aprsmsg = (uint8_t *)data(Offset::fmAPRSMessage());
    encode_ascii(aprsmsg, ctx.get<APRSSystem>(0)->message(), Limit::fmAPRSMessage(), 0x00);
  }

  // Encode GPS systems
  if (! aprs.fromDMRAPRSSystems(ctx))
    return false;
  if (0 < ctx.count<GPSSystem>()) {
    // If there is at least one GPS system defined -> set auto TX interval.
    //  This setting might be overridden by any analog APRS system below
    aprs.setAutoTXInterval(Interval::fromSeconds(ctx.get<GPSSystem>(0)->period()));
    aprs.setManualTXInterval(Interval::fromSeconds(ctx.get<GPSSystem>(0)->period()));
  }

  return true;
}

bool
DMR6X2UVCodeplug::createGPSSystems(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  // replaces D868UVCodeplug::createGPSSystems

  // Before creating any GPS/APRS systems, get global auto TX interval
  APRSSettingsElement aprs(data(Offset::aprsSettings()));
  unsigned pos_interval = aprs.autoTXInterval().seconds();

  // Create APRS system (if enabled)
  uint8_t *aprsmsg = (uint8_t *)data(Offset::fmAPRSMessage());
  if (aprs.isValid()) {
    APRSSystem *sys = aprs.toFMAPRSSystem();
    if (nullptr == sys) {
      errMsg(err) << "Cannot decode positioning systems.";
      return false;
    }
    sys->setPeriod(pos_interval);
    sys->setMessage(decode_ascii(aprsmsg, Limit::fmAPRSMessage(), 0x00));
    ctx.config()->posSystems()->add(sys); ctx.add(sys,0);
  }

  // Create GPS systems
  for (unsigned int i=0; i<APRSSettingsElement::Limit::dmrSystems(); i++) {
    if (0 == aprs.dmrDestination(i))
      continue;
    if (GPSSystem *sys = aprs.toDMRAPRSSystemObj(i)) {
      sys->setPeriod(pos_interval);
      ctx.config()->posSystems()->add(sys); ctx.add(sys, i);
    } else {
      return false;
    }
  }
  return true;
}

bool
DMR6X2UVCodeplug::linkGPSSystems(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err);
  // replaces D868UVCodeplug::linkGPSSystems

  // Link APRS system
  APRSSettingsElement aprs(data(Offset::aprsSettings()));
  if (aprs.isValid()) {
    aprs.linkFMAPRSSystem(ctx.config()->posSystems()->aprsSystem(0), ctx);
  }

  // Link GPS systems
  for (unsigned int i=0; i<APRSSettingsElement::Limit::dmrSystems(); i++) {
    if (0 == aprs.dmrDestination(i))
      continue;
    aprs.linkDMRAPRSSystem(i, ctx.get<GPSSystem>(i), ctx);
  }

  return true;
}


void
DMR6X2UVCodeplug::allocateRoaming() {
  /* Allocate roaming channels */
  RoamingChannelBitmapElement roaming_channel_bitmap(data(Offset::roamingChannelBitmap()));
  for (uint8_t i=0; i<Limit::roamingChannels(); i++) {
    // Get byte and bit for roaming channel
    // if disabled -> skip
    if (! roaming_channel_bitmap.isEncoded(i))
      continue;
    // Allocate roaming channel
    uint32_t addr = Offset::roamingChannels() + i*D878UVCodeplug::RoamingChannelElement::size();
    if (!isAllocated(addr, 0))
      image(0).addElement(addr, D878UVCodeplug::RoamingChannelElement::size());
  }

  /* Allocate roaming zones. */
  RoamingZoneBitmapElement roaming_zone_bitmap(data(Offset::roamingZoneBitmap()));
  for (uint8_t i=0; i<Limit::roamingZones(); i++) {
    // Get byte and bit for radio ID
    // if disabled -> skip
    if (! roaming_zone_bitmap.isEncoded(i))
      continue;
    // Allocate roaming zone
    uint32_t addr = Offset::roamingZones() + i*D878UVCodeplug::RoamingZoneElement::size();
    if (!isAllocated(addr, 0)) {
      logDebug() << "Allocate roaming zone at " << QString::number(addr, 16);
      image(0).addElement(addr, D878UVCodeplug::RoamingZoneElement::size());
    }
  }
}

bool
DMR6X2UVCodeplug::encodeRoaming(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err);

  // Encode roaming channels
  unsigned int num_roaming_channel = std::min(
        Limit::roamingChannels(), ctx.count<RoamingChannel>());
  for (uint8_t i=0; i<num_roaming_channel; i++) {
    // Encode roaming channel
    uint32_t addr = Offset::roamingChannels() + i*RoamingChannelElement::size();
    RoamingChannelElement rch_elm(data(addr));
    RoamingChannel *rch = ctx.config()->roamingChannels()->get(i)->as<RoamingChannel>();
    rch_elm.clear();
    rch_elm.fromChannel(rch);
    if (! ctx.add(rch, i)) {
      errMsg(err) << "Cannot add index " << i << " for roaming channel '"
                  << rch->name() << "' to codeplug context.";
      return false;
    }
  }

  // Encode roaming zones
  for (unsigned int i=0; i<ctx.count<RoamingZone>(); i++){
    uint32_t addr = Offset::roamingZones() + i*RoamingZoneElement::size();
    RoamingZoneElement zone(data(addr));
    logDebug() << "Encode roaming zone " << ctx.config()->roamingZones()->zone(i)->name()
               << " (" << (i+1) << ") at " << QString::number(addr, 16)
               << " with " << ctx.config()->roamingZones()->zone(i)->count() << " elements.";
    zone.fromRoamingZone(ctx.config()->roamingZones()->zone(i), ctx);
  }

  return true;
}

bool
DMR6X2UVCodeplug::createRoaming(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  // Create or find roaming channels
  RoamingChannelBitmapElement roaming_channel_bitmap(data(Offset::roamingChannelBitmap()));
  for (unsigned int i=0; i<Limit::roamingChannels(); i++) {
    if (! roaming_channel_bitmap.isEncoded(i))
      continue;
    uint32_t addr = Offset::roamingChannels() + i*RoamingChannelElement::size();
    RoamingChannelElement ch(data(addr));
    RoamingChannel *digi = ch.toChannel(ctx);
    ctx.add(digi, i);
  }

  // Create and link roaming zones
  RoamingZoneBitmapElement roaming_zone_bitmap(data(Offset::roamingZoneBitmap()));
  for (unsigned int i=0; i<Limit::roamingZones(); i++) {
    if (! roaming_zone_bitmap.isEncoded(i))
      continue;
    uint32_t addr = Offset::roamingZones() + i*RoamingZoneElement::size();
    RoamingZoneElement z(data(addr));
    RoamingZone *zone = z.toRoamingZone(ctx, err);
    ctx.config()->roamingZones()->add(zone); ctx.add(zone, i);
    z.linkRoamingZone(zone, ctx, err);
  }

  return true;
}

bool
DMR6X2UVCodeplug::linkRoaming(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(err)
  // Pass, no need to link roaming channels.
  return true;
}
