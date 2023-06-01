#include "d868uv_codeplug.hh"
#include "config.hh"
#include "utils.hh"
#include "channel.hh"
#include "gpssystem.hh"
#include "userdatabase.hh"
#include "config.h"
#include "logger.hh"
#include "utils.hh"
#include <cmath>

#include <QTimeZone>
#include <QtEndian>
#include <QSet>

using namespace Signaling;

Code _ctcss_num2code[52] = {
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

uint8_t
D868UVCodeplug::ctcss_code2num(Signaling::Code code) {
  for (uint8_t i=0; i<52; i++) {
    if (code == _ctcss_num2code[i])
      return i;
  }
  return 0;
}

Signaling::Code
D868UVCodeplug::ctcss_num2code(uint8_t num) {
  if (num >= 52)
    return Signaling::SIGNALING_NONE;
  return _ctcss_num2code[num];
}


/* ******************************************************************************************** *
 * Implementation of D868UVCodeplug::ChannelElement
 * ******************************************************************************************** */
D868UVCodeplug::ChannelElement::ChannelElement(uint8_t *ptr, unsigned size)
  : AnytoneCodeplug::ChannelElement(ptr, size)
{
  // pass...
}

D868UVCodeplug::ChannelElement::ChannelElement(uint8_t *ptr)
  : AnytoneCodeplug::ChannelElement(ptr)
{
  // pass...
}

bool
D868UVCodeplug::ChannelElement::ranging() const {
  return getBit(0x0034, 0);
}
void
D868UVCodeplug::ChannelElement::enableRanging(bool enable) {
  setBit(0x0034, 0, enable);
}

bool
D868UVCodeplug::ChannelElement::throughMode() const {
  return getBit(0x0034, 1);
}
void
D868UVCodeplug::ChannelElement::enableThroughMode(bool enable) {
  setBit(0x0034, 1, enable);
}

bool
D868UVCodeplug::ChannelElement::dataACK() const {
  return !getBit(0x0034, 2);
}
void
D868UVCodeplug::ChannelElement::enableDataACK(bool enable) {
  setBit(0x0034, 2, !enable);
}

bool
D868UVCodeplug::ChannelElement::txDigitalAPRS() const {
  return getBit(0x0035, 0);
}
void
D868UVCodeplug::ChannelElement::enableTXDigitalAPRS(bool enable) {
  setBit(0x0035, 0, enable);
}
unsigned
D868UVCodeplug::ChannelElement::digitalAPRSSystemIndex() const {
  return getUInt8(0x0036);
}
void
D868UVCodeplug::ChannelElement::setDigitalAPRSSystemIndex(unsigned idx) {
  setUInt8(0x0036, idx);
}

unsigned
D868UVCodeplug::ChannelElement::dmrEncryptionKeyIndex() const {
  return getUInt8(0x003a);
}
void
D868UVCodeplug::ChannelElement::setDMREncryptionKeyIndex(unsigned idx) {
  setUInt8(0x003a, idx);
}

bool
D868UVCodeplug::ChannelElement::multipleKeyEncryption() const {
  return getBit(0x003b, 0);
}
void
D868UVCodeplug::ChannelElement::enableMultipleKeyEncryption(bool enable) {
  setBit(0x003b, 0, enable);
}

bool
D868UVCodeplug::ChannelElement::randomKey() const {
  return getBit(0x003b, 1);
}
void
D868UVCodeplug::ChannelElement::enableRandomKey(bool enable) {
  setBit(0x003b, 1, enable);
}
bool
D868UVCodeplug::ChannelElement::sms() const {
  return !getBit(0x003b, 2);
}
void
D868UVCodeplug::ChannelElement::enableSMS(bool enable) {
  setBit(0x003b, 0, !enable);
}

Channel *
D868UVCodeplug::ChannelElement::toChannelObj(Context &ctx) const {
  Channel *ch = AnytoneCodeplug::ChannelElement::toChannelObj(ctx);
  if (nullptr == ch)
    return nullptr;

  if (ch->is<DMRChannel>()) {
    DMRChannel *dch = ch->as<DMRChannel>();
    if (AnytoneDMRChannelExtension *ext = dch->anytoneChannelExtension()) {
      ext->enableSMS(sms());
      ext->enableDataACK(dataACK());
      ext->enableThroughMode(throughMode());
    }
  }

  return ch;
}

bool
D868UVCodeplug::ChannelElement::linkChannelObj(Channel *c, Context &ctx) const {
  if (! AnytoneCodeplug::ChannelElement::linkChannelObj(c, ctx))
    return false;

  if (c->is<DMRChannel>()) {
    DMRChannel *dc = c->as<DMRChannel>();
    // Link to GPS system
    if (txDigitalAPRS() && (! ctx.has<GPSSystem>(digitalAPRSSystemIndex())))
      logWarn() << "Cannot link to DMR APRS system index " << digitalAPRSSystemIndex() << ": undefined DMR APRS system.";
    else if (ctx.has<GPSSystem>(digitalAPRSSystemIndex()))
      dc->setAPRSObj(ctx.get<GPSSystem>(digitalAPRSSystemIndex()));
  }

  return true;
}

bool
D868UVCodeplug::ChannelElement::fromChannelObj(const Channel *c, Context &ctx) {
  if (! AnytoneCodeplug::ChannelElement::fromChannelObj(c, ctx))
    return false;

  if (c->is<DMRChannel>()) {
    const DMRChannel *dc = c->as<const DMRChannel>();
    // Set GPS system index
    if (dc->aprsObj() && dc->aprsObj()->is<GPSSystem>()) {
      setDigitalAPRSSystemIndex(ctx.index(dc->aprsObj()->as<GPSSystem>()));
      enableTXDigitalAPRS(true);
      enableRXAPRS(false);
    } else {
      enableTXDigitalAPRS(false);
      enableRXAPRS(false);
    }

    // Handle extension
    if (AnytoneDMRChannelExtension *ext = dc->anytoneChannelExtension()) {
      enableSMS(ext->sms());
      enableDataACK(ext->dataACK());
      enableThroughMode(ext->throughMode());
    }
  }

  return true;
}

/* ******************************************************************************************** *
 * Implementation of D868UVCodeplug::GeneralSettingsElement
 * ******************************************************************************************** */
D868UVCodeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr, unsigned size)
  : AnytoneCodeplug::GeneralSettingsElement(ptr, size)
{
  // pass....
}

D868UVCodeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr)
  : D868UVCodeplug::GeneralSettingsElement(ptr, GeneralSettingsElement::size())
{
  // pass...
}

void
D868UVCodeplug::GeneralSettingsElement::clear() {
  AnytoneCodeplug::GeneralSettingsElement::clear();
}

bool
D868UVCodeplug::GeneralSettingsElement::keyToneEnabled() const {
  return 0x00 != getUInt8(Offset::enableKeyTone());
}
void
D868UVCodeplug::GeneralSettingsElement::enableKeyTone(bool enable) {
  setUInt8(Offset::enableKeyTone(), enable ? 0x01 : 0x00);
}

AnytonePowerSaveSettingsExtension::PowerSave
D868UVCodeplug::GeneralSettingsElement::powerSave() const {
  return (AnytonePowerSaveSettingsExtension::PowerSave)getUInt8(Offset::powerSaveMode());
}
void
D868UVCodeplug::GeneralSettingsElement::setPowerSave(AnytonePowerSaveSettingsExtension::PowerSave mode) {
  setUInt8(Offset::powerSaveMode(), (unsigned int)mode);
}

unsigned
D868UVCodeplug::GeneralSettingsElement::voxLevel() const {
  return ((unsigned)getUInt8(Offset::voxLevel()))*3;
}
void
D868UVCodeplug::GeneralSettingsElement::setVOXLevel(unsigned level) {
  setUInt8(Offset::voxLevel(), level/3);
}

Interval
D868UVCodeplug::GeneralSettingsElement::voxDelay() const {
  return Interval::fromMilliseconds(100 + 500*((unsigned)getUInt8(Offset::voxDelay())));
}
void
D868UVCodeplug::GeneralSettingsElement::setVOXDelay(Interval intv) {
  setUInt8(Offset::voxDelay(), (std::max(100ULL, intv.milliseconds())-100)/500);
}

AnytoneSettingsExtension::VFOScanType
D868UVCodeplug::GeneralSettingsElement::vfoScanType() const {
  return (AnytoneSettingsExtension::VFOScanType) getUInt8(Offset::vfoScanType());
}
void
D868UVCodeplug::GeneralSettingsElement::setVFOScanType(AnytoneSettingsExtension::VFOScanType type) {
  setUInt8(Offset::vfoScanType(), (unsigned)type);
}

unsigned
D868UVCodeplug::GeneralSettingsElement::dmrMicGain() const {
  return (((unsigned)getUInt8(Offset::dmrMicGain())+1)*10)/4;
}
void
D868UVCodeplug::GeneralSettingsElement::setDMRMicGain(unsigned gain) {
  gain = std::min(1U, std::min(10U, gain));
  setUInt8(Offset::dmrMicGain(), (gain*4)/10);
}

AnytoneKeySettingsExtension::KeyFunction
D868UVCodeplug::GeneralSettingsElement::funcKeyAShort() const {
  return this->mapCodeToKeyFunction(getUInt8(Offset::progFuncKeyAShort()));
}
void
D868UVCodeplug::GeneralSettingsElement::setFuncKeyAShort(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKeyAShort(), this->mapKeyFunctionToCode(func));
}

AnytoneKeySettingsExtension::KeyFunction
D868UVCodeplug::GeneralSettingsElement::funcKeyBShort() const {
  return this->mapCodeToKeyFunction(getUInt8(Offset::progFuncKeyBShort()));
}
void
D868UVCodeplug::GeneralSettingsElement::setFuncKeyBShort(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKeyBShort(), this->mapKeyFunctionToCode(func));
}

AnytoneKeySettingsExtension::KeyFunction
D868UVCodeplug::GeneralSettingsElement::funcKeyCShort() const {
  return this->mapCodeToKeyFunction(getUInt8(Offset::progFuncKeyCShort()));
}
void
D868UVCodeplug::GeneralSettingsElement::setFuncKeyCShort(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKeyCShort(), this->mapKeyFunctionToCode(func));
}

AnytoneKeySettingsExtension::KeyFunction
D868UVCodeplug::GeneralSettingsElement::funcKey1Short() const {
  return this->mapCodeToKeyFunction(getUInt8(Offset::progFuncKey1Short()));
}
void
D868UVCodeplug::GeneralSettingsElement::setFuncKey1Short(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKey1Short(), this->mapKeyFunctionToCode(func));
}

AnytoneKeySettingsExtension::KeyFunction
D868UVCodeplug::GeneralSettingsElement::funcKey2Short() const {
  return this->mapCodeToKeyFunction(getUInt8(Offset::progFuncKey2Short()));
}
void
D868UVCodeplug::GeneralSettingsElement::setFuncKey2Short(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKey2Short(), this->mapKeyFunctionToCode(func));
}

AnytoneKeySettingsExtension::KeyFunction
D868UVCodeplug::GeneralSettingsElement::funcKeyALong() const {
  return (AnytoneKeySettingsExtension::KeyFunction)getUInt8(Offset::progFuncKeyALong());
}
void
D868UVCodeplug::GeneralSettingsElement::setFuncKeyALong(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKeyALong(), (unsigned)func);
}

AnytoneKeySettingsExtension::KeyFunction
D868UVCodeplug::GeneralSettingsElement::funcKeyBLong() const {
  return (AnytoneKeySettingsExtension::KeyFunction)getUInt8(Offset::progFuncKeyBLong());
}
void
D868UVCodeplug::GeneralSettingsElement::setFuncKeyBLong(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKeyBLong(), (unsigned)func);
}

