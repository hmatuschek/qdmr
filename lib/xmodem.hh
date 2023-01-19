#ifndef XMODEM_HH
#define XMODEM_HH

#include "usbserial.hh"

/** Implements the XMODEM protocol (1k + crc16 variant).
 *
 * @ingroup rif */
class XModem : public USBSerial
{
  Q_OBJECT

protected:
  /** Possible states of the state machine. */
  enum class State {
    Init, Transfer, Error
  };

  /** Possible XMODEM control bytes. */
  enum CtrlByte {
    SOH = 0x01, STX = 0x02, EOT = 0x04, ACK = 0x06, NAK = 0x15, CAN = 0x18, CRC = 0x43
  };

public:
  /** Constructs a xmodem connection via the USB device specified by @c descriptor. */
  explicit XModem(const USBDeviceDescriptor &descriptor, const ErrorStack &err=ErrorStack(), QObject *parent=nullptr);

  bool receive(QByteArray &buffer, int timeout=-1, const ErrorStack &err=ErrorStack());
  bool send(const QByteArray& buffer, int timeout=-1, const ErrorStack &err=ErrorStack());

protected:
  bool rxByte(uint8_t &b, int timeout=-1, const ErrorStack &err=ErrorStack());
  bool txByte(uint8_t b, int timeout=-1, const ErrorStack &err=ErrorStack());
  bool rxBytes(uint8_t *data, unsigned int size, int timeout=-1, const ErrorStack &err=ErrorStack());
  bool txBytes(const uint8_t *data, unsigned int size, int timeout=-1, const ErrorStack &err=ErrorStack());

  static uint16_t crc_ccitt(const QByteArray &data);

private:
  /// State of the state machine.
  State _state;
  /// Maximum number of retires.
  unsigned int _maxRetry;
};

#endif // XMODEM_HH
