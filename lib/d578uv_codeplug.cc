#include "d578uv_codeplug.hh"
#include "config.hh"
#include "utils.hh"
#include "channel.hh"
#include "gpssystem.hh"
#include "userdatabase.hh"
#include "config.h"
#include "logger.hh"

#include <QTimeZone>
#include <QtEndian>


/* ******************************************************************************************** *
 * Implementation of D578UVCodeplug::ChannelElement
 * ******************************************************************************************** */
D578UVCodeplug::ChannelElement::ChannelElement(uint8_t *ptr, unsigned size)
  : D878UVCodeplug::ChannelElement(ptr, size)
{
  // pass...
}

D578UVCodeplug::ChannelElement::ChannelElement(uint8_t *ptr)
  : D878UVCodeplug::ChannelElement(ptr)
{
  // pass...
}

bool
D578UVCodeplug::ChannelElement::handsFree() const {
  return getBit(0x0034, 2);
}
void
D578UVCodeplug::ChannelElement::enableHandsFree(bool enable) {
  setBit(0x0034, 2, enable);
}

bool
D578UVCodeplug::ChannelElement::roamingEnabled() const {
  // inverted!
  return !getBit(0x0034,3);
}
void
D578UVCodeplug::ChannelElement::enableRoaming(bool enable) {
  // inverted!
  setBit(0x0034, 3, !enable);
}

bool
D578UVCodeplug::ChannelElement::dataACK() const {
  // inverted!
  return !getBit(0x003d,3);
}
void
D578UVCodeplug::ChannelElement::enableDataACK(bool enable) {
  // inverted!
  setBit(0x003d, 3, !enable);
}

unsigned
D578UVCodeplug::ChannelElement::dmrEncryptionKeyIndex() const {
  return 0;
}
void
D578UVCodeplug::ChannelElement::setDMREncryptionKeyIndex(unsigned idx) {
  Q_UNUSED(idx)
}

bool
D578UVCodeplug::ChannelElement::analogScambler() const {
  return getUInt8(0x003a);
}
void
D578UVCodeplug::ChannelElement::enableAnalogScamber(bool enable) {
  setUInt8(0x003a, (enable ? 0x01 : 0x00));
}

Channel *
D578UVCodeplug::ChannelElement::toChannelObj(Context &ctx) const {
  Channel *ch = D878UVCodeplug::ChannelElement::toChannelObj(ctx);
  if (nullptr == ch)
    return nullptr;

  // Apply extensions
  if (FMChannel *fch = ch->as<FMChannel>()) {
    if (AnytoneFMChannelExtension *ext = fch->anytoneChannelExtension()) {
      // Common settings
      ext->enableHandsFree(handsFree());
      // FM specific settings
      ext->enableScrambler(analogScambler());
    }
  } else if (DMRChannel *dch = ch->as<DMRChannel>()) {
    if (AnytoneDMRChannelExtension *ext = dch->anytoneChannelExtension()) {
      // Common settings
      ext->enableHandsFree(handsFree());
      // DMR specific extensions
    }
  }

  // Done.
  return ch;
}


/* ******************************************************************************************** *
 * Implementation of D578UVCodeplug::GeneralSettingsElement
 * ******************************************************************************************** */
static QVector<QTimeZone> _indexToTZ = {
  QTimeZone(-43200), QTimeZone(-39600), QTimeZone(-36000), QTimeZone(-32400),
  QTimeZone(-28800), QTimeZone(-25200), QTimeZone(-21600), QTimeZone(-18000),
  QTimeZone(-14400), QTimeZone(-12600), QTimeZone(-10800), QTimeZone(- 7200),
  QTimeZone(- 3600), QTimeZone(     0), QTimeZone(  3600), QTimeZone(  7200),
  QTimeZone( 10800), QTimeZone( 12600), QTimeZone(-28800), QTimeZone( 14400),
  QTimeZone( 16200), QTimeZone( 18000), QTimeZone( 19800), QTimeZone( 20700),
  QTimeZone( 21600), QTimeZone( 25200), QTimeZone( 28600), QTimeZone( 30600),
  QTimeZone( 32400), QTimeZone( 36000), QTimeZone( 39600), QTimeZone( 43200),
  QTimeZone( 46800) };

D578UVCodeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr, unsigned size)
  : D878UVCodeplug::GeneralSettingsElement(ptr, size)
{
  // pass...
}

D578UVCodeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr)
  : D878UVCodeplug::GeneralSettingsElement(ptr, GeneralSettingsElement::size())
{
  // pass...
}

AnytoneSettingsExtension::PowerSave
D578UVCodeplug::GeneralSettingsElement::powerSave() const {
  return AnytoneSettingsExtension::PowerSave::Off;
}
void
D578UVCodeplug::GeneralSettingsElement::setPowerSave(AnytoneSettingsExtension::PowerSave mode) {
  Q_UNUSED(mode);
}

unsigned int
D578UVCodeplug::GeneralSettingsElement::voxLevel() const {
  return 0;
}
void
D578UVCodeplug::GeneralSettingsElement::setVOXLevel(unsigned int level) {
  Q_UNUSED(level);
}

Interval
D578UVCodeplug::GeneralSettingsElement::voxDelay() const {
  return Interval::fromSeconds(0);
}
void
D578UVCodeplug::GeneralSettingsElement::setVOXDelay(Interval intv) {
  Q_UNUSED(intv);
}

AnytoneSettingsExtension::VFOScanType
D578UVCodeplug::GeneralSettingsElement::vfoScanType() const {
  return (AnytoneSettingsExtension::VFOScanType)getUInt8(Offset::vfoScanType());
}
void
D578UVCodeplug::GeneralSettingsElement::setVFOScanType(AnytoneSettingsExtension::VFOScanType type) {
  setUInt8(Offset::vfoScanType(), (unsigned int) type);
}

unsigned
D578UVCodeplug::GeneralSettingsElement::dmrMicGain() const {
  return (((unsigned)getUInt8(Offset::dmrMicGain())+1)*10)/4;
}
void
D578UVCodeplug::GeneralSettingsElement::setDMRMicGain(unsigned gain) {
  gain = std::min(1U, std::min(10U, gain));
  setUInt8(Offset::dmrMicGain(), (gain*4)/10);
}


bool
D578UVCodeplug::GeneralSettingsElement::vfoModeA() const {
  return getUInt8(Offset::vfoModeA());
}
void
D578UVCodeplug::GeneralSettingsElement::enableVFOModeA(bool enable) {
  setUInt8(Offset::vfoModeA(), (enable ? 0x01 : 0x00));
}

bool
D578UVCodeplug::GeneralSettingsElement::vfoModeB() const {
  return getUInt8(Offset::vfoModeB());
}
void
D578UVCodeplug::GeneralSettingsElement::enableVFOModeB(bool enable) {
  setUInt8(Offset::vfoModeB(), (enable ? 0x01 : 0x00));
}

AnytoneSettingsExtension::STEType
D578UVCodeplug::GeneralSettingsElement::steType() const {
  return (AnytoneSettingsExtension::STEType)getUInt8(Offset::steType());
}
void
D578UVCodeplug::GeneralSettingsElement::setSTEType(AnytoneSettingsExtension::STEType type) {
  setUInt8(Offset::steType(), (unsigned)type);
}
double
D578UVCodeplug::GeneralSettingsElement::steFrequency() const {
  switch ((STEFrequency)getUInt8(Offset::steFrequency())) {
  case STEFrequency::Off: return 0;
  case STEFrequency::Hz55_2: return 55.2;
  case STEFrequency::Hz259_2: return 259.2;
  }
  return 0;
}
void
D578UVCodeplug::GeneralSettingsElement::setSTEFrequency(double freq) {
  if (0 >= freq) {
    setUInt8(Offset::steFrequency(), (unsigned)STEFrequency::Off);
  } else if (100 > freq) {
    setUInt8(Offset::steFrequency(), (unsigned)STEFrequency::Hz55_2);
  } else {
    setUInt8(Offset::steFrequency(), (unsigned)STEFrequency::Hz259_2);
  }
}

Interval
D578UVCodeplug::GeneralSettingsElement::groupCallHangTime() const {
  return Interval::fromSeconds(getUInt8(Offset::groupCallHangTime()));
}
void
D578UVCodeplug::GeneralSettingsElement::setGroupCallHangTime(Interval intv) {
  setUInt8(Offset::groupCallHangTime(), intv.seconds());
}
Interval
D578UVCodeplug::GeneralSettingsElement::privateCallHangTime() const {
  return Interval::fromSeconds(getUInt8(Offset::privateCallHangTime()));
}
void
D578UVCodeplug::GeneralSettingsElement::setPrivateCallHangTime(Interval intv) {
  setUInt8(Offset::privateCallHangTime(), intv.seconds());
}
Interval
D578UVCodeplug::GeneralSettingsElement::preWaveDelay() const {
  return Interval::fromMilliseconds((unsigned)getUInt8(Offset::preWaveDelay())*20);
}
void
D578UVCodeplug::GeneralSettingsElement::setPreWaveDelay(Interval intv) {
  setUInt8(Offset::preWaveDelay(), intv.milliseconds()/20);
}
Interval
D578UVCodeplug::GeneralSettingsElement::wakeHeadPeriod() const {
  return Interval::fromMilliseconds(((unsigned)getUInt8(Offset::wakeHeadPeriod()))*20);
}
void
D578UVCodeplug::GeneralSettingsElement::setWakeHeadPeriod(Interval intv) {
  setUInt8(Offset::wakeHeadPeriod(), intv.milliseconds()/20);
}

