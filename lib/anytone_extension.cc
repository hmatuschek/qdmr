#include "anytone_extension.hh"


/* ********************************************************************************************* *
 * Implementation of AnytoneChannelExtension
 * ********************************************************************************************* */
AnytoneChannelExtension::AnytoneChannelExtension(QObject *parent)
  : ConfigExtension(parent), _talkaround(false)
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


/* ********************************************************************************************* *
 * Implementation of AnytoneAnalogChannelExtension
 * ********************************************************************************************* */
AnytoneAnalogChannelExtension::AnytoneAnalogChannelExtension(QObject *parent)
  : AnytoneChannelExtension(parent), _reverseBurst(false), _rxCustomCTCSS(false),
    _txCustomCTCSS(false), _customCTCSS(0)
{
  // pass...
}

ConfigItem *
AnytoneAnalogChannelExtension::clone() const {
  AnytoneAnalogChannelExtension *ext = new AnytoneAnalogChannelExtension();
  if (! ext->copy(*this)) {
    ext->deleteLater();
    return nullptr;
  }
  return ext;
}

bool
AnytoneAnalogChannelExtension::reverseBurst() const {
  return _reverseBurst;
}
void
AnytoneAnalogChannelExtension::enableReverseBurst(bool enable) {
  if (enable == _reverseBurst)
    return;
  _reverseBurst = enable;
  emit modified(this);
}

bool
AnytoneAnalogChannelExtension::rxCustomCTCSS() const {
  return _rxCustomCTCSS;
}
void
AnytoneAnalogChannelExtension::enableRXCustomCTCSS(bool enable) {
  if (enable == _rxCustomCTCSS)
    return;
  _rxCustomCTCSS = enable;
  emit modified(this);
}
bool
AnytoneAnalogChannelExtension::txCustomCTCSS() const {
  return _txCustomCTCSS;
}
void
AnytoneAnalogChannelExtension::enableTXCustomCTCSS(bool enable) {
  if (enable == _txCustomCTCSS)
    return;
  _txCustomCTCSS = enable;
  emit modified(this);
}
double
AnytoneAnalogChannelExtension::customCTCSS() const {
  return _customCTCSS;
}
void
AnytoneAnalogChannelExtension::setCustomCTCSS(double freq) {
  if (freq == _customCTCSS)
    return;
  _customCTCSS = freq;
  emit modified(this);
}


/* ********************************************************************************************* *
 * Implementation of AnytoneDigitalChannelExtension
 * ********************************************************************************************* */
AnytoneDigitalChannelExtension::AnytoneDigitalChannelExtension(QObject *parent)
  : AnytoneChannelExtension(parent), _callConfirm(false)
{
  // pass...
}

ConfigItem *
AnytoneDigitalChannelExtension::clone() const {
  AnytoneDigitalChannelExtension *ext = new AnytoneDigitalChannelExtension();
  if (! ext->copy(*this)) {
    ext->deleteLater();
    return nullptr;
  }
  return ext;
}

bool
AnytoneDigitalChannelExtension::callConfirm() const {
  return _callConfirm;
}
void
AnytoneDigitalChannelExtension::enableCallConfirm(bool enabled) {
  if (enabled == _callConfirm)
    return;
  _callConfirm = enabled;
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
