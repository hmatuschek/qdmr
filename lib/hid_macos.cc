#include "hid_macos.hh"
#include <IOKit/IOKitLib.h>
#include <IOKit/IOCFPlugIn.h>
#include <IOKit/usb/IOUSBLib.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <logger.hh>


/* ********************************************************************************************* *
 * Implementation of HIDevice::Descriptor
 * ********************************************************************************************* */
HIDevice::Descriptor::Descriptor(const USBDeviceInfo &info, uint32_t locationId, uint16_t device)
  : USBDeviceDescriptor(info, USBDeviceHandle((locationId>>24), device, locationId))
{
  // pass...
}

/* ********************************************************************************************* *
 * Implementation of HIDevice
 * ********************************************************************************************* */
HIDevice::HIDevice(const USBDeviceDescriptor &desc, const ErrorStack &err, QObject *parent)
  : QObject(parent), _dev(nullptr)
{
  // Create the USB HID Manager.
  _HIDManager = IOHIDManagerCreate(kCFAllocatorDefault,
                                   kIOHIDOptionsTypeNone);

  // Create an empty matching dictionary for filtering USB devices in our HID manager.
  CFMutableDictionaryRef matchDict = CFDictionaryCreateMutable(
        kCFAllocatorDefault, 2, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);

  // Specify the USB device manufacturer and product in our matching dictionary.
  int locationId = desc.device().value<USBDeviceHandle>().locationId;
  CFDictionarySetValue(matchDict, CFSTR(kIOHIDLocationIDKey), CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &locationId));

  // Apply the matching to our HID manager.
  IOHIDManagerSetDeviceMatching(_HIDManager, matchDict);
  CFRelease(matchDict);

  // The HID manager will use callbacks when specified USB devices are connected/disconnected.
  IOHIDManagerRegisterDeviceMatchingCallback(_HIDManager, &callback_open, this);
  IOHIDManagerRegisterDeviceRemovalCallback(_HIDManager, &callback_close, this);

  // Add the HID manager to the main run loop
  IOHIDManagerScheduleWithRunLoop(_HIDManager, CFRunLoopGetMain(), kCFRunLoopDefaultMode);

  // Open the HID manager
  IOReturn IOReturn = IOHIDManagerOpen(_HIDManager, kIOHIDOptionsTypeNone);
  if (IOReturn != kIOReturnSuccess) {
    errMsg(err) << "Cannot open HID manager for USB device "
                << QString::number(locationId,16) << ".";
    IOHIDManagerUnscheduleFromRunLoop(_HIDManager, CFRunLoopGetMain(), kCFRunLoopDefaultMode);
    _HIDManager = nullptr;
    return;
  }

  // Run loop until device found.
  for (int k=0; k<4; k++) {
    CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, 0);
    if (_dev)
      return;
    usleep(10000);
  }

  errMsg(err) << "Cannot open USB device "
              << QString::number(locationId,16) << ".";
  IOHIDManagerUnscheduleFromRunLoop(_HIDManager, CFRunLoopGetMain(), kCFRunLoopDefaultMode);
  IOHIDManagerClose(_HIDManager, kIOHIDOptionsTypeNone);
  _HIDManager = nullptr;
}

HIDevice::~HIDevice() {
  if (_dev)
    close();
  if (_HIDManager) {
    IOHIDManagerUnscheduleFromRunLoop(_HIDManager, CFRunLoopGetMain(), kCFRunLoopDefaultMode);
    IOHIDManagerClose(_HIDManager, kIOHIDOptionsTypeNone);
  }
}

QList<USBDeviceDescriptor>
HIDevice::detect(uint16_t vid, uint16_t pid)
{
  /* Get the IO registry which has the system information for connected hardware. */
  io_registry_entry_t entry = IORegistryGetRootEntry(kIOMasterPortDefault);
  if (entry == 0)
    return QList<USBDeviceDescriptor>();

  /* Get an iterator for the USB plane. */
  io_iterator_t iter    = 0;
  kern_return_t kret = IORegistryEntryCreateIterator(entry, kIOUSBPlane, kIORegistryIterateRecursively, &iter);
  if (kret != KERN_SUCCESS || iter == 0)
    return QList<USBDeviceDescriptor>();

  QList<USBDeviceDescriptor> devices;

  io_service_t service = 0;
  while ((service = IOIteratorNext(iter))) {
    IOCFPlugInInterface  **plug  = NULL;
    IOUSBDeviceInterface **dev   = NULL;
    io_string_t            path;
    SInt32                 score = 0;
    IOReturn               ioret;

    /* Pull out IO Plugins for each iterator. */
    kret = IOCreatePlugInInterfaceForService(service, kIOUSBDeviceUserClientTypeID, kIOCFPlugInInterfaceID, &plug, &score);
    IOObjectRelease(service);
    if (kret != KERN_SUCCESS || plug == NULL)
      continue;

    /* Get an IOUSBDeviceInterface for each USB device from the IO Plugin object. */
    ioret = (*plug)->QueryInterface(plug, CFUUIDGetUUIDBytes(kIOUSBDeviceInterfaceID), (void **)&dev);
    (*plug)->Release(plug);
    if (ioret != kIOReturnSuccess || dev == NULL)
       continue;

    /* Print out the path in the IO Plane the device is at. */
    if (IORegistryEntryGetPath(service, kIOServicePlane, path) != KERN_SUCCESS) {
      (*dev)->Release(dev);
      continue;
    }

    UInt16 dev_vendor, dev_product, dev_addr;
    UInt32 dev_location;
    if ( ((*dev)->GetDeviceVendor(dev, &dev_vendor) != kIOReturnSuccess) || (dev_vendor != vid) )
      continue;
    if ( ((*dev)->GetDeviceProduct(dev, &dev_product) != kIOReturnSuccess) || (dev_product != pid) )
      continue;
    if ((*dev)->GetLocationID(dev, &dev_location) != kIOReturnSuccess)
      continue;
    if ((*dev)->GetDeviceAddress(dev, &dev_addr) != kIOReturnSuccess)
      continue;

    logDebug() << "Found device with vid:pid " << QString::number(vid, 16)
               << ":" << QString::number(pid, 16) << " at "
               << QString::number(dev_location, 16) << ".";

    devices.append(HIDevice::Descriptor(
                     USBDeviceInfo(USBDeviceInfo::Class::HID,
                                   vid, pid),
                     dev_location, dev_addr));
    /* All done with this device. */
    (*dev)->Release(dev);
  }
  IOObjectRelease(iter);

  return devices;
}

