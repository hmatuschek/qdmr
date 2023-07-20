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
  inline T conains(const T &n) const {
    return ((lower<n) || (lower==n)) && ((n<upper) || (n==upper));
  }

public:
  T lower;   ///< Specifies the lower bound.
  T upper;   ///< Specifies the upper bound.
};


/** An integer range. */
typedef Range<unsigned int> IntRange;
/** A time range. */
typedef Range<Interval>     TimeRange;
/** A frequency range. */
typedef Range<Frequency>    FrequencyRange;

#endif // RANGES_HH
