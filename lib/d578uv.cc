#include "d578uv.hh"
#include "config.hh"
#include "logger.hh"

#include "d578uv_limits.hh"
#include "d578uv_codeplug.hh"
// uses same callsign db as 878
#include "d878uv2_callsigndb.hh"

#define RBSIZE 16
#define WBSIZE 16


static Radio::Features _d578uv_features =
{
  // show beta-warning
  .betaWarning = true,

  // general capabilities
  .hasDigital  = true,
  .hasAnalog   = true,

  .frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {136., 174.}, {220., 225.}, {400., 480.} },

  // general limits
  .maxRadioIDs         = 250,
  .needsDefaultRadioID = false,
  .maxIntroLineLength  = 14,

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
  .maxCallsignsInDB       = 500000 // maxCallsignsInDB
};


D578UV::D578UV(AnytoneInterface *device, QObject *parent)
  : AnytoneRadio("Anytone AT-D578UV", device, parent), _features(_d578uv_features), _limits(nullptr)
{
  _codeplug = new D578UVCodeplug(this);
  _callsigns = new D878UV2CallsignDB(this);
  _supported_version = "V110";

  // Get device info and determine supported TX frequency bands
  AnytoneInterface::RadioVariant info; _dev->getInfo(info);
  switch (info.bands) {
  case 0x00:
  case 0x01:
  case 0x04:
    _features.frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {136., 174.}, {400., 480.} };
    _limits = new D578UVLimits({ {136., 174.}, {400., 480.} }, this);
    break;
  case 0x02:
    _features.frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {136., 174.}, {430., 440.} };
    _limits = new D578UVLimits({ {136., 174.}, {430., 440.} }, this);
    break;
  case 0x03:
  case 0x05:
    _features.frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {144., 146.}, {430., 440.} };
    _limits = new D578UVLimits({ {144., 146.}, {430., 440.} }, this);
    break;
  case 0x06:
    _features.frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {136., 174.}, {446., 447.} };
    _limits = new D578UVLimits({ {136., 174.}, {446., 447.} }, this);
    break;
  case 0x07:
    _features.frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {144., 148.}, {420., 450.} };
    _limits = new D578UVLimits({ {144., 148.}, {420., 450.} }, this);
    break;
  case 0x08:
    _features.frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {136., 174.}, {400., 470.} };
    _limits = new D578UVLimits({ {136., 174.}, {400., 470.} }, this);
    break;
  case 0x09:
    _features.frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {144., 146.}, {430., 432.} };
    _limits = new D578UVLimits({ {144., 146.}, {430., 432.} }, this);
    break;
  case 0x0a:
    _features.frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {144., 148.}, {430., 450.} };
    _limits = new D578UVLimits({ {144., 148.}, {430., 450.} }, this);
    break;
  case 0x0b:
    _features.frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {136., 174.}, {400., 520.} };
    _limits = new D578UVLimits({ {136., 174.}, {400., 520.} }, this);
    break;
  case 0x0c:
    _features.frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {136., 174.}, {400., 490.} };
    _limits = new D578UVLimits({ {136., 174.}, {400., 490.} }, this);
    break;
  case 0x0d:
    _features.frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {136., 174.}, {403., 470.} };
    _limits = new D578UVLimits({ {136., 174.}, {403., 470.} }, this);
    break;
  case 0x0e:
    _features.frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {136., 174.}, {220.,225.}, {400., 520.} };
    _limits = new D578UVLimits({ {136., 174.}, {220.,225.}, {400., 520.} }, this);
    break;
  case 0x0f:
    _features.frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {144., 148.}, {400., 520.} };
    _limits = new D578UVLimits({ {144., 148.}, {400., 520.} }, this);
    break;
  case 0x10:
    _features.frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {144., 147.}, {430., 440.} };
    _limits = new D578UVLimits({ {144., 147.}, {430., 440.} }, this);
    break;
  case 0x11:
    _features.frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {136., 174.} };
    _limits = new D578UVLimits({ {136., 174.} }, this);
    break;
  default:
    logInfo() << "Unknown band-code 0x" << QString::number(int(info.bands), 16)
              << ": Set freq range to 136-174MHz and 400-480MHz.";
    _features.frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {136., 174.}, {400., 480.} };
    _limits = new D578UVLimits({ {136., 174.}, {400., 480.} }, this);
    break;
  }

  QStringList bands;
  foreach(Radio::Features::FrequencyRange r, _features.frequencyLimits.ranges) {
    bands.append(tr("%1-%2MHz").arg(r.min).arg(r.max));
  }
  logDebug() << "Got band-code " << QString::number(int(info.bands), 16)
             << ": Limit TX frequencies to " << bands.join(", ") << ".";

  _version = info.version;
}

const Radio::Features &
D578UV::features() const {
  return _features;
}

RadioInfo
D578UV::defaultRadioInfo() {
  return RadioInfo(
        RadioInfo::D578UV, "d578uv", "AT-D578UV", "AnyTone", AnytoneInterface::interfaceInfo());
}
