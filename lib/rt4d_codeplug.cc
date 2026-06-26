//
// Created by hannes on 23.06.26.
//

#include "rt4d_codeplug.hh"

#include "config.hh"
#include "intermediaterepresentation.hh"
#include "radioid.hh"


/* ********************************************************************************************* *
 * Implementation of RT4DCodeplug::FrequencyRangeElement
 * ********************************************************************************************* */
RT4DCodeplug::FrequencyRangeElement::FrequencyRangeElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}


RT4DCodeplug::FrequencyRangeElement::LockType
RT4DCodeplug::FrequencyRangeElement::lockType() const {
  return static_cast<RT4DCodeplug::FrequencyRangeElement::LockType>(getUInt8(Offset::lockType()));
}

void
RT4DCodeplug::FrequencyRangeElement::setLockType(LockType lockType) {
  setUInt8(Offset::lockType(), static_cast<unsigned int>(lockType));
}

Frequency
RT4DCodeplug::FrequencyRangeElement::lowerFrequency() const {
  return Frequency::fromMHz(getUInt16_le(Offset::lowerFrequency()));
}

void
RT4DCodeplug::FrequencyRangeElement::setLowerFrequency(const Frequency &lowerFrequency) {
  setUInt16_le(Offset::lowerFrequency(), lowerFrequency.inMHz());
}


Frequency
RT4DCodeplug::FrequencyRangeElement::upperFrequency() const {
  return Frequency::fromMHz(getUInt16_le(Offset::upperFrequency()));
}

void
RT4DCodeplug::FrequencyRangeElement::setUpperFrequency(const Frequency &upperFrequency) {
  setUInt16_le(Offset::upperFrequency(), upperFrequency.inMHz());
}



/* ********************************************************************************************* *
 * Implementation of RT4DCodeplug::DTMFCodeElement
 * ********************************************************************************************* */
RT4DCodeplug::DTMFCodeElement::DTMFCodeElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}

bool
RT4DCodeplug::DTMFCodeElement::isValid() const {
  return Element::isValid() && (0 < getUInt8(Offset::length())
          && (Limit::code() <= getUInt8(Offset::length())));
}

QString
RT4DCodeplug::DTMFCodeElement::code() const {
  return readASCII(Offset::code(), Limit::code(), 0xff);
}

void
RT4DCodeplug::DTMFCodeElement::setCode(const QString &code) {
  writeASCII(Offset::code(), code, Limit::code(), 0xff);
  setUInt8(Offset::padByte(), 0xff);
  setUInt8(Offset::length(),
    std::min(Limit::code(), static_cast<unsigned int>(code.length())));
}



/* ********************************************************************************************* *
 * Implementation of RT4DCodeplug::FirstSettingsElement
 * ********************************************************************************************* */
RT4DCodeplug::FirstSettingsElement::FirstSettingsElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}


bool
RT4DCodeplug::FirstSettingsElement::bootImageEnabled() const {
  return 0x01 == getUInt8(Offset::enableBootImage());
}

void
RT4DCodeplug::FirstSettingsElement::enableBootImage(bool enable) {
  setUInt8(Offset::enableBootImage(), enable ? 0x01 : 0x00);
}


bool
RT4DCodeplug::FirstSettingsElement::bootToneEnabled() const {
  return 0x01 == getUInt8(Offset::enableBootTone());
}

void
RT4DCodeplug::FirstSettingsElement::enableBootTone(bool enable) {
  setUInt8(Offset::enableBootTone(), enable ? 0x01 : 0x00);
}


bool
RT4DCodeplug::FirstSettingsElement::bootTextEnabled() const {
  return 0x01 == getUInt8(Offset::enableBootText());
}

void
RT4DCodeplug::FirstSettingsElement::enableBootText(bool enable) {
  setUInt8(Offset::enableBootText(), enable ? 0x01 : 0x00);
}


unsigned int
RT4DCodeplug::FirstSettingsElement::bootTextLine() const {
  return getUInt8(Offset::bootTextLine());
}

void
RT4DCodeplug::FirstSettingsElement::setBootTextLine(unsigned int line) {
  setUInt8(Offset::bootTextLine(), line);
}


unsigned int
RT4DCodeplug::FirstSettingsElement::bootTextColumn() const {
  return getUInt8(Offset::bootTextColumn());
}

void
RT4DCodeplug::FirstSettingsElement::setBootTextColumn(unsigned int column) {
  setUInt8(Offset::bootTextColumn(), column);
}


bool
RT4DCodeplug::FirstSettingsElement::bootPasswordEnabled() const {
  return 0x01 == getUInt8(Offset::enableBootPassword());
}

void
RT4DCodeplug::FirstSettingsElement::enableBootPassword(bool enable) {
  setUInt8(Offset::enableBootPassword(), enable ? 0x01 : 0x00);
}

QString
RT4DCodeplug::FirstSettingsElement::bootPassword() const {
  return readASCII(Offset::bootPassword(), Limit::bootPasswordLength(), 0xff);
}

void
RT4DCodeplug::FirstSettingsElement::setBootPassword(const QString &password) {
  writeASCII(Offset::bootPassword(), password, Limit::bootPasswordLength(), 0xff);
}


QString
RT4DCodeplug::FirstSettingsElement::bootText() const {
  return readASCII(Offset::bootText(), Limit::bootTextLength(), 0xff);
}

void
RT4DCodeplug::FirstSettingsElement::setBootText(const QString &text) {
  writeASCII(Offset::bootText(), text, Limit::bootTextLength(), 0xff);
}


QString
RT4DCodeplug::FirstSettingsElement::radioName() const {
  return readASCII(Offset::radioName(), Limit::radioNameLength(), 0xff);
}

void
RT4DCodeplug::FirstSettingsElement::setRadioName(const QString &name) {
  writeASCII(Offset::radioName(), name, Limit::radioNameLength(), 0xff);
}


bool
RT4DCodeplug::FirstSettingsElement::voicePromptEnabled() const {
  return 0x01 == getUInt8(Offset::enableVoicePrompt());
}

void
RT4DCodeplug::FirstSettingsElement::enableVoicePrompt(bool enable) {
  setUInt8(Offset::enableVoicePrompt(), enable ? 0x01 : 0x00);
}


bool
RT4DCodeplug::FirstSettingsElement::keyToneEnabled() const {
  return 0x01 == getUInt8(Offset::enableKeyTone());
}

void
RT4DCodeplug::FirstSettingsElement::enableKeyTone(bool enable) {
  setUInt8(Offset::enableKeyTone(), enable ? 0x01 : 0x00);
}


Interval
RT4DCodeplug::FirstSettingsElement::keyLockDelay() const {
  switch (getUInt8(Offset::keyLockDelay())) {
  case 0: return Interval::infinity();
  case 1: return Interval::fromSeconds(5);
  case 2: return Interval::fromSeconds(10);
  case 3: return Interval::fromSeconds(15);
  case 4: return Interval::fromSeconds(30);
  case 5: return Interval::fromSeconds(45);
  case 6: return Interval::fromSeconds(60);
  default: break;
  }
  return Interval::infinity();
}

void
RT4DCodeplug::FirstSettingsElement::setKeyLockDelay(const Interval &delay) {
  if (delay.isInfinite() || delay.isNull()) {
    setUInt8(Offset::keyLockDelay(), 0);
  } else if (delay.seconds() <= 5) {
    setUInt8(Offset::keyLockDelay(), 1);
  } else if (delay.seconds() <= 10) {
    setUInt8(Offset::keyLockDelay(), 2);
  } else if (delay.seconds() <= 15) {
    setUInt8(Offset::keyLockDelay(), 3);
  } else if (delay.seconds() <= 30) {
    setUInt8(Offset::keyLockDelay(), 4);
  } else if (delay.seconds() <= 45) {
    setUInt8(Offset::keyLockDelay(), 5);
  } else if (delay.seconds() <= 60) {
    setUInt8(Offset::keyLockDelay(), 6);
  }
}


Level
RT4DCodeplug::FirstSettingsElement::backlightBrightness() const {
  return Level::fromValue(getUInt8(Offset::backlightBrightness()), Limit::backlightBrightness());
}

void
RT4DCodeplug::FirstSettingsElement::setBacklightBrightness(const Level &level) {
  setUInt8(Offset::backlightBrightness(), level.mapTo(Limit::backlightBrightness()));
}


Interval
RT4DCodeplug::FirstSettingsElement::backlightDuration() const {
  if (! getUInt8(Offset::enableBacklight()))
    return Interval::null();
  if (0 == getUInt8(Offset::backlightDuration()))
    return Interval::infinity();
  if (1 == getUInt8(Offset::backlightDuration()))
    return Interval::fromSeconds(5);
  if (2 == getUInt8(Offset::backlightDuration()))
    return Interval::fromSeconds(10);
  if (3 <= getUInt8(Offset::backlightDuration())) {
    return Interval::fromSeconds(15*(getUInt8(Offset::backlightDuration())-2));
  }
  return Interval::infinity();
}

void
RT4DCodeplug::FirstSettingsElement::setBacklightDuration(const Interval &duration) {
  if (duration.isNull()) {
    setUInt8(Offset::enableBacklight(), 0);
    setUInt8(Offset::backlightDuration(), 0);
  } else if (duration.isInfinite()) {
    setUInt8(Offset::enableBacklight(), 1);
    setUInt8(Offset::backlightDuration(), 0);
  } else if (duration.seconds() <= 5) {
    setUInt8(Offset::enableBacklight(), 1);
    setUInt8(Offset::backlightDuration(), 1);
  } else if (duration.seconds() <= 10) {
    setUInt8(Offset::enableBacklight(), 1);
    setUInt8(Offset::backlightDuration(), 2);
  } else if (duration.seconds() >= 15) {
    setUInt8(Offset::enableBacklight(), 1);
    setUInt8(Offset::backlightDuration(), 2+duration.seconds()/15);
  }
}


RT4DCodeplug::FirstSettingsElement::PowerSaveMode
RT4DCodeplug::FirstSettingsElement::powerSaveMode() const {
  return static_cast<PowerSaveMode>(getUInt8(Offset::powerSaveMode()));
}

void
RT4DCodeplug::FirstSettingsElement::setPowerSaveMode(PowerSaveMode mode) {
  setUInt8(Offset::powerSaveMode(), static_cast<unsigned int>(mode));
}


Interval
RT4DCodeplug::FirstSettingsElement::powerSaveDelay() const {
  if (0 == getUInt8(Offset::powerSaveDelay()))
    return Interval::infinity();
  return Interval::fromSeconds(getUInt8(Offset::powerSaveDelay()));
}

void
RT4DCodeplug::FirstSettingsElement::setPowerSaveDelay(const Interval &delay) {
  if (delay.isInfinite() || delay.isNull()) {
    setUInt8(Offset::powerSaveDelay(), 0);
  } else {
    setUInt8(Offset::powerSaveDelay(), Limit::powerSaveDelay().limit(delay).seconds());
  }
}

Interval
RT4DCodeplug::FirstSettingsElement::menuDuration() const {
  if (0 == getUInt8(Offset::menuDuration()))
    return Interval::infinity();
  if (1 == getUInt8(Offset::menuDuration()))
    return Interval::fromSeconds(5);
  if (2 == getUInt8(Offset::menuDuration()))
    return Interval::fromSeconds(10);
  if (3 <= getUInt8(Offset::menuDuration())) {
    return Interval::fromSeconds(15*(getUInt8(Offset::menuDuration())-2));
  }
  return Interval::infinity();
}

void
RT4DCodeplug::FirstSettingsElement::setMenuDuration(const Interval &duration) {
  if (duration.isNull() || duration.isInfinite()) {
    setUInt8(Offset::menuDuration(), 0);
  } else if (duration.seconds() <= 5) {
    setUInt8(Offset::menuDuration(), 1);
  } else if (duration.seconds() <= 10) {
    setUInt8(Offset::menuDuration(), 2);
  } else if (duration.seconds() >= 15) {
    setUInt8(Offset::menuDuration(), 2+duration.seconds()/15);
  }
}


RT4DCodeplug::FirstSettingsElement::TalkaroundMode
RT4DCodeplug::FirstSettingsElement::talkaroundMode() const {
  return static_cast<TalkaroundMode>(getUInt8(Offset::talkaround()));
}

void
RT4DCodeplug::FirstSettingsElement::setTalkaroundMode(TalkaroundMode mode) {
  setUInt8(Offset::talkaround(), static_cast<unsigned int>(mode));
}


RT4DCodeplug::FirstSettingsElement::AlarmType
RT4DCodeplug::FirstSettingsElement::alarmType() const {
  return static_cast<AlarmType>(getUInt8(Offset::alarmType()));
}

void
RT4DCodeplug::FirstSettingsElement::setAlarmType(AlarmType alarm) {
  setUInt8(Offset::alarmType(), static_cast<unsigned int>(alarm));
}


Interval
RT4DCodeplug::FirstSettingsElement::autoPowerOffDelay() const {
  if (0 == getUInt32_le(Offset::autoPowerOffDelay()))
    return Interval::infinity();
  return Interval::fromSeconds(getUInt32_le(Offset::autoPowerOffDelay()));
}

