#include "radioddity_interface.hh"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "logger.hh"

#define USB_VID 0x15a2
#define USB_PID 0x0073
#define MAX_RETRY 10

static const unsigned char CMD_PRG[]   = "\2PROGRA";
static const unsigned char CMD_PRG2[]  = "M\2";
static const unsigned char CMD_ACK[]   = "A";
static const unsigned char CMD_READ[]  = "Raan";
static const unsigned char CMD_WRITE[] = "Waan...";
static const unsigned char CMD_ENDR[]  = "ENDR";
static const unsigned char CMD_ENDW[]  = "ENDW";
static const unsigned char CMD_CWB0[]  = "CWB\4\0\0\0\0";
static const unsigned char CMD_CWB1[]  = "CWB\4\0\1\0\0";
static const unsigned char CMD_CWB3[]  = "CWB\4\0\3\0\0";
static const unsigned char CMD_CWB4[]  = "CWB\4\0\4\0\0";

RadioddityInterface::RadioddityInterface(const USBDeviceDescriptor &descr, const ErrorStack &err, QObject *parent)
  : HIDevice(descr, err, parent), _current_bank(MEMBANK_NONE), _identifier()
{
  if (isOpen())
    identifier();
}

RadioddityInterface::~RadioddityInterface() {
  if (isOpen())
    close();
}

USBDeviceInfo
RadioddityInterface::interfaceInfo() {
  return USBDeviceInfo(USBDeviceInfo::Class::HID, USB_VID, USB_PID);
}

QList<USBDeviceDescriptor>
RadioddityInterface::detect() {
  return HIDevice::detect(USB_VID, USB_PID);
}

bool
RadioddityInterface::isOpen() const {
  return HIDevice::isOpen();
}

void
RadioddityInterface::close() {
  logDebug() << "Close HID connection.";
  _identifier = RadioInfo();
  HIDevice::close();
}

RadioInfo
RadioddityInterface::identifier(const ErrorStack &err) {
  static unsigned char reply[38];
  unsigned char ack;

  if (_identifier.isValid())
    return _identifier;

  logDebug() << "Radioddity HID interface: Enter program mode.";

  if (! hid_send_recv(CMD_PRG, 7, &ack, 1, err)) {
    errMsg(err) << "Cannot identify radio.";
    return RadioInfo();
  }

  if (ack != CMD_ACK[0]) {
    errMsg(err) << "Cannot identify radio: Wrong PRD acknowledge " << (int)ack <<
                ", expected "<< int(CMD_ACK[0]) << ".";
    return RadioInfo();
  }

  if (! hid_send_recv(CMD_PRG2, 2, reply, 16, err)) {
    errMsg(err) << "Cannot identify radio.";
    return RadioInfo();
  }

  if (! hid_send_recv(CMD_ACK, 1, &ack, 1, err)) {
    errMsg(err) << "Cannot identify radio.";
    return RadioInfo();
  }

  if (ack != CMD_ACK[0]) {
    errMsg(err) << "Cannot identify radio: Wrong PRG2 acknowledge "
                << (int) ack << ", expected " << (int)CMD_ACK[0] << ".";
    return RadioInfo();
  }

  // Reply:
  // 42 46 2d 35 52 ff ff ff 56 32 31 30 00 04 80 04
  //  B  F  -  5  R           V  2  1  0

  // Terminate the string.
  char *p = (char *)memchr(reply, 0xff, sizeof(reply));
  if (p)
    *p = 0;

  if (0 == strcmp((char*)reply, "BF-5R")) {
    _identifier = RadioInfo::byID(RadioInfo::RD5R);
  } else if (0 == strcmp((char*)reply, "MD-760P")) {
    _identifier = RadioInfo::byID(RadioInfo::GD77);
  } else {
    errMsg(err) << "Unknown Radioddity device '" << (char*)reply << "'.";
    return RadioInfo();
  }

  logDebug() << "Got device '" << _identifier.name() << "'.";
  return _identifier;
}


bool
RadioddityInterface::read_start(uint32_t bank, uint32_t addr, const ErrorStack &err) {
  Q_UNUSED(addr)

  if (! selectMemoryBank(MemoryBank(bank), err)) {
    errMsg(err) << "Cannot select memory bank " << bank << ".";
    return false;
  }

  return true;
}

