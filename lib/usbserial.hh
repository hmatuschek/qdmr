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
  class Descriptor: public USBDeviceDescriptor {
  public:
    /** Constructor from VID, PID and device path. */
    Descriptor(uint16_t vid, uint16_t pid, const QString &device);
  };

protected:
  /** Constructs an opens new serial interface to the devices identified by the given vendor and
   * product IDs.
   * @param descriptor Specifies the device to open.
   * @param err The error stack, messages are put onto.
   * @param parent Specifies the parent object. */
  explicit USBSerial(const USBDeviceDescriptor &descriptor,
                     QSerialPort::BaudRate rate=QSerialPort::Baud115200,
                     const ErrorStack &err=ErrorStack(), QObject *parent=nullptr);

public:
  /** Destructor. */
  virtual ~USBSerial();

  /** If @c true, the device has been found and is open. */
  bool isOpen() const;
  /** Closes the interface to the device. */
  void close();

public:
  /** Searches for all USB serial ports with the specified VID/PID. */
  static QList<USBDeviceDescriptor> detect(uint16_t vid, uint16_t pid);

protected slots:
  /** Callback for serial interface errors. */
  void onError(QSerialPort::SerialPortError error_t);
  /** Callback when closing interface. */
  void onClose();
  /** Signaling callback. */
  void signalingChanged();

protected:
  /** Serializes the pinout singals. */
  QString formatPinoutSignals();
};

#endif // USBSERIAL_HH
