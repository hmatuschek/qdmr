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



#define ADDR_DTMF_NUMBERS         0x02500500
#define DTMF_NUMBERS_SIZE         0x00000100

#define ADDR_HOTKEY               0x025C0000
#define HOTKEY_SIZE               0x00000860
#define STATUSMESSAGE_BITMAP      0x025C0B00
#define STATUSMESSAGE_BITMAP_SIZE 0x00000010

#define ADDR_OFFSET_FREQ          0x024C2000
#define NUM_OFFSET_FREQ                  250
#define OFFSET_FREQ_SIZE          0x000003F0

#define ADDR_ALARM_SETTING        0x024C1400
#define ALARM_SETTING_SIZE        0x00000020

#define ADDR_ALARM_SETTING_EXT    0x024c1440
#define ALARM_SETTING_EXT_SIZE    0x00000030

#define FMBC_BITMAP               0x02480210
#define FMBC_BITMAP_SIZE          0x00000020
#define ADDR_FMBC                 0x02480000
#define FMBC_SIZE                 0x00000200
#define ADDR_FMBC_VFO             0x02480200
#define FMBC_VFO_SIZE             0x00000010

#define FIVE_TONE_ID_BITMAP       0x024C0C80
#define FIVE_TONE_ID_BITMAP_SIZE  0x00000010
#define NUM_FIVE_TONE_IDS         100
#define ADDR_FIVE_TONE_ID_LIST    0x024C0000
#define FIVE_TONE_ID_SIZE         0x00000020
#define FIVE_TONE_ID_LIST_SIZE    0x00000c80
#define NUM_FIVE_TONE_FUNCTIONS   16
#define ADDR_FIVE_TONE_FUNCTIONS  0x024C0D00
#define FIVE_TONE_FUNCTION_SIZE   0x00000020
#define FIVE_TONE_FUNCTIONS_SIZE  0x00000200
#define ADDR_FIVE_TONE_SETTINGS   0x024C1000
#define FIVE_TONE_SETTINGS_SIZE   0x00000080

#define ADDR_DTMF_SETTINGS        0x024C1080
#define DTMF_SETTINGS_SIZE        0x00000050

#define NUM_TWO_TONE_IDS          24
#define TWO_TONE_IDS_BITMAP       0x024C1280
#define TWO_TONE_IDS_BITMAP_SIZE  0x00000010
#define ADDR_TWO_TONE_IDS         0x024C1100
#define TWO_TONE_ID_SIZE          0x00000010

#define NUM_TWO_TONE_FUNCTIONS    16
#define TWO_TONE_FUNCTIONS_BITMAP 0x024c2600
#define TWO_TONE_FUNC_BITMAP_SIZE 0x00000010
#define ADDR_TWO_TONE_FUNCTIONS   0x024c2400
#define TWO_TONE_FUNCTION_SIZE    0x00000020

#define ADDR_TWO_TONE_SETTINGS    0x024C1290
#define TWO_TONE_SETTINGS_SIZE    0x00000010

#define ADDR_DMR_ENCRYPTION_LIST  0x024C1700
#define DMR_ENCRYPTION_LIST_SIZE  0x00000040
#define ADDR_DMR_ENCRYPTION_KEYS  0x024C1800
#define DMR_ENCRYPTION_KEYS_SIZE  0x00000500

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

AnytoneDisplaySettingsExtension::Color
D868UVCodeplug::GeneralSettingsElement::callDisplayColor() const {
  return (AnytoneDisplaySettingsExtension::Color)getUInt8(0x00b0);
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
  setUInt8(0x00b0, (unsigned)color);
}

unsigned
D868UVCodeplug::GeneralSettingsElement::gpsUpdatePeriod() const {
  return getUInt8(0x00b1);
}
void
D868UVCodeplug::GeneralSettingsElement::setGPSUpdatePeriod(unsigned sec) {
  setUInt8(0x00b1, sec);
}

bool
D868UVCodeplug::GeneralSettingsElement::showZoneAndContact() const {
  return getUInt8(0x00b2);
}
void
D868UVCodeplug::GeneralSettingsElement::enableShowZoneAndContact(bool enable) {
  setUInt8(0x00b2, (enable ? 0x01 : 0x00));
}

