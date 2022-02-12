#ifndef D878UV2LIMITS_HH
#define D878UV2LIMITS_HH

#include "radiolimits.hh"

/** Implements the limits for the AnyTone AT-D878UV2. */
class D878UV2Limits: public RadioLimits
{
  Q_OBJECT

public:
  /** Constructor. */
  D878UV2Limits(const std::initializer_list<std::pair<double, double>> &freqRanges, QObject *parent=nullptr);
};

#endif // D878UV2LIMITS_HH
