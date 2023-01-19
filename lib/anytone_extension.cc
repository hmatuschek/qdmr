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
