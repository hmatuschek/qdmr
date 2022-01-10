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
  .needsDefaultRadioID = false,
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


D868UV::D868UV(AnytoneInterface *device, QObject *parent)
  : AnytoneRadio("Anytone AT-D868UV", device, parent), _features(_d868uv_features)
{
  _codeplug = new D868UVCodeplug(this);
  _callsigns = new D868UVCallsignDB(this);
  _supported_version = "V102";

  // Get device info and determine supported TX frequency bands
  AnytoneInterface::RadioVariant info; _dev->getInfo(info);
  switch (info.bands) {
  case 0x00:
    _features.frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {136., 174.}, {400., 480.} };
    break;
  case 0x01:
    _features.frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {144., 148.}, {420., 450.} };
    break;
  case 0x02:
    _features.frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {136., 174.}, {430., 450.} };
    break;
  case 0x03:
    _features.frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {144., 146.}, {430., 440.} };
    break;
  case 0x04:
    _features.frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {136., 174.}, {440., 480.} };
    break;
  case 0x05:
    _features.frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {144., 146.}, {440., 480.} };
    break;
  case 0x06:
    _features.frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {136., 174.}, {446., 447.} };
    break;
  case 0x07:
    _features.frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {144., 146.}, {446., 447.} };
    break;
  case 0x08:
    _features.frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {136., 174.}, {440., 470.} };
    break;
  case 0x09:
    _features.frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {144., 146.}, {430., 432.} };
    break;
  case 0x0a:
    _features.frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {144., 148.}, {430., 450.} };
    break;
  case 0x0b:
    _features.frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {144., 146.}, {430., 440.} };
    break;
  case 0x0c:
    _features.frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {136., 174.}, {403., 470.} };
    break;

  default:
    logInfo() << "Unknown band-code" << QString::number(int(info.bands), 16)
              << ": Set freq range to 136-174MHz and 400-480MHz.";
    _features.frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {136., 174.}, {400., 480.} };
    break;
  }

  QStringList bands;
  foreach(Radio::Features::FrequencyRange r, _features.frequencyLimits.ranges) {
    bands.append(tr("%1-%2MHz").arg(r.min).arg(r.max));
  }
  logDebug() << "Got band-code " << QString::number(int(info.bands), 16)
             << ": Limit TX frequencies to " << bands.join(", ") << ".";

  // Store HW version
  _version = info.version;
}

const Radio::Features &
D868UV::features() const {
  return _features;
}

RadioInfo
D868UV::defaultRadioInfo() {
  return RadioInfo(
        RadioInfo::D868UVE, "d868uve", "AT-D868UVE", "AnyTone", AnytoneInterface::interfaceInfo(),
        QList<RadioInfo>{
          RadioInfo(RadioInfo::DMR6X2, "dmr6x2", "DMR-6X2", "BTech", AnytoneInterface::interfaceInfo()),
          RadioInfo(RadioInfo::D868UV, "d868uv", "AT-D868UV", "AnyTone", AnytoneInterface::interfaceInfo())
        });
}
