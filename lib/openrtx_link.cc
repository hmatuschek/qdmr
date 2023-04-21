#include "openrtx_link.hh"


/* ******************************************************************************************** *
 * Implements the OpenRTX link stream socket
 * ******************************************************************************************** */
OpenRTXLinkStream::OpenRTXLinkStream(OpenRTXLink::Protocol proto, OpenRTXLink *link)
  : QIODevice(link), _proto(proto), _inBuffer(), _link(link)
{
  // pass...
}

qint64
OpenRTXLinkStream::writeData(const char *data, qint64 len) {
  QByteArray buffer(data, len);

  ErrorStack err;
  if (! _link->send(OpenRTXLink::Protocol::Stdio, buffer, -1, err)) {
    setErrorString(err.format());
    return -1;
  }

  return len;
}

qint64
OpenRTXLinkStream::readData(char *data, qint64 maxlen) {
  ErrorStack err;
  qint64 n_read = 0, n_toread=maxlen;

  while (0 < n_toread) {
    if (_inBuffer.size()) {
      qint64 n = std::min((qint64)_inBuffer.size(), n_toread);
      memcpy(data, _inBuffer.data(), n);
      _inBuffer.remove(0, n);
      n_toread -= n; data+= n; n_read += n;
    }
    if (0 == n_toread)
      return n_read;
    if (! _link->receive(OpenRTXLink::Protocol::Stdio, _inBuffer, -1, err)) {
      setErrorString(err.format());
      return -1;
    }
  }

  return n_read;
}



/* ******************************************************************************************** *
 * Implements the OpenRTX link datagram socket
 * ******************************************************************************************** */
OpenRTXLinkDatagram::OpenRTXLinkDatagram(OpenRTXLink::Protocol proto, OpenRTXLink *link)
  : PacketStream(link), _proto(proto), _link(link)
{
  // pass...
}

bool
OpenRTXLinkDatagram::receive(QByteArray &buffer, int timeout, const ErrorStack &err) {
  return _link->receive(_proto, buffer, timeout, err);
}

bool
OpenRTXLinkDatagram::send(const QByteArray &buffer, int timeout, const ErrorStack &err) {
  return _link->send(_proto, buffer, timeout, err);
}


/* ******************************************************************************************** *
 * Implements the OpenRTX CAT interface
 * ******************************************************************************************** */
OpenRTXCAT::OpenRTXCAT(OpenRTXLink *link)
  : OpenRTXLinkDatagram(OpenRTXLink::Protocol::CAT, link)
{
  // pass...
}


/* ******************************************************************************************** *
 * Implements the OpenRTX link protocol dispatcher
 * ******************************************************************************************** */
OpenRTXLink::OpenRTXLink(PacketStream *link, QObject *parent)
  : QObject{parent}, _link(link),
    _stdio(new OpenRTXLinkStream(Protocol::Stdio, this)),
    _cat(new OpenRTXCAT(this)),
    _fmp(new OpenRTXLinkDatagram(Protocol::FMP, this)),
    _xmodem(new OpenRTXLinkStream(Protocol::XMODEM, this))
{
  // pass...
}

OpenRTXLinkStream *
OpenRTXLink::stdio() const {
  return _stdio;
}

OpenRTXCAT *OpenRTXLink::cat() const {
  return _cat;
}

OpenRTXLinkDatagram *
OpenRTXLink::fmp() const {
  return _fmp;
}

OpenRTXLinkStream *
OpenRTXLink::xmodem() const {
  return _xmodem;
}

uint8_t
OpenRTXLink::crc8(const QByteArray &data) {
  constexpr uint8_t poly = 0xA6;
  uint8_t crc = 0;
  for (int i=0; i<data.size(); i++) {
    crc ^= data[i];
    for (int j=0; j<8; j++) {
      if (crc & 0x80)
        crc = ( (crc<<1) ^ poly );
      else
        crc <<= 1;
    }
  }
  return crc;
}

bool
OpenRTXLink::send(Protocol proto, const QByteArray &data, int timeout, const ErrorStack &err) {
  QByteArray packet; packet.reserve(data.size()+2);
  packet.append((char)proto);
  packet.append(data);
  packet.append(crc8(packet));
  return this->_link->send(packet, timeout, err);
}

bool
OpenRTXLink::receive(Protocol proto, QByteArray &data, int timeout, const ErrorStack &err) {
  QByteArray buffer;
  while (true) {
    // Receive a datagram
    if (! this->_link->receive(buffer, timeout, err))
      return false;

    // Check CRC
    if (0 != crc8(buffer)) {
      errMsg(err) << "Invalid CRC in RTXLink packet.";
      return false;
    }

    // Dispatch by type
    Protocol rxProto = (Protocol)buffer.at(0);
    // If requested type matches -> done
    if (proto == rxProto) {
      data = buffer.mid(1, buffer.size()-2);
      return true;
    }

    // Otherwise, store and receive next
    if (! _inBuffers.contains((unsigned int)rxProto))
      _inBuffers.insert((unsigned int)rxProto, QList<QByteArray>());
    _inBuffers[(unsigned int)rxProto].append(buffer.mid(1,buffer.size()-2));
  }
}
