#include "radioddity_extensions.hh"

/* ********************************************************************************************* *
 * Implementation of RadiodditySettingsExtension
 * ********************************************************************************************* */
RadiodditySettingsExtension::RadiodditySettingsExtension(QObject *parent)
  : ConfigExtension(parent), _preambleDuration(Interval::fromMilliseconds(360)),
    _monitorType(MonitorType::Silent), _lowBatteryWarnInterval(Interval::fromSeconds(30)),
    _callAlertDuration(Interval::fromSeconds(120)),
    _loneWorkerResponseTime(Interval::fromMinutes(1)),
    _loneWorkerReminderPeriod(Interval::fromSeconds(10)),
    _groupCallHangTime(Interval::fromMilliseconds(3000)),
    _privateCallHangTime(Interval::fromMilliseconds(3000)),
    _downChannelModeVFO(false), _upChannelModeVFO(false), _resetTone(false),
    _unknownNumberTone(false), _artsToneMode(ARTSTone::Once), _digitalTalkPermitTone(false),
    _analogTalkPermitTone(false), _selftestTone(true), _channelFreeIndicationTone(false),
    _disableAllTones(false), _powerSaveMode(true), _wakeupPreamble(true), _disableAllLEDs(false),
    _quickKeyOverrideInhibited(false), _txExitTone(false), _txOnActiveChannel(true),
    _animation(false), _scanMode(ScanMode::Time), _repeaterEndDelay(), _repeaterSTE(), _progPasswd()
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
RadiodditySettingsExtension::lowBatteryWarnInterval() const {
  return _lowBatteryWarnInterval;
}
void
RadiodditySettingsExtension::setLowBatteryWarnInterval(Interval sec) {
  if (_lowBatteryWarnInterval == sec)
    return;
  _lowBatteryWarnInterval = sec;
  emit modified(this);
}

Interval
RadiodditySettingsExtension::callAlertDuration() const {
  return _callAlertDuration;
}
void
RadiodditySettingsExtension::setCallAlertDuration(Interval sec) {
  if (_callAlertDuration == sec)
    return;
  _callAlertDuration = sec;
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
RadiodditySettingsExtension::resetTone() const {
  return _resetTone;
}
void
RadiodditySettingsExtension::enableResetTone(bool enable) {
  if (_resetTone == enable)
    return;
  _resetTone = enable;
  emit modified(this);
}

bool
RadiodditySettingsExtension::unknownNumberTone() const {
  return _unknownNumberTone;
}
void
RadiodditySettingsExtension::enableUnknownNumberTone(bool enable) {
  if (_unknownNumberTone == enable)
    return;
  _unknownNumberTone = enable;
  emit modified(this);
}

RadiodditySettingsExtension::ARTSTone
RadiodditySettingsExtension::artsToneMode() const {
  return _artsToneMode;
}
void
RadiodditySettingsExtension::setARTSToneMode(ARTSTone mode) {
  if (_artsToneMode == mode)
    return;
  _artsToneMode = mode;
  emit modified(this);
}

bool
RadiodditySettingsExtension::digitalTalkPermitTone() const {
  return _digitalTalkPermitTone;
}
void
RadiodditySettingsExtension::enableDigitalTalkPermitTone(bool enable) {
  if (_digitalTalkPermitTone == enable)
    return;
  _digitalTalkPermitTone = enable;
  emit modified(this);
}

bool
RadiodditySettingsExtension::analogTalkPermitTone() const {
  return _analogTalkPermitTone;
}
void
RadiodditySettingsExtension::enableAnalogTalkPermitTone(bool enable) {
  if (_analogTalkPermitTone == enable)
    return;
  _analogTalkPermitTone = enable;
  emit modified(this);
}

bool
RadiodditySettingsExtension::selftestTone() const {
  return _selftestTone;
}
void
RadiodditySettingsExtension::enableSelftestTone(bool enable) {
  if (_selftestTone == enable)
    return;
  _selftestTone = enable;
  emit modified(this);
}

bool
RadiodditySettingsExtension::channelFreeIndicationTone() const {
  return _channelFreeIndicationTone;
}
void
RadiodditySettingsExtension::enableChannelFreeIndicationTone(bool enable) {
  if (_channelFreeIndicationTone == enable)
    return;
  _channelFreeIndicationTone = enable;
  emit modified(this);
}

bool
RadiodditySettingsExtension::allTonesDisabled() const {
  return _disableAllTones;
}
void
RadiodditySettingsExtension::disableAllTones(bool disable) {
  if (_disableAllTones == disable)
    return;
  _disableAllTones = disable;
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
RadiodditySettingsExtension::txExitTone() const {
  return _txExitTone;
}
void
RadiodditySettingsExtension::enableTXExitTone(bool enable) {
  if (_txExitTone == enable)
    return;
  _txExitTone = enable;
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

bool
RadiodditySettingsExtension::animation() const {
  return _animation;
}
void
RadiodditySettingsExtension::enableAnimation(bool enable) {
  if (_animation == enable)
    return;
  _animation = enable;
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

const QString &
RadiodditySettingsExtension::progPassword() const {
  return _progPasswd;
}
void
RadiodditySettingsExtension::setProgPassword(const QString &pwd) {
  if (_progPasswd == pwd)
    return;
  _progPasswd = pwd;
  emit modified(this);
}

