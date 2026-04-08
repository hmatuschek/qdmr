#ifndef D168UV_SATELLITECONFIG_HH
#define D168UV_SATELLITECONFIG_HH

#include "anytone_satelliteconfig.hh"

class D168UVSatelliteConfig : public AnytoneSatelliteConfig
{
Q_OBJECT

public:
  /** Default constructor. */
  explicit D168UVSatelliteConfig(QObject *parent = nullptr);

  SatelliteElement satellite(unsigned int idx);
  bool encode(SatelliteDatabase *db, const ErrorStack &err) override;

  /** Some limits for the satellite config. */
  struct Limit: AnytoneSatelliteConfig::Limit {
    /// Number of satellites.
    static constexpr unsigned int satellites() { return 25; }
  };

protected:
  /** Some internal offsets. */
  struct Offset: AnytoneSatelliteConfig::Offset {
    /// @cond DO_NOT_DOCUMENT
    static constexpr unsigned int satellites() { return 0x2d40000; }
    /// @endcond
  };
};


#endif //QDMR_D168UVSATELLITECONFIG_HH
