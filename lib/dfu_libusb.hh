#ifndef DFU_LIBUSB_HH
#define DFU_LIBUSB_HH

#include <QObject>
#include <libusb.h>
#include "errorstack.hh"
#include "radiointerface.hh"

/** This class implements DFU protocol to access radios.
 *
 * Many manufactures use the standardized DFU protocol to program codeplugs and update the
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
  /** Specialization to address a DFU device uniquely. */
  class Descriptor: public USBDeviceDescriptor
  {
  public:
    /** Constructor from interface info, bus number and device address. */
    Descriptor(const USBDeviceInfo &info, uint8_t bus, uint8_t device);
  };

public:
  /** Opens a connection to the USB-DFU device at vendor @c vid and product @c pid. */
  DFUDevice(const USBDeviceDescriptor &descr, const ErrorStack &err=ErrorStack(), QObject *parent=nullptr);
  /** Destructor. */
	virtual ~DFUDevice();

  /** Returns @c true if the DFU device interface is open. */
  bool isOpen() const;
  /** Closes the DFU interface. */
  void close();

  /** Downloads some data to the device. */
  int download(unsigned block, uint8_t *data, unsigned len, const ErrorStack &err=ErrorStack());
  /** Uploads some data from the device. */
  int upload(unsigned block, uint8_t *data, unsigned len, const ErrorStack &err=ErrorStack());

public:
  /** Finds all DFU interfaces with the specified VID/PID combination. */
  static QList<USBDeviceDescriptor> detect(uint16_t vid, uint16_t pid);

protected:
  /** Internal used function to detach the device. */
  int detach(int timeout, const ErrorStack &err=ErrorStack());
  /** Internal used function to read the current status. */
  int get_status(const ErrorStack &err=ErrorStack());
  /** Internal used function to clear the status. */
  int clear_status(const ErrorStack &err=ErrorStack());
  /** Internal used function to read the state. */
  int get_state(int &pstate, const ErrorStack &err=ErrorStack());
  /** Internal used function to abort the current operation. */
  int abort(const ErrorStack &err=ErrorStack());
  /** Internal used function to busy-wait for a response from the device. */
  int wait_idle(const ErrorStack &err=ErrorStack());

protected:
  /** USB context. */
	libusb_context *_ctx;
  /** USB device object. */
	libusb_device_handle *_dev;
  /** Device status. */
	status_t _status;
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
  DFUSEDevice(const USBDeviceDescriptor &descr, const ErrorStack &err=ErrorStack(), uint16_t blocksize=32, QObject *parent=nullptr);

  void close();

  /** Returns the blocksize in bytes. */
  uint16_t blocksize() const;

  /** Sets the read/write reference address. By default this is @c 0x08000000 (flash program
   * memory address on ST devices). */
  bool setAddress(uint32_t address, const ErrorStack &err=ErrorStack());
  /** Reads a block of data from the device. The address is computed as base address +
   * block*blocksize, where the base address is set using the @c setAddress method. */
  bool readBlock(unsigned block, uint8_t *data, const ErrorStack &err=ErrorStack());
  /** Writes a block of data to the device. The address is computed as base address +
   * block*blocksize, where the base address is set using the @c setAddress method. */
  bool writeBlock(unsigned block, const uint8_t *data, const ErrorStack &err=ErrorStack());
  /** Erases an entire page of memory at the specified address. A page is usually 0x10000 bytes
   * large. */
  bool erasePage(uint32_t address, const ErrorStack &err=ErrorStack());
  /** Erases the entire memory. Not reconmended. */
  bool eraseAll(const ErrorStack &err=ErrorStack());
  /** Releases the read lock. This usually also erases the entire memory. Not reconmended. */
  bool releaseReadLock(const ErrorStack &err=ErrorStack());
  /** Leaves the DFU mode, may boot into the application code. */
  bool leaveDFU(const ErrorStack &err=ErrorStack());

protected:
  /** Holds the block size in bytes. */
  uint16_t _blocksize;
};


#endif // DFU_LIBUSB_HH
