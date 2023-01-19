#include "xmodem.hh"
#include "logger.hh"


XModem::XModem(const USBDeviceDescriptor& descriptor, const ErrorStack& err, QObject *parent)
  : USBSerial(descriptor, err, parent), _state(State::Init), _maxRetry(10)
{
  // pass...
}


bool
XModem::receive(QByteArray &buffer, int timeout, const ErrorStack &err) {
  if (State::Init != _state) {
    errMsg(err) << "Cannot start reception of data. Not in INIT state.";
    return false;
  }

  // Clear buffer
  buffer.clear();

  // Init transfer
  if (! this->txByte(CRC, timeout, err)) {
    _state = State::Error;
    errMsg(err) << "Cannot initialize transfer.";
    return false;
  }

  // Expected sequence number
  uint8_t seqNum = 1;
  QByteArray payload;
  unsigned int retry_count = 0;

  while (true) {
    uint8_t rsp, seq, seq1c;
    uint16_t crc;

    if (! this->rxByte(rsp, timeout, err)) {
      _state = State::Error;
      errMsg(err) << "Cannot initialize transfer.";
      return false;
    }

    switch (_state) {
    case State::Init:
    case State::Transfer:
      switch (rsp) {
      case SOH:
      case STX:
        // receive sequence number
        if ((! rxByte(seq, timeout)) || (! rxByte(seq1c, timeout))) {
          _state = State::Error;
          errMsg(err) << "Cannot receive sequence numbers.";
          return false;
        }

        // allocate payload buffer based on type
        payload.clear();
        if (SOH == rsp)
          payload.resize(128);
        else
          payload.resize(1024);

        // receive payload
        if (! rxBytes((uint8_t *)payload.data(), payload.size(), timeout)) {
          _state = State::Error;
          errMsg(err) << "Cannot receive payload.";
          return false;
        }

        // receive CRC
        if (! rxBytes((uint8_t *)&crc, 2, timeout)) {
          _state = State::Error;
          errMsg(err) << "Cannot receive CRC16.";
          return false;
        }

        // check sequence number. On mismatch -> cancel transfer
        if ((seq != seqNum) || ((255-seq) != seq1c)) {
          _state = State::Init;
          if (! txByte(CAN, timeout, err)) {
            _state = State::Error;
            return false;
          }
          if (! txByte(CAN, timeout, err)) {
            _state = State::Error;
            return false;
          }
          return false;
        }
        seqNum++;

        // Check CRC
        if (crc != crc_ccitt(payload)) {
          if (! txByte(NAK, timeout, err)) {
            _state = State::Error;
            return false;
          }
          retry_count++;
          if (retry_count > _maxRetry) {
            errMsg(err) << "Maximum retries (" << _maxRetry<<") reached.";
            _state = State::Error;
            return false;
          }
        } else {
          if( !txByte(ACK, timeout, err)) {
            _state = State::Error;
            return false;
          }
        }

        if (State::Init == _state)
          _state = State::Transfer;
        // State updated to transfer -> continue transfer
        retry_count = 0;
        break;

      case EOT:
        // Once EOT received -> state back to init
        _state = State::Init;
        // ACK
        if (! txByte(ACK, timeout, err)) {
          _state = State::Error;
          return false;
        }
        // Done.
        return true;

      default:
        // Unknown command:
        _state = State::Error;
        errMsg(err) << "Unknown command " << rsp << ".";
        return false;
      }      
    case State::Error:
      return false;
    }
  }

  // Should never be reached.
}

