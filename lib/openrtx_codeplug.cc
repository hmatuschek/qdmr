#include "openrtx_codeplug.hh"
#include "utils.hh"
#include "logger.hh"
#include "scanlist.hh"
#include "radioid.hh"
#include "contact.hh"
#include "rxgrouplist.hh"
#include "zone.hh"
#include "config.hh"

QVector<unsigned int> _openrtx_ctcss_tone_table{
    670, 693, 719, 744, 770, 797, 825, 854, 885, 915, 948, 974, 1000, 1034,
    1072, 1109, 1148, 1188, 1230, 1273, 1318, 1365, 1413, 1462, 1514, 1567,
    1598, 1622, 1655, 1679, 1713, 1738, 1773, 1799, 1835, 1862, 1899, 1928,
    1966, 1995, 2035, 2065, 2107, 2181, 2257, 2291, 2336, 2418, 2503, 2541
};


/* ********************************************************************************************* *
 * Implementation of OpenRTXCodeplug::ChannelElement
 * ********************************************************************************************* */
OpenRTXCodeplug::ChannelElement::ChannelElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

OpenRTXCodeplug::ChannelElement::ChannelElement(uint8_t *ptr)
  : Element(ptr, 0x5a)
{
  // pass...
}

OpenRTXCodeplug::ChannelElement::~ChannelElement() {
  // pass...
}

bool
OpenRTXCodeplug::ChannelElement::isValid() const {
  return Codeplug::Element::isValid() && (Mode_None != mode());
}

void
OpenRTXCodeplug::ChannelElement::clear() {
  memset(_data, 0, _size);
}


OpenRTXCodeplug::ChannelElement::Mode
OpenRTXCodeplug::ChannelElement::mode() const {
  return (OpenRTXCodeplug::ChannelElement::Mode)getUInt8(OffsetMode);
}

void
OpenRTXCodeplug::ChannelElement::setMode(Mode mode) {
  setUInt8(Offsets::OffsetMode, (uint8_t)mode);
}


bool
OpenRTXCodeplug::ChannelElement::rxOnly() const {
  return getBit(OffsetRXOnly, BitRXOnly);
}

void
OpenRTXCodeplug::ChannelElement::setRXOnly(bool enable) {
  setBit(OffsetRXOnly, BitRXOnly, enable);
}


OpenRTXCodeplug::ChannelElement::Bandwidth
OpenRTXCodeplug::ChannelElement::bandwidth() const {
  return (Bandwidth)getUInt2(OffsetBandwidth, BitBandwidth);
}

void
OpenRTXCodeplug::ChannelElement::setBandwidth(Bandwidth bw) {
  setUInt2(OffsetBandwidth, BitBandwidth, bw);
}


float
OpenRTXCodeplug::ChannelElement::power() const {
  return 10.+0.2*getUInt8(OffsetPower);
}

void
OpenRTXCodeplug::ChannelElement::setPower(float dBm) {
  if (dBm < 10)
    dBm = 10;
  setUInt8(OffsetPower, (uint8_t)((dBm-10)*5));
}


double
OpenRTXCodeplug::ChannelElement::rxFrequency() const {
  return ((double)getUInt32_le(OffsetRXFrequency))/1e6;
}

void
OpenRTXCodeplug::ChannelElement::setRXFrequency(double MHz) {
  setUInt32_le(OffsetRXFrequency, (uint32_t)(MHz*1e6));
}

double
OpenRTXCodeplug::ChannelElement::txFrequency() const {
  return ((double)getUInt32_le(OffsetTXFrequency))/1e6;
}

void
OpenRTXCodeplug::ChannelElement::setTXFrequency(double MHz) {
  setUInt32_le(OffsetTXFrequency, (uint32_t)(MHz*1e6));
}


bool
OpenRTXCodeplug::ChannelElement::hasScanListIndex() const {
  return 0 != scanListIndex();
}

unsigned int
OpenRTXCodeplug::ChannelElement::scanListIndex() const {
  return getUInt8(OffsetScanList);
}

