#include "hd1_interface.hh"
#include "logger.hh"
#include <QThread>
#include <QElapsedTimer>

static constexpr uint8_t FRAME_START = 0x68;
static constexpr uint8_t FRAME_END   = 0x10;
static constexpr uint8_t CMD_INFO    = 0x0d;
static constexpr uint8_t CMD_SMALL   = 0x0f;
static constexpr uint8_t CMD_BULK    = 0x31;
static constexpr int HEADER_LEN      = 10;

/* The progress byte (b4) of each frame of a full write, as the manufacturer's software (HD1 CPS
 * V3.05) sends it: its progress bar, computed from two form properties. Taken from a serial
 * capture of a write. The radio ignores b4 on reads; for writes this keeps the session
 * byte-identical to the original software, which is the only write sequence known to work. */
static const uint8_t writeProgress[] = {
  0x00, 0x01, 0x02, 0x02, 0x03, 0x03, 0x04, 0x05, 0x05, 0x06, 0x06, 0x07, 0x08, 0x08, 0x09, 0x09,
  0x0a, 0x0b, 0x0b, 0x0c, 0x0c, 0x0d, 0x0e, 0x0e, 0x0f, 0x10, 0x10, 0x11, 0x11, 0x12, 0x13, 0x13,
  0x14, 0x14, 0x15, 0x16, 0x16, 0x17, 0x17, 0x18, 0x19, 0x19, 0x1a, 0x1a, 0x1b, 0x1c, 0x1c, 0x1d,
  0x1d, 0x1e, 0x1f, 0x1f, 0x20, 0x20, 0x21, 0x22, 0x22, 0x23, 0x23, 0x24, 0x25, 0x25, 0x26, 0x26,
  0x27, 0x28, 0x28, 0x29, 0x29, 0x2a, 0x2b, 0x2b, 0x2c, 0x2d, 0x2d, 0x2e, 0x2e, 0x2f, 0x30, 0x30,
  0x30, 0x30, 0x30, 0x31, 0x31, 0x31, 0x32, 0x32, 0x32, 0x33, 0x33, 0x33, 0x33, 0x34, 0x34, 0x34,
  0x35, 0x35, 0x35, 0x36, 0x36, 0x36, 0x36, 0x37, 0x37, 0x37, 0x38, 0x38, 0x38, 0x39, 0x39, 0x39,
  0x3a, 0x3a, 0x3a, 0x3a, 0x3b, 0x3b, 0x3b, 0x3c, 0x3c, 0x3c, 0x3d, 0x3d, 0x3d, 0x3d, 0x3e, 0x3e,
  0x3e, 0x3f, 0x3f, 0x3f, 0x40, 0x40, 0x40, 0x40, 0x41, 0x41, 0x41, 0x42, 0x42, 0x42, 0x43, 0x43,
  0x43, 0x43, 0x44, 0x44, 0x44, 0x45, 0x45, 0x45, 0x46, 0x46, 0x46, 0x46, 0x47, 0x47, 0x47, 0x48,
  0x48, 0x48, 0x49, 0x49, 0x49, 0x4a, 0x4a, 0x4a, 0x4a, 0x4b, 0x4b, 0x4b, 0x4c, 0x4c, 0x4c, 0x4d,
  0x4d, 0x4d, 0x4d, 0x4e, 0x4e, 0x4e, 0x4f, 0x4f, 0x4f, 0x50, 0x50, 0x50, 0x50, 0x51, 0x51, 0x51,
  0x52, 0x52, 0x52, 0x53, 0x53, 0x53, 0x53, 0x54, 0x54, 0x54, 0x55, 0x55, 0x55, 0x56, 0x56, 0x56,
  0x57, 0x57, 0x57, 0x57, 0x58, 0x58, 0x58, 0x59, 0x59, 0x59, 0x5a, 0x5a, 0x5a, 0x5a, 0x5b, 0x5b,
  0x5b, 0x5c, 0x5c, 0x5c, 0x5d, 0x5d, 0x5d, 0x5d, 0x5e, 0x5e, 0x5e, 0x5f, 0x5f, 0x5f, 0x60, 0x60,
  0x60, 0x60, 0x61, 0x61, 0x61, 0x62, 0x62, 0x62, 0x63, 0x63, 0x63
};


/* ********************************************************************************************* *
 * Implementation of HD1Interface
 * ********************************************************************************************* */
HD1Interface::HD1Interface(const USBDeviceDescriptor &descr, const ErrorStack &err, QObject *parent)
  : USBSerial(descr, static_cast<QSerialPort::BaudRate>(120000), err, parent),
    _ready(false), _writeCount(0), _timestamp()
{
  // Timeouts are part of the protocol here (the radio is deaf while booting), so they must not
  // be reported as errors.
  disconnect(this, SIGNAL(errorOccurred(QSerialPort::SerialPortError)),
             this, SLOT(onError(QSerialPort::SerialPortError)));
  connect(this, SIGNAL(errorOccurred(QSerialPort::SerialPortError)),
          this, SLOT(onSerialError(QSerialPort::SerialPortError)));
  if (isOpen()) {
    setFlowControl(QSerialPort::NoFlowControl);
    setDataTerminalReady(true);
    setRequestToSend(true);
  }
}