unsigned
D578UVCodeplug::GeneralSettingsElement::wfmChannelIndex() const {
  return getUInt8(Offset::wfmChannelIndex());
}
void
D578UVCodeplug::GeneralSettingsElement::setWFMChannelIndex(unsigned idx) {
  setUInt8(Offset::wfmChannelIndex(), idx);
}
bool
D578UVCodeplug::GeneralSettingsElement::wfmVFOEnabled() const {
  return getUInt8(Offset::wfmVFOEnabled());
}
void
D578UVCodeplug::GeneralSettingsElement::enableWFMVFO(bool enable) {
  setUInt8(Offset::wfmVFOEnabled(), (enable ? 0x01 : 0x00));
}

unsigned
D578UVCodeplug::GeneralSettingsElement::memoryZoneA() const {
  return getUInt8(Offset::memZoneA());
}
void
D578UVCodeplug::GeneralSettingsElement::setMemoryZoneA(unsigned zone) {
  setUInt8(Offset::memZoneA(), zone);
}

unsigned
D578UVCodeplug::GeneralSettingsElement::memoryZoneB() const {
  return getUInt8(Offset::memZoneB());
}
void
D578UVCodeplug::GeneralSettingsElement::setMemoryZoneB(unsigned zone) {
  setUInt8(Offset::memZoneB(), zone);
}

bool
D578UVCodeplug::GeneralSettingsElement::wfmEnabled() const {
  return 0x00 != getUInt8(Offset::wfmEnable());
}
void
D578UVCodeplug::GeneralSettingsElement::enableWFM(bool enable) {
  setUInt8(Offset::wfmEnable(), enable ? 0x01 : 0x00);
}

bool
D578UVCodeplug::GeneralSettingsElement::recording() const {
  return getUInt8(Offset::enableRecoding());
}
void
D578UVCodeplug::GeneralSettingsElement::enableRecording(bool enable) {
  setUInt8(Offset::enableRecoding(), (enable ? 0x01 : 0x00));
}

unsigned
D578UVCodeplug::GeneralSettingsElement::dtmfToneDuration() const {
  return 50;
}
void
D578UVCodeplug::GeneralSettingsElement::setDTMFToneDuration(unsigned ms) {
  Q_UNUSED(ms);
}

bool
D578UVCodeplug::GeneralSettingsElement::manDown() const {
  return false;
}
void
D578UVCodeplug::GeneralSettingsElement::enableManDown(bool enable) {
  Q_UNUSED(enable);
}

unsigned
D578UVCodeplug::GeneralSettingsElement::brightness() const {
  return (getUInt8(Offset::displayBrightness())*10)/4;
}
void
D578UVCodeplug::GeneralSettingsElement::setBrightness(unsigned level) {
  setUInt8(Offset::displayBrightness(), (level*4)/10);
}

bool
D578UVCodeplug::GeneralSettingsElement::backlightPermanent() const {
  return true;
}
Interval
D578UVCodeplug::GeneralSettingsElement::rxBacklightDuration() const {
  return Interval::fromSeconds(0);
}
void
D578UVCodeplug::GeneralSettingsElement::setRXBacklightDuration(Interval intv) {
  Q_UNUSED(intv);
}
void
D578UVCodeplug::GeneralSettingsElement::enableBacklightPermanent() {
  // pass...
}

bool
D578UVCodeplug::GeneralSettingsElement::gps() const {
  return getUInt8(Offset::gpsEnable());
}
void
D578UVCodeplug::GeneralSettingsElement::enableGPS(bool enable) {
  setUInt8(Offset::gpsEnable(), (enable ? 0x01 : 0x00));
}

bool
D578UVCodeplug::GeneralSettingsElement::smsAlert() const {
  return getUInt8(Offset::smsAlert());
}
void
D578UVCodeplug::GeneralSettingsElement::enableSMSAlert(bool enable) {
  setUInt8(Offset::smsAlert(), (enable ? 0x01 : 0x00));
}

bool
D578UVCodeplug::GeneralSettingsElement::wfmMonitor() const {
  return getUInt8(Offset::wfmMonitor());
}
void
D578UVCodeplug::GeneralSettingsElement::enableWFMMonitor(bool enable) {
  setUInt8(Offset::wfmMonitor(), (enable ? 0x01 : 0x00));
}

bool
D578UVCodeplug::GeneralSettingsElement::activeChannelB() const {
  return getUInt8(Offset::activeChannelB());
}
void
D578UVCodeplug::GeneralSettingsElement::enableActiveChannelB(bool enable) {
  setUInt8(Offset::activeChannelB(), (enable ? 0x01 : 0x00));
}

bool
D578UVCodeplug::GeneralSettingsElement::subChannel() const {
  return getUInt8(Offset::subChannel());
}
void
D578UVCodeplug::GeneralSettingsElement::enableSubChannel(bool enable) {
  setUInt8(Offset::subChannel(), (enable ? 0x01 : 0x00));
}

Frequency
D578UVCodeplug::GeneralSettingsElement::tbstFrequency() const {
  switch ((TBSTFrequency)getUInt8(Offset::tbstFrequency())) {
  case TBSTFrequency::Hz1000: return Frequency::fromHz(1000);
  case TBSTFrequency::Hz1450: return Frequency::fromHz(1450);
  case TBSTFrequency::Hz1750: return Frequency::fromHz(1750);
  case TBSTFrequency::Hz2100: return Frequency::fromHz(2100);
  }
  return Frequency::fromHz(1750);
}
void
D578UVCodeplug::GeneralSettingsElement::setTBSTFrequency(Frequency freq) {
  if (1000 == freq.inHz()) {
    setUInt8(Offset::tbstFrequency(), (unsigned)TBSTFrequency::Hz1000);
  } else if (1450 == freq.inHz()) {
    setUInt8(Offset::tbstFrequency(), (unsigned)TBSTFrequency::Hz1450);
  } else if (1750 == freq.inHz()) {
    setUInt8(Offset::tbstFrequency(), (unsigned)TBSTFrequency::Hz1750);
  } else if (2100 == freq.inHz()) {
    setUInt8(Offset::tbstFrequency(), (unsigned)TBSTFrequency::Hz2100);
  } else {
    setUInt8(Offset::tbstFrequency(), (unsigned)TBSTFrequency::Hz1750);
  }
}

bool
D578UVCodeplug::GeneralSettingsElement::callAlert() const {
  return getUInt8(Offset::callAlert());
}
void
D578UVCodeplug::GeneralSettingsElement::enableCallAlert(bool enable) {
  setUInt8(Offset::callAlert(), (enable ? 0x01 : 0x00));
}

QTimeZone
D578UVCodeplug::GeneralSettingsElement::gpsTimeZone() const {
  int index = getUInt8(Offset::gpsTimeZone());
  if (index >= _indexToTZ.size())
    return _indexToTZ.back();
  return _indexToTZ.at(index);
}
void
D578UVCodeplug::GeneralSettingsElement::setGPSTimeZone(const QTimeZone &zone) {
  if (! _indexToTZ.contains(zone))
    setUInt8(Offset::gpsTimeZone(), 13); // <- Set to UTC
  else
    setUInt8(Offset::gpsTimeZone(), _indexToTZ.indexOf(zone));
}

bool
D578UVCodeplug::GeneralSettingsElement::dmrTalkPermit() const {
  return getBit(Offset::talkPermit(), 0);
}
void
D578UVCodeplug::GeneralSettingsElement::enableDMRTalkPermit(bool enable) {
  return setBit(Offset::talkPermit(), 0, enable);
}

bool
D578UVCodeplug::GeneralSettingsElement::dmrResetTone() const {
  return getUInt8(Offset::dmrResetTone());
}
void
D578UVCodeplug::GeneralSettingsElement::enableDMRResetTone(bool enable) {
  return setUInt8(Offset::dmrResetTone(), (enable ? 0x01 : 0x00));
}

bool
D578UVCodeplug::GeneralSettingsElement::idleChannelTone() const {
  return getUInt8(Offset::idleChannelTone());
}
void
D578UVCodeplug::GeneralSettingsElement::enableIdleChannelTone(bool enable) {
  return setUInt8(Offset::idleChannelTone(), (enable ? 0x01 : 0x00));
}

Interval
D578UVCodeplug::GeneralSettingsElement::menuExitTime() const {
  return Interval::fromSeconds(5 + 5*((unsigned) getUInt8(Offset::menuExitTime())));
}
void
D578UVCodeplug::GeneralSettingsElement::setMenuExitTime(Interval intv) {
  setUInt8(Offset::menuExitTime(), (std::max(5ULL, intv.seconds())-5)/5);
}

bool
D578UVCodeplug::GeneralSettingsElement::filterOwnID() const {
  return getUInt8(Offset::filterOwnID());
}
void
D578UVCodeplug::GeneralSettingsElement::enableFilterOwnID(bool enable) {
  setUInt8(Offset::filterOwnID(), (enable ? 0x01 : 0x00));
}

