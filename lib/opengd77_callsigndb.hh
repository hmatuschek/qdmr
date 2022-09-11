#ifndef OPENGD77CALLSIGNDB_HH
#define OPENGD77CALLSIGNDB_HH

#include "callsigndb.hh"
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
class OpenGD77CallsignDB : public CallsignDB
{
  Q_OBJECT

public:
  /** Represents a user-db entry within the binary codeplug.
   *
   * Memory representation of the call-sign DB entry (size 0x13 bytes):
   * @verbinclude opengd77_callsign_db_entry.txt */
  struct __attribute__((packed)) userdb_entry_t {
    uint32_t number;                    ///< DMR ID stored in BCD little-endian.
    char name[15];                      ///< Call or name, up to 15 ASCII chars, 0x00 padded.

    /** Constructor. */
    userdb_entry_t();
    /** Resets the entry. */
    void clear();

    /** Returns the DMR ID number. */
    uint32_t getNumber() const;
    /** Sets the DMR ID number. */
    void setNumber(uint32_t number);

    /** Returns the name of the entry. */
    QString getName() const;
    /** Sets the name of the entry, 15b max.
     * The name gets truncated if longer than 15b. */
    void setName(const QString &name);

    /** Encodes the given user. */
    void fromEntry(const UserDatabase::User &user);
  };

  /** Represents the binary call-sign database header.
   *
   * Memory representation of the call-sign DB header (size: 0x0c bytes):
   * @verbinclude opengd77_callsign_db_header.txt
   **/
  struct __attribute__((packed)) userdb_t {
    char magic[3];                      ///< Fixed string 'ID-'
    uint8_t size;                       ///< Fixed to 0x5d for 15 byte names.
    char version[3];                    ///< Version string? Fixed to '001'
    uint8_t unused6;                    ///< Unused, set to 0x00.
    uint32_t count;                     ///< Number of contacts in DB, 32bit little-endian.

    /** Constructor. */
    userdb_t();
    /** Resets the header. */
    void clear();
    /** Sets the number of DB entries. This number is limited to USERDB_NUM_ENTRIES.*/
    void setSize(unsigned n);
  };


public:
  /** Constructor. */
  explicit OpenGD77CallsignDB(QObject *parent=nullptr);
  /** Destructor. */
  virtual ~OpenGD77CallsignDB();

  /** Encodes as many entries as possible of the given user-database. */
  virtual bool encode(UserDatabase *calldb, const Selection &selection=Selection(),
                      const ErrorStack &err=ErrorStack());
};

#endif // OPENGD77CALLSIGNDB_HH
