#include "dr1801uv_codeplug.hh"
#include "logger.hh"
#include "utils.hh"

#define TOTAL_SIZE 0x0001dd90

#define NUM_CHANNELS                1024
#define ADDR_CHANNEL_ELEMENTS    0x0a660
#define CHANNEL_ELEMENT_SIZE     0x00034
#define ADDR_CHANNEL_NAMES       0x17660
#define CHANNEL_NAME_SIZE        0x00014

#define NUM_CONTACTS                1024
#define ADDR_CONTACT_ELEMENTS    0x04338
#define CONTACT_ELEMENT_SIZE     0x00018

#define NUM_GROUP_LISTS               64
#define ADDR_GROUP_LIST_ELEMENTS 0x1c6e0
#define GROUP_LIST_ELEMENT_SIZE  0x00044
#define GROUP_LIST_MEMBER_COUNT       10

#define NUM_ZONES                    150
#define ADDR_ZONE_ELEMENTS       0x00420
#define ZONE_ELEMENT_SIZE        0x00068

#define ADDR_SETTINGS_ELEMENT    0x003b4
#define SETTINGS_ELEMENT_SIZE    0x00064

/* ******************************************************************************************** *
 * Implementation of DR1801UVCodeplug::ChannelElement
 * ******************************************************************************************** */
DR1801UVCodeplug::ChannelElement::ChannelElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

DR1801UVCodeplug::ChannelElement::ChannelElement(uint8_t *ptr)
  : Element(ptr, CHANNEL_ELEMENT_SIZE)
{
  // pass...
}

bool
DR1801UVCodeplug::ChannelElement::isValid() const {
  return 0xffff != index();
}

void
DR1801UVCodeplug::ChannelElement::clear() {
  memset(_data, 0, _size);
  setIndex(0xffff);
}

uint16_t
DR1801UVCodeplug::ChannelElement::index() const {
  return getUInt16_le(0x0000);
}
void
DR1801UVCodeplug::ChannelElement::setIndex(uint16_t idx) {
  setUInt16_le(0x0000, idx);
}

DR1801UVCodeplug::ChannelElement::Type
DR1801UVCodeplug::ChannelElement::channelType() const {
  return (Type)getUInt8(0x002);
}
void
DR1801UVCodeplug::ChannelElement::setChannelType(Type type) {
  setUInt8(0x0002, (uint8_t)type);
}

Channel::Power
DR1801UVCodeplug::ChannelElement::power() const {
  switch ((Power)getUInt8(0x003)) {
  case Power::Low: return Channel::Power::Low;
  case Power::High: return Channel::Power::High;
  }
  return Channel::Power::Low;
}

void
DR1801UVCodeplug::ChannelElement::setPower(Channel::Power pwr) {
  switch (pwr) {
  case Channel::Power::Min:
  case Channel::Power::Low:
    setUInt8(0x0003, (uint8_t)Power::Low);
    break;
  case Channel::Power::Mid:
  case Channel::Power::High:
  case Channel::Power::Max:
    setUInt8(0x0003, (uint8_t)Power::High);
    break;
  }
}

double
DR1801UVCodeplug::ChannelElement::rxFrequency() const {
  return double(getUInt32_le(0x0004))/1e6;
}
void
DR1801UVCodeplug::ChannelElement::setRXFrequency(double MHz) {
  setUInt32_le(0x0004, MHz*1e6);
}
double
DR1801UVCodeplug::ChannelElement::txFrequency() const {
  return double(getUInt32_le(0x0008))/1e6;
}
void
DR1801UVCodeplug::ChannelElement::setTXFrequency(double MHz) {
  setUInt32_le(0x0008, MHz*1e6);
}

bool
DR1801UVCodeplug::ChannelElement::hasTransmitContact() const {
  return 0 != getUInt16_le(0x000c);
}
unsigned int
DR1801UVCodeplug::ChannelElement::transmitContactIndex() const {
  return getUInt16_le(0x000c)-1;
}
void
DR1801UVCodeplug::ChannelElement::setTransmitContactIndex(unsigned int index) {
  setUInt16_le(0x000c, index+1);
}
void
DR1801UVCodeplug::ChannelElement::clearTransmitContactIndex() {
  setUInt16_le(0x000c, 0);
}

DR1801UVCodeplug::ChannelElement::Admit
DR1801UVCodeplug::ChannelElement::admitCriterion() const {
  return (Admit) getUInt8(0x000e);
}
void
DR1801UVCodeplug::ChannelElement::setAdmitCriterion(Admit admit) {
  setUInt8(0x000e, (uint8_t)admit);
}

unsigned int
DR1801UVCodeplug::ChannelElement::colorCode() const {
  return getUInt8(0x0010);
}
void
DR1801UVCodeplug::ChannelElement::setColorCode(unsigned int cc) {
  setUInt8(0x0010, cc);
}

DMRChannel::TimeSlot
DR1801UVCodeplug::ChannelElement::timeSlot() const {
  switch ((TimeSlot)getUInt8(0x0011)) {
  case TimeSlot::TS1: return DMRChannel::TimeSlot::TS1;
  case TimeSlot::TS2: return DMRChannel::TimeSlot::TS2;
  }
  return DMRChannel::TimeSlot::TS1;
}
void
DR1801UVCodeplug::ChannelElement::setTimeSlot(DMRChannel::TimeSlot ts) {
  switch (ts) {
  case DMRChannel::TimeSlot::TS1: setUInt8(0x0011, (uint8_t)TimeSlot::TS1); break;
  case DMRChannel::TimeSlot::TS2: setUInt8(0x0011, (uint8_t)TimeSlot::TS2); break;
  }
}

bool
DR1801UVCodeplug::ChannelElement::hasEncryptionKey() const {
  return 0 != getUInt8(0x0014);
}
uint8_t
DR1801UVCodeplug::ChannelElement::encryptionKeyIndex() const {
  return getUInt8(0x0014)-1;
}
void
DR1801UVCodeplug::ChannelElement::setEncryptionKeyIndex(uint8_t index) {
  setUInt8(0x0014, index+1);
}
void
DR1801UVCodeplug::ChannelElement::clearEncryptionKeyIndex() {
  setUInt8(0x0014, 0);
}

