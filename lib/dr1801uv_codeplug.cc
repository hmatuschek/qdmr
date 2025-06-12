#include "dr1801uv_codeplug.hh"
#include "logger.hh"
#include "utils.hh"
#include "zone.hh"
#include "config.hh"
#include "intermediaterepresentation.hh"

#include <QRegularExpression>


/* ******************************************************************************************** *
 * Implementation of DR1801UVCodeplug::ChannelBankElement
 * ******************************************************************************************** */
DR1801UVCodeplug::ChannelBankElement::ChannelBankElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

DR1801UVCodeplug::ChannelBankElement::ChannelBankElement(uint8_t *ptr)
  : Element(ptr, ChannelBankElement::size())
{
  // pass...
}

void
DR1801UVCodeplug::ChannelBankElement::clear() {
  memset(_data, 0, ChannelBankElement::size());
}

unsigned int
DR1801UVCodeplug::ChannelBankElement::channelCount() const {
  return getUInt16_le(Offset::channelCount());
}
void
DR1801UVCodeplug::ChannelBankElement::setChannelCount(unsigned int count) {
  count = std::min(Limit::channelCount(), count);
  setUInt16_le(Offset::channelCount(), count);
}

DR1801UVCodeplug::ChannelElement
DR1801UVCodeplug::ChannelBankElement::channel(unsigned int index) const {
  return ChannelElement(_data + Offset::channel() + index*ChannelElement::size());
}

QString
DR1801UVCodeplug::ChannelBankElement::channelName(unsigned int index) const {
  return readASCII(Offset::channelName() + index*Limit::channelNameLength(), Limit::channelNameLength(), 0x00);
}
void
DR1801UVCodeplug::ChannelBankElement::setChannelName(unsigned int index, const QString &name) {
  writeASCII(Offset::channelName() + index*Limit::channelNameLength(), name, Limit::channelNameLength(), 0x00);
}

bool
DR1801UVCodeplug::ChannelBankElement::decode(Context &ctx, const ErrorStack &err) const {
  for (unsigned int i=0,j=0; i<Limit::channelCount() && j<channelCount(); i++) {
    ChannelElement ch = channel(i);
    if (! ch.isValid())
      continue;
    j++;
    Channel *obj = ch.toChannelObj(ctx, err);
    if (nullptr == obj) {
      errMsg(err) << "Cannot decode channel at index " << i << ".";
      return false;
    }
    // Set channel name
    obj->setName(channelName(i));
    // Add channel to context and config
    ctx.add(obj, ch.index());
    ctx.config()->channelList()->add(obj);
  }

  return true;
}

bool
DR1801UVCodeplug::ChannelBankElement::link(Context &ctx, const ErrorStack &err) const {
  for (unsigned int i=0,j=0; i<Limit::channelCount() && j<channelCount(); i++) {
    ChannelElement ch = channel(i);
    if (! ch.isValid())
      continue;
    j++;
    if (! ctx.has<Channel>(ch.index())) {
      errMsg(err) << "Cannot link channel at index " << i
                  << ". Channel not defined.";
      return false;
    }
    Channel *obj = ctx.get<Channel>(ch.index());
    if (! ch.linkChannelObj(obj, ctx, err)) {
      errMsg(err) << "Cannot link channel at index " << i << ".";
      return false;
    }
  }

  return true;
}

bool
DR1801UVCodeplug::ChannelBankElement::encode(Context &ctx, const ErrorStack &err) {
  unsigned int n = std::min(Limit::channelCount(), ctx.count<Channel>());
  setChannelCount(n);

  for (unsigned int i=0; i<n; i++) {
    ChannelElement ch = channel(i);
    if (! ch.encode(ctx.get<Channel>(i), ctx, err)) {
      errMsg(err) << "Cannot encode channel '" << ctx.get<Channel>(i)
                  << "' at index " << i << ".";
      return false;
    }
    ch.setIndex(i);
    setChannelName(i, ctx.get<Channel>(i)->name());
  }

  return true;
}


/* ******************************************************************************************** *
 * Implementation of DR1801UVCodeplug::ChannelElement
 * ******************************************************************************************** */
DR1801UVCodeplug::ChannelElement::ChannelElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

DR1801UVCodeplug::ChannelElement::ChannelElement(uint8_t *ptr)
  : Element(ptr, ChannelElement::size())
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

unsigned int
DR1801UVCodeplug::ChannelElement::index() const {
  return getUInt16_le(Offset::index());
}
void
DR1801UVCodeplug::ChannelElement::setIndex(unsigned int idx) {
  setUInt16_le(Offset::index(), idx);
}

DR1801UVCodeplug::ChannelElement::Type
DR1801UVCodeplug::ChannelElement::channelType() const {
  return (Type)getUInt8(Offset::channelType());
}
void
DR1801UVCodeplug::ChannelElement::setChannelType(Type type) {
  setUInt8(Offset::channelType(), (uint8_t)type);
}

Channel::Power
DR1801UVCodeplug::ChannelElement::power() const {
  switch ((Power)getUInt8(Offset::power())) {
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
    setUInt8(Offset::power(), (uint8_t)Power::Low);
    break;
  case Channel::Power::Mid:
  case Channel::Power::High:
  case Channel::Power::Max:
    setUInt8(Offset::power(), (uint8_t)Power::High);
    break;
  }
}

Frequency
DR1801UVCodeplug::ChannelElement::rxFrequency() const {
  return Frequency::fromHz(getUInt32_le(Offset::rxFrequency()));
}
void
DR1801UVCodeplug::ChannelElement::setRXFrequency(Frequency MHz) {
  setUInt32_le(Offset::rxFrequency(), MHz.inHz());
}
Frequency
DR1801UVCodeplug::ChannelElement::txFrequency() const {
  return Frequency::fromHz(getUInt32_le(Offset::txFrequency()));
}
void
DR1801UVCodeplug::ChannelElement::setTXFrequency(Frequency MHz) {
  setUInt32_le(Offset::txFrequency(), MHz.inHz());
}

bool
DR1801UVCodeplug::ChannelElement::hasTransmitContact() const {
  return 0 != getUInt16_le(Offset::transmitContactIndex());
}
unsigned int
DR1801UVCodeplug::ChannelElement::transmitContactIndex() const {
  return getUInt16_le(Offset::transmitContactIndex())-1;
}
void
DR1801UVCodeplug::ChannelElement::setTransmitContactIndex(unsigned int index) {
  setUInt16_le(Offset::transmitContactIndex(), index+1);
}
void
DR1801UVCodeplug::ChannelElement::clearTransmitContactIndex() {
  setUInt16_le(Offset::transmitContactIndex(), 0);
}

DR1801UVCodeplug::ChannelElement::Admit
DR1801UVCodeplug::ChannelElement::admitCriterion() const {
  return (Admit) getUInt8(Offset::admitCriterion());
}
void
DR1801UVCodeplug::ChannelElement::setAdmitCriterion(Admit admit) {
  setUInt8(Offset::admitCriterion(), (uint8_t)admit);
}

unsigned int
DR1801UVCodeplug::ChannelElement::colorCode() const {
  return getUInt8(Offset::colorCode());
}
void
DR1801UVCodeplug::ChannelElement::setColorCode(unsigned int cc) {
  setUInt8(Offset::colorCode(), cc);
}

DMRChannel::TimeSlot
DR1801UVCodeplug::ChannelElement::timeSlot() const {
  switch ((TimeSlot)getUInt8(Offset::timeSlot())) {
  case TimeSlot::TS1: return DMRChannel::TimeSlot::TS1;
  case TimeSlot::TS2: return DMRChannel::TimeSlot::TS2;
  }
  return DMRChannel::TimeSlot::TS1;
}
void
DR1801UVCodeplug::ChannelElement::setTimeSlot(DMRChannel::TimeSlot ts) {
  switch (ts) {
  case DMRChannel::TimeSlot::TS1: setUInt8(Offset::timeSlot(), (uint8_t)TimeSlot::TS1); break;
  case DMRChannel::TimeSlot::TS2: setUInt8(Offset::timeSlot(), (uint8_t)TimeSlot::TS2); break;
  }
}

bool
DR1801UVCodeplug::ChannelElement::hasEncryptionKey() const {
  return 0 != getUInt8(Offset::encryptionKeyIndex());
}
unsigned int
DR1801UVCodeplug::ChannelElement::encryptionKeyIndex() const {
  return getUInt8(Offset::encryptionKeyIndex())-1;
}
void
DR1801UVCodeplug::ChannelElement::setEncryptionKeyIndex(unsigned int index) {
  setUInt8(Offset::encryptionKeyIndex(), index+1);
}
void
DR1801UVCodeplug::ChannelElement::clearEncryptionKeyIndex() {
  setUInt8(Offset::encryptionKeyIndex(), 0);
}

bool
DR1801UVCodeplug::ChannelElement::dcdm() const {
  return getBit(Offset::dcdm().byte, Offset::dcdm().bit);
}
void
DR1801UVCodeplug::ChannelElement::enableDCDM(bool enable) {
  setBit(Offset::dcdm().byte, Offset::dcdm().bit, enable);
}
bool
DR1801UVCodeplug::ChannelElement::confirmPrivateCall() const {
  return getBit(Offset::confirmPivateCall().byte, Offset::confirmPivateCall().bit);
}
void
DR1801UVCodeplug::ChannelElement::enablePrivateCallConfirmation(bool enable) {
  setBit(Offset::confirmPivateCall().byte, Offset::confirmPivateCall().bit, enable);
}

DR1801UVCodeplug::ChannelElement::SignalingMode
DR1801UVCodeplug::ChannelElement::signalingMode() const {
  return (SignalingMode) getUInt8(Offset::signalingMode());
}
void
DR1801UVCodeplug::ChannelElement::setSignalingMode(SignalingMode mode) {
  setUInt8(Offset::signalingMode(), (uint8_t)mode);
}

bool
DR1801UVCodeplug::ChannelElement::hasAlarmSystem() const {
  return 0 != getUInt8(Offset::alarmSystemIndex());
}
unsigned int
DR1801UVCodeplug::ChannelElement::alarmSystemIndex() const {
  return getUInt8(Offset::alarmSystemIndex())-1;
}
void
DR1801UVCodeplug::ChannelElement::setAlarmSystemIndex(unsigned int index) {
  setUInt8(Offset::alarmSystemIndex(), index+1);
}
void
DR1801UVCodeplug::ChannelElement::clearAlarmSystemIndex() {
  setUInt8(Offset::alarmSystemIndex(), 0);
}

FMChannel::Bandwidth
DR1801UVCodeplug::ChannelElement::bandwidth() const {
  switch ((Bandwidth)getUInt8(Offset::bandwidth())) {
  case Bandwidth::Narrow: return FMChannel::Bandwidth::Narrow;
  case Bandwidth::Wide: return FMChannel::Bandwidth::Wide;
  }
  return FMChannel::Bandwidth::Narrow;
}
void
DR1801UVCodeplug::ChannelElement::setBandwidth(FMChannel::Bandwidth bw) {
  switch (bw){
  case FMChannel::Bandwidth::Narrow: setUInt8(Offset::bandwidth(), (uint8_t)Bandwidth::Narrow); break;
  case FMChannel::Bandwidth::Wide:   setUInt8(Offset::bandwidth(), (uint8_t)Bandwidth::Wide); break;
  }
}

bool
DR1801UVCodeplug::ChannelElement::autoScanEnabled() const {
  return 0x01 == getUInt8(Offset::autoScan());
}
void
DR1801UVCodeplug::ChannelElement::enableAutoScan(bool enable) {
  setUInt8(Offset::autoScan(), enable ? 0x01 : 0x00);
}
bool
DR1801UVCodeplug::ChannelElement::hasScanList() const {
  return 0x00 != getUInt8(Offset::scanListIndex());
}
unsigned int
DR1801UVCodeplug::ChannelElement::scanListIndex() const {
  return getUInt8(Offset::scanListIndex())-1;
}
void
DR1801UVCodeplug::ChannelElement::setScanListIndex(unsigned int index) {
  setUInt8(Offset::scanListIndex(), index+1);
}
void
DR1801UVCodeplug::ChannelElement::clearScanListIndex() {
  setUInt8(Offset::scanListIndex(), 0);
}

SelectiveCall
DR1801UVCodeplug::ChannelElement::rxTone() const {
  uint16_t ctcss_dcs = getUInt16_le(Offset::rxSubtoneCode());
  SubToneType type = (SubToneType)getUInt8(Offset::rxSubtoneType());
  DCSMode dcsMode = (DCSMode)getUInt8(Offset::rxDCSMode());
  switch (type) {
  case SubToneType::None: return SelectiveCall();
  case SubToneType::CTCSS: return SelectiveCall(float(ctcss_dcs)/10);
  case SubToneType::DCS: return SelectiveCall(ctcss_dcs, DCSMode::Inverted == dcsMode);
  }
  return SelectiveCall();
}
void
DR1801UVCodeplug::ChannelElement::setRXTone(const SelectiveCall &code) {
  uint16_t ctcss_dcs = 0;
  SubToneType type = SubToneType::None;
  DCSMode dcsMode = DCSMode::Normal;
  if (code.isCTCSS()) {
    type = SubToneType::CTCSS;
    ctcss_dcs = code.Hz()*10;
  } else if (code.isDCS()) {
    type = SubToneType::DCS;
    ctcss_dcs = code.octalCode();
    dcsMode = code.isInverted() ? DCSMode::Inverted : DCSMode::Normal;
  }
  setUInt16_le(Offset::rxSubtoneCode(), ctcss_dcs);
  setUInt8(Offset::rxSubtoneType(), (uint8_t)type);
  setUInt8(Offset::rxDCSMode(), (uint8_t)dcsMode);
}

