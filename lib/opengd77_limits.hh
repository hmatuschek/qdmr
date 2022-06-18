#ifndef OPENGD77LIMITS_HH
#define OPENGD77LIMITS_HH

#include "radiolimits.hh"

/** Implements the limits for the OpenGD77 firmware.
 * @ingroup @ogd77 */
class OpenGD77Limits: public RadioLimits
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit OpenGD77Limits(QObject *parent=nullptr);
};

#endif // OPENGD77LIMITS_HH
