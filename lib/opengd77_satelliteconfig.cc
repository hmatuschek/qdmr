#include "opengd77_satelliteconfig.hh"
#include "errorstack.hh"


OpenGD77SatelliteConfig::OpenGD77SatelliteConfig(QObject *parent)
  : OpenGD77BaseSatelliteConfig(parent)
{
  image(FLASH).addElement(Offset::satellites(), size()); // of 0x11a0 bytes
}


bool
OpenGD77SatelliteConfig::isValid() const {
  return OpenGD77BaseCodeplug::AdditionalSettingsElement((uint8_t *)data(Offset::satellites(), FLASH))
      .isValid();
}


void
OpenGD77SatelliteConfig::initialize() {
  OpenGD77BaseCodeplug::AdditionalSettingsElement(data(Offset::satellites(), FLASH)).clear();
}


bool
OpenGD77SatelliteConfig::encode(SatelliteDatabase *db, const ErrorStack &err) {
  OpenGD77BaseCodeplug::AdditionalSettingsElement settings(data(Offset::satellites(), FLASH));
  OpenGD77BaseCodeplug::SatelliteBankElement bank = settings.satellites();
  bank.clear();

  if (! bank.encode(db, err)) {
    errMsg(err) << "Cannot encode satellite config for OpenUV380.";
    return false;
  }

  return true;
}
