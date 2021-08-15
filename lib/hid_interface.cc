#include "hid_interface.hh"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "logger.hh"

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

HID::HID(int vid, int pid, QObject *parent)
  : HIDevice(vid, pid, parent), _current_bank(MEMBANK_NONE)
{
  // pass...
}

HID::~HID() {
  if (isOpen())
    close();
}


bool
HID::isOpen() const {
  return HIDevice::isOpen();
}

void
HID::close() {
  HIDevice::close();
}

QString
HID::identifier() {
  static unsigned char reply[38];
  unsigned char ack;

  if (! hid_send_recv(CMD_PRG, 7, &ack, 1)) {
    _errorMessage = tr("Cannot identify radio: %1").arg(_errorMessage);
    logError() << _errorMessage;
    return QString();
  }

  if (ack != CMD_ACK[0]) {
    _errorMessage = tr("Cannot identify radio: Wrong PRD acknowledge %1, expected %2.")
        .arg(ack,0,10).arg(CMD_ACK[0], 0, 16);
    logError() << _errorMessage;
    return QString();
  }

  if (! hid_send_recv(CMD_PRG2, 2, reply, 16)) {
    _errorMessage = tr("Cannot identify radio: %1").arg(_errorMessage);
    logError() << _errorMessage;
    return QString();
  }

  if (! hid_send_recv(CMD_ACK, 1, &ack, 1)) {
    _errorMessage = tr("Cannot identify radio: %1").arg(_errorMessage);
    logError() << _errorMessage;
    return QString();
  }

  if (ack != CMD_ACK[0]) {
    _errorMessage = tr("Cannot identify radio: Wrong PRG2 acknowledge %1, expected %2.")
        .arg(ack, 0, 16).arg(CMD_ACK[0], 0, 16);
    logError() << _errorMessage;
    return QString();
  }

  // Reply:
  // 42 46 2d 35 52 ff ff ff 56 32 31 30 00 04 80 04
  //  B  F  -  5  R           V  2  1  0

  // Terminate the string.
  char *p = (char *)memchr(reply, 0xff, sizeof(reply));
  if (p)
    *p = 0;
  return (char*)reply;
}


bool
HID::read_start(uint32_t bank, uint32_t addr) {
  if (! selectMemoryBank(MemoryBank(bank))) {
    _errorMessage = tr("Cannot select memory bank %1: %2").arg(bank).arg(_errorMessage);
    return false;
  }

  return true;
}

bool
HID::read(uint32_t bank, uint32_t addr, unsigned char *data, int nbytes)
{
  unsigned char cmd[4], reply[32+4];
  int n;

  if (! selectMemoryBank(MemoryBank(bank))) {
    _errorMessage = tr("Cannot select memory bank %1: %2").arg(bank).arg(_errorMessage);
    return false;
  }

  // send data
  for (n=0; n<nbytes; n+=32) {
    cmd[0] = CMD_READ[0];
    cmd[1] = (addr + n) >> 8;
    cmd[2] = addr + n;
    cmd[3] = 32;
    if (! hid_send_recv(cmd, 4, reply, sizeof(reply)))
      return false;
    else
      memcpy(data + n, reply + 4, 32);
  }

  return true;
}

bool
HID::read_finish()
{
  unsigned char ack;

  if (! hid_send_recv(CMD_ENDR, 4, &ack, 1)) {
    _errorMessage = tr("%1: Cannot finish read(): %2").arg(__func__).arg(_errorMessage);
    return false;
  }
  if (ack != CMD_ACK[0]) {
    _errorMessage = tr("%1: Cannot finish read(): Wrong acknowledge %2, expected %3.")
        .arg(__func__).arg(ack).arg(CMD_ACK[0]);
    return false;
  }

  return true;
}


bool
HID::write_start(uint32_t bank, uint32_t addr) {
  if (! selectMemoryBank(MemoryBank(bank))) {
    _errorMessage = tr("Cannot select memory bank %1: %2").arg(bank).arg(_errorMessage);
    return false;
  }

  return true;
}

bool
HID::write(uint32_t bank, uint32_t addr, unsigned char *data, int nbytes)
{
  unsigned char ack, cmd[4+32];

  if (! selectMemoryBank(MemoryBank(bank))) {
    _errorMessage = tr("Cannot select memory bank %1: %2").arg(bank).arg(_errorMessage);
    return false;
  }

  // send data
  for (int n=0; n<nbytes; n+=32) {
    cmd[0] = CMD_WRITE[0];
    cmd[1] = (addr + n) >> 8;
    cmd[2] = addr + n;
    cmd[3] = 32;
    memcpy(cmd + 4, data + n, 32);
    if (! hid_send_recv(cmd, 4+32, &ack, 1))
      return false;
    else if (ack != CMD_ACK[0]) {
      _errorMessage = tr("%1: Cannot write block: Wrong acknowledge %2, expected %3.")
          .arg(__func__).arg(ack, 0, 16).arg(CMD_ACK[0], 0, 16);
      n-=32;
    }
  }

  return true;
}

bool
HID::write_finish()
{
  unsigned char ack;

  if (! hid_send_recv(CMD_ENDW, 4, &ack, 1)) {
    _errorMessage = tr("%1: Cannot finish write(): %2").arg(__func__).arg(_errorMessage);
    return false;
  }
  if (ack != CMD_ACK[0]) {
    _errorMessage = tr("%1: Cannot finish write(): Wrong acknowledge %2, expected %3.")
        .arg(__func__).arg(ack, 0, 16).arg(CMD_ACK[0], 0, 16);
    return false;
  }

  return true;
}

bool
HID::selectMemoryBank(MemoryBank bank) {
  unsigned char ack;
  const uint8_t *cmd = nullptr;

  if (_current_bank == bank)
    return true;

  // Select command by memory bank
  switch (bank) {
  case MEMBANK_CODEPLUG_LOWER : cmd = CMD_CWB0; break;
  case MEMBANK_CODEPLUG_UPPER : cmd = CMD_CWB1; break;
  case MEMBANK_CALLSIGN_DB :    cmd = CMD_CWB3; break;
  default:
    _errorMessage = tr("Cannot set memory bank: Unknown bank %1").arg(bank);
    return false;
  }

  logDebug() << "Selecting memory bank " << bank;

  // select memory bank
  if (! hid_send_recv(cmd, 8, &ack, 1)) {
    _errorMessage = tr("Cannot select memory bank: %1").arg(_errorMessage);
    return false;
  }
  if (ack != CMD_ACK[0]) {
    _errorMessage = tr("Cannot select memory bank: Wrong acknowledge %3, expected %4.")
        .arg(ack, 0, 16).arg(CMD_ACK[0], 0, 16);
    return false;
  }

  _current_bank = bank;

  return true;
}
