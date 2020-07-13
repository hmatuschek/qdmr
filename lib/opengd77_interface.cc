#include "opengd77_interface.hh"
#include "logger.hh"
#include <QtEndian>

#define BLOCK_SIZE  32
#define SECTOR_SIZE 4096

/* ********************************************************************************************* *
 * Implementation of OpenGD77Interface::ReadRequest
 * ********************************************************************************************* */
bool
OpenGD77Interface::ReadRequest::initReadEEPROM(uint32_t addr, uint16_t length) {
  type = 'R';
  command = READ_EEPROM;
  addr = qToBigEndian(addr);
  length = qToBigEndian(length);
  return true;
}

bool
OpenGD77Interface::ReadRequest::initReadFlash(uint32_t addr, uint16_t length) {
  type = 'R';
  command = READ_FLASH;
  addr = qToBigEndian(addr);
  length = qToBigEndian(length);
  return true;
}


/* ********************************************************************************************* *
 * Implementation of OpenGD77Interface::WriteRequest
 * ********************************************************************************************* */
bool
OpenGD77Interface::WriteRequest::initWriteEEPROM(uint32_t addr, const uint8_t *data, uint16_t size) {
  if (size > 32)
    size = 32;
  type = 'W';
  command = WRITE_EEPROM;
  payload.address = qToBigEndian(addr);
  payload.length = qToBigEndian(size);
  memcpy(payload.data, data, size);
  return true;
}

bool
OpenGD77Interface::WriteRequest::initSetFlashSector(uint32_t addr) {
  uint32_t sec = addr/SECTOR_SIZE;
  type = 'W';
  command = SET_FLASH_SECTOR;
  sector[0] = ((sec>>16) & 0xff);
  sector[1] = ((sec>>8) & 0xff);
  sector[2] = (sec & 0xff);
  return true;
}

bool
OpenGD77Interface::WriteRequest::initWriteFlash(uint32_t addr, const uint8_t *data, uint16_t size) {
  if (size > 32)
    size = 32;
  type = 'W';
  command = WRITE_SECTOR_BUFFER;
  payload.address = qToBigEndian(addr);
  payload.length = qToBigEndian(size);
  memcpy(payload.data, data, size);
  return true;
}

bool
OpenGD77Interface::WriteRequest::initFinishWriteFlash() {
  type = 'W';
  command = WRITE_FLASH_SECTOR;
  return true;
}


/* ********************************************************************************************* *
 * Implementation of OpenGD77Interface::CommandRequest
 * ********************************************************************************************* */
void
OpenGD77Interface::CommandRequest::initShowCPSScreen() {
  type = 'C';
  command = SHOW_CPS_SCREEN;
  x = 0;
  y = 0;
  size = 0;
  alignment = 0;
  inverted = 0;
  memset(message, 0, sizeof(message));
}

void
OpenGD77Interface::CommandRequest::initClearScreen() {
  type = 'C';
  command = CLEAR_SCREEN;
  x = 0;
  y = 0;
  size = 0;
  alignment = 0;
  inverted = 0;
  memset(message, 0, sizeof(message));
}

void
OpenGD77Interface::CommandRequest::initDisplay(uint8_t x, uint8_t y,
                                               const char *message, uint8_t size,
                                               uint8_t alignment, uint8_t inverted) {
  type = 'C';
  command = DISPLAY;
  this->x = x;
  this->y = y;
  this->size = std::min(size, uint8_t(16));
  this->alignment = alignment;
  this->inverted = inverted;
  memcpy(this->message, message, this->size);
}

void
OpenGD77Interface::CommandRequest::initRenderCPS() {
  type = 'C';
  command = RENDER_CPS;
  x = 0;
  y = 0;
  size = 0;
  alignment = 0;
  inverted = 0;
  memset(message, 0, sizeof(message));
}

void
OpenGD77Interface::CommandRequest::initCloseScreen() {
  type = 'C';
  command = CLOSE_CPS_SCREEN;
  x = 0;
  y = 0;
  size = 0;
  alignment = 0;
  inverted = 0;
  memset(message, 0, sizeof(message));
}

