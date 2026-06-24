//
// Created by hannes on 23.06.26.
//

#include "rt4d_codeplug.hh"

#include "ranges.hh"

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
  return Level::fromValue(getUInt8(Offset::dmrMicGain()), {0, 24});
}

void
RT4DCodeplug::FirstSettingsElement::setDmrMicGain(const Level &level) {
  setUInt8(Offset::dmrMicGain(), level.mapTo({0,24}));
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


RT4DCodeplug::FirstSettingsElement::SMSFormat
RT4DCodeplug::FirstSettingsElement::smsFormat() const {
  return static_cast<RT4DCodeplug::FirstSettingsElement::SMSFormat>(
    getUInt8(Offset::smsFormat()));
}

void
RT4DCodeplug::FirstSettingsElement::setSmsFormat(SMSFormat smsFormat) {
  setUInt8(Offset::smsFormat(), static_cast<unsigned int>(smsFormat));
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



/* ********************************************************************************************* *
 * Implementation of RT4DCodeplug
 * ********************************************************************************************* */
RT4DCodeplug::RT4DCodeplug(QObject *parent)
  : Codeplug(parent)
{
  // Preallocate entrie codeplug
  addImage("Radtel RT4D Codeplug, FW 3.25");
  image(0).addElement(Offset::firstSettings(), FirstSettingsElement::size());
  image(0).element(0).data().fill(0xff);
  image(0).addElement(Offset::channels(), ChannelBankElement::size());
  image(0).addElement(Offset::secondSettings(), SecondSettingsElement::size());
  image(0).addElement(Offset::zones(), ZoneBankElement::size());
  image(0).addElement(Offset::contacts(), ContactBankElement::size());
  image(0).addElement(Offset::groupLists(), GroupListBankElement::size());
  image(0).addElement(Offset::keys(), EncryptionKeyBankElement::size());
  image(0).addElement(Offset::messages(), MessageBankElement::size());
  image(0).addElement(Offset::fmBroadcast(), FMBroadcastChannelBankElement::size());
}


bool
RT4DCodeplug::index(Config *config, Context &ctx, const ErrorStack &err) const {
  return false;
}


bool
RT4DCodeplug::encode(Config *config, const Flags &flags, const ErrorStack &err) {
  return false;
}


bool
RT4DCodeplug::decode(Config *config, const ErrorStack &err) {
  return false;
}