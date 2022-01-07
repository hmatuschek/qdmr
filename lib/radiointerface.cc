#include "radiointerface.hh"

#include "anytone_interface.hh"
#include "opengd77_interface.hh"
#include "radioddity_interface.hh"
#include "tyt_interface.hh"

/* ********************************************************************************************* *
 * Implementation of RadioInterface::Descriptor
 * ********************************************************************************************* */
RadioInterface::Descriptor::Descriptor(const InterfaceInfo &info, const QString &device)
  : InterfaceInfo(info), _device(device)
{
  // pass...
}

RadioInterface::Descriptor::Descriptor(const InterfaceInfo &info, const USBDeviceAddress &device)
  : InterfaceInfo(info), _device(QVariant::fromValue<USBDeviceAddress>(device))
{
  // pass...
}

RadioInterface::Descriptor::Descriptor(const Descriptor &other)
  : InterfaceInfo(other), _device(other._device)
{
  // pass...
}

RadioInterface::Descriptor &
RadioInterface::Descriptor::operator =(const Descriptor &other) {
  InterfaceInfo::operator =(other);
  _device = other._device;
  return *this;
}

QString
RadioInterface::Descriptor::description() const {
  if (InterfaceInfo::Class::Serial == _class) {
    return QString("Serial interface '%1'").arg(_device.toString());
  } else if (InterfaceInfo::Class::DFU == _class) {
    USBDeviceAddress addr = _device.value<USBDeviceAddress>();
    return QString("USB device in DFU mode: bus %1, device %2").arg(addr.bus).arg(addr.device);
  } else if (InterfaceInfo::Class::HID == _class) {
    USBDeviceAddress addr = _device.value<USBDeviceAddress>();
    return QString("USB HID: bus %1, device %2").arg(addr.bus).arg(addr.device);
  }
  return "Invalid";
}

const QVariant &
RadioInterface::Descriptor::device() const {
  return _device;
}


/* ********************************************************************************************* *
 * Implementation of RadioInterface
 * ********************************************************************************************* */
RadioInterface::RadioInterface()
{
	// pass...
}

RadioInterface::~RadioInterface() {
  // pass...
}

bool
RadioInterface::write_finish(const ErrorStack &err) {
  Q_UNUSED(err)
  return true;
}

bool
RadioInterface::reboot(const ErrorStack &err) {
  Q_UNUSED(err)
  return true;
}

QList<RadioInterface::Descriptor>
RadioInterface::detect() {
  QList<RadioInterface::Descriptor> res;
  res.append(AnytoneInterface::detect());
  res.append(OpenGD77Interface::detect());
  res.append(RadioddityInterface::detect());
  res.append(TyTInterface::detect());
  return res;
}
