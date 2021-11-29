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


DFUDevice::DFUDevice(unsigned vid, unsigned pid, const ErrorStack &err, QObject *parent)
  : QObject(parent), _ctx(nullptr), _dev(nullptr)
{
  logDebug() << "Try to detect USB DFU interface " << QString::number(vid,16)
             << ":" << QString::number(pid,16) << ".";

  int error = libusb_init(&_ctx);
  if (error < 0) {
    errMsg(err) << "Libusb init failed (" << error << "): "
                << libusb_strerror((enum libusb_error) error) << ".";
    return;
  }

  if (! (_dev = libusb_open_device_with_vid_pid(_ctx, vid, pid))) {
    errMsg(err) << "Cannot open device " << QString::number(vid, 16)
                << ":" << QString::number(pid, 16)
                << ": " << libusb_strerror((enum libusb_error) error);
    libusb_exit(_ctx);
    _ctx = nullptr;
    return;
  }

  if (libusb_kernel_driver_active(_dev, 0) && libusb_detach_kernel_driver(_dev, 0)) {
    errMsg(err) << "Cannot detach kernel driver for device " << QString::number(vid, 16)
                << ":" << QString::number(pid, 16) << ". Interface claim will likely fail.";
  }

  if (0 > (error = libusb_claim_interface(_dev, 0))) {
    errMsg(err) << "Failed to claim USB interface: " << libusb_strerror((enum libusb_error) error);
    libusb_close(_dev);
    _dev = nullptr;
    libusb_exit(_ctx);
    _ctx = nullptr;
    return;
  }

  logDebug() << "Connected to DFU device " << QString::number(vid,16)
             << ":" << QString::number(pid,16) << ".";
}


DFUDevice::~DFUDevice() {
  close();
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