bool
HIDevice::isOpen() const {
  return nullptr != _dev;
}

//
// Send a request to the device.
// Store the reply into the rdata[] array.
// Terminate in case of errors.
//
bool
HIDevice::hid_send_recv(const unsigned char *data, unsigned nbytes,
                        unsigned char *rdata, unsigned rlength, const ErrorStack &err)
{
  unsigned char buf[42];
  unsigned k;
  IOReturn result;

  if (! isOpen())
    return false;

  memset(buf, 0, sizeof(buf));
  buf[0] = 1;
  buf[1] = 0;
  buf[2] = nbytes;
  buf[3] = nbytes >> 8;
  if (nbytes > 0)
    memcpy(buf+4, data, nbytes);
  nbytes += 4;

  _nbytes_received = 0;
  memset(_receive_buf, 0, sizeof(_receive_buf));

  uint retrycount = 0;
again:
  // Write to HID device.
  result = IOHIDDeviceSetReport(_dev, kIOHIDReportTypeOutput, 0, buf, sizeof(buf));
  if (result != kIOReturnSuccess) {
    errMsg(err) << "HID output error: " << result << "!";
    return false;
  }

  // Run application loop until reply received.
  CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, 0);
  for (k = 0; _nbytes_received <= 0; k++) {
    usleep(100);
    CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, 0);
    if (k >= 1000) {
      retrycount++;
      if (retrycount<100)
        goto again;
      errMsg(err) << "HID IO error: Exceeded max. retry count.";
      return false;
    }
  }
  usleep(100);

  if (_nbytes_received != sizeof(_receive_buf)) {
    errMsg(err) << "Short read: " << _nbytes_received << " bytes instead of "
                << (int)sizeof(_receive_buf) << "!";
    return false;
  }

  if ((_receive_buf[0] != 3) || (_receive_buf[1] != 0) || (_receive_buf[3] != 0)) {
    errMsg(err) << "Incorrect reply. Expected {3,0,0}, got {" << int(_receive_buf[0]) << ","
                << int(_receive_buf[1]) << "," << int(_receive_buf[3]) << "}.";
    return false;
  }

  if (_receive_buf[2] != rlength) {
    errMsg(err) << "Incorrect reply length " << (int)_receive_buf[2]
                << ", expected " << rlength << "!";
    return false;
  }

  memcpy(rdata, _receive_buf+4, rlength);

  return true;
}

//
// Callback: data is received from the HID device
//
void
HIDevice::callback_input(void *context, IOReturn result, void *sender, IOHIDReportType type,
                         uint32_t reportID, uint8_t *data, CFIndex nbytes)
{
  Q_UNUSED(sender);
  Q_UNUSED(type);
  Q_UNUSED(reportID);

  HIDevice *self = reinterpret_cast<HIDevice *>(context);

  if (result != kIOReturnSuccess) {
    logError() << "HID input error: " << result << ".";
    self->close();
    return;
  }

  if (nbytes > CFIndex(sizeof(self->_receive_buf))) {
    logError() << "Too large HID input: " << (int)nbytes << " bytes!";
    self->close();
    return;
  }

  self->_nbytes_received = nbytes;
  if (nbytes > 0)
    memcpy(self->_receive_buf, data, nbytes);
}

//
// Callback: device specified in the matching dictionary has been added
//
void
HIDevice::callback_open(void *context, IOReturn result, void *sender, IOHIDDeviceRef deviceRef)
{
  Q_UNUSED(result);
  Q_UNUSED(sender);

  HIDevice *self = reinterpret_cast<HIDevice *>(context);
  IOReturn o = IOHIDDeviceOpen(deviceRef, kIOHIDOptionsTypeSeizeDevice);
  if (o != kIOReturnSuccess) {
    logError() << "Cannot open HID device!";
    return;
  }

  // Register input callback.
  IOHIDDeviceRegisterInputReportCallback(deviceRef, self->_transfer_buf, sizeof(self->_transfer_buf),
                                         callback_input, self);

  self->_dev = deviceRef;
}

//
// Callback: device specified in the matching dictionary has been removed
//
void
HIDevice::callback_close(void *context, IOReturn result, void *sender, IOHIDDeviceRef deviceRef)
{
  Q_UNUSED(result);
  Q_UNUSED(sender);

  HIDevice *self = reinterpret_cast<HIDevice *>(context);

  // De-register input callback.
  IOHIDDeviceRegisterInputReportCallback(
        deviceRef, self->_transfer_buf, sizeof(self->_transfer_buf), NULL, NULL);
}

//
// Close HID device.
//
void
HIDevice::close()
{
  if (! _dev)
    return;
  IOHIDDeviceClose(_dev, kIOHIDOptionsTypeNone);
  _dev = nullptr;
}