void
OpenGD77Interface::CommandRequest::initCommand(Option option) {
  type = 'C';
  command = COMMAND;
  this->option = option;
  y = 0;
  size = 0;
  alignment = 0;
  inverted = 0;
  memset(message, 0, sizeof(message));
}

/* ********************************************************************************************* *
 * Implementation of OpenGD77Interface
 * ********************************************************************************************* */
OpenGD77Interface::OpenGD77Interface(QObject *parent)
  : USBSerial(0x1fc9, 0x0094, parent), _sector(-1)
{
  // pass...
}

OpenGD77Interface::~OpenGD77Interface() {
  // pass...
}

void
OpenGD77Interface::close() {
  if (isOpen())
    USBSerial::close();
}

QString
OpenGD77Interface::identifier() {
  if (isOpen())
    return "OpenGD77";
  else
    return "";
}

bool
OpenGD77Interface::write_start(uint32_t bank, uint32_t addr) {
  if (EEPROM == bank) {
    if (_sector >=0) {
      if (! finishWriteFlash())
        return false;
    }
    _sector = -1;
  } else if (FLASH == bank) {
    int32_t sector = addr/SECTOR_SIZE;
    if (_sector != sector) {
      if (! finishWriteFlash())
        return false;
    }
  }
  return true;
}

bool
OpenGD77Interface::write(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes)
{
  if (EEPROM == bank) {
    if ((0 <= _sector) && (! finishWriteFlash()))
      return false;
    for (int i=0; i<nbytes; i+=32) {
      if (! writeEEPROM(addr+i, data+i, 32)) {
        _sector = -1;
        return false;
      }
    }
    return true;
  }

start:
  int32_t sector = addr/SECTOR_SIZE;

  if (0 > _sector) {
    if (! setFlashSector(addr))
      return false;
    _sector = sector;
  }

  if (sector == _sector) {
    for (int i=0; i<nbytes; i+=32) {
      if (! writeFlash(addr+i, data+i, 32)) {
        _sector = -1;
        return false;
      }
    }
  } else {
    _sector = -1;
    if (! finishWriteFlash()) {
      return false;
    }
    goto start;
  }

  return true;
}

bool
OpenGD77Interface::write_finish() {
  _sector = -1;
  return finishWriteFlash();
}

bool
OpenGD77Interface::read_start(uint32_t bank, uint32_t addr) {
  // pass...
  return true;
}

bool
OpenGD77Interface::read(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes) {
  if (! isOpen()) {
    _errorMessage = "Cannot read block: Device not open!";
    logError() << _errorMessage;
    return false;
  }

  for (int i=0; i<nbytes; i+=32) {
    bool ok;
    if (EEPROM == bank)
      ok = readEEPROM(addr+i, data+i, 32);
    else if (FLASH == bank)
      ok = readFlash(addr+i, data+i, 32);
    else {
      _errorMessage = tr("%1: Cannot read from bank %2: Unknown memory bank.")
          .arg(__func__).arg(bank);
      logError() << _errorMessage;
      return false;
    }

    if (! ok)
      return false;
  }

  return true;
}

bool
OpenGD77Interface::read_finish() {
  // pass...
  return true;
}


