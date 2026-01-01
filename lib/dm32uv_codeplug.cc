#include "dm32uv_codeplug.hh"
#include "intermediaterepresentation.hh"
#include "config.hh"
#include "logger.hh"
#include <QRegularExpression>



/* ******************************************************************************************** *
 * Implementation of DM32UVCodeplug::ChannelElement
 * ******************************************************************************************** */
DM32UVCodeplug::ChannelElement::ChannelElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

DM32UVCodeplug::ChannelElement::ChannelElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}


QString
DM32UVCodeplug::ChannelElement::name() const {
  return readASCII(Offset::name(), Limit::nameLength(), 0x00);
}

void
DM32UVCodeplug::ChannelElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::nameLength(), 0x00);
}


Frequency
DM32UVCodeplug::ChannelElement::rxFrequency() const {
  return Frequency::fromHz(getBCD8_le(Offset::rxFrequency())*10);
}

void
DM32UVCodeplug::ChannelElement::setRXFrequency(const Frequency &freq) {
  setBCD8_le(Offset::rxFrequency(), freq.inHz()/10);
}


bool
DM32UVCodeplug::ChannelElement::validTXFrequency() const {
  return std::numeric_limits<uint32_t>::max() != getUInt32_le(Offset::txFrequency());
}

Frequency
DM32UVCodeplug::ChannelElement::txFrequency() const {
  if (validTXFrequency())
    return Frequency::fromHz(getBCD8_le(Offset::txFrequency())*10);
  return Frequency();
}

void
DM32UVCodeplug::ChannelElement::setTXFrequency(const Frequency &freq) {
  setBCD8_le(Offset::txFrequency(), freq.inHz()/10);
}

void
DM32UVCodeplug::ChannelElement::clearTXFrequency() {
  setUInt32_le(Offset::txFrequency(), std::numeric_limits<uint32_t>::max());
}


DM32UVCodeplug::ChannelElement::ChannelType
DM32UVCodeplug::ChannelElement::channelType() const {
  return (ChannelType)getUInt2(Offset::channelType());
}

void
DM32UVCodeplug::ChannelElement::setChannelType(ChannelType type) {
  setUInt2(Offset::channelType(), (unsigned int)type);
}


Channel::Power
DM32UVCodeplug::ChannelElement::power() const {
  switch ((Power)getUInt2(Offset::power())) {
  case Power::Low: return Channel::Power::Low;
  case Power::Medium: return Channel::Power::Mid;
  case Power::High: return Channel::Power::High;
  }
  return Channel::Power::Low;
}

void
DM32UVCodeplug::ChannelElement::setPower(Channel::Power power) {
  switch (power) {
  case Channel::Power::Min:
  case Channel::Power::Low:
    setUInt2(Offset::power(), (unsigned int)Power::Low);
    break;
  case Channel::Power::Mid:
    setUInt2(Offset::power(), (unsigned int)Power::Medium);
    break;
  case Channel::Power::High:
  case Channel::Power::Max:
    setUInt2(Offset::power(), (unsigned int)Power::High);
    break;
  }
}


bool
DM32UVCodeplug::ChannelElement::loneWorkerEnabled() const {
  return getBit(Offset::loneWorker());
}

void
DM32UVCodeplug::ChannelElement::enableLoneWorker(bool enable) {
  setBit(Offset::loneWorker(), enable);
}


FMChannel::Bandwidth
DM32UVCodeplug::ChannelElement::bandwidth() const {
  if (getBit(Offset::bandwidth())) {
    return FMChannel::Bandwidth::Wide;
  }
  return FMChannel::Bandwidth::Narrow;
}

void
DM32UVCodeplug::ChannelElement::setBandwidth(FMChannel::Bandwidth bw) {
  switch (bw) {
    case FMChannel::Bandwidth::Narrow: clearBit(Offset::bandwidth()); break;
    case FMChannel::Bandwidth::Wide: setBit(Offset::bandwidth()); break;
  }
}


bool
DM32UVCodeplug::ChannelElement::validScanListIndex() const {
  return 0 != getUInt5(Offset::scanListIndex());
}

unsigned int
DM32UVCodeplug::ChannelElement::scanListIndex() const {
  return getUInt5(Offset::scanListIndex())-1;
}

void
DM32UVCodeplug::ChannelElement::setScanListIndex(unsigned int idx) {
  setUInt5(Offset::scanListIndex(), idx+1);
}

void
DM32UVCodeplug::ChannelElement::clearScanListIndex() {
  setUInt5(Offset::scanListIndex(), 0);
}


bool
DM32UVCodeplug::ChannelElement::preventTalkaroundEnabled() const {
  return getBit(Offset::preventTalkaround());
}

void
DM32UVCodeplug::ChannelElement::enablePreventTalkaround(bool enabled) {
  setBit(Offset::preventTalkaround(), enabled);
}


DM32UVCodeplug::ChannelElement::Admit
DM32UVCodeplug::ChannelElement::admitCriterion() const {
  return (DM32UVCodeplug::ChannelElement::Admit) getUInt2(Offset::admitCriterion());
}

void
DM32UVCodeplug::ChannelElement::setAdmitCriterion(Admit admit) {
  setUInt2(Offset::admitCriterion(), (unsigned int)admit);
}


bool
DM32UVCodeplug::ChannelElement::rxDMRAPRSEnabled() const {
  return getBit(Offset::rxDMRAPRS());
}

void
DM32UVCodeplug::ChannelElement::enableRXDMRAPRS(bool enable) {
  setBit(Offset::rxDMRAPRS(), enable);
}


bool
DM32UVCodeplug::ChannelElement::emergencyNotificationEnabled() const {
  return getBit(Offset::emergencyNotification());
}

void
DM32UVCodeplug::ChannelElement::enableEmergencyNotification(bool enable) {
  setBit(Offset::emergencySystemIndex(), enable);
}

bool
DM32UVCodeplug::ChannelElement::emergencyACKEnabled() const {
  return getBit(Offset::emergencyACK());
}

void
DM32UVCodeplug::ChannelElement::enableEmergencyACK(bool enable) {
  setBit(Offset::emergencyACK(), enable);
}

bool
DM32UVCodeplug::ChannelElement::validEmergencySystemIndex() const {
  return 0 != getUInt3(Offset::emergencySystemIndex());
}

unsigned int
DM32UVCodeplug::ChannelElement::emergencySystemIndex() const {
  return getUInt3(Offset::emergencySystemIndex())-1;
}

void
DM32UVCodeplug::ChannelElement::setEmergencySystemIndex(unsigned int idx) {
  setUInt3(Offset::emergencySystemIndex(), idx+1);
}

void
DM32UVCodeplug::ChannelElement::clearEmergencySystemIndex() {
  setUInt3(Offset::emergencySystemIndex(), 0);
}


unsigned
DM32UVCodeplug::ChannelElement::squelchLevel() const {
  return (10*getUInt4(Offset::squelchLevel()))/Limit::squelchLevel();
}

void
DM32UVCodeplug::ChannelElement::setSquelchLevel(unsigned int level) {
  setUInt4(Offset::squelchLevel(), (level*Limit::squelchLevel())/10);
}


bool
DM32UVCodeplug::ChannelElement::rxOnlyEnabled() const {
  return getBit(Offset::rxOnly());
}

void
DM32UVCodeplug::ChannelElement::enableRXOnly(bool enable) {
  setBit(Offset::rxOnly(), enable);
}


bool
DM32UVCodeplug::ChannelElement::dmrAPRSEnabled() const {
  return getBit(Offset::dmrAPRS());
}

void
DM32UVCodeplug::ChannelElement::enableDMRAPRS(bool enable) {
  return setBit(Offset::dmrAPRS(), enable);
}


bool
DM32UVCodeplug::ChannelElement::privateCallACKEnabled() const {
  return getBit(Offset::privateCallACK());
}

void
DM32UVCodeplug::ChannelElement::enablePrivateCallACK(bool enable) {
  return setBit(Offset::privateCallACK(), enable);
}

bool
DM32UVCodeplug::ChannelElement::dataACKEnabled() const {
  return getBit(Offset::dataACK());
}

void
DM32UVCodeplug::ChannelElement::enableDataACK(bool enable) {
  return setBit(Offset::dataACK(), enable);
}

bool
DM32UVCodeplug::ChannelElement::dcdmEnabled() const {
  return getBit(Offset::dcdm());
}

void
DM32UVCodeplug::ChannelElement::enableDCDM(bool enable) {
  return setBit(Offset::dcdm(), enable);
}


DMRChannel::TimeSlot
DM32UVCodeplug::ChannelElement::timeslot() const {
  return getBit(Offset::timeslot()) ? DMRChannel::TimeSlot::TS2 : DMRChannel::TimeSlot::TS1;
}

void
DM32UVCodeplug::ChannelElement::setTimeslot(DMRChannel::TimeSlot ts) {
  setBit(Offset::timeslot(), DMRChannel::TimeSlot::TS2 == ts);
}

unsigned int
DM32UVCodeplug::ChannelElement::colorCode() const {
  return getUInt4(Offset::colorcode());
}

void
DM32UVCodeplug::ChannelElement::setColorCode(unsigned int cc) {
  setUInt4(Offset::colorcode(), cc);
}


bool
DM32UVCodeplug::ChannelElement::encryptionEnabled() const {
  return getBit(Offset::encryptionEnable());
}

void
DM32UVCodeplug::ChannelElement::enableEncryption(bool enable) {
  setBit(Offset::encryptionEnable(), enable);
}


bool
DM32UVCodeplug::ChannelElement::validGroupListIndex() const {
  return 0 != getUInt6(Offset::groupListIndex());
}

unsigned int
DM32UVCodeplug::ChannelElement::groupListIndex() const {
  return getUInt6(Offset::groupListIndex())-1;
}

void
DM32UVCodeplug::ChannelElement::setGroupListIndex(unsigned int idx) {
  setUInt6(Offset::groupListIndex(), idx+1);
}

void
DM32UVCodeplug::ChannelElement::clearGroupListIndex() {
  setUInt6(Offset::groupListIndex(), 0);
}


unsigned int
DM32UVCodeplug::ChannelElement::dmrAPRSChannelIndex() const {
  return getUInt4(Offset::dmrAPRSChannelIndex());
}

void
DM32UVCodeplug::ChannelElement::setDMRAPRSChannelIndex(unsigned int idx) {
  setUInt4(Offset::dmrAPRSChannelIndex(), idx);
}


SelectiveCall
DM32UVCodeplug::ChannelElement::rxTone() const {
  return decodeSelectiveCall(getUInt16_le(Offset::rxTone()));
}

void
DM32UVCodeplug::ChannelElement::setRXTone(const SelectiveCall &tone) {
  setUInt16_le(Offset::rxTone(), encodeSelectiveCall(tone));
}

SelectiveCall
DM32UVCodeplug::ChannelElement::txTone() const {
  return decodeSelectiveCall(getUInt16_le(Offset::txTone()));
}

void
DM32UVCodeplug::ChannelElement::setTXTone(const SelectiveCall &tone) {
  setUInt16_le(Offset::txTone(), encodeSelectiveCall(tone));
}


bool
DM32UVCodeplug::ChannelElement::voxEnabled() const {
  return getBit(Offset::vox());
}

void
DM32UVCodeplug::ChannelElement::enableVOX(bool enable) {
  return setBit(Offset::vox(), enable);
}


unsigned int
DM32UVCodeplug::ChannelElement::dmrIdIndex() const {
  return getUInt8(Offset::dmrIdIndex());
}

void
DM32UVCodeplug::ChannelElement::setDMRIdIndex(unsigned int id) {
  setUInt8(Offset::dmrIdIndex(), id);
}


Channel *
DM32UVCodeplug::ChannelElement::decode(Context &ctx, const ErrorStack &err) const {
  Channel *ch = nullptr;

  if ((ChannelType::FM == channelType()) || (ChannelType::FMFixed == channelType())) {
    FMChannel *fm = new FMChannel(); ch = fm;
    fm->setBandwidth(bandwidth());
    switch (admitCriterion()) {
    case Admit::Always: fm->setAdmit(FMChannel::Admit::Always); break;
    case Admit::ChannelFree:
    case Admit::ToneOrCCMatch: fm->setAdmit(FMChannel::Admit::Free); break;
    case Admit::ToneMismatch: fm->setAdmit(FMChannel::Admit::Tone); break;
    }
    fm->setSquelch(squelchLevel());
    fm->setRXTone(rxTone());
    fm->setTXTone(txTone());
  } else if ((ChannelType::DMR == channelType()) || (ChannelType::DMRFixed == channelType())) {
    DMRChannel *dmr = new DMRChannel(); ch = dmr;
    switch (admitCriterion()) {
    case Admit::Always: dmr->setAdmit(DMRChannel::Admit::Always); break;
    case Admit::ToneOrCCMatch: dmr->setAdmit(DMRChannel::Admit::ColorCode); break;
    case Admit::ChannelFree: dmr->setAdmit(DMRChannel::Admit::Free); break;
    default: dmr->setAdmit(DMRChannel::Admit::Always); break;
    }
    dmr->setTimeSlot(timeslot());
    dmr->setColorCode(colorCode());
  }

  ch->setName(name());
  ch->setRXFrequency(rxFrequency());
  if (validTXFrequency())
    ch->setTXFrequency(txFrequency());
  ch->setPower(power());
  ch->setRXOnly(rxOnlyEnabled());

  if (voxEnabled())
    ch->setVOXDefault();
  else
    ch->setVOX(0);

  return ch;
}