bool
D868UVCodeplug::GeneralSettingsElement::keyToneLevelAdjustable() const {
  return 0 == keyToneLevel();
}
unsigned
D868UVCodeplug::GeneralSettingsElement::keyToneLevel() const {
  return ((unsigned)getUInt8(0x00b3))*10/15;
}
void
D868UVCodeplug::GeneralSettingsElement::setKeyToneLevel(unsigned level) {
  setUInt8(0x00b3, level*10/15);
}
void
D868UVCodeplug::GeneralSettingsElement::setKeyToneLevelAdjustable() {
  setUInt8(0x00b3, 0);
}

bool
D868UVCodeplug::GeneralSettingsElement::gpsUnitsImperial() const {
  return getUInt8(0x00b4);
}
void
D868UVCodeplug::GeneralSettingsElement::enableGPSUnitsImperial(bool enable) {
  setUInt8(0x00b4, (enable ? 0x01 : 0x00));
}

bool
D868UVCodeplug::GeneralSettingsElement::knobLock() const {
  return getBit(0x00b5, 0);
}
void
D868UVCodeplug::GeneralSettingsElement::enableKnobLock(bool enable) {
  setBit(0x00b5, 0, enable);
}
bool
D868UVCodeplug::GeneralSettingsElement::keypadLock() const {
  return getBit(0x00b5, 1);
}
void
D868UVCodeplug::GeneralSettingsElement::enableKeypadLock(bool enable) {
  setBit(0x00b5, 1, enable);
}
bool
D868UVCodeplug::GeneralSettingsElement::sidekeysLock() const {
  return getBit(0x00b5, 3);
}
void
D868UVCodeplug::GeneralSettingsElement::enableSidekeysLock(bool enable) {
  setBit(0x00b5, 3, enable);
}
bool
D868UVCodeplug::GeneralSettingsElement::keyLockForced() const {
  return getBit(0x00b5, 4);
}
void
D868UVCodeplug::GeneralSettingsElement::enableKeyLockForced(bool enable) {
  setBit(0x00b5, 4, enable);
}

bool
D868UVCodeplug::GeneralSettingsElement::showLastHeard() const {
  return getUInt8(0x00b6);
}
void
D868UVCodeplug::GeneralSettingsElement::enableShowLastHeard(bool enable) {
  setUInt8(0x00b6, (enable ? 0x01 : 0x00));
}

Frequency D868UVCodeplug::GeneralSettingsElement::autoRepeaterMinFrequencyVHF() const {
  return Frequency::fromHz(getUInt32_le(0x00b8)*10);
}
void
D868UVCodeplug::GeneralSettingsElement::setAutoRepeaterMinFrequencyVHF(Frequency freq) {
  setUInt32_le(0x00b8, freq.inHz()/10);
}
Frequency
D868UVCodeplug::GeneralSettingsElement::autoRepeaterMaxFrequencyVHF() const {
  return Frequency::fromHz(getUInt32_le(0x00bc)*10);
}
void
D868UVCodeplug::GeneralSettingsElement::setAutoRepeaterMaxFrequencyVHF(Frequency freq) {
  setUInt32_le(0x00bc, freq.inHz()/10);
}

Frequency
D868UVCodeplug::GeneralSettingsElement::autoRepeaterMinFrequencyUHF() const {
  return Frequency::fromHz(getUInt32_le(0x00c0)*10);
}
void
D868UVCodeplug::GeneralSettingsElement::setAutoRepeaterMinFrequencyUHF(Frequency freq) {
  setUInt32_le(0x00c0, freq.inHz()/10);
}
Frequency
D868UVCodeplug::GeneralSettingsElement::autoRepeaterMaxFrequencyUHF() const {
  return Frequency::fromHz(getUInt32_le(0x00c4)*10);
}
void
D868UVCodeplug::GeneralSettingsElement::setAutoRepeaterMaxFrequencyUHF(Frequency freq) {
  setUInt32_le(0x00c4, freq.inHz()/10);
}