bool
DR1801UVCodeplug::ChannelElement::dcdm() const {
  return getBit(0x0015, 1);
}
void
DR1801UVCodeplug::ChannelElement::enableDCDM(bool enable) {
  setBit(0x0015, 1, enable);
}
bool
DR1801UVCodeplug::ChannelElement::confirmPrivateCall() const {
  return getBit(0x0015, 0);
}
void
DR1801UVCodeplug::ChannelElement::enablePrivateCallConfirmation(bool enable) {
  setBit(0x0015, 0, enable);
}

DR1801UVCodeplug::ChannelElement::SignalingMode
DR1801UVCodeplug::ChannelElement::signalingMode() const {
  return (SignalingMode) getUInt8(0x0016);
}
void
DR1801UVCodeplug::ChannelElement::setSignalingMode(SignalingMode mode) {
  setUInt8(0x0016, (uint8_t)mode);
}

bool
DR1801UVCodeplug::ChannelElement::hasAlarmSystem() const {
  return 0 != getUInt8(0x0018);
}
uint8_t
DR1801UVCodeplug::ChannelElement::alarmSystemIndex() const {
  return getUInt8(0x0018)-1;
}
void
DR1801UVCodeplug::ChannelElement::setAlarmSystemIndex(uint8_t index) {
  setUInt8(0x0018, index+1);
}
void
DR1801UVCodeplug::ChannelElement::clearAlarmSystemIndex() {
  setUInt8(0x0018, 0);
}

FMChannel::Bandwidth
DR1801UVCodeplug::ChannelElement::bandwidth() const {
  switch ((Bandwidth)getUInt8(0x0019)) {
  case Bandwidth::Narrow: return FMChannel::Bandwidth::Narrow;
  case Bandwidth::Wide: return FMChannel::Bandwidth::Wide;
  }
  return FMChannel::Bandwidth::Narrow;
}
void
DR1801UVCodeplug::ChannelElement::setBandwidth(FMChannel::Bandwidth bw) {
  switch (bw){
  case FMChannel::Bandwidth::Narrow: setUInt8(0x0019, (uint8_t)Bandwidth::Narrow); break;
  case FMChannel::Bandwidth::Wide:   setUInt8(0x0019, (uint8_t)Bandwidth::Wide); break;
  }
}

bool
DR1801UVCodeplug::ChannelElement::autoScanEnabled() const {
  return 0x01 == getUInt8(0x001a);
}
void
DR1801UVCodeplug::ChannelElement::enableAutoScan(bool enable) {
  setUInt8(0x001a, enable ? 0x01 : 0x00);
}
bool
DR1801UVCodeplug::ChannelElement::hasScanList() const {
  return 0x00 != getUInt8(0x001b);
}
uint8_t
DR1801UVCodeplug::ChannelElement::scanListIndex() const {
  return getUInt8(0x001b)-1;
}
void
DR1801UVCodeplug::ChannelElement::setScanListIndex(uint8_t index) {
  setUInt8(0x001b, index+1);
}
void
DR1801UVCodeplug::ChannelElement::clearScanListIndex() {
  setUInt8(0x001b, 0);
}

Signaling::Code
DR1801UVCodeplug::ChannelElement::rxTone() const {
  uint16_t ctcss_dcs = getUInt16_le(0x001c);
  SubToneType type = (SubToneType)getUInt8(0x001e);
  DCSMode dcsMode = (DCSMode)getUInt8(0x001f);
  switch (type) {
  case SubToneType::None: return Signaling::SIGNALING_NONE;
  case SubToneType::CTCSS: return Signaling::fromCTCSSFrequency(float(ctcss_dcs)/10);
  case SubToneType::DCS: return Signaling::fromDCSNumber(ctcss_dcs, DCSMode::Inverted == dcsMode);
  }
  return Signaling::SIGNALING_NONE;
}
void
DR1801UVCodeplug::ChannelElement::setRXTone(Signaling::Code code) {
  uint16_t ctcss_dcs = 0;
  SubToneType type = SubToneType::None;
  DCSMode dcsMode = DCSMode::Normal;
  if (Signaling::isCTCSS(code)) {
    type = SubToneType::CTCSS;
    ctcss_dcs = Signaling::toCTCSSFrequency(code)*10;
  } else if (Signaling::isDCSNormal(code) || Signaling::isDCSInverted(code)) {
    type = SubToneType::DCS;
    ctcss_dcs = Signaling::toDCSNumber(code);
    dcsMode = Signaling::isDCSInverted(code) ? DCSMode::Inverted : DCSMode::Normal;
  }
  setUInt16_le(0x001c, ctcss_dcs);
  setUInt8(0x001e, (uint8_t)type);
  setUInt8(0x001f, (uint8_t)dcsMode);
}

Signaling::Code
DR1801UVCodeplug::ChannelElement::txTone() const {
  uint16_t ctcss_dcs = getUInt16_le(0x0020);
  SubToneType type = (SubToneType)getUInt8(0x0022);
  DCSMode dcsMode = (DCSMode)getUInt8(0x0023);
  switch (type) {
  case SubToneType::None: return Signaling::SIGNALING_NONE;
  case SubToneType::CTCSS: return Signaling::fromCTCSSFrequency(float(ctcss_dcs)/10);
  case SubToneType::DCS: return Signaling::fromDCSNumber(ctcss_dcs, DCSMode::Inverted == dcsMode);
  }
  return Signaling::SIGNALING_NONE;
}
void
DR1801UVCodeplug::ChannelElement::setTXTone(Signaling::Code code) {
  uint16_t ctcss_dcs = 0;
  SubToneType type = SubToneType::None;
  DCSMode dcsMode = DCSMode::Normal;
  if (Signaling::isCTCSS(code)) {
    type = SubToneType::CTCSS;
    ctcss_dcs = Signaling::toCTCSSFrequency(code)*10;
  } else if (Signaling::isDCSNormal(code) || Signaling::isDCSInverted(code)) {
    type = SubToneType::DCS;
    ctcss_dcs = Signaling::toDCSNumber(code);
    dcsMode = Signaling::isDCSInverted(code) ? DCSMode::Inverted : DCSMode::Normal;
  }
  setUInt16_le(0x0020, ctcss_dcs);
  setUInt8(0x0022, (uint8_t)type);
  setUInt8(0x0023, (uint8_t)dcsMode);
}

