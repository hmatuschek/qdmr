#include "md2017.hh"


Radio::Features _md2017_features = {
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



MD2017::MD2017(DFUDevice *device, QObject *parent)
  : TyTRadio(device, parent), _name("TyT DM-2017")
{
  //_codeplug(nullptr), _callsigndb(nullptr);
}

MD2017::~MD2017() {
  // pass...
}

const QString &
MD2017::name() const {
  return _name;
}

const Radio::Features &
MD2017::features() const {
  return _md2017_features;
}

const CodePlug &
MD2017::codeplug() const {
  return _codeplug;
}

CodePlug &
MD2017::codeplug() {
  return _codeplug;
}

const CallsignDB *
MD2017::callsignDB() const {
  return &_callsigndb;
}

CallsignDB *
MD2017::callsignDB() {
  return &_callsigndb;
}