SelectiveCall
DR1801UVCodeplug::ChannelElement::txTone() const {
  uint16_t ctcss_dcs = getUInt16_le(Offset::txSubtoneCode());
  SubToneType type = (SubToneType)getUInt8(Offset::txSubtoneType());
  DCSMode dcsMode = (DCSMode)getUInt8(Offset::txDCSMode());
  switch (type) {
  case SubToneType::None: return SelectiveCall();
  case SubToneType::CTCSS: return SelectiveCall(float(ctcss_dcs)/10);
  case SubToneType::DCS: return SelectiveCall(ctcss_dcs, DCSMode::Inverted == dcsMode);
  }
  return SelectiveCall();
}
void
DR1801UVCodeplug::ChannelElement::setTXTone(const SelectiveCall &code) {
  uint16_t ctcss_dcs = 0;
  SubToneType type = SubToneType::None;
  DCSMode dcsMode = DCSMode::Normal;
  if (code.isCTCSS()) {
    type = SubToneType::CTCSS;
    ctcss_dcs = code.Hz()*10;
  } else if (code.isDCS()) {
    type = SubToneType::DCS;
    ctcss_dcs = code.octalCode();
    dcsMode = code.isInverted() ? DCSMode::Inverted : DCSMode::Normal;
  }
  setUInt16_le(Offset::txSubtoneCode(), ctcss_dcs);
  setUInt8(Offset::txSubtoneType(), (uint8_t)type);
  setUInt8(Offset::txDCSMode(), (uint8_t)dcsMode);
}

bool
DR1801UVCodeplug::ChannelElement::talkaround() const {
  return getBit(Offset::talkaround(), 7);
}
void
DR1801UVCodeplug::ChannelElement::enableTalkaround(bool enable) {
  setBit(Offset::talkaround(), 7, enable);
}

bool
DR1801UVCodeplug::ChannelElement::hasPTTID() const {
  return 0 != getUInt8(Offset::pttIDIndex());
}
unsigned int
DR1801UVCodeplug::ChannelElement::pttIDIndex() const {
  return getUInt8(Offset::pttIDIndex())-1;
}
void
DR1801UVCodeplug::ChannelElement::setPTTIDIndex(unsigned int idx) {
  setUInt8(Offset::pttIDIndex(), idx+1);
}
void
DR1801UVCodeplug::ChannelElement::clearPTTID() {
  setUInt8(Offset::pttIDIndex(), 0);
}

bool
DR1801UVCodeplug::ChannelElement::hasGroupList() const {
  return 0 != getUInt8(Offset::groupListIndex());
}
unsigned int
DR1801UVCodeplug::ChannelElement::groupListIndex() const {
  return getUInt8(Offset::groupListIndex())-1;
}
void
DR1801UVCodeplug::ChannelElement::setGroupListIndex(unsigned int index) {
  setUInt8(Offset::groupListIndex(), index+1);
}
void
DR1801UVCodeplug::ChannelElement::clearGroupListIndex() {
  setUInt8(Offset::groupListIndex(), 0);
}

bool
DR1801UVCodeplug::ChannelElement::loneWorker() const {
  return 0x01 == getUInt8(Offset::loneWorker());
}
void
DR1801UVCodeplug::ChannelElement::enableLoneWorker(bool enable) {
  setUInt8(Offset::loneWorker(), enable ? 0x01 : 0x00);
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

bool
DR1801UVCodeplug::ChannelElement::encode(Channel *channel, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err);

  // Encode common properties
  setPower(channel->power());
  setRXFrequency(channel->rxFrequency());
  setTXFrequency(channel->txFrequency());
  if (channel->scanList())
    setScanListIndex(ctx.index(channel->scanList()));
  else
    clearScanListIndex();

  // Encode type specific settings
  if (channel->is<FMChannel>()) {
    FMChannel *fm = channel->as<FMChannel>();
    setChannelType(Type::FM);
    setBandwidth(fm->bandwidth());
    setRXTone(fm->rxTone());
    setTXTone(fm->txTone());
    switch (fm->admit()) {
    case FMChannel::Admit::Always: setAdmitCriterion(Admit::Always); break;
    case FMChannel::Admit::Free:   setAdmitCriterion(Admit::ChannelFree); break;
    case FMChannel::Admit::Tone:   setAdmitCriterion(Admit::ColorCode_or_Tone); break;
    }
  } else if (channel->is<DMRChannel>()) {
    DMRChannel *dmr = channel->as<DMRChannel>();
    setChannelType(Type::DMR);
    setBandwidth(FMChannel::Bandwidth::Narrow);
    if (dmr->txContactObj())
      setTransmitContactIndex(ctx.index(dmr->txContactObj()));
    else
      clearTransmitContactIndex();
    switch (dmr->admit()) {
    case DMRChannel::Admit::Always:    setAdmitCriterion(Admit::Always); break;
    case DMRChannel::Admit::Free:      setAdmitCriterion(Admit::ChannelFree); break;
    case DMRChannel::Admit::ColorCode: setAdmitCriterion(Admit::ColorCode_or_Tone); break;
    }
    setColorCode(dmr->colorCode());
    setTimeSlot(dmr->timeSlot());
    if (dmr->commercialExtension() && dmr->commercialExtension()->encryptionKey())
      setEncryptionKeyIndex(ctx.index(dmr->commercialExtension()->encryptionKey()));
    else
      clearEncryptionKeyIndex();
    if (dmr->groupListObj())
      setGroupListIndex(ctx.index(dmr->groupListObj()));
    else
      clearGroupListIndex();
  }

  return true;
}


/* ******************************************************************************************** *
 * Implementation of DR1801UVCodeplug::ContactBankElement
 * ******************************************************************************************** */
DR1801UVCodeplug::ContactBankElement::ContactBankElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

DR1801UVCodeplug::ContactBankElement::ContactBankElement(uint8_t *ptr)
  : Element(ptr, ContactBankElement::size())
{
  // pass...
}

void
DR1801UVCodeplug::ContactBankElement::clear() {
  memset(_data, 0, ContactBankElement::size());
}

unsigned int
DR1801UVCodeplug::ContactBankElement::contactCount() const {
  return getUInt16_le(Offset::contactCount());
}
void
DR1801UVCodeplug::ContactBankElement::setContactCount(unsigned int count) {
  count = std::min(Limit::contactCount(), count);
  setUInt16_le(Offset::contactCount(), count);
}

unsigned int
DR1801UVCodeplug::ContactBankElement::firstIndex() const {
  return getUInt16_le(Offset::firstIndex())-1;
}
void
DR1801UVCodeplug::ContactBankElement::setFirstIndex(unsigned int index) {
  setUInt16_le(Offset::firstIndex(), index+1);
}

DR1801UVCodeplug::ContactElement
DR1801UVCodeplug::ContactBankElement::contact(unsigned int index) const {
  return ContactElement(_data + Offset::contacts() + index*ContactElement::size());
}

bool
DR1801UVCodeplug::ContactBankElement::decode(Context &ctx, const ErrorStack &err) const {
  // Get first element in list
  unsigned int currentIndex = firstIndex();
  ContactElement currentContact = contact(currentIndex);

  for (unsigned int i=0; i<contactCount(); i++) {
    DMRContact *obj = currentContact.toContactObj(ctx, err);
    if (nullptr == obj) {
      errMsg(err) << "Cannot decode contact element at index " << currentIndex;
      return false;
    }
    // Add to context and config
    ctx.add(obj, currentIndex);
    ctx.config()->contacts()->add(obj);
    // continue with successor
    if (currentContact.hasSuccessor()) {
      currentIndex = currentContact.successorIndex();
      currentContact = contact(currentIndex);
    }
  }

  return true;
}

bool
DR1801UVCodeplug::ContactBankElement::link(Context &ctx, const ErrorStack &err) const {
  if (0 == contactCount())
    return true;

  unsigned int currentIndex = firstIndex();
  for (unsigned int i=0; i<contactCount(); i++) {
    ContactElement currentContact = contact(currentIndex);
    if (! ctx.has<DMRContact>(currentIndex)) {
      errMsg(err) << "Cannot link contact at index " << currentIndex << ", not defined.";
    }
    DMRContact *obj = ctx.get<DMRContact>(currentIndex);
    if (! currentContact.linkContactObj(obj, ctx, err)) {
      errMsg(err) << "Cannot link contact element at index " << currentIndex;
      return false;
    }
    currentIndex = currentContact.successorIndex();
  }

  return true;
}