bool
OpenGD77Interface::readEEPROM(uint32_t addr, uint8_t *data, uint16_t len) {
  if (! isOpen()) {
    _errorMessage = "Cannot read block: Device not open!";
    logError() << _errorMessage;
    return false;
  }

  ReadRequest req; req.initReadEEPROM(addr, 32);
  if (sizeof(ReadRequest) != QSerialPort::write((const char *)&req, sizeof(ReadRequest))) {
    _errorMessage = tr("Cannot write to serial port: %1").arg(USBSerial::errorMessage());
    logError() << _errorMessage;
    return false;
  }

  if (! waitForReadyRead(1000)) {
    _errorMessage = tr("Cannot read from serial port: Timeout!");
    logError() << _errorMessage;
    return false;
  }

  ReadResponse resp;
  int retlen = QSerialPort::read((char *)&resp, sizeof(ReadResponse));

  if (0 > retlen) {
    _errorMessage = tr("Cannot read from serial port: %1").arg(USBSerial::errorMessage());
    logError() << _errorMessage;
    return false;
  } else if (0 == retlen) {
    _errorMessage = tr("Cannot read from serial port: Device returned empty message.");
    logError() << _errorMessage;
    return false;
  }

  if ('R' != resp.type) {
    _errorMessage = tr("Cannot read from device: Device returned error '%1'").arg(resp.type);
    logError() << _errorMessage;
    return false;
  }

  if (qFromBigEndian(req.length) != qFromBigEndian(resp.length)) {
    _errorMessage = tr("Cannot read from device: Device returned invalid length 0x%1.")
        .arg(qFromBigEndian(resp.length), 4, 16);
    logError() << _errorMessage;
    return false;
  }

  mempcpy(data, resp.data, qFromBigEndian(resp.length));
  return true;
}


bool
OpenGD77Interface::writeEEPROM(uint32_t addr, const uint8_t *data, uint16_t len) {
  WriteRequest req; req.initWriteEEPROM(addr, data, len);
  WriteResponse resp;

  if ((8+len) != QSerialPort::write((const char *)&req, 8+len)) {
    _errorMessage = tr("Cannot write to serial port: %1").arg(USBSerial::errorMessage());
    logError() << _errorMessage;
    return false;
  }

  if (! waitForReadyRead(1000)) {
    _errorMessage = tr("Cannot read from serial port: Timeout!");
    logError() << _errorMessage;
    return false;
  }

  int retlen = QSerialPort::read((char *)&resp, sizeof(WriteResponse));

  if (0 > retlen) {
    _errorMessage = tr("Cannot read from serial port: %1").arg(USBSerial::errorMessage());
    logError() << _errorMessage;
    return false;
  } else if (0 == retlen) {
    _errorMessage = tr("Cannot write EEPROM: Device returned empty message.");
    logError() << _errorMessage;
    return false;
  }

  if ((req.type != resp.type) || (req.command != resp.command)) {
    _errorMessage = tr("Cannot write EEPROM at %1: Device returned error '%2'").arg(addr, 6, 16).arg(resp.type);
    logError() << _errorMessage;
    return false;
  }

  return true;
}


bool
OpenGD77Interface::readFlash(uint32_t addr, uint8_t *data, uint16_t len) {
  if (! isOpen()) {
    _errorMessage = "Cannot read block: Device not open!";
    logError() << _errorMessage;
    return false;
  }

  ReadRequest req; req.initReadFlash(addr, 32);
  if (sizeof(ReadRequest) != QSerialPort::write((const char *)&req, sizeof(ReadRequest))) {
    _errorMessage = tr("Cannot write to serial port: %1").arg(USBSerial::errorMessage());
    logError() << _errorMessage;
    return false;
  }

  if (! waitForReadyRead(1000)) {
    _errorMessage = tr("Cannot read from serial port: Timeout!");
    logError() << _errorMessage;
    return false;
  }

  ReadResponse resp;
  int retlen = QSerialPort::read((char *)&resp, sizeof(ReadResponse));

  if (0 > retlen) {
    _errorMessage = tr("Cannot read from serial port: %1").arg(USBSerial::errorMessage());
    logError() << _errorMessage;
    return false;
  } else if (0 == retlen) {
    _errorMessage = tr("Cannot read from serial port: Device returned empty message.");
    logError() << _errorMessage;
    return false;
  }

  if ('R' != resp.type) {
    _errorMessage = tr("Cannot read from device: Device returned error '%1'").arg(resp.type);
    logError() << _errorMessage;
    return false;
  }

  if (qFromBigEndian(req.length) != qFromBigEndian(resp.length)) {
    _errorMessage = tr("Cannot read from device: Device returned invalid length 0x%1.")
        .arg(qFromBigEndian(resp.length), 4, 16);
    logError() << _errorMessage;
    return false;
  }

  mempcpy(data, resp.data, qFromBigEndian(resp.length));
  return true;
}

