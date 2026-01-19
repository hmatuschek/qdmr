#include "anytone_extension.hh"

/* ********************************************************************************************* *
 * Implementation of AnytoneAPRSFrequency
 * ********************************************************************************************* */
AnytoneAPRSFrequency::AnytoneAPRSFrequency(QObject *parent)
  : ConfigObject(parent), _frequency(Frequency::fromHz(0))
{
  // pass...
}

ConfigItem *
AnytoneAPRSFrequency::clone() const {
  AnytoneAPRSFrequency *obj = new AnytoneAPRSFrequency();
  if (! obj->copy(*this)) {
    obj->deleteLater();
    return nullptr;
  }
  return obj;
}

Frequency
AnytoneAPRSFrequency::frequency() const {
  return _frequency;
}
void
AnytoneAPRSFrequency::setFrequency(Frequency freq) {
  if (_frequency == freq)
    return;
  _frequency = freq;
  emit modified(this);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneAPRSFrequencyRef
 * ********************************************************************************************* */
AnytoneAPRSFrequencyRef::AnytoneAPRSFrequencyRef(QObject *parent)
  : ConfigObjectReference(AnytoneAPRSFrequency::staticMetaObject, parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of AnytoneAPRSFrequencyList
 * ********************************************************************************************* */
AnytoneAPRSFrequencyList::AnytoneAPRSFrequencyList(QObject *parent)
  : ConfigObjectList(AnytoneAPRSFrequency::staticMetaObject, parent)
{
  // pass...
}

ConfigItem *
AnytoneAPRSFrequencyList::allocateChild(const YAML::Node &node, ConfigItem::Context &ctx, const ErrorStack &err) {
  Q_UNUSED(node); Q_UNUSED(ctx); Q_UNUSED(err);
  return new AnytoneAPRSFrequency();
}


/* ********************************************************************************************* *
 * Implementation of AnytoneChannelExtension
 * ********************************************************************************************* */
AnytoneChannelExtension::AnytoneChannelExtension(QObject *parent)
  : ConfigExtension(parent), _talkaround(false), _frequencyCorrection(0), _handsFree(false),
    _fmAPRSFrequency(new AnytoneAPRSFrequencyRef(this)), _aprsPTT(APRSPTT::Off)
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

AnytoneAPRSFrequencyRef *
AnytoneChannelExtension::fmAPRSFrequency() const {
  return _fmAPRSFrequency;
}

AnytoneChannelExtension::APRSPTT
AnytoneChannelExtension::aprsPTT() const {
  return _aprsPTT;
}
void
AnytoneChannelExtension::setAPRSPTT(APRSPTT mode) {
  if (_aprsPTT == mode)
    return;
  _aprsPTT = mode;
  emit modified(this);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneAnalogChannelExtension
 * ********************************************************************************************* */
AnytoneFMChannelExtension::AnytoneFMChannelExtension(QObject *parent)
  : AnytoneChannelExtension(parent), _reverseBurst(false), _rxCustomCTCSS(false),
    _txCustomCTCSS(false), _customCTCSS(0), _squelchMode(SquelchMode::Carrier),
    _scramblerFrequency()
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

Frequency
AnytoneFMChannelExtension::scramblerFrequency() const {
  return _scramblerFrequency;
}
void
AnytoneFMChannelExtension::setScramblerFrequency(const Frequency &f) {
  if (f == _scramblerFrequency)
    return;
  _scramblerFrequency = f;
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
 * Implementation of AnytoneFMAPRSSettingsExtension
 * ********************************************************************************************* */
AnytoneFMAPRSSettingsExtension::AnytoneFMAPRSSettingsExtension(QObject *parent)
  : ConfigExtension(parent), _txDelay(Interval::fromMilliseconds(60)),
    _preWaveDelay(Interval::fromMilliseconds(0)), _passAll(false), _reportPosition(false),
    _reportMicE(false), _reportObject(false), _reportItem(false), _reportMessage(false),
    _reportWeather(false), _reportNMEA(false), _reportStatus(false), _reportOther(false),
    _frequencies(new AnytoneAPRSFrequencyList(this))
{
  // pass...
}

ConfigItem *
AnytoneFMAPRSSettingsExtension::clone() const {
  AnytoneFMAPRSSettingsExtension *ext = new AnytoneFMAPRSSettingsExtension();
  if (! ext->copy(*this)) {
    ext->deleteLater();
    return nullptr;
  }
  return ext;
}

Interval
AnytoneFMAPRSSettingsExtension::txDelay() const {
  return _txDelay;
}
void
AnytoneFMAPRSSettingsExtension::setTXDelay(Interval intv) {
  if (_txDelay == intv)
    return;
  _txDelay = intv;
  emit modified(this);
}

Interval
AnytoneFMAPRSSettingsExtension::preWaveDelay() const {
  return _preWaveDelay;
}
void
AnytoneFMAPRSSettingsExtension::setPreWaveDelay(Interval intv) {
  if (_preWaveDelay == intv)
    return;
  _preWaveDelay = intv;
  emit modified(this);
}

bool
AnytoneFMAPRSSettingsExtension::passAll() const {
  return _passAll;
}
void
AnytoneFMAPRSSettingsExtension::enablePassAll(bool enable) {
  if (_passAll == enable)
    return;
  _passAll = enable;
  emit modified(this);
}

bool
AnytoneFMAPRSSettingsExtension::reportPosition() const {
  return _reportPosition;
}
void
AnytoneFMAPRSSettingsExtension::enableReportPosition(bool enable) {
  if (_reportPosition == enable)
    return;
  _reportPosition = enable;
  emit modified(this);
}

bool
AnytoneFMAPRSSettingsExtension::reportMicE() const {
  return _reportMicE;
}
void
AnytoneFMAPRSSettingsExtension::enableReportMicE(bool enable) {
  if (_reportMicE == enable)
    return;
  _reportMicE = enable;
  emit modified(this);
}

bool
AnytoneFMAPRSSettingsExtension::reportObject() const {
  return _reportObject;
}
void
AnytoneFMAPRSSettingsExtension::enableReportObject(bool enable) {
  if (_reportObject == enable)
    return;
  _reportObject = enable;
  emit modified(this);
}

bool
AnytoneFMAPRSSettingsExtension::reportItem() const {
  return _reportItem;
}
void
AnytoneFMAPRSSettingsExtension::enableReportItem(bool enable) {
  if (_reportItem == enable)
    return;
  _reportItem = enable;
  emit modified(this);
}

bool
AnytoneFMAPRSSettingsExtension::reportMessage() const {
  return _reportMessage;
}
void
AnytoneFMAPRSSettingsExtension::enableReportMessage(bool enable) {
  if (_reportMessage == enable)
    return;
  _reportMessage = enable;
  emit modified(this);
}

bool
AnytoneFMAPRSSettingsExtension::reportWeather() const {
  return _reportWeather;
}
void
AnytoneFMAPRSSettingsExtension::enableReportWeather(bool enable) {
  if (_reportWeather == enable)
    return;
  _reportWeather = enable;
  emit modified(this);
}

bool
AnytoneFMAPRSSettingsExtension::reportNMEA() const {
  return _reportNMEA;
}
void
AnytoneFMAPRSSettingsExtension::enableReportNMEA(bool enable) {
  if (_reportNMEA == enable)
    return;
  _reportNMEA = enable;
  emit modified(this);
}

bool
AnytoneFMAPRSSettingsExtension::reportStatus() const {
  return _reportStatus;
}
void
AnytoneFMAPRSSettingsExtension::enableReportStatus(bool enable) {
  if (_reportStatus == enable)
    return;
  _reportStatus = enable;
  emit modified(this);
}

bool
AnytoneFMAPRSSettingsExtension::reportOther() const {
  return _reportOther;
}
void
AnytoneFMAPRSSettingsExtension::enableReportOther(bool enable) {
  if (_reportOther == enable)
    return;
  _reportOther = enable;
  emit modified(this);
}

AnytoneAPRSFrequencyList *
AnytoneFMAPRSSettingsExtension::frequencies() const {
  return _frequencies;
}
