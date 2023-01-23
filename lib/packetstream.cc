#include "packetstream.hh"

/* ******************************************************************************************** *
 * Implementation of PacketStream interface
 * ******************************************************************************************** */
PacketStream::PacketStream(QObject *parent)
  : QObject{parent}
{
  // pass...
}

PacketStream::~PacketStream() {
  // pass...
}


/* ******************************************************************************************** *
 * Implementation of SlipStream
 * ******************************************************************************************** */
SlipStream::SlipStream(QIODevice *device, QObject *parent)
  : PacketStream(parent), _device(device), _buffer()
{
  if (_device)
    _device->setParent(this);
}

bool
SlipStream::receive(QByteArray &buffer, int timeout, const ErrorStack &err) {
  while (! _buffer.contains(END_OF_PACKET)) {
    // If there is some stuff to read -> append to buffer and continue
    if (_device->bytesAvailable()) {
      _buffer.append(_device->readAll());
      continue;
    }
    if (! _device->waitForReadyRead(timeout)) {
      errMsg(err) << _device->errorString();
      errMsg(err) << "Cannot read from device.";
      return false;
    }
    _buffer.append(_device->readAll());
  }

  // Unpack packet
  int endIndex = _buffer.indexOf(END_OF_PACKET);

  // Clear output buffer
  buffer.clear();
  buffer.reserve(endIndex);

  for (int i=0; i<endIndex; i++) {
    if (ESCAPE == _buffer[i]) {
      if (((i+1) < endIndex) && (ESCAPED_C0 == _buffer[i+1])) {
        buffer.append('\xC0'); i+= 1;
      } else if (((i+1) < endIndex) && (ESCAPED_DB == _buffer[i+1])) {
        buffer.append('\xDB'); i+= 1;
      } else {
        buffer.append(_buffer[i]);
      }
    }
  }
  _buffer.remove(0, endIndex+1);

  return true;
}


bool
SlipStream::send(const QByteArray &buffer, int timeout, const ErrorStack &err) {
  QByteArray outBuffer;
  outBuffer.reserve(buffer.size() + 1
                    + buffer.count(END_OF_PACKET)
                    + buffer.count(ESCAPE));

  for (int i=0; i<buffer.size(); i++) {
    if (END_OF_PACKET == buffer[i]) {
      outBuffer.append(ESCAPE);
      outBuffer.append(ESCAPED_C0);
    } else if (ESCAPE == buffer[i]) {
      outBuffer.append(ESCAPE);
      outBuffer.append(ESCAPED_DB);
    } else {
      outBuffer.append(buffer[i]);
    }
  }
  outBuffer.append(END_OF_PACKET);

  _device->write(outBuffer);
  if (! _device->waitForBytesWritten(timeout)) {
    errMsg(err) << "Cannot write to the device.";
    return false;
  }
  return true;
}
