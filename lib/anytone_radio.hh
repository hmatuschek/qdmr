/** @defgroup anytone Anytone Radios
 * Base class for all anytone radios.
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
  explicit AnytoneRadio(QObject *parent=nullptr);

public:
  const QString &name() const;
  const CodePlug &codeplug() const;
  CodePlug &codeplug();

protected:
  /** Thread main routine, performs all blocking IO operations for codeplug up- and download. */
  void run();

  /** Connects to the radio, if a radio interface is passed to the constructor, this interface
   * instance is used. */
  bool connect();
  /** Downloads the codeplug from the radio. This method block until the download is complete. */
  bool download();
  /** Uploads the encoded codeplug to the radio. This method block until the upload is complete. */
  bool upload();
  /** Uploads the encoded callsign database to the radio.
   * This method block until the upload is complete. */
  bool uploadCallsigns();

protected:
  /** The device identifier. */
  QString _name;
  /** The interface to the radio. */
  AnytoneInterface *_dev;
  /** If @c true, the codeplug on the radio gets updated upon upload. If @c false, it gets
   * overridden. */
  CodePlug::Flags _codeplugFlags;
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
