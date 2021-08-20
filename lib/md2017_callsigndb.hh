#ifndef DM2017_CALLSIGNDB_HH
#define DM2017_CALLSIGNDB_HH

#include "tyt_callsigndb.hh"

/** Device specific implementation of the call-sign DB for the TyT MD-2017.
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

  bool encode(UserDatabase *db, const Selection &selection);
};

#endif // DM2017_CALLSIGNDB_HH
