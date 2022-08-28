#ifndef D878UVLIMITS_HH
#define D878UVLIMITS_HH

#include "anytone_limits.hh"

/** Implements the limits for the AnyTone AT-D878UV.
 * @ingroup d878uv */
class D878UVLimits: public AnytoneLimits
{
  Q_OBJECT

public:
  /** Constructor. */
  D878UVLimits(const std::initializer_list<std::pair<double, double>> &rxFreqRanges,
               const std::initializer_list<std::pair<double, double>> &txFreqRanges,
               const QString &hardwareRevision, QObject *parent=nullptr);
};

#endif // D878UVLIMITS_HH
