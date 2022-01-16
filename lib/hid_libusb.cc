#include "hid_libusb.hh"
#include "logger.hh"

#define HID_INTERFACE   0                   // interface index
#define TIMEOUT_MSEC    500                 // receive timeout
#define MAX_RETRY       100                 // Number of retries

/* ********************************************************************************************* *
 * Implementation of HIDevice::Descriptor
 * ********************************************************************************************* */
HIDevice::Descriptor::Descriptor(const USBDeviceInfo &info, uint8_t bus, uint8_t device)
  : USBDeviceDescriptor(info, USBDeviceAddress(bus, device))
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of HIDevice
 * ********************************************************************************************* */
HIDevice::HIDevice(int vid, int pid, const ErrorStack &err, QObject *parent)
  : QObject(parent), _ctx(nullptr), _dev(nullptr), _transfer(nullptr)
{
  logDebug() << "Try to detect USB HID interface "
             << QString::number(vid, 16) << ":"
             << QString::number(pid, 16) << ".";

  int error = libusb_init(&_ctx);
  if (error < 0) {
    err.take(_cbError);
    errMsg(err) << "Cannot init libusb (" << error
                << "): " << libusb_strerror((enum libusb_error) error) << ".";
    _ctx = nullptr;
    return;
  }

  if (! (_dev = libusb_open_device_with_vid_pid(_ctx, vid, pid))) {
    err.take(_cbError);
    errMsg(err) << "Cannot find USB device " << QString::number(vid, 16)
                << ":" << QString::number(pid, 16) << ".";
    libusb_exit(_ctx);
    _ctx = nullptr;
    return;
  }

  if (libusb_kernel_driver_active(_dev, 0))
    libusb_detach_kernel_driver(_dev, 0);

  error = libusb_claim_interface(_dev, HID_INTERFACE);
  if (error < 0) {
    err.take(_cbError);
    errMsg(err) << "Failed to claim USB interface (" << error
                << "): " << libusb_strerror((enum libusb_error) error) << ".";
    libusb_close(_dev);
    libusb_exit(_ctx);
    _dev = nullptr;
    _ctx = nullptr;
  }
}

HIDevice::HIDevice(const USBDeviceDescriptor &descr, const ErrorStack &err, QObject *parent)
  : QObject(parent), _ctx(nullptr), _dev(nullptr), _transfer(nullptr)
{
  if (USBDeviceInfo::Class::HID != descr.interfaceClass()) {
    errMsg(err) << "Cannot connect to HID device using a non HID descriptor: "
                << descr.description() << ".";
    return;
  }

  int error = libusb_init(&_ctx);
  if (error < 0) {
    errMsg(err) << "Libusb init failed (" << error << "): "
                << libusb_strerror((enum libusb_error) error) << ".";
    return;
  }

  int num=0;
  libusb_device **lst;
  libusb_device *dev=nullptr;
  if (0 > (num = libusb_get_device_list(_ctx, &lst))) {
    errMsg(err) << "Cannot obtain list of USB devices.";
    libusb_exit(_ctx);
    _ctx = nullptr;
    return;
  }

  logDebug() << "Try to detect USB HID interface " << descr.description() << ".";
  USBDeviceAddress addr = descr.device().value<USBDeviceAddress>();
  for (int i=0; (i<num)&&(nullptr!=lst[i]); i++) {
    if (addr.bus != libusb_get_bus_number(lst[i]))
      continue;
    if (addr.device != libusb_get_device_address(lst[i]))
      continue;
    libusb_device_descriptor usb_descr;
    if (0 > libusb_get_device_descriptor(lst[i],&usb_descr))
      continue;
    if (descr.vendorId() != usb_descr.idVendor)
      continue;
    if (descr.productId() != usb_descr.idProduct)
      continue;
    logDebug() << "Matching device found at bus " << addr.bus << ", device " << addr.device
               << " with vendor ID " << QString::number(usb_descr.idVendor, 16)
               << " and product ID " << QString::number(usb_descr.idProduct, 16) << ".";
    libusb_ref_device(lst[i]); dev = lst[i];
  }
  // Unref all devices and free list, matching device was referenced earlier
  libusb_free_device_list(lst, 1);

  if (nullptr == dev) {
    errMsg(err) << "No matching device found: " << descr.description() << ".";
    libusb_exit(_ctx);
    _ctx = nullptr;
    return;
  }

  if (0 > (error = libusb_open(dev, &_dev))) {
    errMsg(err) << "Cannot open device " << descr.description()
                << ": " << libusb_strerror((enum libusb_error) error) << ".";
    libusb_unref_device(dev);
    libusb_exit(_ctx);
    _ctx = nullptr;
  }

  if (libusb_kernel_driver_active(_dev, 0))
    libusb_detach_kernel_driver(_dev, 0);

  error = libusb_claim_interface(_dev, HID_INTERFACE);
  if (error < 0) {
    errMsg(err) << "Failed to claim HID interface (" << error
                << "): " << libusb_strerror((enum libusb_error) error) << ".";
    libusb_close(_dev);
    libusb_exit(_ctx);
    _dev = nullptr;
    _ctx = nullptr;
  }
}

