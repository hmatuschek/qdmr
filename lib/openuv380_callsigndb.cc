#include "openuv380_callsigndb.hh"
#include "utils.hh"
#include "userdatabase.hh"
#include "logger.hh"
#include <QtEndian>


/* ******************************************************************************************** *
 * Implementation of OpenUV380CallsignDB::DatabaseEntryElement
 * ******************************************************************************************** */
OpenUV380CallsignDB::DatabaseEntryElement::DatabaseEntryElement(uint8_t *ptr)
  : OpenGD77BaseCallsignDB::DatabaseEntryElement(ptr, size())
{
  // pass...
}

void
OpenUV380CallsignDB::DatabaseEntryElement::clear() {
  memset(_data, 0xff, size());
}

void
OpenUV380CallsignDB::DatabaseEntryElement::setText(const QString &text) {
  QByteArray data = pack(text);
  auto n = std::min(3*Limit::textLength()/4, (unsigned int)data.size());
  memcpy(_data+Offset::text(), data.constData(), n);
}


/* ******************************************************************************************** *
 * Implementation of OpenUV380CallsignDB
 * ******************************************************************************************** */
OpenUV380CallsignDB::OpenUV380CallsignDB(bool extended, QObject *parent)
  : OpenGD77BaseCallsignDB(parent)
{
  addImage("OpenUV380 call-sign database");
  if (extended)
    logDebug() << "Used extended call-sign DB memory.";
}


bool
OpenUV380CallsignDB::encode(UserDatabase *calldb, const Flags &selection, const ErrorStack &err) {
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
  if (n)
    logDebug() << "Store " << n << " entries starting from "
               << users.front().id << ":" << users.front().call << ", " << users.front().name << " in " << users.front().city
               << " to " << users.back().id << ":" << users.back().call << ", " << users.back().name << " in " << users.back().city;

  // Allocate segment0 for user db if requested
  unsigned size = align_size(DatabaseHeaderElement::size()+n0*DatabaseEntryElement::size(),
                             Limit::blockSize());
  this->image(0).addElement(Offset::header(), size);
  size = align_size(n1*DatabaseEntryElement::size(), Limit::blockSize());
  if (n1)
    this->image(0).addElement(Offset::entries1(), size);

  // Encode user DB
  DatabaseHeaderElement header(this->data(Offset::header()));
  header.clear(); header.setEntrySize(DatabaseEntryElement::size()); header.setEntryCount(n);
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
