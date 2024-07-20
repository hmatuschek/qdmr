#include "c7000device.hh"
#include "logger.hh"
#include <QtEndian>
#include <QThread>

#define C7000_VID 0x1206
#define C7000_PID 0x0227


/* ********************************************************************************************* *
 * Implementation of C7000Device::Packet
 * ********************************************************************************************* */
C7000Device::Packet::Packet()
  : _encoded()
{
  // pass...
}

C7000Device::Packet::Packet(uint8_t command, uint8_t sub, uint8_t flags, const QByteArray &payload)
{
  _encoded.resize(9 + payload.size()); _encoded.fill(0);
  _encoded[0] = 0x68;
  _encoded[1] = flags;
  _encoded[2] = command;
  _encoded[3] = sub;
  *((uint16_t *)(_encoded.data()+6)) = qToLittleEndian((uint16_t)payload.size());
  memcpy(_encoded.data()+8,payload.constData(), payload.size());
  _encoded[8+payload.size()] = 0x10;

  uint32_t crc = 0xffff;
  for (int i=0; i<(_encoded.size()/2); i++) {
    uint16_t v = qFromLittleEndian(*(uint16_t *)(_encoded.constData()+2*i));
    if (crc < v) crc += 0xffff;
    crc -= v;
  }
  if (_encoded.size()%2) {
    uint16_t v = _encoded[_encoded.size()-1];
    if (crc < v) crc += 0xffff;
    crc -= v;
  }
  *((uint16_t *)(_encoded.data()+4)) = qToLittleEndian((uint16_t)crc);
}

C7000Device::Packet::Packet(const QByteArray &buffer)
  : _encoded()
{
  _encoded.append(buffer);
  if (! isValid())
    _encoded.clear();
}


bool
C7000Device::Packet::isValid() const {
  if (_encoded.size() < 9) return false;
  if (0x68 != _encoded[0]) return false;
  if (0x10 != _encoded[8+payloadSize()]) return false;
  int size = _encoded.size(); //9 + payloadSize();

  uint32_t crc = 0xffff;
  for (int i=0; i<size; i++) {
    uint16_t v = qFromLittleEndian(*(uint16_t *)(_encoded.constData()+2*i));
    if (crc < v) crc += 0xffff;
    crc -= v;
  }
  if (size%2) {
    uint16_t v = _encoded[_encoded.size()-1];
    if (crc < v) crc += 0xffff;
    crc -= v;
  }
  /*if (crc)
    return false;*/

  return true;
}

uint8_t
C7000Device::Packet::flags() const {
  return _encoded[1];
}

uint8_t
C7000Device::Packet::command() const {
  return _encoded[2];
}

uint8_t
C7000Device::Packet::subcommand() const {
  return _encoded[3];
}

uint16_t
C7000Device::Packet::payloadSize() const {
  return qFromLittleEndian(*(const uint16_t *)(_encoded.constData()+6));
}

QByteArray
C7000Device::Packet::payload() const {
  return _encoded.mid(8,payloadSize());
}

const QByteArray &
C7000Device::Packet::encoded() const {
  return _encoded;
}


/* ********************************************************************************************* *
 * Implementation of C7000Device::Descriptor
 * ********************************************************************************************* */
C7000Device::Descriptor::Descriptor(const USBDeviceInfo &info, uint8_t bus, uint8_t device)
  : USBDeviceDescriptor(info, USBDeviceHandle(bus, device))
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of C7000Device
 * ********************************************************************************************* */
C7000Device::C7000Device(const USBDeviceDescriptor &descr, const ErrorStack &err, QObject *parent)
  : QObject(parent), _ctx(nullptr), _dev(nullptr)
{
  if (USBDeviceInfo::Class::C7K != descr.interfaceClass()) {
    errMsg(err) << "Cannot connect to C7000 device using a non C7K descriptor: "
                << descr.description() << ".";
    return;
  }

  int error = libusb_init(&_ctx);
  if (error < 0) {
    errMsg(err) << "Libusb init failed (" << error << "): "
                << libusb_strerror((enum libusb_error) error) << ".";
    return;
  }

  int num=0;
  libusb_device **lst;
  libusb_device *dev=nullptr;
  if (0 > (num = libusb_get_device_list(_ctx, &lst))) {
    errMsg(err) << "Cannot obtain list of USB devices.";
    libusb_exit(_ctx);
    _ctx = nullptr;
    return;
  }

  logDebug() << "Try to detect USB C7000 interface " << descr.description() << ".";
  USBDeviceHandle addr = descr.device().value<USBDeviceHandle>();
  for (int i=0; (i<num)&&(nullptr!=lst[i]); i++) {
    if (addr.bus != libusb_get_bus_number(lst[i]))
      continue;
    if (addr.device != libusb_get_device_address(lst[i]))
      continue;
    libusb_device_descriptor usb_descr;
    if (0 > libusb_get_device_descriptor(lst[i],&usb_descr))
      continue;
    if (descr.vendorId() != usb_descr.idVendor)
      continue;
    if (descr.productId() != usb_descr.idProduct)
      continue;
    logDebug() << "Matching device found at bus " << addr.bus << ", device " << addr.device
               << " with vendor ID " << QString::number(usb_descr.idVendor, 16)
               << " and product ID " << QString::number(usb_descr.idProduct, 16) << ".";
    libusb_ref_device(lst[i]); dev = lst[i];
  }
  // Unref all devices and free list, matching device was referenced earlier
  libusb_free_device_list(lst, 1);

  if (nullptr == dev) {
    errMsg(err) << "No matching device found: " << descr.description() << ".";
    libusb_exit(_ctx);
    _ctx = nullptr;
    return;
  }

  if (0 > (error = libusb_open(dev, &_dev))) {
    errMsg(err) << "Cannot open device " << descr.description()
                << ": " << libusb_strerror((enum libusb_error) error) << ".";
    libusb_unref_device(dev);
    libusb_exit(_ctx);
    _ctx = nullptr;
    return;
  }


  if (libusb_kernel_driver_active(_dev, 0) && libusb_detach_kernel_driver(_dev, 0)) {
    errMsg(err) << "Cannot detach kernel driver for device " << descr.description()
                << ". Interface claim will likely fail.";
  }

  if (0 > (error = libusb_claim_interface(_dev, 0))) {
    errMsg(err) << "Failed to claim USB interface "  << descr.description()
                << ": " << libusb_strerror((enum libusb_error) error) << ".";
    libusb_close(_dev);
    _dev = nullptr;
    libusb_exit(_ctx);
    _ctx = nullptr;
    return;
  }

  logDebug() << "Connected to C7000 device " << descr.description() << ".";
}

