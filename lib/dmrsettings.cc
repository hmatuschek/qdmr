#include "dmrsettings.hh"


DMRSettings::DMRSettings(QObject *parent)
  : ConfigExtension{parent}, _privateCallMatch(true), _groupCallMatch(true),
    _privateCallHangTime(Interval::fromSeconds(5)), _groupCallHangTime(Interval::fromSeconds(3)),
    _sendTalkerAlias(false), _talkerAliasEncoding(TalkerAliasEncoding::Iso8),
    _smsFormat(SMSFormat::DMR), _preamble(Interval::fromMilliseconds(100))
{
  // pass...
}


void
DMRSettings::clear() {
  ConfigExtension::clear();

  _privateCallMatch = true;
  _groupCallMatch = true;
  _privateCallHangTime = Interval::fromSeconds(5);
  _groupCallHangTime = Interval::fromSeconds(3);
  _sendTalkerAlias = false;
  _talkerAliasEncoding = TalkerAliasEncoding::Iso8;
  _smsFormat = SMSFormat::DMR;
}


ConfigItem *
DMRSettings::clone() const {
  auto obj = new DMRSettings();
  if (! obj->copy(*this)) {
    delete obj;
    return nullptr;
  }
  return obj;
}


bool
DMRSettings::privateCallMatchEnabled() const {
  return _privateCallMatch;
}

void
DMRSettings::enablePrivateCallMatch(bool enable) {
  if (_privateCallMatch == enable)
    return;
  _privateCallMatch = enable;
  emit modified(this);
}


bool DMRSettings::groupCallMatchEnabled() const {
  return _groupCallMatch;
}

void
DMRSettings::enableGroupCallMatch(bool enable) {
  if (_groupCallMatch == enable)
    return;
  _groupCallMatch = enable;
  emit modified(this);
}


Interval
DMRSettings::privateCallHangTime() const {
  return _privateCallHangTime;
}

void
DMRSettings::setPrivateCallHangTime(const Interval &dur) {
  if (_privateCallHangTime == dur)
    return;
  _privateCallHangTime = dur;
  emit modified(this);
}


Interval
DMRSettings::groupCallHangTime() const {
  return _groupCallHangTime;
}

void
DMRSettings::setGroupCallHangTime(const Interval &dur) {
  if (_groupCallHangTime == dur)
    return;
  _groupCallHangTime = dur;
  emit modified(this);
}


bool
DMRSettings::sendTalkerAliasEnabled() const {
  return _sendTalkerAlias;
}

void
DMRSettings::enableSendTalkerAlias(bool enable) {
  if (_sendTalkerAlias == enable)
    return;
  _sendTalkerAlias = enable;
  emit modified(this);
}


DMRSettings::TalkerAliasEncoding
DMRSettings::talkerAliasEncoding() const {
  return _talkerAliasEncoding;
}

void
DMRSettings::setTalkerAliasEncoding(TalkerAliasEncoding encoding) {
  if (_talkerAliasEncoding == encoding)
    return;
  _talkerAliasEncoding = encoding;
  emit modified(this);
}


DMRSettings::SMSFormat
DMRSettings::smsFormat() const {
  return _smsFormat;
}

void
DMRSettings::setSMSFormat(SMSFormat format) {
  if (_smsFormat == format)
    return;
  _smsFormat = format;
  emit modified(this);
}


Interval
DMRSettings::preamble() const {
  return _preamble;
}

void
DMRSettings::setPreamble(const Interval &dur) {
  if (_preamble == dur)
    return;
  _preamble = dur;
  emit modified(this);
}

