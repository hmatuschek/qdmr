#ifndef USBSERIAL_HH
#define USBSERIAL_HH

#include <QObject>
#include <QSerialPort>
#include "radiointerface.hh"

/** Implements a serial connection to a radio via USB.
 *
 * The correct serial port is selected by the given VID and PID to the constructor.
 *
 * @ingroup rif
 */
class USBSerial : public QSerialPort, public RadioInterface
{
  Q_OBJECT

protected:
  /** Constructs an opens new serial interface to the devices identified by the given vendor and
   * product IDs.
   * @param vid Vendor ID of device.
   * @param pid Product ID of device.
   * @param parent Specifies the parent object. */
  explicit USBSerial(unsigned vid, unsigned pid, QObject *parent=nullptr);

public:
  /** Destrutor. */
  virtual ~USBSerial();

  /** If @c true, the device has been found and is open. */
  bool isOpen() const;
  /** Closes the interface to the device. */
  void close();

  /** Returns the last error message. */
  const QString &errorMessage() const;

protected slots:
  /** Callback for serial interface errors. */
  void onError(QSerialPort::SerialPortError error_t);
  /** Callback when closing interface. */
  void onClose();

protected:
  /** Holds the last error message. */
  QString _errorMessage;
};

#endif // USBSERIAL_HH
