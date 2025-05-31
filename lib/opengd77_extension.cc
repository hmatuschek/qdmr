#include "opengd77_extension.hh"
#include "utils.hh"

/* ******************************************************************************************** *
 * Implementation of OpenGD77ChannelExtension
 * ******************************************************************************************** */
OpenGD77ChannelExtension::OpenGD77ChannelExtension(QObject *parent)
  : ConfigExtension(parent), _zoneSkip(false), _allSkip(false), _beep(true), _powerSave(true),
    _location(), _txTalkerAliasTS1(TalkerAlias::None), _txTalkerAliasTS2(TalkerAlias::None)
{
  // pass...
}

ConfigItem *
OpenGD77ChannelExtension::clone() const {
  OpenGD77ChannelExtension *ex = new OpenGD77ChannelExtension();
  if (! ex->copy(*this)) {
    ex->deleteLater();
    return nullptr;
  }
  return ex;
}


bool
OpenGD77ChannelExtension::scanZoneSkip() const {
  return _zoneSkip;
}

void
OpenGD77ChannelExtension::enableScanZoneSkip(bool enable) {
  _zoneSkip = enable;
}


bool
OpenGD77ChannelExtension::scanAllSkip() const {
  return _allSkip;
}

void
OpenGD77ChannelExtension::enableScanAllSkip(bool enable) {
  _allSkip = enable;
}


bool
OpenGD77ChannelExtension::beep() const {
  return _beep;
}

void
OpenGD77ChannelExtension::enableBeep(bool enable) {
  _beep = enable;
}


bool
OpenGD77ChannelExtension::powerSave() const {
  return _powerSave;
}

void
OpenGD77ChannelExtension::enablePowerSave(bool enable) {
  _powerSave = enable;
}

const QGeoCoordinate &
OpenGD77ChannelExtension::location() const {
  return _location;
}

void
OpenGD77ChannelExtension::setLocation(const QGeoCoordinate &loc) {
  _location = loc;
}

QString
OpenGD77ChannelExtension::locator() const {
  if (_location.isValid())
    return deg2loc(location(), 8);
  return "";
}

void
OpenGD77ChannelExtension::setLocator(const QString &loc) {
  _location = loc2deg(loc);
}


OpenGD77ChannelExtension::TalkerAlias
OpenGD77ChannelExtension::talkerAliasTS1() const {
  return _txTalkerAliasTS1;
}

void
OpenGD77ChannelExtension::setTalkerAliasTS1(TalkerAlias ta) {
  _txTalkerAliasTS1 = ta;
}


OpenGD77ChannelExtension::TalkerAlias
OpenGD77ChannelExtension::talkerAliasTS2() const {
  return _txTalkerAliasTS2;
}

void
OpenGD77ChannelExtension::setTalkerAliasTS2(TalkerAlias ta) {
  _txTalkerAliasTS2 = ta;
}



/* ******************************************************************************************** *
 * Implementation of OpenGD77ContactExtension
 * ******************************************************************************************** */
OpenGD77ContactExtension::OpenGD77ContactExtension(QObject *parent)
  : ConfigExtension(parent), _timeSlotOverride(TimeSlotOverride::None)
{
  // pass...
}

ConfigItem *
OpenGD77ContactExtension::clone() const {
  OpenGD77ContactExtension *ex = new OpenGD77ContactExtension();
  if (! ex->copy(*this)) {
    ex->deleteLater();
    return nullptr;
  }
  return ex;
}

OpenGD77ContactExtension::TimeSlotOverride
OpenGD77ContactExtension::timeSlotOverride() const {
  return _timeSlotOverride;
}
void
OpenGD77ContactExtension::setTimeSlotOverride(TimeSlotOverride ts) {
  _timeSlotOverride = ts;
}


/* ******************************************************************************************** *
 * Implementation of OpenGD77APRSSystemExtension
 * ******************************************************************************************** */
OpenGD77APRSSystemExtension::OpenGD77APRSSystemExtension(QObject *parent)
  : ConfigExtension(parent), _location()
{
  // pass...
}


ConfigItem *
OpenGD77APRSSystemExtension::clone() const {
  auto ex = new OpenGD77APRSSystemExtension();
  if (! ex->copy(*this)) {
    ex->deleteLater();
    return nullptr;
  }
  return ex;
}


const QGeoCoordinate &
OpenGD77APRSSystemExtension::location() const {
  return _location;
}

void
OpenGD77APRSSystemExtension::setLocation(const QGeoCoordinate &loc) {
  _location = loc;
}

QString
OpenGD77APRSSystemExtension::locator() const {
  if (_location.isValid())
    return deg2loc(location(), 8);
  return "";
}

void
OpenGD77APRSSystemExtension::setLocator(const QString &loc) {
  _location = loc2deg(loc);
}