bool
XModem::send(const QByteArray &buffer, int timeout, const ErrorStack &err) {
  if (State::Init != _state) {
    errMsg(err) << "Cannot start reception of data. Not in INIT state.";
    return false;
  }

  // Wait for CRC
  uint8_t cmd;
  if ((! rxByte(cmd, timeout, err)) || (CRC != cmd)) {
    _state = State::Error;
    errMsg(err) << "Cannot start transfer.";
    return false;
  }

  uint8_t seqNum = 1;
  quint64 offset = 0;
  quint64 length = buffer.length();
  QByteArray payload; payload.resize(1024);

  while (length) {
    _state = State::Transfer;
    // Copy a chunk
    payload.fill(0);
    quint64 n_send = std::min(1024ULL, length);
    memcpy(payload.data(), buffer.constData()+offset, n_send);

    // Transmit header
    if ((!txByte(STX, timeout, err)) || (!txByte(seqNum, timeout, err)) || (!txByte(255-seqNum, timeout, err))) {
      _state = State::Error;
      errMsg(err) << "Cannot send header.";
      return false;
    }

    // Transmit data
    if (!txBytes((const uint8_t *)payload.constData(), payload.length(), timeout, err)) {
      _state = State::Error;
      errMsg(err) << "Cannot send payload";
      return false;
    }

    // Transmit CRC
    uint16_t crc = crc_ccitt(payload);
    if (!txBytes((const uint8_t *)&crc, 2, timeout, err)) {
      _state = State::Error;
      errMsg(err) << "Cannot send CRC.";
      return false;
    }

    // Receive ACK/NACK/CAN
    if (! rxByte(cmd, timeout, err)) {
      _state = State::Error;
      errMsg(err) << "Cannot receive response.";
      return false;
    }

    switch (cmd) {
    case ACK:
      // Increment sequence number
      seqNum++;
      // Update length and offset
      length -= n_send; offset += n_send;
      break;
    case NAK:
      // Resend
      break;
    case CAN:
      // Canceled by receiver
      _state = State::Init;
      return false;
    }
  }

  return true;
}


bool
XModem::txByte(uint8_t byte, int timeout, const ErrorStack &err) {
  if (! putChar((char)byte)) {
    errMsg(err) << "Cannot send byte: " << this->errorString();
    return false;
  }
  if (! waitForBytesWritten(timeout)) {
    errMsg(err) << "Write time-out.";
    return false;
  }
  return true;
}

bool
XModem::rxByte(uint8_t &byte, int timeout, const ErrorStack &err) {
  if ((! bytesAvailable()) && (!waitForReadyRead(timeout))) {
    errMsg(err) << "Read time-out.";
    return false;
  }
  if (! getChar((char *)&byte)) {
    errMsg(err) << "Cannot receive byte: " << this->errorString();
    return false;
  }
  return true;
}

bool
XModem::txBytes(const uint8_t *buffer, unsigned int length, int timeout, const ErrorStack &err) {
  while (length) {
    qint64 nb_written = QSerialPort::write((const char *)buffer, length);
    if (0 > nb_written) {
      errMsg(err) << "Cannot write to interface: " << this->errorString();
      return false;
    }
    length -= nb_written;
    if (! waitForBytesWritten(timeout)) {
      errMsg(err) << "Read time-out.";
      return false;
    }
  }

  return true;
}

bool
XModem::rxBytes(uint8_t *buffer, unsigned int length, int timeout, const ErrorStack &err) {
  while (length) {
    if ((! bytesAvailable()) && (!waitForReadyRead(timeout))) {
      errMsg(err) << "Read time-out.";
      return false;
    }
    qint64 nb_read = QSerialPort::read((char *)buffer, length);
    if (0 > nb_read) {
      errMsg(err) << "Cannot read from interface: " << this->errorString();
      return false;
    }
    length -= nb_read;
  }

  return true;
}


uint16_t
XModem::crc_ccitt(const QByteArray &data) {
  uint16_t x   = 0;
  uint16_t crc = 0;
  const uint8_t *buf = ((const uint8_t *) data.constData());

  for (int i = 0; i < data.length(); i++) {
    x   = (crc >> 8) ^ buf[i];
    x  ^= x >> 4;
    crc = (crc << 8) ^ (x << 12) ^ (x << 5) ^ x;
  }

  return crc ^ 0xFFFF;
}
