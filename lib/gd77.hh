/** @defgroup gd77 Radioddity GD-77
 * Device specific classes for Radioddity GD-77 and GD-77S.
 *
 * \image html gd77.jpg "GD-77" width=200px
 * \image latex gd77.jpg "GD-77" width=200px
 *
 * @warning This device and its codplug are implemented based on the code by Serge and has not been
 * tested! I do not own a GD-77 or GD-77S, hence I cannot test and verify the implementation!
 *
 * @todo Get a handheld.
 * @todo Reverse-engineer and implement everything.
 * @ingroup dsc */

#ifndef GD77_HH
#define GD77_HH

#include "radio.hh"
#include "hid_interface.hh"
#include "gd77_codeplug.hh"


/** Implements an USB interface to the Radioddity GD-77(S) VHF/UHF 5W DMR (Tier I&II) radios.
 *
 * @ingroup gd77 */
class GD77 : public Radio
{
	Q_OBJECT

public:
	/** Do not construct this class directly, rather use @c Radio::detect. */
	explicit GD77(QObject *parent=nullptr);

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

protected:
  /** The device identifier. */
	QString _name;
  /** The interface to the radio. */
	HID *_dev;
  /** The generic configuration. */
	Config *_config;
  /** The actual binary codeplug representation. */
	GD77Codeplug _codeplug;
};

#endif // GD77_HH
