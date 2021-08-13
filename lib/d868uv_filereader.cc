#include "d868uv_filereader.hh"
#include <QtEndian>

#define HEADER_SIZE              0x000000e2

/* ********************************************************************************************* *
 * Implementation of D868UVFileReader::ChannelElement
 * ********************************************************************************************* */
D868UVFileReader::ChannelElement::ChannelElement(const uint8_t *ptr)
  : Element(ptr), _nameLength(0)
{
  _nameLength = strnlen((char *)_data+0x30,16);
}

uint16_t
D868UVFileReader::ChannelElement::index() const {
  return qFromLittleEndian(*(uint16_t *)_data + 0x00);
}

D868UVCodeplug::channel_t::Mode
D868UVFileReader::ChannelElement::mode() const {
  return (D868UVCodeplug::channel_t::Mode) *(_data + 0x0b);
}

double
D868UVFileReader::ChannelElement::rxFrequency() const {
  return double(qFromLittleEndian(*(uint32_t *)(_data + 0x02)))/1e6;
}

double
D868UVFileReader::ChannelElement::txFrequency() const {
  double rx  = double(qFromLittleEndian(*(uint32_t *)(_data + 0x02)))/1e6;
  double off = double(qFromLittleEndian(*(uint32_t *)(_data + 0x07)))/1e6;
  D868UVCodeplug::channel_t::RepeaterMode repeater_type = (D868UVCodeplug::channel_t::RepeaterMode)*(_data+0x06);

  switch(repeater_type) {
  case D868UVCodeplug::channel_t::RM_TXPOS: return rx+off;
  case D868UVCodeplug::channel_t::RM_TXNEG: return rx-off;
  case D868UVCodeplug::channel_t::RM_SIMPLEX:
  default:
    break;
  }

  return rx;
}

Channel::Power
D868UVFileReader::ChannelElement::power() const {
  D868UVCodeplug::channel_t::Power pwr = (D868UVCodeplug::channel_t::Power) * (_data + 0x0c);
  switch(pwr) {
  case D868UVCodeplug::channel_t::POWER_LOW: return Channel::LowPower;
  case D868UVCodeplug::channel_t::POWER_MIDDLE: return Channel::MidPower;
  case D868UVCodeplug::channel_t::POWER_HIGH: return Channel::HighPower;
  case D868UVCodeplug::channel_t::POWER_TURBO: return Channel::MaxPower;
  default:
    break;
  }
  return Channel::MinPower;
}

AnalogChannel::Bandwidth
D868UVFileReader::ChannelElement::bandwidth() const {
  D868UVCodeplug::channel_t::Bandwidth bw = (D868UVCodeplug::channel_t::Bandwidth) * (_data + 0x0d);
  return (D868UVCodeplug::channel_t::BW_25_KHZ == bw) ? AnalogChannel::BWWide : AnalogChannel::BWNarrow;
}

bool
D868UVFileReader::ChannelElement::rxOnly() const {
  return *(_data + 0x0f);
}

Signaling::Code
D868UVFileReader::ChannelElement::rxSignaling() const {
  ///@todo Implement.
  return Signaling::SIGNALING_NONE;
}

Signaling::Code
D868UVFileReader::ChannelElement::txSignaling() const {
  ///@todo Implement.
  return Signaling::SIGNALING_NONE;
}

DigitalChannel::Admit
D868UVFileReader::ChannelElement::admitDigital() const {
  D868UVCodeplug::channel_t::Admit admit = (D868UVCodeplug::channel_t::Admit) * (_data + 0x20);
  switch (admit) {
  case D868UVCodeplug::channel_t::ADMIT_CH_FREE: return DigitalChannel::AdmitFree;
  case D868UVCodeplug::channel_t::ADMIT_COLORCODE: return DigitalChannel::AdmitColorCode;
  case D868UVCodeplug::channel_t::ADMIT_ALWAYS:
  default:
    break;
  }
  return DigitalChannel::AdmitNone;
}

AnalogChannel::Admit
D868UVFileReader::ChannelElement::admitAnalog() const {
  D868UVCodeplug::channel_t::Admit admit = (D868UVCodeplug::channel_t::Admit) * (_data + 0x20);
  switch (admit) {
  case D868UVCodeplug::channel_t::ADMIT_CH_FREE: return AnalogChannel::AdmitFree;
  case D868UVCodeplug::channel_t::ADMIT_ALWAYS:
  default:
    break;
  }
  return AnalogChannel::AdmitNone;
}

uint8_t
D868UVFileReader::ChannelElement::colorCode() const {
  return *(_data + 0x28);
}

DigitalChannel::TimeSlot
D868UVFileReader::ChannelElement::timeSlot() const {
  return (0==(*(_data + 0x2a))) ? DigitalChannel::TimeSlot1 : DigitalChannel::TimeSlot2;
}

QString
D868UVFileReader::ChannelElement::name() const {
  return QString::fromLocal8Bit((char *)(_data + 0x30), _nameLength);
}

size_t
D868UVFileReader::ChannelElement::size() const {
  return 0x30 + _nameLength+1 + 0x14;
}


/* ********************************************************************************************* *
 * Implementation of D868UVFileReader::RadioIDElement
 * ********************************************************************************************* */