bool
DM32UVCodeplug::ChannelElement::link(Channel *channel, Context &ctx, const ErrorStack &err) const {
  // Link scan list
  if (validScanListIndex()) {
    if (! ctx.has<ScanList>(scanListIndex())) {
      errMsg(err) << "Cannot link channel: Scan list with index " << scanListIndex()
                  << " not defined.";
      return false;
    }
    channel->setScanList(ctx.get<ScanList>(scanListIndex()));
  }

  if (channel->is<DMRChannel>()) {
    auto dmr = channel->as<DMRChannel>();
    // Link group list
    if (validGroupListIndex()) {
      if (! ctx.has<RXGroupList>(groupListIndex())) {
        errMsg(err) << "Cannot resolve group list index " << groupListIndex() << ".";
        return false;
      }
      dmr->setGroupListObj(ctx.get<RXGroupList>(groupListIndex()));
    }

    // Link DMR ID
    if (! ctx.has<DMRRadioID>(dmrIdIndex())) {
      errMsg(err) << "Cannot resolve radio id index " << dmrIdIndex() << ".";
      return false;
    }
    if (0 == dmrIdIndex())
      dmr->setRadioIdObj(DefaultRadioID::get());
    else
      dmr->setRadioIdObj(ctx.get<DMRRadioID>(dmrIdIndex()));

    /*if (dmrAPRSEnabled()) {
      if (! ctx.has<GPSSystem>(dmrAPRSChannelIndex())) {
        errMsg(err) << "Unknown GPS system index " << dmrAPRSChannelIndex() << ".";
        return false;
      }
      dmr->setAPRSObj(ctx.get<GPSSystem>(dmrAPRSChannelIndex()));
    }*/
  }

  return true;
}


SelectiveCall
DM32UVCodeplug::ChannelElement::decodeSelectiveCall(uint16_t code) {
  if (0xffff == code)
    return SelectiveCall();

  uint8_t type = code >> 14;
  code &= 0x3fff;

  if (0 == type) {
    return SelectiveCall(double((code>>4)&0xf)*10 + double((code>>4)&0xf)*1 + double(code & 0xf)/10);
  } else if ((1 == type) || (2 == type)) {
    return SelectiveCall(code, 2 == type);
  }

  return SelectiveCall();
}

uint16_t
DM32UVCodeplug::ChannelElement::encodeSelectiveCall(const SelectiveCall &tone) {
  if (! tone.isValid())
    return 0xffff;
  if (tone.isCTCSS())
    return (((tone.mHz()/10000) % 10) << 8) | (((tone.mHz()/1000) % 10) << 4) | ((tone.mHz()/100) % 10);
  if (tone.isDCS())
    return ((tone.isInverted() ? 2 : 1) << 14) | tone.octalCode();
  return 0xffff;
}



/* ******************************************************************************************** *
 * Implementation of DM32UVCodeplug::ChannelBankElement
 * ******************************************************************************************** */
DM32UVCodeplug::ChannelBankElement::ChannelBankElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}


unsigned int
DM32UVCodeplug::ChannelBankElement::channelCount() const {
  return getUInt16_le(Offset::channelCount());
}

void
DM32UVCodeplug::ChannelBankElement::setChannelCount(unsigned int n) {
  setUInt16_le(Offset::channelCount(), n);
}



/* ******************************************************************************************** *
 * Implementation of DM32UVCodeplug::ContactElement
 * ******************************************************************************************** */
DM32UVCodeplug::ContactElement::ContactElement(uint8_t *data, size_t size)
  : Element{data,size}
{
  // pass...
}

DM32UVCodeplug::ContactElement::ContactElement(uint8_t *data)
  : Element{data, size()}
{
  // pass...
}


QString
DM32UVCodeplug::ContactElement::name() const {
  return readASCII(Offset::name(), Limit::nameLength(), 0x00);
}

void
DM32UVCodeplug::ContactElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::nameLength(), 0x00);
}


DMRContact::Type
DM32UVCodeplug::ContactElement::callType() const {
  switch ((Type)getUInt8(Offset::callType())) {
  case Type::Private: return DMRContact::Type::PrivateCall;
  case Type::Group: return DMRContact::Type::GroupCall;
  case Type::All: return DMRContact::Type::AllCall;
  }
  return DMRContact::Type::PrivateCall;
}

void
DM32UVCodeplug::ContactElement::setCallType(DMRContact::Type type) {
  switch (type) {
  case DMRContact::Type::PrivateCall:
    setUInt8(Offset::callType(), (unsigned int)Type::Private);
    break;
  case DMRContact::Type::GroupCall:
    setUInt8(Offset::callType(), (unsigned int)Type::Group);
    break;
  case DMRContact::Type::AllCall:
    setUInt8(Offset::callType(), (unsigned int)Type::All);
    break;
  }
}

unsigned int
DM32UVCodeplug::ContactElement::dmrId() const {
  return getUInt24_le(Offset::dmrId());
}


void
DM32UVCodeplug::ContactElement::setDMRId(unsigned int id) {
  setUInt24_le(Offset::dmrId(), id);
}

DMRContact *
DM32UVCodeplug::ContactElement::decode(Context &ctx, const ErrorStack &err) const {
  Q_UNUSED(ctx); Q_UNUSED(err);
  return new DMRContact(callType(), name(), dmrId());
}



/* ******************************************************************************************** *
 * Implementation of DM32UVCodeplug::ContactIndexElement::ContactBitmapElement
 * ******************************************************************************************** */
DM32UVCodeplug::ContactIndexElement::ContactBitmapElement::ContactBitmapElement(uint8_t *ptr)
  : Codeplug::InvertedBitmapElement(ptr, size())
{
  // pass...
}


/* ******************************************************************************************** *
 * Implementation of DM32UVCodeplug::ContactIndexElement::EntryElement
 * ******************************************************************************************** */
DM32UVCodeplug::ContactIndexElement::EntryElement::EntryElement(uint8_t *ptr, size_t size)
  : Element{ptr, size}
{
  // pass...
}

DM32UVCodeplug::ContactIndexElement::EntryElement::EntryElement(uint8_t *ptr)
  : Element{ptr, size()}
{
  // pass...
}

DMRContact::Type
DM32UVCodeplug::ContactIndexElement::EntryElement::callType() const {
  switch ((Type)getUInt4(Offset::callType())) {
  case Type::Private: return DMRContact::Type::PrivateCall;
  case Type::Group: return DMRContact::Type::GroupCall;
  case Type::All: return DMRContact::Type::AllCall;
  }
  return DMRContact::Type::PrivateCall;
}

void
DM32UVCodeplug::ContactIndexElement::EntryElement::setCallType(DMRContact::Type type) {
  switch (type) {
  case DMRContact::Type::PrivateCall:
    setUInt4(Offset::callType(), (unsigned int)Type::Private);
    break;
  case DMRContact::Type::GroupCall:
    setUInt4(Offset::callType(), (unsigned int)Type::Group);
    break;
  case DMRContact::Type::AllCall:
    setUInt4(Offset::callType(), (unsigned int)Type::All);
    break;
  }
}



/* ******************************************************************************************** *
 * Implementation of DM32UVCodeplug::ContactIndexElement
 * ******************************************************************************************** */
DM32UVCodeplug::ContactIndexElement::ContactIndexElement(uint8_t *ptr, size_t size)
  : Element{ptr, size}
{
  // pass...
}

DM32UVCodeplug::ContactIndexElement::ContactIndexElement(uint8_t *ptr)
  : Element{ptr, size()}
{
  // pass...
}


unsigned int
DM32UVCodeplug::ContactIndexElement::contactCount() const {
  return getUInt16_le(Offset::contactCount());
}

void
DM32UVCodeplug::ContactIndexElement::setContactCount(unsigned int n) {
  setUInt16_le(Offset::contactCount(), n);
}

unsigned int
DM32UVCodeplug::ContactIndexElement::groupCallCount() const {
  return getUInt16_le(Offset::groupCount());
}

void
DM32UVCodeplug::ContactIndexElement::setGroupCallCount(unsigned int n) {
  setUInt16_le(Offset::groupCount(), n);
}

unsigned int
DM32UVCodeplug::ContactIndexElement::privateCallCount() const {
  return getUInt16_le(Offset::privateCount());
}

void
DM32UVCodeplug::ContactIndexElement::setPrivateCallCount(unsigned int n) {
  setUInt16_le(Offset::privateCount(), n);
}


DM32UVCodeplug::ContactIndexElement::ContactBitmapElement
DM32UVCodeplug::ContactIndexElement::bitmap() const {
  return ContactBitmapElement(_data + Offset::bitmap());
}

DM32UVCodeplug::ContactIndexElement::EntryElement
DM32UVCodeplug::ContactIndexElement::indexEntry(unsigned int n) {
  return EntryElement(_data + Offset::index() + n*EntryElement::size());
}

DM32UVCodeplug::ContactIndexElement::EntryElement
DM32UVCodeplug::ContactIndexElement::sortedIndexEntry(unsigned int n) {
  return EntryElement(_data + Offset::sorted() + n*EntryElement::size());
}



/* ******************************************************************************************** *
 * Implementation of DM32UVCodeplug::GroupListElement
 * ******************************************************************************************** */
DM32UVCodeplug::GroupListElement::GroupListElement(uint8_t *ptr, size_t size)
  : Element{ptr, size}
{
  // pass...
}

DM32UVCodeplug::GroupListElement::GroupListElement(uint8_t *ptr)
  : Element{ptr, size()}
{
  // pass...
}


QString
DM32UVCodeplug::GroupListElement::name() const {
  return readASCII(Offset::name(), Limit::nameLength(), 0x00);
}

void
DM32UVCodeplug::GroupListElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::nameLength(), 0x00);
}


bool
DM32UVCodeplug::GroupListElement::validId(unsigned int n) {
  return 0 != getUInt24_le(Offset::ids() + n*Offset::betweenIds());
}

unsigned int
DM32UVCodeplug::GroupListElement::id(unsigned int n) {
  return getUInt24_le(Offset::ids() + n*Offset::betweenIds());
}

void
DM32UVCodeplug::GroupListElement::setId(unsigned int n, unsigned int idx) {
  setUInt24_le(Offset::ids() + n*Offset::betweenIds(), idx);
}

void
DM32UVCodeplug::GroupListElement::clearId(unsigned int n) {
  setUInt24_le(Offset::ids() + n*Offset::betweenIds(), 0);
}


RXGroupList *
DM32UVCodeplug::GroupListElement::decode(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err); Q_UNUSED(ctx);
  return new RXGroupList(name());
}

bool
DM32UVCodeplug::GroupListElement::link(RXGroupList *gl, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err);

  for (unsigned int i=0; i<Limit::contacts(); i++) {
    if (! validId(i))
      continue;
    auto contact = ctx.config()->contacts()->findDigitalContact(id(i));
    if (nullptr == contact) {
      contact = new DMRContact(DMRContact::GroupCall, "Group Call", id(i));
      ctx.config()->contacts()->add(contact);
      logInfo() << "Cannot find DMR contact with ID " << id(i) << ", create one.";
    } else if (DMRContact::Type::GroupCall != contact->type()) {
      logWarn() << "Cannot add non-group-call contact to group list.";
      continue;
    }
    gl->addContact(contact);
  }

  return true;
}



/* ******************************************************************************************** *
 * Implementation of DM32UVCodeplug::GroupListBankElement::GroupListBitmapElement
 * ******************************************************************************************** */
DM32UVCodeplug::GroupListBankElement::GroupListBitmapElement::GroupListBitmapElement(uint8_t *ptr)
  : BitmapElement(ptr, 4)
{
  // pass...
}



/* ******************************************************************************************** *
 * Implementation of DM32UVCodeplug::GroupListBankElement
 * ******************************************************************************************** */
DM32UVCodeplug::GroupListBankElement::GroupListBankElement(uint8_t *ptr, size_t size)
  : Element{ptr, size}
{
  // pass...
}

DM32UVCodeplug::GroupListBankElement::GroupListBankElement(uint8_t *ptr)
  : Element{ptr, size()}
{
  // pass...
}


DM32UVCodeplug::GroupListBankElement::GroupListBitmapElement
DM32UVCodeplug::GroupListBankElement::bitmap() const {
  return GroupListBitmapElement(_data + Offset::bitmap());
}

DM32UVCodeplug::GroupListElement
DM32UVCodeplug::GroupListBankElement::groupList(unsigned int n) const {
  return GroupListElement(_data + Offset::groupLists() + n*GroupListElement::size());
}


bool
DM32UVCodeplug::GroupListBankElement::decode(Context &ctx, const ErrorStack &err) {
  for (unsigned int i=0,c=0; i<Limit::groupLists(); i++) {
    if (! bitmap().isEncoded(i))
      continue;
    auto gl = groupList(i).decode(ctx, err);
    if (nullptr == gl) {
      errMsg(err) << "Cannot decode " << i << "-th group list.";
      return false;
    }
    ctx.add(gl, c++);
    ctx.config()->rxGroupLists()->add(gl);
  }

  return true;
}

bool
DM32UVCodeplug::GroupListBankElement::link(Context &ctx, const ErrorStack &err) {
  for (unsigned int i=0,c=0; i<Limit::groupLists(); i++) {
    if (! bitmap().isEncoded(i))
      continue;
    auto gl = ctx.get<RXGroupList>(c++);
    if (! groupList(i).link(gl, ctx, err)) {
      errMsg(err) << "Cannot link " << i << "-th group list.";
      return false;
    }
  }

  return true;
}



/* ******************************************************************************************** *
 * Implementation of DM32UVCodeplug::RadioIdElement
 * ******************************************************************************************** */
DM32UVCodeplug::RadioIdElement::RadioIdElement(uint8_t *ptr)
  : Element{ptr, size()}
{
  // pass...
}


unsigned int
DM32UVCodeplug::RadioIdElement::id() const {
  return getUInt24_le(Offset::id());
}

void
DM32UVCodeplug::RadioIdElement::setId(unsigned int id) {
  setUInt24_le(Offset::id(), id);
}


QString
DM32UVCodeplug::RadioIdElement::name() const {
  return readASCII(Offset::name(), Limit::nameLength(), 0x00);
}

void
DM32UVCodeplug::RadioIdElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::nameLength(), 0x00);
}


DMRRadioID *
DM32UVCodeplug::RadioIdElement::decode(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(err);
  return new DMRRadioID(name(), id());
}



/* ******************************************************************************************** *
 * Implementation of DM32UVCodeplug::RadioIdBankElement
 * ******************************************************************************************** */
