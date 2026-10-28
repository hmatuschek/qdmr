#include "anytone_codeplug.hh"
#include "utils.hh"
#include "logger.hh"
#include "anytone_extension.hh"
#include "config.hh"
#include "intermediaterepresentation.hh"
#include <QTimeZone>
#include <QRegularExpression>


QVector<char> _anytone_bin_dtmf_tab = {
  '0','1','2','3','4','5','6','7','8','9','A','B','C','D','*','#'
};


/* ******************************************************************************************** *
 * Implementation of AnytoneCodeplugGen1::CTCSS
 * ******************************************************************************************** */
SelectiveCall
AnytoneCodeplugGen1::CTCSS::_codeTable[] = {
  SelectiveCall(62.5),  SelectiveCall(67.0),  SelectiveCall(69.3),  SelectiveCall(71.9),
  SelectiveCall(74.4),  SelectiveCall(77.0),  SelectiveCall(79.7),  SelectiveCall(82.5),
  SelectiveCall(85.4),  SelectiveCall(88.5),  SelectiveCall(91.5),  SelectiveCall(94.8),
  SelectiveCall(97.4), SelectiveCall(100.0), SelectiveCall(103.5), SelectiveCall(107.2),
 SelectiveCall(110.9), SelectiveCall(114.8), SelectiveCall(118.8), SelectiveCall(123.0),
 SelectiveCall(127.3), SelectiveCall(131.8), SelectiveCall(136.5), SelectiveCall(141.3),
 SelectiveCall(146.2), SelectiveCall(151.4), SelectiveCall(156.7), SelectiveCall(159.8),
 SelectiveCall(162.2), SelectiveCall(165.5), SelectiveCall(167.9), SelectiveCall(171.3),
 SelectiveCall(173.8), SelectiveCall(177.3), SelectiveCall(179.9), SelectiveCall(183.5),
 SelectiveCall(186.2), SelectiveCall(189.9), SelectiveCall(192.8), SelectiveCall(196.6),
 SelectiveCall(199.5), SelectiveCall(203.5), SelectiveCall(206.5), SelectiveCall(210.7),
 SelectiveCall(218.1), SelectiveCall(225.7), SelectiveCall(229.1), SelectiveCall(233.6),
 SelectiveCall(241.8), SelectiveCall(250.3), SelectiveCall(254.1), SelectiveCall()
};

uint8_t
AnytoneCodeplugGen1::CTCSS::encode(const SelectiveCall &code) {
  for (uint8_t i=0; i<52; i++) {
    if (code == _codeTable[i])
      return i;
  }
  return customIndex();
}

SelectiveCall
AnytoneCodeplugGen1::CTCSS::decode(uint8_t num) {
  if (num >= 52)
    return SelectiveCall();
  return _codeTable[num];
}





/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::InvertedBytemapElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::InvertedBytemapElement::InvertedBytemapElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

void
AnytoneCodeplugGen1::InvertedBytemapElement::clear() {
  memset(_data, 0xff, _size);
}

bool
AnytoneCodeplugGen1::InvertedBytemapElement::isEncoded(unsigned int idx) const {
  if (idx >= _size)
    return false;
  return 0 == _data[idx];
}

void
AnytoneCodeplugGen1::InvertedBytemapElement::setEncoded(unsigned int idx, bool enable) {
  if (idx >= _size)
    return;
  _data[idx] = enable ? 0x00 : 0xff;
}

void
AnytoneCodeplugGen1::InvertedBytemapElement::enableFirst(unsigned int n) {
  memset(_data, 0x00, n);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::ChannelElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::ChannelElement::ChannelElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::ChannelElement::ChannelElement(uint8_t *ptr)
  : Element(ptr, ChannelElement::size())
{
  // pass...
}

AnytoneCodeplugGen1::ChannelElement::~ChannelElement() {
  // pass...
}

void
AnytoneCodeplugGen1::ChannelElement::clear() {
  setRXFrequency(0);
  setTXOffset(0);
  setMode(Mode::Analog);
  setPower(Channel::Power::Low);
  setBandwidth(FMChannel::Bandwidth::Narrow);
  setRXTone(SelectiveCall());
  setTXTone(SelectiveCall());
  setBit(0x0008, 5, false); // Unused set to 0
}

unsigned
AnytoneCodeplugGen1::ChannelElement::rxFrequency() const {
  return ((unsigned)getBCD8_be(Offset::rxFrequency()))*10;
}
void
AnytoneCodeplugGen1::ChannelElement::setRXFrequency(unsigned hz) {
  setBCD8_be(Offset::rxFrequency(), hz/10);
}

unsigned
AnytoneCodeplugGen1::ChannelElement::txOffset() const {
  return ((unsigned)getBCD8_be(Offset::txFrequencyOffset()))*10;
}
void
AnytoneCodeplugGen1::ChannelElement::setTXOffset(unsigned hz) {
  setBCD8_be(Offset::txFrequencyOffset(), hz/10);
}

unsigned
AnytoneCodeplugGen1::ChannelElement::txFrequency() const {
  unsigned rx = rxFrequency(), off = txOffset();
  if (RepeaterMode::Simplex == repeaterMode())
    return rx;
  else if (RepeaterMode::Positive == repeaterMode())
    return rx+off;
  return rx-off;
}
void
AnytoneCodeplugGen1::ChannelElement::setTXFrequency(unsigned hz) {
  unsigned rx = rxFrequency();
  if (rx == hz) {
    setTXOffset(0); setRepeaterMode(RepeaterMode::Simplex);
  } else if (rx < hz) {
    setTXOffset(hz-rx); setRepeaterMode(RepeaterMode::Positive);
  } else {
    setTXOffset(rx-hz); setRepeaterMode(RepeaterMode::Negative);
  }
}

AnytoneCodeplugGen1::ChannelElement::Mode
AnytoneCodeplugGen1::ChannelElement::mode() const {
  return (Mode) getUInt2(Offset::channelMode());
}
void
AnytoneCodeplugGen1::ChannelElement::setMode(Mode mode) {
  setUInt2(Offset::channelMode(), (unsigned)mode);
}

Channel::Power
AnytoneCodeplugGen1::ChannelElement::power() const {
  switch ((Power)getUInt2(Offset::power())) {
  case POWER_LOW: return Channel::Power::Low;
  case POWER_MIDDLE: return Channel::Power::Mid;
  case POWER_HIGH: return Channel::Power::High;
  case POWER_TURBO: return Channel::Power::Max;
  }
  return Channel::Power::Low;
}
void
AnytoneCodeplugGen1::ChannelElement::setPower(Channel::Power power) {
  switch (power) {
  case Channel::Power::Min:
  case Channel::Power::Low:
    setUInt2(Offset::power(), (unsigned)POWER_LOW);
    break;
  case Channel::Power::Mid:
    setUInt2(Offset::power(), (unsigned)POWER_MIDDLE);
    break;
  case Channel::Power::High:
    setUInt2(Offset::power(), (unsigned)POWER_HIGH);
    break;
  case Channel::Power::Max:
    setUInt2(Offset::power(), (unsigned)POWER_TURBO);
    break;
  }
}

FMChannel::Bandwidth
AnytoneCodeplugGen1::ChannelElement::bandwidth() const {
  if (getBit(Offset::bandwidth()))
    return FMChannel::Bandwidth::Wide;
  return FMChannel::Bandwidth::Narrow;
}
void
AnytoneCodeplugGen1::ChannelElement::setBandwidth(FMChannel::Bandwidth bw) {
  switch (bw) {
  case FMChannel::Bandwidth::Narrow: setBit(Offset::bandwidth(), false); break;
  case FMChannel::Bandwidth::Wide: setBit(Offset::bandwidth(), true); break;
  }
}

AnytoneCodeplugGen1::ChannelElement::RepeaterMode
AnytoneCodeplugGen1::ChannelElement::repeaterMode() const {
  return (RepeaterMode)getUInt2(Offset::repeaterMode());
}
void
AnytoneCodeplugGen1::ChannelElement::setRepeaterMode(RepeaterMode mode) {
  setUInt2(Offset::repeaterMode(), (unsigned)mode);
}

AnytoneCodeplugGen1::ChannelElement::SignalingMode
AnytoneCodeplugGen1::ChannelElement::rxSignalingMode() const {
  return (SignalingMode)getUInt2(Offset::rxSignalingMode());
}
void
AnytoneCodeplugGen1::ChannelElement::setRXSignalingMode(SignalingMode mode) {
  setUInt2(Offset::rxSignalingMode(), (unsigned)mode);
}

SelectiveCall
AnytoneCodeplugGen1::ChannelElement::rxTone() const {
  if (SignalingMode::None == rxSignalingMode())
    return SelectiveCall();
  else if (SignalingMode::CTCSS == rxSignalingMode())
    return rxCTCSS();
  else if (SignalingMode::DCS == rxSignalingMode())
    return rxDCS();
  return SelectiveCall();
}
void
AnytoneCodeplugGen1::ChannelElement::setRXTone(const SelectiveCall &code) {
  if (code.isInvalid()) {
    setRXSignalingMode(SignalingMode::None);
  } else if (code.isCTCSS()) {
    setRXSignalingMode(SignalingMode::CTCSS);
    setRXCTCSS(code);
  } else if (code.isDCS()) {
    setRXSignalingMode(SignalingMode::DCS);
    setRXDCS(code);
  }
}

AnytoneCodeplugGen1::ChannelElement::SignalingMode
AnytoneCodeplugGen1::ChannelElement::txSignalingMode() const {
  return (SignalingMode)getUInt2(Offset::txSignalingMode());
}
void
AnytoneCodeplugGen1::ChannelElement::setTXSignalingMode(SignalingMode mode) {
  setUInt2(Offset::txSignalingMode(), (unsigned)mode);
}

SelectiveCall
AnytoneCodeplugGen1::ChannelElement::txTone() const {
  if (SignalingMode::None == txSignalingMode())
    return SelectiveCall();
  else if (SignalingMode::CTCSS == txSignalingMode())
    return txCTCSS();
  else if (SignalingMode::DCS == txSignalingMode())
    return txDCS();
  return SelectiveCall();
}
void
AnytoneCodeplugGen1::ChannelElement::setTXTone(const SelectiveCall &code) {
  if (code.isInvalid()) {
    setTXSignalingMode(SignalingMode::None);
  } else if (code.isCTCSS()) {
    setTXSignalingMode(SignalingMode::CTCSS);
    setTXCTCSS(code);
  } else if (code.isDCS()) {
    setTXSignalingMode(SignalingMode::DCS);
    setTXDCS(code);
  }
}

bool
AnytoneCodeplugGen1::ChannelElement::rxTxSwapped() const {
  return getBit(Offset::swapRXTX());
}
void
AnytoneCodeplugGen1::ChannelElement::enableSwapRxTx(bool enable) {
  setBit(Offset::swapRXTX(), enable);
}
bool
AnytoneCodeplugGen1::ChannelElement::rxOnly() const {
  return getBit(Offset::rxOnly());
}
void
AnytoneCodeplugGen1::ChannelElement::enableRXOnly(bool enable) {
  setBit(Offset::rxOnly(), enable);
}
bool
AnytoneCodeplugGen1::ChannelElement::callConfirm() const {
  return getBit(Offset::callConfirm());
}
void
AnytoneCodeplugGen1::ChannelElement::enableCallConfirm(bool enable) {
  setBit(Offset::callConfirm(), enable);
}
bool
AnytoneCodeplugGen1::ChannelElement::talkaround() const {
  return getBit(Offset::talkaround());
}
void
AnytoneCodeplugGen1::ChannelElement::enableTalkaround(bool enable) {
  setBit(Offset::talkaround(), enable);
}


bool
AnytoneCodeplugGen1::ChannelElement::txCTCSSIsCustom() const {
  return CTCSS::customIndex() == getUInt8(Offset::txCTCSS());
}

SelectiveCall
AnytoneCodeplugGen1::ChannelElement::txCTCSS() const {
  if (txCTCSSIsCustom())
    return {customCTCSSFrequency()};
  return CTCSS::decode(getUInt8(Offset::txCTCSS()));
}

void
AnytoneCodeplugGen1::ChannelElement::setTXCTCSS(const SelectiveCall &tone) {
  setUInt8(Offset::txCTCSS(), CTCSS::encode(tone));
  if (txCTCSSIsCustom()) setCustomCTCSSFrequency(tone.Hz());
}

void
AnytoneCodeplugGen1::ChannelElement::enableTXCustomCTCSS() {
  setUInt8(Offset::txCTCSS(), CTCSS::customIndex());
}


bool
AnytoneCodeplugGen1::ChannelElement::rxCTCSSIsCustom() const {
  return CTCSS::customIndex() == getUInt8(Offset::rxCTCSS());
}

SelectiveCall
AnytoneCodeplugGen1::ChannelElement::rxCTCSS() const {
  if (rxCTCSSIsCustom())
    return {customCTCSSFrequency()};
  return CTCSS::decode(getUInt8(Offset::rxCTCSS()));
}

void
AnytoneCodeplugGen1::ChannelElement::setRXCTCSS(const SelectiveCall &tone) {
  setUInt8(Offset::rxCTCSS(), CTCSS::encode(tone));
  if (rxCTCSSIsCustom()) setCustomCTCSSFrequency(tone.Hz());
}

void
AnytoneCodeplugGen1::ChannelElement::enableRXCustomCTCSS() {
  setUInt8(Offset::rxCTCSS(), CTCSS::customIndex());
}


SelectiveCall
AnytoneCodeplugGen1::ChannelElement::txDCS() const {
  uint16_t code = getUInt16_le(Offset::txDCS());
  if (512 > code)
    return SelectiveCall::fromBinaryDCS(code, false);
  return SelectiveCall::fromBinaryDCS(code-512, true);
}
void
AnytoneCodeplugGen1::ChannelElement::setTXDCS(const SelectiveCall &code) {
  if (code.isDCS())
    setUInt16_le(Offset::txDCS(), code.binCode() + (code.isInverted() ? 512 : 0));
  else
    setUInt16_le(Offset::txDCS(), 0);
}

SelectiveCall
AnytoneCodeplugGen1::ChannelElement::rxDCS() const {
  uint16_t code = getUInt16_le(Offset::rxDCS());
  if (512 > code)
    return SelectiveCall::fromBinaryDCS(code, false);
  return SelectiveCall::fromBinaryDCS(code-512, true);
}
void
AnytoneCodeplugGen1::ChannelElement::setRXDCS(const SelectiveCall &code) {
  if (code.isDCS())
    setUInt16_le(Offset::rxDCS(), code.binCode() + (code.isInverted() ? 512 : 0));
  else
    setUInt16_le(Offset::rxDCS(), 0);
}

double
AnytoneCodeplugGen1::ChannelElement::customCTCSSFrequency() const {
  return ((double) getUInt16_le(Offset::customCTCSS()))/10;
}
void
AnytoneCodeplugGen1::ChannelElement::setCustomCTCSSFrequency(double hz) {
  setUInt16_le(Offset::customCTCSS(), hz*10);
}

unsigned
AnytoneCodeplugGen1::ChannelElement::twoToneDecodeIndex() const {
  return getUInt16_le(Offset::twoFunctionIndex());
}
void
AnytoneCodeplugGen1::ChannelElement::setTwoToneDecodeIndex(unsigned idx) {
  setUInt16_le(Offset::twoFunctionIndex(), idx);
}

unsigned
AnytoneCodeplugGen1::ChannelElement::contactIndex() const {
  return getUInt32_le(Offset::contactIndex());
}
void
AnytoneCodeplugGen1::ChannelElement::setContactIndex(unsigned idx) {
  return setUInt32_le(Offset::contactIndex(), idx);
}

unsigned
AnytoneCodeplugGen1::ChannelElement::radioIDIndex() const {
  return getUInt8(Offset::radioIdIndex());
}
void
AnytoneCodeplugGen1::ChannelElement::setRadioIDIndex(unsigned idx) {
  return setUInt8(Offset::radioIdIndex(), idx);
}

AnytoneFMChannelExtension::SquelchMode
AnytoneCodeplugGen1::ChannelElement::squelchMode() const {
  return (AnytoneFMChannelExtension::SquelchMode)getUInt3(Offset::squelchMode());
}
void
AnytoneCodeplugGen1::ChannelElement::setSquelchMode(AnytoneFMChannelExtension::SquelchMode mode) {
  setUInt3(Offset::squelchMode(), (unsigned)mode);
}

AnytoneCodeplugGen1::ChannelElement::Admit
AnytoneCodeplugGen1::ChannelElement::admit() const {
  return (Admit)getUInt2(Offset::admitCriterion());
}
void
AnytoneCodeplugGen1::ChannelElement::setAdmit(Admit admit) {
  setUInt2(Offset::admitCriterion(), (unsigned)admit);
}

AnytoneCodeplugGen1::ChannelElement::OptSignaling
AnytoneCodeplugGen1::ChannelElement::optionalSignaling() const {
  return (OptSignaling)getUInt2(Offset::optionalSingnaling());
}
void
AnytoneCodeplugGen1::ChannelElement::setOptionalSignaling(OptSignaling sig) {
  setUInt2(Offset::optionalSingnaling(), (unsigned)sig);
}

bool
AnytoneCodeplugGen1::ChannelElement::hasScanListIndex() const {
  return 0xff != scanListIndex();
}
unsigned
AnytoneCodeplugGen1::ChannelElement::scanListIndex() const {
  return getUInt8(Offset::scanListIndex());
}
void
AnytoneCodeplugGen1::ChannelElement::setScanListIndex(unsigned idx) {
  setUInt8(Offset::scanListIndex(), idx);
}
void
AnytoneCodeplugGen1::ChannelElement::clearScanListIndex() {
  setScanListIndex(0xff);
}

bool
AnytoneCodeplugGen1::ChannelElement::hasGroupListIndex() const {
  return 0xff != groupListIndex();
}
unsigned
AnytoneCodeplugGen1::ChannelElement::groupListIndex() const {
  return getUInt8(Offset::groupListIndex());
}
void
AnytoneCodeplugGen1::ChannelElement::setGroupListIndex(unsigned idx) {
  setUInt8(Offset::groupListIndex(), idx);
}
void
AnytoneCodeplugGen1::ChannelElement::clearGroupListIndex() {
  setGroupListIndex(0xff);
}

unsigned
AnytoneCodeplugGen1::ChannelElement::twoToneIDIndex() const {
  return getUInt8(Offset::twoToneIDIndex());
}
void
AnytoneCodeplugGen1::ChannelElement::setTwoToneIDIndex(unsigned idx) {
  setUInt8(Offset::twoToneIDIndex(), idx);
}
unsigned
AnytoneCodeplugGen1::ChannelElement::fiveToneIDIndex() const {
  return getUInt8(Offset::fiveToneIDIndex());
}
void
AnytoneCodeplugGen1::ChannelElement::setFiveToneIDIndex(unsigned idx) {
  setUInt8(Offset::fiveToneIDIndex(), idx);
}
unsigned
AnytoneCodeplugGen1::ChannelElement::dtmfIDIndex() const {
  return getUInt8(Offset::dtmfIDIndex());
}
void
AnytoneCodeplugGen1::ChannelElement::setDTMFIDIndex(unsigned idx) {
  setUInt8(Offset::dtmfIDIndex(), idx);
}

unsigned
AnytoneCodeplugGen1::ChannelElement::colorCode() const {
  return getUInt8(Offset::colorCode());
}
void
AnytoneCodeplugGen1::ChannelElement::setColorCode(unsigned code) {
  setUInt8(Offset::colorCode(), code);
}

DMRChannel::TimeSlot
AnytoneCodeplugGen1::ChannelElement::timeSlot() const {
  if (false == getBit(Offset::timeSlot()))
    return DMRChannel::TimeSlot::TS1;
  return DMRChannel::TimeSlot::TS2;
}
void
AnytoneCodeplugGen1::ChannelElement::setTimeSlot(DMRChannel::TimeSlot ts) {
  if (DMRChannel::TimeSlot::TS1 == ts)
    setBit(Offset::timeSlot(), false);
  else
    setBit(Offset::timeSlot(), true);
}

bool
AnytoneCodeplugGen1::ChannelElement::smsConfirm() const {
  return getBit(Offset::smsConfirm());
}
void
AnytoneCodeplugGen1::ChannelElement::enableSMSConfirm(bool enable) {
  setBit(Offset::smsConfirm(), enable);
}
bool
AnytoneCodeplugGen1::ChannelElement::simplexTDMA() const {
  return getBit(Offset::simplexTDMA());
}
void
AnytoneCodeplugGen1::ChannelElement::enableSimplexTDMA(bool enable) {
  setBit(Offset::simplexTDMA(), enable);
}
bool
AnytoneCodeplugGen1::ChannelElement::adaptiveTDMA() const {
  return getBit(Offset::adaptiveTDMA());
}
void
AnytoneCodeplugGen1::ChannelElement::enableAdaptiveTDMA(bool enable) {
  setBit(Offset::adaptiveTDMA(), enable);
}
bool
AnytoneCodeplugGen1::ChannelElement::rxAPRS() const {
  return getBit(Offset::rxAPRS());
}
void
AnytoneCodeplugGen1::ChannelElement::enableRXAPRS(bool enable) {
  setBit(Offset::rxAPRS(), enable);
}

bool
AnytoneCodeplugGen1::ChannelElement::loneWorker() const {
  return getBit(Offset::loneWorker());
}
void
AnytoneCodeplugGen1::ChannelElement::enableLoneWorker(bool enable) {
  setBit(Offset::loneWorker(), enable);
}

QString
AnytoneCodeplugGen1::ChannelElement::name() const {
  return readASCII(Offset::name(), Limit::nameLength(), 0x00);
}
void
AnytoneCodeplugGen1::ChannelElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::nameLength(), 0x00);
}


