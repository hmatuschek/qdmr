#include "tyt_extensions.hh"
#include "logger.hh"

/* ******************************************************************************************** *
 * Implementation of TyTChannelExtension
 * ******************************************************************************************** */
TyTChannelExtension::TyTChannelExtension(QObject *parent)
  : ConfigExtension(parent), _loneWorker(false), _autoScan(false), _talkaround(false),
    _dataCallConfirmed(false), _privateCallConfirmed(false), _emergencyAlarmConfirmed(false),
    _displayPTTId(true), _rxRefFrequency(RefFrequency::Low), _txRefFrequency(RefFrequency::Low),
    _tightSquelch(false), _compressedUDPHeader(false), _reverseBurst(true),
    _killTone(KillTone::Off), _inCallCriterion(InCallCriterion::Always), _allowInterrupt(false),
    _dcdm(false), _dcdmLeader(false)
{
  // pass...
}

ConfigItem *
TyTChannelExtension::clone() const {
  TyTChannelExtension *ex = new TyTChannelExtension();
  if (! ex->copy(*this)) {
    ex->deleteLater();
    return nullptr;
  }
  return ex;
}


bool
TyTChannelExtension::loneWorker() const {
  return _loneWorker;
}
void
TyTChannelExtension::enableLoneWorker(bool enable) {
  if (_loneWorker == enable)
    return;
  _loneWorker = enable;
  emit modified(this);
}

bool
TyTChannelExtension::autoScan() const {
  return _autoScan;
}
void
TyTChannelExtension::enableAutoScan(bool enable) {
  if (_autoScan == enable)
    return;
  _autoScan = enable;
  emit modified(this);
}

bool
TyTChannelExtension::talkaround() const {
  return _talkaround;
}
void
TyTChannelExtension::enableTalkaround(bool enable) {
  if (_talkaround == enable)
    return;
  _talkaround = enable;
  emit modified(this);
}

bool
TyTChannelExtension::dataCallConfirmed() const {
  return _dataCallConfirmed;
}
void
TyTChannelExtension::enableDataCallConfirmed(bool enable) {
  if (_dataCallConfirmed == enable)
    return;
  _dataCallConfirmed = enable;
  emit modified(this);
}

bool
TyTChannelExtension::privateCallConfirmed() const {
  return _privateCallConfirmed;
}
void
TyTChannelExtension::enablePrivateCallConfirmed(bool enable) {
  if (_privateCallConfirmed == enable)
    return;
  _privateCallConfirmed = enable;
  emit modified(this);
}

bool
TyTChannelExtension::emergencyAlarmConfirmed() const {
  return _emergencyAlarmConfirmed;
}
void
TyTChannelExtension::enableEmergencyAlarmConfirmed(bool enable) {
  if (_emergencyAlarmConfirmed == enable)
    return;
  _emergencyAlarmConfirmed = enable;
  emit modified(this);
}

bool
TyTChannelExtension::displayPTTId() const {
  return _displayPTTId;
}
void
TyTChannelExtension::enableDisplayPTTId(bool enable) {
  if (_displayPTTId == enable)
    return;
  _displayPTTId = enable;
  emit modified(this);
}

TyTChannelExtension::RefFrequency
TyTChannelExtension::rxRefFrequency() const {
  return _rxRefFrequency;
}
void
TyTChannelExtension::setRXRefFrequency(RefFrequency ref) {
  if (_rxRefFrequency == ref)
    return;
  _rxRefFrequency = ref;
  emit modified(this);
}

TyTChannelExtension::RefFrequency
TyTChannelExtension::txRefFrequency() const {
  return _txRefFrequency;
}
void
TyTChannelExtension::setTXRefFrequency(RefFrequency ref) {
  if (_txRefFrequency == ref)
    return;
  _txRefFrequency = ref;
  emit modified(this);
}

bool
TyTChannelExtension::tightSquelch() const {
  return _tightSquelch;
}
void
TyTChannelExtension::enableTightSquelch(bool enable) {
  if (_tightSquelch == enable)
    return;
  _tightSquelch = enable;
  emit modified(this);
}

bool
TyTChannelExtension::compressedUDPHeader() const {
  return _compressedUDPHeader;
}
void
TyTChannelExtension::enableCompressedUDPHeader(bool enable) {
  if (_compressedUDPHeader == enable)
    return;
  _compressedUDPHeader = enable;
  emit modified(this);
}

