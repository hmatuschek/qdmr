#include "d868uv_filereader.hh"
#include <QtEndian>

#include "logger.hh"
#include "utils.hh"


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
  return double(qFromLittleEndian(*(uint32_t *)(_data + 0x02)))/1e5;
}

double
D868UVFileReader::ChannelElement::txFrequency() const {
  double rx  = double(qFromLittleEndian(*(uint32_t *)(_data + 0x02)))/1e5;
  double off = double(qFromLittleEndian(*(uint32_t *)(_data + 0x07)))/1e5;
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
  _nameLength = strnlen((char *)(_data+0x04), 16);
}

uint8_t
D868UVFileReader::RadioIDElement::index() const {
  return *(_data + 0x00);
}

uint32_t
D868UVFileReader::RadioIDElement::id() const {
  uint32_t a=_data[1], b=_data[2], c=_data[3];
  return ((c<<16) | (b<<8) | a);
}

QString
D868UVFileReader::RadioIDElement::name() const {
  return QString::fromLocal8Bit((char *)(_data+4), _nameLength);
}

size_t
D868UVFileReader::RadioIDElement::size() const {
  return 4 + _nameLength+1;
}


/* ********************************************************************************************* *
 * Implementation of D868UVFileReader::ZoneElement
 * ********************************************************************************************* */
D868UVFileReader::ZoneElement::ZoneElement(const uint8_t *ptr)
  : AnytoneFileReader::Element(ptr)
{
  _numChannels = *(_data + 0x01);
  _nameLength = strnlen((char *)(_data+0x02 + 2*_numChannels + 2*2), 16);
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
  return QString::fromLocal8Bit((char *)(_data+0x02+2*_numChannels+2*2), _nameLength);
}

size_t
D868UVFileReader::ZoneElement::size() const {
  return 0x02 + 2*_numChannels + 2*2 + _nameLength+1;
}


/* ********************************************************************************************* *
 * Implementation of D868UVFileReader::ScanListElement
 * ********************************************************************************************* */
D868UVFileReader::ScanListElement::ScanListElement(const uint8_t *ptr)
  : AnytoneFileReader::Element(ptr)
{
  _nameLength = strnlen((char *)(_data+0x01), 16);
  _numChannels = *(_data + 0x01 + _nameLength+1 + 0x0b);
}

uint8_t
D868UVFileReader::ScanListElement::index() const {
  return *(_data+0x00);
}

uint8_t
D868UVFileReader::ScanListElement::numChannels() const {
  return _numChannels;
}

D868UVCodeplug::scanlist_t::PriChannel
D868UVFileReader::ScanListElement::prioChannelSelect() const {
  return (D868UVCodeplug::scanlist_t::PriChannel)(* (_data + 0x01 + _nameLength+1 + 0x01));
}

uint16_t
D868UVFileReader::ScanListElement::prioChannel1() const {
  return qFromLittleEndian((uint16_t)(* (_data + 0x01 + _nameLength+1 + 0x02)));
}

uint16_t
D868UVFileReader::ScanListElement::prioChannel2() const {
  return qFromLittleEndian((uint16_t)(* (_data + 0x01 + _nameLength+1 + 0x04)));
}

uint16_t
D868UVFileReader::ScanListElement::channel(uint8_t index) const {
  return qFromLittleEndian(*(uint16_t *)(_data + 0x01 + _nameLength+1 + 0x0b + 0x03 + 2*index));
}

QString
D868UVFileReader::ScanListElement::name() const {
  return QString::fromLocal8Bit((char *)(_data+1), _nameLength);
}

size_t
D868UVFileReader::ScanListElement::size() const {
  return 0x0f + 2*_numChannels + _nameLength+1;
}


/* ********************************************************************************************* *
 * Implementation of D868UVFileReader::AnalogContactElement
 * ********************************************************************************************* */
D868UVFileReader::AnalogContactElement::AnalogContactElement(const uint8_t *ptr)
  : AnytoneFileReader::Element(ptr)
{
  _numberLength = *(_data+1);
  _nameLength = strnlen((char *)(_data+0x02+_numberLength), 16);
}

