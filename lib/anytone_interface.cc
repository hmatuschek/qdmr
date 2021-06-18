#include "anytone_interface.hh"
#include "logger.hh"
#include <QtEndian>

/* ********************************************************************************************* *
 * Implementation of AnytoneInterface::ReadRequest
 * ********************************************************************************************* */
AnytoneInterface::ReadRequest::ReadRequest(uint32_t addr) {
  cmd = 'R';
  this->addr = qToBigEndian(addr);
  size = 16;
}

/* ********************************************************************************************* *
 * Implementation of AnytoneInterface::ReadResponse
 * ********************************************************************************************* */
bool
AnytoneInterface::ReadResponse::check(uint32_t addr, QString &msg) const {
  if ('W' != cmd) {
    msg = QObject::tr("Invalid read response: Expected command 'W' got '%1'").arg(cmd);
    return false;
  }
  if (qFromBigEndian(this->addr) != addr) {
    msg = QObject::tr("Invalid read response: Expected address '%1' got '%2'")
        .arg(addr, 8, 16, QChar('0')).arg(qFromLittleEndian(this->addr), 8, 16, QChar('0'));
    return false;
  }
  if (16 != size) {
    msg = QObject::tr("Invalid read response: Expected size 64 got %1").arg((int)size);
    return false;
  }
  // Compute checksum
  uint8_t crc=((const uint8_t *)this)[1];
  for (uint8_t i=2; i<(size+6); i++)
    crc += ((const uint8_t *)this)[i];
  // compare
  if (crc != sum) {
    msg = QObject::tr("Invalid read response: Expected check-sum %1 got %2")
        .arg((int)crc).arg((int)sum);
    return false;
  }
  // check ACK flag.
  if (6 != ack) {
    msg = QObject::tr("Invalid read response: Expected ACK 6 got %1").arg((int)ack);
    return false;
  }
  // ok
  return true;
}

/* ********************************************************************************************* *
 * Implementation of AnytoneInterface::WriteRequest
 * ********************************************************************************************* */
AnytoneInterface::WriteRequest::WriteRequest(uint32_t addr, const char *data) {
  cmd = 'W';
  this->addr = qToBigEndian(addr);
  size = 16;
   memcpy(this->data, data, size);
  sum = 0;
  uint8_t *b=(uint8_t *)this;
  for (uint8_t i=1; i<(size+6); i++)
    sum += b[i];
  ack = 6;
}


/* ********************************************************************************************* *
 * Implementation of AnytoneInterface::RadioInfo
 * ********************************************************************************************* */
AnytoneInterface::RadioInfo::RadioInfo()
  : name(""), bands(0x00), version("")
{
  // pass...
}

bool
AnytoneInterface::RadioInfo::isValid() const {
  return ! name.isEmpty();
}


/* ********************************************************************************************* *
 * Implementation of AnytoneInterface
 * ********************************************************************************************* */
AnytoneInterface::AnytoneInterface(QObject *parent)
  : USBSerial(0x28e9, 0x018a, parent), _state(STATE_INITIALIZED), _info()
{
  if (isOpen()) {
    _state = STATE_OPEN;
  } else {
    _state = STATE_ERROR;
    return;
  }

  // enter program mode
  if (! this->enter_program_mode())
    return;
  // identify device
  if (! this->request_identifier(_info)) {
    _info = RadioInfo();
    _state = STATE_ERROR;
  }
}

AnytoneInterface::~AnytoneInterface() {
  this->close();
}

void
AnytoneInterface::close() {
  switch (_state) {
  case STATE_INITIALIZED:
  case STATE_OPEN:
    USBSerial::close();
    break;
  case STATE_PROGRAM:
    this->reboot();
    break;
  case STATE_CLOSED:
  case STATE_ERROR:
    break;
  }
}

QString
AnytoneInterface::identifier() {
  return _info.name;
}

bool
AnytoneInterface::getInfo(RadioInfo &info) {
  if (_info.isValid()) {
    info = _info;
    return true;
  }
  return false;
}