bool
TyTChannelExtension::reverseBurst() const {
  return _reverseBurst;
}
void
TyTChannelExtension::enableReverseBurst(bool enable) {
  if (_reverseBurst == enable)
    return;
  _reverseBurst = enable;
  emit modified(this);
}

TyTChannelExtension::KillTone
TyTChannelExtension::killTone() const {
  return _killTone;
}
void
TyTChannelExtension::setKillTone(KillTone tone) {
  if (_killTone == tone)
    return;
  _killTone = tone;
  emit modified(this);
}

TyTChannelExtension::InCallCriterion
TyTChannelExtension::inCallCriterion() const {
  return _inCallCriterion;
}
void
TyTChannelExtension::setInCallCriterion(InCallCriterion crit) {
  if (_inCallCriterion == crit)
    return;
  _inCallCriterion = crit;
  emit modified(this);
}

bool
TyTChannelExtension::allowInterrupt() const {
  return _allowInterrupt;
}
void
TyTChannelExtension::enableAllowInterrupt(bool enable) {
  if (_allowInterrupt == enable)
    return;
  _allowInterrupt = enable;
  emit modified(this);
}

bool
TyTChannelExtension::dcdm() const {
  return _dcdm;
}
void
TyTChannelExtension::enableDCDM(bool enable) {
  if (_dcdm == enable)
    return;
  _dcdm = enable;
  emit modified(this);
}

bool
TyTChannelExtension::dcdmLeader() const {
  return _dcdmLeader;
}
void
TyTChannelExtension::enableDCDMLeader(bool enable) {
  if (_dcdmLeader == enable)
    return;
  _dcdmLeader = enable;
  emit modified(this);
}

/*ConfigItem *
TyTChannelExtension::allocateChild(QMetaProperty &prop, const YAML::Node &node,
                                   const Context &ctx, const ErrorStack &err)
{
  Q_UNUSED(prop); Q_UNUSED(node); Q_UNUSED(ctx); Q_UNUSED(err)
  // There are no further extension/children to TyTChannelExtension.
  return nullptr;
}*/


/* ******************************************************************************************** *
 * Implementation of TyTScanListExtension
 * ******************************************************************************************** */
TyTScanListExtension::TyTScanListExtension(QObject *parent)
  : ConfigExtension(parent), _holdTime(500), _prioritySampleTime(2000)
{
  // pass...
}

ConfigItem *
TyTScanListExtension::clone() const {
  TyTScanListExtension *ex = new TyTScanListExtension();
  if (! ex->copy(*this)) {
    ex->deleteLater();
    return nullptr;
  }
  return ex;
}

unsigned
TyTScanListExtension::holdTime() const {
  return _holdTime;
}
void
TyTScanListExtension::setHoldTime(unsigned ms) {
  if (_holdTime == ms)
    return;
  _holdTime = ms;
  emit modified(this);
}

unsigned
TyTScanListExtension::prioritySampleTime() const {
  return _prioritySampleTime;
}
void
TyTScanListExtension::setPrioritySampleTime(unsigned ms) {
  if (_prioritySampleTime == ms)
    return;
  _prioritySampleTime = ms;
  emit modified(this);
}

/*ConfigItem *
TyTScanListExtension::allocateChild(QMetaProperty &prop, const YAML::Node &node,
                                    const Context &ctx, const ErrorStack &err)
{
  Q_UNUSED(prop); Q_UNUSED(node); Q_UNUSED(ctx); Q_UNUSED(err)
  // There are no further extension/children to TyTScanListExtension.
  return nullptr;
}*/


/* ******************************************************************************************** *
 * Implementation of TyTButtonSettings
 * ******************************************************************************************** */
TyTButtonSettings::TyTButtonSettings(QObject *parent)
  : ConfigExtension(parent)
{
  _sideButton1Short = Disabled;
  _sideButton1Long  = Tone1750Hz;
  _sideButton2Short = MonitorToggle;
  _sideButton2Long  = Disabled;
  _sideButton3Short = Disabled;
  _sideButton3Long = Disabled;
  _progButton1Short = Disabled;
  _progButton1Long = Disabled;
  _progButton2Short = Disabled;
  _progButton2Long = Disabled;
  _longPressDuration = 1000;
}

ConfigItem *
TyTButtonSettings::clone() const {
  TyTButtonSettings *ex = new TyTButtonSettings();
  if (! ex->copy(*this)) {
    ex->deleteLater();
    return nullptr;
  }
  return ex;
}

