/** @defgroup d168uv Anytone AT-D168UV
 * Device specific classes for Anytone AT-D168UVII.
 *
 * @ingroup anytone */
#ifndef __D168UV2_HH__
#define __D168UV2_HH__

#include "anytone_radio.hh"
#include "anytone_interface.hh"
//#include "d878uv2_callsigndb.hh"

/** Implements an interface to Anytone AT-D168UVII VHF/UHF 5W DMR (Tier I & II) radios.
 *
 * @ingroup d128uv */
class D168UV: public AnytoneRadio
{
	Q_OBJECT

public:
  /** Do not construct this class directly, rather use @c Radio::detect. */
  explicit D168UV(AnytoneInterface *device=nullptr, QObject *parent=nullptr);

  const RadioLimits &limits() const;

  /** Returns the default radio information. The actual instance may have different properties
   * due to variants of the same radio. */
  static RadioInfo defaultRadioInfo();

private:
  /** The limits for the radio. */
  RadioLimits *_limits;
};

#endif // __D878UV2_HH__
