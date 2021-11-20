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

bool
TyTChannelExtension::copy(const ConfigItem &other) {
  const TyTChannelExtension *ex = other.as<TyTChannelExtension>();
  if ((nullptr == ex) || (! ConfigExtension::copy(other)))
    return false;

  enableLoneWorker(ex->loneWorker());
  enableAutoScan(ex->autoScan());
  enableTalkaround(ex->talkaround());
  enableDataCallConfirmed(ex->dataCallConfirmed());
  enablePrivateCallConfirmed(ex->privateCallConfirmed());
  enableEmergencyAlarmConfirmed(ex->emergencyAlarmConfirmed());
  enableDisplayPTTId(ex->displayPTTId());
  setRXRefFrequency(ex->rxRefFrequency());
  setTXRefFrequency(ex->txRefFrequency());
  enableTightSquelch(ex->tightSquelch());
  enableCompressedUDPHeader(ex->compressedUDPHeader());
  enableReverseBurst(ex->reverseBurst());
  setKillTone(ex->killTone());
  setInCallCriterion(ex->inCallCriterion());
  enableAllowInterrupt(ex->allowInterrupt());
  enableDCDM(ex->dcdm());
  enableDCDMLeader(ex->dcdmLeader());

  return true;
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

ConfigItem *
TyTChannelExtension::allocateChild(QMetaProperty &prop, const YAML::Node &node,
                                   const Context &ctx, const ErrorStack &err)
{
  Q_UNUSED(prop); Q_UNUSED(node); Q_UNUSED(ctx); Q_UNUSED(err)
  // There are no further extension/children to TyTChannelExtension.
  return nullptr;
}


/* ******************************************************************************************** *
 * Implementation of TyTScanListExtension
 * ******************************************************************************************** */
TyTScanListExtension::TyTScanListExtension(QObject *parent)
  : ConfigExtension(parent), _holdTime(500), _prioritySampleTime(2000)
{
  // pass...
}

bool
TyTScanListExtension::copy(const ConfigItem &other) {
  const TyTScanListExtension *ex = other.as<TyTScanListExtension>();
  if ((nullptr==ex) || (!ConfigExtension::copy(other)))
    return false;

  setHoldTime(ex->holdTime());
  setPrioritySampleTime(ex->prioritySampleTime());

  return true;
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

ConfigItem *
TyTScanListExtension::allocateChild(QMetaProperty &prop, const YAML::Node &node,
                                    const Context &ctx, const ErrorStack &err)
{
  Q_UNUSED(prop); Q_UNUSED(node); Q_UNUSED(ctx); Q_UNUSED(err)
  // There are no further extension/children to TyTScanListExtension.
  return nullptr;
}


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
  _longPressDuration = 1000;
}

bool
TyTButtonSettings::copy(const ConfigItem &other) {
  const TyTButtonSettings *ex = other.as<TyTButtonSettings>();
  if ((nullptr==ex) || (!ConfigExtension::copy(other)))
    return false;
  _sideButton1Short = ex->_sideButton1Short;
  _sideButton1Long = ex->_sideButton1Long;
  _sideButton2Short = ex->_sideButton2Short;
  _sideButton2Long = ex->_sideButton2Long;
  _longPressDuration = ex->_longPressDuration;
  return true;
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
  _sideButton1Short = action;
}

TyTButtonSettings::ButtonAction
TyTButtonSettings::sideButton1Long() const {
  return _sideButton1Long;
}
void
TyTButtonSettings::setSideButton1Long(ButtonAction action) {
  _sideButton1Long = action;
}

TyTButtonSettings::ButtonAction
TyTButtonSettings::sideButton2Short() const {
  return _sideButton2Short;
}
void
TyTButtonSettings::setSideButton2Short(ButtonAction action) {
  _sideButton2Short = action;
}

TyTButtonSettings::ButtonAction
TyTButtonSettings::sideButton2Long() const {
  return _sideButton2Long;
}
void
TyTButtonSettings::setSideButton2Long(ButtonAction action) {
  _sideButton2Long = action;
}

unsigned
TyTButtonSettings::longPressDuration() const {
  return _longPressDuration;
}
void
TyTButtonSettings::setLongPressDuration(unsigned dur) {
  _longPressDuration = dur;
}

ConfigItem *
TyTButtonSettings::allocateChild(QMetaProperty &prop, const YAML::Node &node,
                                 const Context &ctx, const ErrorStack &err)
{
  Q_UNUSED(prop); Q_UNUSED(node); Q_UNUSED(ctx); Q_UNUSED(err)
  // There are no further extension/children to TyTButtonSettings.
  return nullptr;
}


/* ******************************************************************************************** *
 * Implementation of TyTButtonSettings
 * ******************************************************************************************** */
TyTConfigExtension::TyTConfigExtension(QObject *parent)
  : ConfigExtension(parent), _buttonSettings(new TyTButtonSettings(this))
{
  // Pass...
}

bool
TyTConfigExtension::copy(const ConfigItem &other) {
  const TyTConfigExtension *ex = other.as<TyTConfigExtension>();
  if ((nullptr==ex) || (!ConfigExtension::copy(other)))
    return false;
  return _buttonSettings->copy(*_buttonSettings);
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

ConfigItem *
TyTConfigExtension::allocateChild(QMetaProperty &prop, const YAML::Node &node,
                                  const Context &ctx, const ErrorStack &err)
{
  Q_UNUSED(prop); Q_UNUSED(node); Q_UNUSED(ctx); Q_UNUSED(err)
  // All extensions are pre-allocated. So nothing to do here.
  return nullptr;
}