bool
DR1801UVCodeplug::ChannelElement::talkaround() const {
  return getBit(0x0025, 7);
}
void
DR1801UVCodeplug::ChannelElement::enableTalkaround(bool enable) {
  setBit(0x0025, 7, enable);
}

bool
DR1801UVCodeplug::ChannelElement::hasPTTID() const {
  return 0 != getUInt8(0x0028);
}
uint8_t
DR1801UVCodeplug::ChannelElement::pttIDIndex() const {
  return getUInt8(0x0028)-1;
}
void
DR1801UVCodeplug::ChannelElement::setPTTIDIndex(uint8_t idx) {
  setUInt8(0x0028, idx+1);
}
void
DR1801UVCodeplug::ChannelElement::clearPTTID() {
  setUInt8(0x0028, 0);
}

bool
DR1801UVCodeplug::ChannelElement::hasGroupList() const {
  return 0 != getUInt8(0x002a);
}
uint8_t
DR1801UVCodeplug::ChannelElement::groupListIndex() const {
  return getUInt8(0x002a)-1;
}
void
DR1801UVCodeplug::ChannelElement::setGroupListIndex(uint8_t index) {
  setUInt8(0x002a, index+1);
}
void
DR1801UVCodeplug::ChannelElement::clearGroupListIndex() {
  setUInt8(0x002a, 0);
}

bool
DR1801UVCodeplug::ChannelElement::loneWorker() const {
  return 0x01 == getUInt8(0x002f);
}
void
DR1801UVCodeplug::ChannelElement::enableLoneWorker(bool enable) {
  setUInt8(0x002f, enable ? 0x01 : 0x00);
}


Channel *
DR1801UVCodeplug::ChannelElement::toChannelObj(Context &ctx, const ErrorStack &err) const {
  Q_UNUSED(ctx);

  Channel *ch = nullptr;
  if (Type::FM == channelType()) {
    FMChannel *fm = new FMChannel(); ch = fm;
    switch (admitCriterion()) {
    case Admit::Always: fm->setAdmit(FMChannel::Admit::Always); break;
    case Admit::ColorCode_or_Tone: fm->setAdmit(FMChannel::Admit::Tone); break;
    case Admit::ChannelFree: fm->setAdmit(FMChannel::Admit::Free); break;
    }
    fm->setBandwidth(bandwidth());
    fm->setRXTone(rxTone());
    fm->setTXTone(txTone());
  } else if (Type::DMR == channelType()) {
    DMRChannel *dmr = new DMRChannel(); ch = dmr;
    switch (admitCriterion()) {
    case Admit::Always: dmr->setAdmit(DMRChannel::Admit::Always); break;
    case Admit::ColorCode_or_Tone: dmr->setAdmit(DMRChannel::Admit::ColorCode); break;
    case Admit::ChannelFree: dmr->setAdmit(DMRChannel::Admit::Free); break;
    }
    dmr->setColorCode(colorCode());
    dmr->setTimeSlot(timeSlot());
  } else {
    errMsg(err) <<  "Unknonwn channel type " << (uint8_t)channelType() << ".";
    return nullptr;
  }

  ch->setPower(power());
  ch->setRXFrequency(rxFrequency());
  ch->setTXFrequency(txFrequency());

  return ch;
}

bool
DR1801UVCodeplug::ChannelElement::linkChannelObj(Channel *channel, Context &ctx, const ErrorStack &err) const {

  // Common references
  if (hasScanList()) {
    if (! ctx.has<ScanList>(scanListIndex())) {
      errMsg(err) << "Scanlist with index " << scanListIndex() << " not known.";
      return false;
    }
    channel->setScanList(ctx.get<ScanList>(scanListIndex()));
  }

  // Handle DMR specifc references
  if (DMRChannel *dmr = channel->as<DMRChannel>()) {
    if (hasTransmitContact()) {
      if (! ctx.has<DMRContact>(transmitContactIndex())) {
        errMsg(err) << "DMR contact with index " << transmitContactIndex() << " not known.";
        return false;
      }
      dmr->setTXContactObj(ctx.get<DMRContact>(transmitContactIndex()));
    }
    if (hasGroupList()) {
      if (! ctx.has<RXGroupList>(groupListIndex())) {
        errMsg(err) << "Group list with index " << groupListIndex() << " not known.";
        return false;
      }
      dmr->setGroupListObj(ctx.get<RXGroupList>(groupListIndex()));
    }
  }

  return true;
}



/* ******************************************************************************************** *
 * Implementation of DR1801UVCodeplug::ContactElement
 * ******************************************************************************************** */
DR1801UVCodeplug::ContactElement::ContactElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

DR1801UVCodeplug::ContactElement::ContactElement(uint8_t *ptr)
  : Element(ptr, CONTACT_ELEMENT_SIZE)
{
  // pass...
}

bool
DR1801UVCodeplug::ContactElement::isValid() const {
  return 0 != getUInt8(0x0002) &&  // has name
      0 != getUInt24_le(0x0004) && // has DMR ID
      0 != getUInt8(0x0007);       // has call type
}

void
DR1801UVCodeplug::ContactElement::clear() {
  memset(_data, 0, _size);
}

bool
DR1801UVCodeplug::ContactElement::hasSuccessor() const {
  return 0x0000 != getUInt16_le(0x0000);
}
uint16_t
DR1801UVCodeplug::ContactElement::successorIndex() const {
  return getUInt16_le(0x0000)-1;
}
void
DR1801UVCodeplug::ContactElement::setSuccessorIndex(uint16_t index) {
  setUInt16_le(0x0000, index+1);
}
void
DR1801UVCodeplug::ContactElement::clearSuccessorIndex() {
  setUInt16_le(0x0000, 0);
}