void
RT4DCodeplug::FirstSettingsElement::setAutoPowerOffDelay(const Interval &delay) {
  if (delay.isInfinite() || delay.isNull()) {
    setUInt32_le(Offset::autoPowerOffDelay(), 0);
  } else {
    setUInt32_le(Offset::autoPowerOffDelay(), delay.seconds());
  }
}


RT4DCodeplug::FirstSettingsElement::TransmitPriority
RT4DCodeplug::FirstSettingsElement::txPriority() const {
  return static_cast<TransmitPriority>(getUInt32_le(Offset::txPriority()));
}

void
RT4DCodeplug::FirstSettingsElement::setTxPriority(TransmitPriority priority) {
  setUInt32_le(Offset::txPriority(), static_cast<unsigned int>(priority));
}


RT4DCodeplug::FirstSettingsElement::PTTMode
RT4DCodeplug::FirstSettingsElement::pttMode() const {
  return static_cast<PTTMode>(getUInt8(Offset::pttMode()));
}

void
RT4DCodeplug::FirstSettingsElement::setPttMode(PTTMode mode) {
  setUInt8(Offset::pttMode(), static_cast<unsigned int>(mode));
}


RT4DCodeplug::FrequencyRangeElement
RT4DCodeplug::FirstSettingsElement::frequencyRange(unsigned int n) const {
  n = std::min(n, Limit::frequencyRanges());
  return FrequencyRangeElement(_data + Offset::frequencyRanges() + n*FrequencyRangeElement::size());
}


RT4DCodeplug::FirstSettingsElement::ScanMode
RT4DCodeplug::FirstSettingsElement::scanMode() const {
  return static_cast<ScanMode>(getUInt8(Offset::scanMode()));
}

void
RT4DCodeplug::FirstSettingsElement::setScanMode(ScanMode mode) {
  setUInt8(Offset::scanMode(), static_cast<unsigned int>(mode));
}


RT4DCodeplug::FirstSettingsElement::ScanRevertChannel
RT4DCodeplug::FirstSettingsElement::scanRevertChannel() const {
  return static_cast<ScanRevertChannel>(getUInt8(Offset::scanRevertChannel()));
}

void
RT4DCodeplug::FirstSettingsElement::setScanRevertChannel(ScanRevertChannel channel) {
  setUInt8(Offset::scanRevertChannel(), static_cast<unsigned int>(channel));
}


Interval
RT4DCodeplug::FirstSettingsElement::scanDwellDuration() const {
  return Interval::fromSeconds(getUInt8(Offset::scanDwellTime()));
}

void
RT4DCodeplug::FirstSettingsElement::setScanDwellDuration(const Interval &duration) {
  if (duration.isInfinite() || duration.isNull()) {
    setUInt8(Offset::scanDwellTime(), 0);
  } else {
    setUInt8(Offset::scanDwellTime(), duration.seconds());
  }
}

Interval
RT4DCodeplug::FirstSettingsElement::scanInterval() const {
  return Interval::fromSeconds(getUInt8(Offset::scanInterval()));
}

void
RT4DCodeplug::FirstSettingsElement::setScanInterval(const Interval &interval) {
  if (interval.isInfinite() || interval.isNull()) {
    setUInt8(Offset::scanInterval(), 0);
  } else {
    setUInt8(Offset::scanInterval(), interval.seconds());
  }
}


Interval
RT4DCodeplug::FirstSettingsElement::rssiUpdatePeriod() const {
  if (0 == getUInt8(Offset::rssiUpdatePeriod())) {
    return Interval::infinity();
  }
  return Interval::fromMilliseconds(
    static_cast<unsigned int>(getUInt8(Offset::rssiUpdatePeriod()))*100);
}

void
RT4DCodeplug::FirstSettingsElement::setRssiUpdatePeriod(const Interval &period) {
  if (period.isInfinite() || period.isNull()) {
    setUInt8(Offset::rssiUpdatePeriod(), 0);
  } else {
    setUInt8(Offset::rssiUpdatePeriod(), period.milliseconds()/100);
  }
}


Level
RT4DCodeplug::FirstSettingsElement::displayContrast() const {
  return Level::fromValue(getUInt8(Offset::displayContrast()), {1,25});
}

void
RT4DCodeplug::FirstSettingsElement::setDisplayContrast(const Level &contrast) {
  setUInt8(Offset::displayContrast(),
    contrast.mapTo({1, 25}));
}


RT4DCodeplug::FirstSettingsElement::FrequencyPrecision
RT4DCodeplug::FirstSettingsElement::frequencyPrecision() const {
  return static_cast<FrequencyPrecision>(getUInt8(Offset::frequencyPrecision()));
}

void
RT4DCodeplug::FirstSettingsElement::setFrequencyPrecision(FrequencyPrecision precision) {
  setUInt8(Offset::frequencyPrecision(), static_cast<unsigned int>(precision));
}


Frequency
RT4DCodeplug::FirstSettingsElement::tbstFrequency() const {
  return Frequency::fromHz(getUInt16_le(Offset::tbstFrequency()));
}

void
RT4DCodeplug::FirstSettingsElement::setTbstFrequency(const Frequency &frequency) {
  setUInt16_le(Offset::tbstFrequency(), frequency.inHz());
}


Level
RT4DCodeplug::FirstSettingsElement::fmSquelch() const {
  return Level::fromValue(getUInt8(Offset::fmSquelchLevel()),{1,10});
}

void
RT4DCodeplug::FirstSettingsElement::setFmSquelch(const Level &level) {
  setUInt8(Offset::fmSquelchLevel(), level.mapTo({1, 10}));
}


Level
RT4DCodeplug::FirstSettingsElement::fmMicGain() const {
  return Level::fromValue(getUInt8(Offset::fmMicGain()), {1,31});
}

void
RT4DCodeplug::FirstSettingsElement::setFmMicGain(const Level &level) {
  setUInt8(Offset::fmMicGain(), level.mapTo({1, 31}));
}


Level
RT4DCodeplug::FirstSettingsElement::fmSpeakerGain() const {
  return Level::fromValue(getUInt8(Offset::fmSpeakerGain()), {1,63});
}

void
RT4DCodeplug::FirstSettingsElement::setFmSpeakerGain(const Level &level) {
  setUInt8(Offset::fmSpeakerGain(), level.mapTo({1, 63}));
}


bool
RT4DCodeplug::FirstSettingsElement::fmBotToneEnabled() const {
  return 0x01 == getUInt8(Offset::enableFmBotTone());
}

void
RT4DCodeplug::FirstSettingsElement::enableFmBotTone(bool enable) {
  setUInt8(Offset::enableFmBotTone(), enable ? 1 : 0);
}

RT4DCodeplug::FirstSettingsElement::FMEotTone
RT4DCodeplug::FirstSettingsElement::fmEotTone() const {
  return static_cast<FMEotTone>(getUInt8(Offset::fmEotTone()));
}

void
RT4DCodeplug::FirstSettingsElement::setFMEotTone(FMEotTone tone) {
  setUInt8(Offset::fmEotTone(), static_cast<unsigned int>(tone));
}


RT4DCodeplug::FirstSettingsElement::FrequencyDetectionRange
RT4DCodeplug::FirstSettingsElement::frequencyDetectionRange() const {
  return static_cast<FrequencyDetectionRange>(getUInt8(Offset::frequencyDetectionRange()));
}

void
RT4DCodeplug::FirstSettingsElement::setFrequencyDetectionRange(FrequencyDetectionRange range) {
  setUInt8(Offset::frequencyDetectionRange(), static_cast<unsigned int>(range));
}


Interval
RT4DCodeplug::FirstSettingsElement::frequencyDetectionPeriod() const {
  return Interval::fromMilliseconds(
    static_cast<unsigned int>(getUInt8(Offset::detectionPeriod()))*100);
}

void
RT4DCodeplug::FirstSettingsElement::setFrequencyDetectionPeriod(const Interval &period) {
  setUInt8(Offset::detectionPeriod(),
    Limit::Range<Interval>{Interval::null(), Interval::fromSeconds(2)}.limit(period).milliseconds()/200);
}

Level
RT4DCodeplug::FirstSettingsElement::fmGlitchThreshold() const {
  return Level::fromValue(getUInt8(Offset::fmGlitchThreshold()), {0, 10});
}

void
RT4DCodeplug::FirstSettingsElement::setFmGlitchThreshold(const Level &level) {
  setUInt8(Offset::fmGlitchThreshold(), level.mapTo({0, 10}));
}


Interval
RT4DCodeplug::FirstSettingsElement::tbstDuration() const {
  return Interval::fromSeconds(getUInt8(Offset::tbstDuration()));
}

void
RT4DCodeplug::FirstSettingsElement::setTbstDuration(const Interval &duration) {
  setUInt8(Offset::tbstDuration(),
    Limit::Range<Interval>{Interval::null(), Interval::fromSeconds(120)}.limit(duration).seconds());
}


unsigned int
RT4DCodeplug::FirstSettingsElement::radioDMRId() const {
  return getBCD8_le(Offset::radioDMRId());
}

void
RT4DCodeplug::FirstSettingsElement::setRadioDMRId(unsigned int id) {
  setBCD8_le(Offset::radioDMRId(), id);
}


bool
RT4DCodeplug::FirstSettingsElement::dmrRemoteControlEnabled() const {
  return 0x01 == getUInt8(Offset::enableDMRRemoteControl());
}

void
RT4DCodeplug::FirstSettingsElement::enableDmrRemoteControl(bool enable) {
  setUInt8(Offset::enableDMRRemoteControl(), enable ? 1 : 0);
}


Level
RT4DCodeplug::FirstSettingsElement::dmrTxNoiseReduction() const {
  Level::fromValue(getUInt8(Offset::dmrTxNoiseReduction()), {0,4});
}

void
RT4DCodeplug::FirstSettingsElement::setDmrTxNoiseReduction(const Level &level) {
  setUInt8(Offset::dmrTxNoiseReduction(), level.mapTo({0,4}));
}

Level
RT4DCodeplug::FirstSettingsElement::dmrRxNoiseReduction() const {
  Level::fromValue(getUInt8(Offset::dmrRxNoiseReduction()), {0,4});
}

void
RT4DCodeplug::FirstSettingsElement::setDmrRxNoiseReduction(const Level &level) {
  setUInt8(Offset::dmrRxNoiseReduction(), level.mapTo({0,4}));
}


Level
RT4DCodeplug::FirstSettingsElement::dmrMicGain() const {
  return Level::fromValue(getUInt8(Offset::dmrMicGain()), {1, 24});
}

void
RT4DCodeplug::FirstSettingsElement::setDmrMicGain(const Level &level) {
  setUInt8(Offset::dmrMicGain(), level.mapTo({1,24}));
}

Level
RT4DCodeplug::FirstSettingsElement::dmrSpeakerGain() const {
  return Level::fromValue(getUInt8(Offset::dmrSpeakerGain()), {0, 24});
}

void
RT4DCodeplug::FirstSettingsElement::setDmrSpeakerGain(const Level &level) {
  setUInt8(Offset::dmrSpeakerGain(), level.mapTo({0,24}));
}


RT4DCodeplug::FirstSettingsElement::DMREotTone
RT4DCodeplug::FirstSettingsElement::dmrEotTone() const {
  return static_cast<DMREotTone>(getUInt8(Offset::dmrEotTone()));
}

void
RT4DCodeplug::FirstSettingsElement::setDmrEotTone(DMREotTone dt) {
  setUInt8(Offset::dmrEotTone(), static_cast<unsigned int >(dt));
}


Interval
RT4DCodeplug::FirstSettingsElement::groupCallHangTime() const {
  return Interval::fromMilliseconds(getUInt16_le(Offset::groupCallHangTime()));
}

void
RT4DCodeplug::FirstSettingsElement::setGroupCallHangTime(const Interval &time) {
  setUInt16_le(Offset::groupCallHangTime(), time.milliseconds());
}

Interval
RT4DCodeplug::FirstSettingsElement::privateCallHangTime() const {
  return Interval::fromMilliseconds(getUInt16_le(Offset::privateCallHangTime()));
}

void
RT4DCodeplug::FirstSettingsElement::setPrivateCallHangTime(const Interval &time) {
  setUInt16_le(Offset::privateCallHangTime(), time.milliseconds());
}


Level
RT4DCodeplug::FirstSettingsElement::dmrSquelch() const {
  return Level::fromValue(getUInt8(Offset::dmrSquelchLevel()), {0,10});
}

void
RT4DCodeplug::FirstSettingsElement::setDmrSquelch(const Level &level) {
  setUInt8(Offset::dmrSquelchLevel(), level.mapTo({0,10}));
}


RT4DCodeplug::FirstSettingsElement::TalkerAliasDisplayMode
RT4DCodeplug::FirstSettingsElement::talkerAliasDisplayMode() const {
  return static_cast<RT4DCodeplug::FirstSettingsElement::TalkerAliasDisplayMode>(
    getUInt8(Offset::talkerAliasDisplayMode()));
}

