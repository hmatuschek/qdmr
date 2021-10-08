#include "md390.hh"


Radio::Features _md390_features = {
  .betaWarning = true,

  .hasDigital = true,
  .hasAnalog = true,

  // The frequency range actually depends on the model. It is not possible to infer the model.
  // Hence all channels are read during connection and the frquency range is inspected.
  .frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {136., 174.}, {400., 480.} },

  .maxRadioIDs        = 1,
  .needsDefaultRadioID = true,
  .maxIntroLineLength = 10,

  .maxChannels = 1000,
  .maxChannelNameLength = 16,
  .allowChannelNoDefaultContact = true,

  .maxZones = 250,
  .maxZoneNameLength = 16,
  .maxChannelsInZone = 16,
  .hasABZone = true,

  .hasScanlists = true,
  .maxScanlists = 250,
  .maxScanlistNameLength = 16,
  .maxChannelsInScanlist = 31,
  .scanListNeedsPriority = false,

  .maxContacts = 1000,
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

  .hasCallsignDB = false,
  .callsignDBImplemented = false,
  .maxCallsignsInDB = 0
};



MD390::MD390(TyTInterface *device, QObject *parent)
  : TyTRadio(device, parent), _name("TyT MD-390")
{
  // pass...
}

const QString &
MD390::name() const {
  return _name;
}

const Radio::Features &
MD390::features() const {
  return _md390_features;
}

const Codeplug &
MD390::codeplug() const {
  return _codeplug;
}

Codeplug &
MD390::codeplug() {
  return _codeplug;
}

const CallsignDB *
MD390::callsignDB() const {
  return nullptr;
}

CallsignDB *
MD390::callsignDB() {
  return nullptr;
}

RadioInfo
MD390::defaultRadioInfo() {
  return RadioInfo(RadioInfo::MD390, "md390", "MD-390", "TyT");
}
