#include "dm1701.hh"
#include "config.hh"
#include "logger.hh"
#include "utils.hh"

static Radio::Features _dm1701_features =
{
  .betaWarning = false,

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


DM1701::DM1701(TyTInterface *device, QObject *parent)
  : TyTRadio(device, parent), _name("Baofeng DM-1701"), _codeplug()
{
  // pass...
}

const QString &
DM1701::name() const {
  return _name;
}

const Radio::Features &
DM1701::features() const {
  return _dm1701_features;
}

RadioInfo
DM1701::defaultRadioInfo() {
  return RadioInfo(
        RadioInfo::DM1701, "dm1701", "DM-1701", "Baofeng",
        QList<RadioInfo>(), TyTInterface::interfaceInfo());
}

const Codeplug &
DM1701::codeplug() const {
  return _codeplug;
}

Codeplug &
DM1701::codeplug() {
  return _codeplug;
}
