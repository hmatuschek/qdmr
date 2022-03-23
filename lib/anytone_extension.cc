#include "anytone_extension.hh"

/* ********************************************************************************************* *
 * Implementation of AnytoneZoneExtension
 * ********************************************************************************************* */
AnytoneZoneExtension::AnytoneZoneExtension(QObject *parent)
  : ConfigExtension(parent), _hidden(false)
{
  // pass...
}

ConfigItem *
AnytoneZoneExtension::clone() const {
  AnytoneZoneExtension *obj = new AnytoneZoneExtension();
  if (! obj->copy(*this)) {
    obj->deleteLater();
    return nullptr;
  }
  return obj;
}

bool
AnytoneZoneExtension::hidden() const {
  return _hidden;
}
void
AnytoneZoneExtension::enableHidden(bool enable) {
  if (_hidden == enable)
    return;
  _hidden = enable;
  emit modified(this);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneContactExtension
 * ********************************************************************************************* */
AnytoneContactExtension::AnytoneContactExtension(QObject *parent)
  : ConfigExtension(parent), _alertType(AlertType::None)
{
  // pass...
}

ConfigItem *
AnytoneContactExtension::clone() const {
  AnytoneContactExtension *obj = new AnytoneContactExtension();
  if (! obj->copy(*this)) {
    obj->deleteLater();
    return nullptr;
  }
  return obj;
}

AnytoneContactExtension::AlertType
AnytoneContactExtension::alertType() const {
  return _alertType;
}
void
AnytoneContactExtension::setAlertType(AlertType type) {
  _alertType = type;
}
