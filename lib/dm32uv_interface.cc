#include "dm32uv_interface.hh"
#include "logger.hh"
#include <QtEndian>

#define USB_VID 0x067b
#define USB_PID 0x23a3

#define TIMEOUT 1000


/* ********************************************************************************************* *
 * Device detection request
 * ********************************************************************************************* */
bool
DM32UVInterface::DeviceDetectionRequest::send(DM32UVInterface *dev, const ErrorStack &err) const {
  return dev->send((const char *)this, sizeof(DeviceDetectionRequest), TIMEOUT, err);
}


/* ********************************************************************************************* *
 * Device detection response
 * ********************************************************************************************* */
bool
DM32UVInterface::DeviceDetectionResponse::receive(DM32UVInterface *dev, const ErrorStack &err) {
  quint64 n = sizeof(DeviceDetectionResponse); char *data = (char *)this;
  if (! dev->receive(data, 1, TIMEOUT, err)) {
    errMsg(err) << "Cannot read response.";
    return false;
  }
  n -= 1; data += 1;

  if (6 != this->result) {
    errMsg(err) << "Invalid response. Expected 6, got " << Qt::hex << this->result << ".";
    return false;
  }

  return dev->receive(data, n, TIMEOUT, err);
}

QString
DM32UVInterface::DeviceDetectionResponse::identifier() const {
  return QString::fromLatin1(QByteArray(payload, 7));
}


/* ********************************************************************************************* *
 * Password request
 * ********************************************************************************************* */
bool
DM32UVInterface::PasswordRequest::send(DM32UVInterface *dev, const ErrorStack &err) const {
  return dev->send((const char *)this, sizeof(PasswordRequest), TIMEOUT, err);
}

/* ********************************************************************************************* *
 * Password response
 * ********************************************************************************************* */
bool
DM32UVInterface::PasswordResponse::receive(DM32UVInterface *dev, const ErrorStack &err) {
  qint64 n = sizeof(PasswordResponse); char *data = (char *)this;
  if (! dev->receive(data, 1, TIMEOUT, err))
    return false;

  n -= 1; data += 1;
  if (0x50 != this->result) {
    errMsg(err) << "Invalid response. Expected 50h, got " << Qt::hex << this->code << "h.";
    return false;
  }

  return dev->receive(data, n, TIMEOUT, err);
}


/* ********************************************************************************************* *
 * System Info Request
 * ********************************************************************************************* */
bool
DM32UVInterface::SysinfoRequest::send(DM32UVInterface *dev, const ErrorStack &err) const {
  return dev->send((const char *)this, sizeof(SysinfoRequest), TIMEOUT, err);
}

/* ********************************************************************************************* *
 * Generic ACK Response
 * ********************************************************************************************* */
bool
DM32UVInterface::ACKResponse::receive(DM32UVInterface *dev, const ErrorStack &err) {
  if (! dev->receive((char *)this, 1, TIMEOUT, err))
    return false;

  if (0x06 != result) {
    errMsg(err) << "Unexpected response " << Qt::hex << result << "h, expected 6h.";
    return false;
  }

  return true;
}


/* ********************************************************************************************* *
 * Value Request
 * ********************************************************************************************* */
DM32UVInterface::ValueRequest::ValueRequest(ValueId vId)
  : valueId((uint8_t)vId)
{
  // pass...
}

DM32UVInterface::ValueRequest::ValueRequest(uint8_t vId)
  : valueId(vId)
{
  // pass...
}

bool
DM32UVInterface::ValueRequest::send(DM32UVInterface *dev, const ErrorStack &err) const {
  return dev->send((const char *)this, sizeof(ValueRequest), TIMEOUT, err);
}

/* ********************************************************************************************* *
 * Value Response
 * ********************************************************************************************* */
