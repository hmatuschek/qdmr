#ifndef DM2017_CALLSIGNDB_HH
#define DM2017_CALLSIGNDB_HH

#include "tyt_callsigndb.hh"

class MD2017CallsignDB : public TyTCallsignDB
{
  Q_OBJECT

public:
  explicit MD2017CallsignDB(QObject *parent=nullptr);
  virtual ~MD2017CallsignDB();

  bool encode(UserDatabase *db, const Selection &selection);
};

#endif // DM2017_CALLSIGNDB_HH
