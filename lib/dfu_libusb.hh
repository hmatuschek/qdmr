#ifndef DFU_LIBUSB_HH
#define DFU_LIBUSB_HH

#include <QObject>
#include <libusb.h>

/** This class implements DFU protocol to access radios.
 *
 * Many manufactures use the standardized DFU protocoll to programm codeplugs and update the
 * firmware of their radios. This class implements this protocol, see
 * https://www.usb.org/sites/default/files/DFU_1.1.pdf for details.
 *
 * @ingroup rif */
class DFUDevice: public QObject
{
	Q_OBJECT

private:
  /** Status message from device. */
  struct __attribute__((packed)) status_t {
		unsigned  status       : 8;
		unsigned  poll_timeout : 24;
		unsigned  state        : 8;
		unsigned  string_index : 8;
  };

public:
  /** Opens a connection to the USB-DFU devuce at vendor @c vid and product @c pid. */
	DFUDevice(unsigned vid, unsigned pid, QObject *parent=nullptr);
  /** Destructor. */
	virtual ~DFUDevice();

  /** Retuns @c true if the DFU device interface is open. */
  virtual bool isOpen() const;
  /** Closes the DFU iterface. */
  virtual void close();

  /** Downloads some data to the device. */
  int download(unsigned block, uint8_t *data, unsigned len);
  /** Uploads some data from the device. */
  int upload(unsigned block, uint8_t *data, unsigned len);

  /** Returns the last error message. */
  const QString &errorMessage() const;

protected:
  /** Internal used function to detach the device. */
	int detach(int timeout);
  /** Internal used function to read the current status. */
	int get_status();
  /** Internal used function to clear the status. */
	int clear_status();
  /** Internal used function to read the state. */
	int get_state(int &pstate);
  /** Internal used function to abort the current operation. */
	int abort();
  /** Internal used function to busy-wait for a response from the device. */
	int wait_idle();

protected:
  /** USB context. */
	libusb_context *_ctx;
  /** USB device object. */
	libusb_device_handle *_dev;
  /** Device status. */
	status_t _status;
  /** Holds the last error message. */
  QString _errorMessage;
};


/** Implements the ST MCU extesions for the DFU protocol, aka DfuSe.
 * This class implements the extensions to the DFU protocaol used by ST for their MCUs. This is
 * also known as DfuSe.
 *
 * @ingroup rif */
class DFUSEDevice: public DFUDevice
{
public:
  /** Constructor, also connects to the specifed VID/PID device found first. The @c blocksize
   * specifies the blocksize for every read and write operation. */
  DFUSEDevice(unsigned vid, unsigned pid, uint16_t blocksize=32, QObject *parent=nullptr);

  void close();

  /** Returns the blocksize in bytes. */
  uint16_t blocksize() const;

  /** Sets the read/write reference address. By default this is @c 0x08000000 (flash program
   * memory address on ST devices). */
  bool setAddress(uint32_t address);
  /** Reads a block of data from the device. The address is computed as base address +
   * block*blocksize, where the base address is set using the @c setAddress method. */
  bool readBlock(unsigned block, uint8_t *data);
  /** Writes a block of data to the device. The address is computed as base address +
   * block*blocksize, where the base address is set using the @c setAddress method. */
  bool writeBlock(unsigned block, const uint8_t *data);
  /** Erases an entire page of memory at the specified address. A page is usually 0x10000 bytes
   * large. */
  bool erasePage(uint32_t address);
  /** Erases the entire memory. Not reconmended. */
  bool eraseAll();
  /** Releases the read lock. This usually also erases the entire memory. Not reconmended. */
  bool releaseReadLock();
  /** Leaves the DFU mode, may boot into the application code. */
  bool leaveDFU();

protected:
  /** Holds the block size in bytes. */
  uint16_t _blocksize;
};


#endif // DFU_LIBUSB_HH
