#ifndef GD77CALLSIGNDB_HH
#define GD77CALLSIGNDB_HH

#include "callsigndb.hh"
#include "userdatabase.hh"

/** Represents and encodes the binary format for the call-sign database within the radio.
 *
 * The call-sign DB gets stored at a separate memory bank (no. 3) at address @c 0x0000. The encoded
 * DB consists of the DB header followed by the DB entries. The number of entries is limited to
 * 10920 leading to a max DB size of 0x20000 bytes.
 *
 * The header is encoded as:
 * @verbinclude gd77callsigndb.txt
 * The number of DB entries is encoded as a 32bit little-endian integer.
 *
 * Each DB entry is encoded as:
 * @verbinclude gd77callsigndbentry.txt
 * The DMR ID gets encoded a 8 BCD digits in little endian while the name is encoded as up-to 8
 * ASCII bytes 0-terminated and padded.
 *
 * @ingroup gd77 */
class GD77CallsignDB : public CallsignDB
{
  Q_OBJECT

  /** Represents a user-db entry within the binary codeplug. */
  struct __attribute__((packed)) userdb_entry_t {
    uint32_t number;                    ///< DMR ID stored in BCD little-endian.
    char name[8];                       ///< Call or name, upto 8 ASCII chars, 0x00 padded.

    userdb_entry_t();
    void clear();

    uint32_t getNumber() const;
    void setNumber(uint32_t number);

    QString getName() const;
    void setName(const QString &name);

    void fromEntry(const UserDatabase::User &user);
  };

  struct __attribute__((packed)) userdb_t {
    char magic[8];                      ///< Fixed string 'ID-V001\0'
    uint32_t count;                     ///< Number of contacts in DB, 32bit little-endian.

    userdb_t();
    void clear();

    void setSize(uint32_t n);
  };


public:
  /** Constructor. */
  explicit GD77CallsignDB(QObject *parent=nullptr);
  /** Destructor. */
  virtual ~GD77CallsignDB();

  /** Encodes as many entries as possible of the given user-database. */
  virtual bool encode(UserDatabase *calldb, const Selection &selection=Selection());
};

#endif // GD77CALLSIGNDB_HH
