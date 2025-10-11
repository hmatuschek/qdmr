#include "callsigndb.hh"


/* ********************************************************************************************* *
 * Implementation of CallsignDB::Flags
 * ********************************************************************************************* */
CallsignDB::Flags::Flags(int64_t count)
  : TransferFlags(), _count(count)
{
  // pass...
}

CallsignDB::Flags::Flags(const Flags &other)
  : TransferFlags(other), _count(other._count)
{
  // pass...
}

bool
CallsignDB::Flags::hasCountLimit() const {
  return (0 <= _count);
}

size_t
CallsignDB::Flags::countLimit() const {
  if (0 > _count)
    return std::numeric_limits<size_t>::max();
  return _count;
}

void
CallsignDB::Flags::setCountLimit(size_t n) {
  _count = n;
}

void
CallsignDB::Flags::clearCountLimit() {
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
