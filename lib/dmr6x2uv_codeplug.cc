#include "dmr6x2uv_codeplug.hh"
#include "utils.hh"


#define NUM_CHANNELS              4000
#define NUM_CHANNEL_BANKS         32
#define CHANNEL_BANK_0            0x00800000
#define CHANNEL_BANK_SIZE         0x00002000
#define CHANNEL_BANK_31           0x00fc0000
#define CHANNEL_BANK_31_SIZE      0x00000800
#define CHANNEL_BANK_OFFSET       0x00040000
#define CHANNEL_SIZE              0x00000040
#define CHANNEL_BITMAP            0x024c1500
#define CHANNEL_BITMAP_SIZE       0x00000200

#define ADDR_APRS_SETTINGS        0x02501000
#define APRS_SETTINGS_SIZE        0x00000040
#define NUM_DMRAPRS_SYSTEMS                8
#define ADDR_DMRAPRS_SETTINGS     0x02501040
#define DMRAPRS_SETTINGS_SIZE     0x00000060

#define ADDR_APRS_MESSAGE         0x02501200 // Address of APRS messages
#define APRS_MESSAGE_SIZE         0x00000040 // Size of APRS messages

#define NUM_ROAMING_CHANNEL         250
#define ADDR_ROAMING_CHANNEL_BITMAP 0x01042000
#define ROAMING_CHANNEL_BITMAP_SIZE 0x00000020
#define ADDR_ROAMING_CHANNEL_0      0x01040000
#define ROAMING_CHANNEL_SIZE        0x00000020
#define ROAMING_CHANNEL_OFFSET      0x00000020

#define NUM_ROAMING_ZONES           64
#define NUM_CH_PER_ROAMINGZONE      64
#define ADDR_ROAMING_ZONE_BITMAP    0x01042080
#define ROAMING_ZONE_BITMAP_SIZE    0x00000010
#define ADDR_ROAMING_ZONE_0         0x01043000
#define ROAMING_ZONE_SIZE           0x00000080
#define ROAMING_ZONE_OFFSET         0x00000080


/* ********************************************************************************************* *
 * Implementation of DMR6X2UVCodeplug::GeneralSettingsElement
 * ********************************************************************************************* */
DMR6X2UVCodeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr, unsigned size)
  : D878UVCodeplug::GeneralSettingsElement(ptr, size)
{
  // pass...
}