bool
DR1801UVCodeplug::ContactBankElement::encode(Context &ctx, const ErrorStack &err) {
  unsigned int n = std::min(Limit::contactCount(), ctx.count<DMRContact>());
  setContactCount(n); setFirstIndex(0);
  for (unsigned int i=0; i<n; i++) {
    ContactElement contact = this->contact(i);
    if (! contact.encode(ctx.get<DMRContact>(i), ctx, err)) {
      errMsg(err) << "Cannot encode contact '" << ctx.get<DMRContact>(i)->name()
                  << "' at index " << i << ".";
      return false;
    }
    if ((i+1) < n)
      contact.setSuccessorIndex(i+1);
    else
      contact.clearSuccessorIndex();
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
  : Element(ptr, ContactElement::size())
{
  // pass...
}

bool
DR1801UVCodeplug::ContactElement::isValid() const {
  return 0 != getUInt8(Offset::nameLength()) &&  // has name
      0 != getUInt24_le(0x0004) && // has DMR ID
      0 != getUInt8(0x0007);       // has call type
}

void
DR1801UVCodeplug::ContactElement::clear() {
  memset(_data, 0, _size);
}

bool
DR1801UVCodeplug::ContactElement::hasSuccessor() const {
  return 0x0000 != getUInt16_le(Offset::successorIndex());
}
uint16_t
DR1801UVCodeplug::ContactElement::successorIndex() const {
  return getUInt16_le(Offset::successorIndex())-1;
}
void
DR1801UVCodeplug::ContactElement::setSuccessorIndex(uint16_t index) {
  setUInt16_le(Offset::successorIndex(), index+1);
}
void
DR1801UVCodeplug::ContactElement::clearSuccessorIndex() {
  setUInt16_le(Offset::successorIndex(), 0);
}

uint32_t
DR1801UVCodeplug::ContactElement::dmrID() const {
  if (DMRContact::AllCall == type())
    return 0xffffff;
  return getUInt24_le(Offset::dmrID());
}
void
DR1801UVCodeplug::ContactElement::setDMRID(uint32_t id) {
  setUInt24_le(Offset::dmrID(), id);
}

DMRContact::Type
DR1801UVCodeplug::ContactElement::type() const {
  switch ((CallType)getUInt8(Offset::callType())) {
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
    setUInt8(Offset::callType(), (uint8_t)CallType::AllCall);
    setDMRID(0xffffff);
    break;
  case DMRContact::PrivateCall: setUInt8(Offset::callType(), (uint8_t)CallType::PrivateCall); break;
  case DMRContact::GroupCall: setUInt8(Offset::callType(), (uint8_t)CallType::GroupCall); break;
  }
}

QString
DR1801UVCodeplug::ContactElement::name() const {
  return readASCII(Offset::name(), getUInt8(Offset::nameLength()), 0x00);
}
void
DR1801UVCodeplug::ContactElement::setName(const QString &name) {
  uint8_t len = std::min(Limit::nameLength(), (unsigned int)name.size());
  setUInt8(Offset::nameLength(), len);
  writeASCII(Offset::name(), name, len, 0x00);
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

bool
DR1801UVCodeplug::ContactElement::encode(DMRContact *contact, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(err);
  setName(contact->name());
  setCallType(contact->type());
  setDMRID(contact->number());
  return true;
}


/* ******************************************************************************************** *
 * Implementation of DR1801UVCodeplug::GroupListBankElement
 * ******************************************************************************************** */
DR1801UVCodeplug::GroupListBankElement::GroupListBankElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

DR1801UVCodeplug::GroupListBankElement::GroupListBankElement(uint8_t *ptr)
  : Element(ptr, GroupListBankElement::size())
{
  // pass...
}

void
DR1801UVCodeplug::GroupListBankElement::clear() {
  memset(_data, 0, _size);
}

unsigned int
DR1801UVCodeplug::GroupListBankElement::groupListCount() const {
  return getUInt8(Offset::groupListCount());
}
void
DR1801UVCodeplug::GroupListBankElement::setGroupListCount(unsigned int count) {
  count = std::min(Limit::groupListCount(), count);
  setUInt8(Offset::groupListCount(), count);
}

DR1801UVCodeplug::GroupListElement
DR1801UVCodeplug::GroupListBankElement::groupList(unsigned int index) const {
  return GroupListElement(_data + Offset::groupLists() + index*GroupListElement::size());
}

bool
DR1801UVCodeplug::GroupListBankElement::decode(Context &ctx, const ErrorStack &err) const {
  for (unsigned int i=0,j=0; i<Limit::groupListCount() && j<groupListCount(); i++) {
    GroupListElement gl = groupList(i);
    if (! gl.isValid())
      continue;
    j++;
    RXGroupList *obj = gl.toGroupListObj(ctx, err);
    if (nullptr == obj) {
      errMsg(err) << "Cannot decode group list at index " << i << ".";
      return false;
    }
    // Add group list to context and config
    ctx.add(obj, gl.index());
    ctx.config()->rxGroupLists()->add(obj);
  }

  return true;
}

bool
DR1801UVCodeplug::GroupListBankElement::link(Context &ctx, const ErrorStack &err) const {
  for (unsigned int i=0,j=0; i<Limit::groupListCount() && j<groupListCount(); i++) {
    GroupListElement gl = groupList(i);
    if (! gl.isValid())
      continue;
    j++;
    if (! ctx.has<RXGroupList>(gl.index())) {
      errMsg(err) << "Cannot link group list at index " << i
                  << ". Group list not defined.";
      return false;
    }
    if (! gl.linkGroupListObj(ctx.get<RXGroupList>(gl.index()), ctx, err)) {
      errMsg(err) << "Cannot link group list at index " << i << ".";
      return false;
    }
  }

  return true;
}

bool
DR1801UVCodeplug::GroupListBankElement::encode(Context &ctx, const ErrorStack &err) {
  unsigned int n = std::min(Limit::groupListCount(), ctx.count<RXGroupList>());
  setGroupListCount(n);
  for (unsigned int i=0; i<n; i++) {
    GroupListElement lst = groupList(i);
    if (! lst.encode(ctx.get<RXGroupList>(i), ctx, err)) {
      errMsg(err) << "Cannot encode group list '" << ctx.get<RXGroupList>(i)->name()
                  << "' at index " << i << ".";
      return false;
    }
    lst.setIndex(i);
  }

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
  : Element(ptr, GroupListElement::size())
{
  // pass...
}

void
DR1801UVCodeplug::GroupListElement::clear() {
  memset(_data, 0, _size);
}

bool
DR1801UVCodeplug::GroupListElement::isValid() const {
  return 0 != getUInt16_le(Offset::index());
}

unsigned int
DR1801UVCodeplug::GroupListElement::index() const {
  return getUInt16_le(Offset::index())-1;
}
void
DR1801UVCodeplug::GroupListElement::setIndex(unsigned int index) {
  setUInt16_le(Offset::index(), index+1);
}

unsigned int
DR1801UVCodeplug::GroupListElement::count() const {
  return getUInt16_le(Offset::count());
}
void
DR1801UVCodeplug::GroupListElement::setCount(unsigned int n) {
  n = std::min(Limit::members(), n);
  setUInt16_le(Offset::count(), n);
}

bool
DR1801UVCodeplug::GroupListElement::hasMemberIndex(unsigned int n) const {
  return 0 != getUInt16_le(Offset::members() + n*0x02);
}
unsigned int DR1801UVCodeplug::GroupListElement::memberIndex(unsigned int n) const {
  return getUInt16_le(Offset::members() + n*0x02) - 1;
}
void
DR1801UVCodeplug::GroupListElement::setMemberIndex(unsigned int n, unsigned int index) {
  setUInt16_le(Offset::members() + n*0x02, index+1);
}
void
DR1801UVCodeplug::GroupListElement::clearMemberIndex(unsigned int n) {
  setUInt16_le(Offset::members() + n*0x02, 0);
}

RXGroupList *
DR1801UVCodeplug::GroupListElement::toGroupListObj(Context &ctx, const ErrorStack &err) const {
  Q_UNUSED(ctx); Q_UNUSED(err);
  // Simply derive name from index
  return new RXGroupList(QString("Group List %1").arg(index()+1));
}

bool
DR1801UVCodeplug::GroupListElement::linkGroupListObj(RXGroupList *list, Context &ctx, const ErrorStack &err) const {
  if (! isValid())
    return false;

  for (unsigned int i=0; i<count(); i++) {
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

bool
DR1801UVCodeplug::GroupListElement::encode(RXGroupList *list, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)
  unsigned int n = std::min(Limit::members(), (unsigned int)list->count());
  setCount(n);
  for (unsigned int i=0; i<n; i++) {
    setMemberIndex(i, ctx.index(list->contact(i)));
  }
  return true;
}


/* ******************************************************************************************** *
 * Implementation of DR1801UVCodeplug::ZoneBankElement
 * ******************************************************************************************** */
DR1801UVCodeplug::ZoneBankElement::ZoneBankElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

DR1801UVCodeplug::ZoneBankElement::ZoneBankElement(uint8_t *ptr)
  : Element(ptr, ZoneBankElement::size())
{
  // pass...
}

void
DR1801UVCodeplug::ZoneBankElement::clear() {
  memset(_data, 0, _size);
}

unsigned int
DR1801UVCodeplug::ZoneBankElement::zoneCount() const {
  return getUInt8(Offset::zoneCount());
}
void
DR1801UVCodeplug::ZoneBankElement::setZoneCount(unsigned int count) {
  count = std::min(Limit::zoneCount(), count);
  setUInt8(Offset::zoneCount(), count);
}

unsigned int
DR1801UVCodeplug::ZoneBankElement::upZoneIndex() const {
  return getUInt16_le(Offset::upZoneIndex());
}
void
DR1801UVCodeplug::ZoneBankElement::setUpZoneIndex(unsigned int index) {
  setUInt16_le(Offset::upZoneIndex(), index);
}

unsigned int
DR1801UVCodeplug::ZoneBankElement::downZoneIndex() const {
  return getUInt16_le(Offset::downZoneIndex());
}
void
DR1801UVCodeplug::ZoneBankElement::setDownZoneIndex(unsigned int index) {
  setUInt16_le(Offset::downZoneIndex(), index);
}

DR1801UVCodeplug::ZoneElement
DR1801UVCodeplug::ZoneBankElement::zone(unsigned int index) const {
  return ZoneElement(_data + Offset::zones() + index*ZoneElement::size());
}

bool
DR1801UVCodeplug::ZoneBankElement::decode(Context &ctx, const ErrorStack &err) const {
  for (unsigned int i=0,j=0; i<Limit::zoneCount() && j<zoneCount(); i++) {
    ZoneElement zone(this->zone(i));
    if (! zone.isValid())
      continue;
    j++;

    Zone *obj = zone.toZoneObj(ctx, err);
    if (nullptr == obj) {
      errMsg(err) << "Cannot create zone at index " << i << ".";
      return false;
    }

    // Store zone in context
    ctx.add(obj, zone.index());
    // Store zone in config
    ctx.config()->zones()->add(obj);
  }

  return true;
}

bool
DR1801UVCodeplug::ZoneBankElement::link(Context &ctx, const ErrorStack &err) const {
  for (unsigned int i=0, j=0; i<Limit::zoneCount() && j<zoneCount(); i++) {
    ZoneElement zone(this->zone(i));
    if (! zone.isValid())
      continue;
    j++;

    if (! ctx.has<Zone>(zone.index())) {
      errMsg(err) << "Cannot link zone at index " << i << ", not defined.";
      return false;
    }

    if (! zone.linkZoneObj(ctx.get<Zone>(zone.index()), ctx, err)) {
      errMsg(err) << "Cannot link zone at index " << i << ".";
      return false;
    }
  }

  return true;
}

bool
DR1801UVCodeplug::ZoneBankElement::encode(Context &ctx, const ErrorStack &err) {
  setZoneCount(ctx.count<Zone>());
  for (unsigned int i=0; i<ctx.count<Zone>(); i++) {
    ZoneElement zone = this->zone(i);
    if (! zone.encode(ctx.get<Zone>(i), ctx, err)) {
      errMsg(err) << "Cannot encode zone bank.";
      return false;
    }
    zone.setIndex(i);
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
  : Element(ptr, ZoneElement::size())
{
  // pass...
}

void
DR1801UVCodeplug::ZoneElement::clear() {
  memset(_data, 0, _size);
}

bool
DR1801UVCodeplug::ZoneElement::isValid() const {
  // Read name-length and channel count
  return (0 != getUInt8(Offset::nameLength())) && (0 != getUInt8(Offset::numEntries()));
}

QString
DR1801UVCodeplug::ZoneElement::name() const {
  uint8_t n = getUInt8(Offset::nameLength());
  return readASCII(Offset::name(), n, 0x00);
}
void
DR1801UVCodeplug::ZoneElement::setName(const QString &name) {
  uint8_t n = std::min(qsizetype(32), name.length());
  setUInt8(Offset::nameLength(), n);
  writeASCII(Offset::name(), name, Limit::nameLength(), 0x00);
}

unsigned int
DR1801UVCodeplug::ZoneElement::index() const {
  return getUInt16_le(Offset::index());
}
void
DR1801UVCodeplug::ZoneElement::setIndex(unsigned int index) {
  setUInt16_le(Offset::index(), index);
}

unsigned int
DR1801UVCodeplug::ZoneElement::numEntries() const {
  return getUInt8(Offset::numEntries());
}
unsigned int
DR1801UVCodeplug::ZoneElement::entryIndex(unsigned int n) {
  n = std::min(Limit::memberCount(), n);
  return getUInt16_le(Offset::members()+2*n);
}
void
DR1801UVCodeplug::ZoneElement::setEntryIndex(unsigned int n, unsigned int index) {
  n = std::min(Limit::memberCount(), n);
  setUInt16_le(Offset::members() + 2*n, index);
}

Zone *
DR1801UVCodeplug::ZoneElement::toZoneObj(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx)

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

bool
DR1801UVCodeplug::ZoneElement::encode(Zone *zone, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  setName(zone->name());
  unsigned int n = std::min(Limit::memberCount(), (unsigned int)zone->A()->count());
  setUInt8(Offset::numEntries(), n);
  for (unsigned int i=0; i<n; i++) {
    setEntryIndex(i, ctx.index(zone->A()->get(i)->as<Channel>()));
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
  : Element(ptr, SettingsElement::size())
{
  // psas...
}

void
DR1801UVCodeplug::SettingsElement::clear() {
  memset(_data, 0, _size);
}

unsigned int
DR1801UVCodeplug::SettingsElement::dmrID() const {
  return getUInt24_le(Offset::dmrID());
}
void
DR1801UVCodeplug::SettingsElement::setDMRID(unsigned int id) {
  setUInt24_le(Offset::dmrID(), id);
}

DR1801UVCodeplug::SettingsElement::PowerSaveMode
DR1801UVCodeplug::SettingsElement::powerSaveMode() const {
  if (0x00 == getUInt8(Offset::powerSaveEnabled())) {
    // Power save disabled
    return PowerSaveMode::Off;
  }
  return (PowerSaveMode)getUInt8(Offset::powerSaveMode());
}
void
DR1801UVCodeplug::SettingsElement::setPowerSaveMode(PowerSaveMode mode) {
  setUInt8(Offset::powerSaveEnabled(), PowerSaveMode::Off != mode ? 0x01 : 0x00);
  setUInt8(Offset::powerSaveMode(), (uint8_t) mode);
}

unsigned int
DR1801UVCodeplug::SettingsElement::voxSensitivity() const {
  if (0x00 == getUInt8(Offset::voxEnabled())) {
    return 0;
  }
  return getUInt8(Offset::voxSensitivity())*10/3;
}
void
DR1801UVCodeplug::SettingsElement::setVOXSensitivity(unsigned int sens) {
  if (0 == sens) {
    setUInt8(Offset::voxEnabled(), 0x00);
  } else {
    setUInt8(Offset::voxEnabled(), 0x01);
    setUInt8(Offset::voxSensitivity(), 1+sens*2/10);
  }
}
unsigned int
DR1801UVCodeplug::SettingsElement::voxDelay() const {
  return getUInt8(Offset::voxDelay())*500;
}
void
DR1801UVCodeplug::SettingsElement::setVOXDelay(unsigned int ms) {
  setUInt8(Offset::voxDelay(), ms/500);
}

bool
DR1801UVCodeplug::SettingsElement::encryptionEnabled() const {
  return 0x01 == getUInt8(Offset::encryptionEnabled());
}
void
DR1801UVCodeplug::SettingsElement::enableEncryption(bool enable) {
  setUInt8(Offset::encryptionEnabled(), enable ? 0x01 : 0x00);
}

bool
DR1801UVCodeplug::SettingsElement::keyLockEnabled() const {
  return 0x01 == getUInt8(Offset::keyLockEnabled());
}
void
DR1801UVCodeplug::SettingsElement::enableKeyLock(bool enable) {
  setUInt8(Offset::keyLockEnabled(), enable ? 0x01 : 0x00);
}
unsigned int
DR1801UVCodeplug::SettingsElement::keyLockDelay() const {
  return getUInt8(Offset::keyLockDelay());
}
void
DR1801UVCodeplug::SettingsElement::setKeyLockDelay(unsigned int sec) {
  setUInt8(Offset::keyLockDelay(), sec);
}
bool
DR1801UVCodeplug::SettingsElement::lockSideKey1() const {
  return getBit(Offset::lockSideKey1().byte, Offset::lockSideKey1().bit);
}
void
DR1801UVCodeplug::SettingsElement::enableLockSideKey1(bool enable) {
  setBit(Offset::lockSideKey1().byte, Offset::lockSideKey1().bit, enable);
}
bool
DR1801UVCodeplug::SettingsElement::lockSideKey2() const {
  return getBit(Offset::lockSideKey2().byte, Offset::lockSideKey2().bit);
}
void
DR1801UVCodeplug::SettingsElement::enableLockSideKey2(bool enable) {
  setBit(Offset::lockSideKey2().byte, Offset::lockSideKey2().bit, enable);
}
bool
DR1801UVCodeplug::SettingsElement::lockPTT() const {
  return getBit(Offset::lockPTT().byte, Offset::lockPTT().bit);
}
void
DR1801UVCodeplug::SettingsElement::enableLockPTT(bool enable) {
  setBit(Offset::lockPTT().byte, Offset::lockPTT().bit, enable);
}

DR1801UVCodeplug::SettingsElement::Language
DR1801UVCodeplug::SettingsElement::language() const {
  return (Language) getUInt8(Offset::language());
}
void
DR1801UVCodeplug::SettingsElement::setLanguage(Language lang) {
  setUInt8(Offset::language(), (uint8_t)lang);
}

DR1801UVCodeplug::SettingsElement::SquelchMode
DR1801UVCodeplug::SettingsElement::squelchMode() const {
  return (SquelchMode) getUInt8(Offset::squelchMode());
}
void
DR1801UVCodeplug::SettingsElement::setSquelchMode(SquelchMode mode) {
  setUInt8(Offset::squelchMode(), (uint8_t)mode);
}

bool
DR1801UVCodeplug::SettingsElement::rogerTonesEnabled() const {
  return 0x01 == getUInt8(Offset::rogerTonesEnabled());
}
void
DR1801UVCodeplug::SettingsElement::enableRogerTones(bool enable) {
  setUInt8(Offset::rogerTonesEnabled(), enable ? 0x01 : 0x00);
}
bool
DR1801UVCodeplug::SettingsElement::dmrCallOutToneEnabled() const {
  return getBit(Offset::dmrCallOutToneEnabled());
}
void
DR1801UVCodeplug::SettingsElement::enableDMRCallOutTone(bool enable) {
  setBit(Offset::dmrCallOutToneEnabled(), enable);
}
bool
DR1801UVCodeplug::SettingsElement::fmCallOutToneEnabled() const {
  return getBit(Offset::fmCallOutToneEnabled());
}
void
DR1801UVCodeplug::SettingsElement::enableFMCallOutTone(bool enable) {
  setBit(Offset::fmCallOutToneEnabled(), enable);
}
bool
DR1801UVCodeplug::SettingsElement::dmrVoiceEndToneEnabled() const {
  return getBit(Offset::dmrVoiceEndToneEnabled());
}
void
DR1801UVCodeplug::SettingsElement::enableDMRVoiceEndTone(bool enable) {
  setBit(Offset::dmrVoiceEndToneEnabled(), enable);
}
bool
DR1801UVCodeplug::SettingsElement::fmVoiceEndToneEnabled() const {
  return getBit(Offset::fmVoiceEndToneEnabled());
}
void
DR1801UVCodeplug::SettingsElement::enableFMVoiceEndTone(bool enable) {
  setBit(Offset::fmVoiceEndToneEnabled(), enable);
}
bool
DR1801UVCodeplug::SettingsElement::dmrCallEndToneEnabled() const {
  return getBit(Offset::dmrCallEndToneEnabled());
}
void
DR1801UVCodeplug::SettingsElement::enableDMRCallEndTone(bool enable) {
  setBit(Offset::dmrCallEndToneEnabled(), enable);
}
bool
DR1801UVCodeplug::SettingsElement::messageToneEnabled() const {
  return getBit(Offset::messageToneEnabled());
}
void
DR1801UVCodeplug::SettingsElement::enableMessageTone(bool enable) {
  setBit(Offset::messageToneEnabled(), enable);
}

DR1801UVCodeplug::SettingsElement::RingTone
DR1801UVCodeplug::SettingsElement::ringTone() const {
  return (RingTone) getUInt8(Offset::ringTone());
}
void
DR1801UVCodeplug::SettingsElement::setRingTone(RingTone tone) {
  setUInt8(Offset::ringTone(), (uint8_t)tone);
}

QString
DR1801UVCodeplug::SettingsElement::radioName() const {
  return readASCII(Offset::radioName(), Limit::radioNameLength(), 0x00);
}
void
DR1801UVCodeplug::SettingsElement::setRadioName(const QString &name) {
  writeASCII(Offset::radioName(), name, Limit::radioNameLength(), 0x00);
}

float
DR1801UVCodeplug::SettingsElement::reverseBurstFrequency() const {
  return float(getUInt16_le(Offset::reverseBurstFrequency()))/10;
}
void
DR1801UVCodeplug::SettingsElement::setReverseBurstFrequency(float Hz) {
  setUInt16_le(Offset::reverseBurstFrequency(), Hz*10);
}

DR1801UVCodeplug::SettingsElement::BacklightTime
DR1801UVCodeplug::SettingsElement::backlightTime() const {
  return (BacklightTime) getUInt8(Offset::backlightTime());
}
void
DR1801UVCodeplug::SettingsElement::setBacklightTime(BacklightTime time) {
  setUInt8(Offset::backlightTime(), (uint8_t)time);
}

bool
DR1801UVCodeplug::SettingsElement::campandingEnabled() const {
  return 0x01 == getUInt8(Offset::campandingEnabled());
}
void
DR1801UVCodeplug::SettingsElement::enableCampanding(bool enable) {
  setUInt8(Offset::campandingEnabled(), enable ? 0x01 : 0x00);
}

DR1801UVCodeplug::SettingsElement::TuningMode
DR1801UVCodeplug::SettingsElement::tunigModeUp() const {
  return (TuningMode) getUInt8(Offset::tuningModeUp());
}
void
DR1801UVCodeplug::SettingsElement::setTuningModeUp(TuningMode mode) {
  setUInt8(Offset::tuningModeUp(), (uint8_t)mode);
}
DR1801UVCodeplug::SettingsElement::TuningMode
DR1801UVCodeplug::SettingsElement::tunigModeDown() const {
  return (TuningMode) getUInt8(Offset::tunigModeDown());
}
void
DR1801UVCodeplug::SettingsElement::setTuningModeDown(TuningMode mode) {
  setUInt8(Offset::tunigModeDown(), (uint8_t)mode);
}

DR1801UVCodeplug::SettingsElement::DisplayMode
DR1801UVCodeplug::SettingsElement::displayMode() const {
  return (DisplayMode)getUInt8(Offset::displayMode());
}
void
DR1801UVCodeplug::SettingsElement::setDisplayMode(DisplayMode mode) {
  setUInt8(Offset::displayMode(), (uint8_t)mode);
}

DR1801UVCodeplug::SettingsElement::DualWatchMode
DR1801UVCodeplug::SettingsElement::dualWatchMode() const {
  return (DualWatchMode) getUInt8(Offset::dualWatchMode());
}
void
DR1801UVCodeplug::SettingsElement::setDualWatchMode(DualWatchMode mode) {
  setUInt8(Offset::dualWatchMode(), (uint8_t)mode);
}

DR1801UVCodeplug::SettingsElement::ScanMode
DR1801UVCodeplug::SettingsElement::scanMode() const {
  return (ScanMode) getUInt8(Offset::scanMode());
}
void
DR1801UVCodeplug::SettingsElement::setScanMode(ScanMode mode) {
  setUInt8(Offset::scanMode(), (uint8_t)mode);
}

DR1801UVCodeplug::SettingsElement::BootScreen
DR1801UVCodeplug::SettingsElement::bootScreen() const {
  return (BootScreen) getUInt8(Offset::bootScreen());
}
void
DR1801UVCodeplug::SettingsElement::setBootScreen(BootScreen mode) {
  setUInt8(Offset::bootScreen(), (uint8_t) mode);
}

QString
DR1801UVCodeplug::SettingsElement::bootLine1() const {
  return readASCII(Offset::bootLine1(), Limit::bootLineLength(), 0x00);
}
void
DR1801UVCodeplug::SettingsElement::setBootLine1(const QString &line) {
  writeASCII(Offset::bootLine1(), line, Limit::bootLineLength(), 0x00);
}
QString
DR1801UVCodeplug::SettingsElement::bootLine2() const {
  return readASCII(Offset::bootLine2(), Limit::bootLineLength(), 0x00);
}
void
DR1801UVCodeplug::SettingsElement::setBootLine2(const QString &line) {
  writeASCII(Offset::bootLine2(), line, Limit::bootLineLength(), 0x00);
}

bool
DR1801UVCodeplug::SettingsElement::ledEnabled() const {
  return 0x01 == getUInt8(Offset::ledEnabled());
}
void
DR1801UVCodeplug::SettingsElement::enableLED(bool enabled) {
  setUInt8(Offset::ledEnabled(), enabled ? 0x01 : 0x00);
}

unsigned int
DR1801UVCodeplug::SettingsElement::loneWorkerResponseTime() const {
  return getUInt8(Offset::loneWorkerResponseTime());
}
void
DR1801UVCodeplug::SettingsElement::setLoneWorkerResponseTime(unsigned int sec) {
  setUInt8(Offset::loneWorkerResponseTime(), sec);
}
unsigned int
DR1801UVCodeplug::SettingsElement::loneWorkerReminderTime() const {
  return getUInt8(Offset::loneWorkerReminderTime());
}
void
DR1801UVCodeplug::SettingsElement::setLoneWorkerReminderTime(unsigned int sec) {
  setUInt8(Offset::loneWorkerReminderTime(), sec);
}

bool
DR1801UVCodeplug::SettingsElement::bootPasswordEnabled() const {
  return getBit(Offset::bootPasswordEnabled());
}
QString
DR1801UVCodeplug::SettingsElement::bootPassword() const {
  return readASCII(Offset::bootPassword(), Limit::bootPasswordLength(), 0x00);
}
void
DR1801UVCodeplug::SettingsElement::setBootPassword(const QString &passwd) {
  setBit(Offset::bootPasswordEnabled(), true);
  setUInt8(Offset::boolPasswordLength(),
           std::min(Limit::bootPasswordLength(), (unsigned int)passwd.length()));
  writeASCII(Offset::bootPassword(), passwd, Limit::bootPasswordLength(), 0x00);
}
void
DR1801UVCodeplug::SettingsElement::clearBootPassword() {
  setBit(Offset::bootPasswordEnabled(), false);
  setUInt8(Offset::boolPasswordLength(), 0);
  writeASCII(Offset::bootPassword(), "", Limit::bootPasswordLength(), 0x00);
}

bool
DR1801UVCodeplug::SettingsElement::progPasswordEnabled() const {
  return getBit(Offset::progPasswordEnabled());
}
QString
DR1801UVCodeplug::SettingsElement::progPassword() const {
  return readASCII(Offset::progPassword(), Limit::progPasswordLength(), 0x00);
}
void
DR1801UVCodeplug::SettingsElement::setProgPassword(const QString &passwd) {
  setBit(Offset::progPasswordEnabled(), true);
  setUInt8(Offset::progPasswordLength(),
           std::min(Limit::progPasswordLength(), (unsigned int)passwd.length()));
  writeASCII(Offset::progPassword(), passwd, Limit::progPasswordLength(), 0x00);
}
void
DR1801UVCodeplug::SettingsElement::clearProgPassword() {
  setBit(Offset::progPasswordEnabled(), false);
  setUInt8(Offset::progPasswordLength(), 0);
  writeASCII(Offset::progPassword(), "", Limit::progPasswordLength(), 0x00);
}

bool
DR1801UVCodeplug::SettingsElement::updateConfig(Config *config, const ErrorStack &err) {
  Q_UNUSED(err);

  // Store radio ID
  config->radioIDs()->add(new DMRRadioID(radioName(), dmrID()));
  config->settings()->setDefaultId(config->radioIDs()->getId(0));

  // Handle VOX settings.
  config->settings()->setVOX(voxSensitivity());

  // Handle intro lines
  config->settings()->setIntroLine1(bootLine1());
  config->settings()->setIntroLine2(bootLine2());

  return true;
}

bool
DR1801UVCodeplug::SettingsElement::fromConfig(Config *config, const ErrorStack &err) {
  // Store radio ID
  DMRRadioID *id = config->settings()->defaultId();
  if (nullptr == id) {
    errMsg(err) << "Cannot encode radio ID and name. No default DMR radio ID.";
    return false;
  }
  setRadioName(id->name());
  setDMRID(id->number());

  setVOXSensitivity(config->settings()->vox());
  setBootLine1(config->settings()->introLine1());
  setBootLine2(config->settings()->introLine2());

  return true;
}

/* ******************************************************************************************** *
 * Implementation of DR1801UVCodeplug::ScanListBankElement
 * ******************************************************************************************** */
DR1801UVCodeplug::ScanListBankElement::ScanListBankElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

DR1801UVCodeplug::ScanListBankElement::ScanListBankElement(uint8_t *ptr)
  : Element(ptr, ScanListBankElement::size())
{
  // pass...
}

void
DR1801UVCodeplug::ScanListBankElement::clear() {
  memset(_data, 0, _size);
}

unsigned int
DR1801UVCodeplug::ScanListBankElement::scanListCount() const {
  return getUInt8(Offset::scanListCount());
}
void
DR1801UVCodeplug::ScanListBankElement::setScanListCount(unsigned int count) {
  count = std::min(Limit::scanListCount(), count);
  setUInt8(Offset::scanListCount(), count);
}

DR1801UVCodeplug::ScanListElement
DR1801UVCodeplug::ScanListBankElement::scanList(unsigned int index) const {
  return ScanListElement(_data + Offset::scanLists() + index*ScanListElement::size());
}

bool
DR1801UVCodeplug::ScanListBankElement::decode(Context &ctx, const ErrorStack &err) const {
  for (unsigned int i=0, j=0; i<Limit::scanListCount() && j<scanListCount(); i++) {
    ScanListElement sl = scanList(i);
    if (! sl.isValid())
      continue;
    j++;

    ScanList *obj = sl.toScanListObj(ctx, err);
    if (nullptr == obj) {
      errMsg(err) << "Cannot decode scan list at index " << i << ".";
      return false;
    }
    // Add scan list to ctx and config
    ctx.add(obj, sl.index());
    ctx.config()->scanlists()->add(obj);
  }

  return true;
}

bool
DR1801UVCodeplug::ScanListBankElement::link(Context &ctx, const ErrorStack &err) const {
  for (unsigned int i=0,j=0; i<Limit::scanListCount() && j<scanListCount(); i++) {
    ScanListElement sl = scanList(i);
    if (! sl.isValid())
      continue;
    j++;

    if (! ctx.has<ScanList>(sl.index())) {
      errMsg(err) << "Cannot link scan list at index " << i
                  << ". Scan list not defined.";
      return false;
    }
    if (! sl.linkScanListObj(ctx.get<ScanList>(sl.index()), ctx, err)) {
      errMsg(err) << "Cannot link scan list at index " << i << ".";
      return false;
    }
  }

  return true;
}

bool
DR1801UVCodeplug::ScanListBankElement::encode(Context &ctx, const ErrorStack &err) {
  unsigned int n = std::min(Limit::scanListCount(), ctx.count<ScanList>());
  setScanListCount(n);
  for (unsigned int i=0; i<n; i++) {
    ScanListElement scan = this->scanList(i);
    if (! scan.encode(ctx.get<ScanList>(i), ctx, err)) {
      errMsg(err) << "Cannot encode scan list '" << ctx.get<ScanList>(i)->name()
                  << " at index " << i << ".";
      return false;
    }
    scan.setIndex(i);
  }
  return true;
}


/* ******************************************************************************************** *
 * Implementation of DR1801UVCodeplug::ScanListElement
 * ******************************************************************************************** */
DR1801UVCodeplug::ScanListElement::ScanListElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

DR1801UVCodeplug::ScanListElement::ScanListElement(uint8_t *ptr)
  : Element(ptr, ScanListElement::size())
{
  // pass...
}

void
DR1801UVCodeplug::ScanListElement::clear() {
  memset(_data, 0, _size);
}

bool
DR1801UVCodeplug::ScanListElement::isValid() const {
  return 0 != getUInt8(Offset::index()); // Check index
}

unsigned int
DR1801UVCodeplug::ScanListElement::index() const {
  return getUInt8(Offset::index())-1;
}
void
DR1801UVCodeplug::ScanListElement::setIndex(unsigned int idx) {
  setUInt8(Offset::index(), idx+1);
}

unsigned int
DR1801UVCodeplug::ScanListElement::entryCount() const {
  return getUInt8(Offset::memberCount());
}
void
DR1801UVCodeplug::ScanListElement::setEntryCount(unsigned int num) {
  setUInt8(Offset::memberCount(), num);
}

DR1801UVCodeplug::ScanListElement::PriorityChannel
DR1801UVCodeplug::ScanListElement::priorityChannel1() const {
  return (PriorityChannel) getUInt8(Offset::priorityChannel1());
}
void
DR1801UVCodeplug::ScanListElement::setPriorityChannel1(PriorityChannel mode) {
  setUInt8(Offset::priorityChannel1(), (uint8_t) mode);
}
unsigned int
DR1801UVCodeplug::ScanListElement::priorityChannel1Index() const {
  return getUInt16_le(Offset::priorityChannel1Index());
}
void
DR1801UVCodeplug::ScanListElement::setPriorityChannel1Index(unsigned int index) {
  setUInt16_le(Offset::priorityChannel1Index(), index);
}

DR1801UVCodeplug::ScanListElement::PriorityChannel
DR1801UVCodeplug::ScanListElement::priorityChannel2() const {
  return (PriorityChannel) getUInt8(Offset::priorityChannel2());
}
void
DR1801UVCodeplug::ScanListElement::setPriorityChannel2(PriorityChannel mode) {
  setUInt8(Offset::priorityChannel2(), (uint8_t) mode);
}
unsigned int
DR1801UVCodeplug::ScanListElement::priorityChannel2Index() const {
  return getUInt16_le(Offset::priorityChannel2Index());
}
void
DR1801UVCodeplug::ScanListElement::setPriorityChannel2Index(unsigned int index) {
  setUInt16_le(Offset::priorityChannel2Index(), index);
}

DR1801UVCodeplug::ScanListElement::RevertChannel
DR1801UVCodeplug::ScanListElement::revertChannel() const {
  return (RevertChannel) getUInt8(Offset::revertChannel());
}
void
DR1801UVCodeplug::ScanListElement::setRevertChannel(RevertChannel mode) {
  setUInt8(Offset::revertChannel(), (uint8_t) mode);
}
unsigned int
DR1801UVCodeplug::ScanListElement::revertChannelIndex() const {
  return getUInt16_le(Offset::revertChannelIndex());
}
void
DR1801UVCodeplug::ScanListElement::setRevertChannelIndex(unsigned int index) {
  setUInt16_le(Offset::revertChannelIndex(), index);
}

QString
DR1801UVCodeplug::ScanListElement::name() const {
  return readASCII(Offset::name(), Limit::nameLength(), 0x00);
}
void
DR1801UVCodeplug::ScanListElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::nameLength(), 0x00);
}

unsigned int
DR1801UVCodeplug::ScanListElement::entryIndex(unsigned int n) {
  return getUInt16_le(Offset::memberIndices() + 2*n);
}
void
DR1801UVCodeplug::ScanListElement::setEntryIndex(unsigned int n, unsigned int index) {
  setUInt16_le(Offset::memberIndices()+2*n, index);
}

ScanList *
DR1801UVCodeplug::ScanListElement::toScanListObj(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(err)
  return new ScanList(name());
}

bool
DR1801UVCodeplug::ScanListElement::linkScanListObj(ScanList *obj, Context &ctx, const ErrorStack &err) {
  // Link priority channels.
  switch (priorityChannel1()) {
  case PriorityChannel::Selected:
    obj->setPrimaryChannel(SelectedChannel::get());
    break;
  case PriorityChannel::Fixed:
    if (! ctx.has<Channel>(priorityChannel1Index())) {
      errMsg(err) << "Cannot link to priority channel 1: Channel with index " << priorityChannel1Index() << " not defined.";
      return false;
    }
    obj->setPrimaryChannel(ctx.get<Channel>(priorityChannel1Index()));
    break;
  case PriorityChannel::None: break;
  }

  switch (priorityChannel2()) {
  case PriorityChannel::Selected:
    obj->setSecondaryChannel(SelectedChannel::get());
    break;
  case PriorityChannel::Fixed:
    if (! ctx.has<Channel>(priorityChannel2Index())) {
      errMsg(err) << "Cannot link to priority channel 2: Channel with index " << priorityChannel2Index() << " not defined.";
      return false;
    }
    obj->setSecondaryChannel(ctx.get<Channel>(priorityChannel2Index()));
    break;
  case PriorityChannel::None: break;
  }

  for (unsigned int i=0; i<entryCount(); i++) {
    if (! ctx.has<Channel>(entryIndex(i))) {
      errMsg(err) << "Cannot link scan-list entry " << i
                  << ": Channel with index " << entryIndex(i) << " not defined.";
      return false;
    }
    obj->addChannel(ctx.get<Channel>(entryIndex(i)));
  }

  return true;
}

bool
DR1801UVCodeplug::ScanListElement::encode(ScanList *obj, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err);

  setName(obj->name());

  if (nullptr == obj->primaryChannel())
    setPriorityChannel1(PriorityChannel::None);
  else if (obj->primaryChannel() && (SelectedChannel::get() == obj->primaryChannel())) {
    setPriorityChannel1(PriorityChannel::Selected);
  } else {
    setPriorityChannel1(PriorityChannel::Fixed);
    setPriorityChannel1Index(ctx.index(obj->primaryChannel()));
  }

  if (nullptr == obj->secondaryChannel())
    setPriorityChannel2(PriorityChannel::None);
  else if (obj->secondaryChannel() && (SelectedChannel::get() == obj->secondaryChannel())) {
    setPriorityChannel2(PriorityChannel::Selected);
  } else {
    setPriorityChannel2(PriorityChannel::Fixed);
    setPriorityChannel2Index(ctx.index(obj->secondaryChannel()));
  }

  if (nullptr == obj->revertChannel())
    setRevertChannel(RevertChannel::LastActive);
  else if (obj->revertChannel() && (SelectedChannel::get() == obj->revertChannel())) {
    setRevertChannel(RevertChannel::Selected);
  } else {
    setRevertChannel(RevertChannel::Fixed);
    setRevertChannelIndex(ctx.index(obj->revertChannel()));
  }

  unsigned int n = std::min(Limit::memberCount(), (unsigned int)obj->count());
  for (unsigned int i=0; i<n; i++) {
    setEntryIndex(i, ctx.index(obj->channel(i)));
  }

  return true;
}


/* ******************************************************************************************** *
 * Implementation of DR1801UVCodeplug::MessageBankElement
 * ******************************************************************************************** */
DR1801UVCodeplug::MessageBankElement::MessageBankElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

DR1801UVCodeplug::MessageBankElement::MessageBankElement(uint8_t *ptr)
  : Element(ptr, MessageBankElement::size())
{
  // pass...
}

void
DR1801UVCodeplug::MessageBankElement::clear() {
  memset(_data, 0, _size);
}

unsigned int
DR1801UVCodeplug::MessageBankElement::messageCount() const {
  return getUInt8(Offset::messageCount());
}
void
DR1801UVCodeplug::MessageBankElement::setMessageCount(unsigned int count) {
  setUInt8(Offset::messageCount(), count);
}

DR1801UVCodeplug::MessageElement
DR1801UVCodeplug::MessageBankElement::message(unsigned int n) const {
  return MessageElement(_data + Offset::messages() + n*MessageElement::size());
}

bool
DR1801UVCodeplug::MessageBankElement::decode(Context &ctx, const ErrorStack &err) const {
  Q_UNUSED(err);

  ctx.config()->smsExtension()->smsTemplates()->clear();

  for (unsigned int i=0,j=0; i<Limit::messageCount() && j<messageCount(); i++) {
    MessageElement msg = message(i);
    if (! msg.isValid())
      continue;
    j++;
    SMSTemplate *sms = new SMSTemplate();
    sms->setName(QString("Message %1").arg(msg.index()));
    sms->setMessage(msg.text());
    ctx.config()->smsExtension()->smsTemplates()->add(sms);
  }

  return true;
}

bool
DR1801UVCodeplug::MessageBankElement::encode(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err);

  setMessageCount(0);

  SMSExtension *smsExt = ctx.config()->smsExtension();
  unsigned int count = std::min(Limit::messageCount(), (unsigned int)smsExt->smsTemplates()->count());
  for (unsigned int i=0; i<count; i++) {
    MessageElement msg = message(i);
    msg.setText(smsExt->smsTemplates()->message(i)->message());
    msg.setIndex(i+1);
  }

  setMessageCount(count);
  return true;
}


/* ******************************************************************************************** *
 * Implementation of DR1801UVCodeplug::MessageElement
 * ******************************************************************************************** */
DR1801UVCodeplug::MessageElement::MessageElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

DR1801UVCodeplug::MessageElement::MessageElement(uint8_t *ptr)
  : Element(ptr, MessageElement::size())
{
  // pass...
}

void
DR1801UVCodeplug::MessageElement::clear() {
  memset(_data, 0, _size);
}

bool
DR1801UVCodeplug::MessageElement::isValid() const {
  return 0x00 != getUInt8(Offset::index());
}

unsigned int
DR1801UVCodeplug::MessageElement::index() const {
  return getUInt8(Offset::index())-1;
}
void
DR1801UVCodeplug::MessageElement::setIndex(unsigned int index) {
  setUInt8(Offset::index(), index+1);
}

QString
DR1801UVCodeplug::MessageElement::text() const {
  return readASCII(Offset::text(), Limit::textLength(), 0x00);
}
void
DR1801UVCodeplug::MessageElement::setText(const QString &text) {
  setUInt8(Offset::textLength(),
           std::min(Limit::textLength(), (unsigned int)text.length()));
  writeASCII(Offset::text(), text, Limit::textLength(), 0x00);
}


/* ******************************************************************************************** *
 * Implementation of DR1801UVCodeplug::KeySettingsElement
 * ******************************************************************************************** */
DR1801UVCodeplug::KeySettingsElement::KeySettingsElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

DR1801UVCodeplug::KeySettingsElement::KeySettingsElement(uint8_t *ptr)
  : Element(ptr, KeySettingsElement::size())
{
  // pass...
}

void
DR1801UVCodeplug::KeySettingsElement::clear() {
  memset(_data, 0, _size);
}

DR1801UVCodeplug::KeySettingsElement::Function
DR1801UVCodeplug::KeySettingsElement::sideKey1Short() const {
  return (Function) getUInt8(Offset::sideKey1Short());
}
void
DR1801UVCodeplug::KeySettingsElement::setSideKey1Short(Function func) {
  setUInt8(Offset::sideKey1Short(), (uint8_t) func);
}

DR1801UVCodeplug::KeySettingsElement::Function
DR1801UVCodeplug::KeySettingsElement::sideKey1Long() const {
  return (Function) getUInt8(Offset::sideKey1Long());
}
void
DR1801UVCodeplug::KeySettingsElement::setSideKey1Long(Function func) {
  setUInt8(Offset::sideKey1Long(), (uint8_t) func);
}

DR1801UVCodeplug::KeySettingsElement::Function
DR1801UVCodeplug::KeySettingsElement::sideKey2Short() const {
  return (Function) getUInt8(Offset::sideKey2Short());
}
void
DR1801UVCodeplug::KeySettingsElement::setSideKey2Short(Function func) {
  setUInt8(Offset::sideKey2Short(), (uint8_t) func);
}

DR1801UVCodeplug::KeySettingsElement::Function
DR1801UVCodeplug::KeySettingsElement::sideKey2Long() const {
  return (Function) getUInt8(Offset::sideKey2Long());
}
void
DR1801UVCodeplug::KeySettingsElement::setSideKey2Long(Function func) {
  setUInt8(Offset::sideKey2Long(), (uint8_t) func);
}

DR1801UVCodeplug::KeySettingsElement::Function
DR1801UVCodeplug::KeySettingsElement::topKeyShort() const {
  return (Function) getUInt8(Offset::topKeyShort());
}
void
DR1801UVCodeplug::KeySettingsElement::setTopKeyShort(Function func) {
  setUInt8(Offset::topKeyShort(), (uint8_t) func);
}

DR1801UVCodeplug::KeySettingsElement::Function
DR1801UVCodeplug::KeySettingsElement::topKeyLong() const {
  return (Function) getUInt8(Offset::topKeyLong());
}
void
DR1801UVCodeplug::KeySettingsElement::setTopKeyLong(Function func) {
  setUInt8(Offset::topKeyLong(), (uint8_t) func);
}


/* ******************************************************************************************** *
 * Implementation of DR1801UVCodeplug::VFOBankElement
 * ******************************************************************************************** */
DR1801UVCodeplug::VFOBankElement::VFOBankElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

DR1801UVCodeplug::VFOBankElement::VFOBankElement(uint8_t *ptr)
  : Element(ptr, VFOBankElement::size())
{
  // pass...
}

void
DR1801UVCodeplug::VFOBankElement::clear() {
  memset(_data, 0, _size);
}

DR1801UVCodeplug::ChannelElement
DR1801UVCodeplug::VFOBankElement::vfoA() const {
  return ChannelElement(_data + Offset::vfoA());
}

DR1801UVCodeplug::ChannelElement
DR1801UVCodeplug::VFOBankElement::vfoB() const {
  return ChannelElement(_data + Offset::vfoB());
}

QString
DR1801UVCodeplug::VFOBankElement::nameA() const {
  return readASCII(Offset::nameA(), Limit::nameLength(), 0x00);
}
void
DR1801UVCodeplug::VFOBankElement::setNameA(const QString &name) {
  writeASCII(Offset::nameB(), name, Limit::nameLength(), 0x00);
}

QString
DR1801UVCodeplug::VFOBankElement::nameB() const {
  return readASCII(Offset::nameB(), Limit::nameLength(), 0x00);
}
void
DR1801UVCodeplug::VFOBankElement::setNameB(const QString &name) {
  writeASCII(Offset::nameB(), name, Limit::nameLength(), 0x00);
}


/* ******************************************************************************************** *
 * Implementation of DR1801UVCodeplug::EncryptionKeyBankElement
 * ******************************************************************************************** */
DR1801UVCodeplug::EncryptionKeyBankElement::EncryptionKeyBankElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

DR1801UVCodeplug::EncryptionKeyBankElement::EncryptionKeyBankElement(uint8_t *ptr)
  : Element(ptr, EncryptionKeyBankElement::size())
{
  // pass...
}

void
DR1801UVCodeplug::EncryptionKeyBankElement::clear() {
  memset(_data, 0, _size);
}

DR1801UVCodeplug::EncryptionKeyElement
DR1801UVCodeplug::EncryptionKeyBankElement::key(unsigned int index) const {
  return EncryptionKeyElement(_data + index*EncryptionKeyElement::size());
}

bool
DR1801UVCodeplug::EncryptionKeyBankElement::decode(Context &ctx, const ErrorStack &err) const {
  for (unsigned int i=0; i<Limit::keyCount(); i++) {
    EncryptionKey *obj = key(i).toKeyObj(ctx, err);
    if (nullptr == obj) {
      errMsg(err) << "Cannot decode encryption key at index " << i << ".";
      return false;
    }
    // Add key to index and config
    ctx.add(obj, key(i).index());
    ctx.config()->commercialExtension()->encryptionKeys()->add(obj);
  }
  return true;
}

bool
DR1801UVCodeplug::EncryptionKeyBankElement::link(Context &ctx, const ErrorStack &err) const
{
  Q_UNUSED(ctx); Q_UNUSED(err);

  // Nothing to do
  return true;
}

bool
DR1801UVCodeplug::EncryptionKeyBankElement::encode(Context &ctx, const ErrorStack &err) {
  unsigned int n = std::min(Limit::keyCount(), ctx.count<BasicEncryptionKey>());
  for (unsigned int i=0; i<Limit::keyCount(); i++) {
    EncryptionKeyElement key = this->key(i);
    if (i>=n) {
      key.clear();
      continue;
    }
    if (! key.encode(ctx.get<BasicEncryptionKey>(i), ctx, err)) {
      errMsg(err) << "Cannot encode DMR encryption key '" << ctx.get<BasicEncryptionKey>(i)->name()
                  << "' at index " << i << ".";
      return false;
    }
    key.setIndex(i);
  }
  return true;
}


/* ******************************************************************************************** *
 * Implementation of DR1801UVCodeplug::EncryptionKeyElement
 * ******************************************************************************************** */
DR1801UVCodeplug::EncryptionKeyElement::EncryptionKeyElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

DR1801UVCodeplug::EncryptionKeyElement::EncryptionKeyElement(uint8_t *ptr)
  : Element(ptr, EncryptionKeyElement::size())
{
  // pass...
}

void
DR1801UVCodeplug::EncryptionKeyElement::clear() {
  memset(_data, 0, _size);
}

bool
DR1801UVCodeplug::EncryptionKeyElement::isValid() const {
  return 0x00 != getUInt8(Offset::index());
}

unsigned int
DR1801UVCodeplug::EncryptionKeyElement::index() const {
  return getUInt8(Offset::index())-1;
}
void
DR1801UVCodeplug::EncryptionKeyElement::setIndex(unsigned int index) {
  setUInt8(Offset::index(), index+1);
}

unsigned int
DR1801UVCodeplug::EncryptionKeyElement::keyLength() const {
  return getUInt16_le(Offset::length());
}

QString
DR1801UVCodeplug::EncryptionKeyElement::key() const {
  return readASCII(Offset::key(), Limit::keyLength(), 0x00);
}
void
DR1801UVCodeplug::EncryptionKeyElement::setKey(const QString &key) {
  setUInt8(Offset::length(), 8);
  writeASCII(Offset::key(), key, Limit::keyLength(), 0x00);
}

EncryptionKey *
DR1801UVCodeplug::EncryptionKeyElement::toKeyObj(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx);

  if (! isValid()) {
    errMsg(err) << "Cannot decode an invalid encryption key.";
    return nullptr;
  }

  BasicEncryptionKey *obj = new BasicEncryptionKey();
  if (! obj->fromHex(key(), err)) {
    errMsg(err) << "Cannot decode key '" << key() << "'.";
    delete obj;
    return nullptr;
  }

  return obj;
}

bool
DR1801UVCodeplug::EncryptionKeyElement::linkKeyObj(EncryptionKey *obj, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(obj); Q_UNUSED(ctx); Q_UNUSED(err);
  // There is nothing to do.
  return true;
}

bool
DR1801UVCodeplug::EncryptionKeyElement::encode(EncryptionKey *obj, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx);

  if (!obj->is<BasicEncryptionKey>()) {
    errMsg(err) << "Cannot encode AES encryption key. Not supported by the device.";
    return false;
  }
  BasicEncryptionKey *key = obj->as<BasicEncryptionKey>();
  setKey(key->key().toHex());

  return true;
}

/* ******************************************************************************************** *
 * Implementation of DR1801UVCodeplug::DTMFSettingsElement
 * ******************************************************************************************** */
DR1801UVCodeplug::DTMFSettingsElement::DTMFSettingsElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

DR1801UVCodeplug::DTMFSettingsElement::DTMFSettingsElement(uint8_t *ptr)
  : Element(ptr, DR1801UVCodeplug::DTMFSettingsElement::size())
{
  // pass...
}

void
DR1801UVCodeplug::DTMFSettingsElement::clear() {
  memset(_data, 0, _size);
}

QString
DR1801UVCodeplug::DTMFSettingsElement::radioID() const {
  return readASCII(Offset::radioID(), Limit::radioIDLength(), 0x00);
}
void
DR1801UVCodeplug::DTMFSettingsElement::setRadioID(const QString &id) {
  setUInt8(Offset::radioIDLength(),
           std::min(Limit::radioIDLength(), (unsigned int)id.length()));
  writeASCII(Offset::radioIDLength(), id, Limit::radioIDLength(), 0x00);
}

QString
DR1801UVCodeplug::DTMFSettingsElement::killCode() const {
  return readASCII(Offset::killCode(), Limit::killCodeLength(), 0x00);
}
void
DR1801UVCodeplug::DTMFSettingsElement::setKillCode(const QString &code) {
  setUInt8(Offset::killCodeLength(),
           std::min(Limit::killCodeLength(), (unsigned int)code.length()));
  writeASCII(Offset::killCode(), code, Limit::killCodeLength(), 0x00);
}

QString
DR1801UVCodeplug::DTMFSettingsElement::wakeCode() const {
  return readASCII(Offset::wakeCode(), Limit::wakeCodeLength(), 0x00);
}
void
DR1801UVCodeplug::DTMFSettingsElement::setWakeCode(const QString &code) {
  setUInt8(Offset::wakeCodeLength(),
           std::min(Limit::wakeCodeLength(), (unsigned int)code.length()));
  writeASCII(Offset::wakeCode(), code, Limit::wakeCodeLength(), 0x00);
}

DR1801UVCodeplug::DTMFSettingsElement::NonNumber
DR1801UVCodeplug::DTMFSettingsElement::delimiter() const {
  return (NonNumber) getUInt8(Offset::delimiter());
}
void
DR1801UVCodeplug::DTMFSettingsElement::setDelimiter(NonNumber code) {
  setUInt8(Offset::delimiter(), (uint8_t)code);
}

DR1801UVCodeplug::DTMFSettingsElement::NonNumber
DR1801UVCodeplug::DTMFSettingsElement::groupCode() const {
  return (NonNumber) getUInt8(Offset::groupCode());
}
void
DR1801UVCodeplug::DTMFSettingsElement::setGroupCode(NonNumber code) {
  setUInt8(Offset::groupCode(), (uint8_t)code);
}

DR1801UVCodeplug::DTMFSettingsElement::Response
DR1801UVCodeplug::DTMFSettingsElement::response() const {
  return (Response) getUInt8(Offset::response());
}
void
DR1801UVCodeplug::DTMFSettingsElement::setResponse(Response resp) {
  setUInt8(Offset::response(), (uint8_t)resp);
}

unsigned int
DR1801UVCodeplug::DTMFSettingsElement::autoResetTime() const {
  return getUInt8(Offset::autoResetTime());
}
void
DR1801UVCodeplug::DTMFSettingsElement::setAutoResetTime(unsigned int sec) {
  setUInt8(Offset::autoResetTime(), sec);
}

bool
DR1801UVCodeplug::DTMFSettingsElement::killWakeEnabled() const {
  return 0x01 == getUInt8(Offset::killWake());
}
void
DR1801UVCodeplug::DTMFSettingsElement::enableKillWake(bool enable) {
  setUInt8(Offset::killWake(), enable ? 0x01 : 0x00);
}

DR1801UVCodeplug::DTMFSettingsElement::Kill
DR1801UVCodeplug::DTMFSettingsElement::killType() const {
  return (Kill) getUInt8(Offset::killType());
}
void
DR1801UVCodeplug::DTMFSettingsElement::setKillType(Kill type) {
  setUInt8(Offset::killType(), (uint8_t)type);
}

DR1801UVCodeplug::DTMFSystemBankElement
DR1801UVCodeplug::DTMFSettingsElement::dtmfSystems() const {
  return DTMFSystemBankElement(_data+Offset::dtmfSystems());
}

DR1801UVCodeplug::DTMFIDBankElement
DR1801UVCodeplug::DTMFSettingsElement::dtmfIDs() const {
  return DTMFIDBankElement(_data + Offset::dtmfIDs());
}

DR1801UVCodeplug::PTTIDBankElement
DR1801UVCodeplug::DTMFSettingsElement::pttIDs() const {
  return PTTIDBankElement(_data + Offset::pttIDs());
}


/* ******************************************************************************************** *
 * Implementation of DR1801UVCodeplug::DTMFSystemBankElement
 * ******************************************************************************************** */
DR1801UVCodeplug::DTMFSystemBankElement::DTMFSystemBankElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

DR1801UVCodeplug::DTMFSystemBankElement::DTMFSystemBankElement(uint8_t *ptr)
  : Element(ptr, DTMFSystemBankElement::size())
{
  // pass...
}

void
DR1801UVCodeplug::DTMFSystemBankElement::clear() {
  memset(_data, 0, _size);
}

unsigned int
DR1801UVCodeplug::DTMFSystemBankElement::systemCount() const {
  return getUInt8(Offset::systemCount());
}
void
DR1801UVCodeplug::DTMFSystemBankElement::setSystemCount(unsigned int count) {
  setUInt8(Offset::systemCount(), count);
}

DR1801UVCodeplug::DTMFSystemElement
DR1801UVCodeplug::DTMFSystemBankElement::system(unsigned int n) const {
  return DTMFSystemElement(_data + Offset::systems() + n*DTMFSystemElement::size());
}


/* ******************************************************************************************** *
 * Implementation of DR1801UVCodeplug::DTMFSystemElement
 * ******************************************************************************************** */
DR1801UVCodeplug::DTMFSystemElement::DTMFSystemElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

DR1801UVCodeplug::DTMFSystemElement::DTMFSystemElement(uint8_t *ptr)
  : Element(ptr, DTMFSystemElement::size())
{
  // pass...
}

void
DR1801UVCodeplug::DTMFSystemElement::clear() {
  memset(_data, 0, _size);
}

bool
DR1801UVCodeplug::DTMFSystemElement::sideToneEnabled() const {
  return 0x01 == getUInt8(Offset::sideTone());
}
void
DR1801UVCodeplug::DTMFSystemElement::enableSideTone(bool enable) {
  setUInt8(Offset::sideTone(), enable ? 0x01 : 0x00);
}

unsigned int
DR1801UVCodeplug::DTMFSystemElement::preTime() const {
  return getUInt16_le(Offset::preTime());
}
void
DR1801UVCodeplug::DTMFSystemElement::setPreTime(unsigned int ms) {
  setUInt16_le(Offset::preTime(), ms);
}

unsigned int
DR1801UVCodeplug::DTMFSystemElement::codeDuration() const {
  return getUInt16_le(Offset::codeDuration());
}
void
DR1801UVCodeplug::DTMFSystemElement::setCodeDuration(unsigned int ms) {
  setUInt16_le(Offset::codeDuration(), ms);
}

unsigned int
DR1801UVCodeplug::DTMFSystemElement::codeItervall() const {
  return getUInt16_le(Offset::codeIntervall());
}
void
DR1801UVCodeplug::DTMFSystemElement::setCodeItervall(unsigned int ms) {
  setUInt16_le(Offset::codeIntervall(), ms);
}

unsigned int
DR1801UVCodeplug::DTMFSystemElement::resetTime() const {
  return getUInt16_le(Offset::resetTime());
}
void
DR1801UVCodeplug::DTMFSystemElement::setResetTime(unsigned int ms) {
  setUInt16_le(Offset::resetTime(), ms);
}


/* ******************************************************************************************** *
 * Implementation of DR1801UVCodeplug::DTMFIDBankElement
 * ******************************************************************************************** */
DR1801UVCodeplug::DTMFIDBankElement::DTMFIDBankElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

DR1801UVCodeplug::DTMFIDBankElement::DTMFIDBankElement(uint8_t *ptr)
  : Element(ptr, DTMFIDBankElement::size())
{
  // pass...
}

void
DR1801UVCodeplug::DTMFIDBankElement::clear() {
  memset(_data, 0, _size);
}

unsigned int
DR1801UVCodeplug::DTMFIDBankElement::idCount() const {
  return getUInt8(Offset::idCount());
}
void
DR1801UVCodeplug::DTMFIDBankElement::setIDCount(unsigned int n) {
  setUInt8(Offset::idCount(), n);
}

DR1801UVCodeplug::DTMFIDElement
DR1801UVCodeplug::DTMFIDBankElement::id(unsigned int n) const {
  return DTMFIDElement(_data + Offset::ids() + n*DTMFIDElement::size());
}


/* ******************************************************************************************** *
 * Implementation of DR1801UVCodeplug::DTMFIDElement
 * ******************************************************************************************** */
QVector<QChar> DR1801UVCodeplug::DTMFIDElement::_bin_dtmf_tab = {
  '0','1','2','3','4','5','6','7','8','9','A','B','C','D','*','#'
};

DR1801UVCodeplug::DTMFIDElement::DTMFIDElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

DR1801UVCodeplug::DTMFIDElement::DTMFIDElement(uint8_t *ptr)
  : Element(ptr, DR1801UVCodeplug::DTMFIDElement::size())
{
  // pass...
}

void
DR1801UVCodeplug::DTMFIDElement::clear() {
  memset(_data, 0, _size);
}

unsigned int
DR1801UVCodeplug::DTMFIDElement::numberLength() const {
  return getUInt8(Offset::numberLength());
}
void
DR1801UVCodeplug::DTMFIDElement::setNumberLength(unsigned int len) {
  setUInt8(Offset::numberLength(), len);
}

QString
DR1801UVCodeplug::DTMFIDElement::number() const {
  QString number; number.reserve(16);
  for (unsigned int i=0; i<numberLength(); i++) {
    number.append(_bin_dtmf_tab[getUInt8(Offset::number()+i)&0xf]);
  }
  return number;
}

void
DR1801UVCodeplug::DTMFIDElement::setNumber(const QString &number) {
  QString lnumber = number.toLower();
  QRegularExpression re("[0-9a-d*#]+");
  if (! re.match(lnumber).isValid())
    return;
  setNumberLength(std::min(Limit::numberLength(), (unsigned int)lnumber.length()));
  for (unsigned int i=0; i<numberLength(); i++) {
    setUInt8(Offset::number() + i, _bin_dtmf_tab.indexOf(lnumber.at(i)));
  }
}


/* ******************************************************************************************** *
 * Implementation of DR1801UVCodeplug::PTTIDBankElement
 * ******************************************************************************************** */
DR1801UVCodeplug::PTTIDBankElement::PTTIDBankElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

DR1801UVCodeplug::PTTIDBankElement::PTTIDBankElement(uint8_t *ptr)
  : Element(ptr, PTTIDBankElement::size())
{
  // pass...
}

void
DR1801UVCodeplug::PTTIDBankElement::clear() {
  memset(_data, 0, _size);
}

unsigned int
DR1801UVCodeplug::PTTIDBankElement::idCount() const {
  return getUInt8(Offset::idCount());
}
void
DR1801UVCodeplug::PTTIDBankElement::setPTTIDCount(unsigned int n) {
  setUInt8(Offset::idCount(), n);
}

DR1801UVCodeplug::PTTIDElement
DR1801UVCodeplug::PTTIDBankElement::pttID(unsigned int n) {
  return PTTIDElement(_data + Offset::ids() + n*PTTIDElement::size());
}


/* ******************************************************************************************** *
 * Implementation of DR1801UVCodeplug::PTTIDElement
 * ******************************************************************************************** */
DR1801UVCodeplug::PTTIDElement::PTTIDElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

DR1801UVCodeplug::PTTIDElement::PTTIDElement(uint8_t *ptr)
  : Element(ptr, PTTIDElement::size())
{
  // pass...
}

void
DR1801UVCodeplug::PTTIDElement::clear() {
  memset(_data, 0, _size);
}

bool
DR1801UVCodeplug::PTTIDElement::hasDTMFSystem() const {
  return 0 != getUInt8(Offset::dtmfSystemIndex());
}
unsigned int
DR1801UVCodeplug::PTTIDElement::dtmfSystemIndex() const {
  return getUInt8(Offset::dtmfSystemIndex())-1;
}
void
DR1801UVCodeplug::PTTIDElement::setDTMFSystemIndex(unsigned int index) {
  setUInt8(Offset::dtmfSystemIndex(), index+1);
}
void
DR1801UVCodeplug::PTTIDElement::clearDTMFSystem() {
  setUInt8(Offset::dtmfSystemIndex(), 0);
}

DR1801UVCodeplug::PTTIDElement::Transmit
DR1801UVCodeplug::PTTIDElement::transmitMode() const {
  return (Transmit) getUInt8(Offset::transmitMode());
}
void
DR1801UVCodeplug::PTTIDElement::setTransmitMode(Transmit mode) {
  setUInt8(Offset::transmitMode(), (uint8_t)mode);
}

DR1801UVCodeplug::PTTIDElement::IDMode
DR1801UVCodeplug::PTTIDElement::idMode() const {
  return (IDMode) getUInt8(Offset::idMode());
}
void
DR1801UVCodeplug::PTTIDElement::setIDMode(IDMode mode) {
  setUInt8(Offset::idMode(), (uint8_t)mode);
}

bool
DR1801UVCodeplug::PTTIDElement::hasBOTID() const {
  return 0 != getUInt8(Offset::botIDIndex());
}
unsigned int
DR1801UVCodeplug::PTTIDElement::botIDIndex() const {
  return getUInt8(Offset::botIDIndex())-1;
}
void
DR1801UVCodeplug::PTTIDElement::setBOTIDIndex(unsigned int index) {
  setUInt8(Offset::botIDIndex(), index+1);
}
void
DR1801UVCodeplug::PTTIDElement::clearBOTID() {
  setUInt8(Offset::botIDIndex(), 0);
}

bool
DR1801UVCodeplug::PTTIDElement::hasEOTID() const {
  return 0 != getUInt8(Offset::eotIDIndex());
}
unsigned int
DR1801UVCodeplug::PTTIDElement::eotIDIndex() const {
  return getUInt8(Offset::eotIDIndex())-1;
}
void
DR1801UVCodeplug::PTTIDElement::setEOTIDIndex(unsigned int index) {
  setUInt8(Offset::eotIDIndex(), index+1);
}
void
DR1801UVCodeplug::PTTIDElement::clearEOTID() {
  setUInt8(Offset::eotIDIndex(), 0);
}


/* ******************************************************************************************** *
 * Implementation of DR1801UVCodeplug::AlarmSystemBankElement
 * ******************************************************************************************** */
DR1801UVCodeplug::AlarmSystemBankElement::AlarmSystemBankElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

DR1801UVCodeplug::AlarmSystemBankElement::AlarmSystemBankElement(uint8_t *ptr)
  : Element(ptr, AlarmSystemBankElement::size())
{
  // pass...
}

void
DR1801UVCodeplug::AlarmSystemBankElement::clear() {
  memset(_data, 0, _size);
}

unsigned int
DR1801UVCodeplug::AlarmSystemBankElement::alarmSystemCount() const {
  return getUInt8(Offset::alarmSystemCount());
}
void
DR1801UVCodeplug::AlarmSystemBankElement::setAlarmSystemCount(unsigned int n) {
  setUInt8(Offset::alarmSystemCount(), n);
}

DR1801UVCodeplug::AlarmSystemElement
DR1801UVCodeplug::AlarmSystemBankElement::alarmSystem(unsigned int n) const {
  return AlarmSystemElement(_data + Offset::alarmSystems() + n*AlarmSystemElement::size());
}


/* ******************************************************************************************** *
 * Implementation of DR1801UVCodeplug::AlarmSystemElement
 * ******************************************************************************************** */
DR1801UVCodeplug::AlarmSystemElement::AlarmSystemElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

DR1801UVCodeplug::AlarmSystemElement::AlarmSystemElement(uint8_t *ptr)
  : Element(ptr, AlarmSystemElement::size())
{
  // pass...
}

void
DR1801UVCodeplug::AlarmSystemElement::clear() {
  memset(_data, 0, _size);
}

bool
DR1801UVCodeplug::AlarmSystemElement::isValid() const {
  return 0 != getUInt8(Offset::index());
}

unsigned int
DR1801UVCodeplug::AlarmSystemElement::index() const {
  return getUInt8(Offset::index())-1;
}
void
DR1801UVCodeplug::AlarmSystemElement::setIndex(unsigned int index) {
  setUInt8(Offset::index(), index+1);
}
void
DR1801UVCodeplug::AlarmSystemElement::clearIndex() {
  setUInt8(Offset::index(), 0);
}

bool
DR1801UVCodeplug::AlarmSystemElement::alarmEnabled() const {
  return 0x01 == getUInt8(Offset::alarmEnabled());
}
void
DR1801UVCodeplug::AlarmSystemElement::enableAlarm(bool enable) {
  setUInt8(Offset::alarmEnabled(), enable ? 0x01 : 0x00);
}

bool
DR1801UVCodeplug::AlarmSystemElement::noAlarmChannel() const {
  return 0 == getUInt16_le(Offset::alarmChannelIndex());
}
bool
DR1801UVCodeplug::AlarmSystemElement::alarmChannelIsSelected() const {
  return 0xffff == getUInt16_le(Offset::alarmChannelIndex());
}
unsigned int
DR1801UVCodeplug::AlarmSystemElement::alarmChannelIndex() const {
  return getUInt16_le(Offset::alarmChannelIndex())-1;
}
void
DR1801UVCodeplug::AlarmSystemElement::setAlarmChannelIndex(unsigned int index) {
  setUInt16_le(Offset::alarmChannelIndex(), index+1);
}
void
DR1801UVCodeplug::AlarmSystemElement::setAlarmChannelSelected() {
  setUInt16_le(Offset::alarmChannelIndex(), 0xffff);
}
void
DR1801UVCodeplug::AlarmSystemElement::clearAlarmChannel() {
  setUInt16_le(Offset::alarmChannelIndex(), 0);
}

QString
DR1801UVCodeplug::AlarmSystemElement::name() const {
  return readASCII(Offset::name(), Limit::nameLength(), 0x00);
}
void
DR1801UVCodeplug::AlarmSystemElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::nameLength(), 0x00);
}


/* ******************************************************************************************** *
 * Implementation of DR1801UVCodeplug::DMRSettingsElement
 * ******************************************************************************************** */
DR1801UVCodeplug::DMRSettingsElement::DMRSettingsElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

DR1801UVCodeplug::DMRSettingsElement::DMRSettingsElement(uint8_t *ptr)
  : Element(ptr, DMRSettingsElement::size())
{
  // pass...
}

void
DR1801UVCodeplug::DMRSettingsElement::clear() {
  memset(_data, 0, _size);
}

unsigned int
DR1801UVCodeplug::DMRSettingsElement::holdTime() const {
  return getUInt8(Offset::holdTime());
}
void
DR1801UVCodeplug::DMRSettingsElement::setHoldTime(unsigned int sec) {
  sec = Limit::holdTime().limit(sec);
  setUInt8(Offset::holdTime(), sec);
}

unsigned int
DR1801UVCodeplug::DMRSettingsElement::remoteListen() const {
  return getUInt8(Offset::remoteListen());
}
void
DR1801UVCodeplug::DMRSettingsElement::setRemoteListen(unsigned int sec) {
  sec = Limit::remoteListen().limit(sec);
  setUInt8(Offset::remoteListen(), sec);
}

unsigned int
DR1801UVCodeplug::DMRSettingsElement::activeWait() const {
  return getUInt8(Offset::activeWait());
}
void
DR1801UVCodeplug::DMRSettingsElement::setActiveWait(unsigned int ms) {
  ms = Limit::activeWait().limit(ms);
  setUInt8(Offset::activeWait(), ms);
}

unsigned int
DR1801UVCodeplug::DMRSettingsElement::activeResend() const {
  return getUInt8(Offset::activeResend());
}
void
DR1801UVCodeplug::DMRSettingsElement::setActiveResend(unsigned int count) {
  count = Limit::activeResend().limit(count);
  setUInt8(Offset::activeResend(), count);
}

unsigned int
DR1801UVCodeplug::DMRSettingsElement::preSend() const {
  return getUInt8(Offset::preSend());
}
void
DR1801UVCodeplug::DMRSettingsElement::setPreSend(unsigned int count) {
  count = Limit::preSend().limit(count);
  setUInt8(Offset::preSend(), count);
}

unsigned int
DR1801UVCodeplug::DMRSettingsElement::voiceHead() const {
  return getUInt8(Offset::voiceHead());
}
void
DR1801UVCodeplug::DMRSettingsElement::setVoiceHead(unsigned int count) {
  count = Limit::voiceHead().limit(count);
  setUInt8(Offset::voiceHead(), count);
}

DR1801UVCodeplug::DMRSettingsElement::SMSFormat
DR1801UVCodeplug::DMRSettingsElement::smsFormat() const {
  return (SMSFormat) getUInt8(Offset::smsFormat());
}
void
DR1801UVCodeplug::DMRSettingsElement::setSMSFormat(SMSFormat format) {
  setUInt8(Offset::smsFormat(), (uint8_t) format);
}

bool
DR1801UVCodeplug::DMRSettingsElement::killEnc() const {
  return 0x01 == getUInt8(Offset::killEnc());
}
void
DR1801UVCodeplug::DMRSettingsElement::enableKillEnc(bool enable) {
  setUInt8(Offset::killEnc(), enable ? 0x01 : 0x00);
}
bool
DR1801UVCodeplug::DMRSettingsElement::killDec() const {
  return 0x01 == getUInt8(Offset::killDec());
}
void
DR1801UVCodeplug::DMRSettingsElement::enableKillDec(bool enable) {
  setUInt8(Offset::killDec(), enable ? 0x01 : 0x00);
}

bool
DR1801UVCodeplug::DMRSettingsElement::activeEnc() const {
  return 0x01 == getUInt8(Offset::activeEnc());
}
void
DR1801UVCodeplug::DMRSettingsElement::enableActiveEnc(bool enable) {
  setUInt8(Offset::activeEnc(), enable ? 0x01 : 0x00);
}
bool
DR1801UVCodeplug::DMRSettingsElement::activeDec() const {
  return 0x01 == getUInt8(Offset::activeDec());
}
void
DR1801UVCodeplug::DMRSettingsElement::enableActiveDec(bool enable) {
  setUInt8(Offset::activeDec(), enable ? 0x01 : 0x00);
}

bool
DR1801UVCodeplug::DMRSettingsElement::checkEnc() const {
  return 0x01 == getUInt8(Offset::checkEnc());
}
void
DR1801UVCodeplug::DMRSettingsElement::enableCheckEnc(bool enable) {
  setUInt8(Offset::checkEnc(), enable ? 0x01 : 0x00);
}
bool
DR1801UVCodeplug::DMRSettingsElement::checkDec() const {
  return 0x01 == getUInt8(Offset::checkDec());
}
void
DR1801UVCodeplug::DMRSettingsElement::enableCheckDec(bool enable) {
  setUInt8(Offset::checkDec(), enable ? 0x01 : 0x00);
}

bool
DR1801UVCodeplug::DMRSettingsElement::callAlterEnc() const {
  return getBit(Offset::callAlterEnc());
}
void
DR1801UVCodeplug::DMRSettingsElement::enableCallAlterEnc(bool enable) {
  setBit(Offset::callAlterEnc(), enable);
}
bool
DR1801UVCodeplug::DMRSettingsElement::callAlterDec() const {
  return getBit(Offset::callAlterDec());
}
void
DR1801UVCodeplug::DMRSettingsElement::enableCallAlterDec(bool enable) {
  setBit(Offset::callAlterDec(), enable);
}

bool
DR1801UVCodeplug::DMRSettingsElement::remoteMonitorEnc() const {
  return getBit(Offset::remoteMonitorEnc());
}
void
DR1801UVCodeplug::DMRSettingsElement::enableRemoteMonitorEnc(bool enable) {
  setBit(Offset::remoteMonitorEnc(), enable);
}
bool
DR1801UVCodeplug::DMRSettingsElement::remoteMonitorDec() const {
  return getBit(Offset::remoteMonitorDec());
}
void
DR1801UVCodeplug::DMRSettingsElement::enableRemoteMonitorDec(bool enable) {
  setBit(Offset::remoteMonitorDec(), enable);
}

bool
DR1801UVCodeplug::DMRSettingsElement::decode(Context &ctx, const ErrorStack &err) const {
  Q_UNUSED(err);

  switch(smsFormat()) {
  case SMSFormat::IPData: ctx.config()->smsExtension()->setFormat(SMSExtension::Format::Motorola); break;
  case SMSFormat::CompressedIP: ctx.config()->smsExtension()->setFormat(SMSExtension::Format::Hytera); break;
  case SMSFormat::DefinedData: ctx.config()->smsExtension()->setFormat(SMSExtension::Format::DMR); break;
  }

  return true;
}

bool
DR1801UVCodeplug::DMRSettingsElement::encode(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err);

  switch(ctx.config()->smsExtension()->format()) {
  case SMSExtension::Format::Motorola: setSMSFormat(SMSFormat::IPData); break;
  case SMSExtension::Format::Hytera: setSMSFormat(SMSFormat::CompressedIP); break;
  case SMSExtension::Format::DMR: setSMSFormat(SMSFormat::DefinedData); break;
  }

  return true;
}


