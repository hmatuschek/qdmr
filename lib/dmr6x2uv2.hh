/** @defgroup dmr6x2uv2 BTECH DMR-6X2UV PRO
 * Device specific classes for BTECH DMR-6X2UV PRO.
 *
 * This is basically the same device as the DMR6X2 but with larger memory (larger calls-sign db),
 * FM APRS RX, Bluetooth and GPS roaming.
 *
 * \image html dmr6x2uv.jpg "DMR-6X2UV" width=200px
 * \image latex dmr6x2uv.jpg "DMR-6X2UV" width=200px
 *
 * @ingroup anytone */

#ifndef DMR6X2UV2_HH
#define DMR6X2UV2_HH

#include "anytone_radio.hh"
#include "anytone_interface.hh"

/** Represents a BTECH DMR-6X2UV PRO.
 * @ingroup dmr6x2uv2 */
class DMR6X2UV2: public AnytoneRadio
{
  Q_OBJECT

public:
  /** Do not construct this class directly, rather use @c Radio::detect. */
  explicit DMR6X2UV2(AnytoneInterface *device=nullptr, QObject *parent=nullptr);

  const RadioLimits &limits() const;

  /** Returns the default radio information. The actual instance may have different properties
   * due to variants of the same radio. */
  static RadioInfo defaultRadioInfo();

private:
  RadioLimits *_limits;
};


#endif // DMR6X2UV_HH
