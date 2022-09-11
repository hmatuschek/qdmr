/** @defgroup ortx Open RTX Firmware
 * Implements a radio running the Open RTX firmware.
 * @ingroup dsc */

#ifndef OPENGRTX_HH
#define OPENGRTX_HH

#include "radio.hh"
//#include "openrtx_interface.hh"
#include "openrtx_codeplug.hh"

class OpenRTXInterface;

/** Implements an USB interface to radios running the Open RTX firmware.
 *
 * @ingroup ortx */
class OpenRTX: public Radio
{
	Q_OBJECT

public:
	/** Do not construct this class directly, rather use @c Radio::detect. */
  explicit OpenRTX(OpenRTXInterface *device=nullptr, QObject *parent=nullptr);
  virtual ~OpenRTX();

	const QString &name() const;
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
                   const Codeplug::Flags &flags = Codeplug::Flags(),
                   const ErrorStack &err=ErrorStack());
  /** Encodes the given user-database and uploades it to the device. */
  bool startUploadCallsignDB(UserDatabase *db, bool blocking=false,
                             const CallsignDB::Selection &selection=CallsignDB::Selection(),
                             const ErrorStack &err=ErrorStack());

protected:
  /** Thread main routine, performs all blocking IO operations for codeplug up- and download. */
	void run();

  /** Connects to the radio, if a radio interface is passed to the constructor, this interface
   * instance is used. */
  bool connect(const ErrorStack &err=ErrorStack());
  /** Implements the actual download process. */
  bool download(const ErrorStack &err=ErrorStack());
  /** Implements the actual codeplug upload process. */
  bool upload(const ErrorStack &err=ErrorStack());

protected:
  /** The device identifier. */
	QString _name;
  /** The interface to the radio. */
  OpenRTXInterface *_dev;
  /** The generic configuration. */
	Config *_config;
  /** The actual binary codeplug representation. */
  OpenRTXCodeplug _codeplug;
};

#endif // OPENGD77_HH