/* ******************************************************************************************** *
 * Implementation of DR1801UVCodeplug::OneTouchSettingsElement
 * ******************************************************************************************** */
DR1801UVCodeplug::OneTouchSettingsElement::OneTouchSettingsElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

DR1801UVCodeplug::OneTouchSettingsElement::OneTouchSettingsElement(uint8_t *ptr)
  : Element(ptr, OneTouchSettingsElement::size())
{
  // pass...
}

void
DR1801UVCodeplug::OneTouchSettingsElement::clear() {
  memset(_data, 0, _size);
}
unsigned int
DR1801UVCodeplug::OneTouchSettingsElement::settingsCount() const {
  return Limit::settingsCount();
}

DR1801UVCodeplug::OneTouchSettingElement
DR1801UVCodeplug::OneTouchSettingsElement::setting(unsigned int n) const {
  return OneTouchSettingElement(_data + Offset::settings() + n*OneTouchSettingElement::size());
}


/* ******************************************************************************************** *
 * Implementation of DR1801UVCodeplug::OneTouchSettingElement
 * ******************************************************************************************** */
DR1801UVCodeplug::OneTouchSettingElement::OneTouchSettingElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

DR1801UVCodeplug::OneTouchSettingElement::OneTouchSettingElement(uint8_t *ptr)
  : Element(ptr, OneTouchSettingElement::size())
{
  // pass...
}