Channel *
AnytoneCodeplugGen1::ChannelElement::toChannelObj(Context &ctx) const {
  Q_UNUSED(ctx)

  Channel *ch;

  if ((Mode::Analog == mode()) || (Mode::MixedAnalog == mode())) {
    if (Mode::MixedAnalog == mode())
      logWarn() << "Mixed mode channels are not supported (for now). Treat ch '"
                << name() <<"' as analog channel.";
    FMChannel *ach = new FMChannel();
    switch(this->admit()) {
    case Admit::Always: ach->setAdmit(FMChannel::Admit::Always); break;
    case Admit::Busy: ach->setAdmit(FMChannel::Admit::Free); break;
    case Admit::Tone: ach->setAdmit(FMChannel::Admit::Tone); break;
    default: ach->setAdmit(FMChannel::Admit::Always); break;
    }
    ach->setRXTone(rxTone());
    ach->setTXTone(txTone());
    ach->setBandwidth(bandwidth());
    // no per channel squelch settings
    ach->setSquelchDefault();
    ach->extended()->enableTalkaround(talkaround());

    // Create extension
    AnytoneFMChannelExtension *ext = new AnytoneFMChannelExtension();
    ach->setAnytoneChannelExtension(ext);
    ext->enableRXCustomCTCSS(rxCTCSSIsCustom());
    ext->enableTXCustomCTCSS(txCTCSSIsCustom());
    ext->setCustomCTCSS(customCTCSSFrequency());
    ext->setSquelchMode(squelchMode());

    // done
    ch = ach;
  } else if ((Mode::Digital == mode()) || (Mode::MixedDigital == mode())) {
    if (Mode::MixedDigital == mode())
      logWarn() << "Mixed mode channels are not supported (for now). Treat ch '"
                << name() <<"' as digital channel.";
    DMRChannel *dch = new DMRChannel();
    switch (this->admit()) {
    case Admit::Always: dch->setAdmit(DMRChannel::Admit::Always); break;
    case Admit::Free: dch->setAdmit(DMRChannel::Admit::Free); break;
    case Admit::DifferentColorCode:dch->setAdmit(DMRChannel::Admit::ColorCode); break;
    case Admit::SameColorCode:
      logWarn() << "Cannot decode admit cirit. 'same CC', use 'different CC' instead.";
      dch->setAdmit(DMRChannel::Admit::ColorCode);
      break;
    }
    dch->setColorCode(colorCode());
    dch->setTimeSlot(timeSlot());
    dch->extended()->enableTalkaround(talkaround());
    dch->extended()->enablePrivateCallConfirm(callConfirm());
    dch->extended()->enableSMSConfirm(smsConfirm());
    dch->extended()->enableDCDM(simplexTDMA());
    dch->extended()->enableLoneWorker(loneWorker());

    // Create extension
    AnytoneDMRChannelExtension *ext = new AnytoneDMRChannelExtension();
    dch->setAnytoneChannelExtension(ext);
    ext->enableAdaptiveTDMA(adaptiveTDMA());
    // Done
    ch = dch;
  } else {
    logError() << "Cannot create channel '" << name()
               << "': Channel type " << (unsigned)mode() << "not supported.";
    return nullptr;
  }

  ch->setName(name());
  ch->setRXFrequency(Frequency::fromHz(rxFrequency()));
  ch->setTXFrequency(Frequency::fromHz(txFrequency()));
  ch->setPower(power());
  ch->setRXOnly(rxOnly());

  // No per channel vox & tot setting
  ch->setVOXDefault();
  ch->setDefaultTimeout();

  return ch;
}

bool
AnytoneCodeplugGen1::ChannelElement::linkChannelObj(Channel *c, Context &ctx) const {
  if (Mode::Digital == mode()) {
    // If channel is digital
    DMRChannel *dc = c->as<DMRChannel>();
    if (nullptr == dc)
      return false;

    // Check if default contact is set, in fact a valid contact index is mandatory.
    if (! ctx.has<DMRContact>(contactIndex())) {
      logError() << "Cannot resolve contact index " << contactIndex() << " for channel '"
                 << c->name() << "'.";
      return false;
    }
    dc->setContact(ctx.get<DMRContact>(contactIndex()));

    // Set if RX group list is set
    if (hasGroupListIndex() && ctx.has<RXGroupList>(groupListIndex()))
      dc->setGroupList(ctx.get<RXGroupList>(groupListIndex()));

    // Link radio ID
    DMRRadioID *rid = ctx.get<DMRRadioID>(radioIDIndex());
    if (rid == ctx.config()->settings()->defaultIdRef()->as<DMRRadioID>())
      dc->setRadioId(DefaultRadioID::get());
    else
      dc->setRadioId(rid);
  } else if (Mode::Analog == mode()) {
    // If channel is analog
    FMChannel *ac = c->as<FMChannel>();
    if (nullptr == ac)
      return false;
  }

  // For both, analog and digital channels:

  // If channel has scan list
  if (hasScanListIndex() && ctx.has<ScanList>(scanListIndex()))
    c->setScanList(ctx.get<ScanList>(scanListIndex()));

  return true;
}

bool
AnytoneCodeplugGen1::ChannelElement::fromChannelObj(const Channel *c, Context &ctx) {
  // Clear channel element
  clear();

  // set channel name
  setName(c->name());
  // set rx and tx frequencies
  setRXFrequency(c->rxFrequency().inHz());
  setTXFrequency(c->txFrequency().inHz());
  // set power
  if (c->defaultPower())
    setPower(ctx.config()->settings()->power());
  else
    setPower(c->power());
  // set tx-enable
  enableRXOnly(c->rxOnly());
  if (nullptr == c->scanList())
    clearScanListIndex();
  else
    setScanListIndex(ctx.index(c->scanList()));

  // Dispatch by channel type
  if (c->is<FMChannel>()) {
    const FMChannel *ac = c->as<const FMChannel>();
    setMode(Mode::Analog);
    // set admit criterion
    switch (ac->admit()) {
    case FMChannel::Admit::Always: setAdmit(Admit::Always); break;
    case FMChannel::Admit::Free: setAdmit(Admit::Busy); break;
    case FMChannel::Admit::Tone: setAdmit(Admit::Tone); break;
    }
    // squelch mode
    setRXTone(ac->rxTone());
    setTXTone(ac->txTone());
    if (ac->rxTone().isValid())
      setSquelchMode(AnytoneFMChannelExtension::SquelchMode::SubTone);
    else
      setSquelchMode(AnytoneFMChannelExtension::SquelchMode::Carrier);
    // set bandwidth
    setBandwidth(ac->bandwidth());
    // Apply common settings
    enableTalkaround(ac->extended()->talkaround());
    // Apply FM settings

    // Handle extension
    if (AnytoneFMChannelExtension *ext = ac->anytoneChannelExtension()) {
      setCustomCTCSSFrequency(ext->customCTCSS());
      if (ext->rxCustomCTCSS())
        enableRXCustomCTCSS();
      if (ext->txCustomCTCSS())
        enableTXCustomCTCSS();
      setSquelchMode(ext->squelchMode());
    }
  } else if (c->is<DMRChannel>()) {
    const DMRChannel *dc = c->as<const DMRChannel>();
    // pack digital channel config.
    setMode(Mode::Digital);
    // set admit criterion
    switch(dc->admit()) {
    case DMRChannel::Admit::Always: setAdmit(Admit::Always); break;
    case DMRChannel::Admit::Free: setAdmit(Admit::Free); break;
    case DMRChannel::Admit::ColorCode: setAdmit(Admit::DifferentColorCode); break;
    }
    // set color code
    setColorCode(dc->colorCode());
    // set time-slot
    setTimeSlot(dc->timeSlot());
    // link transmit contact
    if (nullptr == dc->contact())
      setContactIndex(0);
    else
      setContactIndex(ctx.index(dc->contact()));
    // link RX group list
    if (nullptr == dc->groupList())
      clearGroupListIndex();
    else
      setGroupListIndex(ctx.index(dc->groupList()));
    // Set radio ID
    if ((nullptr == dc->radioId()) || (DefaultRadioID::get() == dc->radioId())) {
      if (nullptr == ctx.config()->settings()->defaultIdRef()->as<DMRRadioID>()) {
        logWarn() << "No default radio ID set: using index 0.";
        setRadioIDIndex(0);
      } else {
        setRadioIDIndex(ctx.index(ctx.config()->settings()->defaultIdRef()->as<DMRRadioID>()));
      }
    } else {
      setRadioIDIndex(ctx.index(dc->radioId()));
    }
    // Apply common settings
    enableTalkaround(dc->extended()->talkaround());
    // Apply DMR settings
    enableCallConfirm(dc->extended()->privateCallConfirm());
    enableSMSConfirm(dc->extended()->smsConfirm());
    enableSimplexTDMA(dc->extended()->dcdm());
    enableLoneWorker(dc->extended()->loneWorker());
    // Handle extension
    if (AnytoneDMRChannelExtension *ext = dc->anytoneChannelExtension()) {
      enableAdaptiveTDMA(ext->adaptiveTDMA());
    }
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::ChannelBitmapElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::ChannelBitmapElement::ChannelBitmapElement(uint8_t *ptr, size_t size)
  : BitmapElement(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::ChannelBitmapElement::ChannelBitmapElement(uint8_t *ptr)
  : BitmapElement(ptr, ChannelBitmapElement::size())
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::ContactElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::ContactElement::ContactElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::ContactElement::ContactElement(uint8_t *ptr)
  : Element(ptr, ContactElement::size())
{
  // pass...
}

AnytoneCodeplugGen1::ContactElement::~ContactElement() {
  // pass...
}

void
AnytoneCodeplugGen1::ContactElement::clear() {
  memset(_data, 0, _size);
}

bool
AnytoneCodeplugGen1::ContactElement::isValid() const {
  return !name().isEmpty();
}

DMRContact::Type
AnytoneCodeplugGen1::ContactElement::type() const {
  switch (getUInt8(Offset::type())) {
  case 0: return DMRContact::PrivateCall;
  case 1: return DMRContact::GroupCall;
  case 2: return DMRContact::AllCall;
  }
  return DMRContact::PrivateCall;
}
void
AnytoneCodeplugGen1::ContactElement::setType(DMRContact::Type type) {
  switch (type) {
  case DMRContact::PrivateCall: setUInt8(Offset::type(), 0); break;
  case DMRContact::GroupCall: setUInt8(Offset::type(), 1); break;
  case DMRContact::AllCall: setUInt8(Offset::type(), 2); break;
  }
}

QString
AnytoneCodeplugGen1::ContactElement::name() const {
  return readASCII(Offset::name(), Limit::nameLength(), 0x00);
}
void
AnytoneCodeplugGen1::ContactElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::nameLength(), 0x00);
}

unsigned
AnytoneCodeplugGen1::ContactElement::number() const {
  return getBCD8_be(Offset::number());
}
void
AnytoneCodeplugGen1::ContactElement::setNumber(unsigned number) {
  setBCD8_be(Offset::number(), number);
}

AnytoneContactExtension::AlertType
AnytoneCodeplugGen1::ContactElement::alertType() const {
  switch (getUInt8(Offset::alertType())) {
  case (uint8_t)AnytoneContactExtension::AlertType::None:
    return AnytoneContactExtension::AlertType::None;
  case (uint8_t)AnytoneContactExtension::AlertType::Ring:
    return AnytoneContactExtension::AlertType::Ring;
  case (uint8_t)AnytoneContactExtension::AlertType::Online:
    return AnytoneContactExtension::AlertType::Online;
  default:
    break;
  }
  logWarn() << "Unknown value " << getUInt8(Offset::alertType())
            << " for alert type of AnyTone contact element. Maybe the codeplug implementation is"
               " outdated. Consider reporting it at https://github.com/hmatuschek/qdmr.";
  return AnytoneContactExtension::AlertType::None;
}
void
AnytoneCodeplugGen1::ContactElement::setAlertType(AnytoneContactExtension::AlertType type) {
  setUInt8(Offset::alertType(), (unsigned)type);
}

DMRContact *
AnytoneCodeplugGen1::ContactElement::toContactObj(Context &ctx) const {
  Q_UNUSED(ctx);

  // Common settings
  DMRContact *cont = new DMRContact();
  cont->setType(type());
  cont->setName(name());
  cont->setNumber(number());
  cont->setRing(AnytoneContactExtension::AlertType::None != alertType());

  // Create AnyTone specific extension
  AnytoneContactExtension *ext = new AnytoneContactExtension();
  cont->setAnytoneExtension(ext);
  ext->setAlertType(alertType());

  return cont;
}

bool
AnytoneCodeplugGen1::ContactElement::fromContactObj(const DMRContact *contact, Context &ctx) {
  Q_UNUSED(ctx)

  clear();

  setType(contact->type());
  setName(contact->name());
  setNumber(contact->number());
  setAlertType(contact->ring() ? AnytoneContactExtension::AlertType::Ring :
                                 AnytoneContactExtension::AlertType::None);

  if (AnytoneContactExtension *ext = contact->anytoneExtension()) {
    setAlertType(ext->alertType());
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::ContactBitmapElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::ContactBitmapElement::ContactBitmapElement(uint8_t *ptr, size_t size)
  : InvertedBitmapElement(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::ContactBitmapElement::ContactBitmapElement(uint8_t *ptr)
  : InvertedBitmapElement(ptr, ContactBitmapElement::size())
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::DTMFContactElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::DTMFContactElement::DTMFContactElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::DTMFContactElement::DTMFContactElement(uint8_t *ptr)
  : Element(ptr, DTMFContactElement::size())
{
  // pass...
}

AnytoneCodeplugGen1::DTMFContactElement::~DTMFContactElement() {
  // pass...
}

void
AnytoneCodeplugGen1::DTMFContactElement::clear() {
  memset(_data, 0, _size);
}

QString
AnytoneCodeplugGen1::DTMFContactElement::number() const {
  QString number;
  int n = getUInt8(Offset::numDigits());
  for (int i=0; i<n; i++) {
    uint8_t byte = _data[Offset::digits() + i/2];
    if (0 == (i%2))
      number.append(_anytone_bin_dtmf_tab[(byte>>4)&0xf]);
    else
      number.append(_anytone_bin_dtmf_tab[(byte>>0)&0xf]);
  }
  return number;
}
void
AnytoneCodeplugGen1::DTMFContactElement::setNumber(const QString &number) {
  if (! validDTMFNumber(number))
    return;
  memset(_data+Offset::digits(), 0, Limit::digitCount()/2);
  unsigned int n = std::min((unsigned int)number.length(), Limit::digitCount());
  setUInt8(Offset::numDigits(), n);
  for (unsigned int i=0; i<n; i++) {
    if (0 == (i%2))
      _data[Offset::digits() + i/2] |= (_anytone_bin_dtmf_tab.indexOf(number[i].toLatin1())<<4);
    else
      _data[Offset::digits() + i/2] |= (_anytone_bin_dtmf_tab.indexOf(number[i].toLatin1())<<0);
  }
}

QString
AnytoneCodeplugGen1::DTMFContactElement::name() const {
  return readASCII(Offset::name(), Limit::nameLength(), 0x00);
}
void
AnytoneCodeplugGen1::DTMFContactElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::nameLength(), 0x00);
}

DTMFContact *
AnytoneCodeplugGen1::DTMFContactElement::toContact() const {
  return new DTMFContact(name(), number());
}

bool
AnytoneCodeplugGen1::DTMFContactElement::fromContact(const DTMFContact *contact) {
  setNumber(contact->number());
  setName(contact->name());
  return true;
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::DTMFContactBytemapElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::DTMFContactBytemapElement::DTMFContactBytemapElement(uint8_t *ptr, size_t size)
  : InvertedBytemapElement(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::DTMFContactBytemapElement::DTMFContactBytemapElement(uint8_t *ptr)
  : InvertedBytemapElement(ptr, DTMFContactBytemapElement::size())
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::GroupListElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::GroupListElement::GroupListElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::GroupListElement::GroupListElement(uint8_t *ptr)
  : Element(ptr, GroupListElement::size())
{
  // pass...
}

void
AnytoneCodeplugGen1::GroupListElement::clear() {
  memset(_data, 0x00, _size);
  // set member indices to 0xffffffff
  memset(_data, 0xff, Offset::betweenMembers()*Limit::members());
}

bool
AnytoneCodeplugGen1::GroupListElement::isValid() const {
  return !name().isEmpty();
}

QString
AnytoneCodeplugGen1::GroupListElement::name() const {
  return readASCII(Offset::name(), Limit::nameLength(), 0x00);
}
void
AnytoneCodeplugGen1::GroupListElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::nameLength(), 0x00);
}

bool
AnytoneCodeplugGen1::GroupListElement::hasMemberIndex(unsigned n) const {
  return 0xffffffff != memberIndex(n);
}
unsigned
AnytoneCodeplugGen1::GroupListElement::memberIndex(unsigned n) const {
  return getUInt32_le(Offset::members() + Offset::betweenMembers()*n);
}
void
AnytoneCodeplugGen1::GroupListElement::setMemberIndex(unsigned n, unsigned idx) {
  setUInt32_le(Offset::members() + Offset::betweenMembers()*n, idx);
}
void
AnytoneCodeplugGen1::GroupListElement::clearMemberIndex(unsigned n) {
  setMemberIndex(n, 0xffffffff);
}

RXGroupList *
AnytoneCodeplugGen1::GroupListElement::toGroupListObj() const {
  return new RXGroupList(name());
}

bool
AnytoneCodeplugGen1::GroupListElement::linkGroupList(RXGroupList *lst, Context &ctx) const {
  for (uint8_t i=0; i<Limit::members(); i++) {
    // Disabled contact -> continue
    if (! hasMemberIndex(i))
      continue;
    // Missing contact ignore.
    if (! ctx.has<DMRContact>(memberIndex(i))) {
      logWarn() << "Cannot link contact " << memberIndex(i) << " to group list '"
                << this->name() << "': Invalid contact index. Ignored.";
      continue;
    }
    lst->addContact(ctx.get<DMRContact>(memberIndex(i)));
  }
  return true;
}

bool
AnytoneCodeplugGen1::GroupListElement::fromGroupListObj(const RXGroupList *lst, Context &ctx) {
  clear();

  // set name of group list
  setName(lst->name());

  int j=0;
  // set members
  for (uint8_t i=0; i<Limit::members(); i++) {
    // Skip non-private-call entries
    while((lst->count() > j) && (DMRContact::GroupCall != lst->contact(j)->type())) {
      logWarn() << "Contact '" << lst->contact(j)->name() << "' in group list '" << lst->name()
                << "' is not a group call. Skip entry.";
      j++;
    }

    if (lst->count() > j) {
      setMemberIndex(i, ctx.index(lst->contact(j))); j++;
    } else {
      clearMemberIndex(i);
    }
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::GroupListBitmapElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::GroupListBitmapElement::GroupListBitmapElement(uint8_t *ptr, size_t size)
  : BitmapElement(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::GroupListBitmapElement::GroupListBitmapElement(uint8_t *ptr)
  : BitmapElement(ptr, GroupListBitmapElement::size())
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::ScanListElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::ScanListElement::ScanListElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::ScanListElement::ScanListElement(uint8_t *ptr)
  : Element(ptr, ScanListElement::size())
{
  // pass...
}

void
AnytoneCodeplugGen1::ScanListElement::clear() {
  memset(_data, 0, _size);
  setPriorityChannels(PriChannel::Off);
  clearPrimaryChannel();
  clearSecondaryChannel();
  setLookBackTimeA(Interval::fromSeconds(150));
  setLookBackTimeB(Interval::fromSeconds(250));
  setDropOutDelay(Interval::fromSeconds(290));
  setDwellTime(Interval::fromSeconds(290));
  setRevertChannel(RevertChannel::Selected);
  // clear members
  memset(_data+Offset::members(), 0xff, Offset::betweenMembers()*Limit::members());
}

AnytoneCodeplugGen1::ScanListElement::PriChannel
AnytoneCodeplugGen1::ScanListElement::priorityChannels() const {
  return (PriChannel) getUInt8(Offset::priorityChannel());
}
void
AnytoneCodeplugGen1::ScanListElement::setPriorityChannels(PriChannel sel) {
  setUInt8(Offset::priorityChannel(), (unsigned)sel);
}

bool
AnytoneCodeplugGen1::ScanListElement::hasPrimary() const {
  return 0xffff != getUInt16_le(Offset::primaryChannel());
}
bool
AnytoneCodeplugGen1::ScanListElement::primaryIsSelected() const {
  return 0 == getUInt16_le(Offset::primaryChannel());
}
unsigned
AnytoneCodeplugGen1::ScanListElement::primary() const {
  return ((unsigned)getUInt16_le(Offset::primaryChannel()))-1;
}
void
AnytoneCodeplugGen1::ScanListElement::setPrimary(unsigned idx) {
  setUInt16_le(Offset::primaryChannel(), idx+1);
}
void
AnytoneCodeplugGen1::ScanListElement::setPrimarySelected() {
  setUInt16_le(Offset::primaryChannel(), 0);
}
void
AnytoneCodeplugGen1::ScanListElement::clearPrimaryChannel() {
  setUInt16_le(Offset::primaryChannel(), 0xffff);
}

bool
AnytoneCodeplugGen1::ScanListElement::hasSecondary() const {
  return 0xffff != getUInt16_le(Offset::secondaryChannel());
}
bool
AnytoneCodeplugGen1::ScanListElement::secondaryIsSelected() const {
  return 0 == getUInt16_le(Offset::secondaryChannel());
}
unsigned
AnytoneCodeplugGen1::ScanListElement::secondary() const {
  return ((unsigned)getUInt16_le(Offset::secondaryChannel()))-1;
}
void
AnytoneCodeplugGen1::ScanListElement::setSecondary(unsigned idx) {
  setUInt16_le(Offset::secondaryChannel(), idx+1);
}
void
AnytoneCodeplugGen1::ScanListElement::setSecondarySelected() {
  setUInt16_le(Offset::secondaryChannel(), 0);
}
void
AnytoneCodeplugGen1::ScanListElement::clearSecondaryChannel() {
  setUInt16_le(Offset::secondaryChannel(), 0xffff);
}

Interval
AnytoneCodeplugGen1::ScanListElement::lookBackTimeA() const {
  return Interval::fromSeconds((unsigned)getUInt16_le(Offset::lookBackTimeA()) * 10);
}
void
AnytoneCodeplugGen1::ScanListElement::setLookBackTimeA(const Interval& sec) {
  setUInt16_le(Offset::lookBackTimeA(), sec.seconds()/10);
}

Interval AnytoneCodeplugGen1::ScanListElement::lookBackTimeB() const {
  return Interval::fromSeconds((unsigned)getUInt16_le(Offset::lookBackTimeB())*10);
}
void
AnytoneCodeplugGen1::ScanListElement::setLookBackTimeB(const Interval &sec) {
  setUInt16_le(Offset::lookBackTimeB(), sec.seconds()/10);
}

Interval
AnytoneCodeplugGen1::ScanListElement::dropOutDelay() const {
  return Interval::fromSeconds((unsigned)getUInt16_le(Offset::dropOutDelay())*10);
}
void
AnytoneCodeplugGen1::ScanListElement::setDropOutDelay(const Interval &sec) {
  setUInt16_le(Offset::dropOutDelay(), sec.seconds()/10);
}

Interval
AnytoneCodeplugGen1::ScanListElement::dwellTime() const {
  return Interval::fromSeconds((unsigned)getUInt16_le(Offset::dwellTime()) * 10);
}
void
AnytoneCodeplugGen1::ScanListElement::setDwellTime(const Interval &sec) {
  setUInt16_le(Offset::dwellTime(), sec.seconds()/10);
}

AnytoneCodeplugGen1::ScanListElement::RevertChannel
AnytoneCodeplugGen1::ScanListElement::revertChannel() const {
  return (RevertChannel)getUInt8(Offset::revertChannel());
}
void
AnytoneCodeplugGen1::ScanListElement::setRevertChannel(RevertChannel type) {
  setUInt8(Offset::revertChannel(), (unsigned)type);
}

QString
AnytoneCodeplugGen1::ScanListElement::name() const {
  return readASCII(Offset::name(), Limit::nameLength(), 0x00);
}
void
AnytoneCodeplugGen1::ScanListElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::nameLength(), 0x00);
}

bool
AnytoneCodeplugGen1::ScanListElement::hasMemberIndex(unsigned n) const {
  return 0xffff != memberIndex(n);
}
unsigned
AnytoneCodeplugGen1::ScanListElement::memberIndex(unsigned n) const {
  return getUInt16_le(Offset::members() + Offset::betweenMembers()*n);
}
void
AnytoneCodeplugGen1::ScanListElement::setMemberIndex(unsigned n, unsigned idx) {
  setUInt16_le(Offset::members() + Offset::betweenMembers()*n, idx);
}
void
AnytoneCodeplugGen1::ScanListElement::clearMemberIndex(unsigned n) {
  setMemberIndex(n, 0xffff);
}

ScanList *
AnytoneCodeplugGen1::ScanListElement::toScanListObj() const {
  return new ScanList(name());
}

bool
AnytoneCodeplugGen1::ScanListElement::linkScanListObj(ScanList *lst, Context &ctx) const {
  if (((PriChannel::Both == priorityChannels()) || (PriChannel::Primary==priorityChannels())) && hasPrimary()) {
    if (primaryIsSelected())
      lst->setPrimaryChannel(SelectedChannel::get());
    else if (ctx.has<Channel>(primary()))
      lst->setPrimaryChannel(ctx.get<Channel>(primary()));
  }

  if (((PriChannel::Both == priorityChannels()) || (PriChannel::Secondary==priorityChannels())) && hasSecondary()) {
    if (secondaryIsSelected())
      lst->setSecondaryChannel(SelectedChannel::get());
    else if (ctx.has<Channel>(secondary()))
      lst->setSecondaryChannel(ctx.get<Channel>(secondary()));
  }

  for (uint16_t i=0; i<Limit::members(); i++) {
    if (! hasMemberIndex(i))
      continue;
    if (! ctx.has<Channel>(memberIndex(i))) {
      logError() << "Cannot link scanlist '" << name() << "', channel index "
                 << memberIndex(i) << " unknown.";
      continue;
    }
    lst->addChannel(ctx.get<Channel>(memberIndex(i)));
  }

  return true;
}

bool
AnytoneCodeplugGen1::ScanListElement::fromScanListObj(ScanList *lst, Context &ctx) {
  clear();

  setName(lst->name());

  if (lst->primaryChannel() && lst->secondaryChannel()) {
    setPriorityChannels(PriChannel::Both);
  } else if (lst->primaryChannel()) {
    setPriorityChannels(PriChannel::Primary);
  } else if (lst->secondaryChannel()) {
    setPriorityChannels(PriChannel::Secondary);
  }

  if (lst->primaryChannel()) {
    if (SelectedChannel::get() == lst->primaryChannel())
      setPrimarySelected();
    else
      setPrimary(ctx.index(lst->primaryChannel()));
  }

  if (lst->secondaryChannel()) {
    if (SelectedChannel::get() == lst->secondaryChannel())
      setSecondarySelected();
    else
      setSecondary(ctx.index(lst->secondaryChannel()));
  }

  for (int i=0; i<std::min((int)Limit::members(), lst->count()); i++) {
    if (SelectedChannel::get() == lst->channel(i))
      continue;
    setMemberIndex(i, ctx.index(lst->channel(i)));
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::ScanListBitmapElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::ScanListBitmapElement::ScanListBitmapElement(uint8_t *ptr, size_t size)
  : BitmapElement(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::ScanListBitmapElement::ScanListBitmapElement(uint8_t *ptr)
  : BitmapElement(ptr, ScanListBitmapElement::size())
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::RadioIDElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::RadioIDElement::RadioIDElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::RadioIDElement::RadioIDElement(uint8_t *ptr)
  : Element(ptr, RadioIDElement::size())
{
  // pass...
}

void
AnytoneCodeplugGen1::RadioIDElement::clear() {
  memset(_data, 0x00, _size);
}

unsigned AnytoneCodeplugGen1::RadioIDElement::number() const {
  return getBCD8_be(Offset::number());
}
void
AnytoneCodeplugGen1::RadioIDElement::setNumber(unsigned number) {
  setBCD8_be(Offset::number(), number);
}

QString
AnytoneCodeplugGen1::RadioIDElement::name() const {
  return readASCII(Offset::name(), Limit::nameLength(), 0x00);
}
void
AnytoneCodeplugGen1::RadioIDElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::nameLength(), 0x00);
}

DMRRadioID *
AnytoneCodeplugGen1::RadioIDElement::toRadioID() const {
  return new DMRRadioID(name(), number());
}
bool
AnytoneCodeplugGen1::RadioIDElement::fromRadioID(DMRRadioID *id) {
  setName(id->name());
  setNumber(id->number());
  return true;
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::RadioIDBitmapElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::RadioIDBitmapElement::RadioIDBitmapElement(uint8_t *ptr, size_t size)
  : BitmapElement(ptr, size)
{
   // pass...
}

AnytoneCodeplugGen1::RadioIDBitmapElement::RadioIDBitmapElement(uint8_t *ptr)
  : BitmapElement(ptr, RadioIDBitmapElement::size())
{
  // pass...
}



/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::PrimaryRadioIdElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::PrimaryRadioIdElement::PrimaryRadioIdElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}

void
AnytoneCodeplugGen1::PrimaryRadioIdElement::clear() {
  memset(_data, 0x00, size());
}

bool
AnytoneCodeplugGen1::PrimaryRadioIdElement::isValid() const {
  return 0 != number() && ! name().isEmpty();
}

unsigned int
AnytoneCodeplugGen1::PrimaryRadioIdElement::number() const {
  return getBCD8_be(Offset::id());
}

void
AnytoneCodeplugGen1::PrimaryRadioIdElement::setNumber(unsigned number) {
  setBCD8_be(Offset::id(), number);
}


bool
AnytoneCodeplugGen1::PrimaryRadioIdElement::enabled() const {
  return 0x01 == getUInt8(Offset::enabled());
}

void
AnytoneCodeplugGen1::PrimaryRadioIdElement::enable(bool enabled) {
  setUInt8(Offset::enabled(), enabled ? 0x01 : 0x00);
}


QString
AnytoneCodeplugGen1::PrimaryRadioIdElement::name() const {
  return readASCII(Offset::name(), Limit::name(), 0x00);
}

void
AnytoneCodeplugGen1::PrimaryRadioIdElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::name(), 0x00);
}

bool
AnytoneCodeplugGen1::PrimaryRadioIdElement::encode(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err);

  bool enabled = this->enabled();
  if (! flags.updateCodeplug())
    enabled = false;

  clear();

  if (nullptr == ctx.config()->settings()->defaultId())
    return true;

  auto id = ctx.config()->settings()->defaultId();
  setName(id->name());
  setNumber(id->number());
  enable(enabled);

  return true;
}

bool
AnytoneCodeplugGen1::PrimaryRadioIdElement::decode(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(err);
  // pass...
  return true;
}

bool
AnytoneCodeplugGen1::PrimaryRadioIdElement::link(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err);

  if (! isValid()) return true;

  // Check if ID is already present as a DMR radio id
  auto id = ctx.config()->radioIDs()->find(number());
  // If not, create one
  if (nullptr == id) {
    id = new DMRRadioID(name(), number());
    ctx.config()->radioIDs()->add(id, 0);
  }
  // ... and set as default.
  ctx.config()->settings()->setDefaultId(id);

  return true;
}



