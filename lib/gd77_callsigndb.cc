#include "gd77_callsigndb.hh"
#include "utils.hh"
#include "userdatabase.hh"
#include "logger.hh"
#include <QtEndian>

#define OFFSET_USERDB       0x00000

#define USERDB_MAX_ENTRIES  10920
#define BLOCK_SIZE          32



/* ******************************************************************************************** *
 * Implementation of GD77CallsignDB::userdb_entry_t
 * ******************************************************************************************** */
GD77CallsignDB::userdb_entry_t::userdb_entry_t() {
  clear();
}

void
GD77CallsignDB::userdb_entry_t::clear() {
  memset(this, 0, sizeof(userdb_entry_t));
}

uint32_t
GD77CallsignDB::userdb_entry_t::getNumber() const {
  return decode_dmr_id_bcd_le((uint8_t *)&number);
}
void
GD77CallsignDB::userdb_entry_t::setNumber(uint32_t number) {
  encode_dmr_id_bcd_le((uint8_t *)&(this->number), number);
}

QString
GD77CallsignDB::userdb_entry_t::getName() const {
  return decode_ascii((const uint8_t *)name, 7, 0x00);
}
void
GD77CallsignDB::userdb_entry_t::setName(const QString &name) {
  encode_ascii((uint8_t *)(this->name), name, 7, 0x00);
}

void
GD77CallsignDB::userdb_entry_t::fromEntry(const UserDatabase::User &user) {
  clear();
  setNumber(user.id);
  setName(user.call);
}


/* ******************************************************************************************** *
 * Implementation of GD77CallsignDB::userdb_t
 * ******************************************************************************************** */
GD77CallsignDB::userdb_t::userdb_t() {
  clear();
}

void
GD77CallsignDB::userdb_t::clear() {
  memcpy(magic, "ID-V001\0", 8);
  count = 0;
}

void
GD77CallsignDB::userdb_t::setSize(uint n) {
  count = qToLittleEndian(std::min(n, uint(USERDB_MAX_ENTRIES)));
}


/* ******************************************************************************************** *
 * Implementation of OpenGD77CallsignDB
 * ******************************************************************************************** */
GD77CallsignDB::GD77CallsignDB(QObject *parent)
  : CallsignDB(parent)
{
  addImage("GD77 call-sign database");
}

GD77CallsignDB::~GD77CallsignDB() {
  // pass...
}

bool
GD77CallsignDB::encode(UserDatabase *calldb, const Selection &selection) {
  // Limit entries to USERDB_NUM_ENTRIES
  qint64 n = std::min(calldb->count(), qint64(USERDB_MAX_ENTRIES));
  if (selection.hasCountLimit())
    n = std::min(n, (qint64)selection.countLimit());
  // If there are no entries -> done.
  if (0 == n)
    return true;

  // Select first n entries and sort them in ascending order of their IDs
  logDebug() << "Select first " << n << " entries out off " << calldb->count() << ".";
  QVector<UserDatabase::User> users;
  for (uint i=0; i<n; i++)
    users.append(calldb->user(i));
  logDebug() << "Sort selected w.r.t their ID.";
  std::sort(users.begin(), users.end(),
            [](const UserDatabase::User &a, const UserDatabase::User &b) { return a.id < b.id; });

  // Allocate segment for user db if requested
  uint size = align_size(sizeof(userdb_t)+n*sizeof(userdb_entry_t), BLOCK_SIZE);
  logDebug() << "Allocate " << size << "bytes for call-sign DB.";
  this->image(0).addElement(OFFSET_USERDB, size);

  // Encode user DB
  userdb_t *userdb = (userdb_t *)this->data(OFFSET_USERDB);
  userdb->clear(); userdb->setSize(n);
  userdb_entry_t *db = (userdb_entry_t *)this->data(OFFSET_USERDB+sizeof(userdb_t));
  for (uint i=0; i<n; i++) {
    db[i].fromEntry(users[i]);
  }

  return true;
}
