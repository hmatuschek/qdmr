#include "radiosettings.hh"

RadioSettings::RadioSettings(QObject *parent)
  : ConfigObject("", parent), _introLine1(""), _introLine2(""), _micLevel(3), _speech(false),
    _squelch(1), _power(Channel::Power::High), _vox(0), _transmitTimeOut(0)
{
  // pass
}

const QString &
RadioSettings::introLine1() const {
  return _introLine1;
}
void
RadioSettings::setIntroLine1(const QString &line) {
  _introLine1 = line;
  emit modified(this);
}

const QString &
RadioSettings::introLine2() const {
  return _introLine2;
}
void
RadioSettings::setIntroLine2(const QString &line) {
  _introLine2 = line;
  emit modified(this);
}

uint
RadioSettings::micLevel() const {
  return _micLevel;
}
void
RadioSettings::setMicLevel(uint value) {
  _micLevel = value;
  emit modified(this);
}

bool
RadioSettings::speech() const {
  return _speech;
}
void
RadioSettings::enableSpeech(bool enabled) {
  _speech = enabled;
  emit modified(this);
}

uint
RadioSettings::squelch() const {
  return _squelch;
}
void
RadioSettings::setSquelch(uint squelch) {
  squelch = std::min(10u, squelch);
  _squelch = squelch;
  emit modified(this);
}

Channel::Power
RadioSettings::power() const {
  return _power;
}
void
RadioSettings::setPower(Channel::Power power) {
  _power = power;
  emit modified(this);
}

bool
RadioSettings::voxDisabled() const {
  return 0 == vox();
}
uint
RadioSettings::vox() const {
  return _vox;
}
void
RadioSettings::setVOX(uint level) {
  _vox = level;
  emit modified(this);
}
void
RadioSettings::disableVOX() {
  setVOX(0);
}

bool
RadioSettings::totDisabled() const {
  return 0==tot();
}
uint
RadioSettings::tot() const {
  return _transmitTimeOut;
}
void
RadioSettings::setTOT(uint sec) {
  _transmitTimeOut = sec;
  emit modified(this);
}
void
RadioSettings::disableTOT() {
  setTOT(0);
}
