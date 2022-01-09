#include "usbdevice.hh"
#include <QTextStream>

#include "radioinfo.hh"


/* ********************************************************************************************* *
 * Implementation of InterfaceInfo
 * ********************************************************************************************* */
USBDeviceInfo::USBDeviceInfo()
  : _class(Class::None), _vid(0), _pid(0)
{
  // pass...
}

USBDeviceInfo::USBDeviceInfo(Class cls, uint16_t vid, uint16_t pid)
  : _class(cls), _vid(vid), _pid(pid)
{
  // pass...
}

USBDeviceInfo::USBDeviceInfo(const USBDeviceInfo &other)
  : _class(other._class), _vid(other._vid), _pid(other._pid)
{
  // pass...
}

USBDeviceInfo &
USBDeviceInfo::operator =(const USBDeviceInfo &other) {
  _class = other._class;
  _vid = other._vid;
  _pid = other._pid;
  return *this;
}

bool
USBDeviceInfo::operator ==(const USBDeviceInfo &other) const {
  return (other._class == _class) && (other._vid == _vid) && (other._pid == _pid);
}
bool
USBDeviceInfo::operator !=(const USBDeviceInfo &other) const {
  return !(*this == other);
}

USBDeviceInfo::~USBDeviceInfo() {
  // pass...
}

bool
USBDeviceInfo::isValid() const {
  return Class::None != _class;
}

USBDeviceInfo::Class
USBDeviceInfo::interfaceClass() const {
  return _class;
}

uint16_t
USBDeviceInfo::vendorId() const {
  return _vid;
}
uint16_t
USBDeviceInfo::productId() const {
  return _pid;
}

QString
USBDeviceInfo::description() const {
  QString res;
  QTextStream stream(&res);
  switch (_class) {
  case Class::None:
    stream << "Invalid";
    break;
  case Class::Serial:
    stream << "USB-serial interface " << QString::number(_vid,16) << ":" << QString::number(_pid,16);
    break;
  case Class::DFU:
    stream << "USB device in DFU mode " << QString::number(_vid,16) << ":" << QString::number(_pid,16);
    break;
  case Class::HID:
    stream << "HID " << QString::number(_vid,16) << ":" << QString::number(_pid,16);
    break;
  }
  return res;
}

QString
USBDeviceInfo::longDescription() const {
  QStringList radios;
  foreach (RadioInfo radio, RadioInfo::allRadios(*this, true)) {
    radios.append(QString("%1 %2").arg(radio.manufactuer(), radio.name()));
  }
  // This should not happen
  if (radios.isEmpty())
    return QString("Unknown interface.");

  return QString("Possibly interfacing %1").arg(radios.join(", "));
}



/* ********************************************************************************************* *
 * Implementation of USBDeviceDescriptor
 * ********************************************************************************************* */
USBDeviceDescriptor::USBDeviceDescriptor(const USBDeviceInfo &info, const QString &device)
  : USBDeviceInfo(info), _device(device)
{
  // pass...
}

USBDeviceDescriptor::USBDeviceDescriptor(const USBDeviceInfo &info, const USBDeviceAddress &device)
  : USBDeviceInfo(info), _device(QVariant::fromValue<USBDeviceAddress>(device))
{
  // pass...
}

USBDeviceDescriptor::USBDeviceDescriptor(const USBDeviceDescriptor &other)
  : USBDeviceInfo(other), _device(other._device)
{
  // pass...
}

USBDeviceDescriptor &
USBDeviceDescriptor::operator =(const USBDeviceDescriptor &other) {
  USBDeviceInfo::operator =(other);
  _device = other._device;
  return *this;
}

QString
USBDeviceDescriptor::description() const {
  if (USBDeviceInfo::Class::Serial == _class) {
    return QString("Serial interface '%1'").arg(_device.toString());
  } else if (USBDeviceInfo::Class::DFU == _class) {
    USBDeviceAddress addr = _device.value<USBDeviceAddress>();
    return QString("USB device in DFU mode: bus %1, device %2").arg(addr.bus).arg(addr.device);
  } else if (USBDeviceInfo::Class::HID == _class) {
    USBDeviceAddress addr = _device.value<USBDeviceAddress>();
    return QString("USB HID: bus %1, device %2").arg(addr.bus).arg(addr.device);
  }
  return "Invalid";
}

const QVariant &
USBDeviceDescriptor::device() const {
  return _device;
}


