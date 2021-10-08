#include "tyt_interface.hh"
#include "logger.hh"
#include <unistd.h>
#include "utils.hh"


TyTInterface::TyTInterface(unsigned vid, unsigned pid, QObject *parent)
  : DFUDevice(vid, pid, parent), RadioInterface()
{
  if (! DFUDevice::isOpen())
    return;

  // Enter Programming Mode.
  if (wait_idle())
    return;
  if (md380_command(0x91, 0x01))
    return;

  // Get device identifier in a static buffer.
  const char *idstr = identify();
  if (idstr && (0==strcmp("MD390", idstr))) {
    _ident = RadioInfo::byID(RadioInfo::MD390);
  } else if (idstr && (0==strcmp("MD-UV390", idstr))) {
    _ident = RadioInfo::byID(RadioInfo::UV390);
  } else if (idstr && (0==strcmp("2017", idstr))) {
    _ident = RadioInfo::byID(RadioInfo::MD2017);
  } else if (idstr) {
    logError() << "Unknown TyT device '" << idstr << "'";
  }

  // Zero address.
  set_address(0x00000000);
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
}

bool
TyTInterface::isOpen() const {
  return DFUDevice::isOpen() && _ident.isValid();
}

const QString &
TyTInterface::errorMessage() const {
  return DFUDevice::errorMessage();
}

RadioInfo
TyTInterface::identifier() {
  return _ident;
}

int
TyTInterface::md380_command(uint8_t a, uint8_t b)
{
  unsigned char cmd[2] = { a, b };

  if (int error = download(0, cmd, 2))
    return error;

  usleep(100000);
  return wait_idle();
}


int
TyTInterface::set_address(uint32_t address)
{
  unsigned char cmd[5] =
  { 0x21,
    (uint8_t)address,
    (uint8_t)(address >> 8),
    (uint8_t)(address >> 16),
    (uint8_t)(address >> 24), };

  if (int error = download(0, cmd, 5))
    return error;

  return wait_idle();
}


int
TyTInterface::erase_block(uint32_t address)
{
  unsigned char cmd[5] =
  { 0x41,
    (uint8_t)address,
    (uint8_t)(address >> 8),
    (uint8_t)(address >> 16),
    (uint8_t)(address >> 24), };

  if (int error = download(0, cmd, 5))
    return error;

  wait_idle();

  return 0;
}


const char *
TyTInterface::identify()
{
  static uint8_t data[64];

  md380_command(0xa2, 0x01);

  if (upload(0, data, 64))
    return nullptr;

  return (const char*) data;
}


bool
TyTInterface::erase(unsigned start, unsigned size, void(*progress)(unsigned, void *), void *ctx) {
  int error;
  // Enter Programming Mode.
  if ((error = get_status()))
    return false;
  if ((error = wait_idle()))
    return false;
  if ((error = md380_command(0x91, 0x01)))
    return false;
  usleep(100000);

  unsigned end = start+size;
  start = align_addr(start, 0x10000);
  end = align_size(end, 0x10000);
  size = end-start;

  for (unsigned i=0; i<size; i+=0x10000) {
    erase_block(start+i);
    if (progress)
      progress((i*100)/size, ctx);
  }

  // Zero address.
  return (0 == set_address(0x00000000));
}

bool
TyTInterface::read_start(uint32_t bank, uint32_t addr) {
  Q_UNUSED(bank);
  Q_UNUSED(addr);
  // pass...
  return true;
}

bool
TyTInterface::read(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes) {
  Q_UNUSED(bank);

  if (nullptr == data) {
    _errorMessage = tr("%1 Cannot write data into nullptr!").arg(__func__);
    return false;
  }

  uint32_t block = addr/1024;
  return 0 == upload(block+2, data, nbytes);
}

bool
TyTInterface::read_finish() {
  // pass...
  return true;
}


bool
TyTInterface::write_start(uint32_t bank, uint32_t addr) {
  Q_UNUSED(bank);
  Q_UNUSED(addr);
  return true;
}

bool
TyTInterface::write(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes) {
  Q_UNUSED(bank);

  if (nullptr == data) {
    _errorMessage = tr("%1 Cannot read data from nullptr!").arg(__func__);
    return false;
  }

  uint32_t block = addr/1024;
  if (download(block+2, data, nbytes))
    return false;

  return 0 == wait_idle();
}

bool
TyTInterface::write_finish() {
  // pass...
  return true;
}


bool
TyTInterface::reboot() {

  if (! _ctx)
    return false;

  if (wait_idle())
    return false;

  unsigned char cmd[2] = { 0x91, 0x05 };
  return download(0, cmd, 2);
}