TyTButtonSettings::ButtonAction
TyTButtonSettings::sideButton1Short() const {
  return _sideButton1Short;
}
void
TyTButtonSettings::setSideButton1Short(ButtonAction action) {
  if (_sideButton1Short == action)
    return;
  _sideButton1Short = action;
  emit modified(this);
}

TyTButtonSettings::ButtonAction
TyTButtonSettings::sideButton1Long() const {
  return _sideButton1Long;
}
void
TyTButtonSettings::setSideButton1Long(ButtonAction action) {
  if (_sideButton1Long == action)
    return;
  _sideButton1Long = action;
  emit modified(this);
}

TyTButtonSettings::ButtonAction
TyTButtonSettings::sideButton2Short() const {
  return _sideButton2Short;
}
void
TyTButtonSettings::setSideButton2Short(ButtonAction action) {
  if (_sideButton2Short == action)
    return;
  _sideButton2Short = action;
  emit modified(this);
}

TyTButtonSettings::ButtonAction
TyTButtonSettings::sideButton2Long() const {
  return _sideButton2Long;
}
void
TyTButtonSettings::setSideButton2Long(ButtonAction action) {
  if (_sideButton2Long == action)
    return;
  _sideButton2Long = action;
  emit modified(this);
}

TyTButtonSettings::ButtonAction
TyTButtonSettings::sideButton3Short() const {
  return _sideButton3Short;
}
void
TyTButtonSettings::setSideButton3Short(ButtonAction action) {
  if (_sideButton3Short == action)
    return;
  _sideButton3Short = action;
  emit modified(this);
}

TyTButtonSettings::ButtonAction
TyTButtonSettings::sideButton3Long() const {
  return _sideButton3Long;
}
void
TyTButtonSettings::setSideButton3Long(ButtonAction action) {
  if (_sideButton3Long == action)
    return;
  _sideButton3Long = action;
  emit modified(this);
}

TyTButtonSettings::ButtonAction
TyTButtonSettings::progButton1Short() const {
  return _progButton1Short;
}
void
TyTButtonSettings::setProgButton1Short(ButtonAction action) {
  if (_progButton1Short == action)
    return;
  _progButton1Short = action;
  emit modified(this);
}

TyTButtonSettings::ButtonAction
TyTButtonSettings::progButton1Long() const {
  return _progButton1Long;
}
void
TyTButtonSettings::setProgButton1Long(ButtonAction action) {
  if (_progButton1Long == action)
    return;
  _progButton1Long = action;
  emit modified(this);
}

TyTButtonSettings::ButtonAction
TyTButtonSettings::progButton2Short() const {
  return _progButton2Short;
}
void
TyTButtonSettings::setProgButton2Short(ButtonAction action) {
  if (_progButton2Short == action)
    return;
  _progButton2Short = action;
  emit modified(this);
}

TyTButtonSettings::ButtonAction
TyTButtonSettings::progButton2Long() const {
  return _progButton2Long;
}
void
TyTButtonSettings::setProgButton2Long(ButtonAction action) {
  if (_progButton2Long == action)
    return;
  _progButton2Long = action;
  emit modified(this);
}

unsigned
TyTButtonSettings::longPressDuration() const {
  return _longPressDuration;
}
void
TyTButtonSettings::setLongPressDuration(unsigned dur) {
  _longPressDuration = dur;
}

/*ConfigItem *
TyTButtonSettings::allocateChild(QMetaProperty &prop, const YAML::Node &node,
                                 const Context &ctx, const ErrorStack &err)
{
  Q_UNUSED(prop); Q_UNUSED(node); Q_UNUSED(ctx); Q_UNUSED(err)
  // There are no further extension/children to TyTButtonSettings.
  return nullptr;
}*/


/* ******************************************************************************************** *
 * Implementation of TyTMenuSettings
 * ******************************************************************************************** */
TyTMenuSettings::TyTMenuSettings(QObject *parent)
  : ConfigExtension(parent), _inifiniteHangTime(false), _hangTime(10), _textMessage(true),
    _callAlert(true), _contactEditing(true), _manualDial(true), _remoteRadioCheck(true),
    _remoteMonitor(true), _remoteRadioEnable(true), _remoteRadioDisable(true), _scan(true),
    _scanListEditing(true), _callLogMissed(true), _callLogAnswered(true), _callLogOutgoing(true),
    _talkaround(true), _alertTone(true), _power(true), _backlight(true), _bootScreen(true),
    _keypadLock(true), _ledIndicator(true), _squelch(true), _vox(true), _password(true),
    _displayMode(true), _radioProgramming(true), _gpsInformation(true)
{
  // pass...
}

