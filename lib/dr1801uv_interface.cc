#include "dr1801uv_interface.hh"
#include <QtEndian>
#include "logger.hh"
#include "dr1801uv.hh"

// Identifies itself as "Prolific USB-serial controller" (chip in cable).
#define USB_VID 0x067b
#define USB_PID 0x23a3


/* ********************************************************************************************* *
 * Implementation of DR1801UVInterface::PrepareReadRequest
 * ********************************************************************************************* */
DR1801UVInterface::PrepareReadRequest::PrepareReadRequest(uint32_t baudrate)
  : baudrate(qToBigEndian(baudrate))
{
  // pass...
}

void
DR1801UVInterface::PrepareReadRequest::setBaudrate(uint32_t baudrate) {
  this->baudrate = qToBigEndian(baudrate);
}


/* ********************************************************************************************* *
 * Implementation of DR1801UVInterface::PrepareReadResponse
 * ********************************************************************************************* */
bool
DR1801UVInterface::PrepareReadResponse::isSuccessful() const {
  return 0x01 == success;
}
uint32_t
DR1801UVInterface::PrepareReadResponse::getSize() const {
  return qFromBigEndian(size);
}


/* ********************************************************************************************* *
 * Implementation of DR1801UVInterface
 * ********************************************************************************************* */
DR1801UVInterface::DR1801UVInterface(const USBDeviceDescriptor &descriptor,
                                     const ErrorStack &err, QObject *parent)
  : AuctusA6Interface(descriptor, err, parent), _identifier(), _bytesToRead(0)
{
  if (! enterProgrammingMode(err)) {
    errMsg(err) << "Cannot connect to DR-1801UV.";
    return;
  }

  QString id;
  if (! getDeviceInfo(id, err)) {
    errMsg(err) << "Cannot connect to DR-1801UV.";
    return;
  }

  QStringList lst = id.split(",");
  if (lst.size()<2) {
    errMsg(err) << "Cannot extract device ID from device info '" << id
                << "'. Expected comma-separated list of strings.";
    _state = ERROR;
    return;
  }

  _identifier = lst.at(1);
}

RadioInfo
DR1801UVInterface::identifier(const ErrorStack &err) {
  if ((!isOpen()) || (IDLE != _state) || _identifier.isEmpty())
    return RadioInfo();
  if ("BF1801" == _identifier) {
    return DR1801UV::defaultRadioInfo();
  }

  errMsg(err) << "Unknown device '" << _identifier << "'.";
  return RadioInfo();
}

uint32_t
DR1801UVInterface::bytesToTransfer() const {
  return _bytesToRead;
}

bool
DR1801UVInterface::read_start(uint32_t bank, uint32_t addr, const ErrorStack &err) {
  Q_UNUSED(bank); Q_UNUSED(addr)

  if (! isOpen()) {
    errMsg(err) << "Cannot read codeplug from device: Interface not open.";
    return false;
  }

  if (IDLE != _state) {
    errMsg(err) << "Cannot read codeplug from device: Interface not in idle state. "
                << "State=" << _state << ".";
    return false;
  }

  PrepareReadResponse resp;
  if (! prepareReading(QSerialPort::Baud115200, resp, err)) {
    errMsg(err) << "Cannot start reading the codeplug from " << _identifier << ".";
    _state = ERROR;
    return false;
  }

  _bytesToRead = resp.getSize();
  if (! startReading(err)) {
    errMsg(err) << "Cannot start reading the codeplug form " << _identifier << ".";
    _state = ERROR;
    return false;
  }

  logDebug() << "Start reading " << _bytesToRead << "b of codeplug memory.";

  return true;
}

bool
DR1801UVInterface::read(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes, const ErrorStack &err) {
  // Note: Cannot control where to read. Once started, the device simply sends the entire codpelug.
  Q_UNUSED(bank); Q_UNUSED(addr);

  if ((!isOpen()) || (READ_THROUGH != _state)) {
    errMsg(err) << "Cannot read codeplug from device.";
    return false;
  }

  if (_bytesToRead < nbytes)
    nbytes = _bytesToRead;

  while (nbytes > bytesAvailable()) {
    if (! waitForReadyRead(2000)) {
      errMsg(err) << QSerialPort::errorString();
      errMsg(err) << "Cannot read from device '" << portName() << "'.";
      return false;
    }
  }
  int n = QSerialPort::read((char *)data, nbytes);
  if (0 > n) {
    errMsg(err) << "Cannot read from serial port: " << QSerialPort::errorString() << ".";
    return false;
  }

  _bytesToRead -= n;
  if (0 == _bytesToRead)
    _state = IDLE;

  return true;
}

