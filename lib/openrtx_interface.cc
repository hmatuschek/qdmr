#include "openrtx_interface.hh"

OpenRTXInterface::OpenRTXInterface(const USBDeviceDescriptor &descr, const ErrorStack &err, QObject *parent)
  : USBSerial(descr, USBSerial::Baud115200, err, parent)
{
  // pass...
}


bool
OpenRTXInterface::isOpen() const {
  return USBSerial::isOpen();
}

void
OpenRTXInterface::close() {
  USBSerial::close();
}


RadioInfo
OpenRTXInterface::identifier(const ErrorStack &err) {
  Q_UNUSED(err)
  return RadioInfo();
}


bool
OpenRTXInterface::read_start(uint32_t bank, uint32_t addr, const ErrorStack &err) {
  return false;
}

bool
OpenRTXInterface::read(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes, const ErrorStack &err) {
  return false;
}

bool
OpenRTXInterface::read_finish(const ErrorStack &err) {
  return false;
}


bool
OpenRTXInterface::write_start(uint32_t bank, uint32_t addr, const ErrorStack &err) {
  return false;
}

bool
OpenRTXInterface::write(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes, const ErrorStack &err) {
  return false;
}

bool
OpenRTXInterface::write_finish(const ErrorStack &err) {
  return false;
}

bool
OpenRTXInterface::reboot(const ErrorStack &err) {
  return false;
}