ConfigItem *
TyTMenuSettings::clone() const {
  TyTMenuSettings *ex = new TyTMenuSettings();
  if (! ex->copy(*this)) {
    ex->deleteLater();
    return nullptr;
  }
  return ex;
}

/*ConfigItem *
TyTMenuSettings::allocateChild(QMetaProperty &prop, const YAML::Node &node, const Context &ctx, const ErrorStack &err) {
  Q_UNUSED(prop); Q_UNUSED(node); Q_UNUSED(ctx); Q_UNUSED(err)
  // There are no further extension/children to TyTButtonSettings.
  return nullptr;
}*/

bool
TyTMenuSettings::hangtimeIsInfinite() const {
  return _inifiniteHangTime;
}
void
TyTMenuSettings::setHangtimeInfinite(bool infinite) {
  if (_inifiniteHangTime == infinite)
    return;
  _inifiniteHangTime = infinite;
  if (_inifiniteHangTime)
    _hangTime = 0;
  emit modified(this);
}

unsigned
TyTMenuSettings::hangTime() const {
  return _hangTime;
}

void
TyTMenuSettings::setHangTime(unsigned sec) {
  if (_hangTime == sec)
    return;
  _hangTime = sec;
  _inifiniteHangTime = (0 == _hangTime);
  emit modified(this);
}

bool
TyTMenuSettings::textMessage() const {
  return _textMessage;
}
void
TyTMenuSettings::enableTextMessage(bool enable) {
  if (_textMessage == enable)
    return;
  _textMessage = enable;
  emit modified(this);
}

bool
TyTMenuSettings::callAlert() const {
  return _callAlert;
}
void
TyTMenuSettings::enableCallAlert(bool enable) {
  if (_callAlert == enable)
    return;
  _callAlert = enable;
  emit modified(this);
}

bool
TyTMenuSettings::contactEditing() const {
  return _contactEditing;
}
void
TyTMenuSettings::enableContactEditing(bool enable) {
  if (_contactEditing == enable)
    return;
  _contactEditing = enable;
  emit modified(this);
}

bool
TyTMenuSettings::manualDial() const {
  return _manualDial;
}
void
TyTMenuSettings::enableManualDial(bool enable) {
  if (_manualDial == enable)
    return;
  _manualDial = enable;
  emit modified(this);
}

bool
TyTMenuSettings::remoteRadioCheck() const {
  return _remoteRadioCheck;
}
void
TyTMenuSettings::enableRemoteRadioCheck(bool enable) {
  if (_remoteRadioCheck == enable)
    return;
  _remoteRadioCheck = enable;
  emit modified(this);
}

bool
TyTMenuSettings::remoteMonitor() const {
  return _remoteMonitor;
}
void
TyTMenuSettings::enableRemoteMonitor(bool enable) {
  if (_remoteMonitor == enable)
    return;
  _remoteMonitor = enable;
  emit modified(this);
}

bool
TyTMenuSettings::remoteRadioEnable() const {
  return _remoteRadioEnable;
}
void
TyTMenuSettings::enableRemoteRadioEnable(bool enable) {
  if (_remoteRadioEnable == enable)
    return;
  _remoteRadioEnable = enable;
  emit modified(this);
}

bool
TyTMenuSettings::remoteRadioDisable() const {
  return _remoteRadioDisable;
}
void
TyTMenuSettings::enableRemoteRadioDisable(bool enable) {
  if (_remoteRadioDisable == enable)
    return;
  _remoteRadioDisable = enable;
  emit modified(this);
}

bool
TyTMenuSettings::scan() const {
  return _scan;
}
void
TyTMenuSettings::enableScan(bool enable) {
  if (_scan == enable)
    return;
  _scan = enable;
  emit modified(this);
}

bool
TyTMenuSettings::scanListEditing() const {
  return _scanListEditing;
}
void
TyTMenuSettings::enableScanListEditing(bool enable) {
  if (_scanListEditing == enable)
    return;
  _scanListEditing = enable;
  emit modified(this);
}

bool
TyTMenuSettings::callLogMissed() const {
  return _callLogMissed;
}
void
TyTMenuSettings::enableCallLogMissed(bool enable) {
  if (_callLogMissed == enable)
    return;
  _callLogMissed = enable;
  emit modified(this);
}

