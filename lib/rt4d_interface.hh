//
// Created by hannes on 22.06.26.
//

#ifndef LIBDMRCONF_RT4D_INTERFACE_HH
#define LIBDMRCONF_RT4D_INTERFACE_HH

#include "usbserial.hh"

/** Implements the communication protocol for the Radtel RD-4D.
 * The protocol is documented at
 * https://github.com/dmr-tools/anytone-emu/blob/main/doc/Radtel/RT4D/radtel-protocol.md */
class RT4DInterface: public USBSerial
{
  Q_OBJECT

  /** Common command request. */
  struct Q_PACKED CommandRequest {
    /// Encodes the command type.
    enum class Type {
      Command = 0x05
    };

    /// Encodes the actual command.
    enum class Command {
      ProgMode = 0x10,
      Reboot   = 0xee
    };

    uint8_t header1;   ///< Fixed header '4'
    uint8_t header2;   ///< Fixed header 'R'
    uint8_t type;      ///< Command type.
    uint8_t command;   ///< The command.
    uint8_t crc;       ///< CRC.

    /** Constructs a command request. Also sets header and computes CRC. */
    CommandRequest(Type type, Command command);
    /** Send this command to the given device.
     * This call is blocking and returns @c true on success. */
    bool send(RT4DInterface *device, const ErrorStack &err=ErrorStack()) const;
  };


  /** A common success/ACK response to command and write requests. */
  struct Q_PACKED ACKResponse {
    uint8_t header;  ///< Fixed header 6h.
    /** Receives and checks the response. Returns @c true on success. */
    bool receive(RT4DInterface *device, const ErrorStack &err=ErrorStack());
  };


  /** A read request for exactly 1024 bytes. */
  struct Q_PACKED ReadRequest {
    uint8_t header;   ///< Fixed header `R`
    uint16_t page;    ///< Encodes the 1k page in uint16_be.
    uint8_t crc;      ///< CRC.

    /** Creates a read request for the page at the given address.
     * Also computes CRC. */
    ReadRequest(uint32_t address);
    /** Sens the request. Blocks until the entire request is sent and returns @c true on success. */
    bool send(RT4DInterface *device, const ErrorStack &err=ErrorStack()) const;
  };


  /** Read response.
   * Contains the entire page read. */
  struct Q_PACKED ReadResponse {
    uint8_t header;         ///< Fixed header 'R'.
    uint16_t page;          ///< The page being read.
    uint8_t payload[1024];  ///< Its content.
    uint8_t crc;            ///< CRC

    /** Receives a read response and also verifies the packet. */
    bool receive(RT4DInterface *device, const ErrorStack &err=ErrorStack());
    /** Computes the page address. */
    uint32_t address() const;
  };


  /** A write request for a particular page within a segment. */
  struct Q_PACKED WriteRequest {
    uint8_t
      segment : 4,          ///< The segment index/number.
      header : 4;           ///< Fixed header 9h.
    uint16_t offset_page;   ///< 1k-offset within segment.
    int8_t payload[1024];   ///< The 1k page to write.
    uint8_t crc;            ///< CRC.

    /** Constructs write request from page, offset (in bytes) and payload. */
    WriteRequest(uint8_t sequence, uint32_t offset, const uint8_t *payload, int size);

    /** Sets the request and blocks until the entire request is sent. Returns @c true on success. */
    bool send(RT4DInterface *device, const ErrorStack &err=ErrorStack()) const;
  };

protected:
  /** Possible states, the interface might be in. */
  enum class State {
    Closed, Open, Connected, Identified, Error
  };

public:
  /** Constructs a new RT4D interface for the given USB descriptor. The constructor also enters
   * the SYSINFO state and identifies the radio. */
  explicit RT4DInterface(const USBDeviceDescriptor &descr,
                         const ErrorStack &err=ErrorStack(), QObject *parent=nullptr);


  /** Returns the identifier. */
  RadioInfo identifier(const ErrorStack &err=ErrorStack()) override;
  void close() override;

  bool read_start(uint32_t bank, uint32_t address, const ErrorStack &err=ErrorStack()) override;
  bool read(uint32_t bank, uint32_t address, uint8_t *data, int nbytes, const ErrorStack &err=ErrorStack()) override;
  bool read_finish(const ErrorStack &err=ErrorStack()) override;

  bool write_start(uint32_t bank, uint32_t address, const ErrorStack &err=ErrorStack()) override;
  bool write(uint32_t bank, uint32_t address, uint8_t *data, int nbytes, const ErrorStack &err=ErrorStack()) override;
  bool write_finish(const ErrorStack &err=ErrorStack()) override;

public:
  /** Returns some information about this interface. */
  static USBDeviceInfo interfaceInfo();
  /** Tries to find all interfaces connected AnyTone radios. */
  static QList<USBDeviceDescriptor> detect(bool saveOnly=true);

protected:
  /** Enters program mode. */
  bool enter_program_mode(const ErrorStack &err = ErrorStack());
  /** Identifies the radio. */
  bool request_identifier(const ErrorStack &err = ErrorStack());
  /** Helper function to send a request and receives the associated response. */
  template<class Request, class Response>
  bool sendReceive(const Request &req, Response &res, const ErrorStack &err=ErrorStack()) {
    if (! req.send(this, err))
      return false;
    return res.receive(this, err);
  }

  /** Send some data. */
  bool send(const char *data, qint64 n, int timeout, const ErrorStack &err=ErrorStack());
  /** Receives some data. */
  bool receive(char *data, qint64 n, int timeout, const ErrorStack &err=ErrorStack());

  /** Maps an address to segment and offset. */
  static QPair<int, uint32_t> mapToSegment(uint32_t address);

protected:
  /** Current state. */
  State _state;
  /** Radio info, identifying the radio. */
  RadioInfo _info;
  /** Firmware version string. */
  QByteArray _firmwareVersion;

  /** Static map mapping address ranges to segments.
   * This is only needed because reading and writing do not follow the same address space.
   * Reading is performed in raw address space, while writing is performed on segments addressed
   * as segment index and offset within that segment. */
  static const QVector<QPair<uint32_t, uint32_t>> _segmentMap;
};

#endif //LIBDMRCONF_RT4D_INTERFACE_HH
