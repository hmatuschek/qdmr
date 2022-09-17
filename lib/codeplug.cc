#include "codeplug.hh"
#include "config.hh"
#include <QtEndian>
#include "logger.hh"


/* ********************************************************************************************* *
 * Implementation of CodePlug::Flags
 * ********************************************************************************************* */
Codeplug::Flags::Flags()
  : updateCodePlug(true), autoEnableGPS(false), autoEnableRoaming(false)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of CodePlug::Element
 * ********************************************************************************************* */
Codeplug::Element::Element(uint8_t *ptr, size_t size)
  : _data(ptr), _size(size)
{
  // pass...
}

Codeplug::Element::Element(const Element &other)
  : _data(other._data), _size(other._size)
{
  // pass...
}

Codeplug::Element::~Element() {
  // pass...
}

bool
Codeplug::Element::isValid() const {
  return nullptr != _data;
}

void
Codeplug::Element::clear() {
  // pass...
}

bool
Codeplug::Element::fill(uint8_t value, unsigned offset, int size) {
  if (0 > size)
    size = _size-offset;
  if ((offset+size) > _size) {
    logFatal() << "Cannot fill codeplug element from " << QString::number(offset,16)
               << " size " << QString::number(size, 16) << ": overflow.";
    return false;
  }
  memset(_data+offset, value, size);
  return true;
}

bool
Codeplug::Element::getBit(unsigned offset, unsigned bit) const {
  if (offset >= _size) {
    logFatal() << "Cannot get bit at " << QString::number(offset, 16)
               << " bit " << bit << ": Overflow.";
    return false;
  }

  uint8_t *ptr = (_data+offset);
  return (1<<bit) & (*ptr);
}

void
Codeplug::Element::setBit(unsigned offset, unsigned bit, bool value) {
  if (offset >= _size) {
    logFatal() << "Cannot set bit at " << QString::number(offset, 16)
               << " bit " << bit << ": Overflow.";
    return;
  }

  uint8_t *ptr = (_data+offset);
  if (value)
    (*ptr) |= (1<<bit);
  else
    (*ptr) &= ~(1<<bit);
}

void
Codeplug::Element::clearBit(unsigned offset, unsigned bit) {
  if (offset >= _size) {
    logFatal() << "Cannot clear bit at " << QString::number(offset, 16)
               << " bit " << bit << ": Overflow.";
    return;
  }

  uint8_t *ptr = (_data+offset);
  (*ptr) &= ~(1<<bit);
}

uint8_t
Codeplug::Element::getUInt2(unsigned offset, unsigned bit) const {
  if (offset >= _size) {
    logFatal() << "Cannot get uint2 at " << QString::number(offset, 16)
               << " bit " << bit << ": Overflow.";
    return 0;
  }

  return (((*(_data+offset)) >> bit) & 0b11);
}
void
Codeplug::Element::setUInt2(unsigned offset, unsigned bit, uint8_t value) {
  if (offset >= _size) {
    logFatal() << "Cannot set uint2 at " << QString::number(offset, 16)
               << " bit " << bit << ": Overflow.";
    return;
  }

  *(_data+offset) &= ~(0b11 << bit);
  *(_data+offset) |= ((value & 0b11)<<bit);
}

uint8_t
Codeplug::Element::getUInt3(unsigned offset, unsigned bit) const {
  if (offset >= _size) {
    logFatal() << "Cannot get uint3 at " << QString::number(offset, 16)
               << " bit " << bit << ": Overflow.";
    return 0;
  }

  return (((*(_data+offset)) >> bit) & 0b111);
}
void
Codeplug::Element::setUInt3(unsigned offset, unsigned bit, uint8_t value) {
  if (offset >= _size) {
    logFatal() << "Cannot set uint3 at " << QString::number(offset, 16)
               << " bit " << bit << ": Overflow.";
    return;
  }

  *(_data+offset) &= ~(0b111 << bit);
  *(_data+offset) |= ((value & 0b111)<<bit);
}

