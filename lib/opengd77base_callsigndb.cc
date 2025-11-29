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
  memset(_data, 0x00, size());
}

void
OpenGD77BaseCallsignDB::DatabaseEntryElement::setId(unsigned int id) {
  setUInt24_le(Offset::dmrID(), id);
}

void
OpenGD77BaseCallsignDB::DatabaseEntryElement::setText(const QString &text) {
  QByteArray data = pack(text);
  auto n = std::min(3*Limit::textLength()/4, (unsigned int)data.size());
  memcpy(_data+Offset::text(), data.constData(), n);
}

bool
OpenGD77BaseCallsignDB::DatabaseEntryElement::fromEntry(const UserDatabase::User &user) {
  clear();
  setId(user.id);

  QString txt;
  QTextStream stream(&txt);
  stream << user.call << " " << user.name;
  if (! user.city.isEmpty())
    stream << "." << user.city;
  if (! user.state.isEmpty())
    stream << "." << user.state;
  if (! user.country.isEmpty())
    stream << "." << user.country;
  setText(txt);

  return true;
}

QByteArray
OpenGD77BaseCallsignDB::DatabaseEntryElement::pack(const QString &text) {
  // Encode chars
  QVector<uint> codes;
  foreach (QChar c, text) {
    if (! _lut.contains(c))
      continue;
    codes.append(_lut.indexOf(c));
  }

  // Fix size to multiples of 4
  if (codes.size() % 4)
    codes.resize(codes.size() + (4-(codes.size() % 4)));

  // Pack
  QByteArray data(3*codes.size()/4, '\x00');
  for (int i=0, o=0; i<codes.size(); i += 4, o += 3) {
    uint32_t encoded
        = ((uint32_t)(codes[i+0] & 0x3f) << 18) |
          ((uint32_t)(codes[i+1] & 0x3f) << 12) |
          ((uint32_t)(codes[i+2] & 0x3f) << 06) |
          ((uint32_t)(codes[i+3] & 0x3f) << 00);
    data[o+0] = (encoded>>16) & 0xff;
    data[o+1] = (encoded>> 8) & 0xff;
    data[o+2] = (encoded>> 0) & 0xff;
  }

  return data;
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
  writeASCII(Offset::magic(), "Id", 2);
  setUInt8(Offset::format(), (unsigned int)Format::Compressed);
  setEntrySize(16); // Default text length = 16
  writeASCII(Offset::version(),"001", 3);
  setUInt32_le(Offset::entryCount(), 0);
}

void
OpenGD77BaseCallsignDB::DatabaseHeaderElement::setEntrySize(unsigned int size) {
  setUInt8(Offset::entrySize(), 0x4a+size);
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
