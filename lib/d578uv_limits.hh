#ifndef D578UVLIMITS_HH
#define D578UVLIMITS_HH

#include "radiolimits.hh"

/** Implements the limits for the AnyTone AT-D878UV. */
class D578UVLimits: public RadioLimits
{
  Q_OBJECT

public:
  /** Constructor. */
  D578UVLimits(const std::initializer_list<std::pair<double, double>> &freqRanges, QObject *parent=nullptr);
};

#endif // D578UVLIMITS_HH