bool
DM32UVInterface::ValueResponse::receive(DM32UVInterface *dev, const ErrorStack &err) {
  char *data = (char *)this;
  if (! dev->receive(data, 1, TIMEOUT, err))
    return false;

  if ('V' != this->response_type) {
    errMsg(err) << "Unexpected response " << Qt::hex << this->response_type
                << "h, expected " << Qt::hex << 'V';
    return false;
  }
  data += 1;

  // Receive value ID and length
  if (! dev->receive(data, 2, TIMEOUT, err))
    return false;
  data += 2;

  return dev->receive(data, this->length, TIMEOUT, err);
}

QString
DM32UVInterface::ValueResponse::string() const {
  return QString::fromLatin1(QByteArray((const char *)payload,length));
}

uint32_t
DM32UVInterface::ValueResponse::lowerMemoryBound() const {
  return qFromLittleEndian(memory.lower);
}

uint32_t
DM32UVInterface::ValueResponse::upperMemoryBound() const {
  return qFromLittleEndian(memory.upper);
}


/* ********************************************************************************************* *
 * Enter Program Mode Request
 * ********************************************************************************************* */
bool
DM32UVInterface::EnterProgramModeRequest::send(DM32UVInterface *dev, const ErrorStack &err) const {
  return dev->send((const char *)this, sizeof(EnterProgramModeRequest), TIMEOUT, err);
}


/* ********************************************************************************************* *
 * Unknown 02h Request
 * ********************************************************************************************* */
bool
DM32UVInterface::Unknown02Request::send(DM32UVInterface *dev, const ErrorStack &err) const {
  return dev->send((const char *)this, sizeof(Unknown02Request), TIMEOUT, err);
}

/* ********************************************************************************************* *
 * Unknown 02h Response
 * ********************************************************************************************* */
bool
DM32UVInterface::Unknown02Response::receive(DM32UVInterface *dev, const ErrorStack &err) {
  return dev->receive((char *)this, sizeof(Unknown02Response), TIMEOUT, err);
}


/* ********************************************************************************************* *
 * Ping Request
 * ********************************************************************************************* */
bool
DM32UVInterface::PingRequest::send(DM32UVInterface *dev, const ErrorStack &err) const {
  return dev->send((const char *)this, sizeof(PingRequest), TIMEOUT, err);
}


/* ********************************************************************************************* *
 * Read Request
 * ********************************************************************************************* */
DM32UVInterface::ReadRequest::ReadRequest(uint32_t addr, uint16_t len)
{
  address[0] = (addr >>  0) & 0xff;
  address[1] = (addr >>  8) & 0xff;
  address[2] = (addr >> 16) & 0xff;
  length = qToLittleEndian(len);
}

bool
DM32UVInterface::ReadRequest::send(DM32UVInterface *dev, const ErrorStack &err) const {
  return dev->send((const char *)this, sizeof(ReadRequest), TIMEOUT, err);
}

/* ********************************************************************************************* *
 * Read Response
 * ********************************************************************************************* */
bool
DM32UVInterface::ReadResponse::receive(DM32UVInterface *dev, const ErrorStack &err) {
  char *data = (char *)this;

  if (! dev->receive(data, 1, TIMEOUT, err))
    return false;
  if ('W' != this->response_type) {
    errMsg(err) << "Unexpected response " << Qt::hex << (uint8_t)this->response_type
                << "h, expected " << Qt::hex << (uint8_t)'W' << "h.";
    return false;
  }
  data += 1;

  // Receive address (24bit) and length (16bit)
  if (! dev->receive(data, 5, TIMEOUT, err))
    return false;
  data += 5;
  // Receive payload
  return dev->receive(data, this->length(), TIMEOUT, err);
}

uint32_t
DM32UVInterface::ReadResponse::address() const {
  return (((uint32_t)_address[0]) << 0)
         | (((uint32_t)_address[1]) << 8)
         | (((uint32_t)_address[2]) << 16);
}

uint16_t
DM32UVInterface::ReadResponse::length() const {
  return qFromLittleEndian(_length);
}