bool
AnytoneInterface::write_start(uint32_t bank, uint32_t addr)
{
  Q_UNUSED(bank);
  Q_UNUSED(addr);

  if ((STATE_PROGRAM != _state) && (! enter_program_mode()))
    return false;

  return true;
}

bool
AnytoneInterface::write(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes)
{
  if (0 != bank) {
    _errorMessage = tr("Anytone: Cannot write to bank %1. There is only one (idx=0).").arg(bank);
    logError() << _errorMessage;
    return false;
  }

  if (STATE_PROGRAM != _state) {
    _errorMessage = "Anytone: Cannot write data to device: Not in programming mode.";
    logError() << _errorMessage;
    return false;
  }

  logDebug() << "Anytone: Write " << nbytes << "b to addr 0x" << QString::number(addr, 16) << "...";

  for (int i=0; i<nbytes; i+=16) {
    uint8_t ack;
    WriteRequest req(addr+i, (const char *)(data+i));
    if (! send_receive((const char *)&req, sizeof(WriteRequest),(char *)&ack, 1)) {
      _errorMessage = tr("Anytone: Cannot write data to device: %1").arg(_errorMessage);
      logError() << _errorMessage;
      return false;
    }
    if (0x06 != ack) {
      _errorMessage = tr("Anytone: Cannot write data to device: Unexpected response %1, expected 06.")
          .arg(ack, 2, 16, QChar('0'));
      logError() << _errorMessage;
      return false;
    }
  }

  return true;
}

bool
AnytoneInterface::write_finish() {
  // pass...
  return true;
}

bool
AnytoneInterface::read_start(uint32_t bank, uint32_t addr) {
  Q_UNUSED(bank);
  Q_UNUSED(addr);

  if ((STATE_PROGRAM != _state) && (! enter_program_mode()))
    return false;

  return true;
}

bool
AnytoneInterface::read(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes) {
  if (0 != bank) {
    _errorMessage = tr("Anytone: Cannot read from bank %1. There is only one (idx=0).").arg(bank);
    logError() << _errorMessage;
    return false;
  }

  if (STATE_PROGRAM != _state) {
    _errorMessage = "Anytone: Cannot read data from device: Not in programming mode.";
    logError() << _errorMessage;
    return false;
  }

  //logDebug() << "Anytone: Read " << nbytes << "b from addr 0x" << QString::number(addr, 16) << "...";

  for (int i=0; i<nbytes; i+=16) {
    ReadRequest req(addr + i);
    ReadResponse resp;
    if (! send_receive((const char *)&req, sizeof(ReadRequest),
                       (char *)&resp, sizeof(ReadResponse))) {
      _errorMessage = tr("Anytone: Cannot read data from device: %1").arg(_errorMessage);
      logError() << _errorMessage;
      return false;
    }
    if (! resp.check(addr+i, _errorMessage)) {
      _errorMessage = tr("Anytone: Cannot read data from device: %1").arg(_errorMessage);
      logError() << _errorMessage;
      return false;
    }
    memcpy(data+i, resp.data, 16);
  }

  return true;
}

bool
AnytoneInterface::read_finish() {
  // pass...
  return true;
}

bool
AnytoneInterface::reboot() {
  if (STATE_PROGRAM == _state) {
    if (! leave_program_mode())
      return false;
    _state = STATE_OPEN;
  }
  if (STATE_OPEN == _state) {
    this->close();
    _state = STATE_CLOSED;
  }
  return true;
}


