#ifndef LEVEL_HH
#define LEVEL_HH

#include <QString>
#include <QMetaType>
#include <limits>
#include <yaml-cpp/yaml.h>
#include "codeplug.hh"



/** Some simple class implementing a [1-10] level setting. Also supports the "values" null and
 * invalid. */
class Level
{
protected:
  /** Constructor from value. */
  inline constexpr Level(unsigned int value) : _level(value) {}

public:
  /** Default constructor. */
  Level();

  inline constexpr Level(const Level &other)
    : _level(other._level)
  {
    // pass...
  }

  inline Level &operator =(const Level &other) = default;

  inline bool isNull() const { return 0 == _level; }  ///< Test for 0.
  /** Test for invalid level. */
  inline bool isInvalid() const {
    return std::numeric_limits<unsigned int>::max() == _level;
  }
  /** Test for finite values. */
  inline bool isFinite() const { return (!isNull()) && (!isInvalid()); }

  inline bool operator ==(const Level &other) const { ///< Comparison.
    return _level == other._level;
  }
  inline bool operator<  (const Level &other) const {///< Comparison.
    return _level < other._level;
  }
  inline bool operator<=  (const Level &other) const {///< Comparison.
    return _level <= other._level;
  }
  inline bool operator>  (const Level &other) const {///< Comparison.
    return _level > other._level;
  }
  inline bool operator>=  (const Level &other) const {///< Comparison.
    return _level >= other._level;
  }

  /** Returns the value of the level. */
  inline unsigned int value() const { return _level; }
  inline unsigned int mapTo(const Codeplug::Element::Limit::Range<unsigned int> &range) const {
    if (isNull() || isInvalid())
      return 0;
    return Codeplug::Element::Limit::Range<unsigned int>{1,10}.mapTo(range, value());
  }

  /** Format the frequency. */
  QString format() const;
  /** Parses a frequency. */
  bool parse(const QString &value);

public:
  /** Constructs null level. */
  inline static constexpr Level null() { return Level(0); }
  /** Constructs an invalid level. */
  inline static constexpr Level invalid() { return Level(std::numeric_limits<unsigned int>::max()); }
  /** Constructs a proper level. */
  inline static constexpr Level fromValue(unsigned int value, const Codeplug::Element::Limit::Range<unsigned int> range={1,10}) {
    if (0 == value)
      return Level::null();
    return Level(range.mapTo({1,10},value));
  }

protected:
  /** The actual level value. */
  unsigned int _level;
};


Q_DECLARE_METATYPE(Level)


namespace YAML
{
/** Implements the conversion to and from YAML::Node. */
template<>
struct convert<Level>
{
  /** Serializes the interval. */
  static Node encode(const Level& rhs) {
    return Node(rhs.format().toStdString());
  }

  /** Parses the interval. */
  static bool decode(const Node& node, Level& rhs) {
    // Usually means default level
    if (node.IsNull()) {
      rhs = Level::invalid();
      return true;
    }
    // Fails on non-scalars
    if (!node.IsScalar())
      return false;
    // Otherwise, parse string.
    return rhs.parse(QString::fromStdString(node.as<std::string>()));
  }
};
}


#endif // LEVEL_HH
