//
// Created by hannes on 22.06.26.
//

#include "rt4d_interface.hh"

#include <QThread>

#include "logger.hh"
#include <qendian.h>

#define TIMEOUT 1000


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

bool
RT4DInterface::CommandRequest::send(RT4DInterface *device, const ErrorStack &err) const {
  //logDebug() << "Send " << QByteArray((const char *)this, sizeof(RT4DInterface::CommandRequest)).toHex(' ') << ".";
  return device->send(
    reinterpret_cast<const char *>(this), sizeof(CommandRequest),TIMEOUT, err);
}


/* ********************************************************************************************* *
 * Implementation of RT4DInterface::ACKResponse
 * ********************************************************************************************* */
bool
RT4DInterface::ACKResponse::receive(RT4DInterface *device, const ErrorStack &err) {
  if (! device->receive(reinterpret_cast<char *>(this), 1, TIMEOUT, err)) {
    errMsg(err) << "Cannot read response: " << device->errorString() << ".";
    return false;
  }
  if (0x06 != header) {
    errMsg(err) << "Unexpected response " << Qt::hex << static_cast<int>(header) << ".";
    return false;
  }
  return true;
}


/* ********************************************************************************************* *
 * Implementation of RT4DInterface::ReadRequest
 * ********************************************************************************************* */
RT4DInterface::ReadRequest::ReadRequest(uint32_t address) {
  header = 'R';
  this->page = qToBigEndian(static_cast<uint16_t>(address>>10));
  this->crc = 0;
  this->crc = _rt4d_crc(reinterpret_cast<uint8_t *>(this), sizeof(ReadRequest));
}

bool
RT4DInterface::ReadRequest::send(RT4DInterface *device, const ErrorStack &err) const {
  //logDebug() << "Send " << QByteArray((const char *)this, sizeof(RT4DInterface::ReadRequest)).toHex(' ') << ".";
  return device->send(
      reinterpret_cast<const char *>(this), sizeof(ReadRequest),
      TIMEOUT, err);
}


/* ********************************************************************************************* *
 * Implementation of RT4DInterface::ReadResponse
 * ********************************************************************************************* */
bool
RT4DInterface::ReadResponse::receive(RT4DInterface *device, const ErrorStack &err) {
  if (! device->receive(reinterpret_cast<char *>(this), sizeof(ReadResponse),TIMEOUT, err))
    return false;

  if ('R' != header) {
    errMsg(err) << "Unexpected header: 0x" << Qt::hex << static_cast<int>(header) << ".";
    return false;
  }

  if (crc != _rt4d_crc(reinterpret_cast<uint8_t *>(this), sizeof(ReadResponse)-1)) {
    errMsg(err) << "CRC mismatch.";
    return false;
  }

  return true;
}

uint32_t
RT4DInterface::ReadResponse::address() const {
  return static_cast<uint32_t>(page) << 10;
}



/* ********************************************************************************************* *
 * Implementation of RT4DInterface::WriteRequest
 * ********************************************************************************************* */
RT4DInterface::WriteRequest::WriteRequest(uint8_t sequence,
                                          uint32_t offset,
                                          const uint8_t *payload, int size) {
  header = 9;
  this->segment = sequence;
  this->offset_page = qToBigEndian(static_cast<uint16_t>(offset>>10));
  memset(this->payload, 0, 1024);
  memcpy(this->payload, payload, std::min(1024, size));
  this->crc = 0;
  this->crc = _rt4d_crc(reinterpret_cast<uint8_t *>(this), sizeof(WriteRequest));
}

bool
RT4DInterface::WriteRequest::send(RT4DInterface *device, const ErrorStack &err) const {
  return device->send(
      reinterpret_cast<const char *>(this), sizeof(WriteRequest), TIMEOUT, err);
}



/* ********************************************************************************************* *
 * Implementation of RT4DInterface
 * ********************************************************************************************* */
