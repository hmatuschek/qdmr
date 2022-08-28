#ifndef MD390LIMITS_HH
#define MD390LIMITS_HH

#include "radiolimits.hh"

/** Implements the radio limits for TyT MD-390 radios.
 * @ingroup md390 */
class MD390Limits: public RadioLimits
{
  Q_OBJECT

public:
  /** Constructor from frequency ranges. */
  MD390Limits(const std::initializer_list<std::pair<double,double>> &freqRanges, QObject *parent=nullptr);
};

#endif // MD390LIMITS_HH
