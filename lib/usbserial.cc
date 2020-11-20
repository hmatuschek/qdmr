#include "usbserial.hh"
#include "logger.hh"
#include <QSerialPortInfo>

USBSerial::USBSerial(unsigned vid, unsigned pid, QObject *parent)
  : QSerialPort(parent), RadioInterface(), _errorMessage()
{
  logDebug() << "Try to detect USB serial interface " << hex << vid << ":" << pid << ".";

  // Find matching serial port by VID/PID.
  QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
  foreach (QSerialPortInfo port, ports) {
    if (port.hasProductIdentifier() && (pid == port.productIdentifier()) &&
        port.hasVendorIdentifier() && (vid == port.vendorIdentifier()))
    {
      logDebug() << "Found serial port " << hex << vid << ":" << pid << ": "
                 << port.portName() << " '" << port.description() << "'.";
      this->setPort(port);
      this->setBaudRate(921600);
      if (! this->open(QIODevice::ReadWrite)) {
        _errorMessage = tr("%1: Cannot open serial port '%2': %3")
            .arg(__func__).arg(port.portName()).arg(this->errorString());
        logDebug() << "Cannot open serial port '" << port.portName()
                   << "': " << this->errorString();
      } else {
        break;
      }
    }
  }
  if ((! this->isOpen()) && _errorMessage.isEmpty()) {
    _errorMessage = tr("%1: No serial port found with %2:%3.")
        .arg(__func__).arg(vid, 4, 16).arg(pid, 4, 16);
    return;
  }

  logDebug() << "Openend serial port " << this->portName() << " with "
             << this->baudRate() << "baud.";

  connect(this, SIGNAL(aboutToClose()), this, SLOT(onClose()));
  connect(this, SIGNAL(errorOccurred(QSerialPort::SerialPortError)),
          this, SLOT(onError(QSerialPort::SerialPortError)));
}

USBSerial::~USBSerial() {
  QSerialPort::close();
}

bool
USBSerial::isOpen() const {
  return QSerialPort::isOpen();
}

void
USBSerial::close() {
  QSerialPort::close();
}

const QString &
USBSerial::errorMessage() const {
  return _errorMessage;
}

void
USBSerial::onError(QSerialPort::SerialPortError err) {
  //logError() <<": Serial port error: (" << err << ") " << errorString() << ".";
}

void
USBSerial::onClose() {
  //logInfo() << ": Serial port will close now.";
}
