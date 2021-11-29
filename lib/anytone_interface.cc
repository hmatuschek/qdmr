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
AnytoneInterface::RadioVariant::RadioVariant()
  : name(""), bands(0x00), version("")
{
  // pass...
}

bool
AnytoneInterface::RadioVariant::isValid() const {
  return ! name.isEmpty();
}


/* ********************************************************************************************* *
 * Implementation of AnytoneInterface
 * ********************************************************************************************* */
AnytoneInterface::AnytoneInterface(const ErrorStack &err, QObject *parent)
  : USBSerial(0x28e9, 0x018a, err, parent), _state(STATE_INITIALIZED), _info()
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
    _info = RadioVariant();
    _state = STATE_ERROR;
  }
}

AnytoneInterface::~AnytoneInterface() {
  if (isOpen())
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

RadioInfo
AnytoneInterface::identifier(const ErrorStack &err) {
  if (! _info.isValid())
    return RadioInfo();
  if ("D868UVE" == _info.name) {
    return RadioInfo::byID(RadioInfo::D868UVE);
  } else if ("D6X2UV" == _info.name) {
    return RadioInfo::byID(RadioInfo::DMR6X2);
  } else if ("D878UV" == _info.name) {
    return RadioInfo::byID(RadioInfo::D878UV);
  } else if ("D878UV2" == _info.name) {
    return RadioInfo::byID(RadioInfo::D878UVII);
  } else if ("D578UV" == _info.name) {
    return RadioInfo::byID(RadioInfo::D578UV);
  }

  errMsg(err) << "Unsupported AnyTone radio '" << _info.name << "'.";
  return RadioInfo();
}

bool
AnytoneInterface::getInfo(RadioVariant &info) {
  if (_info.isValid()) {
    info = _info;
    return true;
  }
  return false;
}

bool
AnytoneInterface::write_start(uint32_t bank, uint32_t addr, const ErrorStack &err)
{
  Q_UNUSED(bank); Q_UNUSED(addr)
  if ((STATE_PROGRAM != _state) && (! enter_program_mode(err)))
    return false;

  return true;
}

bool
AnytoneInterface::write(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes, const ErrorStack &err)
{
  if (0 != bank) {
    errMsg(err) << "Anytone: Cannot write to bank " << bank << ". There is only one (idx=0).";
    return false;
  }

  if (STATE_PROGRAM != _state) {
    errMsg(err) << "Anytone: Cannot write data to device: Not in programming mode.";
    return false;
  }

  //logDebug() << "Anytone: Write " << nbytes << "b to addr 0x" << QString::number(addr, 16) << "...";

  for (int i=0; i<nbytes; i+=16) {
    uint8_t ack;
    WriteRequest req(addr+i, (const char *)(data+i));
    if (! send_receive((const char *)&req, sizeof(WriteRequest),(char *)&ack, 1, err)) {
      errMsg(err) << "Anytone: Cannot write data to device.";
      return false;
    }
    if (0x06 != ack) {
      errMsg(err) << "Anytone: Cannot write data to device: Unexpected response "
                  << (int)ack << ", expected 6.";
      return false;
    }
  }

  return true;
}

bool
AnytoneInterface::write_finish(const ErrorStack &err) {
  Q_UNUSED(err)
  return true;
}

bool
AnytoneInterface::read_start(uint32_t bank, uint32_t addr, const ErrorStack &err) {
  Q_UNUSED(bank); Q_UNUSED(addr);

  if ((STATE_PROGRAM != _state) && (! enter_program_mode(err)))
    return false;

  return true;
}

bool
AnytoneInterface::read(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes, const ErrorStack &err) {
  if (0 != bank) {
    errMsg(err) << "Anytone: Cannot read from bank " << bank << ". There is only one (idx=0).";
    return false;
  }

  if (STATE_PROGRAM != _state) {
    errMsg(err) << "Anytone: Cannot read data from device: Not in programming mode.";
    return false;
  }

  //logDebug() << "Anytone: Read " << nbytes << "b from addr 0x" << QString::number(addr, 16) << "...";

  for (int i=0; i<nbytes; i+=16) {
    ReadRequest req(addr + i);
    ReadResponse resp;
    if (! send_receive((const char *)&req, sizeof(ReadRequest),
                       (char *)&resp, sizeof(ReadResponse), err)) {
      errMsg(err) << "Anytone: Cannot read data from device.";
      return false;
    }
    QString error_message;
    if (! resp.check(addr+i, error_message)) {
      errMsg(err) << "Anytone: Cannot read data from device: " << error_message << ".";
      return false;
    }
    memcpy(data+i, resp.data, 16);
  }

  return true;
}

bool
AnytoneInterface::read_finish(const ErrorStack &err) {
  Q_UNUSED(err)
  return true;
}

bool
AnytoneInterface::reboot(const ErrorStack &err) {
  if (STATE_PROGRAM == _state) {
    if (! leave_program_mode(err))
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
AnytoneInterface::enter_program_mode(const ErrorStack &err) {
  if (STATE_PROGRAM == _state) {
    logDebug() << "Already in program mode. Skip.";
    return true;
  } else if (STATE_OPEN != _state) {
    errMsg(err) << "Anytone: Cannot enter program mode. Device is in state " << _state << ".";
    return false;
  }

  char ack[3];
  // send "enter program mode" command
  if (! send_receive("PROGRAM", 7, ack, 3, err)) {
    errMsg(err) << "Anytone: Cannot enter program mode.";
    return false;
  }
  // check response
  if (0 != memcmp(ack, "QX\6", 3)) {
    errMsg(err) << "Anytone: Cannot enter program mode: Unexpected response. "
                << "Expected 515806 got " << QString::number(ack[0], 16)
                << QString::number(ack[1], 16)
                << QString::number(ack[1], 16) << ".";
    close();
    _state = STATE_ERROR;
    return false;
  }
  logDebug() << "Anytone: In program-mode now.";
  _state = STATE_PROGRAM;
  return true;
}

bool
AnytoneInterface::request_identifier(RadioVariant &info, const ErrorStack &err) {
  if (STATE_PROGRAM != _state) {
      errMsg(err) << "Anytone: Cannot request identifier. Device not in program mode, is in state "
               << _state << ".";
      return false;
    }

  RadioInfoResponse resp;
  // send "identify" command (in program mode)
  if (! send_receive("\2", 1, (char *)&resp, sizeof(RadioInfoResponse))) {
    errMsg(err) << "Anytone: Cannot request identifier.";
    return false;
  }
  // check response
  if (('I'!=resp.prefix) || (0x06 != resp.eot)) {
    errMsg(err) << "Anytone: Cannot request identifier: Unexpected response.";
    close();
    _state = STATE_ERROR;
    return false;
  }
  info.name = QString::fromLocal8Bit(resp.model, strnlen(resp.model, sizeof(resp.model))).simplified();
  info.bands = resp.bands;
  info.version = QString::fromLocal8Bit(resp.version, strnlen(resp.version, sizeof(resp.version))).simplified();
  logDebug() << "Found radio '" << info.name << "', version '" << info.version << "'.";
  return true;
}

bool
AnytoneInterface::leave_program_mode(const ErrorStack &err) {
  if (STATE_OPEN == _state) {
    logDebug() << "Device in open mode -> no need to leave program mode.";
    return true;
  } else if (STATE_PROGRAM != _state) {
    errMsg(err) << "Anytone: Cannot leave program mode. Device in state " << _state << ".";
    return false;
  }

  char ack[1];
  if (! send_receive("END", 3, ack, 1)) {
    errMsg(err) << "Anytone: Cannot leave program mode.";
    return false;
  }

  logDebug() << "Anytone: Left program-mode.";
  _state = STATE_OPEN;
  return true;
}

bool
AnytoneInterface::send_receive(const char *cmd, int clen, char *resp, int rlen, const ErrorStack &err) {
  // Try to write command to device
  if (clen != QSerialPort::write(cmd, clen)) {
    errMsg(err) << "Cannot send command to device.";
    close();
    _state = STATE_ERROR;
    return false;
  }

  // Read from device until complete response has been read
  char *p = resp;
  int len = rlen;
  while (len > 0) {
    if (! waitForReadyRead(1000)) {
      errMsg(err) << "No response from device: Timeout.";
      close();
      _state = STATE_ERROR;
      return false;
    }

    int r = QSerialPort::read(p, len);
    if (r < 0) {
      errMsg(err) << "Cannot read response from device.";
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
