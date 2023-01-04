#ifndef DMR6X2UVLIMITS_HH
#define DMR6X2UVLIMITS_HH

#include "anytone_limits.hh"

/** Implements the limits for the BTECH DMR-6X2UV.
 * @ingroup dmr6x2uv */
class DMR6X2UVLimits: public AnytoneLimits
{
  Q_OBJECT

public:
  /** Constructor. */
  DMR6X2UVLimits(const std::initializer_list<std::pair<double, double>> &rxFreqRanges,
                 const std::initializer_list<std::pair<double, double>> &txFreqRanges,
                 const QString &hardwareRevision, QObject *parent=nullptr);
};

#endif // D878UVLIMITS_HH
