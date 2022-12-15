#include "dmr6x2uv_codeplug.hh"

/* ********************************************************************************************* *
 * Implementation of DMR6X2UVCodeplug::GeneralSettingsElement
 * ********************************************************************************************* */
DMR6X2UVCodeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr, unsigned size)
  : D878UVCodeplug::GeneralSettingsElement(ptr, size)
{
  // pass...
}

DMR6X2UVCodeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr)
  : D878UVCodeplug::GeneralSettingsElement(ptr, 0x0100)
{
  // pass...
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::simplexRepeaterEnabled() const {
  return 0x01 == getUInt8(0x00b2);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableSimplexRepeater(bool enable) {
  setUInt8(0x00b2, enable ? 0x01 : 0x00);
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::monitorSimplexRepeaterEnabled() const {
  return 0x01 == getUInt8(0x00b3);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableMonitorSimplexRepeater(bool enable) {
  setUInt8(0x00b3, enable ? 0x01 : 0x00);
}

DMR6X2UVCodeplug::GeneralSettingsElement::SimplexRepeaterSlot
DMR6X2UVCodeplug::GeneralSettingsElement::simplexRepeaterTimeslot() const {
  switch (getUInt8(0x00b7)) {
  case 0x00:
    return SimplexRepeaterSlot::TS1;
  case 0x01:
    return SimplexRepeaterSlot::TS2;
  case 0x02:
  default:
    return SimplexRepeaterSlot::Channel;
  }
}

void
DMR6X2UVCodeplug::GeneralSettingsElement::setSimplexRepeaterTimeslot(SimplexRepeaterSlot slot) {
  switch (slot) {
  case SimplexRepeaterSlot::TS1:     setUInt8(0x00b7, 0x00); break;
  case SimplexRepeaterSlot::TS2:     setUInt8(0x00b7, 0x01); break;
  case SimplexRepeaterSlot::Channel: setUInt8(0x00b7, 0x02); break;
  }
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::fromConfig(const Flags &flags, Context &ctx)
{
  if (! D878UVCodeplug::GeneralSettingsElement::fromConfig(flags, ctx))
    return false;
  // apply DMR-6X2UV specific settings.
  return true;
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::updateConfig(Context &ctx) {
  if (! D878UVCodeplug::GeneralSettingsElement::updateConfig(ctx))
    return false;
  // Extract DMR-6X2UV specific settings.
  return true;
}


/* ********************************************************************************************* *
 * Implementation of DMR6X2UVCodeplug
 * ********************************************************************************************* */
DMR6X2UVCodeplug::DMR6X2UVCodeplug(QObject *parent)
  : D878UVCodeplug(parent)
{
  // pass...
}


