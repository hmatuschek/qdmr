#include "libdmrconf/usbdevice.hh"
#include "libdmrconf/anytone_radio.hh"

int main(void) {
  // Example code to detect an AnytoneDevice

  // First, search matching devices (only AnyTones)
  QList<USBDeviceDescriptor> devices = AnytoneInterface::detect();
  if (1 != devices.count()) {
    // Either none or more than one device found...
    return -1;
  }

  // A place to put error messages
  ErrorStack err;
  Radio *radio = AnytoneRadio::detect(devices.first(), RadioInfo(), err);
  if (nullptr == radio) {
    // There went something wrong, check err.
    return -1;
  }

  // Read codeplug from device blocking.
  if (! radio->startDownload(true, err)) {
    // Some download error, check err.
    delete radio;
    return -1;
  }

  // Decode codeplug into genericCodeplug
  Config genericCodeplug;
  if (! radio->codeplug().decode(&genericCodeplug, err)) {
    // Some decoding error, check err.
    delete radio;
    return -1;
  }

  // Do whatever you like with the codeplug.

  return 0;
}
