#include "dr1801uv_interface.hh"
#include <QtEndian>
#include "logger.hh"
#include "dr1801uv.hh"

// Identifies itself as "Prolific USB-serial controller" (chip in cable).
#define USB_VID 0x067b
#define USB_PID 0x23a3

#define WRITE_CODEPLUG_OFFSET 0x00000304
#define WRITE_CODEPLUG_SIZE   0x0001da8c

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
 * Implementation of DR1801UVInterface::PrepareWriteRequest
 * ********************************************************************************************* */
DR1801UVInterface::PrepareWriteRequest::PrepareWriteRequest(uint32_t s, uint32_t speed, uint16_t crc=0)
  : _unknown0(qToBigEndian((uint16_t)0x0001)), size(qToBigEndian(s)),
    checksum(crc), baudRate(qToBigEndian(speed))
{
  // pass...
}

void
DR1801UVInterface::PrepareWriteRequest::updateCRC(const uint8_t *data, size_t length) {
  uint16_t *ptr = (uint16_t*)data;
  for (unsigned int i=0; i<length/2;i++) {
    checksum ^= ptr[i];
  }
}

/* ********************************************************************************************* *
 * Implementation of DR1801UVInterface::PrepareWriteResponse
 * ********************************************************************************************* */
bool
DR1801UVInterface::PrepareWriteResponse::isSuccessful() const {
  return 0x0100 == qFromBigEndian(responseCode);
}


/* ********************************************************************************************* *
 * Implementation of DR1801UVInterface::CodeplugWriteResponse
 * ********************************************************************************************* */
bool
DR1801UVInterface::CodeplugWriteResponse::isSuccessful() const {
  return 0x01 == success;
}


/* ********************************************************************************************* *
 * Implementation of DR1801UVInterface
 * ********************************************************************************************* */
DR1801UVInterface::DR1801UVInterface(const USBDeviceDescriptor &descriptor,
                                     const ErrorStack &err, QObject *parent)
  : AuctusA6Interface(descriptor, err, parent), _identifier()
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

bool
DR1801UVInterface::readCodeplug(
    Codeplug &codeplug, std::function<void(unsigned int, unsigned int)> progress,
    const ErrorStack &err)
{
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
  if (! prepareReading(ReadSpeed, resp, err)) {
    errMsg(err) << "Cannot start reading the codeplug from " << _identifier << ".";
    _state = ERROR;
    return false;
  }

  unsigned int bytesToTransfer = resp.getSize(), total = bytesToTransfer;
  if (! startReading(err)) {
    errMsg(err) << "Cannot start reading the codeplug form " << _identifier << ".";
    _state = ERROR;
    return false;
  }

  if (codeplug.image(0).element(0).memSize() != bytesToTransfer) {
    errMsg(err) << "Codeplug size mismatch! Expected " << codeplug.image(0).element(0).memSize()
                << " radio sends " << bytesToTransfer << ".";
    _state = ERROR;
    return false;
  }

  if (progress)
    progress(0, total);
  logDebug() << "Start reading " << bytesToTransfer << "b of codeplug memory.";

  unsigned int offset = 0;
  while (bytesToTransfer) {
    unsigned n = std::min(256U, bytesToTransfer);
    while (n > bytesAvailable()) {
      if (! waitForReadyRead(2000)) {
        errMsg(err) << QSerialPort::errorString();
        errMsg(err) << "Cannot read from device '" << portName() << "'.";
        _state = ERROR;
        return false;
      }
    }
    if (! QSerialPort::read((char*)codeplug.image(0).data(offset), n)) {
      errMsg(err) << QSerialPort::errorString();
      errMsg(err) << "Cannot read codeplug from device.";
      _state = ERROR;
      return false;
    }
    if (progress)
      progress(offset, total);
  }

  // Set the baud-rate back to 9600
  logDebug() << "Reset baudrate to " << DefaultSpeed << ".";
  if (! this->setBaudRate(DefaultSpeed)) {
    errMsg(err) << "Cannot set baud-rate of serial port '" << portName() << "'.";
    return false;
  }
  QThread::msleep(250);

  _state = IDLE;

  return true;
}

