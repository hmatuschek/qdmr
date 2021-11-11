#include "tyt_extensions.hh"
#include "logger.hh"

/* ******************************************************************************************** *
 * Implementation of TyTButtonSettings
 * ******************************************************************************************** */
TyTButtonSettings::TyTButtonSettings(QObject *parent)
  : ConfigExtension(parent)
{
  _sideButton1Short = Disabled;
  _sideButton1Long  = Tone1750Hz;
  _sideButton2Short = MonitorToggle;
  _sideButton2Long  = Disabled;
  _longPressDuration = 1000;
}

bool
TyTButtonSettings::copy(const ConfigItem &other) {
  const TyTButtonSettings *ex = other.as<TyTButtonSettings>();
  if ((nullptr==ex) || (!ConfigExtension::copy(other)))
    return false;
  _sideButton1Short = ex->_sideButton1Short;
  _sideButton1Long = ex->_sideButton1Long;
  _sideButton2Short = ex->_sideButton2Short;
  _sideButton2Long = ex->_sideButton2Long;
  _longPressDuration = ex->_longPressDuration;
  return true;
}

ConfigItem *
TyTButtonSettings::clone() const {
  TyTButtonSettings *ex = new TyTButtonSettings();
  if (! ex->copy(*this)) {
    ex->deleteLater();
    return nullptr;
  }
  return ex;
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

ConfigItem *
TyTButtonSettings::allocateChild(QMetaProperty &prop, const YAML::Node &node, const Context &ctx) {
  Q_UNUSED(prop); Q_UNUSED(node); Q_UNUSED(ctx);
  // There are no further extension/children to TyTButtonSettings.
  return nullptr;
}


/* ******************************************************************************************** *
 * Implementation of TyTButtonSettings
 * ******************************************************************************************** */
TyTConfigExtension::TyTConfigExtension(QObject *parent)
  : ConfigExtension(parent), _buttonSettings(new TyTButtonSettings(this))
{
  // Pass...
}

bool
TyTConfigExtension::copy(const ConfigItem &other) {
  const TyTConfigExtension *ex = other.as<TyTConfigExtension>();
  if ((nullptr==ex) || (!ConfigExtension::copy(other)))
    return false;
  return _buttonSettings->copy(*_buttonSettings);
}

ConfigItem *
TyTConfigExtension::clone() const {
  TyTConfigExtension *ex = new TyTConfigExtension();
  if (! ex->copy(*this)) {
    ex->deleteLater();
    return nullptr;
  }
  return ex;
}

TyTButtonSettings *
TyTConfigExtension::buttonSettings() const {
  return _buttonSettings;
}

ConfigItem *
TyTConfigExtension::allocateChild(QMetaProperty &prop, const YAML::Node &node, const Context &ctx) {
  Q_UNUSED(prop); Q_UNUSED(node); Q_UNUSED(ctx)
  // All extensions are pre-allocated. So nothing to do here.
  return nullptr;
}