uint32_t
DR1801UVCodeplug::ContactElement::dmrID() const {
  if (DMRContact::AllCall == type())
    return 0xffffff;
  return getUInt24_le(0x0004);
}
void
DR1801UVCodeplug::ContactElement::setDMRID(uint32_t id) {
  setUInt24_le(0x0004, id);
}

DMRContact::Type
DR1801UVCodeplug::ContactElement::type() const {
  switch ((CallType)getUInt8(0x0007)) {
  case CallType::AllCall: return DMRContact::AllCall;
  case CallType::PrivateCall: return DMRContact::PrivateCall;
  case CallType::GroupCall: return DMRContact::GroupCall;
  }
  return DMRContact::PrivateCall;
}
void
DR1801UVCodeplug::ContactElement::setCallType(DMRContact::Type type) {
  switch (type) {
  case DMRContact::AllCall:
    setUInt8(0x0007, (uint8_t)CallType::AllCall);
    setDMRID(0xffffff);
    break;
  case DMRContact::PrivateCall: setUInt8(0x0007, (uint8_t)CallType::PrivateCall); break;
  case DMRContact::GroupCall: setUInt8(0x0007, (uint8_t)CallType::GroupCall); break;
  }
}

QString
DR1801UVCodeplug::ContactElement::name() const {
  return readASCII(0x0008, getUInt8(0x0002), 0x00);
}
void
DR1801UVCodeplug::ContactElement::setName(const QString &name) {
  uint8_t len = std::min(16, name.size());
  setUInt8(0x0002, len);
  writeASCII(0x0008, name, 16, 0x00);
}

DMRContact *
DR1801UVCodeplug::ContactElement::toContactObj(Context &ctx, const ErrorStack &err) const {
  Q_UNUSED(ctx); Q_UNUSED(err)
  return new DMRContact(type(), name(), dmrID());
}
bool
DR1801UVCodeplug::ContactElement::linkContactObj(DMRContact *contact, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(contact); Q_UNUSED(ctx); Q_UNUSED(err);
  return true;
}



/* ******************************************************************************************** *
 * Implementation of DR1801UVCodeplug::GroupListElement
 * ******************************************************************************************** */
DR1801UVCodeplug::GroupListElement::GroupListElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

DR1801UVCodeplug::GroupListElement::GroupListElement(uint8_t *ptr)
  : Element(ptr, GROUP_LIST_ELEMENT_SIZE)
{
  // pass...
}

void
DR1801UVCodeplug::GroupListElement::clear() {
  memset(_data, 0, _size);
}

bool
DR1801UVCodeplug::GroupListElement::isValid() const {
  return 0 != getUInt16_le(0x0002);
}

uint16_t
DR1801UVCodeplug::GroupListElement::index() const {
  return getUInt16_le(0x0002)-1;
}
void
DR1801UVCodeplug::GroupListElement::setIndex(uint16_t index) {
  setUInt16_le(0x0002, index-1);
}

uint16_t
DR1801UVCodeplug::GroupListElement::count() const {
  return getUInt16_le(0x0000);
}
bool
DR1801UVCodeplug::GroupListElement::hasMemberIndex(uint8_t n) const {
  return 0 != getUInt16_le(0x0004 + n*0x02);
}
uint16_t
DR1801UVCodeplug::GroupListElement::memberIndex(uint8_t n) const {
  return getUInt16_le(0x0004 + n*0x02) - 1;
}
void
DR1801UVCodeplug::GroupListElement::setMemberIndex(uint8_t n, uint16_t index) {
  setUInt16_le(0x0004 + n*0x02, index+1);
}
void
DR1801UVCodeplug::GroupListElement::clearMemberIndex(uint8_t n) {
  setUInt16_le(0x0004 + n*0x02, 0);
}

RXGroupList *
DR1801UVCodeplug::GroupListElement::toGroupListObj(Context &ctx, const ErrorStack &err) const {
  Q_UNUSED(ctx); Q_UNUSED(err);
  // Simply derive name from index
  return new RXGroupList(QString("Group List %1").arg(index()+1));
}

bool
DR1801UVCodeplug::GroupListElement::linkGroupListObj(RXGroupList *list, Context &ctx, const ErrorStack &err) {
  if (! isValid())
    return false;

  for (int i=0; i<GROUP_LIST_MEMBER_COUNT; i++) {
    if (! hasMemberIndex(i))
      continue;
    if (! ctx.has<DMRContact>(memberIndex(i))) {
      errMsg(err) << "Member index " << memberIndex(i) << " is not known.";
      return false;
    }
    list->addContact(ctx.get<DMRContact>(memberIndex(i)));
  }

  return true;
}


/* ******************************************************************************************** *
 * Implementation of DR1801UVCodeplug::ZoneElement
 * ******************************************************************************************** */
DR1801UVCodeplug::ZoneElement::ZoneElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

DR1801UVCodeplug::ZoneElement::ZoneElement(uint8_t *ptr)
  : Element(ptr, ZONE_ELEMENT_SIZE)
{
  // pass...
}

void
DR1801UVCodeplug::ZoneElement::clear() {
  memset(_data, 0, _size);
}

bool
DR1801UVCodeplug::ZoneElement::isValid() const {
  return 0 != getUInt16_le(0x0024);
}

QString
DR1801UVCodeplug::ZoneElement::name() const {
  uint8_t n = getUInt8(0x0020);
  return readASCII(0x0000, n, 0x00);
}
void
DR1801UVCodeplug::ZoneElement::setName(const QString &name) {
  uint8_t n = std::min(32, name.length());
  setUInt8(0x0020, n);
  writeASCII(0x0000, name, 32, 0x00);
}

unsigned int
DR1801UVCodeplug::ZoneElement::index() const {
  return getUInt16_le(0x0024)-1;
}
void
DR1801UVCodeplug::ZoneElement::setIndex(unsigned int index) {
  setUInt16_le(0x0024, index+1);
}

