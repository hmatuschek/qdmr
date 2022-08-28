#ifndef GD77LIMITS_HH
#define GD77LIMITS_HH

#include "radiolimits.hh"

/** Implements the limits for the Radioddity GD77.
 * @ingroup gd77 */
class GD77Limits: public RadioLimits
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit GD77Limits(QObject *parent=nullptr);
};

#endif // GD77LIMITS_HH
