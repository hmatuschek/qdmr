#include "d878uv.hh"
#include "config.hh"
#include "logger.hh"

#include "d878uv_codeplug.hh"
// uses same callsign db as 878
#include "d868uv_callsigndb.hh"

#define RBSIZE 16
#define WBSIZE 16


static Radio::Features _d878uv_features =
{
  // show beta-warning
  .betaWarning = true,

  // general capabilities
  .hasDigital  = true,
  .hasAnalog   = true,

  .vhfLimits = {136., 174.},
  .uhfLimits = {400., 480.},

  // general limits
  .maxRadioIDs        = 250,
  .maxNameLength      = 16,
  .maxIntroLineLength = 14,

  // channel limits
  .maxChannels                  = 4000,
  .maxChannelNameLength         = 16,
  .allowChannelNoDefaultContact = false,

  // zone limits
  .maxZones               = 250,
  .maxZoneNameLength      = 16,
  .maxChannelsInZone      = 250,
  .hasABZone              = false,

  // scanlist limits
  .hasScanlists           = true,
  .maxScanlists           = 250,
  .maxScanlistNameLength  = 16,
  .maxChannelsInScanlist  = 31,
  .scanListNeedsPriority  = false,

  // contact list limits
  .maxContacts            = 10000,
  .maxContactNameLength   = 16,

  // rx group list limits
  .maxGrouplists          = 250,
  .maxGrouplistNameLength = 16,
  .maxContactsInGrouplist = 64,

  .hasGPS = true,
  .maxGPSSystems = 8,

  .hasAPRS = true,
  .maxAPRSSystems = 1,

  .hasRoaming               = true,
  .maxRoamingChannels       = 250,
  .maxRoamingZones          = 64,
  .maxChannelsInRoamingZone = 64,

  // call-sign database limits
  .hasCallsignDB          = true,  // hasCallsignDB
  .callsignDBImplemented  = true,  // callsignDBImplemented
  .maxCallsignsInDB       = 200000 // maxCallsignsInDB
};


D878UV::D878UV(QObject *parent)
  : AnytoneRadio("Anytone AT-D878UV", parent)
{
  _codeplug = new D878UVCodeplug(this);
  _callsigns = new D868UVCallsignDB(this);
}

const Radio::Features &
D878UV::features() const {
  return _d878uv_features;
}