bool
D578UVCodeplug::GeneralSettingsElement::startupTone() const {
  return getUInt8(Offset::startupTone());
}
void
D578UVCodeplug::GeneralSettingsElement::enableStartupTone(bool enable) {
  return setUInt8(Offset::startupTone(), (enable ? 0x01 : 0x00));
}

bool
D578UVCodeplug::GeneralSettingsElement::callEndPrompt() const {
  return getUInt8(Offset::callEndPrompt());
}
void
D578UVCodeplug::GeneralSettingsElement::enableCallEndPrompt(bool enable) {
  return setUInt8(Offset::callEndPrompt(), (enable ? 0x01 : 0x00));
}

unsigned
D578UVCodeplug::GeneralSettingsElement::maxSpeakerVolume() const {
  return (((unsigned)getUInt8(Offset::maxSpeakerVolume()))*10)/8;
}
void
D578UVCodeplug::GeneralSettingsElement::setMaxSpeakerVolume(unsigned level) {
  setUInt8(Offset::maxSpeakerVolume(), (level*8)/10);
}

bool
D578UVCodeplug::GeneralSettingsElement::remoteStunKill() const {
  return getUInt8(Offset::remoteStunKill());
}
void
D578UVCodeplug::GeneralSettingsElement::enableRemoteStunKill(bool enable) {
  setUInt8(Offset::remoteStunKill(), (enable ? 0x01 : 0x00));
}

bool
D578UVCodeplug::GeneralSettingsElement::remoteMonitor() const {
  return getUInt8(Offset::remoteMonitor());
}
void
D578UVCodeplug::GeneralSettingsElement::enableRemoteMonitor(bool enable) {
  setUInt8(Offset::remoteMonitor(), (enable ? 0x01 : 0x00));
}

bool
D578UVCodeplug::GeneralSettingsElement::getGPSPosition() const {
  return getUInt8(Offset::getGPSPosition());
}
void
D578UVCodeplug::GeneralSettingsElement::enableGetGPSPosition(bool enable) {
  return setUInt8(Offset::getGPSPosition(), (enable ? 0x01 : 0x00));
}

Interval
D578UVCodeplug::GeneralSettingsElement::longPressDuration() const {
  return Interval::fromSeconds(((unsigned)getUInt8(Offset::longPressDuration()))+1);
}
void
D578UVCodeplug::GeneralSettingsElement::setLongPressDuration(Interval ms) {
  setUInt8(Offset::longPressDuration(), std::max(1ULL,ms.seconds())-1);
}

bool
D578UVCodeplug::GeneralSettingsElement::volumeChangePrompt() const {
  return getUInt8(Offset::volumeChangePrompt());
}
void
D578UVCodeplug::GeneralSettingsElement::enableVolumeChangePrompt(bool enable) {
  setUInt8(Offset::volumeChangePrompt(), (enable ? 0x01 : 0x01));
}

AnytoneAutoRepeaterSettingsExtension::Direction
D578UVCodeplug::GeneralSettingsElement::autoRepeaterDirectionA() const {
  return (AnytoneAutoRepeaterSettingsExtension::Direction) getUInt8(Offset::autoRepeaterDirA());
}
void
D578UVCodeplug::GeneralSettingsElement::setAutoRepeaterDirectionA(AnytoneAutoRepeaterSettingsExtension::Direction dir) {
  setUInt8(Offset::autoRepeaterDirA(), (unsigned)dir);
}

AnytoneDMRSettingsExtension::SlotMatch
D578UVCodeplug::GeneralSettingsElement::monitorSlotMatch() const {
  return (AnytoneDMRSettingsExtension::SlotMatch)getUInt8(Offset::monSlotMatch());
}
void
D578UVCodeplug::GeneralSettingsElement::setMonitorSlotMatch(AnytoneDMRSettingsExtension::SlotMatch match) {
  setUInt8(Offset::monSlotMatch(), (unsigned)match);
}

bool
D578UVCodeplug::GeneralSettingsElement::monitorColorCodeMatch() const {
  return getUInt8(Offset::monColorCodeMatch());
}
void
D578UVCodeplug::GeneralSettingsElement::enableMonitorColorCodeMatch(bool enable) {
  setUInt8(Offset::monColorCodeMatch(), (enable ? 0x01 : 0x00));
}

bool
D578UVCodeplug::GeneralSettingsElement::monitorIDMatch() const {
  return getUInt8(Offset::monIDMatch());
}
void
D578UVCodeplug::GeneralSettingsElement::enableMonitorIDMatch(bool enable) {
  setUInt8(Offset::monIDMatch(), (enable ? 0x01 : 0x00));
}

bool
D578UVCodeplug::GeneralSettingsElement::monitorTimeSlotHold() const {
  return getUInt8(Offset::monTimeSlotHold());
}
void
D578UVCodeplug::GeneralSettingsElement::enableMonitorTimeSlotHold(bool enable) {
  setUInt8(Offset::monTimeSlotHold(), (enable ? 0x01 : 0x00));
}

AnytoneDisplaySettingsExtension::LastCallerDisplayMode
D578UVCodeplug::GeneralSettingsElement::lastCallerDisplayMode() const {
  return (AnytoneDisplaySettingsExtension::LastCallerDisplayMode)getUInt8(Offset::lastCallerDisplay());
}
void
D578UVCodeplug::GeneralSettingsElement::setLastCallerDisplayMode(AnytoneDisplaySettingsExtension::LastCallerDisplayMode mode) {
  setUInt8(Offset::lastCallerDisplay(), (unsigned)mode);
}

unsigned
D578UVCodeplug::GeneralSettingsElement::fmCallHold() const {
  return getUInt8(Offset::fmCallHold());
}
void
D578UVCodeplug::GeneralSettingsElement::setFMCallHold(unsigned sec) {
  setUInt8(Offset::fmCallHold(), sec);
}

bool
D578UVCodeplug::GeneralSettingsElement::displayClock() const {
  return getUInt8(Offset::showClock());
}
void
D578UVCodeplug::GeneralSettingsElement::enableDisplayClock(bool enable) {
  setUInt8(Offset::showClock(), (enable ? 0x01 : 0x00));
}

bool
D578UVCodeplug::GeneralSettingsElement::gpsMessageEnabled() const {
  return getUInt8(Offset::enableGPSMessage());
}
void
D578UVCodeplug::GeneralSettingsElement::enableGPSMessage(bool enable) {
  setUInt8(Offset::enableGPSMessage(), (enable ? 0x01 : 0x00));
}

bool
D578UVCodeplug::GeneralSettingsElement::enhanceAudio() const {
  return getUInt8(Offset::enhanceAudio());
}
void
D578UVCodeplug::GeneralSettingsElement::enableEnhancedAudio(bool enable) {
  setUInt8(Offset::enhanceAudio(), (enable ? 0x01 : 0x00));
}

Frequency
D578UVCodeplug::GeneralSettingsElement::minVFOScanFrequencyUHF() const {
  return Frequency::fromHz(((unsigned)getUInt32_le(Offset::minVFOScanUHF()))*10);
}
void
D578UVCodeplug::GeneralSettingsElement::setMinVFOScanFrequencyUHF(Frequency freq) {
  setUInt32_le(Offset::minVFOScanUHF(), freq.inHz()/10);
}

Frequency
D578UVCodeplug::GeneralSettingsElement::maxVFOScanFrequencyUHF() const {
  return Frequency::fromHz(((unsigned)getUInt32_le(Offset::maxVFOScanUHF()))*10);
}
void
D578UVCodeplug::GeneralSettingsElement::setMaxVFOScanFrequencyUHF(Frequency freq) {
  setUInt32_le(Offset::maxVFOScanUHF(), freq.inHz()/10);
}

Frequency
D578UVCodeplug::GeneralSettingsElement::minVFOScanFrequencyVHF() const {
  return Frequency::fromHz(((unsigned)getUInt32_le(Offset::minVFOScanVHF()))*10);
}
void
D578UVCodeplug::GeneralSettingsElement::setMinVFOScanFrequencyVHF(Frequency freq) {
  setUInt32_le(Offset::minVFOScanVHF(), freq.inHz()/10);
}

Frequency
D578UVCodeplug::GeneralSettingsElement::maxVFOScanFrequencyVHF() const {
  return Frequency::fromHz(((unsigned)getUInt32_le(Offset::maxVFOScanVHF()))*10);
}
void
D578UVCodeplug::GeneralSettingsElement::setMaxVFOScanFrequencyVHF(Frequency freq) {
  setUInt32_le(Offset::maxVFOScanVHF(), freq.inHz()/10);
}

void
D578UVCodeplug::GeneralSettingsElement::callToneMelody(Melody &melody) const {
  QVector<QPair<double, unsigned int>> tones; tones.reserve(5);
  for (int i=0; i<5; i++) {
    double freq = getUInt16_le(Offset::callToneTones()+2*i);
    unsigned int duration = getUInt16_le(Offset::callToneDurations()+2*i);
    if (duration) tones.append({freq, duration});
  }
  melody.infer(tones);
}
void
D578UVCodeplug::GeneralSettingsElement::setCallToneMelody(const Melody &melody) {
  unsigned int n=std::min(5U, (unsigned int)melody.count());
  QVector<QPair<double, unsigned int>> tones = melody.toTones();
  for (unsigned int i=0; i<n; i++) {
    setUInt16_le(Offset::callToneTones()+2*i, tones.at(i).first);
    setUInt16_le(Offset::callToneDurations()+2*i, tones.at(i).second);
  }
}

