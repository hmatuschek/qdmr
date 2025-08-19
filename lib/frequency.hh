#ifndef FREQUENCY_HH
#define FREQUENCY_HH

#include <yaml-cpp/yaml.h>
#include <QString>
#include <QMetaType>
#include <QList>
#include <QPair>


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

  inline bool operator<   (const Frequency &other) const {   ///< Comparison.
    return _frequency < other._frequency;
  }
  inline bool operator <=(const Frequency &other) const {    ///< Comparison.
    return _frequency <= other._frequency;
  }
  inline bool operator==  (const Frequency &other) const {   ///< Comparison.
    return _frequency == other._frequency;
  }
  inline bool operator != (const Frequency &other) const {   ///< Comparison.
    return _frequency != other._frequency;
  }
  inline bool operator>   (const Frequency &other) const {   ///< Comparison.
    return _frequency > other._frequency;
  }
  inline bool operator >=(const Frequency &other) const {    ///< Comparison.
    return _frequency >= other._frequency;
  }

  /** Format the frequency. */
  QString format(Format f=Format::Automatic) const;
  /** Parses a frequency. */
  bool parse(const QString &value);
  /** Pareses a frequency. */
  static Frequency fromString(const QString &freq);

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


public:
  /** Searches for the nearest frequency and returns an associated value. */
  template <class T>
  class MapNearest
  {
  protected:
    typedef QList<QPair<Frequency, T>> container;
    typedef typename container::const_iterator const_iterator;

  public:
    /** Constructs a nearest frequency mapping. */
    MapNearest(const QList<QPair<Frequency, T>> &items)
      : _items(items)
    {
      // Sort items ascending.
      std::sort(_items.begin(), _items.end(), compItem);
    }

    /** Searches for the nearest item to the given frequency. */
    const_iterator
    find(const Frequency &f) const {
      auto lower = std::lower_bound(_items.begin(), _items.end(), f, compItemValue);

      if (_items.begin() == lower)
        return lower;

      auto upper = lower--;
      if (_items.end() == upper)
        return lower;

      if ((f.inHz()-lower->first.inHz()) < (upper->first.inHz()-f.inHz()))
        return lower;

      return upper;
    }

    /** Returns the value associated with the nearest item for the given frequency. */
    T value(const Frequency &f) const {
      return find(f)->second;
    }

    /** Returns the frequency of the nearest item for the given frequency. */
    Frequency key(const Frequency &f) const {
      return find(f)->first;
    }

  protected:
    /** Comparison of items by frequency. */
    inline static bool compItem(const QPair<Frequency, T> &a, const QPair<Frequency, T> &b) {
      return a.first < b.first;
    }

    /** Comparison of item and frequency. */
    inline static bool compItemValue(const QPair<Frequency, T> &a, const Frequency &b) {
      return a.first < b;
    }

  protected:
    /** All items. */
    container _items;
  };
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
      if (! node.IsScalar())
        return false;
      return rhs.parse(QString::fromStdString(node.as<std::string>()));
    }
  };
}

#endif // FREQUENCY_HH
