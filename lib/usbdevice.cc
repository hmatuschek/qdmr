#include "usbdevice.hh"
#include <QTextStream>
#include <QSerialPortInfo>
#include <libusb.h>
#include "logger.hh"
#include "radioinfo.hh"

#include "anytone_interface.hh"
#include "radioddity_interface.hh"
#include "opengd77_interface.hh"
#include "tyt_interface.hh"
#include "dr1801uv_interface.hh"

/* ********************************************************************************************* *
 * Implementation of USBDeviceHandle
 * ********************************************************************************************* */
USBDeviceHandle::USBDeviceHandle()
  : bus(0xff), device(0xff)
{
  // pass...
}

USBDeviceHandle::USBDeviceHandle(uint8_t busno, uint8_t deviceno, uint32_t locid)
  : bus(busno), device(deviceno), locationId(locid)
{
  // pass...
}

bool
USBDeviceHandle::operator==(const USBDeviceHandle &other) {
  return (bus == other.bus) && (device == other.device);
}


/* ********************************************************************************************* *
 * Implementation of USBDeviceInfo
 * ********************************************************************************************* */
USBDeviceInfo::USBDeviceInfo()
  : _class(Class::None), _vid(0), _pid(0)
{
  // pass...
}

USBDeviceInfo::USBDeviceInfo(Class cls, uint16_t vid, uint16_t pid, bool save, bool identifiable)
  : _class(cls), _vid(vid), _pid(pid), _save(save), _identifiable(identifiable)
{
  // pass...
}

USBDeviceInfo::USBDeviceInfo(const USBDeviceInfo &other)
  : _class(other._class), _vid(other._vid), _pid(other._pid), _save(other._save),
    _identifiable(other._identifiable)
{
  // pass...
}

USBDeviceInfo &
USBDeviceInfo::operator =(const USBDeviceInfo &other) {
  _class = other._class;
  _vid = other._vid;
  _pid = other._pid;
  _save = other._save;
  _identifiable = other._identifiable;
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

bool
USBDeviceInfo::isSave() const {
  return _save;
}

bool
USBDeviceInfo::isIdentifiable() const {
  return _identifiable;
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
    radios.append(QString("%1 %2").arg(radio.manufacturer(), radio.name()));
  }
  // This should not happen
  if (radios.isEmpty())
    return QString("Unknown interface.");

  return QString("Possibly interfacing %1").arg(radios.join(", "));
}



/* ********************************************************************************************* *
 * Implementation of USBDeviceDescriptor
 * ********************************************************************************************* */
USBDeviceDescriptor::USBDeviceDescriptor()
  : USBDeviceInfo(), _device()
{
  // pass...
}

USBDeviceDescriptor::USBDeviceDescriptor(const USBDeviceInfo &info, const QString &device)
  : USBDeviceInfo(info), _device(device)
{
  // pass...
}

USBDeviceDescriptor::USBDeviceDescriptor(const USBDeviceInfo &info, const USBDeviceHandle &device)
  : USBDeviceInfo(info), _device(QVariant::fromValue<USBDeviceHandle>(device))
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

bool
USBDeviceDescriptor::isValid() const {
  if (! USBDeviceInfo::isValid())
    return false;

  // dispatch by device class
  switch (_class) {
  case Class::None:
    return false;
  case Class::Serial:
    return validSerial();
  case Class::DFU:
  case Class::HID:
    return validRawUSB();
  }
  return false;
}

bool
USBDeviceDescriptor::validRawUSB() const {
  int error, num;
  libusb_context *ctx;
  if (0 > (error = libusb_init(&ctx))) {
    logError() << "Libusb init failed (" << error << "): "
               << libusb_strerror((enum libusb_error) error) << ".";
    return false;
  }

  libusb_device **lst;
  if (0 == (num = libusb_get_device_list(ctx, &lst))) {
    logDebug() << "No USB devices found at all.";
    // unref devices and free list
    libusb_free_device_list(lst, 1);
    libusb_exit(ctx);
    return false;
  }

  USBDeviceHandle addr = _device.value<USBDeviceHandle>();
  logDebug() << "Search for a device matching VID:PID "
             << QString::number(_vid, 16) << ":" << QString::number(_pid, 16)
             << " at bus " << addr.bus << ", device " << addr.device << ".";
  bool found = false;
  for (int i=0; (i<num)&&(nullptr!=lst[i]); i++) {
    libusb_device_descriptor descr;
    libusb_get_device_descriptor(lst[i], &descr);
    found = ( (_vid == descr.idVendor) && (_pid == descr.idProduct) &&
              (libusb_get_bus_number(lst[i])==addr.bus) &&
              (libusb_get_device_address(lst[i])==addr.device) );
    if (found) {
      logDebug() << "Found device on bus=" << libusb_get_bus_number(lst[i])
                 << ", device=" << libusb_get_device_address(lst[i])
                 << " with " << QString::number(_vid, 16) << ":" << QString::number(_pid, 16) << ".";
      break;
    }
  }

  // Free list and context
  libusb_free_device_list(lst, 1);
  libusb_exit(ctx);

  // done.
  return found;
}

bool
USBDeviceDescriptor::validSerial() const {
  QSerialPortInfo info(_device.toString());

  logDebug() << "Check if serial port " << _device.toString() << " still exisist and has VID:PID "
             << QString::number(_vid, 16) << ":" << QString::number(_pid, 16) << ".";

  if (info.isNull()) {
    logDebug() << "Serial port " << info.portName() << "("
               << _device.toString() << ") is not valid anymore.";
    return false;
  }

  if (info.hasProductIdentifier() && info.hasVendorIdentifier())
    return ((_vid == info.vendorIdentifier()) && (_pid == info.productIdentifier()));
  return true;
}

QString
USBDeviceDescriptor::description() const {
  if (USBDeviceInfo::Class::Serial == _class) {
    return QString("Serial interface '%1'").arg(_device.toString());
  } else if (USBDeviceInfo::Class::DFU == _class) {
    USBDeviceHandle addr = _device.value<USBDeviceHandle>();
    return QString("USB device in DFU mode: bus %1, device %2").arg(addr.bus).arg(addr.device);
  } else if (USBDeviceInfo::Class::HID == _class) {
    USBDeviceHandle addr = _device.value<USBDeviceHandle>();
    return QString("USB HID: bus %1, device %2").arg(addr.bus).arg(addr.device);
  }
  return "Invalid";
}

const QVariant &
USBDeviceDescriptor::device() const {
  return _device;
}

QString
USBDeviceDescriptor::deviceHandle() const {
  switch (_class) {
  case Class::None:
    break;
  case Class::DFU:
  case Class::HID:
    return QString("%1:%2").arg(_device.value<USBDeviceHandle>().bus)
        .arg(_device.value<USBDeviceHandle>().device);
  case Class::Serial:
    return _device.toString();
  }

  return "[invalid]";
}

QList<USBDeviceDescriptor>
USBDeviceDescriptor::detect() {
  QList<USBDeviceDescriptor> res;
  res.append(AnytoneInterface::detect());
  res.append(OpenGD77Interface::detect());
  res.append(RadioddityInterface::detect());
  res.append(TyTInterface::detect());
  res.append(DR1801UVInterface::detect());
  return res;
}

