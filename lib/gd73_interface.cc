#include "gd73_interface.hh"
#include "logger.hh"
#include <QtEndian>

#define BLOCK_SIZE 0x35

GD73Interface::GD73Interface(const USBDeviceDescriptor &descriptor, const ErrorStack &err, QObject *parent)
  : C7000Device(descriptor, err, parent), RadioInterface()
{
  Packet request, response;
  if (nullptr == _dev) {
    errMsg(err) << "Cannot initialize GD73 interface: C7000 interface not open.";
    return;
  }

  request = Packet(0x01, 0x04);
  if (! sendRecv(request, response, err)) {
    errMsg(err) << "Cannot enter programming mode.";
    C7000Device::close();
  }
  logDebug() << "Entered prog mode. Response: " << response.payload().toHex() << ".";
}

bool
GD73Interface::isOpen() const {
  return C7000Device::isOpen();
}

RadioInfo
GD73Interface::identifier(const ErrorStack &err) {
  Q_UNUSED(err);
  return RadioInfo::byID(RadioInfo::GD73);
}

void
GD73Interface::close() {
  C7000Device::close();
}

bool
GD73Interface::write_start(uint32_t bank, uint32_t addr, const ErrorStack &err) {
  Q_UNUSED(bank); Q_UNUSED(addr); Q_UNUSED(err);
  return true;
}

bool
GD73Interface::write(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes, const ErrorStack &err) {
  Q_UNUSED(bank);

  if ((addr%BLOCK_SIZE) || (nbytes!=BLOCK_SIZE)) {
    errMsg(err) << "Address and size must align with block size of 35h";
    return false;
  }

  C7000Device::Packet request, response;
  QByteArray payload; payload.resize(2);
  *((uint16_t *)payload.data()) = qToLittleEndian((uint16_t)(addr/BLOCK_SIZE));
  payload.append((char *)data, nbytes);
  request = C7000Device::Packet(0x01, 0x00, 0x0f, payload);
  if (! sendRecv(request, response, err)) {
    errMsg(err) << "Cannot send write command.";
    return false;
  }

  return true;
}

bool
GD73Interface::write_finish(const ErrorStack &err) {
  Q_UNUSED(err);
  return true;
}

bool
GD73Interface::read_start(uint32_t bank, uint32_t addr, const ErrorStack &err) {
  Q_UNUSED(bank); Q_UNUSED(addr); Q_UNUSED(err);
  _lastSequence = 0xffff;
  logDebug() << "Start codeplug read, seqnr=" << Qt::hex << _lastSequence << "h.";
  return true;
}

bool
GD73Interface::read(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes, const ErrorStack &err) {
  Q_UNUSED(bank);

  if ((addr%BLOCK_SIZE) || (nbytes!=BLOCK_SIZE)) {
    errMsg(err) << "Address and size must align with block size of 35h";
    return false;
  }

  //logDebug() << "Read " << nbytes << "bytes from address " << Qt::hex << addr << "h.";

  uint16_t seqNum = addr/BLOCK_SIZE;
  if (uint16_t(_lastSequence+1) != seqNum) {
    errMsg(err) << "Out-of-sequence read: Expected seqnr. " << uint16_t(_lastSequence+1)
                << " got " << seqNum << ".";
    return false;
  }

  C7000Device::Packet request, response;
  if (0xffff == _lastSequence) {
    // Request start-of-read
    request = C7000Device::Packet(0x01, 0x02);
  } else {
    QByteArray payload(2,0); *((uint16_t *)payload.data()) = qToLittleEndian(_lastSequence);
    request = C7000Device::Packet(0x04, 0x01, 0x0f, payload);
  }

  if (! sendRecv(request, response, err)) {
    errMsg(err) << "Cannot read codeplug from device.";
    return false;
  }

  _lastSequence = qFromLittleEndian(*(uint16_t *)response.payload().data());
  memcpy(data, response.payload().data()+2, nbytes);

  return true;
}

bool
GD73Interface::read_finish(const ErrorStack &err) {
  Q_UNUSED(err);
  _lastSequence = 0xffff;
  return true;
}