void
DR1801UVCodeplug::OneTouchSettingElement::clear() {
  memset(_data, 0, _size);
}

bool
DR1801UVCodeplug::OneTouchSettingElement::isValid() const {
  return Element::isValid() && (Type::Disabled != type());
}

bool
DR1801UVCodeplug::OneTouchSettingElement::hasContact() const {
  return 0 != getUInt16_le(Offset::contactIndex());
}
unsigned int
DR1801UVCodeplug::OneTouchSettingElement::contactIndex() const {
  return getUInt16_le(Offset::contactIndex())-1;
}
void
DR1801UVCodeplug::OneTouchSettingElement::setContactIndex(unsigned int index) {
  setUInt16_le(Offset::contactIndex(), index+1);
}
void
DR1801UVCodeplug::OneTouchSettingElement::clearContact() {
  setUInt16_le(Offset::contactIndex(), 0);
}

DR1801UVCodeplug::OneTouchSettingElement::Action
DR1801UVCodeplug::OneTouchSettingElement::action() const {
  return (Action) getUInt8(Offset::action());
}
void
DR1801UVCodeplug::OneTouchSettingElement::setAction(Action action) {
  setUInt8(Offset::action(), (uint8_t) action);
}

bool
DR1801UVCodeplug::OneTouchSettingElement::hasMessage() const {
  return 0 != getUInt8(Offset::messageIndex());
}
unsigned int
DR1801UVCodeplug::OneTouchSettingElement::messageIndex() const {
  return getUInt8(Offset::messageIndex())-1;
}
void
DR1801UVCodeplug::OneTouchSettingElement::setMessageIndex(unsigned int index) {
  setUInt8(Offset::messageIndex(), index+1);
}
void
DR1801UVCodeplug::OneTouchSettingElement::clearMessage() {
  setUInt8(Offset::messageIndex(), 0);
}

