#include "openuv380_satelliteconfig.hh"
#include "errorstack.hh"


OpenUV380SatelliteConfig::OpenUV380SatelliteConfig(QObject *parent)
  : OpenGD77BaseSatelliteConfig(parent)
{
  image(FLASH).addElement(Offset::satellites(), size()); // of 0x11a0 bytes
}


bool
OpenUV380SatelliteConfig::encode(SatelliteDatabase *db, const ErrorStack &err) {
  SatelliteBankElement bank(data(Offset::satellites(), FLASH));
  if (! bank.encode(db, err)) {
    errMsg(err) << "Cannot encode satellite config for OpenUV380.";
    return false;
  }

  return true;
}
