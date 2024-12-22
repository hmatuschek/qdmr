#include "openuv380_callsigndb.hh"
#include "utils.hh"
#include "userdatabase.hh"
#include <QtEndian>

#define USERDB_SIZE         0x40000
#define USERDB_NUM_ENTRIES  (USERDB_SIZE-sizeof(userdb_t))/sizeof(userdb_entry_t)


/* ******************************************************************************************** *
 * Implementation of OpenUV380CallsignDB
 * ******************************************************************************************** */
OpenUV380CallsignDB::OpenUV380CallsignDB(QObject *parent)
  : OpenGD77BaseCallsignDB(parent)
{
  addImage("OpenUV380 call-sign database");
}


bool
OpenUV380CallsignDB::encode(UserDatabase *calldb, const Selection &selection, const ErrorStack &err) {
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
  unsigned size = align_size(sizeof(userdb_t)+n*sizeof(userdb_entry_t), Limit::blockSize());
  this->image(0).addElement(Offset::callsignDB(), size);

  // Encode user DB
  userdb_t *userdb = (userdb_t *)this->data(Offset::callsignDB());
  userdb->clear(); userdb->setSize(n);
  userdb_entry_t *db = (userdb_entry_t *)this->data(Offset::callsignDB()+sizeof(userdb_t));
  for (unsigned i=0; i<n; i++) {
    db[i].fromEntry(users[i]);
  }

  return true;
}
