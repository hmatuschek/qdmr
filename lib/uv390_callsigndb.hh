#ifndef UV390CALLSIGNDB_HH
#define UV390CALLSIGNDB_HH

#include "dfufile.hh"
#include "userdatabase.hh"


class UV390CallsignDB : public DFUFile
{
  Q_OBJECT

public:
  /** Represents a search index over the complete callsign database.
   *
   * Memmory layout of encoded Callsign/User database:
   * @verbinclude uv390userdb.txt
   */
  struct __attribute__((packed)) callsign_db_t {
    /** Represents an index entry, a pair of DMR ID and callsign DB index.
     *
     * Memmory layout of encoded Callsign/User database index entry:
     * @verbinclude uv390userdbentry.txt
     */
    struct __attribute__((packed)) entry_t {
      uint8_t val[4]; // Combined index and ID MSBs;

      /// Empty constructor.
      entry_t();
      /** Clears this entry. */
      void clear();
      void set(uint id, uint index);

      /** Returns true, if the database index entry is valid. */
      bool isValid() const;
    };

    /** Represents an entry within the callsign database.
     * The callsign DB entries must be ordered by their DMR id.
     *
     * Memmory layout of encoded Callsign/User database index entry:
     * @verbinclude uv390userdbcallsign.txt
     */
    struct __attribute__((packed)) callsign_t {
      uint32_t dmrid:24,                  ///< DMR id in binary little-endian
        _unused: 8;                       ///< Unused set to 0xff.
      uint8_t callsign[16];               ///< 16 x ASCII zero-terminated.
      uint8_t name[100];                  ///< Descriptive name, nickname, city, state, country. 100 x ASCII zero-terminated.


      /// Empty constructor.
      callsign_t();

      /// Clears the DB entry.
      void clear();
      /// Returns @c true if entry is valid.
      bool isValid() const;
      /** Sets the ID of the entry. */
      void setID(uint32_t dmrid);
      /** Sets the call of the entry. */
      void setCall(const QString &call);
      /** Sets the name, city, country etc. of the entry. */
      void setName(const QString &name);
      /** Constructs an entry from the given user. */
      void fromUser(const UserDatabase::User &user);
    };

    uint8_t n[3];                         ///< Number of contacts in compete database, 24bit big-endian.
    entry_t index[4096];                  ///< 4096 index entries, default 0xff.
    callsign_t db[122197];                ///< up to 122197 database callsign entries.

    /// Empty constructor.
    callsign_db_t();

    /// Clears the complete callsign database.
    void clear();
    void setN(uint N);
    /// Fills the callsign database from the given user db.
    void fromUserDB(const UserDatabase *db);
  };

public:
  explicit UV390CallsignDB(QObject *parent=nullptr);

  void encode(UserDatabase *db);
};

#endif // UV390CALLSIGNDB_HH
