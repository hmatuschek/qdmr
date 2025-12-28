#ifndef DM32UVLIMITS_HH
#define DM32UVLIMITS_HH

#include "radiolimits.hh"

/** Implements the configuration limits for the BTECH DR-1801UV.
 * @ingroup dm32uv */
class DM32UVLimits : public RadioLimits
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit DM32UVLimits(QObject *parent=nullptr);
};

#endif // DM32UV
