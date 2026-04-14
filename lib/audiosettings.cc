#include "audiosettings.hh"


AudioSettings::AudioSettings(QObject *parent)
  : ConfigExtension(parent), _fmMicGain(Level::null()), _m17MicGain(Level::null()),
    _satMicGain(Level::null()), _voxDelay(Interval::null())
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
  setFMMicGain(Level::null());
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
  setM17MicGain(Level::null());
}

bool
AudioSettings::satMicGainEnabled() const {
  return _satMicGain.isFinite();
}

Level
AudioSettings::satMicGain() const {
  return _satMicGain;
}

void
AudioSettings::setSatMicGain(Level satMicGain) {
  if (_satMicGain == satMicGain)
    return;
  _satMicGain = satMicGain;
  emit modified(this);
}

void
AudioSettings::disableSatMicGain() {
  setSatMicGain(Level::null());
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