#ifndef DM32UVINTERFACE_HH
#define DM32UVINTERFACE_HH

#include "usbserial.hh"
#include <QObject>
#include "dm32uv.hh"


/** Interface to Baofeng Dm-32UV devices.
 *
 * The protocol is documented in detail at
 * https://github.com/infamy/DM32-Protocol-Spec/blob/main/02-CONNECTION-SEQUENCE.md
 *
 * @ingroup dm32uv */
class DM32UVInterface : public USBSerial
{
  Q_OBJECT

protected:
  /** Device detection request. */
  struct Q_PACKED DeviceDetectionRequest {
    /** Request content. */
    const char payload[7] = {'P','S','E','A','R','C','H'};

    /** Send request though the given interface. */
    bool send(DM32UVInterface *dev, const ErrorStack &err=ErrorStack()) const;
  };

  /** Device detection response. */
  struct Q_PACKED DeviceDetectionResponse {
    /** Result flag. */
    uint8_t result;
    /** Response payload. */
    char payload[7];

    /** Receive response though the given interface. */
    bool receive(DM32UVInterface *dev, const ErrorStack &err=ErrorStack());
    /** Extracts the identifier from the response payload. */
    QString identifier() const;
  };


  /** Password request. */
  struct Q_PACKED PasswordRequest {
    /** Request content. */
    const char payload[7] = {'P', 'A', 'S', 'S', 'S', 'T', 'A'};

    /** Send request though the given interface. */
    bool send(DM32UVInterface *dev, const ErrorStack &err=ErrorStack()) const;
  };

  /** Passwort response. */
  struct Q_PACKED PasswordResponse {
    /** Response result code. */
    uint8_t result;
    /** Some unknown data. */
    uint16_t code;

    /** Receive response though the given interface. */
    bool receive(DM32UVInterface *dev, const ErrorStack &err=ErrorStack());
  };


  /** System info request. */
  struct Q_PACKED SysinfoRequest {
    /** Request content. */
    const char payload[7] = {'S','Y','S','I','N','F','O'};

    /** Send request though the given interface. */
    bool send(DM32UVInterface *dev, const ErrorStack &err=ErrorStack()) const;
  };

  /** System info response. */
  struct Q_PACKED ACKResponse {
    /** Response result code. */
    uint8_t result;

    /** Receive response though the given interface. */
    bool receive(DM32UVInterface *dev, const ErrorStack &err=ErrorStack());
  };


  /** Value request. These requests are used to read some global information like address ranges
   * and versions. */
  struct Q_PACKED ValueRequest {
    /** Some possible value IDs. */
    enum class ValueId{
      FirmwareVersion = 0x1, BuildDate = 0x3, MainConfigMemory = 0xa, CallSignDBMemory = 0xf
    };
    /** Static request type. */
    const char request_type = 'V';
    /** Some unknown fields. Likely some flags and length field. */
    uint8_t unused[3];
    /** The value ID to read. */
    uint8_t valueId;

    /** Constructor from value ID. */
    ValueRequest(ValueId valueId);
    /** Constructor from raw value ID. */
    explicit ValueRequest(uint8_t valueId);

    /** Send request though the given interface. */
    bool send(DM32UVInterface *dev, const ErrorStack &err=ErrorStack()) const;
  };

  /** Value response. */
  struct Q_PACKED ValueResponse {
    /** The received response code, should be 'V'. */
    char response_type;
    /** The value ID read. */
    uint8_t valueId;
    /** THe length of the payload. The format depends on the value being read. */
    uint8_t length;
    union {
      /** A memory range. */
      struct {
        /** Lower bound in little endian. */
        uint32_t lower;
        /** Upper bound (inclusive) in little endian. */
        uint32_t upper;
      } memory;
      // Raw payload.
      uint8_t payload[255];
    };

    /** Receive response though the given interface. */
    bool receive(DM32UVInterface *dev, const ErrorStack &err=ErrorStack());
    /** Extracts a string from the response payload. */
    QString string() const;
    /** Extracts the lower memory bound. */
    uint32_t lowerMemoryBound() const;
    /** Extracts the upper memory bound. */
    uint32_t upperMemoryBound() const;
  };


  /** Enter program mode request. */
  struct Q_PACKED EnterProgramModeRequest {
    /** Request content. */
    const char payload[12] = {
      '\xff', '\xff', '\xff', '\xff', '\x0c',
      'P','R','O','G','R','A','M'
    };

