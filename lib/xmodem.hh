#ifndef XMODEM_HH
#define XMODEM_HH

#include "usbserial.hh"

/** Implements the XMODEM protocol (1k + crc16 variant) for USB-Serial devices.
 *
 * Provides two methods to send and receive an entire "file".
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

  /** Receives an entire file from the device.
   * @param buffer The buffer to store the data in. The contents of the buffer will be cleared.
   * @param timeout Specifies the time-out in milliseconds. If a negative number is set, no
   *        time-out applies.
   * @param err Optional stack for error messages. */
  bool receive(QByteArray &buffer, int timeout=-1, const ErrorStack &err=ErrorStack());

  /** Sends the contents of @c buffer to the device.
   *  @param buffer [in] Specifies the data to send.
   *  @param timeout Specifies the time-out in milliseconds. If a negativ number is passed, no
   *         time-out applies.
   *  @param err Specifies the stack for error messages. */
  bool send(const QByteArray& buffer, int timeout=-1, const ErrorStack &err=ErrorStack());

protected:
  /** Recives a single byte from the device. */
  bool rxByte(uint8_t &b, int timeout=-1, const ErrorStack &err=ErrorStack());
  /** Sends a single byte to the device. */
  bool txByte(uint8_t b, int timeout=-1, const ErrorStack &err=ErrorStack());
  /** Receives exactly @c size bytes from the device. */
  bool rxBytes(uint8_t *data, unsigned int size, int timeout=-1, const ErrorStack &err=ErrorStack());
  /** Sends exactly @c size bytes to the device. */
  bool txBytes(const uint8_t *data, unsigned int size, int timeout=-1, const ErrorStack &err=ErrorStack());
  /** Computes the CRC16 checksum. */
  static uint16_t crc_ccitt(const QByteArray &data);

private:
  /// State of the state machine.
  State _state;
  /// Maximum number of retires.
  unsigned int _maxRetry;
};

#endif // XMODEM_HH
