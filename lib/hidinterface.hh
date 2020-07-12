#ifndef HIDINTERFACE_HH
#define HIDINTERFACE_HH

#include <QtGlobal>
#include <QObject>
#include "radiointerface.hh"

#ifdef Q_OS_MACOS
#include "hid_macos.hh"
#else
#include "hid_libusb.hh"
#endif

/** Implements a radio interface for radios using the HID USB schema.
 *
 * @todo Should implement @c RadioInterface.
 *
 * @ingroup rif */
class HID: public HIDevice, public RadioInterface
{
	Q_OBJECT

public:
  /** Connects to the radio with given vendor and product ID. */
	explicit HID(int vid, int pid, QObject *parent = nullptr);
  /** Destructor. */
	virtual ~HID();

  /** Returns @c true if the connection was established. */
	bool isOpen() const;

  void close();

  /** Returns radio identifier string. */
	QString identifier();

  bool read_start(uint32_t bank, uint32_t addr);
  /** Reads a block of data from the device at the given block number.
   * @param bno The block number.
   * @param data Pointer to memory where the read data is stored.
   * @param nbytes The number of bytes to read.
   * @returns @c true on success. */
  bool read(uint32_t bank, uint32_t addr, unsigned char *data, int nbytes);
  /** Signal read operation complete. */
  bool read_finish();

  bool write_start(uint32_t bank, uint32_t addr);
  /** Writes a block of data to the device at the given block number.
   * @param bno The block number.
   * @param data Pointer to memory where the read data is stored.
   * @param nbytes The number of bytes to read.
   * @returns @c true on success. */
  bool write(uint32_t bank, uint32_t addr, unsigned char *data, int nbytes);
  /** Signal write operation complete. */
	bool write_finish();

  /** Retruns the last error message. */
  inline const QString &errorMessage() const { return _errorMessage; }

private:
  bool selectMemoryBank(uint addr);
};

#endif // HIDINTERFACE_HH
