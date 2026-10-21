#ifndef HD1_LIMITS_HH
#define HD1_LIMITS_HH

#include "radiolimits.hh"

/** Limits of the Ailunce HD1.
 * @ingroup hd1 */
class HD1Limits : public RadioLimits
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit HD1Limits(QObject *parent=nullptr);
};

#endif // HD1_LIMITS_HH
