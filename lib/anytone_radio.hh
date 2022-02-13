/** @defgroup anytone Anytone Radios
 * Base classes for all anytone radios.
 *
 * Anytone radios share a lot of common formats and interfaces.
 * Consequently there are base-classes for radio interfaces (@c AnytoneInterface), codeplugs
 * (@c AnytoneCodeplug) and radios (@c AnytoneRadio). This helps to keep the burden of implementing
 * the support for each radio at a minimum.
 *
 * @ingroup dsc */
#ifndef __ANYTONE_RADIO_HH__
#define __ANYTONE_RADIO_HH__

#include "radio.hh"
#include "anytone_interface.hh"
#include "anytone_codeplug.hh"

/** Implements an interface to Anytone radios.
 *
 * The transfer of the codeplug to the device is performed in 4 steps.
 *
 * First only the bitmaps of all lists are downloaded from the device. Then all elements that are
 * not touched or only updated by the common codeplug config are downloaded. Then, the common
 * config gets applied to the binary codeplug. That is, all channels, contacts, zones, group-lists
 * and scan-lists are generated and their bitmaps gets updated accordingly. Also the general config
 * gets updated from the common codeplug settings. Finally, the resulting binary codeplug gets
 * written back to the device.
 *
 * This rather complex method of writing a codeplug to the device is needed to maintain all
 * settings within the radio that are not defined within the common codeplug config while keeping
 * the amount of data being read from and written to the device small.
 *
 * @ingroup anytone */
class AnytoneRadio: public Radio
{
  Q_OBJECT

protected:
  /** Do not construct this class directly. */
  explicit AnytoneRadio(const QString &name, AnytoneInterface *device=nullptr, QObject *parent=nullptr);

public:
  /** Destructor. */
  virtual ~AnytoneRadio();

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
  AnytoneInterface *_dev;
  /** If @c true, the codeplug on the radio gets updated upon upload. If @c false, it gets
   * overridden. */
  Codeplug::Flags _codeplugFlags;
  /** The generic configuration. */
  Config *_config;
  /** A weak reference to the user-database. */
  UserDatabase *_userDB;
  /** The actual binary codeplug representation. */
  AnytoneCodeplug *_codeplug;
  /** The actual binary callsign database representation. */
  CallsignDB *_callsigns;
};

#endif // __D868UV_HH__