C7000Device::~C7000Device() {
  close();
}

USBDeviceInfo
C7000Device::interfaceInfo() {
  return USBDeviceInfo(USBDeviceInfo::Class::C7K, C7000_VID, C7000_PID);
}


QList<USBDeviceDescriptor>
C7000Device::detect(bool saveOnly)
{
  Q_UNUSED(saveOnly)
  QList<USBDeviceDescriptor> res;

  int error, num;
  libusb_context *ctx;
  if (0 > (error = libusb_init(&ctx))) {
    logError() << "Libusb init failed (" << error << "): "
               << libusb_strerror((enum libusb_error) error) << ".";
    return res;
  }

  libusb_device **lst;
  if (0 == (num = libusb_get_device_list(ctx, &lst))) {
    logDebug() << "No USB devices found at all.";
    // unref devices and free list
    libusb_free_device_list(lst, 1);
    return res;
  }

  logDebug() << "Search for C7000 devices matching VID:PID "
             << QString::number(C7000_VID, 16) << ":" << QString::number(C7000_PID, 16) << ".";
  for (int i=0; (i<num)&&(nullptr!=lst[i]); i++) {
    libusb_device_descriptor descr;
    libusb_get_device_descriptor(lst[i], &descr);
    if ((C7000_VID == descr.idVendor) && (C7000_PID == descr.idProduct)) {
      logDebug() << "Found device on bus=" << libusb_get_bus_number(lst[i])
                 << ", device=" << libusb_get_device_address(lst[i])
                 << " with " << QString::number(C7000_VID, 16) << ":" << QString::number(C7000_PID, 16) << ".";
      res.append(C7000Device::Descriptor(
                   USBDeviceInfo(USBDeviceInfo::Class::C7K, C7000_VID, C7000_PID),
                   libusb_get_bus_number(lst[i]),
                   libusb_get_device_address(lst[i])));
    }
  }

  libusb_free_device_list(lst, 1);
  return res;
}

bool
C7000Device::isOpen() const {
  return nullptr != _dev;
}

void
C7000Device::close() {
  logDebug() << "Close C7000 interface.";
  if (nullptr != _dev) {
    libusb_release_interface(_dev, 0);
    libusb_close(_dev);
  }
  if (nullptr != _ctx)
    libusb_exit(_ctx);
  _ctx = nullptr;
  _dev = nullptr;
}

bool
C7000Device::sendRecv(const Packet &request, Packet &response, const ErrorStack &err) {
  if (! request.isValid()) {
    errMsg(err) << "Cannot send invalid request.";
    return false;
  }

  uint8_t buffer[64];
  int bytes_send, bytes_received;

  memcpy(buffer, request.encoded().constData(), request.encoded().size());
  int ret = libusb_bulk_transfer(_dev, 0x02, buffer, request.encoded().size(), &bytes_send, 1000);
  if (ret) {
    errMsg(err) << "Cannot send command to device: " << libusb_error_name(ret) << ".";
    return false;
  }
  QObject().thread()->usleep(1000);

  unsigned int retry_count = 0;
retry_receive:
  ret = libusb_bulk_transfer(_dev, 0x81, buffer, 64, &bytes_received, 1000);
  if (ret) {
    errMsg(err) << "Cannot receive response from device: " << libusb_error_name(ret) << ".";
    return false;
  }

  if (0x68 != buffer[0]) {
    if ((++retry_count) > 10) {
      errMsg(err) << "Cannot receive response from device: Retry count of 10 exceeded.";
      return false;
    }
    goto retry_receive;
  }

  response = Packet(QByteArray((const char *)buffer, bytes_received));
  if (! response.isValid()) {
    errMsg(err) << "Invalid response received.";
    return false;
  }

  return true;
}
