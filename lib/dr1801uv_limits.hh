#ifndef DR1801UVLIMITS_HH
#define DR1801UVLIMITS_HH

#include "radiolimits.hh"

/** Implements the configuration limits for the BTECH DR-1801UV.
 * @ingroup dr1801uv */
class DR1801UVLimits : public RadioLimits
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit DR1801UVLimits(QObject *parent=nullptr);
};

#endif // DR1801UV
