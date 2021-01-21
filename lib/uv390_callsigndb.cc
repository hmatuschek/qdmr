#include "uv390_callsigndb.hh"
#include "utils.hh"
#include <QtEndian>

#define MAX_CALLSIGNS    0x0001dd55  // Maximum number of callsings in DB
#define CALLSIGN_START   0x00200000  // Start of callsign database
#define CALLSIGN_OFFSET  0x00204003  // Start of callsign entries


/* ******************************************************************************************** *
 * Implementation of UV390Codeplug::callsign_db_t::entry_t
 * ******************************************************************************************** */
UV390CallsignDB::callsign_db_t::entry_t::entry_t() {
  clear();
}

void
UV390CallsignDB::callsign_db_t::entry_t::clear() {
  memset(val, 0xff, sizeof(val));
}

void
UV390CallsignDB::callsign_db_t::entry_t::set(uint32_t id, uint32_t ind) {
  val[0] = id>>16;
  val[1] = ((id>>8)&0xf0) | ((ind>>16) & 0xf);
  val[2] = ind>>8;
  val[3] = ind;
}

bool
UV390CallsignDB::callsign_db_t::entry_t::isValid() const {
  return (0xffffffff != *(uint32_t *)val);
}

/* ******************************************************************************************** *
 * Implementation of UV390Codeplug::callsign_db_t::callsign_t
 * ******************************************************************************************** */
UV390CallsignDB::callsign_db_t::callsign_t::callsign_t() {
  clear();
}

void
UV390CallsignDB::callsign_db_t::callsign_t::clear() {
  memset(this, 0xff, sizeof(callsign_t));
}

bool
UV390CallsignDB::callsign_db_t::callsign_t::isValid() const {
  return (0xffffff != dmrid);
}

void
UV390CallsignDB::callsign_db_t::callsign_t::setID(uint32_t dmrid) {
  this->dmrid = qToLittleEndian(dmrid);
}

void
UV390CallsignDB::callsign_db_t::callsign_t::setCall(const QString &call) {
  encode_ascii(callsign, call, 16, 0x00);
}

void
UV390CallsignDB::callsign_db_t::callsign_t::setName(const QString &name) {
  encode_ascii(this->name, name, 100, 0x00);
}

/* ******************************************************************************************** *
 * Implementation of UV390Codeplug::callsign_db_t::entry_t
 * ******************************************************************************************** */
void
UV390CallsignDB::callsign_db_t::callsign_t::fromUser(const UserDatabase::User &user) {
  clear();
  setID(user.id);
  setCall(user.call);
  QString name = user.name;
  if (! user.surname.isEmpty())
    name += " " + user.surname;
  if (! user.country.isEmpty())
    name += ", " + user.country;
  setName(name);
}

UV390CallsignDB::callsign_db_t::callsign_db_t() {
  clear();
}

void
UV390CallsignDB::callsign_db_t::clear() {
  memset(n, 0, sizeof(n));
  for (int i=0; i<4096; i++)
    index[i].clear();
}

void
UV390CallsignDB::callsign_db_t::setN(uint N) {
  n[0] = (N>>16)&0xff; n[1] = (N>>8)&0xff; n[2] = (N>>0)&0xff;
}

void
UV390CallsignDB::callsign_db_t::fromUserDB(const UserDatabase *db) {
  // Clear database and index
  clear();
  // Limit users to MAX_CALLSIGNS entries
  uint N = std::min(qint64(MAX_CALLSIGNS), db->count());
  setN(N);

  // If there are no entries -> done.
  if (0 == N)
    return;

  // Select n users and sort them in ascending order of their IDs
  QVector<UserDatabase::User> users;
  for (uint i=0; i<N; i++)
    users.append(db->user(i));
  std::sort(users.begin(), users.end(),
            [](const UserDatabase::User &a, const UserDatabase::User &b) { return a.id < b.id; });

  // First index entry
  int  j = 0;
  this->index[j++].set(users[0].id, 1);
  uint cidh = (users[0].id >> 12);

  // Store users and update index
  for (uint i=0; i<N; i++) {
    this->db[i].fromUser(users[i]);
    uint idh = (users[i].id >> 12);
    if (idh != cidh) {
      this->index[j++].set(users[i].id, i+1);
      cidh = idh;
    }
  }
}


/* ********************************************************************************************* *
 * Implementation of UV390CallsignDB
 * ********************************************************************************************* */
UV390CallsignDB::UV390CallsignDB(QObject *parent)
  : DFUFile(parent)
{
  // allocate and clear DB memory
  addImage("TYT MD-UV390 Callsign database.");
}

void
UV390CallsignDB::encode(UserDatabase *db) {
  // Determine size of call-sign DB in memory
  qint64 n = std::min(db->count(), qint64(MAX_CALLSIGNS));
  qint64 size = align_size(0x4003 + 120*n, 1024);

  // allocate & clear memory
  if (0 == image(0).numElements())
    this->image(0).addElement(CALLSIGN_START, size);
  memset(data(CALLSIGN_START), 0xff, size);

  // Encode call-sign DB
  ((callsign_db_t *)data(CALLSIGN_START))->fromUserDB(db);
}
