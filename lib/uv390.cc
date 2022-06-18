#include "uv390.hh"
#include "uv390_limits.hh"


RadioLimits *UV390::_limits = nullptr;


UV390::UV390(TyTInterface *device, QObject *parent)
  : TyTRadio(device, parent), _name("TyT MD-UV390")
{
  // pass...
}

UV390::~UV390() {
  // pass...
}

const QString &
UV390::name() const {
  return _name;
}

const RadioLimits &
UV390::limits() const {
  if (nullptr == _limits)
    _limits = new UV390Limits();
  return *_limits;
}

const Codeplug &
UV390::codeplug() const {
  return _codeplug;
}

Codeplug &
UV390::codeplug() {
  return _codeplug;
}

const CallsignDB *
UV390::callsignDB() const {
  return &_callsigndb;
}

CallsignDB *
UV390::callsignDB() {
  return &_callsigndb;
}

RadioInfo
UV390::defaultRadioInfo() {
  return RadioInfo(
        RadioInfo::UV390, "uv390", "MD-UV390", "TyT", TyTInterface::interfaceInfo(),
        QList<RadioInfo>{
          RadioInfo(RadioInfo::UV380, "MD-UV380", "TyT", TyTInterface::interfaceInfo()),
          RadioInfo(RadioInfo::RT3S, "RT3S", "Retevis", TyTInterface::interfaceInfo())
        });
}