AnytoneAutoRepeaterSettingsExtension::Direction
D868UVCodeplug::GeneralSettingsElement::autoRepeaterDirectionB() const {
  return (AnytoneAutoRepeaterSettingsExtension::Direction)getUInt8(0x00c8);
}
void
D868UVCodeplug::GeneralSettingsElement::setAutoRepeaterDirectionB(AnytoneAutoRepeaterSettingsExtension::Direction dir) {
  setUInt8(0x00c8, (unsigned)dir);
}

bool
D868UVCodeplug::GeneralSettingsElement::defaultChannel() const {
  return getUInt8(0x00ca);
}
void
D868UVCodeplug::GeneralSettingsElement::enableDefaultChannel(bool enable) {
  setUInt8(0x00ca, (enable ? 0x01 : 0x00));
}

unsigned
D868UVCodeplug::GeneralSettingsElement::defaultZoneIndexA() const {
  return getUInt8(0x00cb);
}
void
D868UVCodeplug::GeneralSettingsElement::setDefaultZoneIndexA(unsigned idx) {
  setUInt8(0x00cb, idx);
}

unsigned
D868UVCodeplug::GeneralSettingsElement::defaultZoneIndexB() const {
  return getUInt8(0x00cc);
}
void
D868UVCodeplug::GeneralSettingsElement::setDefaultZoneIndexB(unsigned idx) {
  setUInt8(0x00cc, idx);
}

bool
D868UVCodeplug::GeneralSettingsElement::defaultChannelAIsVFO() const {
  return 0xff == defaultChannelAIndex();
}
unsigned
D868UVCodeplug::GeneralSettingsElement::defaultChannelAIndex() const {
  return getUInt8(0x00cd);
}
void
D868UVCodeplug::GeneralSettingsElement::setDefaultChannelAIndex(unsigned idx) {
  setUInt8(0x00cd, idx);
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
  return getUInt8(0x00ce);
}
void
D868UVCodeplug::GeneralSettingsElement::setDefaultChannelBIndex(unsigned idx) {
  setUInt8(0x00ce, idx);
}
void
D868UVCodeplug::GeneralSettingsElement::setDefaultChannelBToVFO() {
  setDefaultChannelBIndex(0xff);
}

bool
D868UVCodeplug::GeneralSettingsElement::keepLastCaller() const {
  return getUInt8(0x00cf);
}
void
D868UVCodeplug::GeneralSettingsElement::enableKeepLastCaller(bool enable) {
  setUInt8(0x00cf, (enable ? 0x01 : 0x00));
}

bool
D868UVCodeplug::GeneralSettingsElement::fromConfig(const Flags &flags, Context &ctx) {
  if (! AnytoneCodeplug::GeneralSettingsElement::fromConfig(flags, ctx))
    return false;

  setGPSUpdatePeriod(0x05);
  // Set measurement system based on system locale (0x00==Metric)
  enableGPSUnitsImperial(QLocale::ImperialSystem == QLocale::system().measurementSystem());

  if (AnytoneSettingsExtension *ext = ctx.config()->settings()->anytoneExtension()) {
    // Encode key settings
    enableKnobLock(ext->keySettings()->knobLockEnabled());
    enableKeypadLock(ext->keySettings()->keypadLockEnabled());
    enableSidekeysLock(ext->keySettings()->sideKeysLockEnabled());
    enableKeyLockForced(ext->keySettings()->forcedKeyLockEnabled());

    // Encode tone settings
    setKeyToneLevel(ext->toneSettings()->keyToneLevel());

    // Encode menu settings
    setMenuExitTime(ext->menuSettings()->duration());

    // Encode display settings
    setCallDisplayColor(ext->displaySettings()->callColor());
    enableShowZoneAndContact(ext->displaySettings()->showZoneAndContactEnabled());

    // Encode auto-repeater settings
    setAutoRepeaterDirectionB(ext->autoRepeaterSettings()->directionB());
    setAutoRepeaterMinFrequencyVHF(ext->autoRepeaterSettings()->vhfMin());
    setAutoRepeaterMaxFrequencyVHF(ext->autoRepeaterSettings()->vhfMax());
    setAutoRepeaterMinFrequencyUHF(ext->autoRepeaterSettings()->uhfMin());
    setAutoRepeaterMaxFrequencyUHF(ext->autoRepeaterSettings()->uhfMax());

    // Encode other settings
    enableGPSUnitsImperial(AnytoneSettingsExtension::Units::Archaic == ext->units());
    enableKeepLastCaller(ext->keepLastCallerEnabled());
  }

  return true;
}

