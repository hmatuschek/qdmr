#include "auctus_a6_interface.hh"
#include <QMetaEnum>
#include <QtEndian>
#include <QThread>
#include "logger.hh"

#define TIMEOUT 5000

AuctusA6Interface::AuctusA6Interface(
    const USBDeviceDescriptor &descriptor, const ErrorStack &err, QObject *parent)
  : USBSerial(descriptor, QSerialPort::Baud9600, err, parent), _state(CLOSED)
{
  if (isOpen()) {
    _state = IDLE;
  } else {
    errMsg(err) << "Cannot construct Auctus A6 interface.";
    _state = ERROR;
    return;
  }
  if (! clear(QSerialPort::AllDirections)) {
    logWarn() << "Cannot clear RX/TX buffer of serial interface.";
  }
  logDebug() << "Open interface to Auctus A6 based radio.";
}

AuctusA6Interface::State
AuctusA6Interface::state() const {
  return _state;
}


bool
AuctusA6Interface::send_receive(uint16_t command,
                                const uint8_t *params, uint8_t plen,
                                uint8_t *response, uint8_t &rlen, const ErrorStack &err)
{
  if (! send(command, params, plen, err)) {
    errMsg(err) << "Cannot send command.";
    return false;
  }

  uint16_t rcommand=0;
  if (! receive(rcommand, response, rlen, err)) {
    errMsg(err) << "Cannot receive response.";
    return false;
  }

  if ((rcommand&0x7fff) != command) {
    errMsg(err) << "Request and response commands mismatch. Expected "
                << QString::number(command, 16) << " got "
                << QString::number(command&0x7fff, 16) << ".";
    return false;
  }

  return true;
}


bool
AuctusA6Interface::send(uint16_t command,
                        const uint8_t *params, uint8_t plen, const ErrorStack &err)
{
  // check parameter length
  if (plen>249) {
    errMsg(err) << "Parameter length cannot exceed 250 bytes.";
    errMsg(err) << "Cannot send request " << QString::number(command, 16) << "h.";
    return false;
  }

  // assemble request
  uint8_t buffer[255], *ptr=buffer;
  uint8_t total_length = 6 + plen;
  memset(buffer, 0, total_length);
  // start of packet
  *ptr = 0xaa; ptr += sizeof(uint8_t);
  // length
  *ptr = 6 + plen; ptr += sizeof(uint8_t); // total length
  // command
  (*(uint16_t *)(ptr)) = qToBigEndian(command); ptr += sizeof(uint16_t);
  // copy parameters
  memcpy(ptr, params, plen); ptr += plen;
  // compute CRC
  for (int i=0; i<(3+plen); i++)
    (*ptr) ^= buffer[1+i]; // Skip start-of-packet byte.
  ptr += sizeof(uint8_t);
  // end of packet
  *ptr = 0xbb;

  // send request
  logDebug() << "Send " << QByteArray((const char *)buffer, total_length).toHex();
  if (total_length != QSerialPort::write((const char*)buffer, total_length)) {
    errMsg(err) << "QSerialPort: " << errorString();
    errMsg(err) << "Cannot send request " << QString::number(command, 16) << "h.";
    return false;
  }
  if (! flush()) {
    logWarn() << "No data writen to the device, " << bytesToWrite()
              << " of " << total_length << " left in buffer.";
  }

  return true;
}


bool
AuctusA6Interface::receive(uint16_t &command,
                           uint8_t *response, uint8_t &rlen, const ErrorStack &err)
{
  uint8_t buffer[255]; memset(buffer, 0x00, sizeof(buffer));

  // wait for start-of-packet byte
  while (0xaa != buffer[0]) {
    if (! read(buffer, 1, TIMEOUT, err)) {
      errMsg(err) << "Cannot read response " << QString::number(command, 16) << "h.";
      return false;
    }
  }

  // read length bytes
  uint8_t total_length = 2;
  if (! read(buffer+1, 1, TIMEOUT, err)) {
    errMsg(err) << "Cannot read response " << QString::number(command, 16) << "h.";
    return false;
  }

  // get length
  total_length = buffer[1];
  if (6 > total_length) {
    errMsg(err) << "Invalid packet: Expected minimum lenght of 6h, got "
                << QString::number(total_length, 16) << "h.";
    return false;
  }

  // read remaining packet
  if (! read(buffer+2, total_length-2, TIMEOUT, err)) {
    errMsg(err) << "Cannot read response " << QString::number(command, 16) << "h.";
    return false;
  }

  logDebug() << "Got response " << QByteArray((const char *)buffer, total_length).toHex() << ".";

  // unpack command
  command = qFromBigEndian((*(uint16_t *)(buffer+2)));

  // check CRC
  uint8_t crc = 0;
  for (int i=1; i<(total_length-1); i++)
    crc ^= buffer[i];
  if (crc) {
    errMsg(err) << "Invalid response: Invalid CRC.";
    return false;
  }

  // Check if response buffer is sufficiently large
  if (rlen < (total_length-6)) {
    errMsg(err) << "Cannot store response in response buffer, buffer too small. "
                << "Got " << rlen << "bytes, needs " << (total_length-6) << "bytes.";
    return false;
  }

  // copy response payload
  rlen = total_length-6;
  memcpy(response, buffer+4, rlen);

  // done.
  return true;
}

bool
AuctusA6Interface::read(uint8_t *data, qint64 n, unsigned int timeout_ms, const ErrorStack &err)
{
  while (n > 0) {
    if (0 == bytesAvailable()) {
      if (! waitForReadyRead(timeout_ms)) {
        errMsg(err) << "QSerialPort: " << errorString();
        return false;
      }
    }

    qint64 k = QSerialPort::read((char *)data, std::min(n, bytesAvailable()));
    if (0 > k) {
      errMsg(err) << "QSerialPort: " << errorString();
      return false;
    }

    n -= k;
    data += k;
  }

  return true;
}