uint8_t
Codeplug::Element::getUInt4(unsigned offset, unsigned bit) const {
  if (offset >= _size) {
    logFatal() << "Cannot get uint4 at " << QString::number(offset, 16)
               << " bit " << bit << ": Overflow.";
    return 0;
  }

  return (((*(_data+offset)) >> bit) & 0b1111);
}
void
Codeplug::Element::setUInt4(unsigned offset, unsigned bit, uint8_t value) {
  if (offset >= _size) {
    logFatal() << "Cannot set uint4 at " << QString::number(offset, 16)
               << " bit " << bit << ": Overflow.";
    return;
  }

  *(_data+offset) &= ~(0b1111 << bit);
  *(_data+offset) |= ((value & 0b1111)<<bit);
}

uint8_t
Codeplug::Element::getUInt5(unsigned offset, unsigned bit) const {
  if (offset >= _size) {
    logFatal() << "Cannot get uint5 at " << QString::number(offset, 16)
               << " bit " << bit << ": Overflow.";
    return 0;
  }

  return (((*(_data+offset)) >> bit) & 0b11111);
}
void
Codeplug::Element::setUInt5(unsigned offset, unsigned bit, uint8_t value) {
  if (offset >= _size) {
    logFatal() << "Cannot get uint5 at " << QString::number(offset, 16)
               << " bit " << bit << ": Overflow.";
    return;
  }

  *(_data+offset) &= ~(0b11111 << bit);
  *(_data+offset) |= ((value & 0b11111)<<bit);
}

uint8_t
Codeplug::Element::getUInt6(unsigned offset, unsigned bit) const {
  if (offset >= _size) {
    logFatal() << "Cannot get uint6 at " << QString::number(offset, 16)
               << " bit " << bit << ": Overflow.";
    return 0;
  }

  return (((*(_data+offset)) >> bit) & 0b111111);
}
void
Codeplug::Element::setUInt6(unsigned offset, unsigned bit, uint8_t value) {
  if (offset >= _size) {
    logFatal() << "Cannot set uint6 at " << QString::number(offset, 16)
               << " bit " << bit << ": Overflow.";
    return;
  }

  *(_data+offset) &= ~(0b111111 << bit);
  *(_data+offset) |= ((value & 0b111111)<<bit);
}

uint8_t
Codeplug::Element::getUInt8(unsigned offset) const {
  if (offset >= _size) {
    logFatal() << "Cannot get uint8 at " << QString::number(offset, 16) << ": Overflow.";
    return 0;
  }

  return _data[offset];
}
void
Codeplug::Element::setUInt8(unsigned offset, uint8_t value) {
  if (offset >= _size) {
    logFatal() << "Cannot set uint8 at " << QString::number(offset, 16) << ": Overflow.";
    return;
  }

  _data[offset] = value;
}

int8_t
Codeplug::Element::getInt8(unsigned offset) const {
  if (offset >= _size) {
    logFatal() << "Cannot get int8 at " << QString::number(offset, 16) << ": Overflow.";
    return 0;
  }
  return ((int8_t *)_data)[offset];
}
void
Codeplug::Element::setInt8(unsigned offset, int8_t value) {
  if (offset >= _size) {
    logFatal() << "Cannot set int8 at " << QString::number(offset, 16) << ": Overflow.";
    return;
  }

  ((int8_t *)_data)[offset] = value;
}

