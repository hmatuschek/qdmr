#include "tyt_callsigndb.hh"
#include <QtEndian>

#include "utils.hh"


#define MAX_CALLSIGNS                122197  // Maximum number of callsings in DB

#define ADDR_CALLSIGN_INDEX      0x00200000  // Start of callsign database
#define NUM_INDEX_ENTRIES              4096
#define INDEX_ENTRY_SIZE         0x00000004  // Size of a index entry

#define ADDR_CALLSIGNS           0x00204003  // Start of callsign entries
#define CALLSIGN_ENTRY_SIZE      0x00000078  // Size of a call-sign entry



TyTCallsignDB::TyTCallsignDB(QObject *parent)
  : CallsignDB(parent)
{
  // allocate and clear DB memory
  addImage("TYT Callsign database.");
}

TyTCallsignDB::~TyTCallsignDB() {
  // pass...
}

bool
TyTCallsignDB::encode(UserDatabase *db, const Selection &selection) {
  // Allocate space for callsign db
  size_t n = db->count();
  if (selection.hasCountLimit())
    n = std::min(n, selection.countLimit());
  alloate(n);
  clearIndex();

  // Select n users and sort them in ascending order of their IDs
  QVector<UserDatabase::User> users;
  for (uint i=0; i<n; i++)
    users.append(db->user(i));
  std::sort(users.begin(), users.end(),
            [](const UserDatabase::User &a, const UserDatabase::User &b) { return a.id < b.id; });

  // First index entry
  int  j = 0;
  setIndexEntry(j++, users[0].id, 1);
  uint cidh = (users[0].id >> 12);

  // Store users and update index
  for (uint i=0; i<n; i++) {
    setEntry(i, users[i]);
    uint idh = (users[i].id >> 12);
    if (idh != cidh) {
      setIndexEntry(j++,users[i].id, i+1);
      cidh = idh;
    }
  }

  return true;
}

void
TyTCallsignDB::alloate(uint n) {
  n = std::min(n, uint(MAX_CALLSIGNS));
  qint64 size = align_size(0x0003 + INDEX_ENTRY_SIZE*NUM_INDEX_ENTRIES + CALLSIGN_ENTRY_SIZE*n, 1024);

  // allocate & clear memory
  if (0 == image(0).numElements())
    this->image(0).addElement(ADDR_CALLSIGN_INDEX, size);
  memset(data(ADDR_CALLSIGN_INDEX), 0xff, size);
}

void
TyTCallsignDB::clearIndex() {
  setNumEntries(0);
  for (int i=0; i<NUM_INDEX_ENTRIES; i++)
    clearIndexEntry(i);
}

void
TyTCallsignDB::setNumEntries(uint n) {
  uint8_t *ptr = data(ADDR_CALLSIGN_INDEX);
  ptr[0] = ((n>>16) & 0xff);
  ptr[1] = ((n>> 8) & 0xff);
  ptr[3] = ((n>> 0) & 0xff);
}

void
TyTCallsignDB::clearIndexEntry(uint n) {
  uint8_t *ptr = data(ADDR_CALLSIGN_INDEX + 0x0003 + n*INDEX_ENTRY_SIZE);
  memset(ptr, 0xff, INDEX_ENTRY_SIZE);
}

void
TyTCallsignDB::setIndexEntry(uint n, uint id, uint index) {
  uint8_t *ptr = data(ADDR_CALLSIGN_INDEX + 0x0003 + n*INDEX_ENTRY_SIZE);
  ptr[0] = id>>16;
  ptr[1] = ((id>>8)&0xf0) | ((index>>16) & 0xf);
  ptr[2] = index>>8;
  ptr[3] = index;
}

void
TyTCallsignDB::setEntry(uint n, const UserDatabase::User &user) {
  // Set id
  uint8_t *ptr = data(ADDR_CALLSIGNS + n*CALLSIGN_ENTRY_SIZE);
  *((uint32_t *)(ptr + 0x0000)) = qToLittleEndian(user.id);
  // Set call
  encode_ascii(ptr + 0x0004, user.call, 16);
  // Set name
  QString name = user.name;
  if (! user.surname.isEmpty())
    name += " " + user.surname;
  if (! user.country.isEmpty())
    name += ", " + user.country;
  encode_ascii(ptr + 0x0004, name, 16);
}
