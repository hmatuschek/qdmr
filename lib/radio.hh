/** @defgroup dsc Supported devices
 * This module collects all classes are device specific.
 *
 * That is, implementing device specific configurations, aka codeplugs as well as the specific
 * communication whith these radios.
 */

#ifndef RADIO_HH
#define RADIO_HH

#include <QThread>
#include "radioinfo.hh"
#include "radiointerface.hh"
#include "codeplug.hh"
#include "callsigndb.hh"
#include "errorstack.hh"

class Config;
class UserDatabase;
class RadioLimits;


/** Base class for all Radio objects.
 *
 * The radio objects represents a connected radio. This class controlles the communication
 * with the device as well as the conversion betwenn device specific code-plugs and generic
 * configurations.
 *
 * @ingroup rif
 */
class Radio : public QThread
{
	Q_OBJECT

public:
  /** Possible states of the radio object. */
	typedef enum {
    StatusIdle,            ///< Idle, nothing to do.
    StatusDownload,        ///< Downloading codeplug.
    StatusUpload,          ///< Uploading codeplug.
    StatusUploadCallsigns, ///< Uploading codeplug.
    StatusError            ///< An error occured.
  } Status;

public:
  /** Default constructor. */
	explicit Radio(QObject *parent = nullptr);

  virtual ~Radio();

  /** Returns the name of the radio (e.g., device identifier). */
	virtual const QString &name() const = 0;

  /** Returns the limits for this radio.
   *
   * Call @c RadioLimits::verifyConfig to verify a codeplug with respect to a radio.
   *
   * @since Version 0.10.2 */
  virtual const RadioLimits &limits() const = 0;

  /** Returns the codeplug instance. */
  virtual const Codeplug &codeplug() const = 0;
  /** Returns the codeplug instance. */
  virtual Codeplug &codeplug() = 0;

  /** Returns the call-sign DB instance. */
  virtual const CallsignDB *callsignDB() const;
  /** Returns the call-sign DB instance. */
  virtual CallsignDB *callsignDB();

  /** Returns the current status. */
  Status status() const;

  /** Returns the error stack, passed to @c startDownload, @c startUpload or
   * @c startUploadCallsignDB. It contains the error messages from the upload/download process. */
  const ErrorStack &errorStack() const;

public:
  /** Tries to detect the radio connected to the specified interface or constructs the specified
   * radio using the @c RadioInfo passed by @c force. */
  static Radio *detect(const USBDeviceDescriptor &descr, const RadioInfo &force=RadioInfo(),
                       const ErrorStack &err=ErrorStack());

public slots:
  /** Starts the download of the codeplug.
   * Once the download finished, the codeplug can be accessed and decoded using
   * the @c codeplug() method. */
  virtual bool startDownload(bool blocking=false, const ErrorStack &err=ErrorStack()) = 0;
  /** Derives the device-specific codeplug from the generic configuration and uploads that
   * codeplug to the radio. */
  virtual bool startUpload(
      Config *config, bool blocking=false,
      const Codeplug::Flags &flags = Codeplug::Flags(), const ErrorStack &err=ErrorStack()) = 0;
  /** Assembles the callsign DB from the given one and uploads it to the device. */
  virtual bool startUploadCallsignDB(
      UserDatabase *db, bool blocking=false,
      const CallsignDB::Selection &selection=CallsignDB::Selection(),
      const ErrorStack &err=ErrorStack()) = 0;

signals:
  /** Gets emitted once the codeplug download has been started. */
	void downloadStarted();
  /** Gets emitted on download progress (e.g., for progress bars). */
	void downloadProgress(int percent);
  /** Gets emitted once the codeplug download has been finished. */
  void downloadFinished(Radio *radio, Codeplug *codeplug);
  /** Gets emitted if there was an error during the codeplug download. */
	void downloadError(Radio *radio);

  /** Gets emitted once the codeplug upload has been started. */
	void uploadStarted();
  /** Gets emitted on upload progress (e.g., for progress bars). */
	void uploadProgress(int percent);
  /** Gets emitted if there was an error during the upload. */
	void uploadError(Radio *radio);
  /** Gets emitted once the codeplug upload has been completed successfully. */
	void uploadComplete(Radio *radio);

protected:
  /** The current state/task. */
  Status _task;
  /** The error stack. */
  ErrorStack _errorStack;
};

#endif // RADIO_HH