QByteArray
DM32UVInterface::ReadResponse::payload() const {
  return QByteArray((const char*)_payload, length());
}


/* ********************************************************************************************* *
 * Write request
 * ********************************************************************************************* */
DM32UVInterface::WriteRequest::WriteRequest(uint32_t address, const QByteArray &payload) {
  _address[0] = ((address >>  0) & 0xff);
  _address[1] = ((address >>  8) & 0xff);
  _address[2] = ((address >> 16) & 0xff);
  uint16_t len = std::min(4096LL, payload.length());
  _length = qToLittleEndian(len);
  memcpy(_payload, payload.constData(), len);
}

bool
DM32UVInterface::WriteRequest::send(DM32UVInterface *dev, const ErrorStack &err) const {
  return dev->send((const char *)this, sizeof(EnterProgramModeRequest), TIMEOUT, err);
}



/* ********************************************************************************************* *
 * Implementation of the DM-32UV interface.
 * ********************************************************************************************* */
DM32UVInterface::DM32UVInterface(const USBDeviceDescriptor &descr,
                                 const ErrorStack &err, QObject *parent)
  : USBSerial{descr, QSerialPort::Baud115200, err, parent}, _state(State::Closed), _info()
{
  if (isOpen())
    _state = State::Open;
  else if (QSerialPort::NoError != QSerialPort::error())
    _state = State::Error;

  if (! request_identifier(err))
    _state = State::Error;
  _state = State::SystemInfo;
}


RadioInfo
DM32UVInterface::identifier(const ErrorStack &err) {
  // If not yet requested -> request info
  if ((State::Open == _state) && (! _info.isValid())) {
    if (! request_identifier(err))
      _state = State::Error;
    _state = State::SystemInfo;
  }
  // return it.
  return _info;
}


bool
DM32UVInterface::getAddressMap(DM32UV::AddressMap &map, const ErrorStack &err) {
  // If not yet in program mode -> enter
  if ((State::Program != _state) && (! enter_program_mode(err))) {
    errMsg(err) << "Cannot enter program mode.";
    _state = State::Error;
    return false;
  }
  _state = State::Program;
  logDebug() << "Enter PROG mode.";

  // Map entire codeplug memory region
  for (uint32_t addr=_codeplugMemory.first; addr<_codeplugMemory.second; addr += 0x1000) {
    QThread::msleep(100);
    ReadRequest mapReq(addr+0xfff, 1);
    ReadResponse mapRes;
    if (! sendReceive(mapReq, mapRes, err)) {
      errMsg(err) << "Cannot request codeplug memory map at address "
                  << Qt::hex << (addr+0xfff) << "h.";
      return false;
    }
    // Unpack prefix
    uint8_t prefix = (uint8_t)mapRes.payload()[0];
    // If prefix is invalid -> do not map
    if ((0x00 == prefix) || (0xff == prefix)) {
      logDebug() << "Address " << Qt::hex << addr << "h unallocated.";
      continue;
    }
    // add to address map
    uint32_t vaddr = ((uint32_t)prefix) << 12;
    logDebug() << "Map " << Qt::hex << addr << "h -> " << vaddr << "h.";
    map.map(addr, vaddr);
  }

  return true;
}



bool
DM32UVInterface::read_start(uint32_t bank, uint32_t address, const ErrorStack &err) {
  Q_UNUSED(bank); Q_UNUSED(address);

  // If not yet in program mode -> enter
  if ((State::Program != _state) && (! enter_program_mode(err))) {
    errMsg(err) << "Cannot enter program mode.";
    _state = State::Error;
    return false;
  }

  _state = State::Program;
  return true;
}