uint16_t
Codeplug::Element::getUInt16_be(unsigned offset) const {
  if ((offset+2) > _size) {
    logFatal() << "Cannot get int16 (be) at " << QString::number(offset, 16) << ": Overflow.";
    return 0;
  }

  uint16_t *ptr = (uint16_t *)(_data+offset);
  return qFromBigEndian(*ptr);
}
uint16_t
Codeplug::Element::getUInt16_le(unsigned offset) const {
  if ((offset+2) > _size) {
    logFatal() << "Cannot get int16 (le) at " << QString::number(offset, 16) << ": Overflow.";
    return 0;
  }

  uint16_t *ptr = (uint16_t *)(_data+offset);
  return qFromLittleEndian(*ptr);
}
void
Codeplug::Element::setUInt16_be(unsigned offset, uint16_t value) {
  if ((offset+2) > _size) {
    logFatal() << "Cannot set int16 (be) at " << QString::number(offset, 16) << ": Overflow.";
    return;
  }

  uint16_t *ptr = (uint16_t *)(_data+offset);
  (*ptr) = qToBigEndian(value);
}
void
Codeplug::Element::setUInt16_le(unsigned offset, uint16_t value) {
  if ((offset+2) > _size) {
    logFatal() << "Cannot set int16 (le) at " << QString::number(offset, 16) << ": Overflow.";
    return;
  }

  uint16_t *ptr = (uint16_t *)(_data+offset);
  (*ptr) = qToLittleEndian(value);
}

uint32_t
Codeplug::Element::getUInt24_be(unsigned offset) const {
  if ((offset+3) > _size) {
    logFatal() << "Cannot get int24 (be) at " << QString::number(offset, 16) << ": Overflow.";
    return 0;
  }

  uint8_t *ptr = _data+offset;
  return uint32_t(ptr[2]) + (uint32_t(ptr[1])<<8) + (uint32_t(ptr[0])<<16);
}
uint32_t
Codeplug::Element::getUInt24_le(unsigned offset) const {
  if ((offset+3) > _size) {
    logFatal() << "Cannot get int24 (le) at " << QString::number(offset, 16) << ": Overflow.";
    return 0;
  }

  uint8_t *ptr = _data+offset;
  return uint32_t(ptr[0]) + (uint32_t(ptr[1])<<8) + (uint32_t(ptr[2])<<16);
}
void
Codeplug::Element::setUInt24_be(unsigned offset, uint32_t value) {
  if ((offset+3) > _size) {
    logFatal() << "Cannot set int24 (be) at " << QString::number(offset, 16) << ": Overflow.";
    return;
  }

  uint8_t *ptr = _data+offset;
  ptr[0] = ((value >> 16) & 0xff);
  ptr[1] = ((value >> 8)  & 0xff);
  ptr[2] = ((value >> 0)  & 0xff);
}
void
Codeplug::Element::setUInt24_le(unsigned offset, uint32_t value) {
  if ((offset+3) > _size) {
    logFatal() << "Cannot set int24 (le) at " << QString::number(offset, 16) << ": Overflow.";
    return;
  }

  uint8_t *ptr = _data+offset;
  ptr[0] = ((value >> 0)  & 0xff);
  ptr[1] = ((value >> 8)  & 0xff);
  ptr[2] = ((value >> 16) & 0xff);
}

uint32_t
Codeplug::Element::getUInt32_be(unsigned offset) const {
  if ((offset+4) > _size) {
    logFatal() << "Cannot get int32 (be) at " << QString::number(offset, 16) << ": Overflow.";
    return 0;
  }

  uint32_t *ptr = (uint32_t *)(_data+offset);
  return qFromBigEndian(*ptr);
}
uint32_t
Codeplug::Element::getUInt32_le(unsigned offset) const {
  if ((offset+4) > _size) {
    logFatal() << "Cannot get int32 (le) at " << QString::number(offset, 16) << ": Overflow.";
    return 0;
  }

  uint32_t *ptr = (uint32_t *)(_data+offset);
  return qFromLittleEndian(*ptr);
}
void
Codeplug::Element::setUInt32_be(unsigned offset, uint32_t value) {
  if ((offset+4) > _size) {
    logFatal() << "Cannot set int32 (be) at " << QString::number(offset, 16) << ": Overflow.";
    return;
  }

  uint32_t *ptr = (uint32_t *)(_data+offset);
  (*ptr) = qToBigEndian(value);
}
void
Codeplug::Element::setUInt32_le(unsigned offset, uint32_t value) {
  if ((offset+4) > _size) {
    logFatal() << "Cannot set int32 (le) at " << QString::number(offset, 16) << ": Overflow.";
    return;
  }

  uint32_t *ptr = (uint32_t *)(_data+offset);
  (*ptr) = qToLittleEndian(value);
}