bool
TyTMenuSettings::callLogAnswered() const {
  return _callLogAnswered;
}
void
TyTMenuSettings::enableCallLogAnswered(bool enable) {
  if (_callLogAnswered == enable)
    return;
  _callLogAnswered = enable;
  emit modified(this);
}

bool
TyTMenuSettings::callLogOutgoing() const {
  return _callLogOutgoing;
}
void
TyTMenuSettings::enableCallLogOutgoing(bool enable) {
  if (_callLogOutgoing == enable)
    return;
  _callLogOutgoing = enable;
  emit modified(this);
}

bool
TyTMenuSettings::talkaround() const {
  return _talkaround;
}
void
TyTMenuSettings::enableTalkaround(bool enable) {
  if (_talkaround == enable)
    return;
  _talkaround = enable;
  emit modified(this);
}

bool
TyTMenuSettings::alertTone() const {
  return _alertTone;
}
void
TyTMenuSettings::enableAlertTone(bool enable) {
  if (_alertTone == enable)
    return;
  _alertTone = enable;
  emit modified(this);
}

bool
TyTMenuSettings::power() const {
  return _power;
}
void
TyTMenuSettings::enablePower(bool enable) {
  if (_power == enable)
    return;
  _power = enable;
  emit modified(this);
}

bool
TyTMenuSettings::backlight() const {
  return _backlight;
}
void
TyTMenuSettings::enableBacklight(bool enable) {
  if (_backlight == enable)
    return;
  _backlight = enable;
  emit modified(this);
}

bool
TyTMenuSettings::bootScreen() const {
  return _bootScreen;
}
void
TyTMenuSettings::enableBootScreen(bool enable) {
  if (_bootScreen == enable)
    return;
  _bootScreen = enable;
  emit modified(this);
}

bool
TyTMenuSettings::keypadLock() const {
  return _keypadLock;
}
void
TyTMenuSettings::enableKeypadLock(bool enable) {
  if (_keypadLock == enable)
    return;
  _keypadLock = enable;
  emit modified(this);
}

bool
TyTMenuSettings::ledIndicator() const {
  return _ledIndicator;
}
void
TyTMenuSettings::enableLEDIndicator(bool enable) {
  if (_ledIndicator == enable)
    return;
  _ledIndicator = enable;
  emit modified(this);
}

bool
TyTMenuSettings::squelch() const {
  return _squelch;
}
void
TyTMenuSettings::enableSquelch(bool enable) {
  if (_squelch == enable)
    return;
  _squelch = enable;
  emit modified(this);
}

bool
TyTMenuSettings::vox() const {
  return _vox;
}
void
TyTMenuSettings::enableVOX(bool enable) {
  if (_vox == enable)
    return;
  _vox = enable;
  emit modified(this);
}

bool
TyTMenuSettings::password() const {
  return _password;
}
void
TyTMenuSettings::enablePassword(bool enable) {
  if (_password == enable)
    return;
  _password = enable;
  emit modified(this);
}

bool
TyTMenuSettings::displayMode() const {
  return _displayMode;
}
void
TyTMenuSettings::enableDisplayMode(bool enable) {
  if (_displayMode == enable)
    return;
  _displayMode = enable;
  emit modified(this);
}

bool
TyTMenuSettings::radioProgramming() const {
  return _radioProgramming;
}
void
TyTMenuSettings::enableRadioProgramming(bool enable) {
  if (_radioProgramming == enable)
    return;
  _radioProgramming = enable;
  emit modified(this);
}

bool
TyTMenuSettings::gpsInformation() const {
  return _gpsInformation;
}
void
TyTMenuSettings::enableGPSInformation(bool enable) {
  if (_gpsInformation == enable)
    return;
  _gpsInformation = enable;
  emit modified(this);
}


/* ******************************************************************************************** *
 * Implementation of TyTSettingsExtension
 * ******************************************************************************************** */