DR1801UVCodeplug::OneTouchSettingElement::Type
DR1801UVCodeplug::OneTouchSettingElement::type() const {
  return (Type) getUInt8(Offset::type());
}
void
DR1801UVCodeplug::OneTouchSettingElement::setType(Type type) {
  setUInt8(Offset::type(), (uint8_t)type);
}

bool
DR1801UVCodeplug::OneTouchSettingElement::hasDTMFID() const {
  return 0 != getUInt8(Offset::dtmfIDIndex());
}
unsigned int
DR1801UVCodeplug::OneTouchSettingElement::dtmfIDIndex() const {
  return getUInt8(Offset::dtmfIDIndex())-1;
}
void
DR1801UVCodeplug::OneTouchSettingElement::setDTMFIDIndex(unsigned int index) {
  setUInt8(Offset::dtmfIDIndex(), index+1);
}
void
DR1801UVCodeplug::OneTouchSettingElement::clearDTMFIDIndex() {
  setUInt8(Offset::dtmfIDIndex(), 0);
}


/* ******************************************************************************************** *
 * Implementation of DR1801UVCodeplug
 * ******************************************************************************************** */
DR1801UVCodeplug::DR1801UVCodeplug(QObject *parent)
  : Codeplug(parent)
{
  addImage("BTECH DR-1801UV Codeplug");
  image(0).addElement(0, Offset::size());
}