void
OpenRTXCodeplug::ChannelElement::setScanListIndex(unsigned int index) {
  setUInt8(OffsetScanList, index);
}

void
OpenRTXCodeplug::ChannelElement::clearScanListIndex() {
  setScanListIndex(0);
}


bool
OpenRTXCodeplug::ChannelElement::hasGroupListIndex() const {
  return 0 != groupListIndex();
}

unsigned int
OpenRTXCodeplug::ChannelElement::groupListIndex() const {
  return getUInt8(OffsetGroupList);
}

void
OpenRTXCodeplug::ChannelElement::setGroupListIndex(unsigned int index) {
  setUInt8(OffsetGroupList, index);
}

void
OpenRTXCodeplug::ChannelElement::clearGroupListIndex() {
  setGroupListIndex(0);
}


QString
OpenRTXCodeplug::ChannelElement::name() const {
  return readASCII(OffsetName, StringLength, 0x00);
}

void
OpenRTXCodeplug::ChannelElement::setName(const QString &name) {
  writeASCII(OffsetName, name, StringLength, 0x00);
}

QString
OpenRTXCodeplug::ChannelElement::description() const {
  return readASCII(OffsetDescription, StringLength, 0x00);
}

void
OpenRTXCodeplug::ChannelElement::setDescription(const QString &description) {
  writeASCII(OffsetDescription, description, StringLength, 0x00);
}


float
OpenRTXCodeplug::ChannelElement::latitude() const {
  return getInt8(OffsetChLatInt) + ((float)getUInt16_le(OffsetChLatDec))/65536;
}

void
OpenRTXCodeplug::ChannelElement::setLatitude(float lat) {
  setInt8(OffsetChLatInt, (int)lat);
  setUInt16_le(OffsetChLatDec, std::abs(lat-((int)lat))*65536);
}

float
OpenRTXCodeplug::ChannelElement::longitude() const {
  return getInt8(OffsetChLonInt) + ((float)getUInt16_le(OffsetChLonDec))/65536;
}

void
OpenRTXCodeplug::ChannelElement::setLongitude(float lon) {
  setInt8(OffsetChLonInt, (int)lon);
  setUInt16_le(OffsetChLonDec, std::abs(lon-((int)lon))*65536);
}

unsigned int
OpenRTXCodeplug::ChannelElement::altitude() const {
  return getUInt16_le(OffsetChAltitude);
}
void
OpenRTXCodeplug::ChannelElement::setAltitude(unsigned int alt) {
  setUInt16_le(OffsetChAltitude, alt);
}


Signaling::Code
OpenRTXCodeplug::ChannelElement::rxTone() const {
  if (! getBit(OffsetRXTone, 0))
    return Signaling::SIGNALING_NONE;
  int idx = getUInt8(OffsetRXTone)>>1;
  if (idx >= _openrtx_ctcss_tone_table.size())
    return Signaling::SIGNALING_NONE;
  return Signaling::fromCTCSSFrequency(float(_openrtx_ctcss_tone_table[idx])/10);
}

void
OpenRTXCodeplug::ChannelElement::setRXTone(Signaling::Code code, const ErrorStack &err) {
  if (Signaling::SIGNALING_NONE == code) {
    setBit(OffsetRXTone, 0, false);
    return;
  }
  if (! Signaling::isCTCSS(code)) {
    errMsg(err) << "Can only encode CTCSS tones.";
    setBit(OffsetRXTone, 0, false);
    return;
  }
  if (! _openrtx_ctcss_tone_table.contains((unsigned int)(Signaling::toCTCSSFrequency(code)*10))) {
    errMsg(err) << "Cannot encode CTCSS frequency " << Signaling::toCTCSSFrequency(code) << "Hz: "
                << "Not supported.";
    setBit(OffsetRXTone, 0, false);
    return;
  }

  uint8_t index = _openrtx_ctcss_tone_table.indexOf(
        (unsigned int)(Signaling::toCTCSSFrequency(code)*10));
  setUInt8(OffsetRXTone, (index<<1)|1);
}

