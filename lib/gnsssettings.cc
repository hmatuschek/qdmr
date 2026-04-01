#include "gnsssettings.hh"
#include "utils.hh"


GNSSSettings::GNSSSettings(QObject *parent)
  : ConfigExtension{parent}, _fixedPositionEnabled(false), _fixedPosition(),
    _systems(System::GPS), _units(Units::Metric)
{
  // pass...
}


ConfigItem *
GNSSSettings::clone() const {
  auto obj = new GNSSSettings();
  if (! obj->copy(*this)) {
    delete obj;
    return nullptr;
  }
  return obj;
}


bool
GNSSSettings::fixedPositionEnabled() const {
  return _fixedPosition.isValid() && (_fixedPositionEnabled || _systems.testFlag(System::Fixed));
}

void
GNSSSettings::enableFixedPosition(bool use) {
  if (_fixedPositionEnabled == use)
    return;
  _fixedPositionEnabled = use;
  emit modified(this);
}


const QGeoCoordinate &
GNSSSettings::fixedPosition() const {
  return _fixedPosition;
}

void
GNSSSettings::setFixedPosition(const QGeoCoordinate &pos) {
  if (_fixedPosition == pos)
    return;
  _fixedPosition = pos;
  emit modified(this);
}


QString
GNSSSettings::fixedPositionLocator() const {
  return deg2loc(fixedPosition(), 8);
}

void
GNSSSettings::setFixedPositionLocator(const QString &locator) {
  setFixedPosition(loc2deg(locator));
}


GNSSSettings::Systems
GNSSSettings::systems() const {
  return _systems;
}

void
GNSSSettings::setSystems(Systems systems) {
  if (_systems == systems)
    return;
  _systems = systems;
  emit modified(this);
}


GNSSSettings::Units
GNSSSettings::units() const {
  return _units;
}

void
GNSSSettings::setUnits(Units units) {
  if (_units == units)
    return;
  _units = units;
  emit modified(this);
}
