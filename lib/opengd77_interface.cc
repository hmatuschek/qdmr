#include "opengd77_interface.hh"
#include "logger.hh"
#include "radioinfo.hh"
#include <QtEndian>

#define USB_VID 0x1fc9
#define USB_PID 0x0094

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
OpenGD77Interface::OpenGD77Interface(const USBDeviceDescriptor &descr, const ErrorStack &err, QObject *parent)
  : USBSerial(descr, err, parent), _sector(-1)
{
  // pass...
}

OpenGD77Interface::~OpenGD77Interface() {
  // pass...
}

USBDeviceInfo
OpenGD77Interface::interfaceInfo() {
  return USBDeviceInfo(USBDeviceInfo::Class::Serial, USB_VID, USB_PID);
}

QList<USBDeviceDescriptor>
OpenGD77Interface::detect() {
  return USBSerial::detect(USB_VID, USB_PID);
}

void
OpenGD77Interface::close() {
  if (isOpen())
    USBSerial::close();
}

RadioInfo
OpenGD77Interface::identifier(const ErrorStack &err) {
  Q_UNUSED(err);
  if (isOpen())
    return RadioInfo::byID(RadioInfo::OpenGD77);
  else
    return RadioInfo();
}

bool
OpenGD77Interface::write_start(uint32_t bank, uint32_t addr, const ErrorStack &err)
{
  logDebug() << "Send enter prog mode ...";
  if (! sendShowCPSScreen(err))
    return false;
  //logDebug() << "Send clear screen ...";
  if (! sendClearScreen(err))
    return false;
  //logDebug() << "Send display text ...";
  if (! sendDisplay(0, 0, "qDMR", 3, 1, 0, err))
    return false;
  if (! sendDisplay(0, 16, "Writing", 3, 1, 0, err))
    return false;
  if (! sendDisplay(0, 32, "Codeplug", 3, 1, 0, err))
    return false;
  //logDebug() << "Send 'render CPS' ...";
  if (! sendRenderCPS(err))
    return false;
  //logDebug() << "Send 'flash red LED' ...";
  if (! sendCommand(CommandRequest::FLASH_RED_LED, err))
    return false;
  //logDebug() << "Send save settings and VFOs ...";
  if (! sendCommand(CommandRequest::SAVE_SETTINGS_AND_VFOS, err))
    return false;

  if (EEPROM == bank) {
    if (_sector >= 0) {
      if (! finishWriteFlash(err))
        return false;
    }
    _sector = -1;
  } else if (FLASH == bank) {
    int32_t sector = addr/SECTOR_SIZE;
    if ((-1 != _sector) && (_sector != sector)) {
      if (! finishWriteFlash(err))
        return false;
    }
  }
  return true;
}

