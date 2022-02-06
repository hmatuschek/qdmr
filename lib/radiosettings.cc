#include "radiosettings.hh"

RadioSettings::RadioSettings(QObject *parent)
  : ConfigItem(parent), _introLine1(""), _introLine2(""), _micLevel(3), _speech(false),
    _squelch(1), _power(Channel::Power::High), _vox(0), _transmitTimeOut(0), _tytExtension(nullptr),
    _radioddityExtension(nullptr)
{
  // pass
}

bool
RadioSettings::copy(const ConfigItem &other) {
  const RadioSettings *set = other.as<RadioSettings>();
  if ((nullptr==set) || (!ConfigItem::copy(other)))
    return false;
  if (set->voxDisabled())
    disableVOX();
  if (set->totDisabled())
    disableTOT();
  return true;
}

ConfigItem *
RadioSettings::clone() const {
  RadioSettings *set = new RadioSettings();
  if (! set->copy(*this)) {
    set->deleteLater();
    return nullptr;
  }
  return set;
}

void
RadioSettings::clear() {
  _introLine1.clear();
  _introLine2.clear();
  _micLevel = 3;
  _speech = false;
  _squelch = 1;
  _power = Channel::Power::High;
  disableVOX();
  disableTOT();

  setTyTExtension(nullptr);
  setRadioddityExtension(nullptr);
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

unsigned
RadioSettings::micLevel() const {
  return _micLevel;
}
void
RadioSettings::setMicLevel(unsigned value) {
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

unsigned
RadioSettings::squelch() const {
  return _squelch;
}
void
RadioSettings::setSquelch(unsigned squelch) {
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
unsigned
RadioSettings::vox() const {
  return _vox;
}
void
RadioSettings::setVOX(unsigned level) {
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
unsigned
RadioSettings::tot() const {
  return _transmitTimeOut;
}
void
RadioSettings::setTOT(unsigned sec) {
  _transmitTimeOut = sec;
  emit modified(this);
}
void
RadioSettings::disableTOT() {
  setTOT(0);
}

TyTSettingsExtension *
RadioSettings::tytExtension() const {
  return _tytExtension;
}
void
RadioSettings::setTyTExtension(TyTSettingsExtension *ext) {
  if (_tytExtension) {
    disconnect(_tytExtension, SIGNAL(modified(ConfigItem*)), this, SLOT(onExtensionModified()));
    _tytExtension->deleteLater();
  }
  _tytExtension = ext;
  if (_tytExtension) {
    _tytExtension->setParent(this);
    connect(_tytExtension, SIGNAL(modified(ConfigItem*)), this, SLOT(onExtensionModified()));
  }
  emit modified(this);
}

RadiodditySettingsExtension *
RadioSettings::radioddityExtension() const {
  return _radioddityExtension;
}
void
RadioSettings::setRadioddityExtension(RadiodditySettingsExtension *ext) {
  if (_radioddityExtension) {
    disconnect(_radioddityExtension, SIGNAL(modified(ConfigItem*)), this, SLOT(onExtensionModified()));
    _radioddityExtension->deleteLater();
  }
  _radioddityExtension = ext;
  if (_radioddityExtension) {
    _radioddityExtension->setParent(this);
    connect(_radioddityExtension, SIGNAL(modified(ConfigItem*)), this, SLOT(onExtensionModified()));
  }
  emit modified(this);
}

void
RadioSettings::onExtensionModified() {
  emit modified(this);
}
