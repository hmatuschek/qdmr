#include "opengd77interface.hh"
#include "logger.hh"
#include <QtEndian>

#define BLOCK_SIZE  32
#define SECTOR_SIZE 4096

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
  // pass...
  return true;
}

bool
OpenGD77Interface::write(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes) {
  int32_t sector = addr/SECTOR_SIZE;

  if (addr < 0x10000) {
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

  ReadRequest req;
  ReadResponse resp;
  for (int i=0; i<nbytes; i+=32) {
    // Assemble read request
    req.type = 'R';
    // If address < 0x10000 -> EEPROM else SPI FLASH
    req.command = (0 == bank) ? ReadRequest::READ_EEPROM : ReadRequest::READ_FLASH;
    req.address = qToBigEndian(addr + i);
    req.length = qToBigEndian(32);
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

    mempcpy(data+i, resp.data, qFromBigEndian(resp.length));
  }

  return true;
}

bool
OpenGD77Interface::read_finish() {
  // pass...
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

