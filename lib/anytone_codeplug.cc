#include "anytone_codeplug.hh"
#include "utils.hh"
#include "logger.hh"
#include "anytone_extension.hh"
#include "melody.hh"
#include <QTimeZone>
#include <QRegularExpression>

using namespace Signaling;

#define CUSTOM_CTCSS_TONE 0x33

Code _anytone_ctcss_num2code[52] = {
  SIGNALING_NONE, // 62.5 not supported
  CTCSS_67_0Hz,  SIGNALING_NONE, // 69.3 not supported
  CTCSS_71_9Hz,  CTCSS_74_4Hz,  CTCSS_77_0Hz,  CTCSS_79_7Hz,  CTCSS_82_5Hz,
  CTCSS_85_4Hz,  CTCSS_88_5Hz,  CTCSS_91_5Hz,  CTCSS_94_8Hz,  CTCSS_97_4Hz,  CTCSS_100_0Hz,
  CTCSS_103_5Hz, CTCSS_107_2Hz, CTCSS_110_9Hz, CTCSS_114_8Hz, CTCSS_118_8Hz, CTCSS_123_0Hz,
  CTCSS_127_3Hz, CTCSS_131_8Hz, CTCSS_136_5Hz, CTCSS_141_3Hz, CTCSS_146_2Hz, CTCSS_151_4Hz,
  CTCSS_156_7Hz,
  SIGNALING_NONE, // 159.8 not supported
  CTCSS_162_2Hz,
  SIGNALING_NONE, // 165.5 not supported
  CTCSS_167_9Hz,
  SIGNALING_NONE, // 171.3 not supported
  CTCSS_173_8Hz,
  SIGNALING_NONE, // 177.3 not supported
  CTCSS_179_9Hz,
  SIGNALING_NONE, // 183.5 not supported
  CTCSS_186_2Hz,
  SIGNALING_NONE, // 189.9 not supported
  CTCSS_192_8Hz,
  SIGNALING_NONE, SIGNALING_NONE, // 196.6 & 199.5 not supported
  CTCSS_203_5Hz,
  SIGNALING_NONE, // 206.5 not supported
  CTCSS_210_7Hz, CTCSS_218_1Hz, CTCSS_225_7Hz,
  SIGNALING_NONE, // 229.1 not supported
  CTCSS_233_6Hz, CTCSS_241_8Hz, CTCSS_250_3Hz,
  SIGNALING_NONE, SIGNALING_NONE // 254.1 and custom CTCSS not supported.
};

inline uint8_t
ctcss_code2num(Signaling::Code code) {
  for (uint8_t i=0; i<52; i++) {
    if (code == _anytone_ctcss_num2code[i])
      return i;
  }
  return 0;
}

inline Signaling::Code
ctcss_num2code(uint8_t num) {
  if (num >= 52)
    return Signaling::SIGNALING_NONE;
  return _anytone_ctcss_num2code[num];
}

QVector<char> _anytone_bin_dtmf_tab = {
  '0','1','2','3','4','5','6','7','8','9','A','B','C','D','*','#'
};


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::BitmapElement
 * ********************************************************************************************* */
AnytoneCodeplug::BitmapElement::BitmapElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

void
AnytoneCodeplug::BitmapElement::clear() {
  memset(_data, 0, _size);
}

bool
AnytoneCodeplug::BitmapElement::isEncoded(unsigned int idx) const {
  unsigned int byte = idx/8, bit = idx%8;
  return (_data[byte] & (1 << bit));
}

void
AnytoneCodeplug::BitmapElement::setEncoded(unsigned int idx, bool enable) {
  unsigned int byte = idx/8, bit = idx%8;
  if (enable)
    _data[byte] |= (1 << bit);
  else
    _data[byte] &= ~(1 << bit);
}

