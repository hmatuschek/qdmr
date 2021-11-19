#include "radiointerface.hh"

RadioInterface::RadioInterface()
{
	// pass...
}

RadioInterface::~RadioInterface() {
  // pass...
}

bool
RadioInterface::write_finish(const ErrorStack &err) {
  Q_UNUSED(err)
  return true;
}

bool
RadioInterface::reboot(const ErrorStack &err) {
  Q_UNUSED(err)
  return true;
}