bool
DM32UVInterface::read(uint32_t bank, uint32_t address, uint8_t *data, int nbytes, const ErrorStack &err) {
  Q_UNUSED(bank);

  // align read with 1000h blocks
  if (address & 0xfff) {
    int n = std::min(nbytes, (int)(0x1000 - (address & 0xfff)));
    ReadRequest req(address, n);
    ReadResponse res;
    if (! sendReceive(req, res, err)) {
      errMsg(err) << "Cannot read from " << Qt::hex << address << "h " << n << " bytes.";
      return false;
    }
    std::memcpy(data, res.payload().constData(), n);
    data += n;
    address += n;
    nbytes -= n;
  }

  while (nbytes > 0) {
    int n = std::min(nbytes, 0x1000);
    ReadRequest req(address, n);
    ReadResponse res;
    if (! sendReceive(req, res, err)) {
      errMsg(err) << "Cannot read from " << Qt::hex << address << "h " << n << " bytes.";
      return false;
    }
    std::memcpy(data, res.payload().constData(), n);
    data += n;
    address += n;
    nbytes -= n;
  }

  return true;
}


bool
DM32UVInterface::read_finish(const ErrorStack &err) {
  Q_UNUSED(err)
  // Nothing to do.
  return true;
}


bool
DM32UVInterface::write_start(uint32_t bank, uint32_t address, const ErrorStack &err) {
  Q_UNUSED(bank); Q_UNUSED(address);

  // If not yet in program mode -> enter
  if ((State::Program != _state) && (! enter_program_mode(err))) {
    errMsg(err) << "Cannot enter program mode.";
    _state = State::Error;
    return false;
  }

  _state = State::Program;
  return true;
}


bool
DM32UVInterface::write(uint32_t bank, uint32_t address, uint8_t *data, int nbytes, const ErrorStack &err) {
  Q_UNUSED(bank);

  // align read with 1000h blocks
  if (address & 0xfff) {
    int n = std::min(nbytes, (int)(0x1000 - (address & 0xfff)));
    WriteRequest req(address, QByteArray((const char*)data, n));
    ACKResponse res;
    if (! sendReceive(req, res, err)) {
      errMsg(err) << "Cannot write to " << Qt::hex << address << "h " << n << " bytes.";
      return false;
    }
    data += n;
    address += n;
    nbytes -= n;
  }

  while (nbytes > 0) {
    int n = std::min(nbytes, 0x1000);
    WriteRequest req(address, QByteArray((const char*)data, n));
    ACKResponse res;
    if (! sendReceive(req, res, err)) {
      errMsg(err) << "Cannot write to " << Qt::hex << address << "h " << n << " bytes.";
      return false;
    }
    data += n;
    address += n;
    nbytes -= n;
  }

  return true;
}


bool
DM32UVInterface::write_finish(const ErrorStack &err) {
  Q_UNUSED(err)
  // Nothing to do.
  return true;
}


USBDeviceInfo
DM32UVInterface::interfaceInfo() {
  return USBDeviceInfo(USBDeviceInfo::Class::Serial, 0, 0, false);
}

QList<USBDeviceDescriptor>
DM32UVInterface::detect(bool saveOnly) {
  if (saveOnly)
    return QList<USBDeviceDescriptor>();
  return USBSerial::detect();
}


