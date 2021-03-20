#ifndef CALLSIGNDB_HH
#define CALLSIGNDB_HH

#include "dfufile.hh"

// Forward decl.
class UserDatabase;

/** Abstract base class of all callsign database implementations. */
class CallsignDB : public DFUFile
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit CallsignDB(QObject *parent=nullptr);


public:
  /** Destructor. */
  virtual ~CallsignDB();

  /** Encodes the given user db into the device specific callsign db. */
  virtual bool encode(UserDatabase *db) = 0;
};

#endif // CALLSIGNDB_HH
