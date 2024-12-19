/** @defgroup ogd77 Open GD-77 Firmware
 * Implements a radio running the Open MD-UV380 firmware.
 * @ingroup dsc */

#ifndef OPENUV380_HH
#define OPENUV380_HH

#include "opengd77base.hh"
#include "openuv380_codeplug.hh"
#include "opengd77_callsigndb.hh"


/** Implements an USB interface to Open UV380 VHF/UHF 5W DMR (Tier I&II) radios.
 *
 * @ingroup ogd77 */
class OpenUV380 : public OpenGD77Base
{
	Q_OBJECT

public:
	/** Do not construct this class directly, rather use @c Radio::detect. */
  explicit OpenUV380(OpenGD77Interface *device=nullptr, QObject *parent=nullptr);

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
  OpenUV380Codeplug _codeplug;
  /** The actual binary callsign DB representation. */
  OpenGD77CallsignDB _callsigns;
};

#endif // OPENGD77_HH
