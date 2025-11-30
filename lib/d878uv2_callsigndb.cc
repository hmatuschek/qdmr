#include "userdatabase.hh"
#include "d878uv2_callsigndb.hh"
#include "logger.hh"
#include "utils.hh"
#include <QtEndian>

/* ********************************************************************************************* *
 * Implementation of D878UVCallsignDB
 * ********************************************************************************************* */
D878UV2CallsignDB::D878UV2CallsignDB(QObject *parent)
  : D868UVCallsignDB(parent)
{
  // pass...
}

bool
D878UV2CallsignDB::encode(UserDatabase *db, const Flags &selection, const ErrorStack &err) {
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

  logDebug() << "Encode call-signs from " << users.first().id << ": " << users.first().call << ", "
             << users.first().name << " in " << users.first().city << " to " << users.last().id
             << ": " << users.last().call << ", " << users.last().name << " in " << users.last().city << ".";

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