void
D578UVCodeplug::GeneralSettingsElement::idleToneMelody(Melody &melody) const {
  QVector<QPair<double, unsigned int>> tones; tones.reserve(5);
  for (int i=0; i<5; i++) {
    double frequency = getUInt16_le(Offset::idleToneTones()+2*i);
    unsigned int duration  = getUInt16_le(Offset::idleToneDurations()+2*i);
    if (duration) tones.append({frequency, duration});
  }
  melody.infer(tones);
}
void
D578UVCodeplug::GeneralSettingsElement::setIdleToneMelody(const Melody &melody) {
  unsigned int n=std::min(5U, (unsigned int)melody.count());
  QVector<QPair<double, unsigned int>> tones = melody.toTones();
  for (unsigned int i=0; i<n; i++) {
    setUInt16_le(Offset::idleToneTones()+2*i, tones.at(i).first);
    setUInt16_le(Offset::idleToneDurations()+2*i, tones.at(i).second);
  }
}

void
D578UVCodeplug::GeneralSettingsElement::resetToneMelody(Melody &melody) const {
  QVector<QPair<double, unsigned int>> tones; tones.reserve(5);
  for (int i=0; i<5; i++) {
    double frequency = getUInt16_le(Offset::resetToneTones()+2*i);
    unsigned int duration  = getUInt16_le(Offset::resetToneDurations()+2*i);
    if (duration) tones.append({frequency, duration});
  }
  melody.infer(tones);
}
void
D578UVCodeplug::GeneralSettingsElement::setResetToneMelody(const Melody &melody) {
  unsigned int n=std::min(5U, (unsigned int)melody.count());
  QVector<QPair<double, unsigned int>> tones = melody.toTones();
  for (unsigned int i=0; i<n; i++) {
    setUInt16_le(Offset::resetToneTones()+2*i, tones.at(i).first);
    setUInt16_le(Offset::resetToneDurations()+2*i, tones.at(i).second);
  }
}

bool
D578UVCodeplug::GeneralSettingsElement::hasAutoRepeaterOffsetFrequencyIndexUHF() const {
  return 0xff != autoRepeaterOffsetFrequencyIndexUHF();
}
unsigned
D578UVCodeplug::GeneralSettingsElement::autoRepeaterOffsetFrequencyIndexUHF() const {
  return getUInt8(Offset::autoRepOffsetUHF());
}
void
D578UVCodeplug::GeneralSettingsElement::setAutoRepeaterOffsetFrequenyIndexUHF(unsigned idx) {
  setUInt8(Offset::autoRepOffsetUHF(), idx);
}
void
D578UVCodeplug::GeneralSettingsElement::clearAutoRepeaterOffsetFrequencyIndexUHF() {
  setAutoRepeaterOffsetFrequenyIndexUHF(0xff);
}

bool
D578UVCodeplug::GeneralSettingsElement::hasAutoRepeaterOffsetFrequencyIndexVHF() const {
  return 0xff != autoRepeaterOffsetFrequencyIndexVHF();
}
unsigned
D578UVCodeplug::GeneralSettingsElement::autoRepeaterOffsetFrequencyIndexVHF() const {
  return getUInt8(Offset::autoRepOffsetVHF());
}
void
D578UVCodeplug::GeneralSettingsElement::setAutoRepeaterOffsetFrequenyIndexVHF(unsigned idx) {
  setUInt8(Offset::autoRepOffsetVHF(), idx);
}
void
D578UVCodeplug::GeneralSettingsElement::clearAutoRepeaterOffsetFrequencyIndexVHF() {
  setAutoRepeaterOffsetFrequenyIndexVHF(0xff);
}

unsigned
D578UVCodeplug::GeneralSettingsElement::priorityZoneAIndex() const {
  return getUInt8(Offset::priorityZoneA());
}
void
D578UVCodeplug::GeneralSettingsElement::setPriorityZoneAIndex(unsigned idx) {
  setUInt8(Offset::priorityZoneA(), idx);
}
unsigned
D578UVCodeplug::GeneralSettingsElement::priorityZoneBIndex() const {
  return getUInt8(Offset::priorityZoneB());
}
void
D578UVCodeplug::GeneralSettingsElement::setPriorityZoneBIndex(unsigned idx) {
  setUInt8(Offset::priorityZoneB(), idx);
}

bool
D578UVCodeplug::GeneralSettingsElement::displayCall() const {
  return getUInt8(Offset::callDisplayMode());
}
void
D578UVCodeplug::GeneralSettingsElement::enableDisplayCall(bool enable) {
  setUInt8(Offset::callDisplayMode(), (enable ? 0x01 : 0x00));
}

bool
D578UVCodeplug::GeneralSettingsElement::bluetooth() const {
  return 0 != getUInt8(Offset::bluetooth());
}
void
D578UVCodeplug::GeneralSettingsElement::enableBluetooth(bool enable) {
  setUInt8(Offset::bluetooth(), enable ? 0x01 : 0x00);
}

bool
D578UVCodeplug::GeneralSettingsElement::btAndInternalMic() const {
  return 0 != getUInt8(Offset::btAndInternalMic());
}
void
D578UVCodeplug::GeneralSettingsElement::enableBTAndInternalMic(bool enable) {
  setUInt8(Offset::btAndInternalMic(), enable ? 0x01 : 0x00);
}

bool
D578UVCodeplug::GeneralSettingsElement::btAndInternalSpeaker() const {
  return 0 != getUInt8(Offset::btAndInternalSpeaker());
}
void
D578UVCodeplug::GeneralSettingsElement::enableBTAndInternalSpeaker(bool enable) {
  setUInt8(Offset::btAndInternalSpeaker(), enable ? 0x01 : 0x00);
}

bool
D578UVCodeplug::GeneralSettingsElement::pluginRecTone() const {
  return 0 != getUInt8(Offset::pluginRecTone());
}
void
D578UVCodeplug::GeneralSettingsElement::enablePluginRecTone(bool enable) {
  setUInt8(Offset::pluginRecTone(), enable ? 0x01 : 0x00);
}

Interval
D578UVCodeplug::GeneralSettingsElement::gpsRangingInterval() const {
  return Interval::fromSeconds(getUInt8(Offset::gpsRangingInterval()));
}
void
D578UVCodeplug::GeneralSettingsElement::setGPSRangingInterval(Interval intv) {
  setUInt8(Offset::gpsRangingInterval(), intv.seconds());
}

unsigned int
D578UVCodeplug::GeneralSettingsElement::btMicGain() const {
  return (getUInt8(Offset::btMicGain())+1)*2;
}
void
D578UVCodeplug::GeneralSettingsElement::setBTMicGain(unsigned int gain) {
  gain = std::min(10U, std::max(2U, gain));
  setUInt8(Offset::btMicGain(), gain/2-1);
}

unsigned int
D578UVCodeplug::GeneralSettingsElement::btSpeakerGain() const {
  return (getUInt8(Offset::btSpeakerGain())+1)*2;
}
void
D578UVCodeplug::GeneralSettingsElement::setBTSpeakerGain(unsigned int gain) {
  gain = std::min(10U, std::max(2U, gain));
  setUInt8(Offset::btSpeakerGain(), gain/2-1);
}

bool
D578UVCodeplug::GeneralSettingsElement::displayChannelNumber() const {
  return getUInt8(Offset::showChannelNumber());
}
void
D578UVCodeplug::GeneralSettingsElement::enableDisplayChannelNumber(bool enable) {
  setUInt8(Offset::showChannelNumber(), (enable ? 0x01 : 0x00));
}

bool
D578UVCodeplug::GeneralSettingsElement::showCurrentContact() const {
  return getUInt8(Offset::showCurrentContact());
}
void
D578UVCodeplug::GeneralSettingsElement::enableShowCurrentContact(bool enable) {
  setUInt8(Offset::showCurrentContact(), (enable ? 0x01 : 0x00));
}

Interval
D578UVCodeplug::GeneralSettingsElement::autoRoamPeriod() const {
  return Interval::fromMinutes(getUInt8(Offset::autoRoamPeriod()));
}
void
D578UVCodeplug::GeneralSettingsElement::setAutoRoamPeriod(Interval intv) {
  setUInt8(Offset::autoRoamPeriod(), intv.minutes());
}

AnytoneDisplaySettingsExtension::Color
D578UVCodeplug::GeneralSettingsElement::callDisplayColor() const {
  return (AnytoneDisplaySettingsExtension::Color)getUInt8(Offset::callColor());
}
void
D578UVCodeplug::GeneralSettingsElement::setCallDisplayColor(AnytoneDisplaySettingsExtension::Color color) {
  setUInt8(Offset::callColor(), (unsigned)color);
}

bool
D578UVCodeplug::GeneralSettingsElement::gpsUnitsImperial() const {
  return getUInt8(Offset::gpsUnits());
}
void
D578UVCodeplug::GeneralSettingsElement::enableGPSUnitsImperial(bool enable) {
  setUInt8(Offset::gpsUnits(), (enable ? 0x01 : 0x00));
}

