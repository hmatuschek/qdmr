#ifndef D868UVLIMITS_HH
#define D868UVLIMITS_HH

#include "radiolimits.hh"

class D868UVLimits: public RadioLimits
{
  Q_OBJECT

public:
  D868UVLimits(const std::initializer_list<std::pair<double, double>> &freqRanges, QObject *parent=nullptr);
};

#endif // D868UVLIMITS_HH