bool
OpenGD77Interface::setFlashSector(uint32_t addr) {
  WriteRequest req; req.initSetFlashSector(addr);
  WriteResponse resp;

  if (5 != QSerialPort::write((const char *)&req, 5)) {
    _errorMessage = tr("Cannot write to serial port: %1").arg(USBSerial::errorMessage());
    logError() << _errorMessage;
    return false;
  }

  if (! waitForReadyRead(1000)) {
    _errorMessage = tr("Cannot read from serial port: Timeout!");
    logError() << _errorMessage;
    return false;
  }

  int retlen = QSerialPort::read((char *)&resp, sizeof(WriteResponse));

  if (0 > retlen) {
    _errorMessage = tr("Cannot read from serial port: %1").arg(USBSerial::errorMessage());
    logError() << _errorMessage;
    return false;
  } else if (0 == retlen) {
    _errorMessage = tr("Cannot set flash sector: Device returned empty message.");
    logError() << _errorMessage;
    return false;
  }

  if ((req.type != resp.type) || (req.command != resp.command)) {
    _errorMessage = tr("Cannot set flash sector: Device returned error '%1'").arg(resp.type);
    logError() << _errorMessage;
    return false;
  }

  return true;
}

bool
OpenGD77Interface::writeFlash(uint32_t addr, const uint8_t *data, uint16_t len) {
  WriteRequest req; req.initWriteFlash(addr, data, len);
  WriteResponse resp;

  if ((8+len) != QSerialPort::write((const char *)&req, 8+len)) {
    _errorMessage = tr("Cannot write to serial port: %1").arg(USBSerial::errorMessage());
    logError() << _errorMessage;
    return false;
  }

  if (! waitForReadyRead(1000)) {
    _errorMessage = tr("Cannot read from serial port: Timeout!");
    logError() << _errorMessage;
    return false;
  }

  int retlen = QSerialPort::read((char *)&resp, sizeof(WriteResponse));

  if (0 > retlen) {
    _errorMessage = tr("Cannot read from serial port: %1").arg(USBSerial::errorMessage());
    logError() << _errorMessage;
    return false;
  } else if (0 == retlen) {
    _errorMessage = tr("Cannot write to buffer: Device returned empty message.");
    logError() << _errorMessage;
    return false;
  }

  if ((req.type != resp.type) || (req.command != resp.command)) {
    _errorMessage = tr("Cannot write to buffer at %1: Device returned error '%2'").arg(addr, 6, 16).arg(resp.type);
    logError() << _errorMessage;
    return false;
  }

  return true;
}

bool
OpenGD77Interface::finishWriteFlash() {
  WriteRequest req; req.initFinishWriteFlash();
  WriteResponse resp;

  if ((2) != QSerialPort::write((const char *)&req, 2)) {
    _errorMessage = tr("Cannot write to serial port: %1").arg(USBSerial::errorMessage());
    logError() << _errorMessage;
    return false;
  }

  if (! waitForReadyRead(1000)) {
    _errorMessage = tr("Cannot read from serial port: Timeout!");
    logError() << _errorMessage;
    return false;
  }

  int retlen = QSerialPort::read((char *)&resp, sizeof(WriteResponse));

  if (0 > retlen) {
    _errorMessage = tr("Cannot read from serial port: %1").arg(USBSerial::errorMessage());
    logError() << _errorMessage;
    return false;
  } else if (0 == retlen) {
    _errorMessage = tr("Cannot write to flash: Device returned empty message.");
    logError() << _errorMessage;
    return false;
  }

  if ((req.type != resp.type) || (req.command != resp.command)) {
    _errorMessage = tr("Cannot write to flash: Device returned error '%1'").arg(resp.type);
    logError() << _errorMessage;
    return false;
  }

  return true;
}

