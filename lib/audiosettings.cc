#include "audiosettings.hh"


AudioSettings::AudioSettings(QObject *parent)
  : ConfigItem(parent), _squelch(Level::fromValue(3)), _dmrSquelch(Level::invalid()),
    _micGain(Level::fromValue(5)), _fmMicGain(Level::invalid()), _m17MicGain(Level::invalid()),
    _maxSpeakerVolume(Level::fromValue(10)), _maxHeadphoneVolume(Level::fromValue(10)),
    _vox(Level::null()), _voxDelay(Interval::null()), _speech(false)
{
  // pass...
}

ConfigItem *
AudioSettings::clone() const {
  auto obj = new AudioSettings();
  if (! obj->copy(*this)) {
    delete obj;
    return nullptr;
  }
  return obj;
}

Level
AudioSettings::squelch() const {
  return _squelch;
}

void
AudioSettings::setSquelch(Level squelch) {
  if (squelch.isInvalid())
    squelch = Level::fromValue(3);
  // squelch = 0 -> open squelch
  if (_squelch == squelch)
    return;
  _squelch = squelch;
  emit modified(this);
}


bool
AudioSettings::dmrSquelchEnabled() const {
  return ! _dmrSquelch.isInvalid();
}

Level
AudioSettings::dmrSquelch() const {
  return _dmrSquelch;
}

void
AudioSettings::setDMRSquelch(Level dmrSquelch) {
  if (_dmrSquelch == dmrSquelch)
    return;
  _dmrSquelch = dmrSquelch;
  emit modified(this);
}

void
AudioSettings::disableDMRSquelch() {
  setDMRSquelch(Level::invalid());
}


Level
AudioSettings::micGain() const {
  return _micGain;
}

void
AudioSettings::setMicGain(Level micGain) {
  if (! micGain.isFinite())
    micGain = Level::fromValue(1);
  if (_micGain == micGain)
    return;
  _micGain = micGain;
  emit modified(this);
}


bool
AudioSettings::fmMicGainEnabled() const {
  return _fmMicGain.isFinite();
}

Level
AudioSettings::fmMicGain() const {
  return _fmMicGain;
}

void
AudioSettings::setFMMicGain(Level fmMicGain) {
  if (_fmMicGain == fmMicGain)
    return;
  _fmMicGain = fmMicGain;
  emit modified(this);
}

void
AudioSettings::disableFMMicGain() {
  setFMMicGain(Level::invalid());
}

bool
AudioSettings::m17MicGainEnabled() const {
  return _m17MicGain.isFinite();
}

Level
AudioSettings::m17MicGain() const {
  return _m17MicGain;
}

void
AudioSettings::setM17MicGain(Level m17MicGain) {
  if (_m17MicGain == m17MicGain)
    return;
  _m17MicGain = m17MicGain;
  emit modified(this);
}

void
AudioSettings::disableM17MicGain() {
  setM17MicGain(Level::invalid());
}


bool
AudioSettings::voxEnabled() const {
  return _vox.isFinite();
}

Level
AudioSettings::vox() const {
  return _vox;
}

void
AudioSettings::setVox(Level vox) {
  if (vox.isInvalid())
    vox = Level::null();
  if (_vox == vox)
    return;
  _vox = vox;
  emit modified(this);
}

void
AudioSettings::disableVox() {
  setVox(Level::null());
}


Level
AudioSettings::maxSpeakerVolume() const {
  return _maxSpeakerVolume;
}

void
AudioSettings::setMaxSpeakerVolume(Level maxSpeakerVolume) {
  if (_maxSpeakerVolume == maxSpeakerVolume)
    return;
  _maxSpeakerVolume = maxSpeakerVolume;
  emit modified(this);
}

Level
AudioSettings::maxHeadphoneVolume() const {
  return _maxHeadphoneVolume;
}

void
AudioSettings::setMaxHeadphoneVolume(Level maxHeadphoneVolume) {
  if (_maxHeadphoneVolume == maxHeadphoneVolume)
    return;
  _maxHeadphoneVolume = maxHeadphoneVolume;
  emit modified(this);
}


Interval
AudioSettings::voxDelay() const {
  return _voxDelay;
}

void
AudioSettings::setVOXDelay(Interval voxDelay) {
  if (_voxDelay == voxDelay)
    return;
  _voxDelay = voxDelay;
  emit modified(this);
}

bool
AudioSettings::speechSynthesisEnabled() const {
  return _speech;
}

void
AudioSettings::enableSpeechSynthesis(bool speechSynthesis) {
  if (_speech == speechSynthesis)
    return;
  _speech = speechSynthesis;
  emit modified(this);
}