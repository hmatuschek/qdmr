#include "radioddity_extensions.hh"

/* ********************************************************************************************* *
 * Implementation of RadioddityButtonSettingsExtension
 * ********************************************************************************************* */
RadioddityButtonSettingsExtension::RadioddityButtonSettingsExtension(QObject *parent)
  : ConfigItem(parent), _longPressDuration(Interval::fromMilliseconds(1000)),
    _funcKey1Short(Function::ZoneSelect), _funcKey1Long(Function::ToggleFMRadio),
    _funcKey2Short(Function::ToggleMonitor), _funcKey2Long(Function::ToggleFlashLight),
    _funcKey3Short(Function::BatteryIndicator), _funcKey3Long(Function::ToggleVox)
{
  // pass...
}

ConfigItem *
RadioddityButtonSettingsExtension::clone() const {
  ConfigItem *clone = new RadioddityButtonSettingsExtension();
  if (! clone->copy(*this)) {
    delete clone;
    return nullptr;
  }
  return clone;
}

Interval
RadioddityButtonSettingsExtension::longPressDuration() const {
  return _longPressDuration;
}
void
RadioddityButtonSettingsExtension::setLongPressDuration(Interval interval) {
  if (interval == _longPressDuration)
    return;
  _longPressDuration = interval;
  emit modified(this);
}

RadioddityButtonSettingsExtension::Function
RadioddityButtonSettingsExtension::funcKey1Short() const {
  return _funcKey1Short;
}
void
RadioddityButtonSettingsExtension::setFuncKey1Short(Function func) {
  if (func == _funcKey1Short)
    return;
  _funcKey1Short = func;
  emit modified(this);
}
RadioddityButtonSettingsExtension::Function
RadioddityButtonSettingsExtension::funcKey1Long() const {
  return _funcKey1Long;
}
void
RadioddityButtonSettingsExtension::setFuncKey1Long(Function func) {
  if (func == _funcKey1Long)
    return;
  _funcKey1Long = func;
  emit modified(this);
}

RadioddityButtonSettingsExtension::Function
RadioddityButtonSettingsExtension::funcKey2Short() const {
  return _funcKey2Short;
}
void
RadioddityButtonSettingsExtension::setFuncKey2Short(Function func) {
  if (func == _funcKey2Short)
    return;
  _funcKey2Short = func;
  emit modified(this);
}
RadioddityButtonSettingsExtension::Function
RadioddityButtonSettingsExtension::funcKey2Long() const {
  return _funcKey2Long;
}
void
RadioddityButtonSettingsExtension::setFuncKey2Long(Function func) {
  if (func == _funcKey2Long)
    return;
  _funcKey2Long = func;
  emit modified(this);
}

RadioddityButtonSettingsExtension::Function
RadioddityButtonSettingsExtension::funcKey3Short() const {
  return _funcKey3Short;
}
void
RadioddityButtonSettingsExtension::setFuncKey3Short(Function func) {
  if (func == _funcKey3Short)
    return;
  _funcKey3Short = func;
  emit modified(this);
}
RadioddityButtonSettingsExtension::Function
RadioddityButtonSettingsExtension::funcKey3Long() const {
  return _funcKey3Long;
}
void
RadioddityButtonSettingsExtension::setFuncKey3Long(Function func) {
  if (func == _funcKey3Long)
    return;
  _funcKey3Long = func;
  emit modified(this);
}


/* ********************************************************************************************* *
 * Implementation of RadioddityToneSettingsExtension
 * ********************************************************************************************* */
