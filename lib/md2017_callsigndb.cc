#include "md2017_callsigndb.hh"

MD2017CallsignDB::MD2017CallsignDB(QObject *parent)
  : TyTCallsignDB(parent)
{
  image(0).setName("TYT MD-2017 Callsign database.");
}

MD2017CallsignDB::~MD2017CallsignDB() {
  // pass...
}


