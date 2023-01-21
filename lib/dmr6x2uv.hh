/** @defgroup dmr6x2uv BTECH DMR-6X2UV
 * Device specific classes for BTECH DMR-6X2UV.
 *
 * Athough labeled BTECH (Baofeng USA), this device is basically a relabled AnyTone AT-D868UV.
 * However, there are some minor differences in the codeplug format, hence it needs a separate
 * implementation.
 *
 * \image html dmr6x2uv.jpg "DMR-6X2UV" width=200px
 * \image latex dmr6x2uv.jpg "DMR-6X2UV" width=200px
 *
 * @ingroup anytone */

#ifndef DMR6X2UV_HH
#define DMR6X2UV_HH

#include "anytone_radio.hh"
#include "anytone_interface.hh"

/** Represents a BTECH DMR-6X2UV.
 * @ingroup dmr6x2uv */
class DMR6X2UV: public AnytoneRadio
{
  Q_OBJECT

public:
  /** Do not construct this class directly, rather use @c Radio::detect. */
  explicit DMR6X2UV(AnytoneInterface *device=nullptr, QObject *parent=nullptr);

  const RadioLimits &limits() const;

  /** Returns the default radio information. The actual instance may have different properties
   * due to variants of the same radio. */
  static RadioInfo defaultRadioInfo();

private:
  RadioLimits *_limits;
};


#endif // DMR6X2UV_HH
