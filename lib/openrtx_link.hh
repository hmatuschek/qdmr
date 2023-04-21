#ifndef OPENRTXLINK_HH
#define OPENRTXLINK_HH

#include <QObject>

#include "packetstream.hh"

// Forward declaration
class OpenRTXLink;
class OpenRTXLinkStream;
class OpenRTXLinkDatagram;
class OpenRTXCAT;


/** Implements the OpenRTX link protocol. This is a datagram-oriented protocol, that dispatches
 * several different protocols to talk to the radio. It provides a stream, to the stdio of the
 * radio, a CAT interface, a file-system management protocol (FMP) as well as XMODEM to transfer
 * files. The file transfer must be initialized via the FMP.
 *
 * All these protocols are exposed through specialized interface objects, accessible through this
 * classs.
 *
 * @code
 *  +------------------+-------------------+-------------------+------------------+
 *  |      stdio       |        CAT        |        FMP        |       XMODEM     |
 *  +------------------+-------------------+-------------------+------------------+
 *  |                                 OpenRTXLink                                 |
 *  +-----------------------------------------------------------------------------+
 *  |                              SLIP (SlipStream)                              |
 *  +-----------------------------------------------------------------------------+
 *  |                  USB CDC-ACM (VCOM/Serial-over-USB, USBSerial)              |
 *  +-----------------------------------------------------------------------------+
 * @endcode
 * @ingroup ortx */
class OpenRTXLink: public QObject
{
  Q_OBJECT

protected:
  /** The possible protocols, encapsulated in OpenRTX link. */
  enum class Protocol {
    Stdio = 0, CAT = 1, FMP = 2, XMODEM = 3
  };

public:
  /** Constructor.
   * @param link Specifies the datagram socket to talk to the radio (SLIP).
   * @param parent Specifies the QObject parent. */
  explicit OpenRTXLink(PacketStream *link, QObject *parent = nullptr);

  /** Returns a stream to the stdio of the radio. Implements the @c QIODevice interface. */
  OpenRTXLinkStream *stdio() const;
  /** The CAT interface to the radio. */
  OpenRTXCAT *cat() const;
  /** The file-system management protocol interface to the radio. */
  OpenRTXLinkDatagram *fmp() const;
  /** An XMODEM channel to transfer files. */
  OpenRTXLinkStream *xmodem() const;

protected:
  /** Dispatcher to receive datagrams over OpenRTXLink. */
  bool receive(Protocol proto, QByteArray &data, int timeout=-1, const ErrorStack &err=ErrorStack());
  /** Dispatcher to send datagrams over OpenRTXLink. */
  bool send(Protocol proto, const QByteArray &data, int timeout=-1, const ErrorStack &err=ErrorStack());

  static uint8_t crc8(const QByteArray &data);

protected:
  PacketStream *_link;
  QHash<unsigned int, QList<QByteArray>> _inBuffers;
  OpenRTXLinkStream *_stdio;
  OpenRTXCAT        *_cat;
  OpenRTXLinkDatagram *_fmp;
  OpenRTXLinkStream *_xmodem;

  friend class OpenRTXLinkStream;
  friend class OpenRTXLinkDatagram;
  friend class OpenRTXCAT;
};


class OpenRTXLinkStream: public QIODevice
{
  Q_OBJECT

protected:
  OpenRTXLinkStream(OpenRTXLink::Protocol proto, OpenRTXLink *link);

protected:
  qint64 writeData(const char *data, qint64 len);
  qint64 readData(char *data, qint64 maxlen);

protected:
  OpenRTXLink::Protocol _proto;
  QByteArray _inBuffer;
  OpenRTXLink *_link;

  friend class OpenRTXLink;
};


class OpenRTXLinkDatagram: public PacketStream
{
  Q_OBJECT

protected:
  OpenRTXLinkDatagram(OpenRTXLink::Protocol proto, OpenRTXLink *link);

public:
  bool receive(QByteArray &buffer, int timeout, const ErrorStack &err);
  bool send(const QByteArray &buffer, int timeout, const ErrorStack &err);

protected:
  OpenRTXLink::Protocol _proto;
  OpenRTXLink *_link;

  friend class OpenRTXLink;
};


class OpenRTXCAT: public OpenRTXLinkDatagram
{
  Q_OBJECT

protected:
  /** Hidden constrcutor. An instance of this class can be obtained from @c OpenRTXLink.
   *  @param link Specifies the unerlying RTX-link to the the device. */
  explicit OpenRTXCAT(OpenRTXLink *link);

  friend class OpenRTXLink;
};

#endif // OPENRTXLINK_HH
