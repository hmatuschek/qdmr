#include "userdatabase.hh"
#include "d868uv_callsigndb.hh"
#include "utils.hh"
#include "logger.hh"
#include <QtEndian>


/* ********************************************************************************************* *
 * Implementation of D868UVCallsignDB::EntryElement
 * ********************************************************************************************* */
D868UVCallsignDB::EntryElement::EntryElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

D868UVCallsignDB::EntryElement::EntryElement(uint8_t *ptr)
  : Element(ptr, Limit::totalLength())
{
  // pass...
}

void
D868UVCallsignDB::EntryElement::clear() {
  // Clear header and all strings.
  memset(_data, 0x00, 12);
}

void
D868UVCallsignDB::EntryElement::setCallType(DMRContact::Type type) {
  switch (type) {
  case DMRContact::PrivateCall: setUInt8(Offset::callType(), (unsigned int)CallType::Private); break;
  case DMRContact::GroupCall: setUInt8(Offset::callType(), (unsigned int)CallType::Group); break;
  case DMRContact::AllCall: setUInt8(Offset::callType(), (unsigned int)CallType::All); break;
  }
}

void
D868UVCallsignDB::EntryElement::setNumber(unsigned num) {
  setBCD8_be(Offset::number(), num);
}

void
D868UVCallsignDB::EntryElement::setFriendFlag(bool set) {
  setBit(Offset::friendFlag(), set);
}

void
D868UVCallsignDB::EntryElement::setRingTone(RingTone tone) {
  setUInt2(Offset::ringTone(), (unsigned)tone);
}

void
D868UVCallsignDB::EntryElement::setContent(
    const QString &name, const QString &city, const QString &call, const QString &state,
    const QString &country, const QString &comment)
{
  unsigned addr = Offset::name(), flen;
  flen = std::min(qsizetype(Limit::nameLength()), name.size()); writeASCII(addr, name, flen+1); addr += flen+1;
  flen = std::min(qsizetype(Limit::cityLength()), city.size()); writeASCII(addr, city, flen+1); addr += flen+1;
  flen = std::min(qsizetype(Limit::callLength()), call.size()); writeASCII(addr, call, flen+1); addr += flen+1;
  flen = std::min(qsizetype(Limit::stateLength()), state.size()); writeASCII(addr, state, flen+1); addr += flen+1;
  flen = std::min(qsizetype(Limit::countryLength()), country.size()); writeASCII(addr, country, flen+1); addr += flen+1;
  flen = std::min(qsizetype(Limit::commentLength()), comment.size()); writeASCII(addr, comment, flen+1);
}

unsigned
D868UVCallsignDB::EntryElement::fromUser(const UserDatabase::User &user) {
  clear();
  setCallType(DMRContact::PrivateCall);
  setNumber(user.id);
  setRingTone(RingTone::Off);
  setContent(user.name, user.city, user.call, user.state, user.country, "");
  return size(user);
}

unsigned
D868UVCallsignDB::EntryElement::size(const UserDatabase::User &user) {
  return Limit::headerLength() // header
         + std::min(qsizetype(Limit::nameLength()), user.name.size())+1       // name
         + std::min(qsizetype(Limit::cityLength()), user.city.size())+1       // city
         + std::min(qsizetype(Limit::callLength()), user.call.size())+1       // call
         + std::min(qsizetype(Limit::stateLength()), user.state.size())+1     // state
         + std::min(qsizetype(Limit::countryLength()), user.country.size())+1 // country
         + 1; // no comment but 0x00 terminator
}



/* ********************************************************************************************* *
 * Implementation of D868UVCallsignDB::LimitsElement
 * ********************************************************************************************* */
D868UVCallsignDB::LimitsElement::LimitsElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

D868UVCallsignDB::LimitsElement::LimitsElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}

void
D868UVCallsignDB::LimitsElement::clear() {
  memset(_data, 0x00, _size);
  setTotalSize(0);
}

unsigned
D868UVCallsignDB::LimitsElement::count() const {
  return getUInt32_le(Offset::count());
}
void
D868UVCallsignDB::LimitsElement::setCount(unsigned count) {
  setUInt32_le(Offset::count(), count);
}

unsigned
D868UVCallsignDB::LimitsElement::endOfDB() const {
  return getUInt32_le(Offset::endOfDB());
}
void
D868UVCallsignDB::LimitsElement::setEndOfDB(unsigned addr) {
  setUInt32_le(Offset::endOfDB(), addr);
}
void
D868UVCallsignDB::LimitsElement::setTotalSize(unsigned size) {
  setEndOfDB(D868UVCallsignDB::Offset::callsigns() + size);
}


