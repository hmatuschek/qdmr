#ifndef UV390LIMITS_HH
#define UV390LIMITS_HH

#include "radiolimits.hh"

/** Implements the configuration limits for the TyT MD-UV390 and Retevis RT3S.
 * @ingroup uv390 */
class UV390Limits : public RadioLimits
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit UV390Limits(QObject *parent=nullptr);
};

#endif // UV390LIMITS_HH
