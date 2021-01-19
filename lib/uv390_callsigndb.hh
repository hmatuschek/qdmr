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
      uint32_t id_high: 12,               ///< High bits of DMR ID (23:12).
        index: 20;                        ///< Index in callsign data base, where to find these.

      /// Empty constructor.
      entry_t();

      /** Clears this entry. */
      void clear();
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
      uint8_t dmrid[3];                   ///< DMR id in BCD
      uint8_t _unused;                    ///< Unused set to 0xff.
      char callsign[16];                  ///< 16 x ASCII zero-terminated.
      char name[100];                     ///< Descriptive name, nickname, city, state, country. 100 x ASCII zero-terminated.


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

    uint32_t n : 24;                      ///< Number of contacts in compete database.
    entry_t index[4096];                  ///< 4096 index entries, default 0xff.
    callsign_t db[122197];                ///< 122197 database callsign entries.

    /// Empty constructor.
    callsign_db_t();

    /// Clears the complete callsign database.
    void clear();
    /// Fills the callsign database from the given user db.
    void fromUserDB(const UserDatabase *db);
  };

public:
  explicit UV390CallsignDB(QObject *parent=nullptr);

  void encode(UserDatabase *db);
};

#endif // UV390CALLSIGNDB_HH
