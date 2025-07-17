/** @defgroup ogd77 Open GD-77 Firmware
 * Implements a radio running the Open GD77 firmware.
 * @ingroup dsc */

#ifndef OPENGD77_HH
#define OPENGD77_HH

#include "opengd77base.hh"
#include "opengd77_interface.hh"
#include "opengd77_codeplug.hh"
#include "opengd77_callsigndb.hh"


/** Implements an USB interface to Open GD-77(S) VHF/UHF 5W DMR (Tier I&II) radios.
 *
 * @ingroup ogd77 */
class OpenGD77 : public OpenGD77Base
{
  Q_OBJECT

public:
	/** Do not construct this class directly, rather use @c Radio::detect. */
  explicit OpenGD77(OpenGD77Interface *device=nullptr, QObject *parent=nullptr);

  RadioInfo info() const;
  const QString &name() const;

  const Codeplug &codeplug() const;
  Codeplug &codeplug();

  const CallsignDB *callsignDB() const;
  CallsignDB *callsignDB();

  /** Returns the default radio information. The actual instance may have different properties
   * due to variants of the same radio. */
  static RadioInfo defaultRadioInfo();

protected:
  /** The device identifier. */
  QString _name;
  /** The actual binary codeplug representation. */
  OpenGD77Codeplug _codeplug;
  /** The actual binary callsign DB representation. */
  OpenGD77CallsignDB _callsigns;
};

#endif // OPENGD77_HH
