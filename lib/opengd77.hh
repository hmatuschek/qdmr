/** @defgroup ogd77 Open GD-77 Firmware
 * Implements a radio running the Open GD77 firmware.
 * @ingroup dsc */

#ifndef OPENGD77_HH
#define OPENGD77_HH

#include "radio.hh"
#include "opengd77_interface.hh"
#include "opengd77_codeplug.hh"


/** Implements an USB interface to Open GD-77(S) VHF/UHF 5W DMR (Tier I&II) radios.
 *
 * @ingroup ogd77 */
class OpenGD77 : public Radio
{
	Q_OBJECT

public:
	/** Do not construct this class directly, rather use @c Radio::detect. */
  explicit OpenGD77(QObject *parent=nullptr);
  virtual ~OpenGD77();

	const QString &name() const;
  const Radio::Features &features() const;
  const CodePlug &codeplug() const;
  CodePlug &codeplug();

public slots:
  /** Starts the download of the codeplug and derives the generic configuration from it. */
	bool startDownload(Config *config, bool blocking=false);
  /** Derives the device-specific codeplug from the generic configuration and uploads that
   * codeplug to the radio. */
  bool startUpload(Config *config, bool blocking=false, bool update=true);

protected:
  /** Thread main routine, performs all blocking IO operations for codeplug up- and download. */
	void run();
  /** Implements the actual download process. */
  void download();
  /** Implements the actual upload process. */
  void upload();

protected:
  /** The device identifier. */
	QString _name;
  /** The interface to the radio. */
  OpenGD77Interface *_dev;
  /** The generic configuration. */
	Config *_config;
  /** The actual binary codeplug representation. */
  OpenGD77Codeplug _codeplug;
};

#endif // OPENGD77_HH
