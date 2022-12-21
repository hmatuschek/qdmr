#include "dmr6x2uv_codeplug.hh"

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

#define ADDR_GENERAL_CONFIG       0x02500000
#define GENERAL_CONFIG_SIZE       0x00000100
#define ADDR_GENERAL_CONFIG_EXT1  0x02501280
#define GENERAL_CONFIG_EXT1_SIZE  0x00000030
#define ADDR_GENERAL_CONFIG_EXT2  0x02501400
#define GENERAL_CONFIG_EXT2_SIZE  0x00000100


/* ********************************************************************************************* *
 * Implementation of DMR6X2UVCodeplug::GeneralSettingsElement
 * ********************************************************************************************* */
DMR6X2UVCodeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr, unsigned size)
  : AnytoneCodeplug::GeneralSettingsElement(ptr, size)
{
  // pass...
}

DMR6X2UVCodeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr)
  : AnytoneCodeplug::GeneralSettingsElement(ptr, 0x00e0)
{
  // pass...
}

DMR6X2UVCodeplug::GeneralSettingsElement::DisplayColor
DMR6X2UVCodeplug::GeneralSettingsElement::callsignDisplayColor() const {
  return (DisplayColor)getUInt8(0x00b0);
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::simplexRepeaterEnabled() const {
  return 0x01 == getUInt8(0x00b2);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableSimplexRepeater(bool enable) {
  setUInt8(0x00b2, enable ? 0x01 : 0x00);
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::monitorSimplexRepeaterEnabled() const {
  return 0x01 == getUInt8(0x00b3);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableMonitorSimplexRepeater(bool enable) {
  setUInt8(0x00b3, enable ? 0x01 : 0x00);
}

DMR6X2UVCodeplug::GeneralSettingsElement::SimplexRepeaterSlot
DMR6X2UVCodeplug::GeneralSettingsElement::simplexRepeaterTimeslot() const {
  switch (getUInt8(0x00b7)) {
  case 0x00:
    return SimplexRepeaterSlot::TS1;
  case 0x01:
    return SimplexRepeaterSlot::TS2;
  case 0x02:
  default:
    return SimplexRepeaterSlot::Channel;
  }
}

void
DMR6X2UVCodeplug::GeneralSettingsElement::setSimplexRepeaterTimeslot(SimplexRepeaterSlot slot) {
  switch (slot) {
  case SimplexRepeaterSlot::TS1:     setUInt8(0x00b7, 0x00); break;
  case SimplexRepeaterSlot::TS2:     setUInt8(0x00b7, 0x01); break;
  case SimplexRepeaterSlot::Channel: setUInt8(0x00b7, 0x02); break;
  }
}

unsigned int
DMR6X2UVCodeplug::GeneralSettingsElement::gpsRangingIntervall() const {
  return getUInt8(0x00b1);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setGPSRangingIntervall(unsigned int sec) {
  setUInt8(0x00b1, sec);
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::currentContactShown() const {
  return 0x01 == getUInt8(0x00b4);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::showCurrentContact(bool show) {
  setUInt8(0x00b4, show ? 0x01 : 0x00);
}

unsigned int
DMR6X2UVCodeplug::GeneralSettingsElement::keySoundVolume() const {
  return getUInt8(0x00b5);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setKeySoundVolume(unsigned int vol) {
  vol = std::min(15U, vol);
  setUInt8(0x00b5, vol);
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::professionalKeyLock() const {
  return getBit(0x00b6, 0);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableProfessionalKeyLock(bool enable) {
  setBit(0x00b6, 0, enable);
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::sideKeyLock() const {
  return getBit(0x00b6, 1);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableSideKeyLock(bool enable) {
  setBit(0x00b6, 1, enable);
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::keyboadLock() const {
  return getBit(0x00b6, 2);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableKeyboradLock(bool enable) {
  setBit(0x00b6, 2, enable);
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::knobLock() const {
  return getBit(0x00b6, 3);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableKnobLock(bool enable) {
  setBit(0x00b6, 3, enable);
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::lastCallShownOnStartup() const {
  return 0x01 == getUInt8(0x00b8);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableShowLastCallOnStartup(bool enable) {
  setUInt8(0x00b8, enable ? 0x01 : 0x00);
}

DMR6X2UVCodeplug::GeneralSettingsElement::SMSFormat
DMR6X2UVCodeplug::GeneralSettingsElement::smsFormat() const {
  return (SMSFormat) getUInt8(0x00b9);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setSMSFormat(SMSFormat format) {
  setUInt8(0x00b8, (uint8_t)format);
}

DMR6X2UVCodeplug::GeneralSettingsElement::GPSUnits
DMR6X2UVCodeplug::GeneralSettingsElement::gpsUnits() const {
  return (GPSUnits) getUInt8(0x00ba);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setGPSUnits(GPSUnits units) {
  setUInt8(0x00ba, (uint8_t) units);
}

QPair<double,double>
DMR6X2UVCodeplug::GeneralSettingsElement::vhfAutoRepeaterFrequencyRange() const {
  double min = double(getUInt32_le(0x00bc))/1.0e5; // Stored in 10Hz
  double max = double(getUInt32_le(0x00c0))/1.0e5;
  return QPair<double,double>(min,max);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setVHFAutoRepeaterFrequencyRange(double lower, double upper) {
  setUInt32_le(0x00bc, uint32_t(lower*1e5));
  setUInt32_le(0x00c0, uint32_t(upper*1e5));
}
QPair<double,double>
DMR6X2UVCodeplug::GeneralSettingsElement::uhfAutoRepeaterFrequencyRange() const {
  double min = double(getUInt32_le(0x00c4))/1.0e5; // Stored in 10Hz
  double max = double(getUInt32_le(0x00c8))/1.0e5;
  return QPair<double,double>(min,max);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setUHFAutoRepeaterFrequencyRange(double lower, double upper) {
  setUInt32_le(0x00c4, uint32_t(lower*1e5));
  setUInt32_le(0x00c8, uint32_t(upper*1e5));
}
AnytoneCodeplug::GeneralSettingsElement::AutoRepDir
DMR6X2UVCodeplug::GeneralSettingsElement::autoRepeaterDirectionB() const {
  return (AutoRepDir)getUInt8(0x00cc);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setAutoRepeaterDirectionB(AutoRepDir dir) {
  setUInt8(0x00cc, (uint8_t)dir);
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::addressBookIsSendWithOwnCode() const {
  return 0x01 == getUInt8(0x00cd);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableSendAddressbookWithOwnCode(bool enable) {
  setUInt8(0x00cd, enable ? 0x01 : 0x00);
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::defaultBootChannelEnabled() const {
  return 0x01 == getUInt8(0x00ce);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableDefaultBootChannel(bool enable) {
  setUInt8(0x00ce, enable ? 0x01 : 0x00);
}

unsigned int
DMR6X2UVCodeplug::GeneralSettingsElement::bootZoneAIndex() const {
  return getUInt8(0x00cf);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setBootZoneAIndex(unsigned int index) {
  setUInt8(0x00cf, index);
}
bool
DMR6X2UVCodeplug::GeneralSettingsElement::bootChannelAIsVFO() const {
  return 0xff == bootChannelAIndex();
}
unsigned int
DMR6X2UVCodeplug::GeneralSettingsElement::bootChannelAIndex() const {
  return getUInt8(0x00d1);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setBootChannelAIndex(unsigned int index) {
  return setUInt8(0x00d1, index);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setBootChannelAIsVFO() {
  setBootChannelAIndex(0xff);
}

unsigned int
DMR6X2UVCodeplug::GeneralSettingsElement::bootZoneBIndex() const {
  return getUInt8(0x00d0);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setBootZoneBIndex(unsigned int index) {
  setUInt8(0x00d0, index);
}
bool
DMR6X2UVCodeplug::GeneralSettingsElement::bootChannelBIsVFO() const {
  return 0xff == bootChannelBIndex();
}
unsigned int
DMR6X2UVCodeplug::GeneralSettingsElement::bootChannelBIndex() const {
  return getUInt8(0x00d2);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setBootChannelBIndex(unsigned int index) {
  return setUInt8(0x00d2, index);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setBootChannelBIsVFO() {
  setBootChannelBIndex(0xff);
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::lastCallerIsKept() const {
  return 0x01 == getUInt8(0x00d3);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::enableKeepLastCaller(bool enable) {
  setUInt8(0x00d3, enable ? 0x01 : 0x00);
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::rxBacklightIsAlwaysOn() const {
  return 0x00 == rxBacklightDuration();
}
unsigned int
DMR6X2UVCodeplug::GeneralSettingsElement::rxBacklightDuration() const {
  return getUInt8(0x00d4);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setRXBacklightDuration(unsigned int dur) {
  dur = std::max(30U, dur);
  setUInt8(0x00d4, dur);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setRXBacklightAlwaysOn() {
  setRXBacklightDuration(0);
}

DMR6X2UVCodeplug::GeneralSettingsElement::BackgroundColor
DMR6X2UVCodeplug::GeneralSettingsElement::backgroundColor() const {
  return (BackgroundColor) getUInt8(0x00d5);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setBackgroundColor(BackgroundColor color) {
  setUInt8(0x00d5, (uint8_t)color);
}

unsigned int
DMR6X2UVCodeplug::GeneralSettingsElement::manualDialedGroupCallHangTime() const {
  return getUInt8(0x00d6);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setManualDialedGroupCallHangTime(unsigned int dur) {
  setUInt8(0x00d6, dur);
}
unsigned int
DMR6X2UVCodeplug::GeneralSettingsElement::manualDialedPrivateCallHangTime() const {
  return getUInt8(0x00d7);
}
void
DMR6X2UVCodeplug::GeneralSettingsElement::setManualDialedPrivateCallHangTime(unsigned int dur) {
  setUInt8(0x00d7, dur);
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::fromConfig(const Flags &flags, Context &ctx)
{
  if (! AnytoneCodeplug::GeneralSettingsElement::fromConfig(flags, ctx))
    return false;
  // apply DMR-6X2UV specific settings.
  return true;
}

bool
DMR6X2UVCodeplug::GeneralSettingsElement::updateConfig(Context &ctx) {
  if (! AnytoneCodeplug::GeneralSettingsElement::updateConfig(ctx))
    return false;
  // Extract DMR-6X2UV specific settings.
  return true;
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
  : Codeplug::Element(ptr, 0x0030)
{
  // pass...
}

void
DMR6X2UVCodeplug::ExtendedSettingsElement::clear() {
  Codeplug::Element::clear();
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
 * Implementation of DMR6X2UVCodeplug
 * ********************************************************************************************* */
DMR6X2UVCodeplug::DMR6X2UVCodeplug(QObject *parent)
  : D868UVCodeplug(parent)
{
  // pass...
}