/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::GeneralSettingsElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

void
AnytoneCodeplugGen1::GeneralSettingsElement::clear() {
  memset(_data, 0, _size);
}

bool
AnytoneCodeplugGen1::GeneralSettingsElement::displayFrequency() const {
  return 0!=getUInt8(Offset::displayMode());
}
void
AnytoneCodeplugGen1::GeneralSettingsElement::enableDisplayFrequency(bool enable) {
  setUInt8(Offset::displayMode(), (enable ? 0x01 : 0x00));
}

bool
AnytoneCodeplugGen1::GeneralSettingsElement::autoKeyLock() const {
  return 0!=getUInt8(Offset::autoKeyLock());
}
void
AnytoneCodeplugGen1::GeneralSettingsElement::enableAutoKeyLock(bool enable) {
  setUInt8(Offset::autoKeyLock(), (enable ? 0x01 : 0x00));
}

Interval
AnytoneCodeplugGen1::GeneralSettingsElement::autoShutdownDelay() const {
  switch ((AutoShutdown) getUInt8(Offset::autoShutDown())) {
  case AutoShutdown::Off:         return Interval::fromMinutes(0);
  case AutoShutdown::After10min:  return Interval::fromMinutes(10);
  case AutoShutdown::After30min:  return Interval::fromMinutes(30);
  case AutoShutdown::After60min:  return Interval::fromMinutes(60);
  case AutoShutdown::After120min: return Interval::fromMinutes(120);
  }
  return Interval();
}
void
AnytoneCodeplugGen1::GeneralSettingsElement::setAutoShutdownDelay(Interval intv) {
  if (0 == intv.minutes()) {
    setUInt8(Offset::autoShutDown(), (unsigned)AutoShutdown::Off);
  } else if (intv.minutes() <= 10) {
    setUInt8(Offset::autoShutDown(), (unsigned)AutoShutdown::After10min);
  } else if (intv.minutes() <= 30) {
    setUInt8(Offset::autoShutDown(), (unsigned)AutoShutdown::After30min);
  } else if (intv.minutes() <= 60) {
    setUInt8(Offset::autoShutDown(), (unsigned)AutoShutdown::After60min);
  } else {
    setUInt8(Offset::autoShutDown(), (unsigned)AutoShutdown::After120min);
  }
}


BootSettings::BootDisplay
AnytoneCodeplugGen1::GeneralSettingsElement::bootDisplay() const {
  switch ((BootDisplay) getUInt8(Offset::bootDisplay())) {
    case BootDisplay::Default: return BootSettings::BootDisplay::Logo;
    case BootDisplay::CustomText: return BootSettings::BootDisplay::Text;
    case BootDisplay::CustomImage: return BootSettings::BootDisplay::Image;
  }
  return BootSettings::BootDisplay::Logo;
}

void
AnytoneCodeplugGen1::GeneralSettingsElement::setBootDisplay(BootSettings::BootDisplay mode) {
  switch (mode) {
  case BootSettings::BootDisplay::Logo: setUInt8(Offset::bootDisplay(), (unsigned)BootDisplay::Default); break;
  case BootSettings::BootDisplay::Text: setUInt8(Offset::bootDisplay(), (unsigned)BootDisplay::CustomText); break;
  case BootSettings::BootDisplay::Image: setUInt8(Offset::bootDisplay(), (unsigned)BootDisplay::CustomImage); break;
  }
}


bool
AnytoneCodeplugGen1::GeneralSettingsElement::bootPassword() const {
  return getUInt8(Offset::bootPassword());
}
void
AnytoneCodeplugGen1::GeneralSettingsElement::enableBootPassword(bool enable) {
  setUInt8(Offset::bootPassword(), (enable ? 0x01 : 0x00));
}

Level
AnytoneCodeplugGen1::GeneralSettingsElement::squelchLevelA() const {
  return Level::fromValue(getUInt8(Offset::squelchLevelA()), {1,5});
}
void
AnytoneCodeplugGen1::GeneralSettingsElement::setSquelchLevelA(Level level) {
  setUInt8(Offset::squelchLevelA(), level.mapTo({1,5}));
}
Level
AnytoneCodeplugGen1::GeneralSettingsElement::squelchLevelB() const {
  return Level::fromValue(getUInt8(Offset::squelchLevelB()), {1,5});
}
void
AnytoneCodeplugGen1::GeneralSettingsElement::setSquelchLevelB(Level level) {
  setUInt8(Offset::squelchLevelB(), level.mapTo({1, 5}));
}

