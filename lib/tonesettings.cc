#include "tonesettings.hh"

ToneSettings::ToneSettings(QObject *parent)
  : ConfigItem(parent), _silent(false), _keyTone(Level::null()), _smsTone(true), _ringtone(true),
    _bootTone(false), _bootMelody(new Melody(100, this)),
    _talkPermit(Channel::Type::None),
    _callStart(Channel::Type::Digital), _callStartMelody(new Melody(100, this)),
    _callEnd(Channel::Type::None), _callEndMelody(new Melody(100, this)),
    _channelIdle(Channel::Type::All), _channelIdleMelody(new Melody(100, this)),
    _callReset(true), _callResetMelody(new Melody(100, this))
{
  connect(_bootMelody, &Melody::modified, this, &ToneSettings::modified);
  connect(_callStartMelody, &Melody::modified, this, &ToneSettings::modified);
  connect(_callEndMelody, &Melody::modified, this, &ToneSettings::modified);
  connect(_channelIdleMelody, &Melody::modified, this, &ToneSettings::modified);
  connect(_callResetMelody, &Melody::modified, this, &ToneSettings::modified);
}

ConfigItem *
ToneSettings::clone() const {
  auto obj = new ToneSettings();
  if (! obj->copy(*this)) {
    delete obj;
    return nullptr;
  }
  return obj;
}


bool
ToneSettings::silent() const {
  return _silent;
}

void
ToneSettings::enableSilent(bool enable) {
  if (_silent == enable)
    return;
  _silent = enable;
  emit modified(this);
}


bool
ToneSettings::keyToneEnabled() const {
  return _keyTone.isFinite();
}

Level
ToneSettings::keyToneVolume() const {
  return _keyTone;
}

void
ToneSettings::setKeyToneVolume(Level volume) {
  if (_keyTone == volume)
    return;
  _keyTone = volume;
  emit modified(this);
}

void
ToneSettings::disableKeyTone() {
  setKeyToneVolume(Level::null());
}


bool
ToneSettings::smsToneEnabled() const {
  return _smsTone;
}

void
ToneSettings::enableSMSTone(bool enabled) {
  if (_smsTone == enabled)
    return;
  _smsTone = enabled;
  emit modified(this);
}


bool
ToneSettings::ringtoneEnabled() const {
  return _ringtone;
}

void
ToneSettings::enableRingtone(bool enabled) {
  if (_ringtone == enabled)
    return;
  _ringtone = enabled;
  emit modified(this);
}


bool
ToneSettings::bootToneEnabled() const {
  return _bootTone;
}

void
ToneSettings::enableBootTone(bool enabled) {
  if (_bootTone == enabled)
    return;
  _bootTone = enabled;
  emit modified(this);
}

Melody *
ToneSettings::bootMelody() const {
  return _bootMelody;
}


Channel::Types
ToneSettings::talkPermit() const {
  return _talkPermit;
}

void
ToneSettings::setTalkPermit(Channel::Types type) {
  if (_talkPermit == type)
    return;
  _talkPermit = type;
  emit modified(this);
}


Channel::Types
ToneSettings::callStart() const {
  return _callStart;
}

void
ToneSettings::setCallStart(Channel::Types type) {
  if (_callStart == type)
    return;
  _callStart = type;
  emit modified(this);
}

Melody *
ToneSettings::callStartMelody() const {
  return _callStartMelody;
}


Channel::Types
ToneSettings::callEnd() const {
  return _callEnd;
}

void
ToneSettings::setCallEnd(Channel::Types type) {
  if (_callEnd == type)
    return;
  _callEnd = type;
  emit modified(this);
}

Melody *
ToneSettings::callEndMelody() const {
  return _callEndMelody;
}


Channel::Types
ToneSettings::channelIdle() const {
  return _channelIdle;
}

void
ToneSettings::setChannelIdle(Channel::Types type) {
  if (_channelIdle == type)
    return;
  _channelIdle = type;
  emit modified(this);
}

Melody *
ToneSettings::channelIdleMelody() const {
  return _channelIdleMelody;
}


bool
ToneSettings::callResetEnabled() const {
  return _callReset;
}

void
ToneSettings::enableCallReset(bool enabled) {
  if (_callReset == enabled)
    return;
  _callReset = enabled;
  emit modified(this);
}

Melody *
ToneSettings::callResetMelody() const {
  return _callResetMelody;
}