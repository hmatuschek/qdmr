#include "kydera_interface.hh"
#include <QtEndian>

#define BLOCK_SIZE 0x800 // 2048


/* ********************************************************************************************* *
 * KyderaInterface::ReadRequest
 * ********************************************************************************************* */
KyderaInterface::ReadCommand::ReadCommand(uint16_t blocks)
{
  // Number of blocks is one less that the actual blocks being read
  if (blocks)
    blocks--;
  // Init fields
  memset(this, 0x00, sizeof(ReadCommand));
  memcpy(this->cmd, "Flash Read ", sizeof(this->cmd));
  unknown0c = 0x3c;
  this->blocks = qToBigEndian(blocks);
}


/* ********************************************************************************************* *
 * KyderaInterface::ReadResponse
 * ********************************************************************************************* */
bool
KyderaInterface::ReadResponse::check() const {
  return 0 == strncmp(response, "  Read_2M_",10);
}

uint8_t
KyderaInterface::ReadResponse::blocks() const {
  return unknown0c[6];
}


/* ********************************************************************************************* *
 * KyderaInterface::WriteRequest
 * ********************************************************************************************* */
KyderaInterface::WriteCommand::WriteCommand(uint16_t blocks)
{
  // Init fields
  memset(this, 0x00, sizeof(ReadCommand));
  memcpy(this->cmd, "Flash Write", sizeof(this->cmd));
  unknown0c = 0x3c;
  // This time, simply number of blocks to write
  this->blocks = qToBigEndian(blocks);
}

/* ********************************************************************************************* *
 * KyderaInterface::WriteResponse
 * ********************************************************************************************* */
bool
KyderaInterface::WriteResponse::check() const {
  return 0 == strncmp(response, "  Write_2M_",11);
}


/* ********************************************************************************************* *
 * KyderaInterface::DeviceInfo
 * ********************************************************************************************* */
QString
KyderaInterface::DeviceInfo::device() const {
  size_t len=0;
  for (; len<sizeof(info); len++) {
    if (char(0xff) == info[len])
      break;
  }
  return QString::fromLocal8Bit(info, len).section('+',1);
}


/* ********************************************************************************************* *
 * KyderaInterface
 * ********************************************************************************************* */
KyderaInterface::KyderaInterface(const ErrorStack &err, QObject *parent)
  : USBSerial(0x067B, 0x2303, err, parent), _state(State::Idle), _radioInfo()
{
  // Check state of serial interface
  if (! USBSerial::isOpen()) {
    errMsg(err) << "Cannot init Kydera interface. Serial interface not open.";
    _state = State::Error;
    return;
  }

  // Read radio ID
  if (! readRadioInfo(err)) {
    errMsg(err) << "Cannot identify Kydera device.";
    _state = State::Error;
    return;
  }
}

bool
KyderaInterface::isOpen() const {
  return USBSerial::isOpen() && (State::Error!=_state) && _radioInfo.isValid();
}

RadioInfo
KyderaInterface::identifier(const ErrorStack &err) {
  if (! _radioInfo.isValid()) {
    errMsg(err) << "Radio information not yet read or failed.";
  }
  return _radioInfo;
}


bool
KyderaInterface::write_start(uint32_t bank, uint32_t addr, const ErrorStack &err) {
  Q_UNUSED(bank);

  if (State::Idle != _state) {
    errMsg(err) << "Cannot write to radio, interface not in idle state.";
    return false;
  }

  // Misuse of addr agrument to pass the number of blocks to write
  uint32_t nblock = addr/BLOCK_SIZE;
  WriteCommand cmd(nblock);
  if (sizeof(WriteCommand) != QSerialPort::write((char *)&cmd, sizeof(WriteCommand))) {
    _state = State::Error;
    errMsg(err) << "Cannot send write command: " << USBSerial::errorString();
    return false;
  }

  WriteResponse resp;
  if (sizeof(WriteResponse) != QSerialPort::read((char*)&resp, sizeof(WriteResponse))) {
    _state = State::Error;
    errMsg(err) << "Cannot read response: " << USBSerial::errorString();
    return false;
  }
  if (! resp.check()) {
    _state = State::Error;
    errMsg(err) << "Device returned invalid response.";
    return false;
  }

  DeviceInfo info;
  if (sizeof(DeviceInfo) != QSerialPort::read((char *)&info, sizeof(DeviceInfo))) {
    _state = State::Error;
    errMsg(err) << "Cannot read device info: " << USBSerial::errorString();
    return false;
  }

  _state = State::Write;
  return true;
}

