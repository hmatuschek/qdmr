#ifndef INTERVAL_HH
#define INTERVAL_HH

#include <QString>
#include <QMetaType>
#include <yaml-cpp/yaml.h>

/** Represents a time interval.
 * @ingroup utils */
class Interval
{
public:
  /** Possible formats. */
  enum class Format {
    Automatic, Milliseconds, Seconds, Minutes
  };

protected:
  /** Constructor from milliseconds. */
  explicit Interval(unsigned long long ms);

public:
  /** Default constructor. */
  Interval();
  /** Copy constructor. */
  Interval(const Interval &other);

  inline Interval &operator =(const Interval &other) {   ///< Assignment.
    _duration = other._duration; return *this;
  }

  inline bool isNull() const { return 0 == _duration; }  ///< Test for 0.
  inline bool operator ==(const Interval &other) const { ///< Comparison.
    return _duration == other._duration;
  }

  inline unsigned long long milliseconds() const { return _duration; }      ///< Unit conversion.
  inline unsigned long long seconds() const { return _duration/1000ULL; }   ///< Unit conversion.
  inline unsigned long long minutes() const { return _duration/60000ULL; }  ///< Unit conversion.

  static inline Interval fromMilliseconds(unsigned long long ms) {          ///< Unit conversion.
    return Interval(ms);
  }
  static inline Interval fromSeconds(unsigned long long s) {                ///< Unit conversion.
    return Interval(s*1000ULL);
  }
  static inline Interval fromMinutes(unsigned long long min) {              ///< Unit conversion.
    return Interval(min*60000ULL);
  }

  /** Format the frequency. */
  QString format(Format f=Format::Automatic) const;
  /** Parses a frequency. */
  bool parse(const QString &value);

private:
  /** An interval duration in ms. */
  unsigned long long _duration;
};

Q_DECLARE_METATYPE(Interval)

namespace YAML
{
  /** Implements the conversion to and from YAML::Node. */
  template<>
  struct convert<Interval>
  {
    /** Serializes the interval. */
    static Node encode(const Interval& rhs) {
      return Node(rhs.format().toStdString());
    }

    /** Parses the interval. */
    static bool decode(const Node& node, Interval& rhs) {
      if (!node.IsScalar())
        return false;
      return rhs.parse(QString::fromStdString(node.as<std::string>()));
    }
  };
}


#endif // INTERVAL_HH