bool
D578UVCodeplug::GeneralSettingsElement::knobLock() const {
  return getBit(Offset::knobLock(), 0);
}
void
D578UVCodeplug::GeneralSettingsElement::enableKnobLock(bool enable) {
  setBit(Offset::knobLock(), 0, enable);
}
bool
D578UVCodeplug::GeneralSettingsElement::keypadLock() const {
  return getBit(Offset::keypadLock(), 1);
}
void
D578UVCodeplug::GeneralSettingsElement::enableKeypadLock(bool enable) {
  setBit(Offset::keypadLock(), 1, enable);
}
bool
D578UVCodeplug::GeneralSettingsElement::sidekeysLock() const {
  return getBit(Offset::sideKeyLock(), 3);
}
void
D578UVCodeplug::GeneralSettingsElement::enableSidekeysLock(bool enable) {
  setBit(Offset::sideKeyLock(), 3, enable);
}
bool
D578UVCodeplug::GeneralSettingsElement::keyLockForced() const {
  return getBit(Offset::forceKeyLock(), 4);
}
void
D578UVCodeplug::GeneralSettingsElement::enableKeyLockForced(bool enable) {
  setBit(Offset::forceKeyLock(), 4, enable);
}

Interval
D578UVCodeplug::GeneralSettingsElement::autoRoamDelay() const {
  return Interval::fromSeconds(getUInt8(Offset::autoRoamDelay()));
}
void
D578UVCodeplug::GeneralSettingsElement::setAutoRoamDelay(Interval intv) {
  setUInt8(Offset::autoRoamDelay(), intv.seconds());
}

AnytoneDisplaySettingsExtension::Color
D578UVCodeplug::GeneralSettingsElement::standbyTextColor() const {
  return (AnytoneDisplaySettingsExtension::Color)getUInt8(Offset::standbyTextColor());
}
void
D578UVCodeplug::GeneralSettingsElement::setStandbyTextColor(AnytoneDisplaySettingsExtension::Color color) {
  setUInt8(Offset::standbyTextColor(), (unsigned)color);
}

AnytoneDisplaySettingsExtension::Color
D578UVCodeplug::GeneralSettingsElement::standbyBackgroundColor() const {
  return (AnytoneDisplaySettingsExtension::Color)getUInt8(Offset::standbyBackground());
}
void
D578UVCodeplug::GeneralSettingsElement::setStandbyBackgroundColor(AnytoneDisplaySettingsExtension::Color color) {
  setUInt8(Offset::standbyBackground(), (unsigned)color);
}

bool
D578UVCodeplug::GeneralSettingsElement::showLastHeard() const {
  return getUInt8(Offset::showLastHeard());
}
void
D578UVCodeplug::GeneralSettingsElement::enableShowLastHeard(bool enable) {
  setUInt8(Offset::showLastHeard(), (enable ? 0x01 : 0x00));
}

AnytoneDMRSettingsExtension::SMSFormat
D578UVCodeplug::GeneralSettingsElement::smsFormat() const {
  return (AnytoneDMRSettingsExtension::SMSFormat) getUInt8(Offset::smsFormat());
}
void
D578UVCodeplug::GeneralSettingsElement::setSMSFormat(AnytoneDMRSettingsExtension::SMSFormat fmt) {
  setUInt8(Offset::smsFormat(), (unsigned)fmt);
}

AnytoneAutoRepeaterSettingsExtension::Direction
D578UVCodeplug::GeneralSettingsElement::autoRepeaterDirectionB() const {
  return (AnytoneAutoRepeaterSettingsExtension::Direction)getUInt8(Offset::autoRepeaterDirB());
}
void
D578UVCodeplug::GeneralSettingsElement::setAutoRepeaterDirectionB(AnytoneAutoRepeaterSettingsExtension::Direction dir) {
  setUInt8(Offset::autoRepeaterDirB(), (unsigned)dir);
}

bool
D578UVCodeplug::GeneralSettingsElement::fmSendIDAndContact() const {
  return 0 != getUInt8(Offset::fmSendIDAndContact());
}
void
D578UVCodeplug::GeneralSettingsElement::enableFMSendIDAndContact(bool enable) {
  setUInt8(Offset::fmSendIDAndContact(), enable ? 0x01 : 0x00);
}

bool
D578UVCodeplug::GeneralSettingsElement::defaultChannel() const {
  return getUInt8(Offset::defaultChannels());
}
void
D578UVCodeplug::GeneralSettingsElement::enableDefaultChannel(bool enable) {
  setUInt8(Offset::defaultChannels(), (enable ? 0x01 : 0x00));
}

unsigned
D578UVCodeplug::GeneralSettingsElement::defaultZoneIndexA() const {
  return getUInt8(Offset::defaultZoneA());
}
void
D578UVCodeplug::GeneralSettingsElement::setDefaultZoneIndexA(unsigned idx) {
  setUInt8(Offset::defaultZoneA(), idx);
}

unsigned
D578UVCodeplug::GeneralSettingsElement::defaultZoneIndexB() const {
  return getUInt8(Offset::defaultZoneB());
}
void
D578UVCodeplug::GeneralSettingsElement::setDefaultZoneIndexB(unsigned idx) {
  setUInt8(Offset::defaultZoneB(), idx);
}

bool
D578UVCodeplug::GeneralSettingsElement::defaultChannelAIsVFO() const {
  return 0xff == defaultChannelAIndex();
}
unsigned
D578UVCodeplug::GeneralSettingsElement::defaultChannelAIndex() const {
  return getUInt8(Offset::defaultChannelA());
}
void
D578UVCodeplug::GeneralSettingsElement::setDefaultChannelAIndex(unsigned idx) {
  setUInt8(Offset::defaultChannelA(), idx);
}
void
D578UVCodeplug::GeneralSettingsElement::setDefaultChannelAToVFO() {
  setDefaultChannelAIndex(0xff);
}

bool
D578UVCodeplug::GeneralSettingsElement::defaultChannelBIsVFO() const {
  return 0xff == defaultChannelBIndex();
}
unsigned
D578UVCodeplug::GeneralSettingsElement::defaultChannelBIndex() const {
  return getUInt8(Offset::defaultChannelB());
}
void
D578UVCodeplug::GeneralSettingsElement::setDefaultChannelBIndex(unsigned idx) {
  setUInt8(Offset::defaultChannelB(), idx);
}
void
D578UVCodeplug::GeneralSettingsElement::setDefaultChannelBToVFO() {
  setDefaultChannelBIndex(0xff);
}

unsigned
D578UVCodeplug::GeneralSettingsElement::defaultRoamingZoneIndex() const {
  return getUInt8(Offset::defaultRoamingZone());
}
void
D578UVCodeplug::GeneralSettingsElement::setDefaultRoamingZoneIndex(unsigned idx) {
  setUInt8(Offset::defaultRoamingZone(), idx);
}

bool
D578UVCodeplug::GeneralSettingsElement::repeaterRangeCheck() const {
  return getUInt8(Offset::repRangeCheck());
}
void
D578UVCodeplug::GeneralSettingsElement::enableRepeaterRangeCheck(bool enable) {
  setUInt8(Offset::repRangeCheck(), (enable ? 0x01 : 0x00));
}

Interval
D578UVCodeplug::GeneralSettingsElement::repeaterRangeCheckInterval() const {
  return Interval::fromSeconds(((unsigned)getUInt8(Offset::rangeCheckInterval()))*5);
}
void
D578UVCodeplug::GeneralSettingsElement::setRepeaterRangeCheckInterval(Interval intv) {
  setUInt8(Offset::rangeCheckInterval(), intv.seconds()/5);
}

unsigned
D578UVCodeplug::GeneralSettingsElement::repeaterRangeCheckCount() const {
  return getUInt8(Offset::rangeCheckCount());
}
void
D578UVCodeplug::GeneralSettingsElement::setRepeaterRangeCheckCount(unsigned n) {
  setUInt8(Offset::rangeCheckCount(), n);
}

AnytoneRangingSettingsExtension::RoamStart
D578UVCodeplug::GeneralSettingsElement::roamingStartCondition() const {
  return (AnytoneRangingSettingsExtension::RoamStart)getUInt8(Offset::roamStartCondition());
}
void
D578UVCodeplug::GeneralSettingsElement::setRoamingStartCondition(AnytoneRangingSettingsExtension::RoamStart cond) {
  setUInt8(Offset::roamStartCondition(), (unsigned)cond);
}

bool
D578UVCodeplug::GeneralSettingsElement::separateDisplay() const {
  return getUInt8(Offset::displaySeparator());
}
void
D578UVCodeplug::GeneralSettingsElement::enableSeparateDisplay(bool enable) {
  setUInt8(Offset::displaySeparator(), (enable ? 0x01 : 0x00));
}

bool
D578UVCodeplug::GeneralSettingsElement::keepLastCaller() const {
  return getUInt8(Offset::keepLastCaller());
}
void
D578UVCodeplug::GeneralSettingsElement::enableKeepLastCaller(bool enable) {
  setUInt8(Offset::keepLastCaller(), (enable ? 0x01 : 0x00));
}

