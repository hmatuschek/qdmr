#ifndef DMR6X2UV2LIMITS_HH
#define DMR6X2UV2LIMITS_HH

#include "anytone_limits.hh"

/** Implements the limits for the BTECH DMR-6X2UV PRO.
 * @ingroup dmr6x2uv2 */
class DMR6X2UV2Limits: public AnytoneLimits
{
  Q_OBJECT

public:
  /** Constructor. */
  DMR6X2UV2Limits(const std::initializer_list<std::pair<Frequency, Frequency> > &rxFreqRanges,
                  const std::initializer_list<std::pair<Frequency, Frequency> > &txFreqRanges,
                  const QString &hardwareRevision, QObject *parent=nullptr);
};

#endif // D878UVLIMITS_HH
