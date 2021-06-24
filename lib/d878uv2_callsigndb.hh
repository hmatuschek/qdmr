#ifndef D878UV2CALLSIGNDB_HH
#define D878UV2CALLSIGNDB_HH

#include "d868uv_callsigndb.hh"

/** Represents and encodes the binary format for the call-sign database within the radio.
 *
 * <table>
 *  <tr><th colspan="3">Callsign database</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>        <th>Content</th></tr>
 *  <tr><td>04000000</td> <td>variable</th>    <td>Index of callsign entries. Follows the same
 *   weird format as @c D878UVCodeplug::contact_map_t. Sorted by ID. Empty entries set to
 *   0xffffffffffffffff.</td></tr>
 *  <tr><td>04840000</td> <td>000010</td>      <td>Database limits, see @c limits_t.</td></tr>
 *  <tr><td>05500000</td> <td>variable</td>    <td>The actual DB entries, each entry is of
 *   variable size but shares the same header, see @c entry_t. Order arbitrary.
 *   Filled with 0x00.</td></tr>
 * </table>
 *
 * @ingroup d878uv2 */
class D878UV2CallsignDB : public D868UVCallsignDB
{
  Q_OBJECT

public:
  /** Constructor, does not allocate any memory yet. */
  explicit D878UV2CallsignDB(QObject *parent=nullptr);

  /** Tries to encode as many entries of the given user-database. */
  bool encode(UserDatabase *db, const Selection &selection=Selection());
};

#endif // D868UVCALLSIGNDB_HH
