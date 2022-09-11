#include "radiointerface.hh"

#include "anytone_interface.hh"
#include "opengd77_interface.hh"
#include "radioddity_interface.hh"
#include "tyt_interface.hh"

/* ********************************************************************************************* *
 * Implementation of RadioInterface
 * ********************************************************************************************* */
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