void
HD1Interface::onSerialError(QSerialPort::SerialPortError error) {
  if ((QSerialPort::NoError == error) || (QSerialPort::TimeoutError == error))
    return;
  onError(error);
}

USBDeviceInfo
HD1Interface::interfaceInfo() {
  return USBDeviceInfo(USBDeviceInfo::Class::Serial, 0x1a86, 0x7523, false);
}



RadioInfo
HD1Interface::identifier(const ErrorStack &err) {
  if (! waitReady(err))
    return RadioInfo();
  uint8_t modelData[7];
  if (! request(CMD_SMALL, 0xe800, sizeof(modelData), modelData, err)) {
    errMsg(err) << "Cannot read model string.";
    return RadioInfo();
  }
  QByteArray name(reinterpret_cast<const char *>(modelData), sizeof(modelData));
  if (model() != name) {
    errMsg(err) << "Unknown model '" << name << "', expected '" << model() << "'.";
    return RadioInfo();
  }
  return RadioInfo::byID(RadioInfo::HD1);
}


bool
HD1Interface::waitReady(const ErrorStack &err) {
  if (_ready)
    return true;
  uint8_t info[16];
  for (int attempt=0; attempt<30; attempt++) {
    if (request(CMD_INFO, 0, sizeof(info), info, ErrorStack())) {
      logDebug() << "HD1 answered after " << (attempt+1) << " info request(s).";
      _ready = true;
      return true;
    }
  }
  errMsg(err) << "No answer to the info request: radio off, wrong port or still booting?";
  return false;
}


bool
HD1Interface::read_start(uint32_t bank, uint32_t addr, const ErrorStack &err) {
  Q_UNUSED(bank); Q_UNUSED(addr);
  return waitReady(err);
}

bool
HD1Interface::read(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes, const ErrorStack &err) {
  switch (bank) {
  case Info:
    return request(CMD_INFO, addr, nbytes, data, err);
  case Small:
    // The manufacturer's software reads at most 128 bytes per request.
    for (int offset=0; offset<nbytes; offset+=128) {
      int n = std::min(128, nbytes-offset);
      if (! request(CMD_SMALL, addr+offset, n, data+offset, err))
        return false;
    }
    return true;
  case Bulk:
    if ((addr % pageSize()) || (nbytes % pageSize())) {
      errMsg(err) << "Bulk reads must be whole pages, got " << nbytes << " bytes at "
                  << Qt::hex << addr << "h.";
      return false;
    }
    for (int offset=0; offset<nbytes; offset+=pageSize()) {
      if (! request(CMD_BULK, (addr+offset)/pageSize(), pageSize(), data+offset, err))
        return false;
    }
    return true;
  }
  errMsg(err) << "Unknown memory bank " << bank << ".";
  return false;
}

bool
HD1Interface::read_finish(const ErrorStack &err) {
  Q_UNUSED(err);
  return true;
}


bool
HD1Interface::write_start(uint32_t bank, uint32_t addr, const ErrorStack &err) {
  Q_UNUSED(bank); Q_UNUSED(addr);
  if (! waitReady(err))
    return false;

  // Timestamp frame, echoed by the radio.
  QDateTime when = _timestamp.isValid() ? _timestamp : QDateTime::currentDateTime();
  QByteArray stamp;
  stamp.append(char(FRAME_START)).append(char(CMD_SMALL)).append(char(0x05));
  for (int v : {when.date().year(), when.date().month(), when.date().day(),
                when.time().hour(), when.time().minute(), when.time().second()}) {
    stamp.append(char(v & 0xff)).append(char((v >> 8) & 0xff));
  }
  stamp.append(char(FRAME_END));
  QByteArray echo;
  if (! transfer(stamp, echo, stamp.size(), 1500, err) || (echo != stamp)) {
    errMsg(err) << "Timestamp frame not echoed.";
    return false;
  }

  // The reads the manufacturer's software makes before writing.
  uint8_t info[16], modelData[7], settings[64];
  if (! request(CMD_INFO, 0, sizeof(info), info, err)
      || ! request(CMD_SMALL, 0xe800, sizeof(modelData), modelData, err)
      || ! request(CMD_SMALL, 0x2950, sizeof(settings), settings, err)) {
    errMsg(err) << "Cannot start write session.";
    return false;
  }
  if (model() != QByteArray(reinterpret_cast<const char *>(modelData), sizeof(modelData))) {
    errMsg(err) << "Unexpected model, refusing to write.";
    return false;
  }

  logDebug() << "HD1 write session started (" << when.toString(Qt::ISODate) << ").";
  _writeCount = 0;
  return true;
}

