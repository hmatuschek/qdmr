#ifndef DFU_LIBUSB_HH
#define DFU_LIBUSB_HH

#include <QObject>
#include <libusb.h>
#include "radiointerface.hh"

/** This class implements DFU protocol to access radios.
 *
 * Many manufactures use the standardized DFU protocoll to programm codeplugs and update the
 * firmware of their radios. This class implements this protocol.
 *
 * @ingroup rif */
class DFUDevice: public RadioInferface
{
	Q_OBJECT

  /** Current status. */
	typedef struct {
		unsigned  status       : 8;
		unsigned  poll_timeout : 24;
		unsigned  state        : 8;
		unsigned  string_index : 8;
	} status_t;

public:
  /** Opens a connection to the USB-DFU devuce at vendor @c vid and product @c pid. */
	DFUDevice(unsigned vid, unsigned pid, QObject *parent=nullptr);
  /** Destructor. */
	virtual ~DFUDevice();

	bool isOpen() const { return nullptr != _ident; }
	QString identifier() const { return _ident; }
	void close();

  /** Erases a memory section from @c start to @c finish. */
	bool erase(unsigned start, unsigned finish);
	bool read_block(int bno, uint8_t *data, int nbytes);
	bool write_block(int bno, uint8_t *data, int nbytes);
	bool reboot();

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
  /** Internal used function to wait for a response from the device. */
	int wait_idle();
  /** Internal used function to send a controll command to the device. */
	int md380_command(uint8_t a, uint8_t b);
  /** Internal used function to set the current I/O address. */
	int set_address(uint32_t address);
  /** Internal used function to erase a specific block. */
	int erase_block(uint32_t address);
  /** Internal used function to read the device identifier. */
	const char *identify();
  /** Internal used function to initialize the DFU connection to the device. */
	const char *dfu_init(unsigned vid, unsigned pid);

protected:
  /** USB context. */
	libusb_context *_ctx;
  /** USB device object. */
	libusb_device_handle *_dev;
  /** Device status. */
	status_t _status;
  /** Read identifier. */
  const char *_ident;
};

#endif // DFU_LIBUSB_HH
