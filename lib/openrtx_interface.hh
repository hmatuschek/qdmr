#ifndef OPENRTXINTERFACE_HH
#define OPENRTXINTERFACE_HH

#include "radiointerface.hh"
#include "dfu_libusb.hh"

/** Implements the communication interface to radios running the OpenRTX firmware.
 *
 * @ingroup ortx */
class OpenRTXInterface : public DFUDevice, public RadioInterface
{
  Q_OBJECT

public:
  explicit OpenRTXInterface(const USBDeviceDescriptor &descr, const ErrorStack &err=ErrorStack(), QObject *parent = nullptr);

  bool isOpen() const;
  void close();

  RadioInfo identifier(const ErrorStack &err=ErrorStack());

  bool read_start(uint32_t bank, uint32_t addr, const ErrorStack &err=ErrorStack());
  bool read(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes, const ErrorStack &err=ErrorStack());
  bool read_finish(const ErrorStack &err=ErrorStack());

  bool write_start(uint32_t bank, uint32_t addr, const ErrorStack &err=ErrorStack());
  bool write(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes, const ErrorStack &err=ErrorStack());
  bool write_finish(const ErrorStack &err=ErrorStack());

  bool reboot(const ErrorStack &err=ErrorStack());


};

#endif // OPENRTXINTERFACE_HH
