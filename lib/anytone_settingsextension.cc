#include "anytone_settingsextension.hh"
#include <QTimeZone>



/* ********************************************************************************************* *
 * Implementation of AnytoneDMRSettingsExtension
 * ********************************************************************************************* */
AnytoneDMRSettingsExtension::AnytoneDMRSettingsExtension(QObject *parent)
  : ConfigItem(parent), _groupCallHangTime(Interval::fromSeconds(3)),
    _manualGroupCallHangTime(Interval::fromSeconds(5)),
    _privateCallHangTime(Interval::fromSeconds(5)),
    _manualPrivateCallHangTime(Interval::fromSeconds(7)),
    _preWaveDelay(Interval::fromMilliseconds(100)),
    _wakeHeadPeriod(Interval::fromMilliseconds(100)), _filterOwnID(true),
    _monitorSlotMatch(SlotMatch::Off), _monitorColorCodeMatch(false), _monitorIDMatch(false),
    _monitorTimeSlotHold(true), _smsFormat(SMSFormat::Motorola), _sendTalkerAlias(false),
    _talkerAliasSource(TalkerAliasSource::UserDB), _talkerAliasEncoding(TalkerAliasEncoding::ISO7),
    _encryption(EncryptionType::DMR)
{
  // pass...
}

ConfigItem *
AnytoneDMRSettingsExtension::clone() const {
  AnytoneDMRSettingsExtension *ext = new AnytoneDMRSettingsExtension();
  if (! ext->copy(*this)) {
    ext->deleteLater();
    return nullptr;
  }
  return ext;
}

Interval
AnytoneDMRSettingsExtension::groupCallHangTime() const {
  return _groupCallHangTime;
}
void
AnytoneDMRSettingsExtension::setGroupCallHangTime(Interval sec) {
  if (_groupCallHangTime == sec)
    return;
  _groupCallHangTime = sec;
  emit modified(this);
}

Interval
AnytoneDMRSettingsExtension::manualGroupCallHangTime() const {
  return _manualGroupCallHangTime;
}
void
AnytoneDMRSettingsExtension::setManualGroupCallHangTime(Interval sec) {
  if (_manualGroupCallHangTime == sec)
    return;
  _manualGroupCallHangTime = sec;
  emit modified(this);
}

Interval
AnytoneDMRSettingsExtension::privateCallHangTime() const {
  return _privateCallHangTime;
}
void
AnytoneDMRSettingsExtension::setPrivateCallHangTime(Interval sec) {
  if (_privateCallHangTime == sec)
    return;
  _privateCallHangTime = sec;
  emit modified(this);
}

Interval
AnytoneDMRSettingsExtension::manualPrivateCallHangTime() const {
  return _manualPrivateCallHangTime;
}
void
AnytoneDMRSettingsExtension::setManualPrivateCallHangTime(Interval sec) {
  if (_manualPrivateCallHangTime == sec)
    return;
  _manualPrivateCallHangTime = sec;
  emit modified(this);
}

Interval
AnytoneDMRSettingsExtension::preWaveDelay() const {
  return _preWaveDelay;
}
void
AnytoneDMRSettingsExtension::setPreWaveDelay(Interval ms) {
  if (_preWaveDelay == ms)
    return;
  _preWaveDelay = ms;
  emit modified(this);
}

Interval
AnytoneDMRSettingsExtension::wakeHeadPeriod() const {
  return _wakeHeadPeriod;
}
void
AnytoneDMRSettingsExtension::setWakeHeadPeriod(Interval ms) {
  if (_wakeHeadPeriod == ms)
    return;
  _wakeHeadPeriod = ms;
  emit modified(this);
}

bool
AnytoneDMRSettingsExtension::filterOwnIDEnabled() const {
  return _filterOwnID;
}
void
AnytoneDMRSettingsExtension::enableFilterOwnID(bool enable) {
  if (_filterOwnID == enable)
    return;
  _filterOwnID = enable;
  emit modified(this);
}

AnytoneDMRSettingsExtension::SlotMatch
AnytoneDMRSettingsExtension::monitorSlotMatch() const {
  return _monitorSlotMatch;
}
void
AnytoneDMRSettingsExtension::setMonitorSlotMatch(SlotMatch match) {
  if (_monitorSlotMatch == match)
    return;
  _monitorSlotMatch = match;
  emit modified(this);
}

bool
AnytoneDMRSettingsExtension::monitorColorCodeMatchEnabled() const {
  return _monitorColorCodeMatch;
}
void
AnytoneDMRSettingsExtension::enableMonitorColorCodeMatch(bool enable) {
  if (_monitorColorCodeMatch == enable)
    return;
  _monitorColorCodeMatch = enable;
  emit modified(this);
}

bool
AnytoneDMRSettingsExtension::monitorIDMatchEnabled() const {
  return _monitorIDMatch;
}
void
AnytoneDMRSettingsExtension::enableMonitorIDMatch(bool enable) {
  if (_monitorIDMatch == enable)
    return;
  _monitorIDMatch = enable;
  emit modified(this);
}

bool
AnytoneDMRSettingsExtension::monitorTimeSlotHoldEnabled() const {
  return _monitorTimeSlotHold;
}
void
AnytoneDMRSettingsExtension::enableMonitorTimeSlotHold(bool enable) {
  if (_monitorTimeSlotHold == enable)
    return;
  _monitorTimeSlotHold = enable;
  emit modified(this);
}

AnytoneDMRSettingsExtension::SMSFormat
AnytoneDMRSettingsExtension::smsFormat() const {
  return _smsFormat;
}
void
AnytoneDMRSettingsExtension::setSMSFormat(SMSFormat format) {
  if (_smsFormat == format)
    return;
  _smsFormat = format;
  emit modified(this);
}

bool
AnytoneDMRSettingsExtension::sendTalkerAlias() const {
  return _sendTalkerAlias;
}
void
AnytoneDMRSettingsExtension::enableSendTalkerAlias(bool enable) {
  if (_sendTalkerAlias == enable)
    return;
  _sendTalkerAlias = enable;
  emit modified(this);
}

AnytoneDMRSettingsExtension::TalkerAliasSource
AnytoneDMRSettingsExtension::talkerAliasSource() const {
  return _talkerAliasSource;
}
void
AnytoneDMRSettingsExtension::setTalkerAliasSource(TalkerAliasSource mode) {
  if (mode == _talkerAliasSource)
    return;
  _talkerAliasSource = mode;
  emit modified(this);
}

AnytoneDMRSettingsExtension::TalkerAliasEncoding
AnytoneDMRSettingsExtension::talkerAliasEncoding() const {
  return _talkerAliasEncoding;
}
void
AnytoneDMRSettingsExtension::setTalkerAliasEncoding(TalkerAliasEncoding encoding) {
  if (_talkerAliasEncoding == encoding)
    return;
  _talkerAliasEncoding = encoding;
  emit modified(this);
}

