/** @defgroup uv390 TYT MD-UV390, Retevis RT3S
 * Device specific classes for TYT MD-UV390 and Retevis RT3S.
 *
 * \image html uv390.jpg "MD-UV390" width=200px
 * \image latex uv390.jpg "MD-UV390" width=200px
 *
 * <img src="uv390.jpg" width="200px" align="left"/>
 * The TYT MD-UV390 and the identical Retevis RT-3S are decent VHF/UHF FM and DMR handheld radios.
 * Both radios are available with and without an GPS option. @c libdmrconf will support that
 * feature. Non-GPS variants of that radio will simply ignore any GPS system settings.
 *
 * These radios support up to 3000 channels organized in 250 zones. Each zone may hold up to 64
 * channels for each VFO (64 for VFO A and 64 for VFO B). There are also up to 250 scanlists
 * holding up to 31(?) channels each.
 *
 * The radio can hold up to 3000 contacts (DMR contacts) and 250 RX group lists as well as up to 50
 * pre-programmed messages. Depending on the firmware programmed on the radio, it may also hold a
 * callsign database of up to 100000 entries. This can be used to resolve amlost any DMR ID assigned
 * (at the time of this writing, there are about 140k IDs assigned) to name and callsign.
 *
 * @ingroup dsc */
#ifndef UV390_HH
#define UV390_HH

#include "radio.hh"
#include "dfu_libusb.hh"
#include "uv390_codeplug.hh"
#include "uv390_callsigndb.hh"

/** Implements an USB interface to the TYT MD-UV390 & Retevis RT3S VHF/UHF 5W DMR (Tier I&II) radios.
 *
 * The TYT MD-UV390 and Retevis RT3S radios use a DFU-style communication protocol to read and write
 * codeplugs onto the radio (see @c DFUDevice). This class implements the communication details
 * using DFU protocol.
 *
 * @ingroup uv390 */
class UV390: public Radio
{
	Q_OBJECT

public:
  /** Do not construct this class directly, rather use @c Radio::detect. */
  explicit UV390(QObject *parent=nullptr);

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

private:
  void download();
  void upload();
  void uploadCallsigns();

protected:
  /** The device identifier. */
	QString _name;
  /** The interface to the radio. */
	DFUDevice *_dev;
  /** Holds the flags to controll assembly and upload of code-plugs. */
  CodePlug::Flags _codeplugFlags;
  /** The generic configuration. */
	Config *_config;
  /** A weak reference to the user-database. */
  UserDatabase *_userDB;
  /** The actual binary codeplug representation. */
	UV390Codeplug _codeplug;
  /** The acutal binary callsign-db representation. */
  UV390CallsignDB _callsigns;
};

#endif // UV390_HH
