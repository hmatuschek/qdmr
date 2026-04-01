#include "radiosettings.hh"
#include "radioid.hh"
#include "gnsssettings.hh"


RadioSettings::RadioSettings(QObject *parent)
  : ConfigItem(parent), _introLine1(""), _introLine2(""), _micLevel(Level::fromValue(3)), _speech(false),
    _squelch(Level::fromValue(1)), _power(Channel::Power::High), _vox(Level::null()),
    _transmitTimeOut(Interval::infinity()), _defaultId(new DMRRadioIDReference(this)),
    _gnss(new GNSSSettings(this)), _dmr(new DMRSettings(this)),
    _tytExtension(nullptr), _radioddityExtension(nullptr), _anytoneExtension(nullptr),
    _openGD77(nullptr)
{
  connect(_gnss, &GNSSSettings::modified, this, &RadioSettings::modified);
  connect(_dmr, &DMRSettings::modified, this, &RadioSettings::modified);
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
  ConfigItem::clear();

  _introLine1.clear();
  _introLine2.clear();
  _micLevel = Level::fromValue(3);
  _speech = false;
  _squelch = Level::fromValue(1);
  _power = Channel::Power::High;
  disableVOX();
  disableTOT();
  defaultIdRef()->clear();

  // delete extensions
  setTyTExtension(nullptr);
  setRadioddityExtension(nullptr);
  setAnytoneExtension(nullptr);
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

Level
RadioSettings::micLevel() const {
  return _micLevel;
}
void
RadioSettings::setMicLevel(Level value) {
  if (value.isInvalid() || value.isNull())
    value = Level::fromValue(1);
  if (_micLevel == value)
    return;
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

Level
RadioSettings::squelch() const {
  return _squelch;
}
void
RadioSettings::setSquelch(Level squelch) {
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
  return _vox.isNull();
}
Level
RadioSettings::vox() const {
  return _vox;
}
void
RadioSettings::setVOX(Level level) {
  if (_vox == level)
    return;
  _vox = level;
  emit modified(this);
}
void
RadioSettings::disableVOX() {
  setVOX(Level::null());
}


bool
RadioSettings::totDisabled() const {
  return _transmitTimeOut.isInfinite() || _transmitTimeOut.isNull();
}

Interval
RadioSettings::tot() const {
  return _transmitTimeOut;
}

void
RadioSettings::setTOT(const Interval &sec) {
  if (_transmitTimeOut == sec)
    return;
  _transmitTimeOut = sec;
  emit modified(this);
}

void
RadioSettings::disableTOT() {
  setTOT(Interval::infinity());
}


DMRRadioIDReference *
RadioSettings::defaultIdRef() const {
  return _defaultId;
}
DMRRadioID *
RadioSettings::defaultId() const {
  if (_defaultId->isNull())
    return nullptr;
  return _defaultId->as<DMRRadioID>();
}
void
RadioSettings::setDefaultId(DMRRadioID *id) {
  _defaultId->set(id);
}


GNSSSettings *
RadioSettings::gnss() const {
  return _gnss;
}

DMRSettings *
RadioSettings::dmr() const {
  return _dmr;
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


AnytoneSettingsExtension *
RadioSettings::anytoneExtension() const {
  return _anytoneExtension;
}

void
RadioSettings::setAnytoneExtension(AnytoneSettingsExtension *ext) {
  if (_anytoneExtension) {
    disconnect(_anytoneExtension, SIGNAL(modified(ConfigItem*)), this, SLOT(onExtensionModified()));
    _anytoneExtension->deleteLater();
  }
  _anytoneExtension = ext;
  if (_anytoneExtension) {
    _anytoneExtension->setParent(this);
    connect(_anytoneExtension, SIGNAL(modified(ConfigItem*)), this, SLOT(onExtensionModified()));
  }
  emit modified(this);
}


OpenGD77SettingsExtension *
RadioSettings::openGD77Extension() const {
  return _openGD77;
}

void
RadioSettings::setOpenGD77Extension(OpenGD77SettingsExtension *ext) {
  if (_openGD77) {
    disconnect(_openGD77, SIGNAL(modified(ConfigItem*)), this, SLOT(onExtensionModified()));
    _openGD77->deleteLater();
  }
  _openGD77 = ext;
  if (_openGD77) {
    _openGD77->setParent(this);
    connect(_openGD77, SIGNAL(modified(ConfigItem*)), this, SLOT(onExtensionModified()));
  }
  emit modified(this);
}


void
RadioSettings::onExtensionModified() {
  emit modified(this);
}


bool
RadioSettings::parse(const YAML::Node &node, ConfigItem::Context &ctx, const ErrorStack &err) {
  if (! node)
    return false;

  if (! node.IsMap()) {
    errMsg(err) << node.Mark().line << ":" << node.Mark().column
                << ": Cannot parse radio settings: Expected object.";
    return false;
  }

  if (! node["tot"]) {
    disableTOT();
  } else if (node["tot"] && node["tot"].IsScalar()) {
    Interval to;
    if (! to.parse(QString::fromStdString(node["tot"].as<std::string>()), Interval::Format::Seconds))
      disableTOT();
    else
      setTOT(to);
  }

  return ConfigItem::parse(node, ctx, err);
}


bool
RadioSettings::populate(YAML::Node &node, const Context &context, const ErrorStack &err) {
  if (! ConfigItem::populate(node, context, err))
    return false;

  if (! totDisabled())
    node["tot"] = tot().format().toStdString();

  return true;
}
