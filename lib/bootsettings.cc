#include "bootsettings.hh"


BootSettings::BootSettings(QObject *parent)
: ConfigExtension{parent}, _bootDisplay(BootDisplay::Logo), _message1(), _message2(),
  _bootPasswordEnabled(false), _bootPassword(),
  _defaultChannel(false),
  _zoneA(new ZoneReference(this)), _channelA(new ChannelReference(this)),
  _zoneB(new ZoneReference(this)), _channelB(new ChannelReference(this)),
  _reset(true)
{
  // pass...
}


ConfigItem *
BootSettings::clone() const {
  auto clone = new BootSettings();
  if (! clone->copy(*this)) {
    delete clone;
    return nullptr;
  }
  return clone;
}


BootSettings::BootDisplay
BootSettings::bootDisplay() const {
  return _bootDisplay;
}

void
BootSettings::setBootDisplay(BootDisplay mode) {
  if (_bootDisplay == mode)
    return;
  _bootDisplay = mode;
  emit modified(this);
}


const QString &
BootSettings::message1() const {
  return _message1;
}

void
BootSettings::setMessage1(const QString &message1) {
  if (_message1 == message1)
    return;
  _message1 = message1;
  emit modified(this);
}

const QString &
BootSettings::message2() const {
  return _message2;
}

void
BootSettings::setMessage2(const QString &message2) {
  if (_message2 == message2)
    return;
  _message2 = message2;
  emit modified(this);
}


bool
BootSettings::bootPasswordEnabled() const {
  return _bootPasswordEnabled;
}

void
BootSettings::enableBootPassword(bool enable) {
  if (_bootPasswordEnabled == enable)
    return;
  _bootPasswordEnabled = enable;
  emit modified(this);
}


const QString &
BootSettings::bootPassword() const {
  return _bootPassword;
}

void
BootSettings::setBootPassword(const QString &pass) {
  if (_bootPassword == pass)
    return;
  _bootPassword = pass;
  emit modified(this);
}


bool
BootSettings::defaultChannelEnabled() const {
  return _defaultChannel;
}

void
BootSettings::enableDefaultChannel(bool enable) {
  if (_defaultChannel == enable)
    return;
  _defaultChannel = enable;
  emit modified(this);
}


ZoneReference *
BootSettings::zoneA() const {
  return _zoneA;
}

ChannelReference *
BootSettings::channelA() const {
  return _channelA;
}


ZoneReference *
BootSettings::zoneB() const {
  return _zoneB;
}

ChannelReference *
BootSettings::channelB() const {
  return _channelB;
}


bool
BootSettings::resetEnabled() const {
  return _reset;
}

void
BootSettings::enableReset(bool enable) {
  if (_reset == enable)
    return;
  _reset = enable;
  emit modified(this);
}

