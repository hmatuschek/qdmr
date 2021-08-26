#ifndef MD2017_CALLSIGNDB_HH
#define MD2017_CALLSIGNDB_HH

#include "tyt_callsigndb.hh"

/** Device specific implementation of the call-sign DB for the TyT MD-2017.
 *
 * In fact this callsign DB is identical to the generic @c TyTCallsignDB.
 *
 * @ingroup md2017 */
class MD2017CallsignDB : public TyTCallsignDB
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit MD2017CallsignDB(QObject *parent=nullptr);
  /** Destructor. */
  virtual ~MD2017CallsignDB();
};

#endif // MD2017_CALLSIGNDB_HH
