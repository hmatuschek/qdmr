#ifndef D878UVCALLSIGNDB_HH
#define D878UVCALLSIGNDB_HH

#include "dfufile.hh"
#include "userdatabase.hh"
#include "d878uv_codeplug.hh"

/** Represents and encodes the binary format for the call-sign database within the radio.
 *
 * <table>
 *  <tr><th colspan="3">Callsign database</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>        <th>Content</th></tr>
 *  <tr><td>04000000</td> <td>max. 186a00</th> <td>Index of callsign entries. Follows the same
 *   weird format as @c D878UVCodeplug::contact_map_t. Sorted by ID. Empty entries set to
 *   0xffffffffffffffff.</td></tr>
 *  <tr><td>044c0000</td> <td>unknown</td>     <td>Unknown, kind of an search index.</td></tr>
 *  <td><td>04500000</td> <td>unknown</td>     <td>The actual DB entries, each entry is of
 *   variable size but shares the same header, see @c entry_head_t. Order arbitrary.
 *   Filled with 0x00.</td></tr>
 * </table>
 * @ingroup d878uv */
class D878UVCallsignDB : public DFUFile
{
  Q_OBJECT

public:
  /** Represents the header of an entry the callsign database.
   * Each entry is of variable size. That is, every entry has a common header containing the
   * number, call-type etc. All names, that is contact name, city, callsign, state, country and
   * comment are 0x00 terminated string lists.
   *
   * Max lenghth for name is 16, city is 16, call is 8, state is 16, country is 16 and
   * comment is 16, excluding terminating 0x00.
   *
   * Memmory layout of encoded Callsign/User database entry:
   * @verbinclude d878uvuserdbentry.txt
   */
  struct __attribute__((packed)) enty_head_t {
    typedef enum {
      PRIVATE_CALL = 0,                 ///< A private call entry.
      GROUP_CALL   = 1,                 ///< A group call entry.
      ALL_CALL     = 2                  ///< An all-call entry.
    } CallType;

    typedef enum {
      RING_NONE = 0,
      RING_TONE = 1,
      RING_ONLINE = 2
    } RingTone;

    uint8_t call_type;                  ///< Call type, see @c CallType.
    uint32_t id;                        ///< DMR ID, BCD encoded, big endian.
    uint8_t ring;                       ///< Ring tone, see @c RingTone.
  };

  /** Same index entry used by codeplug to map normal digital contacts to an offset. Here
   * it maps to the byte offset within the database entries. */
  typedef D878UVCodeplug::contact_map_t index_entry_t;

public:
  /** Constructor, does not allocate any memory yet. */
  explicit D878UVCallsignDB(QObject *parent=nullptr);

  /** Tries to encode as many entries of the given user-database. */
  void encode(UserDatabase *db);
};

#endif // D878UVCALLSIGNDB_HH
