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
  };

  struct Q_PACKED ACKResponse {
    uint8_t header;

    bool receive(QIODevice *device, const ErrorStack &err=ErrorStack());
  };

  struct Q_PACKED ReadRequest {
    uint8_t header;
    uint16_t address;
    uint8_t crc;

    ReadRequest(uint16_t address);
  };

  struct Q_PACKED ReadResponse {
    uint8_t header;
    uint16_t address;
    uint8_t payload[1021];
    uint8_t crc;

    bool receive(QIODevice *device, const ErrorStack &err=ErrorStack());
  };

  struct Q_PACKED WriteRequest {
    uint8_t
      sequence : 4,
      header : 4;
    uint16_t address;
    int8_t payload[1024];
    uint8_t crc;

    WriteRequest(uint8_t sequence, uint16_t address, const uint8_t *payload, int size);
  };

public:
  /** Constructs a new RT4D interface for the given USB descriptor. The constructor also enters
   * the SYSINFO state and identifies the radio. */
  explicit RT4DInterface(const USBDeviceDescriptor &descr,
                         const ErrorStack &err=ErrorStack(), QObject *parent=nullptr);

};

#endif //LIBDMRCONF_RT4D_INTERFACE_HH
