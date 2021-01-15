#include "hid_interface.hh"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const unsigned char CMD_PRG[]   = "\2PROGRA";
static const unsigned char CMD_PRG2[]  = "M\2";
static const unsigned char CMD_ACK[]   = "A";
static const unsigned char CMD_READ[]  = "Raan";
static const unsigned char CMD_WRITE[] = "Waan...";
static const unsigned char CMD_ENDR[]  = "ENDR";
static const unsigned char CMD_ENDW[]  = "ENDW";
static const unsigned char CMD_CWB0[]  = "CWB\4\0\0\0\0";
static const unsigned char CMD_CWB1[]  = "CWB\4\0\1\0\0";

HID::HID(int vid, int pid, QObject *parent)
  : HIDevice(vid, pid, parent), _offset(0)
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
    _errorMessage = tr("%1: Cannot identify radio: %2").arg(__func__).arg(_errorMessage);
    return QString();
  }

  if (ack != CMD_ACK[0]) {
    _errorMessage = tr("%1: Cannot identify radio: Wrong PRD acknowledge %2, expected %3.")
        .arg(__func__).arg(ack,0,10).arg(CMD_ACK[0], 0, 16);
    return QString();
  }

  if (! hid_send_recv(CMD_PRG2, 2, reply, 16)) {
    _errorMessage = tr("%1: Cannot identify radio: %2").arg(__func__).arg(_errorMessage);
    return QString();
  }

  if (! hid_send_recv(CMD_ACK, 1, &ack, 1)) {
    _errorMessage = tr("%1: Cannot identify radio: %2").arg(__func__).arg(_errorMessage);
    return QString();
  }

  if (ack != CMD_ACK[0]) {
    _errorMessage = tr("%1: Cannot identify radio: Wrong PRG2 acknowledge %2, expected %3.")
        .arg(__func__).arg(ack, 0, 16).arg(CMD_ACK[0], 0, 16);
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
  return true;
}

bool
HID::read(uint32_t bank, uint32_t addr, unsigned char *data, int nbytes)
{
  Q_UNUSED(bank);

  unsigned char cmd[4], reply[32+4];
  int n;

  if (! selectMemoryBank(addr)) {
    _errorMessage = tr("%1: Cannot read addr 0x%2 (n=%3): %4")
        .arg(__func__).arg(addr,0,16).arg(nbytes);
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
  return true;
}

bool
HID::write(uint32_t bank, uint32_t addr, unsigned char *data, int nbytes)
{
  Q_UNUSED(bank);

  unsigned char ack, cmd[4+32];

  if (! selectMemoryBank(addr)) {
    _errorMessage = tr("%1: Cannot write addr 0x%2 (n=%3): %4")
        .arg(__func__).arg(addr,0,16).arg(nbytes);
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
HID::selectMemoryBank(uint addr) {
  unsigned char ack;

  // select memory bank according to address
  if ((addr < 0x10000) && (_offset != 0)) {
    _offset = 0;
    if (! hid_send_recv(CMD_CWB0, 8, &ack, 1)) {
      _errorMessage = tr("%1: Cannot select memory bank for addr %2: %3").arg(__func__)
          .arg(addr).arg(_errorMessage);
      return false;
    }
    if (ack != CMD_ACK[0]) {
      _errorMessage = tr("%1: Cannot select memory bank for addr %2: Wrong acknowledge %3, expected %4.")
          .arg(__func__).arg(addr).arg(ack, 0, 16).arg(CMD_ACK[0], 0, 16);
      return false;
    }
  } else if ((addr >= 0x10000) && (0 == _offset)) {
    _offset = 0x00010000;
    if (! hid_send_recv(CMD_CWB1, 8, &ack, 1)) {
      _errorMessage = tr("%1: Cannot select memory bank for addr %2: %3").arg(__func__)
          .arg(addr).arg(_errorMessage);
      return false;
    }
    if (ack != CMD_ACK[0]) {
      _errorMessage = tr("%1: Cannot select memory bank for addr %2: Wrong acknowledge %3, expected %4.")
          .arg(__func__).arg(addr).arg(ack, 0, 16).arg(CMD_ACK[0], 0, 16);
      return false;
    }
  }

  return true;
}