AnytoneKeySettingsExtension::KeyFunction
D868UVCodeplug::GeneralSettingsElement::funcKeyCLong() const {
  return (AnytoneKeySettingsExtension::KeyFunction)getUInt8(Offset::progFuncKeyCLong());
}
void
D868UVCodeplug::GeneralSettingsElement::setFuncKeyCLong(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKeyCLong(), (unsigned)func);
}

AnytoneKeySettingsExtension::KeyFunction
D868UVCodeplug::GeneralSettingsElement::funcKey1Long() const {
  return (AnytoneKeySettingsExtension::KeyFunction)getUInt8(Offset::progFuncKey1Long());
}
void
D868UVCodeplug::GeneralSettingsElement::setFuncKey1Long(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKey1Long(), (unsigned)func);
}

AnytoneKeySettingsExtension::KeyFunction
D868UVCodeplug::GeneralSettingsElement::funcKey2Long() const {
  return (AnytoneKeySettingsExtension::KeyFunction)getUInt8(Offset::progFuncKey2Long());
}
void
D868UVCodeplug::GeneralSettingsElement::setFuncKey2Long(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKey2Long(), (unsigned)func);
}

Interval
D868UVCodeplug::GeneralSettingsElement::longPressDuration() const {
  return Interval::fromSeconds(((unsigned)getUInt8(Offset::longPressDuration()))+1);
}
void
D868UVCodeplug::GeneralSettingsElement::setLongPressDuration(Interval ms) {
  setUInt8(Offset::longPressDuration(), std::max(1ULL,ms.seconds())-1);
}

bool
D868UVCodeplug::GeneralSettingsElement::vfoModeA() const {
  return getUInt8(Offset::vfoModeA());
}
void
D868UVCodeplug::GeneralSettingsElement::enableVFOModeA(bool enable) {
  setUInt8(Offset::vfoModeA(), (enable ? 0x01 : 0x00));
}

bool
D868UVCodeplug::GeneralSettingsElement::vfoModeB() const {
  return getUInt8(Offset::vfoModeB());
}
void
D868UVCodeplug::GeneralSettingsElement::enableVFOModeB(bool enable) {
  setUInt8(Offset::vfoModeB(), (enable ? 0x01 : 0x00));
}

unsigned
D868UVCodeplug::GeneralSettingsElement::memoryZoneA() const {
  return getUInt8(Offset::memZoneA());
}
void
D868UVCodeplug::GeneralSettingsElement::setMemoryZoneA(unsigned zone) {
  setUInt8(Offset::memZoneA(), zone);
}

unsigned
D868UVCodeplug::GeneralSettingsElement::memoryZoneB() const {
  return getUInt8(Offset::memZoneB());
}
void
D868UVCodeplug::GeneralSettingsElement::setMemoryZoneB(unsigned zone) {
  setUInt8(Offset::memZoneB(), zone);
}

bool
D868UVCodeplug::GeneralSettingsElement::recording() const {
  return getUInt8(Offset::enableRecoding());
}
void
D868UVCodeplug::GeneralSettingsElement::enableRecording(bool enable) {
  setUInt8(Offset::enableRecoding(), (enable ? 0x01 : 0x00));
}

unsigned
D868UVCodeplug::GeneralSettingsElement::brightness() const {
  return (getUInt8(Offset::displayBrightness())*10)/4;
}
void
D868UVCodeplug::GeneralSettingsElement::setBrightness(unsigned level) {
  setUInt8(Offset::displayBrightness(), (level*4)/10);
}

bool
D868UVCodeplug::GeneralSettingsElement::backlightPermanent() const {
  return rxBacklightDuration().isNull();
}
Interval
D868UVCodeplug::GeneralSettingsElement::rxBacklightDuration() const {
  return Interval::fromSeconds(5*((unsigned)getUInt8(Offset::backlightDuration())));
}
void
D868UVCodeplug::GeneralSettingsElement::setRXBacklightDuration(Interval intv) {
  setUInt8(Offset::backlightDuration(), intv.seconds()/5);
}
void
D868UVCodeplug::GeneralSettingsElement::enableBacklightPermanent() {
  setRXBacklightDuration(Interval());
}

bool
D868UVCodeplug::GeneralSettingsElement::gps() const {
  return getUInt8(Offset::gpsEnable());
}
void
D868UVCodeplug::GeneralSettingsElement::enableGPS(bool enable) {
  setUInt8(Offset::gpsEnable(), (enable ? 0x01 : 0x00));
}

bool
D868UVCodeplug::GeneralSettingsElement::smsAlert() const {
  return getUInt8(Offset::smsAlert());
}
void
D868UVCodeplug::GeneralSettingsElement::enableSMSAlert(bool enable) {
  setUInt8(Offset::smsAlert(), (enable ? 0x01 : 0x00));
}

bool
D868UVCodeplug::GeneralSettingsElement::activeChannelB() const {
  return getUInt8(Offset::activeChannelB());
}
void
D868UVCodeplug::GeneralSettingsElement::enableActiveChannelB(bool enable) {
  setUInt8(Offset::activeChannelB(), (enable ? 0x01 : 0x00));
}

bool
D868UVCodeplug::GeneralSettingsElement::subChannel() const {
  return getUInt8(Offset::subChannel());
}
void
D868UVCodeplug::GeneralSettingsElement::enableSubChannel(bool enable) {
  setUInt8(Offset::subChannel(), (enable ? 0x01 : 0x00));
}

bool
D868UVCodeplug::GeneralSettingsElement::callAlert() const {
  return getUInt8(Offset::callAlert());
}
void
D868UVCodeplug::GeneralSettingsElement::enableCallAlert(bool enable) {
  setUInt8(Offset::callAlert(), (enable ? 0x01 : 0x00));
}

QTimeZone
D868UVCodeplug::GeneralSettingsElement::gpsTimeZone() const {
  return QTimeZone((((int)getUInt8(Offset::gpsTimeZone()))-12)*3600);
}
void
D868UVCodeplug::GeneralSettingsElement::setGPSTimeZone(const QTimeZone &zone) {
  int offset = zone.offsetFromUtc(QDateTime::currentDateTime());
  setUInt8(Offset::gpsTimeZone(), (12 + offset/3600));
}

bool
D868UVCodeplug::GeneralSettingsElement::dmrTalkPermit() const {
  return getBit(Offset::talkPermit(), 0);
}
void
D868UVCodeplug::GeneralSettingsElement::enableDMRTalkPermit(bool enable) {
  return setBit(Offset::talkPermit(), 0, enable);
}

bool
D868UVCodeplug::GeneralSettingsElement::fmTalkPermit() const {
  return getBit(Offset::talkPermit(), 1);
}
void
D868UVCodeplug::GeneralSettingsElement::enableFMTalkPermit(bool enable) {
  return setBit(Offset::talkPermit(), 1, enable);
}

bool
D868UVCodeplug::GeneralSettingsElement::dmrResetTone() const {
  return getUInt8(Offset::dmrResetTone());
}
void
D868UVCodeplug::GeneralSettingsElement::enableDMRResetTone(bool enable) {
  return setUInt8(Offset::dmrResetTone(), (enable ? 0x01 : 0x00));
}

AnytoneAudioSettingsExtension::VoxSource
D868UVCodeplug::GeneralSettingsElement::voxSource() const {
  return (AnytoneAudioSettingsExtension::VoxSource)getUInt8(Offset::voxSource());
}
void
D868UVCodeplug::GeneralSettingsElement::setVOXSource(AnytoneAudioSettingsExtension::VoxSource source) {
  setUInt8(Offset::voxSource(), (unsigned)source);
}

bool
D868UVCodeplug::GeneralSettingsElement::idleChannelTone() const {
  return getUInt8(Offset::idleChannelTone());
}
void
D868UVCodeplug::GeneralSettingsElement::enableIdleChannelTone(bool enable) {
  return setUInt8(Offset::idleChannelTone(), (enable ? 0x01 : 0x00));
}

Interval
D868UVCodeplug::GeneralSettingsElement::menuExitTime() const {
  return Interval::fromSeconds(5 + 5*((unsigned) getUInt8(Offset::menuExitTime())));
}
void
D868UVCodeplug::GeneralSettingsElement::setMenuExitTime(Interval intv) {
  setUInt8(Offset::menuExitTime(), (std::max(5ULL, intv.seconds())-5)/5);
}

bool
D868UVCodeplug::GeneralSettingsElement::startupTone() const {
  return getUInt8(Offset::startupTone());
}
void
D868UVCodeplug::GeneralSettingsElement::enableStartupTone(bool enable) {
  return setUInt8(Offset::startupTone(), (enable ? 0x01 : 0x00));
}

bool
D868UVCodeplug::GeneralSettingsElement::callEndPrompt() const {
  return getUInt8(Offset::callEndPrompt());
}
void
D868UVCodeplug::GeneralSettingsElement::enableCallEndPrompt(bool enable) {
  return setUInt8(Offset::callEndPrompt(), (enable ? 0x01 : 0x00));
}

unsigned
D868UVCodeplug::GeneralSettingsElement::maxSpeakerVolume() const {
  return (((unsigned)getUInt8(Offset::maxSpeakerVolume()))*10)/8;
}
void
D868UVCodeplug::GeneralSettingsElement::setMaxSpeakerVolume(unsigned level) {
  setUInt8(Offset::maxSpeakerVolume(), (level*8)/10);
}

bool
D868UVCodeplug::GeneralSettingsElement::getGPSPosition() const {
  return getUInt8(Offset::getGPSPosition());
}
void
D868UVCodeplug::GeneralSettingsElement::enableGetGPSPosition(bool enable) {
  return setUInt8(Offset::getGPSPosition(), (enable ? 0x01 : 0x00));
}

bool
D868UVCodeplug::GeneralSettingsElement::volumeChangePrompt() const {
  return getUInt8(Offset::volumeChangePrompt());
}
void
D868UVCodeplug::GeneralSettingsElement::enableVolumeChangePrompt(bool enable) {
  setUInt8(Offset::volumeChangePrompt(), (enable ? 0x01 : 0x01));
}

AnytoneAutoRepeaterSettingsExtension::Direction
D868UVCodeplug::GeneralSettingsElement::autoRepeaterDirectionA() const {
  return (AnytoneAutoRepeaterSettingsExtension::Direction) getUInt8(Offset::autoRepeaterDirA());
}
void
D868UVCodeplug::GeneralSettingsElement::setAutoRepeaterDirectionA(AnytoneAutoRepeaterSettingsExtension::Direction dir) {
  setUInt8(Offset::autoRepeaterDirA(), (unsigned)dir);
}

AnytoneDisplaySettingsExtension::LastCallerDisplayMode
D868UVCodeplug::GeneralSettingsElement::lastCallerDisplayMode() const {
  return (AnytoneDisplaySettingsExtension::LastCallerDisplayMode)getUInt8(Offset::lastCallerDisplay());
}
void
D868UVCodeplug::GeneralSettingsElement::setLastCallerDisplayMode(AnytoneDisplaySettingsExtension::LastCallerDisplayMode mode) {
  setUInt8(Offset::lastCallerDisplay(), (unsigned)mode);
}

bool
D868UVCodeplug::GeneralSettingsElement::displayClock() const {
  return getUInt8(Offset::showClock());
}
void
D868UVCodeplug::GeneralSettingsElement::enableDisplayClock(bool enable) {
  setUInt8(Offset::showClock(), (enable ? 0x01 : 0x00));
}

