#include "dfu_libusb.hh"
#include <QDebug>
#include <unistd.h>

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


DFUDevice::DFUDevice(unsigned vid, unsigned pid, QObject *parent)
  : RadioInferface(parent), _ctx(nullptr), _dev(nullptr), _ident(nullptr)
{
  int error = libusb_init(&_ctx);
  if (error < 0) {
    qDebug() << __func__ << "libusb init failed:"
             << error << libusb_strerror((enum libusb_error) error);
    return;
  }

  if (! (_dev = libusb_open_device_with_vid_pid(_ctx, vid, pid))) {
      libusb_exit(_ctx);
      _ctx = 0;
      return;
  }

  if (libusb_kernel_driver_active(_dev, 0))
    libusb_detach_kernel_driver(_dev, 0);

  if (0 > (error = libusb_claim_interface(_dev, 0))) {
    qDebug() << __func__ << "Failed to claim USB interface:"
             << error << libusb_strerror((enum libusb_error) error);
    libusb_close(_dev);
    libusb_exit(_ctx);
    _ctx = 0;
    return;
  }

  // Enter Programming Mode.
  if (wait_idle())
    return;
  if (md380_command(0x91, 0x01))
    return;

  // Get device identifier in a static buffer.
  _ident = identify();

  // Zero address.
  set_address(0x00000000);
}


DFUDevice::~DFUDevice() {
  if (isOpen())
    close();
}

void
DFUDevice::close() {
  if (isOpen()) {
    libusb_release_interface(_dev, 0);
    libusb_close(_dev);
    libusb_exit(_ctx);
    _ctx = nullptr;
    _ident = nullptr;
  }
}


int
DFUDevice::detach(int timeout)
{
  int error = libusb_control_transfer(
        _dev, REQUEST_TYPE_TO_DEVICE, REQUEST_DETACH, timeout, 0, nullptr, 0, 0);
  return error;
}


int
DFUDevice::get_status()
{
  int error = libusb_control_transfer(
        _dev, REQUEST_TYPE_TO_HOST, REQUEST_GETSTATUS, 0, 0, (unsigned char*)&_status, 6, 0);
  if (error < 0) {
    qDebug() << __func__ << ": Recv:" << _status.status << _status.poll_timeout
             << _status.state << _status.string_index;
    return error;
  }
  return 0;
}


int
DFUDevice::clear_status()
{
  libusb_control_transfer(
        _dev, REQUEST_TYPE_TO_DEVICE, REQUEST_CLRSTATUS, 0, 0, NULL, 0, 0);
  return 0;
}


int
DFUDevice::get_state(int &pstate)
{
  unsigned char state;

  int error = libusb_control_transfer(
        _dev, REQUEST_TYPE_TO_HOST, REQUEST_GETSTATE, 0, 0, &state, 1, 0);
  pstate = state;
  if (error < 0) {
    qDebug() << __func__ << ": Recv " << state;
    return error;
  }
  return 0;
}


int
DFUDevice::abort()
{
  int error = libusb_control_transfer(
        _dev, REQUEST_TYPE_TO_DEVICE, REQUEST_ABORT, 0, 0, NULL, 0, 0);
  return error;
}


int
DFUDevice::wait_idle()
{
  int state, error;

  for (;;) {
    error = get_state(state);
    if (error < 0) {
      qDebug() << __func__ << "cannot get state:"
               << error << libusb_strerror((enum libusb_error) error);
      return 1;
    }

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

    if (error < 0) {
      qDebug() << __func__ << "unexpected usb error in state " << state << ":"
               << error << libusb_strerror((enum libusb_error) error);
      return 1;
    }
  }
}


int
DFUDevice::md380_command(uint8_t a, uint8_t b)
{
    unsigned char cmd[2] = { a, b };

    int error = libusb_control_transfer(
          _dev, REQUEST_TYPE_TO_DEVICE, REQUEST_DNLOAD, 0, 0, cmd, 2, 0);

    if (error < 0) {
      qDebug() << __func__ << "cannot send command:"
               << error << libusb_strerror((enum libusb_error) error);
      return 1;
    }

    if ((error = get_status()))
      return error;
    usleep(100000);

    return wait_idle();
}


int
DFUDevice::set_address(uint32_t address)
{
  unsigned char cmd[5] =
  { 0x21,
    (uint8_t)address,
    (uint8_t)(address >> 8),
    (uint8_t)(address >> 16),
    (uint8_t)(address >> 24), };

  int error = libusb_control_transfer(
        _dev, REQUEST_TYPE_TO_DEVICE, REQUEST_DNLOAD, 0, 0, cmd, 5, 0);
  if (error < 0) {
    qDebug() << __func__ << "cannot send command:"
             << error << libusb_strerror((enum libusb_error) error);
    return 1;
  }

  if ((error = get_status()))
    return error;

  return wait_idle();
}


