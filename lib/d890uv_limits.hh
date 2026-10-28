#ifndef D890UVLIMITS_HH
#define D890UVLIMITS_HH

#include "anytone_limits.hh"

/** Implements the limits for the AnyTone AT-D890UV.
 *
 * Describes exactly what @c D890UVCodeplug encodes. Everything else is reported as not
 * supported (yet).
 *
 * @ingroup d890uv */
class D890UVLimits: public AnytoneLimits
{
  Q_OBJECT

public:
  /** Constructor.
   * @param rxFreqRanges Specifies the receive frequency ranges.
   * @param txFreqRanges Specifies the transmit frequency ranges.
   * @param hardwareRevision Specifies the hardware revision as reported by the radio. */
  D890UVLimits(const std::initializer_list<std::pair<Frequency, Frequency> > &rxFreqRanges,
               const std::initializer_list<std::pair<Frequency, Frequency> > &txFreqRanges,
               const QString &hardwareRevision, QObject *parent=nullptr);

  bool verifyConfig(const Config *config, RadioLimitContext &context) const override;
};

#endif // D890UVLIMITS_HH