void
RT4DCodeplug::FirstSettingsElement::setTalkerAliasDisplayMode(TalkerAliasDisplayMode displayMode) {
  setUInt8(Offset::talkerAliasDisplayMode(), static_cast<unsigned int>(displayMode));
}


SMSExtension::Format
RT4DCodeplug::FirstSettingsElement::smsFormat() const {
  switch (static_cast<SMSFormat>(getUInt8(Offset::smsFormat()))) {
    case SMSFormat::Motorola: return SMSExtension::Format::Motorola;
    case SMSFormat::Hytera: return SMSExtension::Format::Hytera;
  }
  return SMSExtension::Format::Hytera;
}

void
RT4DCodeplug::FirstSettingsElement::setSmsFormat(SMSExtension::Format smsFormat) {
  switch (smsFormat) {
  case SMSExtension::Format::Motorola: setUInt8(Offset::smsFormat(), static_cast<unsigned int>(SMSFormat::Motorola)); break;
  case SMSExtension::Format::Hytera: setUInt8(Offset::smsFormat(), static_cast<unsigned int>(SMSFormat::Hytera)); break;
  default: break;
  }
}

RT4DCodeplug::FirstSettingsElement::SMSEncoding
RT4DCodeplug::FirstSettingsElement::smsEncoding() const {
  return static_cast<RT4DCodeplug::FirstSettingsElement::SMSEncoding>(
    getUInt8(Offset::smsEncoding()));
}

void
RT4DCodeplug::FirstSettingsElement::setSmsEncoding(SMSEncoding encoding) {
  setUInt8(Offset::smsEncoding(), static_cast<unsigned int>(encoding));
}


Interval
RT4DCodeplug::FirstSettingsElement::dtmfTransmitDelay() const {
  return Interval::fromMilliseconds(
    static_cast<unsigned int>(getUInt16_le(Offset::dtmfTransmitDelay()))*100);
}

void
RT4DCodeplug::FirstSettingsElement::setDtmfTransmitDelay(const Interval &delay) {
  setUInt16_le(Offset::dtmfTransmitDelay(),
    Limit::Range<Interval>{Interval::null(), Interval::fromSeconds(2)}
    .limit(delay).milliseconds());
}

Interval
RT4DCodeplug::FirstSettingsElement::dtmfToneDuration() const {
  return Interval::fromMilliseconds(
    30 + getUInt8(Offset::dtmfToneDuration())*10);
}

void
RT4DCodeplug::FirstSettingsElement::setDtmfToneDuration(const Interval &duration) {
  setUInt8(Offset::dtmfToneDuration(),
    (Limit::Range<Interval>{Interval::fromMilliseconds(30), Interval::fromMilliseconds(200)}
      .limit(duration).milliseconds()-30)/10);
}

Interval
RT4DCodeplug::FirstSettingsElement::dtmfInterval() const {
  return Interval::fromMilliseconds(
    30 + 10*getUInt8(Offset::dtmfInterval()));
}

void
RT4DCodeplug::FirstSettingsElement::setDtmfInterval(const Interval &interval) {
  setUInt8(Offset::dtmfInterval(),
    (Limit::Range<Interval>{Interval::fromMilliseconds(30), Interval::fromMilliseconds(200)}
    .limit(interval).milliseconds()-30)/10);
}


RT4DCodeplug::FirstSettingsElement::DTMFTransmissionMode
RT4DCodeplug::FirstSettingsElement::dtmfTransmissionMode() const {
  return static_cast<RT4DCodeplug::FirstSettingsElement::DTMFTransmissionMode>(
    getUInt8(Offset::dtmfTransmitMode()));
}

void
RT4DCodeplug::FirstSettingsElement::setDtmfTransmissionMode(DTMFTransmissionMode mode) {
  setUInt8(Offset::dtmfTransmitMode(), static_cast<unsigned int>(mode));
}

unsigned int
RT4DCodeplug::FirstSettingsElement::dtmfCodeIndex() const {
  return getUInt8(Offset::dtmfCodeIndex());
}

void
RT4DCodeplug::FirstSettingsElement::setDtmfCodeIndex(unsigned int index) {
  setUInt8(Offset::dtmfCodeIndex(), Limit::Range<unsigned int>{0,15}.limit(index));
}

bool
RT4DCodeplug::FirstSettingsElement::displayRxDtmfCodeEnabled() const {
  return 0x01 == getUInt8(Offset::enableDtmfRxDisplay());
}

void
RT4DCodeplug::FirstSettingsElement::enableDisplayRxDtmfCode(bool enabled) {
  setUInt8(Offset::enableDtmfRxDisplay(), enabled ? 1 : 0);
}

Level
RT4DCodeplug::FirstSettingsElement::dtmfTxGain() const {
  return Level::fromValue(getUInt8(Offset::dtmfTransmitGain()), {0,128});
}

void
RT4DCodeplug::FirstSettingsElement::setDtmfTxGain(const Level &gain) {
  setUInt8(Offset::dtmfTransmitGain(), gain.mapTo({0, 128}));
}

Level
RT4DCodeplug::FirstSettingsElement::dtmfDecodeThreshold() const {
  Level::fromValue(getUInt8(Offset::dtmfDecodeThreshold()), {0,128});
}

void
RT4DCodeplug::FirstSettingsElement::setDtmfDecodeThreshold(const Level &threshold) {
  setUInt8(Offset::dtmfDecodeThreshold(), threshold.mapTo({0,128}));
}

bool
RT4DCodeplug::FirstSettingsElement::dtmfRemoteControlEnabled() const {
  return 0x01 == getUInt8(Offset::enableDtmfRemoteControl());
}

void
RT4DCodeplug::FirstSettingsElement::enableDtmfRemoteControl(bool enabled) {
  setUInt8(Offset::enableDtmfRemoteControl(), enabled ? 1 : 0);
}

RT4DCodeplug::DTMFCodeElement
RT4DCodeplug::FirstSettingsElement::dtmfCode(unsigned int index) const {
  return DTMFCodeElement(_data + Offset::dtmfCodes() + index * DTMFCodeElement::size());
}

RT4DCodeplug::DTMFCodeElement
RT4DCodeplug::FirstSettingsElement::dtmfRemoteStunCode() const {
  return DTMFCodeElement(_data + Offset::dtmfRemoteStun());
}

RT4DCodeplug::DTMFCodeElement
RT4DCodeplug::FirstSettingsElement::dtmfRemoteKillCode() const {
  return DTMFCodeElement(_data + Offset::dtmfRemoteKill());
}

RT4DCodeplug::DTMFCodeElement
RT4DCodeplug::FirstSettingsElement::dtmfRemoteWakeCode() const {
  return DTMFCodeElement(_data + Offset::dtmfRemoteWake());
}

RT4DCodeplug::DTMFCodeElement
RT4DCodeplug::FirstSettingsElement::dtmfRemoteMonitorCode() const {
  return DTMFCodeElement(_data + Offset::dtmfRemoteMonitor());
}


bool
RT4DCodeplug::FirstSettingsElement::swapFrequenciesEnabled() const {
  return 0x01 == getUInt8(Offset::swapFrequencies());
}

void
RT4DCodeplug::FirstSettingsElement::enableSwapFrequencies(bool enabled) {
  setUInt8(Offset::swapFrequencies(), enabled ? 1 : 0);
}


bool
RT4DCodeplug::FirstSettingsElement::smsNotificationEnabled() const {
  return 0x01 == getUInt8(Offset::smsNotification());
}

void
RT4DCodeplug::FirstSettingsElement::enableSmsNotification(bool enabled) {
  setUInt8(Offset::smsNotification(), enabled ? 1 : 0);
}


Frequency
RT4DCodeplug::FirstSettingsElement::lowerScanFrequency() const {
  return Frequency::fromHz(getUInt32_le(Offset::lowerScanFrequency()));
}

void
RT4DCodeplug::FirstSettingsElement::setLowerScanFrequency(const Frequency &frequency) {
  setUInt32_le(Offset::lowerScanFrequency(), frequency.inHz());
}

Frequency
RT4DCodeplug::FirstSettingsElement::upperScanFrequency() const {
  return Frequency::fromHz(getUInt32_le(Offset::upperScanFrequency()));
}

void
RT4DCodeplug::FirstSettingsElement::setUpperScanFrequency(const Frequency &frequency) {
  setUInt32_le(Offset::upperScanFrequency(), frequency.inHz());
}


bool
RT4DCodeplug::FirstSettingsElement::ledEnabled() const {
  return 0x01 == getUInt8(Offset::enableLed());
}

void
RT4DCodeplug::FirstSettingsElement::enableLed(bool enabled) {
  setUInt8(Offset::enableLed(), enabled ? 1 : 0);
}


bool
RT4DCodeplug::FirstSettingsElement::encode(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err);

  // Encode boot settings
  enableBootImage(BootSettings::BootDisplay::Logo == ctx.config()->settings()->boot()->bootDisplay());
  enableBootText(BootSettings::BootDisplay::Text == ctx.config()->settings()->boot()->bootDisplay());
  setBootText(ctx.config()->settings()->boot()->message1());
  enableBootTone(ctx.config()->settings()->tone()->bootToneEnabled());
  enableBootPassword(ctx.config()->settings()->boot()->bootPasswordEnabled());
  setBootPassword(ctx.config()->settings()->boot()->bootPassword());

  // Audio settings
  enableVoicePrompt(ctx.config()->settings()->audio()->speechSynthesisEnabled());
  setFmSquelch(ctx.config()->settings()->audio()->squelch());
  if (ctx.config()->settings()->audio()->dmrSquelchEnabled())
    setDmrSquelch(ctx.config()->settings()->audio()->dmrSquelch());
  else
    setDmrSquelch(ctx.config()->settings()->audio()->squelch());
  setDmrMicGain(ctx.config()->settings()->audio()->micGain());
  if (ctx.config()->settings()->audio()->fmMicGainEnabled())
    setFmMicGain(ctx.config()->settings()->audio()->fmMicGain());
  else
    setFmMicGain(ctx.config()->settings()->audio()->micGain());
  setDmrSpeakerGain(ctx.config()->settings()->audio()->maxSpeakerVolume());
  setFmSpeakerGain(ctx.config()->settings()->audio()->maxSpeakerVolume());

  // Tone settings
  enableKeyTone(ctx.config()->settings()->tone()->keyToneEnabled());
  enableSmsNotification(ctx.config()->settings()->tone()->smsToneEnabled());
  enableFmBotTone(ctx.config()->settings()->tone()->callStart().testFlag(Channel::Type::FM));
  setFMEotTone(ctx.config()->settings()->tone()->callEnd().testFlag(Channel::Type::FM) ? FMEotTone::Roger1 : FMEotTone::None);
  setDmrEotTone(ctx.config()->settings()->tone()->callEnd().testFlag(Channel::Type::DMR) ? DMREotTone::Roger1 : DMREotTone::None);

  // DMR settings
  if (! ctx.config()->settings()->defaultIdRef()->isNull()) {
    setRadioName(ctx.config()->settings()->defaultId()->name());
    setRadioDMRId(ctx.config()->settings()->defaultId()->number());
  }
  setGroupCallHangTime(ctx.config()->settings()->dmr()->groupCallHangTime());
  setPrivateCallHangTime(ctx.config()->settings()->dmr()->privateCallHangTime());
  setSmsEncoding(SMSEncoding::Unicode);
  setSmsFormat(ctx.config()->smsExtension()->format());

  return true;
}


bool
RT4DCodeplug::FirstSettingsElement::decode(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err);

  // Decode boot settings
  ctx.config()->settings()->boot()->setBootDisplay(bootImageEnabled() ? BootSettings::BootDisplay::Logo : BootSettings::BootDisplay::Text);
  ctx.config()->settings()->boot()->setMessage1(bootText());
  ctx.config()->settings()->boot()->setMessage2("");
  ctx.config()->settings()->tone()->enableBootTone(bootToneEnabled());
  ctx.config()->settings()->boot()->setBootPassword(bootPassword());
  ctx.config()->settings()->boot()->enableBootPassword(bootPasswordEnabled());

  // Audio settings
  ctx.config()->settings()->audio()->enableSpeechSynthesis(voicePromptEnabled());
  ctx.config()->settings()->audio()->setSquelch(fmSquelch());
  if (fmSquelch() != dmrSquelch()) ctx.config()->settings()->audio()->setDMRSquelch(dmrSquelch());
  else ctx.config()->settings()->audio()->setDMRSquelch(Level::invalid());
  ctx.config()->settings()->audio()->setMicGain(dmrMicGain());
  if (dmrMicGain() != fmMicGain()) ctx.config()->settings()->audio()->setFMMicGain(fmMicGain());
  else ctx.config()->settings()->audio()->setFMMicGain(Level::invalid());
  ctx.config()->settings()->audio()->setMaxSpeakerVolume(std::max(dmrSpeakerGain(), fmSpeakerGain()));

  // Tone settings
  ctx.config()->settings()->tone()->setKeyToneVolume(keyToneEnabled() ? Level::fromValue(5) : Level::invalid());
  ctx.config()->settings()->tone()->enableSMSTone(smsNotificationEnabled());
  ctx.config()->settings()->tone()->setCallStart(
    fmBotToneEnabled() ? Channel::Type::FM : Channel::Type::None
    );
  ctx.config()->settings()->tone()->setCallEnd(
    (FMEotTone::None != fmEotTone()) ? Channel::Type::FM : Channel::Type::None  |
    (DMREotTone::None != dmrEotTone() ? Channel::Type::DMR : Channel::Type::None) );

  // Decode DMR settings
  auto id = new DMRRadioID(radioName(), radioDMRId());
  ctx.config()->radioIDs()->add(id);
  ctx.config()->settings()->setDefaultId(id);
  ctx.add(id, 0);
  ctx.config()->settings()->dmr()->setGroupCallHangTime(groupCallHangTime());
  ctx.config()->settings()->dmr()->setPrivateCallHangTime(privateCallHangTime());
  ctx.config()->smsExtension()->setFormat(smsFormat());

  return true;
}