unsigned
D868UVCodeplug::GeneralSettingsElement::maxHeadPhoneVolume() const {
  return (((unsigned)getUInt8(Offset::maxHeadPhoneVolume()))*10)/8;
}
void
D868UVCodeplug::GeneralSettingsElement::setMaxHeadPhoneVolume(unsigned max) {
  setUInt8(Offset::maxHeadPhoneVolume(), (max*8)/10);
}

bool
D868UVCodeplug::GeneralSettingsElement::enhanceAudio() const {
  return getUInt8(Offset::enhanceAudio());
}
void
D868UVCodeplug::GeneralSettingsElement::enableEnhancedAudio(bool enable) {
  setUInt8(Offset::enhanceAudio(), (enable ? 0x01 : 0x00));
}

Frequency
D868UVCodeplug::GeneralSettingsElement::minVFOScanFrequencyUHF() const {
  return Frequency::fromHz(((unsigned)getUInt32_le(Offset::minVFOScanUHF()))*10);
}
void
D868UVCodeplug::GeneralSettingsElement::setMinVFOScanFrequencyUHF(Frequency freq) {
  setUInt32_le(Offset::minVFOScanUHF(), freq.inHz()/10);
}

Frequency
D868UVCodeplug::GeneralSettingsElement::maxVFOScanFrequencyUHF() const {
  return Frequency::fromHz(((unsigned)getUInt32_le(Offset::maxVFOScanUHF()))*10);
}
void
D868UVCodeplug::GeneralSettingsElement::setMaxVFOScanFrequencyUHF(Frequency freq) {
  setUInt32_le(Offset::maxVFOScanUHF(), freq.inHz()/10);
}

Frequency
D868UVCodeplug::GeneralSettingsElement::minVFOScanFrequencyVHF() const {
  return Frequency::fromHz(((unsigned)getUInt32_le(Offset::minVFOScanVHF()))*10);
}
void
D868UVCodeplug::GeneralSettingsElement::setMinVFOScanFrequencyVHF(Frequency freq) {
  setUInt32_le(Offset::minVFOScanVHF(), freq.inHz()/10);
}

Frequency
D868UVCodeplug::GeneralSettingsElement::maxVFOScanFrequencyVHF() const {
  return Frequency::fromHz(((unsigned)getUInt32_le(Offset::maxVFOScanVHF()))*10);
}
void
D868UVCodeplug::GeneralSettingsElement::setMaxVFOScanFrequencyVHF(Frequency freq) {
  setUInt32_le(Offset::maxVFOScanVHF(), freq.inHz()/10);
}

bool
D868UVCodeplug::GeneralSettingsElement::hasAutoRepeaterOffsetFrequencyIndexUHF() const {
  return 0xff != autoRepeaterOffsetFrequencyIndexUHF();
}
unsigned
D868UVCodeplug::GeneralSettingsElement::autoRepeaterOffsetFrequencyIndexUHF() const {
  return getUInt8(Offset::autoRepOffsetUHF());
}
void
D868UVCodeplug::GeneralSettingsElement::setAutoRepeaterOffsetFrequenyIndexUHF(unsigned idx) {
  setUInt8(Offset::autoRepOffsetUHF(), idx);
}
void
D868UVCodeplug::GeneralSettingsElement::clearAutoRepeaterOffsetFrequencyIndexUHF() {
  setAutoRepeaterOffsetFrequenyIndexUHF(0xff);
}

bool
D868UVCodeplug::GeneralSettingsElement::hasAutoRepeaterOffsetFrequencyIndexVHF() const {
  return 0xff != autoRepeaterOffsetFrequencyIndexVHF();
}
unsigned
D868UVCodeplug::GeneralSettingsElement::autoRepeaterOffsetFrequencyIndexVHF() const {
  return getUInt8(Offset::autoRepOffsetVHF());
}
void
D868UVCodeplug::GeneralSettingsElement::setAutoRepeaterOffsetFrequenyIndexVHF(unsigned idx) {
  setUInt8(Offset::autoRepOffsetVHF(), idx);
}
void
D868UVCodeplug::GeneralSettingsElement::clearAutoRepeaterOffsetFrequencyIndexVHF() {
  setAutoRepeaterOffsetFrequenyIndexVHF(0xff);
}

void
D868UVCodeplug::GeneralSettingsElement::callToneMelody(Melody &melody) const {
  QVector<QPair<double, unsigned int>> tones; tones.reserve(5);
  for (int i=0; i<5; i++) {
    double freq = getUInt16_le(Offset::callToneTones()+2*i);
    unsigned int duration = getUInt16_le(Offset::callToneDurations()+2*i);
    if (duration) tones.append({freq, duration});
  }
  melody.infer(tones);
}
void
D868UVCodeplug::GeneralSettingsElement::setCallToneMelody(const Melody &melody) {
  unsigned int n=std::min(5U, (unsigned int)melody.count());
  QVector<QPair<double, unsigned int>> tones = melody.toTones();
  for (unsigned int i=0; i<n; i++) {
    setUInt16_le(Offset::callToneTones()+2*i, tones.at(i).first);
    setUInt16_le(Offset::callToneDurations()+2*i, tones.at(i).second);
  }
}

void
D868UVCodeplug::GeneralSettingsElement::idleToneMelody(Melody &melody) const {
  QVector<QPair<double, unsigned int>> tones; tones.reserve(5);
  for (int i=0; i<5; i++) {
    double frequency = getUInt16_le(Offset::idleToneTones()+2*i);
    unsigned int duration  = getUInt16_le(Offset::idleToneDurations()+2*i);
    if (duration) tones.append({frequency, duration});
  }
  melody.infer(tones);
}
void
D868UVCodeplug::GeneralSettingsElement::setIdleToneMelody(const Melody &melody) {
  unsigned int n=std::min(5U, (unsigned int)melody.count());
  QVector<QPair<double, unsigned int>> tones = melody.toTones();
  for (unsigned int i=0; i<n; i++) {
    setUInt16_le(Offset::idleToneTones()+2*i, tones.at(i).first);
    setUInt16_le(Offset::idleToneDurations()+2*i, tones.at(i).second);
  }
}

void
D868UVCodeplug::GeneralSettingsElement::resetToneMelody(Melody &melody) const {
  QVector<QPair<double, unsigned int>> tones; tones.reserve(5);
  for (int i=0; i<5; i++) {
    double frequency = getUInt16_le(Offset::resetToneTones()+2*i);
    unsigned int duration  = getUInt16_le(Offset::resetToneDurations()+2*i);
    if (duration) tones.append({frequency, duration});
  }
  melody.infer(tones);
}
void
D868UVCodeplug::GeneralSettingsElement::setResetToneMelody(const Melody &melody) {
  unsigned int n=std::min(5U, (unsigned int)melody.count());
  QVector<QPair<double, unsigned int>> tones = melody.toTones();
  for (unsigned int i=0; i<n; i++) {
    setUInt16_le(Offset::resetToneTones()+2*i, tones.at(i).first);
    setUInt16_le(Offset::resetToneDurations()+2*i, tones.at(i).second);
  }
}

unsigned
D868UVCodeplug::GeneralSettingsElement::recordingDelay() const {
  return ((unsigned)getUInt8(Offset::recordingDelay()))*200;
}
void
D868UVCodeplug::GeneralSettingsElement::setRecodringDelay(unsigned ms) {
  setUInt8(Offset::recordingDelay(), ms/200);
}

bool
D868UVCodeplug::GeneralSettingsElement::displayCall() const {
  return getUInt8(Offset::callDisplayMode());
}
void
D868UVCodeplug::GeneralSettingsElement::enableDisplayCall(bool enable) {
  setUInt8(Offset::callDisplayMode(), (enable ? 0x01 : 0x00));
}


AnytoneDisplaySettingsExtension::Color
D868UVCodeplug::GeneralSettingsElement::callDisplayColor() const {
  return (AnytoneDisplaySettingsExtension::Color)getUInt8(Offset::callColor());
}
void
D868UVCodeplug::GeneralSettingsElement::setCallDisplayColor(AnytoneDisplaySettingsExtension::Color color) {
  switch (color) {
  case AnytoneDisplaySettingsExtension::Color::White:
  case AnytoneDisplaySettingsExtension::Color::Black:
    break;
  default:
    color = AnytoneDisplaySettingsExtension::Color::Black;
    break;
  }
  setUInt8(Offset::callColor(), (unsigned)color);
}

Interval
D868UVCodeplug::GeneralSettingsElement::gpsUpdatePeriod() const {
  return Interval::fromSeconds(getUInt8(Offset::gpsPeriod()));
}
void
D868UVCodeplug::GeneralSettingsElement::setGPSUpdatePeriod(Interval sec) {
  setUInt8(Offset::gpsPeriod(), sec.seconds());
}

bool
D868UVCodeplug::GeneralSettingsElement::showCurrentContact() const {
  return getUInt8(Offset::showCurrentContact());
}
void
D868UVCodeplug::GeneralSettingsElement::enableShowCurrentContact(bool enable) {
  setUInt8(Offset::showCurrentContact(), (enable ? 0x01 : 0x00));
}

bool
D868UVCodeplug::GeneralSettingsElement::keyToneLevelAdjustable() const {
  return 0 == keyToneLevel();
}
unsigned
D868UVCodeplug::GeneralSettingsElement::keyToneLevel() const {
  return ((unsigned)getUInt8(Offset::keyToneLevel()))*10/15;
}
void
D868UVCodeplug::GeneralSettingsElement::setKeyToneLevel(unsigned level) {
  setUInt8(Offset::keyToneLevel(), level*10/15);
}
void
D868UVCodeplug::GeneralSettingsElement::setKeyToneLevelAdjustable() {
  setUInt8(Offset::keyToneLevel(), 0);
}

bool
D868UVCodeplug::GeneralSettingsElement::gpsUnitsImperial() const {
  return getUInt8(Offset::gpsUnits());
}
void
D868UVCodeplug::GeneralSettingsElement::enableGPSUnitsImperial(bool enable) {
  setUInt8(Offset::gpsUnits(), (enable ? 0x01 : 0x00));
}

bool
D868UVCodeplug::GeneralSettingsElement::knobLock() const {
  return getBit(Offset::knobLock(), 0);
}
void
D868UVCodeplug::GeneralSettingsElement::enableKnobLock(bool enable) {
  setBit(Offset::knobLock(), 0, enable);
}
bool
D868UVCodeplug::GeneralSettingsElement::keypadLock() const {
  return getBit(Offset::keypadLock(), 1);
}
void
D868UVCodeplug::GeneralSettingsElement::enableKeypadLock(bool enable) {
  setBit(Offset::keypadLock(), 1, enable);
}
bool
D868UVCodeplug::GeneralSettingsElement::sidekeysLock() const {
  return getBit(Offset::sideKeyLock(), 3);
}
void
D868UVCodeplug::GeneralSettingsElement::enableSidekeysLock(bool enable) {
  setBit(Offset::sideKeyLock(), 3, enable);
}
bool
D868UVCodeplug::GeneralSettingsElement::keyLockForced() const {
  return getBit(Offset::forceKeyLock(), 4);
}
void
D868UVCodeplug::GeneralSettingsElement::enableKeyLockForced(bool enable) {
  setBit(Offset::forceKeyLock(), 4, enable);
}

bool
D868UVCodeplug::GeneralSettingsElement::showLastHeard() const {
  return getUInt8(Offset::showLastHeard());
}
void
D868UVCodeplug::GeneralSettingsElement::enableShowLastHeard(bool enable) {
  setUInt8(Offset::showLastHeard(), (enable ? 0x01 : 0x00));
}

