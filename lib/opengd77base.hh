/** @defgroup ogd77 Open GD-77 Firmware
 * Implements a radio running the Open GD77 firmware.
 * @ingroup dsc */

#ifndef OPENGD77BASE_HH
#define OPENGD77BASE_HH

#include <QPointer>

#include "radio.hh"
#include "opengd77_interface.hh"
#include "opengd77_satelliteconfig.hh"
#include "satellitedatabase.hh"


/** Implements an common USB interface to Open GD-77(S) type devices.
 *
 * @ingroup ogd77 */
class OpenGD77Base : public Radio
{
	Q_OBJECT

public:
	/** Do not construct this class directly, rather use @c Radio::detect. */
  explicit OpenGD77Base(OpenGD77Interface *device=nullptr, QObject *parent=nullptr);
  virtual ~OpenGD77Base();

  const RadioLimits &limits() const;

  /** Returns the default radio information. The actual instance may have different properties
   * due to variants of the same radio. */
  static RadioInfo defaultRadioInfo();

public slots:
  /** Starts the download of the codeplug and derives the generic configuration from it. */
  bool startDownload(const TransferFlags &flags, const ErrorStack &err=ErrorStack());
  /** Derives the device-specific codeplug from the generic configuration and uploads that
   * codeplug to the radio. */
  bool startUpload(Config *config, const Codeplug::Flags &flags = Codeplug::Flags(), const ErrorStack &err=ErrorStack());
  /** Encodes the given user-database and uploades it to the device. */
  bool startUploadCallsignDB(UserDatabase *db, const CallsignDB::Flags &selection=CallsignDB::Flags(), const ErrorStack &err=ErrorStack());

  bool startUploadSatelliteConfig(SatelliteDatabase *db, const TransferFlags &flags, const ErrorStack &err);

protected:
  /** Thread main routine, performs all blocking IO operations for codeplug up- and download. */
	void run();

  /** Implements the actual download process. */
  bool download();
  /** Implements the actual codeplug upload process. */
  bool upload();
  /** Implements the actual callsign DB upload process. */
  bool uploadCallsigns();
  /** Implements the actual satellite config upload process. */
  bool uploadSatellites();

protected:
  /** The interface to the radio. */
  OpenGD77Interface *_dev;
  /** Transfer flags. */
  TransferFlags _flags;
  /** The generic configuration. */
	Config *_config;
  /** The satellite configuraiton. */
  QPointer<SatelliteDatabase> _satelliteDatabase;
  OpenGD77BaseSatelliteConfig *_satelliteConfig;

private:
  /** Holds the singleton instance. */
  static RadioLimits *_limits;
};

#endif // OPENGD77BASE_HH
