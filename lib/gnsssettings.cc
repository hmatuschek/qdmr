#include "gnsssettings.hh"

GNSSSettings::GNSSSettings(QObject *parent)
  : ConfigItem{parent}, _fixedPosition(), _systems(System::GPS), _units(Units::Metric)
{
  // pass...
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