unsigned int
DR1801UVCodeplug::ZoneElement::numEntries() const {
  return getUInt8(0x0022);
}
unsigned int
DR1801UVCodeplug::ZoneElement::entryIndex(unsigned int n) {
  n = std::min(32U, n);
  return getUInt16_le(0x0028+2*n);
}
void
DR1801UVCodeplug::ZoneElement::setEntryIndex(unsigned int n, unsigned int index) {
  n = std::min(32U, n);
  setUInt16_le(0x0028 + 2*n, index);
}

Zone *
DR1801UVCodeplug::ZoneElement::toZoneObj(Context &ctx, const ErrorStack &err) {
  if (! isValid()) {
    errMsg(err) << "Cannot create zone from invalid zone element.";
    return nullptr;
  }

  return new Zone(name());
}

bool
DR1801UVCodeplug::ZoneElement::linkZoneObj(Zone *obj, Context &ctx, const ErrorStack &err) {
  if (! isValid()) {
    errMsg(err) << "Cannot link zone using invalid zone element.";
    return false;
  }

  for (unsigned int i=0; i<numEntries(); i++) {
    if (! ctx.has<Channel>(entryIndex(i))) {
      errMsg(err) << "Channel with index " << entryIndex(i) << " not defined.";
      return false;
    }
    obj->A()->add(ctx.get<Channel>(entryIndex(i)));
  }

  return true;
}



/* ******************************************************************************************** *
 * Implementation of DR1801UVCodeplug::SettingsElement
 * ******************************************************************************************** */
DR1801UVCodeplug::SettingsElement::SettingsElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

DR1801UVCodeplug::SettingsElement::SettingsElement(uint8_t *ptr)
  : Element(ptr, SETTINGS_ELEMENT_SIZE)
{
  // psas...
}

void
DR1801UVCodeplug::SettingsElement::clear() {
  memset(_data, 0, _size);
}

unsigned int
DR1801UVCodeplug::SettingsElement::dmrID() const {
  return getUInt24_le(0x0000);
}
void
DR1801UVCodeplug::SettingsElement::setDMRID(unsigned int id) {
  setUInt24_le(0x0000, id);
}

DR1801UVCodeplug::SettingsElement::PowerSaveMode
DR1801UVCodeplug::SettingsElement::powerSaveMode() const {
  if (0x00 == getUInt8(0x0008)) {
    // Power save disabled
    return PowerSaveMode::Off;
  }
  return (PowerSaveMode)getUInt8(0x0009);
}
void
DR1801UVCodeplug::SettingsElement::setPowerSaveMode(PowerSaveMode mode) {
  setUInt8(0x0008, PowerSaveMode::Off != mode ? 0x01 : 0x00);
  setUInt8(0x0009, (uint8_t) mode);
}

unsigned int
DR1801UVCodeplug::SettingsElement::voxSensitivity() const {
  if (0x00 == getUInt8(0x0030)) {
    return 0;
  }
  return getUInt8(0x000a)*10/3;
}
void
DR1801UVCodeplug::SettingsElement::setVOXSensitivity(unsigned int sens) {
  if (0 == sens) {
    setUInt8(0x0030, 0x00);
  } else {
    setUInt8(0x0030, 0x01);
    setUInt8(0x000a, 1+sens*2/10);
  }
}
unsigned int
DR1801UVCodeplug::SettingsElement::voxDelay() const {
  return getUInt8(0x000c)*500;
}
void
DR1801UVCodeplug::SettingsElement::setVOXDelay(unsigned int ms) {
  setUInt8(0x000c, ms/500);
}

bool
DR1801UVCodeplug::SettingsElement::encryptionEnabled() const {
  return 0x01 == getUInt8(0x000d);
}
void
DR1801UVCodeplug::SettingsElement::enableEncryption(bool enable) {
  setUInt8(0x000d, enable ? 0x01 : 0x00);
}

bool
DR1801UVCodeplug::SettingsElement::keyLockEnabled() const {
  return 0x01 == getUInt8(0x0017);
}
void
DR1801UVCodeplug::SettingsElement::enableKeyLock(bool enable) {
  setUInt8(0x0017, enable ? 0x01 : 0x00);
}
unsigned int
DR1801UVCodeplug::SettingsElement::keyLockDelay() const {
  return getUInt8(0x000e);
}
void
DR1801UVCodeplug::SettingsElement::setKeyLockDelay(unsigned int sec) {
  setUInt8(0x000e, sec);
}
bool
DR1801UVCodeplug::SettingsElement::lockSideKey1() const {
  return getBit(0x000f, 1);
}
void
DR1801UVCodeplug::SettingsElement::enableLockSideKey1(bool enable) {
  setBit(0x000f, 1, enable);
}
bool
DR1801UVCodeplug::SettingsElement::lockSideKey2() const {
  return getBit(0x000f, 2);
}
void
DR1801UVCodeplug::SettingsElement::enableLockSideKey2(bool enable) {
  setBit(0x000f, 2, enable);
}
bool
DR1801UVCodeplug::SettingsElement::lockPTT() const {
  return getBit(0x000f, 0);
}
void
DR1801UVCodeplug::SettingsElement::enableLockPTT(bool enable) {
  setBit(0x000f, 0, enable);
}

DR1801UVCodeplug::SettingsElement::Language
DR1801UVCodeplug::SettingsElement::language() const {
  return (Language) getUInt8(0x0010);
}
void
DR1801UVCodeplug::SettingsElement::setLanguage(Language lang) {
  setUInt8(0x0010, (uint8_t)lang);
}

DR1801UVCodeplug::SettingsElement::SquelchMode
DR1801UVCodeplug::SettingsElement::squelchMode() const {
  return (SquelchMode) getUInt8(0x0011);
}
void
DR1801UVCodeplug::SettingsElement::setSquelchMode(SquelchMode mode) {
  setUInt8(0x0011, (uint8_t)mode);
}