DM32UVCodeplug::RadioIdBankElement::RadioIdBankElement(uint8_t *ptr)
  : Element{ptr, size()}
{
  // pass...
}


unsigned int
DM32UVCodeplug::RadioIdBankElement::idCount() const {
  return getUInt8(Offset::count());
}

void
DM32UVCodeplug::RadioIdBankElement::setIdCount(unsigned int n) {
  setUInt8(Offset::count(), n);
}


DM32UVCodeplug::RadioIdElement
DM32UVCodeplug::RadioIdBankElement::id(unsigned int n) const {
  return RadioIdElement(_data + Offset::ids() + n*Offset::betweenIds());
}


bool
DM32UVCodeplug::RadioIdBankElement::decode(Context &ctx, const ErrorStack &err) {
  for (unsigned int i=0; i<idCount(); i++) {
    auto rid = id(i).decode(ctx, err);
    if (nullptr == rid) {
      errMsg(err) << "Cannot decode radio ID at index " << i << ".";
      return false;
    }
    ctx.add(rid, i);
    ctx.config()->radioIDs()->add(rid);
  }

  return true;
}



/* ******************************************************************************************** *
 * Implementation of DM32UVCodeplug::ZoneElement
 * ******************************************************************************************** */
DM32UVCodeplug::ZoneElement::ZoneElement(uint8_t *ptr)
  : Element{ptr, size()}
{
  // pass...
}

QString
DM32UVCodeplug::ZoneElement::name() const {
  return readASCII(Offset::name(), Limit::nameLength(), 0xff);
}

void
DM32UVCodeplug::ZoneElement::setName(const QString &name) {
  return writeASCII(Offset::name(), name, Limit::nameLength(), 0xff);
}


unsigned int
DM32UVCodeplug::ZoneElement::channelCount() const {
  return getUInt8(Offset::channelCount());
}

void
DM32UVCodeplug::ZoneElement::setChannelCount(unsigned int n) {
  setUInt8(Offset::channelCount(), n);
}


bool
DM32UVCodeplug::ZoneElement::channelIndexValid(unsigned int n) const {
  return 0 != getUInt16_le(Offset::channels() + n*sizeof(uint16_t));
}

unsigned int
DM32UVCodeplug::ZoneElement::channelIndex(unsigned int n) const {
  return getUInt16_le(Offset::channels() + n*sizeof(uint16_t)) - 1;
}

void
DM32UVCodeplug::ZoneElement::setChannelIndex(unsigned int n, unsigned int idx) {
  setUInt16_le(Offset::channels() + n*sizeof(uint16_t), idx+1);
}

void
DM32UVCodeplug::ZoneElement::clearChannelIndex(unsigned int n) {
  setUInt16_le(Offset::channels() + n*sizeof(uint16_t), 0);
}


Zone *
DM32UVCodeplug::ZoneElement::decode(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(err);
  return new Zone(name());
}

bool
DM32UVCodeplug::ZoneElement::link(Zone *zone, Context &ctx, const ErrorStack &err) {
  for (unsigned int i=0; i<channelCount(); i++) {
    if (! channelIndexValid(i))
      continue;
    if (! ctx.has<Channel>(channelIndex(i))) {
      errMsg(err) << "Cannot link zone, " << i << "-th channel index " << channelIndex(i)
                  << " not defined.";
      return false;
    }
    zone->A()->add(ctx.get<Channel>(channelIndex(i)));
  }

  return true;
}



/* ******************************************************************************************** *
 * Implementation of DM32UVCodeplug::ZoneBankElement
 * ******************************************************************************************** */
DM32UVCodeplug::ZoneBankElement::ZoneBankElement(uint8_t *ptr)
  : Element{ptr, size()}
{
  // pass...
}

unsigned int
DM32UVCodeplug::ZoneBankElement::count() const {
  return getUInt8(Offset::count());
}

void
DM32UVCodeplug::ZoneBankElement::setCount(unsigned int n) {
  return setUInt8(Offset::count(), n);
}


bool
DM32UVCodeplug::ZoneBankElement::channelIndexAValid() const {
  return 0 != getUInt8(Offset::channelIndexA());
}

unsigned int
DM32UVCodeplug::ZoneBankElement::channelIndexA() const {
  return getUInt8(Offset::channelIndexA())-1;
}

void
DM32UVCodeplug::ZoneBankElement::setChannelIndexA(unsigned int idx) {
  setUInt8(Offset::channelIndexA(), idx+1);
}

void
DM32UVCodeplug::ZoneBankElement::clearChannelIndexA() {
  setUInt8(Offset::channelIndexA(), 0);
}


bool
DM32UVCodeplug::ZoneBankElement::channelIndexBValid() const {
  return 0 != getUInt8(Offset::channelIndexB());
}

unsigned int
DM32UVCodeplug::ZoneBankElement::channelIndexB() const {
  return getUInt8(Offset::channelIndexB())-1;
}

void
DM32UVCodeplug::ZoneBankElement::setChannelIndexB(unsigned int idx) {
  setUInt8(Offset::channelIndexB(), idx+1);
}

void
DM32UVCodeplug::ZoneBankElement::clearChannelIndexB() {
  setUInt8(Offset::channelIndexB(), 0);
}


bool
DM32UVCodeplug::ZoneBankElement::zoneIndexAValid() const {
  return 0 != getUInt8(Offset::zoneIndexA());
}

unsigned int
DM32UVCodeplug::ZoneBankElement::zoneIndexA() const {
  return getUInt8(Offset::zoneIndexA())-1;
}

void
DM32UVCodeplug::ZoneBankElement::setZoneIndexA(unsigned int idx) {
  setUInt8(Offset::zoneIndexA(), idx+1);
}

void
DM32UVCodeplug::ZoneBankElement::clearZoneIndexA() {
  setUInt8(Offset::zoneIndexA(), 0);
}


bool
DM32UVCodeplug::ZoneBankElement::zoneIndexBValid() const {
  return 0 != getUInt8(Offset::zoneIndexB());
}

unsigned int
DM32UVCodeplug::ZoneBankElement::zoneIndexB() const {
  return getUInt8(Offset::zoneIndexB())-1;
}

void
DM32UVCodeplug::ZoneBankElement::setZoneIndexB(unsigned int idx) {
  setUInt8(Offset::zoneIndexB(), idx+1);
}

void
DM32UVCodeplug::ZoneBankElement::clearZoneIndexB() {
  setUInt8(Offset::zoneIndexB(), 0);
}



/* ******************************************************************************************** *
 * Implementation of DM32UVCodeplug::ScanListElement
 * ******************************************************************************************** */
DM32UVCodeplug::ScanListElement::ScanListElement(uint8_t *ptr)
  : Element{ptr, size()}
{
  // pass...
}

QString
DM32UVCodeplug::ScanListElement::name() const {
  return readASCII(Offset::name(), Limit::nameLength(), 0x00);
}

void
DM32UVCodeplug::ScanListElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::nameLength(), 0x00);
}


unsigned int
DM32UVCodeplug::ScanListElement::channelCount() const {
  return getUInt8(Offset::channelCount());
}

void
DM32UVCodeplug::ScanListElement::setChannelCount(unsigned int n) {
  setUInt8(Offset::channelCount(), n);
}


DM32UVCodeplug::ScanListElement::TransmitMode
DM32UVCodeplug::ScanListElement::transmitMode() const {
  return (TransmitMode)getUInt4(Offset::transmitMode());
}

void
DM32UVCodeplug::ScanListElement::setTransmitMode(TransmitMode mode) {
  setUInt4(Offset::transmitMode(), (unsigned int)mode);
}


DM32UVCodeplug::ScanListElement::ToneDetectionMode
DM32UVCodeplug::ScanListElement::toneDetectionMode() const {
  return (ToneDetectionMode) getUInt4(Offset::toneDetection());
}

void
DM32UVCodeplug::ScanListElement::setToneDetectionMode(ToneDetectionMode mode) {
  setUInt4(Offset::toneDetection(), (unsigned int) mode);
}


Interval
DM32UVCodeplug::ScanListElement::hangTime() const {
  return Interval::fromMilliseconds(500 * getUInt4(Offset::hangTime()));
}

void
DM32UVCodeplug::ScanListElement::setHangTime(const Interval &dur) {
  setUInt4(Offset::hangTime(), dur.milliseconds()/500);
}


bool
DM32UVCodeplug::ScanListElement::secondaryChannelIndexValid() const {
  return 0 != getUInt4(Offset::secondaryChannel());
}

unsigned int
DM32UVCodeplug::ScanListElement::secondaryChannelIndex() const {
  return getUInt4(Offset::secondaryChannel())-1;
}

void
DM32UVCodeplug::ScanListElement::setSecondaryChannelIndex(unsigned int idx) {
  setUInt4(Offset::secondaryChannel(), idx+1);
}

void
DM32UVCodeplug::ScanListElement::clearSecondaryChannelIndex() {
  setUInt4(Offset::secondaryChannel(), 0);
}


bool
DM32UVCodeplug::ScanListElement::primaryChannelIndexValid() const {
  return 0 != getUInt4(Offset::primaryChannel());
}

unsigned int
DM32UVCodeplug::ScanListElement::primaryChannelIndex() const {
  return getUInt4(Offset::primaryChannel())-1;
}

void
DM32UVCodeplug::ScanListElement::setPrimaryChannelIndex(unsigned int idx) {
  setUInt4(Offset::primaryChannel(), idx+1);
}

void
DM32UVCodeplug::ScanListElement::clearPrimaryChannelIndex() {
  setUInt4(Offset::primaryChannel(), 0);
}


bool
DM32UVCodeplug::ScanListElement::revertChannelIndexValid() const {
  return 0 != getUInt16_le(Offset::revertChannel());
}

unsigned int
DM32UVCodeplug::ScanListElement::revertChannelIndex() const {
  return getUInt16_le(Offset::revertChannel())-1;
}

void
DM32UVCodeplug::ScanListElement::setRevertChannelIndex(unsigned int idx) {
  setUInt16_le(Offset::revertChannel(), idx+1);
}

void
DM32UVCodeplug::ScanListElement::clearRevertChannelIndex() {
  setUInt16_le(Offset::revertChannel(), 0);
}


Interval
DM32UVCodeplug::ScanListElement::prioritySweepTime() const {
  return Interval::fromMilliseconds(500 + 300*getUInt6(Offset::prioritySweepTime()));
}

void
DM32UVCodeplug::ScanListElement::setPrioritySweepTime(const Interval &dur) {
  if (dur.milliseconds() >= 500) {
    setUInt6(Offset::prioritySweepTime(), (dur.milliseconds()-500)/300);
  } else {
    setUInt6(Offset::prioritySweepTime(), 0);
  }
}


bool
DM32UVCodeplug::ScanListElement::isCurrentChannel(unsigned int n) const {
  return 0 == getUInt16_le(Offset::channels() + n*sizeof(uint16_t));
}

unsigned int
DM32UVCodeplug::ScanListElement::channelIndex(unsigned int n) const {
  return getUInt16_le(Offset::channels() + n*sizeof(uint16_t)) - 1;
}

void
DM32UVCodeplug::ScanListElement::setChannelIndex(unsigned int n, unsigned int idx) {
  setUInt16_le(Offset::channels() + n*sizeof(uint16_t), idx+1);
}

void
DM32UVCodeplug::ScanListElement::setCurrentChannel(unsigned int n) {
  setUInt16_le(Offset::channels() + n*sizeof(uint16_t), 0);
}

ScanList *
DM32UVCodeplug::ScanListElement::decode(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(err);
  return new ScanList(name());
}

bool
DM32UVCodeplug::ScanListElement::link(ScanList *lst, Context &ctx, const ErrorStack &err) {
  for (unsigned int i=0; i<channelCount(); i++) {
    if (isCurrentChannel(i)) {
      lst->addChannel(SelectedChannel::get());
    } else if (! ctx.has<Channel>(channelIndex(i))) {
      errMsg(err) << "Cannot link scan list, channel with index " << channelIndex(i)
                  << "not defined";
      return false;
    }
    lst->addChannel(ctx.get<Channel>(channelIndex(i)));
  }
  return true;
}


/* ******************************************************************************************** *
 * Implementation of DM32UVCodeplug::ScanListBankElement
 * ******************************************************************************************** */
DM32UVCodeplug::ScanListBankElement::ScanListBankElement(uint8_t *ptr)
  : Element{ptr, size()}
{
  // pass...
}


unsigned int
DM32UVCodeplug::ScanListBankElement::count() const {
   return getUInt8(Offset::count());
}

void
DM32UVCodeplug::ScanListBankElement::setCount(unsigned int n) {
  setUInt8(Offset::count(), n);
}


DM32UVCodeplug::ScanListElement
DM32UVCodeplug::ScanListBankElement::scanList(unsigned int n) const {
  return ScanListElement(_data + Offset::scanLists() + n*ScanListElement::size());
}


DM32UVCodeplug::ScanListBankElement::ScanMode
DM32UVCodeplug::ScanListBankElement::scanMode() const {
  return (ScanMode)getUInt8(Offset::scanMode());
}

void
DM32UVCodeplug::ScanListBankElement::setScanMode(ScanMode mode) {
  setUInt8(Offset::scanMode(), (unsigned int)mode);
}


FrequencyRange
DM32UVCodeplug::ScanListBankElement::vhfRange() const {
  return {Frequency::fromMHz(getBCD4_le(Offset::vhfLower())),
    Frequency::fromMHz(getBCD4_le(Offset::vhfUpper())) };
}

void
DM32UVCodeplug::ScanListBankElement::setVHFRange(const FrequencyRange &range) {
  setBCD4_le(Offset::vhfLower(), range.lower.inMHz());
  setBCD4_le(Offset::vhfUpper(), range.upper.inMHz());
}

FrequencyRange
DM32UVCodeplug::ScanListBankElement::uhfRange() const {
  return {Frequency::fromMHz(getBCD4_le(Offset::uhfLower())),
    Frequency::fromMHz(getBCD4_le(Offset::uhfUpper())) };
}