uint8_t
D868UVFileReader::AnalogContactElement::index() const {
  return *(_data + 0x00);
}

QString
D868UVFileReader::AnalogContactElement::number() const {
  return QString::fromLocal8Bit((char *)(_data+0x02), _numberLength);
}

QString
D868UVFileReader::AnalogContactElement::name() const {
  return QString::fromLocal8Bit((char *)(_data+0x02+_numberLength), _nameLength);
}

size_t
D868UVFileReader::AnalogContactElement::size() const {
  return 0x02 + _numberLength + _nameLength+1;
}


/* ********************************************************************************************* *
 * Implementation of D868UVFileReader
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
D868UVFileReader::linkHeader() {
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
  // Store channel in context & config if not VFO channel
  if ((4000 != channel.index()) && (4001 != channel.index())) {
    logDebug() << QString("0x%1").arg(uint(_data-_start), 8,16,QChar('0'))
               << ": Add channel '" << channel.name() << "'.";
    _context.addChannel(ch, channel.index());
  }
  // Advance pointer
  _data += channel.size();
  return true;
}

bool
D868UVFileReader::linkChannels() {
  // Get number of channels and advance pointer
  uint16_t numChannels = qFromLittleEndian(*(uint16_t *)_data); _data += 0x02;
  // Link each channel
  for (uint16_t i=0; i<numChannels; i++) {
    if (! this->linkChannel())
      return false;
  }
  return true;
}

bool
D868UVFileReader::linkChannel() {
  ChannelElement channel(_data);
  // Advance pointer
  _data += channel.size();
  return true;
}


bool
D868UVFileReader::readRadioIDs() {
  // Read number of radio IDs and advance pointer
  uint8_t numIDs = *_data; _data += 1;
  // Read each radio ID
  for (uint8_t i=0; i<numIDs; i++) {
    if (! this->readRadioID())
      return false;
  }
  return true;
}

bool
D868UVFileReader::readRadioID() {
  RadioIDElement radioid(_data);
  // Assemble radio ID & add to context/config
  if (0 == radioid.index()) {
    _context.setDefaultRadioId(radioid.id(), radioid.index());
    _context.config()->setName(radioid.name());
    logDebug() << QString("0x%1").arg(uint(_data-_start), 8,16,QChar('0'))
               << ": Set default radio ID " << radioid.id() << " ('" << radioid.name() << "').";
  } else {
    _context.addRadioId(radioid.id(), radioid.index());
    logDebug() << QString("0x%1").arg(uint(_data-_start), 8,16,QChar('0'))
               << ": Add radio ID " << radioid.id() << " ('" << radioid.name() << "').";
  }
  // Advance pointer
  _data += radioid.size();
  return true;
}

bool
D868UVFileReader::linkRadioIDs() {
  // Read number of radio IDs and advance pointer
  uint8_t numIDs = *_data; _data += 1;
  // skip each radio ID
  for (uint8_t i=0; i<numIDs; i++) {
    RadioIDElement id(_data); _data += id.size();
  }
  return true;
}

bool
D868UVFileReader::readZones() {
  // Read number of zones and advance pointer
  uint8_t numZones = *_data; _data += 1;
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
  if (! _context.config()->zones()->addZone(z)) {
    _message = QObject::tr("0x%1: Cannot add zone to config.").arg(uint(_data-_start), 8,16,QChar('0'));
    return false;
  }
  for (uint8_t i=0; i<zone.numChannels(); i++) {
    if (! _context.hasChannel(zone.channel(i))) {
      _message = QObject::tr("0x%1: Cannot add channel %2 to zone '%3'.")
          .arg(uint(_data-_start), 8,16,QChar('0')).arg(zone.channel(i)).arg(zone.name());
      return false;
    }
    z->A()->addChannel(_context.getChannel(zone.channel(i)));
    logDebug() << "Add channel '" << _context.getChannel(zone.channel(i))->name()
               << "' to zone '" << zone.name() << "'.";
  }
  logDebug() << QString("0x%1").arg(uint(_data-_start), 8,16,QChar('0'))
             << ": Add zone '" << zone.name() << "'.";
  // Advance pointer
  _data += zone.size();
  return true;
}

bool
D868UVFileReader::linkZones() {
  // Read number of zones and advance pointer
  uint8_t numZones = *_data; _data += 1;
  // Skip each zone, zones are linked during creation
  for (uint8_t i=0; i<numZones; i++) {
    ZoneElement zone(_data); _data += zone.size();
  }
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
  ScanListElement sl(_data);

  ScanList *scanlist = new ScanList(sl.name());
  _context.addScanList(scanlist, sl.index());
  logDebug() << QString("0x%1").arg(uint(_data-_start), 8,16,QChar('0'))
             << ": Add scan list '" << sl.name() << "'.";

  for (uint8_t i=0; i<sl.numChannels(); i++) {
    if (! _context.hasChannel(sl.channel(i))) {
      _message = QObject::tr("0x%1: Cannot add channel %2 to scan list '%3'.")
          .arg(uint(_data-_start), 8,16,QChar('0')).arg(sl.channel(i)).arg(sl.name());
      return false;
    }
    scanlist->addChannel(_context.getChannel(sl.channel(i)));
    logDebug() << "Add channel '" << _context.getChannel(sl.channel(i))->name()
               << "' to zone '" << sl.name() << "'.";
    if ((D868UVCodeplug::scanlist_t::PRIO_CHAN_SEL1 & sl.prioChannelSelect()) &&
        (0 != sl.prioChannel1()))
    {
      if (1 == sl.prioChannel1())
        scanlist->setPriorityChannel(SelectedChannel::get());
      else if (_context.hasChannel(sl.prioChannel1()-2))
        scanlist->setPriorityChannel(_context.getChannel(sl.prioChannel1()-2));
      else
        logWarn() << QString("0x%1").arg(uint(_data-_start), 8,16,QChar('0'))
                  << "Cannot link priority channel index " << (sl.prioChannel1()-2)
                  << " in scanlist '" << sl.name() << "'.";
    }
    if ((D868UVCodeplug::scanlist_t::PRIO_CHAN_SEL2 & sl.prioChannelSelect()) &&
        (0 != sl.prioChannel2()))
    {
      if (1 == sl.prioChannel2())
        scanlist->setSecPriorityChannel(SelectedChannel::get());
      else if (_context.hasChannel(sl.prioChannel2()-2))
        scanlist->setSecPriorityChannel(_context.getChannel(sl.prioChannel2()-2));
      else
        logWarn() << QString("0x%1").arg(uint(_data-_start), 8,16,QChar('0'))
                  << "Cannot link sec. priority channel index " << (sl.prioChannel2()-2)
                  << " in scanlist '" << sl.name() << "'.";
    }
  }
  return true;
}

bool
D868UVFileReader::linkScanLists() {
  // Read number of scan lists and advance pointer
  uint8_t numScanlists = *_data; _data += 1;
  // Skip each scan list, scan lists are linked during creation
  for (uint8_t i=0; i<numScanlists; i++) {
    ScanListElement sl(_data); _data += sl.size();
  }
  return true;
}

bool
D868UVFileReader::readAnalogContacts() {
  // Read number of scan lists and advance pointer
  uint8_t numContacts = *_data; _data += 1;
  // Read analog conact
  for (uint8_t i=0; i<numContacts; i++) {
    this->readAnalogContact();
  }
  return true;
}

bool
D868UVFileReader::readAnalogContact() {
  AnalogContactElement ac(_data);
  if (validDTMFNumber(ac.number())) {
    DTMFContact *contact = new DTMFContact(ac.name(), ac.number());
    _context.addAnalogContact(contact, ac.index());
  } else {
    logWarn() << QString("0x%1").arg(uint(_data-_start), 8,16,QChar('0'))
              << ": Skip analog contact '" << ac.name()
              << ". Invalid DTMF number '" << ac.number() << "'.";
  }
  _data += ac.size();
  return true;
}

bool
D868UVFileReader::linkAnalogContacts() {
  // Read number of scan lists and advance pointer
  uint8_t numContacts = *_data; _data += 1;
  // Skip each analog contact, analog contacts need no linking
  for (uint8_t i=0; i<numContacts; i++) {
    AnalogContactElement elm(_data); _data += elm.size();
  }
  return true;
}