AnytoneDisplaySettingsExtension::Color
D578UVCodeplug::GeneralSettingsElement::channelNameColor() const {
  return (AnytoneDisplaySettingsExtension::Color) getUInt8(Offset::channelNameColor());
}
void
D578UVCodeplug::GeneralSettingsElement::setChannelNameColor(AnytoneDisplaySettingsExtension::Color color) {
  setUInt8(Offset::channelNameColor(), (unsigned)color);
}

bool
D578UVCodeplug::GeneralSettingsElement::repeaterCheckNotification() const {
  return getUInt8(Offset::repCheckNotify());
}
void
D578UVCodeplug::GeneralSettingsElement::enableRepeaterCheckNotification(bool enable) {
  setUInt8(Offset::repCheckNotify(), (enable ? 0x01 : 0x00));
}

bool
D578UVCodeplug::GeneralSettingsElement::roaming() const {
  return getUInt8(Offset::roaming());
}
void
D578UVCodeplug::GeneralSettingsElement::enableRoaming(bool enable) {
  setUInt8(Offset::roaming(), (enable ? 0x01 : 0x00));
}

AnytoneKeySettingsExtension::KeyFunction
D578UVCodeplug::GeneralSettingsElement::progFuncKey1Short() const {
  return this->mapCodeToKeyFunction(getUInt8(Offset::progFuncKey1Short()));
}
void
D578UVCodeplug::GeneralSettingsElement::setProgFuncKey1Short(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKey1Short(), this->mapKeyFunctionToCode(func));
}

AnytoneKeySettingsExtension::KeyFunction
D578UVCodeplug::GeneralSettingsElement::progFuncKey2Short() const {
  return this->mapCodeToKeyFunction(getUInt8(Offset::progFuncKey2Short()));
}
void
D578UVCodeplug::GeneralSettingsElement::setProgFuncKey2Short(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKey2Short(), this->mapKeyFunctionToCode(func));
}

AnytoneKeySettingsExtension::KeyFunction
D578UVCodeplug::GeneralSettingsElement::progFuncKey3Short() const {
  return this->mapCodeToKeyFunction(getUInt8(Offset::progFuncKey3Short()));
}
void
D578UVCodeplug::GeneralSettingsElement::setProgFuncKey3Short(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKey3Short(), this->mapKeyFunctionToCode(func));
}

AnytoneKeySettingsExtension::KeyFunction
D578UVCodeplug::GeneralSettingsElement::progFuncKey4Short() const {
  return this->mapCodeToKeyFunction(getUInt8(Offset::progFuncKey4Short()));
}
void
D578UVCodeplug::GeneralSettingsElement::setProgFuncKey4Short(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKey4Short(), this->mapKeyFunctionToCode(func));
}

AnytoneKeySettingsExtension::KeyFunction
D578UVCodeplug::GeneralSettingsElement::progFuncKey5Short() const {
  return this->mapCodeToKeyFunction(getUInt8(Offset::progFuncKey5Short()));
}
void
D578UVCodeplug::GeneralSettingsElement::setProgFuncKey5Short(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKey5Short(), this->mapKeyFunctionToCode(func));
}

AnytoneKeySettingsExtension::KeyFunction
D578UVCodeplug::GeneralSettingsElement::progFuncKey6Short() const {
  return this->mapCodeToKeyFunction(getUInt8(Offset::progFuncKey6Short()));
}
void
D578UVCodeplug::GeneralSettingsElement::setProgFuncKey6Short(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKey6Short(), this->mapKeyFunctionToCode(func));
}

AnytoneKeySettingsExtension::KeyFunction
D578UVCodeplug::GeneralSettingsElement::progFuncKeyAShort() const {
  return this->mapCodeToKeyFunction(getUInt8(Offset::progFuncKeyAShort()));
}
void
D578UVCodeplug::GeneralSettingsElement::setProgFuncKeyAShort(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKeyAShort(), this->mapKeyFunctionToCode(func));
}

AnytoneKeySettingsExtension::KeyFunction
D578UVCodeplug::GeneralSettingsElement::progFuncKeyBShort() const {
  return this->mapCodeToKeyFunction(getUInt8(Offset::progFuncKeyBShort()));
}
void
D578UVCodeplug::GeneralSettingsElement::setProgFuncKeyBShort(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKeyBShort(), this->mapKeyFunctionToCode(func));
}

AnytoneKeySettingsExtension::KeyFunction
D578UVCodeplug::GeneralSettingsElement::progFuncKeyCShort() const {
  return this->mapCodeToKeyFunction(getUInt8(Offset::progFuncKeyCShort()));
}
void
D578UVCodeplug::GeneralSettingsElement::setProgFuncKeyCShort(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKeyCShort(), this->mapKeyFunctionToCode(func));
}

AnytoneKeySettingsExtension::KeyFunction
D578UVCodeplug::GeneralSettingsElement::progFuncKeyDShort() const {
  return this->mapCodeToKeyFunction(getUInt8(Offset::progFuncKeyDShort()));
}
void
D578UVCodeplug::GeneralSettingsElement::setProgFuncKeyDShort(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKeyDShort(), this->mapKeyFunctionToCode(func));
}

AnytoneKeySettingsExtension::KeyFunction
D578UVCodeplug::GeneralSettingsElement::progFuncKey1Long() const {
  return (AnytoneKeySettingsExtension::KeyFunction)getUInt8(Offset::progFuncKey1Long());
}
void
D578UVCodeplug::GeneralSettingsElement::setProgFuncKey1Long(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKey1Long(), (unsigned)func);
}

AnytoneKeySettingsExtension::KeyFunction
D578UVCodeplug::GeneralSettingsElement::progFuncKey2Long() const {
  return (AnytoneKeySettingsExtension::KeyFunction)getUInt8(Offset::progFuncKey2Long());
}
void
D578UVCodeplug::GeneralSettingsElement::setProgFuncKey2Long(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKey2Long(), (unsigned)func);
}

AnytoneKeySettingsExtension::KeyFunction
D578UVCodeplug::GeneralSettingsElement::progFuncKey3Long() const {
  return (AnytoneKeySettingsExtension::KeyFunction)getUInt8(Offset::progFuncKey3Long());
}
void
D578UVCodeplug::GeneralSettingsElement::setProgFuncKey3Long(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKey3Long(), (unsigned)func);
}

AnytoneKeySettingsExtension::KeyFunction
D578UVCodeplug::GeneralSettingsElement::progFuncKey4Long() const {
  return (AnytoneKeySettingsExtension::KeyFunction)getUInt8(Offset::progFuncKey4Long());
}
void
D578UVCodeplug::GeneralSettingsElement::setProgFuncKey4Long(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKey4Long(), (unsigned)func);
}

AnytoneKeySettingsExtension::KeyFunction
D578UVCodeplug::GeneralSettingsElement::progFuncKey5Long() const {
  return (AnytoneKeySettingsExtension::KeyFunction)getUInt8(Offset::progFuncKey5Long());
}
void
D578UVCodeplug::GeneralSettingsElement::setProgFuncKey5Long(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKey5Long(), (unsigned)func);
}

AnytoneKeySettingsExtension::KeyFunction
D578UVCodeplug::GeneralSettingsElement::progFuncKey6Long() const {
  return (AnytoneKeySettingsExtension::KeyFunction)getUInt8(Offset::progFuncKey6Long());
}
void
D578UVCodeplug::GeneralSettingsElement::setProgFuncKey6Long(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKey6Long(), (unsigned)func);
}

AnytoneKeySettingsExtension::KeyFunction
D578UVCodeplug::GeneralSettingsElement::progFuncKeyALong() const {
  return (AnytoneKeySettingsExtension::KeyFunction)getUInt8(Offset::progFuncKeyALong());
}
void
D578UVCodeplug::GeneralSettingsElement::setProgFuncKeyALong(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKeyALong(), (unsigned)func);
}

AnytoneKeySettingsExtension::KeyFunction
D578UVCodeplug::GeneralSettingsElement::progFuncKeyBLong() const {
  return (AnytoneKeySettingsExtension::KeyFunction)getUInt8(Offset::progFuncKeyBLong());
}
void
D578UVCodeplug::GeneralSettingsElement::setProgFuncKeyBLong(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKeyBLong(), (unsigned)func);
}

AnytoneKeySettingsExtension::KeyFunction
D578UVCodeplug::GeneralSettingsElement::progFuncKeyCLong() const {
  return (AnytoneKeySettingsExtension::KeyFunction)getUInt8(Offset::progFuncKeyCLong());
}
void
D578UVCodeplug::GeneralSettingsElement::setProgFuncKeyCLong(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKeyCLong(), (unsigned)func);
}

AnytoneKeySettingsExtension::KeyFunction
D578UVCodeplug::GeneralSettingsElement::progFuncKeyDLong() const {
  return (AnytoneKeySettingsExtension::KeyFunction)getUInt8(Offset::progFuncKeyDLong());
}
void
D578UVCodeplug::GeneralSettingsElement::setProgFuncKeyDLong(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKeyDLong(), (unsigned)func);
}