void
DM32UVCodeplug::ScanListBankElement::setUHFRange(const FrequencyRange &range) {
  setBCD4_le(Offset::uhfLower(), range.lower.inMHz());
  setBCD4_le(Offset::uhfUpper(), range.upper.inMHz());
}


bool
DM32UVCodeplug::ScanListBankElement::decode(Context &ctx, const ErrorStack &err) {
  for (unsigned int i=0; i<count(); i++) {
    auto lst = scanList(i).decode(ctx, err);
    if (nullptr == lst) {
      errMsg(err) << "Cannot decode scan list at index " << i << ".";
      return false;
    }
    ctx.add(lst, i);
    ctx.config()->scanlists()->add(lst);
  }
  return true;
}

bool
DM32UVCodeplug::ScanListBankElement::link(Context &ctx, const ErrorStack &err) {
  for (unsigned int i=0; i<count(); i++) {
    auto lst = ctx.get<ScanList>(i);
    if (! scanList(i).link(lst, ctx, err)) {
      errMsg(err) << "Cannot link scan list at index " << i << ".";
      return false;
    }
  }
  return true;
}


/* ******************************************************************************************** *
 * Implementation of DM32UVCodeplug::RoamingChannelElement
 * ******************************************************************************************** */
DM32UVCodeplug::RoamingChannelElement::RoamingChannelElement(uint8_t *ptr)
  : Element{ptr, size()}
{
  // pass...
}

QString
DM32UVCodeplug::RoamingChannelElement::name() const {
  return readASCII(Offset::name(), Limit::nameLength(), 0x00);
}

void
DM32UVCodeplug::RoamingChannelElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::nameLength(), 0x00);
}

Frequency
DM32UVCodeplug::RoamingChannelElement::rxFrequency() const {
  return Frequency::fromHz((unsigned long long)getBCD8_le(Offset::rxFrequency()) * 10);
}

void
DM32UVCodeplug::RoamingChannelElement::setRXFrequency(const Frequency &f) {
  setBCD8_le(Offset::rxFrequency(), f.inHz()/10);
}

Frequency
DM32UVCodeplug::RoamingChannelElement::txFrequency() const {
  return Frequency::fromHz((unsigned long long)getBCD8_le(Offset::txFrequency()) * 10);
}

void
DM32UVCodeplug::RoamingChannelElement::setTXFrequency(const Frequency &f) {
  setBCD8_le(Offset::txFrequency(), f.inHz()/10);
}

unsigned int
DM32UVCodeplug::RoamingChannelElement::colorCode() const {
  return getUInt8(Offset::colorCode());
}

void
DM32UVCodeplug::RoamingChannelElement::setColorCode(unsigned int cc) {
  setUInt8(Offset::colorCode(), std::min(cc, 15u));
}

DMRChannel::TimeSlot
DM32UVCodeplug::RoamingChannelElement::timeSlot() const {
  switch ((TimeSlot) getUInt8(Offset::timeSlot())) {
  case TimeSlot::TS1: return DMRChannel::TimeSlot::TS1;
  case TimeSlot::TS2: return DMRChannel::TimeSlot::TS2;
  }
  return DMRChannel::TimeSlot::TS1;
}

void
DM32UVCodeplug::RoamingChannelElement::setTimeSlot(DMRChannel::TimeSlot ts) {
  switch (ts) {
  case DMRChannel::TimeSlot::TS1: setUInt8(Offset::timeSlot(), (unsigned int)TimeSlot::TS1); break;
  case DMRChannel::TimeSlot::TS2: setUInt8(Offset::timeSlot(), (unsigned int)TimeSlot::TS2); break;
  }
}

RoamingChannel *
DM32UVCodeplug::RoamingChannelElement::decode(Context &ctx, const ErrorStack &err) const {
  auto rc = new RoamingChannel();
  rc->setName(name());
  rc->setRXFrequency(rxFrequency());
  rc->setTXFrequency(txFrequency());
  rc->overrideColorCode(true);
  rc->setColorCode(colorCode());
  rc->overrideTimeSlot(true);
  rc->setTimeSlot(timeSlot());
  return rc;
}




/* ******************************************************************************************** *
 * Implementation of DM32UVCodeplug::RoamingChannelBankElement
 * ******************************************************************************************** */
DM32UVCodeplug::RoamingChannelBankElement::RoamingChannelBankElement(uint8_t *ptr)
  : Element{ptr, size()}
{
  // pass...
}

unsigned int
DM32UVCodeplug::RoamingChannelBankElement::count() const {
  return getUInt8(Offset::count());
}

void
DM32UVCodeplug::RoamingChannelBankElement::setCount(unsigned int n) {
  setUInt8(Offset::count(), std::min(n, Limit::channels()));
}

DM32UVCodeplug::RoamingChannelElement
DM32UVCodeplug::RoamingChannelBankElement::channel(unsigned int n) {
  return RoamingChannelElement(_data + Offset::channels() + n*Offset::betweenChannels());
}

bool
DM32UVCodeplug::RoamingChannelBankElement::decode(Context &ctx, const ErrorStack &err) {
  for (unsigned int i=0; i<count(); i++) {
    auto rc = channel(i).decode(ctx, err);
    if (nullptr == rc) {
      errMsg(err) << "Cannot decode roaming channel at index " << i << ".";
      return false;
    }
    ctx.add(rc, i);
    ctx.config()->roamingChannels()->add(rc);
  }
  return true;
}



/* ******************************************************************************************** *
 * Implementation of DM32UVCodeplug::RoamingZoneElement
 * ******************************************************************************************** */
DM32UVCodeplug::RoamingZoneElement::RoamingZoneElement(uint8_t *ptr)
  : Element{ptr, size()}
{
  // pass..
}


QString
DM32UVCodeplug::RoamingZoneElement::name() const {
  return readASCII(Offset::name(), Limit::nameLength(), 0x00);
}

void
DM32UVCodeplug::RoamingZoneElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::nameLength(), 0x00);
}

unsigned int
DM32UVCodeplug::RoamingZoneElement::count() const {
  return getUInt8(Offset::channelCount());
}

void
DM32UVCodeplug::RoamingZoneElement::setCount(unsigned int n) {
  setUInt8(Offset::channelCount(), std::min(n, Limit::channels()));
}


bool
DM32UVCodeplug::RoamingZoneElement::channelIndexValid(unsigned int n) {
  return 0 != getUInt8(Offset::channels() + n*Offset::betweenChannels());
}

unsigned int
DM32UVCodeplug::RoamingZoneElement::channelIndex(unsigned int n) {
  return getUInt8(Offset::channels() + n*Offset::betweenChannels())-1;
}

void
DM32UVCodeplug::RoamingZoneElement::setChannelIndex(unsigned int n, unsigned int idx) {
  setUInt8(Offset::channels() + n*Offset::betweenChannels(), idx+1);
}

void
DM32UVCodeplug::RoamingZoneElement::clearChannelIndex(unsigned int n) {
  setUInt8(Offset::channels() + n*Offset::betweenChannels(), 0);
}

RoamingZone *
DM32UVCodeplug::RoamingZoneElement::decode(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(err);
  return new RoamingZone(name());
}

bool
DM32UVCodeplug::RoamingZoneElement::link(RoamingZone *zone, Context &ctx, const ErrorStack &err) {
  for (unsigned int i=0; i<count(); i++) {
    if (! ctx.has<RoamingChannel>(channelIndex(i))) {
      errMsg(err) << "Cannot resolve " << i << "-th channel index " << channelIndex(i)
                  << ": Not defined.";
      return false;
    }
    zone->addChannel(ctx.get<RoamingChannel>(channelIndex(i)));
  }
  return true;
}



/* ******************************************************************************************** *
 * Implementation of DM32UVCodeplug::RoamingZoneBankElement
 * ******************************************************************************************** */
DM32UVCodeplug::RoamingZoneBankElement::RoamingZoneBankElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}

unsigned int
DM32UVCodeplug::RoamingZoneBankElement::count() const {
  return getUInt8(Offset::count());
}

void
DM32UVCodeplug::RoamingZoneBankElement::setCount(unsigned int n) {
  setUInt8(Offset::count(), std::min(n, Limit::zones()));
}


bool
DM32UVCodeplug::RoamingZoneBankElement::autoRoamEnabled() const {
  return 0x01 == getUInt8(Offset::autoRoam());
}

void
DM32UVCodeplug::RoamingZoneBankElement::enableAutoRoam(bool enable) {
  setUInt8(Offset::autoRoam(), enable ? 0x01 : 0x00);
}


Interval
DM32UVCodeplug::RoamingZoneBankElement::roamingDelay() const {
  return Interval::fromMinutes(getUInt8(Offset::roamingDelay()));
}

void
DM32UVCodeplug::RoamingZoneBankElement::setRoamingDelay(const Interval &delay) {
  setUInt8(Offset::roamingDelay(), delay.minutes());
}


bool
DM32UVCodeplug::RoamingZoneBankElement::defaultRoamingZoneIndexValid() const {
  return 0 != getUInt8(Offset::defaultRoamingZone());
}

unsigned int
DM32UVCodeplug::RoamingZoneBankElement::defaultRoamingZoneIndex() const {
  return getUInt8(Offset::defaultRoamingZone())-1;
}

void
DM32UVCodeplug::RoamingZoneBankElement::setDefaultRoamingZoneIndex(unsigned int idx) {
  return setUInt8(Offset::defaultRoamingZone(), idx+1);
}

void
DM32UVCodeplug::RoamingZoneBankElement::clearDefaultRoamingZoneIndex() {
  setUInt8(Offset::defaultRoamingZone(), 0);
}


DM32UVCodeplug::RoamingZoneElement
DM32UVCodeplug::RoamingZoneBankElement::zone(unsigned int n) {
  return RoamingZoneElement(_data + Offset::zones() + n*Offset::betweenZones());
}


bool
DM32UVCodeplug::RoamingZoneBankElement::decode(Context &ctx, const ErrorStack &err) {
  for (unsigned int i=0; i<count(); i++) {
    auto z = zone(i).decode(ctx, err);
    if (nullptr == z) {
      errMsg(err) << "Cannot decode zone at index " << i << ".";
      return false;
    }
    ctx.add(z, i);
    ctx.config()->roamingZones()->add(z);
  }
  return true;
}

bool
DM32UVCodeplug::RoamingZoneBankElement::link(Context &ctx, const ErrorStack &err) {
  for (unsigned int i=0; i<count(); i++) {
    auto z = ctx.get<RoamingZone>(i);
    if (nullptr == z) {
      errMsg(err) << "Cannot link zone at index " << i << ": Cannot resolve index.";
      return false;
    }
    if (! zone(i).link(z, ctx, err)) {
      errMsg(err) << "Cannot link zone at index " << i << ".";
      return false;
    }
  }
  return true;
}



/* ******************************************************************************************** *
 * Implementation of DM32UVCodeplug::SMSTemplateElement
 * ******************************************************************************************** */
DM32UVCodeplug::SMSTemplateElement::SMSTemplateElement(uint8_t *ptr)
  : Element{ptr, size()}
{
  // pass...
}

QString
DM32UVCodeplug::SMSTemplateElement::message() const {
  uint8_t len = getUInt8(Offset::length());
  return readASCII(Offset::message(), len, 0x00);
}

void
DM32UVCodeplug::SMSTemplateElement::setMessage(const QString &msg) {
  unsigned int len = std::min((unsigned int)msg.length(), Limit::messageLength());
  setUInt8(Offset::length(), len);
  writeASCII(Offset::message(), msg, Limit::messageLength(), 0x00);
}

SMSTemplate *
DM32UVCodeplug::SMSTemplateElement::decode(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(err);
  auto tmpl = new SMSTemplate(); tmpl->setMessage(message());
  return tmpl;
}


/* ******************************************************************************************** *
 * Implementation of DM32UVCodeplug::SMSTemplateBankElement
 * ******************************************************************************************** */
DM32UVCodeplug::SMSTemplateBankElement::SMSTemplateBankElement(uint8_t *ptr)
  : Element{ptr, size()}
{
  // pass...
}

unsigned int
DM32UVCodeplug::SMSTemplateBankElement::count() const {
  return getUInt8(Offset::count());
}

void
DM32UVCodeplug::SMSTemplateBankElement::setCount(unsigned int n) {
  n = std::min(n, Limit::messages());
  setUInt8(Offset::count(), n);
}

DM32UVCodeplug::SMSTemplateElement
DM32UVCodeplug::SMSTemplateBankElement::message(unsigned int n) const {
  return SMSTemplateElement(_data + Offset::messages() + n*Offset::betweenMessages());
}

bool
DM32UVCodeplug::SMSTemplateBankElement::decode(Context &ctx, const ErrorStack &err) {
  for (unsigned int i=0; i<count(); i++) {
    auto tmpl = message(i).decode(ctx, err);
    if (nullptr == tmpl) {
      errMsg(err) << "Cannot decode SMS template at index " << i << ".";
      return false;
    }
    ctx.add(tmpl, i);
    ctx.config()->smsExtension()->smsTemplates()->add(tmpl);
  }
  return true;
}



/* ******************************************************************************************** *
 * Implementation of DM32UVCodeplug::GeneralSettingsElement::Color
 * ******************************************************************************************** */
unsigned int
DM32UVCodeplug::GeneralSettingsElement::Color::encode(Code name) {
  return (unsigned int) name;
}

DM32UVCodeplug::GeneralSettingsElement::Color::Code
DM32UVCodeplug::GeneralSettingsElement::Color::decode(unsigned int code) {
  return (Code)code;
}


/* ******************************************************************************************** *
 * Implementation of DM32UVCodeplug::GeneralSettingsElement::Function
 * ******************************************************************************************** */
unsigned int
DM32UVCodeplug::GeneralSettingsElement::KeyFunction::encode(Function func) {
  return (unsigned int) func;
}

DM32UVCodeplug::GeneralSettingsElement::KeyFunction::Function
DM32UVCodeplug::GeneralSettingsElement::KeyFunction::decode(unsigned int code) {
  return (Function)code;
}


/* ******************************************************************************************** *
 * Implementation of DM32UVCodeplug::GeneralSettingsElement
 * ******************************************************************************************** */
