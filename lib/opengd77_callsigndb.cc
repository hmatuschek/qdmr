#include "opengd77_callsigndb.hh"
#include "utils.hh"
#include "userdatabase.hh"
#include <QtEndian>

#define OFFSET_USERDB       0x30000
#define USERDB_SIZE         0x40000
#define USERDB_NUM_ENTRIES  (USERDB_SIZE-sizeof(userdb_t))/sizeof(userdb_entry_t)

#define BLOCK_SIZE  32



/* ******************************************************************************************** *
 * Implementation of OpenGD77CallsignDB::userdb_entry_t
 * ******************************************************************************************** */
OpenGD77CallsignDB::userdb_entry_t::userdb_entry_t() {
  clear();
}

void
OpenGD77CallsignDB::userdb_entry_t::clear() {
  memset(this, 0, sizeof(userdb_entry_t));
}

uint32_t
OpenGD77CallsignDB::userdb_entry_t::getNumber() const {
  return decode_dmr_id_bcd_le((uint8_t *)&number);
}
void
OpenGD77CallsignDB::userdb_entry_t::setNumber(uint32_t number) {
  encode_dmr_id_bcd_le((uint8_t *)&(this->number), number);
}

QString
OpenGD77CallsignDB::userdb_entry_t::getName() const {
  return decode_ascii((const uint8_t *)name, 15, 0x00);
}
void
OpenGD77CallsignDB::userdb_entry_t::setName(const QString &name) {
  encode_ascii((uint8_t *)(this->name), name, 15, 0x00);
}

void
OpenGD77CallsignDB::userdb_entry_t::fromEntry(const UserDatabase::User &user) {
  setNumber(user.id);
  QString tmp = user.call;
  if (!user.name.isEmpty())
    tmp = tmp + " " + user.name;
  setName(tmp);
}


/* ******************************************************************************************** *
 * Implementation of OpenGD77CallsignDB::userdb_t
 * ******************************************************************************************** */
OpenGD77CallsignDB::userdb_t::userdb_t() {
  clear();
}

void
OpenGD77CallsignDB::userdb_t::clear() {
  memset(this, 0, sizeof(userdb_t));
  memcpy(magic, "ID-", 3);
  size = 0x5d; // <- 19 byte entries, 15byte name
  memcpy(version, "001", 3);
  count = 0;
  unused6 = 0;
}

void
OpenGD77CallsignDB::userdb_t::setSize(unsigned n) {
  count = qToLittleEndian(std::min(n, unsigned(USERDB_NUM_ENTRIES)));
}


/* ******************************************************************************************** *
 * Implementation of OpenGD77CallsignDB
 * ******************************************************************************************** */
OpenGD77CallsignDB::OpenGD77CallsignDB(QObject *parent)
  : CallsignDB(parent)
{
  addImage("OpenGD77 call-sign database");
}

OpenGD77CallsignDB::~OpenGD77CallsignDB() {
  // pass...
}

bool
OpenGD77CallsignDB::encode(UserDatabase *calldb, const Selection &selection, const ErrorStack &err) {
  Q_UNUSED(err)

  // Limit entries to USERDB_NUM_ENTRIES
  qint64 n = std::min(calldb->count(), qint64(USERDB_NUM_ENTRIES));
  if (selection.hasCountLimit())
    n = std::min(n, (qint64)selection.countLimit());
  // If there are no entries -> done.
  if (0 == n)
    return true;

  // Select first n entries and sort them in ascending order of their IDs
  QVector<UserDatabase::User> users;
  for (unsigned i=0; i<n; i++)
    users.append(calldb->user(i));
  std::sort(users.begin(), users.end(),
            [](const UserDatabase::User &a, const UserDatabase::User &b) { return a.id < b.id; });

  // Allocate segment for user db if requested
  unsigned size = align_size(sizeof(userdb_t)+n*sizeof(userdb_entry_t), BLOCK_SIZE);
  this->image(0).addElement(OFFSET_USERDB, size);

  // Encode user DB
  userdb_t *userdb = (userdb_t *)this->data(OFFSET_USERDB);
  userdb->clear(); userdb->setSize(n);
  userdb_entry_t *db = (userdb_entry_t *)this->data(OFFSET_USERDB+sizeof(userdb_t));
  for (unsigned i=0; i<n; i++) {
    db[i].fromEntry(users[i]);
  }

  return true;
}
