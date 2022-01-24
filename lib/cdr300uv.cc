#include "cdr300uv.hh"
#include "cdr300uv_codeplug.hh"
#include "kydera_interface.hh"


static Radio::Features _cdr300uv_features =
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
  .hasCallsignDB          = true,   // hasCallsignDB
  .callsignDBImplemented  = false,  // callsignDBImplemented
  .maxCallsignsInDB       = 0       // maxCallsignsInDB
};


CDR300UV::CDR300UV(KyderaInterface *device, QObject *parent)
  : KyderaRadio("CDR-300UV", device, parent)//, _codeplug(new CDR300UVCodeplug(this))
{
  // pass...
}

RadioInfo
CDR300UV::info() const {
  return defaultRadioInfo();
}


RadioInfo
CDR300UV::defaultRadioInfo() {
  return RadioInfo(
        RadioInfo::CDR300UV, "cdr300uv", "CDR-300UV", "Kydera",
        USBDeviceInfo(USBDeviceInfo::Class::Serial, 0x067B, 0x2303, false, false),
        QList<RadioInfo>{
          RadioInfo(RadioInfo::RT73, "rt73", "RT73", "Retevis",
            USBDeviceInfo(USBDeviceInfo::Class::Serial, 0x067B, 0x2303, false, false)),
          RadioInfo(RadioInfo::DB25D, "db25d", "DB25-D", "Radioddity",
            USBDeviceInfo(USBDeviceInfo::Class::Serial, 0x067B, 0x2303, false, false))
        });
}

const Radio::Features &
CDR300UV::features() const {
  return _cdr300uv_features;
}