DM32UVCodeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr)
  : Element{ptr, size()}
{
  // pass...
}


DM32UVCodeplug::GeneralSettingsElement::BootDisplay
DM32UVCodeplug::GeneralSettingsElement::bootDisplay() const {
  return (BootDisplay) getUInt8(Offset::bootDisplay());
}

void
DM32UVCodeplug::GeneralSettingsElement::setBootDisplay(BootDisplay dis) {
  setUInt8(Offset::bootDisplay(), (unsigned int)dis);
}


QString
DM32UVCodeplug::GeneralSettingsElement::bootMessage1() const {
  return readASCII(Offset::bootMessage1(), Limit::bootMessageLength(), 0x00);
}

void
DM32UVCodeplug::GeneralSettingsElement::setBootMessage1(const QString &msg) {
  writeASCII(Offset::bootMessage1(), msg, Limit::bootMessageLength(), 0x00);
}

QString
DM32UVCodeplug::GeneralSettingsElement::bootMessage2() const {
  return readASCII(Offset::bootMessage2(), Limit::bootMessageLength(), 0x00);
}

void
DM32UVCodeplug::GeneralSettingsElement::setBootMessage2(const QString &msg) {
  writeASCII(Offset::bootMessage2(), msg, Limit::bootMessageLength(), 0x00);
}


bool
DM32UVCodeplug::GeneralSettingsElement::mcuResetEnabled() const {
  return 0x01 == getUInt8(Offset::mcuReset());
}

void
DM32UVCodeplug::GeneralSettingsElement::enableMCUReset(bool enable) {
  setUInt8(Offset::mcuReset(), enable ? 0x01 : 0x00);
}


Interval
DM32UVCodeplug::GeneralSettingsElement::autoPowerOffDelay() const {
  switch ((AutoPowerOffDelay)getUInt8(Offset::autoPowerOffDelay())) {
  case AutoPowerOffDelay::Off: return Interval::infinity();
  case AutoPowerOffDelay::T30Min: return Interval::fromMinutes(30);
  case AutoPowerOffDelay::T60Min: return Interval::fromMinutes(60);
  case AutoPowerOffDelay::T2h: return Interval::fromMinutes(120);
  case AutoPowerOffDelay::T4h: return Interval::fromMinutes(240);
  case AutoPowerOffDelay::T8h: return Interval::fromMinutes(480);
  }
  return Interval::infinity();
}

void
DM32UVCodeplug::GeneralSettingsElement::setAutoPowerOffDelay(const Interval &delay) {
  if (delay.isNull() || delay.isInfinite())
    setUInt8(Offset::autoPowerOffDelay(), (unsigned int)AutoPowerOffDelay::Off);
  else if (delay.minutes() <= 30)
    setUInt8(Offset::autoPowerOffDelay(), (unsigned int)AutoPowerOffDelay::T30Min);
  else if (delay.minutes() <= 60)
    setUInt8(Offset::autoPowerOffDelay(), (unsigned int)AutoPowerOffDelay::T60Min);
  else if (delay.minutes() <= 120)
    setUInt8(Offset::autoPowerOffDelay(), (unsigned int)AutoPowerOffDelay::T2h);
  else if (delay.minutes() <= 240)
    setUInt8(Offset::autoPowerOffDelay(), (unsigned int)AutoPowerOffDelay::T4h);
  else if (delay.minutes() <= 480)
    setUInt8(Offset::autoPowerOffDelay(), (unsigned int)AutoPowerOffDelay::T8h);
  else
    setUInt8(Offset::autoPowerOffDelay(), (unsigned int)AutoPowerOffDelay::Off);
}


bool
DM32UVCodeplug::GeneralSettingsElement::radioSilentEnabled() const {
  return getBit(Offset::radioSilent());
}

void
DM32UVCodeplug::GeneralSettingsElement::enableRadioSilent(bool enable) {
  setBit(Offset::radioSilent(), enable);
}


bool
DM32UVCodeplug::GeneralSettingsElement::keyToneEnabled() const {
  return getBit(Offset::keyTone());
}

void
DM32UVCodeplug::GeneralSettingsElement::enableKeyTone(bool enable) {
  setBit(Offset::keyTone(), enable);
}


bool
DM32UVCodeplug::GeneralSettingsElement::smsToneEnabled() const {
  return getBit(Offset::smsTone());
}

void
DM32UVCodeplug::GeneralSettingsElement::enableSMSTone(bool enable) {
  setBit(Offset::smsTone(), enable);
}


bool
DM32UVCodeplug::GeneralSettingsElement::groupCallToneEnabled() const {
  return getBit(Offset::groupCallTone());
}

void
DM32UVCodeplug::GeneralSettingsElement::enableGroupCallTone(bool enable) {
  setBit(Offset::groupCallTone(), enable);
}

bool
DM32UVCodeplug::GeneralSettingsElement::privateCallToneEnabled() const {
  return getBit(Offset::privateCallTone());
}

void
DM32UVCodeplug::GeneralSettingsElement::enablePrivateCallTone(bool enable) {
  setBit(Offset::privateCallTone(), enable);
}


bool
DM32UVCodeplug::GeneralSettingsElement::eotToneEnabled() const {
  return getBit(Offset::eotTone());
}

void
DM32UVCodeplug::GeneralSettingsElement::enableEOTTone(bool enable) {
  setBit(Offset::eotTone(), enable);
}


bool
DM32UVCodeplug::GeneralSettingsElement::talkPermitToneEnabled() const {
  return getBit(Offset::talkPermitTone());
}

void
DM32UVCodeplug::GeneralSettingsElement::enableTalkPermitTone(bool enable) {
  setBit(Offset::talkPermitTone(), enable);
}


bool
DM32UVCodeplug::GeneralSettingsElement::bootToneEnabled() const {
  return getBit(Offset::bootTone());
}

void
DM32UVCodeplug::GeneralSettingsElement::enableBootTone(bool enable) {
  setBit(Offset::bootTone(), enable);
}


bool
DM32UVCodeplug::GeneralSettingsElement::voicePromptEnabled() const {
  return getBit(Offset::voicePrompt());
}

void
DM32UVCodeplug::GeneralSettingsElement::enableVoicePrompt(bool enable) {
  setBit(Offset::voicePrompt(), enable);
}


bool
DM32UVCodeplug::GeneralSettingsElement::lowBatteryToneEnabled() const {
  return getBit(Offset::lowBatteryTone());
}

void
DM32UVCodeplug::GeneralSettingsElement::enableLowBatteryTone(bool enable) {
  setBit(Offset::lowBatteryTone(), enable);
}


DM32UVCodeplug::GeneralSettingsElement::FMRogerTone
DM32UVCodeplug::GeneralSettingsElement::fmRogerTone() const {
  return (FMRogerTone)getUInt2(Offset::fmRogerTone());
}

void
DM32UVCodeplug::GeneralSettingsElement::setFMRogerTone(FMRogerTone tone) {
  setUInt2(Offset::fmRogerTone(), (unsigned int)tone);
}


unsigned int
DM32UVCodeplug::GeneralSettingsElement::displayBrightness() const {
  return (getUInt8(Offset::displayBrightness())*10)/Limit::maxBrightness();
}

void
DM32UVCodeplug::GeneralSettingsElement::setDisplayBrightness(unsigned int n) {
  setUInt8(Offset::displayBrightness(), (n*Limit::maxBrightness())/10);
}


Interval
DM32UVCodeplug::GeneralSettingsElement::backlightDuration() const {
  switch ((BacklightDuration)getUInt8(Offset::backlightDuration())) {
  case BacklightDuration::Infinity: return Interval::infinity();
  case BacklightDuration::T5s: return Interval::fromSeconds(5);
  case BacklightDuration::T10s: return Interval::fromSeconds(10);
  case BacklightDuration::T15s: return Interval::fromSeconds(15);
  case BacklightDuration::T20s: return Interval::fromSeconds(20);
  case BacklightDuration::T25s: return Interval::fromSeconds(25);
  case BacklightDuration::T30s: return Interval::fromSeconds(30);
  case BacklightDuration::T1min: return Interval::fromMinutes(1);
  case BacklightDuration::T2min: return Interval::fromMinutes(2);
  case BacklightDuration::T3min: return Interval::fromMinutes(3);
  case BacklightDuration::T4min: return Interval::fromMinutes(4);
  case BacklightDuration::T5min: return Interval::fromMinutes(5);
  }
  return Interval::infinity();
}

void
DM32UVCodeplug::GeneralSettingsElement::setBacklightDuration(Interval duration) {
  if (duration.isNull() || duration.isInfinite()) {
    setUInt8(Offset::backlightDuration(), (unsigned int)BacklightDuration::Infinity);
  } else if (duration.seconds() <= 5) {
    setUInt8(Offset::backlightDuration(), (unsigned int)BacklightDuration::T5s);
  } else if (duration.seconds() <= 10) {
    setUInt8(Offset::backlightDuration(), (unsigned int)BacklightDuration::T10s);
  } else if (duration.seconds() <= 15) {
    setUInt8(Offset::backlightDuration(), (unsigned int)BacklightDuration::T15s);
  } else if (duration.seconds() <= 20) {
    setUInt8(Offset::backlightDuration(), (unsigned int)BacklightDuration::T20s);
  } else if (duration.seconds() <= 25) {
    setUInt8(Offset::backlightDuration(), (unsigned int)BacklightDuration::T25s);
  } else if (duration.seconds() <= 30) {
    setUInt8(Offset::backlightDuration(), (unsigned int)BacklightDuration::T30s);
  } else if (duration.minutes() <= 1) {
    setUInt8(Offset::backlightDuration(), (unsigned int)BacklightDuration::T1min);
  } else if (duration.minutes() <= 2) {
    setUInt8(Offset::backlightDuration(), (unsigned int)BacklightDuration::T2min);
  } else if (duration.minutes() <= 3) {
    setUInt8(Offset::backlightDuration(), (unsigned int)BacklightDuration::T3min);
  } else if (duration.minutes() <= 4) {
    setUInt8(Offset::backlightDuration(), (unsigned int)BacklightDuration::T4min);
  } else if (duration.minutes() <= 5) {
    setUInt8(Offset::backlightDuration(), (unsigned int)BacklightDuration::T5min);
  }
  setUInt8(Offset::backlightDuration(), (unsigned int)BacklightDuration::Infinity);
}


Interval
DM32UVCodeplug::GeneralSettingsElement::menuDuration() const {
  if (0 == getUInt8(Offset::menuDuration())) {
    return Interval::infinity();
  }
  return Interval::fromSeconds(5 * getUInt8(Offset::menuDuration()));
}

void
DM32UVCodeplug::GeneralSettingsElement::setMenuDuration(Interval duration) {
  if (duration.isNull() || duration.isInfinite()) {
    setUInt8(Offset::menuDuration(), 0);
  } else {
    setUInt8(Offset::menuDuration(), duration.seconds()/5);
  }
}


bool
DM32UVCodeplug::GeneralSettingsElement::showVolmueChange() const {
  return getBit(Offset::showVolumeChange());
}

void
DM32UVCodeplug::GeneralSettingsElement::enableShowVolumeChange(bool enable) {
  setBit(Offset::showVolumeChange(), enable);
}


DM32UVCodeplug::GeneralSettingsElement::DateFormat
DM32UVCodeplug::GeneralSettingsElement::dateFormat() const {
  return getBit(Offset::dateFormat()) ? DateFormat::DDMMYYYY : DateFormat::YYYYMMDD;
}

void
DM32UVCodeplug::GeneralSettingsElement::setDateFormat(DateFormat format) {
  setBit(Offset::dateFormat(), DateFormat::DDMMYYYY == format);
}


bool
DM32UVCodeplug::GeneralSettingsElement::showClock() const {
  return getBit(Offset::showClock());
}

void
DM32UVCodeplug::GeneralSettingsElement::enableShowClock(bool enable) {
  setBit(Offset::showClock(), enable);
}


DM32UVCodeplug::GeneralSettingsElement::Color::Code
DM32UVCodeplug::GeneralSettingsElement::callColor() const {
  return Color::decode(getUInt8(Offset::callColor()));
}

void
DM32UVCodeplug::GeneralSettingsElement::setCallColor(Color::Code c) {
  setUInt8(Offset::callColor(), Color::encode(c));
}

DM32UVCodeplug::GeneralSettingsElement::Color::Code
DM32UVCodeplug::GeneralSettingsElement::standbyColor() const {
  return Color::decode(getUInt8(Offset::standbyColor()));
}

void
DM32UVCodeplug::GeneralSettingsElement::setStandbyColor(Color::Code c) {
  setUInt8(Offset::standbyColor(), Color::encode(c));
}

DM32UVCodeplug::GeneralSettingsElement::Color::Code
DM32UVCodeplug::GeneralSettingsElement::channelNameAColor() const {
  return Color::decode(getUInt8(Offset::channelNameAColor()));
}

void
DM32UVCodeplug::GeneralSettingsElement::setChannelNameAColor(Color::Code c) {
  setUInt8(Offset::channelNameAColor(), Color::encode(c));
}

DM32UVCodeplug::GeneralSettingsElement::Color::Code
DM32UVCodeplug::GeneralSettingsElement::channelNameBColor() const {
  return Color::decode(getUInt8(Offset::channelNameBColor()));
}

void
DM32UVCodeplug::GeneralSettingsElement::setChannelNameBColor(Color::Code c) {
  setUInt8(Offset::channelNameBColor(), Color::encode(c));
}

DM32UVCodeplug::GeneralSettingsElement::Color::Code
DM32UVCodeplug::GeneralSettingsElement::zoneNameAColor() const {
  return Color::decode(getUInt8(Offset::zoneNameAColor()));
}

void
DM32UVCodeplug::GeneralSettingsElement::setZoneNameAColor(Color::Code c) {
  setUInt8(Offset::zoneNameAColor(), Color::encode(c));
}

DM32UVCodeplug::GeneralSettingsElement::Color::Code
DM32UVCodeplug::GeneralSettingsElement::zoneNameBColor() const {
  return Color::decode(getUInt8(Offset::zoneNameBColor()));
}

