#include "kydera_interface.hh"
#include <QtEndian>

#define BLOCK_SIZE 0x800 // 2048


/* ********************************************************************************************* *
 * KyderaInterface::ReadRequest
 * ********************************************************************************************* */
KyderaInterface::ReadCommand::ReadCommand(uint16_t blocks)
{
  // Number of blocks is one less that the actual blocks being read
  //if (blocks)
  //  blocks--;
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
  if (! receive((char *)&resp, sizeof(WriteResponse), err)) {
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
  if (! receive((char *)&info, sizeof(DeviceInfo), err)) {
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
  QByteArray resp(5,0);
  if ((!receive(resp.data(), resp.size())) || ("Write" != resp)) {
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
  QByteArray check(13,0);
  if ((! receive(check.data(), check.size(), err)) || (! check.startsWith("ChecksumW"))) {
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
  if (! receive((char *)&resp, sizeof(ReadResponse), err)) {
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
  if (! receive((char *)&info, sizeof(DeviceInfo), err)) {
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
  if (! receive((char *)data, nbytes, err)) {
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
  QByteArray check(13, 0);
  if (! receive(check.data(), check.size(), err)){
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
  int nb = QSerialPort::write((char *)&cmd, sizeof(ReadCommand));
  if (sizeof(ReadCommand) != nb) {
    _state = State::Error;
    if (QSerialPort::NoError != QSerialPort::error())
      errMsg(err) << "Cannot send read command: " << USBSerial::errorString();
    else
      errMsg(err) << "Cannot send read command: Incomplete write "
                  << nb << "b of " << sizeof(ReadCommand) << "b.";
    return false;
  }

  ReadResponse resp;
  if (! receive((char *)&resp, sizeof(ReadResponse), err)) {
    _state = State::Error;
    errMsg(err) << "Cannot receive response for read command.";
    return false;
  }
  if (! resp.check()) {
    _state = State::Error;
    errMsg(err) << "Device returned invalid response: "
                << QByteArray((char *)&resp, sizeof(ReadResponse)).toHex();
    return false;
  }

  DeviceInfo info;
  if (! receive((char *)&info, sizeof(DeviceInfo), err)) {
    _state = State::Error;
    errMsg(err) << "Cannot receive device info.";
    return false;
  }

  // Read the requested block of 2048 bytes
  nb = QSerialPort::write("Read", 4);
  if (4 != nb) {
    _state = State::Error;
    if (QSerialPort::NoError != QSerialPort::error())
      errMsg(err) << "Cannot send read request: " << USBSerial::errorString();
    else
      errMsg(err) << "Cannot send read request: Incomplete write "
                  << nb << "b of 4b.";
    return false;
  }

  QByteArray data(0x800, 0x00);
  if (! receive(data.data(), data.size(), err)) {
    _state = State::Error;
    errMsg(err) << "Cannot receive first codeplug block (2048b).";
    return false;
  }

  // Read checksum to complete transfer
  data.resize(13);
  if (! receive(data.data(), data.size(), err)) {
    _state = State::Error;
    errMsg(err) << "Cannot receive checksum.";
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

bool
KyderaInterface::receive(char *buffer, unsigned size, const ErrorStack &err) {
  if (State::Error == _state) {
    errMsg(err) << "Cannot read from radio, interface not ready.";
    return false;
  }

  // Read from device until complete response has been read
  char *p = buffer;
  int len = size;
  while (len > 0) {
    if (! waitForReadyRead(1000)) {
      errMsg(err) << "Cannot read from device: Timeout.";
      errMsg(err) << "Incomplete data: '" << QByteArray(buffer, size-len).toHex() << "'.";
      return false;
    }

    int r = QSerialPort::read(p, len);
    if (r < 0) {
      errMsg(err) << "Cannot read from device: " << QSerialPort::errorString();
      errMsg(err) << "Incomplete data: '" << QByteArray(buffer, size-len).toHex() << "'.";
      return false;
    }
    p += r;
    len-=r;
  }

  return true;
}
