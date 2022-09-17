#include "openrtx_codeplug.hh"
#include "utils.hh"
#include "logger.hh"
#include "scanlist.hh"
#include "radioid.hh"
#include "contact.hh"
#include "rxgrouplist.hh"
#include "zone.hh"
#include "config.hh"
#include "config.h"
#include <QtEndian>

QVector<unsigned int> _openrtx_ctcss_tone_table{
    670, 693, 719, 744, 770, 797, 825, 854, 885, 915, 948, 974, 1000, 1034,
    1072, 1109, 1148, 1188, 1230, 1273, 1318, 1365, 1413, 1462, 1514, 1567,
    1598, 1622, 1655, 1679, 1713, 1738, 1773, 1799, 1835, 1862, 1899, 1928,
    1966, 1995, 2035, 2065, 2107, 2181, 2257, 2291, 2336, 2418, 2503, 2541
};


/* ********************************************************************************************* *
 * Implementation of OpenRTXCodeplug::HeaderElement
 * ********************************************************************************************* */
OpenRTXCodeplug::HeaderElement::HeaderElement(uint8_t *ptr, size_t size)
  : Codeplug::Element(ptr, size)
{
  // pass...
}

OpenRTXCodeplug::HeaderElement::HeaderElement(uint8_t *ptr)
  : Codeplug::Element(ptr, 0x0058)
{
  // pass...
}

void
OpenRTXCodeplug::HeaderElement::clear() {
  memset(_data, 0, _size);
  setUInt64_le(OffsetMagic, MagicNumber);
  setVersion();
  setTimestamp();
}

bool
OpenRTXCodeplug::HeaderElement::isValid() const {
  return Codeplug::Element::isValid() && (MagicNumber == getUInt64_le(OffsetMagic));
}

uint16_t
OpenRTXCodeplug::HeaderElement::version() const {
  return getUInt16_le(OffsetVersion);
}

void
OpenRTXCodeplug::HeaderElement::setVersion() {
  setUInt16_le(OffsetVersion, SupportedVersion);
}


QString
OpenRTXCodeplug::HeaderElement::author() const {
  return readASCII(OffsetAuthor, StringLength, 0);
}

void
OpenRTXCodeplug::HeaderElement::setAuthor(const QString &name) {
  writeASCII(OffsetAuthor, name, StringLength, 0);
}

QString
OpenRTXCodeplug::HeaderElement::description() const {
  return readASCII(OffsetDescription, StringLength, 0);
}

void
OpenRTXCodeplug::HeaderElement::setDescription(const QString description) {
  writeASCII(OffsetDescription, description, StringLength, 0);
}


QDateTime
OpenRTXCodeplug::HeaderElement::timestamp() const {
  return QDateTime::fromSecsSinceEpoch(getUInt64_le(OffsetTimestamp), Qt::UTC);
}

void
OpenRTXCodeplug::HeaderElement::setTimestamp(const QDateTime timestamp) {
  setUInt64_le(OffsetTimestamp, timestamp.toUTC().toSecsSinceEpoch());
}


unsigned int
OpenRTXCodeplug::HeaderElement::contactCount() const {
  return getUInt16_le(OffsetContactCount);
}

void
OpenRTXCodeplug::HeaderElement::setContactCount(unsigned int n) {
  setUInt16_le(OffsetContactCount, n);
}

unsigned int
OpenRTXCodeplug::HeaderElement::channelCount() const {
  return getUInt16_le(OffsetChannelCount);
}

void
OpenRTXCodeplug::HeaderElement::setChannelCount(unsigned int n) {
  setUInt16_le(OffsetChannelCount, n);
}

unsigned int
OpenRTXCodeplug::HeaderElement::zoneCount() const {
  return getUInt16_le(OffsetZoneCount);
}

void
OpenRTXCodeplug::HeaderElement::setZoneCount(unsigned int n) {
  return setUInt16_le(OffsetZoneCount, n);
}


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


