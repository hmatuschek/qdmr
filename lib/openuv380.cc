#include "openuv380.hh"
#include "openuv380_satelliteconfig.hh"


OpenUV380::OpenUV380(OpenGD77Interface *device, QObject *parent)
  : OpenGD77Base(device, parent), _name("Open MD-UV380"), _codeplug(),
  _callsigns(device->extendedCallsignDB())
{
  _satelliteConfig = new OpenUV380SatelliteConfig(this);
}


const QString &
OpenUV380::name() const {
  return _name;
}


const Codeplug &
OpenUV380::codeplug() const {
  return _codeplug;
}

Codeplug &
OpenUV380::codeplug() {
  return _codeplug;
}


const CallsignDB *
OpenUV380::callsignDB() const {
  return &_callsigns;
}

CallsignDB *
OpenUV380::callsignDB() {
  return &_callsigns;
}


RadioInfo
OpenUV380::defaultRadioInfo() {
  return RadioInfo(
        RadioInfo::OpenUV380, "openuv380", "OpenMDUV380", "OpenGD77 Project",
        {OpenGD77Interface::interfaceInfo()});
}


