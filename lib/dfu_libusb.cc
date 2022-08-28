#include "dfu_libusb.hh"
#include <unistd.h>
#include "logger.hh"
#include "utils.hh"


// USB request types.
#define REQUEST_TYPE_TO_HOST    0xA1
#define REQUEST_TYPE_TO_DEVICE  0x21

enum {
    REQUEST_DETACH      = 0,
    REQUEST_DNLOAD      = 1,
    REQUEST_UPLOAD      = 2,
    REQUEST_GETSTATUS   = 3,
    REQUEST_CLRSTATUS   = 4,
    REQUEST_GETSTATE    = 5,
    REQUEST_ABORT       = 6,
};

enum {
    appIDLE                 = 0,
    appDETACH               = 1,
    dfuIDLE                 = 2,
    dfuDNLOAD_SYNC          = 3,
    dfuDNBUSY               = 4,
    dfuDNLOAD_IDLE          = 5,
    dfuMANIFEST_SYNC        = 6,
    dfuMANIFEST             = 7,
    dfuMANIFEST_WAIT_RESET  = 8,
    dfuUPLOAD_IDLE          = 9,
    dfuERROR                = 10,
};


/* ********************************************************************************************* *
 * Implementation of DFUDevice::Descriptor
 * ********************************************************************************************* */
DFUDevice::Descriptor::Descriptor(const USBDeviceInfo &info, uint8_t bus, uint8_t device)
  : USBDeviceDescriptor(info, USBDeviceHandle(bus, device))
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of DFUDevice
 * ********************************************************************************************* */
