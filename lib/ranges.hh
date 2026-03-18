#ifndef RANGES_HH
#define RANGES_HH

#include <initializer_list>
#include "interval.hh"
#include "frequency.hh"

/** Simple range class representing some range in some data type. Provides some methods to limit
 * values to these ranges.
 *
 * @ingroup utils */
template <class T>
class Range
{
public:
  /** Maps a given value onto the range. */
  inline T map(const T &n) const {
    if (n < lower)
      return lower;
    if (upper < n)
      return upper;
    return n;
  }

  /** Checks, if the given value lays within the range. */
  inline bool contains(const T &n) const {
    return ((lower<n) || (lower==n)) && ((n<upper) || (n==upper));
  }

public:
  T lower;   ///< Specifies the lower bound.
  T upper;   ///< Specifies the upper bound.
};


/** Implements a set of possibly overlapping ranges. */
template <class T>
class Ranges
{
public:
  /** Checks, if the given value lays within the range. */
  inline bool contains(const T &n) const {
    for (auto range: ranges) {
      if (range.contains(n))
        return true;
    }
    return false;
  }

public:
  /** The set of ranges. */
  QSet<Range<T>> ranges;
};


/** An integer range. */
typedef Range<unsigned int> IntRange;
/** A time range. */
typedef Range<Interval>     TimeRange;
/** A frequency range. */
typedef Range<Frequency>    FrequencyRange;

#endif // RANGES_HH
