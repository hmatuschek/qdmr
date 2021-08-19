#ifndef TYTCALLSIGNDB_HH
#define TYTCALLSIGNDB_HH

#include "callsigndb.hh"

class TyTCallsignDB : public CallsignDB
{
  Q_OBJECT

public:
  explicit TyTCallsignDB(QObject *parent=nullptr);
  virtual ~TyTCallsignDB();
};

#endif // TYTCALLSIGNDB_HH