int
DFUDevice::erase_block(uint32_t address)
{
  unsigned char cmd[5] =
  { 0x41,
    (uint8_t)address,
    (uint8_t)(address >> 8),
    (uint8_t)(address >> 16),
    (uint8_t)(address >> 24), };

  int error = libusb_control_transfer(
        _dev, REQUEST_TYPE_TO_DEVICE, REQUEST_DNLOAD, 0, 0, cmd, 5, 0);
  if (error < 0) {
    qDebug() << __func__ << "cannot send command:"
             << error << libusb_strerror((enum libusb_error) error);
    return 1;
  }

  get_status();
  wait_idle();

  return 0;
}


const char *
DFUDevice::identify()
{
  static uint8_t data[64];

  md380_command(0xa2, 0x01);

  int error = libusb_control_transfer(
        _dev, REQUEST_TYPE_TO_HOST, REQUEST_UPLOAD, 0, 0, data, 64, 0);
  if (error < 0) {
    qDebug() << __func__ << "cannot read data:"
             << error << libusb_strerror((enum libusb_error) error);
    return nullptr;
  }
  get_status();
  return (const char*) data;
}


bool
DFUDevice::erase(unsigned start, unsigned finish)
{
  int error;
  // Enter Programming Mode.
  if ((error = get_status()))
    return false;
  if ((error = wait_idle()))
    return false;
  if ((error = md380_command(0x91, 0x01)))
    return false;
  usleep(100000);

  if (start == 0) {
    // Erase 256kbytes of configuration memory.
    error = erase_block(0x00000000)
        || erase_block(0x00010000)
        || erase_block(0x00020000)
        || erase_block(0x00030000);
    if (error)
      return false;

    if (finish > 256*1024) {
      // Erase 768kbytes of extended configuration memory.
      error = erase_block(0x00110000)
          || erase_block(0x00120000)
          || erase_block(0x00130000)
          || erase_block(0x00140000)
          || erase_block(0x00150000)
          || erase_block(0x00160000)
          || erase_block(0x00170000)
          || erase_block(0x00180000)
          || erase_block(0x00190000)
          || erase_block(0x001a0000)
          || erase_block(0x001b0000)
          || erase_block(0x001c0000)
          || erase_block(0x001d0000);
      if (error)
        return false;
    }
  } else {
    // Erase callsign database.
    unsigned int addr;

    for (addr=start; addr<finish; addr+=0x00010000) {
      if ((error = erase_block(addr)))
        return false;
    }
  }

  // Zero address.
  return (0 == set_address(0x00000000));
}


bool
DFUDevice::read_block(int bno, uint8_t *data, int nbytes)
{
  if (bno >= 256 && bno < 2048)
    bno += 832;

  int error = libusb_control_transfer(
        _dev, REQUEST_TYPE_TO_HOST, REQUEST_UPLOAD, bno+2, 0, data, nbytes, 0);
  if (error < 0) {
    qDebug() << __func__ << "cannot read block" << bno << ", nbytes=" << nbytes
             << ":" << error << libusb_strerror((enum libusb_error) error);
    return false;
  }
  return 0 == get_status();
}


bool
DFUDevice::write_block(int bno, uint8_t *data, int nbytes)
{
  if (bno >= 256 && bno < 2048)
    bno += 832;

  int error = libusb_control_transfer(
        _dev, REQUEST_TYPE_TO_DEVICE, REQUEST_DNLOAD, bno+2, 0, data, nbytes, 0);
  if (error < 0) {
    qDebug() << __func__ << "cannot write block" << bno << " nbytes=" << nbytes
             << ":" << error << libusb_strerror((enum libusb_error) error);
    return false;
  }
  if ((error = get_status()))
    return false;
  return 0 == wait_idle();
}


bool DFUDevice::reboot()
{
  unsigned char cmd[2] = { 0x91, 0x05 };

  if (! _ctx)
    return false;
  if (wait_idle())
    return false;

  int error;
  if (0 > (error = libusb_control_transfer(_dev, REQUEST_TYPE_TO_DEVICE, REQUEST_DNLOAD, 0, 0, cmd, 2, 0))) {
    qDebug() << __func__ << "cannot send command:"
             << error << libusb_strerror((enum libusb_error) error);
    return false;
  }

  return (0 <= get_status());
}
