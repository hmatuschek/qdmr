#include "uv390_callsigndb.hh"
#include "utils.hh"

#define CALLSIGN_START   0x00200000  // Start of callsign database
#define CALLSIGN_END     0x01000000  // End of callsign database
#define CALLSIGN_OFFSET  0x00204003  // Start of callsign entries


/* ******************************************************************************************** *
 * Implementation of UV390Codeplug::callsign_db_t
 * ******************************************************************************************** */
UV390CallsignDB::callsign_db_t::entry_t::entry_t() {
  clear();
}

void
UV390CallsignDB::callsign_db_t::entry_t::clear() {
  id_high = 0xfff;
  index   = 0xfffff;
}

bool
UV390CallsignDB::callsign_db_t::entry_t::isValid() const {
  return (0xfff != id_high) && (0xfffff != index);
}

UV390CallsignDB::callsign_db_t::callsign_t::callsign_t() {
  clear();
}

void
UV390CallsignDB::callsign_db_t::callsign_t::clear() {
  memset(dmrid, 0xff, sizeof(dmrid));
  _unused = 0xff;
  memset(callsign, 0xff, sizeof(callsign));
  memset(name, 0xff, sizeof(name));
}

bool
UV390CallsignDB::callsign_db_t::callsign_t::isValid() const {
  return (0xff != dmrid[0]) && (0xff != dmrid[1]) && (0xff != dmrid[2]);
}

void
UV390CallsignDB::callsign_db_t::callsign_t::setID(uint32_t dmrid) {
  encode_dmr_id_bcd(this->dmrid, dmrid);
}

void
UV390CallsignDB::callsign_db_t::callsign_t::setCall(const QString &call) {
  memset(callsign, 0xff, sizeof(callsign));
  strncpy(callsign, call.toStdString().c_str(), sizeof(callsign));
}

void
UV390CallsignDB::callsign_db_t::callsign_t::setName(const QString &name) {
  memset(this->name, 0xff, sizeof(this->name));
  strncpy(this->name, name.toStdString().c_str(), sizeof(this->name));
}

void
UV390CallsignDB::callsign_db_t::callsign_t::fromUser(const UserDatabase::User &user) {
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
  n = 0;
  for (int i=0; i<4096; i++)
    index[i].clear();
  for (int i=0; i<122197; i++)
    db[i].clear();
}

void
UV390CallsignDB::callsign_db_t::fromUserDB(const UserDatabase *db) {
  // Clear database and index
  clear();
  // Limit users to 122197 entries
  n = std::min(122197ll, db->count());
  // If there are no users -> done.
  if (0 == n)
    return;

  // Select n users and sort them in ascending order of their IDs
  QVector<UserDatabase::User> users;
  for (int i=0; i<n; i++)
    users.append(db->user(i));
  std::sort(users.begin(), users.end(),
            [](const UserDatabase::User &a, const UserDatabase::User &b) { return a.id < b.id; });

  // First index entry
  int  j = 0;
  uint cidh = (users[0].id >> 12);
  this->index[j].id_high = cidh;
  this->index[j].index = 1;
  j++;

  // Store users and update index
  for (int i=0; i<n; i++) {
    this->db[i].fromUser(users[i]);
    uint idh = (users[i].id >> 12);
    if (idh != cidh) {
      this->index[j].id_high = idh;
      this->index[j].index = i+1;
      cidh = idh; j++;
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
  this->image(0).addElement(CALLSIGN_START, CALLSIGN_END-CALLSIGN_START);
}

void
UV390CallsignDB::encode(UserDatabase *db) {
  memset(data(CALLSIGN_START), 0xff, CALLSIGN_END-CALLSIGN_START);
  ((callsign_db_t *)data(CALLSIGN_START))->fromUserDB(db);
}
