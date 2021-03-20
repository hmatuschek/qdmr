#include "callsigndb.hh"


/* ********************************************************************************************* *
 * Implementation of CallsignDB::Selection
 * ********************************************************************************************* */
CallsignDB::Selection::Selection(int64_t count)
  : _count(count)
{
  // pass...
}

CallsignDB::Selection::Selection(const Selection &other)
  : _count(other._count)
{
  // pass...
}

bool
CallsignDB::Selection::hasCountLimit() const {
  return (0 <= _count);
}

size_t
CallsignDB::Selection::countLimit() const {
  if (0 > _count)
    return std::numeric_limits<size_t>::max();
  return _count;
}

void
CallsignDB::Selection::setCountLimit(size_t n) {
  _count = n;
}

void
CallsignDB::Selection::clearCountLimit() {
  _count = -1;
}


/* ********************************************************************************************* *
 * Implementation of CallsignDB
 * ********************************************************************************************* */
CallsignDB::CallsignDB(QObject *parent)
  : DFUFile(parent)
{
  // pass...
}

CallsignDB::~CallsignDB() {
  // pass...
}
