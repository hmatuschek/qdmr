#include "dm1701.hh"
#include "dm1701_limits.hh"

#include "config.hh"
#include "logger.hh"
#include "utils.hh"

RadioLimits *DM1701::_limits = nullptr;


DM1701::DM1701(TyTInterface *device, QObject *parent)
  : TyTRadio(device, parent), _name("Baofeng DM-1701"), _codeplug()
{
  // pass...
}

const QString &
DM1701::name() const {
  return _name;
}

const RadioLimits &
DM1701::limits() const {
  if (nullptr == _limits)
    _limits = new DM1701Limits();
  return *_limits;
}

RadioInfo
DM1701::defaultRadioInfo() {
  return RadioInfo(
        RadioInfo::DM1701, "dm1701", "DM-1701", "Baofeng", TyTInterface::interfaceInfo(),
        QList<RadioInfo>{
          RadioInfo(RadioInfo::RT84, "rt84", "RT84", "Retevis", TyTInterface::interfaceInfo())
        });
}

const Codeplug &
DM1701::codeplug() const {
  return _codeplug;
}

Codeplug &
DM1701::codeplug() {
  return _codeplug;
}
