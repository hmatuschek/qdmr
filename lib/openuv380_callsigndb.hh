#ifndef OPENUV380CALLSIGNDB_HH
#define OPENUV380CALLSIGNDB_HH

#include "opengd77base_callsigndb.hh"
#include "userdatabase.hh"

/** Represents and encodes the binary format for the call-sign database within the radio.
 *
 * The memory layout of the call-sign DB is relatively simple. The DB starts at address
 * @c 0x00030000 with a maximum size of @c 0x00040000. The first 12bytes form the DB header
 * (see @c OpenGD77CallsignDB::userdb_t) followed by the DB entries (see
 * @c OpenGD77CallsignDB::userdb_entry_t).
 *
 * The entries can be of variable size. The size of each entry is encoded in the header. QDMR uses
 * a fixed size of 19bytes per entry. The entries must be sorted in ascending order to allow for an
 * efficient binary search. No index table is used here.
 *
 * @ingroup ogd77 */
class OpenUV380CallsignDB : public OpenGD77BaseCallsignDB
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit OpenUV380CallsignDB(QObject *parent=nullptr);

  /** Encodes as many entries as possible of the given user-database. */
  bool encode(UserDatabase *calldb, const Selection &selection=Selection(),
              const ErrorStack &err=ErrorStack());

protected:
  /** Some internal offsets within the callsign db. */
  struct Offset {
    /// @cond DO_NOT_DOCUMENT
    static constexpr unsigned int callsignDB() { return 0x00050000; }
    /// @endcond
  };
};

#endif // OPENGD77CALLSIGNDB_HH
