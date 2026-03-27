#ifndef D168UVLIMITS_HH
#define D168UVLIMITS_HH

#include "anytone_limits.hh"

/** Implements the limits for the AnyTone AT-D168UV.
 * @ingroup d878uv2 */
class D168UVLimits: public AnytoneLimits
{
  Q_OBJECT

public:
  /** Constructor. */
  D168UVLimits(const std::initializer_list<std::pair<Frequency, Frequency> > &rxFreqRanges,
               const std::initializer_list<std::pair<Frequency, Frequency> > &txFreqRanges,
               const QString &hardwareRevision, QObject *parent=nullptr);
};

#endif // D878UVLIMITS_HH
