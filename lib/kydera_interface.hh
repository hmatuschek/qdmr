#ifndef KYDERAINTERFACE_HH
#define KYDERAINTERFACE_HH

#include "usbserial.hh"

/** Implements the protocol to read/write codeplugs and call-sign DBs for Kydera devices.
 *
 * This protocol is very simple. It is based on serial-over-USB and uses simple plaintext commands
 * to control the dataflow and radio.
 *
 * @ingroup kydera */
class KyderaInterface : public USBSerial
{
  Q_OBJECT

protected:
  /** The read command.
   * It contains the command as well as the number of block (-1) to read. Each block is 2048bytes
   * long. There are also uknown fields, likely some address to read from. */
  struct __attribute__((packed)) ReadCommand {
    char cmd[11];          ///< The ASCII command to do.
    uint8_t unused0b;      ///< Unused set to 0x00.
    uint8_t unknown0c;     ///< Unknown set to 0x3c.
    uint8_t unused0d[4];   ///< Unused, set to 0x00.
    uint16_t blocks;       ///< The number of blocks to transfer, big endian!
    uint8_t unused13[12];  ///< Unused, set to 0x00.

    /** Constructs a read request. */
    ReadCommand(uint16_t blocks);
  };

  /** The response of a read command.
   * Contains the response string as well as some additional information (22b). This response is
   * followed by a device information message. */
  struct __attribute__((packed)) ReadResponse {
    char response[12];     ///< Response string, should be '  Read_2M_\0\0'.
    uint8_t unknown0c[10]; ///< Unknown data, also contains the number of blocks to read?

    /** Checks the response. */
    bool check() const;
    /** Returns the number of blocks to be read. */
    uint8_t blocks() const;
  };

  /** The write command.
   * It contains the command as well as the number of block to write. Each block is 2048bytes
   * long. There are also uknown fields, likely some address to write to. */
  struct __attribute__((packed)) WriteCommand {
    char cmd[11];          ///< The ASCII command to do.
    uint8_t unused0b;      ///< Unused set to 0x00.
    uint8_t unknown0c;     ///< Unknown set to 0x3c.
    uint8_t unused0d[4];   ///< Unused, set to 0x00.
    uint16_t blocks;       ///< The number of blocks to transfer, big endian!
    uint8_t unused13[12];  ///< Unused, set to 0x00.

    /** Constructs a write request. */
    WriteCommand(uint16_t blocks);
  };

  /** The response to a write command.
   * This only contains the response string (12b). This response is followed by a device information
   * message. */
  struct __attribute__((packed)) WriteResponse {
    char response[12];     ///< Response string, should be '  Write_2M_\0'.

    /** Checks the response. */
    bool check() const;
  };

  /** The device information message.
   * This message is send by the device after a read/write command response without any other
   * query by the host (83b). */
  struct __attribute__((packed)) DeviceInfo {
    char revision[16];     ///< Some device/firmware information.
    char date[16];         ///< Some date.
    uint8_t unused36[5];   ///< Empty, set to 0x00.
    char timestamp[10];    ///< Some time stamp.
    char info[34];         ///< Device ID and frequency range.

    /** Unpacks the device ID string. */
    QString device() const;
  };

  /** Possible states of the interface. */
  enum class State {
    Idle,                  ///< Idle, connected to the radio.
    Read,                  ///< Reading from the radio.
    Write,                 ///< Writing to the radio.
    Error                  ///< Captive error state.
  };

public:
  /** Constructs an interface to the Kydera device. */
  KyderaInterface(const USBDeviceDescriptor &descriptor, const ErrorStack &err=ErrorStack(), QObject *parent=nullptr);

  bool isOpen() const;
  RadioInfo identifier(const ErrorStack &err=ErrorStack());

  bool write_start(uint32_t bank, uint32_t addr, const ErrorStack &err=ErrorStack());
  bool write(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes, const ErrorStack &err=ErrorStack());
  bool write_finish(const ErrorStack &err=ErrorStack());

  bool read_start(uint32_t bank, uint32_t addr, const ErrorStack &err=ErrorStack());
  bool read(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes, const ErrorStack &err=ErrorStack());
  bool read_finish(const ErrorStack &err);

public:
  /** Searches for all USB serial ports matching Kydera devices. */
  static QList<USBDeviceDescriptor> detect();

protected:
  /** Internal used method to read blocking. */
  bool receive(char *buffer, unsigned size, const ErrorStack &err=ErrorStack());

protected:
  /** Internal state of the interface. */
  State _state;
  /** The radio info once read, identifying the actual device. */
  RadioInfo _radioInfo;
};

#endif // KYDERAINTERFACE_HH