bool
KyderaInterface::write(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes, const ErrorStack &err) {
  // Blocks are written in sequence. Hence there are no means to write to a specific address within
  // a bank.
  Q_UNUSED(bank); Q_UNUSED(addr);

  // Check state
  if (State::Write != _state) {
    errMsg(err) << "Cannot write to radio, interface not in write state.";
    return false;
  }

  // Check block size. We need to write in entire blocks of 0x800 bytes.
  if (BLOCK_SIZE != nbytes) {
    _state = State::Error;
    errMsg(err) << "Cannot write block of size " << nbytes << "b, needs to be "
                << BLOCK_SIZE << "b.";
    return false;
  }

  // Simply write data to device
  if (BLOCK_SIZE != QSerialPort::write((char *)data, nbytes)) {
    _state = State::Error;
    errMsg(err) << "Cannot write to device: " << QSerialPort::errorString();
    return false;
  }

  // Read response
  QByteArray resp = QSerialPort::read(5);
  if ("Write" != resp) {
    _state = State::Error;
    errMsg(err) << "Cannot write to device: Invalid response.";
    return false;
  }

  return true;
}

bool
KyderaInterface::write_finish(const ErrorStack &err) {
  // Check state
  if (State::Write != _state) {
    errMsg(err) << "Cannot write to radio, interface not in write state.";
    return false;
  }

  // Read checksum 'ChecksumW' + uint32
  QByteArray check = QSerialPort::read(13);
  if ((13 != check.size()) || (! check.startsWith("ChecksumW"))) {
    _state = State::Error;
    errMsg(err) << "Cannot read checksum or invalid response.";
    return false;
  }
  uint32_t crc = qFromLittleEndian(*((uint32_t *)check.right(4).data()));
  ///@todo Implement checksum check.

  ///@todo Read device status back.
  QByteArray status = QSerialPort::readAll();

  return true;
}


bool
KyderaInterface::read_start(uint32_t bank, uint32_t addr, const ErrorStack &err) {
  Q_UNUSED(bank);

  if (State::Idle != _state) {
    errMsg(err) << "Cannot read from radio, interface not in idle state.";
    return false;
  }

  // Misuse of addr agrument to pass the number of blocks to write
  uint32_t nblock = addr/BLOCK_SIZE;
  ReadCommand cmd(nblock);
  if (sizeof(ReadCommand) != QSerialPort::write((char *)&cmd, sizeof(ReadCommand))) {
    _state = State::Error;
    errMsg(err) << "Cannot send read command: " << USBSerial::errorString();
    return false;
  }

  ReadResponse resp;
  if (sizeof(ReadResponse) != QSerialPort::read((char*)&resp, sizeof(ReadResponse))) {
    _state = State::Error;
    errMsg(err) << "Cannot read response: " << USBSerial::errorString();
    return false;
  }
  if (! resp.check()) {
    _state = State::Error;
    errMsg(err) << "Device returned invalid response.";
    return false;
  }

  DeviceInfo info;
  if (sizeof(DeviceInfo) != QSerialPort::read((char *)&info, sizeof(DeviceInfo))) {
    _state = State::Error;
    errMsg(err) << "Cannot read device info: " << USBSerial::errorString();
    return false;
  }

  _state = State::Read;
  return true;
}

