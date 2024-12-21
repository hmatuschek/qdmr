#include "opengd77base_callsigndb.hh"
#include "utils.hh"
#include "userdatabase.hh"
#include <QtEndian>


/* ******************************************************************************************** *
 * Implementation of OpenGD77BaseCallsignDB::userdb_entry_t
 * ******************************************************************************************** */
OpenGD77BaseCallsignDB::userdb_entry_t::userdb_entry_t() {
  clear();
}

void
OpenGD77BaseCallsignDB::userdb_entry_t::clear() {
  memset(this, 0, sizeof(userdb_entry_t));
}

uint32_t
OpenGD77BaseCallsignDB::userdb_entry_t::getNumber() const {
  return decode_dmr_id_bcd_le((uint8_t *)&number);
}
void
OpenGD77BaseCallsignDB::userdb_entry_t::setNumber(uint32_t number) {
  encode_dmr_id_bcd_le((uint8_t *)&(this->number), number);
}

QString
OpenGD77BaseCallsignDB::userdb_entry_t::getName() const {
  return decode_ascii((const uint8_t *)name, 15, 0x00);
}
void
OpenGD77BaseCallsignDB::userdb_entry_t::setName(const QString &name) {
  encode_ascii((uint8_t *)(this->name), name, 15, 0x00);
}

void
OpenGD77BaseCallsignDB::userdb_entry_t::fromEntry(const UserDatabase::User &user) {
  setNumber(user.id);
  QString tmp = user.call;
  if (!user.name.isEmpty())
    tmp = tmp + " " + user.name;
  setName(tmp);
}


/* ******************************************************************************************** *
 * Implementation of OpenGD77BaseCallsignDB::userdb_t
 * ******************************************************************************************** */
OpenGD77BaseCallsignDB::userdb_t::userdb_t() {
  clear();
}

void
OpenGD77BaseCallsignDB::userdb_t::clear() {
  memset(this, 0, sizeof(userdb_t));
  memcpy(magic, "ID-", 3);
  size = 0x5d; // <- 19 byte entries, 15byte name
  memcpy(version, "001", 3);
  count = 0;
  unused6 = 0;
}

void
OpenGD77BaseCallsignDB::userdb_t::setSize(unsigned n) {
  count = n;
}


/* ******************************************************************************************** *
 * Implementation of OpenGD77BaseCallsignDB
 * ******************************************************************************************** */
OpenGD77BaseCallsignDB::OpenGD77BaseCallsignDB(QObject *parent)
  : CallsignDB(parent)
{
  // pass...
}

OpenGD77BaseCallsignDB::~OpenGD77BaseCallsignDB() {
  // pass...
}
