#ifndef FREQUENCY_HH
#define FREQUENCY_HH

#include <yaml-cpp/yaml.h>
#include <QString>
#include <QMetaType>

/** Helper type to encode frequencies without any rounding error.
 * @ingroup utils */
struct Frequency
{
public:
  /** Possible formatting hints. */
  enum class Format {
    Automatic, Hz, kHz, MHz, GHz
  };

protected:
  /** Hidden constructor from frequency in Hz. */
  Frequency(unsigned long long hz);

public:
  /** Default constructor. */
  Frequency();
  /** Copy constructor. */
  Frequency(const Frequency &other);

  /** Assignment. */
  Frequency &operator =   (const Frequency &other);
  inline bool operator==  (const Frequency &other) const {   ///< Comparison.
    return _frequency == other._frequency;
  }

  /** Format the frequency. */
  QString format(Format f=Format::Automatic) const;
  /** Parses a frequency. */
  bool parse(const QString &value);

  inline unsigned long long inHz() const { return _frequency; }    ///< Unit conversion.
  inline double inkHz() const { return double(_frequency)/1e3; }   ///< Unit conversion.
  inline double inMHz() const { return double(_frequency)/1e6; }   ///< Unit conversion.
  inline double inGHz() const { return double(_frequency)/1e9; }   ///< Unit conversion.

  static inline Frequency fromHz(unsigned long long Hz) { return Frequency(Hz); } ///< Unit conversion.
  static inline Frequency fromkHz(double kHz) { return Frequency(kHz*1e3); }      ///< Unit conversion.
  static inline Frequency fromMHz(double MHz) { return Frequency(MHz*1e6); }      ///< Unit conversion.
  static inline Frequency fromGHz(double GHz) { return Frequency(GHz*1e6); }      ///< Unit conversion.

protected:
  /** The actual frequency in Hz. */
  unsigned long long _frequency;
};

Q_DECLARE_METATYPE(Frequency)


namespace YAML
{
  /** Implements the conversion to and from YAML::Node. */
  template<>
  struct convert<Frequency>
  {
    /** Serializes the frequency. */
    static Node encode(const Frequency& rhs) {
      return Node(rhs.format().toStdString());
    }

    /** Parses the frequency. */
    static bool decode(const Node& node, Frequency& rhs) {
      if (!node.IsScalar())
        return false;
      return rhs.parse(QString::fromStdString(node.as<std::string>()));
    }
  };
}
#endif // FREQUENCY_HH