/* ********************************************************************************************* *
 * Implementation of RT4DCodeplug::ChannelElement
 * ********************************************************************************************* */
RT4DCodeplug::ChannelElement::ChannelElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}

void
RT4DCodeplug::ChannelElement::clear() {
  memset(_data, 0xff, size());
}

bool
RT4DCodeplug::ChannelElement::isValid() const {
  return !name().isEmpty();
}


bool
RT4DCodeplug::ChannelElement::promiscuousModeEnabled() const {
  return getBit(Offset::promiscuousMode());
}

void
RT4DCodeplug::ChannelElement::enablePromiscuousMode(bool enable) {
  setBit(Offset::promiscuousMode(), enable);
}


DMRChannel::TimeSlot
RT4DCodeplug::ChannelElement::timeSlot() const {
  return getBit(Offset::timeSlot()) ? DMRChannel::TimeSlot::TS2 : DMRChannel::TimeSlot::TS1;
}

void
RT4DCodeplug::ChannelElement::setTimeSlot(DMRChannel::TimeSlot timeSlot) {
  setBit(Offset::timeSlot(), DMRChannel::TimeSlot::TS2 == timeSlot);
}

bool
RT4DCodeplug::ChannelElement::dualSlotEnabled() const {
  return getBit(Offset::dualSlot());
}

void
RT4DCodeplug::ChannelElement::enableDualSlot(bool enable) {
  setBit(Offset::dualSlot(), enable);
}

bool
RT4DCodeplug::ChannelElement::channelDmrIdEnabled() const {
  return getBit(Offset::dmrIdSource());
}

void
RT4DCodeplug::ChannelElement::enableChannelDmrId(bool enable) {
  setBit(Offset::dmrIdSource(), enable);
}

RT4DCodeplug::ChannelElement::ChannelType
RT4DCodeplug::ChannelElement::channelType() const {
  return getBit(Offset::channelType()) ? ChannelType::FM : ChannelType::DMR;
}

void
RT4DCodeplug::ChannelElement::setChannelType(ChannelType type) {
  setBit(Offset::channelType(), type == ChannelType::FM);
}


bool
RT4DCodeplug::ChannelElement::fmScramblerEnabled() const {
  return 0 != getUInt4(Offset::fmScrambler());
}

unsigned int
RT4DCodeplug::ChannelElement::fmScramblerIndex() const {
  return getUInt4(Offset::fmScrambler())-1;
}

void
RT4DCodeplug::ChannelElement::setFmScramblerIndex(unsigned int index) {
  setUInt4(Offset::fmScrambler(), index+1);
}

void
RT4DCodeplug::ChannelElement::clearFmScrambler() {
  setUInt4(Offset::fmScrambler(), 0);
}


unsigned int
RT4DCodeplug::ChannelElement::colorCode() const {
  return getUInt4(Offset::colorCode());
}

void
RT4DCodeplug::ChannelElement::setColorCode(unsigned int colorCode) {
  setUInt4(Offset::colorCode(), colorCode);
}


DMRChannel::Admit
RT4DCodeplug::ChannelElement::dmrAdmit() const {
  switch (static_cast<DMRAdmit>(getUInt2(Offset::dmrAdmit()))) {
  case DMRAdmit::Always: return DMRChannel::Admit::Always;
  case DMRAdmit::ChannelFree: return DMRChannel::Admit::Free;
  case DMRAdmit::ColorCodeIdle: return DMRChannel::Admit::ColorCode;
  }
}

void
RT4DCodeplug::ChannelElement::setDmrAdmit(DMRChannel::Admit dmrAdmit) {
  switch (dmrAdmit) {
  case DMRChannel::Admit::Always:
    setUInt2(Offset::dmrAdmit(), static_cast<unsigned int>(DMRAdmit::Always));
    break;
  case DMRChannel::Admit::Free:
    setUInt2(Offset::dmrAdmit(), static_cast<unsigned int>(DMRAdmit::ChannelFree));
    break;
  case DMRChannel::Admit::ColorCode:
    setUInt2(Offset::dmrAdmit(), static_cast<unsigned int>(DMRAdmit::ColorCodeIdle));
    break;
  }
}

FMChannel::Admit
RT4DCodeplug::ChannelElement::fmAdmit() const {
  switch (static_cast<DMRAdmit>(getUInt2(Offset::fmAdmit()))) {
  case DMRAdmit::Always: return FMChannel::Admit::Always;
  case DMRAdmit::ChannelFree: return FMChannel::Admit::Free;
  case DMRAdmit::ColorCodeIdle: return FMChannel::Admit::Tone;
  }
}

void
RT4DCodeplug::ChannelElement::setFmAdmit(FMChannel::Admit admit) {
  switch (admit) {
  case FMChannel::Admit::Always:
    setUInt2(Offset::fmAdmit(), static_cast<unsigned int>(FMAdmit::Always));
    break;
  case FMChannel::Admit::Free:
    setUInt2(Offset::fmAdmit(), static_cast<unsigned int>(FMAdmit::ChannelFree));
    break;
  case FMChannel::Admit::Tone:
    setUInt2(Offset::fmAdmit(), static_cast<unsigned int>(FMAdmit::ToneIdle));
    break;
  }
}


RT4DCodeplug::ChannelElement::STEMode
RT4DCodeplug::ChannelElement::steMode() const {
  return static_cast<STEMode>(getUInt3(Offset::steMode()));
}

void
RT4DCodeplug::ChannelElement::setSteMode(STEMode mode) {
  setUInt3(Offset::steMode(), static_cast<unsigned int>(mode));
}


FMChannel::Bandwidth
RT4DCodeplug::ChannelElement::bandwidth() const {
  return getBit(Offset::bandwidth()) ? FMChannel::Bandwidth::Narrow : FMChannel::Bandwidth::Wide;
}

void
RT4DCodeplug::ChannelElement::setBandwidth(FMChannel::Bandwidth bandwidth) {
  setBit(Offset::bandwidth(), FMChannel::Bandwidth::Narrow == bandwidth);
}


RT4DCodeplug::ChannelElement::AnalogDemod
RT4DCodeplug::ChannelElement::analogDemodulation() const {
  return static_cast<AnalogDemod>(getUInt2(Offset::analogDemodulaiton()));
}

void
RT4DCodeplug::ChannelElement::setAnalogDemodulation(AnalogDemod analogDemod) {
  setUInt2(Offset::analogDemodulaiton(), static_cast<unsigned int>(analogDemod));
}


RT4DCodeplug::ChannelElement::SubToneType
RT4DCodeplug::ChannelElement::subToneType() const {
  return static_cast<SubToneType>(getUInt3(Offset::subToneType()));
}

void
RT4DCodeplug::ChannelElement::setSubToneType(SubToneType type) {
  setUInt3(Offset::subToneType(), static_cast<unsigned int>(type));
}


Frequency
RT4DCodeplug::ChannelElement::rxFrequency() const {
  return Frequency::fromHz(getUInt32_le(Offset::rxFrequency())*10);
}

void
RT4DCodeplug::ChannelElement::setRxFrequency(const Frequency &frequency) {
  setUInt32_le(Offset::rxFrequency(), frequency.inHz()/10);
}

Frequency
RT4DCodeplug::ChannelElement::txFrequency() const {
  return Frequency::fromHz(getUInt32_le(Offset::txFrequency())*10);
}

void
RT4DCodeplug::ChannelElement::setTxFrequency(const Frequency &frequency) {
  setUInt32_le(Offset::txFrequency(), frequency.inHz()/10);
}


SelectiveCall
RT4DCodeplug::ChannelElement::rxSubTone() const {
  auto code = getUInt16_le(Offset::rxSubtone());
  if (0 == code) return {};

  uint8_t type = code>>12;
  code &= 0x0fff;
  switch (type) {
  case 0: return { double(code)/10 }; // CTCSS frequency.
  case 1: return SelectiveCall::fromBinaryDCS(code, false);
  case 2: return SelectiveCall::fromBinaryDCS(code, true);
  default: break;
  }
  return {};
}

void
RT4DCodeplug::ChannelElement::setRxSubTone(SelectiveCall subTone) {
  if (! subTone.isValid()) {
    setUInt16_le(Offset::rxSubtone(), 0);
  } else if (subTone.isCTCSS()) {
    setUInt16_le(Offset::rxSubtone(), 1<<12 | (subTone.mHz()/100));
  } else {
    setUInt16_le(Offset::rxSubtone(),
      (subTone.isInverted() ? 2 : 1) << 12 | subTone.binCode());
  }
}

SelectiveCall
RT4DCodeplug::ChannelElement::txSubTone() const {
  auto code = getUInt16_le(Offset::txSubtone());
  if (0 == code) return {};

  uint8_t type = code>>12;
  code &= 0x0fff;
  switch (type) {
  case 0: return { double(code)/10 }; // CTCSS frequency.
  case 1: return SelectiveCall::fromBinaryDCS(code, false);
  case 2: return SelectiveCall::fromBinaryDCS(code, true);
  default: break;
  }
  return {};
}

void
RT4DCodeplug::ChannelElement::setTxSubTone(SelectiveCall subTone) {
  if (! subTone.isValid()) {
    setUInt16_le(Offset::txSubtone(), 0);
  } else if (subTone.isCTCSS()) {
    setUInt16_le(Offset::txSubtone(), 1<<12 | (subTone.mHz()/100));
  } else {
    setUInt16_le(Offset::txSubtone(),
      (subTone.isInverted() ? 2 : 1) << 12 | subTone.binCode());
  }
}


unsigned int
RT4DCodeplug::ChannelElement::contactIndex() const {
  return getUInt16_le(Offset::contactIndex());
}

void
RT4DCodeplug::ChannelElement::setContactIndex(unsigned int index) {
  setUInt16_le(Offset::contactIndex(), index);
}


bool
RT4DCodeplug::ChannelElement::hasGroupListIndex() const {
  return 0 != getUInt8(Offset::groupListIndex());
}

unsigned int
RT4DCodeplug::ChannelElement::groupListIndex() const {
  return getUInt8(Offset::groupListIndex())-1;
}

void
RT4DCodeplug::ChannelElement::setGroupListIndex(unsigned int index) {
  setUInt8(Offset::groupListIndex(), index+1);
}

void
RT4DCodeplug::ChannelElement::clearGroupListIndex() {
  setUInt8(Offset::groupListIndex(), 0);
}


bool
RT4DCodeplug::ChannelElement::hasEncryptionKey() const {
  return 0 != getUInt8(Offset::encryptionKeyIndex());
}

unsigned int
RT4DCodeplug::ChannelElement::encryptionKeyIndex() const {
  return getUInt8(Offset::encryptionKeyIndex())-1;
}

void
RT4DCodeplug::ChannelElement::setEncryptionKeyIndex(unsigned int index) {
  setUInt8(Offset::encryptionKeyIndex(), index+1);
}

void
RT4DCodeplug::ChannelElement::clearEncryptionKeyIndex() {
  setUInt8(Offset::encryptionKeyIndex(), 0);
}


unsigned int
RT4DCodeplug::ChannelElement::channelDmrId() const {
  return getUInt32_le(Offset::channelDmrId());
}

void
RT4DCodeplug::ChannelElement::setChannelDmrId(unsigned int index) {
  setUInt32_le(Offset::channelDmrId(), index);
}


unsigned int
RT4DCodeplug::ChannelElement::muteCode() const {
  return getBCD6_le(Offset::muteCode());
}

void
RT4DCodeplug::ChannelElement::setMuteCode(unsigned int code) {
  setBCD6_le(Offset::muteCode(), code);
}

QString
RT4DCodeplug::ChannelElement::name() const {
  return readASCII(Offset::name(), Limit::name(), 0xff);
}

void
RT4DCodeplug::ChannelElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::name(), 0xff);
}


