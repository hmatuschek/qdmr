#ifndef HID_MACOS_HH
#define HID_MACOS_HH

#include <QObject>
#include <IOKit/hid/IOHIDManager.h>

class HIDevice: public QObject
{
	Q_OBJECT

public:
	HIDevice(int vid, int pid, QObject *parent=nullptr);
	virtual ~HIDevice();

	bool isOpen() const;

	bool hid_send_recv(const unsigned char *data, unsigned nbytes, unsigned char *rdata, unsigned rlength);

	void close();

protected:
	static void callback_input(void *context, IOReturn result, void *sender, IOHIDReportType type,
	                           uint32_t reportID, uint8_t *data, CFIndex nbytes);
	static void callback_open(void *context, IOReturn result, void *sender, IOHIDDeviceRef deviceRef);
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
