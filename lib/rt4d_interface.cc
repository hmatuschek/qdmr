//
// Created by hannes on 22.06.26.
//

#include "rt4d_interface.hh"

#include <qendian.h>

uint8_t _rt4d_crc(uint8_t *data, size_t size) {
  uint8_t crc = 0;
  for (size_t i=0; i<size; i++)
    crc += data[i];
  return crc;
}


/* ********************************************************************************************* *
 * Implementation of RT4DInterface::CommandRequest
 * ********************************************************************************************* */
RT4DInterface::CommandRequest::CommandRequest(Type type, Command command) {
  header1 = '4';
  header2 = 'R';
  this->type = static_cast<uint8_t>(type);
  this->command = static_cast<uint8_t>(command);
  this->crc = 0;
  this->crc = _rt4d_crc(reinterpret_cast<uint8_t *>(this), sizeof(CommandRequest));
}


/* ********************************************************************************************* *
 * Implementation of RT4DInterface::ACKResponse
 * ********************************************************************************************* */
bool
RT4DInterface::ACKResponse::receive(QIODevice *device, const ErrorStack &err) {
  if (1 != device->read(reinterpret_cast<char *>(this), 1)) {
    errMsg(err) << "Cannot read response: " << device->errorString() << ".";
    return false;
  }
  return true;
}


/* ********************************************************************************************* *
 * Implementation of RT4DInterface::ReadRequest
 * ********************************************************************************************* */
RT4DInterface::ReadRequest::ReadRequest(uint16_t address) {
  header = 'R';
  this->address = qToBigEndian(address);
  this->crc = 0;
  this->crc = _rt4d_crc(reinterpret_cast<uint8_t *>(this), sizeof(ReadRequest));
}


/* ********************************************************************************************* *
 * Implementation of RT4DInterface::ReadResponse
 * ********************************************************************************************* */
bool
RT4DInterface::ReadResponse::receive(QIODevice *device, const ErrorStack &err) {
  size_t offset = 0;
  while (offset < sizeof(ReadResponse)) {
    qint64 bytes = device->read(reinterpret_cast<char *>(this) + offset, sizeof(ReadResponse) - offset);
    if (0 > bytes) {
      errMsg(err) << "Cannot read response: " << device->errorString() << ".";
      return false;
    }
    offset += bytes;
  }

  if ('R' != header) {
    errMsg(err) << "Unexpected header: 0x" << Qt::hex << static_cast<int>(header) << ".";
    return false;
  }

  if (! _rt4d_crc(reinterpret_cast<uint8_t *>(this), sizeof(ReadResponse))) {
    errMsg(err) << "CRC mismatch.";
    return false;
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of RT4DInterface::WriteRequest
 * ********************************************************************************************* */
RT4DInterface::WriteRequest::WriteRequest(uint8_t sequence,
                                          uint16_t address,
                                          const uint8_t *payload, int size) {
  header = 9;
  this->sequence = sequence;
  this->address = qToBigEndian(address);
  memcpy(this->payload, payload, std::min(1024, size));
  this->crc = 0;
  this->crc = _rt4d_crc(reinterpret_cast<uint8_t *>(this), sizeof(WriteRequest));
}