#include "tonesettings.hh"

ToneSettings::ToneSettings(QObject *parent)
  : ConfigItem(parent), _silent(false), _keyTone(Level::null())
{
  // pass...
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

