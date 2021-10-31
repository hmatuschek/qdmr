#include "hid_libusb.hh"
#include "logger.hh"

#define HID_INTERFACE   0                   // interface index
#define TIMEOUT_MSEC    500                 // receive timeout
#define MAX_RETRY       100                 // Number of retries


HIDevice::HIDevice(int vid, int pid, QObject *parent)
  : QObject(parent), _ctx(nullptr), _dev(nullptr), _transfer(nullptr), _errorMessage()
{
  logDebug() << "Try to detect USB HID interface "
             << QString::number(vid, 16) << ":"
             << QString::number(pid, 16) << ".";

  int error = libusb_init(&_ctx);
  if (error < 0) {
    _errorMessage = tr("Cannot init libusb (%1): %2")
        .arg(error).arg(libusb_strerror((enum libusb_error) error));
    logError() << _errorMessage;
    _ctx = nullptr;
    return;
  }

  if (! (_dev = libusb_open_device_with_vid_pid(_ctx, vid, pid))) {
    _errorMessage = tr("Cannot find USB device %1:%2")
        .arg(vid,0,16).arg(pid,0,16);
    logDebug() << _errorMessage;
    libusb_exit(_ctx);
    _ctx = nullptr;
    return;
  }

  if (libusb_kernel_driver_active(_dev, 0))
    libusb_detach_kernel_driver(_dev, 0);

  error = libusb_claim_interface(_dev, HID_INTERFACE);
  if (error < 0) {
    _errorMessage = tr("Failed to claim USB interface: %1: %2")
        .arg(error).arg(libusb_strerror((enum libusb_error) error));
    logError() << _errorMessage;
    libusb_close(_dev);
    libusb_exit(_ctx);
    _dev = nullptr;
    _ctx = nullptr;
  }
}

HIDevice::~HIDevice() {
  close();
}

bool
HIDevice::isOpen() const {
  return (nullptr != _ctx) && (nullptr != _dev);
}

void
HIDevice::close() {
  if (nullptr == _ctx)
    return;

  logDebug() << "Closing HIDevice.";

  if (nullptr != _transfer) {
    libusb_free_transfer(_transfer);
    _transfer = nullptr;
  }

  if (nullptr != _dev) {
    libusb_release_interface(_dev, HID_INTERFACE);
    libusb_close(_dev);
    _dev = nullptr;
  }

  libusb_exit(_ctx);
  _ctx = nullptr;
}

bool
HIDevice::hid_send_recv(const unsigned char *data, unsigned nbytes, unsigned char *rdata, unsigned rlength) {
  unsigned char buf[42];
  unsigned char reply[42];
  int reply_len;

  memset(buf, 0, sizeof(buf));
  buf[0] = 1;
  buf[1] = 0;
  buf[2] = nbytes;
  buf[3] = nbytes >> 8;
  if (nbytes > 0)
    memcpy(buf+4, data, nbytes);
  nbytes += 4;

  reply_len = write_read(buf, sizeof(buf), reply, sizeof(reply));
  if (reply_len < 0) {
    return false;
  }

  if (reply_len != sizeof(reply)) {
    _errorMessage = tr("Short read: %1 bytes instead of %2!")
        .arg(reply_len).arg((int)sizeof(reply));
    logError() << _errorMessage;
    return false;
  }
  if (reply[0] != 3 || reply[1] != 0 || reply[3] != 0) {
    _errorMessage = tr("Incorrect reply!");
    logError() << _errorMessage;
    return false;
  }
  if (reply[2] != rlength) {
    _errorMessage = tr("Incorrect reply length %1, expected %1.")
        .arg(reply[2]).arg(rlength);
    logError() << _errorMessage;
    return false;
  }

  memcpy(rdata, reply+4, rlength);
  return true;
}


int
HIDevice::write_read(const unsigned char *data, unsigned length, unsigned char *reply, unsigned rlength)
{
  if (! _transfer) {
    // Allocate transfer descriptor on first invocation.
    _transfer = libusb_alloc_transfer(0);
  }

  libusb_fill_interrupt_transfer(
        _transfer, _dev,
        LIBUSB_RECIPIENT_INTERFACE | LIBUSB_ENDPOINT_IN,
        reply, rlength, read_callback, this, TIMEOUT_MSEC);

  size_t nretry = 0;
again:
  _nbytes_received = 0;
  libusb_submit_transfer(_transfer);

  int result = libusb_control_transfer(
        _dev,
        LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE|LIBUSB_ENDPOINT_OUT,
        0x09/*HID Set_Report*/, (2/*HID output*/ << 8) | 0,
        HID_INTERFACE, (unsigned char*)data, length, TIMEOUT_MSEC);

  if (result < 0) {
    _errorMessage = tr("Error %1 transmitting data via control transfer: %2.")
        .arg(result).arg(libusb_strerror((enum libusb_error) result));
    _transfer = nullptr;
    return -1;
  }

  while (_nbytes_received == 0) {
    result = libusb_handle_events(_ctx);
    if (result < 0) {
      /* Break out of this loop only on fatal error.*/
      if (result != LIBUSB_ERROR_BUSY &&
          result != LIBUSB_ERROR_TIMEOUT &&
          result != LIBUSB_ERROR_OVERFLOW &&
          result != LIBUSB_ERROR_INTERRUPTED) {
        _errorMessage = tr("Error %1 receiving data via interrupt transfer: %2.")
            .arg(result).arg(libusb_strerror((enum libusb_error) result));
        return result;
      }
    }
  }

  if ((_nbytes_received == LIBUSB_ERROR_TIMEOUT) && (nretry < MAX_RETRY)) {
    if (0 == nretry)
      logDebug() << "HID (libusb): timeout. Retry...";
    nretry++;
    goto again;
  } else if (nretry >= MAX_RETRY) {
    logError() << "HID (libusb): Retry limit of " << MAX_RETRY << " exceeded.";
  }

  return _nbytes_received;
}


void
HIDevice::read_callback(struct libusb_transfer *t)
{
  HIDevice *self = (HIDevice *)t->user_data;

  switch (t->status) {
    case LIBUSB_TRANSFER_COMPLETED:
      memcpy(self->_receive_buf, t->buffer, t->actual_length);
      self->_nbytes_received = t->actual_length;
      break;

    case LIBUSB_TRANSFER_CANCELLED:
        self->_nbytes_received = LIBUSB_ERROR_INTERRUPTED;
        self->_errorMessage = libusb_error_name(LIBUSB_ERROR_INTERRUPTED);
        return;

    case LIBUSB_TRANSFER_NO_DEVICE:
        self->_nbytes_received = LIBUSB_ERROR_NO_DEVICE;
        self->_errorMessage = libusb_error_name(LIBUSB_ERROR_NO_DEVICE);
        return;

    case LIBUSB_TRANSFER_TIMED_OUT:
        self->_nbytes_received = LIBUSB_ERROR_TIMEOUT;
        self->_errorMessage = libusb_error_name(LIBUSB_ERROR_TIMEOUT);
        break;

    default:
        self->_nbytes_received = LIBUSB_ERROR_IO;
        self->_errorMessage = libusb_error_name(LIBUSB_ERROR_IO);
   }
}
