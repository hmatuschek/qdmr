#include "transferflags.hh"

TransferFlags::TransferFlags()
  : _blocking(false), _updateDeviceClock(false)
{
  // pass...
}

TransferFlags::TransferFlags(bool blocking, bool updateDeviceClock)
  : _blocking(blocking), _updateDeviceClock(updateDeviceClock)
{
  // pass...
}


bool
TransferFlags::blocking() const {
  return _blocking;
}

void
TransferFlags::setBlocking(bool enable) {
  _blocking = enable;
}


bool
TransferFlags::updateDeviceClock() const {
  return _updateDeviceClock;
}

void
TransferFlags::setUpdateDeviceClock(bool enable) {
  _updateDeviceClock = enable;
}



