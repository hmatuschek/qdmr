#include "d168uv_satelliteconfig.hh"
#include "satellitedatabase.hh"
#include "logger.hh"


/* ********************************************************************************************* *
 * Implementation of D168UVSatelliteConfig
 * ********************************************************************************************* */
D168UVSatelliteConfig::D168UVSatelliteConfig(QObject *parent)
  : AnytoneSatelliteConfig{parent}
{
  // pass...
}


AnytoneSatelliteConfig::SatelliteElement
D168UVSatelliteConfig::satellite(unsigned int idx) {
  return SatelliteElement(data(Offset::satellites() + idx*Offset::betweenSatellites()));
}


bool
D168UVSatelliteConfig::encode(SatelliteDatabase *db, const ErrorStack &err) {
  unsigned int numSat = std::min(Limit::satellites(), db->count());
  image(0).addElement(Offset::satellites(), numSat*SatelliteElement::size());

  for (unsigned int i=0; i<numSat; i++) {
    logDebug() << "Encode sat '" << db->getAt(i).name() << "' at index " << i << ".";
    if (! satellite(i).encode(db->getAt(i), err)) {
      errMsg(err) << "Cannot encode satellite '" << db->getAt(i).name()
                  << "at index " << i << ".";
      return false;
    }
  }

  return true;
}
