#include "opengd77base_callsigndb.hh"
#include "utils.hh"
#include "userdatabase.hh"
#include <QtEndian>


/* ******************************************************************************************** *
 * Implementation of OpenGD77BaseCallsignDB::DatabaseEntryElement
 * ******************************************************************************************** */
QVector<QChar>
OpenGD77BaseCallsignDB::DatabaseEntryElement::_lut =
{
  ' ', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
  'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
  'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
  'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
  's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '.'
};

OpenGD77BaseCallsignDB::DatabaseEntryElement::DatabaseEntryElement(uint8_t *ptr, size_t size)
  : Codeplug::Element(ptr, size)
{
  // pass...
}

OpenGD77BaseCallsignDB::DatabaseEntryElement::DatabaseEntryElement(uint8_t *ptr)
  : Codeplug::Element(ptr, size())
{
  // pass...
}

void
OpenGD77BaseCallsignDB::DatabaseEntryElement::clear() {
  memset(_data, 0xff, size());
}

void
OpenGD77BaseCallsignDB::DatabaseEntryElement::setId(unsigned int id) {
  setUInt24_le(Offset::dmrID(), id);
}

void
OpenGD77BaseCallsignDB::DatabaseEntryElement::setText(const QString &text) {
  QVector<uint> codes;
  foreach (QChar c, text) {
    if (! _lut.contains(c))
      continue;
    codes.append(_lut.indexOf(c));
  }
  // Fix size
  codes.resize(Limit::textLength());
  // Encode in blocks of 4
  for (int i=0, o=0; i<codes.size(); i += 4, o += 3) {
    uint32_t encoded
        = ((uint32_t)(codes[i+0] & 0x3f) << 18)
        | ((uint32_t)(codes[i+1] & 0x3f) << 12)
        | ((uint32_t)(codes[i+2] & 0x3f) << 06)
        | ((uint32_t)(codes[i+3] & 0x3f) << 00);
    setUInt24_be(Offset::text() + o, encoded);
  }
}

void
OpenGD77BaseCallsignDB::DatabaseEntryElement::fromEntry(const UserDatabase::User &user) {
  clear();
  setId(user.id);
  setText(user.call + " " + user.name + " " + user.city);
}



/* ******************************************************************************************** *
 * Implementation of OpenGD77BaseCallsignDB::DatabaseHeaderElement
 * ******************************************************************************************** */
OpenGD77BaseCallsignDB::DatabaseHeaderElement::DatabaseHeaderElement(uint8_t *ptr, size_t size)
  : Codeplug::Element(ptr, size)
{
  // pass...
}

OpenGD77BaseCallsignDB::DatabaseHeaderElement::DatabaseHeaderElement(uint8_t *ptr)
  : Codeplug::Element(ptr, size())
{
  // pass...
}


void
OpenGD77BaseCallsignDB::DatabaseHeaderElement::clear() {
  memset(_data, 0, size());
  writeASCII(Offset::magic(), "ID", 2);
  setUInt8(Offset::format(), (unsigned int)Format::Compressed);
  setUInt8(Offset::entrySize(), 0x59); // = 15 bytes -> 16 chars for text
  writeASCII(Offset::version(),"001", 3);
  setUInt32_le(Offset::entryCount(), 0);
}

void
OpenGD77BaseCallsignDB::DatabaseHeaderElement::setEntryCount(unsigned int count) {
  setUInt32_le(Offset::entryCount(), count);
}



/* ******************************************************************************************** *
 * Implementation of OpenGD77BaseCallsignDB
 * ******************************************************************************************** */
OpenGD77BaseCallsignDB::OpenGD77BaseCallsignDB(QObject *parent)
  : CallsignDB(parent)
{
  // pass...
}

OpenGD77BaseCallsignDB::~OpenGD77BaseCallsignDB() {
  // pass...
}
