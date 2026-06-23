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


  struct Q_PACKED CommandRequest {
    enum class Type {
      Command = 0x05
    };
    enum class Command {
      ProgMode = 0x10,
      Reboot   = 0xee
    };

    uint8_t header1;
    uint8_t header2;
    uint8_t type;
    uint8_t command;
    uint8_t crc;

    CommandRequest(Type type, Command command);
    bool send(QIODevice *device, const ErrorStack &err=ErrorStack()) const;
  };

  struct Q_PACKED ACKResponse {
    uint8_t header;

    bool receive(QIODevice *device, const ErrorStack &err=ErrorStack());
  };

  struct Q_PACKED ReadRequest {
    uint8_t header;
    uint16_t page;
    uint8_t crc;

    ReadRequest(uint32_t address);
    bool send(QIODevice *device, const ErrorStack &err=ErrorStack()) const;
  };

  struct Q_PACKED ReadResponse {
    uint8_t header;
    uint16_t page;
    uint8_t payload[1024];
    uint8_t crc;

    bool receive(QIODevice *device, const ErrorStack &err=ErrorStack());
    uint32_t address() const;
  };

  struct Q_PACKED WriteRequest {
    uint8_t
      sequence : 4,
      header : 4;
    uint16_t offset_page;
    int8_t payload[1024];
    uint8_t crc;

    WriteRequest(uint8_t sequence, uint32_t offset, const uint8_t *payload, int size);

    bool send(QIODevice *device, const ErrorStack &err=ErrorStack()) const;
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

  /** Maps an address to segment and offset. */
  static QPair<int, uint32_t> mapToSegment(uint32_t address);

protected:
  /** Current state. */
  State _state;
  /** Radio info, identifying the radio. */
  RadioInfo _info;
  /** Firmware version string. */
  QByteArray _firmwareVersion;

  static const QVector<QPair<uint32_t, uint32_t>> _segmentMap;
};

#endif //LIBDMRCONF_RT4D_INTERFACE_HH
