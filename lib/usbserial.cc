#include "usbserial.hh"
#include "logger.hh"
#include <QFileInfo>
#include <QSerialPortInfo>

/* ******************************************************************************************** *
 * Implementation of USBSerial::Info
 * ******************************************************************************************** */
USBSerial::Descriptor::Descriptor(uint16_t vid, uint16_t pid, const QString &device)
  : USBDeviceDescriptor(USBDeviceInfo(Class::Serial, vid, pid), device)
{
  // pass...
}

/* ******************************************************************************************** *
 * Implementation of USBSerial
 * ******************************************************************************************** */
USBSerial::USBSerial(unsigned vid, unsigned pid, const ErrorStack &err, QObject *parent)
  : QSerialPort(parent), RadioInterface()
{
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
#ifdef Q_OS_UNIX
        QFileInfo portFileInfo(port.systemLocation());
        if (portFileInfo.exists() &&
            ((! portFileInfo.isReadable()) || (! portFileInfo.isWritable())))
        {
          QString owner = QString(portFileInfo.permission(QFileDevice::ReadOwner) ? "r" : "-")
              + (portFileInfo.permission(QFile::WriteOwner) ? "w" : "-")
              + (portFileInfo.permission(QFile::ExeOwner) ? "x" : "-");
          QString group = QString(portFileInfo.permission(QFileDevice::ReadGroup) ? "r" : "-")
              + (portFileInfo.permission(QFile::WriteGroup) ? "w" : "-")
              + (portFileInfo.permission(QFile::ExeGroup) ? "x" : "-");
          QString other = QString(portFileInfo.permission(QFileDevice::ReadOther) ? "r" : "-")
              + (portFileInfo.permission(QFile::WriteOther) ? "w" : "-")
              + (portFileInfo.permission(QFile::ExeOther) ? "x" : "-");
          errMsg(err) << "Insufficient rights to read or write '" << port.systemLocation()
                      << "' (" << port.description() << "): "
                      << portFileInfo.owner() << ": " << owner << ", "
                      << portFileInfo.group() << ": " << group << ", other: " << other << ".";
          if (portFileInfo.permission(QFile::ReadGroup | QFile::WriteGroup))
            errMsg(err) << "A membership in the group " << portFileInfo.group()
                        << " would grant access.";
        }
#endif
        errMsg(err) << "Cannot open serial port '" << port.portName()
                    << "': " << this->errorString() << ".";
      } else {
        break;
      }
    }
  }

  if (! this->isOpen()) {
    errMsg(err) << "No serial port found with " << QString::number(vid, 16)
                << ":" << QString::number(pid, 16) << ".";
    return;
  }

  logDebug() << "Openend serial port " << this->portName() << " with "
             << this->baudRate() << "baud.";

  connect(this, SIGNAL(aboutToClose()), this, SLOT(onClose()));
  connect(this, SIGNAL(errorOccurred(QSerialPort::SerialPortError)),
          this, SLOT(onError(QSerialPort::SerialPortError)));
}

USBSerial::USBSerial(const USBDeviceDescriptor &descriptor, const ErrorStack &err, QObject *parent)
  : QSerialPort(parent), RadioInterface()
{
  if (USBDeviceInfo::Class::Serial != descriptor.interfaceClass()) {
    errMsg(err) << "Cannot open serial port for a non-serial descriptor: "
                << descriptor.description();
  }

  logDebug() << "Try to open " << descriptor.description() << ".";
  QSerialPortInfo port(descriptor.device().toString());
  this->setPort(port);
  this->setBaudRate(115200);

  if (! this->open(QIODevice::ReadWrite)) {
#ifdef Q_OS_UNIX
    QFileInfo portFileInfo(port.systemLocation());
    if (portFileInfo.exists() &&
        ((! portFileInfo.isReadable()) || (! portFileInfo.isWritable())))
    {
      QString owner = QString(portFileInfo.permission(QFileDevice::ReadOwner) ? "r" : "-")
          + (portFileInfo.permission(QFile::WriteOwner) ? "w" : "-")
          + (portFileInfo.permission(QFile::ExeOwner) ? "x" : "-");
      QString group = QString(portFileInfo.permission(QFileDevice::ReadGroup) ? "r" : "-")
          + (portFileInfo.permission(QFile::WriteGroup) ? "w" : "-")
          + (portFileInfo.permission(QFile::ExeGroup) ? "x" : "-");
      QString other = QString(portFileInfo.permission(QFileDevice::ReadOther) ? "r" : "-")
          + (portFileInfo.permission(QFile::WriteOther) ? "w" : "-")
          + (portFileInfo.permission(QFile::ExeOther) ? "x" : "-");
      errMsg(err) << "Insufficient rights to read or write '" << port.systemLocation()
                  << "' (" << port.description() << "): "
                  << portFileInfo.owner() << ": " << owner << ", "
                  << portFileInfo.group() << ": " << group << ", other: " << other << ".";
      if (portFileInfo.permission(QFile::ReadGroup | QFile::WriteGroup))
        errMsg(err) << "A membership in the group " << portFileInfo.group()
                    << " would grant access.";
    }
#endif
    errMsg(err) << "Cannot open serial port '" << port.portName()
                << "': " << this->errorString() << ".";
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
  logError() << "Serial port error: (" << err << ") " << errorString() << ".";
}

void
USBSerial::onClose() {
  logDebug() << "Serial port will close now.";
}

QList<USBDeviceDescriptor>
USBSerial::detect(uint16_t vid, uint16_t pid) {
  QList<USBDeviceDescriptor> interfaces;
  // Find matching serial port by VID/PID.
  logDebug() << "Search for serial port with matching VID:PID " <<
                QString::number(vid, 16) << ":" << QString::number(pid, 16) << ".";
  QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
  foreach (QSerialPortInfo port, ports) {
    if (port.hasProductIdentifier() && (pid == port.productIdentifier()) &&
        port.hasVendorIdentifier() && (vid == port.vendorIdentifier())) {
      interfaces.append(Descriptor(vid, pid, port.portName()));
      logDebug() << "Found " << port.portName() << " (USB "
                 << QString::number(vid, 16) << ":" << QString::number(pid, 16) << ").";
    }
  }
  return interfaces;
}
