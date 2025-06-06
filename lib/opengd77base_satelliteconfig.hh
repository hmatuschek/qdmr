#ifndef OPENGD77BASE_SATELLITECONFIG_HH
#define OPENGD77BASE_SATELLITECONFIG_HH

#include <orbitalelementsdatabase.hh>
#include <satelliteconfig.hh>
#include <frequency.hh>
#include <signaling.hh>
#include <satellitedatabase.hh>
#include <codeplug.hh>


/** Implements the satellite tracking configuration for the OpenGD77 type radios.
 * @ingroup ogd77 */
class OpenGD77BaseSatelliteConfig : public SatelliteConfig
{
  Q_OBJECT

public:
  /** Possible image types. */
  enum ImageType { EEPROM = 0, FLASH = 1 };


public:
  /** Default constructor. */
  explicit OpenGD77BaseSatelliteConfig(QObject *parent = nullptr);

  /** Size of the image to write. */
  static constexpr unsigned int size() { return 0x11a0; }

  /** Returns @c true, if the additional settings element is valid, that should contain the
   * satellite settings. */
  virtual bool isValid() const = 0;

  /** Initializes and clears the additional settings element. */
  virtual void initialize() = 0;

  /** Encodes the given satellite database. */
  virtual bool encode(SatelliteDatabase *db, const ErrorStack &err=ErrorStack()) = 0;
};

#endif // OPENGD77BASE_SATELLITECONFIG_HH
