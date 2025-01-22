#include "opengd77_satelliteconfig.hh"
#include "errorstack.hh"


OpenGD77SatelliteConfig::OpenGD77SatelliteConfig(QObject *parent)
  : OpenGD77BaseSatelliteConfig(parent)
{
  image(FLASH).addElement(Offset::satellites(), 0x11a0);
}


bool
OpenGD77SatelliteConfig::encode(SatelliteDatabase *db, const ErrorStack &err) {
  SatelliteBankElement bank(data(Offset::satellites(), FLASH));
  if (! bank.encode(db, err)) {
    errMsg(err) << "Cannot encode satellite config for OpenGD77.";
    return false;
  }
  return true;
}