bool
OpenGD77Interface::write(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes, const ErrorStack &err)
{
  if (EEPROM == bank) {
    if ((0 <= _sector) && (! finishWriteFlash(err)))
      return false;
    for (int i=0; i<nbytes; i+=BLOCK_SIZE) {
      if (! writeEEPROM(addr+i, data+i, BLOCK_SIZE, err)) {
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
OpenGD77Interface::write_finish(const ErrorStack &err) {
  _sector = -1;
  if (0 > _sector)
    return true;
  _sector = -1;
  if (! finishWriteFlash(err))
    return false;
  if (! sendCloseScreen(err))
    return false;
  return true;
}

bool
OpenGD77Interface::read_start(uint32_t bank, uint32_t addr, const ErrorStack &err) {
  Q_UNUSED(bank); Q_UNUSED(addr)

  if (! sendShowCPSScreen(err))
    return false;
  if (! sendClearScreen(err))
    return false;
  if (! sendDisplay(0, 0, "qDMR", 3, 1, 0, err))
    return false;
  if (! sendDisplay(0, 16, "Reading", 3, 1, 0, err))
    return false;
  if (! sendDisplay(0, 32, "Codeplug", 3, 1, 0, err))
    return false;
  if (! sendRenderCPS(err))
    return false;
  if (! sendCommand(CommandRequest::FLASH_GREEN_LED, err))
    return false;
  if (! sendCommand(CommandRequest::SAVE_SETTINGS_AND_VFOS, err))
    return false;

  return true;
}

bool
OpenGD77Interface::read(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes, const ErrorStack &err) {
  if (! isOpen()) {
    errMsg(err) << "Cannot read block: Device not open!";
    return false;
  }

  for (int i=0; i<nbytes; i+=BLOCK_SIZE) {
    bool ok;
    if (EEPROM == bank)
      ok = readEEPROM(addr+i, data+i, BLOCK_SIZE, err);
    else if (FLASH == bank)
      ok = readFlash(addr+i, data+i, BLOCK_SIZE, err);
    else {
      errMsg(err) << "Cannot read from bank " << bank << ": Unknown memory bank.";
      return false;
    }

    if (! ok)
      return false;
  }

  return true;
}

bool
OpenGD77Interface::read_finish(const ErrorStack &err) {
  if (! sendCloseScreen(err))
    return false;

  return true;
}

bool
OpenGD77Interface::reboot(const ErrorStack &err) {
  return sendCommand(CommandRequest::SAVE_SETTINGS_NOT_VFOS, err);
}


bool
OpenGD77Interface::readEEPROM(uint32_t addr, uint8_t *data, uint16_t len, const ErrorStack &err) {
  Q_UNUSED(len)

  if (! isOpen()) {
    errMsg(err) << "Cannot read block: Device not open!";
    return false;
  }

  ReadRequest req; req.initReadEEPROM(addr, BLOCK_SIZE);
  if (sizeof(ReadRequest) != QSerialPort::write((const char *)&req, sizeof(ReadRequest))) {
    errMsg(err) << "Cannot write to serial port.";
    return false;
  }

  if (! waitForReadyRead(1000)) {
    errMsg(err) << "Cannot read from serial port: Timeout!";
    return false;
  }

  ReadResponse resp;
  int retlen = QSerialPort::read((char *)&resp, sizeof(ReadResponse));

  if (0 > retlen) {
    errMsg(err) << "Cannot read from serial port";
    return false;
  } else if (0 == retlen) {
    errMsg(err) << "Cannot read from serial port: Device returned empty message.";
    return false;
  }

  if ('R' != resp.type) {
    errMsg(err) << "Cannot read from device: Device returned error '" << resp.type << "'.";
    return false;
  }

  if (qFromBigEndian(req.length) != qFromBigEndian(resp.length)) {
    errMsg(err) << "Cannot read from device: Device returned invalid length " <<
                qFromBigEndian(resp.length) << ".";
    return false;
  }

  memcpy(data, resp.data, qFromBigEndian(resp.length));
  return true;
}


bool
OpenGD77Interface::writeEEPROM(uint32_t addr, const uint8_t *data, uint16_t len, const ErrorStack &err) {
  WriteRequest req; req.initWriteEEPROM(addr, data, len);
  WriteResponse resp;

  if ((8+len) != QSerialPort::write((const char *)&req, 8+len)) {
    errMsg(err) << "Cannot write to serial port.";
    return false;
  }

  if (! waitForReadyRead(1000)) {
    errMsg(err) << "Cannot read from serial port: Timeout!";
    return false;
  }

  int retlen = QSerialPort::read((char *)&resp, sizeof(WriteResponse));

  if (0 > retlen) {
    errMsg(err) << "Cannot read from serial port.";
    return false;
  } else if (0 == retlen) {
    errMsg(err) << "Cannot write EEPROM: Device returned empty message.";
    return false;
  }

  if ((req.type != resp.type) || (req.command != resp.command)) {
    errMsg(err) << "Cannot write EEPROM at " << QString::number(addr, 16)
             << ": Device returned error " << resp.type << ".";
    return false;
  }

  return true;
}


bool
OpenGD77Interface::readFlash(uint32_t addr, uint8_t *data, uint16_t len, const ErrorStack &err) {
  Q_UNUSED(len)

  if (! isOpen()) {
    errMsg(err) << "Cannot read block: Device not open!";
    return false;
  }

  ReadRequest req;
  req.initReadFlash(addr, BLOCK_SIZE);
  if (sizeof(ReadRequest) != QSerialPort::write((const char *)&req, sizeof(ReadRequest))) {
    errMsg(err) << QSerialPort::errorString();
    errMsg(err) << "Cannot write to serial port.";
    return false;
  }

  if (! waitForReadyRead(1000)) {
    errMsg(err) << QSerialPort::errorString();
    errMsg(err) << "Cannot read from serial port: Timeout!";
    return false;
  }

  ReadResponse resp;
  int retlen = QSerialPort::read((char *)&resp, sizeof(ReadResponse));

  if (0 > retlen) {
    errMsg(err) << QSerialPort::errorString();
    errMsg(err) << "Cannot read from serial port.";
    return false;
  } else if (0 == retlen) {
    errMsg(err) << "Cannot read from serial port: Device returned empty message.";
    return false;
  }

  if ('R' != resp.type) {
    errMsg(err) << "Cannot read from device: Device returned error " << resp.type << ".";
    return false;
  }

  if (qFromBigEndian(req.length) != qFromBigEndian(resp.length)) {
    errMsg(err) << "Cannot read from device: Device returned invalid length "
                << qFromBigEndian(resp.length) << ".";
    return false;
  }

  memcpy(data, resp.data, qFromBigEndian(resp.length));
  return true;
}

bool
OpenGD77Interface::setFlashSector(uint32_t addr, const ErrorStack &err) {
  WriteRequest req; req.initSetFlashSector(addr);
  WriteResponse resp;

  if (5 != QSerialPort::write((const char *)&req, 5)) {
    errMsg(err) << QSerialPort::errorString();
    errMsg(err) << "Cannot write to serial port.";
    return false;
  }

  if (! waitForReadyRead(1000)) {
    errMsg(err) << QSerialPort::errorString();
    errMsg(err) << "Cannot read from serial port: Timeout!";
    return false;
  }

  int retlen = QSerialPort::read((char *)&resp, sizeof(WriteResponse));

  if (0 > retlen) {
    errMsg(err) << QSerialPort::errorString();
    errMsg(err) << "Cannot read from serial port.";
    return false;
  } else if (0 == retlen) {
    errMsg(err) << "Cannot set flash sector: Device returned empty message.";
    return false;
  }

  if ((req.type != resp.type) || (req.command != resp.command)) {
    errMsg(err) << "Cannot set flash sector: Device returned error " << resp.type << ".";
    return false;
  }

  return true;
}

bool
OpenGD77Interface::writeFlash(uint32_t addr, const uint8_t *data, uint16_t len, const ErrorStack &err) {
  WriteRequest req; req.initWriteFlash(addr, data, len);
  WriteResponse resp;

  if ((8+len) != QSerialPort::write((const char *)&req, 8+len)) {
    errMsg(err) << QSerialPort::errorString();
    errMsg(err) << "Cannot write to serial port.";
    return false;
  }

  if (! waitForReadyRead(1000)) {
    errMsg(err) << QSerialPort::errorString();
    errMsg(err) << "Cannot read from serial port: Timeout!";
    return false;
  }

  int retlen = QSerialPort::read((char *)&resp, sizeof(WriteResponse));

  if (0 > retlen) {
    errMsg(err) << QSerialPort::errorString();
    errMsg(err) << "Cannot read from serial port.";
    return false;
  } else if (0 == retlen) {
    errMsg(err) << "Cannot write to buffer: Device returned empty message.";
    return false;
  }

  if ((req.type != resp.type) || (req.command != resp.command)) {
    errMsg(err) << "Cannot write to buffer at " << QString::number(addr,16)
             << ": Device returned error " << resp.type << ".";
    return false;
  }

  return true;
}

bool
OpenGD77Interface::finishWriteFlash(const ErrorStack &err) {
  //logDebug() << "Send finish write flash command ...";
  WriteRequest req;
  req.initFinishWriteFlash();
  WriteResponse resp;

  if ((2) != QSerialPort::write((const char *)&req, 2)) {
    errMsg(err) << "Cannot write to serial port.";
    return false;
  }

  if (! waitForReadyRead(1000)) {
    errMsg(err) << "Cannot read from serial port: Timeout!";
    return false;
  }

  int retlen = QSerialPort::read((char *)&resp, sizeof(WriteResponse));

  if (0 > retlen) {
    errMsg(err) << "Cannot read from serial port.";
    return false;
  } else if (0 == retlen) {
    errMsg(err) << "Cannot write to flash: Device returned empty message.";
    return false;
  }

  if ((req.type != resp.type) || (req.command != resp.command)) {
    errMsg(err) << "Cannot write to flash: Device returned error " << resp.type << ".";
    return false;
  }

  return true;
}

bool
OpenGD77Interface::sendShowCPSScreen(const ErrorStack &err) {
  CommandRequest req;
  uint8_t resp;
  req.initShowCPSScreen();

  if (sizeof(CommandRequest) != QSerialPort::write((const char *) &req, sizeof(CommandRequest))) {
    errMsg(err) << "Cannot write to serial port.";
    return false;
  }

  if (! waitForReadyRead(1000)) {
    errMsg(err) << "Cannot read from serial port: Timeout!";
    return false;
  }

  int retlen = QSerialPort::read((char *)&resp, 1);
  if (0 > retlen) {
    errMsg(err) << "Cannot read from serial port.";
    return false;
  } else if (0 == retlen) {
    errMsg(err) << "Cannot send command: Device returned empty message.";
    return false;
  } else if ('-' != resp) {
    errMsg(err) << "Cannot send command: Deviced returned unexpected response '" << (char)resp << "'.";
    return false;
  }

  return true;
}

bool
OpenGD77Interface::sendClearScreen(const ErrorStack &err) {
  CommandRequest req;
  req.initClearScreen();
  uint8_t resp;

  if (sizeof(CommandRequest) != QSerialPort::write((const char *) &req, sizeof(CommandRequest))) {
    errMsg(err) << QSerialPort::errorString();
    errMsg(err) << "Cannot write to serial port.";
    return false;
  }

  if (! waitForReadyRead(1000)) {
    errMsg(err) << "Cannot read from serial port: Timeout!";
    return false;
  }

  int retlen = QSerialPort::read((char *)&resp, 1);

  if (0 > retlen) {
    errMsg(err) << QSerialPort::errorString();
    errMsg(err) << "Cannot read from serial port.";
    return false;
  } else if (0 == retlen) {
    errMsg(err) << "Cannot send command: Device returned empty message.";
    return false;
  } else if ('-' != resp) {
    errMsg(err) << "Cannot send command: Deviced returned unexpected response '" << (char)resp << "'.";
    return false;
  }

  return true;
}

bool
OpenGD77Interface::sendDisplay(uint8_t x, uint8_t y, const char *message, uint8_t iSize, uint8_t alignment, uint8_t inverted, const ErrorStack &err) {
  CommandRequest req;
  req.initDisplay(x,y, message, iSize, alignment, inverted);
  uint8_t resp;

  if (sizeof(CommandRequest) != QSerialPort::write((const char *) &req, sizeof(CommandRequest))) {
    errMsg(err) << "Cannot write to serial port.";
    return false;
  }

  if (! waitForReadyRead(1000)) {
    errMsg(err) << "Cannot read from serial port: Timeout!";
    return false;
  }

  int retlen = QSerialPort::read((char *)&resp, 1);

  if (0 > retlen) {
    errMsg(err) << "Cannot read from serial port.";
    return false;
  } else if (0 == retlen) {
    errMsg(err) << "Cannot send command: Device returned empty message.";
    return false;
  } else if ('-' != resp) {
    errMsg(err) << "Cannot send command: Deviced returned unexpected response '" << (char)resp << "'.";
    return false;
  }

  return true;
}

bool
OpenGD77Interface::sendRenderCPS(const ErrorStack &err) {
  CommandRequest req;
  req.initRenderCPS();

  if (sizeof(CommandRequest) != QSerialPort::write((const char *) &req, sizeof(CommandRequest))) {
    errMsg(err) << "Cannot write to serial port.";
    return false;
  }

  if (! waitForReadyRead(1000)) {
    errMsg(err) << "Cannot read from serial port: Timeout!";
    return false;
  }

  uint8_t resp;
  int retlen = QSerialPort::read((char *)&resp, 1);

  if (0 > retlen) {
    errMsg(err) << "Cannot read from serial port.";
    return false;
  } else if (0 == retlen) {
    errMsg(err) << "Cannot send command: Device returned empty message.";
    return false;
  } else if ('-' != resp) {
    errMsg(err) << "Cannot send command: Deviced returned unexpected response '" << (char)resp << "'.";
    return false;
  }

  return true;
}

bool
OpenGD77Interface::sendCloseScreen(const ErrorStack &err) {
  CommandRequest req; req.initCloseScreen();
  uint8_t resp;

  if (sizeof(CommandRequest) != QSerialPort::write((const char *) &req, sizeof(CommandRequest))) {
    errMsg(err) << "Cannot write to serial port.";
    return false;
  }

  if (! waitForReadyRead(1000)) {
    errMsg(err) << "Cannot read from serial port: Timeout!";
    return false;
  }

  int retlen = QSerialPort::read((char *)&resp, 1);

  if (0 > retlen) {
    errMsg(err) << "Cannot read from serial port.";
    return false;
  } else if (0 == retlen) {
    errMsg(err) << "Cannot send command: Device returned empty message.";
    return false;
  } else if ('-' != resp) {
    errMsg(err) << "Cannot send command: Deviced returned unexpected response '" << (char)resp << "'.";
    return false;
  }

  return true;
}

bool
OpenGD77Interface::sendCommand(CommandRequest::Option option, const ErrorStack &err) {
  CommandRequest req; req.initCommand(option);
  uint8_t resp;

  if (sizeof(CommandRequest) != QSerialPort::write((const char *) &req, sizeof(CommandRequest))) {
    errMsg(err) << "Cannot write to serial port.";
    return false;
  }

  if (! waitForReadyRead(1000)) {
    errMsg(err) << "Cannot read from serial port: Timeout!";
    return false;
  }

  int retlen = QSerialPort::read((char *)&resp, 1);

  if (0 > retlen) {
    errMsg(err) << "Cannot read from serial port.";
    return false;
  } else if (0 == retlen) {
    errMsg(err) << "Cannot send command: Device returned empty message.";
    return false;
  } else if ('-' != resp) {
    errMsg(err) << "Cannot send command: Deviced returned unexpected response '" << (char)resp << "'.";
    return false;
  }

  return true;
}