bool
DR1801UVInterface::writeCodeplug(
    const Codeplug &codeplug, std::function<void(unsigned int, unsigned int)> progress,
    const ErrorStack &err)
{
  if (! isOpen()) {
    errMsg(err) << "Cannot write codeplug to device: Interface not open.";
    return false;
  }

  if (IDLE != _state) {
    errMsg(err) << "Cannot write codeplug to device: Interface not in idle state. "
                << "State=" << _state << ".";
    return false;
  }

  if (! enterProgrammingMode(err)) {
    errMsg(err) << "Cannot write codeplug.";
    return false;
  }

  // Compute CRC over codeplug
  uint16_t crc = 0;
  unsigned int offset = WRITE_CODEPLUG_OFFSET,
      bytesToTransfer = WRITE_CODEPLUG_SIZE,
      total = bytesToTransfer;
  for (unsigned int i=0; i<bytesToTransfer/2; i++) {
    crc ^= *((const uint16_t*)codeplug.image(0).data(offset +2*i));
  }
  if (! prepareWriting(bytesToTransfer, DR1801UVInterface::WriteSpeed, crc, err) ) {
    errMsg(err) << "Cannot initialize codeplug write.";
    return false;
  }

  if (progress)
    progress(0, total);

  while (bytesToTransfer) {
    uint32_t n = std::min(256U, bytesToTransfer);
    if (! QSerialPort::write((char*)codeplug.data(offset), n)) {
      errMsg(err) << "Cannot write codeplug to device.";
      return false;
    }
    // Wait for bytes written
    while (bytesToWrite()) {
      if (! waitForBytesWritten(2000)) {
        errMsg(err) << errorString();
        errMsg(err) << "Cannot write codeplug to the device.";
        _state = ERROR;
        return false;
      }
    }
    offset += n; bytesToTransfer -= n;
    if (progress)
      progress(offset, total);
  }

  if (! receiveWriteACK(err)) {
    errMsg(err) << "Cannot finish write operation properly. Partial write?";
    return false;
  }

  return true;
}

bool
DR1801UVInterface::read_start(uint32_t bank, uint32_t addr, const ErrorStack &err) {
  Q_UNUSED(bank); Q_UNUSED(addr); Q_UNUSED(err);
  return false;
}

bool
DR1801UVInterface::read(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes, const ErrorStack &err) {
  Q_UNUSED(bank); Q_UNUSED(addr); Q_UNUSED(data); Q_UNUSED(nbytes); Q_UNUSED(err);
  return false;
}

bool
DR1801UVInterface::read_finish(const ErrorStack &err) {
  Q_UNUSED(err);
  return false;
}


bool
DR1801UVInterface::write_start(uint32_t bank, uint32_t addr, const ErrorStack &err) {
  Q_UNUSED(bank); Q_UNUSED(addr); Q_UNUSED(err);
  return false;
}

bool
DR1801UVInterface::write(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes, const ErrorStack &err) {
  Q_UNUSED(bank); Q_UNUSED(addr); Q_UNUSED(data); Q_UNUSED(nbytes); Q_UNUSED(err);
  return false;
}

bool
DR1801UVInterface::write_finish(const ErrorStack &err) {
  Q_UNUSED(err);
  return false;
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


/* ********************************************************************************************* *
 * Internal methods communicating with the device.
 * ********************************************************************************************* */
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

  logDebug() << "Set baudrate to " << baudrate << ".";
  if (! this->setBaudRate(baudrate)) {
    errMsg(err) << "Cannot set baud-rate of serial port '" << portName() << "'.";
    return false;
  }
  QThread::msleep(250);

  return true;
}

bool
DR1801UVInterface::prepareWriting(uint32_t size, uint32_t baudrate, uint16_t crc, const ErrorStack &err) {
  PrepareWriteRequest request(size, baudrate, crc);
  PrepareReadResponse response;
  uint8_t respSize = sizeof(request);

  if (! send_receive(PREPARE_CODEPLUG_WRITE, (uint8_t *)&request, sizeof(request), (uint8_t *)&response, respSize, err)) {
    errMsg(err) << "Cannot prepare writing of codeplug.";
    return false;
  }

  if ((sizeof(PrepareWriteResponse) != respSize) || (! response.isSuccessful())) {
    errMsg(err) << "Prepare writing of codeplug failed.";
    return false;
  }

  logDebug() << "Set baudrate to " << baudrate << ".";
  if (! this->setBaudRate(baudrate)) {
    errMsg(err) << "Cannot set baud-rate of serial port '" << portName() << "'.";
    return false;
  }
  QThread::msleep(250);

  _state = WRITE_THROUGH;
  return true;
}

bool
DR1801UVInterface::receiveWriteACK(const ErrorStack &err) {
  // Set the baud-rate back to 9600
  logDebug() << "Reset baudrate to " << DefaultSpeed;
  if (! this->setBaudRate(DefaultSpeed)) {
    errMsg(err) << "Cannot set baud-rate of serial port '" << portName() << "'.";
    return false;
  }
  QThread::msleep(250);
  _state = IDLE;

  // Wait for device response
  uint16_t rcode;
  CodeplugWriteResponse response;
  uint8_t size = sizeof(CodeplugWriteResponse);
  if (! receive(rcode, (uint8_t*) &response, size, err)) {
    errMsg(err) << "Cannot complete codeplug write.";
    return false;
  }

  if (((rcode&0x7fff) != CODEPLUG_WRITTEN) || (! response.isSuccessful())) {
    errMsg(err) << "Cannot complete codeplug write.";
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