RT4DInterface::RT4DInterface(const USBDeviceDescriptor &descr, const ErrorStack &err, QObject *parent)
  : USBSerial{descr, QSerialPort::Baud115200, err, parent}, _state(State::Closed), _info()
{
  if (USBSerial::isOpen()) {
    _state = State::Open;
    setFlowControl(QSerialPort::NoFlowControl);
    setRequestToSend(false);
    setDataTerminalReady(true);
  } else if (QSerialPort::NoError != QSerialPort::error()) {
    _state = State::Error;
  }

  if (! request_identifier(err))
    _state = State::Error;
}

RadioInfo
RT4DInterface::identifier(const ErrorStack &err) {
  // If not yet requested -> request info
  if ((State::Open == _state) && (! _info.isValid())) {
    if (! request_identifier(err))
      _state = State::Error;
  }
  // return it.
  return _info;
}


void
RT4DInterface::close() {
  if (! isOpen())
    return;
  if (State::Connected == _state || State::Identified == _state) {
    CommandRequest command(CommandRequest::Type::Command, CommandRequest::Command::Reboot);
    command.send(this);
  }

  // No explicit "exit programming mode" command exists in the protocol.
  // Cycle DTR low so the radio's USB-serial adapter resets the device.
  setDataTerminalReady(false);
  QThread::msleep(500);

  USBSerial::close();
  _state = State::Closed;
}

const QVector<QPair<uint32_t, uint32_t>> RT4DInterface::_segmentMap  = {
  {0x02000, 0x00400},  // Settings 1, seg 0
  {0x04000, 0x0c000},  // Channels, seg 1
  {0x1c000, 0x00400},  // Settings 2, seg 2
  {0x1e000, 0x20000},  // Zones, seg 3,
  {0x5e000, 0x34000},  // Contacts, seg 4
  {0xc6000, 0x05000},  // Group lists, seg 5
  {0xd0000, 0x03000},  // Keys, seg 6
  {0xd6000, 0x01000},  // Messages, seg 7
  {0xf0000, 0x01000},  // FM broadcast channels, seg 7
};

QPair<int, uint32_t>
RT4DInterface::mapToSegment(uint32_t address) {
  for (qsizetype seg=0; seg<_segmentMap.count(); seg++) {
    if (address >= _segmentMap[seg].first && address < (_segmentMap[seg].first + _segmentMap[seg].second)) {
      return {seg, address-_segmentMap[seg].first};
    }
  }
  return {-1, 0};
}


bool
RT4DInterface::read_start(uint32_t bank, uint32_t address, const ErrorStack &err) {
  Q_UNUSED(bank); Q_UNUSED(address);

  // If not yet in program mode -> enter
  if ((State::Identified != _state) && (! request_identifier(err))) {
    errMsg(err) << "Cannot enter program mode.";
    _state = State::Error;
    return false;
  }

  return true;
}


bool
RT4DInterface::read(uint32_t bank, uint32_t address, uint8_t *data, int nbytes, const ErrorStack &err) {
  Q_UNUSED(bank);

  // Read request must align with 0x400
  if (nbytes != 0x400) {
    errMsg(err) << "Can only read 1024b at once.";
    return false;
  }

  ReadRequest req(address);
  ReadResponse res;
  if (! sendReceive(req, res, err)) {
    errMsg(err) << "Cannot read from " << Qt::hex << address << "h " << 1024 << " bytes.";
    return false;
  }
  std::memcpy(data, res.payload, nbytes);

  return true;
}


bool
RT4DInterface::read_finish(const ErrorStack &err) {
  Q_UNUSED(err)
  // Nothing to do.
  return true;
}


bool
RT4DInterface::write_start(uint32_t bank, uint32_t address, const ErrorStack &err) {
  Q_UNUSED(bank); Q_UNUSED(address);

  // If not yet in program mode -> enter
  if ((State::Identified != _state) && (! request_identifier(err))) {
    errMsg(err) << "Cannot enter program mode.";
    _state = State::Error;
    return false;
  }

  return true;
}