bool
DR1801UVCodeplug::SettingsElement::rogerTonesEnabled() const {
  return 0x01 == getUInt8(0x0013);
}
void
DR1801UVCodeplug::SettingsElement::enableRogerTones(bool enable) {
  setUInt8(0x0013, enable ? 0x01 : 0x00);
}
bool
DR1801UVCodeplug::SettingsElement::dmrCallOutToneEnabled() const {
  return getBit(0x0028, 1);
}
void
DR1801UVCodeplug::SettingsElement::enableDMRCallOutTone(bool enable) {
  setBit(0x0028, 1, enable);
}
bool
DR1801UVCodeplug::SettingsElement::fmCallOutToneEnabled() const {
  return getBit(0x0028, 2);
}
void
DR1801UVCodeplug::SettingsElement::enableFMCallOutTone(bool enable) {
  setBit(0x0028, 2, enable);
}
bool
DR1801UVCodeplug::SettingsElement::dmrVoiceEndToneEnabled() const {
  return getBit(0x0028, 3);
}
void
DR1801UVCodeplug::SettingsElement::enableDMRVoiceEndTone(bool enable) {
  setBit(0x0028, 3, enable);
}
bool
DR1801UVCodeplug::SettingsElement::fmVoiceEndToneEnabled() const {
  return getBit(0x0028, 4);
}
void
DR1801UVCodeplug::SettingsElement::enableFMVoiceEndTone(bool enable) {
  setBit(0x0028, 4, enable);
}
bool
DR1801UVCodeplug::SettingsElement::dmrCallEndToneEnabled() const {
  return getBit(0x0028, 5);
}
void
DR1801UVCodeplug::SettingsElement::enableDMRCallEndTone(bool enable) {
  setBit(0x0028, 5, enable);
}
bool
DR1801UVCodeplug::SettingsElement::messageToneEnabled() const {
  return getBit(0x0028, 6);
}
void
DR1801UVCodeplug::SettingsElement::enableMessageTone(bool enable) {
  setBit(0x0028, 6, enable);
}

DR1801UVCodeplug::SettingsElement::RingTone
DR1801UVCodeplug::SettingsElement::ringTone() const {
  return (RingTone) getUInt8(0x0016);
}
void
DR1801UVCodeplug::SettingsElement::setRingTone(RingTone tone) {
  setUInt8(0x0016, (uint8_t)tone);
}

QString
DR1801UVCodeplug::SettingsElement::radioName() const {
  return readASCII(0x0018, 16, 0x00);
}
void
DR1801UVCodeplug::SettingsElement::setRadioName(const QString &name) {
  writeASCII(0x0018, name, 16, 0x00);
}

float
DR1801UVCodeplug::SettingsElement::reverseBurstFrequency() const {
  return float(getUInt16_le(0x002c))/10;
}
void
DR1801UVCodeplug::SettingsElement::setReverseBurstFrequency(float Hz) {
  setUInt16_le(0x002c, Hz*10);
}

DR1801UVCodeplug::SettingsElement::BacklightTime
DR1801UVCodeplug::SettingsElement::backlightTime() const {
  return (BacklightTime) getUInt8(0x002f);
}
void
DR1801UVCodeplug::SettingsElement::setBacklightTime(BacklightTime time) {
  setUInt8(0x002f, (uint8_t)time);
}

bool
DR1801UVCodeplug::SettingsElement::campandingEnabled() const {
  return 0x01 == getUInt8(0x0032);
}
void
DR1801UVCodeplug::SettingsElement::enableCampanding(bool enable) {
  setUInt8(0x0032, enable ? 0x01 : 0x00);
}

DR1801UVCodeplug::SettingsElement::TuningMode
DR1801UVCodeplug::SettingsElement::tunigModeUp() const {
  return (TuningMode) getUInt8(0x0036);
}
void
DR1801UVCodeplug::SettingsElement::setTuningModeUp(TuningMode mode) {
  setUInt8(0x0036, (uint8_t)mode);
}
DR1801UVCodeplug::SettingsElement::TuningMode
DR1801UVCodeplug::SettingsElement::tunigModeDown() const {
  return (TuningMode) getUInt8(0x0037);
}
void
DR1801UVCodeplug::SettingsElement::setTuningModeDown(TuningMode mode) {
  setUInt8(0x0037, (uint8_t)mode);
}

DR1801UVCodeplug::SettingsElement::DisplayMode
DR1801UVCodeplug::SettingsElement::displayMode() const {
  return (DisplayMode)getUInt8(0x003c);
}
void
DR1801UVCodeplug::SettingsElement::setDisplayMode(DisplayMode mode) {
  setUInt8(0x003c, (uint8_t)mode);
}

DR1801UVCodeplug::SettingsElement::DualWatchMode
DR1801UVCodeplug::SettingsElement::dualWatchMode() const {
  return (DualWatchMode) getUInt8(0x003d);
}
void
DR1801UVCodeplug::SettingsElement::setDualWatchMode(DualWatchMode mode) {
  setUInt8(0x003d, (uint8_t)mode);
}

DR1801UVCodeplug::SettingsElement::ScanMode
DR1801UVCodeplug::SettingsElement::scanMode() const {
  return (ScanMode) getUInt8(0x003e);
}
void
DR1801UVCodeplug::SettingsElement::setScanMode(ScanMode mode) {
  setUInt8(0x003e, (uint8_t)mode);
}

DR1801UVCodeplug::SettingsElement::BootScreen
DR1801UVCodeplug::SettingsElement::bootScreen() const {
  return (BootScreen) getUInt8(0x003f);
}
void
DR1801UVCodeplug::SettingsElement::setBootScreen(BootScreen mode) {
  setUInt8(0x003f, (uint8_t) mode);
}

QString
DR1801UVCodeplug::SettingsElement::bootLine1() const {
  return readASCII(0x0040, 8, 0x00);
}
void
DR1801UVCodeplug::SettingsElement::setBootLine1(const QString &line) {
  writeASCII(0x0040, line, 8, 0x00);
}
QString
DR1801UVCodeplug::SettingsElement::bootLine2() const {
  return readASCII(0x0048, 8, 0x00);
}
void
DR1801UVCodeplug::SettingsElement::setBootLine2(const QString &line) {
  writeASCII(0x0048, line, 8, 0x00);
}

