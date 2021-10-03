#ifndef ANYTONEINTERFACE_HH
#define ANYTONEINTERFACE_HH

#include "usbserial.hh"

/** Implements the interface to Anytone D868UV, D878UV, etc radios.
 *
 * This interface uses a USB serial-port to comunicate with the device. To find the corresponding
 * port, the device-specific VID @c 0x28e9 and PID @c 0x018a are used. Hence no udev rules are
 * needed to access these devices. The user, however, should be a member of the @c dialout group
 * to get access to the serial interfaces.
 *
 * @ingroup anytone */
class AnytoneInterface : public USBSerial
{
  Q_OBJECT

public:
  /** Collects information about the particular radio being accessed. */
  struct RadioVariant {
    /** The name of the radio. */
    QString name;
    /** A code for which bands are supported. */
    char bands;
    /** The (firmware/hardware) version. */
    QString version;

    /** Empty constructor. */
    RadioVariant();
    /** Returns @c true if the radio info is valid. */
    bool isValid() const;
  };

public:
  /** Constructs a new interface to Anyton radios. If a matching device was found, @c isOpen
   * returns @c true. */
  explicit AnytoneInterface(QObject *parent=nullptr);
  /** Destructor. */
  virtual ~AnytoneInterface();

  /** Closes the interface to the device. */
  void close();

  /** Returns an identifier of the radio. */
  RadioInfo identifier();

  /** Reads the radio info from the device and returns it.
   * The information is only read once. */
  bool getInfo(RadioVariant &info);

  bool read_start(uint32_t bank, uint32_t addr);
  bool read(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes);
  bool read_finish();

  bool write_start(uint32_t bank, uint32_t addr);
  bool write(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes);
  bool write_finish();

  bool reboot();

protected:
  /** Send command message to radio to ender program state. */
  bool enter_program_mode();
  /** Sends a request to radio to identify itself. */
  bool request_identifier(RadioVariant &info);
  /** Sends a command message to radio to leave program state and reboot. */
  bool leave_program_mode();
  /** Internal used method to send messages to and receive responses from radio. */
  bool send_receive(const char *cmd, int clen, char *resp, int rlen);

protected:
  /** Binary representation of a read request to the radio. */
  struct __attribute__((packed)) ReadRequest {
    char cmd;      ///< Fixed to 'R'.
    uint32_t addr; ///< Memory address in little-endian.
    uint8_t size;  ///< Fixed to 16.
    /// Constructs a read request for the specified address.
    ReadRequest(uint32_t addr);
  };

  /** Binary representation of a read response from the radio. */
  struct __attribute__((packed)) ReadResponse {
    char cmd;      ///< Fixed to 'W'.
    uint32_t addr; ///< Memory address in big-endian.
    uint8_t size;  ///< Fixed to 16.
    char data[16]; ///< The actual data.
    uint8_t sum;   ///< Sum over address, size and data.
    uint8_t ack;   ///< Fixed to 0x06.
    /** Check the response, returns @c true if read request was successful.
     * @param addr The read address to verify.
     * @param msg On error, contains a message describing the issue. */
    bool check(uint32_t addr, QString &msg) const;
  };

  /** Binary representation of a write request to the radio. */
  struct __attribute__((packed)) WriteRequest {
    char cmd;      ///< Fixed to 'W'
    uint32_t addr; ///< Memory address in big-endian.
    uint8_t size;  ///< Fixed to 16
    char data[16]; ///< The actual data.
    uint8_t sum;   ///< Sum over addr, size and data.
    uint8_t ack;   ///< Fixed to 0x06;

    /** Assembles a write request message to the given address with the given data.
     * @param addr Specifies the address to write to.
     * @param data 16 bytes of payload. */
    WriteRequest(uint32_t addr, const char *data);
  };

  /** Structure of radio information response. */
  struct __attribute__((packed)) RadioInfoResponse {
    char prefix;       ///< Fixed prefix. Set to 'I' on success.
    char model[7];     ///< Model name.
    uint8_t bands;     ///< Frequency bands supported by radio.
    char version[6];   ///< Version number. Either hardware or 'radio' version.
    uint8_t eot;       ///< Fixed 0x06 on success.
  };

  /** Possible states of the radio interface. */
  enum State {
    STATE_INITIALIZED, ///< Initial state.
    STATE_OPEN,        ///< Interface to radio is open.
    STATE_PROGRAM,     ///< Radio is in program mode.
    STATE_CLOSED,      ///< Interface to radio is closed (captive final state).
    STATE_ERROR        ///< An error occurred (captive final state),
                       ///  use @c errorMessage() to get an error message.
  };

  /** Holds the state of the interface. */
  State _state;
  /** Holds the radio info. */
  RadioVariant _info;
};

#endif // ANYTONEINTERFACE_HH
