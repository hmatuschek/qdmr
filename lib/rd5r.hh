/** @defgroup rd5r Baofeng/Radioddity RD-5R device specific classes.
 *
 * The Baofeng/Radioddity RD-5R radio is likely the cheapest fully DMR compatiple (Tier I&II) VHF/UHF
 * radio on the market. Consequently, it is quiet popular and is usually the first DMR radio
 * many operators may buy.
 *
 * It features up to 1024 channels organized in 250 zones, where each zone may contain up to
 * 16 channels. The radio is a dual VFO and each VFO might be assigned to  a different zone. The
 * radio can also hold up to 255 contacts, 64 RX group lists and 250 scanlists.
 *
 * @ingroup dsc */
#ifndef RD5R_HH
#define RD5R_HH

#include "radio.hh"
#include "hidinterface.hh"
#include "rd5r_codeplug.hh"


/** Implements an interface to the Baofeng/Radioddity RD-5R VHF/UHF 5W DMR (Tier I/II) radio.
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

public slots:
  /** Starts the download of the codeplug and derives the generic configuration from it. */
	bool startDownload(Config *config);
  /** Derives the device-specific codeplug from the generic configuration and uploads that
   * codeplug to the radio. */
	bool startUpload(Config *config);

protected:
  /** Main function running in a separate thread performing the up- and download to and from the
   * device. */
	void run();

protected slots:
  /** Internal callback on finished downloads. */
	void onDonwloadFinished();

protected:
  /** Device identifier string. */
	QString _name;
  /** HID interface to the radio. */
	HID *_dev;
  /** Current generic configuration. */
	Config *_config;
  /** Current device specific codeplug. */
	RD5RCodeplug _codeplug;
};

#endif // RD5R_HH
