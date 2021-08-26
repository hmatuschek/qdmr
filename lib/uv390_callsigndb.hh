#ifndef UV390_CALLSIGNDB_HH
#define UV390_CALLSIGNDB_HH

#include "tyt_callsigndb.hh"

/** Device specific implementation of the call-sign DB for the TyT MD-UV390.
 *
 * In fact this callsign DB is identical to the generic @c TyTCallsignDB.
 *
 * @ingroup uv390 */
class UV390CallsignDB : public TyTCallsignDB
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit UV390CallsignDB(QObject *parent=nullptr);
  /** Destructor. */
  virtual ~UV390CallsignDB();
};

#endif // UV390_CALLSIGNDB_HH