RadioddityToneSettingsExtension::RadioddityToneSettingsExtension(QObject *parent)
  : ConfigItem(parent), _lowBatteryWarn(true), _lowBatteryWarnInterval(Interval::fromSeconds(30)),
    _lowBatteryWarnVolume(5), _callAlertDuration(Interval::fromSeconds(120)), _resetTone(false),
    _unknownNumberTone(false), _artsToneMode(ARTSTone::Once), _digitalTalkPermitTone(false),
    _analogTalkPermitTone(false), _selftestTone(true), _channelFreeIndicationTone(false),
    _disableAllTones(false), _txExitTone(false), _keyTone(false), _keyToneVolume(5), _fmMicGain(5)
{
  // pass...
}

ConfigItem *
RadioddityToneSettingsExtension::clone() const {
  RadioddityToneSettingsExtension *ext = new RadioddityToneSettingsExtension();
  if (! ext->copy(*this)) {
    ext->deleteLater();
    return nullptr;
  }
  return ext;
}

bool
RadioddityToneSettingsExtension::lowBatteryWarn() const {
  return _lowBatteryWarn;
}
void
RadioddityToneSettingsExtension::enableLowBatteryWarn(bool enable) {
  if (enable == _lowBatteryWarn)
    return;
  _lowBatteryWarn = enable;
  emit modified(this);
}

Interval
RadioddityToneSettingsExtension::lowBatteryWarnInterval() const {
  return _lowBatteryWarnInterval;
}
void
RadioddityToneSettingsExtension::setLowBatteryWarnInterval(Interval sec) {
  if (_lowBatteryWarnInterval == sec)
    return;
  _lowBatteryWarnInterval = sec;
  emit modified(this);
}

unsigned int
RadioddityToneSettingsExtension::lowBatteryWarnVolume() const {
  return _lowBatteryWarnVolume;
}
void
RadioddityToneSettingsExtension::setLowBatteryWarnVolume(unsigned int volume) {
  volume = std::min(10U, std::max(1U, volume));
  if (volume == _lowBatteryWarnVolume)
    return;
  _lowBatteryWarnVolume = volume;
  emit modified(this);
}

Interval
RadioddityToneSettingsExtension::callAlertDuration() const {
  return _callAlertDuration;
}
void
RadioddityToneSettingsExtension::setCallAlertDuration(Interval sec) {
  if (_callAlertDuration == sec)
    return;
  _callAlertDuration = sec;
  emit modified(this);
}

bool
RadioddityToneSettingsExtension::resetTone() const {
  return _resetTone;
}
void
RadioddityToneSettingsExtension::enableResetTone(bool enable) {
  if (_resetTone == enable)
    return;
  _resetTone = enable;
  emit modified(this);
}

bool
RadioddityToneSettingsExtension::unknownNumberTone() const {
  return _unknownNumberTone;
}
void
RadioddityToneSettingsExtension::enableUnknownNumberTone(bool enable) {
  if (_unknownNumberTone == enable)
    return;
  _unknownNumberTone = enable;
  emit modified(this);
}

RadioddityToneSettingsExtension::ARTSTone
RadioddityToneSettingsExtension::artsToneMode() const {
  return _artsToneMode;
}
void
RadioddityToneSettingsExtension::setARTSToneMode(ARTSTone mode) {
  if (_artsToneMode == mode)
    return;
  _artsToneMode = mode;
  emit modified(this);
}

bool
RadioddityToneSettingsExtension::digitalTalkPermitTone() const {
  return _digitalTalkPermitTone;
}
void
RadioddityToneSettingsExtension::enableDigitalTalkPermitTone(bool enable) {
  if (_digitalTalkPermitTone == enable)
    return;
  _digitalTalkPermitTone = enable;
  emit modified(this);
}

bool
RadioddityToneSettingsExtension::analogTalkPermitTone() const {
  return _analogTalkPermitTone;
}
void
RadioddityToneSettingsExtension::enableAnalogTalkPermitTone(bool enable) {
  if (_analogTalkPermitTone == enable)
    return;
  _analogTalkPermitTone = enable;
  emit modified(this);
}

