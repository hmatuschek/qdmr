#include "d868uv_callsigndb.hh"
#include "utils.hh"
#include <QtEndian>

#define MAX_CALLSIGNS               0x00030d40  // Maximum number of callsings in DB (200k)

#define CALLSIGN_INDEX_BANK0        0x04000000  // Start address of the callsign index table
#define CALLSIGN_INDEX_BANK_OFFSET  0x00040000  // Offset between index banks
#define CALLSIGN_INDEX_BANK_SIZE    0x0001f400  // Size of each callsign index bank

#define CALLSIGN_BANK0              0x04500000  // Start address of the actual call sign entries
#define CALLSIGN_BANK_OFFSET        0x00040000  // Offset between callsign entry bank
#define CALLSIGN_BANK_SIZE          0x000186a0  // Size of each callsign entry bank

#define CALLSIGN_LIMITS             0x044C0000  // Start address of callsign db limits


/* ********************************************************************************************* *
 * Implementation of D868UVCallsignDB::EntryElement
 * ********************************************************************************************* */
D868UVCallsignDB::EntryElement::EntryElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

D868UVCallsignDB::EntryElement::EntryElement(uint8_t *ptr)
  : Element(ptr, 0x0064)
{
  // pass...
}

void
D868UVCallsignDB::EntryElement::setCallType(DigitalContact::Type type) {
  switch (type) {
  case DigitalContact::PrivateCall: setUInt8(0x0000, 0); break;
  case DigitalContact::GroupCall: setUInt8(0x0000, 1); break;
  case DigitalContact::AllCall: setUInt8(0x0000, 2); break;
  }
}

void
D868UVCallsignDB::EntryElement::setNumber(unsigned num) {
  setBCD8_be(0x0001, num);
}

void
D868UVCallsignDB::EntryElement::setRingTone(RingTone tone) {
  setUInt8(0x0005, (unsigned)tone);
}

void
D868UVCallsignDB::EntryElement::setContent(
    const QString &name, const QString &city, const QString &call, const QString &state,
    const QString &country, const QString &comment)
{
  unsigned addr = 0x0006;
  writeASCII(addr, name, 16, 0x00); addr += std::min(16, name.size()); setUInt8(addr, 0); addr++;
  writeASCII(addr, city, 16, 0x00); addr += std::min(16, city.size()); setUInt8(addr, 0); addr++;
  writeASCII(addr, call, 8, 0x00); addr += std::min(8, call.size()); setUInt8(addr, 0); addr++;
  writeASCII(addr, state, 16, 0x00); addr += std::min(16, state.size()); setUInt8(addr, 0); addr++;
  writeASCII(addr, country, 16, 0x00); addr += std::min(16, country.size()); setUInt8(addr, 0); addr++;
  writeASCII(addr, comment, 16, 0x00); addr += std::min(16, comment.size()); setUInt8(addr, 0); addr++;
}

unsigned
D868UVCallsignDB::EntryElement::fromUser(const UserDatabase::User &user) {
  setCallType(DigitalContact::PrivateCall);
  setNumber(user.id);
  setRingTone(RingTone::Off);
  setContent(user.name, user.city, user.call, user.state, user.country, "");
  return size(user);
}

unsigned
D868UVCallsignDB::EntryElement::size(const UserDatabase::User &user) {
  return 6 // header
      + std::min(16, user.name.size())+1 // name
      + std::min(16, user.city.size())+1 // city
      + std::min( 8, user.call.size())+1 // call
      + std::min(16, user.state.size())+1 // state
      + std::min(16, user.country.size())+1 // country
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
  : Element(ptr, 0x0010)
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
  return getUInt32_le(0x0000);
}
void
D868UVCallsignDB::LimitsElement::setCount(unsigned count) {
  setUInt32_le(0x0000, count);
}

