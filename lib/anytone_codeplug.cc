#include "anytone_codeplug.hh"
#include "utils.hh"
using namespace Signaling;

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
 * Implementation of AnytoneCodeplug::ChannelElement
 * ********************************************************************************************* */
AnytoneCodeplug::ChannelElement::ChannelElement(uint8_t *ptr, uint size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::ChannelElement::ChannelElement(uint8_t *ptr)
  : Element(ptr, 0x0040)
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
  setBandwidth(AnalogChannel::Bandwidth::Narrow);
  setBit(0x0008, 5, false); // Unused set to 0
}

uint
AnytoneCodeplug::ChannelElement::rxFrequency() const {
  return ((uint)getBCD8_be(0x0000))*10;
}
void
AnytoneCodeplug::ChannelElement::setRXFrequency(uint hz) {
  setBCD8_be(0x0000, hz/10);
}

uint
AnytoneCodeplug::ChannelElement::txOffset() const {
  return ((uint)getBCD8_be(0x0004))*10;
}
void
AnytoneCodeplug::ChannelElement::setTXOffset(uint hz) {
  setBCD8_be(0x0004, hz/10);
}

uint
AnytoneCodeplug::ChannelElement::txFrequency() const {
  uint rx = rxFrequency(), off = txOffset();
  if (RepeaterMode::Simplex == repeaterMode())
    return rx;
  else if (RepeaterMode::Positive == repeaterMode())
    return rx+off;
  return rx-off;
}
void
AnytoneCodeplug::ChannelElement::setTXFrequency(uint hz) {
  uint rx = rxFrequency();
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
  setUInt2(0x0008, 0, (uint)mode);
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
    setUInt2(0x0008, 2, (uint)POWER_LOW);
    break;
  case Channel::Power::Mid:
    setUInt2(0x0008, 2, (uint)POWER_MIDDLE);
    break;
  case Channel::Power::High:
    setUInt2(0x0008, 2, (uint)POWER_HIGH);
    break;
  case Channel::Power::Max:
    setUInt2(0x0008, 2, (uint)POWER_TURBO);
    break;
  }
}

AnalogChannel::Bandwidth
AnytoneCodeplug::ChannelElement::bandwidth() const {
  if (getBit(0x0008, 4))
    return AnalogChannel::Bandwidth::Wide;
  return AnalogChannel::Bandwidth::Narrow;
}
void
AnytoneCodeplug::ChannelElement::setBandwidth(AnalogChannel::Bandwidth bw) {
  switch (bw) {
  case AnalogChannel::Bandwidth::Narrow: setBit(0x0008, 4, false); break;
  case AnalogChannel::Bandwidth::Wide: setBit(0x0008, 4, true); break;
  }
}

AnytoneCodeplug::ChannelElement::RepeaterMode
AnytoneCodeplug::ChannelElement::repeaterMode() const {
  return (RepeaterMode)getUInt2(0x0008, 6);
}
void
AnytoneCodeplug::ChannelElement::setRepeaterMode(RepeaterMode mode) {
  setUInt2(0x0008, 6, (uint)mode);
}

AnytoneCodeplug::ChannelElement::SignalingMode
AnytoneCodeplug::ChannelElement::rxSignalingMode() const {
  return (SignalingMode)getUInt2(0x0009, 0);
}
void
AnytoneCodeplug::ChannelElement::setRXSignalingMode(SignalingMode mode) {
  setUInt2(0x0009, 0, (uint)mode);
}

