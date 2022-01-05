#include "kydera_codeplug.hh"

#define CHANNEL_SIZE        0x0020

/* ********************************************************************************************* *
 * Implementation of KyderaCodeplug::AddressElement
 * ********************************************************************************************* */
KyderaCodeplug::AddressElement::AddressElement(uint8_t *ptr)
  : Codeplug::Element(ptr, 0x0004)
{
  // pass...
}

unsigned
KyderaCodeplug::AddressElement::address() const {
  return getUInt32_le(0x0000);
}
void
KyderaCodeplug::AddressElement::setAddress(unsigned addr) {
  setUInt32_le(0x0000, addr);
}


/* ********************************************************************************************* *
 * Implementation of KyderaCodeplug::ChannelElement
 * ********************************************************************************************* */
KyderaCodeplug::ChannelElement::ChannelElement(uint8_t *ptr, unsigned size)
  : Codeplug::Element(ptr, size)
{
  // pass...
}

KyderaCodeplug::ChannelElement::ChannelElement(uint8_t *ptr)
  : Codeplug::Element(ptr, CHANNEL_SIZE)
{
  // pass...
}

KyderaCodeplug::ChannelElement::~ChannelElement() {
  // pass...
}

void
KyderaCodeplug::ChannelElement::clear() {
  memset(_data, 0x00, _size);
}

unsigned
KyderaCodeplug::ChannelElement::index() const {
  return getUInt16_le(0x0000);
}
void
KyderaCodeplug::ChannelElement::setIndex(unsigned idx) {
  setUInt16_le(0x0000, idx);
}

QString
KyderaCodeplug::ChannelElement::name() const {
  return readASCII(0x0002, 10, 0x00);
}
void
KyderaCodeplug::ChannelElement::setName(const QString &name) {
  writeASCII(0x0002, name, 10, 0x00);
}

uint32_t
KyderaCodeplug::ChannelElement::rxFrequency() const {
  return getUInt32_le(0x000c)*10;
}
void
KyderaCodeplug::ChannelElement::setRXFrequency(uint32_t freq) {
  setUInt32_le(0x000c, freq/10);
}

uint32_t
KyderaCodeplug::ChannelElement::txFrequency() const {
  return getUInt32_le(0x0010)*10;
}
void
KyderaCodeplug::ChannelElement::setTXFrequency(uint32_t freq) {
  setUInt32_le(0x0010, freq/10);
}

KyderaCodeplug::ChannelElement::Mode
KyderaCodeplug::ChannelElement::mode() const {
  return (Mode)getUInt2(0x0014, 6);
}
void
KyderaCodeplug::ChannelElement::setMode(Mode mode) {
  setUInt2(0x0014, 6, (unsigned)mode);
}

Channel::Power
KyderaCodeplug::ChannelElement::power() const {
  if (getBit(0x0014, 5))
    return Channel::Power::High;
  return Channel::Power::Low;
}
void
KyderaCodeplug::ChannelElement::setPower(Channel::Power power) {
  switch (power) {
  case Channel::Power::Min:
  case Channel::Power::Low:
  case Channel::Power::Mid:
    clearBit(0x0014, 5);
    break;
  case Channel::Power::High:
  case Channel::Power::Max:
    setBit(0x0014, 5);
    break;
  }
}

AnalogChannel::Bandwidth
KyderaCodeplug::ChannelElement::bandwidth() const {
  if (getBit(0x0014, 4))
    return AnalogChannel::Bandwidth::Wide;
  return AnalogChannel::Bandwidth::Narrow;
}
void
KyderaCodeplug::ChannelElement::setBandwidth(AnalogChannel::Bandwidth bw) {
  switch (bw) {
  case AnalogChannel::Bandwidth::Narrow:
    clearBit(0x0014, 4);
    break;
  case AnalogChannel::Bandwidth::Wide:
    setBit(0x0014, 4);
  }
}


/* ********************************************************************************************* *
 * Implementation of actual KyderaCodeplug
 * ********************************************************************************************* */
KyderaCodeplug::KyderaCodeplug(QObject *parent)
  : Codeplug(parent)
{
  // The codeplug consists of a single element.
  addImage("Kydera CDR-300UV codeplug");
}

bool
KyderaCodeplug::index(Config *config, Context &ctx, const ErrorStack &err) const {
  return false;
}



