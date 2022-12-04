#ifndef DR1801UVINTERFACE_HH
#define DR1801UVINTERFACE_HH

#include "auctus_a6_interface.hh"


/** Implements the actual interface to the DR-1801UV, which builds upon the @c AuctusA6Interface.
 *
 * @ingroup dr1801uv. */
class DR1801UVInterface : public AuctusA6Interface
{
  Q_OBJECT

protected:
  /** Implemented commands. */
  enum Command {
    REQUEST_INFO           = 0x0000, ///< Returns some information about the device.
    ENTER_PROGRAMMING_MODE = 0x0104, ///< Puts the device into the programming mode.
    CHECK_PROG_PASSWORD    = 0x002b, ///< Checks the programming password.
    PREPARE_CODEPLUG_READ  = 0x0100, ///< Prepares the codeplug read. Does not start reading.
    START_READ_DATA        = 0x0101, ///< Actually starts reading the codeplug.
    START_WRITE_DATA       = 0x0102  ///< Starts writing the codeplug.
  };

  struct Q_PACKED PrepareReadRequest {
    uint32_t baudrate;

    PrepareReadRequest(uint32_t baudrate);
    void setBaudrate(uint32_t baudrate);
  };

  /** Response to a @c PREPARE_CODEPLUG_READ command.
   *  Contains some information about the codeplug to read. */
  struct Q_PACKED PrepareReadResponse {
    uint8_t  success;                ///< If successful, set to 0x01.
    uint32_t size;                   ///< Contains the size of the codeplug in big-endian.
    uint8_t  _unknown[10];           ///< Some additional information.

    bool isSuccessful() const;
    uint32_t getSize() const;
  };

public:
  DR1801UVInterface(const USBDeviceDescriptor &descriptor,
                    const ErrorStack &err=ErrorStack(), QObject *parent=nullptr);

  RadioInfo identifier(const ErrorStack &err);

  bool read_start(uint32_t bank, uint32_t addr, const ErrorStack &err=ErrorStack());
  bool read(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes, const ErrorStack &err=ErrorStack());
  bool read_finish(const ErrorStack &err=ErrorStack());

  bool write_start(uint32_t bank, uint32_t addr, const ErrorStack &err=ErrorStack());
  bool write(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes, const ErrorStack &err=ErrorStack());
  bool write_finish(const ErrorStack &err=ErrorStack());

public:
  /** Returns some information about this interface. */
  static USBDeviceInfo interfaceInfo();
  /** Tries to find all interfaces connected AnyTone radios. */
  static QList<USBDeviceDescriptor> detect();

protected:
  bool getDeviceInfo(QString &info, const ErrorStack &err=ErrorStack());
  bool enterProgrammingMode(const ErrorStack &err=ErrorStack());
  bool checkProgrammingPassword(const ErrorStack &err=ErrorStack());
  bool prepareReading(uint32_t baudrate, PrepareReadResponse &response, const ErrorStack &err=ErrorStack());
  bool startReading(const ErrorStack &err=ErrorStack());

protected:
  QString _identifier;
  uint32_t _bytesToRead;
};

#endif // DR1801UVINTERFACE_HH
