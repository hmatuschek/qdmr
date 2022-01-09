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

QList<USBDeviceDescriptor>
RadioInterface::detect() {
  QList<USBDeviceDescriptor> res;
  res.append(AnytoneInterface::detect());
  res.append(OpenGD77Interface::detect());
  res.append(RadioddityInterface::detect());
  res.append(TyTInterface::detect());
  return res;
}
