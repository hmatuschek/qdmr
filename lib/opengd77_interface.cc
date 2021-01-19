#include "opengd77_interface.hh"
#include "logger.hh"
#include <QtEndian>

#define BLOCK_SIZE  32
#define SECTOR_SIZE 4096
#define ALIGN_BLOCK_SIZE(n) ((0==((n)%BLOCK_SIZE)) ? (n) : (n)+(BLOCK_SIZE-((n)%BLOCK_SIZE)))

/* ********************************************************************************************* *
 * Implementation of OpenGD77Interface::ReadRequest
 * ********************************************************************************************* */
bool
OpenGD77Interface::ReadRequest::initReadEEPROM(uint32_t addr, uint16_t length) {
  this->type = 'R';
  this->command = READ_EEPROM;
  this->address = qToBigEndian(addr);
  this->length = qToBigEndian(length);
  return true;
}

bool
OpenGD77Interface::ReadRequest::initReadFlash(uint32_t addr, uint16_t length) {
  this->type = 'R';
  this->command = READ_FLASH;
  this->address = qToBigEndian(addr);
  this->length = qToBigEndian(length);
  return true;
}


/* ********************************************************************************************* *
 * Implementation of OpenGD77Interface::WriteRequest
 * ********************************************************************************************* */
bool
OpenGD77Interface::WriteRequest::initWriteEEPROM(uint32_t addr, const uint8_t *data, uint16_t size) {
  if (size > 32)
    size = 32;
  this->type = 'W';
  this->command = WRITE_EEPROM;
  this->payload.address = qToBigEndian(addr);
  this->payload.length = qToBigEndian(size);
  memcpy(this->payload.data, data, size);
  return true;
}

bool
OpenGD77Interface::WriteRequest::initSetFlashSector(uint32_t addr) {
  uint32_t sec = addr/SECTOR_SIZE;
  this->type = 'W';
  this->command = SET_FLASH_SECTOR;
  this->sector[0] = ((sec>>16) & 0xff);
  this->sector[1] = ((sec>>8) & 0xff);
  this->sector[2] = (sec & 0xff);
  return true;
}

bool
OpenGD77Interface::WriteRequest::initWriteFlash(uint32_t addr, const uint8_t *data, uint16_t size) {
  if (size > 32)
    size = 32;
  this->type = 'W';
  this->command = WRITE_SECTOR_BUFFER;
  this->payload.address = qToBigEndian(addr);
  this->payload.length = qToBigEndian(size);
  memcpy(this->payload.data, data, size);
  return true;
}

bool
OpenGD77Interface::WriteRequest::initFinishWriteFlash() {
  this->type = 'W';
  this->command = WRITE_FLASH_SECTOR;
  return true;
}


/* ********************************************************************************************* *
 * Implementation of OpenGD77Interface::CommandRequest
 * ********************************************************************************************* */
void
OpenGD77Interface::CommandRequest::initShowCPSScreen() {
  this->type = 'C';
  this->command = SHOW_CPS_SCREEN;
  this->x = 0;
  this->y = 0;
  this->size = 0;
  this->alignment = 0;
  this->inverted = 0;
  memset(this->message, 0, sizeof(this->message));
}

void
OpenGD77Interface::CommandRequest::initClearScreen() {
  this->type = 'C';
  this->command = CLEAR_SCREEN;
  this->x = 0;
  this->y = 0;
  this->size = 0;
  this->alignment = 0;
  this->inverted = 0;
  memset(this->message, 0, sizeof(this->message));
}

void
OpenGD77Interface::CommandRequest::initDisplay(uint8_t x, uint8_t y,
                                               const char *message, uint8_t iSize,
                                               uint8_t alignment, uint8_t inverted) {
  this->type = 'C';
  this->command = DISPLAY;
  this->x = x;
  this->y = y;
  this->size = std::min(iSize, uint8_t(16));
  this->alignment = alignment;
  this->inverted = inverted;
  strcpy(this->message, message);
}

void
OpenGD77Interface::CommandRequest::initRenderCPS() {
  this->type = 'C';
  this->command = RENDER_CPS;
  this->x = 0;
  this->y = 0;
  this->size = 0;
  this->alignment = 0;
  this->inverted = 0;
  memset(this->message, 0, sizeof(this->message));
}

