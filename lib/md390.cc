#include "md390.hh"
#include "logger.hh"
#include "utils.hh"

#define NUM_CHANNELS                1000
#define ADDR_CHANNELS           0x01ee00
#define CHANNEL_SIZE            0x000040
#define BLOCK_SIZE                  1024


Radio::Features _md390_features = {
  .betaWarning = true,

  .hasDigital = true,
  .hasAnalog = true,

  // The frequency range actually depends on the model. It is not possible to infer the model.
  // Hence all channels are read during connection and the frquency range is inspected.
  .frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {136., 174.}, {350., 520.} },

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
  : TyTRadio(device, parent), _name("TyT MD-390"), _features(_md390_features)
{
  // Read channels and identify device variance based on the channel frequencies. This may block
  // the GUI for some time.
  unsigned addr_start = align_addr(ADDR_CHANNELS, BLOCK_SIZE);
  unsigned channel_offset = ADDR_CHANNELS-addr_start;
  uint total_size = align_size(addr_start+NUM_CHANNELS*CHANNEL_SIZE, BLOCK_SIZE)-addr_start;
  QByteArray buffer(total_size, 0xff);

  if (! _dev->read_start(0, addr_start)) {
    _errorMessage = "Cannot read channels from MD-390, cannot determine variant: "
        + _dev->errorMessage();
    logError() << _errorMessage;
    return;
  }
  for (unsigned i=0; i<(total_size/BLOCK_SIZE); i++){
    if (! _dev->read(0, ADDR_CHANNELS, (uint8_t *)buffer.data()+i*BLOCK_SIZE, BLOCK_SIZE)) {
      _errorMessage = "Cannot read channels from MD-390, cannot determine variant: "
          + _dev->errorMessage();
      logError() << _errorMessage;
      return;
    }
  }
  _dev->read_finish();

  // Determine frequency range of channels
  unsigned channelCount = 0;
  Radio::Features::FrequencyRange range(std::numeric_limits<double>::max(),0);
  for (unsigned i=0; i<NUM_CHANNELS; i++) {
    MD390Codeplug::ChannelElement channel(
          ((uint8_t *)buffer.data()) + channel_offset + i*CHANNEL_SIZE);
    if (! channel.isValid())
      continue;
    if (channel.rxFrequency()) {
      range.min = std::min(range.min, channel.rxFrequency()/1e6);
      range.max = std::max(range.max, channel.rxFrequency()/1e6);
    }
    if (channel.txFrequency()) {
      range.min = std::min(range.min, channel.txFrequency()/1e6);
      range.max = std::max(range.max, channel.txFrequency()/1e6);
    }
    channelCount++;
  }

  logDebug() << "Got frequency range " << range.min << "MHz - " << range.max
             << "MHz from " << channelCount << " channels.";

  if ((137<=range.min) && (174>=range.max)) {
    _features.frequencyLimits = QVector<Radio::Features::FrequencyRange>{{136., 174.}};
    _name += "V";
  } else if ((350<=range.min) && (400>=range.max)) {
    _features.frequencyLimits = QVector<Radio::Features::FrequencyRange>{{350., 400.}};
    _name += "U";
  } else if ((400<=range.min) && (450>=range.max)) {
    _features.frequencyLimits = QVector<Radio::Features::FrequencyRange>{{400., 480.}};
    _name += "U";
  } else if ((450<=range.min) && (520>=range.max)) {
    _features.frequencyLimits = QVector<Radio::Features::FrequencyRange>{{450., 520.}};
    _name += "U";
  } else {
    _errorMessage = tr("Cannot determine frequency range from channel frequencies between "
                       "%1MHz and %2MHz. Will not check frequency ranges")
        .arg(range.min).arg(range.max);
    logError() << _errorMessage;
  }
}

const QString &
MD390::name() const {
  return _name;
}

const Radio::Features &
MD390::features() const {
  return _features;
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
