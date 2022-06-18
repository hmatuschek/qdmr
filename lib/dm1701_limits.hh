#ifndef DM1701LIMITS_HH
#define DM1701LIMITS_HH

#include "radiolimits.hh"

/** Implements the configuration limits for the Baofeng DM-1701.
 * @ingroup dm1701 */
class DM1701Limits : public RadioLimits
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit DM1701Limits(QObject *parent=nullptr);
};

#endif // UV390LIMITS_HH
