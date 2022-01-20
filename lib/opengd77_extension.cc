#include "opengd77_extension.hh"

/* ******************************************************************************************** *
 * Implementation of OpenGD77ChannelExtension
 * ******************************************************************************************** */
OpenGD77ChannelExtension::OpenGD77ChannelExtension(QObject *parent)
  : ConfigExtension(parent), _power(Power::Global)
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
