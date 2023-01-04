#include "roamingchannel.hh"

RoamingChannel::RoamingChannel(QObject *parent)
  : ConfigObject("rc", parent), _rxFrequency(0), _txFrequency(0), _overrideColorCode(false),
    _colorCode(0), _overrideTimeSlot(false), _timeSlot(DMRChannel::TimeSlot::TS1)
{
  // pass...
}

RoamingChannel::RoamingChannel(const RoamingChannel &other, QObject *parent)
  : ConfigObject("rc", parent)
{
  copy(other);
}

void
RoamingChannel::clear() {
  ConfigObject::clear();
  _rxFrequency = _txFrequency = 0;
  _overrideColorCode = false; _colorCode = 0;
  _overrideTimeSlot = false; _timeSlot = DMRChannel::TimeSlot::TS1;
}

double
RoamingChannel::rxFrequency() const {
  return _rxFrequency;
}
void
RoamingChannel::setRXFrequency(double f) {
  if (f == _rxFrequency)
    return;
  _rxFrequency = f;
  emit modified(this);
}

double
RoamingChannel::txFrequency() const {
  return _txFrequency;
}
void
RoamingChannel::setTXFrequency(double f) {
  if (f == _txFrequency)
    return;
  _txFrequency = f;
  emit modified(this);
}

bool
RoamingChannel::colorCodeOverridden() const {
  return _overrideColorCode;
}
void
RoamingChannel::overrideColorCode(bool override) {
  if (override == _overrideColorCode)
    return;
  _overrideColorCode = override;
  emit modified(this);
}
unsigned int
RoamingChannel::colorCode() const {
  return _colorCode;
}
void
RoamingChannel::setColorCode(unsigned int cc) {
  cc = std::min(15U, cc);
  if (_colorCode == cc)
    return;
  _colorCode = cc;
  emit modified(this);
}

bool
RoamingChannel::timeSlotOverridden() const {
  return _overrideTimeSlot;
}
void
RoamingChannel::overrideTimeSlot(bool override) {
  if (override == _overrideTimeSlot)
    return;
  _overrideTimeSlot = override;
  emit modified(this);
}
DMRChannel::TimeSlot
RoamingChannel::timeSlot() const {
  return _timeSlot;
}
void
RoamingChannel::setTimeSlot(DMRChannel::TimeSlot ts) {
  if (_timeSlot == ts)
    return;
  _timeSlot = ts;
  emit modified(this);
}
