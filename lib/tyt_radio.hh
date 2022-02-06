/** @defgroup tyt TYT/Retevis Radios
 * Abstract classes for TYT and Retevis radios.
 *
 * @ingroup dsc */
#ifndef TYT_RADIO_HH
#define TYT_RADIO_HH

#include "radio.hh"
#include "tyt_interface.hh"

/** Implements an USB interface to TYT & Retevis radios.
 *
 * @ingroup tyt */
class TyTRadio: public Radio
{
	Q_OBJECT

public:
  /** Do not construct this class directly, rather use @c Radio::detect. */
  explicit TyTRadio(TyTInterface *device=nullptr, QObject *parent=nullptr);

  virtual ~TyTRadio();

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

private:
  virtual bool download();
  virtual bool upload();
  virtual bool uploadCallsigns();

protected:
  /** The interface to the radio. */
  TyTInterface *_dev;
  /** Holds the flags to controll assembly and upload of code-plugs. */
  Codeplug::Flags _codeplugFlags;
  /** The generic configuration. */
	Config *_config;
  /** A weak reference to the user-database. */
  UserDatabase *_userDB;
};

#endif // UV390_HH
