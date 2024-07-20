#ifndef GD73LIMITS_HH
#define GD73LIMITS_HH

#include "radiolimits.hh"

/** Implements the limits for the Radioddity GD77.
 * @ingroup gd73 */
class GD73Limits: public RadioLimits
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit GD73Limits(QObject *parent=nullptr);
};

#endif // GD73LIMITS_HH
