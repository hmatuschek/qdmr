#include "uv390.hh"


Radio::Features _uv390_features = {
  .betaWarning = true,

  .hasDigital = true,
  .hasAnalog = true,

  .frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {136., 174.}, {400., 480.} },

  .maxRadioIDs        = 1,  /// @todo UV390 supports multiple radio IDs, not implemented yet.
  .needsDefaultRadioID = true,
  .maxIntroLineLength = 10,

  .maxChannels = 3000,
  .maxChannelNameLength = 16,
  .allowChannelNoDefaultContact = true,

  .maxZones = 250,
  .maxZoneNameLength = 16,
  .maxChannelsInZone = 64,
  .hasABZone = true,

  .hasScanlists = true,
  .maxScanlists = 250,
  .maxScanlistNameLength = 16,
  .maxChannelsInScanlist = 31,
  .scanListNeedsPriority = false,

  .maxContacts = 10000,
  .maxContactNameLength = 16,

  .maxGrouplists = 250,
  .maxGrouplistNameLength = 16,
  .maxContactsInGrouplist = 32,

  .hasGPS = true,
  .maxGPSSystems = 16,

  .hasAPRS = false,
  .maxAPRSSystems = 0,

  .hasRoaming = false,
  .maxRoamingChannels = 0,
  .maxRoamingZones = 0,
  .maxChannelsInRoamingZone = 0,

  .hasCallsignDB = true,
  .callsignDBImplemented = false,
  .maxCallsignsInDB = 122197
};



UV390::UV390(TyTInterface *device, QObject *parent)
  : TyTRadio(device, parent), _name("TyT MD-UV390")
{
  // pass...
}

UV390::~UV390() {
  // pass...
}

const QString &
UV390::name() const {
  return _name;
}

const Radio::Features &
UV390::features() const {
  return _uv390_features;
}

RadioInfo
UV390::info() const {
  return defaultRadioInfo();
}

const Codeplug &
UV390::codeplug() const {
  return _codeplug;
}

Codeplug &
UV390::codeplug() {
  return _codeplug;
}

const CallsignDB *
UV390::callsignDB() const {
  return &_callsigndb;
}

CallsignDB *
UV390::callsignDB() {
  return &_callsigndb;
}

RadioInfo
UV390::defaultRadioInfo() {
  return RadioInfo(
        RadioInfo::UV390, "uv390", "MD-UV390", "TyT",
        QList<RadioInfo>{
          RadioInfo(RadioInfo::UV380, "uv380", "MD-UV380", "TyT"),
          RadioInfo(RadioInfo::RT3S, "rt3s", "RT3S", "Retevis")
        });
}