Frequency
D868UVCodeplug::GeneralSettingsElement::autoRepeaterMinFrequencyVHF() const {
  return Frequency::fromHz(getUInt32_le(Offset::autoRepMinVHF())*10);
}
void
D868UVCodeplug::GeneralSettingsElement::setAutoRepeaterMinFrequencyVHF(Frequency freq) {
  setUInt32_le(Offset::autoRepMinVHF(), freq.inHz()/10);
}
Frequency
D868UVCodeplug::GeneralSettingsElement::autoRepeaterMaxFrequencyVHF() const {
  return Frequency::fromHz(getUInt32_le(Offset::autoRepMaxVHF())*10);
}
void
D868UVCodeplug::GeneralSettingsElement::setAutoRepeaterMaxFrequencyVHF(Frequency freq) {
  setUInt32_le(Offset::autoRepMaxVHF(), freq.inHz()/10);
}

Frequency
D868UVCodeplug::GeneralSettingsElement::autoRepeaterMinFrequencyUHF() const {
  return Frequency::fromHz(getUInt32_le(Offset::autoRepMinUHF())*10);
}
void
D868UVCodeplug::GeneralSettingsElement::setAutoRepeaterMinFrequencyUHF(Frequency freq) {
  setUInt32_le(Offset::autoRepMinUHF(), freq.inHz()/10);
}
Frequency
D868UVCodeplug::GeneralSettingsElement::autoRepeaterMaxFrequencyUHF() const {
  return Frequency::fromHz(getUInt32_le(Offset::autoRepMaxUHF())*10);
}
void
D868UVCodeplug::GeneralSettingsElement::setAutoRepeaterMaxFrequencyUHF(Frequency freq) {
  setUInt32_le(Offset::autoRepMaxUHF(), freq.inHz()/10);
}

AnytoneAutoRepeaterSettingsExtension::Direction
D868UVCodeplug::GeneralSettingsElement::autoRepeaterDirectionB() const {
  return (AnytoneAutoRepeaterSettingsExtension::Direction)getUInt8(Offset::autoRepeaterDirB());
}
void
D868UVCodeplug::GeneralSettingsElement::setAutoRepeaterDirectionB(AnytoneAutoRepeaterSettingsExtension::Direction dir) {
  setUInt8(Offset::autoRepeaterDirB(), (unsigned)dir);
}

bool
D868UVCodeplug::GeneralSettingsElement::defaultChannel() const {
  return getUInt8(Offset::defaultChannels());
}
void
D868UVCodeplug::GeneralSettingsElement::enableDefaultChannel(bool enable) {
  setUInt8(Offset::defaultChannels(), (enable ? 0x01 : 0x00));
}

unsigned
D868UVCodeplug::GeneralSettingsElement::defaultZoneIndexA() const {
  return getUInt8(Offset::defaultZoneA());
}
void
D868UVCodeplug::GeneralSettingsElement::setDefaultZoneIndexA(unsigned idx) {
  setUInt8(Offset::defaultZoneA(), idx);
}

unsigned
D868UVCodeplug::GeneralSettingsElement::defaultZoneIndexB() const {
  return getUInt8(Offset::defaultZoneB());
}
void
D868UVCodeplug::GeneralSettingsElement::setDefaultZoneIndexB(unsigned idx) {
  setUInt8(Offset::defaultZoneB(), idx);
}

bool
D868UVCodeplug::GeneralSettingsElement::defaultChannelAIsVFO() const {
  return 0xff == defaultChannelAIndex();
}
unsigned
D868UVCodeplug::GeneralSettingsElement::defaultChannelAIndex() const {
  return getUInt8(Offset::defaultChannelA());
}
void
D868UVCodeplug::GeneralSettingsElement::setDefaultChannelAIndex(unsigned idx) {
  setUInt8(Offset::defaultChannelA(), idx);
}
void
D868UVCodeplug::GeneralSettingsElement::setDefaultChannelAToVFO() {
  setDefaultChannelAIndex(0xff);
}

bool
D868UVCodeplug::GeneralSettingsElement::defaultChannelBIsVFO() const {
  return 0xff == defaultChannelBIndex();
}
unsigned
D868UVCodeplug::GeneralSettingsElement::defaultChannelBIndex() const {
  return getUInt8(Offset::defaultChannelB());
}
void
D868UVCodeplug::GeneralSettingsElement::setDefaultChannelBIndex(unsigned idx) {
  setUInt8(Offset::defaultChannelB(), idx);
}
void
D868UVCodeplug::GeneralSettingsElement::setDefaultChannelBToVFO() {
  setDefaultChannelBIndex(0xff);
}

bool
D868UVCodeplug::GeneralSettingsElement::keepLastCaller() const {
  return getUInt8(Offset::keepLastCaller());
}
void
D868UVCodeplug::GeneralSettingsElement::enableKeepLastCaller(bool enable) {
  setUInt8(Offset::keepLastCaller(), (enable ? 0x01 : 0x00));
}

bool
D868UVCodeplug::GeneralSettingsElement::fromConfig(const Flags &flags, Context &ctx) {
  if (! AnytoneCodeplug::GeneralSettingsElement::fromConfig(flags, ctx))
    return false;

  setGPSUpdatePeriod(Interval::fromSeconds(5));
  // Set measurement system based on system locale (0x00==Metric)
  enableGPSUnitsImperial(QLocale::ImperialSystem == QLocale::system().measurementSystem());
  // Set default VOX sensitivity
  setVOXLevel(ctx.config()->settings()->vox());

  AnytoneSettingsExtension *ext = ctx.config()->settings()->anytoneExtension();
  if (nullptr == ext)
    return true;

  // Power save settings
  setPowerSave(ext->powerSaveSettings()->powerSave());

  // Encode key settings
  enableKnobLock(ext->keySettings()->knobLockEnabled());
  enableKeypadLock(ext->keySettings()->keypadLockEnabled());
  enableSidekeysLock(ext->keySettings()->sideKeysLockEnabled());
  enableKeyLockForced(ext->keySettings()->forcedKeyLockEnabled());

  // Encode tone settings
  setKeyToneLevel(ext->toneSettings()->keyToneLevel());

  // Encode audio settings
  setVOXDelay(ext->audioSettings()->voxDelay());
  setVOXSource(ext->audioSettings()->voxSource());
  setMaxHeadPhoneVolume(ext->audioSettings()->maxHeadPhoneVolume());

  // Encode display settings
  setRXBacklightDuration(ext->displaySettings()->backlightDuration());
  enableShowCurrentContact(ext->displaySettings()->showContact());

  return true;
}

bool
D868UVCodeplug::GeneralSettingsElement::updateConfig(Context &ctx) {
  if (! AnytoneCodeplug::GeneralSettingsElement::updateConfig(ctx))
    return false;

  ctx.config()->settings()->setVOX(voxLevel());

  // Get or add settings extension
  AnytoneSettingsExtension *ext = nullptr;
  if (ctx.config()->settings()->anytoneExtension()) {
    ext = ctx.config()->settings()->anytoneExtension();
  } else {
    ext = new AnytoneSettingsExtension();
    ctx.config()->settings()->setAnytoneExtension(ext);
  }

  // Decode power save settings:
  ext->powerSaveSettings()->setPowerSave(powerSave());

  // Decode key settings
  ext->keySettings()->enableKnobLock(this->knobLock());
  ext->keySettings()->enableKeypadLock(this->keypadLock());
  ext->keySettings()->enableSideKeysLock(this->sidekeysLock());
  ext->keySettings()->enableForcedKeyLock(this->keyLockForced());

  // Decode tone settings
  ext->toneSettings()->setKeyToneLevel(keyToneLevel());

  // Decode audio settings
  ext->audioSettings()->setVOXDelay(voxDelay());
  ext->audioSettings()->setVOXSource(voxSource());
  ext->audioSettings()->setMaxHeadPhoneVolume(this->maxHeadPhoneVolume());

  // Decode display settings
  ext->displaySettings()->enableShowContact(this->showCurrentContact());
  ext->displaySettings()->setBacklightDuration(rxBacklightDuration());

  return true;
}


bool
D868UVCodeplug::GeneralSettingsElement::linkSettings(RadioSettings *settings, Context &ctx, const ErrorStack &err) {
  if (! AnytoneCodeplug::GeneralSettingsElement::linkSettings(settings, ctx, err))
    return false;

  // Get or add settings extension
  AnytoneSettingsExtension *ext = settings->anytoneExtension();
  if (nullptr == ext) {
    ext = new AnytoneSettingsExtension();
    settings->setAnytoneExtension(ext);
  }

  // Nothing to link

  return true;
}

uint8_t
D868UVCodeplug::GeneralSettingsElement::mapKeyFunctionToCode(AnytoneKeySettingsExtension::KeyFunction func) const {
  switch (func) {
  case AnytoneKeySettingsExtension::KeyFunction::Off:               return (uint8_t)KeyFunction::Off;
  case AnytoneKeySettingsExtension::KeyFunction::Voltage:           return (uint8_t)KeyFunction::Voltage;
  case AnytoneKeySettingsExtension::KeyFunction::Power:             return (uint8_t)KeyFunction::Power;
  case AnytoneKeySettingsExtension::KeyFunction::Repeater:          return (uint8_t)KeyFunction::Repeater;
  case AnytoneKeySettingsExtension::KeyFunction::Reverse:           return (uint8_t)KeyFunction::Reverse;
  case AnytoneKeySettingsExtension::KeyFunction::Encryption:        return (uint8_t)KeyFunction::Encryption;
  case AnytoneKeySettingsExtension::KeyFunction::Call:              return (uint8_t)KeyFunction::Call;
  case AnytoneKeySettingsExtension::KeyFunction::VOX:               return (uint8_t)KeyFunction::VOX;
  case AnytoneKeySettingsExtension::KeyFunction::ToggleVFO:         return (uint8_t)KeyFunction::ToggleVFO;
  case AnytoneKeySettingsExtension::KeyFunction::SubPTT:            return (uint8_t)KeyFunction::SubPTT;
  case AnytoneKeySettingsExtension::KeyFunction::Scan:              return (uint8_t)KeyFunction::Scan;
  case AnytoneKeySettingsExtension::KeyFunction::WFM:               return (uint8_t)KeyFunction::WFM;
  case AnytoneKeySettingsExtension::KeyFunction::Alarm:             return (uint8_t)KeyFunction::Alarm;
  case AnytoneKeySettingsExtension::KeyFunction::RecordSwitch:      return (uint8_t)KeyFunction::RecordSwitch;
  case AnytoneKeySettingsExtension::KeyFunction::Record:            return (uint8_t)KeyFunction::Record;
  case AnytoneKeySettingsExtension::KeyFunction::SMS:               return (uint8_t)KeyFunction::SMS;
  case AnytoneKeySettingsExtension::KeyFunction::Dial:              return (uint8_t)KeyFunction::Dial;
  case AnytoneKeySettingsExtension::KeyFunction::GPSInformation:    return (uint8_t)KeyFunction::GPSInformation;
  case AnytoneKeySettingsExtension::KeyFunction::Monitor:           return (uint8_t)KeyFunction::Monitor;
  case AnytoneKeySettingsExtension::KeyFunction::ToggleMainChannel: return (uint8_t)KeyFunction::ToggleMainChannel;
  case AnytoneKeySettingsExtension::KeyFunction::HotKey1:           return (uint8_t)KeyFunction::HotKey1;
  case AnytoneKeySettingsExtension::KeyFunction::HotKey2:           return (uint8_t)KeyFunction::HotKey2;
  case AnytoneKeySettingsExtension::KeyFunction::HotKey3:           return (uint8_t)KeyFunction::HotKey3;
  case AnytoneKeySettingsExtension::KeyFunction::HotKey4:           return (uint8_t)KeyFunction::HotKey4;
  case AnytoneKeySettingsExtension::KeyFunction::HotKey5:           return (uint8_t)KeyFunction::HotKey5;
  case AnytoneKeySettingsExtension::KeyFunction::HotKey6:           return (uint8_t)KeyFunction::HotKey6;
  case AnytoneKeySettingsExtension::KeyFunction::WorkAlone:         return (uint8_t)KeyFunction::WorkAlone;
  case AnytoneKeySettingsExtension::KeyFunction::SkipChannel:       return (uint8_t)KeyFunction::SkipChannel;
  case AnytoneKeySettingsExtension::KeyFunction::DMRMonitor:        return (uint8_t)KeyFunction::DMRMonitor;
  case AnytoneKeySettingsExtension::KeyFunction::SubChannel:        return (uint8_t)KeyFunction::SubChannel;
  case AnytoneKeySettingsExtension::KeyFunction::PriorityZone:      return (uint8_t)KeyFunction::PriorityZone;
  case AnytoneKeySettingsExtension::KeyFunction::VFOScan:           return (uint8_t)KeyFunction::VFOScan;
  case AnytoneKeySettingsExtension::KeyFunction::MICSoundQuality:   return (uint8_t)KeyFunction::MICSoundQuality;
  case AnytoneKeySettingsExtension::KeyFunction::LastCallReply:     return (uint8_t)KeyFunction::LastCallReply;
  case AnytoneKeySettingsExtension::KeyFunction::ChannelType:       return (uint8_t)KeyFunction::ChannelType;
  case AnytoneKeySettingsExtension::KeyFunction::Ranging:           return (uint8_t)KeyFunction::Ranging;
  case AnytoneKeySettingsExtension::KeyFunction::ChannelRanging:    return (uint8_t)KeyFunction::ChannelRanging;
  case AnytoneKeySettingsExtension::KeyFunction::MaxVolume:         return (uint8_t)KeyFunction::MaxVolume;
  case AnytoneKeySettingsExtension::KeyFunction::Slot:              return (uint8_t)KeyFunction::Slot;
  default:                                                          return (uint8_t)KeyFunction::Off;
  }
}

