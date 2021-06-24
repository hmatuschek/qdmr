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


DFUDevice::DFUDevice(unsigned vid, unsigned pid, QObject *parent)
  : QObject(parent), RadioInterface(), _ctx(nullptr), _dev(nullptr), _ident(nullptr)
{
  //logDebug() << "Try to detect USB DFU interface " << Qt::hex << vid << ":" << pid << ".";
  logDebug() << "Try to detect USB DFU interface " << QString::number(vid,16)
             << ":" << QString::number(pid,16) << ".";
  int error = libusb_init(&_ctx);
  if (error < 0) {
    _errorMessage = tr("%1 Libusb init failed: %2 %3").arg(__func__).arg(error)
        .arg(libusb_strerror((enum libusb_error) error));
    return;
  }

  if (! (_dev = libusb_open_device_with_vid_pid(_ctx, vid, pid))) {
    _errorMessage = tr("%1 Cannot open device %2, %3: %4 %5").arg(__func__).arg(vid,0,16).arg(pid,0,16)
        .arg(error).arg(libusb_strerror((enum libusb_error) error));
    libusb_exit(_ctx);
    _ctx = 0;
    return;
  }

  if (libusb_kernel_driver_active(_dev, 0))
    libusb_detach_kernel_driver(_dev, 0);

  if (0 > (error = libusb_claim_interface(_dev, 0))) {
    _errorMessage = tr("%1 Failed to claim USB interface: %2 %3").arg(__func__).arg(error)
        .arg(libusb_strerror((enum libusb_error) error));
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

bool
DFUDevice::isOpen() const {
  return nullptr != _ident;
}

QString
DFUDevice::identifier() {
  return _ident;
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
  if (0 > error)
    _errorMessage = tr("%1 Cannot detatch device: %2 %3").arg(__func__).arg(error)
        .arg(libusb_strerror((enum libusb_error) error));
  return error;
}


int
DFUDevice::get_status()
{
  int error = libusb_control_transfer(
        _dev, REQUEST_TYPE_TO_HOST, REQUEST_GETSTATUS, 0, 0, (unsigned char*)&_status, 6, 0);
  if (0 > error) {
    _errorMessage = tr("%1 Cannot get status: %2 %3. Recv: %4, %5, %6, %7").arg(__func__)
        .arg(error).arg(libusb_strerror((enum libusb_error) error))
        .arg(_status.status).arg(_status.poll_timeout).arg(_status.state)
        .arg(_status.string_index);
    return error;
  }
  return 0;
}


int
DFUDevice::clear_status()
{
  int error = libusb_control_transfer(
        _dev, REQUEST_TYPE_TO_DEVICE, REQUEST_CLRSTATUS, 0, 0, NULL, 0, 0);
  if (0 > error)
    _errorMessage = tr("%1 Cannot clear status: %2 %3").arg(__func__).arg(error)
        .arg(libusb_strerror((enum libusb_error) error));
  return 0;
}


int
DFUDevice::get_state(int &pstate)
{
  unsigned char state;

  int error = libusb_control_transfer(
        _dev, REQUEST_TYPE_TO_HOST, REQUEST_GETSTATE, 0, 0, &state, 1, 0);
  pstate = state;
  if (error < 0)
    _errorMessage = tr("%1 Cannot get state: %2 %3").arg(__func__).arg(error)
        .arg(libusb_strerror((enum libusb_error) error));
  return 0;
}


int
DFUDevice::abort()
{
  int error = libusb_control_transfer(
        _dev, REQUEST_TYPE_TO_DEVICE, REQUEST_ABORT, 0, 0, NULL, 0, 0);
  if (error < 0)
    _errorMessage = tr("%1 Cannot abort: %2 %3").arg(__func__).arg(error)
        .arg(libusb_strerror((enum libusb_error) error));
  return error;
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


int
DFUDevice::md380_command(uint8_t a, uint8_t b)
{
    unsigned char cmd[2] = { a, b };

    int error = libusb_control_transfer(
          _dev, REQUEST_TYPE_TO_DEVICE, REQUEST_DNLOAD, 0, 0, cmd, 2, 0);

    if (error < 0) {
      _errorMessage = tr("%1 Cannot send command: %2 %3").arg(__func__).arg(error)
          .arg(libusb_strerror((enum libusb_error) error));
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
    _errorMessage = tr("%1 Cannot send command: %2 %3").arg(__func__).arg(error)
        .arg(libusb_strerror((enum libusb_error) error));
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
    _errorMessage = tr("%1 Cannot send command: %2 %3").arg(__func__).arg(error)
        .arg(libusb_strerror((enum libusb_error) error));
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
    _errorMessage = tr("%1 Cannot read data: %2 %3").arg(__func__).arg(error)
        .arg(libusb_strerror((enum libusb_error) error));
    return nullptr;
  }
  get_status();

  return (const char*) data;
}


bool
DFUDevice::erase(uint start, uint size, void(*progress)(uint, void *), void *ctx) {
  int error;
  // Enter Programming Mode.
  if ((error = get_status()))
    return false;
  if ((error = wait_idle()))
    return false;
  if ((error = md380_command(0x91, 0x01)))
    return false;
  usleep(100000);

  uint end = start+size;
  start = align_addr(start, 0x10000);
  end = align_size(end, 0x10000);
  size = end-start;

  for (uint i=0; i<size; i+=0x10000) {
    erase_block(start+i);
    if (progress)
      progress((i*100)/size, ctx);
  }

  // Zero address.
  return (0 == set_address(0x00000000));
}

bool
DFUDevice::read_start(uint32_t bank, uint32_t addr) {
  Q_UNUSED(bank);
  Q_UNUSED(addr);
  // pass...
  return true;
}

bool
DFUDevice::read(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes) {
  Q_UNUSED(bank);

  if (nullptr == data) {
    _errorMessage = tr("%1 Cannot write data into nullptr!").arg(__func__);
    return false;
  }
  uint32_t block = addr/1024;
  int error = libusb_control_transfer(
        _dev, REQUEST_TYPE_TO_HOST, REQUEST_UPLOAD, block+2, 0, data, nbytes, 0);
  if (error < 0) {
    _errorMessage = tr("%1 Cannot read block: %2 %3").arg(__func__).arg(error)
        .arg(libusb_strerror((enum libusb_error) error));
    return false;
  }
  return 0 == get_status();
}

bool
DFUDevice::read_finish() {
  // pass...
  return true;
}


bool
DFUDevice::write_start(uint32_t bank, uint32_t addr) {
  Q_UNUSED(bank);
  Q_UNUSED(addr);
  return true;
}

bool
DFUDevice::write(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes) {
  Q_UNUSED(bank);

  if (nullptr == data) {
    _errorMessage = tr("%1 Cannot read data from nullptr!").arg(__func__);
    return false;
  }
  uint32_t block = addr/1024;
  int error = libusb_control_transfer(
        _dev, REQUEST_TYPE_TO_DEVICE, REQUEST_DNLOAD, block+2, 0, data, nbytes, 0);
  if (error < 0) {
    _errorMessage = tr("%1 Cannot write block: %2 %3").arg(__func__).arg(error)
        .arg(libusb_strerror((enum libusb_error) error));
    return false;
  }
  if ((error = get_status()))
    return false;
  return 0 == wait_idle();
}

bool
DFUDevice::write_finish() {
  // pass...
  return true;
}


bool DFUDevice::reboot() {
  unsigned char cmd[2] = { 0x91, 0x05 };

  if (! _ctx)
    return false;
  if (wait_idle())
    return false;

  int error;
  if (0 > (error = libusb_control_transfer(_dev, REQUEST_TYPE_TO_DEVICE, REQUEST_DNLOAD, 0, 0, cmd, 2, 0))) {
    _errorMessage = tr("%1 Cannot send reboot command: %2 %3").arg(__func__).arg(error)
        .arg(libusb_strerror((enum libusb_error) error));
    return false;
  }

  return (0 <= get_status());
}

const QString &
DFUDevice::errorMessage() const {
  return _errorMessage;
}
