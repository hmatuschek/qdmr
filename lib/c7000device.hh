#ifndef C7000DEVICE_HH
#define C7000DEVICE_HH

#include <QObject>
#include <libusb.h>
#include "errorstack.hh"
#include "radiointerface.hh"

/** Base class for all C7000 based radios. This class implements the basic communication protocol
 * to these devices.
 * @ingroup rif */
class C7000Device : public QObject
{
  Q_OBJECT

public:
  /** Request/response packet. */
  struct Packet {
  public:
    /** Default constructor. */
    Packet();
    /** Copy constructor. */
    Packet(const Packet &other)  = default ;
    /** Constructs a request/response from commands and payload. */
    Packet(uint8_t command, uint8_t sub, uint8_t flags=0x0f, const QByteArray &payload=QByteArray());
    /** Constructs a request/response from the given encoded packet. */
    Packet(const QByteArray &buffer);

    /** Assignment. */
    Packet &operator =(const Packet &other) = default;

    /** Returns @c true, if the packet is valid. */
    bool isValid() const;

    uint8_t flags() const;
    uint8_t command() const;
    uint8_t subcommand() const;
    uint16_t payloadSize() const;
    QByteArray payload() const;
    const QByteArray &encoded() const;

  protected:
    /** Holds the encoded packet. */
    QByteArray _encoded;
  };

public:
  /** Specialization to address a DFU device uniquely. */
  class Descriptor: public USBDeviceDescriptor
  {
  public:
    /** Constructor from interface info, bus number and device address. */
    Descriptor(const USBDeviceInfo &info, uint8_t bus, uint8_t device);
  };

public:
  /** Opens a connection to the C7000 device. */
  C7000Device(const USBDeviceDescriptor &descr, const ErrorStack &err=ErrorStack(), QObject *parent=nullptr);
  /** Destructor. */
  virtual ~C7000Device();

  /** Returns @c true if the C7000 interface is open. */
  bool isOpen() const;
  /** Closes the C7000 interface. */
  void close();

public:
  /** Returns some information about the interface. */
  static USBDeviceInfo interfaceInfo();
  /** Finds all C7000 interfaces. */
  static QList<USBDeviceDescriptor> detect(bool saveOnly=true);

protected:
  /** Sends the given request to the device and receives the response. */
  bool sendRecv(const Packet &request, Packet &response, const ErrorStack &err=ErrorStack());

protected:
  /** USB context. */
  libusb_context *_ctx;
  /** USB device object. */
  libusb_device_handle *_dev;
};

#endif // C7000DEVICE_HH