bool
DR1801UVCodeplug::SettingsElement::ledEnabled() const {
  return 0x01 == getUInt8(0x0050);
}
void
DR1801UVCodeplug::SettingsElement::enableLED(bool enabled) {
  setUInt8(0x0050, enabled ? 0x01 : 0x00);
}

unsigned int
DR1801UVCodeplug::SettingsElement::loneWorkerResponseTime() const {
  return getUInt8(0x0051);
}
void
DR1801UVCodeplug::SettingsElement::setLoneWorkerResponseTime(unsigned int sec) {
  setUInt8(0x0051, sec);
}
unsigned int
DR1801UVCodeplug::SettingsElement::loneWorkerReminderTime() const {
  return getUInt8(0x005c);
}
void
DR1801UVCodeplug::SettingsElement::setLoneWorkerReminderTime(unsigned int sec) {
  setUInt8(0x005c, sec);
}

bool
DR1801UVCodeplug::SettingsElement::bootPasswordEnabled() const {
  return getBit(0x0052, 1);
}
QString
DR1801UVCodeplug::SettingsElement::bootPassword() const {
  return readASCII(0x005e, 6, 0x00);
}
void
DR1801UVCodeplug::SettingsElement::setBootPassword(const QString &passwd) {
  setBit(0x0052, 1, true);
  setUInt8(0x005d, std::min(6, passwd.length()));
  writeASCII(0x005e, passwd, 6, 0x00);
}
void
DR1801UVCodeplug::SettingsElement::clearBootPassword() {
  setBit(0x0052, 1, false);
  setUInt8(0x005d, 0);
  writeASCII(0x005e, "", 6, 0x00);
}

bool
DR1801UVCodeplug::SettingsElement::progPasswordEnabled() const {
  return getBit(0x0052, 0);
}
QString
DR1801UVCodeplug::SettingsElement::progPassword() const {
  return readASCII(0x0054, 6, 0x00);
}
void
DR1801UVCodeplug::SettingsElement::setProgPassword(const QString &passwd) {
  setBit(0x0052, 0, true);
  setUInt8(0x0053, std::min(6, passwd.length()));
  writeASCII(0x0054, passwd, 6, 0x00);
}
void
DR1801UVCodeplug::SettingsElement::clearProgPassword() {
  setBit(0x0052, 0, false);
  setUInt8(0x0053, 0);
  writeASCII(0x0054, "", 6, 0x00);
}

bool
DR1801UVCodeplug::SettingsElement::updateConfig(Config *config, const ErrorStack &err) {
  Q_UNUSED(err);

  // Store radio ID
  int idx = config->radioIDs()->add(new DMRRadioID(radioName(), dmrID()));
  config->radioIDs()->setDefaultId(idx);

  // Handle VOX settings.
  config->settings()->setVOX(voxSensitivity());

  // Handle intro lines
  config->settings()->setIntroLine1(bootLine1());
  config->settings()->setIntroLine2(bootLine2());

  return true;
}


/* ******************************************************************************************** *
 * Implementation of DR1801UVCodeplug
 * ******************************************************************************************** */
DR1801UVCodeplug::DR1801UVCodeplug(QObject *parent)
  : Codeplug(parent)
{
  addImage("BTECH DR-1801UV Codeplug");
  image(0).addElement(0, TOTAL_SIZE);
}

bool
DR1801UVCodeplug::index(Config *config, Context &ctx, const ErrorStack &err) const {
  return false;
}

bool
DR1801UVCodeplug::encode(Config *config, const Flags &flags, const ErrorStack &err) {
  return false;
}

bool
DR1801UVCodeplug::decode(Config *config, const ErrorStack &err) {
  Context ctx(config);

  if (! decodeElements(ctx, err)) {
    errMsg(err) << "Cannot decode elements.";
    return false;
  }

  if (! linkElements(ctx, err)) {
    errMsg(err) << "Cannot decode elements.";
    return false;
  }

  return true;
}

bool
DR1801UVCodeplug::decodeElements(Context &ctx, const ErrorStack &err) {
  if (! decodeChannels(ctx, err)) {
    errMsg(err) << "Cannot decode channel elements.";
    return false;
  }

  if (! decodeContacts(ctx, err)) {
    errMsg(err) << "Cannot decode contact elements.";
    return false;
  }

  if (! decodeGroupLists(ctx, err)) {
    errMsg(err) << "Cannot decode group list elements.";
    return false;
  }

  if (! decodeZones(ctx, err)) {
    errMsg(err) << "Cannot decode zone elements.";
    return false;
  }

  if (! decodeSettings(ctx, err)) {
    errMsg(err) << "Cannot decode settings element.";
    return false;
  }

  return true;
}

bool
DR1801UVCodeplug::linkElements(Context &ctx, const ErrorStack &err) {
  if (! linkChannels(ctx, err)) {
    errMsg(err) << "Cannot link channels.";
    return false;
  }

  if (! linkContacts(ctx, err)) {
    errMsg(err) << "Cannot link contacts.";
    return false;
  }

  if (! linkGroupLists(ctx, err)) {
    errMsg(err) << "Cannot link group lists.";
    return false;
  }

  if (! linkZones(ctx, err)) {
    errMsg(err) << "Cannot link zones.";
    return false;
  }

  return true;
}


bool
DR1801UVCodeplug::decodeChannels(Context &ctx, const ErrorStack &err) {
  // Decode all channel elements
  for (int i=0; i<NUM_CHANNELS; i++) {
    ChannelElement ch(data(ADDR_CHANNEL_ELEMENTS + i*CHANNEL_ELEMENT_SIZE));

    // Skip invalid channels
    if (! ch.isValid())
      continue;

    // Decode channel settings
    Channel *obj = ch.toChannelObj(ctx, err);
    if (nullptr == obj) {
      errMsg(err) << "Cannot decode channel at index " << i << ".";
      return false;
    }

    // Decode channel name
    uint8_t *name_ptr = data(ADDR_CHANNEL_NAMES + i*CHANNEL_NAME_SIZE);
    obj->setName(decode_ascii(name_ptr, CHANNEL_NAME_SIZE, 0x00));

    // Add channel to index table
    ctx.add(obj, ch.index());
    // Add channel to config
    ctx.config()->channelList()->add(obj);
  }

  return true;
}

