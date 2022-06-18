/** @defgroup detect Device detection and enumeration.
 * This module collects classes and functions to discover and select interfaces to connected radios.
 *
 * With an increasing number of supported devices, the issue arises, that auto detection of
 * radios may fail or may even harmful. Some manufacturers simply use generic USB-serial chips
 * within the cable to talk to them over USB. These chips may also be used in other devices that may
 * react harmful to qdmrs attempts to identify them. Moreover, the assumption of the first detected
 * device with a specific VID:PID combination may not be valid. To this end, some means of
 * discovering possible radios and selecting a specific one by the user are needed. Also some radios
 * do not identify themselves before any action is performed (i.e., reading, writing the codeplug
 * or callsign db). Hence, for some devices, the user must specify the type of the radio. The latter
 * concerns the Kydera CDR-300UV, Retevis RT73 and similar devices.
 *
 * This module specifies the classes and functions to discover possible radios and to address each
 * one uniquely. They allow for an implementation of a semi-automatic device detection. That is,
 * for the majority of radios, if a single matching VID:PID combination is found, it can be assumed
 * that this device is a radio and it can then be identified by sending commands to it. Some radios,
 * however, like the Kydera CDR-300UV, cannot be identified before the actual codeplug read or
 * write operation. They simply do not provide a command for identification.
 *
 * For a save semi-automatic detection the following steps are performed:
 * @dotfile autodetect.dot "Semi-automatic radio detection"
 *
 *   -# Search for all USB devices with known VID:PID combinations. This can be done using the
 *      @c USBDeviceDescriptor::detect method. It returns a list of matching device descriptors
 *      found. If only one device is found, one may continue with that one if it is save to assume
 *      that the device detected is a DMR radio. The latter is not true for radios using generic
 *      USB CDC-ACM chips, as other serial devices may be connected. If several devices are found or
 *      it is not save to assume a DMR radio, the user must select a device.
 *   -# Once the USB device is selected, one needs to identify the connected radio. Unfortunately,
 *      not all radios can be identified easily by simply sending a command to it. To this end,
 *      one first needs to check if the selected device is identifiable. That is, if the protocol
 *      provides commands to identify the connected radio. The @c USBDeviceInfo provides this
 *      information. If a device is not identifiable, the user must specify the specific connected
 *      radio. This can be done by obtaining all known radios matching the selected USB device
 *      (VID:PID) by calling RadioInfo::allRadios, passing the @c USBDeviceDescriptor.
 *
 * @section detectExample A example for AnyTone devices
 * This example tries to detect an AnyTone device and reads the binary codeplug from it. Once the
 * codeplug is read, it is decoded into its generic device independent representation (@c Config).
 *
 * @code
 * #include "libdmrconf/usbdevice.hh"
 * #include "libdmrconf/anytone_radio.hh"
 *
 * int main(void)
 * {
 *   // First, search matching devices (only AnyTones)
 *   // to find all supported devices, call @c USBDescriptor::detect();
 *   QList<USBDeviceDescriptor> devices = AnytoneInterface::detect();
 *   if (1 != devices.count()) {
 *     // Either none or more than one device found...
 *     return -1;
 *   }
 *
 *   // A place to put error messages
 *   ErrorStack err;
 *   // Dedetect the specific radio and get radio descriptor.
 *   // To detect any radio based on the selected descriptor, call @c Radio::detect().
 *   Radio *radio = AnytoneRadio::detect(devices.first(), RadioInfo(), err);
 *   if (nullptr == radio) {
 *     // There went something wrong, check err.
 *     return -1;
 *   }
 *
 *   // Read codeplug from device blocking.
 *   if (! radio->startDownload(true, err)) {
 *     // Some read error, check err.
 *     delete radio;
 *     return -1;
 *   }
 *
 *   // Decode codeplug into generic representation
 *   Config genericCodeplug;
 *   if (! radio->codeplug().decode(&genericCodeplug, err)) {
 *     // Some decoding error, check err.
 *     delete radio;
 *     return -1;
 *   }
 *
 *   // Do whatever you like with the codeplug.
 *
 *   return 0;
 * }
 * @endcode
 *
 * @ingroup rif */

#ifndef USBDEVICE_HH
#define USBDEVICE_HH

#include <inttypes.h>
#include <QVariant>

/** Combines the USB bus and device number, to address a USB device uniquely.
 *
 * @ingroup detect */
struct USBDeviceHandle {
  uint8_t bus;         ///< Holds the bus number.
  uint8_t device;      ///< Holds the device address.
  uint32_t locationId; ///< On MacOS, holds the location ID.

  /** Empty constructor. */
  USBDeviceHandle();
  /** Constructor from bus and device number. */
  USBDeviceHandle(uint8_t busno, uint8_t deviceno, uint32_t locid=0);

  /** Compares only wrt bus and device number. */
  bool operator==(const USBDeviceHandle &other);
};
Q_DECLARE_METATYPE(USBDeviceHandle)

/** Generic information about a possible radio interface.
 *
 * This class combines the USB vendor, product ID and some meta information about the interface.
 * In particular if it is save to access the device without user ineraction and if the protocol
 * implements means for identifying the specific radio.
 *
 * @ingroup detect */
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
  USBDeviceInfo(Class cls, uint16_t vid, uint16_t pid, bool save=true, bool identifiable=true);
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

  /** Returns @c true if it is save to send commands to this device without user approval.
   * This is true for radios which use somewhat unique VID:PIDs. Radios with generic USB-serial
   * chips are not save, as other devices may use the same chip and sending data to these devices
   * may be harmful. */
  bool isSave() const;

  /** Returns @c true if the radio is identifiable automatically. */
  bool isIdentifiable() const;

protected:
  /** The class of the interface. */
  Class _class;
  /** The USB vid. */
  uint16_t _vid;
  /** The USB pid. */
  uint16_t _pid;
  /** If @c true, it is save to send commands to the device without user approval. */
  bool _save;
  /** If @c true, the device may be identified automatically. */
  bool _identifiable;
};


/** Base class for all radio interface descriptors representing a unique interface to a
 * connected radio.
 *
 * This class extends the @c USBDeviceInfo by some information to identify a USB uniquely. This is
 * either the bus and device number or the path to the serial port.
 *
 * @ingroup detect */
class USBDeviceDescriptor: public USBDeviceInfo
{
protected:
  /** Hidden constructor from info and path string. */
  USBDeviceDescriptor(const USBDeviceInfo &info, const QString &device);
  /** Hidden constructor from info and USB device address. */
  USBDeviceDescriptor(const USBDeviceInfo &info, const USBDeviceHandle &device);

public:
  /** Empty constructor. */
  USBDeviceDescriptor();
  /** Copy constructor. */
  USBDeviceDescriptor(const USBDeviceDescriptor &other);

  /** Assignment */
  USBDeviceDescriptor &operator =(const USBDeviceDescriptor &other);

  /** Returns @c true if the descriptor is still valid. That is, if the described device is still
   * connected. */
  bool isValid() const;

  /** Returns a human readable description of the device. */
  QString description() const;

  /** Returns the device information identifying the interface uniquely. */
  const QVariant &device() const;
  /** Returns a unique string representation of the device information. */
  QString deviceHandle() const;

public:
  /** Searches for all connected radios (may contain false positives). */
  static QList<USBDeviceDescriptor> detect();

protected:
  /** Checks a serial port. */
  bool validSerial() const;
  /** Checks a raw USB device. */
  bool validRawUSB() const;

protected:
  /** Holds some information to identify the radio interface uniquely. */
  QVariant _device;
};

#endif // USBDEVICE_HH