bool
RadioddityToneSettingsExtension::selftestTone() const {
  return _selftestTone;
}
void
RadioddityToneSettingsExtension::enableSelftestTone(bool enable) {
  if (_selftestTone == enable)
    return;
  _selftestTone = enable;
  emit modified(this);
}

bool
RadioddityToneSettingsExtension::channelFreeIndicationTone() const {
  return _channelFreeIndicationTone;
}
void
RadioddityToneSettingsExtension::enableChannelFreeIndicationTone(bool enable) {
  if (_channelFreeIndicationTone == enable)
    return;
  _channelFreeIndicationTone = enable;
  emit modified(this);
}

bool
RadioddityToneSettingsExtension::allTonesDisabled() const {
  return _disableAllTones;
}
void
RadioddityToneSettingsExtension::disableAllTones(bool disable) {
  if (_disableAllTones == disable)
    return;
  _disableAllTones = disable;
  emit modified(this);
}

bool
RadioddityToneSettingsExtension::txExitTone() const {
  return _txExitTone;
}
void
RadioddityToneSettingsExtension::enableTXExitTone(bool enable) {
  if (_txExitTone == enable)
    return;
  _txExitTone = enable;
  emit modified(this);
}

bool
RadioddityToneSettingsExtension::keyTone() const {
  return _keyTone;
}
void
RadioddityToneSettingsExtension::enableKeyTone(bool enable) {
  if (enable == _keyTone)
    return;
  _keyTone = enable;
  emit modified(this);
}

unsigned int
RadioddityToneSettingsExtension::keyToneVolume() const {
  return _keyToneVolume;
}
void
RadioddityToneSettingsExtension::setKeyToneVolume(unsigned int volume) {
  volume = std::min(10U, std::max(1U, volume));
  if (volume == _keyToneVolume)
    return;
  _keyToneVolume = volume;
  emit modified(this);
}

unsigned int
RadioddityToneSettingsExtension::fmMicGain() const {
  return _fmMicGain;
}
void
RadioddityToneSettingsExtension::setFMMicGain(unsigned int gain) {
  gain = std::min(10U, std::max(1U, gain));
  if (gain == _fmMicGain)
    return;
  _fmMicGain = gain;
  emit modified(this);
}


/* ********************************************************************************************* *
 * Implementation of RadioddityBootSettingsExtension
 * ********************************************************************************************* */
RadioddityBootSettingsExtension::RadioddityBootSettingsExtension(QObject *parent)
  : ConfigItem(parent), _displayMode(DisplayMode::Text), _bootPasswd(), _progPasswd()
{
  // pass...
}

ConfigItem *
RadioddityBootSettingsExtension::clone() const {
  auto *ext = new RadioddityBootSettingsExtension();
  if (! ext->copy(*this)) {
    ext->deleteLater();
    return nullptr;
  }
  return ext;
}

RadioddityBootSettingsExtension::DisplayMode
RadioddityBootSettingsExtension::display() const {
  return _displayMode;
}
void
RadioddityBootSettingsExtension::setDisplay(DisplayMode mode) {
  if (_displayMode == mode)
    return;
  _displayMode = mode;
  emit modified(this);
}


const QString &
RadioddityBootSettingsExtension::bootPassword() const {
  return _bootPasswd;
}
void
RadioddityBootSettingsExtension::setBootPassword(const QString &pwd) {
  if (_bootPasswd == pwd)
    return;
  _bootPasswd = pwd;
  emit modified(this);
}

const QString &
RadioddityBootSettingsExtension::progPassword() const {
  return _progPasswd;
}
void
RadioddityBootSettingsExtension::setProgPassword(const QString &pwd) {
  if (_progPasswd == pwd)
    return;
  _progPasswd = pwd;
  emit modified(this);
}


/* ********************************************************************************************* *
 * Implementation of RadiodditySettingsExtension
 * ********************************************************************************************* */
