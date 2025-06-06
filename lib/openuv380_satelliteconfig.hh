#ifndef OPENUV380_SATELLITECONFIG_HH
#define OPENUV380_SATELLITECONFIG_HH

#include "opengd77base_codeplug.hh"
#include "opengd77base_satelliteconfig.hh"


class OpenUV380SatelliteConfig : public OpenGD77BaseSatelliteConfig
{
  Q_OBJECT

public:
  /** Default constructor. */
  explicit OpenUV380SatelliteConfig(QObject *parent = nullptr);

  bool isValid() const;
  void initialize();
  bool encode(SatelliteDatabase *db, const ErrorStack &err=ErrorStack());

public:
  /** Some limits for the satellite config. */
  struct Limit {
    /** The maximum number of satellites. */
    static constexpr unsigned int satellites() {
      return OpenGD77BaseCodeplug::SatelliteBankElement::Limit::satellites();
    }
  };

protected:
  /** Some internal offsets. */
  struct Offset {
    /// @cond DO_NOT_DOCUMENT
    static constexpr unsigned int satellites()        { return 0x020000; }
    /// @endcond
  };
};

#endif // OPENUV380_SATELLITECONFIG_HH
