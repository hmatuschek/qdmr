#include "dm32uv_interface.hh"
#include "logger.hh"
#include <QtEndian>

#define USB_VID 0x067b
#define USB_PID 0x23a3

#define TIMEOUT 500


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
  if (0x50 != this->code) {
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

  if (0x06 == result) {
    errMsg(err) << "Unexpected response " << Qt::hex << result << "h, expected 6h.";
    return false;
  }

  return true;
}


/* ********************************************************************************************* *
 * Value Request
 * ********************************************************************************************* */
DM32UVInterface::ValueRequest::ValueRequest(ValueId vId)
  : request_type('V'), unused{0,0,0}, valueId((uint8_t)vId)
{
  // pass...
}

DM32UVInterface::ValueRequest::ValueRequest(uint8_t vId)
  : request_type('V'), unused{0,0,0}, valueId(vId)
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

  if (! dev->receive(data, 4, TIMEOUT, err))
    return false;
  data += 4;

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
    errMsg(err) << "Unexpected response " << Qt::hex << this->response_type
                << "h, expected " << Qt::hex << 'W' << "h.";
    return false;
  }
  data += 1;

  if (! dev->receive(data, 4, TIMEOUT, err))
    return false;
  data += 4;

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

  // Map entire codeplug memory region
  for (uint32_t addr=_codeplugMemory.first; addr<_codeplugMemory.second; addr += 0x1000) {
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
    if ((0x00 == prefix) || (0xff == prefix))
      continue;
    // add to address map
    uint32_t vaddr = ((uint32_t)prefix) << 12;
    map.map(addr, vaddr);
  }

  return true;
}


USBDeviceInfo
DM32UVInterface::interfaceInfo() {
  return USBDeviceInfo(USBDeviceInfo::Class::Serial, USB_VID, USB_PID, false);
}

QList<USBDeviceDescriptor>
DM32UVInterface::detect(bool saveOnly) {
  if (saveOnly)
    return QList<USBDeviceDescriptor>();
  return USBSerial::detect(USB_VID, USB_PID, false);
}


bool
DM32UVInterface::DM32UVInterface::request_identifier(const ErrorStack &err) {
  if (State::Open != _state) {
    errMsg(err) << "Cannot identify radio, interface not it OPEN state.";
    return false;
  }

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

  // Formally, request password
  PasswordRequest passReq;
  PasswordResponse passRes;
  if (! sendReceive(passReq, passRes, err)) {
    errMsg(err) << "Cannot request programming password.";
    return false;
  }

  // Enter System information mode
  SysinfoRequest sysInfoReq;
  ACKResponse sysInfoRes;
  if (! sendReceive(sysInfoReq, sysInfoRes, err)) {
    errMsg(err) << "Cannot enter system info mode.";
    return false;
  }

  // Request FW version
  ValueRequest fwVersionReq(ValueRequest::ValueId::FirmwareVersion);
  ValueResponse fwVersionRes;
  if (! sendReceive(fwVersionReq, fwVersionRes, err)) {
    errMsg(err) << "Cannot request firmware version.";
    return false;
  }
  _firmwareVersion = fwVersionRes.string();
  logDebug() << "Got firmware version " << _firmwareVersion << ".";

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
