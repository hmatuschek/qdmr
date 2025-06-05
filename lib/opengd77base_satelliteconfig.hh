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
  /** Encodes a satellite for the  OpenGD77 devices.
   * That is a set of orbital elements and transponder information. */
  class SatelliteElement: public Codeplug::Element
  {
  protected:
    /** Hidden constructor. */
    SatelliteElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    SatelliteElement(uint8_t *ptr);

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x0064; }

    void clear();

    /** Sets the name of the element. */
    virtual void setName(const QString &name);

    /** Sets the epoch. */
    virtual void setEpoch(const ::OrbitalElement::Epoch &epoch);
    /** Sets the first derivative of mean motion. */
    virtual void setMeanMotion(double mm);
    /** Sets the first derivative of mean motion. */
    virtual void setMeanMotionDerivative(double dmm);
    /** Sets the inclination. */
    virtual void setInclination(double incl);
    /** Right ascension of the ascending node. */
    virtual void setAscension(double asc);
    /** Sets eccentricity. */
    virtual void setEccentricity(double ecc);
    /** Sets argument of perigee. */
    virtual void setPerigee(double arg);
    /** Set the mean anomaly. */
    virtual void setMeanAnomaly(double ma);
    /** Sets the revolution number at epoch. */
    virtual void setRevolutionNumber(unsigned int num);

    /** Sets the downlink frequency. */
    void setFMDownlink(const Frequency &f);
    /** Sets the uplink frequency. */
    void setFMUplink(const Frequency &f);
    /** Sets the CTCSS tone. */
    void setCTCSS(const SelectiveCall &call);
    /** Sets the APRS downlink frequency. */
    void setAPRSDownlink(const Frequency &f);
    /** Sets the APRS uplink frequency. */
    void setAPRSUplink(const Frequency &f);

    /** Sets the beacon frequency. */
    void setBeacon(const Frequency &f);

    /** Sets the APRS path. */
    void setAPRSPath(const QString &path);

    /** Encodes a satellite. */
    virtual bool encode(const Satellite &sat, const ErrorStack &err = ErrorStack());

  protected:
    /** Writes a fixed point value as a BCD number. Using 0-9 as digits, ah as decimal dot and bh
     * as blank.
     * @param offset Specifies, where to write the fixed point value.
     * @param value The value to write.
     * @param sign If @c true, a sign is written.
     * @param dec The number of digits in the integer part.
     * @param frac The number of digits in the fractional part.
     */
    void writeFixedPoint(const Offset::Bit &offset, double value, bool sign, unsigned int dec, unsigned int frac);
    /** Writes a fixed point value as a BCD number. Using 0-9 as digits and bh as blank. In contrast
     * to @c writeFixedPoint, this function expects no integer part.
     * @param offset Specifies, where to write the fixed point value.
     * @param value The value to write.
     * @param sign If @c true, a sign is written.
     * @param frac The number of digits in the fractional part.
     */
    void writeFractional(const Offset::Bit &offset, double value, bool sign, unsigned int frac);
    /** Write a fixed digit integer value. */
    void writeInteger(const Offset::Bit &offset, int value, bool sign, unsigned dec);
    /** Writes a single digit at the given offset. */
    void writeDigit(const Offset::Bit &offset, uint8_t digit);

  public:
    /** Some limits for the zone bank. */
    struct Limit: public Element::Limit {
      /** The maximum name length. */
      static constexpr unsigned int nameLength() { return 8; }
      /** Maximum length of the APRS path. */
      static constexpr unsigned int pathLength() { return 24; }
    };

  protected:
    /** Some internal offsets within the element. */
    struct Offset: public Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int name()         { return 0x0000; }
      static constexpr Bit epochYear()             { return {0x0008, 4}; }
      static constexpr Bit epochJulienDay()        { return {0x0009, 4}; }
      static constexpr Bit meanMotionDerivative()  { return {0x000f, 4}; }
      static constexpr Bit inclination()           { return {0x0014, 4}; }
      static constexpr Bit ascension()             { return {0x0018, 4}; }
      static constexpr Bit eccentricity()          { return {0x001c, 4}; }
      static constexpr Bit perigee()               { return {0x001f, 0}; }
      static constexpr Bit meanAnomaly()           { return {0x0023, 0}; }
      static constexpr Bit meanMotion()            { return {0x0027, 0}; }
      static constexpr Bit revolutionNumber()      { return {0x002d, 4}; }
      static constexpr unsigned int fmDownlink()   { return 0x0030; }
      static constexpr unsigned int fmUplink()     { return 0x0034; }
      static constexpr unsigned int ctcss()        { return 0x0038; }
      static constexpr unsigned int aprsDownlink() { return 0x003c; }
      static constexpr unsigned int aprsUplink()   { return 0x0040; }
      static constexpr unsigned int beacon()       { return 0x0044; }
      static constexpr unsigned int aprsPath()     { return 0x004c; }
      /// @endcond
    };
  };


  /** Implements the satellite config bank. Holding all satellites to track. */
  class SatelliteBankElement: Codeplug::Element
  {
  protected:
    /** Hidden constructor. */
    SatelliteBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    SatelliteBankElement(uint8_t *ptr);

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x09d8; }

    void clear();

    /** Returns the i-th satellite. */
    SatelliteElement satellite(unsigned int idx);
    /** Encodes the given satellite database. */
    bool encode(SatelliteDatabase *db, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits for the satellite config. */
    struct Limit {
      /** The maximum number of satellites. */
      static constexpr unsigned int satellites() { return 25; }
    };

  protected:
    /** Some internal offsets. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int label()             { return 0x0000; }
      static constexpr unsigned int unknownInteger0()   { return 0x0008; }
      static constexpr unsigned int unknownInteger1()   { return 0x000c; }
      static constexpr unsigned int segmentSize()       { return 0x0010; }
      static constexpr unsigned int satellites()        { return 0x0014; }
      static constexpr unsigned int betweenSatellites() { return SatelliteElement::size(); }
      /// @endcond
    };
  };

public:
  /** Default constructor. */
  explicit OpenGD77BaseSatelliteConfig(QObject *parent = nullptr);

  /** Size of the image to write. */
  static constexpr unsigned int size() { return 0x09c0; }

  /** Encodes the given satellite database. */
  virtual bool encode(SatelliteDatabase *db, const ErrorStack &err=ErrorStack()) = 0;
};

#endif // OPENGD77BASE_SATELLITECONFIG_HH
