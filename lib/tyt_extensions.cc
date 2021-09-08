#include "tyt_extensions.hh"
#include "logger.hh"

/* ******************************************************************************************** *
 * Implementation of TyTButtonSettingsExtension
 * ******************************************************************************************** */
TyTButtonSettingsExtension::TyTButtonSettingsExtension(QObject *parent)
  : ConfigExtension("btn", parent)
{
  _sideButton1Short = Disabled;
  _sideButton1Long  = Tone1750Hz;
  _sideButton2Short = MonitorToggle;
  _sideButton2Long  = Disabled;
  _longPressDuration = 1000;
}

bool
TyTButtonSettingsExtension::label(Context &context) {
  return true;
}

TyTButtonSettingsExtension::ButtonAction
TyTButtonSettingsExtension::sideButton1Short() const {
  return _sideButton1Short;
}
void
TyTButtonSettingsExtension::setSideButton1Short(ButtonAction action) {
  _sideButton1Short = action;
}

TyTButtonSettingsExtension::ButtonAction
TyTButtonSettingsExtension::sideButton1Long() const {
  return _sideButton1Long;
}
void
TyTButtonSettingsExtension::setSideButton1Long(ButtonAction action) {
  _sideButton1Long = action;
}

TyTButtonSettingsExtension::ButtonAction
TyTButtonSettingsExtension::sideButton2Short() const {
  return _sideButton2Short;
}
void
TyTButtonSettingsExtension::setSideButton2Short(ButtonAction action) {
  _sideButton2Short = action;
}

TyTButtonSettingsExtension::ButtonAction
TyTButtonSettingsExtension::sideButton2Long() const {
  return _sideButton2Long;
}
void
TyTButtonSettingsExtension::setSideButton2Long(ButtonAction action) {
  _sideButton2Long = action;
}

uint
TyTButtonSettingsExtension::longPressDuration() const {
  return _longPressDuration;
}
void
TyTButtonSettingsExtension::setLongPressDuration(uint dur) {
  _longPressDuration = dur;
}


/* ******************************************************************************************** *
 * Implementation of TyTButtonSettingsReader
 * ******************************************************************************************** */
// Register extension to config reader
AbstractConfigReader *
TyTButtonSettingsReader::instance = ConfigReader::addExtension(
      TyTButtonSettingsExtension::staticMetaObject.className(), new TyTButtonSettingsReader());

TyTButtonSettingsReader::TyTButtonSettingsReader(QObject *parent)
  : ExtensionReader(parent)
{
  // pass...
}

ConfigObject *
TyTButtonSettingsReader::allocate(const YAML::Node &node, const ConfigObject::Context &ctx) {
  return new TyTButtonSettingsExtension();
}