bool
KyderaInterface::read(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes, const ErrorStack &err) {
  // Blocks are read in sequence. Hence there are no means to read a specific address within a bank.
  Q_UNUSED(bank); Q_UNUSED(addr);

  // Check state
  if (State::Read != _state) {
    errMsg(err) << "Cannot read from radio, interface not in read state.";
    return false;
  }

  // Check block size. We need to read entire blocks of 0x800 bytes.
  if (BLOCK_SIZE != nbytes) {
    _state = State::Error;
    errMsg(err) << "Cannot read block of size " << nbytes << "b, needs to be "
                << BLOCK_SIZE << "b.";
    return false;
  }

  // Send read request
  if (4 != QSerialPort::write("Read", nbytes)) {
    _state = State::Error;
    errMsg(err) << "Cannot send read request: " << QSerialPort::errorString();
    return false;
  }

  // Read response containing data
  if (nbytes != QSerialPort::read((char *)data, nbytes)) {
    _state = State::Error;
    errMsg(err) << "Cannot read from device: " << QSerialPort::errorString();
    return false;
  }

  return true;
}

bool
KyderaInterface::read_finish(const ErrorStack &err) {
  // Check state
  if (State::Write != _state) {
    errMsg(err) << "Cannot write to radio, interface not in write state.";
    return false;
  }

  // Read checksum 'ChecksumR' + uint32
  QByteArray check = QSerialPort::read(13);
  if ((13 != check.size()) || (! check.startsWith("ChecksumR"))) {
    _state = State::Error;
    errMsg(err) << "Cannot read checksum or invalid response.";
    return false;
  }

  return true;
}

bool
KyderaInterface::readRadioInfo(const ErrorStack &err) {
  // Check inferface state
  if (State::Idle != _state) {
    errMsg(err) << "Cannot read radio info, interface not in idle state.";
    return false;
  }

  // Read only one block of the codeplug to obtain the radio ID.
  ReadCommand cmd(1);
  if (sizeof(ReadCommand) != QSerialPort::write((char *)&cmd, sizeof(ReadCommand))) {
    _state = State::Error;
    errMsg(err) << "Cannot send read command: " << USBSerial::errorString();
    return false;
  }

  ReadResponse resp;
  if (sizeof(ReadResponse) != QSerialPort::read((char*)&resp, sizeof(ReadResponse))) {
    _state = State::Error;
    errMsg(err) << "Cannot read response: " << USBSerial::errorString();
    return false;
  }
  if (! resp.check()) {
    _state = State::Error;
    errMsg(err) << "Device returned invalid response.";
    return false;
  }

  DeviceInfo info;
  if (sizeof(DeviceInfo) != QSerialPort::read((char *)&info, sizeof(DeviceInfo))) {
    _state = State::Error;
    errMsg(err) << "Cannot read device info: " << USBSerial::errorString();
    return false;
  }

  // Read the requested block of 2048 bytes
  if (4 != QSerialPort::write("Read")) {
    _state = State::Error;
    errMsg(err) << "Cannot send read request: " << USBSerial::errorString();
    return false;
  }

  QByteArray data = QSerialPort::read(0x800);
  if (0x800 != data.size()) {
    _state = State::Error;
    errMsg(err) << "Cannot read payload: " << USBSerial::errorString();
    return false;
  }

  // Read checksum to complete transfer
  data = QSerialPort::read(13); // 'ChecksumR' + uint32
  if (13 != data.size()) {
    _state = State::Error;
    errMsg(err) << "Cannot read checksum: " << USBSerial::errorString();
    return false;
  }

  // Identify device by ID string
  QString deviceID = info.device();
  if ("DRS-300UV" == deviceID) {
    _radioInfo = RadioInfo::byID(RadioInfo::CDR300UV);
  } else {
    _state = State::Error;
    errMsg(err) << "Uknown Kydera device '" << deviceID << "'.";
    return false;
  }

  return true;
}