bool
RadioddityInterface::read(uint32_t bank, uint32_t addr, unsigned char *data, int nbytes, const ErrorStack &err)
{
  unsigned char cmd[4], reply[32+4];
  int n;

  if (! selectMemoryBank(MemoryBank(bank), err)) {
    errMsg(err) << "Cannot select memory bank " << bank << ".";
    return false;
  }

  // send data
  for (n=0; n<nbytes; n+=32) {
    cmd[0] = CMD_READ[0];
    cmd[1] = (addr + n) >> 8;
    cmd[2] = addr + n;
    cmd[3] = 32;
    if (! hid_send_recv(cmd, 4, reply, sizeof(reply), err))
      return false;
    else
      memcpy(data + n, reply + 4, 32);
  }

  return true;
}

bool
RadioddityInterface::read_finish(const ErrorStack &err)
{
  unsigned char ack;

  if (! hid_send_recv(CMD_ENDR, 4, &ack, 1, err)) {
    errMsg(err) << "Cannot finish read().";
    return false;
  }
  if (ack != CMD_ACK[0]) {
    errMsg(err) << "Cannot finish read(): Wrong acknowledge "
                << (int)ack << ", expected " << (int)CMD_ACK[0] << ".";
    return false;
  }

  logDebug() << "Left program mode.";

  _identifier = RadioInfo();

  return true;
}


bool
RadioddityInterface::write_start(uint32_t bank, uint32_t addr, const ErrorStack &err) {
  Q_UNUSED(addr)

  if (! selectMemoryBank(MemoryBank(bank), err)) {
    errMsg(err) << "Cannot select memory bank " << bank << ".";
    return false;
  }

  return true;
}

bool
RadioddityInterface::write(uint32_t bank, uint32_t addr, unsigned char *data, int nbytes, const ErrorStack &err)
{
  unsigned char ack, cmd[4+32];

  if (! selectMemoryBank(MemoryBank(bank), err)) {
    errMsg(err) << "Cannot select memory bank " << bank << ".";
    return false;
  }

  // send data
  unsigned int count=0;
  for (int n=0; n<nbytes; n+=32) {
    cmd[0] = CMD_WRITE[0];
    cmd[1] = (addr + n) >> 8;
    cmd[2] = addr + n;
    cmd[3] = 32;
    memcpy(cmd + 4, data + n, 32);
    if (! hid_send_recv(cmd, 4+32, &ack, 1, err))
      return false;
    else if (ack != CMD_ACK[0]) {
      errMsg(err) << "Cannot write block: Wrong acknowledge " << (int)ack
                  << ", expected " << (int)CMD_ACK[0] << ".";
      n-=32;

      if ((++count) > MAX_RETRY) {
        errMsg(err) << "Maximum retry count reached. Abort.";
        return false;
      }
    } else {
      count = 0;
    }
  }

  return true;
}

bool
RadioddityInterface::write_finish(const ErrorStack &err)
{
  unsigned char ack;

  if (! hid_send_recv(CMD_ENDW, 4, &ack, 1, err)) {
    errMsg(err) << "Cannot finish write().";
    return false;
  }
  if (ack != CMD_ACK[0]) {
    errMsg(err) << "Cannot finish write(): Wrong acknowledge "
                << (int)ack << ", expected " << (int)CMD_ACK[0] << ".";
    return false;
  }

  logDebug() << "Left program mode.";
  _identifier = RadioInfo();

  return true;
}

bool
RadioddityInterface::selectMemoryBank(MemoryBank bank, const ErrorStack &err) {
  unsigned char ack;
  const uint8_t *cmd = nullptr;

  if (_current_bank == bank)
    return true;

  // Select command by memory bank
  switch (bank) {
  case MEMBANK_CODEPLUG_LOWER : cmd = CMD_CWB0; break;
  case MEMBANK_CODEPLUG_UPPER : cmd = CMD_CWB1; break;
  case MEMBANK_CALLSIGN_LOWER : cmd = CMD_CWB3; break;
  case MEMBANK_CALLSIGN_UPPER : cmd = CMD_CWB4; break;
  default:
    errMsg(err) << "Cannot set memory bank: Unknown bank " << bank << ".";
    return false;
  }

  logDebug() << "Selecting memory bank " << bank << "...";

  // select memory bank
  if (! hid_send_recv(cmd, 8, &ack, 1, err)) {
    errMsg(err) << "Cannot send memory bank select command.";
    return false;
  }
  if (ack != CMD_ACK[0]) {
    errMsg(err) << "Cannot select memory bank: Wrong acknowledge "
                << (int)ack << ", expected " << (int)CMD_ACK[0] << ".";
    return false;
  }

  logDebug() << "Memory bank " << bank << " selected.";
  _current_bank = bank;

  return true;
}
