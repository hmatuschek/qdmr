#ifndef USBSERIAL_HH
#define USBSERIAL_HH

#include <QObject>
#include <QSerialPort>
#include "radiointerface.hh"
#include "errorstack.hh"

/** Implements a serial connection to a radio via USB.
 *
 * The correct serial port is selected by the given VID and PID to the constructor.
 *
 * @ingroup rif
 */
class USBSerial : public QSerialPort, public RadioInterface
{
  Q_OBJECT

public:
  /** Specialization of radio interface info for serial ports. */
  class Descriptor: public RadioInterface::Descriptor {
  public:
    /** Constructor from VID, PID and device path. */
    Descriptor(uint16_t vid, uint16_t pid, const QString &device);
  };

protected:
  /** Constructs an opens new serial interface to the devices identified by the given vendor and
   * product IDs.
   * @param vid Vendor ID of device.
   * @param pid Product ID of device.
   * @param err The error stack, messages are put onto.
   * @param parent Specifies the parent object. */
  explicit USBSerial(unsigned vid, unsigned pid, const ErrorStack &err=ErrorStack(), QObject *parent=nullptr);

  /** Constructs an opens new serial interface to the devices identified by the given vendor and
   * product IDs.
   * @param descriptor Specifies the device to open.
   * @param err The error stack, messages are put onto.
   * @param parent Specifies the parent object. */
  explicit USBSerial(const RadioInterface::Descriptor &descriptor, const ErrorStack &err=ErrorStack(), QObject *parent=nullptr);

public:
  /** Destrutor. */
  virtual ~USBSerial();

  /** If @c true, the device has been found and is open. */
  bool isOpen() const;
  /** Closes the interface to the device. */
  void close();

public:
  /** Searches for all USB serial ports with the specified VID/PID. */
  static QList<RadioInterface::Descriptor> detect(uint16_t vid, uint16_t pid);

protected slots:
  /** Callback for serial interface errors. */
  void onError(QSerialPort::SerialPortError error_t);
  /** Callback when closing interface. */
  void onClose();
};

#endif // USBSERIAL_HH