uint8_t
D578UVCodeplug::GeneralSettingsElement::mapKeyFunctionToCode(AnytoneKeySettingsExtension::KeyFunction func) const {
  switch (func) {
  case AnytoneKeySettingsExtension::KeyFunction::Off:               return (uint8_t)KeyFunction::Off;
  case AnytoneKeySettingsExtension::KeyFunction::Voltage:           return (uint8_t)KeyFunction::Voltage;
  case AnytoneKeySettingsExtension::KeyFunction::Power:             return (uint8_t)KeyFunction::Power;
  case AnytoneKeySettingsExtension::KeyFunction::Repeater:          return (uint8_t)KeyFunction::Repeater;
  case AnytoneKeySettingsExtension::KeyFunction::Reverse:           return (uint8_t)KeyFunction::Reverse;
  case AnytoneKeySettingsExtension::KeyFunction::Encryption:        return (uint8_t)KeyFunction::Encryption;
  case AnytoneKeySettingsExtension::KeyFunction::Call:              return (uint8_t)KeyFunction::Call;
  case AnytoneKeySettingsExtension::KeyFunction::ToggleVFO:         return (uint8_t)KeyFunction::ToggleVFO;
  case AnytoneKeySettingsExtension::KeyFunction::Scan:              return (uint8_t)KeyFunction::Scan;
  case AnytoneKeySettingsExtension::KeyFunction::WFM:               return (uint8_t)KeyFunction::WFM;
  case AnytoneKeySettingsExtension::KeyFunction::Alarm:             return (uint8_t)KeyFunction::Alarm;
  case AnytoneKeySettingsExtension::KeyFunction::RecordSwitch:      return (uint8_t)KeyFunction::RecordSwitch;
  case AnytoneKeySettingsExtension::KeyFunction::Record:            return (uint8_t)KeyFunction::Record;
  case AnytoneKeySettingsExtension::KeyFunction::SMS:               return (uint8_t)KeyFunction::SMS;
  case AnytoneKeySettingsExtension::KeyFunction::Dial:              return (uint8_t)KeyFunction::Dial;
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
  case AnytoneKeySettingsExtension::KeyFunction::Roaming:           return (uint8_t)KeyFunction::Roaming;
  case AnytoneKeySettingsExtension::KeyFunction::MaxVolume:         return (uint8_t)KeyFunction::MaxVolume;
  case AnytoneKeySettingsExtension::KeyFunction::Slot:              return (uint8_t)KeyFunction::Slot;
  case AnytoneKeySettingsExtension::KeyFunction::Zone:              return (uint8_t)KeyFunction::Zone;
  case AnytoneKeySettingsExtension::KeyFunction::MuteA:             return (uint8_t)KeyFunction::MuteA;
  case AnytoneKeySettingsExtension::KeyFunction::MuteB:             return (uint8_t)KeyFunction::MuteB;
  case AnytoneKeySettingsExtension::KeyFunction::RoamingSet:        return (uint8_t)KeyFunction::RoamingSet;
  case AnytoneKeySettingsExtension::KeyFunction::APRSSet:           return (uint8_t)KeyFunction::APRSSet;
  case AnytoneKeySettingsExtension::KeyFunction::ZoneUp:            return (uint8_t)KeyFunction::ZoneUp;
  case AnytoneKeySettingsExtension::KeyFunction::ZoneDown:          return (uint8_t)KeyFunction::ZoneDown;
  case AnytoneKeySettingsExtension::KeyFunction::XBandRepeater:     return (uint8_t)KeyFunction::XBandRepeater;
  case AnytoneKeySettingsExtension::KeyFunction::Speaker:           return (uint8_t)KeyFunction::Speaker;
  case AnytoneKeySettingsExtension::KeyFunction::ChannelName:       return (uint8_t)KeyFunction::ChannelName;
  case AnytoneKeySettingsExtension::KeyFunction::Bluetooth:         return (uint8_t)KeyFunction::Bluetooth;
  case AnytoneKeySettingsExtension::KeyFunction::GPS:               return (uint8_t)KeyFunction::GPS;
  case AnytoneKeySettingsExtension::KeyFunction::CDTScan:           return (uint8_t)KeyFunction::CDTScan;
  case AnytoneKeySettingsExtension::KeyFunction::TBSTSend:          return (uint8_t)KeyFunction::TBSTSend;
  case AnytoneKeySettingsExtension::KeyFunction::APRSSend:          return (uint8_t)KeyFunction::APRSSend;
  case AnytoneKeySettingsExtension::KeyFunction::APRSInfo:          return (uint8_t)KeyFunction::APRSInfo;
  case AnytoneKeySettingsExtension::KeyFunction::GPSRoaming:        return (uint8_t)KeyFunction::GPSRoaming;
  default:                                                          return (uint8_t)KeyFunction::Off;
  }
}

AnytoneKeySettingsExtension::KeyFunction
D578UVCodeplug::GeneralSettingsElement::mapCodeToKeyFunction(uint8_t code) const {
  switch ((KeyFunction)code) {
  case KeyFunction::Off:               return AnytoneKeySettingsExtension::KeyFunction::Off;
  case KeyFunction::Voltage:           return AnytoneKeySettingsExtension::KeyFunction::Voltage;
  case KeyFunction::Power:             return AnytoneKeySettingsExtension::KeyFunction::Power;
  case KeyFunction::Repeater:          return AnytoneKeySettingsExtension::KeyFunction::Repeater;
  case KeyFunction::Reverse:           return AnytoneKeySettingsExtension::KeyFunction::Reverse;
  case KeyFunction::Encryption:        return AnytoneKeySettingsExtension::KeyFunction::Encryption;
  case KeyFunction::Call:              return AnytoneKeySettingsExtension::KeyFunction::Call;
  case KeyFunction::ToggleVFO:         return AnytoneKeySettingsExtension::KeyFunction::ToggleVFO;
  case KeyFunction::Scan:              return AnytoneKeySettingsExtension::KeyFunction::Scan;
  case KeyFunction::WFM:               return AnytoneKeySettingsExtension::KeyFunction::WFM;
  case KeyFunction::Alarm:             return AnytoneKeySettingsExtension::KeyFunction::Alarm;
  case KeyFunction::RecordSwitch:      return AnytoneKeySettingsExtension::KeyFunction::RecordSwitch;
  case KeyFunction::Record:            return AnytoneKeySettingsExtension::KeyFunction::Record;
  case KeyFunction::SMS:               return AnytoneKeySettingsExtension::KeyFunction::SMS;
  case KeyFunction::Dial:              return AnytoneKeySettingsExtension::KeyFunction::Dial;
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
  case KeyFunction::Roaming:           return AnytoneKeySettingsExtension::KeyFunction::Roaming;
  case KeyFunction::MaxVolume:         return AnytoneKeySettingsExtension::KeyFunction::MaxVolume;
  case KeyFunction::Slot:              return AnytoneKeySettingsExtension::KeyFunction::Slot;
  case KeyFunction::Zone:              return AnytoneKeySettingsExtension::KeyFunction::Zone;
  case KeyFunction::MuteA:             return AnytoneKeySettingsExtension::KeyFunction::MuteA;
  case KeyFunction::MuteB:             return AnytoneKeySettingsExtension::KeyFunction::MuteB;
  case KeyFunction::RoamingSet:        return AnytoneKeySettingsExtension::KeyFunction::RoamingSet;
  case KeyFunction::APRSSet:           return AnytoneKeySettingsExtension::KeyFunction::APRSSet;
  case KeyFunction::ZoneUp:            return AnytoneKeySettingsExtension::KeyFunction::ZoneUp;
  case KeyFunction::ZoneDown:          return AnytoneKeySettingsExtension::KeyFunction::ZoneDown;
  case KeyFunction::XBandRepeater:     return AnytoneKeySettingsExtension::KeyFunction::XBandRepeater;
  case KeyFunction::Speaker:           return AnytoneKeySettingsExtension::KeyFunction::Speaker;
  case KeyFunction::ChannelName:       return AnytoneKeySettingsExtension::KeyFunction::ChannelName;
  case KeyFunction::Bluetooth:         return AnytoneKeySettingsExtension::KeyFunction::Bluetooth;
  case KeyFunction::GPS:               return AnytoneKeySettingsExtension::KeyFunction::GPS;
  case KeyFunction::CDTScan:           return AnytoneKeySettingsExtension::KeyFunction::CDTScan;
  case KeyFunction::TBSTSend:          return AnytoneKeySettingsExtension::KeyFunction::TBSTSend;
  case KeyFunction::APRSSend:          return AnytoneKeySettingsExtension::KeyFunction::APRSSend;
  case KeyFunction::APRSInfo:          return AnytoneKeySettingsExtension::KeyFunction::APRSInfo;
  case KeyFunction::GPSRoaming:        return AnytoneKeySettingsExtension::KeyFunction::GPSRoaming;
  default:                             return AnytoneKeySettingsExtension::KeyFunction::Off;
  }
}

unsigned int
D578UVCodeplug::GeneralSettingsElement::repeaterCheckNumNotifications() const {
  return getUInt8(Offset::repCheckNumNotify())+1;
}
void
D578UVCodeplug::GeneralSettingsElement::setRepeaterCheckNumNotifications(unsigned int n) {
  n = std::max(1U, std::min(10U, n));
  setUInt8(Offset::repCheckNumNotify(), n);
}

Interval
D578UVCodeplug::GeneralSettingsElement::transmitTimeoutRekey() const {
  return Interval::fromSeconds(getUInt8(Offset::totRekey()));
}
void
D578UVCodeplug::GeneralSettingsElement::setTransmitTimeoutRekey(Interval dt) {
  setUInt8(Offset::totRekey(), dt.seconds());
}

