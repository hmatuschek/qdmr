#ifndef OPENGD77CALLSIGNDB_HH
#define OPENGD77CALLSIGNDB_HH

#include "dfufile.hh"
#include "userdatabase.hh"


/** Represents and encodes the binary format for the call-sign database within the radio.
 * @ingroup opengd77 */
class OpenGD77CallsignDB : public DFUFile
{
  Q_OBJECT

  /** Represents a user-db entry within the binary codeplug. */
  struct __attribute__((packed)) userdb_entry_t {
    uint32_t number;                    ///< DMR ID stored in BCD little-endian.
    char name[15];                      ///< Call or name, upto 15 ASCII chars, 0x00 padded.

    userdb_entry_t();
    void clear();

    uint32_t getNumber() const;
    void setNumber(uint32_t number);

    QString getName() const;
    void setName(const QString &name);

    void fromEntry(const UserDatabase::User &user);
  };

  struct __attribute__((packed)) userdb_t {
    char magic[3];                      ///< Fixed string 'ID-'
    uint8_t size;                       ///< Fixed to 0x5d for 15 byte names.
    char version[3];                    ///< Version string? Fixed to '001'
    uint8_t unused6;                    ///< Unused, set to 0x00.
    uint16_t count;                     ///< Number of contacts in DB, 16bit little-endian.
    uint16_t unused9;                   ///< Unused, set to 0x0000.

    userdb_t();
    void clear();

    void setSize(uint n);
  };


public:
  /** Constructor. */
  explicit OpenGD77CallsignDB(QObject *parent=nullptr);
  /** Destructor. */
  virtual ~OpenGD77CallsignDB();

  /** Encodes as many entries as possible of the given user-database. */
  virtual bool encode(UserDatabase *calldb);
};

#endif // OPENGD77CALLSIGNDB_HH
