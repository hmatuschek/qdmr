#include "dm32uv_callsigndb.hh"



/* ********************************************************************************************* *
 * Implementation of call sign DB header
 * ********************************************************************************************* */
DM32UVCallsignDB::HeaderElement::HeaderElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}

void
DM32UVCallsignDB::HeaderElement::clear() {
  memset(_data, 0, size());
}

unsigned int
DM32UVCallsignDB::HeaderElement::count() const {
  return getUInt32_le(Offset::count());
}

void
DM32UVCallsignDB::HeaderElement::setCount(unsigned int count) {
  setUInt32_le(Offset::count(), count);
}




/* ********************************************************************************************* *
 * Implementation of call sign DB entry
 * ********************************************************************************************* */
DM32UVCallsignDB::EntryElement::EntryElement(uint8_t *ptr)
  : Codeplug::Element(ptr, size())
{
  // pass...
}

void
DM32UVCallsignDB::EntryElement::clear() {
  memset(_data, 0xff, size());
  setId(0);
}

bool
DM32UVCallsignDB::EntryElement::isValid() const {
  return 0 != id();
}


QString
DM32UVCallsignDB::EntryElement::name() const {
  return readASCII(Offset::name(), Limit::name(), 0x00);
}

void
DM32UVCallsignDB::EntryElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::name(), 0x00);
}


unsigned int
DM32UVCallsignDB::EntryElement::id() const {
  return getUInt32_le(Offset::id());
}

void
DM32UVCallsignDB::EntryElement::setId(unsigned int id) {
  setUInt32_le(Offset::id(), id);
}


QString
DM32UVCallsignDB::EntryElement::repeater() const {
  return readASCII(Offset::repeater(), Limit::repeater(), 0xff);
}

void
DM32UVCallsignDB::EntryElement::setRepeater(const QString &rep) {
  writeASCII(Offset::repeater(), rep, Limit::repeater(), 0xff);
}


QString
DM32UVCallsignDB::EntryElement::city() const {
  return readASCII(Offset::city(), Limit::city(), 0xff);
}

void
DM32UVCallsignDB::EntryElement::setCity(const QString &city) {
  writeASCII(Offset::city(), city, Limit::city(), 0xff);
}


QString
DM32UVCallsignDB::EntryElement::province() const {
  return readASCII(Offset::province(), Limit::province(), 0xff);
}

void
DM32UVCallsignDB::EntryElement::setProvince(const QString &province) {
  writeASCII(Offset::province(), province, Limit::province(), 0xff);
}


QString
DM32UVCallsignDB::EntryElement::country() const {
  return readASCII(Offset::country(), Limit::country(), 0xff);
}

void
DM32UVCallsignDB::EntryElement::setCountry(const QString &country) {
  writeASCII(Offset::country(), country, Limit::country(), 0xff);
}


QString
DM32UVCallsignDB::EntryElement::remark() const {
  return readASCII(Offset::remark(), Limit::remark(), 0xff);
}

void
DM32UVCallsignDB::EntryElement::setRemark(const QString &remark) {
  writeASCII(Offset::remark(), remark, Limit::remark(), 0xff);
}


bool
DM32UVCallsignDB::EntryElement::encode(const UserDatabase::User &user, const ErrorStack &err) {
  Q_UNUSED(err);

  setName(user.call + " " + user.name);
  setId(user.id);
  setCity(user.city);
  setCountry(user.country);
  setRemark(user.comment);

  return true;
}



/* ********************************************************************************************* *
 * Implementation of call sign DB
 * ********************************************************************************************* */
DM32UVCallsignDB::DM32UVCallsignDB(QObject *parent)
  : CallsignDB{parent}
{
  addImage("Call-sign DB for Baofeng DM-32UV");
}

bool
DM32UVCallsignDB::encode(UserDatabase *db, const Flags &selection, const ErrorStack &err) {
  // Limit number of entries.
  unsigned int count = std::min(Limit::entries(), (unsigned int)db->count());
  if (selection.hasCountLimit())
    count = std::min(count, (unsigned int)selection.countLimit());

  // Allocate first block
  if (! isAllocated(Offset::count()))
    image(0).addElement(Offset::count(), Offset::betweenBlocks());

  // Store count
  HeaderElement(data(Offset::count())).setCount(count);

  // Encode first count entries.
  for (unsigned int i=0; i<count; i++) {
    unsigned int block = i/Limit::entriesPerBlock();
    unsigned int withinBlock = i%Limit::entriesPerBlock();
    unsigned int blockAddr = (0 == block) ? Offset::block0()
                                          : Offset::block1() + (block-1)*Offset::betweenBlocks();
    if (! isAllocated(blockAddr))
      image(0).addElement(blockAddr, Offset::betweenBlocks());
    if (! EntryElement(data(blockAddr + withinBlock*EntryElement::size())).encode(db->user(i))) {
      errMsg(err) << "Cannot encode DB entry at index " << i << ".";
      return false;
    }
  }

  return true;
}
