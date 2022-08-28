#include "md390.hh"
#include "md390_limits.hh"

#include "logger.hh"
#include "utils.hh"

#define NUM_CHANNELS                1000
#define ADDR_CHANNELS           0x01ee00
#define CHANNEL_SIZE            0x000040
#define BLOCK_SIZE                  1024


MD390::MD390(TyTInterface *device, const ErrorStack &err, QObject *parent)
  : TyTRadio(device, parent), _name("TyT MD-390"), _limits(nullptr)
{
  // Read channels and identify device variance based on the channel frequencies. This may block
  // the GUI for some time.
  unsigned addr_start = align_addr(ADDR_CHANNELS, BLOCK_SIZE);
  unsigned channel_offset = ADDR_CHANNELS-addr_start;
  uint total_size = align_size(addr_start+NUM_CHANNELS*CHANNEL_SIZE, BLOCK_SIZE)-addr_start;
  QByteArray buffer(total_size, 0xff);

  if (! _dev->read_start(0, addr_start, err)) {
    errMsg(err) << "Cannot read channels from MD-390, cannot determine variant.";
    return;
  }
  for (unsigned i=0; i<(total_size/BLOCK_SIZE); i++){
    if (! _dev->read(0, ADDR_CHANNELS, (uint8_t *)buffer.data()+i*BLOCK_SIZE, BLOCK_SIZE, err)) {
      errMsg(err) << "Cannot read channels from MD-390, cannot determine variant.";
      return;
    }
  }
  _dev->read_finish(err);

  // Determine frequency range of channels
  unsigned channelCount = 0;
  QPair<double, double> range(std::numeric_limits<double>::max(),0);
  for (unsigned i=0; i<NUM_CHANNELS; i++) {
    MD390Codeplug::ChannelElement channel(
          ((uint8_t *)buffer.data()) + channel_offset + i*CHANNEL_SIZE);
    if (! channel.isValid())
      continue;
    if (channel.rxFrequency()) {
      range.first = std::min(range.first, channel.rxFrequency()/1e6);
      range.second = std::max(range.second, channel.rxFrequency()/1e6);
    }
    if (channel.txFrequency()) {
      range.first = std::min(range.first, channel.txFrequency()/1e6);
      range.second = std::max(range.second, channel.txFrequency()/1e6);
    }
    channelCount++;
  }

  logDebug() << "Got frequency range " << range.first << "MHz - " << range.second
             << "MHz from " << channelCount << " channels.";

  if ((137<=range.first) && (174>=range.second)) {
    _limits = new MD390Limits({{136., 174.}}, this);
    _name += "V";
  } else if ((350<=range.first) && (400>=range.second)) {
    _limits = new MD390Limits({{350., 400.}}, this);
    _name += "U";
  } else if ((400<=range.first) && (450>=range.second)) {
    _limits = new MD390Limits({{400., 480.}}, this);
    _name += "U";
  } else if ((450<=range.first) && (520>=range.second)) {
    _limits = new MD390Limits({{450., 520.}}, this);
    _name += "U";
  } else {
    // Invalid frequency range, needs "Ignore frequency limits" in settings to write any codeplug.
    _limits = new MD390Limits({}, this);
    errMsg(err) << "Cannot determine frequency range from channel frequencies between "
                << range.first << "MHz and " << range.second
                << "MHz. Will not check frequency ranges.";
  }
}

const QString &
MD390::name() const {
  return _name;
}

const RadioLimits &
MD390::limits() const {
  return *_limits;
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
  return RadioInfo(RadioInfo::MD390, "md390", "MD-390", "TyT", TyTInterface::interfaceInfo(),
                   QList<RadioInfo>{
                     RadioInfo(RadioInfo::RT8, "rt8", "RT8", "Retevis", TyTInterface::interfaceInfo())
                   });
}
