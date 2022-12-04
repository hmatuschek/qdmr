#include "auctus_a6_interface.hh"
#include <QtEndian>
#include "logger.hh"

#define TIMEOUT 5000

AuctusA6Interface::AuctusA6Interface(
    const USBDeviceDescriptor &descriptor, const ErrorStack &err, QObject *parent)
  : USBSerial(descriptor, err, parent), _state(CLOSED)
{
  if (isOpen()) {
    _state = IDLE;
  } else {
    errMsg(err) << "Cannot construct Auctus A6 interface.";
    _state = ERROR;
    return;
  }
  logDebug() << "Open interface to Auctus A6 based radio.";

  // Set device properties
  if (!setParity(QSerialPort::NoParity)) {
    logWarn() << "Cannot set parity of the serial port to none.";
  }
  if (! setStopBits(QSerialPort::OneStop)) {
    logWarn() << "Cannot set stop bit.";
  }
  if (! setBaudRate(QSerialPort::Baud9600)) {
    logWarn() << "Cannot set speed to 9600 baud.";
  }
  if (! setFlowControl(QSerialPort::HardwareControl)) {
    logWarn() << "Cannot enable hardware flow control.";
  }
}


bool
AuctusA6Interface::send_receive(uint16_t command,
                                const uint8_t *params, uint8_t plen,
                                uint8_t *response, uint8_t &rlen, const ErrorStack &err)
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
    (*ptr) ^= buffer[1+i];
  ptr += sizeof(uint8_t);
  // end of packet
  *ptr = 0xbb;

  // send request
  if (total_length != QSerialPort::write((const char*)buffer, total_length)) {
    errMsg(err) << "QSerialPort: " << errorString();
    errMsg(err) << "Cannot send request " << QString::number(command, 16) << "h.";
    return false;
  }

  // wait for start-of-packet and length bytes
  total_length = 2;
  while (total_length > bytesAvailable()) {
    if (! waitForReadyRead(TIMEOUT)) {
      errMsg(err) << "QSerialPort: " << errorString();
      errMsg(err) << "Cannot read response " << QString::number(command, 16) << "h.";
      return false;
    }
  }
  // read start-of-packet and length
  if (2 != QSerialPort::read((char *)buffer, 2)) {
    errMsg(err) << "QSerialPort: " << errorString();
    return false;
  }
  // check start of packet
  if (0xaa != buffer[0]) {
    errMsg(err) << "Unexpected start-of-packet byte: Expected aah, got "
                << QString::number(buffer[0],16) << "h.";
    return false;
  }
  // get length
  total_length = 2+buffer[1];
  if (6 > total_length) {
    errMsg(err) << "Invalid packet: Expected minimum lenght of 6h, got "
                << QString::number(total_length, 16) << "h.";
    return false;
  }
  // wait for remaining packet
  while (total_length > bytesAvailable()) {
    if (! waitForReadyRead(TIMEOUT)) {
      errMsg(err) << "QSerialPort: " << errorString();
      errMsg(err) << "Cannot read response " << QString::number(command, 16) << "h.";
      return false;
    }
  }
  // read remaining packet
  if ((total_length-2) != QSerialPort::read((char *)(buffer+2), total_length-2)) {
    errMsg(err) << "QSerialPort: " << errorString();
    return false;
  }
  // check response
  uint16_t responseCommand = qFromBigEndian((*(uint16_t *)(buffer+2)));
  if ((! (responseCommand & 0x80)) || (command != (responseCommand&0x7f))) {
    errMsg(err) << "Unexpected response command " << QString::number(responseCommand, 16)
                << "h, expected " << QString::number(command | 0x80, 16) << "h.";
    return false;
  }

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
  memcpy(response, buffer+5, rlen);

  // done.
  return true;
}

