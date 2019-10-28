#include "hidinterface.hh"
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

static unsigned offset = 0;                 // CWD offset

HID::HID(int vid, int pid, QObject *parent)
  : HIDevice(vid, pid, parent)
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

  if (! hid_send_recv(CMD_PRG, 7, &ack, 1))
    return QString();

  if (ack != CMD_ACK[0]) {
    _errorMessage = tr("%1: Wrong PRD acknowledge %2, expected %3.")
        .arg(__func__).arg(ack,0,10).arg(CMD_ACK[0], 0, 16);
    return QString();
  }

  if (! hid_send_recv(CMD_PRG2, 2, reply, 16))
    return QString();


  if (! hid_send_recv(CMD_ACK, 1, &ack, 1))
    return QString();

  if (ack != CMD_ACK[0]) {
    _errorMessage = tr("%1: Wrong PRG2 acknowledge %2, expected %3.")
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
HID::read_block(int bno, unsigned char *data, int nbytes)
{
  unsigned addr = bno * nbytes;
  unsigned char ack, cmd[4], reply[32+4];
  int n;

  // select memory bank according to address
  if ((addr < 0x10000) && (offset != 0)) {
    offset = 0;
    if (! hid_send_recv(CMD_CWB0, 8, &ack, 1))
      return false;
    if (ack != CMD_ACK[0]) {
      _errorMessage = tr("%1: Wrong acknowledge %2, expected %3.")
          .arg(__func__).arg(ack, 0, 16).arg(CMD_ACK[0], 0, 16);
      return false;
    }
  } else if ((addr >= 0x10000) && (0 == offset)) {
    offset = 0x00010000;
    if (! hid_send_recv(CMD_CWB1, 8, &ack, 1))
      return false;
    if (ack != CMD_ACK[0]) {
      _errorMessage = tr("%1: Wrong acknowledge %2, expected %3.")
          .arg(__func__).arg(ack, 0, 16).arg(CMD_ACK[0], 0, 16);
      return false;
    }
  }

  // send data
  for (n=0; n<nbytes; n+=32) {
    cmd[0] = CMD_READ[0];
    cmd[1] = (addr + n) >> 8;
    cmd[2] = addr + n;
    cmd[3] = 32;
    if (! hid_send_recv(cmd, 4, reply, sizeof(reply)))
      n-=32;
    else
      memcpy(data + n, reply + 4, 32);
  }

  return true;
}

bool
HID::read_finish()
{
  unsigned char ack;

  if (! hid_send_recv(CMD_ENDR, 4, &ack, 1))
    return false;
  if (ack != CMD_ACK[0]) {
    _errorMessage = tr("%1: Wrong acknowledge %2, expected %3.")
        .arg(__func__).arg(ack).arg(CMD_ACK[0]);
    return false;
  }

  return true;
}

bool
HID::write_block(int bno, unsigned char *data, int nbytes)
{
  unsigned addr = bno * nbytes;
  unsigned char ack, cmd[4+32];

  // Select memory bank according to address
  if (addr < 0x10000 && offset != 0) {
    offset = 0;
    if (! hid_send_recv(CMD_CWB0, 8, &ack, 1))
      return false;
    if (ack != CMD_ACK[0]) {
      _errorMessage = tr("%1: Wrong acknowledge %2, expected %3.")
          .arg(__func__).arg(ack, 0, 16).arg(CMD_ACK[0], 0, 16);
      return false;
    }
  } else if (addr >= 0x10000 && offset == 0) {
    offset = 0x00010000;
    if (! hid_send_recv(CMD_CWB1, 8, &ack, 1))
      return false;
    if (ack != CMD_ACK[0]) {
      _errorMessage = tr("%1: Wrong acknowledge %2, expected %3.")
          .arg(__func__).arg(ack, 0, 16).arg(CMD_ACK[0], 0, 16);
      return false;
    }
  }

  // send data
  for (int n=0; n<nbytes; n+=32) {
    cmd[0] = CMD_WRITE[0];
    cmd[1] = (addr + n) >> 8;
    cmd[2] = addr + n;
    cmd[3] = 32;
    memcpy(cmd + 4, data + n, 32);
    if (! hid_send_recv(cmd, 4+32, &ack, 1))
      n -= 32;
    else if (ack != CMD_ACK[0]) {
      _errorMessage = tr("%1: Wrong acknowledge %2, expected %3.")
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

  if (! hid_send_recv(CMD_ENDW, 4, &ack, 1))
    return false;

  if (ack != CMD_ACK[0]) {
    _errorMessage = tr("%1: Wrong acknowledge %2, expected %3.")
        .arg(__func__).arg(ack, 0, 16).arg(CMD_ACK[0], 0, 16);
    return false;
  }

  return true;
}