TyTSettingsExtension::TyTSettingsExtension(QObject *parent)
  : ConfigExtension(parent), _monitorType(MonitorType::Open), _allLEDsDisabled(false),
    _talkPermitToneDigital(false), _talkPermitToneAnalog(false), _passwdAndLock(false),
    _channelFreeIndicationTone(true), _allTonesDisabled(false), _powerSaveMode(true),
    _wakeupPreamble(true), _bootPicture(true), _channelModeA(true), _channelModeB(true),
    _channelMode(true), _txPreambleDuration(600), _groupCallHangTime(3000),
    _privateCallHangTime(3000), _lowBatteryWarnInterval(120), _callAlertToneContinuous(false),
    _callAlertToneDuration(0), _loneWorkerResponseTime(1), _loneWorkerReminderTime(10),
    _digitalScanHangTime(1000), _analogScanHangTime(1000), _backlightAlwaysOn(false),
    _backlightDuration(10), _keypadLockManual(true), _keypadLockTime(5*0xff),
    _powerOnPasswordEnabled(false), _powerOnPassword(0), _radioProgPasswordEnabled(false),
    _radioProgPassword(0), _pcProgPassword(""), _privateCallMatch(true), _groupCallMatch(true),
    _channelHangTime(3000)
{
  // pass...
}

ConfigItem *
TyTSettingsExtension::clone() const {
  TyTSettingsExtension *ex = new TyTSettingsExtension();
  if (! ex->copy(*this)) {
    ex->deleteLater();
    return nullptr;
  }

  return ex;
}

TyTSettingsExtension::MonitorType
TyTSettingsExtension::monitorType() const {
  return _monitorType;
}
void
TyTSettingsExtension::setMonitorType(MonitorType type) {
  if (_monitorType == type)
    return;
  _monitorType = type;
  emit modified(this);
}

bool
TyTSettingsExtension::allLEDsDisabled() const {
  return _allLEDsDisabled;
}
void
TyTSettingsExtension::disableAllLEDs(bool disable) {
  if (_allLEDsDisabled == disable)
    return;
  _allLEDsDisabled = disable;
  emit modified(this);
}

bool
TyTSettingsExtension::talkPermitToneDigital() const {
  return _talkPermitToneDigital;
}
void
TyTSettingsExtension::enableTalkPermitToneDigital(bool enable) {
  if (_talkPermitToneDigital == enable)
    return;
  _talkPermitToneDigital = enable;
  emit modified(this);
}

bool
TyTSettingsExtension::talkPermitToneAnalog() const {
  return _talkPermitToneAnalog;
}
void
TyTSettingsExtension::enableTalkPermitToneAnalog(bool enable) {
  if (_talkPermitToneAnalog == enable)
    return;
  _talkPermitToneAnalog = enable;
  emit modified(this);
}

bool
TyTSettingsExtension::passwordAndLock() const {
  return _passwdAndLock;
}
void
TyTSettingsExtension::enablePasswordAndLock(bool enable) {
  if (_passwdAndLock == enable)
    return;
  _passwdAndLock = enable;
  emit modified(this);
}

bool
TyTSettingsExtension::channelFreeIndicationTone() const {
  return _channelFreeIndicationTone;
}
void
TyTSettingsExtension::enableChannelFreeIndicationTone(bool enable) {
  if (_channelFreeIndicationTone == enable)
    return;
  _channelFreeIndicationTone = enable;
  emit modified(this);
}

bool
TyTSettingsExtension::allTonesDisabled() const {
  return _allTonesDisabled;
}
void
TyTSettingsExtension::disableAllTones(bool disable) {
  if (_allTonesDisabled == disable)
    return;
  _allTonesDisabled = disable;
  emit modified(this);
}

bool
TyTSettingsExtension::powerSaveMode() const {
  return _powerSaveMode;
}
void
TyTSettingsExtension::enablePowerSaveMode(bool enable) {
  if (_powerSaveMode == enable)
    return;
  _powerSaveMode = enable;
  emit modified(this);
}

bool
TyTSettingsExtension::wakeupPreamble() const {
  return _wakeupPreamble;
}
void
TyTSettingsExtension::enableWakeupPreamble(bool enable) {
  if (_wakeupPreamble == enable)
    return;
  _wakeupPreamble = enable;
  emit modified(this);
}

bool
TyTSettingsExtension::bootPicture() const {
  return _bootPicture;
}
void
TyTSettingsExtension::enableBootPicture(bool enable) {
  if (_bootPicture == enable)
    return;
  _bootPicture = enable;
  emit modified(this);
}


