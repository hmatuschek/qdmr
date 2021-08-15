#ifndef GD77CALLSIGNDB_HH
#define GD77CALLSIGNDB_HH

#include "callsigndb.hh"
#include "userdatabase.hh"

/** Represents and encodes the binary format for the call-sign database within the radio.
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

    void setSize(uint n);
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
