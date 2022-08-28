#ifndef D868UVLIMITS_HH
#define D868UVLIMITS_HH

#include "anytone_limits.hh"

/** Implements the limits for the AnyTone AT-D878UV.
 * @ingroup d868uv */
class D868UVLimits: public AnytoneLimits
{
  Q_OBJECT

public:
  /** Constructor. */
  D868UVLimits(const std::initializer_list<std::pair<double, double>> &rxFreqRanges,
               const std::initializer_list<std::pair<double, double>> &txFreqRanges,
               const QString &hardwareRevision, QObject *parent=nullptr);
};

#endif // D868UVLIMITS_HH