Signaling::Code
OpenRTXCodeplug::ChannelElement::txTone() const {
  if (! getBit(OffsetTXTone, 0))
    return Signaling::SIGNALING_NONE;
  int idx = getUInt8(OffsetTXTone)>>1;
  if (idx >= _openrtx_ctcss_tone_table.size())
    return Signaling::SIGNALING_NONE;
  return Signaling::fromCTCSSFrequency(float(_openrtx_ctcss_tone_table[idx])/10);
}

void
OpenRTXCodeplug::ChannelElement::setTXTone(Signaling::Code code, const ErrorStack &err) {
  if (Signaling::SIGNALING_NONE == code) {
    setBit(OffsetRXTone, 0, false);
    return;
  }
  if (! Signaling::isCTCSS(code)) {
    errMsg(err) << "Can only encode CTCSS tones.";
    setBit(OffsetRXTone, 0, false);
    return;
  }
  if (! _openrtx_ctcss_tone_table.contains((unsigned int)(Signaling::toCTCSSFrequency(code)*10))) {
    errMsg(err) << "Cannot encode CTCSS frequency " << Signaling::toCTCSSFrequency(code) << "Hz: "
                << "Not supported.";
    setBit(OffsetRXTone, 0, false);
    return;
  }

  uint8_t index = _openrtx_ctcss_tone_table.indexOf(
        (unsigned int)(Signaling::toCTCSSFrequency(code)*10));
  setUInt8(OffsetTXTone, (index<<1)|1);
}


unsigned int
OpenRTXCodeplug::ChannelElement::rxColorCode() const {
  return getUInt4(OffsetRXColorCode, BitRXColorCode);
}

void
OpenRTXCodeplug::ChannelElement::setRXColorCode(unsigned int cc) {
  setUInt4(OffsetRXColorCode, BitRXColorCode, cc);
}

unsigned int
OpenRTXCodeplug::ChannelElement::txColorCode() const {
  return getUInt4(OffsetTXColorCode, BitTXColorCode);
}

void
OpenRTXCodeplug::ChannelElement::setTXColorCode(unsigned int cc) {
  setUInt4(OffsetTXColorCode, BitTXColorCode, cc);
}


DigitalChannel::TimeSlot
OpenRTXCodeplug::ChannelElement::timeslot() const {
  if (1 == getUInt8(OffsetTimeSlot))
    return DigitalChannel::TimeSlot::TS1;
  return DigitalChannel::TimeSlot::TS2;
}

void
OpenRTXCodeplug::ChannelElement::setTimeslot(DigitalChannel::TimeSlot ts) {
  if (DigitalChannel::TimeSlot::TS1 == ts)
    setUInt8(OffsetTimeSlot, 1);
  else
    setUInt8(OffsetTimeSlot, 2);
}


bool
OpenRTXCodeplug::ChannelElement::hasDMRContactIndex() const {
  return 0 != dmrContactIndex();
}

unsigned int
OpenRTXCodeplug::ChannelElement::dmrContactIndex() const {
  return getUInt16_le(OffsetDMRContact);
}

void
OpenRTXCodeplug::ChannelElement::setDMRContactIndex(unsigned int idx) {
  setUInt16_le(OffsetDMRContact, idx);
}

void
OpenRTXCodeplug::ChannelElement::clearDMRContactIndex() {
  setDMRContactIndex(0);
}

unsigned int
OpenRTXCodeplug::ChannelElement::rxChannelAccessNumber() const {
  return getUInt4(OffsetRXCAN, BitRXCAN);
}

void
OpenRTXCodeplug::ChannelElement::setRXChannelAccessNumber(unsigned int cc) {
  setUInt4(OffsetRXCAN, BitRXCAN, cc);
}

unsigned int
OpenRTXCodeplug::ChannelElement::txChannelAccessNumber() const {
  return getUInt4(OffsetTXCAN, BitTXCAN);
}