AnytoneDMRSettingsExtension::EncryptionType
AnytoneDMRSettingsExtension::encryption() const {
  return _encryption;
}
void
AnytoneDMRSettingsExtension::setEncryption(EncryptionType type) {
  if (type == _encryption)
    return;
  _encryption = type;
  emit modified(this);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneGPSSettingsExtension
 * ********************************************************************************************* */
AnytoneGPSSettingsExtension::AnytoneGPSSettingsExtension(QObject *parent)
  : ConfigItem(parent), _gpsUnits(Units::Metric), _timeZone(QTimeZone::utc()),
    _gpsRangeReporting(false), _gpsRangingInterval(Interval::fromSeconds(300)), _mode(GPSMode::GPS)
{
  // pass...
}

ConfigItem *
AnytoneGPSSettingsExtension::clone() const {
  auto *ext = new AnytoneGPSSettingsExtension();
  if (! ext->copy(*this)) {
    ext->deleteLater();
    return nullptr;
  }
  return ext;
}

AnytoneGPSSettingsExtension::Units
AnytoneGPSSettingsExtension::units() const {
  return _gpsUnits;
}
void
AnytoneGPSSettingsExtension::setUnits(Units units) {
  if (_gpsUnits == units)
    return;
  _gpsUnits = units;
  emit modified(this);
}

QTimeZone
AnytoneGPSSettingsExtension::timeZone() const {
  return _timeZone;
}
QString
AnytoneGPSSettingsExtension::ianaTimeZone() const {
  return QString::fromLocal8Bit(_timeZone.id());
}
void
AnytoneGPSSettingsExtension::setTimeZone(const QTimeZone &zone) {
  if (_timeZone == zone)
    return;
  _timeZone = zone;
  emit modified(this);
}
void
AnytoneGPSSettingsExtension::setIANATimeZone(const QString &id) {
  setTimeZone(QTimeZone(id.toLocal8Bit()));
}

bool
AnytoneGPSSettingsExtension::positionReportingEnabled() const {
  return _gpsRangeReporting;
}
void
AnytoneGPSSettingsExtension::enablePositionReporting(bool enable) {
  if (_gpsRangeReporting == enable)
    return;
  _gpsRangeReporting = enable;
  emit modified(this);
}

Interval
AnytoneGPSSettingsExtension::updatePeriod() const {
  return _gpsRangingInterval;
}
void
AnytoneGPSSettingsExtension::setUpdatePeriod(Interval sec) {
  if (_gpsRangingInterval == sec)
    return;
  _gpsRangingInterval = sec;
  emit modified(this);
}

AnytoneGPSSettingsExtension::GPSMode
AnytoneGPSSettingsExtension::mode() const {
  return _mode;
}
void
AnytoneGPSSettingsExtension::setMode(GPSMode mode) {
  if (mode == _mode)
    return;
  _mode = mode;
  emit modified(this);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneRangingSettingsExtension
 * ********************************************************************************************* */
AnytoneRoamingSettingsExtension::AnytoneRoamingSettingsExtension(QObject *parent)
  : ConfigItem(parent),
    _autoRoam(false), _autoRoamPeriod(Interval::fromMinutes(1)), _autoRoamDelay(),
    _repeaterRangeCheck(false), _repeaterCheckInterval(Interval::fromSeconds(5)), _repeaterRangeCheckCount(3),
    _outOfRangeAlert(OutOfRangeAlert::None),
    _roamingStartCondition(RoamStart::Periodic), _roamingReturnCondition(RoamStart::Periodic),
    _notification(false), _notificationCount(1),
    _gpsRoaming(false), _defaultRoamingZone(new RoamingZoneReference(this))
{
  // pass...
}

ConfigItem *
AnytoneRoamingSettingsExtension::clone() const {
  AnytoneRoamingSettingsExtension *ext = new AnytoneRoamingSettingsExtension();
  if (! ext->copy(*this)) {
    ext->deleteLater();
    return nullptr;
  }
  return ext;
}

bool
AnytoneRoamingSettingsExtension::autoRoam() const {
  return _autoRoam;
}
void
AnytoneRoamingSettingsExtension::enableAutoRoam(bool enable) {
  if (enable == _autoRoam)
    return;
  _autoRoam = enable;
  emit modified(this);
}

Interval
AnytoneRoamingSettingsExtension::autoRoamPeriod() const {
  return _autoRoamPeriod;
}
void
AnytoneRoamingSettingsExtension::setAutoRoamPeriod(Interval min) {
  if (_autoRoamPeriod == min)
    return;
  _autoRoamPeriod = min;
  emit modified(this);
}

Interval
AnytoneRoamingSettingsExtension::autoRoamDelay() const {
  return _autoRoamDelay;
}
void
AnytoneRoamingSettingsExtension::setAutoRoamDelay(Interval min) {
  if (_autoRoamDelay == min)
    return;
  _autoRoamDelay = min;
  emit modified(this);
}

bool
AnytoneRoamingSettingsExtension::repeaterRangeCheckEnabled() const {
  return _repeaterRangeCheck;
}
void
AnytoneRoamingSettingsExtension::enableRepeaterRangeCheck(bool enable) {
  if (_repeaterRangeCheck == enable)
    return;
  _repeaterRangeCheck = enable;
  emit modified(this);
}
Interval
AnytoneRoamingSettingsExtension::repeaterCheckInterval() const {
  return _repeaterCheckInterval;
}
void
AnytoneRoamingSettingsExtension::setRepeaterCheckInterval(Interval sec) {
  if (_repeaterCheckInterval == sec)
    return;
  _repeaterCheckInterval = sec;
  emit modified(this);
}

unsigned int
AnytoneRoamingSettingsExtension::repeaterRangeCheckCount() const {
  return _repeaterRangeCheckCount;
}
void
AnytoneRoamingSettingsExtension::setRepeaterRangeCheckCount(unsigned int sec) {
  if (_repeaterRangeCheckCount == sec)
    return;
  _repeaterRangeCheckCount = sec;
  emit modified(this);
}

AnytoneRoamingSettingsExtension::OutOfRangeAlert
AnytoneRoamingSettingsExtension::outOfRangeAlert() const {
  return _outOfRangeAlert;
}
void
AnytoneRoamingSettingsExtension::setOutOfRangeAlert(OutOfRangeAlert type) {
  if (type == _outOfRangeAlert)
    return;
  _outOfRangeAlert = type;
  emit modified(this);
}

AnytoneRoamingSettingsExtension::RoamStart
AnytoneRoamingSettingsExtension::roamingStartCondition() const {
  return _roamingStartCondition;
}
void
AnytoneRoamingSettingsExtension::setRoamingStartCondition(RoamStart start) {
  if (_roamingStartCondition == start)
    return;
  _roamingStartCondition = start;
  emit modified(this);
}

AnytoneRoamingSettingsExtension::RoamStart
AnytoneRoamingSettingsExtension::roamingReturnCondition() const {
  return _roamingReturnCondition;
}
void
AnytoneRoamingSettingsExtension::setRoamingReturnCondition(RoamStart end) {
  if (_roamingReturnCondition == end)
    return;
  _roamingReturnCondition = end;
  emit modified(this);
}

bool
AnytoneRoamingSettingsExtension::notificationEnabled() const {
  return _notification;
}
void
AnytoneRoamingSettingsExtension::enableNotification(bool enable) {
  if (_notification == enable)
    return;
  _notification = enable;
  emit modified(this);
}
unsigned int
AnytoneRoamingSettingsExtension::notificationCount() const {
  return _notificationCount;
}
void
AnytoneRoamingSettingsExtension::setNotificationCount(unsigned int n) {
  if (_notificationCount == n)
    return;
  _notificationCount = n;
  emit modified(this);
}

bool
AnytoneRoamingSettingsExtension::gpsRoaming() const {
  return _gpsRoaming;
}
void
AnytoneRoamingSettingsExtension::enableGPSRoaming(bool enable) {
  if (enable == _gpsRoaming)
    return;
  _gpsRoaming = enable;
  emit modified(this);
}

RoamingZoneReference *
AnytoneRoamingSettingsExtension::defaultZone() const {
  return _defaultRoamingZone;
}


/* ********************************************************************************************* *
 * Implementation of AnytoneSettingsExtension
 * ********************************************************************************************* */
AnytoneSettingsExtension::AnytoneSettingsExtension(QObject *parent)
  : ConfigExtension(parent),
    _bootSettings(new AnytoneBootSettingsExtension(this)),
    _powerSaveSettings(new AnytonePowerSaveSettingsExtension(this)),
    _keySettings(new AnytoneKeySettingsExtension(this)),
    _toneSettings(new AnytoneToneSettingsExtension(this)),
    _displaySettings(new AnytoneDisplaySettingsExtension(this)),
    _audioSettings(new AnytoneAudioSettingsExtension(this)),
    _menuSettings(new AnytoneMenuSettingsExtension(this)),
    _autoRepeaterSettings(new AnytoneAutoRepeaterSettingsExtension(this)),
    _dmrSettings(new AnytoneDMRSettingsExtension(this)),
    _gpsSettings(new AnytoneGPSSettingsExtension(this)),
    _roamingSettings(new AnytoneRoamingSettingsExtension(this)),
    _bluetoothSettings(new AnytoneBluetoothSettingsExtension(this)),
    _repeaterSettings(new AnytoneRepeaterSettingsExtension(this)),
    _satelliteSettings(new AnytoneSatelliteSettingsExtension(this)),
    _vfoScanType(VFOScanType::Time), _modeA(VFOMode::Memory), _modeB(VFOMode::Memory),
    _zoneA(), _zoneB(), _selectedVFO(VFO::A), _subChannel(true),
    _minVFOScanFrequencyUHF(Frequency::fromMHz(430)), _maxVFOScanFrequencyUHF(Frequency::fromMHz(440)),
    _minVFOScanFrequencyVHF(Frequency::fromMHz(144)), _maxVFOScanFrequencyVHF(Frequency::fromMHz(146)),
    _keepLastCaller(false), _vfoStep(Frequency::fromkHz(5)), _steType(STEType::Off), _steFrequency(0),
    _steDuration(Interval::fromMilliseconds(300)), _tbstFrequency(Frequency::fromHz(1750)),
    _proMode(false), _maintainCallChannel(false)
{
  connect(_bootSettings, &AnytoneBootSettingsExtension::modified,
          this, &AnytoneSettingsExtension::modified);
  connect(_keySettings, &AnytoneKeySettingsExtension::modified,
          this, &AnytoneSettingsExtension::modified);
  connect(_toneSettings, &AnytoneToneSettingsExtension::modified,
          this, &AnytoneSettingsExtension::modified);
  connect(_displaySettings, &AnytoneDisplaySettingsExtension::modified,
          this, &AnytoneSettingsExtension::modified);
  connect(_audioSettings, &AnytoneAudioSettingsExtension::modified,
          this, &AnytoneSettingsExtension::modified);
  connect(_menuSettings, &AnytoneMenuSettingsExtension::modified,
          this, &AnytoneSettingsExtension::modified);
  connect(_autoRepeaterSettings, &AnytoneAutoRepeaterSettingsExtension::modified,
          this, &AnytoneSettingsExtension::modified);
  connect(_dmrSettings, &AnytoneDMRSettingsExtension::modified,
          this, &AnytoneSettingsExtension::modified);
  connect(_roamingSettings, &AnytoneRoamingSettingsExtension::modified,
          this, &AnytoneSettingsExtension::modified);
  connect(_repeaterSettings, &AnytoneRepeaterSettingsExtension::modified,
          this, &AnytoneSettingsExtension::modified);
    connect(_satelliteSettings, &AnytoneSatelliteSettingsExtension::modified,
          this, &AnytoneSettingsExtension::modified);
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

AnytonePowerSaveSettingsExtension *
AnytoneSettingsExtension::powerSaveSettings() const {
  return _powerSaveSettings;
}

AnytoneKeySettingsExtension *
AnytoneSettingsExtension::keySettings() const {
  return _keySettings;
}

AnytoneToneSettingsExtension *
AnytoneSettingsExtension::toneSettings() const {
  return _toneSettings;
}

AnytoneDisplaySettingsExtension *
AnytoneSettingsExtension::displaySettings() const {
  return _displaySettings;
}

AnytoneAudioSettingsExtension *
AnytoneSettingsExtension::audioSettings() const {
  return _audioSettings;
}

AnytoneMenuSettingsExtension *
AnytoneSettingsExtension::menuSettings() const {
  return _menuSettings;
}

AnytoneAutoRepeaterSettingsExtension *
AnytoneSettingsExtension::autoRepeaterSettings() const {
  return _autoRepeaterSettings;
}

AnytoneDMRSettingsExtension *
AnytoneSettingsExtension::dmrSettings() const {
  return _dmrSettings;
}

AnytoneGPSSettingsExtension *
AnytoneSettingsExtension::gpsSettings() const {
  return _gpsSettings;
}

AnytoneRoamingSettingsExtension *
AnytoneSettingsExtension::roamingSettings() const {
  return _roamingSettings;
}

AnytoneBluetoothSettingsExtension *
AnytoneSettingsExtension::bluetoothSettings() const {
  return _bluetoothSettings;
}

AnytoneRepeaterSettingsExtension *
AnytoneSettingsExtension::repeaterSettings() const {
  return _repeaterSettings;
}

AnytoneSatelliteSettingsExtension *
AnytoneSettingsExtension::satelliteSettings() const {
  return _satelliteSettings;
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

AnytoneSettingsExtension::VFOMode
AnytoneSettingsExtension::modeA() const {
  return _modeA;
}
void
AnytoneSettingsExtension::setModeA(VFOMode mode) {
  if (_modeA == mode)
    return;
  _modeA = mode;
  emit modified(this);
}

AnytoneSettingsExtension::VFOMode
AnytoneSettingsExtension::modeB() const {
  return _modeB;
}
void
AnytoneSettingsExtension::setModeB(VFOMode mode) {
  if (_modeB == mode)
    return;
  _modeB = mode;
  emit modified(this);
}

ZoneReference *
AnytoneSettingsExtension::zoneA() {
  return &_zoneA;
}
const ZoneReference *
AnytoneSettingsExtension::zoneA() const {
  return &_zoneA;
}
ZoneReference *
AnytoneSettingsExtension::zoneB() {
  return &_zoneB;
}
const ZoneReference *
AnytoneSettingsExtension::zoneB() const {
  return &_zoneB;
}

AnytoneSettingsExtension::VFO
AnytoneSettingsExtension::selectedVFO() const {
  return _selectedVFO;
}
void
AnytoneSettingsExtension::setSelectedVFO(VFO vfo) {
  if (_selectedVFO == vfo)
    return;
  _selectedVFO = vfo;
  emit modified(this);
}

bool
AnytoneSettingsExtension::subChannelEnabled() const {
  return _subChannel;
}
void
AnytoneSettingsExtension::enableSubChannel(bool enable) {
  if (_subChannel == enable)
    return;
  _subChannel = enable;
  emit modified(this);
}

Frequency
AnytoneSettingsExtension::minVFOScanFrequencyUHF() const {
  return _minVFOScanFrequencyUHF;
}
void
AnytoneSettingsExtension::setMinVFOScanFrequencyUHF(Frequency hz) {
  if (_minVFOScanFrequencyUHF == hz)
    return;
  _minVFOScanFrequencyUHF = hz;
  emit modified(this);
}
Frequency
AnytoneSettingsExtension::maxVFOScanFrequencyUHF() const {
  return _maxVFOScanFrequencyUHF;
}
void
AnytoneSettingsExtension::setMaxVFOScanFrequencyUHF(Frequency hz) {
  if (_maxVFOScanFrequencyUHF == hz)
    return;
  _maxVFOScanFrequencyUHF = hz;
  emit modified(this);
}

Frequency
AnytoneSettingsExtension::minVFOScanFrequencyVHF() const {
  return _minVFOScanFrequencyVHF;
}
void
AnytoneSettingsExtension::setMinVFOScanFrequencyVHF(Frequency hz) {
  if (_minVFOScanFrequencyVHF == hz)
    return;
  _minVFOScanFrequencyVHF = hz;
  emit modified(this);
}
Frequency
AnytoneSettingsExtension::maxVFOScanFrequencyVHF() const {
  return _maxVFOScanFrequencyVHF;
}
void
AnytoneSettingsExtension::setMaxVFOScanFrequencyVHF(Frequency hz) {
  if (_maxVFOScanFrequencyVHF == hz)
    return;
  _maxVFOScanFrequencyVHF = hz;
  emit modified(this);
}

bool
AnytoneSettingsExtension::keepLastCallerEnabled() const {
  return _keepLastCaller;
}
void
AnytoneSettingsExtension::enableKeepLastCaller(bool enable) {
  if (_keepLastCaller == enable)
    return;
  _keepLastCaller = enable;
  emit modified(this);
}

Frequency
AnytoneSettingsExtension::vfoStep() const {
  return _vfoStep;
}
void
AnytoneSettingsExtension::setVFOStep(Frequency step) {
  if (_vfoStep == step)
    return;
  _vfoStep = step;
  emit modified(this);
}

AnytoneSettingsExtension::STEType
AnytoneSettingsExtension::steType() const {
  return _steType;
}
void
AnytoneSettingsExtension::setSTEType(STEType type) {
  if (_steType == type)
    return;
  _steType = type;
  emit modified(this);
}
double
AnytoneSettingsExtension::steFrequency() const {
  return _steFrequency;
}
void
AnytoneSettingsExtension::setSTEFrequency(double freq) {
  if (_steFrequency == freq)
    return;
  _steFrequency = freq;
  emit modified(this);
}

Interval
AnytoneSettingsExtension::steDuration() const {
  return _steDuration;
}
void
AnytoneSettingsExtension::setSTEDuration(Interval intv) {
  if (intv == _steDuration)
    return;
  _steDuration = intv;
  emit modified(this);
}

Frequency
AnytoneSettingsExtension::tbstFrequency() const {
  return _tbstFrequency;
}
void
AnytoneSettingsExtension::setTBSTFrequency(Frequency Hz) {
  if (_tbstFrequency == Hz)
    return;
  _tbstFrequency = Hz;
  emit modified(this);
}


bool
AnytoneSettingsExtension::proModeEnabled() const {
  return _proMode;
}

void
AnytoneSettingsExtension::enableProMode(bool enable) {
  if (_proMode == enable)
    return;
  _proMode = enable;
  emit modified(this);
}


bool
AnytoneSettingsExtension::maintainCallChannelEnabled() const {
  return _maintainCallChannel;
}

void
AnytoneSettingsExtension::enableMaintainCallChannel(bool enable) {
  if (_maintainCallChannel == enable)
    return;
  _maintainCallChannel = enable;
  emit modified(this);
}


AnytoneSettingsExtension::FanControl
AnytoneSettingsExtension::fan() const {
  return _fan;
}

void
AnytoneSettingsExtension::setFan(FanControl ctrl) {
  if (_fan == ctrl)
    return;
  _fan = ctrl;
  emit modified(this);
}



/* ********************************************************************************************* *
 * Implementation of AnytoneBootSettingsExtension
 * ********************************************************************************************* */
AnytoneBootSettingsExtension::AnytoneBootSettingsExtension(QObject *parent)
  : ConfigItem(parent), _bootDisplay(BootDisplay::Default), _bootPasswordEnabled(false),
    _defaultChannel(false), _zoneA(new ZoneReference(this)), _channelA(new ChannelReference(this)),
    _zoneB(new ZoneReference(this)), _channelB(new ChannelReference(this)),
    _priorityZoneA(new ZoneReference(this)), _priorityZoneB(new ZoneReference(this)),
    _gpsCheck(false), _reset(true)
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

bool
AnytoneBootSettingsExtension::defaultChannelEnabled() const {
  return _defaultChannel;
}
void
AnytoneBootSettingsExtension::enableDefaultChannel(bool enable) {
  if (_defaultChannel == enable)
    return;
  _defaultChannel = enable;
  emit modified(this);
}

ZoneReference *
AnytoneBootSettingsExtension::zoneA() const {
  return _zoneA;
}
ChannelReference *
AnytoneBootSettingsExtension::channelA() const {
  return _channelA;
}
ZoneReference *
AnytoneBootSettingsExtension::zoneB() const {
  return _zoneB;
}
ChannelReference *
AnytoneBootSettingsExtension::channelB() const {
  return _channelB;
}

ZoneReference *
AnytoneBootSettingsExtension::priorityZoneA() const {
  return _priorityZoneA;
}
ZoneReference *
AnytoneBootSettingsExtension::priorityZoneB() const {
  return _priorityZoneB;
}

bool
AnytoneBootSettingsExtension::gpsCheckEnabled() const {
  return _gpsCheck;
}
void
AnytoneBootSettingsExtension::enableGPSCheck(bool enable) {
  if (_gpsCheck == enable)
    return;
  _gpsCheck = enable;
  emit modified(this);
}

bool
AnytoneBootSettingsExtension::resetEnabled() const {
  return _reset;
}
void
AnytoneBootSettingsExtension::enableReset(bool enable) {
  if (_reset == enable)
    return;
  _reset = enable;
  emit modified(this);
}


/* ********************************************************************************************* *
 * Implementation of AnytonePowerSaveSettingsExtension
 * ********************************************************************************************* */
AnytonePowerSaveSettingsExtension::AnytonePowerSaveSettingsExtension(QObject *parent)
  : ConfigItem(parent), _autoShutDownDelay(), _resetAutoShutdownOnCall(true),
    _powerSave(PowerSave::Save50), _atpc(false)
{
  // pass...
}

ConfigItem *
AnytonePowerSaveSettingsExtension::clone() const {
  AnytonePowerSaveSettingsExtension *ext = new AnytonePowerSaveSettingsExtension();
  if (! ext->copy(*this)) {
    ext->deleteLater();
    return nullptr;
  }
  return ext;
}

Interval
AnytonePowerSaveSettingsExtension::autoShutdown() const {
  return _autoShutDownDelay;
}
void
AnytonePowerSaveSettingsExtension::setAutoShutdown(Interval intv) {
  if (_autoShutDownDelay == intv)
    return;
  _autoShutDownDelay = intv;
  emit modified(this);
}

bool
AnytonePowerSaveSettingsExtension::resetAutoShutdownOnCall() const {
  return _resetAutoShutdownOnCall;
}
void
AnytonePowerSaveSettingsExtension::enableResetAutoShutdownOnCall(bool enable) {
  if (enable == _resetAutoShutdownOnCall)
    return;
  _resetAutoShutdownOnCall = enable;
  emit modified(this);
}

AnytonePowerSaveSettingsExtension::PowerSave
AnytonePowerSaveSettingsExtension::powerSave() const {
  return _powerSave;
}
void
AnytonePowerSaveSettingsExtension::setPowerSave(PowerSave save) {
  if (_powerSave == save)
    return;
  _powerSave = save;
  emit modified(this);
}

bool
AnytonePowerSaveSettingsExtension::atpc() const {
  return _atpc;
}
void
AnytonePowerSaveSettingsExtension::enableATPC(bool enable) {
  if (enable == _atpc)
    return;
  _atpc = enable;
  emit modified(this);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneKeySettingsExtension
 * ********************************************************************************************* */
AnytoneKeySettingsExtension::AnytoneKeySettingsExtension(QObject *parent)
  : ConfigItem(parent),
    _funcKey1Short(KeyFunction::VOX), _funcKey1Long(KeyFunction::ToggleVFO),
    _funcKey2Short(KeyFunction::Reverse), _funcKey2Long(KeyFunction::Off),
    _funcKey3Short(KeyFunction::Power), _funcKey3Long(KeyFunction::Record),
    _funcKey4Short(KeyFunction::Repeater), _funcKey4Long(KeyFunction::SMS),
    _funcKey5Short(KeyFunction::Reverse), _funcKey5Long(KeyFunction::Dial),
    _funcKey6Short(KeyFunction::Encryption), _funcKey6Long(KeyFunction::Off),
    _funcKeyAShort(KeyFunction::Off), _funcKeyALong(KeyFunction::Encryption),
    _funcKeyBShort(KeyFunction::Voltage), _funcKeyBLong(KeyFunction::Call),
    _funcKeyCShort(KeyFunction::Power), _funcKeyCLong(KeyFunction::VOX),
    _funcKeyDShort(KeyFunction::Off), _funcKeyDLong(KeyFunction::Off),
    _funcKnobShort(KeyFunction::Off), _funcKnobLong(KeyFunction::Off),
    _longPressDuration(Interval::fromSeconds(1)), _upDownFunction(UpDownKeyFunction::Channel),
    _autoKeyLock(false), _knobLock(false), _keypadLock(false),
    _sideKeysLock(false), _forcedKeyLock(false)
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

AnytoneKeySettingsExtension::KeyFunction
AnytoneKeySettingsExtension::funcKey3Short() const {
  return _funcKey3Short;
}
void
AnytoneKeySettingsExtension::setFuncKey3Short(KeyFunction func) {
  if (_funcKey3Short == func)
    return;
  _funcKey3Short = func;
  emit modified(this);
}
AnytoneKeySettingsExtension::KeyFunction
AnytoneKeySettingsExtension::funcKey3Long() const {
  return _funcKey3Long;
}
void
AnytoneKeySettingsExtension::setFuncKey3Long(KeyFunction func) {
  if (_funcKey3Long == func)
    return;
  _funcKey3Long = func;
  emit modified(this);
}

AnytoneKeySettingsExtension::KeyFunction
AnytoneKeySettingsExtension::funcKey4Short() const {
  return _funcKey4Short;
}
void
AnytoneKeySettingsExtension::setFuncKey4Short(KeyFunction func) {
  if (_funcKey4Short == func)
    return;
  _funcKey4Short = func;
  emit modified(this);
}
AnytoneKeySettingsExtension::KeyFunction
AnytoneKeySettingsExtension::funcKey4Long() const {
  return _funcKey4Long;
}
void
AnytoneKeySettingsExtension::setFuncKey4Long(KeyFunction func) {
  if (_funcKey4Long == func)
    return;
  _funcKey4Long = func;
  emit modified(this);
}

AnytoneKeySettingsExtension::KeyFunction
AnytoneKeySettingsExtension::funcKey5Short() const {
  return _funcKey5Short;
}
void
AnytoneKeySettingsExtension::setFuncKey5Short(KeyFunction func) {
  if (_funcKey5Short == func)
    return;
  _funcKey5Short = func;
  emit modified(this);
}
AnytoneKeySettingsExtension::KeyFunction
AnytoneKeySettingsExtension::funcKey5Long() const {
  return _funcKey5Long;
}
void
AnytoneKeySettingsExtension::setFuncKey5Long(KeyFunction func) {
  if (_funcKey5Long == func)
    return;
  _funcKey5Long = func;
  emit modified(this);
}

AnytoneKeySettingsExtension::KeyFunction
AnytoneKeySettingsExtension::funcKey6Short() const {
  return _funcKey6Short;
}
void
AnytoneKeySettingsExtension::setFuncKey6Short(KeyFunction func) {
  if (_funcKey6Short == func)
    return;
  _funcKey6Short = func;
  emit modified(this);
}
AnytoneKeySettingsExtension::KeyFunction
AnytoneKeySettingsExtension::funcKey6Long() const {
  return _funcKey6Long;
}
void
AnytoneKeySettingsExtension::setFuncKey6Long(KeyFunction func) {
  if (_funcKey6Long == func)
    return;
  _funcKey6Long = func;
  emit modified(this);
}

AnytoneKeySettingsExtension::KeyFunction
AnytoneKeySettingsExtension::funcKeyAShort() const {
  return _funcKeyAShort;
}
void
AnytoneKeySettingsExtension::setFuncKeyAShort(KeyFunction func) {
  if (_funcKeyAShort == func)
    return;
  _funcKeyAShort = func;
  emit modified(this);
}
AnytoneKeySettingsExtension::KeyFunction
AnytoneKeySettingsExtension::funcKeyALong() const {
  return _funcKeyALong;
}
void
AnytoneKeySettingsExtension::setFuncKeyALong(KeyFunction func) {
  if (_funcKeyALong == func)
    return;
  _funcKeyALong = func;
  emit modified(this);
}

AnytoneKeySettingsExtension::KeyFunction
AnytoneKeySettingsExtension::funcKeyBShort() const {
  return _funcKeyBShort;
}
void
AnytoneKeySettingsExtension::setFuncKeyBShort(KeyFunction func) {
  if (_funcKeyBShort == func)
    return;
  _funcKeyBShort = func;
  emit modified(this);
}
AnytoneKeySettingsExtension::KeyFunction
AnytoneKeySettingsExtension::funcKeyBLong() const {
  return _funcKeyBLong;
}
void
AnytoneKeySettingsExtension::setFuncKeyBLong(KeyFunction func) {
  if (_funcKeyBLong == func)
    return;
  _funcKeyBLong = func;
  emit modified(this);
}

AnytoneKeySettingsExtension::KeyFunction
AnytoneKeySettingsExtension::funcKeyCShort() const {
  return _funcKeyCShort;
}
void
AnytoneKeySettingsExtension::setFuncKeyCShort(KeyFunction func) {
  if (_funcKeyCShort == func)
    return;
  _funcKeyCShort = func;
  emit modified(this);
}
AnytoneKeySettingsExtension::KeyFunction
AnytoneKeySettingsExtension::funcKeyCLong() const {
  return _funcKeyCLong;
}
void
AnytoneKeySettingsExtension::setFuncKeyCLong(KeyFunction func) {
  if (_funcKeyCLong == func)
    return;
  _funcKeyCLong = func;
  emit modified(this);
}

AnytoneKeySettingsExtension::KeyFunction
AnytoneKeySettingsExtension::funcKeyDShort() const {
  return _funcKeyDShort;
}
void
AnytoneKeySettingsExtension::setFuncKeyDShort(KeyFunction func) {
  if (_funcKeyDShort == func)
    return;
  _funcKeyDShort = func;
  emit modified(this);
}
AnytoneKeySettingsExtension::KeyFunction
AnytoneKeySettingsExtension::funcKeyDLong() const {
  return _funcKeyDLong;
}
void
AnytoneKeySettingsExtension::setFuncKeyDLong(KeyFunction func) {
  if (_funcKeyDLong == func)
    return;
  _funcKeyDLong = func;
  emit modified(this);
}


AnytoneKeySettingsExtension::KeyFunction
AnytoneKeySettingsExtension::funcKnobShort() const {
  return _funcKnobShort;
}

void
AnytoneKeySettingsExtension::setFuncKnobShort(KeyFunction func) {
  if (_funcKnobShort == func)
    return;
  _funcKnobShort = func;
  emit modified(this);
}

AnytoneKeySettingsExtension::KeyFunction
AnytoneKeySettingsExtension::funcKnobLong() const {
  return _funcKnobLong;
}

void
AnytoneKeySettingsExtension::setFuncKnobLong(KeyFunction func) {
  if (_funcKnobLong == func)
    return;
  _funcKnobLong = func;
  emit modified(this);
}


Interval
AnytoneKeySettingsExtension::longPressDuration() const {
  return _longPressDuration;
}
void
AnytoneKeySettingsExtension::setLongPressDuration(Interval ms) {
  if (_longPressDuration == ms)
    return;
  _longPressDuration = ms;
  emit modified(this);
}


AnytoneKeySettingsExtension::UpDownKeyFunction
AnytoneKeySettingsExtension::upDownKeyFunction() const {
  return _upDownFunction;
}

void
AnytoneKeySettingsExtension::setUpDownKeyFunction(UpDownKeyFunction func) {
  if (_upDownFunction == func)
    return;
  _upDownFunction = func;
  emit modified(this);
}


bool
AnytoneKeySettingsExtension::autoKeyLockEnabled() const {
  return _autoKeyLock;
}
void
AnytoneKeySettingsExtension::enableAutoKeyLock(bool enabled) {
  if (_autoKeyLock==enabled)
    return;
  _autoKeyLock = enabled;
  emit modified(this);
}

bool
AnytoneKeySettingsExtension::knobLockEnabled() const {
  return _knobLock;
}
void
AnytoneKeySettingsExtension::enableKnobLock(bool enable) {
  if (_knobLock == enable)
    return;
  _knobLock = enable;
  emit modified(this);
}
bool
AnytoneKeySettingsExtension::keypadLockEnabled() const {
  return _keypadLock;
}
void
AnytoneKeySettingsExtension::enableKeypadLock(bool enable) {
  if (_keypadLock == enable)
    return;
  _keypadLock = enable;
  emit modified(this);
}
bool
AnytoneKeySettingsExtension::sideKeysLockEnabled() const {
  return _sideKeysLock;
}
void
AnytoneKeySettingsExtension::enableSideKeysLock(bool enable) {
  if (_sideKeysLock == enable)
    return;
  _sideKeysLock = enable;
  emit modified(this);
}
bool
AnytoneKeySettingsExtension::forcedKeyLockEnabled() const {
  return _forcedKeyLock;
}
void
AnytoneKeySettingsExtension::enableForcedKeyLock(bool enable) {
  if (_forcedKeyLock == enable)
    return;
  _forcedKeyLock = enable;
  emit modified(this);
}



/* ********************************************************************************************* *
 * Implementation of AnytoneToneSettingsExtension
 * ********************************************************************************************* */
AnytoneToneSettingsExtension::AnytoneToneSettingsExtension(QObject *parent)
  : ConfigItem(parent), _keyTone(false), _smsAlert(false), _callAlert(false),
    _talkPermitDigital(false), _talkPermitAnalog(false), _resetToneDigital(false),
    _dmrIdleChannelTone(false), _fmIdleChannelTone(false), _startupTone(false),
    _totNotification(false), _wxAlarm(false),
    _callMelody(new Melody(100, this)), _idleMelody(new Melody(100, this)),
    _resetMelody(new Melody(100, this)), _callEndMelody(new Melody(100, this)),
    _keyToneLevel(0)
{
  connect(_callMelody, &Melody::modified, this, &AnytoneToneSettingsExtension::modified);
  connect(_idleMelody, &Melody::modified, this, &AnytoneToneSettingsExtension::modified);
  connect(_resetMelody, &Melody::modified, this, &AnytoneToneSettingsExtension::modified);
}

ConfigItem *
AnytoneToneSettingsExtension::clone() const {
  AnytoneToneSettingsExtension *ext = new AnytoneToneSettingsExtension();
  if (! ext->copy(*this)) {
    ext->deleteLater();
    return nullptr;
  }
  return ext;
}

bool
AnytoneToneSettingsExtension::keyToneEnabled() const {
  return _keyTone;
}
void
AnytoneToneSettingsExtension::enableKeyTone(bool enable) {
  if (_keyTone==enable)
    return;
  _keyTone = enable;
  emit modified(this);
}

bool
AnytoneToneSettingsExtension::smsAlertEnabled() const {
  return _smsAlert;
}
void
AnytoneToneSettingsExtension::enableSMSAlert(bool enable) {
  if (_smsAlert == enable)
    return;
  _smsAlert = enable;
  emit modified(this);
}

bool
AnytoneToneSettingsExtension::callAlertEnabled() const {
  return _callAlert;
}
void
AnytoneToneSettingsExtension::enableCallAlert(bool enable) {
  if (_callAlert == enable)
    return;
  _callAlert = enable;
  emit modified(this);
}

bool
AnytoneToneSettingsExtension::talkPermitDigitalEnabled() const {
  return _talkPermitDigital;
}
void
AnytoneToneSettingsExtension::enableTalkPermitDigital(bool enable) {
  if (_talkPermitDigital == enable)
    return;
  _talkPermitDigital = enable;
  emit modified(this);
}

bool
AnytoneToneSettingsExtension::talkPermitAnalogEnabled() const {
  return _talkPermitAnalog;
}
void
AnytoneToneSettingsExtension::enableTalkPermitAnalog(bool enable) {
  if (_talkPermitAnalog == enable)
    return;
  _talkPermitAnalog = enable;
  emit modified(this);
}

bool
AnytoneToneSettingsExtension::digitalResetToneEnabled() const {
  return _resetToneDigital;
}
void
AnytoneToneSettingsExtension::enableDigitalResetTone(bool enable) {
  if (_resetToneDigital == enable)
    return;
  _resetToneDigital = enable;
  emit modified(this);
}

bool
AnytoneToneSettingsExtension::dmrIdleChannelToneEnabled() const {
  return _dmrIdleChannelTone;
}
void
AnytoneToneSettingsExtension::enableDMRIdleChannelTone(bool enable) {
  if (_dmrIdleChannelTone == enable)
    return;
  _dmrIdleChannelTone = enable;
  return modified(this);
}

bool
AnytoneToneSettingsExtension::fmIdleChannelToneEnabled() const {
  return _fmIdleChannelTone;
}
void
AnytoneToneSettingsExtension::enableFMIdleChannelTone(bool enable) {
  if (_fmIdleChannelTone == enable)
    return;
  _fmIdleChannelTone = enable;
  return modified(this);
}

bool
AnytoneToneSettingsExtension::startupToneEnabled() const {
  return _startupTone;
}
void
AnytoneToneSettingsExtension::enableStartupTone(bool enable) {
  if (_startupTone == enable)
    return;
  _startupTone = enable;
  return modified(this);
}

bool
AnytoneToneSettingsExtension::totNotification() const {
  return _totNotification;
}
void
AnytoneToneSettingsExtension::enableTOTNotification(bool enable) {
  if (enable == _totNotification)
    return;
  _totNotification = enable;
  emit modified(this);
}

bool
AnytoneToneSettingsExtension::wxAlarm() const {
  return _wxAlarm;
}
void
AnytoneToneSettingsExtension::enableWXAlarm(bool enable) {
  if (enable == _wxAlarm)
    return;
  _wxAlarm = enable;
  emit modified(this);
}

Melody *
AnytoneToneSettingsExtension::callMelody() const {
  return _callMelody;
}
Melody *
AnytoneToneSettingsExtension::idleMelody() const {
  return _idleMelody;
}
Melody *
AnytoneToneSettingsExtension::resetMelody() const {
  return _resetMelody;
}
Melody *
AnytoneToneSettingsExtension::callEndMelody() const {
  return _callEndMelody;
}

unsigned int
AnytoneToneSettingsExtension::keyToneLevel() const {
  return _keyToneLevel;
}
void
AnytoneToneSettingsExtension::setKeyToneLevel(unsigned int level) {
  if (_keyToneLevel == level)
    return;
  _keyToneLevel = level;
  emit modified(this);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneDisplaySettingsExtension
 * ********************************************************************************************* */
AnytoneDisplaySettingsExtension::AnytoneDisplaySettingsExtension(QObject *parent)
  : ConfigItem(parent), _displayFrequency(false), _brightness(5),
    _volumeChangePrompt(true), _callEndPrompt(true),
    _lastCallerDisplay(LastCallerDisplayMode::Both), _showClock(true), _showCall(true),
    _callColor(Color::Orange), _language(Language::English), _dateFormat(DateFormat::DayFirst),
    _showChannelNumber(true), _showGlobalChannelNumber(false), _showColorCode(true),
    _showTimeSlot(true), _showChannelType(true), _showContact(true), _standbyTextColor(Color::White),
    _standbyBackgroundColor(Color::Black), _showLastHeard(false), _backlightDurationTX(),
    _backlightDurationRX(), _customChannelBackground(false), _channelNameColor(Color::Orange),
    _channelBNameColor(Color::Orange), _zoneNameColor(Color::White), _zoneBNameColor(Color::White)
{
  // pass...
}

ConfigItem *
AnytoneDisplaySettingsExtension::clone() const {
  AnytoneDisplaySettingsExtension *ext = new AnytoneDisplaySettingsExtension();
  if (! ext->copy(*this)) {
    ext->deleteLater();
    return nullptr;
  }
  return ext;
}

unsigned int
AnytoneDisplaySettingsExtension::brightness() const {
  return _brightness;
}
void
AnytoneDisplaySettingsExtension::setBrightness(unsigned int level) {
  if (_brightness == level)
    return;
  _brightness = level;
  emit modified(this);
}

bool
AnytoneDisplaySettingsExtension::displayFrequencyEnabled() const {
  return _displayFrequency;
}
void
AnytoneDisplaySettingsExtension::enableDisplayFrequency(bool enable) {
  if (_displayFrequency == enable)
    return;
  _displayFrequency = enable;
  emit modified(this);
}

bool
AnytoneDisplaySettingsExtension::volumeChangePromptEnabled() const {
  return _volumeChangePrompt;
}
void
AnytoneDisplaySettingsExtension::enableVolumeChangePrompt(bool enable) {
  if (_volumeChangePrompt == enable)
    return;
  _volumeChangePrompt = enable;
  emit modified(this);
}

bool
AnytoneDisplaySettingsExtension::callEndPromptEnabled() const {
  return _callEndPrompt;
}
void
AnytoneDisplaySettingsExtension::enableCallEndPrompt(bool enable) {
  if (_callEndPrompt == enable)
    return;
  _callEndPrompt = enable;
  emit modified(this);
}

AnytoneDisplaySettingsExtension::LastCallerDisplayMode
AnytoneDisplaySettingsExtension::lastCallerDisplay() const {
  return _lastCallerDisplay;
}
void
AnytoneDisplaySettingsExtension::setLastCallerDisplay(LastCallerDisplayMode mode) {
  if (_lastCallerDisplay == mode)
    return;
  _lastCallerDisplay = mode;
  emit modified(this);
}

bool
AnytoneDisplaySettingsExtension::showClockEnabled() const {
  return _showClock;
}
void
AnytoneDisplaySettingsExtension::enableShowClock(bool enable) {
  if (_showClock == enable)
    return;
  _showClock = enable;
  emit modified(this);
}

bool
AnytoneDisplaySettingsExtension::showCallEnabled() const {
  return _showCall;
}
void
AnytoneDisplaySettingsExtension::enableShowCall(bool enable) {
  if (_showCall == enable)
    return;
  _showCall = enable;
  emit modified(this);
}

AnytoneDisplaySettingsExtension::Color
AnytoneDisplaySettingsExtension::callColor() const {
  return _callColor;
}
void
AnytoneDisplaySettingsExtension::setCallColor(Color color) {
  if (_callColor == color)
    return;
  _callColor = color;
  emit modified(this);
}

AnytoneDisplaySettingsExtension::Language
AnytoneDisplaySettingsExtension::language() const {
  return _language;
}
void
AnytoneDisplaySettingsExtension::setLanguage(Language lang) {
  if (_language == lang)
    return;
  _language = lang;
  emit modified(this);
}

AnytoneDisplaySettingsExtension::DateFormat
AnytoneDisplaySettingsExtension::dateFormat() const {
  return _dateFormat;
}
void
AnytoneDisplaySettingsExtension::setDateFormat(DateFormat format) {
  if (format == _dateFormat)
    return;
  _dateFormat = format;
  emit modified(this);
}

bool
AnytoneDisplaySettingsExtension::showChannelNumberEnabled() const {
  return _showChannelNumber;
}
void
AnytoneDisplaySettingsExtension::enableShowChannelNumber(bool enable) {
  if (_showChannelNumber == enable)
    return;
  _showChannelNumber = enable;
  emit modified(this);
}

bool
AnytoneDisplaySettingsExtension::showGlobalChannelNumber() const {
  return _showGlobalChannelNumber;
}
void
AnytoneDisplaySettingsExtension::enableShowGlobalChannelNumber(bool enable) {
  if (_showGlobalChannelNumber == enable)
    return;
  _showGlobalChannelNumber = enable;
  emit modified(this);
}

bool
AnytoneDisplaySettingsExtension::showColorCode() const {
  return _showColorCode;
}
void
AnytoneDisplaySettingsExtension::enableShowColorCode(bool enable) {
  if (_showColorCode == enable)
    return;
  _showColorCode = enable;
  emit modified(this);
}

bool
AnytoneDisplaySettingsExtension::showTimeSlot() const {
  return _showTimeSlot;
}
void
AnytoneDisplaySettingsExtension::enableShowTimeSlot(bool enable) {
  if (_showTimeSlot == enable)
    return;
  _showTimeSlot = enable;
  emit modified(this);
}

bool
AnytoneDisplaySettingsExtension::showChannelType() const {
  return _showChannelType;
}
void
AnytoneDisplaySettingsExtension::enableShowChannelType(bool enable) {
  if (_showChannelType == enable)
    return;
  _showChannelType = enable;
  emit modified(this);
}

bool
AnytoneDisplaySettingsExtension::showContact() const {
  return _showContact;
}
void
AnytoneDisplaySettingsExtension::enableShowContact(bool enable) {
  if (_showContact == enable)
    return;
  _showContact = enable;
  emit modified(this);
}

bool
AnytoneDisplaySettingsExtension::showLastHeardEnabled() const {
  return _showLastHeard;
}
void
AnytoneDisplaySettingsExtension::enableShowLastHeard(bool enable) {
  if (_showLastHeard == enable)
    return;
  _showLastHeard = enable;
  emit modified(this);
}

AnytoneDisplaySettingsExtension::Color
AnytoneDisplaySettingsExtension::standbyTextColor() const {
  return _standbyTextColor;
}
void
AnytoneDisplaySettingsExtension::setStandbyTextColor(Color color) {
  if (_standbyTextColor == color)
    return;
  _standbyTextColor = color;
  emit modified(this);
}

AnytoneDisplaySettingsExtension::Color
AnytoneDisplaySettingsExtension::standbyBackgroundColor() const {
  return _standbyBackgroundColor;
}
void
AnytoneDisplaySettingsExtension::setStandbyBackgroundColor(Color color) {
  if (_standbyBackgroundColor == color)
    return;
  _standbyBackgroundColor = color;
  emit modified(this);
}

Interval
AnytoneDisplaySettingsExtension::backlightDurationTX() const {
  return _backlightDurationTX;
}
void
AnytoneDisplaySettingsExtension::setBacklightDurationTX(Interval sec) {
  if (_backlightDurationTX == sec)
    return;
  _backlightDurationTX = sec;
  emit modified(this);
}

AnytoneDisplaySettingsExtension::Color
AnytoneDisplaySettingsExtension::channelNameColor() const {
  return _channelNameColor;
}
void
AnytoneDisplaySettingsExtension::setChannelNameColor(Color color) {
  if (_channelNameColor == color)
    return;
  _channelNameColor = color;
  emit modified(this);
}

AnytoneDisplaySettingsExtension::Color
AnytoneDisplaySettingsExtension::channelBNameColor() const {
  return _channelBNameColor;
}
void
AnytoneDisplaySettingsExtension::setChannelBNameColor(Color color) {
  if (_channelBNameColor == color)
    return;
  _channelBNameColor = color;
  emit modified(this);
}

AnytoneDisplaySettingsExtension::Color
AnytoneDisplaySettingsExtension::zoneNameColor() const {
  return _zoneNameColor;
}
void
AnytoneDisplaySettingsExtension::setZoneNameColor(Color color) {
  if (_zoneNameColor == color)
    return;
  _zoneNameColor = color;
  emit modified(this);
}

AnytoneDisplaySettingsExtension::Color
AnytoneDisplaySettingsExtension::zoneBNameColor() const {
  return _zoneBNameColor;
}
void
AnytoneDisplaySettingsExtension::setZoneBNameColor(Color color) {
  if (_zoneBNameColor == color)
    return;
  _zoneBNameColor = color;
  emit modified(this);
}

Interval
AnytoneDisplaySettingsExtension::backlightDurationRX() const {
  return _backlightDurationRX;
}
void
AnytoneDisplaySettingsExtension::setBacklightDurationRX(Interval sec) {
  if (_backlightDurationRX == sec)
    return;
  _backlightDurationRX = sec;
  emit modified(this);
}

bool
AnytoneDisplaySettingsExtension::customChannelBackground() const {
  return _customChannelBackground;
}
void
AnytoneDisplaySettingsExtension::enableCustomChannelBackground(bool enable) {
  if (enable == _customChannelBackground)
    return;
  _customChannelBackground = enable;
  emit modified(this);
}



/* ********************************************************************************************* *
 * Implementation of AnytoneAudioSettingsExtension
 * ********************************************************************************************* */
AnytoneAudioSettingsExtension::AnytoneAudioSettingsExtension(QObject *parent)
  : ConfigItem(parent),  _voxDelay(), _recording(false), _voxSource(VoxSource::Both),
  _maxVolume(3), _maxHeadPhoneVolume(3), _enhanceAudio(true), _muteDelay(Interval::fromMinutes(1)),
  _enableAnalogMicGain(false), _analogMicGain(1), _speaker(Speaker::Radio),
  _handsetSpeaker(HandsetSpeakerSource::MainChannel), _handsetType(HandsetType::Anytone)
{
  // pass...
}

ConfigItem *
AnytoneAudioSettingsExtension::clone() const {
  AnytoneAudioSettingsExtension *ext = new AnytoneAudioSettingsExtension();
  if (! ext->copy(*this)) {
    ext->deleteLater();
    return nullptr;
  }
  return ext;
}

Interval
AnytoneAudioSettingsExtension::voxDelay() const {
  return _voxDelay;
}
void
AnytoneAudioSettingsExtension::setVOXDelay(Interval ms) {
  if (_voxDelay == ms)
    return;
  _voxDelay = ms;
  emit modified(this);
}

bool
AnytoneAudioSettingsExtension::recordingEnabled() const {
  return _recording;
}
void
AnytoneAudioSettingsExtension::enableRecording(bool enable) {
  if (_recording == enable)
    return;
  _recording = enable;
  emit modified(this);
}

AnytoneAudioSettingsExtension::VoxSource
AnytoneAudioSettingsExtension::voxSource() const {
  return _voxSource;
}
void
AnytoneAudioSettingsExtension::setVOXSource(VoxSource source) {
  if (_voxSource == source)
    return;
  _voxSource = source;
  emit modified(this);
}

unsigned int
AnytoneAudioSettingsExtension::maxVolume() const {
  return _maxVolume;
}
void
AnytoneAudioSettingsExtension::setMaxVolume(unsigned int vol) {
  if (_maxVolume == vol)
    return;
  _maxVolume = vol;
  emit modified(this);
}
unsigned int
AnytoneAudioSettingsExtension::maxHeadPhoneVolume() const {
  return _maxHeadPhoneVolume;
}
void
AnytoneAudioSettingsExtension::setMaxHeadPhoneVolume(unsigned int vol) {
  if (_maxHeadPhoneVolume == vol)
    return;
  _maxHeadPhoneVolume = vol;
  emit modified(this);
}

bool
AnytoneAudioSettingsExtension::enhanceAudioEnabled() const {
  return _enhanceAudio;
}
void
AnytoneAudioSettingsExtension::enableEnhanceAudio(bool enable) {
  if (_enhanceAudio == enable)
    return;
  _enhanceAudio = enable;
  emit modified(this);
}

Interval
AnytoneAudioSettingsExtension::muteDelay() const {
  return _muteDelay;
}
void
AnytoneAudioSettingsExtension::setMuteDelay(Interval intv) {
  if (_muteDelay == intv)
    return;
  _muteDelay = intv;
  emit modified(this);
}

bool
AnytoneAudioSettingsExtension::fmMicGainEnabled() const {
  return _enableAnalogMicGain;
}
void
AnytoneAudioSettingsExtension::enableFMMicGain(bool enable) {
  if (_enableAnalogMicGain == enable)
    return;
  _enableAnalogMicGain = enable;
  emit modified(this);
}
unsigned int
AnytoneAudioSettingsExtension::fmMicGain() const {
  return _analogMicGain;
}
void
AnytoneAudioSettingsExtension::setFMMicGain(unsigned int gain) {
  if (_analogMicGain == gain)
    return;
  _analogMicGain = gain;
  emit modified(this);
}


AnytoneAudioSettingsExtension::Speaker
AnytoneAudioSettingsExtension::speaker() const {
  return _speaker;
}

void
AnytoneAudioSettingsExtension::setSpeaker(Speaker speaker) {
  if (_speaker == speaker)
    return;
  _speaker = speaker;
  emit modified(this);
}


AnytoneAudioSettingsExtension::HandsetSpeakerSource
AnytoneAudioSettingsExtension::handsetSpeaker() const {
  return _handsetSpeaker;
}

void
AnytoneAudioSettingsExtension::setHandsetSpeaker(HandsetSpeakerSource src) {
  if (_handsetSpeaker == src)
    return;
  _handsetSpeaker = src;
  emit modified(this);
}


AnytoneAudioSettingsExtension::HandsetType
AnytoneAudioSettingsExtension::handsetType() const {
  return _handsetType;
}

void
AnytoneAudioSettingsExtension::setHandsetType(HandsetType type) {
  if (_handsetType == type)
    return;
  _handsetType = type;
  emit modified(this);
}



/* ********************************************************************************************* *
 * Implementation of AnytoneMenuSettingsExtension
 * ********************************************************************************************* */
AnytoneMenuSettingsExtension::AnytoneMenuSettingsExtension(QObject *parent)
  : ConfigItem(parent),  _menuDuration(Interval::fromSeconds(15)), _showSeparator(false)
{
  // pass...
}

ConfigItem *
AnytoneMenuSettingsExtension::clone() const {
  AnytoneMenuSettingsExtension *ext = new AnytoneMenuSettingsExtension();
  if (! ext->copy(*this)) {
    ext->deleteLater();
    return nullptr;
  }
  return ext;
}

Interval
AnytoneMenuSettingsExtension::duration() const {
  return _menuDuration;
}
void
AnytoneMenuSettingsExtension::setDuration(Interval sec) {
  if (_menuDuration == sec)
    return;
  _menuDuration = sec;
  emit modified(this);
}

bool
AnytoneMenuSettingsExtension::separatorEnabled() const {
  return _showSeparator;
}
void
AnytoneMenuSettingsExtension::enableSeparator(bool enable) {
  if (_showSeparator == enable)
    return;
  _showSeparator = enable;
  emit modified(this);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneAutoRepeaterSettingsExtension
 * ********************************************************************************************* */
AnytoneAutoRepeaterSettingsExtension::AnytoneAutoRepeaterSettingsExtension(QObject *parent)
  : ConfigItem(parent), _directionA(Direction::Off), _directionB(Direction::Off),
    _minVHF(Frequency::fromMHz(136)), _maxVHF(Frequency::fromMHz(174)),
    _minUHF(Frequency::fromMHz(400)), _maxUHF(Frequency::fromMHz(480)),
    _vhfOffset(new AnytoneAutoRepeaterOffsetRef(this)),
    _uhfOffset(new AnytoneAutoRepeaterOffsetRef(this)),
    _minVHF2(Frequency::fromMHz(136)), _maxVHF2(Frequency::fromMHz(174)),
    _minUHF2(Frequency::fromMHz(400)), _maxUHF2(Frequency::fromMHz(480)),
    _vhf2Offset(new AnytoneAutoRepeaterOffsetRef(this)),
    _uhf2Offset(new AnytoneAutoRepeaterOffsetRef(this)),
    _offsets(new AnytoneAutoRepeaterOffsetList(this))
{
  // pass...
}

ConfigItem *
AnytoneAutoRepeaterSettingsExtension::clone() const {
  AnytoneAutoRepeaterSettingsExtension *ext = new AnytoneAutoRepeaterSettingsExtension();
  if (! ext->copy(*this)) {
    ext->deleteLater();
    return nullptr;
  }
  return ext;
}

AnytoneAutoRepeaterSettingsExtension::Direction
AnytoneAutoRepeaterSettingsExtension::directionA() const {
  return _directionA;
}
void
AnytoneAutoRepeaterSettingsExtension::setDirectionA(Direction dir) {
  if (_directionA == dir)
    return;
  _directionA = dir;
  emit modified(this);
}
AnytoneAutoRepeaterSettingsExtension::Direction
AnytoneAutoRepeaterSettingsExtension::directionB() const {
  return _directionB;
}
void
AnytoneAutoRepeaterSettingsExtension::setDirectionB(Direction dir) {
  if (_directionB == dir)
    return;
  _directionB = dir;
  emit modified(this);
}

Frequency
AnytoneAutoRepeaterSettingsExtension::vhfMin() const {
  return _minVHF;
}
void
AnytoneAutoRepeaterSettingsExtension::setVHFMin(Frequency Hz) {
  if (_minVHF == Hz)
    return;
  _minVHF = Hz;
  emit modified(this);
}
Frequency
AnytoneAutoRepeaterSettingsExtension::vhfMax() const {
  return _maxVHF;
}
void
AnytoneAutoRepeaterSettingsExtension::setVHFMax(Frequency Hz) {
  if (_maxVHF == Hz)
    return;
  _maxVHF = Hz;
  emit modified(this);
}
Frequency
AnytoneAutoRepeaterSettingsExtension::uhfMin() const {
  return _minUHF;
}
void
AnytoneAutoRepeaterSettingsExtension::setUHFMin(Frequency Hz) {
  if (_minUHF == Hz)
    return;
  _minUHF = Hz;
  emit modified(this);
}
Frequency
AnytoneAutoRepeaterSettingsExtension::uhfMax() const {
  return _maxUHF;
}
void
AnytoneAutoRepeaterSettingsExtension::setUHFMax(Frequency Hz) {
  if (_maxUHF == Hz)
    return;
  _maxUHF = Hz;
  emit modified(this);
}

AnytoneAutoRepeaterOffsetRef *
AnytoneAutoRepeaterSettingsExtension::vhfRef() const {
  return _vhfOffset;
}

AnytoneAutoRepeaterOffsetRef *
AnytoneAutoRepeaterSettingsExtension::uhfRef() const {
  return _uhfOffset;
}

Frequency
AnytoneAutoRepeaterSettingsExtension::vhf2Min() const {
  return _minVHF2;
}
void
AnytoneAutoRepeaterSettingsExtension::setVHF2Min(Frequency Hz) {
  if (_minVHF2 == Hz)
    return;
  _minVHF2 = Hz;
  emit modified(this);
}
Frequency
AnytoneAutoRepeaterSettingsExtension::vhf2Max() const {
  return _maxVHF2;
}
void
AnytoneAutoRepeaterSettingsExtension::setVHF2Max(Frequency Hz) {
  if (_maxVHF2 == Hz)
    return;
  _maxVHF2 = Hz;
  emit modified(this);
}
Frequency
AnytoneAutoRepeaterSettingsExtension::uhf2Min() const {
  return _minUHF2;
}
void
AnytoneAutoRepeaterSettingsExtension::setUHF2Min(Frequency Hz) {
  if (_minUHF2 == Hz)
    return;
  _minUHF2 = Hz;
  emit modified(this);
}
Frequency
AnytoneAutoRepeaterSettingsExtension::uhf2Max() const {
  return _maxUHF2;
}
void
AnytoneAutoRepeaterSettingsExtension::setUHF2Max(Frequency Hz) {
  if (_maxUHF2 == Hz)
    return;
  _maxUHF2 = Hz;
  emit modified(this);
}

AnytoneAutoRepeaterOffsetRef *
AnytoneAutoRepeaterSettingsExtension::vhf2Ref() const {
  return _vhf2Offset;
}

AnytoneAutoRepeaterOffsetRef *
AnytoneAutoRepeaterSettingsExtension::uhf2Ref() const {
  return _uhf2Offset;
}

AnytoneAutoRepeaterOffsetList *
AnytoneAutoRepeaterSettingsExtension::offsets() const {
  return _offsets;
}


/* ********************************************************************************************* *
 * Implementation of AnytoneAutoRepeaterOffset
 * ********************************************************************************************* */
AnytoneAutoRepeaterOffset::AnytoneAutoRepeaterOffset(QObject *parent)
  : ConfigObject(parent), _offset()
{
  // pass...
}

ConfigItem *
AnytoneAutoRepeaterOffset::clone() const {
  AnytoneAutoRepeaterOffset *off = new AnytoneAutoRepeaterOffset();
  if (! off->copy(*this)) {
    off->deleteLater();
    return nullptr;
  }
  return off;
}

Frequency
AnytoneAutoRepeaterOffset::offset() const {
  return _offset;
}
void
AnytoneAutoRepeaterOffset::setOffset(Frequency offset) {
  if (_offset == offset)
    return;
  _offset = offset;
  emit modified(this);
}

ConfigItem *
AnytoneAutoRepeaterOffsetList::allocateChild(const YAML::Node &node, ConfigItem::Context &ctx, const ErrorStack &err) {
  Q_UNUSED(node); Q_UNUSED(ctx); Q_UNUSED(err);
  return new AnytoneAutoRepeaterOffset();
}

/* ********************************************************************************************* *
 * Implementation of AnytoneAutoRepeaterOffsetRef
 * ********************************************************************************************* */
AnytoneAutoRepeaterOffsetRef::AnytoneAutoRepeaterOffsetRef(QObject *parent)
  : ConfigObjectReference(AnytoneAutoRepeaterOffset::staticMetaObject, parent)
{
  // pass...
}

/* ********************************************************************************************* *
 * Implementation of AnytoneAutoRepeaterOffsetList
 * ********************************************************************************************* */
AnytoneAutoRepeaterOffsetList::AnytoneAutoRepeaterOffsetList(QObject *parent)
  : ConfigObjectList(AnytoneAutoRepeaterOffset::staticMetaObject, parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of AnytoneBluetoothSettingsExtension
 * ********************************************************************************************* */
AnytoneBluetoothSettingsExtension::AnytoneBluetoothSettingsExtension(QObject *parent)
  : ConfigItem(parent), _bluetoothEnabled(false), _pttLatch(false),
    _pttSleep(Interval::fromMilliseconds(0)), _internalMic(false), _internalSpeaker(false),
    _micGain(0), _speakerGain(0), _holdDuration(), _holdDelay()
{
  // pass...
}

ConfigItem *
AnytoneBluetoothSettingsExtension::clone() const {
  AnytoneBluetoothSettingsExtension *ext = new AnytoneBluetoothSettingsExtension();
  if (! ext->copy(*this)) {
    ext->deleteLater();
    return nullptr;
  }
  return ext;
}

bool
AnytoneBluetoothSettingsExtension::pttLatch() const {
  return _pttLatch;
}
void
AnytoneBluetoothSettingsExtension::enablePTTLatch(bool enable) {
  if (enable == _pttLatch)
    return;
  _pttLatch = enable;
  emit modified(this);
}

bool
AnytoneBluetoothSettingsExtension::bluetoothEnabled() const {
  return _bluetoothEnabled;
}

void
AnytoneBluetoothSettingsExtension::enableBluetooth(bool enable) {
  if (enable == _bluetoothEnabled)
    return;
  _bluetoothEnabled = enable;
  emit modified(this);
}

Interval
AnytoneBluetoothSettingsExtension::pttSleepTimer() const {
  return _pttSleep;
}
void
AnytoneBluetoothSettingsExtension::setPTTSleepTimer(Interval intv) {
  if (intv == _pttSleep)
    return;
  _pttSleep = intv;
  emit modified(this);
}

bool
AnytoneBluetoothSettingsExtension::internalMicEnabled() const {
  return _internalMic;
}

void
AnytoneBluetoothSettingsExtension::enableInternalMic(bool enable) {
  if (enable == _internalMic)
    return;
  _internalMic = enable;
  emit modified(this);
}

bool
AnytoneBluetoothSettingsExtension::internalSpeakerEnabled() const {
  return _internalSpeaker;
}

void
AnytoneBluetoothSettingsExtension::enableInternalSpeaker(bool enable) {
  if (enable == _internalSpeaker)
    return;
  _internalSpeaker = enable;
  emit modified(this);
}

unsigned int
AnytoneBluetoothSettingsExtension::micGain() const {
  return _micGain;
}

void
AnytoneBluetoothSettingsExtension::setMicGain(unsigned int gain) {
  if (_micGain == gain)
    return;
  _micGain = gain;
  emit modified(this);
}

unsigned int
AnytoneBluetoothSettingsExtension::speakerGain() const {
  return _speakerGain;
}

void
AnytoneBluetoothSettingsExtension::setSpeakerGain(unsigned int gain) {
  if (_speakerGain == gain)
    return;
  _speakerGain = gain;
  emit modified(this);
}

const Interval &
AnytoneBluetoothSettingsExtension::holdDuration() const {
  return _holdDuration;
}

void
AnytoneBluetoothSettingsExtension::setHoldDuration(const Interval &dur) {
  if (dur == _holdDuration)
    return;
  _holdDuration = dur;
  emit modified(this);
}

const Interval &
AnytoneBluetoothSettingsExtension::holdDelay() const {
  return _holdDelay;
}

void
AnytoneBluetoothSettingsExtension::setHoldDelay(const Interval &dur) {
  if (dur == _holdDelay)
    return;
  _holdDelay = dur;
  emit modified(this);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneSimplexRepeaterSettingsExtension
 * ********************************************************************************************* */
AnytoneRepeaterSettingsExtension::AnytoneRepeaterSettingsExtension(QObject *parent)
  : ConfigItem(parent), _enabled(false), _monitor(false), _timeSlot(TimeSlot::Channel),
  _secTimeSlot(TimeSlot::Channel), _colorCode(ColorCode::Ignored)
{
  // pass...
}

ConfigItem *
AnytoneRepeaterSettingsExtension::clone() const {
  AnytoneRepeaterSettingsExtension *ext = new AnytoneRepeaterSettingsExtension();
  if (! ext->copy(*this)) {
    ext->deleteLater();
    return nullptr;
  }
  return ext;
}

bool
AnytoneRepeaterSettingsExtension::enabled() const {
  return _enabled;
}
void
AnytoneRepeaterSettingsExtension::enable(bool enable) {
  if (_enabled == enable)
    return;
  _enabled = enable;
  emit modified(this);
}

bool
AnytoneRepeaterSettingsExtension::monitorEnabled() const {
  return _monitor;
}
void
AnytoneRepeaterSettingsExtension::enableMonitor(bool enable) {
  if (_monitor == enable)
    return;
  _monitor = enable;
  emit modified(this);
}

AnytoneRepeaterSettingsExtension::TimeSlot
AnytoneRepeaterSettingsExtension::timeSlot() const {
  return _timeSlot;
}
void
AnytoneRepeaterSettingsExtension::setTimeSlot(TimeSlot ts) {
  if (_timeSlot == ts)
    return;
  _timeSlot = ts;
  emit modified(this);
}


AnytoneRepeaterSettingsExtension::TimeSlot
AnytoneRepeaterSettingsExtension::secTimeSlot() const {
  return _secTimeSlot;
}
void
AnytoneRepeaterSettingsExtension::setSecTimeSlot(TimeSlot ts) {
  if (_secTimeSlot == ts)
    return;
  _secTimeSlot = ts;
  emit modified(this);
}


AnytoneRepeaterSettingsExtension::ColorCode
AnytoneRepeaterSettingsExtension::colorCode() const {
  return _colorCode;
}

void
AnytoneRepeaterSettingsExtension::setColorCode(ColorCode code) {
  if (_colorCode == code)
    return;
  _colorCode = code;
  emit modified(this);
}



/* ********************************************************************************************* *
 * Implementation of AnytoneSatelliteSettingsExtension
 * ********************************************************************************************* */
AnytoneSatelliteSettingsExtension::AnytoneSatelliteSettingsExtension(QObject *parent)
  : ConfigItem{parent}, _power(Channel::Power::High), _squelch(0)
{
  // pass...
}

ConfigItem *
AnytoneSatelliteSettingsExtension::clone() const {
  auto ext = new AnytoneSatelliteSettingsExtension();
  if (! ext->copy(*this)) {
    ext->deleteLater();
    return nullptr;
  }
  return ext;
}


Channel::Power
AnytoneSatelliteSettingsExtension::power() const {
  return _power;
}

void
AnytoneSatelliteSettingsExtension::setPower(Channel::Power power) {
  if (_power == power)
    return;
  _power = power;
  emit modified(this);
}


unsigned int
AnytoneSatelliteSettingsExtension::squelch() const {
  return _squelch;
}

void
AnytoneSatelliteSettingsExtension::setSquelch(unsigned int squelch) {
  if (_squelch == squelch)
    return;
  _squelch = squelch;
  emit modified(this);
}

