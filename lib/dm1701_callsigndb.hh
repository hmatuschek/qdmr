#ifndef DM1701_CALLSIGNDB_HH
#define DM1701_CALLSIGNDB_HH

#include "tyt_callsigndb.hh"

/** Device specific implementation of the call-sign DB for the BTECH DM1701 and Retevis RT84.
 *
 * In fact this callsign DB is identical to the generic @c TyTCallsignDB.
 *
 * @ingroup dm1701 */
class DM1701CallsignDB : public TyTCallsignDB
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit DM1701CallsignDB(QObject *parent=nullptr);
  /** Destructor. */
  virtual ~DM1701CallsignDB();
};

#endif // DM1701_CALLSIGNDB_HH