AnytoneKeySettingsExtension::KeyFunction
D868UVCodeplug::GeneralSettingsElement::mapCodeToKeyFunction(uint8_t code) const {
  switch ((KeyFunction)code) {
  case KeyFunction::Off:               return AnytoneKeySettingsExtension::KeyFunction::Off;
  case KeyFunction::Voltage:           return AnytoneKeySettingsExtension::KeyFunction::Voltage;
  case KeyFunction::Power:             return AnytoneKeySettingsExtension::KeyFunction::Power;
  case KeyFunction::Repeater:          return AnytoneKeySettingsExtension::KeyFunction::Repeater;
  case KeyFunction::Reverse:           return AnytoneKeySettingsExtension::KeyFunction::Reverse;
  case KeyFunction::Encryption:        return AnytoneKeySettingsExtension::KeyFunction::Encryption;
  case KeyFunction::Call:              return AnytoneKeySettingsExtension::KeyFunction::Call;
  case KeyFunction::VOX:               return AnytoneKeySettingsExtension::KeyFunction::VOX;
  case KeyFunction::ToggleVFO:         return AnytoneKeySettingsExtension::KeyFunction::ToggleVFO;
  case KeyFunction::SubPTT:            return AnytoneKeySettingsExtension::KeyFunction::SubPTT;
  case KeyFunction::Scan:              return AnytoneKeySettingsExtension::KeyFunction::Scan;
  case KeyFunction::WFM:               return AnytoneKeySettingsExtension::KeyFunction::WFM;
  case KeyFunction::Alarm:             return AnytoneKeySettingsExtension::KeyFunction::Alarm;
  case KeyFunction::RecordSwitch:      return AnytoneKeySettingsExtension::KeyFunction::RecordSwitch;
  case KeyFunction::Record:            return AnytoneKeySettingsExtension::KeyFunction::Record;
  case KeyFunction::SMS:               return AnytoneKeySettingsExtension::KeyFunction::SMS;
  case KeyFunction::Dial:              return AnytoneKeySettingsExtension::KeyFunction::Dial;
  case KeyFunction::GPSInformation:    return AnytoneKeySettingsExtension::KeyFunction::GPSInformation;
  case KeyFunction::Monitor:           return AnytoneKeySettingsExtension::KeyFunction::Monitor;
  case KeyFunction::ToggleMainChannel: return AnytoneKeySettingsExtension::KeyFunction::ToggleMainChannel;
  case KeyFunction::HotKey1:           return AnytoneKeySettingsExtension::KeyFunction::HotKey1;
  case KeyFunction::HotKey2:           return AnytoneKeySettingsExtension::KeyFunction::HotKey2;
  case KeyFunction::HotKey3:           return AnytoneKeySettingsExtension::KeyFunction::HotKey3;
  case KeyFunction::HotKey4:           return AnytoneKeySettingsExtension::KeyFunction::HotKey4;
  case KeyFunction::HotKey5:           return AnytoneKeySettingsExtension::KeyFunction::HotKey5;
  case KeyFunction::HotKey6:           return AnytoneKeySettingsExtension::KeyFunction::HotKey6;
  case KeyFunction::WorkAlone:         return AnytoneKeySettingsExtension::KeyFunction::WorkAlone;
  case KeyFunction::SkipChannel:       return AnytoneKeySettingsExtension::KeyFunction::SkipChannel;
  case KeyFunction::DMRMonitor:        return AnytoneKeySettingsExtension::KeyFunction::DMRMonitor;
  case KeyFunction::SubChannel:        return AnytoneKeySettingsExtension::KeyFunction::SubChannel;
  case KeyFunction::PriorityZone:      return AnytoneKeySettingsExtension::KeyFunction::PriorityZone;
  case KeyFunction::VFOScan:           return AnytoneKeySettingsExtension::KeyFunction::VFOScan;
  case KeyFunction::MICSoundQuality:   return AnytoneKeySettingsExtension::KeyFunction::MICSoundQuality;
  case KeyFunction::LastCallReply:     return AnytoneKeySettingsExtension::KeyFunction::LastCallReply;
  case KeyFunction::ChannelType:       return AnytoneKeySettingsExtension::KeyFunction::ChannelType;
  case KeyFunction::Ranging:           return AnytoneKeySettingsExtension::KeyFunction::Ranging;
  case KeyFunction::ChannelRanging:    return AnytoneKeySettingsExtension::KeyFunction::ChannelRanging;
  case KeyFunction::MaxVolume:         return AnytoneKeySettingsExtension::KeyFunction::MaxVolume;
  case KeyFunction::Slot:              return AnytoneKeySettingsExtension::KeyFunction::Slot;
  default:                             return AnytoneKeySettingsExtension::KeyFunction::Off;
  }
}


/* ******************************************************************************************** *
 * Implementation of D868UVCodeplug
 * ******************************************************************************************** */
D868UVCodeplug::D868UVCodeplug(const QString &label, QObject *parent)
  : AnytoneCodeplug(label, parent)
{
  // pass...
}

D868UVCodeplug::D868UVCodeplug(QObject *parent)
  : AnytoneCodeplug("AnyTone AT-D868UV Codeplug", parent)
{
  // pass...
}

void
D868UVCodeplug::allocateUpdated() {
  this->allocateVFOSettings();

  // General config
  this->allocateGeneralSettings();
  this->allocateZoneChannelList();
  this->allocateDTMFNumbers();
  this->allocateBootSettings();
  this->allocateRepeaterOffsetFrequencies();

  this->allocateGPSSystems();

  this->allocateSMSMessages();
  this->allocateHotKeySettings();
  this->allocateAlarmSettings();
  this->allocateFMBroadcastSettings();

  this->allocate5ToneIDs();
  this->allocate5ToneFunctions();
  this->allocate5ToneSettings();

  this->allocate2ToneIDs();
  this->allocate2ToneFunctions();
  this->allocate2ToneSettings();

  this->allocateDTMFSettings();

  image(0).addElement(Offset::dmrEncryptionIDs(), DMREncryptionKeyIDListElement::size());
  image(0).addElement(Offset::dmrEncryptionKeys(), DMREncryptionKeyListElement::size());
}

void
D868UVCodeplug::allocateForEncoding() {
  this->allocateChannels();
  this->allocateZones();
  this->allocateContacts();
  this->allocateAnalogContacts();
  this->allocateRXGroupLists();
  this->allocateScanLists();
  this->allocateRadioIDs();
}

void
D868UVCodeplug::allocateForDecoding() {
  this->allocateRadioIDs();
  this->allocateChannels();
  this->allocateZones();
  this->allocateContacts();
  this->allocateAnalogContacts();
  this->allocateRXGroupLists();
  this->allocateScanLists();

  // General config
  this->allocateGeneralSettings();
  this->allocateZoneChannelList();
  this->allocateBootSettings();
  this->allocateRepeaterOffsetFrequencies();

  // GPS settings
  this->allocateGPSSystems();
}


bool
D868UVCodeplug::allocateBitmaps() {
  // Channel bitmap
  image(0).addElement(Offset::channelBitmap(), ChannelBitmapElement::size());
  // Zone bitmap
  image(0).addElement(Offset::zoneBitmap(), ZoneBitmapElement::size());
  // Contacts bitmap
  image(0).addElement(Offset::contactBitmap(), ContactBitmapElement::size());
  // Analog contacts bytemap
  image(0).addElement(Offset::dtmfContactBytemap(), DTMFContactBytemapElement::size());
  // RX group list bitmaps
  image(0).addElement(Offset::groupListBitmap(), GroupListBitmapElement::size());
  // Scan list bitmaps
  image(0).addElement(Offset::scanListBitmap(), ScanListBitmapElement::size());
  // Radio IDs bitmaps
  image(0).addElement(Offset::radioIDBitmap(), RadioIDBitmapElement::size());
  // Message bitmaps
  image(0).addElement(Offset::messageBytemap(), MessageBytemapElement::size());
  // Status messages
  image(0).addElement(Offset::statusMessageBitmap(), StatusMessageBitmapElement::size());
  // FM Broadcast bitmaps
  image(0).addElement(Offset::wfmChannelBitmap(), WFMChannelBitmapElement::size());
  // 5-Tone function bitmaps
  image(0).addElement(Offset::fiveToneIdBitmap(), FiveToneIDBitmapElement::size());
  // 2-Tone function bitmaps
  image(0).addElement(Offset::twoToneIdBitmap(), TwoToneIDBitmapElement::size());
  image(0).addElement(Offset::twoToneFunctionBitmap(), TwoToneFunctionBitmapElement::size());

  return true;
}


