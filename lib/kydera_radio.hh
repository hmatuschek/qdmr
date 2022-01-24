/** @defgroup kydera Kydera Radios
 * Base classes for all Kydera based radios.
 *
 * @ingroup dsc */
#ifndef __KYDERA_RADIO_HH__
#define __KYDERA_RADIO_HH__

#include "radio.hh"
#include "kydera_codeplug.hh"

// Forward declaration
class KyderaInterface;

/** Implements an interface to Kydera radios.
 *
 * @ingroup kydera */
class KyderaRadio: public Radio
{
  Q_OBJECT

protected:
  /** Do not construct this class directly. */
  explicit KyderaRadio(const QString &name, KyderaInterface *device=nullptr, QObject *parent=nullptr);

public:
  /** Destructor. */
  virtual ~KyderaRadio();

  const QString &name() const;
  const Codeplug &codeplug() const;
  Codeplug &codeplug();

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

private:
  /** Downloads the codeplug from the radio. This method block until the download is complete. */
  virtual bool download();
  /** Uploads the encoded codeplug to the radio. This method block until the upload is complete. */
  virtual bool upload();
  /** Uploads the encoded callsign database to the radio.
   * This method block until the upload is complete. */
  virtual bool uploadCallsigns();

protected:
  /** The device identifier. */
  QString _name;
  /** The interface to the radio. */
  KyderaInterface *_dev;
  /** If @c true, the codeplug on the radio gets updated upon upload. If @c false, it gets
   * overridden. */
  Codeplug::Flags _codeplugFlags;
  /** The generic configuration. */
  Config *_config;
  /** A weak reference to the user-database. */
  UserDatabase *_userDB;
  /** The actual binary codeplug representation. */
  KyderaCodeplug *_codeplug;
  /** The actual binary callsign database representation. */
  CallsignDB *_callsigns;
};

#endif // __D868UV_HH__
