#include "openuv380_callsigndb.hh"
#include "utils.hh"
#include "userdatabase.hh"
#include <QtEndian>


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

  // Limit entry count
  qint64 n = std::min(calldb->count(), qint64(Limit::entries()));
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
  unsigned size = align_size(DatabaseHeaderElement::size()+n*DatabaseEntryElement::size(),
                             Limit::blockSize());
  this->image(0).addElement(Offset::header(), size);

  // Encode user DB
  DatabaseHeaderElement header(this->data(Offset::header()));
  header.clear(); header.setEntryCount(n);
  for (unsigned i=0; i<n; i++) {
    DatabaseEntryElement(this->data(Offset::entries() + i*DatabaseEntryElement::size()))
        .fromEntry(users[i]);
  }

  return true;
}
