#include "opengd77_extension.hh"

/* ******************************************************************************************** *
 * Implementation of OpenGD77ChannelExtension
 * ******************************************************************************************** */
OpenGD77ChannelExtension::OpenGD77ChannelExtension(QObject *parent)
  : ConfigExtension(parent), _power(Power::Global), _zoneSkip(false), _allSkip(false)
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

OpenGD77ChannelExtension::Power
OpenGD77ChannelExtension::power() const {
  return _power;
}
void
OpenGD77ChannelExtension::setPower(Power power) {
  _power = power;
  emit modified(this);
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
