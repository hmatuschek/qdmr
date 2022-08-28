#include "md2017.hh"
#include "md2017_limits.hh"


RadioLimits *MD2017::_limits = nullptr;

MD2017::MD2017(TyTInterface *device, QObject *parent)
  : TyTRadio(device, parent), _name("TyT DM-2017")
{
  //_codeplug(nullptr), _callsigndb(nullptr);
}

MD2017::~MD2017() {
  // pass...
}

const QString &
MD2017::name() const {
  return _name;
}

const RadioLimits &
MD2017::limits() const {
  if (nullptr == _limits)
    _limits = new MD2017Limits();
  return *_limits;
}

const Codeplug &
MD2017::codeplug() const {
  return _codeplug;
}

Codeplug &
MD2017::codeplug() {
  return _codeplug;
}

const CallsignDB *
MD2017::callsignDB() const {
  return &_callsigndb;
}

CallsignDB *
MD2017::callsignDB() {
  return &_callsigndb;
}

RadioInfo
MD2017::defaultRadioInfo() {
  return RadioInfo(
        RadioInfo::MD2017, "md2017", "MD-2017", "TyT", TyTInterface::interfaceInfo(),
        QList<RadioInfo>{
          RadioInfo(RadioInfo::RT82, "RT82", "Retevis", TyTInterface::interfaceInfo())
        });
}