bool
DM32UVInterface::DM32UVInterface::request_identifier(const ErrorStack &err) {
  if (State::Open != _state) {
    errMsg(err) << "Cannot identify radio, interface not it OPEN state.";
    return false;
  }

  QThread::msleep(100);
  DeviceDetectionRequest devDetReq;
  DeviceDetectionResponse devDetRes;
  if (! sendReceive(devDetReq, devDetRes, err)) {
    errMsg(err) << "Cannot identify device.";
    return false;
  }

  logDebug() << "Got " << devDetRes.identifier() << ".";
  if ("DP570UV" != devDetRes.identifier()) {
    errMsg(err) << "Unknown radio " << devDetRes.identifier();
    return false;
  }

  QThread::msleep(100);
  // Formally, request password
  PasswordRequest passReq;
  PasswordResponse passRes;
  if (! sendReceive(passReq, passRes, err)) {
    errMsg(err) << "Cannot request programming password.";
    return false;
  }
  logDebug() << "Finished password request.";

  QThread::msleep(100);
  // Enter System information mode
  SysinfoRequest sysInfoReq;
  ACKResponse sysInfoRes;
  if (! sendReceive(sysInfoReq, sysInfoRes, err)) {
    errMsg(err) << "Cannot enter system info mode.";
    return false;
  }
  logDebug() << "Finished SysInfo request.";

  QThread::msleep(100);
  // Request FW version
  ValueRequest fwVersionReq(ValueRequest::ValueId::FirmwareVersion);
  ValueResponse fwVersionRes;
  if (! sendReceive(fwVersionReq, fwVersionRes, err)) {
    errMsg(err) << "Cannot request firmware version.";
    return false;
  }
  _firmwareVersion = fwVersionRes.string();
  logDebug() << "Got firmware version " << _firmwareVersion << ".";

  QThread::msleep(100);
  // Request codeplug memory region
  ValueRequest cpMemReq(ValueRequest::ValueId::MainConfigMemory);
  ValueResponse cpMemRes;
  if (! sendReceive(cpMemReq, cpMemRes, err)) {
    errMsg(err) << "Cannot request codeplug memory region.";
    return false;
  }
  _codeplugMemory = {cpMemRes.lowerMemoryBound(), cpMemRes.upperMemoryBound()};
  logDebug() << "Codeplug memory region: " << Qt::hex << _codeplugMemory.first
             << "h - " << Qt::hex << _codeplugMemory.second << "h.";

  QThread::msleep(100);
  // Request callsign memory region
  ValueRequest callMemReq(ValueRequest::ValueId::CallSignDBMemory);
  ValueResponse callMemRes;
  if (! sendReceive(callMemReq, callMemRes, err)) {
    errMsg(err) << "Cannot request callsign memory region.";
    return false;
  }
  _callsignMemory = {callMemRes.lowerMemoryBound(), callMemRes.upperMemoryBound()};
  logDebug() << "Call-sign DB memory region: " << Qt::hex << _callsignMemory.first
             << "h - " << Qt::hex << _callsignMemory.second << "h.";

  // Done
  _info = RadioInfo::byID(RadioInfo::Radio::DM32UV);

  return true;
}


bool
DM32UVInterface::enter_program_mode(const ErrorStack &err) {
  if (State::SystemInfo != _state) {
    errMsg(err) << "Cannot enter PROGRAM mode. Interface not in SYSINFO mode.";
    return false;
  }

  EnterProgramModeRequest progReq;
  ACKResponse progRes;
  if (! sendReceive(progReq, progRes, err)) {
    errMsg(err) << "Cannot send enter-program-mode request.";
    return false;
  }

  Unknown02Request uknReq;
  Unknown02Response uknRes;
  if (! sendReceive(uknReq, uknRes, err)) {
    errMsg(err) << "Cannot send unknown 02h request.";
    return false;
  }

  PingRequest pingReq;
  ACKResponse pingRes;
  if (! sendReceive(pingReq, pingRes)) {
    errMsg(err) << "Cannot send ping request.";
    return false;
  }

  return true;
}


bool
DM32UVInterface::send(const char *data, qint64 n, int timeout, const ErrorStack &err) {
  Q_UNUSED(timeout)

  logDebug() << "Send " << QByteArray(data, n).toHex(' ') << ".";
  while (n) {
    auto k = QSerialPort::write(data, n);
    if (0 > k) {
      errMsg(err) << "QSerialPort: " << errorString();
      errMsg(err) << "Cannot send device detection request.";
      return false;
    }
    n -= k; data += k;
  }

  return true;
}


bool
DM32UVInterface::receive(char *data, qint64 n, int timeout, const ErrorStack &err) {
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
