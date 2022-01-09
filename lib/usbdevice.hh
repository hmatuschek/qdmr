#ifndef USBDEVICE_HH
#define USBDEVICE_HH

#include <inttypes.h>
#include <QVariant>

/** Just a helper struct to hold the bus number and device address of a specific USB device.
 *
 * @ingroup rif */
struct USBDeviceAddress {
  uint8_t bus;      ///< Holds the bus number.
  uint8_t device;   ///< Holds the device address.
};
Q_DECLARE_METATYPE(USBDeviceAddress)

/** Generic information about a possible radio interface. */
class USBDeviceInfo
{
public:
  /** Possible interface types. */
  enum class Class {
    None,       ///< Class for invalid interface info.
    Serial,     ///< Serial port interface class.
    DFU,        ///< DFU interface class.
    HID         ///< HID (human-interface device) interface class.
  };

public:
  /** Empty constructor. */
  USBDeviceInfo();
  /** Constructor from class, VID and PID. */
  USBDeviceInfo(Class cls, uint16_t vid, uint16_t pid);
  /** Destructor. */
  virtual ~USBDeviceInfo();

  /** Copy constructor. */
  USBDeviceInfo(const USBDeviceInfo &other);
  /** Assignment. */
  USBDeviceInfo &operator =(const USBDeviceInfo &other);

  /** Comparison. */
  bool operator ==(const USBDeviceInfo &other) const;
  /** Comparison. */
  bool operator !=(const USBDeviceInfo &other) const;

  /** Returns @c true if the interface info is valid. */
  bool isValid() const;

  /** Returns the interface class. */
  Class interfaceClass() const;

  /** Returns the vendor ID or 0 if not set. */
  uint16_t vendorId() const;
  /** Returns the product ID or 0 if not set. */
  uint16_t productId() const;

  /** Returns a brief human readable description of the interface. */
  QString description() const;

  /** Returns a more extensive human readable description of the interface. */
  QString longDescription() const;

protected:
  /** The class of the interface. */
  Class _class;
  /** The USB vid. */
  uint16_t _vid;
  /** The USB pid. */
  uint16_t _pid;
};


/** Base class for all radio iterface descriptors representing a unique interface to a
 * connected radio. */
class USBDeviceDescriptor: public USBDeviceInfo
{
protected:
  /** Hidden constructor from info and path string. */
  USBDeviceDescriptor(const USBDeviceInfo &info, const QString &device);
  /** Hidden constructor from info and USB device address. */
  USBDeviceDescriptor(const USBDeviceInfo &info, const USBDeviceAddress &device);

public:
  /** Copy constructor. */
  USBDeviceDescriptor(const USBDeviceDescriptor &other);

  /** Assignment */
  USBDeviceDescriptor &operator =(const USBDeviceDescriptor &other);

  /** Returns a human readable description of the device. */
  QString description() const;
  /** Returns the device information identifying the interface uniquely. */
  const QVariant &device() const;

protected:
  /** Holds some information to identify the radio interface uniquely. */
  QVariant _device;
};

#endif // USBDEVICE_HH
