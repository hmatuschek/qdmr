/** @defgroup d578uv Anytone AT-D578UV
 * Device specific classes for Anytone AT-D578UV.
 *
 * \image html d578uv.jpg "AT-D578UV" width=200px
 * \image latex d578uv.jpg "AT-D578UV" width=200px
 *
 * @ingroup anytone */
#ifndef __D578UV_HH__
#define __D578UV_HH__

#include "anytone_radio.hh"
#include "anytone_interface.hh"
#include "d878uv2_callsigndb.hh"

/** Implements an interface to Anytone AT-D578UV VHF/UHF 50W DMR (Tier I & II) radios.
 *
 * @ingroup d578uv */
class D578UV: public AnytoneRadio
{
	Q_OBJECT

public:
  /** Do not construct this class directly, rather use @c Radio::detect. */
  explicit D578UV(AnytoneInterface *device=nullptr, QObject *parent=nullptr);

  const RadioLimits &limits() const;

  /** Returns the default radio information. The actual instance may have different properties
   * due to variants of the same radio. */
  static RadioInfo defaultRadioInfo();

private:
  /** Holds the limits for the radio. */
  RadioLimits *_limits;
};

#endif // __D878UV_HH__
