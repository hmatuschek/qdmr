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
 * @ingroup d878uv
 */
class AnytoneInterface : public USBSerial
{
  Q_OBJECT

public:
  /** Constructs a new interface to Anyton radios. If a matching device was found, @c isOpen
   * returns @c true. */
  explicit AnytoneInterface(QObject *parent=nullptr);
  /** Destructor. */
  virtual ~AnytoneInterface();

  /** Closes the interface to the device. */
  void close();

  /** Returns an identifier of the radio. */
  QString identifier();

  bool read_start(uint32_t bank, uint32_t addr);
  bool read(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes);
  bool read_finish();

  bool write_start(uint32_t bank, uint32_t addr);
  bool write(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes);
  bool write_finish();

  bool reboot();

protected:
  bool enter_program_mode();
  bool request_identifier(QString &radio, QString &version);
  bool leave_program_mode();
  bool send_receive(const char *cmd, int clen, char *resp, int rlen);

protected:
  typedef struct __attribute__((packed)) {
    char cmd;      ///< Fixed to 'W'.
    uint32_t addr; ///< Memory address in little-endian.
    uint8_t size;  ///< Fixed to 64.
    /// Constructs a read request for the specified address.
    void initRead(uint32_t addr);
  } ReadRequest;

  typedef struct __attribute__((packed)) {
    char cmd;      ///< Fixed to 'R'.
    uint32_t addr; ///< Memory address in little-endian.
    uint8_t size;  ///< Fixed to 64.
    char data[64]; ///< The actual data.
    uint8_t sum;   ///< Sum over address, size and data.
    uint8_t ack;   ///< Fixed to 0x06.
    bool check(uint32_t addr, QString &msg) const;
  } ReadResponse;

  typedef struct __attribute__((packed)) {
    char cmd;      ///< Fixed to 'W'
    uint32_t addr; ///< Memory address in little-endian.
    uint8_t size;  ///< Fixed to 16
    char data[16]; ///< The actual data.
    uint8_t sum;   ///< Sum over addr, size and data.
    uint8_t ack;   ///< Fixed to 0x06;

    void initWrite(uint32_t addr, const char *data);
  } WriteRequest;

  typedef enum {
    STATE_INITIALIZED,
    STATE_OPEN,
    STATE_PROGRAM,
    STATE_CLOSED,
    STATE_ERROR
  } State;

  State _state;
  QString _identifier;
};

#endif // ANYTONEINTERFACE_HH
