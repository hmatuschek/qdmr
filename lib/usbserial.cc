#include "usbserial.hh"
#include "logger.hh"
#include <QFileInfo>
#include <QSerialPortInfo>
#include <QThread>

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
USBSerial::USBSerial(const USBDeviceDescriptor &descriptor, BaudRate rate, const ErrorStack &err, QObject *parent)
  : QSerialPort(parent), RadioInterface()
{
  if (USBDeviceInfo::Class::Serial != descriptor.interfaceClass()) {
    errMsg(err) << "Cannot open serial port for a non-serial descriptor: "
                << descriptor.description();
  }

  logDebug() << "Try to open " << descriptor.description() << ".";
  QSerialPortInfo port(descriptor.device().toString());
  this->setPort(port);
  if (! setParity(QSerialPort::NoParity)) {
    logWarn() << "Cannot set parity of the serial port to none.";
  }
  if (! setStopBits(QSerialPort::OneStop)) {
    logWarn() << "Cannot set stop bit.";
  }
  if (! setBaudRate(rate)) {
    logWarn() << "Cannot set speed to " << rate << " baud.";
  }
  if (! setFlowControl(QSerialPort::HardwareControl)) {
    logWarn() << "Cannot enable hardware flow control.";
  }

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

  logDebug() << "Opened serial port " << this->portName() << " with "
             << this->baudRate() << "baud.";

  connect(this, SIGNAL(aboutToClose()), this, SLOT(onClose()));
  connect(this, SIGNAL(errorOccurred(QSerialPort::SerialPortError)),
          this, SLOT(onError(QSerialPort::SerialPortError)));
  connect(this, SIGNAL(dataTerminalReadyChanged(bool)), this, SLOT(signalingChanged()));
  connect(this, SIGNAL(requestToSendChanged(bool)), this, SLOT(signalingChanged()));
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

void
USBSerial::signalingChanged() {
  logDebug() << "Pinout signals changed to " << formatPinoutSignals() << ".";
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

QString
USBSerial::formatPinoutSignals() {
  if (QSerialPort::NoSignal == pinoutSignals())
    return "None";

  QStringList res;
  if (QSerialPort::TransmittedDataSignal & pinoutSignals())
    res.append("Transitter Detected");
  if (QSerialPort::ReceivedDataSignal & pinoutSignals())
    res.append("Received Data");
  if (QSerialPort::DataTerminalReadySignal & pinoutSignals())
    res.append("Data Terminal Ready");
  if (QSerialPort::DataCarrierDetectSignal & pinoutSignals())
    res.append("Data Carrier Detect");
  if (QSerialPort::DataSetReadySignal & pinoutSignals())
    res.append("Data Set Ready");
  if (QSerialPort::RingIndicatorSignal & pinoutSignals())
    res.append("Ring Indicator");
  if (QSerialPort::RequestToSendSignal & pinoutSignals())
    res.append("Request To Send");
  if (QSerialPort::ClearToSendSignal & pinoutSignals())
    res.append("Clear To Send");
  if (QSerialPort::SecondaryTransmittedDataSignal & pinoutSignals())
    res.append("Secondary Transmitted Data");
  if (QSerialPort::SecondaryReceivedDataSignal & pinoutSignals())
    res.append("Secondary Received Data");

  return res.join(", ");
}
