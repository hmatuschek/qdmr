#ifndef D878UVLIMITS_HH
#define D878UVLIMITS_HH

#include "radiolimits.hh"

/** Implements the limits for the AnyTone AT-D878UV. */
class D878UVLimits: public RadioLimits
{
public:
  /** Constructor. */
  D878UVLimits(const std::initializer_list<std::pair<double, double>> &freqRanges, QObject *parent=nullptr);
};

#endif // D878UVLIMITS_HH
