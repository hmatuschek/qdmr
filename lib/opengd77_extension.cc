#include "opengd77_extension.hh"

/* ******************************************************************************************** *
 * Implementation of OpenGD77ChannelExtension
 * ******************************************************************************************** */
OpenGD77ChannelExtension::OpenGD77ChannelExtension(QObject *parent)
  : ConfigExtension("", parent), _power(Power::Global)
{
  // pass...
}

ConfigObject *
OpenGD77ChannelExtension::allocateChild(QMetaProperty &prop, const YAML::Node &node, const Context &ctx) {
  return nullptr;
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
 * Implementation of OpenGD77ChannelExtensionReader
 * ******************************************************************************************** */
// Register extension to config reader
AbstractConfigReader *
OpenGD77ChannelExtensionReader::instance = ChannelReader::addExtension(new OpenGD77ChannelExtensionReader());

OpenGD77ChannelExtensionReader::OpenGD77ChannelExtensionReader(QObject *parent)
  : ExtensionReader(parent)
{
  // pass...
}

ConfigObject *
OpenGD77ChannelExtensionReader::allocate(const YAML::Node &node, const ConfigObject::Context &ctx) {
  Q_UNUSED(node)
  Q_UNUSED(ctx)
  return new OpenGD77ChannelExtension();
}


/* ******************************************************************************************** *
 * Implementation of OpenGD77ContactExtension
 * ******************************************************************************************** */
OpenGD77ContactExtension::OpenGD77ContactExtension(QObject *parent)
  : ConfigExtension("", parent), _timeSlotOverride(TimeSlotOverride::None)
{
  // pass...
}

ConfigObject *
OpenGD77ContactExtension::allocateChild(QMetaProperty &prop, const YAML::Node &node, const Context &ctx) {
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

/* ******************************************************************************************** *
 * Implementation of OpenGD77ContactExtensionReader
 * ******************************************************************************************** */
// Register extension to config reader
AbstractConfigReader *
OpenGD77ContactExtensionReader::instance = DMRContactReader::addExtension(new OpenGD77ContactExtensionReader());

OpenGD77ContactExtensionReader::OpenGD77ContactExtensionReader(QObject *parent)
  : ExtensionReader(parent)
{
  // pass...
}

ConfigObject *
OpenGD77ContactExtensionReader::allocate(const YAML::Node &node, const ConfigObject::Context &ctx) {
  Q_UNUSED(ctx)
  Q_UNUSED(node)
  return new OpenGD77ContactExtension();
}