void
DM32UVCodeplug::GeneralSettingsElement::setZoneNameBColor(Color::Code c) {
  setUInt8(Offset::zoneNameBColor(), Color::encode(c));
}


DM32UVCodeplug::GeneralSettingsElement::PositionFormat
DM32UVCodeplug::GeneralSettingsElement::positionFormat() const {
  return getBit(Offset::positionFormat()) ? PositionFormat::DMS : PositionFormat::DD;
}

void
DM32UVCodeplug::GeneralSettingsElement::setPositionFormat(PositionFormat format) {
  setBit(Offset::positionFormat(), PositionFormat::DMS == format);
}


DM32UVCodeplug::GeneralSettingsElement::GNSSMode
DM32UVCodeplug::GeneralSettingsElement::gnssMode() const {
  return (GNSSMode)getUInt2(Offset::gnssMode());
}

void
DM32UVCodeplug::GeneralSettingsElement::setGNSSMode(GNSSMode mode) {
  setUInt2(Offset::gnssMode(), (unsigned int )mode);
}


bool
DM32UVCodeplug::GeneralSettingsElement::gnssEnabled() const {
  return getBit(Offset::enableGNSS());
}

void
DM32UVCodeplug::GeneralSettingsElement::enableGNSS(bool enable) {
  setBit(Offset::enableGNSS(), enable);
}


QTimeZone
DM32UVCodeplug::GeneralSettingsElement::timeZone() const {
  return QTimeZone( (((int)getUInt8(Offset::timeZone()))-11) * 3600);
}

void
DM32UVCodeplug::GeneralSettingsElement::setTimeZone(const QTimeZone &timeZone) {
  setUInt8(Offset::timeZone(), timeZone.offsetFromUtc(QDateTime::currentDateTime())/3600+11);
}


Interval
DM32UVCodeplug::GeneralSettingsElement::posUpdatePeriod() const {
  return Interval::fromMinutes(getUInt8(Offset::posUpdatePeriod()));
}

void
DM32UVCodeplug::GeneralSettingsElement::setPosUpdatePeriod(const Interval &period) {
  setUInt8(Offset::posUpdatePeriod(), period.minutes());
}


DM32UVCodeplug::GeneralSettingsElement::RecordMode
DM32UVCodeplug::GeneralSettingsElement::recordMode() const {
  return (RecordMode) getUInt2(Offset::recordMode());
}

void
DM32UVCodeplug::GeneralSettingsElement::setRecordMode(RecordMode mode) {
  setUInt2(Offset::recordMode(), (unsigned int)mode);
}


bool
DM32UVCodeplug::GeneralSettingsElement::recordingEnabled() const {
  return getBit(Offset::enableRecording());
}

void
DM32UVCodeplug::GeneralSettingsElement::enableRecording(bool enable) {
  setBit(Offset::enableRecording(), enable);
}


bool
DM32UVCodeplug::GeneralSettingsElement::groupCallMatchEnabled() const {
  return getBit(Offset::groupCallMatch());
}

void
DM32UVCodeplug::GeneralSettingsElement::enableGroupCallMatch(bool enable) {
  setBit(Offset::groupCallMatch(), enable);
}


bool
DM32UVCodeplug::GeneralSettingsElement::privateCallMatchEnabled() const {
  return getBit(Offset::privateCallMatch());
}

void
DM32UVCodeplug::GeneralSettingsElement::enablePrivateCallMatch(bool enable) {
  setBit(Offset::privateCallMatch(), enable);
}


Interval
DM32UVCodeplug::GeneralSettingsElement::dmrCallHangTime() const {
  return Interval::fromMilliseconds( ((unsigned int)getUInt8(Offset::dmrCallHangTime())) * 500);
}

void
DM32UVCodeplug::GeneralSettingsElement::setDMRCallHangTime(const Interval &time) {
  setUInt8(Offset::dmrCallHangTime(), time.milliseconds()/500);
}


Interval
DM32UVCodeplug::GeneralSettingsElement::activeWaitTime() const {
  return Interval::fromMilliseconds(((unsigned int)getUInt8(Offset::activeWaitTime()))*30 + 300);
}

void
DM32UVCodeplug::GeneralSettingsElement::setActiveWaitTime(Interval waitTime) {
  setUInt8(Offset::activeWaitTime(), (waitTime.milliseconds()-300)/30);
}


unsigned int
DM32UVCodeplug::GeneralSettingsElement::activeRetries() const {
  return getUInt8(Offset::activeReties());
}

void
DM32UVCodeplug::GeneralSettingsElement::setActiveRetries(unsigned int retries) {
  setUInt8(Offset::activeReties(), retries);
}


Interval
DM32UVCodeplug::GeneralSettingsElement::dmrPreambleDuration() const {
  return Interval::fromMilliseconds(((unsigned int)getUInt8(Offset::dmrPreambleDur()))*120 + 120);
}

void
DM32UVCodeplug::GeneralSettingsElement::setDmrPreambleDuration(Interval duration) {
  setUInt8(Offset::dmrPreambleDur(), (duration.milliseconds()-120)/120);
}


bool
DM32UVCodeplug::GeneralSettingsElement::dmrRemoteMonitorEnabled() const {
  return  getBit(Offset::dmrMonitor());
}

void
DM32UVCodeplug::GeneralSettingsElement::enableDMRRemoteMonitor(bool enable) {
  setBit(Offset::dmrMonitor(), enable);
}


bool
DM32UVCodeplug::GeneralSettingsElement::dmrRemoteKillEnabled() const {
  return getBit(Offset::dmrKill());
}

void
DM32UVCodeplug::GeneralSettingsElement::enableDMRRemoteKill(bool enable) {
  setBit(Offset::dmrKill(), enable);
}


bool
DM32UVCodeplug::GeneralSettingsElement::dmrRemoteRadioCheckEnabled() const {
  return getBit(Offset::dmrRadioCheck());
}

void
DM32UVCodeplug::GeneralSettingsElement::enableDMRRemoteRadioCheck(bool enable) {
  setBit(Offset::dmrRadioCheck(), enable);
}


bool
DM32UVCodeplug::GeneralSettingsElement::dmrRemoteReenableEnabled() const {
  return getBit(Offset::dmrReenable());
}

void
DM32UVCodeplug::GeneralSettingsElement::enableDMRRemoteReenable(bool enable) {
  setBit(Offset::dmrReenable(), enable);
}


bool
DM32UVCodeplug::GeneralSettingsElement::dmrRXAlertEnabled() const {
  return getBit(Offset::dmrRXAlert());
}

void
DM32UVCodeplug::GeneralSettingsElement::enableDMRRXAlert(bool enable) {
  setBit(Offset::dmrRXAlert(), enable);
}


SMSExtension::Format
DM32UVCodeplug::GeneralSettingsElement::smsFormat() const {
  switch ((SMSFormat)getUInt2(Offset::smsFormat())) {
  case SMSFormat::Motorola: return SMSExtension::Format::Motorola;
  case SMSFormat::Hytera: return SMSExtension::Format::Hytera;
  case SMSFormat::DMR: return SMSExtension::Format::DMR;
  }
  return SMSExtension::Format::DMR;
}

void
DM32UVCodeplug::GeneralSettingsElement::setSMSFormat(SMSExtension::Format format) {
  switch (format) {
  case SMSExtension::Format::Motorola: setUInt2(Offset::smsFormat(), (unsigned int)SMSFormat::Motorola); break;
  case SMSExtension::Format::Hytera: setUInt2(Offset::smsFormat(), (unsigned int)SMSFormat::Hytera); break;
  case SMSExtension::Format::DMR: setUInt2(Offset::smsFormat(), (unsigned int)SMSFormat::DMR); break;
  }
}


bool
DM32UVCodeplug::GeneralSettingsElement::missedCallNotificationEnabled() const {
  return getBit(Offset::missedCallNotification());
}

void
DM32UVCodeplug::GeneralSettingsElement::enableMissedCallNotification(bool enable) {
  setBit(Offset::missedCallNotification(), enable);
}


Interval
DM32UVCodeplug::GeneralSettingsElement::dmrRemoteMonitorDuration() const {
  return Interval::fromSeconds(((unsigned int)getUInt8(Offset::dmrRemoteMonitorDuration()))*10 + 10);
}

void
DM32UVCodeplug::GeneralSettingsElement::setDMRRemoteMonitorDuration(Interval duration) {
  setUInt8(Offset::dmrRemoteMonitorDuration(), (duration.seconds()-10)/10);
}


DM32UVCodeplug::GeneralSettingsElement::TalkerAliasFormat
DM32UVCodeplug::GeneralSettingsElement::talkerAliasFormat() const {
  return getBit(Offset::dmrTalkerAliasFormat()) ? TalkerAliasFormat::UnicodeU16 : TalkerAliasFormat::ISO8;
}

void
DM32UVCodeplug::GeneralSettingsElement::setTalkerAliasFormat(TalkerAliasFormat format) {
  setBit(Offset::dmrTalkerAliasFormat(), TalkerAliasFormat::UnicodeU16 == format);
}


bool
DM32UVCodeplug::GeneralSettingsElement::txTalkerAliasEnabled() const {
  return getBit(Offset::txTalkerAlias());
}

void
DM32UVCodeplug::GeneralSettingsElement::enableTXTalkerAlias(bool enable) {
  setBit(Offset::txTalkerAlias(), enable);
}


DM32UVCodeplug::GeneralSettingsElement::TalkerAliasSource
DM32UVCodeplug::GeneralSettingsElement::talkerAliasSource() const {
  return getBit(Offset::talkerSource()) ? TalkerAliasSource::OverTheAir : TalkerAliasSource::CallsignDB;
}

void
DM32UVCodeplug::GeneralSettingsElement::setTalkerAliasSource(TalkerAliasSource source) {
  setBit(Offset::talkerSource(), TalkerAliasSource::OverTheAir == source);
}


DM32UVCodeplug::GeneralSettingsElement::DualStandbyMode
DM32UVCodeplug::GeneralSettingsElement::dualStandbyMode() const {
  return (DualStandbyMode) getUInt2(Offset::dualStandbyMode());
}

void
DM32UVCodeplug::GeneralSettingsElement::setDualStandbyMode(DualStandbyMode mode) {
  setUInt2(Offset::dualStandbyMode(), (unsigned int)mode);
}


DM32UVCodeplug::GeneralSettingsElement::VFO
DM32UVCodeplug::GeneralSettingsElement::mainVFO() const {
  return getBit(Offset::mainVFO()) ? VFO::B : VFO::A;
}

void
DM32UVCodeplug::GeneralSettingsElement::setMainVFO(VFO mainVFO) {
  setBit(Offset::mainVFO(), VFO::B == mainVFO);
}


DM32UVCodeplug::GeneralSettingsElement::VFODisplayMode
DM32UVCodeplug::GeneralSettingsElement::vfoDisplayModeA() const {
  return getBit(Offset::displayModeA()) ? VFODisplayMode::ChannelName : VFODisplayMode::Frequency;
}

void
DM32UVCodeplug::GeneralSettingsElement::setVFODisplayModeA(VFODisplayMode mode) {
  setBit(Offset::displayModeA(), VFODisplayMode::ChannelName == mode);
}


DM32UVCodeplug::GeneralSettingsElement::VFODisplayMode
DM32UVCodeplug::GeneralSettingsElement::vfoDisplayModeB() const {
  return getBit(Offset::displayModeB()) ? VFODisplayMode::ChannelName : VFODisplayMode::Frequency;
}

void
DM32UVCodeplug::GeneralSettingsElement::setVFODisplayModeB(VFODisplayMode mode) {
  setBit(Offset::displayModeB(), VFODisplayMode::ChannelName == mode);
}


DM32UVCodeplug::GeneralSettingsElement::VFOMode
DM32UVCodeplug::GeneralSettingsElement::vfoModeA() const {
  return getBit(Offset::vfoModeA()) ? VFOMode::VFO : VFOMode::Channel;
}

void
DM32UVCodeplug::GeneralSettingsElement::setVFOModeA(VFOMode mode) {
  setBit(Offset::vfoModeA(), VFOMode::VFO == mode);
}

DM32UVCodeplug::GeneralSettingsElement::VFOMode
DM32UVCodeplug::GeneralSettingsElement::vfoModeB() const {
  return getBit(Offset::vfoModeB()) ? VFOMode::VFO : VFOMode::Channel;
}

void
DM32UVCodeplug::GeneralSettingsElement::setVFOModeB(VFOMode mode) {
  setBit(Offset::vfoModeB(), VFOMode::VFO == mode);
}


bool
DM32UVCodeplug::GeneralSettingsElement::vfoModeDisabled() const {
  return getBit(Offset::disableVFOMode());
}

void
DM32UVCodeplug::GeneralSettingsElement::disableVFOMode(bool enable) {
  setBit(Offset::disableVFOMode(), enable);
}


Interval
DM32UVCodeplug::GeneralSettingsElement::dualStandbyHangTime() const {
  return Interval::fromMilliseconds(((unsigned int)getUInt8(Offset::dualStandbyHangTime()))*500);
}

void
DM32UVCodeplug::GeneralSettingsElement::setDualStandbyHangTime(Interval hangTime) {
  setUInt8(Offset::dualStandbyHangTime(), hangTime.milliseconds()/500);
}


bool
DM32UVCodeplug::GeneralSettingsElement::sideKeyLockEnabled() const {
  return getBit(Offset::sideKeyLock());
}

void
DM32UVCodeplug::GeneralSettingsElement::enableSideKeyLock(bool enable) {
  setBit(Offset::sideKeyLock(), enable);
}


bool
DM32UVCodeplug::GeneralSettingsElement::knobLockEnabled() const {
  return getBit(Offset::knobLock());
}

void
DM32UVCodeplug::GeneralSettingsElement::enableKnobLock(bool enable) {
  setBit(Offset::knobLock(), enable);
}


