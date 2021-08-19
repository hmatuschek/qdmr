#include "dm2017_callsigndb.hh"

MD2017CallsignDB::MD2017CallsignDB(QObject *parent)
  : TyTCallsignDB(parent)
{
  // allocate and clear DB memory
  addImage("TYT MD-2017 Callsign database.");
}

MD2017CallsignDB::~MD2017CallsignDB() {
  // pass...
}

bool
MD2017CallsignDB::encode(UserDatabase *db, const Selection &selection) {
  return false;
}