Channel *
RT4DCodeplug::ChannelElement::decode(Context &ctx, const ErrorStack &err) const {
  Channel *channel = nullptr;
  if (ChannelType::FM == channelType()) {
    if (AnalogDemod::FM == analogDemodulation()) {
      auto fm = new FMChannel(); channel = fm;
      fm->setAdmit(fmAdmit());
      fm->setRXTone(rxSubTone());
      fm->setTXTone(txSubTone());
      fm->setBandwidth(bandwidth());
    } else if (AnalogDemod::AM == analogDemodulation()) {
      auto am = new AMChannel(); channel = am;
      am->setRXOnly(true);
    } else {
      errMsg(err) << "SSB channels are not yet supported.";
      return nullptr;
    }
  } else if (ChannelType::DMR == channelType()) {
    auto dmr = new DMRChannel(); channel = dmr;
    dmr->setAdmit(dmrAdmit());
    dmr->setColorCode(colorCode());
    dmr->setTimeSlot(timeSlot());
    if (channelDmrIdEnabled()) {
      auto id = ctx.config()->radioIDs()->find(channelDmrId());
      if (nullptr == id) {
        id = new DMRRadioID("Channel ID", channelDmrId());
        ctx.config()->radioIDs()->add(id);
        ctx.add(id, ctx.count<DMRRadioID>());
      }
      dmr->setRadioId(id);
    }
  }

  if (nullptr == channel)
    return nullptr;

  // Set common channel settings
  channel->setName(name());
  channel->setRXFrequency(rxFrequency());
  channel->setTXFrequency(txFrequency());
  channel->setPower(Channel::Power::High);
  return channel;
}


bool
RT4DCodeplug::ChannelElement::link(Channel *ch, Context &ctx, const ErrorStack &err) const {
  if (ch->is<DMRChannel>()) {
    auto dmr = ch->as<DMRChannel>();
    if (! ctx.has<DMRContact>(contactIndex())) {
      errMsg(err) << "Cannot link channel '" << ch->name()
                  << "': transmit contact index " << contactIndex() << " not defined.";
      return false;
    }
    dmr->setContact(ctx.get<DMRContact>(contactIndex()));

    if (hasGroupListIndex() && !ctx.has<RXGroupList>(groupListIndex())) {
      errMsg(err) << "Cannot link channel '" << ch->name()
                  << "': group list index " << contactIndex() << " not defined.";
      return false;
    }
    dmr->setGroupList(ctx.get<RXGroupList>(groupListIndex()));

    if (hasEncryptionKey() && !ctx.has<EncryptionKey>(encryptionKeyIndex())) {
      errMsg(err) << "Cannot link channel '" << ch->name()
                  << "': encryption key index " << encryptionKeyIndex() << " not defined.";
      //return false;
    }
    //dmr->commercialExtension()->setEncryptionKey(ctx.get<EncryptionKey>(encryptionKeyIndex()));
  }

  return true;
}


bool
RT4DCodeplug::ChannelElement::encode(const Channel *ch, Context &ctx, const ErrorStack &err) {
  if (ch->is<DMRChannel>()) {
    auto dmr = ch->as<DMRChannel>();
    setChannelType(ChannelType::DMR);
    setDmrAdmit(dmr->admit());
    setColorCode(dmr->colorCode());
    setTimeSlot(dmr->timeSlot());
    enableChannelDmrId(! dmr->radioIdRef()->isNull());
    if (! dmr->radioIdRef()->isNull()) {
      setChannelDmrId(dmr->radioId()->number());
    }
    setBandwidth(FMChannel::Bandwidth::Narrow);
  } else if (ch->is<FMChannel>()) {
    auto fm = ch->as<FMChannel>();
    setChannelType(ChannelType::FM);
    setAnalogDemodulation(AnalogDemod::FM);
    setFmAdmit(fm->admit());
    setRxSubTone(fm->rxTone());
    setTxSubTone(fm->txTone());
    setBandwidth(fm->bandwidth());
  } else if (ch->is<AMChannel>()) {
    setChannelType(ChannelType::FM);
    setAnalogDemodulation(AnalogDemod::AM);
    setBandwidth(FMChannel::Bandwidth::Narrow);
  }

  setName(ch->name());
  setRxFrequency(ch->rxFrequency());
  setTxFrequency(ch->txFrequency());

  return true;
}



/* ********************************************************************************************* *
 * Implementation of RT4DCodeplug::ChannelBankElement
 * ********************************************************************************************* */
RT4DCodeplug::ChannelBankElement::ChannelBankElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}


bool
RT4DCodeplug::ChannelBankElement::decode(Context &ctx, const ErrorStack &err) {
  for (unsigned int idx=0; idx < Limit::channels(); idx++) {
    if (! ChannelElement(_data + idx*ChannelElement::size()).isValid())
      continue;
    auto obj = ChannelElement(_data + idx*ChannelElement::size()).decode(ctx, err);
    if (nullptr == obj) {
      errMsg(err) << "Cannot decode channel at index " << idx << ".";
      return false;
    }
    ctx.config()->channelList()->add(obj);
    ctx.add(obj, idx);
  }

  return true;
}


bool
RT4DCodeplug::ChannelBankElement::link(Context &ctx, const ErrorStack &err) {
  for (unsigned int idx=0; idx < Limit::channels(); idx++) {
    if (! ChannelElement(_data + idx*ChannelElement::size()).isValid())
      continue;
    if (! ChannelElement(_data + idx*ChannelElement::size()).link(ctx.get<Channel>(idx), ctx, err)) {
      errMsg(err) << "Cannot link channel '" << ctx.get<Channel>(idx)->name() << "' at index " << idx << ".";
      return false;
    }
  }

  return true;
}


bool
RT4DCodeplug::ChannelBankElement::encode(Context &ctx, const ErrorStack &err) {
  for (unsigned int idx=0; idx < Limit::channels(); idx++) {
    ChannelElement element(_data + idx*ChannelElement::size());
    if (! ctx.has<Channel>(idx)) {
      element.clear();
      continue;
    }
    if (! element.encode(ctx.get<Channel>(idx), ctx, err)) {
      errMsg(err) << "Cannot encode channel " << ctx.get<Channel>(idx)->name()
                  << " at index " << idx;
      return false;
    }
  }

  return true;
}



/* ********************************************************************************************* *
 * Implementation of RT4DCodeplug::SecondSettingsElement
 * ********************************************************************************************* */
RT4DCodeplug::SecondSettingsElement::SecondSettingsElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}


bool
RT4DCodeplug::SecondSettingsElement::keyLockEnabled() const {
  return 0x01 == getUInt8(Offset::enableKeyLock());
}

void
RT4DCodeplug::SecondSettingsElement::enableKeyLock(bool enable) {
  setUInt8(Offset::enableKeyLock(), enable ? 1 : 0);
}


RT4DCodeplug::SecondSettingsElement::ActiveVfo
RT4DCodeplug::SecondSettingsElement::activeVfo() const {
  return static_cast<ActiveVfo>(getUInt8(Offset::activeVFO()));
}

void
RT4DCodeplug::SecondSettingsElement::setActiveVfo(ActiveVfo vfo) {
  setUInt8(Offset::activeVFO(), static_cast<unsigned int>(vfo));
}


bool
RT4DCodeplug::SecondSettingsElement::dualChannelEnabled() const {
  return 0x01 == getUInt8(Offset::enableDoubleWait());
}

void
RT4DCodeplug::SecondSettingsElement::enableDualChannel(bool enable) {
  setUInt8(Offset::enableDoubleWait(), enable ? 1 : 0);
  setUInt8(Offset::enableDualDisplay(), enable ? 1 : 0);
}

RT4DCodeplug::SecondSettingsElement::ScanDirection
RT4DCodeplug::SecondSettingsElement::scanDirection() const {
  return static_cast<ScanDirection>(getUInt8(Offset::scanDirection()));
}

void
RT4DCodeplug::SecondSettingsElement::setScanDirection(ScanDirection direction) {
  setUInt8(Offset::scanDirection(), static_cast<unsigned int>(direction));
}


Frequency
RT4DCodeplug::SecondSettingsElement::vfoStepSize() const {
  switch (static_cast<StepSize>(getUInt8(Offset::vfoStepSize()))) {
    case StepSize::Step_1_25kHz: return Frequency::fromkHz(1.25);
    case StepSize::Step_2_5kHz: return Frequency::fromkHz(2.5);
    case StepSize::Step_5kHz: return Frequency::fromkHz(5.0);
    case StepSize::Step_6_25kHz: return Frequency::fromkHz(6.25);
    case StepSize::Step_10kHz: return Frequency::fromkHz(10.0);
    case StepSize::Step_12_5kHz: return Frequency::fromkHz(12.5);
    case StepSize::Step_20kHz: return Frequency::fromkHz(20.0);
    case StepSize::Step_25kHz: return Frequency::fromkHz(25.0);
    case StepSize::Step_50kHz: return Frequency::fromkHz(50.0);
    case StepSize::Step_100kHz: return Frequency::fromkHz(100.0);
    case StepSize::Step_250Hz: return Frequency::fromkHz(250.0);
    case StepSize::Step_500kHz: return Frequency::fromkHz(500.0);
    case StepSize::Step_1MHz: return Frequency::fromkHz(1000.0);
    case StepSize::Step_5MHz: return Frequency::fromkHz(5000.0);
  }
  return {};
}

void
RT4DCodeplug::SecondSettingsElement::setVfoStepSize(const Frequency &frequency) {
  if (frequency <= Frequency::fromkHz(1.25))
    setUInt8(Offset::vfoStepSize(), static_cast<unsigned int>(StepSize::Step_1_25kHz));
  else if (frequency <= Frequency::fromkHz(2.5))
    setUInt8(Offset::vfoStepSize(), static_cast<unsigned int>(StepSize::Step_2_5kHz));
  else if (frequency <= Frequency::fromkHz(5.0))
    setUInt8(Offset::vfoStepSize(), static_cast<unsigned int>(StepSize::Step_5kHz));
  else if (frequency <= Frequency::fromkHz(6.25))
    setUInt8(Offset::vfoStepSize(), static_cast<unsigned int>(StepSize::Step_6_25kHz));
  else if (frequency <= Frequency::fromkHz(10.0))
    setUInt8(Offset::vfoStepSize(), static_cast<unsigned int>(StepSize::Step_10kHz));
  else if (frequency <= Frequency::fromkHz(12.5))
    setUInt8(Offset::vfoStepSize(), static_cast<unsigned int>(StepSize::Step_12_5kHz));
  else if (frequency <= Frequency::fromkHz(20.0))
    setUInt8(Offset::vfoStepSize(), static_cast<unsigned int>(StepSize::Step_20kHz));
  else if (frequency <= Frequency::fromkHz(25.0))
    setUInt8(Offset::vfoStepSize(), static_cast<unsigned int>(StepSize::Step_25kHz));
  else if (frequency <= Frequency::fromkHz(50.0))
    setUInt8(Offset::vfoStepSize(), static_cast<unsigned int>(StepSize::Step_50kHz));
  else if (frequency <= Frequency::fromkHz(100.0))
    setUInt8(Offset::vfoStepSize(), static_cast<unsigned int>(StepSize::Step_100kHz));
  else if (frequency <= Frequency::fromkHz(250.0))
    setUInt8(Offset::vfoStepSize(), static_cast<unsigned int>(StepSize::Step_250Hz));
  else if (frequency <= Frequency::fromkHz(500.0))
    setUInt8(Offset::vfoStepSize(), static_cast<unsigned int>(StepSize::Step_500kHz));
  else if (frequency <= Frequency::fromkHz(1000.0))
    setUInt8(Offset::vfoStepSize(), static_cast<unsigned int>(StepSize::Step_1MHz));
  else
    setUInt8(Offset::vfoStepSize(), static_cast<unsigned int>(StepSize::Step_5MHz));
}


Frequency
RT4DCodeplug::SecondSettingsElement::spectrumCenterFrequency() const {
  return Frequency::fromHz(getUInt32_le(Offset::spectrumCenterFrequency())*10);
}

void
RT4DCodeplug::SecondSettingsElement::setSpectrumCenterFrequency(const Frequency &frequency) {
  setUInt32_le(Offset::spectrumCenterFrequency(), frequency.inHz()/10);
}

Frequency
RT4DCodeplug::SecondSettingsElement::spectrumFrequencyStep() const {
  return Frequency::fromHz(getUInt32_le(Offset::spectrumFrequencyStep())*10);
}

void
RT4DCodeplug::SecondSettingsElement::setSpectrumFrequencyStep(const Frequency &frequency) {
  setUInt32_le(Offset::spectrumFrequencyStep(), frequency.inHz()/10);
}

unsigned int
RT4DCodeplug::SecondSettingsElement::spectrumThreshold() const {
  return getUInt8(Offset::spectrumThreshold());
}

void
RT4DCodeplug::SecondSettingsElement::setSpectrumThreshold(unsigned int threshold) {
  setUInt8(Offset::spectrumThreshold(), threshold);
}


unsigned int
RT4DCodeplug::SecondSettingsElement::fmBroadcastStandbyChannel() const {
  return getUInt8(Offset::fmBroadcastStandbyChannel());
}

void
RT4DCodeplug::SecondSettingsElement::setFmBroadcastStandbyChannel(unsigned int channel) {
  setUInt8(Offset::fmBroadcastStandbyChannel(), channel);
}

bool
RT4DCodeplug::SecondSettingsElement::fmBroadcastStandbyEnabled() const {
  return 0x01 == getUInt8(Offset::enableFmBroadcastStandby());
}