bool
AnytoneCodeplugGen1::GeneralSettingsElement::fromConfig(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err);

  // If auto-enable GPS is enabled
  if (flags.autoEnableGPS()) {
    // Check if GPS is required -> enable
    if (ctx.config()->requiresGPS()) {
      enableGPS(true);
      // Set time zone based on system time zone.
      setGPSTimeZone(QTimeZone::systemTimeZone());
      enableGetGPSPosition(false);
    } else {
      enableGPS(false);
    }
  }

  // Encode boot settings
  setBootDisplay(ctx.config()->settings()->boot()->bootDisplay());
  enableBootPassword(ctx.config()->settings()->boot()->bootPasswordEnabled());
  enableDefaultChannel(
        ctx.config()->settings()->boot()->defaultChannelEnabled() &&
        (! ctx.config()->settings()->boot()->zoneA()->isNull()) &&
        (! ctx.config()->settings()->boot()->zoneB()->isNull()));

  if (defaultChannel()) {
    setDefaultZoneIndexA(ctx.index(ctx.config()->settings()->boot()->zoneA()->as<Zone>()));
    if (ctx.config()->settings()->boot()->channelA()->isNull() ||
        (! ctx.config()->settings()->boot()->zoneA()->as<Zone>()->A()->has(
           ctx.config()->settings()->boot()->channelA()->as<Channel>())))
      setDefaultChannelAToVFO();
    else
      setDefaultChannelAIndex(ctx.config()->settings()->boot()->zoneA()->as<Zone>()->A()->indexOf(
                                ctx.config()->settings()->boot()->channelA()->as<Channel>()));

    setDefaultZoneIndexB(ctx.index(ctx.config()->settings()->boot()->zoneB()->as<Zone>()));
    if (ctx.config()->settings()->boot()->channelB()->isNull() ||
        (! ctx.config()->settings()->boot()->zoneB()->as<Zone>()->A()->has(
           ctx.config()->settings()->boot()->channelB()->as<Channel>())))
      setDefaultChannelBToVFO();
    else
      setDefaultChannelBIndex(ctx.config()->settings()->boot()->zoneB()->as<Zone>()->A()->indexOf(
                                ctx.config()->settings()->boot()->channelB()->as<Channel>()));
  }

  // Handle audio settings:
  setSquelchLevelA(ctx.config()->settings()->audio()->squelch());
  setSquelchLevelB(ctx.config()->settings()->audio()->squelch());
  // Set microphone gain
  // For the 868UV, this setting sets both, FM and DMR mic gain.
  // For all other devices, there is an additional FM mic gain setting.
  setDMRMicGain(ctx.config()->settings()->audio()->micGain());
  setMaxSpeakerVolume(ctx.config()->settings()->audio()->maxSpeakerVolume());

  // Handle tone settings:
  enableKeyTone(ctx.config()->settings()->tone()->keyToneEnabled());
  enableSMSAlert(ctx.config()->settings()->tone()->smsToneEnabled());
  enableCallAlert(ctx.config()->settings()->tone()->ringtoneEnabled());
  enableStartupTone(ctx.config()->settings()->tone()->bootToneEnabled());
  enableDMRTalkPermit(ctx.config()->settings()->tone()->talkPermit().testFlag(Channel::Type::DMR));
  enableFMTalkPermit(ctx.config()->settings()->tone()->talkPermit().testFlag(Channel::Type::FM));
  setCallToneMelody(*ctx.config()->settings()->tone()->callStartMelody());
  setIdleToneMelody(*ctx.config()->settings()->tone()->channelIdleMelody());
  enableDMRResetTone(ctx.config()->settings()->tone()->callResetEnabled());
  setResetToneMelody(*ctx.config()->settings()->tone()->callResetMelody());

  enableGPSUnitsImperial(GNSSSettings::Units::Archaic == ctx.config()->settings()->gnss()->units());

  // Handle extensions
  if (AnytoneSettingsExtension *ext = ctx.config()->settings()->anytoneExtension()) {
    setAutoShutdownDelay(ext->powerSaveSettings()->autoShutdown());
    setVFOScanType(ext->vfoScanType());
    enableVFOModeA(AnytoneSettingsExtension::VFOMode::VFO == ext->modeA());
    enableVFOModeB(AnytoneSettingsExtension::VFOMode::VFO == ext->modeB());
    if ((AnytoneSettingsExtension::VFOMode::VFO == ext->modeA()) || ext->zoneA()->isNull())
      setMemoryZoneA(0);
    else
      setMemoryZoneA(ctx.index(ext->zoneA()->as<Zone>()));
    if ((AnytoneSettingsExtension::VFOMode::VFO == ext->modeB()) || ext->zoneB()->isNull())
      setMemoryZoneB(0);
    else
      setMemoryZoneB(ctx.index(ext->zoneB()->as<Zone>()));
    enableActiveChannelB(AnytoneSettingsExtension::VFO::B == ext->selectedVFO());
    enableSubChannel(ext->subChannelEnabled());
    setMinVFOScanFrequencyUHF(ext->minVFOScanFrequencyUHF());
    setMaxVFOScanFrequencyUHF(ext->maxVFOScanFrequencyUHF());
    setMinVFOScanFrequencyVHF(ext->minVFOScanFrequencyVHF());
    setMaxVFOScanFrequencyVHF(ext->maxVFOScanFrequencyVHF());

    // Encode key settings
    setFuncKeyAShort(ext->keySettings()->funcKeyAShort());
    setFuncKeyALong(ext->keySettings()->funcKeyALong());
    setFuncKeyBShort(ext->keySettings()->funcKeyBShort());
    setFuncKeyBLong(ext->keySettings()->funcKeyBLong());
    setFuncKeyCShort(ext->keySettings()->funcKeyCShort());
    setFuncKeyCLong(ext->keySettings()->funcKeyCLong());
    setFuncKey1Short(ext->keySettings()->funcKey1Short());
    setFuncKey1Long(ext->keySettings()->funcKey1Long());
    setFuncKey2Short(ext->keySettings()->funcKey2Short());
    setFuncKey2Long(ext->keySettings()->funcKey2Long());
    setLongPressDuration(ext->keySettings()->longPressDuration());
    enableAutoKeyLock(ext->keySettings()->autoKeyLockEnabled());

    // Encode display settings
    enableDisplayFrequency(ext->displaySettings()->displayFrequencyEnabled());
    setBrightness(ext->displaySettings()->brightness());
    enableCallEndPrompt(ext->displaySettings()->callEndPromptEnabled());
    setLastCallerDisplayMode(ext->displaySettings()->lastCallerDisplay());
    enableDisplayClock(ext->displaySettings()->showClockEnabled());
    enableDisplayCall(ext->displaySettings()->showCallEnabled());
    setCallDisplayColor(ext->displaySettings()->callColor());
    enableVolumeChangePrompt(ext->displaySettings()->volumeChangePromptEnabled());

    // Encode audio settings
    enableRecording(ext->audioSettings()->recordingEnabled());
    enableEnhancedAudio(ext->audioSettings()->enhanceAudioEnabled());

    // Encode menu settings
    setMenuExitTime(ext->menuSettings()->duration());

    // Encode auto-repeater settings
    setAutoRepeaterDirectionA(ext->autoRepeaterSettings()->directionA());
    setAutoRepeaterDirectionB(ext->autoRepeaterSettings()->directionB());
    if (ext->autoRepeaterSettings()->vhfRef()->isNull())
      clearAutoRepeaterOffsetFrequencyIndexVHF();
    else
      setAutoRepeaterOffsetFrequenyIndexVHF(
            ctx.index(ext->autoRepeaterSettings()->vhfRef()->as<AnytoneAutoRepeaterOffset>()));
    if (ext->autoRepeaterSettings()->uhfRef()->isNull())
      clearAutoRepeaterOffsetFrequencyIndexUHF();
    else
      setAutoRepeaterOffsetFrequenyIndexUHF(
            ctx.index(ext->autoRepeaterSettings()->uhfRef()->as<AnytoneAutoRepeaterOffset>()));
    setAutoRepeaterMinFrequencyVHF(ext->autoRepeaterSettings()->vhfMin());
    setAutoRepeaterMaxFrequencyVHF(ext->autoRepeaterSettings()->vhfMax());
    setAutoRepeaterMinFrequencyUHF(ext->autoRepeaterSettings()->uhfMin());
    setAutoRepeaterMaxFrequencyUHF(ext->autoRepeaterSettings()->uhfMax());

    // Encode GPS Settings
    setGPSTimeZone(ext->gpsSettings()->timeZone());

    // Encode other settings
    enableKeepLastCaller(ext->keepLastCallerEnabled());
  } else if (! flags.updateCodeplug()) {
    clearAutoRepeaterOffsetFrequencyIndexVHF();
    clearAutoRepeaterOffsetFrequencyIndexUHF();
  }

  return true;
}

bool
AnytoneCodeplugGen1::GeneralSettingsElement::updateConfig(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err);

  // get microphone gain
  ctx.config()->settings()->audio()->setMicGain(dmrMicGain());
  ctx.config()->settings()->audio()->setMaxSpeakerVolume(this->maxSpeakerVolume());

  // D868UV does not support speech synthesis?
  ctx.config()->settings()->audio()->enableSpeechSynthesis(false);
  ctx.config()->settings()->audio()->setSquelch(std::max(squelchLevelA(), squelchLevelB()));

  // Handle tone settings
  ctx.config()->settings()->tone()->setKeyToneVolume(
    this->keyToneEnabled() ? Level::fromValue(5) : Level::null());
  ctx.config()->settings()->tone()->enableSMSTone(smsAlert());
  ctx.config()->settings()->tone()->enableRingtone(callAlert());
  ctx.config()->settings()->tone()->enableBootTone(startupTone());
  ctx.config()->settings()->tone()->setTalkPermit(
    (dmrTalkPermit() ? Channel::Type::DMR : Channel::Type::None) |
    (fmTalkPermit() ? Channel::Type::FM : Channel::Type::None) );
  callToneMelody(*ctx.config()->settings()->tone()->callStartMelody());
  idleToneMelody(*ctx.config()->settings()->tone()->channelIdleMelody());
  ctx.config()->settings()->tone()->enableCallReset(dmrResetTone());
  resetToneMelody(*ctx.config()->settings()->tone()->callResetMelody());

  // Store boot settings
  ctx.config()->settings()->boot()->setBootDisplay(bootDisplay());
  ctx.config()->settings()->boot()->enableBootPassword(bootPassword());
  ctx.config()->settings()->boot()->enableDefaultChannel(this->defaultChannel());

  ctx.config()->settings()->gnss()->setUnits(
        this->gpsUnitsImperial() ? GNSSSettings::Units::Archaic :
                                   GNSSSettings::Units::Metric);

  // Set extension
  AnytoneSettingsExtension *ext = nullptr;
  if (ctx.config()->settings()->anytoneExtension())
    ext = ctx.config()->settings()->anytoneExtension();
  else
    ctx.config()->settings()->setAnytoneExtension(ext = new AnytoneSettingsExtension());

  ext->powerSaveSettings()->setAutoShutdown(autoShutdownDelay());
  ext->setVFOScanType(vfoScanType());
  ext->setModeA(vfoModeA() ? AnytoneSettingsExtension::VFOMode::VFO
                           : AnytoneSettingsExtension::VFOMode::Memory);
  ext->setModeB(vfoModeB() ? AnytoneSettingsExtension::VFOMode::VFO
                           : AnytoneSettingsExtension::VFOMode::Memory);
  if ((! vfoModeA()) && ctx.has<Zone>(memoryZoneA()))
    ext->zoneA()->set(ctx.get<Zone>(memoryZoneA()));
  if ((! vfoModeB()) && ctx.has<Zone>(memoryZoneB()))
    ext->zoneB()->set(ctx.get<Zone>(memoryZoneB()));
  ext->setSelectedVFO(activeChannelB() ? AnytoneSettingsExtension::VFO::B
                                       : AnytoneSettingsExtension::VFO::A);
  ext->enableSubChannel(subChannel());
  ext->setMinVFOScanFrequencyUHF(this->minVFOScanFrequencyUHF());
  ext->setMaxVFOScanFrequencyUHF(this->maxVFOScanFrequencyUHF());
  ext->setMinVFOScanFrequencyVHF(this->minVFOScanFrequencyVHF());
  ext->setMaxVFOScanFrequencyVHF(this->maxVFOScanFrequencyVHF());

  // Store key settings
  ext->keySettings()->setFuncKeyAShort(funcKeyAShort());
  ext->keySettings()->setFuncKeyALong(funcKeyALong());
  ext->keySettings()->setFuncKeyBShort(funcKeyBShort());
  ext->keySettings()->setFuncKeyBLong(funcKeyBLong());
  ext->keySettings()->setFuncKeyCShort(funcKeyCShort());
  ext->keySettings()->setFuncKeyCLong(funcKeyCLong());
  ext->keySettings()->setFuncKey1Short(funcKey1Short());
  ext->keySettings()->setFuncKey1Long(funcKey1Long());
  ext->keySettings()->setFuncKey2Short(funcKey2Short());
  ext->keySettings()->setFuncKey2Long(funcKey2Long());
  ext->keySettings()->setLongPressDuration(longPressDuration());
  ext->keySettings()->enableAutoKeyLock(autoKeyLock());

  // Store display settings
  ext->displaySettings()->enableDisplayFrequency(displayFrequency());
  ext->displaySettings()->setBrightness(brightness());
  ext->displaySettings()->enableVolumeChangePrompt(this->volumeChangePrompt());
  ext->displaySettings()->enableCallEndPrompt(this->callEndPrompt());
  ext->displaySettings()->setLastCallerDisplay(this->lastCallerDisplayMode());
  ext->displaySettings()->enableShowClock(displayClock());
  ext->displaySettings()->enableShowCall(displayCall());
  ext->displaySettings()->setCallColor(this->callDisplayColor());

  // Menu settings
  ext->menuSettings()->setDuration(this->menuExitTime());

  // Store audio settings
  ext->audioSettings()->enableRecording(recording());
  ext->audioSettings()->enableEnhanceAudio(this->enhanceAudio());

  // Store auto-repeater settings
  ext->autoRepeaterSettings()->setDirectionA(this->autoRepeaterDirectionA());
  ext->autoRepeaterSettings()->setDirectionB(autoRepeaterDirectionB());
  ext->autoRepeaterSettings()->setVHFMin(this->autoRepeaterMinFrequencyVHF());
  ext->autoRepeaterSettings()->setVHFMax(this->autoRepeaterMaxFrequencyVHF());
  ext->autoRepeaterSettings()->setUHFMin(this->autoRepeaterMinFrequencyUHF());
  ext->autoRepeaterSettings()->setUHFMax(this->autoRepeaterMaxFrequencyUHF());

  // Store GPS settings
  ext->gpsSettings()->setTimeZone(gpsTimeZone());

  // Other settings
  ext->enableKeepLastCaller(this->keepLastCaller());

  return true;
}

bool
AnytoneCodeplugGen1::GeneralSettingsElement::linkSettings(RadioSettings *settings, Context &ctx, const ErrorStack &err) {
  // Link boot settings
  if (this->defaultChannel()) {
    if (! ctx.has<Zone>(this->defaultZoneIndexA())) {
      errMsg(err) << "Cannot link default zone A. Zone index " << this->defaultZoneIndexA()
                  << " not defined.";
      return false;
    }

    Zone *zoneA = ctx.get<Zone>(this->defaultZoneIndexA());
    ctx.config()->settings()->boot()->zoneA()->set(zoneA);
    if (this->defaultChannelAIsVFO()) {
      // pass...
    } else if (this->defaultChannelAIndex() >= (unsigned int)zoneA->A()->count()) {
      errMsg(err) << "Cannot link default channel A. Index " << this->defaultChannelAIndex()
                  << " not defined.";
      return false;
    } else {
      ctx.config()->settings()->boot()->channelA()->set(
            zoneA->A()->get(this->defaultChannelAIndex())->as<Channel>());
    }

    if (! ctx.has<Zone>(this->defaultZoneIndexB())) {
      errMsg(err) << "Cannot link default zone B. Zone index " << this->defaultZoneIndexB()
                  << " not defined.";
      return false;
    }
    Zone *zoneB = ctx.get<Zone>(this->defaultZoneIndexB());
    ctx.config()->settings()->boot()->zoneB()->set(zoneB);
    if (this->defaultChannelBIsVFO()) {
      // pass...
    } else if (this->defaultChannelBIndex() >= (unsigned int)zoneB->A()->count()) {
      errMsg(err) << "Cannot link default channel B. Index " << this->defaultChannelBIndex()
                  << " not defined.";
      return false;
    } else {
      ctx.config()->settings()->boot()->channelB()->set(
            zoneB->A()->get(this->defaultChannelBIndex())->as<Channel>());
    }
  }

  if (! settings->anytoneExtension())
    return false;
  AnytoneSettingsExtension *ext = settings->anytoneExtension();

  // Link repeater offsets
  if (this->hasAutoRepeaterOffsetFrequencyIndexVHF()) {
    if (! ctx.has<AnytoneAutoRepeaterOffset>(this->autoRepeaterOffsetFrequencyIndexVHF())) {
      logWarn() << "Cannot link auto-repeater offset for VHF, index "
                << this->autoRepeaterOffsetFrequencyIndexVHF() << " not defined.";
    } else {
      ext->autoRepeaterSettings()->vhfRef()->set(
            ctx.get<AnytoneAutoRepeaterOffset>(this->autoRepeaterOffsetFrequencyIndexVHF()));
    }
  }

  if (this->hasAutoRepeaterOffsetFrequencyIndexUHF()) {
    if (! ctx.has<AnytoneAutoRepeaterOffset>(this->autoRepeaterOffsetFrequencyIndexUHF())) {
      logWarn() << "Cannot link auto-repeater offset for UHF, index "
                << this->autoRepeaterOffsetFrequencyIndexUHF() << " not defined.";
    } else {
      ext->autoRepeaterSettings()->uhfRef()->set(
            ctx.get<AnytoneAutoRepeaterOffset>(this->autoRepeaterOffsetFrequencyIndexUHF()));
    }
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::ExtendedSettingsElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::ExtendedSettingsElement::ExtendedSettingsElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

bool
AnytoneCodeplugGen1::ExtendedSettingsElement::fromConfig(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err);

  if (! flags.updateCodeplug())
    this->clear();

  if (nullptr == ctx.config()->settings()->anytoneExtension()) {
    // If there is no extension, done
    return true;
  }

  // Get extension
  AnytoneSettingsExtension *ext = ctx.config()->settings()->anytoneExtension();

  // Encode display settings
  setChannelBNameColor(ext->displaySettings()->channelBNameColor());
  setZoneANameColor(ext->displaySettings()->zoneNameColor());
  setZoneBNameColor(ext->displaySettings()->zoneBNameColor());

  return true;
}

bool
AnytoneCodeplugGen1::ExtendedSettingsElement::updateConfig(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err);

  // Get or add extension if not present
  AnytoneSettingsExtension *ext = ctx.config()->settings()->anytoneExtension();
  if (nullptr == ext) {
    ext = new AnytoneSettingsExtension();
    ctx.config()->settings()->setAnytoneExtension(ext);
  }

  // Store display settings
  ext->displaySettings()->setChannelBNameColor(channelBNameColor());
  ext->displaySettings()->setZoneNameColor(zoneANameColor());
  ext->displaySettings()->setZoneBNameColor(zoneBNameColor());

  return true;
}