DFUDevice::DFUDevice(const USBDeviceDescriptor &descr, const ErrorStack &err, QObject *parent)
  : QObject(parent), _ctx(nullptr), _dev(nullptr)
{
  if (USBDeviceInfo::Class::DFU != descr.interfaceClass()) {
    errMsg(err) << "Cannot connect to DFU device using a non DFU descriptor: "
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

  logDebug() << "Try to detect USB DFU interface " << descr.description() << ".";
  USBDeviceHandle addr = descr.device().value<USBDeviceHandle>();
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


  if (libusb_kernel_driver_active(_dev, 0) && libusb_detach_kernel_driver(_dev, 0)) {
    errMsg(err) << "Cannot detach kernel driver for device " << descr.description()
                << ". Interface claim will likely fail.";
  }

  if (0 > (error = libusb_claim_interface(_dev, 0))) {
    errMsg(err) << "Failed to claim USB interface "  << descr.description()
                << ": " << libusb_strerror((enum libusb_error) error) << ".";
    libusb_close(_dev);
    _dev = nullptr;
    libusb_exit(_ctx);
    _ctx = nullptr;
    return;
  }

  logDebug() << "Connected to DFU device " << descr.description() << ".";
}

DFUDevice::~DFUDevice() {
  close();
}

QList<USBDeviceDescriptor>
DFUDevice::detect(uint16_t vid, uint16_t pid)
{
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

  logDebug() << "Search for DFU devices matching VID:PID "
             << QString::number(vid, 16) << ":" << QString::number(pid, 16) << ".";
  for (int i=0; (i<num)&&(nullptr!=lst[i]); i++) {
    libusb_device_descriptor descr;
    libusb_get_device_descriptor(lst[i], &descr);
    if ((vid == descr.idVendor) && (pid == descr.idProduct)) {
      logDebug() << "Found device on bus=" << libusb_get_bus_number(lst[i])
                 << ", device=" << libusb_get_device_address(lst[i])
                 << " with " << QString::number(vid, 16) << ":" << QString::number(pid, 16) << ".";
      res.append(DFUDevice::Descriptor(
                   USBDeviceInfo(USBDeviceInfo::Class::DFU, vid, pid),
                   libusb_get_bus_number(lst[i]),
                   libusb_get_device_address(lst[i])));
    }
  }

  libusb_free_device_list(lst, 1);
  return res;
}

bool
DFUDevice::isOpen() const {
  return nullptr != _dev;
}

void
DFUDevice::close() {
  if (nullptr != _dev) {
    libusb_release_interface(_dev, 0);
    libusb_close(_dev);
  }
  if (nullptr != _ctx)
    libusb_exit(_ctx);
  _ctx = nullptr;
  _dev = nullptr;
}


int
DFUDevice::download(unsigned block, uint8_t *data, unsigned len, const ErrorStack &err) {
  int error = libusb_control_transfer(
        _dev, REQUEST_TYPE_TO_DEVICE, REQUEST_DNLOAD, block, 0, data, len, 0);

  if (error < 0) {
    errMsg(err) << "Cannot write to device: " << libusb_strerror((enum libusb_error) error) << ".";
    return error;
  }

  return get_status();
}

int
DFUDevice::upload(unsigned block, uint8_t *data, unsigned len, const ErrorStack &err) {
  int error = libusb_control_transfer(
        _dev, REQUEST_TYPE_TO_HOST, REQUEST_UPLOAD, block, 0, data, len, 0);

  if (error < 0) {
    errMsg(err) << "Cannot read block: " << libusb_strerror((enum libusb_error) error) << ".";
    return error;
  }

  return get_status();
}

int
DFUDevice::detach(int timeout, const ErrorStack &err)
{
  int error = libusb_control_transfer(
        _dev, REQUEST_TYPE_TO_DEVICE, REQUEST_DETACH, timeout, 0, nullptr, 0, 0);
  if (0 > error) {
    errMsg(err) << "Cannot detach device: " << libusb_strerror((enum libusb_error) error) << ".";
    return error;
  }
  return 0;
}

int
DFUDevice::get_status(const ErrorStack &err)
{
  int error = libusb_control_transfer(
        _dev, REQUEST_TYPE_TO_HOST, REQUEST_GETSTATUS, 0, 0, (unsigned char*)&_status, 6, 0);
  if (0 > error) {
    errMsg(err) << "Cannot get status: " << libusb_strerror((enum libusb_error) error) << ".";
    return error;
  }
  return 0;
}

int
DFUDevice::clear_status(const ErrorStack &err)
{
  int error = libusb_control_transfer(
        _dev, REQUEST_TYPE_TO_DEVICE, REQUEST_CLRSTATUS, 0, 0, NULL, 0, 0);
  if (0 > error) {
    errMsg(err) << "Cannot clear status: " << libusb_strerror((enum libusb_error) error) << ".";
    return error;
  }
  return 0;
}

int
DFUDevice::get_state(int &pstate, const ErrorStack &err)
{
  unsigned char state;

  int error = libusb_control_transfer(
        _dev, REQUEST_TYPE_TO_HOST, REQUEST_GETSTATE, 0, 0, &state, 1, 0);
  pstate = state;
  if (error < 0) {
    errMsg(err) << "Cannot get state: " << libusb_strerror((enum libusb_error) error) << ".";
    return error;
  }
  return 0;
}

int
DFUDevice::abort(const ErrorStack &err)
{
  int error = libusb_control_transfer(
        _dev, REQUEST_TYPE_TO_DEVICE, REQUEST_ABORT, 0, 0, NULL, 0, 0);
  if (error < 0) {
    errMsg(err) << "Cannot abort: " << libusb_strerror((enum libusb_error) error) << ".";
    return error;
  }
  return 0;
}


int
DFUDevice::wait_idle()
{
  int state, error;

  for (;;) {
    if (0 > (error = get_state(state)))
      return 1;

    switch (state) {
      case dfuIDLE:
        return 0;

      case appIDLE:
        error = detach(1000);
        break;

      case dfuERROR:
        error = clear_status();
        break;

      case appDETACH:
      case dfuDNBUSY:
      case dfuMANIFEST_WAIT_RESET:
        usleep(100000);
        continue;

      default:
        error = abort();
        break;
    }

    if (error < 0)
      return 1;
  }
}


