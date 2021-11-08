#include "usbserial.hh"
#include "logger.hh"
#include <QSerialPortInfo>

USBSerial::USBSerial(unsigned vid, unsigned pid, QObject *parent)
  : QSerialPort(parent), RadioInterface()
{
  static int idMetaType = qRegisterMetaType<QSerialPort::SerialPortError>();
  Q_UNUSED(idMetaType);

  logDebug() << "Try to detect USB serial interface "
             << QString::number(vid,16) << ":"
             << QString::number(pid,16) << ".";

  // Find matching serial port by VID/PID.
  QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
  foreach (QSerialPortInfo port, ports) {
    if (port.hasProductIdentifier() && (pid == port.productIdentifier()) &&
        port.hasVendorIdentifier() && (vid == port.vendorIdentifier()))
    {
      logDebug() << "Found serial port " << QString::number(vid,16) << ":"
                 << QString::number(pid,16) << ": " << port.portName()
                 << " '" << port.description() << "'.";
      this->setPort(port);
      this->setBaudRate(115200);
      if (! this->open(QIODevice::ReadWrite)) {
        errMsg() << "Cannot open serial port '" << port.portName()
                 << "': " << this->errorString() << ".";
      } else {
        break;
      }
    }
  }

  if ((! this->isOpen()) && (!hasErrorMessages())) {
    errMsg() << "No serial port found with " << QString::number(vid, 16)
             << ":" << QString::number(pid, 16) << ".";
    return;
  }

  logDebug() << "Openend serial port " << this->portName() << " with "
             << this->baudRate() << "baud.";

  connect(this, SIGNAL(aboutToClose()), this, SLOT(onClose()));
  connect(this, SIGNAL(errorOccurred(QSerialPort::SerialPortError)),
          this, SLOT(onError(QSerialPort::SerialPortError)));
}

USBSerial::~USBSerial() {
  if (isOpen())
    close();
}

bool
USBSerial::isOpen() const {
  return QSerialPort::isOpen();
}

void
USBSerial::close() {
  if (isOpen())
    QSerialPort::close();
}

void
USBSerial::onError(QSerialPort::SerialPortError err) {
  errMsg() << "Serial port error: (" << err << ") " << errorString() << ".";
}

void
USBSerial::onClose() {
  logDebug() << "Serial port will close now.";
}