bool
D868UVCodeplug::GeneralSettingsElement::updateConfig(Context &ctx) {
  if (! AnytoneCodeplug::GeneralSettingsElement::updateConfig(ctx))
    return false;

  // Get or add settings extension
  AnytoneSettingsExtension *ext = nullptr;
  if (ctx.config()->settings()->anytoneExtension()) {
    ext = ctx.config()->settings()->anytoneExtension();
  } else {
    ext = new AnytoneSettingsExtension();
    ctx.config()->settings()->setAnytoneExtension(ext);
  }

  // Decode key settings
  ext->keySettings()->enableKnobLock(this->knobLock());
  ext->keySettings()->enableKeypadLock(this->keypadLock());
  ext->keySettings()->enableSideKeysLock(this->sidekeysLock());
  ext->keySettings()->enableForcedKeyLock(this->keyLockForced());

  // Decode tone settings
  ext->toneSettings()->setKeyToneLevel(keyToneLevel());

  // Menu settings
  ext->menuSettings()->setDuration(this->menuExitTime());

  // Decode display settings
  ext->displaySettings()->setCallColor(this->callDisplayColor());
  ext->displaySettings()->enableShowZoneAndContact(this->showZoneAndContact());

  // Decode auto-repeater settings
  ext->autoRepeaterSettings()->setDirectionB(autoRepeaterDirectionB());
  ext->autoRepeaterSettings()->setVHFMin(this->autoRepeaterMinFrequencyVHF());
  ext->autoRepeaterSettings()->setVHFMax(this->autoRepeaterMaxFrequencyVHF());
  ext->autoRepeaterSettings()->setUHFMin(this->autoRepeaterMinFrequencyUHF());
  ext->autoRepeaterSettings()->setUHFMax(this->autoRepeaterMaxFrequencyUHF());

  // Decode other settings
  ext->setUnits(this->gpsUnitsImperial() ? AnytoneSettingsExtension::Units::Archaic :
                                           AnytoneSettingsExtension::Units::Metric);
  ext->enableKeepLastCaller(this->keepLastCaller());

  return true;
}

bool
D868UVCodeplug::GeneralSettingsElement::linkSettings(RadioSettings *settings, Context &ctx, const ErrorStack &err) {
  // Get or add settings extension
  AnytoneSettingsExtension *ext = nullptr;
  if (settings->anytoneExtension()) {
    ext = settings->anytoneExtension();
  } else {
    ext = new AnytoneSettingsExtension();
    settings->setAnytoneExtension(ext);
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

  image(0).addElement(ADDR_DMR_ENCRYPTION_LIST, DMR_ENCRYPTION_LIST_SIZE);
  image(0).addElement(ADDR_DMR_ENCRYPTION_KEYS, DMR_ENCRYPTION_KEYS_SIZE);
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
  image(0).addElement(STATUSMESSAGE_BITMAP, STATUSMESSAGE_BITMAP_SIZE);
  // FM Broadcast bitmaps
  image(0).addElement(FMBC_BITMAP, FMBC_BITMAP_SIZE);
  // 5-Tone function bitmaps
  image(0).addElement(FIVE_TONE_ID_BITMAP, FIVE_TONE_ID_BITMAP_SIZE);
  // 2-Tone function bitmaps
  image(0).addElement(TWO_TONE_IDS_BITMAP, TWO_TONE_IDS_BITMAP_SIZE);
  image(0).addElement(TWO_TONE_FUNCTIONS_BITMAP, TWO_TONE_FUNC_BITMAP_SIZE);

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
    zone_bitmap.setEncoded(z, true);
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
    image(0).addElement(Offset::zoneChannels()+i*Offset::betweenZoneChannels(), Offset::betweenZoneChannels());
    image(0).addElement(Offset::zoneNames()+i*Offset::betweenZoneNames(), Limit::zoneNameLength());
  }
}

