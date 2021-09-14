/** @defgroup gd77 Radioddity GD-77
 * Device specific classes for Radioddity GD-77 and GD-77S.
 *
 * \image html gd77.jpg "GD-77" width=200px
 * \image latex gd77.jpg "GD-77" width=200px
 *
 * @warning This device and its codplug are implemented based on the code by Serge and has not been
 * tested! I do not own a GD-77 or GD-77S, hence I cannot test and verify the implementation!
 *
 * @ingroup radioddity */

#ifndef GD77_HH
#define GD77_HH

#include "radio.hh"
#include "radioddity_interface.hh"
#include "gd77_codeplug.hh"
#include "gd77_callsigndb.hh"


/** Implements an USB interface to the Radioddity GD-77(S) VHF/UHF 5W DMR (Tier I&II) radios.
 *
 * @ingroup gd77 */
class GD77 : public Radio
{
	Q_OBJECT

public:
	/** Do not construct this class directly, rather use @c Radio::detect. */
  explicit GD77(RadioddityInterface *device=nullptr, QObject *parent=nullptr);

  virtual ~GD77();

	const QString &name() const;
  const Radio::Features &features() const;
  const CodePlug &codeplug() const;
  CodePlug &codeplug();

public slots:
  /** Starts the download of the codeplug and derives the generic configuration from it. */
  bool startDownload(bool blocking=false);
  /** Derives the device-specific codeplug from the generic configuration and uploads that
   * codeplug to the radio. */
  bool startUpload(Config *config, bool blocking=false,
                   const CodePlug::Flags &flags = CodePlug::Flags());
  /** Encodes the given user-database and uploades it to the device. */
  bool startUploadCallsignDB(UserDatabase *db, bool blocking=false,
                             const CallsignDB::Selection &selection=CallsignDB::Selection());

protected:
  /** Thread main routine, performs all blocking IO operations for codeplug up- and download. */
	void run();
  /** Connects to the device. */
  bool connect();
  /** Reads the codeplug from the device. */
  bool download();
  /** Writes the codeplug to the device. */
  bool upload();
  /** Implements the actual callsign DB upload process. */
  bool uploadCallsigns();

protected:
  /** The device identifier. */
	QString _name;
  /** The interface to the radio. */
	RadioddityInterface *_dev;
  /** The generic configuration. */
	Config *_config;
  /** The actual binary codeplug representation. */
	GD77Codeplug _codeplug;
  /** The acutal binary callsign DB representation. */
  GD77CallsignDB _callsigns;

};

#endif // GD77_HH
