#ifndef D168UV_SATELLITECONFIG_HH
#define D168UV_SATELLITECONFIG_HH

#include "anytone_satelliteconfig.hh"

/** Implements the satellite configuration for the AnyTone AT-D168UV.
 * @ingroup d168uv */
class D168UVSatelliteConfig : public AnytoneSatelliteConfig
{
Q_OBJECT

public:
  /** Default constructor. */
  explicit D168UVSatelliteConfig(QObject *parent = nullptr);

  /** Returns the satellite element at the specified index. */
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


#endif //D168UV_SATELLITECONFIG_HH