bool
RT4DInterface::write(uint32_t bank, uint32_t address, uint8_t *data, int nbytes, const ErrorStack &err) {
  Q_UNUSED(bank);

  // Read request must align with 0x400
  if (nbytes != 0x400) {
    errMsg(err) << "Can only write 1024b at once.";
    return false;
  }

  auto segment = mapToSegment(address);
  if (0 > segment.first) {
    errMsg(err) << "Cannot write to address " << Qt::hex << address << "h : Not mapped.";
    return false;
  }

  WriteRequest req(segment.first, segment.second, data, nbytes);
  ACKResponse res;
  if (! sendReceive(req, res, err)) {
    errMsg(err) << "Cannot write to " << Qt::hex << address << "h " << nbytes << " bytes.";
    return false;
  }

  return true;
}


bool
RT4DInterface::write_finish(const ErrorStack &err) {
  Q_UNUSED(err)
  // Nothing to do.
  return true;
}


USBDeviceInfo
RT4DInterface::interfaceInfo() {
  return USBDeviceInfo(USBDeviceInfo::Class::Serial, 0, 0, false);
}

QList<USBDeviceDescriptor>
RT4DInterface::detect(bool saveOnly) {
  if (saveOnly)
    return QList<USBDeviceDescriptor>();
  return USBSerial::detect();
}

bool
RT4DInterface::enter_program_mode(const ErrorStack &err) {
  if ((State::Connected == _state) || (State::Identified == _state))
    return true;

  if (State::Open != _state) {
    errMsg(err) << "Cannot enter PROGRAM mode. Interface not not open.";
    return false;
  }

  CommandRequest progReq(CommandRequest::Type::Command, CommandRequest::Command::ProgMode);
  ACKResponse progRes;
  if (! sendReceive(progReq, progRes, err)) {
    errMsg(err) << "Cannot send enter-program-mode request.";
    return false;
  }

  _state = State::Connected;

  return true;
}

bool
RT4DInterface::request_identifier(const ErrorStack &err) {
  if (_state == State::Identified)
    return true;

  if ((_state == State::Open) && (! enter_program_mode(err))) {
    errMsg(err) << "Cannot request identification.";
    return false;
  }

  if (_state != State::Connected)
    return false;

  ReadRequest req(0x002000);
  ReadResponse res;
  if (! sendReceive(req, res, err)) {
    errMsg(err) << "Cannot request identification.";
    return false;
  }
  logDebug() << "Got blob: " << QByteArray((const char*)res.payload, 0x400).toHex();
  uint16_t magic = qFromLittleEndian(*reinterpret_cast<uint16_t*>(res.payload+12));
  if (0xabcd != magic) {
    errMsg(err) << "Unknown device or firmware version: " << magic;
    return false;
  }

  _state = State::Identified;

  _info = RadioInfo::byID(RadioInfo::RT4D);
  return true;
}


bool
RT4DInterface::send(const char *data, qint64 n, int timeout, const ErrorStack &err) {
  //logDebug() << "Send " << QByteArray(data, n).toHex(' ') << ".";
  while (n) {
    auto k = QSerialPort::write(data, n);
    if (0 > k) {
      errMsg(err) << "QSerialPort: " << errorString();
      errMsg(err) << "Cannot send device detection request.";
      return false;
    }
    n -= k; data += k;
  }

  if (! waitForBytesWritten(timeout)) {
    errMsg(err) << "QSerialPort: " << errorString();
    errMsg(err) << "Timed out waiting for bytes to be written.";
    return false;
  }

  return true;
}


bool
RT4DInterface::receive(char *data, qint64 n, int timeout, const ErrorStack &err) {
  if (bytesAvailable()) {
    auto k = QSerialPort::read(data, n);
    if (0 > k) {
      errMsg(err) << "Cannot read from serial port: " << errorString() << ".";
      return false;
    }
    n -= k; data += k;
  }

  while (n) {
    if (! waitForReadyRead(timeout)) {
      errMsg(err) << "QSerialPort: " << errorString() << ".";
      errMsg(err) << "Cannot read from serial port, timeout.";
      return false;
    }
    auto k = QSerialPort::read(data, n);
    if (0 > k) {
      errMsg(err) << "Cannot read from serial port: " << errorString() << ".";
      return false;
    }
    n -= k; data += k;
  }

  return true;
}