DMR6X2UVCodeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr)
  : D878UVCodeplug::GeneralSettingsElement(ptr, GeneralSettingsElement::size())
{
  // pass...
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::idleChannelTone() const {
  return getUInt8(Offset::idleChannelTone());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableIdleChannelTone(bool enable) {
  return setUInt8(Offset::idleChannelTone(), (enable ? 0x01 : 0x00));
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::vfoModeA() const {
  return getUInt8(Offset::vfoModeA());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableVFOModeA(bool enable) {
  setUInt8(Offset::vfoModeA(), (enable ? 0x01 : 0x00));
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::vfoModeB() const {
  return getUInt8(Offset::vfoModeB());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableVFOModeB(bool enable) {
  setUInt8(Offset::vfoModeB(), (enable ? 0x01 : 0x00));
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::keyToneEnabled() const {
  return 0x00 != getUInt8(Offset::enableKeyTone());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableKeyTone(bool enable) {
  setUInt8(Offset::enableKeyTone(), enable ? 0x01 : 0x00);
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::selectTXContactEnabled() const {
  return 0x01 == getUInt8(Offset::selectTXContact());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableSelectTXContact(bool enable) {
  setUInt8(Offset::selectTXContact(), enable ? 0x01 : 0x00);
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::smsConfirmEnabled() const {
  return 0x01 == getUInt8(Offset::smsConfirm());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableSMSConfirm(bool enable) {
  setUInt8(Offset::smsConfirm(), enable ? 0x01 : 0x00);
}

AnytoneDisplaySettingsExtension::Color
DMR6X2UVCodeplug::GeneralSettingsElement::callDisplayColor() const {
  DisplayColor color = (DisplayColor)getUInt8(Offset::callColor());
  switch (color) {
  case DisplayColor::Orange: return AnytoneDisplaySettingsExtension::Color::Orange;
  case DisplayColor::Red: return AnytoneDisplaySettingsExtension::Color::Red;
  case DisplayColor::Yellow: return AnytoneDisplaySettingsExtension::Color::Yellow;
  case DisplayColor::Green: return AnytoneDisplaySettingsExtension::Color::Green;
  case DisplayColor::Turquoise: return AnytoneDisplaySettingsExtension::Color::Turquoise;
  case DisplayColor::Blue: return AnytoneDisplaySettingsExtension::Color::Blue;
  case DisplayColor::White: return AnytoneDisplaySettingsExtension::Color::White;
  case DisplayColor::Black: return AnytoneDisplaySettingsExtension::Color::Black;
  }

  return AnytoneDisplaySettingsExtension::Color::Orange;
}

void
DMR6X2UVCodeplug::GeneralSettingsElement::setCallDisplayColor(AnytoneDisplaySettingsExtension::Color color) {
  switch (color) {
  case AnytoneDisplaySettingsExtension::Color::Orange:
    setUInt8(Offset::callColor(), (unsigned int)DisplayColor::Orange);
    break;
  case AnytoneDisplaySettingsExtension::Color::Red:
    setUInt8(Offset::callColor(), (unsigned int)DisplayColor::Red);
    break;
  case AnytoneDisplaySettingsExtension::Color::Yellow:
    setUInt8(Offset::callColor(), (unsigned int)DisplayColor::Yellow);
    break;
  case AnytoneDisplaySettingsExtension::Color::Green:
    setUInt8(Offset::callColor(), (unsigned int)DisplayColor::Green);
    break;
  case AnytoneDisplaySettingsExtension::Color::Turquoise:
    setUInt8(Offset::callColor(), (unsigned int)DisplayColor::Turquoise);
    break;
  case AnytoneDisplaySettingsExtension::Color::Blue:
    setUInt8(Offset::callColor(), (unsigned int)DisplayColor::Blue);
    break;
  case AnytoneDisplaySettingsExtension::Color::White:
    setUInt8(Offset::callColor(), (unsigned int)DisplayColor::White);
    break;
  case AnytoneDisplaySettingsExtension::Color::Black:
    setUInt8(Offset::callColor(), (unsigned int)DisplayColor::Black);
    break;
  }
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::simplexRepeaterEnabled() const {
  return 0x01 == getUInt8(Offset::simplexRepEnable());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableSimplexRepeater(bool enable) {
  setUInt8(Offset::simplexRepEnable(), enable ? 0x01 : 0x00);
}

Interval
DMR6X2UVCodeplug::GeneralSettingsElement::gpsRangingInterval() const {
  return Interval::fromSeconds(getUInt8(Offset::gpsRangingInterval()));
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setGPSRangingInterval(Interval intv) {
  setUInt8(Offset::gpsRangingInterval(), intv.seconds());
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::monitorSimplexRepeaterEnabled() const {
  return 0x01 == getUInt8(Offset::simplxRepSpeaker());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableMonitorSimplexRepeater(bool enable) {
  setUInt8(Offset::simplxRepSpeaker(), enable ? 0x01 : 0x00);
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::showCurrentContact() const {
  return getUInt8(Offset::showContact());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableShowCurrentContact(bool enable) {
  setUInt8(Offset::showContact(), (enable ? 0x01 : 0x00));
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::keyToneLevelAdjustable() const {
  return 0 == keyToneLevel();
}
unsigned
DMR6X2UVCodeplug::GeneralSettingsElement::keyToneLevel() const {
  return ((unsigned)getUInt8(Offset::keyToneLevel()))*10/15;
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setKeyToneLevel(unsigned level) {
  setUInt8(Offset::keyToneLevel(), level*10/15);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setKeyToneLevelAdjustable() {
  setUInt8(Offset::keyToneLevel(), 0);
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::knobLock() const {
  return getBit(Offset::knobLock(), 0);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableKnobLock(bool enable) {
  setBit(Offset::knobLock(), 0, enable);
}
bool
DMR6X2UVCodeplug::GeneralSettingsElement::keypadLock() const {
  return getBit(Offset::keypadLock(), 1);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableKeypadLock(bool enable) {
  setBit(Offset::keypadLock(), 1, enable);
}
bool
DMR6X2UVCodeplug::GeneralSettingsElement::sidekeysLock() const {
  return getBit(Offset::sideKeyLock(), 3);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableSidekeysLock(bool enable) {
  setBit(Offset::sideKeyLock(), 3, enable);
}
bool
DMR6X2UVCodeplug::GeneralSettingsElement::keyLockForced() const {
  return getBit(Offset::forceKeyLock(), 4);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableKeyLockForced(bool enable) {
  setBit(Offset::forceKeyLock(), 4, enable);
}

AnytoneSimplexRepeaterSettingsExtension::TimeSlot
DMR6X2UVCodeplug::GeneralSettingsElement::simplexRepeaterTimeslot() const {
  return (AnytoneSimplexRepeaterSettingsExtension::TimeSlot)getUInt8(Offset::simplxRepSlot());
}

void
DMR6X2UVCodeplug::GeneralSettingsElement::setSimplexRepeaterTimeslot(AnytoneSimplexRepeaterSettingsExtension::TimeSlot slot) {
  setUInt8(Offset::simplxRepSlot(), (unsigned int) slot);
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::showLastHeard() const {
  return getUInt8(Offset::showLastHeard());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableShowLastHeard(bool enable) {
  setUInt8(Offset::showLastHeard(), (enable ? 0x01 : 0x00));
}

AnytoneDMRSettingsExtension::SMSFormat
DMR6X2UVCodeplug::GeneralSettingsElement::smsFormat() const {
  return (AnytoneDMRSettingsExtension::SMSFormat) getUInt8(Offset::smsFormat());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setSMSFormat(AnytoneDMRSettingsExtension::SMSFormat format) {
  setUInt8(Offset::smsFormat(), (unsigned int)format);
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::gpsUnitsImperial() const {
  return getUInt8(Offset::gpsUnits());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableGPSUnitsImperial(bool enable) {
  setUInt8(Offset::gpsUnits(), (enable ? 0x01 : 0x00));
}

Frequency
DMR6X2UVCodeplug::GeneralSettingsElement::autoRepeaterMinFrequencyVHF() const {
  return Frequency::fromHz(getUInt32_le(Offset::autoRepMinVHF())*10);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setAutoRepeaterMinFrequencyVHF(Frequency freq) {
  setUInt32_le(Offset::autoRepMinVHF(), freq.inHz()/10);
}
Frequency
DMR6X2UVCodeplug::GeneralSettingsElement::autoRepeaterMaxFrequencyVHF() const {
  return Frequency::fromHz(getUInt32_le(Offset::autoRepMaxVHF())*10);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setAutoRepeaterMaxFrequencyVHF(Frequency freq) {
  setUInt32_le(Offset::autoRepMaxVHF(), freq.inHz()/10);
}

Frequency
DMR6X2UVCodeplug::GeneralSettingsElement::autoRepeaterMinFrequencyUHF() const {
  return Frequency::fromHz(getUInt32_le(Offset::autoRepMinUHF())*10);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setAutoRepeaterMinFrequencyUHF(Frequency freq) {
  setUInt32_le(Offset::autoRepMinUHF(), freq.inHz()/10);
}
Frequency
DMR6X2UVCodeplug::GeneralSettingsElement::autoRepeaterMaxFrequencyUHF() const {
  return Frequency::fromHz(getUInt32_le(Offset::autoRepMaxUHF())*10);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setAutoRepeaterMaxFrequencyUHF(Frequency freq) {
  setUInt32_le(Offset::autoRepMaxUHF(), freq.inHz()/10);
}

AnytoneAutoRepeaterSettingsExtension::Direction
DMR6X2UVCodeplug::GeneralSettingsElement::autoRepeaterDirectionB() const {
  return (AnytoneAutoRepeaterSettingsExtension::Direction)getUInt8(Offset::autoRepeaterDirB());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setAutoRepeaterDirectionB(AnytoneAutoRepeaterSettingsExtension::Direction dir) {
  setUInt8(Offset::autoRepeaterDirB(), (uint8_t)dir);
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::fmSendIDAndContact() const {
  return 0 != getUInt8(Offset::fmSendIDAndContact());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableFMSendIDAndContact(bool enable) {
  setUInt8(Offset::fmSendIDAndContact(), enable ? 0x01 : 0x00);
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::defaultChannel() const {
  return 0x01 == getUInt8(Offset::defaultChannels());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableDefaultChannel(bool enable) {
  setUInt8(Offset::defaultChannels(), enable ? 0x01 : 0x00);
}

unsigned int
DMR6X2UVCodeplug::GeneralSettingsElement::defaultZoneIndexA() const {
  return getUInt8(Offset::defaultZoneA());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setDefaultZoneIndexA(unsigned int index) {
  setUInt8(Offset::defaultZoneA(), index);
}
bool
DMR6X2UVCodeplug::GeneralSettingsElement::defaultChannelAIsVFO() const {
  return 0xff == defaultChannelAIndex();
}
unsigned int
DMR6X2UVCodeplug::GeneralSettingsElement::defaultChannelAIndex() const {
  return getUInt8(Offset::defaultChannelA());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setDefaultChannelAIndex(unsigned int index) {
  return setUInt8(Offset::defaultChannelA(), index);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setDefaultChannelAToVFO() {
  setDefaultChannelAIndex(0xff);
}

unsigned int
DMR6X2UVCodeplug::GeneralSettingsElement::defaultZoneIndexB() const {
  return getUInt8(Offset::defaultZoneB());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setDefaultZoneIndexB(unsigned int index) {
  setUInt8(Offset::defaultZoneB(), index);
}
bool
DMR6X2UVCodeplug::GeneralSettingsElement::defaultChannelBIsVFO() const {
  return 0xff == defaultChannelBIndex();
}
unsigned int
DMR6X2UVCodeplug::GeneralSettingsElement::defaultChannelBIndex() const {
  return getUInt8(Offset::defaultChannelB());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setDefaultChannelBIndex(unsigned int index) {
  return setUInt8(Offset::defaultChannelB(), index);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setDefaultChannelBToVFO() {
  setDefaultChannelBIndex(0xff);
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::keepLastCaller() const {
  return getUInt8(Offset::keepLastCaller());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableKeepLastCaller(bool enable) {
  setUInt8(Offset::keepLastCaller(), (enable ? 0x01 : 0x00));
}

Interval
DMR6X2UVCodeplug::GeneralSettingsElement::rxBacklightDuration() const {
  return Interval::fromSeconds(getUInt8(Offset::rxBacklightDuration()));
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setRXBacklightDuration(Interval dur) {
  unsigned int secs = std::max(30ULL, dur.seconds());
  setUInt8(Offset::rxBacklightDuration(), secs);
}

AnytoneDisplaySettingsExtension::Color
DMR6X2UVCodeplug::GeneralSettingsElement::standbyBackgroundColor() const {
  return (AnytoneDisplaySettingsExtension::Color)getUInt8(Offset::standbyBackground());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setStandbyBackgroundColor(AnytoneDisplaySettingsExtension::Color color) {
  setUInt8(Offset::standbyBackground(), (unsigned)color);
}

unsigned int
DMR6X2UVCodeplug::GeneralSettingsElement::manualDialedGroupCallHangTime() const {
  return getUInt8(Offset::manGrpCallHangTime());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setManualDialedGroupCallHangTime(unsigned int dur) {
  setUInt8(Offset::manGrpCallHangTime(), dur);
}
unsigned int
DMR6X2UVCodeplug::GeneralSettingsElement::manualDialedPrivateCallHangTime() const {
  return getUInt8(Offset::manPrvCallHangTime());
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setManualDialedPrivateCallHangTime(unsigned int dur) {
  setUInt8(Offset::manPrvCallHangTime(), dur);
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::bluetooth() const {
  return false;
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableBluetooth(bool enable) {
  Q_UNUSED(enable)
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::btAndInternalMic() const {
  return false;
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableBTAndInternalMic(bool enable) {
  Q_UNUSED(enable)
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::btAndInternalSpeaker() const {
  return false;
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableBTAndInternalSpeaker(bool enable) {
  Q_UNUSED(enable)
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::pluginRecTone() const {
  return false;
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enablePluginRecTone(bool enable) {
  Q_UNUSED(enable)
}

unsigned int
DMR6X2UVCodeplug::GeneralSettingsElement::btMicGain() const {
  return 0;
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setBTMicGain(unsigned int gain) {
  Q_UNUSED(gain)
}

unsigned int
DMR6X2UVCodeplug::GeneralSettingsElement::btSpeakerGain() const {
  return 0;
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setBTSpeakerGain(unsigned int gain) {
  Q_UNUSED(gain)
}

Interval
DMR6X2UVCodeplug::GeneralSettingsElement::autoRoamPeriod() const {
  return Interval::fromSeconds(0);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setAutoRoamPeriod(Interval intv) {
  Q_UNUSED(intv)
}

Interval
DMR6X2UVCodeplug::GeneralSettingsElement::autoRoamDelay() const {
  return Interval::fromSeconds(0);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setAutoRoamDelay(Interval intv) {
  Q_UNUSED(intv)
}

AnytoneDisplaySettingsExtension::Color
DMR6X2UVCodeplug::GeneralSettingsElement::standbyTextColor() const {
  return AnytoneDisplaySettingsExtension::Color::White;
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setStandbyTextColor(AnytoneDisplaySettingsExtension::Color color) {
  Q_UNUSED(color)
}

unsigned
DMR6X2UVCodeplug::GeneralSettingsElement::defaultRoamingZoneIndex() const {
  return 0;
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setDefaultRoamingZoneIndex(unsigned idx) {
  Q_UNUSED(idx)
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::repeaterRangeCheck() const {
  return false;
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableRepeaterRangeCheck(bool enable) {
  Q_UNUSED(enable)
}

Interval
DMR6X2UVCodeplug::GeneralSettingsElement::repeaterRangeCheckInterval() const {
  return Interval::fromSeconds(0);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setRepeaterRangeCheckInterval(Interval intv) {
  Q_UNUSED(intv)
}

unsigned
DMR6X2UVCodeplug::GeneralSettingsElement::repeaterRangeCheckCount() const {
  return 0;
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setRepeaterRangeCheckCount(unsigned n) {
  Q_UNUSED(n)
}

AnytoneRangingSettingsExtension::RoamStart
DMR6X2UVCodeplug::GeneralSettingsElement::roamingStartCondition() const {
  return AnytoneRangingSettingsExtension::RoamStart::OutOfRange;
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setRoamingStartCondition(AnytoneRangingSettingsExtension::RoamStart cond) {
  Q_UNUSED(cond)
}

Interval
DMR6X2UVCodeplug::GeneralSettingsElement::txBacklightDuration() const {
  return Interval::fromSeconds(0);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setTXBacklightDuration(Interval intv) {
  Q_UNUSED(intv)
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::separateDisplay() const {
  return false;
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableSeparateDisplay(bool enable) {
  Q_UNUSED(enable)
}

AnytoneDisplaySettingsExtension::Color
DMR6X2UVCodeplug::GeneralSettingsElement::channelNameColor() const {
  return AnytoneDisplaySettingsExtension::Color::White;
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setChannelNameColor(AnytoneDisplaySettingsExtension::Color color) {
  Q_UNUSED(color)
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::repeaterCheckNotification() const {
  return false;
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableRepeaterCheckNotification(bool enable) {
  Q_UNUSED(enable)
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::roaming() const {
  return false;
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableRoaming(bool enable) {
  Q_UNUSED(enable)
}

Interval
DMR6X2UVCodeplug::GeneralSettingsElement::muteDelay() const {
  return Interval::fromSeconds(0);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setMuteDelay(Interval min) {
  Q_UNUSED(min)
}

unsigned
DMR6X2UVCodeplug::GeneralSettingsElement::repeaterCheckNumNotifications() const {
  return 0;
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setRepeaterCheckNumNotifications(unsigned num) {
  Q_UNUSED(num)
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::bootGPSCheck() const {
  return false;
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableBootGPSCheck(bool enable) {
  Q_UNUSED(enable)
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::bootReset() const {
  return false;
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableBootReset(bool enable) {
  Q_UNUSED(enable)
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::btHoldTimeEnabled() const {
  return false;
}
bool
DMR6X2UVCodeplug::GeneralSettingsElement::btHoldTimeInfinite() const {
  return false;
}
Interval
DMR6X2UVCodeplug::GeneralSettingsElement::btHoldTime() const {
  return Interval::fromSeconds(0);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setBTHoldTime(Interval interval) {
  Q_UNUSED(interval)
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setBTHoldTimeInfinite() {
  // pass...
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::disableBTHoldTime() {
  // pass...
}

Interval
DMR6X2UVCodeplug::GeneralSettingsElement::btRXDelay() const {
  return Interval::fromMilliseconds(30);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setBTRXDelay(Interval delay) {
  Q_UNUSED(delay)
}


bool
DMR6X2UVCodeplug::GeneralSettingsElement::fromConfig(const Flags &flags, Context &ctx)
{
  if (! D878UVCodeplug::GeneralSettingsElement::fromConfig(flags, ctx))
    return false;

  // apply DMR-6X2UV specific settings.
  if (AnytoneSettingsExtension *ext = ctx.config()->settings()->anytoneExtension()) {
    // Apply simplex repeater settings
    enableSimplexRepeater(ext->simplexRepeaterSettings()->enabled());
    enableMonitorSimplexRepeater(ext->simplexRepeaterSettings()->monitorEnabled());
    setSimplexRepeaterTimeslot(ext->simplexRepeaterSettings()->timeSlot());
  }
  return true;
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::updateConfig(Context &ctx) {
  if (! D878UVCodeplug::GeneralSettingsElement::updateConfig(ctx))
    return false;

  // Get or add settings extension
  AnytoneSettingsExtension *ext = nullptr;
  if (ctx.config()->settings()->anytoneExtension()) {
    ext = ctx.config()->settings()->anytoneExtension();
  } else {
    ext = new AnytoneSettingsExtension();
    ctx.config()->settings()->setAnytoneExtension(ext);
  }
  // Decode simplex-repeater feature.
  ext->simplexRepeaterSettings()->enable(this->simplexRepeaterEnabled());
  ext->simplexRepeaterSettings()->enableMonitor(this->monitorSimplexRepeaterEnabled());
  ext->simplexRepeaterSettings()->setTimeSlot(this->simplexRepeaterTimeslot());

  return true;
}

uint8_t
DMR6X2UVCodeplug::GeneralSettingsElement::mapKeyFunctionToCode(AnytoneKeySettingsExtension::KeyFunction func) const {
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
  case AnytoneKeySettingsExtension::KeyFunction::SimplexRepeater:   return (uint8_t)KeyFunction::SimplexRepeater;
  case AnytoneKeySettingsExtension::KeyFunction::Ranging:           return (uint8_t)KeyFunction::Ranging;
  case AnytoneKeySettingsExtension::KeyFunction::ChannelRanging:    return (uint8_t)KeyFunction::ChannelRanging;
  case AnytoneKeySettingsExtension::KeyFunction::MaxVolume:         return (uint8_t)KeyFunction::MaxVolume;
  case AnytoneKeySettingsExtension::KeyFunction::Slot:              return (uint8_t)KeyFunction::Slot;
  case AnytoneKeySettingsExtension::KeyFunction::Squelch:           return (uint8_t)KeyFunction::Squelch;
  case AnytoneKeySettingsExtension::KeyFunction::Roaming:           return (uint8_t)KeyFunction::Roaming;
  case AnytoneKeySettingsExtension::KeyFunction::Zone:              return (uint8_t)KeyFunction::Zone;
  case AnytoneKeySettingsExtension::KeyFunction::RoamingSet:        return (uint8_t)KeyFunction::RoamingSet;
  case AnytoneKeySettingsExtension::KeyFunction::Mute:              return (uint8_t)KeyFunction::Mute;
  case AnytoneKeySettingsExtension::KeyFunction::CtcssDcsSet:       return (uint8_t)KeyFunction::CtcssDcsSet;
  case AnytoneKeySettingsExtension::KeyFunction::APRSSet:           return (uint8_t)KeyFunction::APRSSet;
  case AnytoneKeySettingsExtension::KeyFunction::APRSSend:          return (uint8_t)KeyFunction::APRSSend;
  default:                                                          return (uint8_t)KeyFunction::Off;
  }
}

AnytoneKeySettingsExtension::KeyFunction
DMR6X2UVCodeplug::GeneralSettingsElement::mapCodeToKeyFunction(uint8_t code) const {
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
  case KeyFunction::SimplexRepeater:   return AnytoneKeySettingsExtension::KeyFunction::SimplexRepeater;
  case KeyFunction::Ranging:           return AnytoneKeySettingsExtension::KeyFunction::Ranging;
  case KeyFunction::ChannelRanging:    return AnytoneKeySettingsExtension::KeyFunction::ChannelRanging;
  case KeyFunction::MaxVolume:         return AnytoneKeySettingsExtension::KeyFunction::MaxVolume;
  case KeyFunction::Slot:              return AnytoneKeySettingsExtension::KeyFunction::Slot;
  case KeyFunction::Squelch:           return AnytoneKeySettingsExtension::KeyFunction::Squelch;
  case KeyFunction::Roaming:           return AnytoneKeySettingsExtension::KeyFunction::Roaming;
  case KeyFunction::Zone:              return AnytoneKeySettingsExtension::KeyFunction::Zone;
  case KeyFunction::RoamingSet:        return AnytoneKeySettingsExtension::KeyFunction::RoamingSet;
  case KeyFunction::Mute:              return AnytoneKeySettingsExtension::KeyFunction::Mute;
  case KeyFunction::CtcssDcsSet:       return AnytoneKeySettingsExtension::KeyFunction::CtcssDcsSet;
  case KeyFunction::APRSSet:           return AnytoneKeySettingsExtension::KeyFunction::APRSSet;
  case KeyFunction::APRSSend:          return AnytoneKeySettingsExtension::KeyFunction::APRSSend;
  default:                             return AnytoneKeySettingsExtension::KeyFunction::Off;
  }
}


/* ********************************************************************************************* *
 * Implementation of DMR6X2UVCodeplug::ExtendedSettingsElement
 * ********************************************************************************************* */
DMR6X2UVCodeplug::ExtendedSettingsElement::ExtendedSettingsElement(uint8_t *ptr, unsigned size)
  : Codeplug::Element(ptr, size)
{
  // pass...
}

DMR6X2UVCodeplug::ExtendedSettingsElement::ExtendedSettingsElement(uint8_t *ptr)
  : Codeplug::Element(ptr, ExtendedSettingsElement::size())
{
  // pass...
}

void
DMR6X2UVCodeplug::ExtendedSettingsElement::clear() {
  Codeplug::Element::clear();
}

bool
DMR6X2UVCodeplug::ExtendedSettingsElement::talkerAliasIsSend() const {
  return 0x01 == getUInt8(0x0000);
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::enableSendTalkerAlias(bool enable) {
  setUInt8(0x0000, enable ? 0x01 : 0x00);
}

DMR6X2UVCodeplug::ExtendedSettingsElement::TalkerAliasSource
DMR6X2UVCodeplug::ExtendedSettingsElement::talkerAliasSource() const {
  return (TalkerAliasSource) getUInt8(0x0001);
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::setTalkerAliasSource(TalkerAliasSource source) {
  setUInt8(0x0001, (uint8_t)source);
}

DMR6X2UVCodeplug::ExtendedSettingsElement::TalkerAliasEncoding
DMR6X2UVCodeplug::ExtendedSettingsElement::talkerAliasEncoding() const {
  return (TalkerAliasEncoding) getUInt8(0x0002);
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::setTalkerAliasEncoding(TalkerAliasEncoding encoding) {
  setUInt8(0x0002, (uint8_t)encoding);
}

DMR6X2UVCodeplug::ExtendedSettingsElement::FontColor
DMR6X2UVCodeplug::ExtendedSettingsElement::fontColor() const {
  return (FontColor) getUInt8(0x0003);
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::setFontColor(FontColor color) {
  setUInt8(0x0003, (uint8_t)color);
}

bool
DMR6X2UVCodeplug::ExtendedSettingsElement::customChannelBackgroundEnabled() const {
  return 0x01 == getUInt8(0x0004);
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::enableCustomChannelBackground(bool enable) {
  setUInt8(0x0004, enable ? 0x01 : 0x00);
}

unsigned int
DMR6X2UVCodeplug::ExtendedSettingsElement::roamingZoneIndex() const {
  return getUInt8(0x0005);
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::setRoamingZoneIndex(unsigned int index) {
  setUInt8(0x0005, index);
}
bool
DMR6X2UVCodeplug::ExtendedSettingsElement::autoRoamingEnabled() const {
  return 0x01 == getUInt8(0x0006);
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::enableAutoRoaming(bool enable) {
  setUInt8(0x006, enable ? 0x01 : 0x00);
}
bool
DMR6X2UVCodeplug::ExtendedSettingsElement::repeaterCheckEnabled() const {
  return 0x01 == getUInt8(0x0007);
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::enableRepeaterCheck(bool enable) {
  setUInt8(0x0007, enable ? 0x01 : 0x00);
}
DMR6X2UVCodeplug::ExtendedSettingsElement::OutOfRangeAlert
DMR6X2UVCodeplug::ExtendedSettingsElement::repeaterOutOfRangeAlert() const {
  return (OutOfRangeAlert) getUInt8(0x0008);
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::setRepeaterOutOfRangeAlert(OutOfRangeAlert alert) {
  setUInt8(0x0008, (uint8_t)alert);
}
unsigned int
DMR6X2UVCodeplug::ExtendedSettingsElement::repeaterOutOfRangeReminder() const {
  return getUInt8(0x0009)+1;
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::setRepeaterOutOfRangeReminder(unsigned int n) {
  n = std::max(1U, std::min(10U, n));
  setUInt8(0x0009, n-1);
}
unsigned int
DMR6X2UVCodeplug::ExtendedSettingsElement::repeaterCheckIntervall() const {
  return (1+getUInt8(0x000a))*5;
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::setRepeaterCheckIntervall(unsigned int intervall) {
  intervall = std::min(50U, std::max(5U, intervall));
  setUInt8(0x000a, intervall/5-1);
}
unsigned int
DMR6X2UVCodeplug::ExtendedSettingsElement::repeaterReconnections() const {
  return getUInt8(0x000b)+3;
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::setRepeaterReconnections(unsigned int n) {
  n = std::max(3U, std::min(5U, n));
  setUInt8(0x000b, n);
}

DMR6X2UVCodeplug::ExtendedSettingsElement::RoamingCondition
DMR6X2UVCodeplug::ExtendedSettingsElement::startRoamingCondition() const {
  return (RoamingCondition) getUInt8(0x000c);
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::setStartRoamingCondition(RoamingCondition cond) {
  setUInt8(0x000c, (uint8_t)cond);
}

unsigned int
DMR6X2UVCodeplug::ExtendedSettingsElement::autoRoamingIntervall() const {
  return ((unsigned int) getUInt8(0x000d)) + 1;
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::setAutoRoamingIntervall(unsigned int minutes) {
  minutes = std::max(1U, std::min(256U, minutes));
  setUInt8(0x000d, minutes-1);
}

unsigned int
DMR6X2UVCodeplug::ExtendedSettingsElement::effectiveRoamingWaitingTime() const {
  return getUInt8(0x000e);
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::setEffectiveRoamingWaitingTime(unsigned int sec) {
  sec = std::min(30U, sec);
  setUInt8(0x000e, sec);
}

DMR6X2UVCodeplug::ExtendedSettingsElement::RoamingCondition
DMR6X2UVCodeplug::ExtendedSettingsElement::roamingReturnCondition() const {
  return (RoamingCondition) getUInt8(0x000f);
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::setRoamingReturnCondition(RoamingCondition cond) {
  setUInt8(0x000f, (uint8_t)cond);
}

unsigned int
DMR6X2UVCodeplug::ExtendedSettingsElement::muteTimer() const {
  return ((unsigned int)getUInt8(0x0010)) + 1;
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::setMuteTimer(unsigned int minutes) {
  minutes = std::max(1U, minutes);
  setUInt8(0x0010, minutes-1);
}

DMR6X2UVCodeplug::ExtendedSettingsElement::EncryptionType
DMR6X2UVCodeplug::ExtendedSettingsElement::encryptionType() const {
  return (EncryptionType) getUInt8(0x0011);
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::setEncryptionType(EncryptionType type) {
  setUInt8(0x0011, (uint8_t)type);
}

DMR6X2UVCodeplug::ExtendedSettingsElement::NameColor
DMR6X2UVCodeplug::ExtendedSettingsElement::zoneANameColor() const {
  return (NameColor) getUInt8(0x0012);
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::setZoneANameColor(NameColor color) {
  setUInt8(0x0012, (uint8_t) color);
}
DMR6X2UVCodeplug::ExtendedSettingsElement::NameColor
DMR6X2UVCodeplug::ExtendedSettingsElement::zoneBNameColor() const {
  return (NameColor) getUInt8(0x0013);
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::setZoneBNameColor(NameColor color) {
  setUInt8(0x0013, (uint8_t) color);
}

DMR6X2UVCodeplug::ExtendedSettingsElement::NameColor
DMR6X2UVCodeplug::ExtendedSettingsElement::channelANameColor() const {
  return (NameColor) getUInt8(0x0014);
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::setChannelANameColor(NameColor color) {
  setUInt8(0x0014, (uint8_t) color);
}
DMR6X2UVCodeplug::ExtendedSettingsElement::NameColor
DMR6X2UVCodeplug::ExtendedSettingsElement::channelBNameColor() const {
  return (NameColor) getUInt8(0x0015);
}
void
DMR6X2UVCodeplug::ExtendedSettingsElement::setChannelBNameColor(NameColor color) {
  setUInt8(0x0015, (uint8_t) color);
}

bool
DMR6X2UVCodeplug::ExtendedSettingsElement::fromConfig(const Flags &flags, Context &ctx)
{
  Q_UNUSED(flags); Q_UNUSED(ctx);
  return true;
}

bool
DMR6X2UVCodeplug::ExtendedSettingsElement::updateConfig(Context &ctx) {
  Q_UNUSED(ctx);
  return true;
}



/* ********************************************************************************************* *
 * Implementation of DMR6X2UVCodeplug::ChannelElement
 * ********************************************************************************************* */
DMR6X2UVCodeplug::ChannelElement::ChannelElement(uint8_t *ptr, unsigned size)
  : AnytoneCodeplug::ChannelElement(ptr, size)
{
  // pass...
}

DMR6X2UVCodeplug::ChannelElement::ChannelElement(uint8_t *ptr)
  : AnytoneCodeplug::ChannelElement(ptr)
{
  // pass...
}

bool
DMR6X2UVCodeplug::ChannelElement::hasScanListIndex() const {
  return hasScanListIndex(0);
}
unsigned
DMR6X2UVCodeplug::ChannelElement::scanListIndex() const {
  return scanListIndex(0);
}
void
DMR6X2UVCodeplug::ChannelElement::setScanListIndex(unsigned idx) {
  setScanListIndex(0, idx);
}
void
DMR6X2UVCodeplug::ChannelElement::clearScanListIndex() {
  clearScanListIndex(0);
}

bool
DMR6X2UVCodeplug::ChannelElement::hasScanListIndex(unsigned int n) const {
  return 0xff != scanListIndex(n);
}
unsigned int
DMR6X2UVCodeplug::ChannelElement::scanListIndex(unsigned int n) const {
  if (n > 7)
    return 0xff;
  return getUInt8(0x0036+n);
}
void
DMR6X2UVCodeplug::ChannelElement::setScanListIndex(unsigned int n, unsigned idx) {
  if (n > 7)
    return;
  setUInt8(0x0036+n, idx);
}
void
DMR6X2UVCodeplug::ChannelElement::clearScanListIndex(unsigned int n) {
  setScanListIndex(n, 0xff);
}

bool
DMR6X2UVCodeplug::ChannelElement::roamingEnabled() const {
  return ! getBit(0x001b, 2);
}
void
DMR6X2UVCodeplug::ChannelElement::enableRoaming(bool enable) {
  setBit(0x001b, 2, !enable);
}

bool
DMR6X2UVCodeplug::ChannelElement::ranging() const {
  return getBit(0x001b, 0);
}
void
DMR6X2UVCodeplug::ChannelElement::enableRanging(bool enable) {
  return setBit(0x001b, 0, enable);
}


/* ********************************************************************************************* *
 * Implementation of DMR6X2UVCodeplug
 * ********************************************************************************************* */
DMR6X2UVCodeplug::DMR6X2UVCodeplug(const QString &label, QObject *parent)
  : D868UVCodeplug(label, parent)
{
  // pass...
}

DMR6X2UVCodeplug::DMR6X2UVCodeplug(QObject *parent)
  : D868UVCodeplug("BTECH DMR-6X2UV", parent)
{
  // pass...
}

bool
DMR6X2UVCodeplug::allocateBitmaps() {
  if (! D868UVCodeplug::allocateBitmaps())
    return false;

  // Roaming channel bitmaps
  image(0).addElement(ADDR_ROAMING_CHANNEL_BITMAP, ROAMING_CHANNEL_BITMAP_SIZE);
  // Roaming zone bitmaps
  image(0).addElement(ADDR_ROAMING_ZONE_BITMAP, ROAMING_ZONE_BITMAP_SIZE);

  return true;
}

void
DMR6X2UVCodeplug::setBitmaps(Context& ctx)
{
  // First set everything common between D868UV and D878UV codeplugs.
  D868UVCodeplug::setBitmaps(ctx);

  // Mark roaming zones
  uint8_t *roaming_zone_bitmap = data(ADDR_ROAMING_ZONE_BITMAP);
  memset(roaming_zone_bitmap, 0x00, ROAMING_ZONE_BITMAP_SIZE);
  unsigned int num_roaming_zones = std::min((unsigned int) NUM_ROAMING_ZONES, ctx.count<RoamingZone>());
  for (unsigned int i=0; i<num_roaming_zones; i++)
    roaming_zone_bitmap[i/8] |= (1<<(i%8));

  // Mark roaming channels
  uint8_t *roaming_ch_bitmap = data(ADDR_ROAMING_CHANNEL_BITMAP);
  memset(roaming_ch_bitmap, 0x00, ROAMING_CHANNEL_BITMAP_SIZE);
  unsigned int num_roaming_channels = std::min((unsigned int)NUM_ROAMING_CHANNEL,ctx.count<RoamingChannel>());
  // Get all (unique) channels used in roaming
  for (unsigned int i=0; i<num_roaming_channels; i++)
    roaming_ch_bitmap[i/8] |= (1<<(i%8));
}

void
DMR6X2UVCodeplug::allocateUpdated() {
  // First allocate everything common between D868UV and DMR-6X2UV codeplugs.
  D868UVCodeplug::allocateUpdated();
}

void
DMR6X2UVCodeplug::allocateForEncoding() {
  // First allocate everything common between D868UV and D878UV codeplugs.
  D868UVCodeplug::allocateForEncoding();
  this->allocateRoaming();
}

void
DMR6X2UVCodeplug::allocateForDecoding() {
  // First allocate everything common between D868UV and D878UV codeplugs.
  D868UVCodeplug::allocateForDecoding();
  this->allocateRoaming();
}

bool
DMR6X2UVCodeplug::encodeElements(const Flags &flags, Context &ctx, const ErrorStack &err)
{
  // Encode everything common between d868uv and d878uv radios.
  if (! D868UVCodeplug::encodeElements(flags, ctx, err))
    return false;

  if (! this->encodeRoaming(flags, ctx, err))
    return false;

  return true;
}


bool
DMR6X2UVCodeplug::decodeElements(Context &ctx, const ErrorStack &err)
{
  // Decode everything commong between d868uv and d878uv codeplugs.
  if (! D868UVCodeplug::decodeElements(ctx, err))
    return false;

  if (! this->createRoaming(ctx, err))
    return false;

  if (! this->linkRoaming(ctx, err))
    return false;

  return true;
}


void
DMR6X2UVCodeplug::allocateGeneralSettings() {
  image(0).addElement(Offset::settings(), GeneralSettingsElement::size());
  image(0).addElement(Offset::settingsExtension(), ExtendedSettingsElement::size());
}

bool
DMR6X2UVCodeplug::encodeGeneralSettings(const Flags &flags, Context &ctx, const ErrorStack &err) {
  if (! GeneralSettingsElement(data(Offset::settings())).fromConfig(flags, ctx)) {
    errMsg(err) << "Cannot encode general settings element.";
    return false;
  }

  if (! ExtendedSettingsElement(data(Offset::settingsExtension())).fromConfig(flags, ctx)) {
    errMsg(err) << "Cannot encode extended settings element.";
    return false;
  }

  return true;
}

bool
DMR6X2UVCodeplug::decodeGeneralSettings(Context &ctx, const ErrorStack &err) {
  if (! GeneralSettingsElement(data(Offset::settings())).updateConfig(ctx)) {
    errMsg(err) << "Cannot decode general settings element.";
    return false;
  }

  if (! ExtendedSettingsElement(data(Offset::settingsExtension())).updateConfig(ctx)) {
    errMsg(err) << "Cannot decode extended settings element.";
    return false;
  }

  return true;
}
bool
DMR6X2UVCodeplug::linkGeneralSettings(Context &ctx, const ErrorStack &err) {
  return GeneralSettingsElement(data(Offset::settings())).linkSettings(ctx.config()->settings(), ctx, err);
}


bool
DMR6X2UVCodeplug::encodeChannels(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)

  // Encode channels
  for (int i=0; i<ctx.config()->channelList()->count(); i++) {
    // enable channel
    uint16_t bank = i/128, idx = i%128;
    ChannelElement ch(data(CHANNEL_BANK_0 + bank*CHANNEL_BANK_OFFSET + idx*CHANNEL_SIZE));
    if (! ch.fromChannelObj(ctx.config()->channelList()->channel(i), ctx))
      return false;
  }
  return true;
}

bool
DMR6X2UVCodeplug::createChannels(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)
  // Create channels
  uint8_t *channel_bitmap = data(CHANNEL_BITMAP);
  for (uint16_t i=0; i<NUM_CHANNELS; i++) {
    // Check if channel is enabled:
    uint16_t  bit = i%8, byte = i/8, bank = i/128, idx = i%128;
    if (0 == ((channel_bitmap[byte]>>bit) & 0x01))
      continue;
    ChannelElement ch(data(CHANNEL_BANK_0 + bank*CHANNEL_BANK_OFFSET + idx*CHANNEL_SIZE));
    if (Channel *obj = ch.toChannelObj(ctx)) {
      ctx.config()->channelList()->add(obj); ctx.add(obj, i);
    }
  }
  return true;
}

bool
DMR6X2UVCodeplug::linkChannels(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  // Link channel objects
  for (uint16_t i=0; i<NUM_CHANNELS; i++) {
    // Check if channel is enabled:
    uint16_t  bit = i%8, byte = i/8, bank = i/128, idx = i%128;
    if (0 == (((*data(CHANNEL_BITMAP+byte))>>bit) & 0x01))
      continue;
    ChannelElement ch(data(CHANNEL_BANK_0 + bank*CHANNEL_BANK_OFFSET + idx*CHANNEL_SIZE));
    if (ctx.has<Channel>(i))
      ch.linkChannelObj(ctx.get<Channel>(i), ctx);
  }
  return true;
}

void
DMR6X2UVCodeplug::allocateGPSSystems() {
  // replaces D868UVCodeplug::allocateGPSSystems

  // APRS settings
  image(0).addElement(ADDR_APRS_SETTINGS, APRS_SETTINGS_SIZE);
  image(0).addElement(ADDR_APRS_MESSAGE, APRS_MESSAGE_SIZE);
  image(0).addElement(ADDR_DMRAPRS_SETTINGS, DMRAPRS_SETTINGS_SIZE);
}

bool
DMR6X2UVCodeplug::encodeGPSSystems(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)
  // replaces D868UVCodeplug::encodeGPSSystems

  // Encode APRS system (there can only be one)
  if (0 < ctx.config()->posSystems()->aprsCount()) {
    D878UVCodeplug::AnalogAPRSSettingsElement(data(ADDR_APRS_SETTINGS))
        .fromAPRSSystem(ctx.config()->posSystems()->aprsSystem(0), ctx);
    uint8_t *aprsmsg = (uint8_t *)data(ADDR_APRS_MESSAGE);
    encode_ascii(aprsmsg, ctx.config()->posSystems()->aprsSystem(0)->message(), 60, 0x00);
  }

  // Encode GPS systems
  D878UVCodeplug::DMRAPRSSettingsElement gps(data(ADDR_DMRAPRS_SETTINGS));
  if (! gps.fromGPSSystems(ctx))
    return false;
  if (0 < ctx.config()->posSystems()->gpsCount()) {
    // If there is at least one GPS system defined -> set auto TX interval.
    //  This setting might be overridden by any analog APRS system below
    D878UVCodeplug::AnalogAPRSSettingsElement aprs(data(ADDR_APRS_SETTINGS));
    aprs.setAutoTXInterval(ctx.config()->posSystems()->gpsSystem(0)->period());
    aprs.setManualTXInterval(ctx.config()->posSystems()->gpsSystem(0)->period());
  }
  return true;
}

bool
DMR6X2UVCodeplug::createGPSSystems(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  // replaces D868UVCodeplug::createGPSSystems

  // Before creating any GPS/APRS systems, get global auto TX interval
  D878UVCodeplug::AnalogAPRSSettingsElement aprs(data(ADDR_APRS_SETTINGS));
  unsigned pos_intervall = aprs.autoTXInterval();

  // Create APRS system (if enabled)
  uint8_t *aprsmsg = (uint8_t *)data(ADDR_APRS_MESSAGE);
  if (aprs.isValid()) {
    APRSSystem *sys = aprs.toAPRSSystem();
    sys->setPeriod(pos_intervall);
    sys->setMessage(decode_ascii(aprsmsg, 60, 0x00));
    ctx.config()->posSystems()->add(sys); ctx.add(sys,0);
  }

  // Create GPS systems
  D878UVCodeplug::DMRAPRSSettingsElement gps_systems(data(ADDR_DMRAPRS_SETTINGS));
  for (int i=0; i<NUM_DMRAPRS_SYSTEMS; i++) {
    if (0 == gps_systems.destination(i))
      continue;
    if (GPSSystem *sys = gps_systems.toGPSSystemObj(i)) {
      sys->setPeriod(pos_intervall);
      ctx.config()->posSystems()->add(sys); ctx.add(sys, i);
    } else {
      return false;
    }
  }
  return true;
}

bool
DMR6X2UVCodeplug::linkGPSSystems(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err);
  // replaces D868UVCodeplug::linkGPSSystems

  // Link APRS system
  D878UVCodeplug::AnalogAPRSSettingsElement aprs(data(ADDR_APRS_SETTINGS));
  if (aprs.isValid()) {
    aprs.linkAPRSSystem(ctx.config()->posSystems()->aprsSystem(0), ctx);
  }

  // Link GPS systems
  D878UVCodeplug::DMRAPRSSettingsElement gps_systems(data(ADDR_DMRAPRS_SETTINGS));
  for (int i=0; i<NUM_DMRAPRS_SYSTEMS; i++) {
    if (0 == gps_systems.destination(i))
      continue;
    gps_systems.linkGPSSystem(i, ctx.get<GPSSystem>(i), ctx);
  }

  return true;
}


void
DMR6X2UVCodeplug::allocateRoaming() {
  /* Allocate roaming channels */
  uint8_t *roaming_channel_bitmap = data(ADDR_ROAMING_CHANNEL_BITMAP);
  for (uint8_t i=0; i<NUM_ROAMING_CHANNEL; i++) {
    // Get byte and bit for roaming channel
    uint16_t bit = i%8, byte = i/8;
    // if disabled -> skip
    if (0 == ((roaming_channel_bitmap[byte]>>bit) & 0x01))
      continue;
    // Allocate roaming channel
    uint32_t addr = ADDR_ROAMING_CHANNEL_0 + i*ROAMING_CHANNEL_OFFSET;
    if (nullptr == data(addr, 0))
      image(0).addElement(addr, ROAMING_CHANNEL_SIZE);
  }

  /* Allocate roaming zones. */
  uint8_t *roaming_zone_bitmap = data(ADDR_ROAMING_ZONE_BITMAP);
  for (uint8_t i=0; i<NUM_ROAMING_ZONES; i++) {
    // Get byte and bit for radio ID
    uint16_t bit = i%8, byte = i/8;
    // if disabled -> skip
    if (0 == ((roaming_zone_bitmap[byte]>>bit) & 0x01))
      continue;
    // Allocate roaming zone
    uint32_t addr = ADDR_ROAMING_ZONE_0 + i*ROAMING_ZONE_OFFSET;
    if (nullptr == data(addr, 0)) {
      image(0).addElement(addr, ROAMING_ZONE_SIZE);
    }
  }
}

bool
DMR6X2UVCodeplug::encodeRoaming(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)

  // Encode roaming channels
  for (uint8_t i=0; i<std::min(NUM_ROAMING_CHANNEL, ctx.config()->roamingChannels()->count()); i++) {
    // Encode roaming channel
    uint32_t addr = ADDR_ROAMING_CHANNEL_0 + i*ROAMING_CHANNEL_OFFSET;
    D878UVCodeplug::RoamingChannelElement rch_elm(data(addr));
    RoamingChannel *rch = ctx.config()->roamingChannels()->get(i)->as<RoamingChannel>();
    rch_elm.clear();
    rch_elm.fromChannel(rch);
    if (! ctx.add(rch, i)) {
      errMsg(err) << "Cannot add index " << i << " for roaming channel '"
                  << rch->name() << "' to codeplug context.";
      return false;
    }
  }

  // Encode roaming zones
  for (int i=0; i<ctx.config()->roamingZones()->count(); i++){
    uint32_t addr = ADDR_ROAMING_ZONE_0+i*ROAMING_ZONE_OFFSET;
    D878UVCodeplug::RoamingZoneElement zone(data(addr));
    zone.fromRoamingZone(ctx.config()->roamingZones()->zone(i), ctx);
  }

  return true;
}

bool
DMR6X2UVCodeplug::createRoaming(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  // Create or find roaming channels
  uint8_t *roaming_channel_bitmap = data(ADDR_ROAMING_CHANNEL_BITMAP);
  for (int i=0; i<NUM_ROAMING_CHANNEL; i++) {
    uint8_t byte=i/8, bit=i%8;
    if (0 == ((roaming_channel_bitmap[byte]>>bit) & 0x01))
      continue;
    uint32_t addr = ADDR_ROAMING_CHANNEL_0 + i*ROAMING_CHANNEL_OFFSET;
    D878UVCodeplug::RoamingChannelElement ch(data(addr));
    RoamingChannel *digi = ch.toChannel(ctx);
    ctx.add(digi, i);
  }

  // Create and link roaming zones
  uint8_t *roaming_zone_bitmap = data(ADDR_ROAMING_ZONE_BITMAP);
  for (int i=0; i<NUM_ROAMING_ZONES; i++) {
    uint8_t byte=i/8, bit=i%8;
    if (0 == ((roaming_zone_bitmap[byte]>>bit) & 0x01))
      continue;
    uint32_t addr = ADDR_ROAMING_ZONE_0 + i*ROAMING_ZONE_OFFSET;
    D878UVCodeplug::RoamingZoneElement z(data(addr));
    RoamingZone *zone = z.toRoamingZone(ctx, err);
    ctx.config()->roamingZones()->add(zone); ctx.add(zone, i);
    z.linkRoamingZone(zone, ctx, err);
  }

  return true;
}

bool
DMR6X2UVCodeplug::linkRoaming(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(err)
  // Pass, no need to link roaming channels.
  return true;
}