void
D868UVCodeplug::setBitmaps(Context& ctx)
{
  // Mark first radio ID as valid
  RadioIDBitmapElement radioid_bitmap(data(Offset::radioIDBitmap()));
  unsigned int num_radio_ids = std::min(Limit::numRadioIDs(), ctx.count<DMRRadioID>());
  radioid_bitmap.clear(); radioid_bitmap.enableFirst(num_radio_ids);

  // Mark valid channels (set bit)
  ChannelBitmapElement channel_bitmap(data(Offset::channelBitmap()));
  unsigned int num_channels = std::min(Limit::numChannels(), ctx.count<Channel>());
  channel_bitmap.clear(); channel_bitmap.enableFirst(num_channels);

  // Mark valid contacts (clear bit)
  ContactBitmapElement contact_bitmap(data(Offset::contactBitmap()));
  unsigned int num_contacts = std::min(Limit::numContacts(), ctx.count<DMRContact>());
  contact_bitmap.clear(); contact_bitmap.enableFirst(num_contacts);

  // Mark valid analog contacts (clear bytes)
  DTMFContactBytemapElement analog_contact_bitmap(data(Offset::dtmfContactBytemap()));
  unsigned int num_dtmf_contacts = std::min(Limit::numDTMFContacts(), ctx.count<DTMFContact>());
  analog_contact_bitmap.clear(); analog_contact_bitmap.enableFirst(num_dtmf_contacts);

  // Mark valid zones (set bits)
  ZoneBitmapElement zone_bitmap(data(Offset::zoneBitmap()));
  unsigned int num_zones = std::min(Limit::numZones(), ctx.count<Zone>());
  zone_bitmap.clear();
  for (unsigned int i=0,z=0; i<num_zones; i++) {
    zone_bitmap.setEncoded(z, true); z++;
    if (0 != ctx.get<Zone>(i)->B()->count()) {
      zone_bitmap.setEncoded(z, true); z++;
    }
  }

  // Mark group lists
  GroupListBitmapElement group_bitmap(data(Offset::groupListBitmap()));
  unsigned int num_group_lists = std::min(Limit::numGroupLists(), ctx.count<RXGroupList>());
  group_bitmap.clear(); group_bitmap.enableFirst(num_group_lists);

  // Mark scan lists
  ScanListBitmapElement scan_bitmap(data(Offset::scanListBitmap()));
  unsigned int num_scan_lists = std::min(Limit::numScanLists(), ctx.count<ScanList>());
  scan_bitmap.clear(); scan_bitmap.enableFirst(num_scan_lists);
}


bool
D868UVCodeplug::encodeElements(const Flags &flags, Context &ctx, const ErrorStack &err)
{
  if (! this->encodeRadioID(flags, ctx, err))
    return false;

  if (! this->encodeGeneralSettings(flags, ctx, err))
    return false;

  if (! this->encodeRepeaterOffsetFrequencies(flags, ctx, err))
    return false;

  if (! this->encodeBootSettings(flags, ctx, err))
    return false;

  if (! this->encodeChannels(flags, ctx, err))
    return false;

  if (! this->encodeContacts(flags, ctx, err))
    return false;

  if (! this->encodeAnalogContacts(flags, ctx, err))
    return false;

  if (! this->encodeRXGroupLists(flags, ctx, err))
    return false;

  if (! this->encodeZones(flags, ctx, err))
    return false;

  if (! this->encodeScanLists(flags, ctx, err))
    return false;

  if (! this->encodeGPSSystems(flags, ctx, err))
    return false;

  return true;
}

bool
D868UVCodeplug::decodeElements(Context &ctx, const ErrorStack &err)
{
  if (! this->setRadioID(ctx, err))
    return false;

  if (! this->decodeGeneralSettings(ctx, err))
    return false;

  if (! this->decodeRepeaterOffsetFrequencies(ctx, err))
    return false;

  if (! this->decodeBootSettings(ctx, err))
    return false;

  if (! this->createChannels(ctx, err))
    return false;

  if (! this->createContacts(ctx, err))
    return false;

  if (! this->createAnalogContacts(ctx, err))
    return false;

  if (! this->createRXGroupLists(ctx, err))
    return false;

  if (! this->linkRXGroupLists(ctx, err))
    return false;

  if (! this->createZones(ctx, err))
    return false;

  if (! this->linkZones(ctx, err))
    return false;

  if (! this->createScanLists(ctx, err))
    return false;

  if (! this->linkScanLists(ctx, err))
    return false;

  if (! this->createGPSSystems(ctx, err))
    return false;

  if (! this->linkChannels(ctx, err))
    return false;

  if (! this->linkGPSSystems(ctx, err))
    return false;

  if (! this->linkGeneralSettings(ctx, err)) {
    return false;
  }

  return true;
}


void
D868UVCodeplug::allocateChannels() {
  /* Allocate channels */
  ChannelBitmapElement channel_bitmap(data(Offset::channelBitmap()));
  for (uint16_t i=0; i<Limit::numChannels(); i++) {
    // if disabled -> skip
    if (! channel_bitmap.isEncoded(i))
      continue;
    // compute address for channel
    uint16_t bank = i/Limit::channelsPerBank(), idx = i%Limit::channelsPerBank();
    uint32_t addr = Offset::channelBanks() +
        + bank * Offset::betweenChannelBanks()
        + idx * ChannelElement::size();
    if (!isAllocated(addr, 0)) {
      image(0).addElement(addr, ChannelElement::size());
    }
  }
}

bool
D868UVCodeplug::encodeChannels(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)

  // Encode channels
  for (int i=0; i<ctx.config()->channelList()->count(); i++) {
    // enable channel
    uint16_t bank = i/Limit::channelsPerBank(), idx = i%Limit::channelsPerBank();
    ChannelElement ch(data(Offset::channelBanks() + bank * Offset::betweenChannelBanks()
                           + idx * ChannelElement::size()));
    if (! ch.fromChannelObj(ctx.config()->channelList()->channel(i), ctx))
      return false;
  }
  return true;
}

bool
D868UVCodeplug::createChannels(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)
  ChannelBitmapElement channel_bitmap(data(Offset::channelBitmap()));

  // Create channels
  for (uint16_t i=0; i<Limit::numChannels(); i++) {
    // Check if channel is enabled:
    uint16_t bank = i/Limit::channelsPerBank(), idx = i%Limit::channelsPerBank();
    if (! channel_bitmap.isEncoded(i))
      continue;
    ChannelElement ch(data(Offset::channelBanks() + bank*Offset::betweenChannelBanks()
                           + idx*ChannelElement::size()));
    if (Channel *obj = ch.toChannelObj(ctx)) {
      ctx.config()->channelList()->add(obj); ctx.add(obj, i);
    }
  }
  return true;
}

bool
D868UVCodeplug::linkChannels(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)
  ChannelBitmapElement channel_bitmap(data(Offset::channelBitmap()));
  // Link channel objects
  for (uint16_t i=0; i<Limit::numChannels(); i++) {
    // Check if channel is enabled:
    if (! channel_bitmap.isEncoded(i))
      continue;
    uint16_t bank = i/Limit::channelsPerBank(), idx = i%Limit::channelsPerBank();
    ChannelElement ch(data(Offset::channelBanks() + bank*Offset::betweenChannelBanks()
                           + idx*ChannelElement::size()));
    if (ctx.has<Channel>(i))
      ch.linkChannelObj(ctx.get<Channel>(i), ctx);
  }
  return true;
}


void
D868UVCodeplug::allocateVFOSettings() {
  // Allocate VFO channels
  image(0).addElement(Offset::vfoA(), ChannelElement::size());
  image(0).addElement(Offset::vfoA()+0x2000, ChannelElement::size());
  image(0).addElement(Offset::vfoB(), ChannelElement::size());
  image(0).addElement(Offset::vfoB()+0x2000, ChannelElement::size());
}

void
D868UVCodeplug::allocateContacts() {
  /* Allocate contacts */
  ContactBitmapElement contact_bitmap(data(Offset::contactBitmap()));
  unsigned contactCount=0;
  for (uint16_t i=0; i<Limit::numContacts(); i++) {
    // enabled if false (ass hole)
    if (! contact_bitmap.isEncoded(i))
      continue;
    contactCount++;
    uint32_t bank_addr = Offset::contactBanks() + (i/Limit::contactsPerBank())*Offset::betweenContactBanks();
    uint32_t addr = bank_addr + ((i%Limit::contactsPerBank())/Limit::contactsPerBlock())*Offset::betweenContactBlocks();
    if (!isAllocated(addr, 0)) {
      image(0).addElement(addr, Offset::betweenContactBlocks());
      memset(data(addr), 0x00, Offset::betweenContactBlocks());
    }
  }

  if (contactCount) {
    image(0).addElement(Offset::contactIndex(), align_size(4*contactCount, 16));
    memset(data(Offset::contactIndex()), 0xff, align_size(4*contactCount, 16));
    image(0).addElement(Offset::contactIdTable(), align_size(ContactMapElement::size()*(1+contactCount), 16));
    memset(data(Offset::contactIdTable()), 0xff, align_size(ContactMapElement::size()*(1+contactCount), 16));
  }
}

bool
D868UVCodeplug::encodeContacts(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)

  QVector<DMRContact*> contacts;
  // Encode contacts and also collect id<->index map
  for (int i=0; i<ctx.config()->contacts()->digitalCount(); i++) {
    uint32_t bank_addr = Offset::contactBanks() + (i/Limit::contactsPerBank())*Offset::betweenContactBanks();
    uint32_t addr = bank_addr + (i%Limit::contactsPerBank())*ContactElement::size();
    ContactElement con(data(addr));
    DMRContact *contact = ctx.config()->contacts()->digitalContact(i);
    if(! con.fromContactObj(contact, ctx))
      return false;
    ((uint32_t *)data(Offset::contactIndex()))[i] = qToLittleEndian(i);
    contacts.append(contact);
  }
  // encode index map for contacts
  std::sort(contacts.begin(), contacts.end(),
            [](DMRContact *a, DMRContact *b) {
    return a->number() < b->number();
  });
  for (int i=0; i<contacts.size(); i++) {
    ContactMapElement el(data(Offset::contactIdTable() + i*ContactMapElement::size()));
    el.setID(contacts[i]->number(), (DMRContact::GroupCall==contacts[i]->type()));
    el.setIndex(ctx.index(contacts[i]));
  }
  return true;
}

bool
D868UVCodeplug::createContacts(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  // Create digital contacts
  ContactBitmapElement contact_bitmap(data(Offset::contactBitmap()));
  for (uint16_t i=0; i<Limit::numContacts(); i++) {
    // Check if contact is enabled:
    if (! contact_bitmap.isEncoded(i))
      continue;
    uint32_t bank_addr = Offset::contactBanks() + (i/Limit::contactsPerBank())*Offset::betweenContactBanks();
    uint32_t addr = bank_addr + (i%Limit::contactsPerBank())*ContactElement::size();
    ContactElement con(data(addr));
    if (DMRContact *obj = con.toContactObj(ctx)) {
      ctx.config()->contacts()->add(obj); ctx.add(obj, i);
    }
  }
  return true;
}


void
D868UVCodeplug::allocateAnalogContacts() {
  /* Allocate analog contacts */
  DTMFContactBytemapElement analog_contact_bytemap(data(Offset::dtmfContactBytemap()));
  for (uint8_t i=0; i<Limit::numDTMFContacts(); i++) {
    // if disabled -> skip
    if (! analog_contact_bytemap.isEncoded(i))
      continue;
    uint32_t bank_addr = Offset::dtmfContacts() + (i/2)*(2*DTMFContactElement::size());
    if (! isAllocated(bank_addr, 0)) {
      image(0).addElement(bank_addr, 2*DTMFContactElement::size());
    }
  }
  image(0).addElement(Offset::dtmfIndex(), 1*Limit::numDTMFContacts());
}

bool
D868UVCodeplug::encodeAnalogContacts(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)

  uint8_t *idxlst = data(Offset::dtmfIndex());
  memset(idxlst, 0xff, 1*Limit::numDTMFContacts());
  for (unsigned int i=0; i<ctx.count<DTMFContact>(); i++) {
    DTMFContactElement cont(data(Offset::dtmfContacts() + i*DTMFContactElement::size()));
    cont.fromContact(ctx.config()->contacts()->dtmfContact(i));
    idxlst[i] = i;
  }
  return true;
}

