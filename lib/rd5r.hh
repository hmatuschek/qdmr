/** @defgroup rd5r Baofeng/Radioddity RD-5R
 * Device specific classes for Baofeng/Radioddity RD-5R.
 *
 * \image html rd5r.jpg "RD-5R" width=200px
 * \image latex rd5r.jpg "RD-5R" width=200px
 *
 * The Baofeng/Radioddity RD-5R radio is likely the cheapest fully DMR compatiple (Tier I&II) VHF/UHF
 * radio on the market. Consequently, it is quiet popular and is usually the first DMR radio
 * many operators may buy. In my opinion it is a decent radio with reasonable sensitivity and audio
 * quality (for a handheld). However, the receiver frontend seems to be identical to the analog
 * Baofeng handhelds and thus suffers from the same well known issue of blocking whenever a strong
 * signal is nearby (even across bands). But you get a lot for a $70 radio.
 *
 * It features up to 1024 channels organized in 250 zones, where each zone may contain up to
 * 16 channels. The radio is a dual VFO and each VFO might be assigned to a different zone. Hence, a
 * zone is just a single list of up to 16 channels (in contrast to many other radios where a zone
 * contains two lists of channels for each VFO).
 *
 * The radio can also hold up to 255 contacts (actually 256, but due to a bug in the firmware RX is
 * disabled whenever all 256 contacts are set), 64 RX group lists and 250 scanlists.
 *
 * @ingroup dsc */
#ifndef RD5R_HH
#define RD5R_HH

#include "radio.hh"
#include "hid_interface.hh"
#include "rd5r_codeplug.hh"


/** Implements an interface to the Baofeng/Radioddity RD-5R VHF/UHF 5W DMR (Tier I/II) radio.
 *
 * The Baofeng/Radioddity RD-5R radio uses a weird HID (human-interface device, see @c HID and
 * @c HIDevice) protocol for communication. This class implements the communication details with
 * the radio to read and write codeplugs on the device.
 *
 * @ingroup rd5r */
class RD5R: public Radio
{
	Q_OBJECT

public:
  /** Constructor.
   * Do not call this constructor directly. Consider using the factory method
   * @c Radio::detect. */
	RD5R(QObject *parent=nullptr );

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
  /** Main function running in a separate thread performing the up- and download to and from the
   * device. */
	void run();

private:
  /** Device identifier string. */
	QString _name;
  /** HID interface to the radio. */
	HID *_dev;
  CodePlug::Flags _codeplugFlags;
  /** Current generic configuration. */
	Config *_config;
  /** Current device specific codeplug. */
	RD5RCodeplug _codeplug;
};

#endif // RD5R_HH
