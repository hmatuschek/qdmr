#include "uv390_callsigndb.hh"

UV390CallsignDB::UV390CallsignDB(QObject *parent)
  : TyTCallsignDB(parent)
{
  image(0).setName("TYT MD-UV390 Callsign database.");
}

UV390CallsignDB::~UV390CallsignDB() {
  // pass...
}