void
OpenRTXCodeplug::ChannelElement::setTXChannelAccessNumber(unsigned int cc) {
  setUInt4(OffsetTXCAN, BitTXCAN, cc);
}


OpenRTXCodeplug::ChannelElement::EncryptionMode
OpenRTXCodeplug::ChannelElement::encryptionMode() const {
  return (EncryptionMode)getUInt4(OffsetEncrMode, BitEncrMode);
}

void
OpenRTXCodeplug::ChannelElement::setEncryptionMode(EncryptionMode mode) {
  return setUInt4(OffsetEncrMode, BitEncrMode, mode);
}


OpenRTXCodeplug::ChannelElement::ChannelMode
OpenRTXCodeplug::ChannelElement::channelMode() const {
  return (ChannelMode) getUInt4(OffsetM17ChMode, BitM17ChMode);
}

void
OpenRTXCodeplug::ChannelElement::setChannelMode(ChannelMode mode) {
  setUInt4(OffsetM17ChMode, BitM17ChMode, mode);
}


bool
OpenRTXCodeplug::ChannelElement::gpsDataEnabled() const {
  return 1 == getUInt8(OffsetM17GPSMode);
}

void
OpenRTXCodeplug::ChannelElement::enableGPSData(bool enable) {
  if (enable)
    setUInt8(OffsetM17GPSMode, 1);
  else
    setUInt8(OffsetM17GPSMode, 0);
}


bool
OpenRTXCodeplug::ChannelElement::hasM17ContactIndex() const {
  return 0 != m17ContactIndex();
}

unsigned int
OpenRTXCodeplug::ChannelElement::m17ContactIndex() const {
  return getUInt16_le(OffsetM17Contact);
}

void
OpenRTXCodeplug::ChannelElement::setM17ContactIndex(unsigned int idx) {
  setUInt16_le(OffsetM17Contact, idx);
}

void
OpenRTXCodeplug::ChannelElement::clearM17ContactIndex() {
  setM17ContactIndex(0);
}


Channel *
OpenRTXCodeplug::ChannelElement::toChannelObj(Codeplug::Context &ctx, const ErrorStack &err) const {
  if (! isValid()) {
    errMsg(err) << "Cannot decode invalid channel.";
    return nullptr;
  }

  if (Mode_M17 == mode()) {
    errMsg(err) << "Cannot decode M17 channel. Not implemented yet.";
    return nullptr;
  }

  Channel *ch = nullptr;
  if (Mode_FM == mode()) {
    AnalogChannel *an = new AnalogChannel(); ch = an;
    switch(bandwidth()) {
    case BW_12_5kHz: an->setBandwidth(AnalogChannel::Bandwidth::Narrow); break;
    case BW_20kHz:
    case BW_25kHz: an->setBandwidth(AnalogChannel::Bandwidth::Wide); break;
    }
    an->setRXTone(rxTone());
    an->setTXTone(txTone());
  } else if (Mode_DMR == mode()) {
    DigitalChannel *dmr = new DigitalChannel(); ch = dmr;
    dmr->setAdmit(DigitalChannel::Admit::ColorCode);
    dmr->setColorCode(rxColorCode());
    dmr->setTimeSlot(timeslot());
  }

  // Common settings
  ch->setName(name());
  ch->setRXOnly(rxOnly());
  if (30 > power()) { // less than 30dBm (1W) min
    ch->setPower(Channel::Power::Min);
  } else if (34 > power()) {
    ch->setPower(Channel::Power::Low);
  } else if (37 > power()) {
    ch->setPower(Channel::Power::Mid);
  } else if (38 > power()) {
    ch->setPower(Channel::Power::High);
  } else {
    ch->setPower(Channel::Power::Max);
  }
  ch->setRXFrequency(rxFrequency());
  ch->setTXFrequency(txFrequency());

  return nullptr;
}

