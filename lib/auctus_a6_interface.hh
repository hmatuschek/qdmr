/** @defgroup auctus Auctus A6/A7 Based devices.
 *
 * Several devices use the radio-on-a-chip Auctus A6, which enables very cheap DMR radios.
 *
 * @ingroup dsc */
#ifndef AUCTUS_A6_INTERFACE_HH
#define AUCTUS_A6_INTERFACE_HH

#include <QObject>
#include "usbserial.hh"

/** Implements the communication interface to radios using the Auctus A6 chip.
 *
 * This includes devices like Cotre A1 or the Baofeng DR-1801. This class only implements
 * the basic communication protocol. Specifics, like particular commands are implemented in
 * derived interface classes.
 *
 * @ingroup auctus */
class AuctusA6Interface : public USBSerial
{
  Q_OBJECT

public:
  /** Possible states of the interface. */
  enum State {
    CLOSED,        ///< Conntection to device is closed.
    IDLE,          ///< Connection is open and device is ready.
    READ_THROUGH,  ///< Read from memory. The device just sends some amount of data.
    WRITE_THROUGH, ///< Write to memory. The device just receives some amount of data.
    ERROR          ///< An error state.
  };

protected:
  /** Hidden constructor. */
  explicit AuctusA6Interface(const USBDeviceDescriptor &descriptor,
                             const ErrorStack &err=ErrorStack(), QObject *parent=nullptr);

public:
  /** Returns the interface state. */
  State state() const;

protected:
  /** Internal used method to send messages to and receive responses from radio. */
  bool send_receive(uint16_t command, const uint8_t *params, uint8_t plen,
                    uint8_t *response, uint8_t &rlen, const ErrorStack &err=ErrorStack());
  /** Internal used method to send a command. */
  bool send(uint16_t command, const uint8_t *params, uint8_t plen, const ErrorStack &err=ErrorStack());
  /** Internal used method to receive a response. */
  bool receive(uint16_t &command, uint8_t *response, uint8_t &rlen, const ErrorStack &err=ErrorStack());

protected:
  /** Holds the state of the interface. */
  State _state;
};

#endif // AUCTUS_A6_INTERFACE_HH
