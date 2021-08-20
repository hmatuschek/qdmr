#ifndef TYTCALLSIGNDB_HH
#define TYTCALLSIGNDB_HH

#include "callsigndb.hh"

/** Base class for all call-sign DBs of TyT/Retevis devices.
 * @ingroup tyt */
class TyTCallsignDB : public CallsignDB
{
  Q_OBJECT

protected:
  /** Hidden constructor. Use one of the device specific call-sign DB classes. */
  explicit TyTCallsignDB(QObject *parent=nullptr);

public:
  virtual ~TyTCallsignDB();
};

#endif // TYTCALLSIGNDB_HH
