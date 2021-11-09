#include "tyt_extensions.hh"
#include "logger.hh"

/* ******************************************************************************************** *
 * Implementation of TyTButtonSettings
 * ******************************************************************************************** */
TyTButtonSettings::TyTButtonSettings(QObject *parent)
  : ConfigExtension("", parent)
{
  _sideButton1Short = Disabled;
  _sideButton1Long  = Tone1750Hz;
  _sideButton2Short = MonitorToggle;
  _sideButton2Long  = Disabled;
  _longPressDuration = 1000;
}

ConfigObject *
TyTButtonSettings::allocateChild(QMetaProperty &prop, const YAML::Node &node, const Context &ctx) {
  return nullptr;
}

TyTButtonSettings::ButtonAction
TyTButtonSettings::sideButton1Short() const {
  return _sideButton1Short;
}
void
TyTButtonSettings::setSideButton1Short(ButtonAction action) {
  _sideButton1Short = action;
}

TyTButtonSettings::ButtonAction
TyTButtonSettings::sideButton1Long() const {
  return _sideButton1Long;
}
void
TyTButtonSettings::setSideButton1Long(ButtonAction action) {
  _sideButton1Long = action;
}

TyTButtonSettings::ButtonAction
TyTButtonSettings::sideButton2Short() const {
  return _sideButton2Short;
}
void
TyTButtonSettings::setSideButton2Short(ButtonAction action) {
  _sideButton2Short = action;
}

TyTButtonSettings::ButtonAction
TyTButtonSettings::sideButton2Long() const {
  return _sideButton2Long;
}
void
TyTButtonSettings::setSideButton2Long(ButtonAction action) {
  _sideButton2Long = action;
}

unsigned
TyTButtonSettings::longPressDuration() const {
  return _longPressDuration;
}
void
TyTButtonSettings::setLongPressDuration(unsigned dur) {
  _longPressDuration = dur;
}


/* ******************************************************************************************** *
 * Implementation of TyTButtonSettingsReader
 * ******************************************************************************************** */
// Register extension to config reader
AbstractConfigReader *
TyTButtonSettingsReader::instance = ConfigReader::addExtension(new TyTButtonSettingsReader());

TyTButtonSettingsReader::TyTButtonSettingsReader(QObject *parent)
  : ExtensionReader(parent)
{
  // pass...
}

ConfigObject *
TyTButtonSettingsReader::allocate(const YAML::Node &node, const ConfigObject::Context &ctx) {
  Q_UNUSED(node)
  Q_UNUSED(ctx)
  return new TyTButtonSettings();
}