void
RT4DCodeplug::SecondSettingsElement::enableFmBroadcastStandby(bool enable) {
  setUInt8(Offset::enableFmBroadcastStandby(), enable ? 1 : 0);
}

RT4DCodeplug::SecondSettingsElement::ChannelMode
RT4DCodeplug::SecondSettingsElement::vfoAMode() const {
  return static_cast<ChannelMode>(getUInt8(Offset::vfoAMode()));
}

void
RT4DCodeplug::SecondSettingsElement::setVfoAMode(ChannelMode mode) {
  setUInt8(Offset::vfoAMode(), static_cast<unsigned int>(mode));
}

RT4DCodeplug::SecondSettingsElement::ChannelMode
RT4DCodeplug::SecondSettingsElement::vfoBMode() const {
  return static_cast<ChannelMode>(getUInt8(Offset::vfoBMode()));
}

void
RT4DCodeplug::SecondSettingsElement::setVfoBMode(ChannelMode mode) {
  setUInt8(Offset::vfoBMode(), static_cast<unsigned int>(mode));
}

RT4DCodeplug::SecondSettingsElement::ChannelDisplayMode
RT4DCodeplug::SecondSettingsElement::vfoADisplayMode() const {
  return static_cast<ChannelDisplayMode>(getUInt8(Offset::vfoAChannelDisplayMode()));
}

void
RT4DCodeplug::SecondSettingsElement::setVfoADisplayMode(ChannelDisplayMode mode) {
  setUInt8(Offset::vfoAChannelDisplayMode(), static_cast<unsigned int>(mode));
}

RT4DCodeplug::SecondSettingsElement::ChannelDisplayMode
RT4DCodeplug::SecondSettingsElement::vfoBDisplayMode() const {
  return static_cast<ChannelDisplayMode>(getUInt8(Offset::vfoBChannelDisplayMode()));
}

void
RT4DCodeplug::SecondSettingsElement::setVfoBDisplayMode(ChannelDisplayMode mode) {
  setUInt8(Offset::vfoBChannelDisplayMode(), static_cast<unsigned int>(mode));
}

unsigned int
RT4DCodeplug::SecondSettingsElement::vfoAZoneIndex() const {
  return getUInt8(Offset::vfoAZoneIndex());
}

void
RT4DCodeplug::SecondSettingsElement::setVfoAZoneIndex(unsigned int index) {
  setUInt8(Offset::vfoAZoneIndex(), index);
}

unsigned int
RT4DCodeplug::SecondSettingsElement::vfoBZoneIndex() const {
  return getUInt8(Offset::vfoBZoneIndex());
}

void
RT4DCodeplug::SecondSettingsElement::setVfoBZoneIndex(unsigned int index) {
  setUInt8(Offset::vfoBZoneIndex(), index);
}

unsigned int
RT4DCodeplug::SecondSettingsElement::vfoAChannelIndex() const {
  return getUInt16_le(Offset::vfoAChannelIndex());
}

void
RT4DCodeplug::SecondSettingsElement::setVfoAChannelIndex(unsigned int index) {
  setUInt16_le(Offset::vfoAChannelIndex(), index);
}

unsigned int
RT4DCodeplug::SecondSettingsElement::vfoBChannelIndex() const {
  return getUInt16_le(Offset::vfoBChannelIndex());
}

void
RT4DCodeplug::SecondSettingsElement::setVfoBChannelIndex(unsigned int index) {
  setUInt16_le(Offset::vfoBChannelIndex(), index);
}


bool
RT4DCodeplug::SecondSettingsElement::subPttEnabled() const {
  return 0x01 == getUInt8(Offset::enableSubPtt());
}

void
RT4DCodeplug::SecondSettingsElement::enableSubPtt(bool enabled) {
  setUInt8(Offset::enableSubPtt(), enabled ? 1 : 0);
}


RT4DCodeplug::SecondSettingsElement::KeyFunction
RT4DCodeplug::SecondSettingsElement::sideKey1Short() const {
  return static_cast<KeyFunction>(getUInt8(Offset::sideKey1Short()));
}

void
RT4DCodeplug::SecondSettingsElement::setSideKey1Short(KeyFunction function) {
  setUInt8(Offset::sideKey1Short(), static_cast<unsigned char>(function));
}

RT4DCodeplug::SecondSettingsElement::KeyFunction
RT4DCodeplug::SecondSettingsElement::sideKey1Long() const {
  return static_cast<KeyFunction>(getUInt8(Offset::sideKey1Long()));
}

void
RT4DCodeplug::SecondSettingsElement::setSideKey1Long(KeyFunction function) {
  setUInt8(Offset::sideKey1Long(), static_cast<unsigned char>(function));
}

RT4DCodeplug::SecondSettingsElement::KeyFunction
RT4DCodeplug::SecondSettingsElement::sideKey2Short() const {
  return static_cast<KeyFunction>(getUInt8(Offset::sideKey2Short()));
}

void
RT4DCodeplug::SecondSettingsElement::setSideKey2Short(KeyFunction function) {
  setUInt8(Offset::sideKey2Short(), static_cast<unsigned char>(function));
}

RT4DCodeplug::SecondSettingsElement::KeyFunction
RT4DCodeplug::SecondSettingsElement::sideKey2Long() const {
  return static_cast<KeyFunction>(getUInt8(Offset::sideKey2Long()));
}

void
RT4DCodeplug::SecondSettingsElement::setSideKey2Long(KeyFunction function) {
  setUInt8(Offset::sideKey2Long(), static_cast<unsigned char>(function));
}

RT4DCodeplug::SecondSettingsElement::KeyFunction
RT4DCodeplug::SecondSettingsElement::quickKey(unsigned int key) const {
  return static_cast<KeyFunction>(getUInt8(Offset::quickKeySettings()+key));
}

void
RT4DCodeplug::SecondSettingsElement::setQuickKey(unsigned int key, KeyFunction function) {
  setUInt8(Offset::quickKeySettings()+key, static_cast<unsigned char>(function));
}


bool
RT4DCodeplug::SecondSettingsElement::decode(Context &ctx, const ErrorStack &errStack) {
  return true;
}

bool
RT4DCodeplug::SecondSettingsElement::link(Context &ctx, const ErrorStack &err) {
  ctx.config()->settings()->boot()->enableDefaultChannel(
    ctx.has<Zone>(vfoAZoneIndex()) && ctx.has<Channel>(vfoAChannelIndex()) &&
    ctx.has<Zone>(vfoBZoneIndex()) && ctx.has<Channel>(vfoBChannelIndex()) );

  if (ctx.has<Zone>(vfoAZoneIndex()) && ctx.has<Channel>(vfoAChannelIndex())) {
    ctx.config()->settings()->boot()->zoneA()->set(ctx.get<Zone>(vfoAZoneIndex()));
    ctx.config()->settings()->boot()->channelA()->set(ctx.get<Channel>(vfoAChannelIndex()));
  }

  if (ctx.has<Zone>(vfoBZoneIndex()) && ctx.has<Channel>(vfoBChannelIndex())) {
    ctx.config()->settings()->boot()->zoneB()->set(ctx.get<Zone>(vfoBZoneIndex()));
    ctx.config()->settings()->boot()->channelB()->set(ctx.get<Channel>(vfoBChannelIndex()));
  }

  return true;
}

bool
RT4DCodeplug::SecondSettingsElement::encode(Context &ctx, const ErrorStack &errStack) {
  if (ctx.config()->settings()->boot()->defaultChannelEnabled()
      && !ctx.config()->settings()->boot()->zoneA()->isNull()
      && !ctx.config()->settings()->boot()->channelA()->isNull()) {
    setVfoAZoneIndex(ctx.index(ctx.config()->settings()->boot()->zoneA()->as<Zone>()));
    setVfoAChannelIndex(ctx.index(ctx.config()->settings()->boot()->channelA()->as<Channel>()));
  }
  if (ctx.config()->settings()->boot()->defaultChannelEnabled()
      && !ctx.config()->settings()->boot()->zoneB()->isNull()
      && !ctx.config()->settings()->boot()->channelB()->isNull()) {
    setVfoBZoneIndex(ctx.index(ctx.config()->settings()->boot()->zoneB()->as<Zone>()));
    setVfoBChannelIndex(ctx.index(ctx.config()->settings()->boot()->channelB()->as<Channel>()));
  }

  return true;
}



/* ********************************************************************************************* *
 * Implementation of RT4DCodeplug::ZoneElement
 * ********************************************************************************************* */
RT4DCodeplug::ZoneElement::ZoneElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}

void
RT4DCodeplug::ZoneElement::clear() {
  memset(_data, 0xff, size());
}

bool
RT4DCodeplug::ZoneElement::isValid() const {
  return !name().isEmpty();
}


unsigned int
RT4DCodeplug::ZoneElement::defaultChannelA() const {
  return getUInt16_le(Offset::defaultChannelA());
}

void
RT4DCodeplug::ZoneElement::setDefaultChannelA(unsigned int channel) {
  setUInt16_le(Offset::defaultChannelA(), channel);
}

unsigned int
RT4DCodeplug::ZoneElement::defaultChannelB() const {
  return getUInt16_le(Offset::defaultChannelB());
}

void
RT4DCodeplug::ZoneElement::setDefaultChannelB(unsigned int channel) {
  setUInt16_le(Offset::defaultChannelB(), channel);
}


QString
RT4DCodeplug::ZoneElement::name() const {
  return readASCII(Offset::name(), Limit::name(), 0xff);
}

void
RT4DCodeplug::ZoneElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::name(), 0xff);
}


bool
RT4DCodeplug::ZoneElement::hasChannelIndex(unsigned int index) const {
  return 0xffff != getUInt16_le(Offset::channels() + 2*index);
}

unsigned int
RT4DCodeplug::ZoneElement::channelIndex(unsigned int index) const {
  return getUInt16_le(Offset::channels() + 2*index);
}

void
RT4DCodeplug::ZoneElement::setChannelIndex(unsigned int index, unsigned int channel) {
  setUInt16_le(Offset::channels() + 2*index, channel);
}

void
RT4DCodeplug::ZoneElement::clearChannelIndex(unsigned int index) {
  setUInt16_le(Offset::channels() + 2*index, 0xffff);
}


bool
RT4DCodeplug::ZoneElement::encode(const Zone *zone, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err);

  clear();
  setName(zone->name());
  for (int idx = 0; idx < zone->A()->count(); idx++) {
    setChannelIndex(idx, ctx.index(zone->A()->get(idx)->as<Channel>()));
  }

  return true;
}

Zone *
RT4DCodeplug::ZoneElement::decode(Context &ctx, const ErrorStack &err) const {
  Q_UNUSED(ctx); Q_UNUSED(err);
  return new Zone(name());
}

bool
RT4DCodeplug::ZoneElement::link(Zone *zone, Context &ctx, const ErrorStack &err) const {
  for (unsigned int idx = 0; idx < Limit::channels(); idx++) {
    if (! hasChannelIndex(idx))
      continue;
    if (! ctx.has<Channel>(channelIndex(idx))) {
      errMsg(err) << "Cannot link zone '" << zone->name()
                  << "': channel index " << channelIndex(idx) << " not defined.";
      return false;
    }
    zone->A()->add(ctx.get<Channel>(channelIndex(idx)));
  }
  return true;
}



/* ********************************************************************************************* *
 * Implementation of RT4DCodeplug::ZoneBankElement
 * ********************************************************************************************* */
RT4DCodeplug::ZoneBankElement::ZoneBankElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}

void
RT4DCodeplug::ZoneBankElement::clear() {
  for (unsigned int idx = 0; idx < Limit::zones(); idx++) {
    ZoneElement(_data + Offset::zones() + idx*ZoneElement::size()).clear();
  }
}

bool
RT4DCodeplug::ZoneBankElement::encode(Context &ctx, const ErrorStack &err) {
  clear();

  for (unsigned int idx = 0; idx < ctx.count<Zone>(); idx++) {
    if (! ZoneElement(_data + Offset::zones() + idx*ZoneElement::size()).encode(ctx.get<Zone>(idx), ctx, err)) {
      errMsg(err) << "Cannot encode zone '" << ctx.get<Zone>(idx)->name() << "' at index " << idx << ".";
      return false;
    }
  }

  return true;
}

bool
RT4DCodeplug::ZoneBankElement::decode(Context &ctx, const ErrorStack &err) const {
  for (unsigned int idx = 0; idx < Limit::zones(); idx++) {
    ZoneElement el(_data + Offset::zones() + idx*ZoneElement::size());
    if (! el.isValid())
      continue;
    auto zone = el.decode(ctx, err);
    if (nullptr == zone) {
      errMsg(err) << "Cannot decode zone at index " << idx << ".";
      return false;
    }
    ctx.config()->zones()->add(zone);
    ctx.add(zone, idx);
  }

  return true;
}

bool
RT4DCodeplug::ZoneBankElement::link(Context &ctx, const ErrorStack &err) const {
  for (unsigned int idx = 0; idx < Limit::zones(); idx++) {
    ZoneElement el(_data + Offset::zones() + idx*ZoneElement::size());
    if (! el.isValid())
      continue;
    if (! el.link(ctx.get<Zone>(idx), ctx, err)) {
      errMsg(err) << "Cannot link zone '" << ctx.get<Zone>(idx) << "' at index " << idx << ".";
      return false;
    }
  }
  return true;
}



