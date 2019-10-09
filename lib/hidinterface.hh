#ifndef HIDINTERFACE_HH
#define HIDINTERFACE_HH

#include <QtGlobal>
#include <QObject>

//#ifdef Q_OS_MACOS
//#include "hid_macos.hh"
//#endif
#include "hid_libusb.hh"


/** Implements a radio interface for radios using the HID USB schema.
 *
 * @todo Should implement @c RadioInterface.
 *
 * @ingroup rif */
class HID: public HIDevice
{
	Q_OBJECT

public:
  /** Connects to the radio with given vendor and product ID. */
	explicit HID(int vid, int pid, QObject *parent = nullptr);
  /** Destructor. */
	virtual ~HID();

  /** Returns radio identifier string. */
	QString identify();

  /** Reads a block of data from the device at the given block number.
   * @param bno The block number.
   * @param data Pointer to memory where the read data is stored.
   * @param nbytes The number of bytes to read.
   * @returns @c true on success. */
	bool readBlock(int bno, unsigned char *data, int nbytes);
  /** Signal read operation complete. */
	bool readFinish();

  /** Writes a block of data to the device at the given block number.
   * @param bno The block number.
   * @param data Pointer to memory where the read data is stored.
   * @param nbytes The number of bytes to read.
   * @returns @c true on success. */
	bool writeBlock(int bno, unsigned char *data, int nbytes);
  /** Signal write operation complete. */
	bool writeFinish();

  /** Retruns the last error message. */
  inline const QString &errorMessage() const { return _errorMessage; }

protected:
  /** Holds the last error message. */
  QString _errorMessage;
};

#endif // HIDINTERFACE_HH
