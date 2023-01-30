#include "dm1701_callsigndb.hh"

DM1701CallsignDB::DM1701CallsignDB(QObject *parent)
  : TyTCallsignDB(parent)
{
  image(0).setName("BTECH DM-1701 Callsign database.");
}

DM1701CallsignDB::~DM1701CallsignDB() {
  // pass...
}