/* ********************************************************************************************* *
 * Implementation of RT4DCodeplug::ContactElement
 * ********************************************************************************************* */
RT4DCodeplug::ContactElement::ContactElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}

void
RT4DCodeplug::ContactElement::clear() {
  memset(_data, 0xff, size());
}

bool
RT4DCodeplug::ContactElement::isValid() const {
  return !name().isEmpty();
}


RT4DCodeplug::ContactElement::Type
RT4DCodeplug::ContactElement::type() const {
  return static_cast<Type>(getUInt8(Offset::type()));
}

void
RT4DCodeplug::ContactElement::setType(Type type) {
  setUInt8(Offset::type(), static_cast<unsigned int>(type));
}


bool
RT4DCodeplug::ContactElement::hasAllCallId() const {
  return 0xaaaaaaaa == getUInt32_le(Offset::id());
}

unsigned int
RT4DCodeplug::ContactElement::id() const {
  return getBCD8_le(Offset::id());
}

void
RT4DCodeplug::ContactElement::setId(unsigned int id) {
  setBCD8_le(Offset::id(), id);
}

void
RT4DCodeplug::ContactElement::setAllCallId() {
  setUInt32_le(Offset::id(), 0xaaaaaaaa);
}


QString
RT4DCodeplug::ContactElement::name() const {
  return readASCII(Offset::name(), Limit::name(), 0xff);
}

void
RT4DCodeplug::ContactElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::name(), 0xff);
}


bool
RT4DCodeplug::ContactElement::encode(const DMRContact *contact, Context &ctx, const ErrorStack &err) {
  setId(contact->number());
  switch (contact->type()) {
  case DMRContact::PrivateCall:
    setType(Type::PrivateCall);
    break;
  case DMRContact::GroupCall:
    setType(Type::GroupCall);
    break;
  case DMRContact::AllCall:
    setType(Type::AllCall);
    setAllCallId();
    break;
  }

  setName(contact->name());

  return true;
}


DMRContact *
RT4DCodeplug::ContactElement::decode(Context &ctx, const ErrorStack &err) const {
  switch (type()) {
    case Type::PrivateCall:
      return new DMRContact(DMRContact::Type::PrivateCall, name(), id());
    case Type::GroupCall:
      return new DMRContact(DMRContact::Type::GroupCall, name(), id());
    case Type::AllCall:
      return new DMRContact(DMRContact::Type::AllCall, name(), 16777215);
  }

  errMsg(err) << "Unknown call type.";
  return nullptr;
}



/* ********************************************************************************************* *
 * Implementation of RT4DCodeplug::ContactBankElement
 * ********************************************************************************************* */
RT4DCodeplug::ContactBankElement::ContactBankElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}

void
RT4DCodeplug::ContactBankElement::clear() {
  for (unsigned int i = 0; i < Limit::contacts(); i++) {
    ContactElement(_data + Offset::contacts() + i*ContactElement::size()).clear();
  }
}

bool
RT4DCodeplug::ContactBankElement::encode(Context &ctx, const ErrorStack &err) {
  clear();

  for (unsigned int i = 0; i < ctx.count<DMRContact>(); i++) {
    ContactElement el(_data + Offset::contacts() + i*ContactElement::size());
    if (! el.encode(ctx.get<DMRContact>(i), ctx, err)) {
      errMsg(err) << "Cannot encode contact '" << ctx.get<DMRContact>(i)->name()
                  << "' at index " << i << ".";
      return false;
    }
  }

  return true;
}

bool
RT4DCodeplug::ContactBankElement::decode(Context &ctx, const ErrorStack &err) const {
  for (unsigned int i = 0; i < Limit::contacts(); i++) {
    ContactElement el(_data + Offset::contacts() + i*ContactElement::size());
    if (! el.isValid())
      continue;
    auto contact = el.decode(ctx, err);
    if (nullptr == contact) {
      errMsg(err) << "Cannot decode contact at index " << i << ".";
      return false;
    }
    ctx.config()->contacts()->add(contact);
    ctx.add(contact, i);
  }

  return true;
}



/* ********************************************************************************************* *
 * Implementation of RT4DCodeplug::GroupListElement
 * ********************************************************************************************* */
RT4DCodeplug::GroupListElement::GroupListElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}

void
RT4DCodeplug::GroupListElement::clear() {
  memset(_data, 0xff, size());
}

bool
RT4DCodeplug::GroupListElement::isValid() const {
  return !name().isEmpty();
}

QString
RT4DCodeplug::GroupListElement::name() const {
  return readASCII(Offset::name(), Limit::name(), 0xff);
}

void
RT4DCodeplug::GroupListElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::name(), 0xff);
}


bool
RT4DCodeplug::GroupListElement::hasGroupCallIndex(unsigned int index) const {
  return 0xffff != getUInt16_le(Offset::groupCalls() + index*2);
}

unsigned int
RT4DCodeplug::GroupListElement::groupCallIndex(unsigned int index) const {
  return getUInt16_le(Offset::groupCalls() + index*2);
}

void
RT4DCodeplug::GroupListElement::setGroupCallIndex(unsigned int index, unsigned int groupCallIndex) {
  setUInt16_le(Offset::groupCalls() + index*2, groupCallIndex);
}

void
RT4DCodeplug::GroupListElement::clearGroupCallIndex(unsigned int index) {
  setUInt16_le(Offset::groupCalls() + index*2, 0xffff);
}


bool
RT4DCodeplug::GroupListElement::encode(const RXGroupList *lst, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err);
  clear();
  setName(lst->name());
  for (int i = 0; i < std::min(lst->count(), (int)Limit::groupCalls()); i++) {
    setGroupCallIndex(i, ctx.index(lst->contact(i)));
  }
  return true;
}

RXGroupList *
RT4DCodeplug::GroupListElement::decode(Context &ctx, const ErrorStack &err) const {
  return new RXGroupList(name());
}

bool
RT4DCodeplug::GroupListElement::link(RXGroupList *lst, Context &ctx, const ErrorStack &err) const {
  for (unsigned int i = 0; i < Limit::groupCalls(); i++) {
    if (! hasGroupCallIndex(i))
      continue;
    if (! ctx.has<DMRContact>(hasGroupCallIndex(i))) {
      errMsg(err) << "Cannot link group list " << lst->name()
                  << ", DMR contact index " << hasGroupCallIndex(i) << " not defined.";
      return false;
    }
    lst->addContact(ctx.get<DMRContact>(groupCallIndex(i)));
  }
  return true;
}


/* ********************************************************************************************* *
 * Implementation of RT4DCodeplug::GroupListBankElement
 * ********************************************************************************************* */
RT4DCodeplug::GroupListBankElement::GroupListBankElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}

void
RT4DCodeplug::GroupListBankElement::clear() {
  for (unsigned int i = 0; i < Limit::groupLists(); i++) {
    GroupListElement(_data + Offset::groupLists() + i*GroupListElement::size()).clear();
  }
}

bool
RT4DCodeplug::GroupListBankElement::encode(Context &ctx, const ErrorStack &err) {
  clear();

  for (unsigned int i = 0; i < ctx.count<RXGroupList>(); i++) {
    GroupListElement el(_data + Offset::groupLists() + i*GroupListElement::size());
    if (! el.encode(ctx.get<RXGroupList>(i), ctx, err)) {
      errMsg(err) << "Cannot encode group list '" << ctx.get<RXGroupList>(i)->name()
                  << "' at index " << i << ".";
      return false;
    }
  }

  return true;
}


bool
RT4DCodeplug::GroupListBankElement::decode(Context &ctx, const ErrorStack &err) const {
  for (unsigned int i = 0; i < Limit::groupLists(); i++) {
    GroupListElement el(_data + Offset::groupLists() + i*GroupListElement::size());
    if (! el.isValid())
      continue;
    auto lst = el.decode(ctx, err);
    if (nullptr == lst) {
      errMsg(err) << "Cannot decode group list at index " << i << ".";
      return false;
    }
    ctx.config()->rxGroupLists()->add(lst);
    ctx.add(lst, i);
  }

  return true;
}


bool
RT4DCodeplug::GroupListBankElement::link(Context &ctx, const ErrorStack &err) const {
  for (unsigned int i = 0; i < Limit::groupLists(); i++) {
    GroupListElement el(_data + Offset::groupLists() + i*GroupListElement::size());
    if (! el.isValid())
      continue;
    if (! el.link(ctx.get<RXGroupList>(i), ctx, err)) {
      errMsg(err) << "Cannot link group list '" << ctx.get<RXGroupList>(i)->name()
                  << "' at index " << i << ".";
      return false;
    }
  }
  return true;
}



/* ********************************************************************************************* *
 * Implementation of RT4DCodeplug::EncryptionKeyElement
 * ********************************************************************************************* */
RT4DCodeplug::EncryptionKeyElement::EncryptionKeyElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}

void
RT4DCodeplug::EncryptionKeyElement::clear() {
  memset(_data, 0xff, sizeof(_data));
}

bool
RT4DCodeplug::EncryptionKeyElement::isValid() const {
  return ! name().isEmpty();
}

unsigned int
RT4DCodeplug::EncryptionKeyElement::keyId() const {
  return getUInt8(Offset::id());
}

void
RT4DCodeplug::EncryptionKeyElement::setKeyId(unsigned int keyId) {
  setUInt8(Offset::id(), keyId);
}

RT4DCodeplug::EncryptionKeyElement::Type
RT4DCodeplug::EncryptionKeyElement::type() const {
  return static_cast<Type>(getUInt8(Offset::type()));
}

void
RT4DCodeplug::EncryptionKeyElement::setType(Type type) {
  setUInt8(Offset::type(), static_cast<unsigned int>(type));
}

QString
RT4DCodeplug::EncryptionKeyElement::name() const {
  return readASCII(Offset::name(), Limit::name(), 0xff);
}

void
RT4DCodeplug::EncryptionKeyElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::name(), 0xff);
}

QByteArray
RT4DCodeplug::EncryptionKeyElement::key() const {
  switch (type()) {
  case Type::ARC4:   return {(const char *)_data+Offset::key(), 8};
  case Type::AES128: return {(const char *)_data+Offset::key(), 16};
  case Type::AES256: return {(const char *)_data+Offset::key(), 32};
  }
  return {};
}

void
RT4DCodeplug::EncryptionKeyElement::setKey(const QByteArray &key) {
  auto len = std::min((qsizetype)64, key.length());
  memcpy(_data+Offset::key(), key.constData(), len);
}

bool
RT4DCodeplug::EncryptionKeyElement::encode(const EncryptionKey *key, Context &ctx, const ErrorStack &err) {
  clear();
  if (key->is<ARC4EncryptionKey>()) {
    setType(Type::ARC4);
  } else if (key->is<AESEncryptionKey>()) {
    if (16 == key->key().size()) {
      setType(Type::AES128);
    } else {
      setType(Type::AES256);
    }
  }
  setKey(key->key());
  setName(key->name());
  return true;
}

EncryptionKey *
RT4DCodeplug::EncryptionKeyElement::decode(Context &ctx, const ErrorStack &err) const {
  EncryptionKey *key = nullptr;
  if (Type::ARC4 == type()) {
    key = new ARC4EncryptionKey();
  } else if (Type::AES128 == type() || Type::AES256 == type()) {
    key = new AESEncryptionKey();
  }
  key->setKey(this->key());
  key->setName(this->name());
  return key;
}



/* ********************************************************************************************* *
 * Implementation of RT4DCodeplug::EncryptionKeyBankElement
 * ********************************************************************************************* */
RT4DCodeplug::EncryptionKeyBankElement::EncryptionKeyBankElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}

void
RT4DCodeplug::EncryptionKeyBankElement::clear() {
  for (unsigned i = 0; i < Limit::keys(); i++) {
    EncryptionKeyElement(_data + Offset::keys() + i*EncryptionKeyElement::size()).clear();
  }
}

bool
RT4DCodeplug::EncryptionKeyBankElement::encode(Context &ctx, const ErrorStack &err) {
  clear();
  for (unsigned i = 0; i < ctx.count<EncryptionKey>(); i++) {
    EncryptionKeyElement el(_data + Offset::keys() + i*EncryptionKeyElement::size());
    if (! el.encode(ctx.get<EncryptionKey>(i), ctx, err)) {
      errMsg(err) << "Cannot encode encryption key '" << ctx.get<EncryptionKey>(i)
                  << "' at index " << i << ".";
      return false;
    }
    el.setKeyId(i);
  }
  return true;
}

bool
RT4DCodeplug::EncryptionKeyBankElement::decode(Context &ctx, const ErrorStack &err) const {
  for (unsigned i = 0; i < Limit::keys(); i++) {
    EncryptionKeyElement el(_data + Offset::keys() + i*EncryptionKeyElement::size());
    if (! el.isValid())
      continue;
    auto key = el.decode(ctx, err);
    if (nullptr == key) {
      errMsg(err) << "Cannot decode key at index " << i << ".";
      return false;
    }
    ctx.config()->commercialExtension()->encryptionKeys()->add(key);
    ctx.add(key, i);
  }

  return true;
}



