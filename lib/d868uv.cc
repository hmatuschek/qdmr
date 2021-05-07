#include "d868uv.hh"
#include "config.hh"
#include "logger.hh"
#include "d868uv_codeplug.hh"
#include "d868uv_callsigndb.hh"

#define RBSIZE 16
#define WBSIZE 16


static Radio::Features _d868uv_features =
{
  // show beta-warning
  .betaWarning = true,

  // general capabilities
  .hasDigital  = true,
  .hasAnalog   = true,

  .frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {136., 174.}, {400., 480.} },

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
  .maxGPSSystems = 1,

  .hasAPRS = false,
  .maxAPRSSystems = 0,

  .hasRoaming               = false,
  .maxRoamingChannels       = 0,
  .maxRoamingZones          = 0,
  .maxChannelsInRoamingZone = 0,

  // call-sign database limits
  .hasCallsignDB          = true,  // hasCallsignDB
  .callsignDBImplemented  = true,  // callsignDBImplemented
  .maxCallsignsInDB       = 200000 // maxCallsignsInDB
};


D868UV::D868UV(QObject *parent)
  : AnytoneRadio("Anytone AT-D868UV", parent)
{
  _codeplug = new D868UVCodeplug(this);
  _callsigns = new D868UVCallsignDB(this);
}

const Radio::Features &
D868UV::features() const {
  return _d868uv_features;
}
