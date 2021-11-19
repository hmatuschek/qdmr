#include "tyt_interface.hh"
#include "logger.hh"
#include <unistd.h>
#include "utils.hh"
#include "errorstack.hh"

TyTInterface::TyTInterface(unsigned vid, unsigned pid, const ErrorStack &err, QObject *parent)
  : DFUDevice(vid, pid, err, parent), RadioInterface()
{
  if (! DFUDevice::isOpen()) {
    errMsg(err) << "Cannot open TyTInterface.";
    return;
  }

  // Enter Programming Mode.
  if (wait_idle()) {
    errMsg(err) << "Device not ready. Close device.";
    close(); return;
  }

  if (md380_command(0x91, 0x01, err)) {
    errMsg(err) << "Cannot enter programming mode. Close device.";
    reboot(err);
    close(); return;
  }

  // Get device identifier in a static buffer.
  const char *idstr = identify(err);
  if (idstr && (0==strcmp("MD390", idstr))) {
    _ident = RadioInfo::byID(RadioInfo::MD390);
  } else if (idstr && (0==strcmp("MD-UV380", idstr))) {
    _ident = RadioInfo::byID(RadioInfo::UV380);
  } else if (idstr && (0==strcmp("MD-UV390", idstr))) {
    _ident = RadioInfo::byID(RadioInfo::UV390);
  } else if (idstr && (0==strcmp("2017", idstr))) {
    _ident = RadioInfo::byID(RadioInfo::MD2017);
  } else if (idstr) {
    errMsg(err) << "Unknown TyT device '" << idstr << "'.";
    close(); return;
  }

  // Zero address.
  if(set_address(0x00000000, err)) {
    errMsg(err) << "Cannot set device address to 0x00000000.";
    close(); return;
  }
}

TyTInterface::~TyTInterface() {
  if (isOpen())
    close();
}

void
TyTInterface::close() {
  if (isOpen()) {
    _ident = RadioInfo();
  }
  DFUDevice::close();
}

bool
TyTInterface::isOpen() const {
  return DFUDevice::isOpen() && _ident.isValid();
}

RadioInfo
TyTInterface::identifier(const ErrorStack &err) {
  Q_UNUSED(err);
  return _ident;
}

int
TyTInterface::md380_command(uint8_t a, uint8_t b, const ErrorStack &err)
{
  unsigned char cmd[2] = { a, b };

  if (int error = download(0, cmd, 2, err))
    return error;

  usleep(100000);
  return wait_idle();
}


int
TyTInterface::set_address(uint32_t address, const ErrorStack &err)
{
  unsigned char cmd[5] =
  { 0x21,
    (uint8_t)address,
    (uint8_t)(address >> 8),
    (uint8_t)(address >> 16),
    (uint8_t)(address >> 24), };

  if (int error = download(0, cmd, 5, err))
    return error;

  return wait_idle();
}


int
TyTInterface::erase_block(uint32_t address, const ErrorStack &err)
{
  unsigned char cmd[5] =
  { 0x41,
    (uint8_t)address,
    (uint8_t)(address >> 8),
    (uint8_t)(address >> 16),
    (uint8_t)(address >> 24), };

  if (int error = download(0, cmd, 5, err))
    return error;

  wait_idle();

  return 0;
}


const char *
TyTInterface::identify(const ErrorStack &err)
{
  static uint8_t data[64];

  md380_command(0xa2, 0x01, err);

  if (upload(0, data, 64, err))
    return nullptr;

  return (const char*) data;
}


bool
TyTInterface::erase(unsigned start, unsigned size, void(*progress)(unsigned, void *), void *ctx, const ErrorStack &err) {
  int error;
  // Enter Programming Mode.
  if ((error = get_status(err)))
    return false;
  if ((error = wait_idle()))
    return false;
  if ((error = md380_command(0x91, 0x01, err)))
    return false;
  usleep(100000);

  unsigned end = start+size;
  start = align_addr(start, 0x10000);
  end = align_size(end, 0x10000);
  size = end-start;

  for (unsigned i=0; i<size; i+=0x10000) {
    erase_block(start+i, err);
    if (progress)
      progress((i*100)/size, ctx);
  }

  // Zero address.
  return (0 == set_address(0x00000000, err));
}

bool
TyTInterface::read_start(uint32_t bank, uint32_t addr, const ErrorStack &err) {
  Q_UNUSED(bank);
  Q_UNUSED(addr);
  Q_UNUSED(err);
  // pass...
  return true;
}

bool
TyTInterface::read(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes, const ErrorStack &err) {
  Q_UNUSED(bank);

  if (nullptr == data) {
    errMsg(err) << "Cannot write data into nullptr!";
    return false;
  }

  uint32_t block = addr/1024;
  return 0 == upload(block+2, data, nbytes, err);
}

bool
TyTInterface::read_finish(const ErrorStack &err) {
  Q_UNUSED(err);
  return true;
}


bool
TyTInterface::write_start(uint32_t bank, uint32_t addr, const ErrorStack &err) {
  Q_UNUSED(bank); Q_UNUSED(addr); Q_UNUSED(err)
  return true;
}

bool
TyTInterface::write(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes, const ErrorStack &err) {
  Q_UNUSED(bank);

  if (nullptr == data) {
    errMsg(err) << "Cannot read data from nullptr!";
    return false;
  }

  uint32_t block = addr/1024;
  if (download(block+2, data, nbytes, err))
    return false;

  return 0 == wait_idle();
}

bool
TyTInterface::write_finish(const ErrorStack &err) {
  Q_UNUSED(err);
  return true;
}


bool
TyTInterface::reboot(const ErrorStack &err) {

  if (! _ctx)
    return false;

  if (wait_idle())
    return false;

  unsigned char cmd[2] = { 0x91, 0x05 };
  return download(0, cmd, 2, err);
}

