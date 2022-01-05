/** @defgroup cdr300uv Kydera CDR-300UV, Retevis RT73
 *
 * \image html rt73.jpg "RT73" width=200px
 * \image latex rt73.jpg "RT73" width=200px
 *
 * Device specific implementation of the Kydera CDR-300UV or Retevis RT73 devices.
 *
 * @ingroup kydera */

#ifndef CDR300UV_HH
#define CDR300UV_HH

#include "kydera_radio.hh"

/** Implements an interface to Kydera CDR-300UV or Retevis RT73 devices.
 *
 * @ingroup cdr300uv */
class CDR300UV : public KyderaRadio
{
  Q_OBJECT

public:
  /** Constructor.
   * The constructor takes the ownership of the interface @c device. */
  CDR300UV(KyderaInterface *device=nullptr, QObject *parent=nullptr);


public:
  /** Returns the default radio info for the Kydera CDR-300UV or Retevis RT73. */
  static RadioInfo defaultRadioInfo();

  const Radio::Features &features() const;
};

#endif // CDR300UV_HH