bool
OpenRTXCodeplug::ChannelElement::linkChannelObj(Channel *c, Context &ctx, const ErrorStack &err) const {
  if (! isValid()) {
    errMsg(err) << "Cannot link invalid channel '" << c->name() << "'.";
    return false;
  }

  if (Mode_M17 == mode()) {
    errMsg(err) << "Cannot link M17 channel '" << c->name() << "', not implemented yet.";
    return false;
  } else if (Mode_DMR == mode()) {
    DigitalChannel *dmr = c->as<DigitalChannel>();

    // Link group list, if set
    if (hasGroupListIndex()) {
      if (! ctx.has<RXGroupList>(groupListIndex())) {
        errMsg(err) << "Cannot link group list index " << groupListIndex()
                    << " for channel '" << c->name() << "': Index not found.";
        return false;
      }
      dmr->setGroupListObj(ctx.get<RXGroupList>(groupListIndex()));
    }

    // Link contact, if set
    if (hasDMRContactIndex()) {
      if (! ctx.has<DigitalContact>(dmrContactIndex())) {
        errMsg(err) << "Cannot link DMR contact index " << dmrContactIndex()
                    << " for channel '" << c->name() << "': Index not found.";
        return false;
      }
      dmr->setTXContactObj(ctx.get<DigitalContact>(dmrContactIndex()));
    }
  }

  // Link scan list, if set
  if (hasScanListIndex()) {
    if (! ctx.has<ScanList>(scanListIndex())) {
      errMsg(err) << "Cannot link scan list index " << scanListIndex()
                  << " for channel '" << c->name() << "': Index not found.";
      return false;
    }
    c->setScanList(ctx.get<ScanList>(scanListIndex()));
  }

  return true;
}