Config *
DR1801UVCodeplug::preprocess(Config *config, const ErrorStack &err) const {
  Config *copy = Codeplug::preprocess(config, err);
  if (nullptr == copy) {
    errMsg(err) << "Cannot pre-process DR1801A6 codeplug.";
    return nullptr;
  }

  // Split dual-zones into two.
  ZoneSplitVisitor splitter;
  if (! splitter.process(copy, err)) {
    errMsg(err) << "Cannot pre-process DR1801A6 codeplug.";
    delete copy;
    return nullptr;
  }

  return copy;
}

bool
DR1801UVCodeplug::postprocess(Config *config, const ErrorStack &err) const {
  if (! Codeplug::postprocess(config, err)) {
    errMsg(err) << "Cannot post-process DR1801A6 codeplug.";
    return false;
  }

  // Merge splitted zones into one.
  ZoneMergeVisitor merger;
  if (! merger.process(config, err)) {
    errMsg(err) << "Cannot post-process DR1801A6 codeplug.";
    return false;
  }

  return true;
}

bool
DR1801UVCodeplug::index(Config *config, Context &ctx, const ErrorStack &err) const {
  Q_UNUSED(err)

  // All indices as 0-based. That is, the first channel gets index 0 etc.

  // There must be a default DMR radio ID.
  if (nullptr == ctx.config()->settings()->defaultId()) {
    errMsg(err) << "No default DMR radio ID specified.";
    errMsg(err) << "Cannot index codplug for encoding for the BTECH DR-1801UV.";
    return false;
  }

  // Map radio IDs
  for (int i=0; i<ctx.config()->radioIDs()->count(); i++)
    ctx.add(ctx.config()->radioIDs()->getId(i), i);

  // Map digital and DTMF contacts
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
DR1801UVCodeplug::encode(Config *config, const Flags &flags, const ErrorStack &err) {
  Q_UNUSED(flags);

  Context ctx(config);
  if (! index(config, ctx, err)) {
    errMsg(err) << "Cannot encode codeplug.";
    return false;
  }

  if (! encodeElements(ctx, err)) {
    errMsg(err) << "Cannot encode codeplug.";
    return false;
  }

  return true;
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
  if (! ChannelBankElement(data(Offset::channelBank())).decode(ctx, err)) {
    errMsg(err) << "Cannot decode channel elements.";
    return false;
  }

  if (! ContactBankElement(data(Offset::contactBank())).decode(ctx, err)) {
    errMsg(err) << "Cannot decode contact elements.";
    return false;
  }

  if (! GroupListBankElement(data(Offset::groupListBank())).decode(ctx, err)) {
    errMsg(err) << "Cannot decode group list elements.";
    return false;
  }

  if (! ZoneBankElement(data(Offset::zoneBank())).decode(ctx, err)) {
    errMsg(err) << "Cannot decode zone elements.";
    return false;
  }

  if (! MessageBankElement(data(Offset::messageBank())).decode(ctx, err)) {
    errMsg(err) << "Cannot decode preset messages.";
    return false;
  }

  if (! SettingsElement(data(Offset::settings())).updateConfig(ctx.config(), err)) {
    errMsg(err) << "Cannot decode settings element.";
    return false;
  }

  if (! DMRSettingsElement(data(Offset::dmrSettings())).decode(ctx, err)) {
    errMsg(err) << "Cannot decode DMR settings element.";
    return false;
  }

  if (! ScanListBankElement(data(Offset::scanListBank())).decode(ctx, err)) {
    errMsg(err) << "Cannot decode scan list elements.";
    return false;
  }

  if (! EncryptionKeyBankElement(data(Offset::encryptionKeyBank())).decode(ctx, err)) {
    logWarn() << "Cannot decode encryption keys:\n" << err.format(" ");
    //errMsg(err) << "Cannot decode encryption keys.";
    //return false;
  }

  return true;
}

bool
DR1801UVCodeplug::linkElements(Context &ctx, const ErrorStack &err) {
  if (! ChannelBankElement(data(Offset::channelBank())).link(ctx, err)) {
    errMsg(err) << "Cannot link channels.";
    return false;
  }

  if (! ContactBankElement(data(Offset::contactBank())).link(ctx, err)) {
    errMsg(err) << "Cannot link contacts.";
    return false;
  }

  if (! GroupListBankElement(data(Offset::groupListBank())).link(ctx, err)) {
    errMsg(err) << "Cannot link group lists.";
    return false;
  }

  if (! ZoneBankElement(data(Offset::zoneBank())).link(ctx, err)) {
    errMsg(err) << "Cannot link zones.";
    return false;
  }

  if (! ScanListBankElement(data(Offset::scanListBank())).link(ctx, err)) {
    errMsg(err) << "Cannot link scan lists.";
    return false;
  }

  return true;
}

bool
DR1801UVCodeplug::encodeElements(Context &ctx, const ErrorStack &err) {
  if (! SettingsElement(data(Offset::settings())).fromConfig(ctx.config(), err)) {
    errMsg(err) << "Cannot encode settings element.";
    return false;
  }

  if (! DMRSettingsElement(data(Offset::dmrSettings())).encode(ctx, err)) {
    errMsg(err) << "Cannot encode DMR settings element.";
    return false;
  }

  if (! ZoneBankElement(data(Offset::zoneBank())).encode(ctx, err)) {
    errMsg(err) << "Cannot encode zones.";
    return false;
  }

  if (! MessageBankElement(data(Offset::messageBank())).encode(ctx, err)) {
    errMsg(err) << "Cannot encode messages.";
    return false;
  }

  if (! ContactBankElement(data(Offset::contactBank())).encode(ctx, err)) {
    errMsg(err) << "Cannot encode contacts.";
    return false;
  }

  if (! ScanListBankElement(data(Offset::scanListBank())).encode(ctx, err)) {
    errMsg(err) << "Cannot encode scan lists.";
    return false;
  }

  if (! ChannelBankElement(data(Offset::channelBank())).encode(ctx, err)) {
    errMsg(err) << "Cannot encode channels.";
    return false;
  }

  if (! GroupListBankElement(data(Offset::groupListBank())).encode(ctx, err)) {
    errMsg(err) << "Cannot encode group lists.";
    return false;
  }

  if (! EncryptionKeyBankElement(data(Offset::encryptionKeyBank())).encode(ctx, err)) {
    errMsg(err) << "Cannot encode encryption keys.";
    return false;
  }

  return true;
}