D868UVFileReader::RadioIDElement::RadioIDElement(const uint8_t *ptr)
  : AnytoneFileReader::Element(ptr)
{
  _nameLength = strnlen((char *)(_data+0x05), 16);
}

uint16_t
D868UVFileReader::RadioIDElement::index() const {
  return qFromLittleEndian(* (uint16_t *)(_data + 0x00));
}

uint32_t
D868UVFileReader::RadioIDElement::id() const {
  return qFromLittleEndian(0xffffff00 & (* (uint32_t *)(_data+0x02)));
}

QString
D868UVFileReader::RadioIDElement::name() const {
  return QString::fromLocal8Bit((char *)(_data+5), _nameLength);
}

size_t
D868UVFileReader::RadioIDElement::size() const {
  return 5 + _nameLength+1;
}


/* ********************************************************************************************* *
 * Implementation of D868UVFileReader::ZoneElement
 * ********************************************************************************************* */
D868UVFileReader::ZoneElement::ZoneElement(const uint8_t *ptr)
  : AnytoneFileReader::Element(ptr)
{
  _numChannels = *(_data + 0x01);
  _nameLength = strnlen((char *)(_data+0x02+2*_numChannels), 16);
}

uint8_t
D868UVFileReader::ZoneElement::index() const {
  return *(_data+0x00);
}

uint8_t
D868UVFileReader::ZoneElement::numChannels() const {
  return _numChannels;
}

uint16_t
D868UVFileReader::ZoneElement::channel(uint8_t index) const {
  return qFromLittleEndian(*(uint16_t *)(_data + 0x02 + 2*index));
}

QString
D868UVFileReader::ZoneElement::name() const {
  return QString::fromLocal8Bit((char *)(_data+0x02+2*_numChannels), _nameLength);
}

size_t
D868UVFileReader::ZoneElement::size() const {
  return 0x02 + 2*_numChannels + _nameLength+1;
}


/* ********************************************************************************************* *
 * Implementation of D868UVFileReader::ChannelElement
 * ********************************************************************************************* */
D868UVFileReader::D868UVFileReader(Config *config, const uint8_t *data, size_t size, QString &message)
  : AnytoneFileReader(config, data, size, message)
{
  // pass...
}

bool
D868UVFileReader::readHeader() {
  // Header content is ignored, only advance pointer to end of header
  _data += HEADER_SIZE;
  return true;
}

bool
D868UVFileReader::readChannels() {
  // Read number of channels and advance pointer
  uint16_t numChannels = qFromLittleEndian(*(uint16_t *)_data); _data += 0x02;
  // Read each channel
  for (uint16_t i=0; i<numChannels; i++) {
    if (! this->readChannel())
      return false;
  }
  return true;
}

bool
D868UVFileReader::readChannel() {
  ChannelElement channel(_data);
  // Assemble channel
  Channel *ch = nullptr;
  if (D868UVCodeplug::channel_t::MODE_ANALOG == channel.mode()) {
    ch = new AnalogChannel(
          channel.name(), channel.rxFrequency(), channel.txFrequency(), channel.power(), 0,
          channel.rxOnly(), channel.admitAnalog(), 2, channel.rxSignaling(), channel.txSignaling(),
          channel.bandwidth(), nullptr, nullptr, nullptr);
  } else if (D868UVCodeplug::channel_t::MODE_DIGITAL == channel.mode()) {
    ch = new DigitalChannel(
          channel.name(), channel.rxFrequency(), channel.txFrequency(), channel.power(), 0,
          channel.rxOnly(), channel.admitDigital(), channel.colorCode(), channel.timeSlot(),
          nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
  }
  // Store channel in context & config
  _context.addChannel(ch, channel.index());
  // Advance pointer
  _data += channel.size();
  return true;
}

bool
D868UVFileReader::readRadioIDs() {
  // Read number of radio IDs and advance pointer
  uint16_t numIDs = qFromLittleEndian(*(uint16_t *)_data); _data += 2;
  // Read each radio ID
  for (uint16_t i=0; i<numIDs; i++) {
    if (! this->readRadioID())
      return false;
  }
  return true;
}

bool
D868UVFileReader::readRadioID() {
  RadioIDElement radioid(_data);
  // Assemble radio ID & add to context/config
  _context.addRadioId(radioid.id(), radioid.index());
  // Advance pointer
  _data += radioid.size();
  return true;
}


bool
D868UVFileReader::readZones() {
  // Read number of zones and advance pointer
  uint8_t numZones = *_data; _data += 1;
  // Read each zone
  for (uint8_t i=0; i<numZones; i++) {
    if (! this->readZone())
      return false;
  }
  return true;
}

bool
D868UVFileReader::readZone() {
  ZoneElement zone(_data);
  // Assemble Zone & add to context/config
  Zone *z = new Zone(zone.name());
  _context.config()->zones()->addZone(z);
  // Advance pointer
  _data += zone.size();
  return true;
}

bool
D868UVFileReader::readScanLists() {
  // Read number of scan lists and advance pointer
  uint8_t numScanlists = *_data; _data += 1;
  // Read each scan list
  for (uint8_t i=0; i<numScanlists; i++) {
    if (! this->readScanList())
      return false;
  }
  return true;
}

bool
D868UVFileReader::readScanList() {
  return true;
}
