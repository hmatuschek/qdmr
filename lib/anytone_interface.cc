#include "anytone_interface.hh"
#include "logger.hh"
#include <QtEndian>

/* ********************************************************************************************* *
 * Implementation of AnytoneInterface::ReadRequest
 * ********************************************************************************************* */
void
AnytoneInterface::ReadRequest::initRead(uint32_t addr) {
  cmd = 'R';
  this->addr = qToLittleEndian(addr);
  size = 64;
}

/* ********************************************************************************************* *
 * Implementation of AnytoneInterface::ReadResponse
 * ********************************************************************************************* */
bool
AnytoneInterface::ReadResponse::check(uint32_t addr, QString &msg) const {
  if ('R' != cmd) {
    msg = QObject::tr("Invalid read response: Expected command 'R' got '%1'").arg(cmd);
    return false;
  }
  if (qFromLittleEndian(this->addr) != addr) {
    msg = QObject::tr("Invalid read response: Expected address '%1' got '%2'")
        .arg(addr, 8, 16, QChar('0')).arg(qFromLittleEndian(this->addr), 8, 16, QChar('0'));
    return false;
  }
  if (64 != size) {
    msg = QObject::tr("Invalid read response: Expected size 64 got %1").arg((int)size);
    return false;
  }
  // Compute checksum
  uint8_t crc=0;
  for (uint8_t i=0; i<size; i++)
    crc += ((const uint8_t *)data)[i];
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
void
AnytoneInterface::WriteRequest::initWrite(uint32_t addr, const char *data) {
  cmd = 'W';
  this->addr = qToLittleEndian(addr);
  size = 16;
  sum = 0;
  for (uint8_t i=0; i<size; i++) {
    this->data[i] = data[i];
    sum += ((const uint8_t *)data)[i];
  }
  ack = 6;
}


/* ********************************************************************************************* *
 * Implementation of AnytoneInterface
 * ********************************************************************************************* */
AnytoneInterface::AnytoneInterface(QObject *parent)
  : USBSerial(0x28e9, 0x018a, parent), _state(STATE_INITIALIZED), _identifier("")
{
  if (isOpen())
    _state = STATE_OPEN;
  else
    _state = STATE_ERROR;
  // identify device
  this->identifier();
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
  if (! _identifier.isEmpty())
    return _identifier;
  if (! enter_program_mode()) {
    return "";
  }
  QString version;
  if (! request_identifier(_identifier, version)) {
    return "";
  }
  return _identifier;
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

  if (STATE_PROGRAM == _state) {
    _errorMessage = "Anytone: Cannot write data to device: Not in programming mode.";
    logError() << _errorMessage;
    return false;
  }

  for (int i=0; i<nbytes; i+=16) {
    uint8_t ack;
    WriteRequest req;
    req.initWrite(addr+i, (const char *)(data+i));
    if (! send_receive((const char *)&req, sizeof(ReadRequest),(char *)&ack, 1)) {
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

  if (STATE_PROGRAM == _state) {
    _errorMessage = "Anytone: Cannot read data from device: Not in programming mode.";
    logError() << _errorMessage;
    return false;
  }

  for (int i=0; i<nbytes; i+=64) {
    ReadRequest req;
    ReadResponse resp;
    req.initRead(addr+i);
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
    memcpy(data+i, resp.data, 64);
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
  return true;
}

bool
AnytoneInterface::request_identifier(QString &radio, QString &version) {
  if (STATE_PROGRAM != _state) {
      _errorMessage = tr("Anytone: Cannot request identifier. "
                         "Device not in program mode, is in state %1.").arg(_state);
      logDebug() << _errorMessage;
      return false;
    }

  char resp[16];
  // send "identify" command (in program mode)
  if (! send_receive("\2", 1, resp, 16)) {
    _errorMessage = tr("Anytone: Cannot request identifier: %1").arg(_errorMessage);
    logError() << _errorMessage;
    return false;
  }
  // check response
  if (('I'!=resp[0]) || (6 != resp[15])) {
    _errorMessage = tr("Anytone: Cannot request identifier: Unexpected response. "
                       "Expected 49...06 got %1...%2.")
        .arg((int)resp[0],2,16,QChar('0')).arg((int)resp[15],2,16,QChar('0'));
    logError() << _errorMessage;
    close();
    _state = STATE_ERROR;
    return false;
  }
  radio = QString::fromLocal8Bit(resp+1,8).simplified();
  version = QString::fromLocal8Bit(resp+9, 6).simplified();
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
      _errorMessage = "Cannot response from device: Timeout.";
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