bool
DR1801UVCodeplug::linkChannels(Context &ctx, const ErrorStack &err) {
  // link all channel elements
  for (int i=0; i<NUM_CHANNELS; i++) {
    ChannelElement ch(data(ADDR_CHANNEL_ELEMENTS + i*CHANNEL_ELEMENT_SIZE));

    // Skip invalid channels
    if (! ch.isValid())
      continue;

    // Link channel if defined
    if (! ctx.has<Channel>(ch.index()))
      continue;

    // Decode channel settings
    if (! ch.linkChannelObj(ctx.get<Channel>(ch.index()), ctx, err)) {
      errMsg(err) << "Cannot link channel '" << ctx.get<Channel>(ch.index())->name()
                  << " at index " << i << ".";
      return false;
    }
  }

  return true;
}


bool
DR1801UVCodeplug::decodeContacts(Context &ctx, const ErrorStack &err) {
  // Decode all contact elements
  for (int i=0; i<NUM_CONTACTS; i++) {
    ContactElement con(data(ADDR_CONTACT_ELEMENTS + i*CONTACT_ELEMENT_SIZE));

    // Skip invalid contacts
    if (! con.isValid())
      continue;

    // Decode contact
    DMRContact *obj = con.toContactObj(ctx, err);
    if (nullptr == obj) {
      errMsg(err) << "Cannot decode contact at index " << i << ".";
      return false;
    }

    // Add contact to index table
    ctx.add(obj, i);
    // Add contact to config
    ctx.config()->contacts()->add(obj);
  }

  return true;
}

bool
DR1801UVCodeplug::linkContacts(Context &ctx, const ErrorStack &err) {
  // link all contact elements
  for (int i=0; i<NUM_CONTACTS; i++) {
    ContactElement con(data(ADDR_CONTACT_ELEMENTS + i*CONTACT_ELEMENT_SIZE));

    // Skip invalid contacts
    if (! con.isValid())
      continue;

    // Link contact if defined
    if (! ctx.has<DMRContact>(i))
      continue;

    // Link contact
    if (! con.linkContactObj(ctx.get<DMRContact>(i), ctx, err)) {
      errMsg(err) << "Cannot link contact '" << ctx.get<Contact>(i)->name()
                  << " at index " << i << ".";
      return false;
    }
  }

  return true;
}


bool
DR1801UVCodeplug::decodeGroupLists(Context &ctx, const ErrorStack &err) {
  // Decode all group list elements
  for (int i=0; i<NUM_GROUP_LISTS; i++) {
    GroupListElement gl(data(ADDR_GROUP_LIST_ELEMENTS + i*GROUP_LIST_ELEMENT_SIZE));

    // Skip invalid group lists
    if (! gl.isValid())
      continue;

    // Decode group list
    RXGroupList *obj = gl.toGroupListObj(ctx, err);
    if (nullptr == obj) {
      errMsg(err) << "Cannot decode group list at index " << i << ".";
      return false;
    }
    logDebug() << "Register group list at index " << gl.index();

    // Add group list to index table
    ctx.add(obj, gl.index());
    // Add group list to config
    ctx.config()->rxGroupLists()->add(obj);
  }

  return true;
}

bool
DR1801UVCodeplug::linkGroupLists(Context &ctx, const ErrorStack &err) {
  // link all group lists
  for (int i=0; i<NUM_GROUP_LISTS; i++) {
    GroupListElement gl(data(ADDR_GROUP_LIST_ELEMENTS + i*GROUP_LIST_ELEMENT_SIZE));

    // Skip invalid group lists
    if (! gl.isValid())
      continue;

    // Link group list if defined
    if (! ctx.has<RXGroupList>(gl.index()))
      continue;

    // Link contact
    if (! gl.linkGroupListObj(ctx.get<RXGroupList>(i), ctx, err)) {
      errMsg(err) << "Cannot link group list '" << ctx.get<RXGroupList>(i)->name()
                  << " at index " << i << ".";
      return false;
    }
  }

  return true;
}


bool
DR1801UVCodeplug::decodeZones(Context &ctx, const ErrorStack &err) {
  // Decode all zone elements
  for (int i=0; i<NUM_ZONES; i++) {
    ZoneElement zone(data(ADDR_ZONE_ELEMENTS + i*ZONE_ELEMENT_SIZE));

    // Skip invalid zones
    if (! zone.isValid())
      continue;

    // Decode zone
    Zone *obj = zone.toZoneObj(ctx, err);
    if (nullptr == obj) {
      errMsg(err) << "Cannot decode zone at index " << i << ".";
      return false;
    }

    // Add zone to index table
    ctx.add(obj, zone.index());
    // Add zone to config
    ctx.config()->zones()->add(obj);
  }

  return true;
}

bool
DR1801UVCodeplug::linkZones(Context &ctx, const ErrorStack &err) {
  // link all zones
  for (int i=0; i<NUM_ZONES; i++) {
    ZoneElement zone(data(ADDR_ZONE_ELEMENTS + i*ZONE_ELEMENT_SIZE));

    // Skip invalid zones
    if (! zone.isValid())
      continue;

    // Link zone if defined
    if (! ctx.has<Zone>(zone.index()))
      continue;

    // Link contact
    if (! zone.linkZoneObj(ctx.get<Zone>(i), ctx, err)) {
      errMsg(err) << "Cannot link zone '" << ctx.get<Zone>(i)->name()
                  << " at index " << i << ".";
      return false;
    }
  }

  return true;
}

bool
DR1801UVCodeplug::decodeSettings(Context &ctx, const ErrorStack &err) {
  return SettingsElement(data(ADDR_SETTINGS_ELEMENT)).updateConfig(ctx.config(), err);
}