bool
OpenRTXCodeplug::ChannelElement::fromChannelObj(const Channel *c, Context &ctx, const ErrorStack &err) {
  clear();

  setName(c->name());
  setRXOnly(c->rxOnly());
  switch (c->power()) {
  case Channel::Power::Min: setPower(27); break;
  case Channel::Power::Low: setPower(30); break;
  case Channel::Power::Mid: setPower(34); break;
  case Channel::Power::High: setPower(37); break;
  case Channel::Power::Max: setPower(38.5); break;
  }
  setRXFrequency(c->rxFrequency());
  setTXFrequency(c->txFrequency());
  if (c->scanListRef()->isNull())
    clearScanListIndex();
  else
    setScanListIndex(ctx.index(c->scanList()));
  clearGroupListIndex();

  if (c->is<AnalogChannel>()) {
    const AnalogChannel *fm = c->as<AnalogChannel>();
    setMode(Mode_FM);
    setRXTone(fm->rxTone(), err);
    setTXTone(fm->txTone(), err);
  } else if (c->is<DigitalChannel>()) {
    const DigitalChannel *dmr = c->as<DigitalChannel>();
    setMode(Mode_DMR);
    if (! dmr->groupList()->isNull())
      setGroupListIndex(ctx.index(dmr->groupListObj()));
    setTXColorCode(dmr->colorCode());
    setRXColorCode(dmr->colorCode());
    setTimeslot(dmr->timeSlot());
    if (! dmr->contact()->isNull())
      setDMRContactIndex(ctx.index(dmr->txContactObj()));
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of OpenRTXCodeplug::ContactElement
 * ********************************************************************************************* */
OpenRTXCodeplug::ContactElement::ContactElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

OpenRTXCodeplug::ContactElement::ContactElement(uint8_t *ptr)
  : Element(ptr, 0x0018)
{
  // pass...
}

OpenRTXCodeplug::ContactElement::~ContactElement() {
  // pass...
}

void
OpenRTXCodeplug::ContactElement::clear() {
}

bool
OpenRTXCodeplug::ContactElement::isValid() const {
  return false;
}

DigitalContact *
OpenRTXCodeplug::ContactElement::toContactObj(Context &ctx) const {
  return nullptr;
}

void
OpenRTXCodeplug::ContactElement::fromContactObj(const DigitalContact *cont, Context &ctx) {
}


/* ********************************************************************************************* *
 * Implementation of OpenRTXCodeplug::ZoneElement
 * ********************************************************************************************* */
OpenRTXCodeplug::ZoneElement::ZoneElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

OpenRTXCodeplug::ZoneElement::ZoneElement(uint8_t *ptr)
  : Element(ptr, 0x0030)
{
  // pass...
}

OpenRTXCodeplug::ZoneElement::~ZoneElement() {
  // pass...
}

void
OpenRTXCodeplug::ZoneElement::clear() {
}
bool
OpenRTXCodeplug::ZoneElement::isValid() const {
  return false;
}

Zone *
OpenRTXCodeplug::ZoneElement::toZoneObj(Context &ctx) const {
  return nullptr;
}

bool
OpenRTXCodeplug::ZoneElement::linkZoneObj(Zone *zone, Context &ctx, bool putInB) const {
  return false;
}

void
OpenRTXCodeplug::ZoneElement::fromZoneObjA(const Zone *zone, Context &ctx) {
}

void
OpenRTXCodeplug::ZoneElement::fromZoneObjB(const Zone *zone, Context &ctx) {
}


/* ********************************************************************************************* *
 * Implementation of OpenRTXCodeplug::GroupListElement
 * ********************************************************************************************* */
OpenRTXCodeplug::GroupListElement::GroupListElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

OpenRTXCodeplug::GroupListElement::GroupListElement(uint8_t *ptr)
  : Element(ptr, 0x0030)
{
  // pass...
}

OpenRTXCodeplug::GroupListElement::~GroupListElement() {
  // pass...
}

void
OpenRTXCodeplug::GroupListElement::clear() {
}

RXGroupList *
OpenRTXCodeplug::GroupListElement::toRXGroupListObj(Context &ctx) {
  return nullptr;
}

bool
OpenRTXCodeplug::GroupListElement::linkRXGroupListObj(int ncnt, RXGroupList *lst, Context &ctx) const {
  return false;
}

void
OpenRTXCodeplug::GroupListElement::fromRXGroupListObj(const RXGroupList *lst, Context &ctx) {
}


/* ********************************************************************************************* *
 * Implementation of OpenRTXCodeplug::ScanListElement
 * ********************************************************************************************* */
OpenRTXCodeplug::ScanListElement::ScanListElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

OpenRTXCodeplug::ScanListElement::ScanListElement(uint8_t *ptr)
  : Element(ptr, 0x0058)
{
  // pass...
}

OpenRTXCodeplug::ScanListElement::~ScanListElement() {
  // pass...
}

void
OpenRTXCodeplug::ScanListElement::clear() {
  // pass...
}

ScanList *
OpenRTXCodeplug::ScanListElement::toScanListObj(Context &ctx) const {
  return nullptr;
}

bool
OpenRTXCodeplug::ScanListElement::linkScanListObj(ScanList *lst, Context &ctx) const {
  return false;
}

void
OpenRTXCodeplug::ScanListElement::fromScanListObj(const ScanList *lst, Context &ctx) {
}


/* ********************************************************************************************* *
 * Implementation of OpenRTXCodeplug::GeneralSettingsElement
 * ********************************************************************************************* */
OpenRTXCodeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pas...
}

OpenRTXCodeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr)
  : Element(ptr, 0x0028)
{
  // pass...
}

OpenRTXCodeplug::GeneralSettingsElement::~GeneralSettingsElement() {
  // pass...
}

void
OpenRTXCodeplug::GeneralSettingsElement::clear() {
}

bool
OpenRTXCodeplug::GeneralSettingsElement::fromConfig(const Config *conf, Context &ctx) {
  return false;
}

bool
OpenRTXCodeplug::GeneralSettingsElement::updateConfig(Config *conf, Context &ctx) {
  return false;
}


/* ********************************************************************************************* *
 * Implementation of OpenRTXCodeplug
 * ********************************************************************************************* */
OpenRTXCodeplug::OpenRTXCodeplug(QObject *parent)
  : Codeplug(parent)
{
  // pass...
}

OpenRTXCodeplug::~OpenRTXCodeplug() {
  // pass...
}