HIDevice::~HIDevice() {
  close();
}

QList<USBDeviceDescriptor>
HIDevice::detect(uint16_t vid, uint16_t pid) {
  QList<USBDeviceDescriptor> res;

  int error, num;
  libusb_context *ctx;
  if (0 > (error = libusb_init(&ctx))) {
    logError() << "Libusb init failed (" << error << "): "
               << libusb_strerror((enum libusb_error) error) << ".";
    return res;
  }

  libusb_device **lst;
  if (0 == (num = libusb_get_device_list(ctx, &lst))) {
    logDebug() << "No USB devices found at all.";
    // unref devices and free list
    libusb_free_device_list(lst, 1);
    return res;
  }

  logDebug() << "Search for HID interfaces matching VID:PID "
             << QString::number(vid, 16) << ":" << QString::number(pid, 16) << ".";
  for (int i=0; (i<num)&&(nullptr!=lst[i]); i++) {
    libusb_device_descriptor descr;
    libusb_get_device_descriptor(lst[i], &descr);
    if ((vid == descr.idVendor) && (pid == descr.idProduct)) {
      logDebug() << "Found device on bus=" << libusb_get_bus_number(lst[i])
                 << ", device=" << libusb_get_device_address(lst[i])
                 << " matching " << QString::number(vid, 16) << ":"
                 << QString::number(pid, 16) << ".";
      res.append(HIDevice::Descriptor(
                   USBDeviceInfo(USBDeviceInfo::Class::HID, vid, pid),
                   libusb_get_bus_number(lst[i]),
                   libusb_get_device_address(lst[i])));
    }
  }

  libusb_free_device_list(lst, 1);
  return res;
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
HIDevice::hid_send_recv(const unsigned char *data, unsigned nbytes,
                        unsigned char *rdata, unsigned rlength, const ErrorStack &err) {
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
    err.take(_cbError);
    return false;
  }

  if (reply_len != sizeof(reply)) {
    errMsg(err) << "Short read: " << reply_len
                << " bytes instead of " << (int)sizeof(reply) << "!";
    return false;
  }
  if (reply[0] != 3 || reply[1] != 0 || reply[3] != 0) {
    errMsg(err) << "Incorrect reply!";
    return false;
  }
  if (reply[2] != rlength) {
    errMsg(err) << "Incorrect reply length " << reply[2]
                << ", expected " << rlength << ".";
    return false;
  }

  memcpy(rdata, reply+4, rlength);
  return true;
}


int
HIDevice::write_read(const unsigned char *data, unsigned length,
                     unsigned char *reply, unsigned rlength, const ErrorStack &err)
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
    err.take(_cbError);
    errMsg(err) << "Error " << result << " transmitting data via control transfer: "
                << libusb_strerror((enum libusb_error) result) << ".";
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
          result != LIBUSB_ERROR_INTERRUPTED)
      {
        err.take(_cbError);
        errMsg(err) << "Error " <<result << " receiving data via interrupt transfer: "
                    << libusb_strerror((enum libusb_error) result) << ".";
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
    errMsg(self->_cbError) << libusb_error_name(LIBUSB_ERROR_INTERRUPTED);
    return;

  case LIBUSB_TRANSFER_NO_DEVICE:
    self->_nbytes_received = LIBUSB_ERROR_NO_DEVICE;
    errMsg(self->_cbError) << libusb_error_name(LIBUSB_ERROR_NO_DEVICE);
    return;

  case LIBUSB_TRANSFER_TIMED_OUT:
    self->_nbytes_received = LIBUSB_ERROR_TIMEOUT;
    errMsg(self->_cbError) << libusb_error_name(LIBUSB_ERROR_TIMEOUT);
    break;

  default:
    self->_nbytes_received = LIBUSB_ERROR_IO;
    errMsg(self->_cbError) << libusb_error_name(LIBUSB_ERROR_IO);
  }
}
