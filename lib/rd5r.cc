#include "rd5r.hh"
#include "config.hh"

#define BSIZE 128


static Radio::Features _rd5r_features =
{
  .betaWarning = false,

  .hasDigital = true,
  .hasAnalog  = true,

  .frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {136., 174.}, {400., 470.} },

  .maxRadioIDs         = 1,
  .needsDefaultRadioID = true,
  .maxIntroLineLength  = 16,

  .maxChannels = 1024,
  .maxChannelNameLength = 16,
  .allowChannelNoDefaultContact = true,

  .maxZones = 250,
  .maxZoneNameLength = 16,
  .maxChannelsInZone = 16,
  .hasABZone = false,

  .hasScanlists = true,
  .maxScanlists = 250,
  .maxScanlistNameLength = 16,
  .maxChannelsInScanlist = 31,
  .scanListNeedsPriority = true,

  .maxContacts = 256,
  .maxContactNameLength = 16,

  .maxGrouplists = 64,
  .maxGrouplistNameLength = 16,
  .maxContactsInGrouplist = 16,

  .hasGPS = false,
  .maxGPSSystems = 0,

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


RD5R::RD5R(RadioddityInterface *device, QObject *parent)
  : RadioddityRadio(device, parent), _name("Baofeng/Radioddity RD-5R"), _codeplug()
{
  // pass...
}

RD5R::~RD5R() {
  // pass...
}

const QString &
RD5R::name() const {
  return _name;
}

const Radio::Features &
RD5R::features() const {
  return _rd5r_features;
}

RadioInfo
RD5R::info() const {
  return defaultRadioInfo();
}

const Codeplug &
RD5R::codeplug() const {
  return _codeplug;
}

Codeplug &
RD5R::codeplug() {
  return _codeplug;
}

RadioInfo
RD5R::defaultRadioInfo() {
  return RadioInfo(
        RadioInfo::RD5R, "rd5r", "RD-5R", "Radioddity", RadioddityInterface::interfaceInfo());
}