Interval
DM32UVCodeplug::GeneralSettingsElement::autoKeyLockDelay() const {
  if (! getBit(Offset::enableAutoKeyLock()))
    return Interval::infinity();
  return Interval::fromMinutes(5 + getUInt8(Offset::autoKeyLockDelay()));
}

void
DM32UVCodeplug::GeneralSettingsElement::setAutoKeyLockDelay(Interval delay) {
  if (delay.isInfinite()||delay.isNull()) {
    setBit(Offset::enableAutoKeyLock(), false);
  } else {
    setBit(Offset::enableAutoKeyLock(), true);
    setUInt8(Offset::autoKeyLockDelay(), delay.minutes()-5);
  }
}


DM32UVCodeplug::GeneralSettingsElement::KeyFunction::Function
DM32UVCodeplug::GeneralSettingsElement::sk1Short() const {
  return KeyFunction::decode(getUInt8(Offset::sk1Short()));
}

void
DM32UVCodeplug::GeneralSettingsElement::setSK1Short(KeyFunction::Function function) {
  setUInt8(Offset::sk1Short(), KeyFunction::encode(function));
}

DM32UVCodeplug::GeneralSettingsElement::KeyFunction::Function
DM32UVCodeplug::GeneralSettingsElement::sk1Long() const {
  return KeyFunction::decode(getUInt8(Offset::sk1Long()));
}

void
DM32UVCodeplug::GeneralSettingsElement::setSK1Long(KeyFunction::Function function) {
  setUInt8(Offset::sk1Long(), KeyFunction::encode(function));
}

DM32UVCodeplug::GeneralSettingsElement::KeyFunction::Function
DM32UVCodeplug::GeneralSettingsElement::sk2Short() const {
  return KeyFunction::decode(getUInt8(Offset::sk2Short()));
}

void
DM32UVCodeplug::GeneralSettingsElement::setSK2Short(KeyFunction::Function function) {
  setUInt8(Offset::sk2Short(), KeyFunction::encode(function));
}

DM32UVCodeplug::GeneralSettingsElement::KeyFunction::Function
DM32UVCodeplug::GeneralSettingsElement::sk2Long() const {
  return KeyFunction::decode(getUInt8(Offset::sk2Long()));
}

void
DM32UVCodeplug::GeneralSettingsElement::setSK2Long(KeyFunction::Function function) {
  setUInt8(Offset::sk2Long(), KeyFunction::encode(function));
}


DM32UVCodeplug::GeneralSettingsElement::KeyFunction::Function
DM32UVCodeplug::GeneralSettingsElement::p1Short() const {
  return KeyFunction::decode(getUInt8(Offset::p1Short()));
}

void
DM32UVCodeplug::GeneralSettingsElement::setP1Short(KeyFunction::Function function) {
  setUInt8(Offset::p1Short(), KeyFunction::encode(function));
}

DM32UVCodeplug::GeneralSettingsElement::KeyFunction::Function
DM32UVCodeplug::GeneralSettingsElement::p1Long() const {
  return KeyFunction::decode(getUInt8(Offset::p1Long()));
}

void
DM32UVCodeplug::GeneralSettingsElement::setP1Long(KeyFunction::Function function) {
  setUInt8(Offset::p1Long(), KeyFunction::encode(function));
}

DM32UVCodeplug::GeneralSettingsElement::KeyFunction::Function
DM32UVCodeplug::GeneralSettingsElement::p2Short() const {
  return KeyFunction::decode(getUInt8(Offset::p2Short()));
}

void
DM32UVCodeplug::GeneralSettingsElement::setP2Short(KeyFunction::Function function) {
  setUInt8(Offset::p2Short(), KeyFunction::encode(function));
}

DM32UVCodeplug::GeneralSettingsElement::KeyFunction::Function
DM32UVCodeplug::GeneralSettingsElement::p2Long() const {
  return KeyFunction::decode(getUInt8(Offset::p2Long()));
}

void
DM32UVCodeplug::GeneralSettingsElement::setP2Long(KeyFunction::Function function) {
  setUInt8(Offset::p2Long(), KeyFunction::encode(function));
}


Interval
DM32UVCodeplug::GeneralSettingsElement::longPressDuration() const {
  return Interval::fromSeconds(getUInt8(Offset::longPressDuration()) + 1);
}

void
DM32UVCodeplug::GeneralSettingsElement::setLongPressDuration(Interval duration) {
  duration = Limit::longPressDuration().limit(duration);
  setUInt8(Offset::longPressDuration(), duration.seconds()-1);
}


Interval
DM32UVCodeplug::GeneralSettingsElement::transmitTimeout() const {
  if (0 == getUInt8(Offset::transmitTimeout()))
    return Interval::infinity();
  return Interval::fromSeconds(((unsigned int)getUInt8(Offset::transmitTimeout()))*5 +  15);
}

void
DM32UVCodeplug::GeneralSettingsElement::setTransmitTimeout(Interval timeout) {
  if (timeout.isInfinite() || timeout.isNull()) {
    setUInt8(Offset::transmitTimeout(), 0);
  } else {
    timeout = Limit::transmitTimeout().limit(timeout);
    setUInt8(Offset::transmitTimeout(), (timeout.seconds()-15)/5);
  }
}


Interval
DM32UVCodeplug::GeneralSettingsElement::transmitTimeoutReminder() const {
  return Interval::fromSeconds(getUInt8(Offset::totReminder()));
}

void
DM32UVCodeplug::GeneralSettingsElement::setTransmitTimeoutReminder(Interval timeout) {
  setUInt8(Offset::totReminder(), timeout.seconds());
}


unsigned int
DM32UVCodeplug::GeneralSettingsElement::voxLevel() const {
  if (0 == getUInt8(Offset::voxLevel()))
    return 0;
  return (getUInt8(Offset::voxLevel())-1)*9/4 + 1;
}

void
DM32UVCodeplug::GeneralSettingsElement::setVOXLevel(unsigned int voxLevel) {
  if (0 == voxLevel)
    setUInt8(Offset::voxLevel(), 0);
  else
    setUInt8(Offset::voxLevel(), (voxLevel-1)*3/10 + 1);
}


Interval
DM32UVCodeplug::GeneralSettingsElement::voxDelay() const {
  return Interval::fromMilliseconds(((unsigned int)getUInt8(Offset::voxDelay()))*100);
}

void
DM32UVCodeplug::GeneralSettingsElement::setVoxDelay(Interval delay) {
  delay = Limit::voxDelay().limit(delay);
  setUInt8(Offset::voxDelay(), delay.milliseconds()/100);
}


DM32UVCodeplug::GeneralSettingsElement::PowerSaveMode
DM32UVCodeplug::GeneralSettingsElement::powerSaveMode() const {
  return (PowerSaveMode) getUInt2(Offset::powerSaveMode());
}

void
DM32UVCodeplug::GeneralSettingsElement::setPowerSaveMode(PowerSaveMode mode) {
  setUInt2(Offset::powerSaveMode(), (unsigned int)mode);
}


bool
DM32UVCodeplug::GeneralSettingsElement::weatherAlarmEnabled() const {
  return getBit(Offset::weatherAlarm());
}

void
DM32UVCodeplug::GeneralSettingsElement::enableWeatherAlarm(bool enable) {
  setBit(Offset::weatherAlarm(), enable);
}


bool
DM32UVCodeplug::GeneralSettingsElement::allLEDsDisabled() const {
  return getBit(Offset::disableLEDs());
}

void
DM32UVCodeplug::GeneralSettingsElement::disableAllLEDs(bool disable) {
  setBit(Offset::disableLEDs(), disable);
}


Frequency
DM32UVCodeplug::GeneralSettingsElement::tbstFrequency() const {
  switch ((TBSTFrequency)getUInt2(Offset::tbstFrequency())) {
  case TBSTFrequency::Hz1000: return Frequency::fromHz(1000);
  case TBSTFrequency::Hz1450: return Frequency::fromHz(1450);
  case TBSTFrequency::Hz1750: return Frequency::fromHz(1750);
  case TBSTFrequency::Hz2100: return Frequency::fromHz(2100);
  }
  return Frequency::fromHz(1750);
}

void
DM32UVCodeplug::GeneralSettingsElement::setTBSTFrequency(const Frequency &frequency) {
  if (frequency.inHz() <= 1000)
    setUInt2(Offset::tbstFrequency(), (unsigned int)TBSTFrequency::Hz1000);
  else if (frequency.inHz() <= 1450)
    setUInt2(Offset::tbstFrequency(), (unsigned int)TBSTFrequency::Hz1450);
  else if (frequency.inHz() <= 1750)
    setUInt2(Offset::tbstFrequency(), (unsigned int)TBSTFrequency::Hz1750);
  else
    setUInt2(Offset::tbstFrequency(), (unsigned int)TBSTFrequency::Hz2100);
}


DM32UVCodeplug::GeneralSettingsElement::STEMode
DM32UVCodeplug::GeneralSettingsElement::steMode() const {
  return (STEMode)getUInt2(Offset::steMode());
}

void
DM32UVCodeplug::GeneralSettingsElement::setSTEMode(STEMode mode) {
  setUInt2(Offset::steMode(), (unsigned int)mode);
}


unsigned int
DM32UVCodeplug::GeneralSettingsElement::fmMicLevel() const {
  return getUInt8(Offset::fmMicLevel())*10/5 + 1;
}

void
DM32UVCodeplug::GeneralSettingsElement::setFMMicLevel(unsigned int level) {
  return setUInt8(Offset::fmMicLevel(), (level*4)/10);
}

unsigned int
DM32UVCodeplug::GeneralSettingsElement::dmrMicLevel() const {
  return getUInt8(Offset::dmrMicLevel())*10/5 + 1;
}

void
DM32UVCodeplug::GeneralSettingsElement::setDMRMicLevel(unsigned int level) {
  return setUInt8(Offset::dmrMicLevel(), (level*4)/10);
}


bool
DM32UVCodeplug::GeneralSettingsElement::decode(Context &ctx, const ErrorStack &err) {
  ctx.config()->settings()->setIntroLine1(bootMessage1());
  ctx.config()->settings()->setIntroLine2(bootMessage2());
  ctx.config()->settings()->enableSpeech(voicePromptEnabled());
  ctx.config()->settings()->setVOX(voxLevel());
  if (transmitTimeout().isInfinite()) ctx.config()->settings()->setTOT(0);
  else ctx.config()->settings()->setTOT(transmitTimeout().seconds());
  ctx.config()->settings()->setMicLevel(std::max(fmMicLevel(), dmrMicLevel()));
  ctx.config()->smsExtension()->setFormat(smsFormat());

  return true;
}



/* ******************************************************************************************** *
 * Implementation of DM32UVCodeplug::APRSSettingsElement
 * ******************************************************************************************** */
DM32UVCodeplug::APRSSettingsElement::APRSSettingsElement(uint8_t *ptr)
  : Element{ptr, size()}
{
  // pass...
}

Interval
DM32UVCodeplug::APRSSettingsElement::updatePeriod() {
  if (0 == getUInt8(Offset::updatePeriod()))
    return Interval::infinity();
  return Interval::fromSeconds(((unsigned int)getUInt8(Offset::updatePeriod()))*30);
}

void
DM32UVCodeplug::APRSSettingsElement::setUpdatePeriod(Interval interval) {
  if (interval.isInfinite() || interval.isNull()) {
    setUInt8(Offset::updatePeriod(), 0);
  } else {
    interval = Limit::updatePeriod().limit(interval);
    setUInt8(Offset::updatePeriod(), (unsigned int)interval.seconds()/30);
  }
}


bool
DM32UVCodeplug::APRSSettingsElement::fixedLocationValid() const {
  return 0 != getUInt8(Offset::enableFixedLocation());
}

QGeoCoordinate
DM32UVCodeplug::APRSSettingsElement::fixedLocation() const {
  static QRegularExpression re(R"((\d+\.\d+)([NSEW]))");
  QString lat = readASCII(Offset::fixedLocationLatitude(), 10, 0x00);
  QString lon = readASCII(Offset::fixedLocationLongitude(), 10, 0x00);
  auto latMatch = re.match(lat), lonMatch = re.match(lon);

  if (latMatch.hasMatch() && lonMatch.hasMatch()) {
    return QGeoCoordinate{
      latMatch.captured(1).toDouble() * (latMatch.captured(2)=="S" ? -1 : 1),
      lonMatch.captured(1).toDouble() * (lonMatch.captured(2)=="W" ? -1 : 1),
    };
  }

  return {};
}

void
DM32UVCodeplug::APRSSettingsElement::setFixedLocation(const QGeoCoordinate &coordinate) {
  if (! coordinate.isValid()) {
    setUInt8(Offset::enableFixedLocation(), 0);
    return;
  }

  QString latString, lonString;
  latString.asprintf("%02.6f%c",std::abs(coordinate.latitude()), coordinate.latitude()<0 ? 'S' : 'N');
  lonString.asprintf("%03.5f%c",std::abs(coordinate.longitude()), coordinate.longitude()<0 ? 'W' : 'E');
  setUInt8(Offset::enableFixedLocation(), 1);
  writeASCII(Offset::fixedLocationLatitude(), latString, 10);
  writeASCII(Offset::fixedLocationLongitude(), lonString, 10);
}

void
DM32UVCodeplug::APRSSettingsElement::clearFixedLocation() {
  setUInt8(Offset::enableFixedLocation(), 0);
}


bool
DM32UVCodeplug::APRSSettingsElement::revertChannelIsCurrent(unsigned int n) {
  return 0 == getUInt16_le(Offset::revertChannelIndices()
    + n * Offset::betweenRevertChannelIndices());
}

unsigned int
DM32UVCodeplug::APRSSettingsElement::revertChannelIndex(unsigned int n) const {
  return getUInt16_le(Offset::revertChannelIndices()
    + n * Offset::betweenRevertChannelIndices()) -1 ;
}

void
DM32UVCodeplug::APRSSettingsElement::setRevertChannelIndex(unsigned int n, unsigned int idx) {
  setUInt16_le(Offset::revertChannelIndices()
    + n * Offset::betweenRevertChannelIndices(), idx+1);
}

