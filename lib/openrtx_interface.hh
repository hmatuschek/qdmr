#ifndef OPENRTXINTERFACE_HH
#define OPENRTXINTERFACE_HH

#include "radiointerface.hh"
#include "packetstream.hh"
#include "usbserial.hh"
#include "xmodem.hh"

/** Implements the communication interface to radios running the OpenRTX firmware.
 *
 * The protocol is called rtxlink and is documented at https://openrtx.org/#/rtxlink. The protocol
 * has several layers. The lowest is a serial interface either as a VCOM (UBS CDC-ACM) or a proper
 * hardware UART. Ontop of that, there is SLIP. Followed by a simple framing layer, that determines
 * the higher-level protocol.
 * @ingroup ortx */
class OpenRTXInterface : public USBSerial
{
  Q_OBJECT

public:
  /** Constructor.
   * @param descr The USB device descriptor. Used to identify a specific USB device.
   * @param err The stack of error messages.
   * @param parent The QObject parent. */
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
