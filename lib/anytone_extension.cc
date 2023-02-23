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
 * Implementation of AnytoneDMRSettingsExtension
 * ********************************************************************************************* */
AnytoneDMRSettingsExtension::AnytoneDMRSettingsExtension(QObject *parent)
  : ConfigItem(parent), _groupCallHangTime(3), _privateCallHangTime(5), _preWaveDelay(100),
    _wakeHeadPeriod(100), _filterOwnID(true), _monitorSlotMatch(SlotMatch::Off),
    _monitorColorCodeMatch(false), _monitorIDMatch(false), _monitorTimeSlotHold(true),
    _smsFormat(SMSFormat::Motorola)
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

unsigned int
AnytoneDMRSettingsExtension::groupCallHangTime() const {
  return _groupCallHangTime;
}
void
AnytoneDMRSettingsExtension::setGroupCallHangTime(unsigned int sec) {
  if (_groupCallHangTime == sec)
    return;
  _groupCallHangTime = sec;
  emit modified(this);
}

unsigned int
AnytoneDMRSettingsExtension::privateCallHangTime() const {
  return _privateCallHangTime;
}
void
AnytoneDMRSettingsExtension::setPrivateCallHangTime(unsigned int sec) {
  if (_privateCallHangTime == sec)
    return;
  _privateCallHangTime = sec;
  emit modified(this);
}

unsigned int
AnytoneDMRSettingsExtension::preWaveDelay() const {
  return _preWaveDelay;
}
void
AnytoneDMRSettingsExtension::setPreWaveDelay(unsigned int ms) {
  if (_preWaveDelay == ms)
    return;
  _preWaveDelay = ms;
  emit modified(this);
}

