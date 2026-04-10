#ifndef PACKETSTREAM_HH
#define PACKETSTREAM_HH

#include <QObject>
#include "errorstack.hh"

class QIODevice;

/** Defines an interface for a datagram socket.
 * That is, just some methods to send and receive packets.
 * @ingroup rif */
class PacketStream : public QObject
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit PacketStream(QObject *parent = nullptr);

public:
  /** Destructor. */
  virtual ~PacketStream();

  /** Returns @c true if the stream is open. */
  virtual bool isOpen() const = 0;
  /** Closes the stream. */
  virtual void close() = 0;

  /** Receives a datagram. Blocks for up to @c timeout milliseconds. */
  virtual bool receive(QByteArray &buffer, int timeout=-1, const ErrorStack &err=ErrorStack()) = 0;
  /** Receives a datagram. Blocks for up to @c timeout milliseconds. */
  virtual bool send(const QByteArray& buffer, int timeout=-1, const ErrorStack &err=ErrorStack()) = 0;
};


/** Implement SLIP (serial line internet protocol).
 * @ingroup rif */
class SlipStream: public PacketStream
{
  Q_OBJECT

public:
  /** Constructor, takes ownership of the device. */
  SlipStream(QIODevice *device, QObject *parent=nullptr);

public:
  bool isOpen() const override;
  void close() override;
  bool receive(QByteArray &buffer, int timeout=-1, const ErrorStack &err=ErrorStack()) override;
  bool send(const QByteArray& buffer, int timeout=-1, const ErrorStack &err=ErrorStack()) override;

protected:
  QIODevice *_device;
  QByteArray _buffer;

private:
  static constexpr char END_OF_PACKET = '\xC0';
  static constexpr char ESCAPE = '\xDB';
  static constexpr char ESCAPED_C0 = '\xDC';
  static constexpr char ESCAPED_DB = '\xDD';
};



#endif // PACKETSTREAM_HH
