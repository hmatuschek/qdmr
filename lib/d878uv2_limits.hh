#ifndef D878UV2LIMITS_HH
#define D878UV2LIMITS_HH

#include "anytone_limits.hh"

/** Implements the limits for the AnyTone AT-D878UV2.
 * @ingroup d878uv2 */
class D878UV2Limits: public AnytoneLimits
{
  Q_OBJECT

public:
  /** Constructor. */
  D878UV2Limits(const std::initializer_list<std::pair<double, double>> &rxFreqRanges,
                const std::initializer_list<std::pair<double, double>> &txFreqRanges,
                const QString &hardwareRevision, QObject *parent=nullptr);
};

#endif // D878UV2LIMITS_HH