AnytoneCodeplug::ChannelElement::SignalingMode
AnytoneCodeplug::ChannelElement::txSignalingMode() const {
  return (SignalingMode)getUInt2(0x0009, 2);
}
void
AnytoneCodeplug::ChannelElement::setTXSignalingMode(SignalingMode mode) {
  setUInt2(0x0009, 2, (uint)mode);
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

Signaling::Code
AnytoneCodeplug::ChannelElement::txCTCSS() const {
  return ctcss_num2code(getUInt8(0x000a));
}
void
AnytoneCodeplug::ChannelElement::setTXCTCSS(Code tone) {
  setUInt8(0x000a, ctcss_code2num(tone));
}
Signaling::Code
AnytoneCodeplug::ChannelElement::rxCTCSS() const {
  return ctcss_num2code(getUInt8(0x000b));
}
void
AnytoneCodeplug::ChannelElement::setRXCTCSS(Code tone) {
  setUInt8(0x000b, ctcss_code2num(tone));
}
Signaling::Code
AnytoneCodeplug::ChannelElement::txDCS() const {
  uint16_t code = getUInt16_le(0x000c);
  if (512 < code)
    return Signaling::fromDCSNumber(dec_to_oct(code), false);
  return Signaling::fromDCSNumber(dec_to_oct(code-512), true);
}
void
AnytoneCodeplug::ChannelElement::setTXDCS(Code code) {
  if (Signaling::isDCSNormal(code))
    setUInt8(0x000c, oct_to_dec(Signaling::toDCSNumber(code)));
  else if (Signaling::isDCSInverted(code))
    setUInt8(0x000c, oct_to_dec(Signaling::toDCSNumber(code))+512);
  else
    setUInt8(0x000c, 0);
}

Signaling::Code
AnytoneCodeplug::ChannelElement::rxDCS() const {
  uint16_t code = getUInt16_le(0x000e);
  if (512 < code)
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

uint
AnytoneCodeplug::ChannelElement::twoToneDecodeIndex() const {
  return getUInt16_le(0x0012);
}
void
AnytoneCodeplug::ChannelElement::setTwoToneDecodeIndex(uint idx) {
  setUInt16_le(0x0012, idx);
}

uint
AnytoneCodeplug::ChannelElement::contactIndex() const {
  return getUInt32_le(0x0014);
}
void
AnytoneCodeplug::ChannelElement::setContactIndex(uint idx) {
  return setUInt32_le(0x0014, idx);
}

uint
AnytoneCodeplug::ChannelElement::radioIDIndex() const {
  return getUInt8(0x0018);
}
void
AnytoneCodeplug::ChannelElement::setRadioIDIndex(uint idx) {
  return setUInt8(0x0018, idx);
}

bool
AnytoneCodeplug::ChannelElement::silentSquelch() const {
  return getBit(0x0019, 4);
}
void
AnytoneCodeplug::ChannelElement::enableSilentSquelch(bool enable) {
  setBit(0x0019, 4, enable);
}

AnytoneCodeplug::ChannelElement::Admit
AnytoneCodeplug::ChannelElement::admit() const {
  return (Admit)getUInt2(0x001a, 0);
}
void
AnytoneCodeplug::ChannelElement::setAdmit(Admit admit) {
  setUInt2(0x001a, 0, (uint)admit);
}

AnytoneCodeplug::ChannelElement::OptSignaling
AnytoneCodeplug::ChannelElement::optionalSignaling() const {
  return (OptSignaling)getUInt2(0x001a, 4);
}
void
AnytoneCodeplug::ChannelElement::setOptionalSignaling(OptSignaling sig) {
  setUInt2(0x001a, 4, (uint)sig);
}

bool
AnytoneCodeplug::ChannelElement::hasScanListIndex() const {
  return 0xff != scanListIndex();
}
uint
AnytoneCodeplug::ChannelElement::scanListIndex() const {
  return getUInt8(0x001b);
}
void
AnytoneCodeplug::ChannelElement::setScanListIndex(uint idx) {
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
uint
AnytoneCodeplug::ChannelElement::groupListIndex() const {
  return getUInt8(0x001c);
}
void
AnytoneCodeplug::ChannelElement::setGroupListIndex(uint idx) {
  setUInt8(0x001c, idx);
}
void
AnytoneCodeplug::ChannelElement::clearGroupListIndex() {
  setGroupListIndex(0xff);
}

uint
AnytoneCodeplug::ChannelElement::twoToneIDIndex() const {
  return getUInt8(0x001d);
}
void
AnytoneCodeplug::ChannelElement::setTwoToneIDIndex(uint idx) {
  setUInt8(0x001d, idx);
}
uint
AnytoneCodeplug::ChannelElement::fiveToneIDIndex() const {
  return getUInt8(0x001e);
}
void
AnytoneCodeplug::ChannelElement::setFiveToneIDIndex(uint idx) {
  setUInt8(0x001e, idx);
}
uint
AnytoneCodeplug::ChannelElement::dtmfIDIndex() const {
  return getUInt8(0x001f);
}
void
AnytoneCodeplug::ChannelElement::setDTMFIDIndex(uint idx) {
  setUInt8(0x001f, idx);
}

uint
AnytoneCodeplug::ChannelElement::colorCode() const {
  return getUInt8(0x0020);
}
void
AnytoneCodeplug::ChannelElement::setColorCode(uint code) {
  setUInt8(0x0020, code);
}

DigitalChannel::TimeSlot
AnytoneCodeplug::ChannelElement::timeSlot() const {
  if (false == getBit(0x0021, 0))
    return DigitalChannel::TimeSlot::TS1;
  return DigitalChannel::TimeSlot::TS2;
}
void
AnytoneCodeplug::ChannelElement::setTimeSlot(DigitalChannel::TimeSlot ts) {
  if (DigitalChannel::TimeSlot::TS1 == ts)
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
uint
AnytoneCodeplug::ChannelElement::encryptionKeyIndex() const {
  return getUInt8(0x0022);
}
void
AnytoneCodeplug::ChannelElement::setEncryptionKeyIndex(uint idx) {
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

bool
AnytoneCodeplug::ChannelElement::ranging() const {
  return getBit(0x0034, 0);
}
void
AnytoneCodeplug::ChannelElement::enableRanging(bool enable) {
  setBit(0x0034, 0, enable);
}
bool
AnytoneCodeplug::ChannelElement::throughMode() const {
  return getBit(0x0034, 1);
}
void
AnytoneCodeplug::ChannelElement::enableThroughMode(bool enable) {
  setBit(0x0034, 1, enable);
}
bool
AnytoneCodeplug::ChannelElement::dataACK() const {
  return !getBit(0x0034, 2);
}
void
AnytoneCodeplug::ChannelElement::enableDataACK(bool enable) {
  setBit(0x0034, 2, !enable);
}

bool
AnytoneCodeplug::ChannelElement::txAPRS() const {
  return getBit(0x0035, 0);
}
void
AnytoneCodeplug::ChannelElement::enableTXAPRS(bool enable) {
  setBit(0x0035, 0, enable);
}
uint
AnytoneCodeplug::ChannelElement::dmrAPRSSystemIndex() const {
  return getUInt8(0x0036);
}
void
AnytoneCodeplug::ChannelElement::setDMRAPRSSystemIndex(uint idx) {
  setUInt8(0x0036, idx);
}

uint
AnytoneCodeplug::ChannelElement::dmrEncryptionKeyIndex() const {
  return getUInt8(0x003a);
}
void
AnytoneCodeplug::ChannelElement::setDMREncryptionKeyIndex(uint idx) {
  setUInt8(0x003a, idx);
}

bool
AnytoneCodeplug::ChannelElement::multipleKeyEncryption() const {
  return getBit(0x003b, 0);
}
void
AnytoneCodeplug::ChannelElement::enableMultipleKeyEncryption(bool enable) {
  setBit(0x003b, 0, enable);
}

bool
AnytoneCodeplug::ChannelElement::randomKey() const {
  return getBit(0x003b, 1);
}
void
AnytoneCodeplug::ChannelElement::enableRandomKey(bool enable) {
  setBit(0x003b, 1, enable);
}
bool
AnytoneCodeplug::ChannelElement::sms() const {
  return !getBit(0x003b, 2);
}
void
AnytoneCodeplug::ChannelElement::enableSMS(bool enable) {
  setBit(0x003b, 0, !enable);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::ContactElement
 * ********************************************************************************************* */
AnytoneCodeplug::ContactElement::ContactElement(uint8_t *ptr, uint size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::ContactElement::ContactElement(uint8_t *ptr)
  : Element(ptr, 0x0064)
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

DigitalContact::Type
AnytoneCodeplug::ContactElement::type() const {
  switch (getUInt8(0x0000)) {
  case 0: return DigitalContact::PrivateCall;
  case 1: return DigitalContact::GroupCall;
  case 2: return DigitalContact::AllCall;
  }
  return DigitalContact::PrivateCall;
}
void
AnytoneCodeplug::ContactElement::setType(DigitalContact::Type type) {
  switch (type) {
  case DigitalContact::PrivateCall: setUInt8(0x0000, 0); break;
  case DigitalContact::GroupCall: setUInt8(0x0000, 1); break;
  case DigitalContact::AllCall: setUInt8(0x0000, 2); break;
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

uint
AnytoneCodeplug::ContactElement::number() const {
  return getBCD8_le(0x0023);
}
void
AnytoneCodeplug::ContactElement::setNumber(uint number) {
  setBCD8_le(0x0023, number);
}

AnytoneCodeplug::ContactElement::AlertType
AnytoneCodeplug::ContactElement::alertType() const {
  return (AlertType) getUInt8(0x0027);
}
void
AnytoneCodeplug::ContactElement::setAlertType(AlertType type) {
  setUInt8(0x0027, (uint)type);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::DTMFContactElement
 * ********************************************************************************************* */
AnytoneCodeplug::DTMFContactElement::DTMFContactElement(uint8_t *ptr, uint size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::DTMFContactElement::DTMFContactElement(uint8_t *ptr)
  : Element(ptr, 0x30)
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
  int n = getUInt8(0x0013);
  for (int i=0; i<n; i++) {
    uint8_t byte = _data[i/2];
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
  memset(_data+0x0000, 0, 7);
  setUInt8(0x0013, number.length());
  for (int i=0; i<number.length(); i++) {
    if (0 == (i%2))
      _data[i/2] |= (_anytone_bin_dtmf_tab.indexOf(number[i].toLatin1())<<4);
    else
      _data[i/2] |= (_anytone_bin_dtmf_tab.indexOf(number[i].toLatin1())<<0);
  }
}

QString
AnytoneCodeplug::DTMFContactElement::name() const {
  return readASCII(0x0020, 15, 0x00);
}
void
AnytoneCodeplug::DTMFContactElement::setName(const QString &name) {
  writeASCII(0x0020, name, 15, 0x00);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::GroupListElement
 * ********************************************************************************************* */
AnytoneCodeplug::GroupListElement::GroupListElement(uint8_t *ptr, uint size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::GroupListElement::GroupListElement(uint8_t *ptr)
  : Element(ptr, 0x120)
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
AnytoneCodeplug::GroupListElement::hasMemberIndex(uint n) const {
  return 0xffffffff != memberIndex(n);
}
uint
AnytoneCodeplug::GroupListElement::memberIndex(uint n) const {
  return getUInt32_le(0x0000 + 4*n);
}
void
AnytoneCodeplug::GroupListElement::setMemberIndex(uint n, uint idx) {
  setUInt32_le(0x0000 + 4*n, idx);
}
void
AnytoneCodeplug::GroupListElement::clearMemberIndex(uint n) {
  setMemberIndex(n, 0xffffffff);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug::ScanListElement
 * ********************************************************************************************* */
AnytoneCodeplug::ScanListElement::ScanListElement(uint8_t *ptr, uint size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneCodeplug::ScanListElement::ScanListElement(uint8_t *ptr)
  : Element(ptr, 0x0090)
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
  setUInt8(0x0001, (uint)sel);
}

bool
AnytoneCodeplug::ScanListElement::hasPrimary() const {
  return 0xffff != getUInt16_le(0x0002);
}
bool
AnytoneCodeplug::ScanListElement::primaryIsSelected() const {
  return 0 == getUInt16_le(0x0002);
}
uint
AnytoneCodeplug::ScanListElement::primary() const {
  return ((uint)getUInt16_le(0x0002))-1;
}
void
AnytoneCodeplug::ScanListElement::setPrimary(uint idx) {
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
uint
AnytoneCodeplug::ScanListElement::secondary() const {
  return ((uint)getUInt16_le(0x0002))-1;
}
void
AnytoneCodeplug::ScanListElement::setSecondary(uint idx) {
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

uint
AnytoneCodeplug::ScanListElement::lookBackTimeA() const {
  return ((uint)getUInt16_le(0x0006))*10;
}
void
AnytoneCodeplug::ScanListElement::setLookBackTimeA(uint sec) {
  setUInt16_le(0x0006, sec/10);
}
uint
AnytoneCodeplug::ScanListElement::lookBackTimeB() const {
  return ((uint)getUInt16_le(0x0008))*10;
}
void
AnytoneCodeplug::ScanListElement::setLookBackTimeB(uint sec) {
  setUInt16_le(0x0008, sec/10);
}
uint
AnytoneCodeplug::ScanListElement::dropOutDelay() const {
  return ((uint)getUInt16_le(0x000a))*10;
}
void
AnytoneCodeplug::ScanListElement::setDropOutDelay(uint sec) {
  setUInt16_le(0x000a, sec/10);
}
uint
AnytoneCodeplug::ScanListElement::dwellTime() const {
  return ((uint)getUInt16_le(0x000c))*10;
}
void
AnytoneCodeplug::ScanListElement::setDwellTime(uint sec) {
  setUInt16_le(0x000c, sec/10);
}

AnytoneCodeplug::ScanListElement::RevertChannel
AnytoneCodeplug::ScanListElement::revertChannel() const {
  return (RevertChannel)getUInt8(0x000e);
}
void
AnytoneCodeplug::ScanListElement::setRevertChannel(RevertChannel type) {
  setUInt8(0x000e, (uint)type);
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
AnytoneCodeplug::ScanListElement::hasMemberIndex(uint n) const {
  return 0xffff != memberIndex(n);
}
uint
AnytoneCodeplug::ScanListElement::memberIndex(uint n) const {
  return getUInt16_le(0x0020+2*n);
}
void
AnytoneCodeplug::ScanListElement::setMemberIndex(uint n, uint idx) {
  setUInt16_le(0x0020+2*n, idx);
}
void
AnytoneCodeplug::ScanListElement::clearMemberIndex(uint n) {
  setMemberIndex(n, 0xffff);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplug
 * ********************************************************************************************* */
AnytoneCodeplug::AnytoneCodeplug(QObject *parent)
  : Codeplug(parent)
{
  // pass...
}

AnytoneCodeplug::~AnytoneCodeplug() {
  // pass...
}

bool
AnytoneCodeplug::index(Config *config, Context &ctx) const {
  return true;
}


