#include "opengd77_extension.hh"

/* ******************************************************************************************** *
 * Implementation of OpenGD77ChannelExtension
 * ******************************************************************************************** */
OpenGD77ChannelExtension::OpenGD77ChannelExtension(QObject *parent)
  : ConfigExtension(parent), _power(Power::Global)
{
  // pass...
}

bool
OpenGD77ChannelExtension::copy(const ConfigItem &other) {
  const OpenGD77ChannelExtension *ex = other.as<OpenGD77ChannelExtension>();
  if ((nullptr==ex) || (! ConfigExtension::copy(other)))
    return false;
  _power = ex->_power;
  return true;
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

ConfigItem *
OpenGD77ChannelExtension::allocateChild(QMetaProperty &prop, const YAML::Node &node,
                                        const Context &ctx, const ErrorStack &err)
{
  Q_UNUSED(prop); Q_UNUSED(node); Q_UNUSED(ctx); Q_UNUSED(err)
  // No extensions yet for this extension
  return nullptr;
}


/* ******************************************************************************************** *
 * Implementation of OpenGD77ContactExtension
 * ******************************************************************************************** */
OpenGD77ContactExtension::OpenGD77ContactExtension(QObject *parent)
  : ConfigExtension(parent), _timeSlotOverride(TimeSlotOverride::None)
{
  // pass...
}

bool
OpenGD77ContactExtension::copy(const ConfigItem &other) {
  const OpenGD77ContactExtension *ex = other.as<OpenGD77ContactExtension>();
  if ((nullptr==ex) || (! ConfigExtension::copy(other)))
    return false;
  _timeSlotOverride = ex->_timeSlotOverride;
  return true;
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

ConfigItem *
OpenGD77ContactExtension::allocateChild(QMetaProperty &prop, const YAML::Node &node,
                                        const Context &ctx, const ErrorStack &err) {
  Q_UNUSED(prop); Q_UNUSED(node); Q_UNUSED(ctx); Q_UNUSED(err)
  // No extension yet of this extension
  return nullptr;
}

OpenGD77ContactExtension::TimeSlotOverride
OpenGD77ContactExtension::timeSlotOverride() const {
  return _timeSlotOverride;
}
void
OpenGD77ContactExtension::setTimeSlotOverride(TimeSlotOverride ts) {
  _timeSlotOverride = ts;
}
