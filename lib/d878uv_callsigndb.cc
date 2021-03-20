#include "d878uv_callsigndb.hh"
#include "utils.hh"
#include <QtEndian>

#define MAX_CALLSIGNS         0x00030d40  // Maximum number of callsings in DB (200k)
#define ADDR_CALLSIGN_INDEX   0x04000000  // Start address of the callsign index table
#define ADDR_CALLSIGN         0x04500000  // Start address of the actuall call sign entries
#define ADDR_UNKNOWN          0x044C0000  // Start address of some unkonwn area.


/* ******************************************************************************************** *
 * Implementation of D878UVCallsignDB::enty_head_t
 * ******************************************************************************************** */
size_t
D878UVCallsignDB::entry_t::fromUser(const UserDatabase::User &user) {
  call_type = PRIVATE_CALL;
  ring = RING_NONE;
  encode_dmr_id_bcd((uint8_t *)&id, user.id);
  uint8_t size = 6;
  char *strings = (char *)body;
  // append name
  uint n = std::min(16, user.name.size());
  strncpy(strings, user.name.toLocal8Bit().data(), n);
  size += (n+1); strings += (n+1);
  // no city
  (*strings) = 0x00; size ++; strings ++;
  // append call
  n = std::min(8, user.call.size());
  strncpy(strings, user.call.toLocal8Bit().data(), n);
  size += (n+1); strings += (n+1);
  // no state
  (*strings) = 0x00; size ++; strings ++;
  // no country
  (*strings) = 0x00; size ++; strings ++;
  // no comment
  (*strings) = 0x00; size ++; strings ++;
  return size;
}

size_t
D878UVCallsignDB::entry_t::getSize(const UserDatabase::User &user) {
  return 6 + // header
      std::min(16, user.name.size())+1 // name
      + 1 // city
      + std::min(8, user.call.size())+1 // call
      + 1 // state
      + 1 // country
      + 1; // comment
}


/* ******************************************************************************************** *
 * Implementation of D878UVCallsignDB::limits_t
 * ******************************************************************************************** */
void
D878UVCallsignDB::limits_t::clear() {
  count = end_of_db = _unused8 = _unused12 = 0;
}

void
D878UVCallsignDB::limits_t::setCount(uint32_t n) {
  count = qToLittleEndian(n);
}

void
D878UVCallsignDB::limits_t::setTotalSize(uint32_t size) {
  end_of_db = qToLittleEndian(ADDR_CALLSIGN + size);
}


/* ********************************************************************************************* *
 * Implementation of D878UVCallsignDB
 * ********************************************************************************************* */
D878UVCallsignDB::D878UVCallsignDB(QObject *parent)
  : CallsignDB(parent)
{
  // allocate and clear DB memory
  addImage("AnyTone AT-D878UV Callsign database.");
}

bool D878UVCallsignDB::encode(UserDatabase *db, const Selection &selection) {
  // Determine size of call-sign DB in memory
  qint64 n = std::min(db->count(), qint64(MAX_CALLSIGNS));
  if (selection.hasCountLimit())
    n = std::min(n, (qint64)selection.countLimit());

  // Select n users and sort them in ascending order of their IDs
  QVector<UserDatabase::User> users;
  for (uint i=0; i<n; i++)
    users.append(db->user(i));
  std::sort(users.begin(), users.end(),
            [](const UserDatabase::User &a, const UserDatabase::User &b) { return a.id < b.id; });

  // Compute total size of callsign db entries
  size_t dbSize = 0;
  size_t indexSize = n*sizeof(index_entry_t);
  for (qint64 i=0; i<n; i++)
    dbSize += entry_t::getSize(users[i]);

  // Allocate and clear space for everything
  image(0).addElement(ADDR_CALLSIGN_INDEX, align_size(indexSize, 16));
  memset(data(ADDR_CALLSIGN_INDEX), 0xff, align_size(indexSize, 16));
  image(0).addElement(ADDR_UNKNOWN, sizeof(limits_t));
  memset(data(ADDR_UNKNOWN), 0x00, sizeof(limits_t));
  image(0).addElement(ADDR_CALLSIGN, align_size(dbSize, 16));
  memset(data(ADDR_CALLSIGN), 0x00, align_size(dbSize, 16));

  // Store DB limits
  limits_t *limits = (limits_t *)data(ADDR_UNKNOWN);
  limits->setCount(n);
  limits->setTotalSize(dbSize);

  // Then fill index and DB
  uint32_t entry_addr = ADDR_CALLSIGN;
  uint8_t *entry_ptr = data(ADDR_CALLSIGN);
  index_entry_t *index_ptr = (index_entry_t *)data(ADDR_CALLSIGN_INDEX);
  for (qint64 i=0; i<n; i++) {
    index_ptr->setID(entry_addr, false);
    index_ptr->setIndex(entry_addr - ADDR_CALLSIGN);
    size_t off = ((entry_t *)entry_ptr)->fromUser(users[i]);
    entry_addr += off; entry_ptr += off; index_ptr += 1;
  }

  return true;
}
