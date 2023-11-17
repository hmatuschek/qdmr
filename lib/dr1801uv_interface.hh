#ifndef DR1801UVINTERFACE_HH
#define DR1801UVINTERFACE_HH

#include "auctus_a6_interface.hh"
#include <functional>


// Forward declarations
class Codeplug;

/** Implements the actual interface to the DR-1801UV, which builds upon the @c AuctusA6Interface.
 *
 * @ingroup dr1801uv */
class DR1801UVInterface : public AuctusA6Interface
{
  Q_OBJECT

public:
  /** Constructs an interface to the BTECH DR-1801UV from the specifeid USB descriptor. */
  DR1801UVInterface(const USBDeviceDescriptor &descriptor,
                    const ErrorStack &err=ErrorStack(), QObject *parent=nullptr);

  RadioInfo identifier(const ErrorStack &err);

  /** Reads the codeplug from the device blocking. */
  bool readCodeplug(Codeplug &codeplug, std::function<void (unsigned int, unsigned int)> progress,
                    const ErrorStack &err=ErrorStack());

  /** Writes the codeplug to the device blocking. */
  bool writeCodeplug(const Codeplug &codeplug, std::function<void(unsigned int, unsigned int)> progress,
                     const ErrorStack &err=ErrorStack());

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
  /** Some default speeds. */
  enum DefaultTransferSpeed {
    /** Initial speed, used to send commands. */
    DefaultSpeed = QSerialPort::Baud9600,
    /** Speed for reading the codeplug. */
    ReadSpeed = QSerialPort::Baud115200,
    /** Speed for writing the codeplug. */
    WriteSpeed = QSerialPort::Baud9600
  };

  /** Implemented commands. */
  enum Command {
    REQUEST_INFO           = 0x0000, ///< Returns some information about the device.
    ENTER_PROGRAMMING_MODE = 0x0104, ///< Puts the device into the programming mode.
    CHECK_PROG_PASSWORD    = 0x002b, ///< Checks the programming password.
    PREPARE_CODEPLUG_READ  = 0x0100, ///< Sets baud rate and prepares codeplug read.
    START_READ_DATA        = 0x0101, ///< Actually starts reading the codeplug.
    PREPARE_CODEPLUG_WRITE = 0x0102, ///< Prepares writing the codeplug.
    CODEPLUG_WRITTEN       = 0x0103  ///< Send by the device once the codeplug was written.
  };

  /** Request to set transfer speed and load codeplug into RAM for transfer. */
  struct Q_PACKED PrepareReadRequest {
    /** The transfer speed. */
    uint32_t baudrate;

    /** Constructor. */
    PrepareReadRequest(uint32_t baudrate);
    /** Sets the baudrate. */
    void setBaudrate(uint32_t baudrate);
  };

  /** Response to a @c PREPARE_CODEPLUG_READ command.
   *  Contains some information about the codeplug to read. */
  struct Q_PACKED PrepareReadResponse {
    uint8_t  success;                ///< If successful, set to 0x01.
    uint32_t size;                   ///< Contains the size of the codeplug in big-endian.
    uint8_t  _unknown[10];           ///< Some additional information.

    /** Returns @c true, if the operation was successful. */
    bool isSuccessful() const;
    /** Returns the codeplug size in bytes. */
    uint32_t getSize() const;
  };

  /** Request to prepare a codeplug write.
   *  Contains some information about the codeplug to write and transfer speed to use. */
  struct Q_PACKED PrepareWriteRequest {
    uint16_t _unknown0;              ///< Just set to 0x0001
    uint32_t size;                   ///< Contains the size of the codeplug in big-endian.
    uint16_t checksum;               ///< A checksum over the codeplug to be written.
    uint32_t baudRate;               ///< The baud rate for the transfer.

    /** Constructor. */
    PrepareWriteRequest(uint32_t size, uint32_t speed, uint16_t crc);
    /** Updates the crc with the data. */
    void updateCRC(const uint8_t *data, size_t length);
  };

  /** Response to a prepare-write request. Just contains a status word. */
  struct Q_PACKED PrepareWriteResponse {
    uint16_t responseCode;           ///< Response code.

    /** Returns @c true, if the operation was successful. */
    bool isSuccessful() const;
  };

  /** Response to a codeplug write. Just contains a status word. */
  struct Q_PACKED CodeplugWriteResponse {
    uint8_t  success;                ///< Response code.
    uint16_t unknown;                ///< Some unkown data.

    /** Returns @c true, if the operation was successful. */
    bool isSuccessful() const;
  };

  /** Puts the device into programming mode. */
  bool enterProgrammingMode(const ErrorStack &err=ErrorStack());
  /** Reads some information about the device. */
  bool getDeviceInfo(QString &info, const ErrorStack &err=ErrorStack());
  /** Checks the if a programming password is set. */
  bool checkProgrammingPassword(const ErrorStack &err=ErrorStack());

  /** Prepares reading the codeplug. */
  bool prepareReading(uint32_t baudrate, PrepareReadResponse &response, const ErrorStack &err=ErrorStack());
  /** Starts the read operation. Once the operation is complete, the device will close the
   * connection. */
  bool startReading(const ErrorStack &err=ErrorStack());

  /** Repares the codeplug write. */
  bool prepareWriting(uint32_t size, uint32_t baudrate, uint16_t crc, const ErrorStack &err=ErrorStack());
  /** Receives the ACK for writing the codeplug. */
  bool receiveWriteACK(const ErrorStack &err=ErrorStack());

protected:
  /** Holds the device identifier, once read. */
  QString _identifier;
};

#endif // DR1801UVINTERFACE_HH