bool
D578UVCodeplug::GeneralSettingsElement::btHoldTimeEnabled() const {
  return 0x00 != getUInt8(Offset::btHoldTime());
}
bool
D578UVCodeplug::GeneralSettingsElement::btHoldTimeInfinite() const {
  return 121U == getUInt8(Offset::btHoldTime());
}
Interval
D578UVCodeplug::GeneralSettingsElement::btHoldTime() const {
  return Interval::fromSeconds(getUInt8(Offset::btHoldTime()));
}
void
D578UVCodeplug::GeneralSettingsElement::setBTHoldTime(Interval interval) {
  unsigned int seconds = std::min(120ULL, std::max(1ULL, interval.seconds()));
  setUInt8(Offset::btHoldTime(), seconds);
}
void
D578UVCodeplug::GeneralSettingsElement::setBTHoldTimeInfinite() {
  setUInt8(Offset::btHoldTime(), 121);
}
void
D578UVCodeplug::GeneralSettingsElement::disableBTHoldTime() {
  setUInt8(Offset::btHoldTime(), 0);
}

Interval
D578UVCodeplug::GeneralSettingsElement::btRXDelay() const {
  if (0 == getUInt8(Offset::btRXDelay()))
    return Interval::fromMilliseconds(30);
  return Interval::fromMilliseconds(((unsigned int)getUInt8(Offset::btRXDelay())+1)*500);
}
void
D578UVCodeplug::GeneralSettingsElement::setBTRXDelay(Interval delay) {
  if (500 >= delay.milliseconds()) {
    setUInt8(Offset::btRXDelay(), 0);
  } else {
    unsigned int millis = std::min(5500ULL, std::max(500ULL, delay.milliseconds()));
    setUInt8(Offset::btRXDelay(), (millis-500)/500);
  }
}


/* ******************************************************************************************** *
 * Implementation of D578UVCodeplug::HotKeySettingsElement
 * ******************************************************************************************** */
D578UVCodeplug::HotKeySettingsElement::HotKeySettingsElement(uint8_t *ptr, size_t size)
  : AnytoneCodeplug::HotKeySettingsElement(ptr, size)
{
  // pass...
}

D578UVCodeplug::HotKeySettingsElement::HotKeySettingsElement(uint8_t *ptr)
  : AnytoneCodeplug::HotKeySettingsElement(ptr, HotKeySettingsElement::size())
{
  // pass...
}

uint8_t *
D578UVCodeplug::HotKeySettingsElement::hotKeySetting(unsigned int n) const {
  if (n >= Limit::numEntries())
    return nullptr;
  return _data + Offset::hotKeySettings() + n*Offset::betweenHotKeySettings();
}


/* ******************************************************************************************** *
 * Implementation of D578UVCodeplug::AirBandChannelElement
 * ******************************************************************************************** */
D578UVCodeplug::AirBandChannelElement::AirBandChannelElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

D578UVCodeplug::AirBandChannelElement::AirBandChannelElement(uint8_t *ptr)
  : Element(ptr, AirBandChannelElement::size())
{
  // pass...
}

void
D578UVCodeplug::AirBandChannelElement::clear() {
  memset(_data, 0, _size);
}

Frequency
D578UVCodeplug::AirBandChannelElement::frequency() const {
  return Frequency::fromHz(((unsigned long long)getBCD8_be(Offset::frequency()))*10);
}
void
D578UVCodeplug::AirBandChannelElement::setFrequency(Frequency freq) {
  setBCD8_be(Offset::frequency(), freq.inHz()/10);
}

QString
D578UVCodeplug::AirBandChannelElement::name() const {
  return readASCII(Offset::name(), Limit::nameLength(), 0x00);
}
void
D578UVCodeplug::AirBandChannelElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::nameLength(), 0x00);
}


/* ******************************************************************************************** *
 * Implementation of D578UVCodeplug::AirBandBitmapElement
 * ******************************************************************************************** */
D578UVCodeplug::AirBandBitmapElement::AirBandBitmapElement(uint8_t *ptr, size_t size)
  : BitmapElement(ptr, size)
{
  // pass...
}

D578UVCodeplug::AirBandBitmapElement::AirBandBitmapElement(uint8_t *ptr)
  : BitmapElement(ptr, AirBandBitmapElement::size())
{
  // pass...
}


/* ******************************************************************************************** *
 * Implementation of D578UVCodeplug
 * ******************************************************************************************** */
D578UVCodeplug::D578UVCodeplug(const QString &label, QObject *parent)
  : D878UVCodeplug(label, parent)
{
  // pass...
}

D578UVCodeplug::D578UVCodeplug(QObject *parent)
  : D878UVCodeplug("AnyTone AT-D578UV Codeplug", parent)
{
  // pass...
}

bool D578UVCodeplug::allocateBitmaps() {
  if (! D878UVCodeplug::allocateBitmaps())
    return false;

  image(0).addElement(Offset::airBandChannelBitmap(), AirBandBitmapElement::size());
  image(0).addElement(Offset::airBandScanBitmap(), AirBandBitmapElement::size());

  return true;
}

void
D578UVCodeplug::allocateUpdated() {
  D878UVCodeplug::allocateUpdated();

  this->allocateAirBand();
}

void
D578UVCodeplug::allocateHotKeySettings() {
  image(0).addElement(Offset::hotKeySettings(), HotKeySettingsElement::size());
}

bool
D578UVCodeplug::encodeChannels(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)

  // Encode channels
  for (int i=0; i<ctx.config()->channelList()->count(); i++) {
    // enable channel
    uint16_t bank = i/Limit::channelsPerBank(), idx = i%Limit::channelsPerBank();
    ChannelElement ch(data(Offset::channelBanks() + bank*Offset::betweenChannelBanks()
                           + idx*ChannelElement::size()));
    ch.fromChannelObj(ctx.config()->channelList()->channel(i), ctx);
  }
  return true;
}

bool
D578UVCodeplug::createChannels(Context &ctx, const ErrorStack &err) {
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
D578UVCodeplug::linkChannels(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  ChannelBitmapElement channel_bitmap(data(Offset::channelBitmap()));

  // Link channel objects
  for (uint16_t i=0; i<Limit::numChannels(); i++) {
    // Check if channel is enabled:
    uint16_t bank = i/Limit::channelsPerBank(), idx = i%Limit::channelsPerBank();
    if (! channel_bitmap.isEncoded(i))
      continue;
    ChannelElement ch(data(Offset::channelBanks() + bank*Offset::betweenChannelBanks()
                           + idx*ChannelElement::size()));
    if (ctx.has<Channel>(i))
      ch.linkChannelObj(ctx.get<Channel>(i), ctx);
  }

  return true;
}

void
D578UVCodeplug::allocateContacts() {
  /* Allocate contacts */
  ContactBitmapElement contact_bitmap(data(Offset::contactBitmap()));
  unsigned contactCount=0;
  for (uint16_t i=0; i<Limit::numContacts(); i++) {
    if (! contact_bitmap.isEncoded(i))
      continue;
    contactCount++;
    uint32_t bank_addr = Offset::contactBanks() + (i/Limit::contactsPerBank())*Offset::betweenContactBanks();
    uint32_t addr = bank_addr + (i%Limit::contactsPerBank())*Offset::betweenContactBlocks();
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
D578UVCodeplug::encodeContacts(const Flags &flags, Context &ctx, const ErrorStack &err) {
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

void
D578UVCodeplug::allocateGeneralSettings() {
  // override allocation of general settings for D878UV code-plug. General settings are larger!
  image(0).addElement(Offset::settings(), GeneralSettingsElement::size());
  image(0).addElement(Offset::gpsMessages(), DMRAPRSMessageElement::size());
  image(0).addElement(Offset::settingsExtension(), GeneralSettingsExtensionElement::size());

}
bool
D578UVCodeplug::encodeGeneralSettings(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  GeneralSettingsElement(data(Offset::settings())).fromConfig(flags, ctx);
  DMRAPRSMessageElement(data(Offset::gpsMessages())).fromConfig(flags, ctx);
  GeneralSettingsExtensionElement(data(Offset::settingsExtension())).fromConfig(flags, ctx);
  return true;
}
bool
D578UVCodeplug::decodeGeneralSettings(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  GeneralSettingsElement(data(Offset::settings())).updateConfig(ctx);
  DMRAPRSMessageElement(data(Offset::gpsMessages())).updateConfig(ctx);
  GeneralSettingsExtensionElement(data(Offset::settingsExtension())).updateConfig(ctx);
  return true;
}

void
D578UVCodeplug::allocateAirBand() {
  // Allocate valid air-band channels
  AirBandBitmapElement bitmap(data(Offset::airBandChannelBitmap()));
  for (unsigned int i=0; i<Limit::airBandChannels(); i++)  {
    if (! bitmap.isEncoded(i))
      return;
    image(0).addElement(Offset::airBandChannels() + i*AirBandChannelElement::size(),
                        AirBandChannelElement::size());
  }
  // allocate air-band VFO channel
  image(0).addElement(Offset::airBandVFO(), AirBandChannelElement::size());
}