void
DM32UVCodeplug::APRSSettingsElement::setRevertChannelToCurrent(unsigned int n) {
  setUInt16_le(Offset::revertChannelIndices()
    + n * Offset::betweenRevertChannelIndices(), 0);
}


Interval
DM32UVCodeplug::APRSSettingsElement::preWaveDelay() const {
  return Interval::fromMilliseconds((unsigned int)getUInt8(Offset::prewaveDelay()) * 100);
}

void
DM32UVCodeplug::APRSSettingsElement::setPreWaveDelay(const Interval &delay) {
  setUInt8(Offset::prewaveDelay(), delay.milliseconds()/100);
}


DMRContact::Type
DM32UVCodeplug::APRSSettingsElement::callType() const {
  switch ((CallType)getUInt8(Offset::callType())) {
  case CallType::Private: return DMRContact::Type::PrivateCall;
  case CallType::Group: return DMRContact::Type::GroupCall;
  }
  return DMRContact::Type::PrivateCall;
}

void
DM32UVCodeplug::APRSSettingsElement::setCallType(DMRContact::Type type) {
  switch (type) {
  case DMRContact::Type::PrivateCall:
    setUInt8(Offset::callType(), (unsigned int) CallType::Private);
    break;
  case DMRContact::Type::AllCall:
  case DMRContact::Type::GroupCall:
    setUInt8(Offset::callType(), (unsigned int) CallType::Group);
    break;
  }
}


unsigned int
DM32UVCodeplug::APRSSettingsElement::destinationId() const {
  return getUInt24_le(Offset::destinationId());
}

void
DM32UVCodeplug::APRSSettingsElement::setDestinationId(unsigned int id) {
  setUInt24_le(Offset::destinationId(), id);
}


bool
DM32UVCodeplug::APRSSettingsElement::decode(Context &ctx, const ErrorStack &err) {
  if (0 == destinationId())
    return true;

  auto aprs = new GPSSystem("DMR APRS System");
  if (updatePeriod().isFinite())
    aprs->setPeriod(updatePeriod().seconds());
  else
    aprs->setPeriod(0);

  ctx.add(aprs, 0);
  ctx.config()->posSystems()->add(aprs);

  return true;
}

bool
DM32UVCodeplug::APRSSettingsElement::link(Context &ctx, const ErrorStack &err) {
  if (0 == destinationId())
    return true;

  auto aprs = ctx.get<GPSSystem>(0);
  if (nullptr == aprs) {
    errMsg(err) << "Cannot resolve DMR APRS System at index 0!";
    return false;
  }

  auto cont = ctx.config()->contacts()->findDigitalContact(destinationId());
  if (nullptr == cont) {
    cont = new DMRContact(callType(), "DMR APRS Contact", destinationId());
    ctx.config()->contacts()->add(cont);
  }

  aprs->setContactObj(cont);
  if (revertChannelIsCurrent(0)) {
    aprs->resetRevertChannel();
  } else {
    if (! ctx.has<Channel>(revertChannelIndex(0))) {
      errMsg(err) << "Cannot resolve revert channel index " << revertChannelIndex(0) << ".";
      return false;
    }
    if (! ctx.get<Channel>(revertChannelIndex(0))->is<DMRChannel>()) {
      errMsg(err) << "Revert channel with index " << revertChannelIndex(0) << " must be DMR channel.";
      return false;
    }
    aprs->setRevertChannel(ctx.get<Channel>(revertChannelIndex(0))->as<DMRChannel>());
  }

  return true;
}


/* ******************************************************************************************** *
 * Implementation of DM32UVCodeplug
 * ******************************************************************************************** */
DM32UVCodeplug::DM32UVCodeplug(QObject *parent)
  : Codeplug{parent}
{
  addImage("Baofeng DM32UV Codeplug");
}


Config *
DM32UVCodeplug::preprocess(Config *config, const ErrorStack &err) const {
  Config *copy = Codeplug::preprocess(config, err);
  if (nullptr == copy) {
    errMsg(err) << "Cannot pre-process DM32UV codeplug.";
    return nullptr;
  }

  // Split dual-zones into two.
  ZoneSplitVisitor splitter;
  if (! splitter.process(copy, err)) {
    errMsg(err) << "Cannot pre-process DM32UV codeplug.";
    delete copy;
    return nullptr;
  }

  return copy;
}

bool
DM32UVCodeplug::postprocess(Config *config, const ErrorStack &err) const {
  if (! Codeplug::postprocess(config, err)) {
    errMsg(err) << "Cannot post-process DR1801A6 codeplug.";
    return false;
  }

  // Merge split zones into one.
  ZoneMergeVisitor merger;
  if (! merger.process(config, err)) {
    errMsg(err) << "Cannot post-process DR1801A6 codeplug.";
    return false;
  }

  return true;
}


bool
DM32UVCodeplug::index(Config *config, Context &ctx, const ErrorStack &err) const {
  Q_UNUSED(err)

  // All indices as 0-based. That is, the first channel gets index 0 etc.

  // There must be a default DMR radio ID.
  if (nullptr == ctx.config()->settings()->defaultId()) {
    errMsg(err) << "No default DMR radio ID specified.";
    errMsg(err) << "Cannot index codeplug for encoding for the BTECH DR-1801UV.";
    return false;
  }

  // Map radio IDs
  for (int i=0; i<ctx.config()->radioIDs()->count(); i++)
    ctx.add(ctx.config()->radioIDs()->getId(i), i);

  // Map DMR contacts
  for (int i=0, d=0; i<config->contacts()->count(); i++) {
    if (config->contacts()->contact(i)->is<DMRContact>()) {
      ctx.add(config->contacts()->contact(i)->as<DMRContact>(), d); d++;
    }
  }

  // Map rx group lists
  for (int i=0; i<config->rxGroupLists()->count(); i++)
    ctx.add(config->rxGroupLists()->list(i), i);

  // Map channels
  for (int i=0; i<config->channelList()->count(); i++)
    ctx.add(config->channelList()->channel(i), i);

  // Map zones
  for (int i=0; i<config->zones()->count(); i++)
    ctx.add(config->zones()->zone(i), i);

  // Map scan lists
  for (int i=0; i<config->scanlists()->count(); i++)
    ctx.add(config->scanlists()->scanlist(i), i);

  return true;
}


bool
DM32UVCodeplug::encode(Config *config, const Flags &flags, const ErrorStack &err) {
  errMsg(err) << "Not implemented yet.";
  return false;
}


bool
DM32UVCodeplug::decode(Config *config, const ErrorStack &err) {
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
DM32UVCodeplug::decodeElements(Context &ctx, const ErrorStack &err) {
  if (! decodeChannels(ctx, err)) {
    errMsg(err) << "Cannot decode channels.";
    return false;
  }

  if (! decodeContacts(ctx, err)) {
    errMsg(err) << "Cannot decode contacts.";
    return false;
  }

  if (! GroupListBankElement(data(Offset::groupListBank())).decode(ctx, err)) {
    errMsg(err) << "Cannot decode group lists.";
    return false;
  }

  if (! RadioIdBankElement(data(Offset::radioIdBank())).decode(ctx, err)) {
    errMsg(err) << "Cannot decode radio IDs.";
    return false;
  }

  if (! decodeZones(ctx, err)) {
    errMsg(err) << "Cannot decode zones.";
    return false;
  }

  if (! ScanListBankElement(data(Offset::scanListBank())).decode(ctx, err)) {
    errMsg(err) << "Cannot decode scan lists.";
    return false;
  }

  if (! RoamingChannelBankElement(data(Offset::roamingChannelBank())).decode(ctx, err)) {
    errMsg(err) << "Cannot decode roaming channels.";
    return false;
  }

  if (! RoamingZoneBankElement(data(Offset::roamingZoneBank())).decode(ctx, err)) {
    errMsg(err) << "Cannot decode roaming zones.";
    return false;
  }

  if (! GeneralSettingsElement(data(Offset::generalSettings())).decode(ctx, err)) {
    errMsg(err) << "Cannot decode general settings.";
    return false;
  }

  if (! APRSSettingsElement(data(Offset::aprsSettings())).decode(ctx, err)) {
    errMsg(err) << "Cannot decode APRS settings.";
    return false;
  }

  return true;
}


bool
DM32UVCodeplug::linkElements(Context &ctx, const ErrorStack &err) {
  if (! linkChannels(ctx, err)) {
    errMsg(err) << "Cannot link channels.";
    return false;
  }

  if (! GroupListBankElement(data(Offset::groupListBank())).link(ctx, err)) {
    errMsg(err) << "Cannot link group lists.";
    return false;
  }

  if (! linkZones(ctx, err)) {
    errMsg(err) << "Cannot link zones.";
    return false;
  }

  if (! ScanListBankElement(data(Offset::scanListBank())).link(ctx, err)) {
    errMsg(err) << "Cannot link scan lists.";
    return false;
  }

  if (! RoamingZoneBankElement(data(Offset::roamingZoneBank())).link(ctx, err)) {
    errMsg(err) << "Cannot link roaming zones.";
    return false;
  }

  if (! APRSSettingsElement(data(Offset::aprsSettings())).link(ctx, err)) {
    errMsg(err) << "Cannot link APRS settings.";
    return false;
  }

  return true;
}


bool
DM32UVCodeplug::decodeChannels(Context &ctx, const ErrorStack &err) {
  ChannelBankElement bank(data(Offset::channelBanks()));
  for (unsigned int i=0; i<bank.channelCount(); i++) {
    unsigned int blockNumber  = i / ChannelBankElement::Limit::channelsPerBlock();
    unsigned int indexInBlock = i % ChannelBankElement::Limit::channelsPerBlock();
    uint32_t addr = Offset::channelBanks()
                    + (0 == blockNumber ? ChannelBankElement::Offset::channelBlock0()
                                        : blockNumber * ChannelBankElement::Offset::betweenChannelBlocks())
                    + indexInBlock * ChannelElement::size();
    // Create channel
    auto ch = ChannelElement(data(addr)).decode(ctx, err);
    if (nullptr == ch) {
      errMsg(err) << "Cannot decode channel at index " << i << ".";
      return false;
    }
    // Store in config and context
    ctx.config()->channelList()->add(ch);
    ctx.add(ch, i);
  }

  return true;
}


bool
DM32UVCodeplug::linkChannels(Context &ctx, const ErrorStack &err) {
  ChannelBankElement bank(data(Offset::channelBanks()));
  for (unsigned int i=0; i<bank.channelCount(); i++) {
    unsigned int blockNumber  = i / ChannelBankElement::Limit::channelsPerBlock();
    unsigned int indexInBlock = i % ChannelBankElement::Limit::channelsPerBlock();
    uint32_t addr = Offset::channelBanks()
                    + (0 == blockNumber ? ChannelBankElement::Offset::channelBlock0()
                                        : blockNumber * ChannelBankElement::Offset::betweenChannelBlocks())
                    + indexInBlock * ChannelElement::size();
    // get channel
    auto ch = ctx.get<Channel>(i);
    // link channel
    if (! ChannelElement(data(addr)).link(ch, ctx, err)) {
      errMsg(err) << "Cannot link channel at index " << i << ".";
      return false;
    }
  }

  return true;
}


bool
DM32UVCodeplug::decodeContacts(Context &ctx, const ErrorStack &err) {
  ContactIndexElement index(data(Offset::contactIndex()));
  for (unsigned int i=0,c=0; (i<ContactBankElement::Limit::contacts()) && (c < index.contactCount()); i++) {
    if (! index.bitmap().isEncoded(i))
      continue;
    unsigned int blockIndex    = i / ContactBankElement::Limit::contactsPerBlock(),
      indexInBlock = i % ContactBankElement::Limit::contactsPerBlock();
    uint32_t addr = Offset::contactBanks()
                    + blockIndex * ContactBankElement::Offset::betweenBlocks()
                    + indexInBlock * ContactElement::size();
    auto contact = ContactElement(data(addr)).decode(ctx, err);
    logDebug() << "Decoded contact '" << contact->name()
               << "' (" << contact->number() << ") at index " << i << ".";
    ctx.add(contact, c++);
    ctx.config()->contacts()->add(contact);
  }

  return true;
}


bool
DM32UVCodeplug::decodeZones(Context &ctx, const ErrorStack &err) {
  ZoneBankElement bank(data(Offset::zoneBanks()));
  for (unsigned int i=0; i<bank.count(); i++) {
    unsigned int blockNumber  = i / ZoneBankElement::Limit::zonesPerBlock();
    unsigned int indexInBlock = i % ZoneBankElement::Limit::zonesPerBlock();
    uint32_t addr = Offset::zoneBanks()
                    + (0 == blockNumber ? ZoneBankElement::Offset::zones0()
                                        : blockNumber * ZoneBankElement::Offset::betweenBlocks())
                    + indexInBlock * ZoneElement::size();
    // Create zone
    auto zone = ZoneElement(data(addr)).decode(ctx, err);
    if (nullptr == zone) {
      errMsg(err) << "Cannot decode zone at index " << i << ".";
      return false;
    }
    // Store in config and context
    ctx.config()->zones()->add(zone);
    ctx.add(zone, i);
  }

  return true;
}


bool
DM32UVCodeplug::linkZones(Context &ctx, const ErrorStack &err) {
  ZoneBankElement bank(data(Offset::zoneBanks()));
  for (unsigned int i=0; i<bank.count(); i++) {
    unsigned int blockNumber  = i / ZoneBankElement::Limit::zonesPerBlock();
    unsigned int indexInBlock = i % ZoneBankElement::Limit::zonesPerBlock();
    uint32_t addr = Offset::zoneBanks()
                    + (0 == blockNumber ? ZoneBankElement::Offset::zones0()
                                        : blockNumber * ZoneBankElement::Offset::betweenBlocks())
                    + indexInBlock * ZoneElement::size();
    // get zone
    auto zone = ctx.get<Zone>(i);
    // link zone
    if (! ZoneElement(data(addr)).link(zone, ctx, err)) {
      errMsg(err) << "Cannot link zone at index " << i << ".";
      return false;
    }
  }

  return true;
}