uint64_t
Codeplug::Element::getUInt64_be(unsigned offset) const {
  if ((offset+8) > _size) {
    logFatal() << "Cannot get int64 (be) at " << QString::number(offset, 16) << ": Overflow.";
    return 0;
  }

  quint64 *ptr = (quint64 *)(_data+offset);
  return qFromBigEndian<quint64>(*ptr);
}
uint64_t
Codeplug::Element::getUInt64_le(unsigned offset) const {
  if ((offset+8) > _size) {
    logFatal() << "Cannot get int64 (le) at " << QString::number(offset, 16) << ": Overflow.";
    return 0;
  }

  quint64 *ptr = (quint64 *)(_data+offset);
  return qFromLittleEndian<quint64>(*ptr);
}
void
Codeplug::Element::setUInt64_be(unsigned offset, uint64_t value) {
  if ((offset+8) > _size) {
    logFatal() << "Cannot set int64 (be) at " << QString::number(offset, 16) << ": Overflow.";
    return;
  }

  quint64 *ptr = (quint64 *)(_data+offset);
  (*ptr) = qToBigEndian<quint64>(value);
}
void
Codeplug::Element::setUInt64_le(unsigned offset, uint64_t value) {
  if ((offset+8) > _size) {
    logFatal() << "Cannot set int64 (le) at " << QString::number(offset, 16) << ": Overflow.";
    return;
  }

  quint64 *ptr = (quint64 *)(_data+offset);
  (*ptr) = qToLittleEndian<quint64>(value);
}

uint8_t
Codeplug::Element::getBCD2(unsigned offset) const {
  if ((offset+1) > _size) {
    logFatal() << "Cannot get BCD2 at " << QString::number(offset, 16) << ": Overflow.";
    return 0;
  }

  uint8_t val = getUInt8(offset);
  return (val & 0xf) + ((val>>4) & 0xf)*10;
}
void
Codeplug::Element::setBCD2(unsigned offset, uint8_t val) {
  if ((offset+1) > _size) {
    logFatal() << "Cannot get BCD2 at " << QString::number(offset, 16) << ": Overflow.";
    return;
  }

  uint8_t a  = (val / 10) % 10;
  uint8_t b  = (val /  1) % 10;
  setUInt8(offset, (a << 4) + b);
}

uint16_t
Codeplug::Element::getBCD4_be(unsigned offset) const {
  if ((offset+2) > _size) {
    logFatal() << "Cannot get BCD4 (be) at " << QString::number(offset, 16) << ": Overflow.";
    return 0;
  }

  uint32_t val = getUInt16_be(offset);
  return (val & 0xf) + ((val>>4) & 0xf)*10 + ((val>>8) & 0xf)*100 + ((val>>12) & 0xf)*1000;
}
void
Codeplug::Element::setBCD4_be(unsigned offset, uint16_t val) {
  if ((offset+2) > _size) {
    logFatal() << "Cannot set BCD4 (be) at " << QString::number(offset, 16) << ": Overflow.";
    return;
  }

  uint32_t a  = (val / 1000) % 10;
  uint32_t b  = (val /  100) % 10;
  uint32_t c  = (val /   10) % 10;
  uint32_t d  = (val /    1) % 10;
  setUInt16_be(offset, (a << 12) + (b << 8) + (c << 4) + d);
}
uint16_t
Codeplug::Element::getBCD4_le(unsigned offset) const {
  if ((offset+2) > _size) {
    logFatal() << "Cannot get BCD4 (le) at " << QString::number(offset, 16) << ": Overflow.";
    return 0;
  }

  uint32_t val = getUInt16_le(offset);
  return (val & 0xf) + ((val>>4) & 0xf)*10 + ((val>>8) & 0xf)*100 + ((val>>12) & 0xf)*1000;
}
void
Codeplug::Element::setBCD4_le(unsigned offset, uint16_t val) {
  if ((offset+2) > _size) {
    logFatal() << "Cannot set BCD4 (le) at " << QString::number(offset, 16) << ": Overflow.";
    return;
  }

  uint32_t a  = (val / 1000) % 10;
  uint32_t b  = (val /  100) % 10;
  uint32_t c  = (val /   10) % 10;
  uint32_t d  = (val /    1) % 10;
  setUInt16_le(offset, (a << 12) + (b << 8) + (c << 4) + d);
}