RadiodditySettingsExtension::RadiodditySettingsExtension(QObject *parent)
  : ConfigExtension(parent), _monitorType(MonitorType::Silent),
    _loneWorkerResponseTime(Interval::fromMinutes(1)),
    _loneWorkerReminderPeriod(Interval::fromSeconds(10)),
    _groupCallHangTime(Interval::fromMilliseconds(3000)),
    _privateCallHangTime(Interval::fromMilliseconds(3000)), _downChannelModeVFO(false),
    _upChannelModeVFO(false), _powerSaveMode(true), _wakeupPreamble(true),
    _preambleDuration(Interval::fromMilliseconds(360)), _powerSaveDelay(Interval::fromSeconds(10)),
    _disableAllLEDs(false), _quickKeyOverrideInhibited(false), _txOnActiveChannel(true),
    _scanMode(ScanMode::Time), _repeaterEndDelay(), _repeaterSTE(), _txInterrupt(false),
    _language(Language::English),
    _buttonSettings(new RadioddityButtonSettingsExtension(this)),
    _toneSettings(new RadioddityToneSettingsExtension(this)),
    _bootSettings(new RadioddityBootSettingsExtension(this))
{
  // pass...
}

ConfigItem *
RadiodditySettingsExtension::clone() const {
  RadiodditySettingsExtension *ext = new RadiodditySettingsExtension();
  if (! ext->copy(*this)) {
    ext->deleteLater();
    return nullptr;
  }
  return ext;
}

RadiodditySettingsExtension::MonitorType
RadiodditySettingsExtension::monitorType() const {
  return _monitorType;
}
void
RadiodditySettingsExtension::setMonitorType(MonitorType type) {
  if (_monitorType == type)
    return;
  _monitorType = type;
  emit modified(this);
}

Interval
RadiodditySettingsExtension::loneWorkerResponseTime() const {
  return _loneWorkerResponseTime;
}
void
RadiodditySettingsExtension::setLoneWorkerResponseTime(Interval min) {
  if (_loneWorkerResponseTime == min)
    return;
  _loneWorkerResponseTime = min;
  emit modified(this);
}

Interval
RadiodditySettingsExtension::loneWorkerReminderPeriod() const {
  return _loneWorkerReminderPeriod;
}
void
RadiodditySettingsExtension::setLoneWorkerReminderPeriod(Interval sec) {
  if (_loneWorkerReminderPeriod == sec)
    return;
  _loneWorkerReminderPeriod = sec;
  emit modified(this);
}

Interval
RadiodditySettingsExtension::groupCallHangTime() const {
  return _groupCallHangTime;
}
void
RadiodditySettingsExtension::setGroupCallHangTime(Interval ms) {
  if (_groupCallHangTime == ms)
    return;
  _groupCallHangTime = ms;
  emit modified(this);
}

Interval
RadiodditySettingsExtension::privateCallHangTime() const {
  return _privateCallHangTime;
}
void
RadiodditySettingsExtension::setPrivateCallHangTime(Interval ms) {
  if (_privateCallHangTime == ms)
    return;
  _privateCallHangTime = ms;
  emit modified(this);
}

bool
RadiodditySettingsExtension::downChannelModeVFO() const {
  return _downChannelModeVFO;
}
void
RadiodditySettingsExtension::enableDownChannelModeVFO(bool enable) {
  if (_downChannelModeVFO == enable)
    return;
  _downChannelModeVFO = enable;
  emit modified(this);
}

bool
RadiodditySettingsExtension::upChannelModeVFO() const {
  return _upChannelModeVFO;
}
void
RadiodditySettingsExtension::enableUpChannelModeVFO(bool enable) {
  if (_upChannelModeVFO == enable)
    return;
  _upChannelModeVFO = enable;
  emit modified(this);
}

bool
RadiodditySettingsExtension::powerSaveMode() const {
  return _powerSaveMode;
}
void
RadiodditySettingsExtension::enablePowerSaveMode(bool enable) {
  if (_powerSaveMode == enable)
    return;
  _powerSaveMode = enable;
  emit modified(this);
}

