#ifndef D868UVLIMITS_HH
#define D868UVLIMITS_HH

#include "radiolimits.hh"

/** Implements the limits for the AnyTone AT-D878UV. */
class D868UVLimits: public RadioLimits
{
  Q_OBJECT

public:
  /** Constructor. */
  D868UVLimits(const std::initializer_list<std::pair<double, double>> &freqRanges, QObject *parent=nullptr);
};

#endif // D868UVLIMITS_HH
