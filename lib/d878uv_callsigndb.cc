#include "d878uv_callsigndb.hh"
#include "utils.hh"
#include <QtEndian>

#define MAX_CALLSIGNS         0x00030d40  // Maximum number of callsings in DB (200k)
#define ADDR_CALLSIGN_INDEX   0x04000000  // Start address of the callsign index table
#define ADDR_CALLSIGN         0x04500000  // Start address of the actuall call sign entries
#define ADDR_UNKNOWN          0x044C0000  // Start address of some unkonwn area.

/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::callsign_db_t::entry_t
 * ******************************************************************************************** */

/* ********************************************************************************************* *
 * Implementation of D878UVCallsignDB
 * ********************************************************************************************* */
D878UVCallsignDB::D878UVCallsignDB(QObject *parent)
  : DFUFile(parent)
{
  // allocate and clear DB memory
  addImage("AnyTone AT-D878UV Callsign database.");
}

void
D878UVCallsignDB::encode(UserDatabase *db) {
  // Determine size of call-sign DB in memory
  qint64 n = std::min(db->count(), qint64(MAX_CALLSIGNS));
  qint64 size = align_size(0x4003 + 120*n, 1024);

}
