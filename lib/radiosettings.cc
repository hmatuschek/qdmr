#include "radiosettings.hh"
#include "radioid.hh"
#include "gnsssettings.hh"


RadioSettings::RadioSettings(QObject *parent)
  : ConfigItem(parent),
    _power(Channel::Power::High),
    _transmitTimeOut(Interval::infinity()), _defaultId(new DMRRadioIDReference(this)),
    _boot(new BootSettings(this)), _audio(new AudioSettings(this)),
    _tone(new ToneSettings(this)),
    _gnss(new GNSSSettings(this)), _dmr(new DMRSettings(this)),
    _tytExtension(nullptr), _radioddityExtension(nullptr),
    _anytoneExtension(nullptr), _openGD77(nullptr)
{
  connect(_boot, &BootSettings::modified, this, &RadioSettings::modified);
  connect(_audio, &AudioSettings::modified, this, &RadioSettings::modified);
  connect(_tone, &ToneSettings::modified, this, &RadioSettings::modified);
  connect(_gnss, &GNSSSettings::modified, this, &RadioSettings::modified);
  connect(_dmr, &DMRSettings::modified, this, &RadioSettings::modified);
}

bool
RadioSettings::copy(const ConfigItem &other) {
  const RadioSettings *set = other.as<RadioSettings>();
  if ((nullptr==set) || (!ConfigItem::copy(other)))
    return false;
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

  _power = Channel::Power::High;
  disableTOT();
  defaultIdRef()->clear();

  // delete extensions
  setTyTExtension(nullptr);
  setRadioddityExtension(nullptr);
  setAnytoneExtension(nullptr);
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

BootSettings *
RadioSettings::boot() const {
  return _boot;
}

AudioSettings *
RadioSettings::audio() const {
  return _audio;
}

ToneSettings *
RadioSettings::tone() const {
  return _tone;
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

  /// @todo Remove with 0.17.0. Only parse "old" global settings, will be serialized in boot and audio sections.
  if (Level micLevel; node["micLevel"] && node["micLevel"].IsScalar()
    && micLevel.parse(QString::fromStdString(node["micLevel"].as<std::string>())))
    audio()->setMicGain(micLevel);
  if (node["speech"] && node["speech"].IsScalar())
    audio()->enableSpeechSynthesis("true" == node["speech"].as<std::string>());
  if (Level squelch; node["squelch"] && node["squelch"].IsScalar()
    && squelch.parse(QString::fromStdString(node["squelch"].as<std::string>())))
    audio()->setSquelch(squelch);
  if (Level vox; node["vox"] && node["vox"].IsScalar()
    && vox.parse(QString::fromStdString(node["vox"].as<std::string>())))
    audio()->setVox(vox);
  if (node["introLine1"] && node["introLine1"].IsScalar())
    boot()->setMessage1(QString::fromStdString(node["introLine1"].as<std::string>()));
  if (node["introLine2"] && node["introLine2"].IsScalar())
    boot()->setMessage2(QString::fromStdString(node["introLine2"].as<std::string>()));
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