bool
D868UVCodeplug::createAnalogContacts(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  DTMFContactBytemapElement analog_contact_bytemap(data(Offset::dtmfContactBytemap()));
  for (unsigned int i=0; i<Limit::numDTMFContacts(); i++) {
    // if disabled -> skip
    if (! analog_contact_bytemap.isEncoded(i))
      continue;
    DTMFContactElement cont(data(Offset::dtmfContacts() + i*DTMFContactElement::size()));
    if (DTMFContact *dtmf = cont.toContact()) {
      ctx.config()->contacts()->add(dtmf);
      ctx.add(dtmf, i);
    }
  }

  return true;
}


void
D868UVCodeplug::allocateRadioIDs() {
  /* Allocate radio IDs */
  RadioIDBitmapElement radioid_bitmap(data(Offset::radioIDBitmap()));
  for (uint8_t i=0; i<Limit::numRadioIDs(); i++) {
    // if disabled -> skip
    if (! radioid_bitmap.isEncoded(i))
      continue;
    // Allocate radio IDs individually
    uint32_t addr = Offset::radioIDs() + i*RadioIDElement::size();
    if (! isAllocated(addr, 0)) {
      image(0).addElement(addr, RadioIDElement::size());
    }
  }
}

bool
D868UVCodeplug::encodeRadioID(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)

  // Encode radio IDs
  for (unsigned int i=0; i<ctx.count<DMRRadioID>(); i++) {
    RadioIDElement(data(Offset::radioIDs() + i*RadioIDElement::size())).fromRadioID(
          ctx.config()->radioIDs()->getId(i));
  }
  return true;
}

bool
D868UVCodeplug::setRadioID(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  // Find a valid RadioID
  RadioIDBitmapElement radio_id_bitmap(data(Offset::radioIDBitmap()));
  for (uint16_t i=0; i<Limit::numRadioIDs(); i++) {
    if (! radio_id_bitmap.isEncoded(i))
      continue;
    RadioIDElement id(data(Offset::radioIDs() + i*RadioIDElement::size()));
    if (DMRRadioID *rid = id.toRadioID()) {
      ctx.config()->radioIDs()->add(rid);  ctx.add(rid, i);
    }
  }
  return true;
}


void
D868UVCodeplug::allocateRXGroupLists() {
  /*
   * Allocate group lists
   */
  GroupListBitmapElement grouplist_bitmap(data(Offset::groupListBitmap()));
  for (uint16_t i=0; i<Limit::numGroupLists(); i++) {
    // if disabled -> skip
    if (! grouplist_bitmap.isEncoded(i))
      continue;
    // Allocate RX group lists indivitually
    uint32_t addr = Offset::groupLists() + i*Offset::betweenGroupLists();
    if (! isAllocated(addr, 0)) {
      image(0).addElement(addr, GroupListElement::size());
      GroupListElement(data(addr)).clear();
    }
  }

}

bool
D868UVCodeplug::encodeRXGroupLists(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)

  // Encode RX group-lists
  for (int i=0; i<ctx.config()->rxGroupLists()->count(); i++) {
    GroupListElement grp(data(Offset::groupLists() + i*Offset::betweenGroupLists()));
    grp.fromGroupListObj(ctx.config()->rxGroupLists()->list(i), ctx);
  }

  return true;
}

bool
D868UVCodeplug::createRXGroupLists(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  // Create RX group lists
  GroupListBitmapElement grouplist_bitmap(data(Offset::groupListBitmap()));
  for (uint16_t i=0; i<Limit::numGroupLists(); i++) {
    // check if group list is enabled
    if (! grouplist_bitmap.isEncoded(i))
      continue;
    // construct RXGroupList from definition
    GroupListElement grp(data(Offset::groupLists() + i*Offset::betweenGroupLists()));
    if (RXGroupList *obj = grp.toGroupListObj()) {
      ctx.config()->rxGroupLists()->add(obj); ctx.add(obj, i);
    }
  }
  return true;
}

bool
D868UVCodeplug::linkRXGroupLists(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  GroupListBitmapElement grouplist_bitmap(data(Offset::groupListBitmap()));
  for (uint16_t i=0; i<Limit::numGroupLists(); i++) {
    // check if group list is enabled
    if (! grouplist_bitmap.isEncoded(i))
      continue;

    // link group list
    GroupListElement grp(data(Offset::groupLists() + i*Offset::betweenGroupLists()));
    if (! grp.linkGroupList(ctx.get<RXGroupList>(i), ctx)) {
      logError() << "Cannot link RX group list idx=" << i;
      return false;
    }
  }

  return true;
}


void
D868UVCodeplug::allocateZones() {
  ZoneBitmapElement zone_bitmap(data(Offset::zoneBitmap()));
  for (uint16_t i=0; i<Limit::numZones(); i++) {
    // if invalid -> skip
    if (! zone_bitmap.isEncoded(i))
      continue;
    // Allocate zone itself
    image(0).addElement(Offset::zoneChannels()+i*Offset::betweenZoneChannels(), Size::zoneChannels());
    image(0).addElement(Offset::zoneNames()+i*Offset::betweenZoneNames(), Size::zoneName());
  }
}

bool
D868UVCodeplug::encodeZones(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err);

  // Encode zones
  unsigned zidx = 0;
  for (unsigned int i=0; i<ctx.count<Zone>(); i++) {
    Zone *zone = ctx.get<Zone>(i);
    // Clear name and channel list
    uint8_t  *name     = (uint8_t *)data(Offset::zoneNames() + zidx*Offset::betweenZoneNames());
    uint16_t *channels = (uint16_t *)data(Offset::zoneChannels() + zidx*Offset::betweenZoneChannels());
    memset(name, 0, Size::zoneName());
    memset(channels, 0xff, Size::zoneChannels());
    if (zone->B()->count())
      encode_ascii(name, zone->name()+" A", Limit::zoneNameLength(), 0);
    else
      encode_ascii(name, zone->name(), Limit::zoneNameLength(), 0);
    // Handle list A
    for (int j=0; j<zone->A()->count(); j++) {
      channels[j] = qToLittleEndian(ctx.index(zone->A()->get(j)->as<Channel>()));
    }

    if (! encodeZone(zidx, zone, false, flags, ctx, err))
      return false;
    zidx++;

    if (0 == zone->B()->count())
      continue;

    // Process list B if present
    name     = (uint8_t *)data(Offset::zoneNames() + zidx*Offset::betweenZoneNames());
    channels = (uint16_t *)data(Offset::zoneChannels() + zidx*Offset::betweenZoneChannels());
    memset(name, 0, Size::zoneName());
    memset(channels, 0xff, Size::zoneChannels());
    encode_ascii(name, zone->name()+" B", Limit::zoneNameLength(), 0);
    // Handle list B
    for (int j=0; j<zone->B()->count(); j++) {
      channels[j] = qToLittleEndian(ctx.index(zone->B()->get(j)->as<Channel>()));
    }

    if (! encodeZone(zidx, zone, true, flags, ctx, err))
      return false;
    zidx++;
  }
  return true;
}

bool
D868UVCodeplug::encodeZone(int i, Zone *zone, bool isB, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(i); Q_UNUSED(zone); Q_UNUSED(isB); Q_UNUSED(flags); Q_UNUSED(ctx); Q_UNUSED(err)
  return true;
}

bool
D868UVCodeplug::createZones(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  // Create zones
  ZoneBitmapElement zone_bitmap(data(Offset::zoneBitmap()));
  QString last_zonename, last_zonebasename; Zone *last_zone = nullptr;
  bool extend_last_zone = false;
  for (uint16_t i=0; i<Limit::numZones(); i++) {
    // Check if zone is enabled:
    if (! zone_bitmap.isEncoded(i))
      continue;
    // Determine whether this zone should be combined with the previous one
    QString zonename = decode_ascii(
          data(Offset::zoneNames()+i*Offset::betweenZoneNames()),
          Limit::zoneNameLength(), 0);
    QString zonebasename = zonename; zonebasename.chop(2);
    extend_last_zone = ( zonename.endsWith(" B") && last_zonename.endsWith(" A")
                         && (zonebasename == last_zonebasename)
                         && (nullptr != last_zone) && (0 == last_zone->B()->count()) );
    last_zonename = zonename;
    last_zonebasename = zonebasename;

    // If enabled, create zone with name
    if (! extend_last_zone) {
      last_zone = new Zone(zonename);
      if (! decodeZone(i, last_zone, false, ctx, err)) {
        last_zone->deleteLater();
        return false;
      }
      // add to config
      logDebug() << "Store zone '" << zonename << "' at index " << i << ".";
      ctx.config()->zones()->add(last_zone); ctx.add(last_zone, i);
    } else {
      // when extending the last zone, chop its name to remove the "... A" part.
      last_zone->setName(last_zonebasename);
      if (! decodeZone(i, last_zone, true, ctx, err)) {
        last_zone->deleteLater();
        return false;
      }
      logDebug() << "Store merged zone '" << last_zonebasename << "' at index " << i << ".";
      ctx.add(last_zone, i);
    }
  }
  return true;
}

bool
D868UVCodeplug::decodeZone(int i, Zone *zone, bool isB, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(i); Q_UNUSED(zone); Q_UNUSED(isB); Q_UNUSED(ctx); Q_UNUSED(err)
  return true;
}

bool
D868UVCodeplug::linkZones(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  // Create zones
  ZoneBitmapElement zone_bitmap(data(Offset::zoneBitmap()));
  QString last_zonename, last_zonebasename; Zone *last_zone = nullptr;
  bool extend_last_zone = false;
  for (uint16_t i=0; i<Limit::numZones(); i++) {
    // Check if zone is enabled:
    if (! zone_bitmap.isEncoded(i))
      continue;
    // Determine whether this zone should be combined with the previous one
    QString zonename = decode_ascii(
          data(Offset::zoneNames()+i*Offset::betweenZoneNames()),
          Limit::zoneNameLength(), 0);
    QString zonebasename = zonename; zonebasename.chop(2);
    extend_last_zone = ( zonename.endsWith(" B") && last_zonename.endsWith(" A")
                         && (zonebasename == last_zonebasename)
                         && (nullptr != last_zone) && (0 == last_zone->B()->count()) );
    last_zonename = zonename;
    last_zonebasename = zonebasename;

    // If enabled, get zone
    if (! extend_last_zone) {
      last_zone = ctx.get<Zone>(i);
    } else {
      // when extending the last zone, chop its name to remove the "... A" part.
      last_zone->setName(last_zonebasename);
    }

    // link zone
    uint16_t *channels = (uint16_t *)data(Offset::zoneChannels()+i*Offset::betweenZoneChannels());
    for (uint8_t j=0; j<Limit::numChannelsPerZone(); j++, channels++) {
      // If not enabled -> continue
      if (0xffff == *channels)
        continue;
      // Get channel index and check if defined
      uint16_t cidx = qFromLittleEndian(*channels);
      if (! ctx.has<Channel>(cidx))
        continue;
      // If defined -> add channel to zone obj
      if (extend_last_zone)
        last_zone->B()->add(ctx.get<Channel>(cidx));
      else
        last_zone->A()->add(ctx.get<Channel>(cidx));
    }

    if (! linkZone(i, last_zone, extend_last_zone, ctx, err))
      return false;
  }
  return true;
}

bool
D868UVCodeplug::linkZone(int i, Zone *zone, bool isB, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(i); Q_UNUSED(zone); Q_UNUSED(isB); Q_UNUSED(ctx); Q_UNUSED(err)
  return true;
}

