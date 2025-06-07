/** @defgroup gd73 Radioddity GD-73
 * Device specific classes for Radioddity GD-73A and GD-73E.
 *
 * \image html gd73.webp "GD-73" width=200px
 * \image latex gd73.webp "GD-73" width=200px
 *
 * @ingroup radioddity */

#ifndef GD73_HH
#define GD73_HH

#include "radio.hh"
#include "gd73_interface.hh"
#include "gd73_codeplug.hh"



/** Implements an USB interface to the Radioddity GD-73 UHF 2W DMR (Tier I&II) radio.
 *
 * @ingroup gd73 */
class GD73 : public Radio
{
  Q_OBJECT

public:
  /** Do not construct this class directly, rather use @c Radio::detect. */
  explicit GD73(GD73Interface *device=nullptr, QObject *parent=nullptr);

  const QString &name() const;
  const RadioLimits &limits() const;
  const Codeplug &codeplug() const;
  Codeplug &codeplug();

  /** Returns the default radio information. The actual instance may have different properties
   * due to variants of the same radio. */
  static RadioInfo defaultRadioInfo();

public slots:
  /** Starts the download of the codeplug and derives the generic configuration from it. */
  bool startDownload(const TransferFlags &flags, const ErrorStack &err=ErrorStack());

  /** Derives the device-specific codeplug from the generic configuration and uploads that
   * codeplug to the radio. */
  bool startUpload(Config *config, const Codeplug::Flags &flags = Codeplug::Flags(),
                   const ErrorStack &err=ErrorStack());

  /** Encodes the given user-database and uploades it to the device. */
  bool startUploadCallsignDB(UserDatabase *db, const CallsignDB::Flags &selection=CallsignDB::Flags(),
                             const ErrorStack &err=ErrorStack());

  bool startUploadSatelliteConfig(SatelliteDatabase *db, const TransferFlags &flags, const ErrorStack &err);

protected:
  /** Thread main routine, performs all blocking IO operations for codeplug up- and download. */
  void run();

private:
  virtual bool download();
  virtual bool upload();
  virtual bool uploadCallsigns();

protected:
  /** The device identifier. */
  QString _name;

  /** The interface to the radio. */
  GD73Interface *_dev;
  /** Holds the flags to control assembly and upload of code-plugs. */
  Codeplug::Flags _codeplugFlags;
  /** The generic configuration. */
  Config *_config;

  /** The codeplug. */
  GD73Codeplug _codeplug;

private:
  /** Holds the signleton instance of the radio limits for this radio. */
  static RadioLimits *_limits;
};

#endif // GD77_HH
