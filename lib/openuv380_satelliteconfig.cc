#include "openuv380_satelliteconfig.hh"
#include "errorstack.hh"


OpenUV380SatelliteConfig::OpenUV380SatelliteConfig(QObject *parent)
  : OpenGD77BaseSatelliteConfig(parent)
{
  image(FLASH).addElement(Offset::satellites(), size()); // of 0x11a0 bytes
}

bool
OpenUV380SatelliteConfig::isValid() const {
  return OpenGD77BaseCodeplug::AdditionalSettingsElement((uint8_t *)data(Offset::satellites(), FLASH))
      .isValid();
}


void
OpenUV380SatelliteConfig::initialize() {
  OpenGD77BaseCodeplug::AdditionalSettingsElement(data(Offset::satellites(), FLASH)).clear();
}


bool
OpenUV380SatelliteConfig::encode(SatelliteDatabase *db, const ErrorStack &err) {
  OpenGD77BaseCodeplug::AdditionalSettingsElement settings(data(Offset::satellites(), FLASH));
  if (! settings.isValid()) {
    errMsg(err) << "Cannot encode satellite config for OpenUV380: Invalid settings element.";
    return false;
  }

  OpenGD77BaseCodeplug::SatelliteBankElement bank = settings.satellites();
  bank.clear();

  if (! bank.encode(db, err)) {
    errMsg(err) << "Cannot encode satellite config for OpenUV380.";
    return false;
  }

  return true;
}