/* ********************************************************************************************* *
 * Implementation of RT4DCodeplug::MessageElement
 * ********************************************************************************************* */
RT4DCodeplug::MessageElement::MessageElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}

void
RT4DCodeplug::MessageElement::clear() {
  memset(_data, 0xff, size());
}

bool
RT4DCodeplug::MessageElement::isValid() const {
  return indexIsValid();
}

bool
RT4DCodeplug::MessageElement::indexIsValid() const {
  return 0xff != getUInt8(Offset::index());
}

unsigned int
RT4DCodeplug::MessageElement::index() const {
  return getUInt8(Offset::index());
}

void
RT4DCodeplug::MessageElement::setIndex(unsigned int index) {
  setUInt8(Offset::index(), index);
}

void
RT4DCodeplug::MessageElement::clearIndex() {
  setUInt8(Offset::index(), 0xff);
}

QString
RT4DCodeplug::MessageElement::text() const {
  return readASCII(Offset::text(), Limit::text(), 0xff);
}

void
RT4DCodeplug::MessageElement::setText(const QString &text) {
  writeASCII(Offset::text(), text, Limit::text(), 0xff);
}

bool
RT4DCodeplug::MessageElement::encode(const SMSTemplate *message, Context &ctx, const ErrorStack &err) {
  clear();
  setText(message->message());
  return true;
}

SMSTemplate *RT4DCodeplug::MessageElement::decode(Context &ctx, const ErrorStack &err) const {
  auto msg = new SMSTemplate();
  msg->setMessage(text());
  return msg;
}



/* ********************************************************************************************* *
 * Implementation of RT4DCodeplug::MessageBankElement
 * ********************************************************************************************* */
RT4DCodeplug::MessageBankElement::MessageBankElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}

void
RT4DCodeplug::MessageBankElement::clear() {
  for (unsigned i = 0; i < Limit::messages(); i++) {
    MessageElement(_data+Offset::messages() + i*MessageElement::size()).clear();
  }
}

bool
RT4DCodeplug::MessageBankElement::encode(Context &ctx, const ErrorStack &err) {
  clear();

  for (unsigned i = 0; i < ctx.count<SMSTemplate>(); i++) {
    MessageElement el(_data + Offset::messages() + i*MessageElement::size());
    if (! el.encode(ctx.get<SMSTemplate>(i), ctx, err)) {
      errMsg(err) << "Cannot encode SMS template at index " << i << ".";
      return false;
    }
    el.setIndex(i);
  }

  return true;
}

bool
RT4DCodeplug::MessageBankElement::decode(Context &ctx, const ErrorStack &err) const {
  for (unsigned i = 0; i < Limit::messages(); i++) {
    MessageElement el(_data + Offset::messages() + i*MessageElement::size());
    if (! el.isValid())
      continue;
    auto msg = el.decode(ctx, err);
    if (nullptr == msg) {
      errMsg(err) << "Cannot decode SMS template at index " << i << ".";
      return false;
    }
    ctx.config()->smsExtension()->smsTemplates()->add(msg);
    ctx.add(msg, i);
  }

  return true;
}



/* ********************************************************************************************* *
 * Implementation of RT4DCodeplug
 * ********************************************************************************************* */
RT4DCodeplug::RT4DCodeplug(QObject *parent)
  : Codeplug(parent)
{
  // Preallocate entrie codeplug
  addImage("Radtel RT4D Codeplug, FW 3.25");
  image(0).addElement(Offset::firstSettings(), FirstSettingsElement::size());
  image(0).addElement(Offset::channels(), ChannelBankElement::size());
  image(0).addElement(Offset::secondSettings(), SecondSettingsElement::size());
  image(0).addElement(Offset::zones(), ZoneBankElement::size());
  image(0).addElement(Offset::contacts(), ContactBankElement::size());
  image(0).addElement(Offset::groupLists(), GroupListBankElement::size());
  image(0).addElement(Offset::keys(), EncryptionKeyBankElement::size());
  image(0).addElement(Offset::messages(), MessageBankElement::size());
  image(0).addElement(Offset::fmBroadcast(), FMBroadcastChannelBankElement::size());
}


Config *
RT4DCodeplug::preprocess(Config *config, const ErrorStack &err) const {
  Config *copy = Codeplug::preprocess(config, err);
  if (nullptr == copy) {
    errMsg(err) << "Cannot pre-process RT-4D codeplug.";
    return nullptr;
  }

  // Add a all-call as first DMR contact if not present.
  bool hasFirstAllCall = false;
  for (int i=0; i<copy->contacts()->count(); i++) {
    if (Contact *cont = copy->contacts()->get(i)->as<DMRContact>()) {
      hasFirstAllCall = DMRContact::Type::AllCall == cont->as<DMRContact>()->type();
      break;
    }
  }
  if (! hasFirstAllCall) {
    copy->contacts()->add(new DMRContact(
      DMRContact::AllCall, "All Call", 16777215), 0);
  }

  // Remove all M17 channels
  ObjectFilterVisitor m17Filter{M17Channel::staticMetaObject};
  if (! m17Filter.process(copy, err)) {
    errMsg(err) << "Remove M17 channels.";
    delete copy;
    return nullptr;
  }

  // Keep only ARC4, AES (128 and 256)
  EncryptionKeyFilterVisitor encFilter{
    EncryptionKeyFilterVisitor::Filter(ARC4EncryptionKey::staticMetaObject, 40),
    EncryptionKeyFilterVisitor::Filter(AESEncryptionKey::staticMetaObject, 128),
    EncryptionKeyFilterVisitor::Filter(AESEncryptionKey::staticMetaObject, 256)
  };
  if (! encFilter.process(copy, err)) {
    errMsg(err) << "Clear encryption keys.";
    errMsg(err) << "Cannot pre-process DM32UV codeplug.";
    delete copy;
    return nullptr;
  }

  // Split dual-zones into two.
  ZoneSplitVisitor splitter;
  if (! splitter.process(copy, err)) {
    errMsg(err) << "Cannot split dual VFO zones.";
    errMsg(err) << "Cannot pre-process DM32UV codeplug.";
    delete copy;
    return nullptr;
  }

  return copy;
}

bool
RT4DCodeplug::postprocess(Config *config, const ErrorStack &err) const {
  if (! Codeplug::postprocess(config, err)) {
    errMsg(err) << "Cannot post-process RT-4D codeplug.";
    return false;
  }

  // Merge split zones into one.
  ZoneMergeVisitor merger;
  if (! merger.process(config, err)) {
    errMsg(err) << "Cannot merge zones.";
    return false;
  }

  return true;
}


bool
RT4DCodeplug::index(Config *config, Context &ctx, const ErrorStack &err) const {
  Q_UNUSED(err)

  // All indices as 0-based. That is, the first channel gets index 0 etc.

  // Map DMR contacts
  for (int i=0, d=0; i<config->contacts()->count(); i++) {
    if (config->contacts()->contact(i)->is<DMRContact>()) {
      ctx.add(config->contacts()->contact(i)->as<DMRContact>(), d); d++;
    }
  }

  // Map rx group lists
  for (int i=0; i<config->rxGroupLists()->count(); i++)
    ctx.add(config->rxGroupLists()->list(i), i);

  // Map channels
  for (int i=0; i<config->channelList()->count(); i++)
    ctx.add(config->channelList()->channel(i), i);

  // Map zones
  for (int i=0; i<config->zones()->count(); i++)
    ctx.add(config->zones()->zone(i), i);

  // Map scan lists
  for (int i=0; i<config->scanlists()->count(); i++)
    ctx.add(config->scanlists()->scanlist(i), i);

  // Map encryption keys
  if (config->commercialExtension()) {
    for (int i=0; i<config->commercialExtension()->encryptionKeys()->count(); i++)
      ctx.add(config->commercialExtension()->encryptionKeys()->key(i), i);
  }
  return true;
}


bool
RT4DCodeplug::encode(Config *config, const Flags &flags, const ErrorStack &err) {
  Q_UNUSED(flags);

  Context ctx(config);
  ctx.remTable(&DigitalContact::staticMetaObject, true);
  ctx.addTable(&DMRContact::staticMetaObject);
  ctx.addTable(&DMRContact::staticMetaObject);
  ctx.remTable(&BasicEncryptionKey::staticMetaObject, true);
  ctx.remTable(&ARC4EncryptionKey::staticMetaObject, true);
  ctx.remTable(&AESEncryptionKey::staticMetaObject, true);
  ctx.addTable(&EncryptionKey::staticMetaObject);
  if (! index(config, ctx, err)) {
    errMsg(err) << "Index elements.";
    return false;
  }

  if (! encodeElements(ctx, err)) {
    errMsg(err) << "Cannot encode codeplug.";
    return false;
  }

  return true;
}


bool
RT4DCodeplug::decode(Config *config, const ErrorStack &err) {
  Context ctx(config);
  // Remove tables for each encryption method
  ctx.remTable(&BasicEncryptionKey::staticMetaObject, true);
  ctx.remTable(&ARC4EncryptionKey::staticMetaObject, true);
  ctx.remTable(&AESEncryptionKey::staticMetaObject, true);
  // Add common index table for all encryption keys.
  ctx.addTable(&EncryptionKey::staticMetaObject);

  if (! decodeElements(ctx, err)) {
    errMsg(err) << "Cannot decode elements.";
    return false;
  }

  if (! linkElements(ctx, err)) {
    errMsg(err) << "Cannot decode elements.";
    return false;
  }

  return true;
}



bool
RT4DCodeplug::decodeElements(Context &ctx, const ErrorStack &err) {
  if (! FirstSettingsElement(data(Offset::firstSettings())).decode(ctx, err)) {
    errMsg(err) << "Cannot decode first settings element.";
    return false;
  }

  if (! ChannelBankElement(data(Offset::channels())).decode(ctx, err)) {
    errMsg(err) << "Cannot decode channels.";
    return false;
  }

  if (! SecondSettingsElement(data(Offset::secondSettings())).decode(ctx, err)) {
    errMsg(err) << "Cannot decode second settings element.";
    return false;
  }

  if (! ZoneBankElement(data(Offset::zones())).decode(ctx, err)) {
    errMsg(err) << "Cannot decode zones.";
    return false;
  }

  if (! ContactBankElement(data(Offset::contacts())).decode(ctx, err)) {
    errMsg(err) << "Cannot decode contacts.";
    return false;
  }

  if (! GroupListBankElement(data(Offset::groupLists())).decode(ctx, err)) {
    errMsg(err) << "Cannot decode group lists.";
    return false;
  }

  if (! EncryptionKeyBankElement(data(Offset::keys())).decode(ctx, err)) {
    errMsg(err) << "Cannot decode keys.";
    return false;
  }

  if (! MessageBankElement(data(Offset::messages())).decode(ctx, err)) {
    errMsg(err) << "Cannot decode messages.";
    return false;
  }

  return true;
}


bool
RT4DCodeplug::linkElements(Context &ctx, const ErrorStack &err) {
  if (! ChannelBankElement(data(Offset::channels())).link(ctx, err)) {
    errMsg(err) << "Cannot link channels.";
    return false;
  }

  if (! SecondSettingsElement(data(Offset::secondSettings())).link(ctx, err)) {
    errMsg(err) << "Cannot link second settings element.";
    return false;
  }

  if (! ZoneBankElement(data(Offset::zones())).link(ctx, err)) {
    errMsg(err) << "Cannot link zones.";
    return false;
  }

  if (! GroupListBankElement(data(Offset::groupLists())).link(ctx, err)) {
    errMsg(err) << "Cannot link group lists.";
    return false;
  }

  return true;
}


bool
RT4DCodeplug::encodeElements(Context &ctx, const ErrorStack &err) {
  if (! FirstSettingsElement(data(Offset::firstSettings())).encode(ctx, err)) {
    errMsg(err) << "Cannot encode first settings element.";
    return false;
  }

  if (! ChannelBankElement(data(Offset::channels())).encode(ctx, err)) {
    errMsg(err) << "Cannot encode channels.";
    return false;
  }


  if (! SecondSettingsElement(data(Offset::secondSettings())).encode(ctx, err)) {
    errMsg(err) << "Cannot encode second settings element.";
    return false;
  }

  if (! ZoneBankElement(data(Offset::zones())).encode(ctx, err)) {
    errMsg(err) << "Cannot encode zones.";
    return false;
  }

  if (! ContactBankElement(data(Offset::contacts())).encode(ctx, err)) {
    errMsg(err) << "Cannot encode contacts.";
    return false;
  }

  if (! GroupListBankElement(data(Offset::groupLists())).encode(ctx, err)) {
    errMsg(err) << "Cannot encode group lists.";
    return false;
  }

  if (! EncryptionKeyBankElement(data(Offset::keys())).encode(ctx, err)) {
    errMsg(err) << "Cannot encode keys.";
    return false;
  }

  if (! MessageBankElement(data(Offset::messages())).encode(ctx, err)) {
    errMsg(err) << "Cannot encode messages.";
    return false;
  }

  return true;
}