void
AnytoneCodeplug::BitmapElement::enableFirst(unsigned int n) {
  unsigned int byte = n/8, bit=n%8;
  memset(_data, 0xff, byte);
  for (unsigned int i=0; i<bit; i++) {
    _data[byte] |= (1<<i);
  }
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::InvertedBitmapElement
 * ********************************************************************************************* */
AnytoneCodeplug::InvertedBitmapElement::InvertedBitmapElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

void
AnytoneCodeplug::InvertedBitmapElement::clear() {
  memset(_data, 0xff, _size);
}

bool
AnytoneCodeplug::InvertedBitmapElement::isEncoded(unsigned int idx) const {
  unsigned int byte = idx/8, bit = idx%8;
  return 0 == (_data[byte] & (1 << bit));
}

void
AnytoneCodeplug::InvertedBitmapElement::setEncoded(unsigned int idx, bool enable) {
  unsigned int byte = idx/8, bit = idx%8;
  if (enable)
    _data[byte] &= ~(1 << bit);
  else
    _data[byte] |= (1 << bit);
}

void
AnytoneCodeplug::InvertedBitmapElement::enableFirst(unsigned int n) {
  unsigned int byte = n/8, bit=n%8;
  memset(_data, 0x00, byte);
  for (unsigned int i=0; i<bit; i++) {
    _data[byte] &= ~(1<<i);
  }
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::InvertedBytemapElement
 * ********************************************************************************************* */
AnytoneCodeplug::InvertedBytemapElement::InvertedBytemapElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

void
AnytoneCodeplug::InvertedBytemapElement::clear() {
  memset(_data, 0xff, _size);
}

bool
AnytoneCodeplug::InvertedBytemapElement::isEncoded(unsigned int idx) const {
  if (idx >= _size)
    return false;
  return 0 == _data[idx];
}

void
AnytoneCodeplug::InvertedBytemapElement::setEncoded(unsigned int idx, bool enable) {
  if (idx >= _size)
    return;
  _data[idx] = enable ? 0x00 : 0xff;
}

void
AnytoneCodeplug::InvertedBytemapElement::enableFirst(unsigned int n) {
  memset(_data, 0x00, n);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::ChannelElement
 * ********************************************************************************************* */
AnytoneCodeplug::ChannelElement::ChannelElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::ChannelElement::ChannelElement(uint8_t *ptr)
  : Element(ptr, ChannelElement::size())
{
  // pass...
}

AnytoneCodeplug::ChannelElement::~ChannelElement() {
  // pass...
}

void
AnytoneCodeplug::ChannelElement::clear() {
  setRXFrequency(0);
  setTXOffset(0);
  setMode(Mode::Analog);
  setPower(Channel::Power::Low);
  setBandwidth(FMChannel::Bandwidth::Narrow);
  setRXTone(Signaling::SIGNALING_NONE);
  setTXTone(Signaling::SIGNALING_NONE);
  setBit(0x0008, 5, false); // Unused set to 0
}

unsigned
AnytoneCodeplug::ChannelElement::rxFrequency() const {
  return ((unsigned)getBCD8_be(0x0000))*10;
}
void
AnytoneCodeplug::ChannelElement::setRXFrequency(unsigned hz) {
  setBCD8_be(0x0000, hz/10);
}

unsigned
AnytoneCodeplug::ChannelElement::txOffset() const {
  return ((unsigned)getBCD8_be(0x0004))*10;
}
void
AnytoneCodeplug::ChannelElement::setTXOffset(unsigned hz) {
  setBCD8_be(0x0004, hz/10);
}

unsigned
AnytoneCodeplug::ChannelElement::txFrequency() const {
  unsigned rx = rxFrequency(), off = txOffset();
  if (RepeaterMode::Simplex == repeaterMode())
    return rx;
  else if (RepeaterMode::Positive == repeaterMode())
    return rx+off;
  return rx-off;
}
void
AnytoneCodeplug::ChannelElement::setTXFrequency(unsigned hz) {
  unsigned rx = rxFrequency();
  if (rx == hz) {
    setTXOffset(0); setRepeaterMode(RepeaterMode::Simplex);
  } else if (rx < hz) {
    setTXOffset(hz-rx); setRepeaterMode(RepeaterMode::Positive);
  } else {
    setTXOffset(rx-hz); setRepeaterMode(RepeaterMode::Negative);
  }
}

AnytoneCodeplug::ChannelElement::Mode
AnytoneCodeplug::ChannelElement::mode() const {
  return (Mode) getUInt2(0x0008, 0);
}
void
AnytoneCodeplug::ChannelElement::setMode(Mode mode) {
  setUInt2(0x0008, 0, (unsigned)mode);
}

Channel::Power
AnytoneCodeplug::ChannelElement::power() const {
  switch ((Power)getUInt2(0x0008, 2)) {
  case POWER_LOW: return Channel::Power::Low;
  case POWER_MIDDLE: return Channel::Power::Mid;
  case POWER_HIGH: return Channel::Power::High;
  case POWER_TURBO: return Channel::Power::Max;
  }
  return Channel::Power::Low;
}
void
AnytoneCodeplug::ChannelElement::setPower(Channel::Power power) {
  switch (power) {
  case Channel::Power::Min:
  case Channel::Power::Low:
    setUInt2(0x0008, 2, (unsigned)POWER_LOW);
    break;
  case Channel::Power::Mid:
    setUInt2(0x0008, 2, (unsigned)POWER_MIDDLE);
    break;
  case Channel::Power::High:
    setUInt2(0x0008, 2, (unsigned)POWER_HIGH);
    break;
  case Channel::Power::Max:
    setUInt2(0x0008, 2, (unsigned)POWER_TURBO);
    break;
  }
}

FMChannel::Bandwidth
AnytoneCodeplug::ChannelElement::bandwidth() const {
  if (getBit(0x0008, 4))
    return FMChannel::Bandwidth::Wide;
  return FMChannel::Bandwidth::Narrow;
}
void
AnytoneCodeplug::ChannelElement::setBandwidth(FMChannel::Bandwidth bw) {
  switch (bw) {
  case FMChannel::Bandwidth::Narrow: setBit(0x0008, 4, false); break;
  case FMChannel::Bandwidth::Wide: setBit(0x0008, 4, true); break;
  }
}

AnytoneCodeplug::ChannelElement::RepeaterMode
AnytoneCodeplug::ChannelElement::repeaterMode() const {
  return (RepeaterMode)getUInt2(0x0008, 6);
}
void
AnytoneCodeplug::ChannelElement::setRepeaterMode(RepeaterMode mode) {
  setUInt2(0x0008, 6, (unsigned)mode);
}

AnytoneCodeplug::ChannelElement::SignalingMode
AnytoneCodeplug::ChannelElement::rxSignalingMode() const {
  return (SignalingMode)getUInt2(0x0009, 0);
}
void
AnytoneCodeplug::ChannelElement::setRXSignalingMode(SignalingMode mode) {
  setUInt2(0x0009, 0, (unsigned)mode);
}

Signaling::Code
AnytoneCodeplug::ChannelElement::rxTone() const {
  if (SignalingMode::None == rxSignalingMode())
    return Signaling::SIGNALING_NONE;
  else if (SignalingMode::CTCSS == rxSignalingMode())
    return rxCTCSS();
  else if (SignalingMode::DCS == rxSignalingMode())
    return rxDCS();
  return Signaling::SIGNALING_NONE;
}
void
AnytoneCodeplug::ChannelElement::setRXTone(Code code) {
  if (Signaling::SIGNALING_NONE == code) {
    setRXSignalingMode(SignalingMode::None);
  } else if (Signaling::isCTCSS(code)) {
    setRXSignalingMode(SignalingMode::CTCSS);
    setRXCTCSS(code);
  } else if (Signaling::isDCSInverted(code) || Signaling::isDCSNormal(code)) {
    setRXSignalingMode(SignalingMode::DCS);
    setRXDCS(code);
  }
}

AnytoneCodeplug::ChannelElement::SignalingMode
AnytoneCodeplug::ChannelElement::txSignalingMode() const {
  return (SignalingMode)getUInt2(0x0009, 2);
}
void
AnytoneCodeplug::ChannelElement::setTXSignalingMode(SignalingMode mode) {
  setUInt2(0x0009, 2, (unsigned)mode);
}

Signaling::Code
AnytoneCodeplug::ChannelElement::txTone() const {
  if (SignalingMode::None == txSignalingMode())
    return Signaling::SIGNALING_NONE;
  else if (SignalingMode::CTCSS == txSignalingMode())
    return txCTCSS();
  else if (SignalingMode::DCS == txSignalingMode())
    return txDCS();
  return Signaling::SIGNALING_NONE;
}
void
AnytoneCodeplug::ChannelElement::setTXTone(Code code) {
  if (Signaling::SIGNALING_NONE == code) {
    setTXSignalingMode(SignalingMode::None);
  } else if (Signaling::isCTCSS(code)) {
    setTXSignalingMode(SignalingMode::CTCSS);
    setTXCTCSS(code);
  } else if (Signaling::isDCSInverted(code) || Signaling::isDCSNormal(code)) {
    setTXSignalingMode(SignalingMode::DCS);
    setTXDCS(code);
  }
}

bool
AnytoneCodeplug::ChannelElement::ctcssPhaseReversal() const {
  return getBit(0x0009, 4);
}
void
AnytoneCodeplug::ChannelElement::enableCTCSSPhaseReversal(bool enable) {
  setBit(0x0009, 4, enable);
}
bool
AnytoneCodeplug::ChannelElement::rxOnly() const {
  return getBit(0x0009, 5);
}
void
AnytoneCodeplug::ChannelElement::enableRXOnly(bool enable) {
  setBit(0x0009, 5, enable);
}
bool
AnytoneCodeplug::ChannelElement::callConfirm() const {
  return getBit(0x0009, 6);
}
void
AnytoneCodeplug::ChannelElement::enableCallConfirm(bool enable) {
  setBit(0x0009, 6, enable);
}
bool
AnytoneCodeplug::ChannelElement::talkaround() const {
  return getBit(0x0009, 7);
}
void
AnytoneCodeplug::ChannelElement::enableTalkaround(bool enable) {
  setBit(0x0009, 7, enable);
}

bool
AnytoneCodeplug::ChannelElement::txCTCSSIsCustom() const {
  return CUSTOM_CTCSS_TONE == getUInt8(0x000a);
}
Signaling::Code
AnytoneCodeplug::ChannelElement::txCTCSS() const {
  return ctcss_num2code(getUInt8(0x000a));
}
void
AnytoneCodeplug::ChannelElement::setTXCTCSS(Code tone) {
  setUInt8(0x000a, ctcss_code2num(tone));
}
void
AnytoneCodeplug::ChannelElement::enableTXCustomCTCSS() {
  setUInt8(0x000a, CUSTOM_CTCSS_TONE);
}
bool
AnytoneCodeplug::ChannelElement::rxCTCSSIsCustom() const {
  return CUSTOM_CTCSS_TONE == getUInt8(0x000b);
}
Signaling::Code
AnytoneCodeplug::ChannelElement::rxCTCSS() const {
  return ctcss_num2code(getUInt8(0x000b));
}
void
AnytoneCodeplug::ChannelElement::setRXCTCSS(Code tone) {
  setUInt8(0x000b, ctcss_code2num(tone));
}
void
AnytoneCodeplug::ChannelElement::enableRXCustomCTCSS() {
  setUInt8(0x000b, CUSTOM_CTCSS_TONE);
}

Signaling::Code
AnytoneCodeplug::ChannelElement::txDCS() const {
  uint16_t code = getUInt16_le(0x000c);
  if (512 > code)
    return Signaling::fromDCSNumber(dec_to_oct(code), false);
  return Signaling::fromDCSNumber(dec_to_oct(code-512), true);
}
void
AnytoneCodeplug::ChannelElement::setTXDCS(Code code) {
  if (Signaling::isDCSNormal(code))
    setUInt16_le(0x000c, oct_to_dec(Signaling::toDCSNumber(code)));
  else if (Signaling::isDCSInverted(code))
    setUInt16_le(0x000c, oct_to_dec(Signaling::toDCSNumber(code))+512);
  else
    setUInt16_le(0x000c, 0);
}

Signaling::Code
AnytoneCodeplug::ChannelElement::rxDCS() const {
  uint16_t code = getUInt16_le(0x000e);
  if (512 > code)
    return Signaling::fromDCSNumber(dec_to_oct(code), false);
  return Signaling::fromDCSNumber(dec_to_oct(code-512), true);
}
void
AnytoneCodeplug::ChannelElement::setRXDCS(Code code) {
  if (Signaling::isDCSNormal(code))
    setUInt16_le(0x000e, oct_to_dec(Signaling::toDCSNumber(code)));
  else if (Signaling::isDCSInverted(code))
    setUInt16_le(0x000e, oct_to_dec(Signaling::toDCSNumber(code))+512);
  else
    setUInt16_le(0x000e, 0);
}

double
AnytoneCodeplug::ChannelElement::customCTCSSFrequency() const {
  return ((double) getUInt16_le(0x0010))/10;
}
void
AnytoneCodeplug::ChannelElement::setCustomCTCSSFrequency(double hz) {
  setUInt16_le(0x0010, hz*10);
}

unsigned
AnytoneCodeplug::ChannelElement::twoToneDecodeIndex() const {
  return getUInt16_le(0x0012);
}
void
AnytoneCodeplug::ChannelElement::setTwoToneDecodeIndex(unsigned idx) {
  setUInt16_le(0x0012, idx);
}

unsigned
AnytoneCodeplug::ChannelElement::contactIndex() const {
  return getUInt32_le(0x0014);
}
void
AnytoneCodeplug::ChannelElement::setContactIndex(unsigned idx) {
  return setUInt32_le(0x0014, idx);
}

unsigned
AnytoneCodeplug::ChannelElement::radioIDIndex() const {
  return getUInt8(0x0018);
}
void
AnytoneCodeplug::ChannelElement::setRadioIDIndex(unsigned idx) {
  return setUInt8(0x0018, idx);
}

AnytoneFMChannelExtension::SquelchMode
AnytoneCodeplug::ChannelElement::squelchMode() const {
  return (AnytoneFMChannelExtension::SquelchMode)getUInt3(0x0019, 4);
}
void
AnytoneCodeplug::ChannelElement::setSquelchMode(AnytoneFMChannelExtension::SquelchMode mode) {
  setUInt3(0x0019, 4, (unsigned)mode);
}

AnytoneCodeplug::ChannelElement::Admit
AnytoneCodeplug::ChannelElement::admit() const {
  return (Admit)getUInt2(0x001a, 0);
}
void
AnytoneCodeplug::ChannelElement::setAdmit(Admit admit) {
  setUInt2(0x001a, 0, (unsigned)admit);
}

AnytoneCodeplug::ChannelElement::OptSignaling
AnytoneCodeplug::ChannelElement::optionalSignaling() const {
  return (OptSignaling)getUInt2(0x001a, 4);
}
void
AnytoneCodeplug::ChannelElement::setOptionalSignaling(OptSignaling sig) {
  setUInt2(0x001a, 4, (unsigned)sig);
}

bool
AnytoneCodeplug::ChannelElement::hasScanListIndex() const {
  return 0xff != scanListIndex();
}
unsigned
AnytoneCodeplug::ChannelElement::scanListIndex() const {
  return getUInt8(0x001b);
}
void
AnytoneCodeplug::ChannelElement::setScanListIndex(unsigned idx) {
  setUInt8(0x001b, idx);
}
void
AnytoneCodeplug::ChannelElement::clearScanListIndex() {
  setScanListIndex(0xff);
}

bool
AnytoneCodeplug::ChannelElement::hasGroupListIndex() const {
  return 0xff != groupListIndex();
}
unsigned
AnytoneCodeplug::ChannelElement::groupListIndex() const {
  return getUInt8(0x001c);
}
void
AnytoneCodeplug::ChannelElement::setGroupListIndex(unsigned idx) {
  setUInt8(0x001c, idx);
}
void
AnytoneCodeplug::ChannelElement::clearGroupListIndex() {
  setGroupListIndex(0xff);
}

unsigned
AnytoneCodeplug::ChannelElement::twoToneIDIndex() const {
  return getUInt8(0x001d);
}
void
AnytoneCodeplug::ChannelElement::setTwoToneIDIndex(unsigned idx) {
  setUInt8(0x001d, idx);
}
unsigned
AnytoneCodeplug::ChannelElement::fiveToneIDIndex() const {
  return getUInt8(0x001e);
}
void
AnytoneCodeplug::ChannelElement::setFiveToneIDIndex(unsigned idx) {
  setUInt8(0x001e, idx);
}
unsigned
AnytoneCodeplug::ChannelElement::dtmfIDIndex() const {
  return getUInt8(0x001f);
}
void
AnytoneCodeplug::ChannelElement::setDTMFIDIndex(unsigned idx) {
  setUInt8(0x001f, idx);
}

unsigned
AnytoneCodeplug::ChannelElement::colorCode() const {
  return getUInt8(0x0020);
}
void
AnytoneCodeplug::ChannelElement::setColorCode(unsigned code) {
  setUInt8(0x0020, code);
}

DMRChannel::TimeSlot
AnytoneCodeplug::ChannelElement::timeSlot() const {
  if (false == getBit(0x0021, 0))
    return DMRChannel::TimeSlot::TS1;
  return DMRChannel::TimeSlot::TS2;
}
void
AnytoneCodeplug::ChannelElement::setTimeSlot(DMRChannel::TimeSlot ts) {
  if (DMRChannel::TimeSlot::TS1 == ts)
    setBit(0x0021, 0, false);
  else
    setBit(0x0021, 0, true);
}

bool
AnytoneCodeplug::ChannelElement::smsConfirm() const {
  return getBit(0x0021, 1);
}
void
AnytoneCodeplug::ChannelElement::enableSMSConfirm(bool enable) {
  setBit(0x0021, 1, enable);
}
bool
AnytoneCodeplug::ChannelElement::simplexTDMA() const {
  return getBit(0x0021, 2);
}
void
AnytoneCodeplug::ChannelElement::enableSimplexTDMA(bool enable) {
  setBit(0x0021, 2, enable);
}
bool
AnytoneCodeplug::ChannelElement::adaptiveTDMA() const {
  return getBit(0x0021, 4);
}
void
AnytoneCodeplug::ChannelElement::enableAdaptiveTDMA(bool enable) {
  setBit(0x0021, 4, enable);
}
bool
AnytoneCodeplug::ChannelElement::rxAPRS() const {
  return getBit(0x0021, 5);
}
void
AnytoneCodeplug::ChannelElement::enableRXAPRS(bool enable) {
  setBit(0x0021, 5, enable);
}
bool
AnytoneCodeplug::ChannelElement::enhancedEncryption() const {
  return getBit(0x0021, 6);
}
void
AnytoneCodeplug::ChannelElement::enableEnhancedEncryption(bool enable) {
  setBit(0x0021, 6, enable);
}
bool
AnytoneCodeplug::ChannelElement::loneWorker() const {
  return getBit(0x0021, 7);
}
void
AnytoneCodeplug::ChannelElement::enableLoneWorker(bool enable) {
  setBit(0x0021, 7, enable);
}

bool
AnytoneCodeplug::ChannelElement::hasEncryptionKeyIndex() const {
  return 0xff != encryptionKeyIndex();
}
unsigned
AnytoneCodeplug::ChannelElement::encryptionKeyIndex() const {
  return getUInt8(0x0022);
}
void
AnytoneCodeplug::ChannelElement::setEncryptionKeyIndex(unsigned idx) {
  setUInt8(0x0022, idx);
}
void
AnytoneCodeplug::ChannelElement::clearEncryptionKeyIndex() {
  setEncryptionKeyIndex(0xff);
}

QString
AnytoneCodeplug::ChannelElement::name() const {
  return readASCII(0x0023, 16, 0x00);
}
void
AnytoneCodeplug::ChannelElement::setName(const QString &name) {
  writeASCII(0x0023, name, 16, 0x00);
}


Channel *
AnytoneCodeplug::ChannelElement::toChannelObj(Context &ctx) const {
  Q_UNUSED(ctx)

  Channel *ch;
  AnytoneChannelExtension *ch_ext = nullptr;

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

    // Create extension
    AnytoneFMChannelExtension *ext = new AnytoneFMChannelExtension(); ch_ext = ext;
    ach->setAnytoneChannelExtension(ext);
    ext->enableReverseBurst(ctcssPhaseReversal());
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

    // Create extension
    AnytoneDMRChannelExtension *ext = new AnytoneDMRChannelExtension(); ch_ext = ext;
    dch->setAnytoneChannelExtension(ext);
    ext->enableCallConfirm(callConfirm());
    ext->enableSMSConfirm(smsConfirm());
    ext->enableSimplexTDMA(simplexTDMA());
    ext->enableAdaptiveTDMA(adaptiveTDMA());
    ext->enableLoneWorker(loneWorker());
    // Done
    ch = dch;
  } else {
    logError() << "Cannot create channel '" << name()
               << "': Channel type " << (unsigned)mode() << "not supported.";
    return nullptr;
  }

  ch->setName(name());
  ch->setRXFrequency(rxFrequency()/1e6);
  ch->setTXFrequency(txFrequency()/1e6);
  ch->setPower(power());
  ch->setRXOnly(rxOnly());

  // No per channel vox & tot setting
  ch->setVOXDefault();
  ch->setDefaultTimeout();

  // Apply common channel extension settings
  if (nullptr != ch_ext) {
    ch_ext->enableTalkaround(talkaround());
  }

  return ch;
}

bool
AnytoneCodeplug::ChannelElement::linkChannelObj(Channel *c, Context &ctx) const {
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
    dc->setTXContactObj(ctx.get<DMRContact>(contactIndex()));

    // Set if RX group list is set
    if (hasGroupListIndex() && ctx.has<RXGroupList>(groupListIndex()))
      dc->setGroupListObj(ctx.get<RXGroupList>(groupListIndex()));

    // Link radio ID
    DMRRadioID *rid = ctx.get<DMRRadioID>(radioIDIndex());
    if (rid == ctx.config()->radioIDs()->defaultId())
      dc->setRadioIdObj(DefaultRadioID::get());
    else
      dc->setRadioIdObj(rid);
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
AnytoneCodeplug::ChannelElement::fromChannelObj(const Channel *c, Context &ctx) {
  // Clear channel element
  clear();

  // set channel name
  setName(c->name());
  // set rx and tx frequencies
  setRXFrequency(c->rxFrequency()*1e6);
  setTXFrequency(c->txFrequency()*1e6);
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
    if (Signaling::SIGNALING_NONE != ac->rxTone())
      setSquelchMode(AnytoneFMChannelExtension::SquelchMode::SubTone);
    else
      setSquelchMode(AnytoneFMChannelExtension::SquelchMode::Carrier);
    // set bandwidth
    setBandwidth(ac->bandwidth());
    // Handle extension
    if (AnytoneFMChannelExtension *ext = ac->anytoneChannelExtension()) {
      // Apply common settings
      enableTalkaround(ext->talkaround());
      // Apply FM settings
      enableCTCSSPhaseReversal(ext->reverseBurst());
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
    if (nullptr == dc->txContactObj())
      setContactIndex(0);
    else
      setContactIndex(ctx.index(dc->txContactObj()));
    // link RX group list
    if (nullptr == dc->groupListObj())
      clearGroupListIndex();
    else
      setGroupListIndex(ctx.index(dc->groupListObj()));
    // Set radio ID
    if ((nullptr == dc->radioIdObj()) || (DefaultRadioID::get() == dc->radioIdObj())) {
      if (nullptr == ctx.config()->radioIDs()->defaultId()) {
        logWarn() << "No default radio ID set: using index 0.";
        setRadioIDIndex(0);
      } else {
        setRadioIDIndex(ctx.index(ctx.config()->radioIDs()->defaultId()));
      }
    } else {
      setRadioIDIndex(ctx.index(dc->radioIdObj()));
    }
    // Handle extension
    if (AnytoneDMRChannelExtension *ext = dc->anytoneChannelExtension()) {
      // Apply common settings
      enableTalkaround(ext->talkaround());
      // Apply DMR settings
      enableCallConfirm(ext->callConfirm());
      enableSMSConfirm(ext->smsConfirm());
      enableSimplexTDMA(ext->simplexTDMA());
      enableAdaptiveTDMA(ext->adaptiveTDMA());
      enableLoneWorker(ext->loneWorker());
    }
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::ChannelBitmapElement
 * ********************************************************************************************* */
AnytoneCodeplug::ChannelBitmapElement::ChannelBitmapElement(uint8_t *ptr, size_t size)
  : BitmapElement(ptr, size)
{
  // pass...
}

AnytoneCodeplug::ChannelBitmapElement::ChannelBitmapElement(uint8_t *ptr)
  : BitmapElement(ptr, ChannelBitmapElement::size())
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::ContactElement
 * ********************************************************************************************* */
AnytoneCodeplug::ContactElement::ContactElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::ContactElement::ContactElement(uint8_t *ptr)
  : Element(ptr, ContactElement::size())
{
  // pass...
}

AnytoneCodeplug::ContactElement::~ContactElement() {
  // pass...
}

void
AnytoneCodeplug::ContactElement::clear() {
  memset(_data, 0, _size);
}

bool
AnytoneCodeplug::ContactElement::isValid() const {
  return !name().isEmpty();
}

DMRContact::Type
AnytoneCodeplug::ContactElement::type() const {
  switch (getUInt8(0x0000)) {
  case 0: return DMRContact::PrivateCall;
  case 1: return DMRContact::GroupCall;
  case 2: return DMRContact::AllCall;
  }
  return DMRContact::PrivateCall;
}
void
AnytoneCodeplug::ContactElement::setType(DMRContact::Type type) {
  switch (type) {
  case DMRContact::PrivateCall: setUInt8(0x0000, 0); break;
  case DMRContact::GroupCall: setUInt8(0x0000, 1); break;
  case DMRContact::AllCall: setUInt8(0x0000, 2); break;
  }
}

QString
AnytoneCodeplug::ContactElement::name() const {
  return readASCII(0x0001, 16, 0x00);
}
void
AnytoneCodeplug::ContactElement::setName(const QString &name) {
  writeASCII(0x0001, name, 16, 0x00);
}

unsigned
AnytoneCodeplug::ContactElement::number() const {
  return getBCD8_be(0x0023);
}
void
AnytoneCodeplug::ContactElement::setNumber(unsigned number) {
  setBCD8_be(0x0023, number);
}

AnytoneContactExtension::AlertType
AnytoneCodeplug::ContactElement::alertType() const {
  switch (getUInt8(0x0027)) {
  case (uint8_t)AnytoneContactExtension::AlertType::None:
    return AnytoneContactExtension::AlertType::None;
  case (uint8_t)AnytoneContactExtension::AlertType::Ring:
    return AnytoneContactExtension::AlertType::Ring;
  case (uint8_t)AnytoneContactExtension::AlertType::Online:
    return AnytoneContactExtension::AlertType::Online;
  default:
    break;
  }
  logWarn() << "Unknown value " << getUInt8(0x0027)
            << " for alert type of AnyTone contact element. Maybe the codeplug implementation is"
               " outdated. Consider reporting it at https://github.com/hmatuschek/qdmr.";
  return AnytoneContactExtension::AlertType::None;
}
void
AnytoneCodeplug::ContactElement::setAlertType(AnytoneContactExtension::AlertType type) {
  setUInt8(0x0027, (unsigned)type);
}

DMRContact *
AnytoneCodeplug::ContactElement::toContactObj(Context &ctx) const {
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
AnytoneCodeplug::ContactElement::fromContactObj(const DMRContact *contact, Context &ctx) {
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
 * Implementation of AnytoneCodeplug::ContactBitmapElement
 * ********************************************************************************************* */
AnytoneCodeplug::ContactBitmapElement::ContactBitmapElement(uint8_t *ptr, size_t size)
  : InvertedBitmapElement(ptr, size)
{
  // pass...
}

AnytoneCodeplug::ContactBitmapElement::ContactBitmapElement(uint8_t *ptr)
  : InvertedBitmapElement(ptr, ContactBitmapElement::size())
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::DTMFContactElement
 * ********************************************************************************************* */
AnytoneCodeplug::DTMFContactElement::DTMFContactElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::DTMFContactElement::DTMFContactElement(uint8_t *ptr)
  : Element(ptr, DTMFContactElement::size())
{
  // pass...
}

AnytoneCodeplug::DTMFContactElement::~DTMFContactElement() {
  // pass...
}

void
AnytoneCodeplug::DTMFContactElement::clear() {
  memset(_data, 0, _size);
}

QString
AnytoneCodeplug::DTMFContactElement::number() const {
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
AnytoneCodeplug::DTMFContactElement::setNumber(const QString &number) {
  if (! validDTMFNumber(number))
    return;
  memset(_data+Offset::digits(), 0, Limit::digitCount()/2);
  unsigned int n = std::min((unsigned int)number.length(), Limit::digitCount());
  setUInt8(Offset::digits(), n);
  for (unsigned int i=0; i<n; i++) {
    if (0 == (i%2))
      _data[Offset::digits() + i/2] |= (_anytone_bin_dtmf_tab.indexOf(number[i].toLatin1())<<4);
    else
      _data[Offset::digits() + i/2] |= (_anytone_bin_dtmf_tab.indexOf(number[i].toLatin1())<<0);
  }
}

QString
AnytoneCodeplug::DTMFContactElement::name() const {
  return readASCII(Offset::name(), Limit::nameLength(), 0x00);
}
void
AnytoneCodeplug::DTMFContactElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::nameLength(), 0x00);
}

DTMFContact *
AnytoneCodeplug::DTMFContactElement::toContact() const {
  return new DTMFContact(name(), number());
}

bool
AnytoneCodeplug::DTMFContactElement::fromContact(const DTMFContact *contact) {
  setNumber(contact->number());
  setName(contact->name());
  return true;
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::DTMFContactBytemapElement
 * ********************************************************************************************* */
AnytoneCodeplug::DTMFContactBytemapElement::DTMFContactBytemapElement(uint8_t *ptr, size_t size)
  : InvertedBytemapElement(ptr, size)
{
  // pass...
}

AnytoneCodeplug::DTMFContactBytemapElement::DTMFContactBytemapElement(uint8_t *ptr)
  : InvertedBytemapElement(ptr, DTMFContactBytemapElement::size())
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::GroupListElement
 * ********************************************************************************************* */
AnytoneCodeplug::GroupListElement::GroupListElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::GroupListElement::GroupListElement(uint8_t *ptr)
  : Element(ptr, GroupListElement::size())
{
  // pass...
}

void
AnytoneCodeplug::GroupListElement::clear() {
  memset(_data, 0x00, _size);
  // set member indices to 0xffffffff
  memset(_data, 0xff, 4*64);
}

bool
AnytoneCodeplug::GroupListElement::isValid() const {
  return !name().isEmpty();
}

QString
AnytoneCodeplug::GroupListElement::name() const {
  return readASCII(0x0100, 16, 0x00);
}
void
AnytoneCodeplug::GroupListElement::setName(const QString &name) {
  writeASCII(0x0100, name, 16, 0x00);
}

bool
AnytoneCodeplug::GroupListElement::hasMemberIndex(unsigned n) const {
  return 0xffffffff != memberIndex(n);
}
unsigned
AnytoneCodeplug::GroupListElement::memberIndex(unsigned n) const {
  return getUInt32_le(0x0000 + 4*n);
}
void
AnytoneCodeplug::GroupListElement::setMemberIndex(unsigned n, unsigned idx) {
  setUInt32_le(0x0000 + 4*n, idx);
}
void
AnytoneCodeplug::GroupListElement::clearMemberIndex(unsigned n) {
  setMemberIndex(n, 0xffffffff);
}

RXGroupList *
AnytoneCodeplug::GroupListElement::toGroupListObj() const {
  return new RXGroupList(name());
}

bool
AnytoneCodeplug::GroupListElement::linkGroupList(RXGroupList *lst, Context &ctx) const {
  for (uint8_t i=0; i<64; i++) {
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
AnytoneCodeplug::GroupListElement::fromGroupListObj(const RXGroupList *lst, Context &ctx) {
  clear();

  // set name of group list
  setName(lst->name());

  int j=0;
  // set members
  for (uint8_t i=0; i<64; i++) {
    // Skip non-private-call entries
    while((lst->count() > j) && (DMRContact::GroupCall != lst->contact(j)->type())) {
      logWarn() << "Contact '" << lst->contact(i)->name() << "' in group list '" << lst->name()
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
 * Implementation of AnytoneCodeplug::GroupListBitmapElement
 * ********************************************************************************************* */
AnytoneCodeplug::GroupListBitmapElement::GroupListBitmapElement(uint8_t *ptr, size_t size)
  : BitmapElement(ptr, size)
{
  // pass...
}

AnytoneCodeplug::GroupListBitmapElement::GroupListBitmapElement(uint8_t *ptr)
  : BitmapElement(ptr, GroupListBitmapElement::size())
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::ScanListElement
 * ********************************************************************************************* */
AnytoneCodeplug::ScanListElement::ScanListElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::ScanListElement::ScanListElement(uint8_t *ptr)
  : Element(ptr, ScanListElement::size())
{
  // pass...
}

void
AnytoneCodeplug::ScanListElement::clear() {
  memset(_data, 0, _size);
  setPriorityChannels(PriChannel::Off);
  clearPrimaryChannel();
  clearSecondaryChannel();
  setLookBackTimeA(150);
  setLookBackTimeB(250);
  setDropOutDelay(290);
  setDwellTime(290);
  setRevertChannel(RevertChannel::Selected);
  // clear members
  memset(_data+0x0020, 0xff, 2*50);
}

AnytoneCodeplug::ScanListElement::PriChannel
AnytoneCodeplug::ScanListElement::priorityChannels() const {
  return (PriChannel) getUInt8(0x0001);
}
void
AnytoneCodeplug::ScanListElement::setPriorityChannels(PriChannel sel) {
  setUInt8(0x0001, (unsigned)sel);
}

bool
AnytoneCodeplug::ScanListElement::hasPrimary() const {
  return 0xffff != getUInt16_le(0x0002);
}
bool
AnytoneCodeplug::ScanListElement::primaryIsSelected() const {
  return 0 == getUInt16_le(0x0002);
}
unsigned
AnytoneCodeplug::ScanListElement::primary() const {
  return ((unsigned)getUInt16_le(0x0002))-1;
}
void
AnytoneCodeplug::ScanListElement::setPrimary(unsigned idx) {
  setUInt16_le(0x0002, idx+1);
}
void
AnytoneCodeplug::ScanListElement::setPrimarySelected() {
  setUInt16_le(0x0002, 0);
}
void
AnytoneCodeplug::ScanListElement::clearPrimaryChannel() {
  setUInt16_le(0x0002, 0xffff);
}

bool
AnytoneCodeplug::ScanListElement::hasSecondary() const {
  return 0xffff != getUInt16_le(0x0004);
}
bool
AnytoneCodeplug::ScanListElement::secondaryIsSelected() const {
  return 0 == getUInt16_le(0x0004);
}
unsigned
AnytoneCodeplug::ScanListElement::secondary() const {
  return ((unsigned)getUInt16_le(0x0002))-1;
}
void
AnytoneCodeplug::ScanListElement::setSecondary(unsigned idx) {
  setUInt16_le(0x0004, idx+1);
}
void
AnytoneCodeplug::ScanListElement::setSecondarySelected() {
  setUInt16_le(0x0004, 0);
}
void
AnytoneCodeplug::ScanListElement::clearSecondaryChannel() {
  setUInt16_le(0x0004, 0xffff);
}

unsigned
AnytoneCodeplug::ScanListElement::lookBackTimeA() const {
  return ((unsigned)getUInt16_le(0x0006))*10;
}
void
AnytoneCodeplug::ScanListElement::setLookBackTimeA(unsigned sec) {
  setUInt16_le(0x0006, sec/10);
}
unsigned
AnytoneCodeplug::ScanListElement::lookBackTimeB() const {
  return ((unsigned)getUInt16_le(0x0008))*10;
}
void
AnytoneCodeplug::ScanListElement::setLookBackTimeB(unsigned sec) {
  setUInt16_le(0x0008, sec/10);
}
unsigned
AnytoneCodeplug::ScanListElement::dropOutDelay() const {
  return ((unsigned)getUInt16_le(0x000a))*10;
}
void
AnytoneCodeplug::ScanListElement::setDropOutDelay(unsigned sec) {
  setUInt16_le(0x000a, sec/10);
}
unsigned
AnytoneCodeplug::ScanListElement::dwellTime() const {
  return ((unsigned)getUInt16_le(0x000c))*10;
}
void
AnytoneCodeplug::ScanListElement::setDwellTime(unsigned sec) {
  setUInt16_le(0x000c, sec/10);
}

AnytoneCodeplug::ScanListElement::RevertChannel
AnytoneCodeplug::ScanListElement::revertChannel() const {
  return (RevertChannel)getUInt8(0x000e);
}
void
AnytoneCodeplug::ScanListElement::setRevertChannel(RevertChannel type) {
  setUInt8(0x000e, (unsigned)type);
}

QString
AnytoneCodeplug::ScanListElement::name() const {
  return readASCII(0x000f, 16, 0x00);
}
void
AnytoneCodeplug::ScanListElement::setName(const QString &name) {
  writeASCII(0x000f, name, 16, 0x00);
}

bool
AnytoneCodeplug::ScanListElement::hasMemberIndex(unsigned n) const {
  return 0xffff != memberIndex(n);
}
unsigned
AnytoneCodeplug::ScanListElement::memberIndex(unsigned n) const {
  return getUInt16_le(0x0020+2*n);
}
void
AnytoneCodeplug::ScanListElement::setMemberIndex(unsigned n, unsigned idx) {
  setUInt16_le(0x0020+2*n, idx);
}
void
AnytoneCodeplug::ScanListElement::clearMemberIndex(unsigned n) {
  setMemberIndex(n, 0xffff);
}

ScanList *
AnytoneCodeplug::ScanListElement::toScanListObj() const {
  return new ScanList(name());
}

bool
AnytoneCodeplug::ScanListElement::linkScanListObj(ScanList *lst, Context &ctx) const {
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

  for (uint16_t i=0; i<50; i++) {
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
AnytoneCodeplug::ScanListElement::fromScanListObj(ScanList *lst, Context &ctx) {
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

  for (int i=0; i<std::min(50, lst->count()); i++) {
    if (SelectedChannel::get() == lst->channel(i))
      continue;
    setMemberIndex(i, ctx.index(lst->channel(i)));
  }

  return false;
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::ScanListBitmapElement
 * ********************************************************************************************* */
AnytoneCodeplug::ScanListBitmapElement::ScanListBitmapElement(uint8_t *ptr, size_t size)
  : BitmapElement(ptr, size)
{
  // pass...
}

AnytoneCodeplug::ScanListBitmapElement::ScanListBitmapElement(uint8_t *ptr)
  : BitmapElement(ptr, ScanListBitmapElement::size())
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::RadioIDElement
 * ********************************************************************************************* */
AnytoneCodeplug::RadioIDElement::RadioIDElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::RadioIDElement::RadioIDElement(uint8_t *ptr)
  : Element(ptr, RadioIDElement::size())
{
  // pass...
}

void
AnytoneCodeplug::RadioIDElement::clear() {
  memset(_data, 0x00, _size);
}

unsigned AnytoneCodeplug::RadioIDElement::number() const {
  return getBCD8_be(0x0000);
}
void
AnytoneCodeplug::RadioIDElement::setNumber(unsigned number) {
  setBCD8_be(0x0000, number);
}

QString
AnytoneCodeplug::RadioIDElement::name() const {
  return readASCII(0x0005, 16, 0x00);
}
void
AnytoneCodeplug::RadioIDElement::setName(const QString &name) {
  writeASCII(0x0005, name, 16, 0x00);
}

DMRRadioID *
AnytoneCodeplug::RadioIDElement::toRadioID() const {
  return new DMRRadioID(name(), number());
}
bool
AnytoneCodeplug::RadioIDElement::fromRadioID(DMRRadioID *id) {
  setName(id->name());
  setNumber(id->number());
  return true;
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::RadioIDBitmapElement
 * ********************************************************************************************* */
AnytoneCodeplug::RadioIDBitmapElement::RadioIDBitmapElement(uint8_t *ptr, size_t size)
  : BitmapElement(ptr, size)
{
   // pass...
}

AnytoneCodeplug::RadioIDBitmapElement::RadioIDBitmapElement(uint8_t *ptr)
  : BitmapElement(ptr, RadioIDBitmapElement::size())
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::GeneralSettingsElement
 * ********************************************************************************************* */
AnytoneCodeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

void
AnytoneCodeplug::GeneralSettingsElement::clear() {
  memset(_data, 0, _size);
}

bool
AnytoneCodeplug::GeneralSettingsElement::displayFrequency() const {
  return 0!=getUInt8(Offset::displayMode());
}
void
AnytoneCodeplug::GeneralSettingsElement::enableDisplayFrequency(bool enable) {
  setUInt8(Offset::displayMode(), (enable ? 0x01 : 0x00));
}

bool
AnytoneCodeplug::GeneralSettingsElement::autoKeyLock() const {
  return 0!=getUInt8(Offset::autoKeyLock());
}
void
AnytoneCodeplug::GeneralSettingsElement::enableAutoKeyLock(bool enable) {
  setUInt8(Offset::autoKeyLock(), (enable ? 0x01 : 0x00));
}

Interval
AnytoneCodeplug::GeneralSettingsElement::autoShutdownDelay() const {
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
AnytoneCodeplug::GeneralSettingsElement::setAutoShutdownDelay(Interval intv) {
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

AnytoneBootSettingsExtension::BootDisplay
AnytoneCodeplug::GeneralSettingsElement::bootDisplay() const {
  return (AnytoneBootSettingsExtension::BootDisplay) getUInt8(Offset::bootDisplay());
}
void
AnytoneCodeplug::GeneralSettingsElement::setBootDisplay(AnytoneBootSettingsExtension::BootDisplay mode) {
  setUInt8(Offset::bootDisplay(), (unsigned)mode);
}

bool
AnytoneCodeplug::GeneralSettingsElement::bootPassword() const {
  return getUInt8(Offset::bootPassword());
}
void
AnytoneCodeplug::GeneralSettingsElement::enableBootPassword(bool enable) {
  setUInt8(Offset::bootPassword(), (enable ? 0x01 : 0x00));
}

unsigned
AnytoneCodeplug::GeneralSettingsElement::squelchLevelA() const {
  return getUInt8(Offset::squelchLevelA());
}
void
AnytoneCodeplug::GeneralSettingsElement::setSquelchLevelA(unsigned level) {
  setUInt8(Offset::squelchLevelA(), level);
}
unsigned AnytoneCodeplug::GeneralSettingsElement::squelchLevelB() const {
  return getUInt8(Offset::squelchLevelB());
}
void
AnytoneCodeplug::GeneralSettingsElement::setSquelchLevelB(unsigned level) {
  setUInt8(Offset::squelchLevelB(), level);
}

bool
AnytoneCodeplug::GeneralSettingsElement::fromConfig(const Flags &flags, Context &ctx) {
  // Set microphone gain
  // For the 868UV, this setting sets both, FM and DMR mic gain.
  // For all other devices, there is an additional FM mic gain setting.
  setDMRMicGain(ctx.config()->settings()->micLevel());

  // If auto-enable GPS is enabled
  if (flags.autoEnableGPS) {
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
  // Set default squelch level
  if (0 == ctx.config()->settings()->squelch()) {
    setSquelchLevelA(0);
    setSquelchLevelB(0);
  } else if (1 == ctx.config()->settings()->squelch()) {
    setSquelchLevelA(1);
    setSquelchLevelB(1);
  } else {
    setSquelchLevelA(ctx.config()->settings()->squelch()/2);
    setSquelchLevelB(ctx.config()->settings()->squelch()/2);
  }

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

    // Encode boot settings
    setBootDisplay(ext->bootSettings()->bootDisplay());
    enableBootPassword(ext->bootSettings()->bootPasswordEnabled());
    enableDefaultChannel(
          ext->bootSettings()->defaultChannelEnabled() &&
          (! ext->bootSettings()->zoneA()->isNull()) &&
          (! ext->bootSettings()->zoneB()->isNull()));

    if (defaultChannel()) {
      setDefaultZoneIndexA(ctx.index(ext->bootSettings()->zoneA()->as<Zone>()));
      if (ext->bootSettings()->channelA()->isNull() ||
          (! ext->bootSettings()->zoneA()->as<Zone>()->A()->has(
             ext->bootSettings()->channelA()->as<Channel>())))
        setDefaultChannelAToVFO();
      else
        setDefaultChannelAIndex(
              ctx.index(ext->bootSettings()->channelA()->as<Channel>()));

      setDefaultZoneIndexB(ctx.index(ext->bootSettings()->zoneA()->as<Zone>()));
      if (ext->bootSettings()->channelB()->isNull() ||
          (! ext->bootSettings()->zoneB()->as<Zone>()->A()->has(
             ext->bootSettings()->channelB()->as<Channel>())))
        setDefaultChannelBToVFO();
      else
        setDefaultChannelBIndex(ctx.index(ext->bootSettings()->channelB()->as<Channel>()));
    }

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

    // Encode tone settings
    enableKeyTone(ext->toneSettings()->keyToneEnabled());
    enableSMSAlert(ext->toneSettings()->smsAlertEnabled());
    enableCallAlert(ext->toneSettings()->callAlertEnabled());
    enableDMRTalkPermit(ext->toneSettings()->talkPermitDigitalEnabled());
    enableFMTalkPermit(ext->toneSettings()->talkPermitAnalogEnabled());
    enableDMRResetTone(ext->toneSettings()->digitalResetToneEnabled());
    enableIdleChannelTone(ext->toneSettings()->dmrIdleChannelToneEnabled());
    enableStartupTone(ext->toneSettings()->startupToneEnabled());
    setCallToneMelody(*(ext->toneSettings()->callMelody()));
    setIdleToneMelody(*(ext->toneSettings()->idleMelody()));
    setResetToneMelody(*(ext->toneSettings()->resetMelody()));

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
    setMaxSpeakerVolume(ext->audioSettings()->maxVolume());

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
    enableGPSUnitsImperial(AnytoneGPSSettingsExtension::Units::Archaic == ext->gpsSettings()->units());

    // Encode other settings
    enableKeepLastCaller(ext->keepLastCallerEnabled());
  } else if (! flags.updateCodePlug) {
    clearAutoRepeaterOffsetFrequencyIndexVHF();
    clearAutoRepeaterOffsetFrequencyIndexUHF();
  }

  return true;
}

bool
AnytoneCodeplug::GeneralSettingsElement::updateConfig(Context &ctx) {
  // get microphone gain
  ctx.config()->settings()->setMicLevel(dmrMicGain());
  // D868UV does not support speech synthesis?
  ctx.config()->settings()->enableSpeech(false);
  ctx.config()->settings()->setSquelch(std::max(squelchLevelA(), squelchLevelB())*2);

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

  // Store boot settings
  ext->bootSettings()->setBootDisplay(bootDisplay());
  ext->bootSettings()->enableBootPassword(bootPassword());
  ext->bootSettings()->enableDefaultChannel(this->defaultChannel());

  // Store key settings
  ext->keySettings()->setFuncKey1Short(funcKeyAShort());
  ext->keySettings()->setFuncKey1Long(funcKeyALong());
  ext->keySettings()->setFuncKey2Short(funcKeyBShort());
  ext->keySettings()->setFuncKey2Long(funcKeyBLong());
  ext->keySettings()->setFuncKey3Short(funcKeyCShort());
  ext->keySettings()->setFuncKey3Long(funcKeyCLong());
  ext->keySettings()->setFuncKey1Short(funcKey1Short());
  ext->keySettings()->setFuncKey1Long(funcKey1Long());
  ext->keySettings()->setFuncKey2Short(funcKey2Short());
  ext->keySettings()->setFuncKey2Long(funcKey2Long());
  ext->keySettings()->setLongPressDuration(longPressDuration());
  ext->keySettings()->enableAutoKeyLock(autoKeyLock());

  // Store tone settings
  ext->toneSettings()->enableKeyTone(this->keyToneEnabled());
  ext->toneSettings()->enableSMSAlert(smsAlert());
  ext->toneSettings()->enableCallAlert(callAlert());
  ext->toneSettings()->enableTalkPermitDigital(this->dmrTalkPermit());
  ext->toneSettings()->enableTalkPermitAnalog(this->fmTalkPermit());
  ext->toneSettings()->enableDigitalResetTone(this->dmrResetTone());
  ext->toneSettings()->enableDMRIdleChannelTone(this->idleChannelTone());
  ext->toneSettings()->enableStartupTone(this->startupTone());
  this->callToneMelody(*(ext->toneSettings()->callMelody()));
  this->idleToneMelody(*(ext->toneSettings()->idleMelody()));
  this->resetToneMelody(*(ext->toneSettings()->resetMelody()));

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
  ext->audioSettings()->setMaxVolume(this->maxSpeakerVolume());
  ext->audioSettings()->enableEnhanceAudio(this->enhanceAudio());

  // Store auto-repeater settings
  ext->autoRepeaterSettings()->setDirectionA(this->autoRepeaterDirectionA());
  ext->autoRepeaterSettings()->setDirectionB(autoRepeaterDirectionB());
  ext->autoRepeaterSettings()->setVHFMin(this->autoRepeaterMinFrequencyVHF());
  ext->autoRepeaterSettings()->setVHFMax(this->autoRepeaterMaxFrequencyVHF());
  ext->autoRepeaterSettings()->setUHFMin(this->autoRepeaterMinFrequencyUHF());
  ext->autoRepeaterSettings()->setUHFMax(this->autoRepeaterMaxFrequencyUHF());

  // Store GPS settings
  ext->gpsSettings()->setUnits(this->gpsUnitsImperial() ? AnytoneGPSSettingsExtension::Units::Archaic :
                                                          AnytoneGPSSettingsExtension::Units::Metric);
  ext->gpsSettings()->setTimeZone(gpsTimeZone());

  // Other settings
  ext->enableKeepLastCaller(this->keepLastCaller());

  return true;
}

bool
AnytoneCodeplug::GeneralSettingsElement::linkSettings(RadioSettings *settings, Context &ctx, const ErrorStack &err) {
  if (! settings->anytoneExtension())
    return false;

  AnytoneSettingsExtension *ext = settings->anytoneExtension();

  // Link boot settings
  if (this->defaultChannel()) {
    if (! ctx.has<Zone>(this->defaultZoneIndexA())) {
      errMsg(err) << "Cannot link default zone A. Zone index " << this->defaultZoneIndexA()
                  << " not defined.";
      return false;
    }
    ext->bootSettings()->zoneA()->set(ctx.get<Zone>(this->defaultZoneIndexA()));
    if (this->defaultChannelAIsVFO()) {
      // pass...
    } else if (! ctx.has<Channel>(this->defaultChannelAIndex())) {
      errMsg(err) << "Cannot link default channel A. Index " << this->defaultChannelAIndex()
                  << " not defined.";
      return false;
    } else {
      ext->bootSettings()->channelA()->set(ctx.get<Channel>(this->defaultChannelAIndex()));
    }

    if (! ctx.has<Zone>(this->defaultZoneIndexB())) {
      errMsg(err) << "Cannot link default zone B. Zone index " << this->defaultZoneIndexB()
                  << " not defined.";
      return false;
    }
    ext->bootSettings()->zoneB()->set(ctx.get<Zone>(this->defaultZoneIndexB()));
    if (this->defaultChannelBIsVFO()) {
      // pass...
    } else if (! ctx.has<Channel>(this->defaultChannelBIndex())) {
      errMsg(err) << "Cannot link default channel B. Index " << this->defaultChannelBIndex()
                  << " not defined.";
      return false;
    } else {
      ext->bootSettings()->channelB()->set(ctx.get<Channel>(this->defaultChannelBIndex()));
    }
  }

  // Link repeater offsets
  if (this->hasAutoRepeaterOffsetFrequencyIndexVHF()) {
    if (! ctx.has<AnytoneAutoRepeaterOffset>(this->autoRepeaterOffsetFrequencyIndexVHF())) {
      errMsg(err) << "Cannot link auto-repeater offset for VHF, index "
                  << this->autoRepeaterOffsetFrequencyIndexVHF() << " not defined.";
      return false;
    }
    ext->autoRepeaterSettings()->vhfRef()->set(
          ctx.get<AnytoneAutoRepeaterOffset>(this->autoRepeaterOffsetFrequencyIndexVHF()));
  }

  if (this->hasAutoRepeaterOffsetFrequencyIndexUHF()) {
    if (! ctx.has<AnytoneAutoRepeaterOffset>(this->autoRepeaterOffsetFrequencyIndexUHF())) {
      errMsg(err) << "Cannot link auto-repeater offset for UHF, index "
                  << this->autoRepeaterOffsetFrequencyIndexUHF() << " not defined.";
      return false;
    }
    ext->autoRepeaterSettings()->uhfRef()->set(
          ctx.get<AnytoneAutoRepeaterOffset>(this->autoRepeaterOffsetFrequencyIndexUHF()));
  }

  // Link auto-repeater
  if (hasAutoRepeaterOffsetFrequencyIndexVHF()) {
    if (! ctx.has<AnytoneAutoRepeaterOffset>(this->autoRepeaterOffsetFrequencyIndexVHF())) {
      errMsg(err) << "Cannot link auto-repeater offset frequency for VHF, index "
                  << this->autoRepeaterOffsetFrequencyIndexVHF() << " not defined.";
      return false;
    }
    ext->autoRepeaterSettings()->vhfRef()->set(
          ctx.get<AnytoneAutoRepeaterOffset>(this->autoRepeaterOffsetFrequencyIndexVHF()));
  }
  if (hasAutoRepeaterOffsetFrequencyIndexUHF()) {
    if (! ctx.has<AnytoneAutoRepeaterOffset>(this->autoRepeaterOffsetFrequencyIndexUHF())) {
      errMsg(err) << "Cannot link auto-repeater offset frequency for UHF, index "
                  << this->autoRepeaterOffsetFrequencyIndexUHF() << " not defined.";
      return false;
    }
    ext->autoRepeaterSettings()->uhfRef()->set(
          ctx.get<AnytoneAutoRepeaterOffset>(this->autoRepeaterOffsetFrequencyIndexUHF()));
  }


  return true;
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::ExtendedSettingsElement
 * ********************************************************************************************* */
AnytoneCodeplug::ExtendedSettingsElement::ExtendedSettingsElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

bool
AnytoneCodeplug::ExtendedSettingsElement::fromConfig(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err);

  if (! flags.updateCodePlug)
    this->clear();

  if (nullptr == ctx.config()->settings()->anytoneExtension()) {
    // If there is no extension, done
    return true;
  }

  // Get extension
  AnytoneSettingsExtension *ext = ctx.config()->settings()->anytoneExtension();

  // Encode DMR settings
  enableSendTalkerAlias(ext->dmrSettings()->sendTalkerAlias());
  setTalkerAliasSource(ext->dmrSettings()->talkerAliasSource());
  setTalkerAliasEncoding(ext->dmrSettings()->talkerAliasEncoding());

  // Encode display settings
  setChannelBNameColor(ext->displaySettings()->channelBNameColor());
  setZoneANameColor(ext->displaySettings()->zoneNameColor());
  setZoneBNameColor(ext->displaySettings()->zoneBNameColor());

  return true;
}

bool
AnytoneCodeplug::ExtendedSettingsElement::updateConfig(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err);

  // Get or add extension if not present
  AnytoneSettingsExtension *ext = ctx.config()->settings()->anytoneExtension();
  if (nullptr == ext) {
    ext = new AnytoneSettingsExtension();
    ctx.config()->settings()->setAnytoneExtension(ext);
  }

  // Store DMR settings
  ext->dmrSettings()->enableSendTalkerAlias(sendTalkerAlias());
  ext->dmrSettings()->setTalkerAliasSource(talkerAliasSource());
  ext->dmrSettings()->setTalkerAliasEncoding(talkerAliasEncoding());

  // Store display settings
  ext->displaySettings()->setChannelBNameColor(channelBNameColor());
  ext->displaySettings()->setZoneNameColor(zoneANameColor());
  ext->displaySettings()->setZoneBNameColor(zoneBNameColor());

  return true;
}

bool
AnytoneCodeplug::ExtendedSettingsElement::linkConfig(Context &ctx, const ErrorStack &err) {
  // Get or add extension if not present
  AnytoneSettingsExtension *ext = ctx.config()->settings()->anytoneExtension();
  if (nullptr == ext) {
    errMsg(err) << "Cannot link config extension: not set.";
    return false;
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::ZoneChannelListElement
 * ********************************************************************************************* */
AnytoneCodeplug::ZoneChannelListElement::ZoneChannelListElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pas...
}

AnytoneCodeplug::ZoneChannelListElement::ZoneChannelListElement(uint8_t *ptr)
  : Element(ptr, ZoneChannelListElement::size())
{
  // pass...
}

void
AnytoneCodeplug::ZoneChannelListElement::clear() {
  memset(_data, 0x00, _size);
  memset(_data, 0xff, 2*250);
  memset(_data+0x200, 0xff, 2*250);
}

bool
AnytoneCodeplug::ZoneChannelListElement::hasChannelA(unsigned n) const {
  return 0xffff == channelIndexA(n);
}
unsigned
AnytoneCodeplug::ZoneChannelListElement::channelIndexA(unsigned n) const {
  return getUInt16_le(0x0000 + 2*n);
}
void
AnytoneCodeplug::ZoneChannelListElement::setChannelIndexA(unsigned n, unsigned idx) {
  setUInt16_le(0x0000 + 2*n, idx);
}
void
AnytoneCodeplug::ZoneChannelListElement::clearChannelIndexA(unsigned n) {
  setChannelIndexA(n, 0xffff);
}

bool
AnytoneCodeplug::ZoneChannelListElement::hasChannelB(unsigned n) const {
  return 0xffff == channelIndexB(n);
}
unsigned
AnytoneCodeplug::ZoneChannelListElement::channelIndexB(unsigned n) const {
  return getUInt16_le(0x0200 + 2*n);
}
void
AnytoneCodeplug::ZoneChannelListElement::setChannelIndexB(unsigned n, unsigned idx) {
  setUInt16_le(0x0200 + 2*n, idx);
}
void
AnytoneCodeplug::ZoneChannelListElement::clearChannelIndexB(unsigned n) {
  setChannelIndexB(n, 0xffff);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::ZoneBitmapElement
 * ********************************************************************************************* */
AnytoneCodeplug::ZoneBitmapElement::ZoneBitmapElement(uint8_t *ptr, size_t size)
  : BitmapElement(ptr, size)
{
  // pass...
}

AnytoneCodeplug::ZoneBitmapElement::ZoneBitmapElement(uint8_t *ptr)
  : BitmapElement(ptr, ZoneBitmapElement::size())
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::BootSettingsElement
 * ********************************************************************************************* */
AnytoneCodeplug::BootSettingsElement::BootSettingsElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::BootSettingsElement::BootSettingsElement(uint8_t *ptr)
  : Element(ptr, BootSettingsElement::size())
{
  // pass...
}

void
AnytoneCodeplug::BootSettingsElement::clear() {
  memset(_data, 0x00, _size);
}

QString
AnytoneCodeplug::BootSettingsElement::introLine1() const {
  return readASCII(0x0000, 16, 0x00);
}
void
AnytoneCodeplug::BootSettingsElement::setIntroLine1(const QString &txt) {
  writeASCII(0x0000, txt, 16, 0x00);
}
QString
AnytoneCodeplug::BootSettingsElement::introLine2() const {
  return readASCII(0x0010, 16, 0x00);
}
void
AnytoneCodeplug::BootSettingsElement::setIntroLine2(const QString &txt) {
  writeASCII(0x0010, txt, 16, 0x00);
}

QString
AnytoneCodeplug::BootSettingsElement::password() const {
  return readASCII(0x0020, 8, 0x00);
}
void
AnytoneCodeplug::BootSettingsElement::setPassword(const QString &txt) {
  QRegularExpression pattern("[0-9]{0,8}");
  if (pattern.match(txt).isValid())
    writeASCII(0x0020, txt, 8, 0x00);
}

bool
AnytoneCodeplug::BootSettingsElement::fromConfig(const Flags &flags, Context &ctx) {
  Q_UNUSED(flags)
  setIntroLine1(ctx.config()->settings()->introLine1());
  setIntroLine2(ctx.config()->settings()->introLine2());

  // Handle extensions
  if (ctx.config()->settings()->anytoneExtension()) {
    AnytoneSettingsExtension *ext = ctx.config()->settings()->anytoneExtension();
    setPassword(ext->bootSettings()->bootPassword());
  }

  return true;
}

bool
AnytoneCodeplug::BootSettingsElement::updateConfig(Context &ctx) {
  ctx.config()->settings()->setIntroLine1(introLine1());
  ctx.config()->settings()->setIntroLine2(introLine2());

  // Create/update extension
  AnytoneSettingsExtension *ext = nullptr;
  if (ctx.config()->settings()->anytoneExtension())
    ext = ctx.config()->settings()->anytoneExtension();
  else
    ctx.config()->settings()->setAnytoneExtension(ext = new AnytoneSettingsExtension());
  ext->bootSettings()->setBootPassword(password());

  return true;
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::DMRAPRSSettingsElement
 * ********************************************************************************************* */
AnytoneCodeplug::DMRAPRSSettingsElement::DMRAPRSSettingsElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::DMRAPRSSettingsElement::DMRAPRSSettingsElement(uint8_t *ptr)
  : Element(ptr, DMRAPRSSettingsElement::size())
{
  // pass...
}

void
AnytoneCodeplug::DMRAPRSSettingsElement::clear() {
  memset(_data, 0x00, _size);
}

unsigned
AnytoneCodeplug::DMRAPRSSettingsElement::manualInterval() const {
  return getUInt8(0x0000);
}
void
AnytoneCodeplug::DMRAPRSSettingsElement::setManualInterval(unsigned sec) {
  setUInt8(0x0000, sec);
}

bool
AnytoneCodeplug::DMRAPRSSettingsElement::automatic() const {
  return 0!=getUInt8(0x0001);
}
unsigned
AnytoneCodeplug::DMRAPRSSettingsElement::automaticInterval() const {
  return 45 + 15*((unsigned)getUInt8(0x0001));
}
void
AnytoneCodeplug::DMRAPRSSettingsElement::setAutomaticInterval(unsigned sec) {
  if (sec<60)
    sec = 60;
  setUInt8(0x0001, (sec-45)/15);
}
void
AnytoneCodeplug::DMRAPRSSettingsElement::disableAutomatic() {
  setUInt8(0x0001, 0x00);
}

bool
AnytoneCodeplug::DMRAPRSSettingsElement::fixedLocation() const {
  return getUInt8(0x0002);
}
QGeoCoordinate
AnytoneCodeplug::DMRAPRSSettingsElement::location() const {
  double latitude  = getUInt8(0x0003) + double(getUInt8(0x0004))/60 + double(getUInt8(0x0005))/3600;
  if (getUInt8(0x0006)) latitude *= -1;
  double longitude = getUInt8(0x0007) + double(getUInt8(0x0008))/60 + double(getUInt8(0x0009))/3600;
  if (getUInt8(0x000a)) longitude *= -1;
  return QGeoCoordinate(latitude, longitude);
}
void
AnytoneCodeplug::DMRAPRSSettingsElement::setLocation(const QGeoCoordinate &pos) {
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
  setUInt8(0x0003, lat_deg); setUInt8(0x0004, lat_min); setUInt8(0x0005, lat_sec); setUInt8(0x0006, (south ? 0x01 : 0x00));
  setUInt8(0x0007, lon_deg); setUInt8(0x0008, lon_min); setUInt8(0x0009, lon_sec); setUInt8(0x000a, (west ? 0x01 : 0x00));
}
void
AnytoneCodeplug::DMRAPRSSettingsElement::enableFixedLocation(bool enable) {
  setUInt8(0x0002, (enable ? 0x01 : 0x00));
}

Channel::Power
AnytoneCodeplug::DMRAPRSSettingsElement::power() const {
  switch (getUInt8(0x000b)) {
  case 0x00: return Channel::Power::Low;
  case 0x01: return Channel::Power::Mid;
  case 0x02: return Channel::Power::High;
  case 0x03: return Channel::Power::Max;
  }
  return Channel::Power::Low;
}
void
AnytoneCodeplug::DMRAPRSSettingsElement::setPower(Channel::Power power) {
  switch (power) {
  case Channel::Power::Min:
  case Channel::Power::Low:
    setUInt8(0x000b, 0x00);
    break;
  case Channel::Power::Mid:
    setUInt8(0x000b, 0x01);
    break;
  case Channel::Power::High:
    setUInt8(0x000b, 0x02);
    break;
  case Channel::Power::Max:
    setUInt8(0x000b, 0x03);
    break;
  }
}

bool
AnytoneCodeplug::DMRAPRSSettingsElement::hasChannel(unsigned n) const {
  return 0xffff != channelIndex(n);
}
bool
AnytoneCodeplug::DMRAPRSSettingsElement::channelIsVFOA(unsigned n) const {
  return 0x0fa0 == channelIndex(n);
}
bool
AnytoneCodeplug::DMRAPRSSettingsElement::channelIsVFOB(unsigned n) const {
  return 0x0fa1 == channelIndex(n);
}
bool
AnytoneCodeplug::DMRAPRSSettingsElement::channelIsSelected(unsigned n) const {
  return 0x0fa2 == channelIndex(n);
}
unsigned
AnytoneCodeplug::DMRAPRSSettingsElement::channelIndex(unsigned n) const {
  return getUInt16_le(0x000c + 2*n);
}
void
AnytoneCodeplug::DMRAPRSSettingsElement::setChannelIndex(unsigned n, unsigned idx) {
  setUInt16_le(0x000c + 2*n, idx);
}
void
AnytoneCodeplug::DMRAPRSSettingsElement::setChannelVFOA(unsigned n) {
  setChannelIndex(n, 0x0fa0);
}
void
AnytoneCodeplug::DMRAPRSSettingsElement::setChannelVFOB(unsigned n) {
  setChannelIndex(n, 0x0fa1);
}
void
AnytoneCodeplug::DMRAPRSSettingsElement::setChannelSelected(unsigned n) {
  setChannelIndex(n, 0x0fa2);
}
void
AnytoneCodeplug::DMRAPRSSettingsElement::clearChannel(unsigned n) {
  setChannelIndex(n, 0xffff);
}

unsigned
AnytoneCodeplug::DMRAPRSSettingsElement::destination() const {
  return getBCD8_be(0x001c);
}
void
AnytoneCodeplug::DMRAPRSSettingsElement::setDestination(unsigned id) {
  setBCD8_be(0x001c, id);
}

DMRContact::Type
AnytoneCodeplug::DMRAPRSSettingsElement::callType() const {
  switch (getUInt8(0x0020)) {
  case 0x00: return DMRContact::PrivateCall;
  case 0x01: return DMRContact::GroupCall;
  case 0x02: return DMRContact::AllCall;
  }
  return DMRContact::PrivateCall;
}
void
AnytoneCodeplug::DMRAPRSSettingsElement::setCallType(DMRContact::Type type) {
  switch (type) {
  case DMRContact::PrivateCall: setUInt8(0x0020, 0x00); break;
  case DMRContact::GroupCall: setUInt8(0x0020, 0x01); break;
  case DMRContact::AllCall: setUInt8(0x0020, 0x02); break;
  }
}

bool
AnytoneCodeplug::DMRAPRSSettingsElement::timeSlotOverride() const {
  return 0 != getUInt8(0x0021);
}
DMRChannel::TimeSlot
AnytoneCodeplug::DMRAPRSSettingsElement::timeslot() const {
  if (1 == getUInt8(0x0021))
    return DMRChannel::TimeSlot::TS1;
  else if (2 == getUInt8(0x0021))
    return DMRChannel::TimeSlot::TS2;
  return DMRChannel::TimeSlot::TS1;
}
void
AnytoneCodeplug::DMRAPRSSettingsElement::overrideTimeSlot(DMRChannel::TimeSlot ts) {
  if (DMRChannel::TimeSlot::TS1 == ts)
    setUInt8(0x0021, 0x01);
  else
    setUInt8(0x0021, 0x02);
}
void
AnytoneCodeplug::DMRAPRSSettingsElement::disableTimeSlotOverride() {
  setUInt8(0x0021, 0x00);
}

bool
AnytoneCodeplug::DMRAPRSSettingsElement::fromConfig(const Flags &flags, Context &ctx) {
  Q_UNUSED(flags)

  if (1 < ctx.config()->posSystems()->gpsCount()) {
    logDebug() << "D868UV only supports a single independent GPS positioning system.";
  } else if (0 == ctx.config()->posSystems()->gpsCount()) {
    return true;
  }

  GPSSystem *sys = ctx.config()->posSystems()->gpsSystem(0);
  setDestination(sys->contactObj()->number());
  setCallType(sys->contactObj()->type());
  setManualInterval(sys->period());
  setAutomaticInterval(sys->period());
  disableTimeSlotOverride();
  if (SelectedChannel::get() == sys->revertChannel()->as<Channel>()) {
    setChannelSelected(0);
  } else if (sys->revert()->is<DMRChannel>()) {
    setChannelIndex(0, ctx.index(sys->revertChannel()));
  } else {
    clearChannel(0);
  }
  return true;
}

bool
AnytoneCodeplug::DMRAPRSSettingsElement::createGPSSystem(uint8_t i, Context &ctx) {
  GPSSystem *sys = new GPSSystem(QString("GPS sys %1").arg(i+1), nullptr, nullptr, automaticInterval());
  ctx.config()->posSystems()->add(sys); ctx.add(sys, i);
  return true;
}

bool
AnytoneCodeplug::DMRAPRSSettingsElement::linkGPSSystem(uint8_t i, Context &ctx) {
  DMRContact *cont = nullptr;
  // Find matching contact, if not found -> create one.
  if (nullptr == (cont = ctx.config()->contacts()->findDigitalContact(destination()))) {
    cont = new DMRContact(callType(), QString("GPS target"), destination());
    ctx.config()->contacts()->add(cont);
  }
  ctx.get<GPSSystem>(i)->setContactObj(cont);

  // Check if there is a revert channel set
  if ((! channelIsSelected(i)) && (ctx.has<Channel>(channelIndex(i))) && (ctx.get<Channel>(channelIndex(i)))->is<DMRChannel>()) {
    DMRChannel *ch = ctx.get<Channel>(channelIndex(i))->as<DMRChannel>();
    ctx.get<GPSSystem>(i)->setRevertChannel(ch);
  }
  return true;
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::DMRAPRSMessageElement
 * ********************************************************************************************* */
AnytoneCodeplug::DMRAPRSMessageElement::DMRAPRSMessageElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::DMRAPRSMessageElement::DMRAPRSMessageElement(uint8_t *ptr)
  : Element(ptr, DMRAPRSMessageElement::size())
{
  // pass...
}

void
AnytoneCodeplug::DMRAPRSMessageElement::clear() {
  memset(_data, 0x00, _size);
}

QString
AnytoneCodeplug::DMRAPRSMessageElement::message() const {
  return readASCII(Offset::message(), Limit::length(), 0x00);
}

void
AnytoneCodeplug::DMRAPRSMessageElement::setMessage(const QString &message) {
  writeASCII(Offset::message(), message, Limit::length(), 0x00);
}

bool
AnytoneCodeplug::DMRAPRSMessageElement::fromConfig(Codeplug::Flags flags, Context &ctx) {
  Q_UNUSED(flags); Q_UNUSED(ctx)
  return true;
}

bool
AnytoneCodeplug::DMRAPRSMessageElement::updateConfig(Context &ctx) const {
  Q_UNUSED(ctx)
  return true;
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::RepeaterOffsetListElement
 * ********************************************************************************************* */
AnytoneCodeplug::RepeaterOffsetListElement::RepeaterOffsetListElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::RepeaterOffsetListElement::RepeaterOffsetListElement(uint8_t *ptr)
  : Element(ptr, RepeaterOffsetListElement::size())
{
  // pass...
}

void
AnytoneCodeplug::RepeaterOffsetListElement::clear() {
  memset(_data, 0x00, _size);
  for (unsigned int i=0; i<Limit::numEntries(); i++)
    clearOffset(i);
}

bool
AnytoneCodeplug::RepeaterOffsetListElement::isSet(unsigned int n) const {
  if (n >= Limit::numEntries())
    return false;
  return 0 != getUInt32_le(Offset::frequencies() + n * Offset::betweenFrequencies());
}

Frequency
AnytoneCodeplug::RepeaterOffsetListElement::offset(unsigned int n) const {
  if (n >= Limit::numEntries())
    return Frequency::fromHz(0);

  return Frequency::fromHz(
        ((unsigned long long)getUInt32_le(Offset::frequencies()
                                          + n * Offset::betweenFrequencies()))*10);
}

void
AnytoneCodeplug::RepeaterOffsetListElement::setOffset(unsigned int n, Frequency freq) {
  if (n >= Limit::numEntries())
    return;

  setUInt32_le(Offset::frequencies() + n*Offset::betweenFrequencies(), freq.inHz()/10);
}

void
AnytoneCodeplug::RepeaterOffsetListElement::clearOffset(unsigned int n) {
  if (n >= Limit::numEntries())
    return;
  setUInt32_le(Offset::frequencies() + n*Offset::betweenFrequencies(), 0);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::MessageListElement
 * ********************************************************************************************* */
AnytoneCodeplug::MessageListElement::MessageListElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::MessageListElement::MessageListElement(uint8_t *ptr)
  : Element(ptr, MessageListElement::size())
{
  // pass...
}

void
AnytoneCodeplug::MessageListElement::clear() {
  memset(_data, 0x00, _size);
  clearNext();
  clearIndex();
}

bool
AnytoneCodeplug::MessageListElement::hasNext() const {
  return 0xff != next();
}
unsigned
AnytoneCodeplug::MessageListElement::next() const {
  return getUInt8(0x0002);
}
void
AnytoneCodeplug::MessageListElement::setNext(unsigned idx) {
  setUInt8(0x0002, idx);
}
void
AnytoneCodeplug::MessageListElement::clearNext() {
  setNext(0xff);
}

bool
AnytoneCodeplug::MessageListElement::hasIndex() const {
  return 0xff != index();
}
unsigned
AnytoneCodeplug::MessageListElement::index() const {
  return getUInt8(0x0003);
}
void
AnytoneCodeplug::MessageListElement::setIndex(unsigned idx) {
  setUInt8(0x0003, idx);
}
void
AnytoneCodeplug::MessageListElement::clearIndex() {
  setIndex(0xff);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::MessageElement
 * ********************************************************************************************* */
AnytoneCodeplug::MessageElement::MessageElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::MessageElement::MessageElement(uint8_t *ptr)
  : Element(ptr, MessageElement::size())
{
  // pass...
}

void
AnytoneCodeplug::MessageElement::clear() {
  memset(_data, 0x00, _size);
}

QString
AnytoneCodeplug::MessageElement::message() const {
  return readASCII(0x0000, 99, 0x00);
}
void
AnytoneCodeplug::MessageElement::setMessage(const QString &msg) {
  writeASCII(0x0000, msg, 99, 0x00);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::MessageBytemapElement
 * ********************************************************************************************* */
AnytoneCodeplug::MessageBytemapElement::MessageBytemapElement(uint8_t *ptr, size_t size)
  : InvertedBytemapElement(ptr, size)
{
  // pass...
}

AnytoneCodeplug::MessageBytemapElement::MessageBytemapElement(uint8_t *ptr)
  : InvertedBytemapElement(ptr, MessageBytemapElement::size())
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::AnalogQuickCallElement
 * ********************************************************************************************* */
AnytoneCodeplug::AnalogQuickCallElement::AnalogQuickCallElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::AnalogQuickCallElement::AnalogQuickCallElement(uint8_t *ptr)
  : Element(ptr, AnalogQuickCallElement::size())
{
  // pass...
}

void
AnytoneCodeplug::AnalogQuickCallElement::clear() {
  memset(_data, 0x00, _size);
  clearContactIndex();
}

AnytoneCodeplug::AnalogQuickCallElement::Type
AnytoneCodeplug::AnalogQuickCallElement::type() const {
  return (Type)getUInt8(0x0000);
}
void
AnytoneCodeplug::AnalogQuickCallElement::setType(Type type) {
  setUInt8(0x0000, (unsigned)type);
}

bool
AnytoneCodeplug::AnalogQuickCallElement::hasContactIndex() const {
  return 0xff != contactIndex();
}
unsigned
AnytoneCodeplug::AnalogQuickCallElement::contactIndex() const {
  return getUInt8(0x0001);
}
void
AnytoneCodeplug::AnalogQuickCallElement::setContactIndex(unsigned idx) {
  setUInt8(0x0001, idx);
}
void
AnytoneCodeplug::AnalogQuickCallElement::clearContactIndex() {
  setContactIndex(0xff);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::AnalogQuickCallsElement
 * ********************************************************************************************* */
AnytoneCodeplug::AnalogQuickCallsElement::AnalogQuickCallsElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::AnalogQuickCallsElement::AnalogQuickCallsElement(uint8_t *ptr)
  : Element(ptr, AnalogQuickCallsElement::size())
{
  // pass...
}

void
AnytoneCodeplug::AnalogQuickCallsElement::clear() {
  memset(_data, 0x00, _size);
  for (unsigned int i=0; i<Limit::numEntries(); i++)
    AnalogQuickCallElement(quickCall(i)).clear();
}

uint8_t *
AnytoneCodeplug::AnalogQuickCallsElement::quickCall(unsigned int n) const {
  n = std::min(Limit::numEntries(), n);
  return _data + n*AnalogQuickCallElement::size();
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::StatusMessagesElement
 * ********************************************************************************************* */
AnytoneCodeplug::StatusMessagesElement::StatusMessagesElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::StatusMessagesElement::StatusMessagesElement(uint8_t *ptr)
  : Element(ptr, StatusMessagesElement::size())
{
  // pass...
}

void
AnytoneCodeplug::StatusMessagesElement::clear() {
  memset(_data, 0x00, _size);
}

QString
AnytoneCodeplug::StatusMessagesElement::message(unsigned int n) const {
  n = std::min(Limit::numMessages(), n);
  return readASCII(Offset::messages()+n*Offset::betweenMessages(), Limit::messageLength(), 0x00);
}

void
AnytoneCodeplug::StatusMessagesElement::setMessage(unsigned int n, const QString &msg) {
  if (n >= Limit::numMessages())
    return;
  writeASCII(Offset::messages()+n*Offset::betweenMessages(), msg, Limit::messageLength(), 0x00);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::StatusMessageBitmapElement
 * ********************************************************************************************* */
AnytoneCodeplug::StatusMessageBitmapElement::StatusMessageBitmapElement(uint8_t *ptr, size_t size)
  : BitmapElement(ptr, size)
{
  // pass...
}

AnytoneCodeplug::StatusMessageBitmapElement::StatusMessageBitmapElement(uint8_t *ptr)
  : BitmapElement(ptr, StatusMessageBitmapElement::size())
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::HotKeyElement
 * ********************************************************************************************* */
AnytoneCodeplug::HotKeyElement::HotKeyElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::HotKeyElement::HotKeyElement(uint8_t *ptr)
  : Element(ptr, HotKeyElement::size())
{
  // pass...
}

void
AnytoneCodeplug::HotKeyElement::clear() {
  memset(_data, 0x00, _size);
  clearContactIndex();
  clearMessageIndex();
}

AnytoneCodeplug::HotKeyElement::Type
AnytoneCodeplug::HotKeyElement::type() const {
  return (Type)getUInt8(0x0000);
}
void
AnytoneCodeplug::HotKeyElement::setType(Type type) {
  setUInt8(0x0000, (unsigned)type);
}

AnytoneCodeplug::HotKeyElement::MenuItem
AnytoneCodeplug::HotKeyElement::menuItem() const {
  return (MenuItem) getUInt8(0x0001);
}
void
AnytoneCodeplug::HotKeyElement::setMenuItem(MenuItem item) {
  setUInt8(0x0001, (unsigned)item);
}

AnytoneCodeplug::HotKeyElement::CallType
AnytoneCodeplug::HotKeyElement::callType() const {
  return (CallType)getUInt8(0x0002);
}
void
AnytoneCodeplug::HotKeyElement::setCallType(CallType type) {
  setUInt8(0x0002, (unsigned)type);
}

AnytoneCodeplug::HotKeyElement::DigiCallType
AnytoneCodeplug::HotKeyElement::digiCallType() const {
  return (DigiCallType)getUInt8(0x0003);
}
void
AnytoneCodeplug::HotKeyElement::setDigiCallType(DigiCallType type) {
  setUInt8(0x0003, (unsigned)type);
}

bool
AnytoneCodeplug::HotKeyElement::hasContactIndex() const {
  return 0xffffffff != contactIndex();
}
unsigned
AnytoneCodeplug::HotKeyElement::contactIndex() const {
  return getUInt32_le(0x0004);
}
void
AnytoneCodeplug::HotKeyElement::setContactIndex(unsigned idx) {
  setUInt32_le(0x0004, idx);
}
void
AnytoneCodeplug::HotKeyElement::clearContactIndex() {
  setContactIndex(0xffffffff);
}

bool
AnytoneCodeplug::HotKeyElement::hasMessageIndex() const {
  return 0xff != messageIndex();
}
unsigned
AnytoneCodeplug::HotKeyElement::messageIndex() const {
  return getUInt8(0x0008);
}
void
AnytoneCodeplug::HotKeyElement::setMessageIndex(unsigned idx) {
  setUInt8(0x0008, idx);
}
void
AnytoneCodeplug::HotKeyElement::clearMessageIndex() {
  setMessageIndex(0xff);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::HotKeySettingsElement
 * ********************************************************************************************* */
AnytoneCodeplug::HotKeySettingsElement::HotKeySettingsElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::HotKeySettingsElement::HotKeySettingsElement(uint8_t *ptr)
  : Element(ptr, HotKeySettingsElement::size())
{
  // pass...
}

void
AnytoneCodeplug::HotKeySettingsElement::clear() {
  memset(_data, 0x00, _size);
  for (unsigned int i=0; i<Limit::numEntries(); i++)
    HotKeyElement(hotKeySetting(i)).clear();
}

uint8_t *
AnytoneCodeplug::HotKeySettingsElement::hotKeySetting(unsigned int n) const {
  n = std::min(Limit::numEntries(), n);
  return _data + Offset::hotKeySettings() + n*Offset::betweenHotKeySettings();
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::AlarmSettingElement::AnalogAlarm
 * ********************************************************************************************* */
AnytoneCodeplug::AlarmSettingElement::AnalogAlarm::AnalogAlarm(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::AlarmSettingElement::AnalogAlarm::AnalogAlarm(uint8_t *ptr)
  : Element(ptr, AlarmSettingElement::AnalogAlarm::size())
{
  // pass...
}

void
AnytoneCodeplug::AlarmSettingElement::AnalogAlarm::clear() {
  memset(_data, 0x00, _size);
  setChannelSelected();
}

AnytoneCodeplug::AlarmSettingElement::AnalogAlarm::Action
AnytoneCodeplug::AlarmSettingElement::AnalogAlarm::action() const {
  return (Action) getUInt8(0x0000);
}
void
AnytoneCodeplug::AlarmSettingElement::AnalogAlarm::setAction(Action action) {
  setUInt8(0x0000, (unsigned)action);
}

AnytoneCodeplug::AlarmSettingElement::AnalogAlarm::ENIType
AnytoneCodeplug::AlarmSettingElement::AnalogAlarm::encodingType() const {
  return (ENIType) getUInt8(0x0001);
}
void
AnytoneCodeplug::AlarmSettingElement::AnalogAlarm::setEncodingType(ENIType type) {
  setUInt8(0x0001, (unsigned)type);
}

unsigned
AnytoneCodeplug::AlarmSettingElement::AnalogAlarm::emergencyIndex() const {
  return getUInt8(0x0002);
}
void
AnytoneCodeplug::AlarmSettingElement::AnalogAlarm::setEmergencyIndex(unsigned idx) {
  setUInt8(0x0002, idx);
}

unsigned
AnytoneCodeplug::AlarmSettingElement::AnalogAlarm::duration() const {
  return getUInt8(0x0003);
}
void
AnytoneCodeplug::AlarmSettingElement::AnalogAlarm::setDuration(unsigned sec) {
  setUInt8(0x0003, sec);
}

unsigned
AnytoneCodeplug::AlarmSettingElement::AnalogAlarm::txDuration() const {
  return getUInt8(0x0004);
}
void
AnytoneCodeplug::AlarmSettingElement::AnalogAlarm::setTXDuration(unsigned sec) {
  setUInt8(0x0004, sec);
}

unsigned
AnytoneCodeplug::AlarmSettingElement::AnalogAlarm::rxDuration() const {
  return getUInt8(0x0005);
}
void
AnytoneCodeplug::AlarmSettingElement::AnalogAlarm::setRXDuration(unsigned sec) {
  setUInt8(0x0005, sec);
}

bool
AnytoneCodeplug::AlarmSettingElement::AnalogAlarm::channelIsSelected() const {
  return getUInt8(0x0008);
}
unsigned
AnytoneCodeplug::AlarmSettingElement::AnalogAlarm::channelIndex() const {
  return getUInt16_le(0x0006);
}
void
AnytoneCodeplug::AlarmSettingElement::AnalogAlarm::setChannelIndex(unsigned idx) {
  setUInt16_le(0x0006, idx);
  setUInt8(0x0008, 0x00);
}
void
AnytoneCodeplug::AlarmSettingElement::AnalogAlarm::setChannelSelected() {
  setUInt8(0x0008, 0x01);
}

bool
AnytoneCodeplug::AlarmSettingElement::AnalogAlarm::repeatContinuously() const {
  return 0 == repetitions();
}
unsigned
AnytoneCodeplug::AlarmSettingElement::AnalogAlarm::repetitions() const {
  return getUInt8(0x0009);
}
void
AnytoneCodeplug::AlarmSettingElement::AnalogAlarm::setRepetitions(unsigned num) {
  setUInt8(0x0009, num);
}
void
AnytoneCodeplug::AlarmSettingElement::AnalogAlarm::setRepatContinuously() {
  setRepetitions(0x00);
}

/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::AlarmSettingElement::DigitalAlarm
 * ********************************************************************************************* */
AnytoneCodeplug::AlarmSettingElement::DigitalAlarm::DigitalAlarm(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::AlarmSettingElement::DigitalAlarm::DigitalAlarm(uint8_t *ptr)
  : Element(ptr, AlarmSettingElement::DigitalAlarm::size())
{
  // pass...
}

void
AnytoneCodeplug::AlarmSettingElement::DigitalAlarm::clear() {
  memset(_data, 0x00, _size);
  setChannelSelected();
}

AnytoneCodeplug::AlarmSettingElement::DigitalAlarm::Action
AnytoneCodeplug::AlarmSettingElement::DigitalAlarm::action() const {
  return (Action) getUInt8(0x0000);
}
void
AnytoneCodeplug::AlarmSettingElement::DigitalAlarm::setAction(Action action) {
  setUInt8(0x0000, (unsigned)action);
}

unsigned
AnytoneCodeplug::AlarmSettingElement::DigitalAlarm::duration() const {
  return getUInt8(0x0001);
}
void
AnytoneCodeplug::AlarmSettingElement::DigitalAlarm::setDuration(unsigned sec) {
  setUInt8(0x0001, sec);
}

unsigned
AnytoneCodeplug::AlarmSettingElement::DigitalAlarm::txDuration() const {
  return getUInt8(0x0002);
}
void
AnytoneCodeplug::AlarmSettingElement::DigitalAlarm::setTXDuration(unsigned sec) {
  setUInt8(0x0002, sec);
}

unsigned
AnytoneCodeplug::AlarmSettingElement::DigitalAlarm::rxDuration() const {
  return getUInt8(0x0003);
}
void
AnytoneCodeplug::AlarmSettingElement::DigitalAlarm::setRXDuration(unsigned sec) {
  setUInt8(0x0003, sec);
}

bool
AnytoneCodeplug::AlarmSettingElement::DigitalAlarm::channelIsSelected() const {
  return getUInt8(0x0006);
}
unsigned
AnytoneCodeplug::AlarmSettingElement::DigitalAlarm::channelIndex() const {
  return getUInt16_le(0x0004);
}
void
AnytoneCodeplug::AlarmSettingElement::DigitalAlarm::setChannelIndex(unsigned idx) {
  setUInt16_le(0x0004, idx);
  setUInt8(0x0006, 0x00);
}
void
AnytoneCodeplug::AlarmSettingElement::DigitalAlarm::setChannelSelected() {
  setUInt8(0x0006, 0x01);
}

bool
AnytoneCodeplug::AlarmSettingElement::DigitalAlarm::repeatContinuously() const {
  return 0 == repetitions();
}
unsigned
AnytoneCodeplug::AlarmSettingElement::DigitalAlarm::repetitions() const {
  return getUInt8(0x0007);
}
void
AnytoneCodeplug::AlarmSettingElement::DigitalAlarm::setRepetitions(unsigned num) {
  setUInt8(0x0007, num);
}
void
AnytoneCodeplug::AlarmSettingElement::DigitalAlarm::setRepatContinuously() {
  setRepetitions(0x00);
}

unsigned
AnytoneCodeplug::AlarmSettingElement::DigitalAlarm::voiceBroadcastDuration() const {
  return getUInt8(0x0008)+1;
}
void
AnytoneCodeplug::AlarmSettingElement::DigitalAlarm::setVoiceBroadcastDuration(unsigned min) {
  if (1 > min) min = 1;
  setUInt8(0x0008, min-1);
}

unsigned
AnytoneCodeplug::AlarmSettingElement::DigitalAlarm::areaBroadcastDuration() const {
  return getUInt8(0x0009)+1;
}
void
AnytoneCodeplug::AlarmSettingElement::DigitalAlarm::setAreaBroadcastDuration(unsigned min) {
  if (1 > min) min = 1;
  setUInt8(0x0009, min-1);
}

bool
AnytoneCodeplug::AlarmSettingElement::DigitalAlarm::vox() const {
  return getUInt8(0x000a);
}
void
AnytoneCodeplug::AlarmSettingElement::DigitalAlarm::enableVOX(bool enable) {
  setUInt8(0x000a, (enable ? 0x01 : 0x00));
}

bool
AnytoneCodeplug::AlarmSettingElement::DigitalAlarm::rxAlarm() const {
  return getUInt8(0x000b);
}
void
AnytoneCodeplug::AlarmSettingElement::DigitalAlarm::enableRXAlarm(bool enable) {
  setUInt8(0x000b, (enable ? 0x01 : 0x00));
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::AlarmSettingElement
 * ********************************************************************************************* */
AnytoneCodeplug::AlarmSettingElement::AlarmSettingElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::AlarmSettingElement::AlarmSettingElement(uint8_t *ptr)
  : Element(ptr, AlarmSettingElement::size())
{
  // pass...
}

void
AnytoneCodeplug::AlarmSettingElement::clear() {
  AnalogAlarm(analog()).clear();
  DigitalAlarm(digital()).clear();
}

uint8_t *
AnytoneCodeplug::AlarmSettingElement::analog() const {
  return _data + Offset::analog();
}
uint8_t *
AnytoneCodeplug::AlarmSettingElement::digital() const {
  return _data + Offset::digital();
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::DigitalAlarmExtensionElement
 * ********************************************************************************************* */
AnytoneCodeplug::DigitalAlarmExtensionElement::DigitalAlarmExtensionElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::DigitalAlarmExtensionElement::DigitalAlarmExtensionElement(uint8_t *ptr)
  : Element(ptr, DigitalAlarmExtensionElement::size())
{
  // pass...
}

void
AnytoneCodeplug::DigitalAlarmExtensionElement::clear() {
  memset(_data, 0x00, _size);
}

DMRContact::Type
AnytoneCodeplug::DigitalAlarmExtensionElement::callType() const {
  switch (getUInt8(Offset::callType())) {
  case 0x00: return DMRContact::PrivateCall;
  case 0x01: return DMRContact::GroupCall;
  case 0x02: return DMRContact::AllCall;
  }
  return DMRContact::PrivateCall;
}
void
AnytoneCodeplug::DigitalAlarmExtensionElement::setCallType(DMRContact::Type type) {
  switch (type) {
  case DMRContact::PrivateCall: setUInt8(Offset::callType(), 0x00); break;
  case DMRContact::GroupCall: setUInt8(Offset::callType(), 0x01); break;
  case DMRContact::AllCall: setUInt8(Offset::callType(), 0x02); break;
  }
}

unsigned
AnytoneCodeplug::DigitalAlarmExtensionElement::destination() const {
  return getBCD8_be(Offset::destination());
}
void
AnytoneCodeplug::DigitalAlarmExtensionElement::setDestination(unsigned number) {
  setBCD8_be(Offset::destination(), number);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::FiveToneIDElement
 * ********************************************************************************************* */
AnytoneCodeplug::FiveToneIDElement::FiveToneIDElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::FiveToneIDElement::FiveToneIDElement(uint8_t *ptr)
  : Element(ptr, FiveToneIDElement::size())
{
  // pass...
}

void
AnytoneCodeplug::FiveToneIDElement::clear() {
  memset(_data, 0x00, _size);
}

AnytoneCodeplug::FiveToneIDElement::Standard
AnytoneCodeplug::FiveToneIDElement::standard() const {
  return (Standard) getUInt8(0x0001);
}
void
AnytoneCodeplug::FiveToneIDElement::setStandard(Standard std) {
  setUInt8(0x0001, (unsigned)std);
}

unsigned
AnytoneCodeplug::FiveToneIDElement::toneDuration() const {
  return getUInt8(0x0003);
}
void
AnytoneCodeplug::FiveToneIDElement::setToneDuration(unsigned ms) {
  setUInt8(0x0003, ms);
}

QString
AnytoneCodeplug::FiveToneIDElement::id() const {
  unsigned len = getUInt8(0x0002);
  QString id;
  for (unsigned i=0; i<len; i++) {
    uint8_t b = getUInt8(0x0004+(i/2));
    if (0 == (i%2))
      id.append(QString::number((b>>4)&0xf,16));
    else
      id.append(QString::number((b>>0)&0xf,16));
  }
  return id;
}
void
AnytoneCodeplug::FiveToneIDElement::setID(const QString &id) {
  unsigned len = 0;
  for (int i=0; i<id.length(); i++) {
    bool ok;
    if (0 == (len%2)) {
      setUInt4(0x0004+len/2, 4, id.mid(i, 1).toUInt(&ok, 16));
    } else {
      setUInt4(0x0004+len/2, 0, id.mid(i, 1).toUInt(&ok, 16));
    }
    len++;
  }
  setUInt8(0x0002, len);
}

QString
AnytoneCodeplug::FiveToneIDElement::name() const {
  return readASCII(0x0018, 7, 0x00);
}
void
AnytoneCodeplug::FiveToneIDElement::setName(const QString &name) {
  writeASCII(0x0018, name, 7, 0x00);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::FiveToneIDBitmapElement
 * ********************************************************************************************* */
AnytoneCodeplug::FiveToneIDBitmapElement::FiveToneIDBitmapElement(uint8_t *ptr, size_t size)
  : BitmapElement(ptr, size)
{
  // pass...
}

AnytoneCodeplug::FiveToneIDBitmapElement::FiveToneIDBitmapElement(uint8_t *ptr)
  : BitmapElement(ptr, FiveToneIDBitmapElement::size())
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::FiveToneIDListElement
 * ********************************************************************************************* */
AnytoneCodeplug::FiveToneIDListElement::FiveToneIDListElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::FiveToneIDListElement::FiveToneIDListElement(uint8_t *ptr)
  : Element(ptr, FiveToneIDListElement::size())
{
  // pass...
}

void
AnytoneCodeplug::FiveToneIDListElement::clear() {
  memset(_data, 0, _size);
}

uint8_t *
AnytoneCodeplug::FiveToneIDListElement::member(unsigned int n) const {
  if (n >= Limit::numEntries())
    return nullptr;
  return _data + n*FiveToneIDElement::size();
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::FiveToneFunctionElement
 * ********************************************************************************************* */
AnytoneCodeplug::FiveToneFunctionElement::FiveToneFunctionElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::FiveToneFunctionElement::FiveToneFunctionElement(uint8_t *ptr)
  : Element(ptr, FiveToneFunctionElement::size())
{
  // pass...
}

void
AnytoneCodeplug::FiveToneFunctionElement::clear() {
  memset(_data, 0x00, _size);
}

AnytoneCodeplug::FiveToneFunctionElement::Function
AnytoneCodeplug::FiveToneFunctionElement::function() const {
  return (Function) getUInt8(0x0000);
}
void
AnytoneCodeplug::FiveToneFunctionElement::setFunction(Function function) {
  setUInt8(0x0000, (unsigned)function);
}

AnytoneCodeplug::FiveToneFunctionElement::Response
AnytoneCodeplug::FiveToneFunctionElement::response() const {
  return (Response) getUInt8(0x0001);
}
void
AnytoneCodeplug::FiveToneFunctionElement::setResponse(Response response) {
  setUInt8(0x0001, (unsigned)response);
}

QString
AnytoneCodeplug::FiveToneFunctionElement::id() const {
  unsigned len = getUInt8(0x0002);
  QString id;
  for (unsigned i=0; i<len; i++) {
    uint8_t b = getUInt8(0x0003+(i/2));
    if (0 == (i%2))
      id.append(QString::number((b>>4)&0xf, 16));
    else
      id.append(QString::number((b>>0)&0xf, 16));
  }
  return id;
}
void
AnytoneCodeplug::FiveToneFunctionElement::setID(const QString &id) {
  unsigned len = 0;
  for (int i=0; i<id.length(); i++) {
    bool ok;
    if (0 == (len%2)) {
      setUInt4(0x0003+len/2, 4, id.mid(i, 1).toUInt(&ok, 16));
    } else {
      setUInt4(0x0003+len/2, 0, id.mid(i, 1).toUInt(&ok, 16));
    }
    len++;
  }
  setUInt8(0x0002, len);
}

QString
AnytoneCodeplug::FiveToneFunctionElement::name() const {
  return readASCII(0x000f, 7, 0x00);
}
void
AnytoneCodeplug::FiveToneFunctionElement::setName(const QString &name) {
  writeASCII(0x000f, name, 7, 0x00);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::FiveToneFunctionListElement
 * ********************************************************************************************* */
AnytoneCodeplug::FiveToneFunctionListElement::FiveToneFunctionListElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::FiveToneFunctionListElement::FiveToneFunctionListElement(uint8_t *ptr)
  : Element(ptr, FiveToneFunctionListElement::size())
{
  // pass...
}

void
AnytoneCodeplug::FiveToneFunctionListElement::clear() {
  memset(_data, 0, _size);
}

uint8_t *
AnytoneCodeplug::FiveToneFunctionListElement::function(unsigned int n) const {
  if (n >= Limit::numFunctions())
    return nullptr;
  return _data + n*FiveToneFunctionElement::size();
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::FiveToneSettingsElement
 * ********************************************************************************************* */
AnytoneCodeplug::FiveToneSettingsElement::FiveToneSettingsElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::FiveToneSettingsElement::FiveToneSettingsElement(uint8_t *ptr)
  : Element(ptr, FiveToneSettingsElement::size())
{
  // pass...
}

void
AnytoneCodeplug::FiveToneSettingsElement::clear() {
  memset(_data, 0x00, _size);
}

AnytoneCodeplug::FiveToneSettingsElement::Response
AnytoneCodeplug::FiveToneSettingsElement::decodingResponse() const {
  return (Response) getUInt8(0x0021);
}
void
AnytoneCodeplug::FiveToneSettingsElement::setDecodingResponse(Response response) {
  setUInt8(0x0021, (unsigned)response);
}

AnytoneCodeplug::FiveToneSettingsElement::Standard
AnytoneCodeplug::FiveToneSettingsElement::decodingStandard() const {
  return (Standard) getUInt8(0x0022);
}
void
AnytoneCodeplug::FiveToneSettingsElement::setDecodingStandard(Standard standard) {
  setUInt8(0x0022, (unsigned)standard);
}

unsigned
AnytoneCodeplug::FiveToneSettingsElement::decodingToneDuration() const {
  return getUInt8(0x0024);
}
void
AnytoneCodeplug::FiveToneSettingsElement::setDecodingToneDuration(unsigned ms) {
  setUInt8(0x0024, ms);
}

QString
AnytoneCodeplug::FiveToneSettingsElement::id() const {
  unsigned len = getUInt8(0x0023);
  QString id;
  for (unsigned i=0; i<len; i++) {
    uint8_t b = getUInt8(0x0025+(i/2));
    if (0 == (i%2))
      id.append(QString::number((b>>4)&0xf, 16));
    else
      id.append(QString::number((b>>0)&0xf, 16));
  }
  return id;
}
void
AnytoneCodeplug::FiveToneSettingsElement::setID(const QString &id) {
  unsigned len = 0;
  for (int i=0; i<id.length(); i++) {
    bool ok;
    if (0 == (len%2)) {
      setUInt4(0x0025+len/2, 4, id.mid(i, 1).toUInt(&ok, 16));
    } else {
      setUInt4(0x0025+len/2, 0, id.mid(i, 1).toUInt(&ok, 16));
    }
    len++;
  }
  setUInt8(0x0023, len);
}

unsigned
AnytoneCodeplug::FiveToneSettingsElement::postEncodeDelay() const {
  return ((unsigned)getUInt8(0x002c))*10;
}
void
AnytoneCodeplug::FiveToneSettingsElement::setPostEncodeDelay(unsigned ms) {
  setUInt8(0x002c, ms/10);
}

bool
AnytoneCodeplug::FiveToneSettingsElement::hasPTTID() const {
  return 0 != pttID();
}
unsigned
AnytoneCodeplug::FiveToneSettingsElement::pttID() const {
  return getUInt8(0x002d);
}
void
AnytoneCodeplug::FiveToneSettingsElement::setPTTID(unsigned id) {
  setUInt8(0x002d, id);
}
void
AnytoneCodeplug::FiveToneSettingsElement::clearPTTID() {
  setPTTID(0);
}

unsigned
AnytoneCodeplug::FiveToneSettingsElement::autoResetTime() const {
  return ((unsigned)getUInt8(0x002e))*10;
}
void
AnytoneCodeplug::FiveToneSettingsElement::setAutoResetTime(unsigned s) {
  setUInt8(0x002e, s/10);
}

unsigned
AnytoneCodeplug::FiveToneSettingsElement::firstDelay() const {
  return ((unsigned)getUInt8(0x002f))*10;
}
void
AnytoneCodeplug::FiveToneSettingsElement::setFirstDelay(unsigned ms) {
  setUInt8(0x002f, ms/10);
}

bool
AnytoneCodeplug::FiveToneSettingsElement::sidetoneEnabled() const {
  return getUInt8(0x0030);
}
void
AnytoneCodeplug::FiveToneSettingsElement::enableSidetone(bool enable) {
  setUInt8(0x0030, (enable ? 0x01 : 0x00));
}

unsigned
AnytoneCodeplug::FiveToneSettingsElement::stopCode() const {
  return getUInt8(0x0032);
}
void
AnytoneCodeplug::FiveToneSettingsElement::setStopCode(unsigned code) {
  setUInt8(0x0032, code);
}

unsigned
AnytoneCodeplug::FiveToneSettingsElement::stopTime() const {
  return ((unsigned)getUInt8(0x0033))*10;
}
void
AnytoneCodeplug::FiveToneSettingsElement::setStopTime(unsigned ms) {
  setUInt8(0x0033, ms/10);
}

unsigned
AnytoneCodeplug::FiveToneSettingsElement::decodeTime() const {
  return ((unsigned)getUInt8(0x0034))*10;
}
void
AnytoneCodeplug::FiveToneSettingsElement::setDecodeTime(unsigned ms) {
  setUInt8(0x0034, ms/10);
}

unsigned
AnytoneCodeplug::FiveToneSettingsElement::delayAfterStop() const {
  return ((unsigned)getUInt8(0x0035))*10;
}
void
AnytoneCodeplug::FiveToneSettingsElement::setDelayAfterStop(unsigned ms) {
  setUInt8(0x0035, ms/10);
}

unsigned
AnytoneCodeplug::FiveToneSettingsElement::preTime() const {
  return ((unsigned)getUInt8(0x0036))*10;
}
void
AnytoneCodeplug::FiveToneSettingsElement::setPreTime(unsigned ms) {
  setUInt8(0x0036, ms/10);
}

AnytoneCodeplug::FiveToneSettingsElement::Standard
AnytoneCodeplug::FiveToneSettingsElement::botStandard() const {
  return (Standard) getUInt8(0x0041);
}
void
AnytoneCodeplug::FiveToneSettingsElement::setBOTStandard(Standard standard) {
  setUInt8(0x0041, (unsigned)standard);
}

unsigned
AnytoneCodeplug::FiveToneSettingsElement::botToneDuration() const {
  return getUInt8(0x0043);
}
void
AnytoneCodeplug::FiveToneSettingsElement::setBOTToneDuration(unsigned ms) {
  setUInt8(0x0043, ms);
}

QString
AnytoneCodeplug::FiveToneSettingsElement::botID() const {
  unsigned len = getUInt8(0x0042);
  QString id;
  for (unsigned i=0; i<len; i++) {
    uint8_t b = getUInt8(0x0044+(i/2));
    if (0 == (i%2))
      id.append(QString::number((b>>4)&0xf, 16));
    else
      id.append(QString::number((b>>0)&0xf, 16));
  }
  return id;
}
void
AnytoneCodeplug::FiveToneSettingsElement::setBOTID(const QString &id) {
  unsigned len = 0;
  for (int i=0; i<id.length(); i++) {
    bool ok;
    if (0 == (len%2)) {
      setUInt4(0x0044+len/2, 4, id.mid(i, 1).toUInt(&ok, 16));
    } else {
      setUInt4(0x0044+len/2, 0, id.mid(i, 1).toUInt(&ok, 16));
    }
    len++;
  }
  setUInt8(0x0042, len);
}

AnytoneCodeplug::FiveToneSettingsElement::Standard
AnytoneCodeplug::FiveToneSettingsElement::eotStandard() const {
  return (Standard) getUInt8(0x0061);
}
void
AnytoneCodeplug::FiveToneSettingsElement::setEOTStandard(Standard standard) {
  setUInt8(0x0061, (unsigned)standard);
}

unsigned
AnytoneCodeplug::FiveToneSettingsElement::eotToneDuration() const {
  return getUInt8(0x0063);
}
void
AnytoneCodeplug::FiveToneSettingsElement::setEOTToneDuration(unsigned ms) {
  setUInt8(0x0063, ms);
}

QString
AnytoneCodeplug::FiveToneSettingsElement::eotID() const {
  unsigned len = getUInt8(0x0062);
  QString id;
  for (unsigned i=0; i<len; i++) {
    uint8_t b = getUInt8(0x0064+(i/2));
    if (0 == (i%2))
      id.append(QString::number((b>>4)&0xf, 16));
    else
      id.append(QString::number((b>>0)&0xf, 16));
  }
  return id;
}
void
AnytoneCodeplug::FiveToneSettingsElement::setEOTID(const QString &id) {
  unsigned len = 0;
  for (int i=0; i<id.length(); i++) {
    bool ok;
    if (0 == (len%2)) {
      setUInt4(0x0064+len/2, 4, id.mid(i, 1).toUInt(&ok, 16));
    } else {
      setUInt4(0x0064+len/2, 0, id.mid(i, 1).toUInt(&ok, 16));
    }
    len++;
  }
  setUInt8(0x0062, len);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::TwoToneIDElement
 * ********************************************************************************************* */
AnytoneCodeplug::TwoToneIDElement::TwoToneIDElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::TwoToneIDElement::TwoToneIDElement(uint8_t *ptr)
  : Element(ptr, TwoToneIDElement::size())
{
  // pass...
}

void
AnytoneCodeplug::TwoToneIDElement::clear() {
  memset(_data, 0x00, _size);
}

double
AnytoneCodeplug::TwoToneIDElement::firstTone() const {
  return ((double)getUInt16_le(Offset::firstTone()))/10;
}
void
AnytoneCodeplug::TwoToneIDElement::setFirstTone(double f) {
  setUInt16_le(Offset::firstTone(), f*10);
}

double
AnytoneCodeplug::TwoToneIDElement::secondTone() const {
  return ((double)getUInt16_le(Offset::secondTone()))/10;
}
void
AnytoneCodeplug::TwoToneIDElement::setSecondTone(double f) {
  setUInt16_le(Offset::secondTone(), f*10);
}

QString
AnytoneCodeplug::TwoToneIDElement::name() const {
  return readASCII(Offset::name(), 7, 0x00);
}
void
AnytoneCodeplug::TwoToneIDElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::nameLength(), 0x00);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::TwoToneIDBitmapElement
 * ********************************************************************************************* */
AnytoneCodeplug::TwoToneIDBitmapElement::TwoToneIDBitmapElement(uint8_t *ptr, size_t size)
  : BitmapElement(ptr, size)
{
  // pass...
}

AnytoneCodeplug::TwoToneIDBitmapElement::TwoToneIDBitmapElement(uint8_t *ptr)
  : BitmapElement(ptr, TwoToneIDBitmapElement::size())
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::TwoToneFunctionElement
 * ********************************************************************************************* */
AnytoneCodeplug::TwoToneFunctionElement::TwoToneFunctionElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::TwoToneFunctionElement::TwoToneFunctionElement(uint8_t *ptr)
  : Element(ptr, TwoToneFunctionElement::size())
{
  // pass...
}

void
AnytoneCodeplug::TwoToneFunctionElement::clear() {
  memset(_data, 0x00, _size);
}

double
AnytoneCodeplug::TwoToneFunctionElement::firstTone() const {
  return ((double)getUInt16_le(Offset::firstTone()))/10;
}
void
AnytoneCodeplug::TwoToneFunctionElement::setFirstTone(double f) {
  setUInt16_le(Offset::firstTone(), f*10);
}

double
AnytoneCodeplug::TwoToneFunctionElement::secondTone() const {
  return ((double)getUInt16_le(Offset::secondTone()))/10;
}
void
AnytoneCodeplug::TwoToneFunctionElement::setSecondTone(double f) {
  setUInt16_le(Offset::secondTone(), f*10);
}

AnytoneCodeplug::TwoToneFunctionElement::Response
AnytoneCodeplug::TwoToneFunctionElement::response() const {
  return (Response) getUInt8(Offset::response());
}
void
AnytoneCodeplug::TwoToneFunctionElement::setResponse(Response resp) {
  setUInt8(Offset::response(), (unsigned)resp);
}

QString
AnytoneCodeplug::TwoToneFunctionElement::name() const {
  return readASCII(Offset::name(), Limit::nameLength(), 0x00);
}
void
AnytoneCodeplug::TwoToneFunctionElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::nameLength(), 0x00);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::TwoToneFunctionBitmapElement
 * ********************************************************************************************* */
AnytoneCodeplug::TwoToneFunctionBitmapElement::TwoToneFunctionBitmapElement(uint8_t *ptr, size_t size)
  : BitmapElement(ptr, size)
{
  // pass...
}

AnytoneCodeplug::TwoToneFunctionBitmapElement::TwoToneFunctionBitmapElement(uint8_t *ptr)
  : BitmapElement(ptr, TwoToneFunctionBitmapElement::size())
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::TwoToneSettingsElement
 * ********************************************************************************************* */
AnytoneCodeplug::TwoToneSettingsElement::TwoToneSettingsElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::TwoToneSettingsElement::TwoToneSettingsElement(uint8_t *ptr)
  : Element(ptr, TwoToneSettingsElement::size())
{
  // pass...
}

void
AnytoneCodeplug::TwoToneSettingsElement::clear() {
  memset(_data, 0x00, _size);
}

unsigned
AnytoneCodeplug::TwoToneSettingsElement::firstToneDuration() const {
  return ((unsigned)getUInt8(0x0009))*100;
}
void
AnytoneCodeplug::TwoToneSettingsElement::setFirstToneDuration(unsigned ms) {
  setUInt8(0x0009, ms/100);
}

unsigned
AnytoneCodeplug::TwoToneSettingsElement::secondToneDuration() const {
  return ((unsigned)getUInt8(0x000a))*100;
}
void
AnytoneCodeplug::TwoToneSettingsElement::setSecondToneDuration(unsigned ms) {
  setUInt8(0x000a, ms/100);
}

unsigned
AnytoneCodeplug::TwoToneSettingsElement::longToneDuration() const {
  return ((unsigned)getUInt8(0x000b))*100;
}
void
AnytoneCodeplug::TwoToneSettingsElement::setLongToneDuration(unsigned ms) {
  setUInt8(0x000b, ms/100);
}

unsigned
AnytoneCodeplug::TwoToneSettingsElement::gapDuration() const {
  return ((unsigned)getUInt8(0x000c))*100;
}
void
AnytoneCodeplug::TwoToneSettingsElement::setGapDuration(unsigned ms) {
  setUInt8(0x000c, ms/100);
}

unsigned
AnytoneCodeplug::TwoToneSettingsElement::autoResetTime() const {
  return ((unsigned)getUInt8(0x000d))*10;
}
void
AnytoneCodeplug::TwoToneSettingsElement::setAutoResetTime(unsigned sec) {
  setUInt8(0x000d, sec/10);
}

bool
AnytoneCodeplug::TwoToneSettingsElement::sidetone() const {
  return getUInt8(0x000e);
}
void
AnytoneCodeplug::TwoToneSettingsElement::enableSidetone(bool enable) {
  setUInt8(0x000e, (enable ? 0x01 : 0x00));
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::DTMFSettingsElement
 * ********************************************************************************************* */
AnytoneCodeplug::DTMFSettingsElement::DTMFSettingsElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::DTMFSettingsElement::DTMFSettingsElement(uint8_t *ptr)
  : Element(ptr, DTMFSettingsElement::size())
{
  // pass...
}

void
AnytoneCodeplug::DTMFSettingsElement::clear() {
  memset(_data, 0x00, _size);
}


unsigned
AnytoneCodeplug::DTMFSettingsElement::intervalSymbol() const {
  return getUInt8(0x0000);
}
void
AnytoneCodeplug::DTMFSettingsElement::setIntervalSymbol(unsigned symb) {
  setUInt8(0x0000, symb);
}

unsigned
AnytoneCodeplug::DTMFSettingsElement::groupCode() const {
  return getUInt8(0x0001);
}
void
AnytoneCodeplug::DTMFSettingsElement::setGroupCode(unsigned symb) {
  setUInt8(0x0001, symb);
}

AnytoneCodeplug::DTMFSettingsElement::Response
AnytoneCodeplug::DTMFSettingsElement::response() const {
  return (Response)getUInt8(0x0002);
}
void
AnytoneCodeplug::DTMFSettingsElement::setResponse(Response resp) {
  setUInt8(0x0002, (unsigned)resp);
}

unsigned
AnytoneCodeplug::DTMFSettingsElement::preTime() const {
  return ((unsigned)getUInt8(0x0003)*10);
}
void
AnytoneCodeplug::DTMFSettingsElement::setPreTime(unsigned ms) {
  setUInt8(0x0003, ms/10);
}

unsigned
AnytoneCodeplug::DTMFSettingsElement::firstDigitDuration() const {
  return ((unsigned)getUInt8(0x0004)*10);
}
void
AnytoneCodeplug::DTMFSettingsElement::setFirstDigitDuration(unsigned ms) {
  setUInt8(0x0004, ms/10);
}

unsigned
AnytoneCodeplug::DTMFSettingsElement::autoResetTime() const {
  return ((unsigned)getUInt8(0x0005)*10);
}
void
AnytoneCodeplug::DTMFSettingsElement::setAutoResetTime(unsigned sec) {
  setUInt8(0x0005, sec/10);
}

QString
AnytoneCodeplug::DTMFSettingsElement::id() const {
  QString id;
  for (int i=0; (i<3)&&(0xff!=getUInt8(0x0006+i)); i++) {
    id.append(QString::number(getUInt8(0x0006+i),16));
  }
  return id;
}
void
AnytoneCodeplug::DTMFSettingsElement::setID(const QString &id) {
  int len = std::min(3, id.length());
  bool ok;
  for (int i=0; i<len; i++)
    setUInt8(0x0006+i, id.mid(i,1).toUInt(&ok, 16));
}

unsigned
AnytoneCodeplug::DTMFSettingsElement::postEncodingDelay() const {
  return ((unsigned)getUInt8(0x0009)*10);
}
void
AnytoneCodeplug::DTMFSettingsElement::setPostEncodingDelay(unsigned ms) {
  setUInt8(0x0009, ms/10);
}

unsigned
AnytoneCodeplug::DTMFSettingsElement::pttIDPause() const {
  return ((unsigned)getUInt8(0x000a)*10);
}
void
AnytoneCodeplug::DTMFSettingsElement::setPTTIDPause(unsigned sec) {
  setUInt8(0x000a, sec/10);
}

bool
AnytoneCodeplug::DTMFSettingsElement::pttIDEnabled() const {
  return getUInt8(0x000b);
}
void
AnytoneCodeplug::DTMFSettingsElement::enablePTTID(bool enable) {
  setUInt8(0x000b, (enable ? 0x01 : 0x00));
}

unsigned
AnytoneCodeplug::DTMFSettingsElement::dCodePause() const {
  return ((unsigned)getUInt8(0x000c));
}
void
AnytoneCodeplug::DTMFSettingsElement::setDCodePause(unsigned sec) {
  setUInt8(0x000c, sec);
}

bool
AnytoneCodeplug::DTMFSettingsElement::sidetone() const {
  return getUInt8(0x000d);
}
void
AnytoneCodeplug::DTMFSettingsElement::enableSidetone(bool enable) {
  setUInt8(0x000d, (enable ? 0x01 : 0x00));
}

QString
AnytoneCodeplug::DTMFSettingsElement::botID() const {
  QString id;
  for (int i=0; (i<16)&&(0xff!=getUInt8(0x0010+i)); i++) {
    id.append(QString::number(getUInt8(0x0010+i),16));
  }
  return id;
}
void
AnytoneCodeplug::DTMFSettingsElement::setBOTID(const QString &id) {
  int len = std::min(16, id.length());
  bool ok;
  for (int i=0; i<len; i++)
    setUInt8(0x0010+i, id.mid(i,1).toUInt(&ok, 16));
}

QString
AnytoneCodeplug::DTMFSettingsElement::eotID() const {
  QString id;
  for (int i=0; (i<16)&&(0xff!=getUInt8(0x0020+i)); i++) {
    id.append(QString::number(getUInt8(0x0020+i),16));
  }
  return id;
}
void
AnytoneCodeplug::DTMFSettingsElement::setEOTID(const QString &id) {
  int len = std::min(16, id.length());
  bool ok;
  for (int i=0; i<len; i++)
    setUInt8(0x0020+i, id.mid(i,1).toUInt(&ok, 16));
}

QString
AnytoneCodeplug::DTMFSettingsElement::remoteKillID() const {
  QString id;
  for (int i=0; (i<16)&&(0xff!=getUInt8(0x0030+i)); i++) {
    id.append(QString::number(getUInt8(0x0030+i),16));
  }
  return id;
}
void
AnytoneCodeplug::DTMFSettingsElement::setRemoteKillID(const QString &id) {
  int len = std::min(16, id.length());
  bool ok;
  for (int i=0; i<len; i++)
    setUInt8(0x0030+i, id.mid(i,1).toUInt(&ok, 16));
}


QString
AnytoneCodeplug::DTMFSettingsElement::remoteStunID() const {
  QString id;
  for (int i=0; (i<16)&&(0xff!=getUInt8(0x0040+i)); i++) {
    id.append(QString::number(getUInt8(0x0040+i),16));
  }
  return id;
}
void
AnytoneCodeplug::DTMFSettingsElement::setRemoteStunID(const QString &id) {
  int len = std::min(16, id.length());
  bool ok;
  for (int i=0; i<len; i++)
    setUInt8(0x0040+i, id.mid(i,1).toUInt(&ok, 16));
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::DTMFIDListElement
 * ********************************************************************************************* */
AnytoneCodeplug::DTMFIDListElement::DTMFIDListElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::DTMFIDListElement::DTMFIDListElement(uint8_t *ptr)
  : Element(ptr, DTMFIDListElement::size())
{
  // pass...
}

void
AnytoneCodeplug::DTMFIDListElement::clear() {
  memset(_data, 0xff, _size);
}

bool
AnytoneCodeplug::DTMFIDListElement::hasNumber(unsigned int n) const {
  if (n >= Limit::numEntries())
    return false;
  return 0xff != getUInt8(n*Limit::numberLength());
}

QString
AnytoneCodeplug::DTMFIDListElement::number(unsigned int n) const {
  if (n >= Limit::numEntries())
    return "";
  uint8_t *num = _data + n*Limit::numberLength();
  return decode_dtmf_bin(num, Limit::numberLength(), 0xff);
}

void
AnytoneCodeplug::DTMFIDListElement::setNumber(unsigned int n, const QString &number) {
  if (n >= Limit::numEntries())
    return;
  uint8_t *num = _data + n*Limit::numberLength();
  encode_dtmf_bin(number, num, Limit::numberLength(), 0xff);
}

void
AnytoneCodeplug::DTMFIDListElement::clearNumber(unsigned int n) {
  if (n >= Limit::numEntries())
    return;
  uint8_t *num = _data + n*Limit::numberLength();
  memset(num, 0xff, Limit::numberLength());
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::WFMChannelListElement
 * ********************************************************************************************* */
AnytoneCodeplug::WFMChannelListElement::WFMChannelListElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::WFMChannelListElement::WFMChannelListElement(uint8_t *ptr)
  : Element(ptr, WFMChannelListElement::size())
{
  // pass...
}

void
AnytoneCodeplug::WFMChannelListElement::clear() {
  memset(_data, 0x00, _size);
}

bool
AnytoneCodeplug::WFMChannelListElement::hasChannel(unsigned int n) const {
  if (n >= Limit::numEntries())
    return false;
  return 0 != getBCD8_le(n*Offset::betweenChannels());
}

Frequency
AnytoneCodeplug::WFMChannelListElement::channel(unsigned int n) const {
  if (n >= Limit::numEntries())
    return Frequency();
  return Frequency::fromHz(((unsigned long long)getBCD8_le(n*Offset::betweenChannels()))*100);
}

void
AnytoneCodeplug::WFMChannelListElement::setChannel(unsigned int n, Frequency freq) {
  if (n >= Limit::numEntries())
    return;
  setBCD8_le(n*Offset::betweenChannels(), freq.inHz()/100);
}

void
AnytoneCodeplug::WFMChannelListElement::clearChannel(unsigned int n) {
  if (n >= Limit::numEntries())
    return;
  setBCD8_le(n*Offset::betweenChannels(), 0);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::WFMChannelBitmapElement
 * ********************************************************************************************* */
AnytoneCodeplug::WFMChannelBitmapElement::WFMChannelBitmapElement(uint8_t *ptr, size_t size)
  : BitmapElement(ptr, size)
{
  // pass...
}

AnytoneCodeplug::WFMChannelBitmapElement::WFMChannelBitmapElement(uint8_t *ptr)
  : BitmapElement(ptr, WFMChannelBitmapElement::size())
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::WFMVFOElement
 * ********************************************************************************************* */
AnytoneCodeplug::WFMVFOElement::WFMVFOElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::WFMVFOElement::WFMVFOElement(uint8_t *ptr)
  : Element(ptr, WFMVFOElement::size())
{
  // pass...
}

void
AnytoneCodeplug::WFMVFOElement::clear() {
  memset(_data, 0x00, _size);
  setFrequency(Frequency::fromMHz(88));
}

Frequency
AnytoneCodeplug::WFMVFOElement::frequency() const {
  return Frequency::fromHz(((unsigned long long)getBCD8_le(0))*100);
}

void
AnytoneCodeplug::WFMVFOElement::setFrequency(Frequency freq) {
  setBCD8_le(0, freq.inHz()/100);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::DMREncryptionKeyIDListElement
 * ********************************************************************************************* */
AnytoneCodeplug::DMREncryptionKeyIDListElement::DMREncryptionKeyIDListElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::DMREncryptionKeyIDListElement::DMREncryptionKeyIDListElement(uint8_t *ptr)
  : Element(ptr, DMREncryptionKeyIDListElement::size())
{
  // pass...
}

void
AnytoneCodeplug::DMREncryptionKeyIDListElement::clear() {
  memset(_data, 0xff, _size);
}

bool
AnytoneCodeplug::DMREncryptionKeyIDListElement::hasID(unsigned int n) const {
  if (n >= Limit::numEntries())
    return false;
  return 0xffff == getUInt16_be(n*Offset::betweenIDs());
}

uint16_t
AnytoneCodeplug::DMREncryptionKeyIDListElement::id(unsigned int n) const {
  if (n >= Limit::numEntries())
    return 0xffff;
  return getUInt16_be(n*Offset::betweenIDs());
}

void
AnytoneCodeplug::DMREncryptionKeyIDListElement::setID(unsigned int n, uint16_t id) {
  if (n >= Limit::numEntries())
    return;
  setUInt16_be(n*Offset::betweenIDs(), id);
}

void
AnytoneCodeplug::DMREncryptionKeyIDListElement::clearID(unsigned int n) {
  if (n >= Limit::numEntries())
    return;
  setUInt16_be(n*Offset::betweenIDs(), 0xffff);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::DMREncryptionKeyListElement
 * ********************************************************************************************* */
AnytoneCodeplug::DMREncryptionKeyListElement::DMREncryptionKeyListElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::DMREncryptionKeyListElement::DMREncryptionKeyListElement(uint8_t *ptr)
  : Element(ptr, DMREncryptionKeyListElement::size())
{
  // pass...
}

void
AnytoneCodeplug::DMREncryptionKeyListElement::clear() {
  memset(_data, 0x00, _size);
  for (unsigned int i=0; i<Limit::numEntries(); i++) {
    setKey(i, QByteArray::fromHex("FFFF"));
  }
}

QByteArray
AnytoneCodeplug::DMREncryptionKeyListElement::key(unsigned int n) const {
  if (n >= Limit::numEntries())
    return QByteArray();
  return QByteArray::fromRawData((const char *)_data + Offset::keys() + n*Offset::betweenKeys(), 2);
}

void
AnytoneCodeplug::DMREncryptionKeyListElement::setKey(unsigned int n, const QByteArray &key) {
  if ((n >= Limit::numEntries()) || (2 != key.size()))
    return;
  memcpy(_data + Offset::keys() + n*Offset::betweenKeys(), key.constData(), 2);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::ContactMapElement
 * ********************************************************************************************* */
AnytoneCodeplug::ContactMapElement::ContactMapElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::ContactMapElement::ContactMapElement(uint8_t *ptr)
  : Element(ptr, ContactMapElement::size())
{
  // pass...
}

void
AnytoneCodeplug::ContactMapElement::clear() {
  memset(_data, 0xff, _size);
}

bool
AnytoneCodeplug::ContactMapElement::isValid() const {
  return (0xffffffff!=getUInt32_le(0x0000)) && (0xffffffff!=getUInt32_le(0x0004));
}

bool
AnytoneCodeplug::ContactMapElement::isGroup() const {
  uint32_t tmp = getUInt32_le(0x0000);
  return tmp & 0x01;
}
unsigned
AnytoneCodeplug::ContactMapElement::id() const {
  uint32_t tmp = getUInt32_le(0x0000);
  tmp = tmp>>1;
  return decode_dmr_id_bcd_le((uint8_t *)&tmp);
}
void
AnytoneCodeplug::ContactMapElement::setID(unsigned id, bool group) {
  uint32_t tmp; encode_dmr_id_bcd_le((uint8_t *)&tmp, id);
  tmp = ( (tmp << 1) | (group ? 1 : 0) );
  setUInt32_le(0x0000, tmp);
}

unsigned
AnytoneCodeplug::ContactMapElement::index() const {
  return getUInt32_le(0x0004);
}
void
AnytoneCodeplug::ContactMapElement::setIndex(unsigned idx) {
  setUInt32_le(0x0004, idx);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug
 * ********************************************************************************************* */
AnytoneCodeplug::AnytoneCodeplug(const QString &label, QObject *parent)
  : Codeplug(parent), _label(label)
{
  // pass...
}

AnytoneCodeplug::~AnytoneCodeplug() {
  // pass...
}

void
AnytoneCodeplug::clear() {
  while (this->numImages())
    remImage(0);

  addImage(_label);

  // Allocate bitmaps
  this->allocateBitmaps();
}

bool
AnytoneCodeplug::index(Config *config, Context &ctx, const ErrorStack &err) const {
  Q_UNUSED(err)

  // All indices as 0-based. That is, the first channel gets index 0 etc.

  // Map radio IDs
  for (int i=0; i<config->radioIDs()->count(); i++)
    ctx.add(config->radioIDs()->getId(i), i);

  // Map digital and DTMF contacts
  for (int i=0, d=0, a=0; i<config->contacts()->count(); i++) {
    if (config->contacts()->contact(i)->is<DMRContact>()) {
      ctx.add(config->contacts()->contact(i)->as<DMRContact>(), d); d++;
    } else if (config->contacts()->contact(i)->is<DTMFContact>()) {
      ctx.add(config->contacts()->contact(i)->as<DTMFContact>(), a); a++;
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

  // Map DMR APRS systems
  for (int i=0,a=0,d=0; i<config->posSystems()->count(); i++) {
    if (config->posSystems()->system(i)->is<GPSSystem>()) {
      ctx.add(config->posSystems()->system(i)->as<GPSSystem>(), d); d++;
    } else if (config->posSystems()->system(i)->is<APRSSystem>()) {
      ctx.add(config->posSystems()->system(i)->as<APRSSystem>(), a); a++;
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

  return true;
}

bool
AnytoneCodeplug::encode(Config *config, const Flags &flags, const ErrorStack &err) {
  Context ctx(config);
  // Register table for auto-repeater offsets
  ctx.addTable(&AnytoneAutoRepeaterOffset::staticMetaObject);

  if (! index(config, ctx, err)) {
    errMsg(err) << "Cannot encode anytone codeplug.";
    return false;
  }

  // If codeplug is generated from scratch -> clear and reallocate
  if (! flags.updateCodePlug) {
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
AnytoneCodeplug::decode(Config *config, const ErrorStack &err) {
  // Maps code-plug indices to objects
  Context ctx(config);
  // Register table for auto-repeater offsets
  ctx.addTable(&AnytoneAutoRepeaterOffset::staticMetaObject);

  return this->decodeElements(ctx, err);
}
