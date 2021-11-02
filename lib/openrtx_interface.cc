#include "openrtx_interface.hh"

OpenRTXInterface::OpenRTXInterface(QObject *parent)
  : DFUDevice(0x0000, 0x0000, parent), RadioInterface()
{
  // pass...
}


bool
OpenRTXInterface::isOpen() const {
  return DFUDevice::isOpen();
}

void
OpenRTXInterface::close() {
  DFUDevice::close();
}

const QString &
OpenRTXInterface::errorMessage() const {
  return DFUDevice::errorMessage();
}


RadioInfo
OpenRTXInterface::identifier() {
  return RadioInfo();
}


bool
OpenRTXInterface::read_start(uint32_t bank, uint32_t addr) {
  return false;
}

bool
OpenRTXInterface::read(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes) {
  return false;
}

bool
OpenRTXInterface::read_finish() {
  return false;
}


bool
OpenRTXInterface::write_start(uint32_t bank, uint32_t addr) {
  return false;
}

bool
OpenRTXInterface::write(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes) {
  return false;
}

bool
OpenRTXInterface::write_finish() {
  return false;
}

bool
OpenRTXInterface::reboot() {
  return false;
}
