#include "openuv380_satelliteconfig.hh"
#include "errorstack.hh"
#include "logger.hh"


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
    QString magic = settings.magic();
    unsigned int version = settings.version();
    // Only refuse when a real "OpenGD77" header is present with an unknown
    // version: that's a newer firmware format we must not trash. Anything else
    // (virgin flash 0xff, leftovers from a previous firmware, factory garbage)
    // is fair game — the firmware itself ignores the region without the magic.
    if (magic == "OpenGD77") {
      errMsg(err) << "Refusing to overwrite OpenGD77 additional-settings region: "
                  << "version " << version << " is not supported (expected 1). "
                  << "The radio firmware may be newer than this qdmr build.";
      return false;
    }
    logDebug() << "OpenUV380 additional-settings region at 0x"
               << QString::number(Offset::satellites(), 16)
               << " has no valid header (magic='" << magic
               << "', version=" << version << "). "
               << "Initializing OpenGD77 settings header.";
    settings.clear();
  }

  OpenGD77BaseCodeplug::SatelliteBankElement bank = settings.satellites();
  bank.clear();

  if (! bank.encode(db, err)) {
    errMsg(err) << "Cannot encode satellite config for OpenUV380.";
    return false;
  }

  return true;
}