bool
D868UVCodeplug::encodeZones(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)

  // Encode zones
  unsigned zidx = 0;
  for (int i=0; i<ctx.config()->zones()->count(); i++) {
    // Clear name and channel list
    uint8_t  *name     = (uint8_t *)data(Offset::zoneChannels() + zidx*Offset::betweenZoneChannels());
    uint16_t *channels = (uint16_t *)data(Offset::zoneNames() + zidx*Offset::betweenZoneNames());
    memset(name, 0, Limit::zoneNameLength());
    memset(channels, 0xff, Offset::betweenZoneChannels());
    if (ctx.config()->zones()->zone(i)->B()->count())
      encode_ascii(name, ctx.config()->zones()->zone(i)->name()+" A", 16, 0);
    else
      encode_ascii(name, ctx.config()->zones()->zone(i)->name(), 16, 0);
    // Handle list A
    for (int j=0; j<ctx.config()->zones()->zone(i)->A()->count(); j++) {
      channels[j] = qToLittleEndian(ctx.index(ctx.config()->zones()->zone(i)->A()->get(j)));
    }

    if (! encodeZone(zidx, ctx.config()->zones()->zone(i), false, flags, ctx, err))
      return false;
    zidx++;

    if (! ctx.config()->zones()->zone(i)->B()->count())
      continue;

    // Process list B if present
    name     = (uint8_t *)data(Offset::zoneChannels() + zidx*Offset::betweenZoneChannels());
    channels = (uint16_t *)data(Offset::zoneNames() + zidx*Offset::betweenZoneNames());
    memset(name, 0, Offset::betweenZoneChannels());
    memset(channels, 0xff, Limit::zoneNameLength());
    encode_ascii(name, ctx.config()->zones()->zone(i)->name()+" B", 16, 0);
    // Handle list B
    for (int j=0; j<ctx.config()->zones()->zone(i)->B()->count(); j++) {
      channels[j] = qToLittleEndian(ctx.index(ctx.config()->zones()->zone(i)->B()->get(j)));
    }

    if (! encodeZone(zidx, ctx.config()->zones()->zone(i), true, flags, ctx, err))
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
    QString zonename = decode_ascii(data(Offset::zoneNames()+i*Offset::betweenZoneNames()),
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
    QString zonename = decode_ascii(data(Offset::zoneNames()+i*Offset::betweenZoneNames()),
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
  image(0).addElement(Offset::zoneChannels(), Offset::betweenZoneChannels());
}


void
D868UVCodeplug::allocateDTMFNumbers() {
  image(0).addElement(ADDR_DTMF_NUMBERS, DTMF_NUMBERS_SIZE);
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
    uint8_t bank = i/Limit::numMessagePerBank();
    if (! messages_bytemap.isEncoded(i))
      continue;
    message_count++;
    uint32_t addr = Offset::messageBanks() + bank*Offset::betweenMessageBanks();
    if (!isAllocated(addr, 0)) {
      image(0).addElement(addr, Limit::numMessagePerBank()*MessageElement::size());
    }
  }
  if (message_count) {
    image(0).addElement(Offset::messageIndex(), 0x10*message_count);
  }
}

void
D868UVCodeplug::allocateHotKeySettings() {
  // Allocate Hot Keys
  image(0).addElement(ADDR_HOTKEY, HOTKEY_SIZE);
}

void
D868UVCodeplug::allocateRepeaterOffsetFrequencies() {
  // Offset frequencies
  image(0).addElement(ADDR_OFFSET_FREQ, OFFSET_FREQ_SIZE);
}

bool
D868UVCodeplug::encodeRepeaterOffsetFrequencies(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err);

  // If no AnyTone extension is present -> leave untouched.
  if (! ctx.config()->settings()->anytoneExtension())
    return true;

  for (int i=0; i<NUM_OFFSET_FREQ; i++) {
    uint32_t *offsetFreqPtr = (uint32_t *)data(ADDR_OFFSET_FREQ + i*sizeof(uint32_t));
    if (i < (int)ctx.count<AnytoneAutoRepeaterOffset>()) {
      (*offsetFreqPtr) = qToLittleEndian(ctx.get<AnytoneAutoRepeaterOffset>(i)->offset().inHz()/10);
    } else {
      *offsetFreqPtr = 0;
    }
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
  for (int i=0; i<NUM_OFFSET_FREQ; i++) {
    uint32_t *offsetFreqPtr = (uint32_t *)data(ADDR_OFFSET_FREQ + i*sizeof(uint32_t));
    if (0 == *offsetFreqPtr)
      continue;
    AnytoneAutoRepeaterOffset *offset = new AnytoneAutoRepeaterOffset();
    offset->setOffset(Frequency::fromHz(qFromLittleEndian(*offsetFreqPtr)*10ULL));
    ext->autoRepeaterSettings()->offsets()->add(offset);
    ctx.add(offset, i);
  }

  return true;
}


void
D868UVCodeplug::allocateAlarmSettings() {
  // Alarm settings
  image(0).addElement(ADDR_ALARM_SETTING, ALARM_SETTING_SIZE);
  image(0).addElement(ADDR_ALARM_SETTING_EXT, ALARM_SETTING_EXT_SIZE);
}

void
D868UVCodeplug::allocateFMBroadcastSettings() {
  // FM broad-cast settings
  image(0).addElement(ADDR_FMBC, FMBC_SIZE+FMBC_VFO_SIZE);
}

void
D868UVCodeplug::allocate5ToneIDs() {
  // Allocate 5-tone functions
  uint8_t *bitmap = data(FIVE_TONE_ID_BITMAP);
  for (uint8_t i=0; i<NUM_FIVE_TONE_IDS; i++) {
    uint16_t  bit = i%8, byte = i/8;
    if (0 == (bitmap[byte] & (1<<bit)))
      continue;
    image(0).addElement(ADDR_FIVE_TONE_ID_LIST + i*FIVE_TONE_ID_SIZE, FIVE_TONE_ID_SIZE);
  }
}

void
D868UVCodeplug::allocate5ToneFunctions() {
  image(0).addElement(ADDR_FIVE_TONE_FUNCTIONS, FIVE_TONE_FUNCTIONS_SIZE);
}

void
D868UVCodeplug::allocate5ToneSettings() {
  image(0).addElement(ADDR_FIVE_TONE_SETTINGS, FIVE_TONE_SETTINGS_SIZE);
}

void
D868UVCodeplug::allocate2ToneIDs() {
  // Allocate 2-tone encoding
  uint8_t *enc_bitmap = data(TWO_TONE_IDS_BITMAP);
  for (uint8_t i=0; i<NUM_TWO_TONE_IDS; i++) {
    uint16_t  bit = i%8, byte = i/8;
    if (0 == (enc_bitmap[byte] & (1<<bit)))
      continue;
    image(0).addElement(ADDR_TWO_TONE_IDS + i*TWO_TONE_ID_SIZE, TWO_TONE_ID_SIZE);
  }
}


void
D868UVCodeplug::allocate2ToneFunctions() {
  // Allocate 2-tone decoding
  uint8_t *dec_bitmap = data(TWO_TONE_FUNCTIONS_BITMAP);
  for (uint8_t i=0; i<NUM_TWO_TONE_FUNCTIONS; i++) {
    uint16_t  bit = i%8, byte = i/8;
    if (0 == (dec_bitmap[byte] & (1<<bit)))
      continue;
    image(0).addElement(ADDR_TWO_TONE_FUNCTIONS + i*TWO_TONE_FUNCTION_SIZE, TWO_TONE_FUNCTION_SIZE);
  }
}

void
D868UVCodeplug::allocate2ToneSettings() {
  image(0).addElement(ADDR_TWO_TONE_SETTINGS, TWO_TONE_SETTINGS_SIZE);
}


void
D868UVCodeplug::allocateDTMFSettings() {
  image(0).addElement(ADDR_DTMF_SETTINGS, DTMF_SETTINGS_SIZE);
}
