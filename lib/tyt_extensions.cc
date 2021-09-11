#include "tyt_extensions.hh"
#include "logger.hh"

/* ******************************************************************************************** *
 * Implementation of TyTButtonSettings
 * ******************************************************************************************** */
TyTButtonSettings::TyTButtonSettings(QObject *parent)
  : ConfigExtension("btn", parent)
{
  _sideButton1Short = Disabled;
  _sideButton1Long  = Tone1750Hz;
  _sideButton2Short = MonitorToggle;
  _sideButton2Long  = Disabled;
  _longPressDuration = 1000;
}

bool
TyTButtonSettings::label(Context &context) {
  return true;
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

uint
TyTButtonSettings::longPressDuration() const {
  return _longPressDuration;
}
void
TyTButtonSettings::setLongPressDuration(uint dur) {
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
  return new TyTButtonSettings();
}