bool
HD1Interface::write(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes, const ErrorStack &err) {
  switch (bank) {
  case Small:
    return writeFrame(CMD_SMALL, addr, data, nbytes, err);
  case Bulk:
    if ((addr % pageSize()) || (nbytes % pageSize())) {
      errMsg(err) << "Bulk writes must be whole pages, got " << nbytes << " bytes at "
                  << Qt::hex << addr << "h.";
      return false;
    }
    return writeFrame(CMD_BULK, addr/pageSize(), data, nbytes, err);
  }
  errMsg(err) << "Cannot write to memory bank " << bank << ".";
  return false;
}

bool
HD1Interface::write_finish(const ErrorStack &err) {
  Q_UNUSED(err);
  return true;
}


bool
HD1Interface::reboot(const ErrorStack &err) {
  Q_UNUSED(err);
  if (! isOpen())
    return false;
  logDebug() << "End HD1 session, the radio reboots.";
  QSerialPort::write("END", 3);
  waitForBytesWritten(1000);
  // The radio reboots and prints its boot banner.
  drain(1000, 10000);
  _ready = false;
  return true;
}

bool
HD1Interface::setDateTime(const QDateTime &datetime, const ErrorStack &err) {
  Q_UNUSED(err);
  _timestamp = datetime;
  return true;
}


bool
HD1Interface::request(uint8_t cmd, uint16_t addr, uint16_t len, uint8_t *data, const ErrorStack &err) {
  QByteArray req;
  req.append(char(FRAME_START)).append(char(cmd)).append(char(0x00)).append(char(0x01))
      .append(char(0x00)).append(char(0x00))
      .append(char(len & 0xff)).append(char(len >> 8))
      .append(char(addr & 0xff)).append(char(addr >> 8))
      .append(char(FRAME_END));
  int timeout = (CMD_BULK == cmd) ? 3000 : 1500;
  ErrorStack lastErr;
  for (int attempt=0; attempt<3; attempt++) {
    QByteArray rep;
    lastErr = ErrorStack();
    if (! transfer(req, rep, HEADER_LEN+len+1, timeout, lastErr))
      continue;
    if ((uint8_t(rep[0]) != FRAME_START) || (uint8_t(rep[rep.size()-1]) != FRAME_END)
        || (uint8_t(rep[1]) != cmd) || (rep.mid(6, 4) != req.mid(6, 4))) {
      errMsg(lastErr) << "Reply does not match the request: " << rep.left(HEADER_LEN).toHex(' ');
      continue;
    }
    memcpy(data, rep.constData()+HEADER_LEN, len);
    return true;
  }
  errMsg(err) << lastErr.format();
  errMsg(err) << "Cannot read " << len << " bytes from " << Qt::hex << cmd << "h@" << addr << "h.";
  return false;
}

bool
HD1Interface::writeFrame(uint8_t cmd, uint16_t addr, const uint8_t *data, uint16_t len, const ErrorStack &err) {
  uint8_t progress = (_writeCount < sizeof(writeProgress)) ? writeProgress[_writeCount] : 0x63;
  QByteArray req;
  req.append(char(FRAME_START)).append(char(cmd)).append(char(0x01)).append(char(0x01))
      .append(char(progress)).append(char((CMD_BULK == cmd) ? 0x31 : 0x00))
      .append(char(len & 0xff)).append(char(len >> 8))
      .append(char(addr & 0xff)).append(char(addr >> 8));
  req.append(reinterpret_cast<const char *>(data), len);
  req.append(char(FRAME_END));

  QByteArray expected = req.left(HEADER_LEN);
  if (CMD_BULK == cmd)
    expected[2] = 0x02;
  expected.append(char(FRAME_END));

  QByteArray ack;
  // No retry: a half-applied write must be looked at, not repeated blindly.
  if (! transfer(req, ack, expected.size(), 5000, err) || (ack != expected)) {
    errMsg(err) << "Write " << (_writeCount+1) << " to " << Qt::hex << cmd << "h@" << addr
                << "h not acknowledged: " << ack.toHex(' ');
    return false;
  }
  _writeCount++;
  return true;
}

bool
HD1Interface::transfer(const QByteArray &out, QByteArray &in, int n, int timeout, const ErrorStack &err) {
  clear(QSerialPort::Input);
  if ((out.size() != QSerialPort::write(out)) || (! waitForBytesWritten(timeout))) {
    errMsg(err) << "Cannot send to radio: " << errorString() << ".";
    return false;
  }
  in.clear();
  QElapsedTimer timer; timer.start();
  while (in.size() < n) {
    int left = timeout - int(timer.elapsed());
    if ((left <= 0) || ((0 == bytesAvailable()) && (! waitForReadyRead(left)))) {
      errMsg(err) << "Timeout: got " << in.size() << " of " << n << " bytes.";
      return false;
    }
    in.append(QSerialPort::read(n - in.size()));
  }
  return true;
}

void
HD1Interface::drain(int quiet, int timeout) {
  QElapsedTimer timer; timer.start();
  while (timer.elapsed() < timeout) {
    if (! waitForReadyRead(quiet))
      return;
    readAll();
  }
}
