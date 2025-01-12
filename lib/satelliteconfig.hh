#ifndef SATELLITECONFIG_HH
#define SATELLITECONFIG_HH

#include "dfufile.hh"


// Forward declarations
class SatelliteDatabase;


/** Baseclass of all satellite database configurations.
 * That is, the device specific encoding of the satellite orbitals and transponder information.
 * @ingroup conf */
class SatelliteConfig : public DFUFile
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit SatelliteConfig(QObject *parent = nullptr);

public:
  /** Encodes the given satellite db into the device specific satellite configuration. */
  virtual bool encode(SatelliteDatabase *db, const ErrorStack &err=ErrorStack()) = 0;
};

#endif // SATELLITECONFIG_HH
