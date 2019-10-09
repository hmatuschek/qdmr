/** @defgroup uv390 TYT UV-390, Retevis RT3S device specific classes.
 * @ingroup dsc */
#ifndef UV390_HH
#define UV390_HH

#include "radio.hh"
#include "dfu_libusb.hh"
#include "uv390_codeplug.hh"


/** Implements an interface to the TYT UV-390 & Retevis RT3S VHF/UHF 5W DMR (Tier I&II) radios.
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

public slots:
  /** Starts the codeplug download. On success the codeplug gets decoded and the given generic
   * configuration gets updated from its content. */
	bool startDownload(Config *config);
  /** Uloads a codeplug to the device.
   * Updates the codeplug from the given generic configuration and starts the upload of that
   * codeplug to the device. */
	bool startUpload(Config *config);

protected:
  /** Thread main routine, performs all blocking IO operations for codeplug up- and download. */
	void run();

protected slots:
  /** Internal used callback. */
	void onDonwloadFinished();

protected:
  /** The device identifier. */
	QString _name;
  /** The interface to the radio. */
	DFUDevice *_dev;
  /** The generic configuration. */
	Config *_config;
  /** The actual binary codeplug representation. */
	UV390Codeplug _codeplug;
};

#endif // UV390_HH