uint32_t
Codeplug::Element::getBCD8_be(unsigned offset) const {
  if ((offset+4) > _size) {
    logFatal() << "Cannot get BCD8 (be) at " << QString::number(offset, 16) << ": Overflow.";
    return 0;
  }

  uint32_t val = getUInt32_be(offset);
  return (val & 0xf) + ((val>>4) & 0xf)*10 + ((val>>8) & 0xf)*100 + ((val>>12) & 0xf)*1000 +
      ((val>>16) & 0xf)*10000 + ((val>>20) & 0xf)*100000 + ((val>>24) & 0xf)*1000000 +
      ((val>>28) & 0xf)*10000000;
}
void
Codeplug::Element::setBCD8_be(unsigned offset, uint32_t val) {
  if ((offset+4) > _size) {
    logFatal() << "Cannot set BCD8 (be) at " << QString::number(offset, 16) << ": Overflow.";
    return;
  }

  uint32_t a  = (val / 10000000) % 10;
  uint32_t b  = (val /  1000000) % 10;
  uint32_t c  = (val /   100000) % 10;
  uint32_t d  = (val /    10000) % 10;
  uint32_t e  = (val /     1000) % 10;
  uint32_t f  = (val /      100) % 10;
  uint32_t g  = (val /       10) % 10;
  uint32_t h  = (val /        1) % 10;
  setUInt32_be(offset, (a << 28) + (b << 24) + (c << 20) + (d << 16) + (e << 12) + (f << 8) + (g << 4) + h);
}
uint32_t
Codeplug::Element::getBCD8_le(unsigned offset) const {
  if ((offset+4) > _size) {
    logFatal() << "Cannot get BCD8 (le) at " << QString::number(offset, 16) << ": Overflow.";
    return 0;
  }

  uint32_t val = getUInt32_le(offset);
  return (val & 0xf) + ((val>>4) & 0xf)*10 + ((val>>8) & 0xf)*100 + ((val>>12) & 0xf)*1000 +
      ((val>>16) & 0xf)*10000 + ((val>>20) & 0xf)*100000 + ((val>>24) & 0xf)*1000000 +
      ((val>>28) & 0xf)*10000000;
}
void
Codeplug::Element::setBCD8_le(unsigned offset, uint32_t val) {
  if ((offset+4) > _size) {
    logFatal() << "Cannot set BCD8 (le) at " << QString::number(offset, 16) << ": Overflow.";
    return;
  }

  uint32_t a  = (val / 10000000) % 10;
  uint32_t b  = (val /  1000000) % 10;
  uint32_t c  = (val /   100000) % 10;
  uint32_t d  = (val /    10000) % 10;
  uint32_t e  = (val /     1000) % 10;
  uint32_t f  = (val /      100) % 10;
  uint32_t g  = (val /       10) % 10;
  uint32_t h  = (val /        1) % 10;
  setUInt32_le(offset, (a << 28) + (b << 24) + (c << 20) + (d << 16) + (e << 12) + (f << 8) + (g << 4) + h);
}