bool
AnytoneCodeplugGen1::ExtendedSettingsElement::linkConfig(Context &ctx, const ErrorStack &err) {
  // Get or add extension if not present
  AnytoneSettingsExtension *ext = ctx.config()->settings()->anytoneExtension();
  if (nullptr == ext) {
    errMsg(err) << "Cannot link config extension: not set.";
    return false;
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::ZoneChannelListElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::ZoneChannelListElement::ZoneChannelListElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pas...
}

AnytoneCodeplugGen1::ZoneChannelListElement::ZoneChannelListElement(uint8_t *ptr)
  : Element(ptr, ZoneChannelListElement::size())
{
  // pass...
}

void
AnytoneCodeplugGen1::ZoneChannelListElement::clear() {
  memset(_data, 0x00, _size);
  memset(_data + Offset::channelsA(), 0xff, Offset::betweenChannels()*Limit::zones());
  memset(_data + Offset::channelsB(), 0xff, Offset::betweenChannels()*Limit::zones());
}

bool
AnytoneCodeplugGen1::ZoneChannelListElement::hasChannelA(unsigned n) const {
  return 0xffff != channelIndexA(n);
}
unsigned
AnytoneCodeplugGen1::ZoneChannelListElement::channelIndexA(unsigned n) const {
  return getUInt16_le(Offset::channelsA() + Offset::betweenChannels()*n);
}
void
AnytoneCodeplugGen1::ZoneChannelListElement::setChannelIndexA(unsigned n, unsigned idx) {
  setUInt16_le(Offset::channelsA() + Offset::betweenChannels()*n, idx);
}
void
AnytoneCodeplugGen1::ZoneChannelListElement::clearChannelIndexA(unsigned n) {
  setChannelIndexA(n, 0xffff);
}

bool
AnytoneCodeplugGen1::ZoneChannelListElement::hasChannelB(unsigned n) const {
  return 0xffff != channelIndexB(n);
}
unsigned
AnytoneCodeplugGen1::ZoneChannelListElement::channelIndexB(unsigned n) const {
  return getUInt16_le(Offset::channelsB() + Offset::betweenChannels()*n);
}
void
AnytoneCodeplugGen1::ZoneChannelListElement::setChannelIndexB(unsigned n, unsigned idx) {
  setUInt16_le(Offset::channelsB() + Offset::betweenChannels()*n, idx);
}
void
AnytoneCodeplugGen1::ZoneChannelListElement::clearChannelIndexB(unsigned n) {
  setChannelIndexB(n, 0xffff);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::ZoneBitmapElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::ZoneBitmapElement::ZoneBitmapElement(uint8_t *ptr, size_t size)
  : BitmapElement(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::ZoneBitmapElement::ZoneBitmapElement(uint8_t *ptr)
  : BitmapElement(ptr, ZoneBitmapElement::size())
{
  // pass...
}



/* ******************************************************************************************** *
 * Implementation of AnytoneCodeplugGen1::HiddenZoneBitmapElement
 * ******************************************************************************************** */
AnytoneCodeplugGen1::HiddenZoneBitmapElement::HiddenZoneBitmapElement(uint8_t *ptr, size_t size)
  : BitmapElement(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::HiddenZoneBitmapElement::HiddenZoneBitmapElement(uint8_t *ptr)
  : BitmapElement(ptr, HiddenZoneBitmapElement::size())
{
  // pass...
}



/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::BootSettingsElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::BootSettingsElement::BootSettingsElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::BootSettingsElement::BootSettingsElement(uint8_t *ptr)
  : Element(ptr, BootSettingsElement::size())
{
  // pass...
}

void
AnytoneCodeplugGen1::BootSettingsElement::clear() {
  memset(_data, 0x00, _size);
}

QString
AnytoneCodeplugGen1::BootSettingsElement::introLine1() const {
  return readASCII(Offset::introLine1(), Limit::introLineLength(), 0x00);
}
void
AnytoneCodeplugGen1::BootSettingsElement::setIntroLine1(const QString &txt) {
  writeASCII(Offset::introLine1(), txt, Limit::introLineLength(), 0x00);
}
QString
AnytoneCodeplugGen1::BootSettingsElement::introLine2() const {
  return readASCII(Offset::introLine2(), Limit::introLineLength(), 0x00);
}
void
AnytoneCodeplugGen1::BootSettingsElement::setIntroLine2(const QString &txt) {
  writeASCII(Offset::introLine2(), txt, Limit::introLineLength(), 0x00);
}

QString
AnytoneCodeplugGen1::BootSettingsElement::password() const {
  return readASCII(Offset::password(), Limit::passwordLength(), 0x00);
}
void
AnytoneCodeplugGen1::BootSettingsElement::setPassword(const QString &txt) {
  QRegularExpression pattern("[0-9]{0,8}");
  if (pattern.match(txt).hasMatch())
    writeASCII(Offset::password(), txt, Limit::passwordLength(), 0x00);
}

bool
AnytoneCodeplugGen1::BootSettingsElement::fromConfig(const Flags &flags, Context &ctx) {
  Q_UNUSED(flags)
  setIntroLine1(ctx.config()->settings()->boot()->message1());
  setIntroLine2(ctx.config()->settings()->boot()->message2());

  if (ctx.config()->settings()->boot()->bootPasswordEnabled())
    setPassword(ctx.config()->settings()->boot()->bootPassword());
  else
    setPassword("");

  return true;
}

bool
AnytoneCodeplugGen1::BootSettingsElement::updateConfig(Context &ctx) {
  ctx.config()->settings()->boot()->setMessage1(introLine1());
  ctx.config()->settings()->boot()->setMessage2(introLine2());

  ctx.config()->settings()->boot()->setBootPassword(password());
  // Do NOT derive the enabled-flag from the password string here. The radio stores
  // that flag separately, in the general settings, and it is already decoded by
  // GeneralSettingsElement::updateConfig(), which runs before this element. A radio
  // may well hold a non-empty but *disabled* boot password; inferring "enabled" from
  // "password is not empty" overwrites the correct flag, and the next write then
  // enables the password and locks the user out of their radio.

  return true;
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::DMRAPRSSettingsElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::DMRAPRSSettingsElement::DMRAPRSSettingsElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::DMRAPRSSettingsElement::DMRAPRSSettingsElement(uint8_t *ptr)
  : Element(ptr, DMRAPRSSettingsElement::size())
{
  // pass...
}

void
AnytoneCodeplugGen1::DMRAPRSSettingsElement::clear() {
  memset(_data, 0x00, _size);
}

Interval
AnytoneCodeplugGen1::DMRAPRSSettingsElement::manualInterval() const {
  return Interval::fromSeconds(getUInt8(Offset::manualInterval()));
}
void
AnytoneCodeplugGen1::DMRAPRSSettingsElement::setManualInterval(const Interval &sec) {
  setUInt8(Offset::manualInterval(), sec.seconds());
}

bool
AnytoneCodeplugGen1::DMRAPRSSettingsElement::automatic() const {
  return 0!=getUInt8(Offset::automaticInterval());
}
Interval
AnytoneCodeplugGen1::DMRAPRSSettingsElement::automaticInterval() const {
  return Interval::fromSeconds(45 + 15*((unsigned)getUInt8(Offset::automaticInterval())));
}
void
AnytoneCodeplugGen1::DMRAPRSSettingsElement::setAutomaticInterval(const Interval &sec) {
  unsigned int s = sec.seconds();
  if (s < 60) s = 60;
  setUInt8(Offset::automaticInterval(), (s-45)/15);
}
void
AnytoneCodeplugGen1::DMRAPRSSettingsElement::disableAutomatic() {
  setUInt8(Offset::automaticInterval(), 0x00);
}

bool
AnytoneCodeplugGen1::DMRAPRSSettingsElement::fixedLocation() const {
  return getUInt8(Offset::fixedLocation());
}
QGeoCoordinate
AnytoneCodeplugGen1::DMRAPRSSettingsElement::location() const {
  double latitude  = getUInt8(Offset::latitudeDeg()) + double(getUInt8(Offset::latitudeMin()))/60
      + double(getUInt8(Offset::latitudeSec()))/3600;
  if (getUInt8(Offset::latitudeHemi())) latitude *= -1;
  double longitude = getUInt8(Offset::longitudeDeg()) + double(getUInt8(Offset::longitudeMin()))/60
      + double(getUInt8(Offset::longitudeSec()))/3600;
  if (getUInt8(Offset::longitudeHemi())) longitude *= -1;
  return QGeoCoordinate(latitude, longitude);
}
void
AnytoneCodeplugGen1::DMRAPRSSettingsElement::setLocation(const QGeoCoordinate &pos) {
  double latitude = pos.latitude();
  bool south = (0 > latitude); latitude = std::abs(latitude);
  unsigned lat_deg = int(latitude); latitude -= lat_deg; latitude *= 60;
  unsigned lat_min = int(latitude); latitude -= lat_min; latitude *= 60;
  unsigned lat_sec = int(latitude);
  double longitude = pos.longitude();
  bool west = (0 > longitude); longitude = std::abs(longitude);
  unsigned lon_deg = int(longitude); longitude -= lon_deg; longitude *= 60;
  unsigned lon_min = int(longitude); longitude -= lon_min; longitude *= 60;
  unsigned lon_sec = int(longitude);
  setUInt8(Offset::latitudeDeg(), lat_deg);  setUInt8(Offset::latitudeMin(), lat_min);
  setUInt8(Offset::latitudeSec(), lat_sec); setUInt8(Offset::latitudeHemi(), (south ? 0x01 : 0x00));
  setUInt8(Offset::longitudeDeg(), lon_deg); setUInt8(Offset::longitudeMin(), lon_min);
  setUInt8(Offset::longitudeSec(), lon_sec); setUInt8(Offset::longitudeHemi(), (west ? 0x01 : 0x00));
}
void
AnytoneCodeplugGen1::DMRAPRSSettingsElement::enableFixedLocation(bool enable) {
  setUInt8(Offset::fixedLocation(), (enable ? 0x01 : 0x00));
}

Channel::Power
AnytoneCodeplugGen1::DMRAPRSSettingsElement::power() const {
  switch (getUInt8(Offset::power())) {
  case 0x00: return Channel::Power::Low;
  case 0x01: return Channel::Power::Mid;
  case 0x02: return Channel::Power::High;
  case 0x03: return Channel::Power::Max;
  }
  return Channel::Power::Low;
}
void
AnytoneCodeplugGen1::DMRAPRSSettingsElement::setPower(Channel::Power power) {
  switch (power) {
  case Channel::Power::Min:
  case Channel::Power::Low:
    setUInt8(Offset::power(), 0x00);
    break;
  case Channel::Power::Mid:
    setUInt8(Offset::power(), 0x01);
    break;
  case Channel::Power::High:
    setUInt8(Offset::power(), 0x02);
    break;
  case Channel::Power::Max:
    setUInt8(Offset::power(), 0x03);
    break;
  }
}

bool
AnytoneCodeplugGen1::DMRAPRSSettingsElement::hasChannel(unsigned n) const {
  return 0xffff != channelIndex(n);
}
bool
AnytoneCodeplugGen1::DMRAPRSSettingsElement::channelIsVFOA(unsigned n) const {
  return 0x0fa0 == channelIndex(n);
}
bool
AnytoneCodeplugGen1::DMRAPRSSettingsElement::channelIsVFOB(unsigned n) const {
  return 0x0fa1 == channelIndex(n);
}
bool
AnytoneCodeplugGen1::DMRAPRSSettingsElement::channelIsSelected(unsigned n) const {
  return 0x0fa2 == channelIndex(n);
}
unsigned
AnytoneCodeplugGen1::DMRAPRSSettingsElement::channelIndex(unsigned n) const {
  return getUInt16_le(0x000c + 2*n);
}
void
AnytoneCodeplugGen1::DMRAPRSSettingsElement::setChannelIndex(unsigned n, unsigned idx) {
  setUInt16_le(0x000c + 2*n, idx);
}
void
AnytoneCodeplugGen1::DMRAPRSSettingsElement::setChannelVFOA(unsigned n) {
  setChannelIndex(n, 0x0fa0);
}
void
AnytoneCodeplugGen1::DMRAPRSSettingsElement::setChannelVFOB(unsigned n) {
  setChannelIndex(n, 0x0fa1);
}
void
AnytoneCodeplugGen1::DMRAPRSSettingsElement::setChannelSelected(unsigned n) {
  setChannelIndex(n, 0x0fa2);
}
void
AnytoneCodeplugGen1::DMRAPRSSettingsElement::clearChannel(unsigned n) {
  setChannelIndex(n, 0xffff);
}

unsigned
AnytoneCodeplugGen1::DMRAPRSSettingsElement::destination() const {
  return getBCD8_be(0x001c);
}
void
AnytoneCodeplugGen1::DMRAPRSSettingsElement::setDestination(unsigned id) {
  setBCD8_be(0x001c, id);
}

DMRContact::Type
AnytoneCodeplugGen1::DMRAPRSSettingsElement::callType() const {
  switch (getUInt8(0x0020)) {
  case 0x00: return DMRContact::PrivateCall;
  case 0x01: return DMRContact::GroupCall;
  case 0x02: return DMRContact::AllCall;
  }
  return DMRContact::PrivateCall;
}
void
AnytoneCodeplugGen1::DMRAPRSSettingsElement::setCallType(DMRContact::Type type) {
  switch (type) {
  case DMRContact::PrivateCall: setUInt8(0x0020, 0x00); break;
  case DMRContact::GroupCall: setUInt8(0x0020, 0x01); break;
  case DMRContact::AllCall: setUInt8(0x0020, 0x02); break;
  }
}

bool
AnytoneCodeplugGen1::DMRAPRSSettingsElement::timeSlotOverride() const {
  return 0 != getUInt8(0x0021);
}
DMRChannel::TimeSlot
AnytoneCodeplugGen1::DMRAPRSSettingsElement::timeslot() const {
  if (1 == getUInt8(0x0021))
    return DMRChannel::TimeSlot::TS1;
  else if (2 == getUInt8(0x0021))
    return DMRChannel::TimeSlot::TS2;
  return DMRChannel::TimeSlot::TS1;
}
void
AnytoneCodeplugGen1::DMRAPRSSettingsElement::overrideTimeSlot(DMRChannel::TimeSlot ts) {
  if (DMRChannel::TimeSlot::TS1 == ts)
    setUInt8(0x0021, 0x01);
  else
    setUInt8(0x0021, 0x02);
}
void
AnytoneCodeplugGen1::DMRAPRSSettingsElement::disableTimeSlotOverride() {
  setUInt8(0x0021, 0x00);
}

bool
AnytoneCodeplugGen1::DMRAPRSSettingsElement::fromConfig(const Flags &flags, Context &ctx) {
  Q_UNUSED(flags)

  // Encode fixed location if valid and enabled
  if (ctx.config()->settings()->gnss()->fixedPosition().isValid()) {
    setLocation(ctx.config()->settings()->gnss()->fixedPosition());
    // Enable if there are no GNSS enabled
    enableFixedLocation(ctx.config()->settings()->gnss()->fixedPositionEnabled());
  }

  if (1 < ctx.count<DMRAPRSSystem>()) {
    logDebug() << "D868UV only supports a single independent GPS positioning system.";
  } else if (0 == ctx.count<DMRAPRSSystem>()) {
    return true;
  }

  DMRAPRSSystem *sys = ctx.get<DMRAPRSSystem>(0);
  setDestination(sys->contact()->number());
  setCallType(sys->contact()->type());
  setManualInterval(sys->period());
  setAutomaticInterval(sys->period());
  disableTimeSlotOverride();
  if (! sys->hasRevertChannel()) {
    setChannelSelected(0);
  } else if (sys->revertChannelRef()->is<DMRChannel>()) {
    setChannelIndex(0, ctx.index(sys->revertChannel()));
  } else {
    clearChannel(0);
  }
  return true;
}


bool
AnytoneCodeplugGen1::DMRAPRSSettingsElement::updateConfig(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err);

  if (location().isValid()) {
    ctx.config()->settings()->gnss()->setFixedPosition(location());
    ctx.config()->settings()->gnss()->enableFixedPosition(fixedLocation());
  }

  return true;
}


bool
AnytoneCodeplugGen1::DMRAPRSSettingsElement::createGPSSystem(uint8_t i, Context &ctx) {
  DMRAPRSSystem *sys = new DMRAPRSSystem(QString("GPS sys %1").arg(i+1), nullptr, nullptr, automaticInterval());
  ctx.config()->posSystems()->add(sys); ctx.add(sys, i);
  return true;
}

bool
AnytoneCodeplugGen1::DMRAPRSSettingsElement::linkGPSSystem(uint8_t i, Context &ctx) {
  DMRContact *cont = nullptr;
  // Find matching contact, if not found -> create one.
  for (unsigned int i=0; i<ctx.count<DigitalContact>(); i++) {
    if (ctx.get<DMRContact>(i)->number() == destination()) {
      cont = ctx.get<DMRContact>(i);
      break;
    }
  }
  if (nullptr == cont) {
    cont = new DMRContact(callType(), QString("GPS target"), destination());
    ctx.config()->contacts()->add(cont);
  }
  ctx.get<DMRAPRSSystem>(i)->setContact(cont);

  // Check if there is a revert channel set
  if ((! channelIsSelected(i)) && (ctx.has<Channel>(channelIndex(i))) && (ctx.get<Channel>(channelIndex(i)))->is<DMRChannel>()) {
    DMRChannel *ch = ctx.get<Channel>(channelIndex(i))->as<DMRChannel>();
    ctx.get<DMRAPRSSystem>(i)->setRevertChannel(ch);
  } else {
    ctx.get<DMRAPRSSystem>(i)->resetRevertChannel();
  }
  return true;
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::DMRAPRSMessageElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::DMRAPRSMessageElement::DMRAPRSMessageElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::DMRAPRSMessageElement::DMRAPRSMessageElement(uint8_t *ptr)
  : Element(ptr, DMRAPRSMessageElement::size())
{
  // pass...
}

void
AnytoneCodeplugGen1::DMRAPRSMessageElement::clear() {
  memset(_data, 0x00, _size);
}

QString
AnytoneCodeplugGen1::DMRAPRSMessageElement::message() const {
  return readASCII(Offset::message(), Limit::length(), 0x00);
}

void
AnytoneCodeplugGen1::DMRAPRSMessageElement::setMessage(const QString &message) {
  writeASCII(Offset::message(), message, Limit::length(), 0x00);
}

bool
AnytoneCodeplugGen1::DMRAPRSMessageElement::fromConfig(Codeplug::Flags flags, Context &ctx) {
  Q_UNUSED(flags); Q_UNUSED(ctx)
  return true;
}

bool
AnytoneCodeplugGen1::DMRAPRSMessageElement::updateConfig(Context &ctx) const {
  Q_UNUSED(ctx)
  return true;
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::RepeaterOffsetListElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::RepeaterOffsetListElement::RepeaterOffsetListElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::RepeaterOffsetListElement::RepeaterOffsetListElement(uint8_t *ptr)
  : Element(ptr, RepeaterOffsetListElement::size())
{
  // pass...
}

void
AnytoneCodeplugGen1::RepeaterOffsetListElement::clear() {
  memset(_data, 0x00, _size);
  for (unsigned int i=0; i<Limit::numEntries(); i++)
    clearOffset(i);
}

bool
AnytoneCodeplugGen1::RepeaterOffsetListElement::isSet(unsigned int n) const {
  if (n >= Limit::numEntries())
    return false;
  return 0 != getUInt32_le(Offset::frequencies() + n * Offset::betweenFrequencies());
}

Frequency
AnytoneCodeplugGen1::RepeaterOffsetListElement::offset(unsigned int n) const {
  if (n >= Limit::numEntries())
    return Frequency::fromHz(0);

  return Frequency::fromHz(
        ((unsigned long long)getUInt32_le(Offset::frequencies()
                                          + n * Offset::betweenFrequencies()))*10);
}

void
AnytoneCodeplugGen1::RepeaterOffsetListElement::setOffset(unsigned int n, Frequency freq) {
  if (n >= Limit::numEntries())
    return;

  setUInt32_le(Offset::frequencies() + n*Offset::betweenFrequencies(), freq.inHz()/10);
}

void
AnytoneCodeplugGen1::RepeaterOffsetListElement::clearOffset(unsigned int n) {
  if (n >= Limit::numEntries())
    return;
  setUInt32_le(Offset::frequencies() + n*Offset::betweenFrequencies(), 0);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::MessageListElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::MessageListElement::MessageListElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::MessageListElement::MessageListElement(uint8_t *ptr)
  : Element(ptr, MessageListElement::size())
{
  // pass...
}

void
AnytoneCodeplugGen1::MessageListElement::clear() {
  memset(_data, 0x00, _size);
  clearNext();
  clearIndex();
}

bool
AnytoneCodeplugGen1::MessageListElement::hasNext() const {
  return 0xff != next();
}
unsigned
AnytoneCodeplugGen1::MessageListElement::next() const {
  return getUInt8(Offset::next());
}
void
AnytoneCodeplugGen1::MessageListElement::setNext(unsigned idx) {
  setUInt8(Offset::next(), idx);
}
void
AnytoneCodeplugGen1::MessageListElement::clearNext() {
  setNext(0xff);
}

bool
AnytoneCodeplugGen1::MessageListElement::hasIndex() const {
  return 0xff != index();
}
unsigned
AnytoneCodeplugGen1::MessageListElement::index() const {
  return getUInt8(Offset::index());
}
void
AnytoneCodeplugGen1::MessageListElement::setIndex(unsigned idx) {
  setUInt8(Offset::index(), idx);
}
void
AnytoneCodeplugGen1::MessageListElement::clearIndex() {
  setIndex(0xff);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::MessageElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::MessageElement::MessageElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::MessageElement::MessageElement(uint8_t *ptr)
  : Element(ptr, MessageElement::size())
{
  // pass...
}

void
AnytoneCodeplugGen1::MessageElement::clear() {
  memset(_data, 0x00, _size);
}

QString
AnytoneCodeplugGen1::MessageElement::message() const {
  return readASCII(Offset::message(), Limit::messageLength(), 0x00);
}
void
AnytoneCodeplugGen1::MessageElement::setMessage(const QString &msg) {
  writeASCII(Offset::message(), msg, Limit::messageLength(), 0x00);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::MessageBytemapElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::MessageBytemapElement::MessageBytemapElement(uint8_t *ptr, size_t size)
  : InvertedBytemapElement(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::MessageBytemapElement::MessageBytemapElement(uint8_t *ptr)
  : InvertedBytemapElement(ptr, MessageBytemapElement::size())
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::AnalogQuickCallElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::AnalogQuickCallElement::AnalogQuickCallElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::AnalogQuickCallElement::AnalogQuickCallElement(uint8_t *ptr)
  : Element(ptr, AnalogQuickCallElement::size())
{
  // pass...
}

void
AnytoneCodeplugGen1::AnalogQuickCallElement::clear() {
  memset(_data, 0x00, _size);
  clearContactIndex();
}

AnytoneCodeplugGen1::AnalogQuickCallElement::Type
AnytoneCodeplugGen1::AnalogQuickCallElement::type() const {
  return (Type)getUInt8(Offset::type());
}
void
AnytoneCodeplugGen1::AnalogQuickCallElement::setType(Type type) {
  setUInt8(Offset::type(), (unsigned)type);
}

bool
AnytoneCodeplugGen1::AnalogQuickCallElement::hasContactIndex() const {
  return 0xff != contactIndex();
}
unsigned
AnytoneCodeplugGen1::AnalogQuickCallElement::contactIndex() const {
  return getUInt8(Offset::contactIndex());
}
void
AnytoneCodeplugGen1::AnalogQuickCallElement::setContactIndex(unsigned idx) {
  setUInt8(Offset::contactIndex(), idx);
}
void
AnytoneCodeplugGen1::AnalogQuickCallElement::clearContactIndex() {
  setContactIndex(0xff);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::AnalogQuickCallsElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::AnalogQuickCallsElement::AnalogQuickCallsElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::AnalogQuickCallsElement::AnalogQuickCallsElement(uint8_t *ptr)
  : Element(ptr, AnalogQuickCallsElement::size())
{
  // pass...
}

void
AnytoneCodeplugGen1::AnalogQuickCallsElement::clear() {
  memset(_data, 0x00, _size);
  for (unsigned int i=0; i<Limit::numEntries(); i++)
    AnalogQuickCallElement(quickCall(i)).clear();
}

uint8_t *
AnytoneCodeplugGen1::AnalogQuickCallsElement::quickCall(unsigned int n) const {
  n = std::min(Limit::numEntries(), n);
  return _data + n*AnalogQuickCallElement::size();
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::StatusMessagesElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::StatusMessagesElement::StatusMessagesElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::StatusMessagesElement::StatusMessagesElement(uint8_t *ptr)
  : Element(ptr, StatusMessagesElement::size())
{
  // pass...
}

void
AnytoneCodeplugGen1::StatusMessagesElement::clear() {
  memset(_data, 0x00, _size);
}

QString
AnytoneCodeplugGen1::StatusMessagesElement::message(unsigned int n) const {
  n = std::min(Limit::numMessages(), n);
  return readASCII(Offset::messages()+n*Offset::betweenMessages(), Limit::messageLength(), 0x00);
}

void
AnytoneCodeplugGen1::StatusMessagesElement::setMessage(unsigned int n, const QString &msg) {
  if (n >= Limit::numMessages())
    return;
  writeASCII(Offset::messages()+n*Offset::betweenMessages(), msg, Limit::messageLength(), 0x00);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::StatusMessageBitmapElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::StatusMessageBitmapElement::StatusMessageBitmapElement(uint8_t *ptr, size_t size)
  : BitmapElement(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::StatusMessageBitmapElement::StatusMessageBitmapElement(uint8_t *ptr)
  : BitmapElement(ptr, StatusMessageBitmapElement::size())
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::HotKeyElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::HotKeyElement::HotKeyElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::HotKeyElement::HotKeyElement(uint8_t *ptr)
  : Element(ptr, HotKeyElement::size())
{
  // pass...
}

void
AnytoneCodeplugGen1::HotKeyElement::clear() {
  memset(_data, 0x00, _size);
  clearContactIndex();
  clearMessageIndex();
}

AnytoneCodeplugGen1::HotKeyElement::Type
AnytoneCodeplugGen1::HotKeyElement::type() const {
  return (Type)getUInt8(Offset::type());
}
void
AnytoneCodeplugGen1::HotKeyElement::setType(Type type) {
  setUInt8(Offset::type(), (unsigned)type);
}

AnytoneCodeplugGen1::HotKeyElement::MenuItem
AnytoneCodeplugGen1::HotKeyElement::menuItem() const {
  return (MenuItem) getUInt8(Offset::menuItem());
}
void
AnytoneCodeplugGen1::HotKeyElement::setMenuItem(MenuItem item) {
  setUInt8(Offset::menuItem(), (unsigned)item);
}

AnytoneCodeplugGen1::HotKeyElement::CallType
AnytoneCodeplugGen1::HotKeyElement::callType() const {
  return (CallType)getUInt8(Offset::callType());
}
void
AnytoneCodeplugGen1::HotKeyElement::setCallType(CallType type) {
  setUInt8(Offset::callType(), (unsigned)type);
}

AnytoneCodeplugGen1::HotKeyElement::DigiCallType
AnytoneCodeplugGen1::HotKeyElement::digiCallType() const {
  return (DigiCallType)getUInt8(Offset::digiCallType());
}
void
AnytoneCodeplugGen1::HotKeyElement::setDigiCallType(DigiCallType type) {
  setUInt8(Offset::digiCallType(), (unsigned)type);
}

bool
AnytoneCodeplugGen1::HotKeyElement::hasContactIndex() const {
  return 0xffffffff != contactIndex();
}
unsigned
AnytoneCodeplugGen1::HotKeyElement::contactIndex() const {
  return getUInt32_le(Offset::contactIndex());
}
void
AnytoneCodeplugGen1::HotKeyElement::setContactIndex(unsigned idx) {
  setUInt32_le(Offset::contactIndex(), idx);
}
void
AnytoneCodeplugGen1::HotKeyElement::clearContactIndex() {
  setContactIndex(0xffffffff);
}

bool
AnytoneCodeplugGen1::HotKeyElement::hasMessageIndex() const {
  return 0xff != messageIndex();
}
unsigned
AnytoneCodeplugGen1::HotKeyElement::messageIndex() const {
  return getUInt8(Offset::messageIndex());
}
void
AnytoneCodeplugGen1::HotKeyElement::setMessageIndex(unsigned idx) {
  setUInt8(Offset::messageIndex(), idx);
}
void
AnytoneCodeplugGen1::HotKeyElement::clearMessageIndex() {
  setMessageIndex(0xff);
}



/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::HotKeySettingsElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::HotKeySettingsElement::HotKeySettingsElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::HotKeySettingsElement::HotKeySettingsElement(uint8_t *ptr)
  : Element(ptr, HotKeySettingsElement::size())
{
  // pass...
}

void
AnytoneCodeplugGen1::HotKeySettingsElement::clear() {
  memset(_data, 0x00, _size);
  for (unsigned int i=0; i<Limit::numEntries(); i++)
    HotKeyElement(hotKeySetting(i)).clear();
}

uint8_t *
AnytoneCodeplugGen1::HotKeySettingsElement::hotKeySetting(unsigned int n) const {
  n = std::min(Limit::numEntries(), n);
  return _data + Offset::hotKeySettings() + n*Offset::betweenHotKeySettings();
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::AlarmSettingElement::AnalogAlarm
 * ********************************************************************************************* */
AnytoneCodeplugGen1::AlarmSettingElement::AnalogAlarm::AnalogAlarm(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::AlarmSettingElement::AnalogAlarm::AnalogAlarm(uint8_t *ptr)
  : Element(ptr, AlarmSettingElement::AnalogAlarm::size())
{
  // pass...
}

void
AnytoneCodeplugGen1::AlarmSettingElement::AnalogAlarm::clear() {
  memset(_data, 0x00, _size);
  setChannelSelected();
}

AnytoneCodeplugGen1::AlarmSettingElement::AnalogAlarm::Action
AnytoneCodeplugGen1::AlarmSettingElement::AnalogAlarm::action() const {
  return (Action) getUInt8(Offset::action());
}
void
AnytoneCodeplugGen1::AlarmSettingElement::AnalogAlarm::setAction(Action action) {
  setUInt8(Offset::action(), (unsigned)action);
}

AnytoneCodeplugGen1::AlarmSettingElement::AnalogAlarm::ENIType
AnytoneCodeplugGen1::AlarmSettingElement::AnalogAlarm::encodingType() const {
  return (ENIType) getUInt8(Offset::encodingType());
}
void
AnytoneCodeplugGen1::AlarmSettingElement::AnalogAlarm::setEncodingType(ENIType type) {
  setUInt8(Offset::encodingType(), (unsigned)type);
}

unsigned
AnytoneCodeplugGen1::AlarmSettingElement::AnalogAlarm::emergencyIndex() const {
  return getUInt8(Offset::emergencyIndex());
}
void
AnytoneCodeplugGen1::AlarmSettingElement::AnalogAlarm::setEmergencyIndex(unsigned idx) {
  setUInt8(Offset::emergencyIndex(), idx);
}

Interval
AnytoneCodeplugGen1::AlarmSettingElement::AnalogAlarm::duration() const {
  return Interval::fromSeconds(getUInt8(0x0003));
}
void
AnytoneCodeplugGen1::AlarmSettingElement::AnalogAlarm::setDuration(const Interval &sec) {
  setUInt8(Offset::duration(), sec.seconds());
}

Interval
AnytoneCodeplugGen1::AlarmSettingElement::AnalogAlarm::txDuration() const {
  return Interval::fromSeconds(getUInt8(Offset::txDuration()));
}
void
AnytoneCodeplugGen1::AlarmSettingElement::AnalogAlarm::setTXDuration(const Interval &sec) {
  setUInt8(Offset::txDuration(), sec.seconds());
}

Interval
AnytoneCodeplugGen1::AlarmSettingElement::AnalogAlarm::rxDuration() const {
  return Interval::fromSeconds(getUInt8(Offset::rxDuration()));
}
void
AnytoneCodeplugGen1::AlarmSettingElement::AnalogAlarm::setRXDuration(const Interval &sec) {
  setUInt8(Offset::rxDuration(), sec.seconds());
}

bool
AnytoneCodeplugGen1::AlarmSettingElement::AnalogAlarm::channelIsSelected() const {
  return getUInt8(Offset::channelIsSelected());
}
unsigned
AnytoneCodeplugGen1::AlarmSettingElement::AnalogAlarm::channelIndex() const {
  return getUInt16_le(Offset::channelIndex());
}
void
AnytoneCodeplugGen1::AlarmSettingElement::AnalogAlarm::setChannelIndex(unsigned idx) {
  setUInt16_le(Offset::channelIndex(), idx);
  setUInt8(Offset::channelIsSelected(), 0x00);
}
void
AnytoneCodeplugGen1::AlarmSettingElement::AnalogAlarm::setChannelSelected() {
  setUInt8(Offset::channelIsSelected(), 0x01);
}

bool
AnytoneCodeplugGen1::AlarmSettingElement::AnalogAlarm::repeatContinuously() const {
  return 0 == repetitions();
}
unsigned
AnytoneCodeplugGen1::AlarmSettingElement::AnalogAlarm::repetitions() const {
  return getUInt8(Offset::repetitions());
}
void
AnytoneCodeplugGen1::AlarmSettingElement::AnalogAlarm::setRepetitions(unsigned num) {
  setUInt8(Offset::repetitions(), num);
}
void
AnytoneCodeplugGen1::AlarmSettingElement::AnalogAlarm::setRepatContinuously() {
  setRepetitions(0x00);
}



/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::AlarmSettingElement::DigitalAlarm
 * ********************************************************************************************* */
AnytoneCodeplugGen1::AlarmSettingElement::DigitalAlarm::DigitalAlarm(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::AlarmSettingElement::DigitalAlarm::DigitalAlarm(uint8_t *ptr)
  : Element(ptr, AlarmSettingElement::DigitalAlarm::size())
{
  // pass...
}

void
AnytoneCodeplugGen1::AlarmSettingElement::DigitalAlarm::clear() {
  memset(_data, 0x00, _size);
  setChannelSelected();
}

AnytoneCodeplugGen1::AlarmSettingElement::DigitalAlarm::Action
AnytoneCodeplugGen1::AlarmSettingElement::DigitalAlarm::action() const {
  return (Action) getUInt8(Offset::action());
}
void
AnytoneCodeplugGen1::AlarmSettingElement::DigitalAlarm::setAction(Action action) {
  setUInt8(Offset::action(), (unsigned)action);
}

Interval
AnytoneCodeplugGen1::AlarmSettingElement::DigitalAlarm::duration() const {
  return Interval::fromSeconds(getUInt8(Offset::duration()));
}
void
AnytoneCodeplugGen1::AlarmSettingElement::DigitalAlarm::setDuration(const Interval &sec) {
  setUInt8(Offset::duration(), sec.seconds());
}

Interval
AnytoneCodeplugGen1::AlarmSettingElement::DigitalAlarm::txDuration() const {
  return Interval::fromSeconds(getUInt8(Offset::txDuration()));
}
void
AnytoneCodeplugGen1::AlarmSettingElement::DigitalAlarm::setTXDuration(const Interval &sec) {
  setUInt8(Offset::txDuration(), sec.seconds());
}

Interval
AnytoneCodeplugGen1::AlarmSettingElement::DigitalAlarm::rxDuration() const {
  return Interval::fromSeconds(getUInt8(Offset::rxDuration()));
}
void
AnytoneCodeplugGen1::AlarmSettingElement::DigitalAlarm::setRXDuration(const Interval &sec) {
  setUInt8(Offset::rxDuration(), sec.seconds());
}

bool
AnytoneCodeplugGen1::AlarmSettingElement::DigitalAlarm::channelIsSelected() const {
  return getUInt8(Offset::channelIsSelected());
}
unsigned
AnytoneCodeplugGen1::AlarmSettingElement::DigitalAlarm::channelIndex() const {
  return getUInt16_le(Offset::channelIndex());
}
void
AnytoneCodeplugGen1::AlarmSettingElement::DigitalAlarm::setChannelIndex(unsigned idx) {
  setUInt16_le(Offset::channelIndex(), idx);
  setUInt8(Offset::channelIsSelected(), 0x00);
}
void
AnytoneCodeplugGen1::AlarmSettingElement::DigitalAlarm::setChannelSelected() {
  setUInt8(Offset::channelIsSelected(), 0x01);
}

bool
AnytoneCodeplugGen1::AlarmSettingElement::DigitalAlarm::repeatContinuously() const {
  return 0 == repetitions();
}
unsigned
AnytoneCodeplugGen1::AlarmSettingElement::DigitalAlarm::repetitions() const {
  return getUInt8(Offset::repetitions());
}
void
AnytoneCodeplugGen1::AlarmSettingElement::DigitalAlarm::setRepetitions(unsigned num) {
  setUInt8(Offset::repetitions(), num);
}
void
AnytoneCodeplugGen1::AlarmSettingElement::DigitalAlarm::setRepatContinuously() {
  setRepetitions(0x00);
}

Interval
AnytoneCodeplugGen1::AlarmSettingElement::DigitalAlarm::voiceBroadcastDuration() const {
  return Interval::fromMinutes(getUInt8(Offset::voiceBroadcastDuration())+1);
}
void
AnytoneCodeplugGen1::AlarmSettingElement::DigitalAlarm::setVoiceBroadcastDuration(const Interval &min) {
  unsigned int m = min.minutes();
  if (1 > m) m = 1;
  setUInt8(Offset::voiceBroadcastDuration(), m-1);
}

Interval
AnytoneCodeplugGen1::AlarmSettingElement::DigitalAlarm::areaBroadcastDuration() const {
  return Interval::fromMinutes(getUInt8(Offset::areaBroadcastDuration())+1);
}
void
AnytoneCodeplugGen1::AlarmSettingElement::DigitalAlarm::setAreaBroadcastDuration(const Interval &min) {
  unsigned int m = min.minutes();
  if (1 > m) m = 1;
  setUInt8(Offset::areaBroadcastDuration(), m-1);
}

bool
AnytoneCodeplugGen1::AlarmSettingElement::DigitalAlarm::vox() const {
  return getUInt8(Offset::vox());
}
void
AnytoneCodeplugGen1::AlarmSettingElement::DigitalAlarm::enableVOX(bool enable) {
  setUInt8(Offset::vox(), (enable ? 0x01 : 0x00));
}

bool
AnytoneCodeplugGen1::AlarmSettingElement::DigitalAlarm::rxAlarm() const {
  return getUInt8(Offset::rxAlarm());
}
void
AnytoneCodeplugGen1::AlarmSettingElement::DigitalAlarm::enableRXAlarm(bool enable) {
  setUInt8(Offset::rxAlarm(), (enable ? 0x01 : 0x00));
}



/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::AlarmSettingElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::AlarmSettingElement::AlarmSettingElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::AlarmSettingElement::AlarmSettingElement(uint8_t *ptr)
  : Element(ptr, AlarmSettingElement::size())
{
  // pass...
}

void
AnytoneCodeplugGen1::AlarmSettingElement::clear() {
  AnalogAlarm(analog()).clear();
  DigitalAlarm(digital()).clear();
}

uint8_t *
AnytoneCodeplugGen1::AlarmSettingElement::analog() const {
  return _data + Offset::analog();
}
uint8_t *
AnytoneCodeplugGen1::AlarmSettingElement::digital() const {
  return _data + Offset::digital();
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::DigitalAlarmExtensionElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::DigitalAlarmExtensionElement::DigitalAlarmExtensionElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::DigitalAlarmExtensionElement::DigitalAlarmExtensionElement(uint8_t *ptr)
  : Element(ptr, DigitalAlarmExtensionElement::size())
{
  // pass...
}

void
AnytoneCodeplugGen1::DigitalAlarmExtensionElement::clear() {
  memset(_data, 0x00, _size);
}

DMRContact::Type
AnytoneCodeplugGen1::DigitalAlarmExtensionElement::callType() const {
  switch (getUInt8(Offset::callType())) {
  case 0x00: return DMRContact::PrivateCall;
  case 0x01: return DMRContact::GroupCall;
  case 0x02: return DMRContact::AllCall;
  }
  return DMRContact::PrivateCall;
}
void
AnytoneCodeplugGen1::DigitalAlarmExtensionElement::setCallType(DMRContact::Type type) {
  switch (type) {
  case DMRContact::PrivateCall: setUInt8(Offset::callType(), 0x00); break;
  case DMRContact::GroupCall: setUInt8(Offset::callType(), 0x01); break;
  case DMRContact::AllCall: setUInt8(Offset::callType(), 0x02); break;
  }
}

unsigned
AnytoneCodeplugGen1::DigitalAlarmExtensionElement::destination() const {
  return getBCD8_be(Offset::destination());
}
void
AnytoneCodeplugGen1::DigitalAlarmExtensionElement::setDestination(unsigned number) {
  setBCD8_be(Offset::destination(), number);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::FiveToneIDElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::FiveToneIDElement::FiveToneIDElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::FiveToneIDElement::FiveToneIDElement(uint8_t *ptr)
  : Element(ptr, FiveToneIDElement::size())
{
  // pass...
}

void
AnytoneCodeplugGen1::FiveToneIDElement::clear() {
  memset(_data, 0x00, _size);
}

AnytoneCodeplugGen1::FiveToneIDElement::Standard
AnytoneCodeplugGen1::FiveToneIDElement::standard() const {
  return (Standard) getUInt8(Offset::standard());
}
void
AnytoneCodeplugGen1::FiveToneIDElement::setStandard(Standard std) {
  setUInt8(Offset::standard(), (unsigned)std);
}

Interval
AnytoneCodeplugGen1::FiveToneIDElement::toneDuration() const {
  return Interval::fromMilliseconds(getUInt8(Offset::toneDuration()));
}
void
AnytoneCodeplugGen1::FiveToneIDElement::setToneDuration(const Interval &ms) {
  setUInt8(Offset::toneDuration(), ms.milliseconds());
}

QString
AnytoneCodeplugGen1::FiveToneIDElement::id() const {
  unsigned len = getUInt8(Offset::idLength());
  QString id;
  for (unsigned i=0; i<len; i++) {
    uint8_t b = getUInt8(Offset::id()+(i/2));
    if (0 == (i%2))
      id.append(QString::number((b>>4)&0xf,16));
    else
      id.append(QString::number((b>>0)&0xf,16));
  }
  return id;
}
void
AnytoneCodeplugGen1::FiveToneIDElement::setID(const QString &id) {
  unsigned len = 0;
  for (int i=0; i<id.length(); i++) {
    bool ok;
    if (0 == (len%2)) {
      setUInt4(Offset::id()+len/2, 4, id.mid(i, 1).toUInt(&ok, 16));
    } else {
      setUInt4(Offset::id()+len/2, 0, id.mid(i, 1).toUInt(&ok, 16));
    }
    len++;
  }
  setUInt8(Offset::idLength(), len);
}

QString
AnytoneCodeplugGen1::FiveToneIDElement::name() const {
  return readASCII(Offset::name(), Limit::nameLength(), 0x00);
}
void
AnytoneCodeplugGen1::FiveToneIDElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::nameLength(), 0x00);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::FiveToneIDBitmapElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::FiveToneIDBitmapElement::FiveToneIDBitmapElement(uint8_t *ptr, size_t size)
  : BitmapElement(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::FiveToneIDBitmapElement::FiveToneIDBitmapElement(uint8_t *ptr)
  : BitmapElement(ptr, FiveToneIDBitmapElement::size())
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::FiveToneIDListElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::FiveToneIDListElement::FiveToneIDListElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::FiveToneIDListElement::FiveToneIDListElement(uint8_t *ptr)
  : Element(ptr, FiveToneIDListElement::size())
{
  // pass...
}

void
AnytoneCodeplugGen1::FiveToneIDListElement::clear() {
  memset(_data, 0, _size);
}

uint8_t *
AnytoneCodeplugGen1::FiveToneIDListElement::member(unsigned int n) const {
  if (n >= Limit::numEntries())
    return nullptr;
  return _data + n*FiveToneIDElement::size();
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::FiveToneFunctionElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::FiveToneFunctionElement::FiveToneFunctionElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::FiveToneFunctionElement::FiveToneFunctionElement(uint8_t *ptr)
  : Element(ptr, FiveToneFunctionElement::size())
{
  // pass...
}

void
AnytoneCodeplugGen1::FiveToneFunctionElement::clear() {
  memset(_data, 0x00, _size);
}

AnytoneCodeplugGen1::FiveToneFunctionElement::Function
AnytoneCodeplugGen1::FiveToneFunctionElement::function() const {
  return (Function) getUInt8(Offset::function());
}
void
AnytoneCodeplugGen1::FiveToneFunctionElement::setFunction(Function function) {
  setUInt8(Offset::function(), (unsigned)function);
}

AnytoneCodeplugGen1::FiveToneFunctionElement::Response
AnytoneCodeplugGen1::FiveToneFunctionElement::response() const {
  return (Response) getUInt8(Offset::response());
}
void
AnytoneCodeplugGen1::FiveToneFunctionElement::setResponse(Response response) {
  setUInt8(Offset::response(), (unsigned)response);
}

QString
AnytoneCodeplugGen1::FiveToneFunctionElement::id() const {
  unsigned len = getUInt8(Offset::idLength());
  QString id;
  for (unsigned i=0; i<len; i++) {
    uint8_t b = getUInt8(Offset::id()+(i/2));
    if (0 == (i%2))
      id.append(QString::number((b>>4)&0xf, 16));
    else
      id.append(QString::number((b>>0)&0xf, 16));
  }
  return id;
}
void
AnytoneCodeplugGen1::FiveToneFunctionElement::setID(const QString &id) {
  unsigned len = 0;
  for (int i=0; i<id.length(); i++) {
    bool ok;
    if (0 == (len%2)) {
      setUInt4(Offset::id()+len/2, 4, id.mid(i, 1).toUInt(&ok, 16));
    } else {
      setUInt4(Offset::id()+len/2, 0, id.mid(i, 1).toUInt(&ok, 16));
    }
    len++;
  }
  setUInt8(Offset::idLength(), len);
}

QString
AnytoneCodeplugGen1::FiveToneFunctionElement::name() const {
  return readASCII(Offset::name(), Limit::nameLength(), 0x00);
}
void
AnytoneCodeplugGen1::FiveToneFunctionElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::nameLength(), 0x00);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::FiveToneFunctionListElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::FiveToneFunctionListElement::FiveToneFunctionListElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::FiveToneFunctionListElement::FiveToneFunctionListElement(uint8_t *ptr)
  : Element(ptr, FiveToneFunctionListElement::size())
{
  // pass...
}

void
AnytoneCodeplugGen1::FiveToneFunctionListElement::clear() {
  memset(_data, 0, _size);
}

uint8_t *
AnytoneCodeplugGen1::FiveToneFunctionListElement::function(unsigned int n) const {
  if (n >= Limit::numFunctions())
    return nullptr;
  return _data + n*FiveToneFunctionElement::size();
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::FiveToneSettingsElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::FiveToneSettingsElement::FiveToneSettingsElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::FiveToneSettingsElement::FiveToneSettingsElement(uint8_t *ptr)
  : Element(ptr, FiveToneSettingsElement::size())
{
  // pass...
}

void
AnytoneCodeplugGen1::FiveToneSettingsElement::clear() {
  memset(_data, 0x00, _size);
}

AnytoneCodeplugGen1::FiveToneSettingsElement::Response
AnytoneCodeplugGen1::FiveToneSettingsElement::decodingResponse() const {
  return (Response) getUInt8(Offset::decodingResponse());
}
void
AnytoneCodeplugGen1::FiveToneSettingsElement::setDecodingResponse(Response response) {
  setUInt8(Offset::decodingResponse(), (unsigned)response);
}

AnytoneCodeplugGen1::FiveToneSettingsElement::Standard
AnytoneCodeplugGen1::FiveToneSettingsElement::decodingStandard() const {
  return (Standard) getUInt8(Offset::decodingStandard());
}
void
AnytoneCodeplugGen1::FiveToneSettingsElement::setDecodingStandard(Standard standard) {
  setUInt8(Offset::decodingStandard(), (unsigned)standard);
}

Interval
AnytoneCodeplugGen1::FiveToneSettingsElement::decodingToneDuration() const {
  return Interval::fromMilliseconds(getUInt8(Offset::decodingToneDuration()));
}
void
AnytoneCodeplugGen1::FiveToneSettingsElement::setDecodingToneDuration(const Interval &ms) {
  setUInt8(Offset::decodingToneDuration(), ms.milliseconds());
}

QString
AnytoneCodeplugGen1::FiveToneSettingsElement::id() const {
  unsigned len = getUInt8(Offset::idLength());
  QString id;
  for (unsigned i=0; i<len; i++) {
    uint8_t b = getUInt8(Offset::id()+(i/2));
    if (0 == (i%2))
      id.append(QString::number((b>>4)&0xf, 16));
    else
      id.append(QString::number((b>>0)&0xf, 16));
  }
  return id;
}
void
AnytoneCodeplugGen1::FiveToneSettingsElement::setID(const QString &id) {
  unsigned len = 0;
  for (int i=0; i<id.length(); i++) {
    bool ok;
    if (0 == (len%2)) {
      setUInt4(Offset::id()+len/2, 4, id.mid(i, 1).toUInt(&ok, 16));
    } else {
      setUInt4(Offset::id()+len/2, 0, id.mid(i, 1).toUInt(&ok, 16));
    }
    len++;
  }
  setUInt8(Offset::idLength(), len);
}

Interval
AnytoneCodeplugGen1::FiveToneSettingsElement::postEncodeDelay() const {
  return Interval::fromMilliseconds((unsigned) getUInt8(Offset::postDecodeDelay()) * 10);
}
void
AnytoneCodeplugGen1::FiveToneSettingsElement::setPostEncodeDelay(const Interval &ms) {
  setUInt8(Offset::postDecodeDelay(), ms.milliseconds()/10);
}

bool
AnytoneCodeplugGen1::FiveToneSettingsElement::hasPTTID() const {
  return 0 != pttID();
}
unsigned
AnytoneCodeplugGen1::FiveToneSettingsElement::pttID() const {
  return getUInt8(Offset::pttId());
}
void
AnytoneCodeplugGen1::FiveToneSettingsElement::setPTTID(unsigned id) {
  setUInt8(Offset::pttId(), id);
}
void
AnytoneCodeplugGen1::FiveToneSettingsElement::clearPTTID() {
  setPTTID(0);
}

Interval
AnytoneCodeplugGen1::FiveToneSettingsElement::autoResetTime() const {
  return Interval::fromSeconds((unsigned)getUInt8(Offset::autoResetTime())*10);
}
void
AnytoneCodeplugGen1::FiveToneSettingsElement::setAutoResetTime(const Interval &s) {
  setUInt8(Offset::autoResetTime(), s.seconds()/10);
}

Interval
AnytoneCodeplugGen1::FiveToneSettingsElement::firstDelay() const {
  return Interval::fromSeconds((unsigned)getUInt8(Offset::firstDelay())*10);
}
void
AnytoneCodeplugGen1::FiveToneSettingsElement::setFirstDelay(const Interval &ms) {
  setUInt8(Offset::firstDelay(), ms.seconds()/10);
}

bool
AnytoneCodeplugGen1::FiveToneSettingsElement::sidetoneEnabled() const {
  return getUInt8(Offset::sidetoneEnabled());
}
void
AnytoneCodeplugGen1::FiveToneSettingsElement::enableSidetone(bool enable) {
  setUInt8(Offset::sidetoneEnabled(), (enable ? 0x01 : 0x00));
}

unsigned
AnytoneCodeplugGen1::FiveToneSettingsElement::stopCode() const {
  return getUInt8(Offset::stopCode());
}
void
AnytoneCodeplugGen1::FiveToneSettingsElement::setStopCode(unsigned code) {
  setUInt8(Offset::stopCode(), code);
}

Interval
AnytoneCodeplugGen1::FiveToneSettingsElement::stopTime() const {
  return Interval::fromMilliseconds((unsigned)getUInt8(Offset::stopTime())*10);
}
void
AnytoneCodeplugGen1::FiveToneSettingsElement::setStopTime(const Interval &ms) {
  setUInt8(Offset::stopTime(), ms.milliseconds()/10);
}

Interval
AnytoneCodeplugGen1::FiveToneSettingsElement::decodeTime() const {
  return Interval::fromMilliseconds((unsigned)getUInt8(Offset::decodeTime())*10);
}
void
AnytoneCodeplugGen1::FiveToneSettingsElement::setDecodeTime(const Interval &ms) {
  setUInt8(Offset::decodeTime(), ms.milliseconds()/10);
}

Interval
AnytoneCodeplugGen1::FiveToneSettingsElement::delayAfterStop() const {
  return Interval::fromMilliseconds((unsigned)getUInt8(Offset::delayAfterStop())*10);
}
void
AnytoneCodeplugGen1::FiveToneSettingsElement::setDelayAfterStop(const Interval &ms) {
  setUInt8(Offset::delayAfterStop(), ms.milliseconds()/10);
}

Interval
AnytoneCodeplugGen1::FiveToneSettingsElement::preTime() const {
  return Interval::fromMilliseconds((unsigned)getUInt8(Offset::preTime())*10);
}
void
AnytoneCodeplugGen1::FiveToneSettingsElement::setPreTime(const Interval &ms) {
  setUInt8(Offset::preTime(), ms.milliseconds()/10);
}

AnytoneCodeplugGen1::FiveToneSettingsElement::Standard
AnytoneCodeplugGen1::FiveToneSettingsElement::botStandard() const {
  return (Standard) getUInt8(Offset::botStandard());
}
void
AnytoneCodeplugGen1::FiveToneSettingsElement::setBOTStandard(Standard standard) {
  setUInt8(Offset::botStandard(), (unsigned)standard);
}

Interval
AnytoneCodeplugGen1::FiveToneSettingsElement::botToneDuration() const {
  return Interval::fromMilliseconds(getUInt8(Offset::botToneDuration()));
}
void
AnytoneCodeplugGen1::FiveToneSettingsElement::setBOTToneDuration(const Interval &ms) {
  setUInt8(Offset::botToneDuration(), ms.milliseconds());
}

QString
AnytoneCodeplugGen1::FiveToneSettingsElement::botID() const {
  unsigned len = getUInt8(Offset::botIdLength());
  QString id;
  for (unsigned i=0; i<len; i++) {
    uint8_t b = getUInt8(Offset::botId()+(i/2));
    if (0 == (i%2))
      id.append(QString::number((b>>4)&0xf, 16));
    else
      id.append(QString::number((b>>0)&0xf, 16));
  }
  return id;
}
void
AnytoneCodeplugGen1::FiveToneSettingsElement::setBOTID(const QString &id) {
  unsigned len = 0;
  for (int i=0; i<id.length(); i++) {
    bool ok;
    if (0 == (len%2)) {
      setUInt4(Offset::botId()+len/2, 4, id.mid(i, 1).toUInt(&ok, 16));
    } else {
      setUInt4(Offset::botId()+len/2, 0, id.mid(i, 1).toUInt(&ok, 16));
    }
    len++;
  }
  setUInt8(Offset::botIdLength(), len);
}

AnytoneCodeplugGen1::FiveToneSettingsElement::Standard
AnytoneCodeplugGen1::FiveToneSettingsElement::eotStandard() const {
  return (Standard) getUInt8(Offset::eotStandard());
}
void
AnytoneCodeplugGen1::FiveToneSettingsElement::setEOTStandard(Standard standard) {
  setUInt8(Offset::eotStandard(), (unsigned)standard);
}

Interval
AnytoneCodeplugGen1::FiveToneSettingsElement::eotToneDuration() const {
  return Interval::fromMilliseconds(getUInt8(Offset::eotToneDuration()));
}
void
AnytoneCodeplugGen1::FiveToneSettingsElement::setEOTToneDuration(const Interval &ms) {
  setUInt8(Offset::eotToneDuration(), ms.milliseconds());
}

QString
AnytoneCodeplugGen1::FiveToneSettingsElement::eotID() const {
  unsigned len = getUInt8(Offset::eotIdLength());
  QString id;
  for (unsigned i=0; i<len; i++) {
    uint8_t b = getUInt8(Offset::eotId()+(i/2));
    if (0 == (i%2))
      id.append(QString::number((b>>4)&0xf, 16));
    else
      id.append(QString::number((b>>0)&0xf, 16));
  }
  return id;
}
void
AnytoneCodeplugGen1::FiveToneSettingsElement::setEOTID(const QString &id) {
  unsigned len = 0;
  for (int i=0; i<id.length(); i++) {
    bool ok;
    if (0 == (len%2)) {
      setUInt4(Offset::eotId()+len/2, 4, id.mid(i, 1).toUInt(&ok, 16));
    } else {
      setUInt4(Offset::eotId()+len/2, 0, id.mid(i, 1).toUInt(&ok, 16));
    }
    len++;
  }
  setUInt8(Offset::eotIdLength(), len);
}



/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::TwoToneIDElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::TwoToneIDElement::TwoToneIDElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::TwoToneIDElement::TwoToneIDElement(uint8_t *ptr)
  : Element(ptr, TwoToneIDElement::size())
{
  // pass...
}

void
AnytoneCodeplugGen1::TwoToneIDElement::clear() {
  memset(_data, 0x00, _size);
}

double
AnytoneCodeplugGen1::TwoToneIDElement::firstTone() const {
  return ((double)getUInt16_le(Offset::firstTone()))/10;
}
void
AnytoneCodeplugGen1::TwoToneIDElement::setFirstTone(double f) {
  setUInt16_le(Offset::firstTone(), f*10);
}

double
AnytoneCodeplugGen1::TwoToneIDElement::secondTone() const {
  return ((double)getUInt16_le(Offset::secondTone()))/10;
}
void
AnytoneCodeplugGen1::TwoToneIDElement::setSecondTone(double f) {
  setUInt16_le(Offset::secondTone(), f*10);
}

QString
AnytoneCodeplugGen1::TwoToneIDElement::name() const {
  return readASCII(Offset::name(), 7, 0x00);
}
void
AnytoneCodeplugGen1::TwoToneIDElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::nameLength(), 0x00);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::TwoToneIDBitmapElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::TwoToneIDBitmapElement::TwoToneIDBitmapElement(uint8_t *ptr, size_t size)
  : BitmapElement(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::TwoToneIDBitmapElement::TwoToneIDBitmapElement(uint8_t *ptr)
  : BitmapElement(ptr, TwoToneIDBitmapElement::size())
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::TwoToneFunctionElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::TwoToneFunctionElement::TwoToneFunctionElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::TwoToneFunctionElement::TwoToneFunctionElement(uint8_t *ptr)
  : Element(ptr, TwoToneFunctionElement::size())
{
  // pass...
}

void
AnytoneCodeplugGen1::TwoToneFunctionElement::clear() {
  memset(_data, 0x00, _size);
}

double
AnytoneCodeplugGen1::TwoToneFunctionElement::firstTone() const {
  return ((double)getUInt16_le(Offset::firstTone()))/10;
}
void
AnytoneCodeplugGen1::TwoToneFunctionElement::setFirstTone(double f) {
  setUInt16_le(Offset::firstTone(), f*10);
}

double
AnytoneCodeplugGen1::TwoToneFunctionElement::secondTone() const {
  return ((double)getUInt16_le(Offset::secondTone()))/10;
}
void
AnytoneCodeplugGen1::TwoToneFunctionElement::setSecondTone(double f) {
  setUInt16_le(Offset::secondTone(), f*10);
}

AnytoneCodeplugGen1::TwoToneFunctionElement::Response
AnytoneCodeplugGen1::TwoToneFunctionElement::response() const {
  return (Response) getUInt8(Offset::response());
}
void
AnytoneCodeplugGen1::TwoToneFunctionElement::setResponse(Response resp) {
  setUInt8(Offset::response(), (unsigned)resp);
}

QString
AnytoneCodeplugGen1::TwoToneFunctionElement::name() const {
  return readASCII(Offset::name(), Limit::nameLength(), 0x00);
}
void
AnytoneCodeplugGen1::TwoToneFunctionElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::nameLength(), 0x00);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::TwoToneFunctionBitmapElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::TwoToneFunctionBitmapElement::TwoToneFunctionBitmapElement(uint8_t *ptr, size_t size)
  : BitmapElement(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::TwoToneFunctionBitmapElement::TwoToneFunctionBitmapElement(uint8_t *ptr)
  : BitmapElement(ptr, TwoToneFunctionBitmapElement::size())
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::TwoToneSettingsElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::TwoToneSettingsElement::TwoToneSettingsElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::TwoToneSettingsElement::TwoToneSettingsElement(uint8_t *ptr)
  : Element(ptr, TwoToneSettingsElement::size())
{
  // pass...
}

void
AnytoneCodeplugGen1::TwoToneSettingsElement::clear() {
  memset(_data, 0x00, _size);
}

Interval
AnytoneCodeplugGen1::TwoToneSettingsElement::firstToneDuration() const {
  return Interval::fromMilliseconds((unsigned)getUInt8(Offset::firstToneDuration()) * 100);
}
void
AnytoneCodeplugGen1::TwoToneSettingsElement::setFirstToneDuration(const Interval &ms) {
  setUInt8(Offset::firstToneDuration(), ms.milliseconds()/100);
}

Interval
AnytoneCodeplugGen1::TwoToneSettingsElement::secondToneDuration() const {
  return Interval::fromMilliseconds((unsigned)getUInt8(Offset::secondToneDuration())*100);
}
void
AnytoneCodeplugGen1::TwoToneSettingsElement::setSecondToneDuration(const Interval &ms) {
  setUInt8(Offset::secondToneDuration(), ms.milliseconds()/100);
}

Interval
AnytoneCodeplugGen1::TwoToneSettingsElement::longToneDuration() const {
  return Interval::fromMilliseconds((unsigned)getUInt8(Offset::longToneDuration())*100);
}
void
AnytoneCodeplugGen1::TwoToneSettingsElement::setLongToneDuration(const Interval &ms) {
  setUInt8(Offset::longToneDuration(), ms.milliseconds()/100);
}

Interval
AnytoneCodeplugGen1::TwoToneSettingsElement::gapDuration() const {
  return Interval::fromMilliseconds((unsigned)getUInt8(Offset::gapDuration())*100);
}
void
AnytoneCodeplugGen1::TwoToneSettingsElement::setGapDuration(const Interval &ms) {
  setUInt8(Offset::gapDuration(), ms.milliseconds()/100);
}

Interval
AnytoneCodeplugGen1::TwoToneSettingsElement::autoResetTime() const {
  return Interval::fromSeconds((unsigned)getUInt8(Offset::autoResetTime())*10);
}
void
AnytoneCodeplugGen1::TwoToneSettingsElement::setAutoResetTime(const Interval &sec) {
  setUInt8(Offset::autoResetTime(), sec.seconds()/10);
}

bool
AnytoneCodeplugGen1::TwoToneSettingsElement::sidetone() const {
  return getUInt8(Offset::sidetone());
}
void
AnytoneCodeplugGen1::TwoToneSettingsElement::enableSidetone(bool enable) {
  setUInt8(Offset::sidetone(), (enable ? 0x01 : 0x00));
}



/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::DTMFSettingsElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::DTMFSettingsElement::DTMFSettingsElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::DTMFSettingsElement::DTMFSettingsElement(uint8_t *ptr)
  : Element(ptr, DTMFSettingsElement::size())
{
  // pass...
}

void
AnytoneCodeplugGen1::DTMFSettingsElement::clear() {
  memset(_data, 0x00, _size);
}


unsigned
AnytoneCodeplugGen1::DTMFSettingsElement::intervalSymbol() const {
  return getUInt8(Offset::intervalSymbol());
}
void
AnytoneCodeplugGen1::DTMFSettingsElement::setIntervalSymbol(unsigned symb) {
  setUInt8(Offset::intervalSymbol(), symb);
}

unsigned
AnytoneCodeplugGen1::DTMFSettingsElement::groupCode() const {
  return getUInt8(Offset::groupCode());
}
void
AnytoneCodeplugGen1::DTMFSettingsElement::setGroupCode(unsigned symb) {
  setUInt8(Offset::groupCode(), symb);
}

AnytoneCodeplugGen1::DTMFSettingsElement::Response
AnytoneCodeplugGen1::DTMFSettingsElement::response() const {
  return (Response)getUInt8(Offset::response());
}
void
AnytoneCodeplugGen1::DTMFSettingsElement::setResponse(Response resp) {
  setUInt8(Offset::response(), (unsigned)resp);
}

Interval
AnytoneCodeplugGen1::DTMFSettingsElement::preTime() const {
  return Interval::fromMilliseconds((unsigned)getUInt8(Offset::preTime())*10);
}
void
AnytoneCodeplugGen1::DTMFSettingsElement::setPreTime(const Interval &ms) {
  setUInt8(Offset::preTime(), ms.milliseconds()/10);
}

Interval
AnytoneCodeplugGen1::DTMFSettingsElement::firstDigitDuration() const {
  return Interval::fromMilliseconds((unsigned)getUInt8(Offset::firstDigitDuration())*10);
}
void
AnytoneCodeplugGen1::DTMFSettingsElement::setFirstDigitDuration(const Interval &ms) {
  setUInt8(Offset::firstDigitDuration(), ms.milliseconds()/10);
}

Interval
AnytoneCodeplugGen1::DTMFSettingsElement::autoResetTime() const {
  return Interval::fromSeconds((unsigned)getUInt8(Offset::autoResetTime())*10);
}
void
AnytoneCodeplugGen1::DTMFSettingsElement::setAutoResetTime(const Interval &sec) {
  setUInt8(Offset::autoResetTime(), sec.seconds()/10);
}

QString
AnytoneCodeplugGen1::DTMFSettingsElement::id() const {
  QString id;
  for (unsigned int i=0; (i<Limit::idLength())&&(0xff!=getUInt8(Offset::id()+i)); i++) {
    id.append(QString::number(getUInt8(Offset::id()+i),16));
  }
  return id;
}
void
AnytoneCodeplugGen1::DTMFSettingsElement::setID(const QString &id) {
  int len = std::min((qsizetype)Limit::idLength(), id.length());
  bool ok;
  for (int i=0; i<len; i++)
    setUInt8(Offset::id()+i, id.mid(i,1).toUInt(&ok, 16));
}

Interval
AnytoneCodeplugGen1::DTMFSettingsElement::postEncodingDelay() const {
  return Interval::fromMilliseconds((unsigned)getUInt8(Offset::postEncodingDelay())*10);
}
void
AnytoneCodeplugGen1::DTMFSettingsElement::setPostEncodingDelay(const Interval &ms) {
  setUInt8(Offset::postEncodingDelay(), ms.milliseconds()/10);
}

Interval
AnytoneCodeplugGen1::DTMFSettingsElement::pttIDPause() const {
  return Interval::fromSeconds((unsigned)getUInt8(Offset::pttIDPause())*10);
}
void
AnytoneCodeplugGen1::DTMFSettingsElement::setPTTIDPause(const Interval &sec) {
  setUInt8(Offset::pttIDPause(), sec.seconds()/10);
}

bool
AnytoneCodeplugGen1::DTMFSettingsElement::pttIDEnabled() const {
  return getUInt8(Offset::pttIDEnabled());
}
void
AnytoneCodeplugGen1::DTMFSettingsElement::enablePTTID(bool enable) {
  setUInt8(Offset::pttIDEnabled(), (enable ? 0x01 : 0x00));
}

Interval
AnytoneCodeplugGen1::DTMFSettingsElement::dCodePause() const {
  return Interval::fromSeconds((unsigned)getUInt8(Offset::dCodePause()));
}
void
AnytoneCodeplugGen1::DTMFSettingsElement::setDCodePause(const Interval &sec) {
  setUInt8(Offset::dCodePause(), sec.seconds());
}

bool
AnytoneCodeplugGen1::DTMFSettingsElement::sidetone() const {
  return getUInt8(Offset::sidetone());
}
void
AnytoneCodeplugGen1::DTMFSettingsElement::enableSidetone(bool enable) {
  setUInt8(Offset::sidetone(), (enable ? 0x01 : 0x00));
}

QString
AnytoneCodeplugGen1::DTMFSettingsElement::botID() const {
  QString id;
  for (unsigned int i=0; (i<Limit::botIdLength())&&(0xff!=getUInt8(Offset::botID()+i)); i++) {
    id.append(QString::number(getUInt8(Offset::botID()+i),16));
  }
  return id;
}
void
AnytoneCodeplugGen1::DTMFSettingsElement::setBOTID(const QString &id) {
  int len = std::min(qsizetype(Limit::botIdLength()), id.length());
  bool ok;
  for (int i=0; i<len; i++)
    setUInt8(Offset::botID()+i, id.mid(i,1).toUInt(&ok, 16));
}

QString
AnytoneCodeplugGen1::DTMFSettingsElement::eotID() const {
  QString id;
  for (unsigned int i=0; (i<Limit::eotIdLength())&&(0xff!=getUInt8(Offset::eotID()+i)); i++) {
    id.append(QString::number(getUInt8(Offset::eotID()+i),16));
  }
  return id;
}
void
AnytoneCodeplugGen1::DTMFSettingsElement::setEOTID(const QString &id) {
  int len = std::min(qsizetype(Limit::eotIdLength()), id.length());
  bool ok;
  for (int i=0; i<len; i++)
    setUInt8(Offset::eotID()+i, id.mid(i,1).toUInt(&ok, 16));
}

QString
AnytoneCodeplugGen1::DTMFSettingsElement::remoteKillID() const {
  QString id;
  for (unsigned int i=0; (i<Limit::remoteKillIdLength()) && (0xff!=getUInt8(Offset::remoteKillID()+i)); i++) {
    id.append(QString::number(getUInt8(Offset::remoteKillID()+i),16));
  }
  return id;
}
void
AnytoneCodeplugGen1::DTMFSettingsElement::setRemoteKillID(const QString &id) {
  int len = std::min(qsizetype(Limit::remoteKillIdLength()), id.length());
  bool ok;
  for (int i=0; i<len; i++)
    setUInt8(Offset::remoteKillID()+i, id.mid(i,1).toUInt(&ok, 16));
}


QString
AnytoneCodeplugGen1::DTMFSettingsElement::remoteStunID() const {
  QString id;
  for (unsigned int i=0; (i<Limit::remteStunIdLength()) && (0xff!=getUInt8(Offset::remoteStunID()+i)); i++) {
    id.append(QString::number(getUInt8(Offset::remoteStunID()+i),16));
  }
  return id;
}
void
AnytoneCodeplugGen1::DTMFSettingsElement::setRemoteStunID(const QString &id) {
  int len = std::min(qsizetype(Limit::remteStunIdLength()), id.length());
  bool ok;
  for (int i=0; i<len; i++)
    setUInt8(Offset::remoteStunID()+i, id.mid(i,1).toUInt(&ok, 16));
}



/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::DTMFIDListElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::DTMFIDListElement::DTMFIDListElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::DTMFIDListElement::DTMFIDListElement(uint8_t *ptr)
  : Element(ptr, DTMFIDListElement::size())
{
  // pass...
}

void
AnytoneCodeplugGen1::DTMFIDListElement::clear() {
  memset(_data, 0xff, _size);
}

bool
AnytoneCodeplugGen1::DTMFIDListElement::hasNumber(unsigned int n) const {
  if (n >= Limit::numEntries())
    return false;
  return 0xff != getUInt8(n*Limit::numberLength());
}

QString
AnytoneCodeplugGen1::DTMFIDListElement::number(unsigned int n) const {
  if (n >= Limit::numEntries())
    return "";
  uint8_t *num = _data + n*Limit::numberLength();
  return decode_dtmf_bin(num, Limit::numberLength(), 0xff);
}

void
AnytoneCodeplugGen1::DTMFIDListElement::setNumber(unsigned int n, const QString &number) {
  if (n >= Limit::numEntries())
    return;
  uint8_t *num = _data + n*Limit::numberLength();
  encode_dtmf_bin(number, num, Limit::numberLength(), 0xff);
}

void
AnytoneCodeplugGen1::DTMFIDListElement::clearNumber(unsigned int n) {
  if (n >= Limit::numEntries())
    return;
  uint8_t *num = _data + n*Limit::numberLength();
  memset(num, 0xff, Limit::numberLength());
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::WFMChannelListElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::WFMChannelListElement::WFMChannelListElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::WFMChannelListElement::WFMChannelListElement(uint8_t *ptr)
  : Element(ptr, WFMChannelListElement::size())
{
  // pass...
}

void
AnytoneCodeplugGen1::WFMChannelListElement::clear() {
  memset(_data, 0x00, _size);
}

bool
AnytoneCodeplugGen1::WFMChannelListElement::hasChannel(unsigned int n) const {
  if (n >= Limit::numEntries())
    return false;
  return 0 != getBCD8_le(n*Offset::betweenChannels());
}

Frequency
AnytoneCodeplugGen1::WFMChannelListElement::channel(unsigned int n) const {
  if (n >= Limit::numEntries())
    return Frequency();
  return Frequency::fromHz(((unsigned long long)getBCD8_le(n*Offset::betweenChannels()))*100);
}

void
AnytoneCodeplugGen1::WFMChannelListElement::setChannel(unsigned int n, Frequency freq) {
  if (n >= Limit::numEntries())
    return;
  setBCD8_le(n*Offset::betweenChannels(), freq.inHz()/100);
}

void
AnytoneCodeplugGen1::WFMChannelListElement::clearChannel(unsigned int n) {
  if (n >= Limit::numEntries())
    return;
  setBCD8_le(n*Offset::betweenChannels(), 0);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::WFMChannelBitmapElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::WFMChannelBitmapElement::WFMChannelBitmapElement(uint8_t *ptr, size_t size)
  : BitmapElement(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::WFMChannelBitmapElement::WFMChannelBitmapElement(uint8_t *ptr)
  : BitmapElement(ptr, WFMChannelBitmapElement::size())
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::WFMVFOElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::WFMVFOElement::WFMVFOElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::WFMVFOElement::WFMVFOElement(uint8_t *ptr)
  : Element(ptr, WFMVFOElement::size())
{
  // pass...
}

void
AnytoneCodeplugGen1::WFMVFOElement::clear() {
  memset(_data, 0x00, _size);
  setFrequency(Frequency::fromMHz(88));
}

Frequency
AnytoneCodeplugGen1::WFMVFOElement::frequency() const {
  return Frequency::fromHz(((unsigned long long)getBCD8_le(0))*100);
}

void
AnytoneCodeplugGen1::WFMVFOElement::setFrequency(Frequency freq) {
  setBCD8_le(0, freq.inHz()/100);
}



/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::DMREncryptionKeyListElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::DMREncryptionKeyListElement::DMREncryptionKeyListElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::DMREncryptionKeyListElement::DMREncryptionKeyListElement(uint8_t *ptr)
  : Element(ptr, DMREncryptionKeyListElement::size())
{
  // pass...
}

void
AnytoneCodeplugGen1::DMREncryptionKeyListElement::clear() {
  memset(_data, 0x00, _size);
}

bool
AnytoneCodeplugGen1::DMREncryptionKeyListElement::hasKey(unsigned int n) const {
  if (n >= Limit::numEntries())
    return false;
  return 0 != getUInt16_be(n*Offset::betweenKeys());
}

QByteArray
AnytoneCodeplugGen1::DMREncryptionKeyListElement::key(unsigned int n) const {
  if (n >= Limit::numEntries())
    return QByteArray::fromHex("0000");
  return QByteArray((char *)_data + n*Offset::betweenKeys(), 2);
}

void
AnytoneCodeplugGen1::DMREncryptionKeyListElement::setKey(unsigned int n, const BasicEncryptionKey &key) {
  if (n >= Limit::numEntries())
    return;
  memcpy(_data + n*Offset::betweenKeys(), key.key().data(), 2);
}

void
AnytoneCodeplugGen1::DMREncryptionKeyListElement::clearKey(unsigned int n) {
  if (n >= Limit::numEntries())
    return;
  setUInt16_be(n*Offset::betweenKeys(), 0000);
}



/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::EnhancedEncryptionKeyListElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::EnhancedEncryptionKeyListElement::EnhancedEncryptionKeyListElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::EnhancedEncryptionKeyListElement::EnhancedEncryptionKeyListElement(uint8_t *ptr)
  : Element(ptr, EnhancedEncryptionKeyListElement::size())
{
  // pass...
}

void
AnytoneCodeplugGen1::EnhancedEncryptionKeyListElement::clear() {
  memset(_data, 0x00, _size);
  for (unsigned int i=0; i<Limit::numEntries(); i++) {
    setKey(i, QByteArray::fromHex("FFFF"));
  }
}

QByteArray
AnytoneCodeplugGen1::EnhancedEncryptionKeyListElement::key(unsigned int n) const {
  if (n >= Limit::numEntries())
    return QByteArray();
  return QByteArray::fromRawData((const char *)_data + Offset::keys() + n*Offset::betweenKeys(), 2);
}

void
AnytoneCodeplugGen1::EnhancedEncryptionKeyListElement::setKey(unsigned int n, const QByteArray &key) {
  if ((n >= Limit::numEntries()) || (2 != key.size()))
    return;
  memcpy(_data + Offset::keys() + n*Offset::betweenKeys(), key.constData(), 2);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1::ContactMapElement
 * ********************************************************************************************* */
AnytoneCodeplugGen1::ContactMapElement::ContactMapElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen1::ContactMapElement::ContactMapElement(uint8_t *ptr)
  : Element(ptr, ContactMapElement::size())
{
  // pass...
}

void
AnytoneCodeplugGen1::ContactMapElement::clear() {
  memset(_data, 0xff, _size);
}

bool
AnytoneCodeplugGen1::ContactMapElement::isValid() const {
  return (0xffffffff!=getUInt32_le(Offset::id()))
      && (0xffffffff!=getUInt32_le(Offset::index()));
}

bool
AnytoneCodeplugGen1::ContactMapElement::isGroup() const {
  uint32_t tmp = getUInt32_le(Offset::id());
  return tmp & 0x01;
}
unsigned
AnytoneCodeplugGen1::ContactMapElement::id() const {
  uint32_t tmp = getUInt32_le(Offset::id());
  tmp = tmp >> 1;
  return decode_dmr_id_bcd_le((uint8_t *)&tmp);
}
void
AnytoneCodeplugGen1::ContactMapElement::setID(unsigned id, bool group) {
  uint32_t tmp; encode_dmr_id_bcd_le((uint8_t *)&tmp, id);
  tmp = ( (tmp << 1) | (group ? 1 : 0) );
  setUInt32_le(Offset::id(), tmp);
}

unsigned
AnytoneCodeplugGen1::ContactMapElement::index() const {
  return getUInt32_le(Offset::index());
}
void
AnytoneCodeplugGen1::ContactMapElement::setIndex(unsigned idx) {
  setUInt32_le(Offset::index(), idx);
}



/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen1
 * ********************************************************************************************* */
AnytoneCodeplugGen1::AnytoneCodeplugGen1(const QString &label, QObject *parent)
  : AnytoneCodeplugBase(parent), _label(label)
{
  // pass...
}

AnytoneCodeplugGen1::~AnytoneCodeplugGen1() {
  // pass...
}

void
AnytoneCodeplugGen1::clear() {
  while (this->numImages())
    remImage(0);

  addImage(_label);

  // Allocate bitmaps
  this->allocateBitmaps();
}

bool
AnytoneCodeplugGen1::index(Config *config, Context &ctx, const ErrorStack &err) const {
  Q_UNUSED(err)

  // AM channels are indexed separately.
  ctx.addTable(&AMChannel::staticMetaObject);

  // Map radio IDs
  for (int i=0; i<config->radioIDs()->count(); i++) {
    if (config->radioIDs()->get(i)->is<DMRRadioID>())
      ctx.add(config->radioIDs()->get(i)->as<DMRRadioID>(), i);
  }

  // Map digital and DTMF contacts
  for (int i=0, d=0, a=0; i<config->contacts()->count(); i++) {
    if (config->contacts()->contact(i)->is<DMRContact>()) {
      ctx.add(config->contacts()->contact(i)->as<DMRContact>(), d); d++;
    } else if (config->contacts()->contact(i)->is<DTMFContact>()) {
      ctx.add(config->contacts()->contact(i)->as<DTMFContact>(), a); a++;
    } else {
      logInfo() << "Cannot index contact '" << config->contacts()->contact(i)->name()
                << "'. Contact type '" << config->contacts()->contact(i)->metaObject()->className()
                << "' not supported by or implemented for AnyTone devices.";
    }
  }

  // Map rx group lists
  for (int i=0; i<config->rxGroupLists()->count(); i++)
    ctx.add(config->rxGroupLists()->list(i), i);

  // Map channels
  for (int i=0, common=0, am=0; i<config->channelList()->count(); i++) {
    if (config->channelList()->channel(i)->is<DMRChannel>() ||
        config->channelList()->channel(i)->is<FMChannel>()) {
      ctx.add(config->channelList()->channel(i), common++);
    } else if (config->channelList()->channel(i)->is<AMChannel>()) {
      ctx.add(config->channelList()->channel(i), am++);
    }
  }

  // Map zones
  for (int i=0; i<config->zones()->count(); i++)
    ctx.add(config->zones()->zone(i), i);

  // Map scan lists
  for (int i=0; i<config->scanlists()->count(); i++)
    ctx.add(config->scanlists()->scanlist(i), i);

  // Map DMR APRS systems
  for (int i=0,a=0,d=0; i<config->posSystems()->count(); i++) {
    if (config->posSystems()->system(i)->is<DMRAPRSSystem>()) {
      ctx.add(config->posSystems()->system(i)->as<DMRAPRSSystem>(), d); d++;
    } else if (config->posSystems()->system(i)->is<FMAPRSSystem>()) {
      auto *aprs = config->posSystems()->system(i)->as<FMAPRSSystem>();
      ctx.add(aprs, a); a++;
      // Index FM APRS frequencies (referenced in channel extensions).
      if (auto *ext = aprs->anytoneExtension()) {
        for (int j=0; j<ext->frequencies()->count(); j++) {
          // Index 0 = default, so index is 1-based
          ctx.add(ext->frequencies()->get(j)->as<AnytoneAPRSFrequency>(), j+1);
        }
      }
    }
  }

  // Map roaming
  for (int i=0; i<config->roamingZones()->count(); i++)
    ctx.add(config->roamingZones()->zone(i), i);
  for (int i=0; i<config->roamingChannels()->count(); i++)
    ctx.add(config->roamingChannels()->channel(i), i);

  // Map auto-repeater offsets
  if (config->settings()->anytoneExtension()) {
    auto *autoRep = config->settings()->anytoneExtension()->autoRepeaterSettings();
    for (int i=0; i<autoRep->offsets()->count(); i++)
      ctx.add(autoRep->offsets()->get(i)->as<AnytoneAutoRepeaterOffset>(), i);
  }

  // Map SMS templates
  for (int i=0; i<config->smsExtension()->smsTemplates()->count(); i++)
    ctx.add(config->smsExtension()->smsTemplates()->get(i)->as<SMSTemplate>(), i);

  if (config->commercialExtension()) {
    // Map all encryption keys
    for (int i=0, basic=0, enhanced=0, aes=0; i<config->commercialExtension()->encryptionKeys()->count(); i++) {
      auto key = config->commercialExtension()->encryptionKeys()->key(i);
      if (key->is<BasicEncryptionKey>())
        ctx.add(key->as<BasicEncryptionKey>(), basic++);
      else if (key->is<ARC4EncryptionKey>())
        ctx.add(key->as<ARC4EncryptionKey>(), enhanced++);
      else if (key->is<AESEncryptionKey>())
        ctx.add(key->as<AESEncryptionKey>(), aes++);
    }
  }

  return true;
}


bool
AnytoneCodeplugGen1::decodeElements(Context &ctx, const ErrorStack &err) {
  if (! this->createElements(ctx, err)) {
    errMsg(err) << "Cannot decode AnyTone codeplug: Creation of config objects failed.";
    return false;
  }

  if (! this->linkElements(ctx, err)) {
    errMsg(err) << "Cannot decode AnyTone codeplug: Linking of config objects failed.";
    return false;
  }

  return true;
}


Config *
AnytoneCodeplugGen1::preprocess(Config *config, const ErrorStack &err) const {
  Config *intermediate = Codeplug::preprocess(config, err);
  if (nullptr == intermediate) {
    errMsg(err) << "Cannot pre-process codeplug for anytone device.";
    return nullptr;
  }

  // Remove all AM & M17 channels
  ObjectFilterVisitor amFilter{AMChannel::staticMetaObject, M17Channel::staticMetaObject};
  if (! amFilter.process(intermediate, err)) {
    errMsg(err) << "Remove AM & M17 channels.";
    delete intermediate;
    return nullptr;
  }

  ZoneSplitVisitor splitter;
  if (! splitter.process(intermediate, err)) {
    errMsg(err) << "Split multi-VFO zones.";
    delete intermediate;
    return nullptr;
  }

  return intermediate;
}


bool
AnytoneCodeplugGen1::postprocess(Config *config, const ErrorStack &err) const {
  if (! Codeplug::postprocess(config, err)) {
    errMsg(err) << "Cannot post-process codeplug for anytone device.";
    return false;
  }

  ZoneMergeVisitor merger;
  if (! merger.process(config, err)) {
    errMsg(err) << "Cannot post-process codeplug for anytone device.";
    return false;
  }

  return true;
}


bool
AnytoneCodeplugGen1::encode(Config *config, const Flags &flags, const ErrorStack &err) {
  Context ctx(config);
  // Register table for auto-repeater offsets
  ctx.addTable(&AnytoneAutoRepeaterOffset::staticMetaObject);
  // Register table for FM APRS frequencies
  ctx.addTable(&AnytoneAPRSFrequency::staticMetaObject);

  if (! index(config, ctx, err)) {
    errMsg(err) << "Cannot encode anytone codeplug.";
    return false;
  }

  // If codeplug is generated from scratch -> clear and reallocate
  if (! flags.updateCodeplug()) {
    // Clear codeplug
    this->clear();
    // Then allocate elements
    this->allocateUpdated();
  }

  // First set bitmaps
  this->setBitmaps(ctx);
  // Allocate all memory elements representing the common config
  this->allocateForEncoding();

  // Then encode everything.
  return this->encodeElements(flags, ctx, err);
}

bool
AnytoneCodeplugGen1::decode(Config *config, const ErrorStack &err) {
  // Maps code-plug indices to objects
  Context ctx(config);

  // Register table for auto-repeater offsets
  ctx.addTable(&AnytoneAutoRepeaterOffset::staticMetaObject);
  // Register table for FM APRS frequencies
  ctx.addTable(&AnytoneAPRSFrequency::staticMetaObject);

  return this->decodeElements(ctx, err);
}
