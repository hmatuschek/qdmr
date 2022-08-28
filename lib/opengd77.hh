/** @defgroup ogd77 Open GD-77 Firmware
 * Implements a radio running the Open GD77 firmware.
 * @ingroup dsc */

#ifndef OPENGD77_HH
#define OPENGD77_HH

#include "radio.hh"
#include "opengd77_interface.hh"
#include "opengd77_codeplug.hh"
#include "opengd77_callsigndb.hh"


/** Implements an USB interface to Open GD-77(S) VHF/UHF 5W DMR (Tier I&II) radios.
 *
 * @ingroup ogd77 */
class OpenGD77 : public Radio
{
	Q_OBJECT

public:
	/** Do not construct this class directly, rather use @c Radio::detect. */
  explicit OpenGD77(OpenGD77Interface *device=nullptr, QObject *parent=nullptr);
  virtual ~OpenGD77();

	const QString &name() const;
  const RadioLimits &limits() const;
  const Codeplug &codeplug() const;
  Codeplug &codeplug();

  /** Returns the default radio information. The actual instance may have different properties
   * due to variants of the same radio. */
  static RadioInfo defaultRadioInfo();

public slots:
  /** Starts the download of the codeplug and derives the generic configuration from it. */
  bool startDownload(bool blocking=false, const ErrorStack &err=ErrorStack());
  /** Derives the device-specific codeplug from the generic configuration and uploads that
   * codeplug to the radio. */
  bool startUpload(Config *config, bool blocking=false,
                   const Codeplug::Flags &flags = Codeplug::Flags(), const ErrorStack &err=ErrorStack());
  /** Encodes the given user-database and uploades it to the device. */
  bool startUploadCallsignDB(UserDatabase *db, bool blocking=false,
                             const CallsignDB::Selection &selection=CallsignDB::Selection(), const ErrorStack &err=ErrorStack());

protected:
  /** Thread main routine, performs all blocking IO operations for codeplug up- and download. */
	void run();

  /** Implements the actual download process. */
  bool download();
  /** Implements the actual codeplug upload process. */
  bool upload();
  /** Implements the actual callsign DB upload process. */
  bool uploadCallsigns();

protected:
  /** The device identifier. */
	QString _name;
  /** The interface to the radio. */
  OpenGD77Interface *_dev;
  /** The generic configuration. */
	Config *_config;
  /** The actual binary codeplug representation. */
  OpenGD77Codeplug _codeplug;
  /** The actual binary callsign DB representation. */
  OpenGD77CallsignDB _callsigns;

private:
  /** Holds the singleton instance. */
  static RadioLimits *_limits;
};

#endif // OPENGD77_HH
