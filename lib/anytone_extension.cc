#include "anytone_extension.hh"


/* ********************************************************************************************* *
 * Implementation of AnytoneChannelExtension
 * ********************************************************************************************* */
AnytoneChannelExtension::AnytoneChannelExtension(QObject *parent)
  : ConfigExtension(parent), _talkaround(false), _frequencyCorrection(0), _handsFree(false)
{
  // pass...
}

bool
AnytoneChannelExtension::talkaround() const {
  return _talkaround;
}
void
AnytoneChannelExtension::enableTalkaround(bool enable) {
  if (enable == _talkaround)
    return;
  _talkaround = enable;
  emit modified(this);
}

int
AnytoneChannelExtension::frequencyCorrection() const {
  return _frequencyCorrection;
}
void
AnytoneChannelExtension::setFrequencyCorrection(int corr) {
  if (corr == _frequencyCorrection)
    return;
  _frequencyCorrection = corr;
  emit modified(this);
}

bool
AnytoneChannelExtension::handsFree() const {
  return _handsFree;
}
void
AnytoneChannelExtension::enableHandsFree(bool enable) {
  if (enable == _handsFree)
    return;
  _handsFree = enable;
  emit modified(this);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneAnalogChannelExtension
 * ********************************************************************************************* */
AnytoneFMChannelExtension::AnytoneFMChannelExtension(QObject *parent)
  : AnytoneChannelExtension(parent), _reverseBurst(false), _rxCustomCTCSS(false),
    _txCustomCTCSS(false), _customCTCSS(0), _scrambler(false)
{
  // pass...
}

ConfigItem *
AnytoneFMChannelExtension::clone() const {
  AnytoneFMChannelExtension *ext = new AnytoneFMChannelExtension();
  if (! ext->copy(*this)) {
    ext->deleteLater();
    return nullptr;
  }
  return ext;
}

bool
AnytoneFMChannelExtension::reverseBurst() const {
  return _reverseBurst;
}
void
AnytoneFMChannelExtension::enableReverseBurst(bool enable) {
  if (enable == _reverseBurst)
    return;
  _reverseBurst = enable;
  emit modified(this);
}

bool
AnytoneFMChannelExtension::rxCustomCTCSS() const {
  return _rxCustomCTCSS;
}
void
AnytoneFMChannelExtension::enableRXCustomCTCSS(bool enable) {
  if (enable == _rxCustomCTCSS)
    return;
  _rxCustomCTCSS = enable;
  emit modified(this);
}
bool
AnytoneFMChannelExtension::txCustomCTCSS() const {
  return _txCustomCTCSS;
}
void
AnytoneFMChannelExtension::enableTXCustomCTCSS(bool enable) {
  if (enable == _txCustomCTCSS)
    return;
  _txCustomCTCSS = enable;
  emit modified(this);
}
double
AnytoneFMChannelExtension::customCTCSS() const {
  return _customCTCSS;
}
void
AnytoneFMChannelExtension::setCustomCTCSS(double freq) {
  if (freq == _customCTCSS)
    return;
  _customCTCSS = freq;
  emit modified(this);
}

AnytoneFMChannelExtension::SquelchMode
AnytoneFMChannelExtension::squelchMode() const {
  return _squelchMode;
}
void
AnytoneFMChannelExtension::setSquelchMode(SquelchMode mode) {
  if (mode == _squelchMode)
    return;
  _squelchMode = mode;
  emit modified(this);
}

bool
AnytoneFMChannelExtension::scrambler() const {
  return _scrambler;
}
void
AnytoneFMChannelExtension::enableScrambler(bool enable) {
  if (enable == _scrambler)
    return;
  _scrambler = enable;
  emit modified(this);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneDigitalChannelExtension
 * ********************************************************************************************* */
AnytoneDMRChannelExtension::AnytoneDMRChannelExtension(QObject *parent)
  : AnytoneChannelExtension(parent), _callConfirm(false), _sms(true), _smsConfirm(false),
    _dataACK(true), _simplexTDMA(false), _adaptiveTDMA(false), _loneWorker(false),
    _throughMode(false)
{
  // pass...
}

ConfigItem *
AnytoneDMRChannelExtension::clone() const {
  AnytoneDMRChannelExtension *ext = new AnytoneDMRChannelExtension();
  if (! ext->copy(*this)) {
    ext->deleteLater();
    return nullptr;
  }
  return ext;
}

bool
AnytoneDMRChannelExtension::callConfirm() const {
  return _callConfirm;
}
void
AnytoneDMRChannelExtension::enableCallConfirm(bool enabled) {
  if (enabled == _callConfirm)
    return;
  _callConfirm = enabled;
  emit modified(this);
}

bool
AnytoneDMRChannelExtension::sms() const {
  return _sms;
}
void
AnytoneDMRChannelExtension::enableSMS(bool enable) {
  if (enable == _sms)
    return;
  _sms = enable;
  emit modified(this);
}

bool
AnytoneDMRChannelExtension::smsConfirm() const {
  return _smsConfirm;
}
void
AnytoneDMRChannelExtension::enableSMSConfirm(bool enabled) {
  if (enabled == _smsConfirm)
    return;
  _smsConfirm = enabled;
  emit modified(this);
}

bool
AnytoneDMRChannelExtension::dataACK() const {
  return _dataACK;
}
void
AnytoneDMRChannelExtension::enableDataACK(bool enable) {
  if (enable==_dataACK)
    return;
  _dataACK = enable;
  emit modified(this);
}

bool
AnytoneDMRChannelExtension::simplexTDMA() const {
  return _simplexTDMA;
}
void
AnytoneDMRChannelExtension::enableSimplexTDMA(bool enable) {
  if (enable == _simplexTDMA)
    return;
  _simplexTDMA = enable;
  emit modified(this);
}

bool
AnytoneDMRChannelExtension::adaptiveTDMA() const {
  return _adaptiveTDMA;
}
void
AnytoneDMRChannelExtension::enableAdaptiveTDMA(bool enable) {
  if (enable == _adaptiveTDMA)
    return;
  _adaptiveTDMA = enable;
  emit modified(this);
}

bool
AnytoneDMRChannelExtension::loneWorker() const {
  return _loneWorker;
}
void
AnytoneDMRChannelExtension::enableLoneWorker(bool enable) {
  if (enable == _loneWorker)
    return;
  _loneWorker = enable;
  emit modified(this);
}

bool
AnytoneDMRChannelExtension::throughMode() const {
  return _throughMode;
}
void
AnytoneDMRChannelExtension::enableThroughMode(bool enable) {
  if (enable == _throughMode)
    return;
  _throughMode = enable;
  emit modified(this);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneZoneExtension
 * ********************************************************************************************* */
AnytoneZoneExtension::AnytoneZoneExtension(QObject *parent)
  : ConfigExtension(parent), _hidden(false)
{
  // pass...
}

ConfigItem *
AnytoneZoneExtension::clone() const {
  AnytoneZoneExtension *obj = new AnytoneZoneExtension();
  if (! obj->copy(*this)) {
    obj->deleteLater();
    return nullptr;
  }
  return obj;
}

bool
AnytoneZoneExtension::hidden() const {
  return _hidden;
}
void
AnytoneZoneExtension::enableHidden(bool enable) {
  if (_hidden == enable)
    return;
  _hidden = enable;
  emit modified(this);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneContactExtension
 * ********************************************************************************************* */
AnytoneContactExtension::AnytoneContactExtension(QObject *parent)
  : ConfigExtension(parent), _alertType(AlertType::None)
{
  // pass...
}

ConfigItem *
AnytoneContactExtension::clone() const {
  AnytoneContactExtension *obj = new AnytoneContactExtension();
  if (! obj->copy(*this)) {
    obj->deleteLater();
    return nullptr;
  }
  return obj;
}

AnytoneContactExtension::AlertType
AnytoneContactExtension::alertType() const {
  return _alertType;
}
void
AnytoneContactExtension::setAlertType(AlertType type) {
  _alertType = type;
}


/* ********************************************************************************************* *
 * Implementation of AnytoneSettingsExtension
 * ********************************************************************************************* */
AnytoneSettingsExtension::AnytoneSettingsExtension(QObject *parent)
  : ConfigExtension(parent), _bootSettings(new AnytoneBootSettingsExtension(this)),
    _keySettings(new AnytoneKeySettingsExtension(this)),
    _keyTone(false), _displayFrequency(false), _autoKeyLock(false), _autoShutDownDelay(0),
    _powerSave(PowerSave::Save50), _voxDelay(0), _vfoScanType(VFOScanType::TO)
{
  // pass...
}

ConfigItem *
AnytoneSettingsExtension::clone() const {
  AnytoneSettingsExtension *ext = new AnytoneSettingsExtension();
  if (! ext->copy(*this)) {
    ext->deleteLater();
    return nullptr;
  }
  return ext;
}

AnytoneBootSettingsExtension *
AnytoneSettingsExtension::bootSettings() const {
  return _bootSettings;
}

AnytoneKeySettingsExtension *
AnytoneSettingsExtension::keySettings() const {
  return _keySettings;
}

bool
AnytoneSettingsExtension::keyToneEnabled() const {
  return _keyTone;
}
void
AnytoneSettingsExtension::enableKeyTone(bool enable) {
  if (_keyTone==enable)
    return;
  _keyTone = enable;
  emit modified(this);
}

bool
AnytoneSettingsExtension::displayFrequencyEnabled() const {
  return _displayFrequency;
}
void
AnytoneSettingsExtension::enableDisplayFrequency(bool enable) {
  if (_displayFrequency == enable)
    return;
  _displayFrequency = enable;
  emit modified(this);
}

bool
AnytoneSettingsExtension::autoKeyLockEnabled() const {
  return _autoKeyLock;
}
void
AnytoneSettingsExtension::enableAutoKeyLock(bool enabled) {
  if (_autoKeyLock==enabled)
    return;
  _autoKeyLock = enabled;
  emit modified(this);
}

unsigned int
AnytoneSettingsExtension::autoShutDownDelay() const {
  return _autoShutDownDelay;
}
void
AnytoneSettingsExtension::setAutoShutDownDelay(unsigned int min) {
  if (_autoShutDownDelay == min)
    return;
  _autoShutDownDelay = min;
  emit modified(this);
}

AnytoneSettingsExtension::PowerSave
AnytoneSettingsExtension::powerSave() const {
  return _powerSave;
}
void
AnytoneSettingsExtension::setPowerSave(PowerSave save) {
  if (_powerSave == save)
    return;
  _powerSave = save;
  emit modified(this);
}

unsigned int
AnytoneSettingsExtension::voxDelay() const {
  return _voxDelay;
}
void
AnytoneSettingsExtension::setVOXDelay(unsigned int ms) {
  if (_voxDelay == ms)
    return;
  _voxDelay = ms;
  emit modified(this);
}

AnytoneSettingsExtension::VFOScanType
AnytoneSettingsExtension::vfoScanType() const {
  return _vfoScanType;
}
void
AnytoneSettingsExtension::setVFOScanType(VFOScanType type) {
  if (_vfoScanType == type)
    return;
  _vfoScanType = type;
  emit modified(this);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneBootSettingsExtension
 * ********************************************************************************************* */
AnytoneBootSettingsExtension::AnytoneBootSettingsExtension(QObject *parent)
  : ConfigItem(parent), _bootDisplay(BootDisplay::Default), _bootPasswordEnabled(false)
{
  // pass...
}

ConfigItem *
AnytoneBootSettingsExtension::clone() const {
  AnytoneBootSettingsExtension *ext = new AnytoneBootSettingsExtension();
  if (! ext->copy(*this)) {
    ext->deleteLater();
    return nullptr;
  }
  return ext;
}

AnytoneBootSettingsExtension::BootDisplay
AnytoneBootSettingsExtension::bootDisplay() const {
  return _bootDisplay;
}
void
AnytoneBootSettingsExtension::setBootDisplay(BootDisplay mode) {
  if (_bootDisplay == mode)
    return;
  _bootDisplay = mode;
  emit modified(this);
}

bool
AnytoneBootSettingsExtension::bootPasswordEnabled() const {
  return _bootPasswordEnabled;
}
void
AnytoneBootSettingsExtension::enableBootPassword(bool enable) {
  if (_bootPasswordEnabled == enable)
    return;
  _bootPasswordEnabled = enable;
  emit modified(this);
}

const QString &
AnytoneBootSettingsExtension::bootPassword() const {
  return _bootPassword;
}
void
AnytoneBootSettingsExtension::setBootPassword(const QString &pass) {
  if (_bootPassword == pass)
    return;
  _bootPassword = pass;
  emit modified(this);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneKeySettingsExtension
 * ********************************************************************************************* */
AnytoneKeySettingsExtension::AnytoneKeySettingsExtension(QObject *parent)
  : ConfigItem(parent)
{
  // pass...
}

ConfigItem *
AnytoneKeySettingsExtension::clone() const {
  AnytoneKeySettingsExtension *ext = new AnytoneKeySettingsExtension();
  if (! ext->copy(*this)) {
    ext->deleteLater();
    return nullptr;
  }
  return ext;
}

AnytoneKeySettingsExtension::KeyFunction
AnytoneKeySettingsExtension::progFuncKey1Short() const {
  return _progFuncKey1Short;
}
void
AnytoneKeySettingsExtension::setProgFuncKey1Short(KeyFunction func) {
  if (_progFuncKey1Short == func)
    return;
  _progFuncKey1Short = func;
  emit modified(this);
}
AnytoneKeySettingsExtension::KeyFunction
AnytoneKeySettingsExtension::progFuncKey1Long() const {
  return _progFuncKey1Long;
}
void
AnytoneKeySettingsExtension::setProgFuncKey1Long(KeyFunction func) {
  if (_progFuncKey1Long == func)
    return;
  _progFuncKey1Long = func;
  emit modified(this);
}

AnytoneKeySettingsExtension::KeyFunction
AnytoneKeySettingsExtension::progFuncKey2Short() const {
  return _progFuncKey2Short;
}
void
AnytoneKeySettingsExtension::setProgFuncKey2Short(KeyFunction func) {
  if (_progFuncKey2Short == func)
    return;
  _progFuncKey2Short = func;
  emit modified(this);
}
AnytoneKeySettingsExtension::KeyFunction
AnytoneKeySettingsExtension::progFuncKey2Long() const {
  return _progFuncKey2Long;
}
void
AnytoneKeySettingsExtension::setProgFuncKey2Long(KeyFunction func) {
  if (_progFuncKey2Long == func)
    return;
  _progFuncKey2Long = func;
  emit modified(this);
}

AnytoneKeySettingsExtension::KeyFunction
AnytoneKeySettingsExtension::progFuncKey3Short() const {
  return _progFuncKey3Short;
}
void
AnytoneKeySettingsExtension::setProgFuncKey3Short(KeyFunction func) {
  if (_progFuncKey3Short == func)
    return;
  _progFuncKey3Short = func;
  emit modified(this);
}
AnytoneKeySettingsExtension::KeyFunction
AnytoneKeySettingsExtension::progFuncKey3Long() const {
  return _progFuncKey3Long;
}
void
AnytoneKeySettingsExtension::setProgFuncKey3Long(KeyFunction func) {
  if (_progFuncKey3Long == func)
    return;
  _progFuncKey3Long = func;
  emit modified(this);
}

AnytoneKeySettingsExtension::KeyFunction
AnytoneKeySettingsExtension::funcKey1Short() const {
  return _funcKey1Short;
}
void
AnytoneKeySettingsExtension::setFuncKey1Short(KeyFunction func) {
  if (_funcKey1Short == func)
    return;
  _funcKey1Short = func;
  emit modified(this);
}
AnytoneKeySettingsExtension::KeyFunction
AnytoneKeySettingsExtension::funcKey1Long() const {
  return _funcKey1Long;
}
void
AnytoneKeySettingsExtension::setFuncKey1Long(KeyFunction func) {
  if (_funcKey1Long == func)
    return;
  _funcKey1Long = func;
  emit modified(this);
}

AnytoneKeySettingsExtension::KeyFunction
AnytoneKeySettingsExtension::funcKey2Short() const {
  return _funcKey2Short;
}
void
AnytoneKeySettingsExtension::setFuncKey2Short(KeyFunction func) {
  if (_funcKey2Short == func)
    return;
  _funcKey2Short = func;
  emit modified(this);
}
AnytoneKeySettingsExtension::KeyFunction
AnytoneKeySettingsExtension::funcKey2Long() const {
  return _funcKey2Long;
}
void
AnytoneKeySettingsExtension::setFuncKey2Long(KeyFunction func) {
  if (_funcKey2Long == func)
    return;
  _funcKey2Long = func;
  emit modified(this);
}

unsigned int
AnytoneKeySettingsExtension::longPressDuration() const {
  return _longPressDuration;
}
void
AnytoneKeySettingsExtension::setLongPressDuration(unsigned int ms) {
  if (_longPressDuration == ms)
    return;
  _longPressDuration = ms;
  emit modified(this);
}
