#include "tyt_callsigndb.hh"
#include <QtEndian>

#include "utils.hh"


#define MAX_CALLSIGNS                122197  // Maximum number of callsings in DB

#define ADDR_CALLSIGN_INDEX      0x00200000  // Start of callsign database
#define NUM_INDEX_ENTRIES              4096
#define INDEX_ENTRY_SIZE         0x00000004  // Size of a index entry

#define ADDR_CALLSIGNS           0x00204003  // Start of callsign entries
#define CALLSIGN_ENTRY_SIZE      0x00000078  // Size of a call-sign entry


/* ********************************************************************************************* *
 * Implementation of TyTCallsignDB::IndexElement
 * ********************************************************************************************* */
TyTCallsignDB::IndexElement::IndexElement(uint8_t *ptr, size_t size)
  : Codeplug::Element(ptr, size)
{
  // pass...
}

TyTCallsignDB::IndexElement::IndexElement(uint8_t *ptr)
  : Codeplug::Element(ptr, 0x0003 + NUM_INDEX_ENTRIES*INDEX_ENTRY_SIZE)
{
  // pass...
}

TyTCallsignDB::IndexElement::~IndexElement() {
  // pass...
}

void
TyTCallsignDB::IndexElement::clear() {
  setNumEntries(0);
  for (int i=0; i<NUM_INDEX_ENTRIES; i++)
    Entry(_data+0x03 + i*INDEX_ENTRY_SIZE).clear();
}

void
TyTCallsignDB::IndexElement::setNumEntries(unsigned n) {
  uint8_t *ptr = (_data + 0x0000);
  ptr[0] = ((n>>16) & 0xff);
  ptr[1] = ((n>> 8) & 0xff);
  ptr[2] = ((n>> 0) & 0xff);
}

void
TyTCallsignDB::IndexElement::setIndexEntry(unsigned n, unsigned id, unsigned index) {
  Entry(_data+0x03 + n*INDEX_ENTRY_SIZE).set(id, index);
}


/* ********************************************************************************************* *
 * Implementation of TyTCallsignDB::IndexElement::Entry
 * ********************************************************************************************* */
TyTCallsignDB::IndexElement::Entry::Entry(uint8_t *ptr, size_t size)
  : Codeplug::Element(ptr, size)
{
  // pass...
}

TyTCallsignDB::IndexElement::Entry::Entry(uint8_t *ptr)
  : Codeplug::Element(ptr, INDEX_ENTRY_SIZE)
{
  // pass...
}

TyTCallsignDB::IndexElement::Entry::~Entry() {
  // pass...
}

void
TyTCallsignDB::IndexElement::Entry::clear() {
  memset(_data, 0xff, INDEX_ENTRY_SIZE);
}

void
TyTCallsignDB::IndexElement::Entry::set(unsigned id, unsigned index) {
  _data[0] = id>>16;
  _data[1] = ((id>>8)&0xf0) | ((index>>16) & 0xf);
  _data[2] = index>>8;
  _data[3] = index;
}


/* ********************************************************************************************* *
 * Implementation of TyTCallsignDB::EntryElement
 * ********************************************************************************************* */
TyTCallsignDB::EntryElement::EntryElement(uint8_t *ptr, size_t size)
  : Codeplug::Element(ptr, size)
{
  // pass...
}

TyTCallsignDB::EntryElement::EntryElement(uint8_t *ptr)
  : Codeplug::Element(ptr, CALLSIGN_ENTRY_SIZE)
{
  // pass...
}

TyTCallsignDB::EntryElement::~EntryElement() {
  // pass...
}

void
TyTCallsignDB::EntryElement::clear() {
  memset(_data, 0xff, CALLSIGN_ENTRY_SIZE);
}

void
TyTCallsignDB::EntryElement::set(const UserDatabase::User &user) {
  // Set id
  *((uint32_t *)(_data + 0x0000)) = qToLittleEndian(user.id);
  _data[3] = 0xff;

  // Set call
  encode_ascii(_data + 0x0004, user.call, 16);

  // Set name
  QString name = user.name;
  if (! user.surname.isEmpty())
    name += " " + user.surname;
  if (! user.country.isEmpty())
    name += ", " + user.country;
  encode_ascii(_data + 0x0014, name, 100);
}


/* ********************************************************************************************* *
 * Implementation of TyTCallsignDB
 * ********************************************************************************************* */
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

  // Clear DB index
  clearIndex();

  // Select n users and sort them in ascending order of their IDs
  QVector<UserDatabase::User> users;
  for (unsigned i=0; i<n; i++)
    users.append(db->user(i));
  std::sort(users.begin(), users.end(),
            [](const UserDatabase::User &a, const UserDatabase::User &b) { return a.id < b.id; });

  // Store number of entries
  setNumEntries(n);

  // First index entry
  int  j = 0;
  setIndexEntry(j++, users[0].id, 1);
  unsigned cidh = (users[0].id >> 12);

  // Store users and update index
  for (unsigned i=0; i<n; i++) {
    setEntry(i, users[i]);
    unsigned idh = (users[i].id >> 12);
    if (idh != cidh) {
      setIndexEntry(j++,users[i].id, i+1);
      cidh = idh;
    }
  }

  return true;
}

void
TyTCallsignDB::alloate(unsigned n) {
  n = std::min(n, unsigned(MAX_CALLSIGNS));
  qint64 size = align_size(0x0003 + INDEX_ENTRY_SIZE*NUM_INDEX_ENTRIES + CALLSIGN_ENTRY_SIZE*n, 1024);

  // allocate & clear memory
  if (0 == image(0).numElements())
    this->image(0).addElement(ADDR_CALLSIGN_INDEX, size);
  memset(data(ADDR_CALLSIGN_INDEX), 0xff, size);
}

void
TyTCallsignDB::clearIndex() {
  IndexElement(data(ADDR_CALLSIGN_INDEX)).clear();
}

void
TyTCallsignDB::setNumEntries(unsigned n) {
  IndexElement(data(ADDR_CALLSIGN_INDEX)).setNumEntries(n);
}

void
TyTCallsignDB::setIndexEntry(unsigned n, unsigned id, unsigned index) {
  IndexElement(data(ADDR_CALLSIGN_INDEX)).setIndexEntry(n, id, index);
}

void
TyTCallsignDB::setEntry(unsigned n, const UserDatabase::User &user) {
  // Get pointer to entry
  EntryElement(data(ADDR_CALLSIGNS + n*CALLSIGN_ENTRY_SIZE)).set(user);
}
