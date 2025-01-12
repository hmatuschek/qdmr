#ifndef OPENGD77_SATELLITECONFIG_HH
#define OPENGD77_SATELLITECONFIG_HH

#include "opengd77base_satelliteconfig.hh"


class OpenGD77SatelliteConfig : public OpenGD77BaseSatelliteConfig
{
  Q_OBJECT

public:
  /** Default constructor. */
  explicit OpenGD77SatelliteConfig(QObject *parent = nullptr);

  /** Encodes the given satellite database. */
  virtual bool encode(SatelliteDatabase *db, const ErrorStack &err=ErrorStack());

public:
  /** Some limits for the satellite config. */
  struct Limit {
    /** The maximum number of satellites. */
    static constexpr unsigned int satellites() {
      return OpenGD77BaseSatelliteConfig::SatelliteBankElement::Limit::satellites();
    }
  };

protected:
  /** Some internal offsets. */
  struct Offset {
    /// @cond DO_NOT_DOCUMENT
    static constexpr unsigned int satellites()        { return 0x000000; }
    /// @endcond
  };
};

#endif // OPENGD77_SATELLITECONFIG_HH