    /** Send request though the given interface. */
    bool send(DM32UVInterface *dev, const ErrorStack &err=ErrorStack()) const;
  };


  /** Unkown 02h request */
  struct Q_PACKED Unknown02Request {
    /** Request content. */
    const char payload[1] {'\x02'};

    /** Send request though the given interface. */
    bool send(DM32UVInterface *dev, const ErrorStack &err=ErrorStack()) const;
   };

  /** Unknown 02h response. */
  struct Q_PACKED Unknown02Response {
    /** Some unknown response payload. */
    char payload[8];

    /** Receive response though the given interface. */
    bool receive(DM32UVInterface *dev, const ErrorStack &err=ErrorStack());
  };


  /** Ping request. */
  struct Q_PACKED PingRequest {
    /** Request content. */
    const char payload[1] = {'\x06'};

    /** Send request though the given interface. */
    bool send(DM32UVInterface *dev, const ErrorStack &err=ErrorStack()) const;
  };


  /** Read request. */
  struct Q_PACKED ReadRequest {
    /** Static request type. */
    const char request_type = 'R';
    /** 24bit little endian address. */
    uint8_t address[3];
    /** 16bit little endian length. */
    uint16_t length;

    /** Constructs a read request for the given address and length. */
    ReadRequest(uint32_t address, uint16_t length);
    /** Send request though the given interface. */
    bool send(DM32UVInterface *dev, const ErrorStack &err=ErrorStack()) const;
  };

  /** Read response. */
  struct Q_PACKED ReadResponse {
    /** Static respone type field. */
    const char response_type = 'W';
    /** 24bit little endian address. */
    uint8_t _address[3];
    /** 16bit payload length. */
    uint16_t _length;
    /** Payload of upto 4096 bytes. */
    uint8_t _payload[4096];

    /** Unpacks the address. */
    uint32_t address() const;
    /** Unpacks the length. */
    uint16_t length() const;
    /** Returns the payload as byte array. */
    QByteArray payload() const;

    /** Receive response though the given interface. */
    bool receive(DM32UVInterface *dev, const ErrorStack &err=ErrorStack());
  };


  /** Write request. */
  struct Q_PACKED WriteRequest {
    /** Static request type field. */
    const char request_type = 'W';
    /** Encodes the 24bit address in little endian. */
    uint8_t _address[3];
    /** Encodes the 16bit length in little endian. */
    uint16_t _length;
    /** Holds the payload. */
    uint8_t _payload[4096];

    /** Constructs a new write request to the given address with the given payload. */
    WriteRequest(uint32_t address, const QByteArray &payload);
    /** Send request though the given interface. */
    bool send(DM32UVInterface *dev, const ErrorStack &err=ErrorStack()) const;
  };


protected:
  /** Possible states, the interface might be in. */
  enum class State {
    Closed, Open, Connected, SystemInfo, Program, Error
  };

public:
  /** Constructs a new DM32UV interface for the given USB descriptor. The constructor also enters
   * the SYSINFO state and identifies the radio. */
  explicit DM32UVInterface(const USBDeviceDescriptor &descr,
                           const ErrorStack &err=ErrorStack(), QObject *parent=nullptr);

  /** Returns the radio info, after identifying the radio. */
  RadioInfo identifier(const ErrorStack &err=ErrorStack()) override;
  /** Reads the obfuscation address map from the device. */
  bool getAddressMap(DM32UV::AddressMap &map, const ErrorStack &err=ErrorStack());

public:
  /** Returns some information about this interface. */
  static USBDeviceInfo interfaceInfo();
  /** Tries to find all interfaces connected AnyTone radios. */
  static QList<USBDeviceDescriptor> detect(bool saveOnly=true);

protected:
  /*** Identifies the radio. */
  bool request_identifier(const ErrorStack &err = ErrorStack());
  /** Enters program mode. */
  bool enter_program_mode(const ErrorStack &err = ErrorStack());
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

protected:
  /** Current state, the interface is in. */
  State _state;
  /** Radio info, identifying the radio. */
  RadioInfo _info;
  /** Firmware version string. */
  QString _firmwareVersion;
  /** Codeplug memory range. */
  QPair<uint32_t, uint32_t> _codeplugMemory;
  /** Callsign memory range. */
  QPair<uint32_t, uint32_t> _callsignMemory;
};

#endif // DM32UVINTERFACE_HH
