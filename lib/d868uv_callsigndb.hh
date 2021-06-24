#ifndef D868UVCALLSIGNDB_HH
#define D868UVCALLSIGNDB_HH

#include "callsigndb.hh"
#include "d868uv_codeplug.hh"

/** Represents and encodes the binary format for the call-sign database within the radio.
 *
 * <table>
 *  <tr><th colspan="3">Callsign database</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>        <th>Content</th></tr>
 *  <tr><td>04000000</td> <td>max. 186a00</th> <td>Index of callsign entries. Follows the same
 *   weird format as @c D878UVCodeplug::contact_map_t. Sorted by ID. Empty entries set to
 *   0xffffffffffffffff.</td></tr>
 *  <tr><td>044c0000</td> <td>unknown</td>     <td>Database limits, see @c limits_t.</td></tr>
 *  <tr><td>04500000</td> <td>unknown</td>     <td>The actual DB entries, each entry is of
 *   variable size but shares the same header, see @c entry_t. Order arbitrary.
 *   Filled with 0x00.</td></tr>
 * </table>
 *
 * @ingroup d868uv */
class D868UVCallsignDB : public CallsignDB
{
  Q_OBJECT

public:
  /** Represents the header of an entry in the callsign database.
   * Each entry is of variable size. That is, every entry has a common header containing the
   * number, call-type etc. All strings, that is contact name, city, callsign, state, country and
   * comment are 0x00 terminated strings in a lists.
   *
   * Max lenghth for name is 16, city is 16, callsign is 8, state is 16, country is 16 and
   * comment is 16, excluding terminating 0x00.
   *
   * Memmory layout of encoded Callsign/User database entry:
   * @verbinclude d878uvcallsigndbentry.txt
   */
  struct __attribute__((packed)) entry_t {
    /** Possible call types for each entry of the callsign db. */
    typedef enum {
      PRIVATE_CALL = 0,                 ///< A private call entry.
      GROUP_CALL   = 1,                 ///< A group call entry.
      ALL_CALL     = 2                  ///< An all-call entry.
    } CallType;

    /** Notification tones for callsign entry. */
    typedef enum {
      RING_NONE = 0,
      RING_TONE = 1,
      RING_ONLINE = 2
    } RingTone;

    uint8_t call_type;                  ///< Call type, see @c CallType.
    uint32_t id;                        ///< DMR ID, BCD encoded, big endian.
    uint8_t ring;                       ///< Ring tone, see @c RingTone.

    uint8_t body[94];                   ///< Up to 94 bytes name, city, callsign, state, country and comment.

    /** Constructs a database entry from the given user.
     * @returns The size of the entry. */
    size_t fromUser(const UserDatabase::User &user);
    /** Computes the size of the database entry for the given user. */
    static size_t getSize(const UserDatabase::User &user);
  };


  /** Same index entry used by the codeplug to map normal digital contacts to an contact index. Here
   * it maps to the byte offset within the database entries. */
  typedef D868UVCodeplug::contact_map_t index_entry_t;

  /** Stores some basic limits of the callsign db.
   *
   * Memmory layout of encoded Callsign/User database entry:
   * @verbinclude d878uvcallsigndblimit.txt */
  struct __attribute__((packed)) limits_t {
    uint32_t count;                     ///< Number of db entries, little-endian.
    uint32_t end_of_db;                 ///< Memory address of end of entries, or where to insert
                                        ///  the next entry, little endian.
    uint32_t _unused8;                  ///< Unused? Set to 0x000000.
    uint32_t _unused12;                 ///< Unused? Set to 0x000000.

    /** Clear the db limits. */
    void clear();
    /** Sets the total amount of database entries. */
    void setCount(uint32_t n);
    /** Sets the total size of the db (only the entry section, not index). */
    void setTotalSize(uint32_t size);
  };


public:
  /** Constructor, does not allocate any memory yet. */
  explicit D868UVCallsignDB(QObject *parent=nullptr);

  /** Tries to encode as many entries of the given user-database. */
  bool encode(UserDatabase *db, const Selection &selection=Selection());
};

#endif // D868UVCALLSIGNDB_HH
