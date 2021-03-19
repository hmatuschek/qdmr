/** @defgroup d878uv Anytone AT-D878UV
 * Device specific classes for Anytone AT-D878UV.
 *
 * \image html d878uv.jpg "AT-D878UV" width=200px
 * \image latex d878uv.jpg "AT-D878UV" width=200px
 *
 * @ingroup dsc */
#ifndef __D878UV_HH__
#define __D878UV_HH__

#include "radio.hh"
#include "anytone_interface.hh"
#include "d878uv_codeplug.hh"
#include "d878uv_callsigndb.hh"


/** Implements an interface to Anytone AT-D878UV VHF/UHF 7W DMR (Tier I & II) radios.
 *
 * The reverse-engineering of the D878UVCodeplug was quiet hard as it is huge and the radio
 * provides a lot of bells and whistles. Moreover, the binary code-plug file created by the
 * windows CPS does not directly relate to the data being written to the device. These two issues
 * (a lot of features and a huge code-plug) require that the transfer of the code-plug to the
 * device is performed in 4 steps.
 *
 * First only the bitmaps of all lists are downloaded from the device. Then all elements that are
 * not touched or only updated by the common code-plug config are downloaded. Then, the common
 * config gets applied to the binary code-plug. That is, all channels, contacts, zones, group-lists
 * and scan-lists are generated and their bitmaps gets updated accordingly. Also the general config
 * gets updated from the common code-plug settings. Finally, the resulting binar code-plug gets
 * written back to the device.
 *
 * This rather complex method of writing a code-plug to the device is needed to maintain all
 * settings within the radio that are not defined within the common code-plug config while keeping
 * the amount of data being read from and written to the device small.
 *
 * @ingroup d878uv */
class D878UV: public Radio
{
	Q_OBJECT

public:
  /** Do not construct this class directly, rather use @c Radio::detect. */
  explicit D878UV(QObject *parent=nullptr);

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
  bool startUploadCallsignDB(UserDatabase *db, bool blocking=false);

protected:
  /** Thread main routine, performs all blocking IO operations for codeplug up- and download. */
	void run();

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
  D878UVCodeplug _codeplug;
  D878UVCallsignDB _callsigns;
};

#endif // __D878UV_HH__
