#ifndef RD5RLIMITS_HH
#define RD5RLIMITS_HH

#include "radiolimits.hh"

/** Implements the configuration limits for the Radioddity RD-5R.
 * @ingroup rd5r */
class RD5RLimits : public RadioLimits
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit RD5RLimits(QObject *parent=nullptr);
};

#endif // RD5RLIMITS_HH
