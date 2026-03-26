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
OpenRTXLinkDatagram::isOpen() const {
  return _link->isOpen();
}

void
OpenRTXLinkDatagram::close() {
  // Cannot close data-gram socket by its own.
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

QByteArray
OpenRTXCAT::info(const ErrorStack &err) {
  if (! send(QByteArray("GIN",3), _link->timeout(), err)) {
    errMsg(err) << "Cannot request device info.";
    return QByteArray();
  }

  QByteArray res;
  if (! receive(res, _link->timeout(), err)) {
    errMsg(err) << "Cannot receive device info.";
    return QByteArray();
  }

  if ('A' == res[0]) {
    errMsg(err) << "Cannot obtain device info. Received error code " << (int8_t)res[1];
    return QByteArray();
  } else if ('D' == res[0]) {
    return res.right(16);
  }

  errMsg(err) << "Obtained unexpected response " << res.toHex(' ');
  return QByteArray();
}



/* ******************************************************************************************** *
 * Implements the OpenRTX link protocol dispatcher
 * ******************************************************************************************** */
OpenRTXLink::OpenRTXLink(PacketStream *link, QObject *parent)
  : QObject{parent}, _link(link),
    _stdio(new OpenRTXLinkStream(Protocol::Stdio, this)),
    _cat(new OpenRTXCAT(this)),
    _fmp(new OpenRTXLinkDatagram(Protocol::FMP, this)),
    _xmodem(new OpenRTXLinkStream(Protocol::XMODEM, this)),
    _timeout(2000)
{
  if (_link) _link->setParent(this);
}

OpenRTXLink::~OpenRTXLink() {
  if (_link)
    _link->close();
}

bool
OpenRTXLink::isOpen() const {
  return _link && _link->isOpen();
}

void
OpenRTXLink::close() {
  if (_link) _link->close();
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

unsigned int
OpenRTXLink::timeout() const {
  return _timeout;
}

uint16_t
OpenRTXLink::crc16(const QByteArray &data) {
  uint16_t x = 0, crc = 0;
  for (qsizetype i=0; i<data.length(); i++) {
    x = (crc >> 8) ^ data[i];
    x ^= x >> 4;
    crc = (crc << 8) ^ (x << 12) ^ (x << 5) ^ x;
  }
  return crc;
}

bool
OpenRTXLink::send(Protocol proto, const QByteArray &data, int timeout, const ErrorStack &err) {
  QByteArray packet; packet.reserve(data.size()+3);
  packet.append((char)proto);
  packet.append(data);
  uint16_t crc = crc16(data);
  packet.append(crc&0xff);
  packet.append(crc>>8);
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
    if (0 != crc16(buffer)) {
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