/* ********************************************************************************************* *
 * Implementation of D868UVCallsignDB
 * ********************************************************************************************* */
D868UVCallsignDB::D868UVCallsignDB(QObject *parent)
  : CallsignDB(parent)
{
  // allocate and clear DB memory
  addImage("AnyTone AT-D878UV Callsign database.");
}

bool D868UVCallsignDB::encode(UserDatabase *db, const Flags &selection, const ErrorStack &err) {
  Q_UNUSED(err)

  // Determine size of call-sign DB in memory
  qint64 n = std::min(db->count(), qint64(Limit::entries()));
  // If DB size is limited by settings
  if (selection.hasCountLimit())
    n = std::min(n, (qint64)selection.countLimit());
  logDebug() << "Encode " << n << " entries.";

  // Select n users and sort them in ascending order of their IDs
  QVector<UserDatabase::User> users;
  users.reserve(n);
  for (unsigned i=0; i<n; i++)
    users.append(db->user(i));
  std::sort(users.begin(), users.end(),
            [](const UserDatabase::User &a, const UserDatabase::User &b) { return a.id < b.id; });

  // Compute total size of callsign db entries
  size_t dbSize = 0;
  size_t indexSize = n*IndexEntryElement::size();
  for (qint64 i=0; i<n; i++)
    dbSize += EntryElement::size(users[i]);

  // Allocate DB limits
  image(0).addElement(Offset::limits(), LimitsElement::size());
  // Store DB limits
  LimitsElement limits(data(Offset::limits()));
  limits.clear();
  limits.setCount(n);
  limits.setTotalSize(dbSize);

  // Allocate index banks
  for (int i=0; 0<indexSize; i++, indexSize-=std::min(indexSize, size_t(IndexBankElement::size()))) {
    size_t addr = Offset::index() + i*Offset::betweenIndexBanks();
    size_t size = align_size(std::min(indexSize, size_t(IndexBankElement::size())), 16);
    image(0).addElement(addr, size);
    memset(data(addr), 0xff, size);
  }

  // Allocate entry banks
  for (int i=0; 0<dbSize; i++, dbSize-=std::min(dbSize, size_t(EntryBankElement::size()))) {
    size_t addr = Offset::callsigns() + i*Offset::betweenCallsignBanks();
    size_t size = align_size(std::min(dbSize, size_t(EntryBankElement::size())), 16);
    image(0).addElement(addr, size);
    memset(data(addr), 0x00, size);
  }

  // Fill index, the offset of the entry is not the real memory offset,
  // but a virtual one without the gaps.
  uint32_t entry_offset = 0;
  uint32_t index_offset = 0;
  uint32_t index_bank   = 0;
  for (qint64 i=0; i<n; i++, index_offset+=IndexEntryElement::size()) {
    if (IndexBankElement::size() <= index_offset) {
      index_offset = 0; index_bank += 1;
    }
    IndexEntryElement index(data(Offset::index()+index_bank*Offset::betweenIndexBanks()+index_offset));
    index.setID(users[i].id, false);
    index.setIndex(entry_offset);
    entry_offset += EntryElement::size(users[i]);
  }

  // Then store DB entries
  uint32_t entry_bank = 0;
  entry_offset = 0;
  for (qint64 i=0; i<n; i++) {
    // Get size of current entry
    uint32_t entry_size = EntryElement::size(users[i]);
    // Check if entry fits into bank
    if (EntryBankElement::size() < (entry_offset+entry_size)) {
      // If not, split
      uint8_t buffer[100]; EntryElement(buffer).fromUser(users[i]);
      uint32_t n1 = (EntryBankElement::size()-entry_offset);
      uint32_t n2 = entry_size-n1;
      if (0 != n1) {
        // Copy first half
        uint8_t *ptr = data(Offset::callsigns()+entry_bank*Offset::betweenCallsignBanks()+entry_offset);
        memcpy(ptr, buffer, n1);
      }
      // advance bank counter
      entry_bank++; entry_offset = 0;
      // copy second half
      uint8_t *ptr = data(Offset::callsigns()+entry_bank*Offset::betweenCallsignBanks()+entry_offset);
      memcpy(ptr, buffer+n1, n2);
      // Update offset
      entry_offset += n2;
    } else {
      // when it fits, just add
      EntryElement(data(Offset::callsigns()+entry_bank*Offset::betweenCallsignBanks()+entry_offset))
          .fromUser(users[i]);
      entry_offset += entry_size;
    }
  }

  return true;
}
