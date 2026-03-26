#include "openrtx_interface.hh"
#include "usbserial.hh"
#include "logger.hh"
#include <QSerialPort>
#include <QSerialPortInfo>

#define USB_VID 0x0483
#define USB_PID 0x5740


OpenRTXInterface::OpenRTXInterface(const USBDeviceDescriptor &descriptor, const ErrorStack &err, QObject *parent)
  : QObject(parent), RadioInterface(), _rtxLink(nullptr)
{
  if (USBDeviceInfo::Class::Serial != descriptor.interfaceClass()) {
    errMsg(err) << "Cannot open serial port for a non-serial descriptor: "
                << descriptor.description();
    return;
  }

  auto serial = new QSerialPort(this);
  logDebug() << "Try to open " << descriptor.description() << ".";
  QSerialPortInfo port(descriptor.device().toString());
  serial->setPort(port);
  if (! serial->open(QIODevice::ReadWrite)) {
    errMsg(err) << "Cannot open port: " << serial->errorString();
    return;
  }

  auto slip = new SlipStream(serial, this);
  _rtxLink = new OpenRTXLink(slip, this);
}


bool
OpenRTXInterface::isOpen() const {
  return _rtxLink && _rtxLink->isOpen();
}

void
OpenRTXInterface::close() {
  if (_rtxLink) _rtxLink->close();
}


RadioInfo
OpenRTXInterface::identifier(const ErrorStack &err) {
  QByteArray id = _rtxLink->cat()->info(err);
  if (id.isEmpty())
    return RadioInfo();

  logInfo() << "Detected '" << QString::fromLatin1(id) << "'.";
  return RadioInfo::byID(RadioInfo::OpenRTX);
}


bool
OpenRTXInterface::read_start(uint32_t bank, uint32_t addr, const ErrorStack &err) {
  return false;
}

bool
OpenRTXInterface::read(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes, const ErrorStack &err) {
  return false;
}

bool
OpenRTXInterface::read_finish(const ErrorStack &err) {
  return false;
}


bool
OpenRTXInterface::write_start(uint32_t bank, uint32_t addr, const ErrorStack &err) {
  return false;
}

bool
OpenRTXInterface::write(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes, const ErrorStack &err) {
  return false;
}

bool
OpenRTXInterface::write_finish(const ErrorStack &err) {
  return false;
}

bool
OpenRTXInterface::reboot(const ErrorStack &err) {
  return false;
}



USBDeviceInfo
OpenRTXInterface::interfaceInfo() {
  return USBDeviceInfo(USBDeviceInfo::Class::Serial, USB_VID, USB_PID, false);
}

QList<USBDeviceDescriptor>
OpenRTXInterface::detect(bool saveOnly) {
  QList<USBDeviceDescriptor> interfaces;
  // Find matching serial port by VID/PID.
  logDebug() << "Search for serial port with matching VID:PID " <<
    QString::number(USB_VID, 16) << ":" << QString::number(USB_PID, 16) << ".";
  QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
  foreach (QSerialPortInfo port, ports) {
    if (port.hasProductIdentifier() && (USB_PID == port.productIdentifier()) &&
        port.hasVendorIdentifier() && (USB_VID == port.vendorIdentifier())) {
      interfaces.append(USBSerial::Descriptor(USB_VID, USB_PID, port.portName(), saveOnly));
      logDebug() << "Found " << port.portName() << " (USB "
                 << QString::number(USB_VID, 16) << ":" << QString::number(USB_PID, 16) << ").";
    }
  }
  return interfaces;
}
