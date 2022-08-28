#ifndef D578UVLIMITS_HH
#define D578UVLIMITS_HH

#include "anytone_limits.hh"

/** Implements the limits for the AnyTone AT-D878UV.
 * @ingroup d578 */
class D578UVLimits: public AnytoneLimits
{
  Q_OBJECT

public:
  /** Constructor. */
  D578UVLimits(const std::initializer_list<std::pair<double, double>> &rxFreqRanges,
               const std::initializer_list<std::pair<double, double>> &txFreqRanges,
               const QString &hardwareRevision, QObject *parent=nullptr);
};

#endif // D578UVLIMITS_HH