bool
AnytoneInterface::enter_program_mode() {
  if (STATE_PROGRAM == _state) {
    logDebug() << "Already in program mode. Skip.";
    return true;
  } else if (STATE_OPEN != _state) {
    _errorMessage = tr("Anytone: Cannot enter program mode. Device is in state %1.").arg(_state);
    logDebug() << _errorMessage;
    return false;
  }

  char ack[3];
  // send "enter program mode" command
  if (! send_receive("PROGRAM", 7, ack, 3)) {
    _errorMessage = tr("Anytone: Cannot enter program mode: %1").arg(_errorMessage);
    logError() << _errorMessage;
    return false;
  }
  // check response
  if (0 != memcmp(ack, "QX\6", 3)) {
    _errorMessage = tr("Anytone: Cannot enter program mode: Unexpected response. "
                       "Expected 515806 got %1%2%3.")
        .arg((int)ack[0],2,16,QChar('0')).arg((int)ack[1],2,16,QChar('0'))
        .arg((int)ack[2],2,16,QChar('0'));
    logError() << _errorMessage;
    close();
    _state = STATE_ERROR;
    return false;
  }
  logDebug() << "Anytone: In program-mode now.";
  _state = STATE_PROGRAM;
  return true;
}

bool
AnytoneInterface::request_identifier(RadioInfo &info) {
  if (STATE_PROGRAM != _state) {
      _errorMessage = tr("Anytone: Cannot request identifier. "
                         "Device not in program mode, is in state %1.").arg(_state);
      logDebug() << _errorMessage;
      return false;
    }

  RadioInfoResponse resp;
  // send "identify" command (in program mode)
  if (! send_receive("\2", 1, (char *)&resp, sizeof(RadioInfoResponse))) {
    _errorMessage = tr("Anytone: Cannot request identifier: %1").arg(_errorMessage);
    logError() << _errorMessage;
    return false;
  }
  // check response
  if (('I'!=resp.prefix) || (0x06 != resp.eot)) {
    _errorMessage = tr("Anytone: Cannot request identifier: Unexpected response. "
                       "Expected 49...06 got %1...%2.")
        .arg((int)resp.prefix,2,16,QChar('0'))
        .arg((int)resp.eot,2,16,QChar('0'));
    logError() << _errorMessage;
    close();
    _state = STATE_ERROR;
    return false;
  }
  info.name = QString::fromLocal8Bit(resp.model, strnlen(resp.model, sizeof(resp.model))).simplified();
  info.bands = resp.bands;
  info.version = QString::fromLocal8Bit(resp.version, strnlen(resp.version, sizeof(resp.model))).simplified();
  logDebug() << "Found radio '" << info.name << "', version '" << info.version << "'.";
  return true;
}

bool
AnytoneInterface::leave_program_mode() {
  if (STATE_OPEN == _state) {
    logDebug() << "Device in open mode -> no need to leave program mode.";
    return true;
  } else if (STATE_PROGRAM != _state) {
    _errorMessage = tr("Anytone: Cannot leave program mode. Device in state %1.").arg(_state);
    logError() << _errorMessage;
    return false;
  }

  char ack[1];
  if (! send_receive("END", 3, ack, 1)) {
    _errorMessage = tr("Anytone: Cannot leave program mode: %1").arg(_errorMessage);
    return false;
  }

  logDebug() << "Anytone: Left program-mode.";
  _state = STATE_OPEN;
  return true;
}

bool
AnytoneInterface::send_receive(const char *cmd, int clen, char *resp, int rlen) {
  // Try to write command to device
  if (clen != QSerialPort::write(cmd, clen)) {
    _errorMessage = "Cannot send command to device: " + QSerialPort::errorString();
    logError() << _errorMessage;
    close();
    _state = STATE_ERROR;
    return false;
  }

  // Read from device until complete response has been read
  char *p = resp;
  int len = rlen;
  while (len > 0) {
    if (! waitForReadyRead(1000)) {
      _errorMessage = "No response from device: Timeout.";
      logError() << _errorMessage;
      close();
      _state = STATE_ERROR;
      return false;
    }

    int r = QSerialPort::read(p, len);
    if (r < 0) {
      _errorMessage = "Cannot read response from device: " + QSerialPort::errorString();
      logError() << _errorMessage;
      close();
      _state = STATE_ERROR;
      return false;
    }
    p += r;
    len-=r;
  }

  // done
  return true;
}