void
OpenGD77Interface::CommandRequest::initCloseScreen() {
  this->type = 'C';
  this->command = CLOSE_CPS_SCREEN;
  this->x = 0;
  this->y = 0;
  this->size = 0;
  this->alignment = 0;
  this->inverted = 0;
  memset(this->message, 0, sizeof(this->message));
}

void
OpenGD77Interface::CommandRequest::initCommand(Option option) {
  this->type = 'C';
  this->command = COMMAND;
  this->option = option;
  this->y = 0;
  this->size = 0;
  this->alignment = 0;
  this->inverted = 0;
  memset(this->message, 0, sizeof(this->message));
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
OpenGD77Interface::write_start(uint32_t bank, uint32_t addr)
{
  if (! sendShowCPSScreen())
    return false;
  if (! sendClearScreen())
    return false;
  if (! sendDisplay(0, 0, "qDMR", 3, 1, 0))
    return false;
  if (! sendDisplay(0, 16, "Writing", 3, 1, 0))
    return false;
  if (! sendDisplay(0, 32, "Codeplug", 3, 1, 0))
    return false;
  if (! sendRenderCPS())
    return false;
  if (! sendCommand(CommandRequest::FLASH_RED_LED))
    return false;
  if (! sendCommand(CommandRequest::SAVE_SETTINGS_AND_VFOS))
    return false;

  if (EEPROM == bank) {
    if (_sector >= 0) {
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
    for (int i=0; i<nbytes; i+=BLOCK_SIZE) {
      if (! writeEEPROM(addr+i, data+i, BLOCK_SIZE)) {
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
    for (int i=0; i<nbytes; i+=BLOCK_SIZE) {
      if (! writeFlash(addr+i, data+i, BLOCK_SIZE)) {
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
  if (0 > _sector)
    return true;
  _sector = -1;
  if (! finishWriteFlash())
    return false;
  if (! sendCloseScreen())
    return false;
  return true;
}

bool
OpenGD77Interface::read_start(uint32_t bank, uint32_t addr) {
  if (! sendShowCPSScreen())
    return false;
  if (! sendClearScreen())
    return false;
  if (! sendDisplay(0, 0, "qDMR", 3, 1, 0))
    return false;
  if (! sendDisplay(0, 16, "Reading", 3, 1, 0))
    return false;
  if (! sendDisplay(0, 32, "Codeplug", 3, 1, 0))
    return false;
  if (! sendRenderCPS())
    return false;
  if (! sendCommand(CommandRequest::FLASH_GREEN_LED))
    return false;
  if (! sendCommand(CommandRequest::SAVE_SETTINGS_AND_VFOS))
    return false;

  return true;
}

bool
OpenGD77Interface::read(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes) {
  if (! isOpen()) {
    _errorMessage = "Cannot read block: Device not open!";
    logError() << _errorMessage;
    return false;
  }

  for (int i=0; i<nbytes; i+=BLOCK_SIZE) {
    bool ok;
    if (EEPROM == bank)
      ok = readEEPROM(addr+i, data+i, BLOCK_SIZE);
    else if (FLASH == bank)
      ok = readFlash(addr+i, data+i, BLOCK_SIZE);
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
  if (! sendCloseScreen())
    return false;

  return true;
}

bool
OpenGD77Interface::reboot() {
  return sendCommand(CommandRequest::SAVE_SETTINGS_NOT_VFOS);
}


bool
OpenGD77Interface::readEEPROM(uint32_t addr, uint8_t *data, uint16_t len) {
  if (! isOpen()) {
    _errorMessage = "Cannot read block: Device not open!";
    logError() << __FILE__ << ": " << _errorMessage;
    return false;
  }

  ReadRequest req; req.initReadEEPROM(addr, BLOCK_SIZE);
  if (sizeof(ReadRequest) != QSerialPort::write((const char *)&req, sizeof(ReadRequest))) {
    _errorMessage = tr("Cannot write to serial port: %1").arg(USBSerial::errorMessage());
    logError() << __FILE__ << ": " << _errorMessage;
    return false;
  }

  if (! waitForReadyRead(1000)) {
    _errorMessage = tr("Cannot read from serial port: Timeout!");
    logError() << __FILE__ << ": " << _errorMessage;
    return false;
  }

  ReadResponse resp;
  int retlen = QSerialPort::read((char *)&resp, sizeof(ReadResponse));

  if (0 > retlen) {
    _errorMessage = tr("Cannot read from serial port: %1").arg(USBSerial::errorMessage());
    logError() << __FILE__ << ": " << _errorMessage;
    return false;
  } else if (0 == retlen) {
    _errorMessage = tr("Cannot read from serial port: Device returned empty message.");
    logError() << __FILE__ << ": " << _errorMessage;
    return false;
  }

  if ('R' != resp.type) {
    _errorMessage = tr("Cannot read from device: Device returned error '%1'").arg(resp.type);
    logError() << __FILE__ << ": " << _errorMessage;
    return false;
  }

  if (qFromBigEndian(req.length) != qFromBigEndian(resp.length)) {
    _errorMessage = tr("Cannot read from device: Device returned invalid length 0x%1.")
        .arg(qFromBigEndian(resp.length), 4, 16);
    logError() << __FILE__ << ": " << _errorMessage;
    return false;
  }

  memcpy(data, resp.data, qFromBigEndian(resp.length));
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

  ReadRequest req;
  req.initReadFlash(addr, BLOCK_SIZE);
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

  memcpy(data, resp.data, qFromBigEndian(resp.length));
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
  WriteRequest req;
  req.initFinishWriteFlash();
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

bool
OpenGD77Interface::sendShowCPSScreen() {
  CommandRequest req;
  uint8_t resp;
  req.initShowCPSScreen();

  if (sizeof(CommandRequest) != QSerialPort::write((const char *) &req, sizeof(CommandRequest))) {
    _errorMessage = tr("Cannot write to serial port: %1").arg(USBSerial::errorMessage());
    logError() << _errorMessage;
    return false;
  }

  if (! waitForReadyRead(1000)) {
    _errorMessage = tr("Cannot read from serial port: Timeout!");
    logError() << _errorMessage;
    return false;
  }

  int retlen = QSerialPort::read((char *)&resp, 1);
  if (0 > retlen) {
    _errorMessage = tr("Cannot read from serial port: %1").arg(USBSerial::errorMessage());
    logError() << _errorMessage;
    return false;
  } else if (0 == retlen) {
    _errorMessage = tr("Cannot send command: Device returned empty message.");
    logError() << _errorMessage;
    return false;
  } else if ('-' != resp) {
    _errorMessage = tr("Cannot send command: Deviced returned unexpected response '%1'").arg(resp);
    logError() << _errorMessage;
    return false;
  }

  return true;
}

bool
OpenGD77Interface::sendClearScreen() {
  CommandRequest req;
  req.initClearScreen();
  uint8_t resp;

  if (sizeof(CommandRequest) != QSerialPort::write((const char *) &req, sizeof(CommandRequest))) {
    _errorMessage = tr("Cannot write to serial port: %1").arg(USBSerial::errorMessage());
    logError() << _errorMessage;
    return false;
  }

  if (! waitForReadyRead(1000)) {
    _errorMessage = tr("Cannot read from serial port: Timeout!");
    logError() << _errorMessage;
    return false;
  }

  int retlen = QSerialPort::read((char *)&resp, 1);

  if (0 > retlen) {
    _errorMessage = tr("Cannot read from serial port: %1").arg(USBSerial::errorMessage());
    logError() << _errorMessage;
    return false;
  } else if (0 == retlen) {
    _errorMessage = tr("Cannot send command: Device returned empty message.");
    logError() << _errorMessage;
    return false;
  } else if ('-' != resp) {
    _errorMessage = tr("Cannot send command: Deviced returned unexpected response '%1'").arg(resp);
    logError() << _errorMessage;
    return false;
  }

  return true;
}

bool
OpenGD77Interface::sendDisplay(uint8_t x, uint8_t y, const char *message, uint8_t iSize, uint8_t alignment, uint8_t inverted) {
  CommandRequest req;
  req.initDisplay(x,y, message, iSize, alignment, inverted);
  uint8_t resp;

  if (sizeof(CommandRequest) != QSerialPort::write((const char *) &req, sizeof(CommandRequest))) {
    _errorMessage = tr("Cannot write to serial port: %1").arg(USBSerial::errorMessage());
    logError() << _errorMessage;
    return false;
  }

  if (! waitForReadyRead(1000)) {
    _errorMessage = tr("Cannot read from serial port: Timeout!");
    logError() << _errorMessage;
    return false;
  }

  int retlen = QSerialPort::read((char *)&resp, 1);

  if (0 > retlen) {
    _errorMessage = tr("Cannot read from serial port: %1").arg(USBSerial::errorMessage());
    logError() << _errorMessage;
    return false;
  } else if (0 == retlen) {
    _errorMessage = tr("Cannot send command: Device returned empty message.");
    logError() << _errorMessage;
    return false;
  } else if ('-' != resp) {
    _errorMessage = tr("Cannot send command: Deviced returned unexpected response '%1'").arg(resp);
    logError() << _errorMessage;
    return false;
  }

  return true;
}

bool
OpenGD77Interface::sendRenderCPS() {
  CommandRequest req;
  req.initRenderCPS();

  if (sizeof(CommandRequest) != QSerialPort::write((const char *) &req, sizeof(CommandRequest))) {
    _errorMessage = tr("Cannot write to serial port: %1").arg(USBSerial::errorMessage());
    logError() << _errorMessage;
    return false;
  }

  if (! waitForReadyRead(1000)) {
    _errorMessage = tr("Cannot read from serial port: Timeout!");
    logError() << _errorMessage;
    return false;
  }

  uint8_t resp;
  int retlen = QSerialPort::read((char *)&resp, 1);

  if (0 > retlen) {
    _errorMessage = tr("Cannot read from serial port: %1").arg(USBSerial::errorMessage());
    logError() << _errorMessage;
    return false;
  } else if (0 == retlen) {
    _errorMessage = tr("Cannot send command: Device returned empty message.");
    logError() << _errorMessage;
    return false;
  } else if ('-' != resp) {
    _errorMessage = tr("Cannot send command: Deviced returned unexpected response '%1'").arg(resp);
    logError() << _errorMessage;
    return false;
  }

  return true;
}

bool
OpenGD77Interface::sendCloseScreen() {
  CommandRequest req; req.initCloseScreen();
  uint8_t resp;

  if (sizeof(CommandRequest) != QSerialPort::write((const char *) &req, sizeof(CommandRequest))) {
    _errorMessage = tr("Cannot write to serial port: %1").arg(USBSerial::errorMessage());
    logError() << _errorMessage;
    return false;
  }

  if (! waitForReadyRead(1000)) {
    _errorMessage = tr("Cannot read from serial port: Timeout!");
    logError() << _errorMessage;
    return false;
  }

  int retlen = QSerialPort::read((char *)&resp, 1);

  if (0 > retlen) {
    _errorMessage = tr("Cannot read from serial port: %1").arg(USBSerial::errorMessage());
    logError() << _errorMessage;
    return false;
  } else if (0 == retlen) {
    _errorMessage = tr("Cannot send command: Device returned empty message.");
    logError() << _errorMessage;
    return false;
  } else if ('-' != resp) {
    _errorMessage = tr("Cannot send command: Deviced returned unexpected response '%1'").arg(resp);
    logError() << _errorMessage;
    return false;
  }

  return true;
}

bool
OpenGD77Interface::sendCommand(CommandRequest::Option option) {
  CommandRequest req; req.initCommand(option);
  uint8_t resp;

  if (sizeof(CommandRequest) != QSerialPort::write((const char *) &req, sizeof(CommandRequest))) {
    _errorMessage = tr("Cannot write to serial port: %1").arg(USBSerial::errorMessage());
    logError() << _errorMessage;
    return false;
  }

  if (! waitForReadyRead(1000)) {
    _errorMessage = tr("Cannot read from serial port: Timeout!");
    logError() << _errorMessage;
    return false;
  }

  int retlen = QSerialPort::read((char *)&resp, 1);

  if (0 > retlen) {
    _errorMessage = tr("Cannot read from serial port: %1").arg(USBSerial::errorMessage());
    logError() << _errorMessage;
    return false;
  } else if (0 == retlen) {
    _errorMessage = tr("Cannot send command: Device returned empty message.");
    logError() << _errorMessage;
    return false;
  } else if ('-' != resp) {
    _errorMessage = tr("Cannot send command: Deviced returned unexpected response '%1'").arg(resp);
    logError() << _errorMessage;
    return false;
  }

  return true;
}






