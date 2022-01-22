#ifndef HID_MACOS_HH
#define HID_MACOS_HH

#include <QObject>
#include <IOKit/hid/IOHIDManager.h>
#include "errorstack.hh"
#include "radiointerface.hh"

/** Implements the HID radio interface MacOS X API.
 * @ingroup rif */
class HIDevice: public QObject
{
	Q_OBJECT

public:
  /** Specialization to address a HI device uniquely. */
  class Descriptor: public USBDeviceDescriptor
  {
  public:
    /** Constructor from interface info, bus number and device address. */
    Descriptor(const USBDeviceInfo &info, uint32_t locationId, uint16_t device);
  };

public:
  /** Opens a connection to the device with given vendor and product ID. */
	HIDevice(const USBDeviceDescriptor &descr, const ErrorStack &err=ErrorStack(), QObject *parent=nullptr);
  /** Destrutor. */
	virtual ~HIDevice();

  /** Returns @c true if the connection was established. */
	bool isOpen() const;

  /** Send command/data to the device and store response in @c rdata.
   * @param data Pointer to the command/data to send.
   * @param nbytes The number of bytes to send.
   * @param rdata Pointer to receive buffer.
   * @param rlength Size of receive buffer.
   * @param err The stack to put error messages on. */
	bool hid_send_recv(const unsigned char *data, unsigned nbytes,
                     unsigned char *rdata, unsigned rlength,
                     const ErrorStack &err=ErrorStack());

  /** Close connection to device. */
	void close();

public:
  /** Finds all HID interfaces with the specified VID/PID combination. */
  static QList<USBDeviceDescriptor> detect(uint16_t vid, uint16_t pid);

protected:
  /** Internal callback for response data. */
	static void callback_input(void *context, IOReturn result, void *sender, IOHIDReportType type,
	                           uint32_t reportID, uint8_t *data, CFIndex nbytes);
  /** Internal callback for device opend. */
	static void callback_open(void *context, IOReturn result, void *sender, IOHIDDeviceRef deviceRef);
  /** Internal callback for device closed. */
	static void callback_close(void *ontext, IOReturn result, void *sender, IOHIDDeviceRef deviceRef);

protected:
  /** Device manager. */
  IOHIDManagerRef _HIDManager;
	/** Device handle. */
	volatile IOHIDDeviceRef _dev;
	/** Device buffer. */
	unsigned char _transfer_buf[42];
	/** Receive buffer. */
	unsigned char _receive_buf[42];
	/** Receive result. */
	volatile int _nbytes_received = 0;
};

#endif // HID_MACOS_HH
