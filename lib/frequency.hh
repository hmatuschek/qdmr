#ifndef FREQUENCY_HH
#define FREQUENCY_HH

#include <yaml-cpp/yaml.h>
#include <QString>
#include <QMetaType>
#include <QList>
#include <QPair>


/** Common base for all frequencies. That is frequencies (positive) and frequency offsets
 * (positive and negative). To this end, the type stores frequencies in Hz as a signed value. */
struct FrequencyBase
{
public:
  /** Frequency units. */
  enum class Unit { Auto, Hz, kHz, MHz, GHz };

protected:
  /** Hidden constructor from offset in Hz. */
  FrequencyBase(qint64 hz) : _frequency(hz) {}

public:
  /** Copy constructor. */
  FrequencyBase(const FrequencyBase &other);

  /** Assignment. */
  FrequencyBase &operator = (const FrequencyBase &other);

  /** Retruns @c true of the frequency is negative. */
  inline bool isNegative() const { return 0 > _frequency; }
  /** Retruns @c true of the frequency is positive. */
  inline bool isPositive() const { return 0 < _frequency; }
  /** Retruns @c true of the frequency is zero. */
  inline bool isZero() const { return 0 == _frequency; }

  /** Format the frequency. */
  QString format(Unit unit = Unit::Auto) const;
  /** Parses a frequency. */
  bool parse(const QString &value, Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive);

  inline long long inHz() const { return _frequency; }             ///< Unit conversion.
  inline double inkHz() const { return double(_frequency)/1e3; }   ///< Unit conversion.
  inline double inMHz() const { return double(_frequency)/1e6; }   ///< Unit conversion.
  inline double inGHz() const { return double(_frequency)/1e9; }   ///< Unit conversion.

  /** Returns the most appropriate unit for the frequency value. */
  Unit unit() const;

  /** Returns unit as type base on string input */
  Unit unitFromString(const QString& input) const;

  /** Checks if frequency is a multiple of unit. */
  bool isMultipleOf(Unit unit) const;

  /** Helper for string conversion of unit. */
  static QString unitName(Unit unit);

protected:
  /** The actual frequency in Hz. */
  qint64 _frequency;
};

struct Frequency;

/** Helper type to represent frequency differences aka offsets.
 * @ingroup utils */
struct FrequencyOffset: public FrequencyBase
{
friend struct Frequency;
protected:
  /** Hidden constructor. */
  FrequencyOffset(qint64 Hz) : FrequencyBase(Hz) { }

public:
  /** Default constructor. */
  FrequencyOffset();
  /** Copy constructor. */
  FrequencyOffset(const FrequencyOffset &other);

  /** Assignment. */
  FrequencyOffset &operator = (const FrequencyOffset &other);

  /** Invert an offset */
  inline FrequencyOffset invert() const { return FrequencyOffset(this->_frequency * -1); }

  /** Returns a positive frequency offset. */
  inline FrequencyOffset abs() const {
    return FrequencyOffset(std::abs(this->_frequency));
  }

  /** Parses an offset. */
  static FrequencyOffset fromString(const QString &freq);

  static inline FrequencyOffset fromHz(qint64 Hz)   { return FrequencyOffset(Hz); }             ///< Unit conversion.
  static inline FrequencyOffset fromkHz(double kHz) { return FrequencyOffset(kHz*1e3); }      ///< Unit conversion.
  static inline FrequencyOffset fromMHz(double MHz) { return FrequencyOffset(MHz*1e6); }      ///< Unit conversion.
  static inline FrequencyOffset fromGHz(double GHz) { return FrequencyOffset(GHz*1e6); }      ///< Unit conversion.
};



/** Helper type to encode frequencies without any rounding error.
 * This is a specialization of @c FrequencyBase, enforcing a positive integer.
 * @ingroup utils */
struct Frequency: public FrequencyBase
{
protected:
  /** Hidden constructor from frequency in Hz. */
  Frequency(quint64 hz);

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

  /** Frequency arithmetic. May result in an invalid frequency, if the negative offset is larger
   * than the frequency. */
  Frequency operator+(const FrequencyOffset &offset) const;
  /** Frequency arithmetic. May result in an invalid frequency, if the negative offset is larger
   * than the frequency. */
  Frequency &operator+=(const FrequencyOffset &offset);
  FrequencyOffset operator-(const Frequency &other) const;    ///< Frequency arithmetic.

  /** Pareses a frequency. */
  static Frequency fromString(const QString &freq);

  inline quint64 inHz() const { return _frequency; }    ///< Unit conversion.

  static inline Frequency fromHz(quint64 Hz)  { return Frequency(Hz); }      ///< Unit conversion.
  static inline Frequency fromkHz(double kHz) { return Frequency(kHz*1e3); } ///< Unit conversion.
  static inline Frequency fromMHz(double MHz) { return Frequency(MHz*1e6); } ///< Unit conversion.
  static inline Frequency fromGHz(double GHz) { return Frequency(GHz*1e6); } ///< Unit conversion.

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
      return rhs.parse(QString::fromStdString(node.as<std::string>()), Qt::CaseSensitive);
    }
  };
}

#endif // FREQUENCY_HH
