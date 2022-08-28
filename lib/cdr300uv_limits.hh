#ifndef CDR300UVLIMITS_HH
#define CDR300UVLIMITS_HH

#include "radiolimits.hh"

/** Implements the radio limits for TyT MD-390 radios.
 * @ingroup cdr300uv */
class CDR300UVLimits: public RadioLimits
{
  Q_OBJECT

public:
  /** Constructor from frequency ranges. */
  CDR300UVLimits(QObject *parent=nullptr);
};

#endif // MD390LIMITS_HH
