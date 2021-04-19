/** @defgroup dm1701 Baofeng DM-1701, Retevis RT84
 * Device specific classes for Baofeng DM-1701 and Retevis RT84.
 *
 * \image html dm1701.png "DM-1701" width=200px
 * \image latex dm1701.png "DM-1701" width=200px
 *
 * The Baofeng DM-1701 and the identical Retevis RT84.
 *
 * Features:
 *   - VHF/UHF, 136-174 MHz and 400-480 MHz
 *   - Pout = 1, 3 and 5W
 *   - 3000 channels
 *   - 10000 contacts
 *   - 250 RX group lists with ?? contacts each
 *   - 250 zones, 64 channels each (A & B)
 *   - 250 scan lists, 31 channels each
 *   -
 * @ingroup dsc */
#ifndef DM1701_HH
#define DM1701_HH

#include "radio.hh"
#include "dfu_libusb.hh"
#include "uv390_codeplug.hh"
#include "uv390_callsigndb.hh"

/** Implements an USB interface to the  Baofeng DM-1701 and Retevis RT84 VHF/UHF 5W DMR (Tier I&II) radios.
 *
 * The  Baofeng DM-1701 and Retevis RT84 radios use a DFU-style communication protocol to read and write
 * codeplugs onto the radio (see @c DFUDevice). This class implements the communication details
 * using DFU protocol.
 *
 * @ingroup dm1701 */
class DM1701: public Radio
{
	Q_OBJECT

public:
  /** Do not construct this class directly, rather use @c Radio::detect. */
  explicit DM1701(QObject *parent=nullptr);

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

#endif // DM1701_HH
