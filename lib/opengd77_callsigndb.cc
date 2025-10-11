#include "opengd77_callsigndb.hh"
#include "utils.hh"
#include "userdatabase.hh"
#include <QtEndian>


/* ******************************************************************************************** *
 * Implementation of OpenGD77CallsignDB
 * ******************************************************************************************** */
OpenGD77CallsignDB::OpenGD77CallsignDB(QObject *parent)
  : OpenGD77BaseCallsignDB(parent)
{
  addImage("OpenGD77 call-sign database");
}


bool
OpenGD77CallsignDB::encode(UserDatabase *calldb, const Flags &selection, const ErrorStack &err) {
  Q_UNUSED(err)

  // Limit entry count
  auto n = std::min((unsigned int)calldb->count(), Limit::entries());
  if (selection.hasCountLimit())
    n = std::min(n, (unsigned int)selection.countLimit());
  // If there are no entries -> done.
  if (0 == n)
    return true;
  auto n0 = std::min(n, Limit::entries0()),
      n1 = std::min(n - n0, Limit::entries1());

  // Select first n entries and sort them in ascending order of their IDs
  QVector<UserDatabase::User> users;
  for (unsigned i=0; i<n; i++)
    users.append(calldb->user(i));
  std::sort(users.begin(), users.end(),
            [](const UserDatabase::User &a, const UserDatabase::User &b) { return a.id < b.id; });

  // Allocate segment0 for user db if requested
  unsigned size = align_size(DatabaseHeaderElement::size()+n0*DatabaseEntryElement::size(),
                             Limit::blockSize());
  this->image(0).addElement(Offset::header(), size);
  size = align_size(n1*DatabaseEntryElement::size(), Limit::blockSize());
  if (n1)
    this->image(0).addElement(Offset::entries1(), size);

  // Encode user DB
  DatabaseHeaderElement header(this->data(Offset::header()));
  header.clear(); header.setEntryCount(n);
  for (unsigned i=0; i<n0; i++) {
    DatabaseEntryElement(this->data(Offset::entries0() + i*DatabaseEntryElement::size()))
        .fromEntry(users[i]);
  }
  for (unsigned i=0; i<n1; i++) {
    DatabaseEntryElement(this->data(Offset::entries1() + i*DatabaseEntryElement::size()))
        .fromEntry(users[n0+i]);
  }

  return true;
}