void
OpenRTXCodeplug::clear() {
  // Clear general config
  clearGeneralSettings();
  // Clear contacts
  clearContacts();
  // clear zones
  clearZones();
  // clear scan lists
  clearScanLists();
  // clear group lists
  clearGroupLists();
}

bool
OpenRTXCodeplug::index(Config *config, Context &ctx, const ErrorStack &err) const {
  // All indices as 1-based. That is, the first channel gets index 1.

  // Map radio IDs
  for (int i=0; i<config->radioIDs()->count(); i++)
    ctx.add(config->radioIDs()->getId(i), i+1);

  // Map digital and DTMF contacts
  for (int i=0, d=0, a=0; i<config->contacts()->count(); i++) {
    if (config->contacts()->contact(i)->is<DigitalContact>()) {
      ctx.add(config->contacts()->contact(i)->as<DigitalContact>(), d+1); d++;
    } else if (config->contacts()->contact(i)->is<DTMFContact>()) {
      ctx.add(config->contacts()->contact(i)->as<DTMFContact>(), a+1); a++;
    }
  }

  // Map rx group lists
  for (int i=0; i<config->rxGroupLists()->count(); i++)
    ctx.add(config->rxGroupLists()->list(i), i+1);

  // Map channels
  for (int i=0; i<config->channelList()->count(); i++)
    ctx.add(config->channelList()->channel(i), i+1);

  // Map zones
  for (int i=0; i<config->zones()->count(); i++)
    ctx.add(config->zones()->zone(i), i+1);

  // Map scan lists
  for (int i=0; i<config->scanlists()->count(); i++)
    ctx.add(config->scanlists()->scanlist(i), i+1);

  // Map DMR APRS systems
  for (int i=0,a=0,d=0; i<config->posSystems()->count(); i++) {
    if (config->posSystems()->system(i)->is<GPSSystem>()) {
      ctx.add(config->posSystems()->system(i)->as<GPSSystem>(), d+1); d++;
    } else if (config->posSystems()->system(i)->is<APRSSystem>()) {
      ctx.add(config->posSystems()->system(i)->as<APRSSystem>(), a+1); a++;
    }
  }

  // Map roaming
  for (int i=0; i<config->roaming()->count(); i++)
    ctx.add(config->roaming()->zone(i), i+1);

  return true;
}

bool
OpenRTXCodeplug::encode(Config *config, const Flags &flags, const ErrorStack &err) {
  // Check if default DMR id is set.
  if (nullptr == config->radioIDs()->defaultId()) {
    errMsg(err) << "Cannot encode TyT codeplug: No default radio ID specified.";
    return false;
  }

  // Create index<->object table.
  Context ctx(config);
  if (! index(config, ctx, err))
    return false;

  return this->encodeElements(flags, ctx, err);
}

bool
OpenRTXCodeplug::encodeElements(const Flags &flags, Context &ctx, const ErrorStack &err) {
  // General config
  if (! this->encodeGeneralSettings(ctx.config(), flags, ctx, err)) {
    errMsg(err) << "Cannot encode general settings.";
    return false;
  }

  // Define Contacts
  if (! this->encodeContacts(ctx.config(), flags, ctx, err)) {
    errMsg(err) << "Cannot encode contacts.";
    return false;
  }

  if (! this->encodeChannels(ctx.config(), flags, ctx, err)) {
    errMsg(err) << "Cannot encode channels.";
    return false;
  }

  if (! this->encodeZones(ctx.config(), flags, ctx, err)) {
    errMsg(err) << "Cannot encode zones.";
    return false;
  }

  if (! this->encodeScanLists(ctx.config(), flags, ctx, err)) {
    errMsg(err) << "Cannot encode scan lists.";
    return false;
  }

  if (! this->encodeGroupLists(ctx.config(), flags, ctx, err)) {
    errMsg(err) << "Cannot encode group lists.";
    return false;
  }

  return true;
}

