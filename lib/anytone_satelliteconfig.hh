#ifndef ANYTONE_SATELLITECONFIG_HH
#define ANYTONE_SATELLITECONFIG_HH

#include "satelliteconfig.hh"
#include "frequency.hh"
#include "signaling.hh"
#include "codeplug.hh"
#include "orbitalelementsdatabase.hh"

// Forward declarations
class Satellite;

/** Implementation of satellite confifuration for all AnyTone devices.
 * @ingroup anytone */
class AnytoneSatelliteConfig : public SatelliteConfig
{
  Q_OBJECT

public:
  class SatelliteElement: Codeplug::Element
  {
  protected:
    /** Internal encoding of sub tone type. */
    enum class ToneType {
      None = 0, CTCSS = 1, DCS = 2
    };

  protected:
    /** Hidden constructor. */
    SatelliteElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    explicit SatelliteElement(uint8_t *ptr);

    static constexpr unsigned int size() { return 0x200; }

    void clear();

    /** Sets the satellite name. */
    void setName(const QString &name);

    /** Sets the epoch of the orbital element. */
    void setEpoch(const OrbitalElement::Epoch &epoch);
    /** Sets the derivative of the mean motion. */
    void setMeanMotionDerivative(double dmm);
    /** Sets the inclination. */
    void setInclination(double incl);
    /** Sets the right ascension of the ascending node. */
    void setAscension(double asc);
    /** Sets the eccentricity */
    void setEccentricity(double ecc);
    /** Sets the argument of perigee. */
    void setPerigee(double peri);
    /** Sets the mean anomaly. */
    void setAnomaly(double ma);
    /** Sets the mean motion. */
    void setMeanMotion(double mm);
    /** Sets the revolution number. */
    void setRevolution(unsigned int num);

    /** Sets the downlink frequency. */
    void setDownlink(const Frequency &f);
    /** Sets the downlink sub tone. */
    void setDownlinkTone(const SelectiveCall &tone);
    /** Sets the uplink frequency. */
    void setUplink(const Frequency &f);
    /** Sets the uplink sub tone. */
    void setUplinkTone(const SelectiveCall &tone);

    /** Encodes the given satellite. */
    bool encode(const Satellite &sat, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits for the satellite. */
    struct Limit: Element::Limit {
      /** Maximum size of satellite name. */
      static constexpr unsigned int name() { return 8; }
    };

  protected:
    /** Some internal offsets. */
    struct Offset: Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int name() { return 0x0000; }
      static constexpr unsigned int epochYear() { return 0x0008; }
      static constexpr unsigned int epochDay() { return 0x000a; }
      static constexpr unsigned int meanMotionDerivative() { return 0x0017; }
      static constexpr unsigned int inclination() { return 0x0021; }
      static constexpr unsigned int ascension() { return 0x002a; }
      static constexpr unsigned int eccentricity() { return 0x0033; }
      static constexpr unsigned int perigee() { return 0x003b; }
      static constexpr unsigned int anomaly() { return 0x0044; }
      static constexpr unsigned int meanMotion() { return 0x004d; }
      static constexpr unsigned int revolution() { return 0x0058; }
      static constexpr unsigned int downlinkFrequency() { return 0x0060; }
      static constexpr unsigned int uplinkFrequency() { return 0x0064; }
      static constexpr unsigned int uplinkToneType() { return 0x0068; }
      static constexpr unsigned int downlinkToneType() { return 0x0069; }
      static constexpr unsigned int uplinkCTCSS() { return 0x006a; }
      static constexpr unsigned int downlinkCTCSS() { return 0x006b; }
      static constexpr unsigned int uplinkDCS() { return 0x006c; }
      static constexpr unsigned int downlinkDCS() { return 0x006e; }
      /// @endcond
    };
  };

public:
  /** Default constructor. */
  explicit AnytoneSatelliteConfig(QObject *parent = nullptr);

  SatelliteElement satellite(unsigned int idx);
  bool encode(SatelliteDatabase *db, const ErrorStack &err=ErrorStack());

public:
  /** Some limits for the satellite config. */
  struct Limit {
    static constexpr unsigned int satellites() { return 80; }
  };

protected:
  /** Some internal offsets. */
  struct Offset {
    /// @cond DO_NOT_DOCUMENT
    static constexpr unsigned int satellites() { return 0x2ec0000; }
    static constexpr unsigned int betweenSatellites() { return SatelliteElement::size(); }
    /// @endcond
  };
};

#endif // ANYTONE_SATELLITECONFIG_HH