unsigned
D868UVCallsignDB::LimitsElement::endOfDB() const {
  return getUInt32_le(0x0004);
}
void
D868UVCallsignDB::LimitsElement::setEndOfDB(unsigned addr) {
  setUInt32_le(0x0004, addr);
}
void
D868UVCallsignDB::LimitsElement::setTotalSize(unsigned size) {
  setEndOfDB(CALLSIGN_BANK0 + size);
}

unsigned
D868UVCallsignDB::LimitsElement::size() {
  return 0x0010;
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

bool D868UVCallsignDB::encode(UserDatabase *db, const Selection &selection, const ErrorStack &err) {
  Q_UNUSED(err)

  // Determine size of call-sign DB in memory
  qint64 n = std::min(db->count(), qint64(MAX_CALLSIGNS));
  // If DB size is limited by settings
  if (selection.hasCountLimit())
    n = std::min(n, (qint64)selection.countLimit());

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
  image(0).addElement(CALLSIGN_LIMITS, LimitsElement::size());
  memset(data(CALLSIGN_LIMITS), 0x00, LimitsElement::size());
  // Store DB limits
  LimitsElement limits(data(CALLSIGN_LIMITS));
  limits.setCount(n);
  limits.setTotalSize(dbSize);

  // Allocate index banks
  for (int i=0; 0<indexSize; i++, indexSize-=std::min(indexSize, size_t(CALLSIGN_INDEX_BANK_SIZE))) {
    size_t addr = CALLSIGN_INDEX_BANK0 + i*CALLSIGN_INDEX_BANK_OFFSET;
    size_t size = align_size(std::min(indexSize, size_t(CALLSIGN_INDEX_BANK_SIZE)), 16);
    image(0).addElement(addr, size);
    memset(data(addr), 0xff, size);
  }

  // Allocate entry banks
  for (int i=0; 0<dbSize; i++, dbSize-=std::min(dbSize, size_t(CALLSIGN_BANK_SIZE))) {
    size_t addr = CALLSIGN_BANK0 + i*CALLSIGN_BANK_OFFSET;
    size_t size = align_size(std::min(dbSize, size_t(CALLSIGN_BANK_SIZE)), 16);
    image(0).addElement(addr, size);
    memset(data(addr), 0x00, size);
  }

  // Fill index, the offset of the entry is not the real memory offset,
  // but a virtual one without the gaps.
  uint32_t entry_offset = 0;
  uint32_t index_offset = 0;
  uint32_t index_bank   = 0;
  for (qint64 i=0; i<n; i++, index_offset+=IndexEntryElement::size()) {
    if (CALLSIGN_INDEX_BANK_SIZE <= index_offset) {
      index_offset = 0; index_bank += 1;
    }
    IndexEntryElement index(data(CALLSIGN_INDEX_BANK0+index_bank*CALLSIGN_BANK_OFFSET+index_offset));
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
    if (CALLSIGN_BANK_SIZE < (entry_offset+entry_size)) {
      // If not, split
      uint8_t buffer[100]; EntryElement(buffer).fromUser(users[i]);
      uint32_t n1 = (CALLSIGN_BANK_SIZE-entry_offset);
      uint32_t n2 = entry_size-n1;
      // Copy first half
      uint8_t *ptr = data(CALLSIGN_BANK0+entry_bank*CALLSIGN_BANK_OFFSET+entry_offset);
      memcpy(ptr, buffer, n1);
      // advance bank counter
      entry_bank++; entry_offset = 0;
      // copy second half
      ptr = data(CALLSIGN_BANK0+entry_bank*CALLSIGN_BANK_OFFSET+entry_offset);
      memcpy(ptr, buffer+n1, n2);
      // Update offset
      entry_offset += n2;
    } else {
      // when it fits, just add
      EntryElement(data(CALLSIGN_BANK0+entry_bank*CALLSIGN_BANK_OFFSET+entry_offset))
          .fromUser(users[i]);
      entry_offset += entry_size;
    }
  }

  return true;
}
