#include "radiointerface.hh"

RadioInterface::RadioInterface()
{
	// pass...
}

RadioInterface::~RadioInterface() {
  // pass...
}

bool
RadioInterface::read_finish() {
  return true;
}

bool
RadioInterface::write_finish() {
  return true;
}

bool
RadioInterface::reboot() {
  return true;
}