unsigned int
AnytoneDMRSettingsExtension::wakeHeadPeriod() const {
  return _wakeHeadPeriod;
}
void
AnytoneDMRSettingsExtension::setWakeHeadPeriod(unsigned int ms) {
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


/* ********************************************************************************************* *
 * Implementation of AnytoneRangingSettingsExtension
 * ********************************************************************************************* */
AnytoneRangingSettingsExtension::AnytoneRangingSettingsExtension(QObject *parent)
  : ConfigItem(parent), _gpsRangeReporting(false), _gpsRangingInterval(300), _autoRoamPeriod(1),
    _autoRoamDelay(0), _repeaterRangeCheck(false), _repeaterCheckInterval(5),
    _repeaterRangeCheckCount(3), _roamingStartCondition(RoamStart::Periodic)
{
  // pass...
}

ConfigItem *
AnytoneRangingSettingsExtension::clone() const {
  AnytoneRangingSettingsExtension *ext = new AnytoneRangingSettingsExtension();
  if (! ext->copy(*this)) {
    ext->deleteLater();
    return nullptr;
  }
  return ext;
}

bool
AnytoneRangingSettingsExtension::gpsRangeReportingEnabled() const {
  return _gpsRangeReporting;
}
void
AnytoneRangingSettingsExtension::enableGPSRangeReporting(bool enable) {
  if (_gpsRangeReporting == enable)
    return;
  _gpsRangeReporting = enable;
  emit modified(this);
}

unsigned int
AnytoneRangingSettingsExtension::gpsRangingInterval() const {
  return _gpsRangingInterval;
}
void
AnytoneRangingSettingsExtension::setGPSRangingInterval(unsigned int sec) {
  if (_gpsRangingInterval == sec)
    return;
  _gpsRangingInterval = sec;
  emit modified(this);
}

unsigned int
AnytoneRangingSettingsExtension::autoRoamPeriod() const {
  return _autoRoamPeriod;
}
void
AnytoneRangingSettingsExtension::setAutoRoamPeriod(unsigned int min) {
  if (_autoRoamPeriod == min)
    return;
  _autoRoamPeriod = min;
  emit modified(this);
}

unsigned int
AnytoneRangingSettingsExtension::autoRoamDelay() const {
  return _autoRoamDelay;
}
void
AnytoneRangingSettingsExtension::setAutoRoamDelay(unsigned int min) {
  if (_autoRoamDelay == min)
    return;
  _autoRoamDelay = min;
  emit modified(this);
}

bool
AnytoneRangingSettingsExtension::repeaterRangeCheckEnabled() const {
  return _repeaterRangeCheck;
}
void
AnytoneRangingSettingsExtension::enableRepeaterRangeCheck(bool enable) {
  if (_repeaterRangeCheck == enable)
    return;
  _repeaterRangeCheck = enable;
  emit modified(this);
}
unsigned int
AnytoneRangingSettingsExtension::repeaterCheckInterval() const {
  return _repeaterCheckInterval;
}
void
AnytoneRangingSettingsExtension::setRepeaterCheckInterval(unsigned int sec) {
  if (_repeaterCheckInterval == sec)
    return;
  _repeaterCheckInterval = sec;
  emit modified(this);
}

unsigned int
AnytoneRangingSettingsExtension::repeaterRangeCheckCount() const {
  return _repeaterRangeCheckCount;
}
void
AnytoneRangingSettingsExtension::setRepeaterRangeCheckCount(unsigned int sec) {
  if (_repeaterRangeCheckCount == sec)
    return;
  _repeaterRangeCheckCount = sec;
  emit modified(this);
}

AnytoneRangingSettingsExtension::RoamStart
AnytoneRangingSettingsExtension::roamingStartCondition() const {
  return _roamingStartCondition;
}
void
AnytoneRangingSettingsExtension::setRoamingStartCondition(RoamStart start) {
  if (_roamingStartCondition == start)
    return;
  _roamingStartCondition = start;
  emit modified(this);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneSettingsExtension
 * ********************************************************************************************* */
AnytoneSettingsExtension::AnytoneSettingsExtension(QObject *parent)
  : ConfigExtension(parent), _bootSettings(new AnytoneBootSettingsExtension(this)),
    _keySettings(new AnytoneKeySettingsExtension(this)),
    _toneSettings(new AnytoneToneSettingsExtension(this)),
    _displaySettings(new AnytoneDisplaySettingsExtension(this)),
    _audioSettings(new AnytoneAudioSettingsExtension(this)),
    _menuSettings(new AnytoneMenuSettingsExtension(this)),
    _autoRepeaterSettings(new AnytoneAutoRepeaterSettingsExtension(this)),
    _dmrSettings(new AnytoneDMRSettingsExtension(this)),
    _rangingSettings(new AnytoneRangingSettingsExtension(this)),
    _autoShutDownDelay(0), _powerSave(PowerSave::Save50), _vfoScanType(VFOScanType::TO),
    _modeA(VFOMode::Memory), _modeB(VFOMode::Memory), _zoneA(), _zoneB(), _selectedVFO(VFO::A),
    _subChannel(true), _timeZone(QTimeZone::utc()), _minVFOScanFrequencyUHF(430000000U),
    _maxVFOScanFrequencyUHF(440000000U), _minVFOScanFrequencyVHF(144000000U),
    _maxVFOScanFrequencyVHF(146000000U), _keepLastCaller(false), _vfoStep(2.5),
    _steType(STEType::Off), _steFrequency(0), _tbstFrequency(1750), _proMode(false),
    _maintainCallChannel(false)
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
  connect(_rangingSettings, &AnytoneRangingSettingsExtension::modified,
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

AnytoneRangingSettingsExtension *
AnytoneSettingsExtension::rangingSettings() const {
  return _rangingSettings;
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

QTimeZone
AnytoneSettingsExtension::timeZone() const {
  return _timeZone;
}
QString
AnytoneSettingsExtension::ianaTimeZone() const {
  return QString::fromLocal8Bit(_timeZone.id());
}
void
AnytoneSettingsExtension::setTimeZone(const QTimeZone &zone) {
  if (_timeZone == zone)
    return;
  _timeZone = zone;
  emit modified(this);
}
void
AnytoneSettingsExtension::setIANATimeZone(const QString &id) {
  setTimeZone(QTimeZone(id.toLocal8Bit()));
}

unsigned int
AnytoneSettingsExtension::minVFOScanFrequencyUHF() const {
  return _minVFOScanFrequencyUHF;
}
void
AnytoneSettingsExtension::setMinVFOScanFrequencyUHF(unsigned hz) {
  if (_minVFOScanFrequencyUHF == hz)
    return;
  _minVFOScanFrequencyUHF = hz;
  emit modified(this);
}
unsigned int
AnytoneSettingsExtension::maxVFOScanFrequencyUHF() const {
  return _maxVFOScanFrequencyUHF;
}
void
AnytoneSettingsExtension::setMaxVFOScanFrequencyUHF(unsigned hz) {
  if (_maxVFOScanFrequencyUHF == hz)
    return;
  _maxVFOScanFrequencyUHF = hz;
  emit modified(this);
}

unsigned int
AnytoneSettingsExtension::minVFOScanFrequencyVHF() const {
  return _minVFOScanFrequencyVHF;
}
void
AnytoneSettingsExtension::setMinVFOScanFrequencyVHF(unsigned hz) {
  if (_minVFOScanFrequencyVHF == hz)
    return;
  _minVFOScanFrequencyVHF = hz;
  emit modified(this);
}
unsigned int
AnytoneSettingsExtension::maxVFOScanFrequencyVHF() const {
  return _maxVFOScanFrequencyVHF;
}
void
AnytoneSettingsExtension::setMaxVFOScanFrequencyVHF(unsigned hz) {
  if (_maxVFOScanFrequencyVHF == hz)
    return;
  _maxVFOScanFrequencyVHF = hz;
  emit modified(this);
}

AnytoneSettingsExtension::Units
AnytoneSettingsExtension::units() const {
  return _gpsUnits;
}
void
AnytoneSettingsExtension::setUnits(Units units) {
  if (_gpsUnits == units)
    return;
  _gpsUnits = units;
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

double
AnytoneSettingsExtension::vfoStep() const {
  return _vfoStep;
}
void
AnytoneSettingsExtension::setVFOStep(double step) {
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

unsigned int
AnytoneSettingsExtension::tbstFrequency() const {
  return _tbstFrequency;
}
void
AnytoneSettingsExtension::setTBSTFrequency(unsigned int Hz) {
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


/* ********************************************************************************************* *
 * Implementation of AnytoneBootSettingsExtension
 * ********************************************************************************************* */
AnytoneBootSettingsExtension::AnytoneBootSettingsExtension(QObject *parent)
  : ConfigItem(parent), _bootDisplay(BootDisplay::Default), _bootPasswordEnabled(false),
    _defaultChannel(false), _zoneA(new ZoneReference(this)), _channelA(new ChannelReference(this)),
    _zoneB(new ZoneReference(this)), _channelB(new ChannelReference(this)),
    _priorityZoneA(new ZoneReference(this)), _priorityZoneB(new ZoneReference(this)),
    _defaultRoamingZone(new RoamingZoneReference(this))
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

RoamingZoneReference *
AnytoneBootSettingsExtension::defaultRoamingZone() const {
  return _defaultRoamingZone;
}


/* ********************************************************************************************* *
 * Implementation of AnytoneKeySettingsExtension
 * ********************************************************************************************* */
AnytoneKeySettingsExtension::AnytoneKeySettingsExtension(QObject *parent)
  : ConfigItem(parent),
    _progFuncKey1Short(KeyFunction::Off), _progFuncKey1Long(KeyFunction::DigitalEncryption),
    _progFuncKey2Short(KeyFunction::Voltage), _progFuncKey2Long(KeyFunction::Call),
    _progFuncKey3Short(KeyFunction::Power), _progFuncKey3Long(KeyFunction::VOX),
    _funcKey1Short(KeyFunction::VOX), _funcKey1Long(KeyFunction::VFOChannel),
    _funcKey2Short(KeyFunction::Reverse), _funcKey2Long(KeyFunction::Off),
    _longPressDuration(1000), _autoKeyLock(false), _knobLock(false), _keypadLock(false),
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
    _idleChannelTone(false), _startupTone(false), _callMelody(new Melody(100, this)),
    _idleMelody(new Melody(100, this)), _resetMelody(new Melody(100, this)), _keyToneLevel(0)
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
AnytoneToneSettingsExtension::idleChannelToneEnabled() const {
  return _idleChannelTone;
}
void
AnytoneToneSettingsExtension::enableIdleChannelTone(bool enable) {
  if (_idleChannelTone == enable)
    return;
  _idleChannelTone = enable;
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
  : ConfigItem(parent), _displayFrequency(false), _brightness(5), _backlightDuration(10),
    _volumeChangePrompt(true), _callEndPrompt(true),
    _lastCallerDisplay(LastCallerDisplayMode::Both), _showClock(true), _showCall(true),
    _callColor(Color::Orange), _showZoneAndContact(true), _language(Language::English),
    _showChannelNumber(true), _showContact(true), _standbyTextColor(Color::White),
    _showLastHeard(false)
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

unsigned int
AnytoneDisplaySettingsExtension::backlightDuration() const {
  return _backlightDuration;
}
void
AnytoneDisplaySettingsExtension::setBacklightDuration(unsigned int sec) {
  if (_backlightDuration == sec)
    return;
  _backlightDuration = sec;
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

bool
AnytoneDisplaySettingsExtension::showZoneAndContactEnabled() const {
  return _showZoneAndContact;
}
void
AnytoneDisplaySettingsExtension::enableShowZoneAndContact(bool enable) {
  if (_showZoneAndContact == enable)
    return;
  _showZoneAndContact = enable;
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
AnytoneDisplaySettingsExtension::showContactEnabled() const {
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


/* ********************************************************************************************* *
 * Implementation of AnytoneAudioSettingsExtension
 * ********************************************************************************************* */
AnytoneAudioSettingsExtension::AnytoneAudioSettingsExtension(QObject *parent)
  : ConfigItem(parent),  _voxDelay(0), _recording(false), _voxSource(VoxSource::Both),
    _maxVolume(3), _maxHeadPhoneVolume(3), _enhanceAudio(true)
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

unsigned int
AnytoneAudioSettingsExtension::voxDelay() const {
  return _voxDelay;
}
void
AnytoneAudioSettingsExtension::setVOXDelay(unsigned int ms) {
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


/* ********************************************************************************************* *
 * Implementation of AnytoneAudioSettingsExtension
 * ********************************************************************************************* */
AnytoneMenuSettingsExtension::AnytoneMenuSettingsExtension(QObject *parent)
  : ConfigItem(parent),  _menuDuration(15)
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

unsigned int
AnytoneMenuSettingsExtension::duration() const {
  return _menuDuration;
}
void
AnytoneMenuSettingsExtension::setDuration(unsigned int sec) {
  if (_menuDuration == sec)
    return;
  _menuDuration = sec;
  emit modified(this);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneAutoRepeaterSettingsExtension
 * ********************************************************************************************* */
AnytoneAutoRepeaterSettingsExtension::AnytoneAutoRepeaterSettingsExtension(QObject *parent)
  : ConfigItem(parent), _directionA(Direction::Off), _directionB(Direction::Off),
    _minVHF(136000000U), _maxVHF(174000000U), _minUHF(400000000U), _maxUHF(480000000U),
    _vhfOffset(new AnytoneAutoRepeaterOffsetRef(this)),
    _uhfOffset(new AnytoneAutoRepeaterOffsetRef(this)),
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

unsigned int
AnytoneAutoRepeaterSettingsExtension::vhfMin() const {
  return _minVHF;
}
void
AnytoneAutoRepeaterSettingsExtension::setVHFMin(unsigned int Hz) {
  if (_minVHF == Hz)
    return;
  _minVHF = Hz;
  emit modified(this);
}
unsigned int
AnytoneAutoRepeaterSettingsExtension::vhfMax() const {
  return _maxVHF;
}
void
AnytoneAutoRepeaterSettingsExtension::setVHFMax(unsigned int Hz) {
  if (_maxVHF == Hz)
    return;
  _maxVHF = Hz;
  emit modified(this);
}
unsigned int
AnytoneAutoRepeaterSettingsExtension::uhfMin() const {
  return _minUHF;
}
void
AnytoneAutoRepeaterSettingsExtension::setUHFMin(unsigned int Hz) {
  if (_minUHF == Hz)
    return;
  _minUHF = Hz;
  emit modified(this);
}
unsigned int
AnytoneAutoRepeaterSettingsExtension::uhfMax() const {
  return _maxUHF;
}
void
AnytoneAutoRepeaterSettingsExtension::setUHFMax(unsigned int Hz) {
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

AnytoneAutoRepeaterOffsetList *
AnytoneAutoRepeaterSettingsExtension::offsets() const {
  return _offsets;
}

/* ********************************************************************************************* *
 * Implementation of AnytoneAutoRepeaterOffset
 * ********************************************************************************************* */
AnytoneAutoRepeaterOffset::AnytoneAutoRepeaterOffset(QObject *parent)
  : ConfigObject(parent), _offset(0)
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

unsigned int
AnytoneAutoRepeaterOffset::offset() const {
  return _offset;
}
void
AnytoneAutoRepeaterOffset::setOffset(unsigned int offset) {
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