bool
DR1801UVInterface::read_finish(const ErrorStack &err) {

  return (_state == IDLE) && (0 == _bytesToRead);
}


bool
DR1801UVInterface::write_start(uint32_t bank, uint32_t addr, const ErrorStack &err) {

}

bool
DR1801UVInterface::write(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes, const ErrorStack &err) {

}

bool
DR1801UVInterface::write_finish(const ErrorStack &err) {

}

USBDeviceInfo
DR1801UVInterface::interfaceInfo() {
  return USBDeviceInfo(USBDeviceInfo::Class::Serial, USB_VID, USB_PID, false, true);
}

QList<USBDeviceDescriptor>
DR1801UVInterface::detect() {
  return USBSerial::detect(USB_VID, USB_PID);
}

bool
DR1801UVInterface::getDeviceInfo(QString &info, const ErrorStack &err) {
  uint8_t response[255], respLen = 255;
  if (! send_receive(REQUEST_INFO, nullptr, 0, response, respLen, err)) {
    errMsg(err) << "Cannot request device information.";
    _state = ERROR;
    return false;
  }

  // check status byte
  if (1 > respLen) {
    errMsg(err) << "Invalid response lenght.";
    errMsg(err) << "Cannot request device information.";
    _state = ERROR;
    return false;
  }

  if (0x01 != response[0]) {
    errMsg(err) << "Device returned no success. Expected 01h, got "
                << QString::number(response[0],16) << "h.";
    errMsg(err) << "Cannot request device information.";
    _state = ERROR;
    return false;
  }

  if (1 < respLen) {
    logDebug() << QByteArray((const char *)response+1, respLen-1);
    info = QString::fromLatin1((char *)(response+1), respLen-1);
  }

  return true;
}

bool
DR1801UVInterface::enterProgrammingMode(const ErrorStack &err) {
  uint8_t response[255], respLen=255;

  if (! send_receive(ENTER_PROGRAMMING_MODE, nullptr, 0, response, respLen, err)) {
    errMsg(err) << "Cannot enter programming mode.";
    _state = ERROR;
    return false;
  }

  if ((1 != respLen) || (0x01 != response[0])) {
    errMsg(err) << "Unexpected response from radio. Expected 01h, got "
                << QString::number(response[0], 16) << "h.";
    errMsg(err) << "Cannot enter programming mode.";
    return false;
  }

  return true;
}

bool
DR1801UVInterface::checkProgrammingPassword(const ErrorStack &err) {
  // No argument -> no password
  uint8_t request[] = {0x00}, reqLen=sizeof(request),
      response[255], respLen=255;

  if (! send_receive(CHECK_PROG_PASSWORD, request, reqLen, response, respLen, err)) {
    errMsg(err) << "Cannot enter programming mode.";
    _state = ERROR;
    return false;
  }

  if ((1 != respLen) || (0x02 != response[0])) {
    errMsg(err) << "Unexpected response from radio. Expected 02h, got "
                << QString::number(response[0], 16) << "h.";
    errMsg(err) << "Cannot enter programming mode.";
    return false;
  }

  return true;
}

bool
DR1801UVInterface::prepareReading(uint32_t baudrate, PrepareReadResponse &response, const ErrorStack &err) {
  PrepareReadRequest request(baudrate);
  uint8_t respSize = sizeof(response);

  if (! send_receive(PREPARE_CODEPLUG_READ, (uint8_t *)&request, sizeof(request), (uint8_t *)&response, respSize, err)) {
    errMsg(err) << "Cannot prepare reading of codeplug.";
    return false;
  }

  if ((sizeof(PrepareReadResponse) != respSize) || (! response.isSuccessful())) {
    errMsg(err) << "Prepare reading of codeplug failed.";
    return false;
  }

  if (! this->setBaudRate(baudrate)) {
    errMsg(err) << "Cannot set baud-rate of serial port '" << portName() << "'.";
    return false;
  }

  return true;
}

bool
DR1801UVInterface::startReading(const ErrorStack &err) {
  uint8_t cmd[6] = {0xaa, 0x06, (START_READ_DATA>>8), START_READ_DATA&0xff, 0x06, 0xbb};

  logDebug() << "Send "  << QByteArray((const char *)cmd, 6).toHex() << ".";
  if (6 != QSerialPort::write((const char *)cmd, 6)) {
    errMsg(err) << QSerialPort::errorString();
    errMsg(err) << "Cannot initalize codeplug read.";
    return false;
  }
  flush();

 _state = READ_THROUGH;
 return true;
}