QString
Codeplug::Element::readASCII(unsigned offset, unsigned maxlen, uint8_t eos) const {
  QString txt;
  uint8_t *ptr = (uint8_t *)(_data+offset);
  for (unsigned i=0; (i<maxlen)&&(ptr[i])&&(eos!=ptr[i]); i++) {
    txt.append(QChar::fromLatin1(ptr[i]));
  }
  return txt;
}
void
Codeplug::Element::writeASCII(unsigned offset, const QString &txt, unsigned maxlen, uint8_t eos) {
  uint8_t *ptr = (uint8_t *)(_data+offset);
  for (unsigned i=0; i<maxlen; i++) {
    if (i < unsigned(txt.length()))
      ptr[i] = txt.at(i).toLatin1();
    else
      ptr[i] = eos;
  }
}

QString
Codeplug::Element::readUnicode(unsigned offset, unsigned maxlen, uint16_t eos) const {
  QString txt;
  uint16_t *ptr = (uint16_t *)(_data+offset);
  for (unsigned i=0; (i<maxlen)&&(eos!=ptr[i]); i++) {
    txt.append(QChar(ptr[i]));
  }
  return txt;
}
void
Codeplug::Element::writeUnicode(unsigned offset, const QString &txt, unsigned maxlen, uint16_t eos) {
  uint16_t *ptr = (uint16_t *)(_data+offset);
  for (unsigned i=0; i<maxlen; i++) {
    if (i < unsigned(txt.length()))
      ptr[i] = txt.at(i).unicode();
    else
      ptr[i] = eos;
  }
}


/* ********************************************************************************************* *
 * Implementation of CodePlug::Context
 * ********************************************************************************************* */
Codeplug::Context::Context(Config *config)
  : _config(config), _tables()
{
  // Add tables for common elements
  addTable(&DMRRadioID::staticMetaObject);
  addTable(&DMRContact::staticMetaObject);
  addTable(&DTMFContact::staticMetaObject);
  addTable(&RXGroupList::staticMetaObject);
  addTable(&Channel::staticMetaObject);
  addTable(&Zone::staticMetaObject);
  addTable(&ScanList::staticMetaObject);
  addTable(&GPSSystem::staticMetaObject);
  addTable(&APRSSystem::staticMetaObject);
  addTable(&RoamingZone::staticMetaObject);
}

Config  *
Codeplug::Context::config() const {
  return _config;
}

bool
Codeplug::Context::hasTable(const QMetaObject *obj) const {
  // Find a matching table
  if (_tables.contains(obj->className()))
    return true;
  else if (obj->superClass())
    return hasTable(obj->superClass());
  return false;
}

Codeplug::Context::Table &
Codeplug::Context::getTable(const QMetaObject *obj) {
  if (_tables.contains(obj->className()))
    return _tables[obj->className()];
  return getTable(obj->superClass());
}

bool
Codeplug::Context::addTable(const QMetaObject *obj) {
  if (hasTable(obj))
    return false;
  _tables.insert(obj->className(), Table());
  return true;
}

ConfigItem *
Codeplug::Context::obj(const QMetaObject *elementType, unsigned idx) {
  if (! hasTable(elementType))
    return nullptr;
  return getTable(elementType).objects.value(idx, nullptr);
}

int
Codeplug::Context::index(ConfigItem *obj) {
  if (nullptr == obj)
    return -1;
  if (! hasTable(obj->metaObject()))
    return -1;
  return getTable(obj->metaObject()).indices.value(obj, -1);
}

bool
Codeplug::Context::add(ConfigItem *obj, unsigned idx) {
  if (!hasTable(obj->metaObject()))
    return false;
  if (getTable(obj->metaObject()).indices.contains(obj))
    return false;
  if (getTable(obj->metaObject()).objects.contains(idx))
    return false;
  getTable(obj->metaObject()).objects.insert(idx, obj);
  getTable(obj->metaObject()).indices.insert(obj, idx);
  return true;
}


/* ********************************************************************************************* *
 * Implementation of CodePlug
 * ********************************************************************************************* */
Codeplug::Codeplug(QObject *parent)
  : DFUFile(parent)
{
	// pass...
}

Codeplug::~Codeplug() {
	// pass...
}
