#ifndef HID_MACOS_HH
#define HID_MACOS_HH

#include <QObject>
#include <libusb.h>
#include "errorstack.hh"
#include "radiointerface.hh"

/** Implements the HID radio interface using libusb.
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
    Descriptor(const USBDeviceInfo &info, uint8_t bus, uint8_t device);
  };


public:
  /** Connects to the device with given vendor and product ID. */
  HIDevice(const USBDeviceDescriptor &descr, const ErrorStack &err=ErrorStack(), QObject *parent=nullptr);
  /** Destructor. */
	virtual ~HIDevice();

  /** Returns @c true if the connection is established. */
	bool isOpen() const;
  /** Send command/data to the device and store response in @c rdata.
   * @param data Pointer to the command/data to send.
   * @param nbytes The number of bytes to send.
   * @param rdata Pointer to receive buffer.
   * @param rlength Size of receive buffer.
   * @param err Passes an error stack to put error messages on. */
  bool hid_send_recv(const unsigned char *data, unsigned nbytes,
                     unsigned char *rdata, unsigned rlength, const ErrorStack &err=ErrorStack());

  /** Close connection to device. */
	void close();

public:
  /** Finds all HID interfaces with the specified VID/PID combination. */
  static QList<USBDeviceDescriptor> detect(uint16_t vid, uint16_t pid);

protected:
  /** Internal used implementation of send_recv(). */
  int write_read(const unsigned char *data, unsigned length,
                 unsigned char *reply, unsigned rlength, const ErrorStack &err=ErrorStack());
  /** Callback for response data. */
  static void read_callback(struct libusb_transfer *t);

protected:
  /** libusb context. */
  libusb_context *_ctx;
  /** libusb device. */
  libusb_device_handle *_dev;
	/** libusb async transfer descriptor. */
	struct libusb_transfer *_transfer;
	/** Receive buffer. */
	unsigned char _receive_buf[42];
	/** Receive result. */
	volatile int _nbytes_received;
  /** Internal used error stack for the static callback function. */
  ErrorStack _cbError;
};

#endif // HID_MACOS_HH