bool
TyTSettingsExtension::channelMode() const {
  return _channelMode;
}
void
TyTSettingsExtension::enableChannelMode(bool enable) {
  if (_channelMode == enable)
    return;
  _channelMode = enable;
  emit modified(this);
}
bool
TyTSettingsExtension::channelModeA() const {
  return _channelModeA;
}
void
TyTSettingsExtension::enableChannelModeA(bool enable) {
  if (_channelModeA == enable)
    return;
  _channelModeA = enable;
  emit modified(this);
}
bool
TyTSettingsExtension::channelModeB() const {
  return _channelModeB;
}
void
TyTSettingsExtension::enableChannelModeB(bool enable) {
  if (_channelModeB == enable)
    return;
  _channelModeB = enable;
  emit modified(this);
}

unsigned
TyTSettingsExtension::txPreambleDuration() const {
  return _txPreambleDuration;
}
void
TyTSettingsExtension::setTXPreambleDuration(unsigned ms) {
  if (_txPreambleDuration == ms)
    return;
  _txPreambleDuration = ms;
  emit modified(this);
}

unsigned
TyTSettingsExtension::groupCallHangTime() const {
  return _groupCallHangTime;
}
void
TyTSettingsExtension::setGroupCallHangTime(unsigned ms) {
  if (_groupCallHangTime == ms)
    return;
  _groupCallHangTime = ms;
  emit modified(this);
}

unsigned
TyTSettingsExtension::privateCallHangTime() const {
  return _privateCallHangTime;
}
void
TyTSettingsExtension::setPrivateCallHangTime(unsigned ms) {
  if (_privateCallHangTime == ms)
    return;
  _privateCallHangTime = ms;
  emit modified(this);
}

unsigned
TyTSettingsExtension::lowBatteryWarnInterval() const {
  return _lowBatteryWarnInterval;
}
void
TyTSettingsExtension::setLowBatteryWarnInterval(unsigned sec) {
  if (_lowBatteryWarnInterval == sec)
    return;
  _lowBatteryWarnInterval = sec;
  emit modified(this);
}

bool
TyTSettingsExtension::callAlertToneContinuous() const {
  return _callAlertToneContinuous;
}
void
TyTSettingsExtension::enableCallAlertToneContinuous(bool enable) {
  if (_callAlertToneContinuous == enable)
    return;
  _callAlertToneContinuous = enable;
  emit modified(this);
}
unsigned
TyTSettingsExtension::callAlertToneDuration() const {
  return _callAlertToneDuration;
}
void
TyTSettingsExtension::setCallAlertToneDuration(unsigned sec) {
  if (_callAlertToneDuration == sec)
    return;
  _callAlertToneDuration = sec;
  emit modified(this);
}

unsigned
TyTSettingsExtension::loneWorkerResponseTime() const {
  return _loneWorkerResponseTime;
}
void
TyTSettingsExtension::setLoneWorkerResponseTime(unsigned min) {
  if (_loneWorkerResponseTime == min)
    return;
  _loneWorkerResponseTime = min;
  emit modified(this);
}

unsigned
TyTSettingsExtension::loneWorkerReminderTime() const {
  return _loneWorkerReminderTime;
}
void
TyTSettingsExtension::setLoneWorkerReminderTime(unsigned sec) {
  if (_loneWorkerReminderTime == sec)
    return;
  _loneWorkerReminderTime = sec;
  emit modified(this);
}

unsigned
TyTSettingsExtension::digitalScanHangTime() const {
  return _digitalScanHangTime;
}
void
TyTSettingsExtension::setDigitalScanHangTime(unsigned ms) {
  if (_digitalScanHangTime == ms)
    return;
  _digitalScanHangTime = ms;
  emit modified(this);
}

unsigned
TyTSettingsExtension::analogScanHangTime() const {
  return _analogScanHangTime;
}
void
TyTSettingsExtension::setAnalogScanHangTime(unsigned ms) {
  if (_analogScanHangTime == ms)
    return;
  _analogScanHangTime = ms;
  emit modified(this);
}

bool
TyTSettingsExtension::backlightAlwaysOn() const {
  return _backlightAlwaysOn;
}
void
TyTSettingsExtension::enableBacklightAlwaysOn(bool enable) {
  if (_backlightAlwaysOn == enable)
    return;
  _backlightAlwaysOn = enable;
  emit modified(this);
}
unsigned
TyTSettingsExtension::backlightDuration() const {
  return _backlightDuration;
}
void
TyTSettingsExtension::setBacklightDuration(unsigned sec) {
  if (_backlightDuration == sec)
    return;
  _backlightDuration = sec;
  emit modified(this);
}