void
D868UVCodeplug::allocateScanLists() {
  ScanListBitmapElement scanlist_bitmap(data(Offset::scanListBitmap()));
  for (uint8_t i=0; i<Limit::numScanLists(); i++) {
    // if disabled -> skip
    if (! scanlist_bitmap.isEncoded(i))
      continue;
    // Allocate scan lists indivitually
    uint8_t bank = (i/Limit::numScanListsPerBank()), bank_idx = (i%Limit::numScanListsPerBank());
    uint32_t addr = Offset::scanListBanks() + bank*Offset::betweenScanListBanks()
        + bank_idx*Offset::betweenScanLists();
    if (!isAllocated(addr, 0)) {
      image(0).addElement(addr, ScanListElement::size());
      ScanListElement(data(addr)).clear();
    }
  }
}

bool
D868UVCodeplug::encodeScanLists(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err);

  // Encode scan lists
  unsigned int num_scan_lists = std::min(Limit::numScanLists(), ctx.count<ScanList>());
  for (unsigned int i=0; i<num_scan_lists; i++) {
    uint8_t bank = i/Limit::numScanListsPerBank(), idx = i%Limit::numScanListsPerBank();
    ScanListElement scan(data(Offset::scanListBanks() + bank*Offset::betweenScanListBanks()
                              + idx*Offset::betweenScanLists()));
    scan.fromScanListObj(ctx.config()->scanlists()->scanlist(i), ctx);
  }
  return true;
}

bool
D868UVCodeplug::createScanLists(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  // Create scan lists
  ScanListBitmapElement scanlist_bitmap(data(Offset::scanListBitmap()));
  for (unsigned int i=0; i<Limit::numScanLists(); i++) {
    if (! scanlist_bitmap.isEncoded(i))
      continue;
    uint8_t bank = i/Limit::numScanListsPerBank(), bank_idx = i%Limit::numScanListsPerBank();
    uint32_t addr = Offset::scanListBanks() + bank*Offset::betweenScanListBanks()
        + bank_idx*Offset::betweenScanLists();
    ScanListElement scanl(data(addr));
    // Create scanlist
    ScanList *obj = scanl.toScanListObj();
    ctx.config()->scanlists()->add(obj); ctx.add(obj, i);
  }
  return true;
}

bool
D868UVCodeplug::linkScanLists(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  ScanListBitmapElement scanlist_bitmap(data(Offset::scanListBitmap()));
  for (unsigned i=0; i<Limit::numScanLists(); i++) {
    if (! scanlist_bitmap.isEncoded(i))
      continue;
    uint8_t bank = i/Limit::numScanListsPerBank(), bank_idx = i%Limit::numScanListsPerBank();
    uint32_t addr = Offset::scanListBanks() + bank*Offset::betweenScanListBanks()
        + bank_idx*Offset::betweenScanLists();
    ScanListElement scanl(data(addr));
    // Create scanlist
    ScanList *obj = ctx.get<ScanList>(i);
    // Link scanlists immediately, all channels are defined already
    ctx.config()->scanlists()->add(obj); scanl.linkScanListObj(obj, ctx);
  }
  return true;
}


void
D868UVCodeplug::allocateGeneralSettings() {
  image(0).addElement(Offset::settings(), GeneralSettingsElement::size());
}

bool
D868UVCodeplug::encodeGeneralSettings(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  return GeneralSettingsElement(data(Offset::settings())).fromConfig(flags, ctx);
}

bool
D868UVCodeplug::decodeGeneralSettings(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  return GeneralSettingsElement(data(Offset::settings())).updateConfig(ctx);
}

bool
D868UVCodeplug::linkGeneralSettings(Context &ctx, const ErrorStack &err) {
  return GeneralSettingsElement(data(Offset::settings())).linkSettings(ctx.config()->settings(), ctx, err);
}

void
D868UVCodeplug::allocateZoneChannelList() {
  image(0).addElement(Offset::zoneChannelList(), ZoneChannelListElement::size());
}


void
D868UVCodeplug::allocateDTMFNumbers() {
  image(0).addElement(Offset::dtmfIDList(), DTMFIDListElement::size());
}


void
D868UVCodeplug::allocateBootSettings() {
  image(0).addElement(Offset::bootSettings(), BootSettingsElement::size());
}

bool
D868UVCodeplug::encodeBootSettings(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  return BootSettingsElement(data(Offset::bootSettings())).fromConfig(flags, ctx);
}

bool
D868UVCodeplug::decodeBootSettings(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  return BootSettingsElement(data(Offset::bootSettings())).updateConfig(ctx);
}


void
D868UVCodeplug::allocateGPSSystems() {
  image(0).addElement(Offset::dmrAPRSSettings(), DMRAPRSSettingsElement::size());
  image(0).addElement(Offset::dmrAPRSMessage(), DMRAPRSMessageElement::size());
}

bool
D868UVCodeplug::encodeGPSSystems(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  DMRAPRSSettingsElement gps(data(Offset::dmrAPRSSettings()));
  return gps.fromConfig(flags, ctx);
}

bool
D868UVCodeplug::createGPSSystems(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  QSet<uint8_t> systems;
  // First find all GPS systems linked, that is referenced by any channel
  // Create channels
  ChannelBitmapElement channel_bitmap(data(Offset::channelBitmap()));
  for (uint16_t i=0; i<Limit::numChannels(); i++) {
    // Check if channel is enabled:
    uint16_t  bank = i/128, idx = i%128;
    if (! channel_bitmap.isEncoded(i))
      continue;
    if (ctx.get<Channel>(i)->is<FMChannel>())
      continue;
    ChannelElement ch(data(Offset::channelBanks() + bank*Offset::betweenChannelBanks()
                           + idx*ChannelElement::size()));
    if (ch.txDigitalAPRS())
      systems.insert(ch.digitalAPRSSystemIndex());
  }
  // Then create all referenced GPS systems
  DMRAPRSSettingsElement gps(data(Offset::dmrAPRSSettings()));
  for (QSet<uint8_t>::iterator idx=systems.begin(); idx!=systems.end(); idx++)
    gps.createGPSSystem(*idx, ctx);
  return true;
}

bool
D868UVCodeplug::linkGPSSystems(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  DMRAPRSSettingsElement gps(data(Offset::dmrAPRSSettings()));
  // Then link all referenced GPS systems
  for (uint8_t i=0; i<Limit::dmrAPRSSystems(); i++) {
    if (! ctx.has<GPSSystem>(i))
      continue;
    gps.linkGPSSystem(i, ctx);
  }
  return true;
}


void
D868UVCodeplug::allocateSMSMessages() {
  // Prefab. SMS messages
  MessageBytemapElement messages_bytemap(data(Offset::messageBytemap()));
  unsigned message_count = 0;
  for (uint8_t i=0; i<Limit::numMessages(); i++) {
    if (! messages_bytemap.isEncoded(i))
      continue;
    message_count++;
    uint32_t addr = Offset::messageBanks() + (i/Limit::numMessagePerBank())*Offset::betweenMessageBanks();
    if (!isAllocated(addr, 0)) {
      image(0).addElement(addr, Size::messageBank());
    }
  }
  if (message_count) {
    image(0).addElement(Offset::messageIndex(), Size::messageIndex()*message_count);
  }
}

void
D868UVCodeplug::allocateHotKeySettings() {
  // Allocate Hot Keys
  image(0).addElement(Offset::analogQuickCall(), AnalogQuickCallsElement::size());
  image(0).addElement(Offset::statusMessages(), StatusMessagesElement::size());
  image(0).addElement(Offset::hotKeySettings(), HotKeySettingsElement::size());
}

void
D868UVCodeplug::allocateRepeaterOffsetFrequencies() {
  // Offset frequencies
  image(0).addElement(Offset::offsetFrequencies(), RepeaterOffsetListElement::size());
}

bool
D868UVCodeplug::encodeRepeaterOffsetFrequencies(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err);

  // If no AnyTone extension is present -> leave untouched.
  if (! ctx.config()->settings()->anytoneExtension())
    return true;

  RepeaterOffsetListElement offsets(data(Offset::offsetFrequencies()));
  offsets.clear();
  for (unsigned int i=0; i<ctx.count<AnytoneAutoRepeaterOffset>(); i++) {
    offsets.setOffset(i, ctx.get<AnytoneAutoRepeaterOffset>(i)->offset());
  }
  return true;
}

bool
D868UVCodeplug::decodeRepeaterOffsetFrequencies(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  // Allocate extension, if not present.
  AnytoneSettingsExtension *ext = nullptr;
  if (ctx.config()->settings()->anytoneExtension()) {
    ext = ctx.config()->settings()->anytoneExtension();
  } else {
    ext = new AnytoneSettingsExtension();
    ctx.config()->settings()->setAnytoneExtension(ext);
  }

  // Decode offsets.
  RepeaterOffsetListElement offsets(data(Offset::offsetFrequencies()));
  for (unsigned int i=0; i<RepeaterOffsetListElement::Limit::numEntries(); i++) {
    if (offsets.isSet(i)) {
      AnytoneAutoRepeaterOffset *offset = new AnytoneAutoRepeaterOffset();
      offset->setOffset(offsets.offset(i));
      ext->autoRepeaterSettings()->offsets()->add(offset);
      ctx.add(offset, i);
    }
  }

  return true;
}


void
D868UVCodeplug::allocateAlarmSettings() {
  // Alarm settings
  image(0).addElement(Offset::alarmSettings(), AlarmSettingElement::size());
  image(0).addElement(Offset::alarmSettingsExtension(), DigitalAlarmExtensionElement::size());
}

void
D868UVCodeplug::allocateFMBroadcastSettings() {
  // FM broad-cast settings
  image(0).addElement(Offset::wfmChannels(), WFMChannelListElement::size());
  image(0).addElement(Offset::wfmVFO(), WFMVFOElement::size());
}

void
D868UVCodeplug::allocate5ToneIDs() {
  // Allocate 5-tone functions
  FiveToneIDBitmapElement bitmap(data(Offset::fiveToneIdBitmap()));
  for (uint8_t i=0; i<FiveToneIDListElement::Limit::numEntries(); i++) {
    if (! bitmap.isEncoded(i))
      continue;
    image(0).addElement(Offset::fiveToneIdList() + i*FiveToneIDElement::size(), FiveToneIDElement::size());
  }
}

void
D868UVCodeplug::allocate5ToneFunctions() {
  image(0).addElement(Offset::fiveToneFunctions(), FiveToneFunctionListElement::size());
}

void
D868UVCodeplug::allocate5ToneSettings() {
  image(0).addElement(Offset::fiveToneSettings(), FiveToneSettingsElement::size());
}

void
D868UVCodeplug::allocate2ToneIDs() {
  // Allocate 2-tone encoding
  TwoToneIDBitmapElement enc_bitmap(data(Offset::twoToneIdBitmap()));
  for (uint8_t i=0; i<Limit::numTwoToneIDs(); i++) {
    if (! enc_bitmap.isEncoded(i))
      continue;
    image(0).addElement(Offset::twoToneIdList() + i*TwoToneIDElement::size(), TwoToneIDElement::size());
  }
}


void
D868UVCodeplug::allocate2ToneFunctions() {
  // Allocate 2-tone decoding
  TwoToneFunctionBitmapElement dec_bitmap(data(Offset::twoToneFunctionBitmap()));
  for (uint8_t i=0; i<Limit::numTwoToneFunctions(); i++) {
    if (! dec_bitmap.isEncoded(i))
      continue;
    image(0).addElement(Offset::twoToneFunctionList() + i*TwoToneFunctionElement::size(),
                        TwoToneFunctionElement::size());
  }
}

void
D868UVCodeplug::allocate2ToneSettings() {
  image(0).addElement(Offset::twoToneSettings(), TwoToneSettingsElement::size());
}


void
D868UVCodeplug::allocateDTMFSettings() {
  image(0).addElement(Offset::dtmfSettings(), DTMFSettingsElement::size());
}