OpenRTXCodeplug::Mode
OpenRTXCodeplug::ChannelElement::mode() const {
  return (OpenRTXCodeplug::Mode)getUInt8(OffsetMode);
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


DMRChannel::TimeSlot
OpenRTXCodeplug::ChannelElement::timeslot() const {
  if (1 == getUInt8(OffsetTimeSlot))
    return DMRChannel::TimeSlot::TS1;
  return DMRChannel::TimeSlot::TS2;
}

void
OpenRTXCodeplug::ChannelElement::setTimeslot(DMRChannel::TimeSlot ts) {
  if (DMRChannel::TimeSlot::TS1 == ts)
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
  Q_UNUSED(ctx)

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
    FMChannel *an = new FMChannel(); ch = an;
    switch(bandwidth()) {
    case BW_12_5kHz: an->setBandwidth(FMChannel::Bandwidth::Narrow); break;
    case BW_20kHz:
    case BW_25kHz: an->setBandwidth(FMChannel::Bandwidth::Wide); break;
    }
    an->setRXTone(rxTone());
    an->setTXTone(txTone());
  } else if (Mode_DMR == mode()) {
    DMRChannel *dmr = new DMRChannel(); ch = dmr;
    dmr->setAdmit(DMRChannel::Admit::ColorCode);
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

  return ch;
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
    DMRChannel *dmr = c->as<DMRChannel>();

    // Link group list, if set
    /*if (hasGroupListIndex()) {
      if (! ctx.has<RXGroupList>(groupListIndex())) {
        errMsg(err) << "Cannot link group list index " << groupListIndex()
                    << " for channel '" << c->name() << "': Index not found.";
        return false;
      }
      dmr->setGroupListObj(ctx.get<RXGroupList>(groupListIndex()));
    }*/

    // Link contact, if set
    if (hasDMRContactIndex()) {
      if (! ctx.has<DMRContact>(dmrContactIndex())) {
        errMsg(err) << "Cannot link DMR contact index " << dmrContactIndex()
                    << " for channel '" << c->name() << "': Index not found.";
        return false;
      }
      dmr->setTXContactObj(ctx.get<DMRContact>(dmrContactIndex()));
    }
  }

  // Link scan list, if set
  /*if (hasScanListIndex()) {
    if (! ctx.has<ScanList>(scanListIndex())) {
      errMsg(err) << "Cannot link scan list index " << scanListIndex()
                  << " for channel '" << c->name() << "': Index not found.";
      return false;
    }
    c->setScanList(ctx.get<ScanList>(scanListIndex()));
  }*/

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

  if (c->is<FMChannel>()) {
    const FMChannel *fm = c->as<FMChannel>();
    setMode(Mode_FM);
    setRXTone(fm->rxTone(), err);
    setTXTone(fm->txTone(), err);
  } else if (c->is<DMRChannel>()) {
    const DMRChannel *dmr = c->as<DMRChannel>();
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
  : Element(ptr, 0x0027)
{
  // pass...
}

OpenRTXCodeplug::ContactElement::~ContactElement() {
  // pass...
}

void
OpenRTXCodeplug::ContactElement::clear() {
  memset(_data, 0, _size);
}

bool
OpenRTXCodeplug::ContactElement::isValid() const {
  return Codeplug::Element::isValid() && ( (Mode_M17 == mode()) || (Mode_DMR == mode()) );
}


QString
OpenRTXCodeplug::ContactElement::name() const {
  return readASCII(OffsetName, StringLength, 0);
}

void
OpenRTXCodeplug::ContactElement::setName(const QString &name) {
  writeASCII(OffsetName, name, StringLength, 0);
}


OpenRTXCodeplug::Mode
OpenRTXCodeplug::ContactElement::mode() const {
  return (Mode) getUInt8(OffsetMode);
}

void
OpenRTXCodeplug::ContactElement::setMode(Mode mode) {
  setUInt8(OffsetMode, mode);
}


unsigned int
OpenRTXCodeplug::ContactElement::dmrId() const {
  return getUInt32_le(OffsetDMRId);
}

void
OpenRTXCodeplug::ContactElement::setDMRId(unsigned int id) {
  setUInt32_le(OffsetDMRId, id);
}


bool
OpenRTXCodeplug::ContactElement::dmrRing() const {
  return getBit(OffsetDMRRing, BitDMRRing);
}

void
OpenRTXCodeplug::ContactElement::enableDMRRing(bool enable) {
  return setBit(OffsetDMRRing, BitDMRRing, enable);
}


DMRContact::Type
OpenRTXCodeplug::ContactElement::dmrContactType() const {
  // This is not specified yet?!?
  return (DMRContact::Type)getUInt2(OffsetDMRCallType, BitDMRCallType);
}
void
OpenRTXCodeplug::ContactElement::setDMRContactType(DMRContact::Type type) {
  setUInt2(OffsetDMRCallType, BitDMRCallType, type);
}


QString
OpenRTXCodeplug::ContactElement::m17Call() const {
  static const char charMap[] = "xABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-/.";
  uint64_t encoded; memcpy(((uint8_t *)&encoded)+2, _data+OffsetM17Address, 6);
  QString result;
  while (encoded) {
    result.append(charMap[encoded%40]);
    encoded /= 40;
  }
  return result;
}

bool
OpenRTXCodeplug::ContactElement::setM17Call(const QString &call, const ErrorStack &err) {
  if (call.size() > 9) {
    errMsg(err) << "Cannot encode calls longer than 9 chars.";
    return false;
  }
  QString C = call.toUpper();

  uint64_t encoded = 0;
  for (QString::const_reverse_iterator it=C.rbegin(); it!=C.rend(); it++) {
    encoded *= 40;
    char c = QChar(*it).toLatin1();
    if (('A' <= c) && ('Z' >= c))
      encoded += (c-'A')+1;
    else if (('0' <= c) && ('9' >= c))
      encoded += (c-'0')+27;
    else if ('-' == c)
      encoded += 37;
    else if ('/' == c)
      encoded += 38;
    else if ('.' == c)
      encoded += 39;
    else {
      errMsg(err) << "Invalid char '" << *it << "' for an M17 call [A-Z,0-9,-,/,.].";
      return false;
    }
  }
  memcpy(_data+OffsetM17Address, ((uint8_t*)&encoded)+2, 6);
  return true;
}

DigitalContact *OpenRTXCodeplug::ContactElement::toContactObj(Context &ctx, const ErrorStack &err) const {
  Q_UNUSED(ctx)

  if (! isValid()) {
    errMsg(err) << "Cannot decode invalid contacts.";
    return nullptr;
  }

  if (Mode_DMR == mode()) {
    DMRContact *contact = new DMRContact();

    contact->setName(name());
    contact->setRing(dmrRing());
    contact->setNumber(dmrId());
    contact->setType(dmrContactType());

    return contact;
  } else if (Mode_M17 == mode()) {
    M17Contact *contact = new M17Contact();

    contact->setName(name());
    contact->setRing(dmrRing());
    contact->setCall(m17Call());

    return contact;
  }

  return nullptr;
}

bool
OpenRTXCodeplug::ContactElement::fromContactObj(const DigitalContact *cont, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(err);

  setName(cont->name());
  enableDMRRing(cont->ring());

  if (cont->is<DMRContact>()) {
    const DMRContact *dmr = cont->as<DMRContact>();
    setMode(Mode_DMR);
    setDMRId(dmr->number());
    setDMRContactType(dmr->type());
  } else if (cont->is<M17Contact>()) {
    const M17Contact *m17 = cont->as<M17Contact>();
    setM17Call(m17->call());
  } else {
    errMsg(err) << "Cannot encode contact '" << cont->name() << "': Contact type '"
                << cont->metaObject()->className() << "' not supported by OpenRTX firmware.";
    return false;
  }

  return true;
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
  : Element(ptr, 0x0022)
{
  _size = 0x22 + channelCount()*4;
}

OpenRTXCodeplug::ZoneElement::~ZoneElement() {
  // pass...
}

void
OpenRTXCodeplug::ZoneElement::clear() {
  memset(_data, 0, 0x22);
  setChannelCount(0);
}

bool
OpenRTXCodeplug::ZoneElement::isValid() const {
  return Codeplug::Element::isValid();
}


QString
OpenRTXCodeplug::ZoneElement::name() const {
  return readASCII(OffsetName, StringLength, 0);
}

void
OpenRTXCodeplug::ZoneElement::setName(const QString &name) {
  writeASCII(OffsetName, name, StringLength, 0);
}


unsigned int
OpenRTXCodeplug::ZoneElement::channelCount() const {
  return getUInt16_le(OffsetCount);
}

void
OpenRTXCodeplug::ZoneElement::setChannelCount(unsigned int n) {
  setUInt16_le(OffsetCount, n);
  _size = 0x22 + sizeof(uint32_t)*n;
}


unsigned int
OpenRTXCodeplug::ZoneElement::channelIndex(unsigned int n) const {
  return getUInt32_le(OffsetChannel + sizeof(uint32_t)*n);
}

void
OpenRTXCodeplug::ZoneElement::setChannelIndex(unsigned int n, unsigned int idx) {
  setUInt32_le(OffsetChannel + sizeof(uint32_t)*n, idx);
}


Zone *
OpenRTXCodeplug::ZoneElement::toZoneObj(Context &ctx) const {
  Q_UNUSED(ctx)
  Zone *zone = new Zone();
  zone->setName(name());
  return zone;
}

bool
OpenRTXCodeplug::ZoneElement::linkZoneObj(Zone *zone, Context &ctx, bool putInB, const ErrorStack &err) const {
  for (unsigned int i=0; i<channelCount(); i++) {
    if (! ctx.has<Channel>(channelIndex(i))) {
      errMsg(err) << "Cannot link zone '" << zone->name() << "': Channel index " << channelIndex(i)
                  << " not known.";
      return false;
    }
    if (putInB)
      zone->B()->add(ctx.get<Channel>(channelIndex(i)));
    else
      zone->A()->add(ctx.get<Channel>(channelIndex(i)));
  }
  return true;
}

void
OpenRTXCodeplug::ZoneElement::fromZoneObjA(const Zone *zone, Context &ctx) {
  if (zone->B()->count())
    setName(zone->name()+" A");
  else
    setName(zone->name());
  setChannelCount(zone->A()->count());
  for (int i=0; i<zone->A()->count(); i++) {
    setChannelIndex(i, ctx.index(zone->A()->get(i)));
  }
}

void
OpenRTXCodeplug::ZoneElement::fromZoneObjB(const Zone *zone, Context &ctx) {
  setName(zone->name() + " B");
  setChannelCount(zone->B()->count());
  for (int i=0; i<zone->B()->count(); i++) {
    setChannelIndex(i, ctx.index(zone->B()->get(i)));
  }
}


/* ********************************************************************************************* *
 * Implementation of OpenRTXCodeplug
 * ********************************************************************************************* */
OpenRTXCodeplug::OpenRTXCodeplug(QObject *parent)
  : Codeplug(parent)
{
  addImage("OpenRTX codeplug v0.1");
  image(0).addElement(0x0000, HeaderSize);
}

OpenRTXCodeplug::~OpenRTXCodeplug() {
  // pass...
}

void
OpenRTXCodeplug::clear() {
  remImage(0);
  addImage("OpenRTX codeplug v0.1");
  image(0).addElement(0x0000, HeaderSize);
}

bool
OpenRTXCodeplug::index(Config *config, Context &ctx, const ErrorStack &err) const {
  Q_UNUSED(err)

  // All indices as 1-based. That is, the first channel gets index 1 etc.

  // Map DMR contacts
  for (int i=0, c=0; i<config->contacts()->count(); i++) {
    Contact *contact = config->contacts()->contact(i);
    if (contact->is<DMRContact>() || contact->is<M17Contact>()) {
      ctx.add(contact, c+1); c++;
    } else {
      logInfo() << "Cannot index contact '" << contact->name()
                << "'. Contact type '" << contact->metaObject()->className()
                << "' not supported by or implemented for OpenRTX devices.";
    }
  }

  // Map channels
  for (int i=0; i<config->channelList()->count(); i++) {
    Channel *channel = config->channelList()->channel(i);
    ctx.add(channel, i+1);
  }

  // Map zones
  for (int i=0; i<config->zones()->count(); i++)
    ctx.add(config->zones()->zone(i), i+1);

  return true;
}

bool
OpenRTXCodeplug::encode(Config *config, const Flags &flags, const ErrorStack &err) {
  // Check if default DMR id is set.
  if (nullptr == config->radioIDs()->defaultId()) {
    errMsg(err) << "Cannot encode OpenRTX codeplug: No default radio ID specified.";
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
  HeaderElement header(data(0));
  header.clear();
  header.setAuthor(ctx.config()->radioIDs()->defaultId()->name());
  header.setDescription("Encoded by qdmr v" VERSION_STRING);

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

  if (! this->linkChannels(ctx.config(), ctx, err)) {
    errMsg(err) << "Cannot link channels.";
    return false;
  }

  if (! this->linkZones(ctx.config(), ctx, err)) {
    errMsg(err) << "Cannot link zones.";
    return false;
  }

  return true;
}

unsigned int
OpenRTXCodeplug::numContacts() {
  return HeaderElement(data(0x0000)).contactCount();
}

unsigned int
OpenRTXCodeplug::offsetContact(unsigned int n) {
  return HeaderSize + n*ContactSize;
}

bool
OpenRTXCodeplug::encodeContacts(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(config); Q_UNUSED(flags)

  /// @todo Limit number of contacts.
  unsigned int numContacts = ctx.count<DigitalContact>();
  HeaderElement(data(0x0000)).setContactCount(numContacts);
  image(0).addElement(offsetContact(0), numContacts*ContactSize);

  for (unsigned int i=0; i<ctx.count<DigitalContact>(); i++) {
    ContactElement contact(data(offsetContact(i)));
    if (! contact.fromContactObj(ctx.get<DigitalContact>(i+1), ctx, err)) {
      errMsg(err) << "Cannot encode contact '" << ctx.get<DigitalContact>(i+1)->name() <<"'.";
      return false;
    }
  }

  return true;
}

bool
OpenRTXCodeplug::createContacts(Config *config, Context &ctx, const ErrorStack &err) {
  unsigned int numContacts = HeaderElement(data(0x0000)).contactCount();

  for (unsigned int i=0; i<numContacts; i++) {
    DigitalContact *contact = ContactElement(data(offsetContact(i))).toContactObj(ctx, err);
    if (nullptr == contact) {
      errMsg(err) << "Cannot create " << (i+1) << "-th contact.";
      return false;
    }
    config->contacts()->add(contact);
    ctx.add(contact, i+1);
  }

  return true;
}


unsigned int
OpenRTXCodeplug::numChannels() {
  return HeaderElement(data(0x0000)).channelCount();
}

unsigned int
OpenRTXCodeplug::offsetChannel(unsigned int n) {
  unsigned int numContacts = HeaderElement(data(0x0000)).contactCount();
  return HeaderSize + numContacts*ContactSize + n*ChannelSize;
}

bool
OpenRTXCodeplug::encodeChannels(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags)

  /// @todo Limit number of channels.
  unsigned int numChannels = config->channelList()->count();
  HeaderElement(data(0x0000)).setChannelCount(numChannels);
  image(0).addElement(offsetChannel(0), numChannels*ChannelSize);

  for (int i=0; i<config->channelList()->count(); i++) {
    ChannelElement ch(data(offsetChannel(i)));
    if (! ch.fromChannelObj(config->channelList()->channel(i), ctx, err)) {
      errMsg(err) << "Cannot encode " << (i+1) << "-th channel '"
                  << config->channelList()->channel(i)->name() << "'.";
      return false;
    }
    ctx.add(config->channelList()->channel(i), i+1);
  }

  return true;
}

bool
OpenRTXCodeplug::createChannels(Config *config, Context &ctx, const ErrorStack &err) {
  unsigned int numChannels = HeaderElement(data(0x0000)).channelCount();
  unsigned int offsetChannels = offsetChannel(0);
  for (unsigned int i=0; i<numChannels; i++) {
    ChannelElement ch(data(offsetChannels + i*ChannelSize));
    Channel *chObj = ch.toChannelObj(ctx, err);
    if (nullptr == chObj) {
      errMsg(err) << "Cannot decode " << (i+1) << "-th channel.";
      return false;
    }
    config->channelList()->add(chObj);
    ctx.add(chObj, i+1);
  }

  return true;
}

bool
OpenRTXCodeplug::linkChannels(Config *config, Context &ctx, const ErrorStack &err) {
  unsigned int numChannels = HeaderElement(data(0x0000)).channelCount();
  unsigned int offsetChannels = offsetChannel(0);
  for (unsigned int i=0; i<numChannels; i++) {
    ChannelElement ch(data(offsetChannels + i*ChannelSize));
    Channel *chObj = config->channelList()->channel(i);
    if (! ch.linkChannelObj(chObj, ctx, err)) {
      errMsg(err) << "Cannot link " << (i+1) << "-th channel "
                  << chObj->name() << ".";
      return false;
    }
  }

  return true;
}


unsigned int
OpenRTXCodeplug::numZones() {
  return HeaderElement(data(0x0000)).zoneCount();
}

unsigned int
OpenRTXCodeplug::offsetZoneOffsets() {
  HeaderElement header(data(0x0000));
  unsigned int numContacts = header.contactCount();
  unsigned int numChannels = header.channelCount();
  return  HeaderSize + numContacts*ContactSize + numChannels*ChannelSize;
}

unsigned int
OpenRTXCodeplug::offsetZone(unsigned int n) {
  HeaderElement header(data(0x0000));
  unsigned int numContacts = header.contactCount();
  unsigned int numChannels = header.channelCount();
  uint32_t *ptr = (uint32_t *)data(HeaderSize + numContacts*ContactSize + numChannels*ChannelSize
                                   + n*sizeof(uint32_t));
  return qFromLittleEndian(*ptr);
}

bool
OpenRTXCodeplug::encodeZones(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)

  // Count zones (A + B)
  unsigned int zoneCount=0;
  for (int i=0; i<config->zones()->count(); i++) {
    zoneCount++;
    // Check if B contains channels
    if (config->zones()->zone(i)->B()->count())
      zoneCount++;
  }

  // Allocate zone offsets
  HeaderElement(data(0x0000)).setZoneCount(zoneCount);
  image(0).addElement(offsetZoneOffsets(), zoneCount*sizeof(uint32_t));
  uint32_t *offsets = (uint32_t *)data(offsetZoneOffsets());

  // Allocate and encode zones
  uint32_t currentOffset = offsetZoneOffsets() + zoneCount*sizeof(uint32_t);
  for (unsigned int z=0, i=0; i<zoneCount; i++,z++) {
    // Allocate & encode zone A
    unsigned int zoneSize = ZoneHeaderSize+config->zones()->zone(z)->A()->count()*sizeof(uint32_t);
    image(0).addElement(currentOffset, zoneSize);
    offsets[i] = qToLittleEndian(currentOffset);
    ZoneElement(data(currentOffset)).fromZoneObjA(config->zones()->zone(z), ctx);
    currentOffset += zoneSize;
    // Allocate & encode zone B, if not empty
    if (ZoneHeaderSize+config->zones()->zone(z)->B()->count()) {
      i++;
      unsigned int zoneSize = ZoneHeaderSize+config->zones()->zone(z)->B()->count()*sizeof(uint32_t);
      image(0).addElement(currentOffset, zoneSize);
      offsets[i] = qToLittleEndian(currentOffset);
      ZoneElement(data(currentOffset)).fromZoneObjB(config->zones()->zone(z), ctx);
      currentOffset += zoneSize;
    }
  }

  return true;
}

bool
OpenRTXCodeplug::createZones(Config *config, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)
  unsigned int zoneCount = numZones();
  uint32_t *zoneOffsets = (uint32_t *) data(offsetZoneOffsets());

  Zone *last_zone = nullptr;
  for (unsigned int i=0; i<zoneCount; i++) {
    ZoneElement zone(data(qFromLittleEndian(zoneOffsets[i])));
    if (! zone.isValid())
      continue;
    bool is_ext = (nullptr != last_zone) && (zone.name().endsWith(" B")) &&
        (zone.name().startsWith(last_zone->name()));
    Zone *obj = last_zone;
    if (! is_ext) {
      last_zone = obj = new Zone(zone.name());
      if (zone.name().endsWith(" A"))
        obj->setName(zone.name().chopped(2));
      config->zones()->add(obj); ctx.add(obj, i+1);
    }
  }

  return true;
}

bool
OpenRTXCodeplug::linkZones(Config *config, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(config); Q_UNUSED(err)

  unsigned int zoneCount = numZones();
  uint32_t *zoneOffsets = (uint32_t *) data(offsetZoneOffsets());

  Zone *last_zone = nullptr;
  for (unsigned int i=0, z=0; i<zoneCount; i++, z++) {
    ZoneElement zone(data(qFromLittleEndian(zoneOffsets[i])));
    if (! zone.isValid())
      continue;
    if (ctx.has<Zone>(i+1)) {
      Zone *obj = last_zone = ctx.get<Zone>(i+1);
      for (unsigned int i=0; i<zone.channelCount(); i++) {
        if (! ctx.has<Channel>(zone.channelIndex(i))) {
          logWarn() << "Cannot link channel with index " << zone.channelIndex(i)
                    << " channel not defined.";
          continue;
        }
        obj->A()->add(ctx.get<Channel>(zone.channelIndex(i)));
      }
    } else {
      Zone *obj = last_zone; last_zone = nullptr;
      for (unsigned int i=0; i<zone.channelCount(); i++) {
        if (! ctx.has<Channel>(zone.channelIndex(i))) {
          logWarn() << "Cannot link channel with index " << zone.channelIndex(i)
                    << " channel not defined.";
          continue;
        }
        obj->B()->add(ctx.get<Channel>(zone.channelIndex(i)));
      }
    }
  }

  return true;
}
