#include "anytone_codeplug.hh"

/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::ChannelElement
 * ********************************************************************************************* */
AnytoneCodeplug::ChannelElement::ChannelElement(uint8_t *ptr, uint size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::ChannelElement::ChannelElement(uint8_t *ptr)
  : Element(ptr, 0x0040)
{
  // pass...
}

AnytoneCodeplug::ChannelElement::~ChannelElement() {
  // pass...
}

void
AnytoneCodeplug::ChannelElement::clear() {
  setRXFrequency(0);
  setTXOffset(0);
  setMode(Mode::Analog);
  setPower(Channel::Power::Low);
  setBandwidth(AnalogChannel::Bandwidth::Narrow);
  setBit(0x0008, 5, false); // Unused set to 0
}

uint
AnytoneCodeplug::ChannelElement::rxFrequency() const {
  return ((uint)getBCD8_be(0x0000))*10;
}
void
AnytoneCodeplug::ChannelElement::setRXFrequency(uint hz) {
  setBCD8_be(0x0000, hz/10);
}

uint
AnytoneCodeplug::ChannelElement::txOffset() const {
  return ((uint)getBCD8_be(0x0004))*10;
}
void
AnytoneCodeplug::ChannelElement::setTXOffset(uint hz) {
  setBCD8_be(0x0004, hz/10);
}

AnytoneCodeplug::ChannelElement::Mode
AnytoneCodeplug::ChannelElement::mode() const {
  return (Mode) getUInt2(0x0008, 0);
}
void
AnytoneCodeplug::ChannelElement::setMode(Mode mode) {
  setUInt2(0x0008, 0, (uint)mode);
}

Channel::Power
AnytoneCodeplug::ChannelElement::power() const {
  switch ((Power)getUInt2(0x0008, 2)) {
  case POWER_LOW: return Channel::Power::Low;
  case POWER_MIDDLE: return Channel::Power::Mid;
  case POWER_HIGH: return Channel::Power::High;
  case POWER_TURBO: return Channel::Power::Max;
  }
  return Channel::Power::Low;
}
void
AnytoneCodeplug::ChannelElement::setPower(Channel::Power power) {
  switch (power) {
  case Channel::Power::Min:
  case Channel::Power::Low:
    setUInt2(0x0008, 2, (uint)POWER_LOW);
    break;
  case Channel::Power::Mid:
    setUInt2(0x0008, 2, (uint)POWER_MIDDLE);
    break;
  case Channel::Power::High:
    setUInt2(0x0008, 2, (uint)POWER_HIGH);
    break;
  case Channel::Power::Max:
    setUInt2(0x0008, 2, (uint)POWER_TURBO);
    break;
  }
}

AnalogChannel::Bandwidth
AnytoneCodeplug::ChannelElement::bandwidth() const {
  if (getBit(0x0008, 4))
    return AnalogChannel::Bandwidth::Wide;
  return AnalogChannel::Bandwidth::Narrow;
}
void
AnytoneCodeplug::ChannelElement::setBandwidth(AnalogChannel::Bandwidth bw) {
  switch (bw) {
  case AnalogChannel::Bandwidth::Narrow: setBit(0x0008, 4, false); break;
  case AnalogChannel::Bandwidth::Wide: setBit(0x0008, 4, true); break;
  }
}

AnytoneCodeplug::ChannelElement::RepeaterMode
AnytoneCodeplug::ChannelElement::repeaterMode() const {
  return (RepeaterMode)getUInt2(0x0008, 6);
}
void
AnytoneCodeplug::ChannelElement::setRepeaterMode(RepeaterMode mode) {
  setUInt2(0x0008, 6, (uint)mode);
}

AnytoneCodeplug::ChannelElement::SignalingMode
AnytoneCodeplug::ChannelElement::rxSignalingMode() const {
  return (SignalingMode)getUInt2(0x0009, 0);
}
void
AnytoneCodeplug::ChannelElement::setRXSignalingMode(SignalingMode mode) {
  setUInt2(0x0009, 0, (uint)mode);
}

AnytoneCodeplug::ChannelElement::SignalingMode
AnytoneCodeplug::ChannelElement::txSignalingMode() const {
  return (SignalingMode)getUInt2(0x0009, 2);
}
void
AnytoneCodeplug::ChannelElement::setTXSignalingMode(SignalingMode mode) {
  setUInt2(0x0009, 2, (uint)mode);
}

bool
AnytoneCodeplug::ChannelElement::ctcssPhaseReversal() const {
  return getBit(0x0009, 4);
}
void
AnytoneCodeplug::ChannelElement::enableCTCSSPhaseReversal(bool enable) {
  setBit(0x0009, 4, enable);
}
bool
AnytoneCodeplug::ChannelElement::rxOnly() const {
  return getBit(0x0009, 5);
}
void
AnytoneCodeplug::ChannelElement::enableRXOnly(bool enable) {
  setBit(0x0009, 5, enable);
}
bool
AnytoneCodeplug::ChannelElement::callConfirm() const {
  return getBit(0x0009, 6);
}
void
AnytoneCodeplug::ChannelElement::enableCallConfirm(bool enable) {
  setBit(0x0009, 6, enable);
}
bool
AnytoneCodeplug::ChannelElement::talkaround() const {
  return getBit(0x0009, 7);
}
void
AnytoneCodeplug::ChannelElement::enableTalkaround(bool enable) {
  setBit(0x0009, 7, enable);
}




/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug
 * ********************************************************************************************* */
AnytoneCodeplug::AnytoneCodeplug(QObject *parent)
  : CodePlug(parent)
{
  // pass...
}

AnytoneCodeplug::~AnytoneCodeplug() {
  // pass...
}

bool
AnytoneCodeplug::index(Config *config, Context &ctx) const {
  return true;
}


