#ifndef LIBDMRCONF_RT4D_LIMITS_HH
#define LIBDMRCONF_RT4D_LIMITS_HH

#include "radiolimits.hh"

/** Implements the configuration limits for the Radtel RT-4D.
 * @ingroup rt4d */
class RT4DLimits : public RadioLimits
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit RT4DLimits(QObject *parent=nullptr);
};

#endif // LIBDMRCONF_RT4D_LIMITS_HH