bool
OpenRTXCodeplug::decode(Config *config, const ErrorStack &err) {
  // Clear config object
  config->clear();

  // Create index<->object table.
  Context ctx(config);

  return this->decodeElements(ctx, err);
}

bool
OpenRTXCodeplug::decodeElements(Context &ctx, const ErrorStack &err) {
  if (! this->decodeGeneralSettings(ctx.config(), ctx, err)) {
    errMsg(err) << "Cannot decode general settings.";
    return false;
  }

  if (! this->createContacts(ctx.config(), ctx, err)) {
    errMsg(err) << "Cannot create contacts.";
    return false;
  }

  if (! this->createChannels(ctx.config(), ctx, err)) {
    errMsg(err) << "Cannot create channels.";
    return false;
  }

  if (! this->createZones(ctx.config(), ctx, err)) {
    errMsg(err) << "Cannot create zones.";
    return false;
  }

  if (! this->createScanLists(ctx.config(), ctx, err)) {
    errMsg(err) << "Cannot create scan lists.";
    return false;
  }

  if (! this->createGroupLists(ctx.config(), ctx, err)) {
    errMsg(err) << "Cannot create group lists.";
    return false;
  }

  if (! this->linkChannels(ctx.config(), ctx, err)) {
    errMsg(err) << "Cannot link channels.";
    return false;
  }

  if (! this->linkZones(ctx.config(), ctx, err)) {
    errMsg(err) << "Cannot link zones.";
    return false;
  }

  if (! this->linkScanLists(ctx.config(), ctx, err)) {
    errMsg(err) << "Cannot link scan lists.";
    return false;
  }

  if (! this->linkGroupLists(ctx.config(), ctx, err)) {
    errMsg(err) << "Cannot link group lists.";
    return false;
  }

  return true;
}


void
OpenRTXCodeplug::clearGeneralSettings(const ErrorStack &err) {
}

bool
OpenRTXCodeplug::encodeGeneralSettings(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  return false;
}

bool
OpenRTXCodeplug::decodeGeneralSettings(Config *config, Context &ctx, const ErrorStack &err) {
  return false;
}


void
OpenRTXCodeplug::clearContacts(const ErrorStack &err) {
}

bool
OpenRTXCodeplug::encodeContacts(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  return false;
}

bool
OpenRTXCodeplug::createContacts(Config *config, Context &ctx, const ErrorStack &err) {
  return false;
}


void
OpenRTXCodeplug::clearChannels(const ErrorStack &err) {
}

bool
OpenRTXCodeplug::encodeChannels(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  return false;
}

bool
OpenRTXCodeplug::createChannels(Config *config, Context &ctx, const ErrorStack &err) {
  return false;
}

bool
OpenRTXCodeplug::linkChannels(Config *config, Context &ctx, const ErrorStack &err) {
  return false;
}


void
OpenRTXCodeplug::clearZones(const ErrorStack &err) {
}

bool
OpenRTXCodeplug::encodeZones(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  return false;
}

bool
OpenRTXCodeplug::createZones(Config *config, Context &ctx, const ErrorStack &err) {
  return false;
}

bool
OpenRTXCodeplug::linkZones(Config *config, Context &ctx, const ErrorStack &err) {
  return false;
}


void
OpenRTXCodeplug::clearGroupLists(const ErrorStack &err) {
}

bool
OpenRTXCodeplug::encodeGroupLists(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  return false;
}

bool
OpenRTXCodeplug::createGroupLists(Config *config, Context &ctx, const ErrorStack &err) {
  return false;
}

bool
OpenRTXCodeplug::linkGroupLists(Config *config, Context &ctx, const ErrorStack &err) {
  return false;
}


void
OpenRTXCodeplug::clearScanLists(const ErrorStack &err) {
}

bool
OpenRTXCodeplug::encodeScanLists(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  return false;
}

bool
OpenRTXCodeplug::createScanLists(Config *config, Context &ctx, const ErrorStack &err) {
  return false;
}

bool
OpenRTXCodeplug::linkScanLists(Config *config, Context &ctx, const ErrorStack &err) {
  return false;
}
