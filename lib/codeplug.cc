#include "codeplug.hh"
#include "config.hh"
#include <QtEndian>


/* ********************************************************************************************* *
 * Implementation of CodePlug::Flags
 * ********************************************************************************************* */
CodePlug::Flags::Flags()
  : updateCodePlug(true), autoEnableGPS(false), autoEnableRoaming(false)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of CodePlug::Element
 * ********************************************************************************************* */
CodePlug::Element::Element(uint8_t *ptr)
  : _data(ptr)
{
  // pass...
}

CodePlug::Element::Element(const Element &other)
  : _data(other._data)
{
  // pass...
}

CodePlug::Element::~Element() {
  // pass...
}

bool
CodePlug::Element::isValid() const {
  return nullptr != _data;
}

bool
CodePlug::Element::getBit(uint offset, uint bit) const {
  uint8_t *ptr = (_data+offset);
  return (1<<bit) & (*ptr);
}

void
CodePlug::Element::setBit(uint offset, uint bit, bool value) {
  uint8_t *ptr = (_data+offset);
  if (value)
    (*ptr) |= (1<<bit);
  else
    (*ptr) &= ~(1<<bit);
}

void
CodePlug::Element::clearBit(uint offset, uint bit) {
  uint8_t *ptr = (_data+offset);
  (*ptr) &= ~(1<<bit);
}

uint8_t
CodePlug::Element::getUInt8(uint offset) const {
  return _data[offset];
}

void
CodePlug::Element::setUInt8(uint offset, uint8_t value) const {
  _data[offset] = value;
}

uint16_t
CodePlug::Element::getUInt16_be(uint offset) const {
  uint16_t *ptr = (uint16_t *)(_data+offset);
  return qFromBigEndian(*ptr);
}

uint16_t
CodePlug::Element::getUInt16_le(uint offset) const {
  uint16_t *ptr = (uint16_t *)(_data+offset);
  return qFromLittleEndian(*ptr);
}

void
CodePlug::Element::setUInt16_be(uint offset, uint16_t value) {
  uint16_t *ptr = (uint16_t *)(_data+offset);
  (*ptr) = qToBigEndian(value);
}

void
CodePlug::Element::setUInt16_le(uint offset, uint16_t value) {
  uint16_t *ptr = (uint16_t *)(_data+offset);
  (*ptr) = qToLittleEndian(value);
}

uint32_t
CodePlug::Element::getUInt32_be(uint offset) const {
  uint32_t *ptr = (uint32_t *)(_data+offset);
  return qFromBigEndian(*ptr);
}

uint32_t
CodePlug::Element::getUInt32_le(uint offset) const {
  uint32_t *ptr = (uint32_t *)(_data+offset);
  return qFromLittleEndian(*ptr);
}

void
CodePlug::Element::setUInt32_be(uint offset, uint32_t value) {
  uint32_t *ptr = (uint32_t *)(_data+offset);
  (*ptr) = qToBigEndian(value);
}

void
CodePlug::Element::setUInt32_le(uint offset, uint32_t value) {
  uint32_t *ptr = (uint32_t *)(_data+offset);
  (*ptr) = qToLittleEndian(value);
}


/* ********************************************************************************************* *
 * Implementation of CodePlug
 * ********************************************************************************************* */
CodePlug::CodePlug(QObject *parent)
  : DFUFile(parent)
{
	// pass...
}

CodePlug::~CodePlug() {
	// pass...
}
