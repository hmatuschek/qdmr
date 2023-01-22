/** @defgroup auctus Auctus A6/A7 Based devices.
 * @ingroup dsc */
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
  total_length = buffer[1];
  if (6 > total_length) {
    errMsg(err) << "Invalid packet: Expected minimum lenght of 6h, got "
                << QString::number(total_length, 16) << "h.";
    return false;
  }
  // wait for remaining packet
  while ((total_length-2) > bytesAvailable()) {
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
  logDebug() << "Got response " << QByteArray((const char *)buffer, total_length).toHex() << ".";

  // check response
  uint16_t responseCommand = qFromBigEndian((*(uint16_t *)(buffer+2)));
  if ((! (responseCommand & 0x8000)) || (command != (responseCommand&0x7fff))) {
    errMsg(err) << "Unexpected response command " << QString::number(responseCommand, 16)
                << "h, expected " << QString::number(command | 0x8000, 16) << "h.";
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
  memcpy(response, buffer+4, rlen);

  // done.
  return true;
}

