#ifndef MD2017LIMITS_HH
#define MD2017LIMITS_HH

#include "radiolimits.hh"

/** Implements the limits for the TyT MD-2017.
 * @ingroup md2017 */
class MD2017Limits: public RadioLimits
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit MD2017Limits(QObject *parent=nullptr);
};

#endif // MD2017LIMITS_HH
