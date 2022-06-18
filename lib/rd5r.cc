#include "rd5r.hh"
#include "config.hh"
#include "radiolimits.hh"
#include "rd5r_limits.hh"

#define BSIZE 128


RadioLimits *RD5R::_limits = nullptr;

RD5R::RD5R(RadioddityInterface *device, QObject *parent)
  : RadioddityRadio(device, parent), _name("Baofeng/Radioddity RD-5R"), _codeplug()
{
  // pass...
}

RD5R::~RD5R() {
  // pass...
}

const QString &
RD5R::name() const {
  return _name;
}

const RadioLimits &
RD5R::limits() const {
  if (nullptr == _limits)
    _limits = new RD5RLimits();
  return *_limits;
}

const Codeplug &
RD5R::codeplug() const {
  return _codeplug;
}

Codeplug &
RD5R::codeplug() {
  return _codeplug;
}

RadioInfo
RD5R::defaultRadioInfo() {
  return RadioInfo(
        RadioInfo::RD5R, "rd5r", "RD-5R", "Radioddity", RadioddityInterface::interfaceInfo());
}
