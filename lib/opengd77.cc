#include "opengd77.hh"
#include "logger.hh"
#include "config.hh"


OpenGD77::OpenGD77(OpenGD77Interface *device, QObject *parent)
  : OpenGD77Base(device, parent), _name("Open GD-77"), _codeplug(), _callsigns()
{
  // pass...
}


const QString &
OpenGD77::name() const {
  return _name;
}



const Codeplug &
OpenGD77::codeplug() const {
  return _codeplug;
}

Codeplug &
OpenGD77::codeplug() {
  return _codeplug;
}


const CallsignDB *
OpenGD77::callsignDB() const {
  return &_callsigns;
}

CallsignDB *
OpenGD77::callsignDB() {
  return &_callsigns;
}


RadioInfo
OpenGD77::defaultRadioInfo() {
  return RadioInfo(
        RadioInfo::OpenGD77, "opengd77", "OpenGD77", "OpenGD77 Project", OpenGD77Interface::interfaceInfo());
}