bool
TyTSettingsExtension::keypadLockManual() const {
  return _keypadLockManual;
}
void
TyTSettingsExtension::enableKeypadLockManual(bool enable) {
  if (_keypadLockManual == enable)
    return;
  _keypadLockManual = enable;
  emit modified(this);
}
unsigned
TyTSettingsExtension::keypadLockTime() const {
  return _keypadLockTime;
}
void
TyTSettingsExtension::setKeypadLockTime(unsigned sec) {
  if (_keypadLockTime == sec)
    return;
  _keypadLockTime = sec;
  emit modified(this);
}

bool
TyTSettingsExtension::powerOnPasswordEnabled() const {
  return _powerOnPasswordEnabled;
}
void
TyTSettingsExtension::enablePowerOnPassword(bool enable) {
  if (_powerOnPasswordEnabled == enable)
    return;
  _powerOnPasswordEnabled = enable;
  emit modified(this);
}
unsigned
TyTSettingsExtension::powerOnPassword() const {
  return _powerOnPassword;
}
void
TyTSettingsExtension::setPowerOnPassword(unsigned passwd) {
  if (_powerOnPassword == passwd)
    return;
  _powerOnPassword = passwd;
  emit modified(this);
}

bool
TyTSettingsExtension::radioProgPasswordEnabled() const {
  return _radioProgPassword;
}
void
TyTSettingsExtension::enableRadioProgPassword(bool enable) {
  if (_radioProgPasswordEnabled == enable)
    return;
  _radioProgPasswordEnabled = enable;
  emit modified(this);
}
unsigned
TyTSettingsExtension::radioProgPassword() const {
  return _radioProgPassword;
}
void
TyTSettingsExtension::setRadioProgPassword(unsigned passwd) {
  if (_radioProgPassword == passwd)
    return;
  _radioProgPassword = passwd;
  emit modified(this);
}

const QString &
TyTSettingsExtension::pcProgPassword() const {
  return _pcProgPassword;
}
void
TyTSettingsExtension::setPCProgPassword(const QString &passwd) {
  if (_pcProgPassword == passwd)
    return;
  _pcProgPassword = passwd;
  emit modified(this);
}

bool
TyTSettingsExtension::privateCallMatch() const {
  return _privateCallMatch;
}
void
TyTSettingsExtension::enablePrivateCallMatch(bool enable) {
  if (_privateCallMatch == enable)
    return;
  _privateCallMatch = enable;
  emit modified(this);
}

bool
TyTSettingsExtension::groupCallMatch() const {
  return _groupCallMatch;
}
void
TyTSettingsExtension::enableGroupCallMatch(bool enable) {
  if (_groupCallMatch == enable)
    return;
  _groupCallMatch = enable;
  emit modified(this);
}

unsigned
TyTSettingsExtension::channelHangTime() const {
  return _channelHangTime;
}
void
TyTSettingsExtension::setChannelHangTime(unsigned ms) {
  if (_channelHangTime == ms)
    return;
  _channelHangTime = ms;
  emit modified(this);
}

/*ConfigItem *
TyTSettingsExtension::allocateChild(QMetaProperty &prop, const YAML::Node &node, const Context &ctx, const ErrorStack &err) {
  Q_UNUSED(prop); Q_UNUSED(node); Q_UNUSED(ctx); Q_UNUSED(err)
  // No extensions to this extension.
  return nullptr;
}*/


/* ******************************************************************************************** *
 * Implementation of TyTButtonSettings
 * ******************************************************************************************** */
TyTConfigExtension::TyTConfigExtension(QObject *parent)
  : ConfigExtension(parent), _buttonSettings(new TyTButtonSettings(this)),
    _menuSettings(new TyTMenuSettings(this))
{
  // Pass...
}

ConfigItem *
TyTConfigExtension::clone() const {
  TyTConfigExtension *ex = new TyTConfigExtension();
  if (! ex->copy(*this)) {
    ex->deleteLater();
    return nullptr;
  }
  return ex;
}

TyTButtonSettings *
TyTConfigExtension::buttonSettings() const {
  return _buttonSettings;
}

TyTMenuSettings *
TyTConfigExtension::menuSettings() const {
  return _menuSettings;
}

/*ConfigItem *
TyTConfigExtension::allocateChild(QMetaProperty &prop, const YAML::Node &node,
                                  const Context &ctx, const ErrorStack &err)
{
  Q_UNUSED(prop); Q_UNUSED(node); Q_UNUSED(ctx); Q_UNUSED(err)
  // All extensions are pre-allocated. So nothing to do here.
  return nullptr;
}*/
