#ifndef DFU_LIBUSB_HH
#define DFU_LIBUSB_HH

#include <QObject>
#include <libusb.h>
#include "errorstack.hh"

/** This class implements DFU protocol to access radios.
 *
 * Many manufactures use the standardized DFU protocoll to programm codeplugs and update the
 * firmware of their radios. This class implements this protocol, see
 * https://www.usb.org/sites/default/files/DFU_1.1.pdf for details.
 *
 * @ingroup rif */
class DFUDevice: public QObject, public ErrorStack
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
  bool isOpen() const;
  /** Closes the DFU iterface. */
  void close();

  /** Downloads some data to the device. */
  int download(unsigned block, uint8_t *data, unsigned len);
  /** Uploads some data from the device. */
  int upload(unsigned block, uint8_t *data, unsigned len);

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
};

#endif // DFU_LIBUSB_HH
