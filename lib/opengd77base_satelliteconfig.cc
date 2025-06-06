#include "opengd77base_satelliteconfig.hh"
#include "satellitedatabase.hh"




/* ********************************************************************************************* *
 * Implementation of OpenGD77BaseSatelliteConfig
 * ********************************************************************************************* */
OpenGD77BaseSatelliteConfig::OpenGD77BaseSatelliteConfig(QObject *parent)
  : SatelliteConfig{parent}
{
  addImage("OpenGD77 satellite configuration EEPROM");
  addImage("OpenGD77 satellite configuration FLASH");
}