bool
RadiodditySettingsExtension::wakeupPreamble() const {
  return _wakeupPreamble;
}
void
RadiodditySettingsExtension::enableWakeupPreamble(bool enable) {
  if (_wakeupPreamble == enable)
    return;
  _wakeupPreamble = enable;
  emit modified(this);
}

Interval
RadiodditySettingsExtension::preambleDuration() const {
  return _preambleDuration;
}
void
RadiodditySettingsExtension::setPreambleDuration(Interval ms) {
  if (_preambleDuration == ms)
    return;
  _preambleDuration = ms;
  emit modified(this);
}

Interval
RadiodditySettingsExtension::powerSaveDelay() const {
  return _powerSaveDelay;
}
void
RadiodditySettingsExtension::setPowerSaveDelay(Interval interv) {
  if (interv == _powerSaveDelay)
    return;
  _powerSaveDelay = interv;
  emit modified(this);
}

bool
RadiodditySettingsExtension::allLEDsDisabled() const {
  return _disableAllLEDs;
}
void
RadiodditySettingsExtension::disableAllLEDs(bool disable) {
  if (_disableAllLEDs == disable)
    return;
  _disableAllLEDs = disable;
  emit modified(this);
}

bool
RadiodditySettingsExtension::quickKeyOverrideInhibited() const {
  return _quickKeyOverrideInhibited;
}
void
RadiodditySettingsExtension::inhibitQuickKeyOverride(bool inhibit) {
  if (_quickKeyOverrideInhibited == inhibit)
    return;
  _quickKeyOverrideInhibited = inhibit;
  emit modified(this);
}

bool
RadiodditySettingsExtension::txOnActiveChannel() const {
  return _txOnActiveChannel;
}
void
RadiodditySettingsExtension::enableTXOnActiveChannel(bool enable) {
  if (_txOnActiveChannel == enable)
    return;
  _txOnActiveChannel = enable;
  emit modified(this);
}

RadiodditySettingsExtension::ScanMode
RadiodditySettingsExtension::scanMode() const {
  return _scanMode;
}
void
RadiodditySettingsExtension::setScanMode(ScanMode mode) {
  if (_scanMode == mode)
    return;
  _scanMode = mode;
  emit modified(this);
}

Interval
RadiodditySettingsExtension::repeaterEndDelay() const {
  return _repeaterEndDelay;
}
void
RadiodditySettingsExtension::setRepeaterEndDelay(Interval delay) {
  if (_repeaterEndDelay == delay)
    return;
  _repeaterEndDelay = delay;
  emit modified(this);
}

Interval
RadiodditySettingsExtension::repeaterSTE() const {
  return _repeaterSTE;
}
void
RadiodditySettingsExtension::setRepeaterSTE(Interval ste) {
  if (_repeaterSTE == ste)
    return;
  _repeaterSTE = ste;
  emit modified(this);
}

bool
RadiodditySettingsExtension::txInterrupt() const {
  return _txInterrupt;
}
void
RadiodditySettingsExtension::enableTXInterrupt(bool enable) {
  if (enable == _txInterrupt)
    return;
  _txInterrupt = enable;
  emit modified(this);
}

RadiodditySettingsExtension::Language
RadiodditySettingsExtension::language() const {
  return _language;
}
void
RadiodditySettingsExtension::setLanguage(Language lang) {
  if (lang == _language)
    return;
  _language = lang;
  emit modified(this);
}

RadioddityButtonSettingsExtension *
RadiodditySettingsExtension::buttons() const {
  return _buttonSettings;
}

RadioddityToneSettingsExtension *
RadiodditySettingsExtension::tone() const {
  return _toneSettings;
}

RadioddityBootSettingsExtension *
RadiodditySettingsExtension::boot() const {
  return _bootSettings;
}
