#ifndef OPENGD77INTERFACE_HH
#define OPENGD77INTERFACE_HH

#include "usbserial.hh"
#include "errorstack.hh"

/** Implements the interface to a radio running the Open GD77 firmware.
 *
 * This interface uses a USB serial-port to communicate with the device. To find the corresponding
 * port, the device-specific VID @c 0x1fc9 and PID @c 0x0094 are used. Hence no udev rules are
 * needed to access these devices. The user, however, should be a member of the @c dialout group
 * to get access to the serial interfaces.
 *
 *
 * @section ogd77cmd Command requests
 * The overall command requests structure is
 *
 * @verbinclude opengd77_protocol_command_request.txt
 *
 * where the optional and variable length payload field is determined by the command flag. The
 * request starts with the command prefix 'C' (43h) followed by the command flag. Following
 * command flags are known.
 *
 * <table>
 *  <tr><th>Flag</th><th>Command</th></tr>
 *  <tr><td>00h</td> <td>Show CPS screen.</td></tr>
 *  <tr><td>01h</td> <td>Clear screen.</td></tr>
 *  <tr><td>02h</td> <td>Display text.</td></tr>
 *  <tr><td>03h</td> <td>Render screen.</td></tr>
 *  <tr><td>05h</td> <td>Close CPS screen.</td></tr>
 *  <tr><td>06h</td> <td>Control radio.</td></tr>
 *  <tr><td>07h</td> <td>Start GPS logging.</td></tr>
 *  <tr><td>feh</td> <td>Ping request.</td></tr>
 * </table>
 *
 * @subsection ogd77cmd_cps_screen Show CPS Screen (00h)
 * Reserves the screen for the CPS. The content is not cleared.
 *
 * The command is quiet simple
 * @verbinclude opengd77_protocol_command_show_cps_screen_request.txt
 * as is the response
 * @verbinclude opengd77_protocol_command_okay_response.txt
 *
 * @subsection ogd77cmd_clear_screen Clear Screen (01h)
 * Once the screen has been reserved, this command clears it.
 *
 * Also this command is quiet simple
 * @verbinclude opengd77_protocol_command_clear_screen_request.txt
 * as is the response
 * @verbinclude opengd77_protocol_command_okay_response.txt
 *
 * @subsection ogd77cmd_set_text Set text (02h)
 * This command has a variable payload size.
 * @verbinclude opengd77_protocol_command_display_text_request.txt
 * <table>
 *  <tr><th>Field</th><th>Meaning</th></tr>
 *  <tr><td>Column Address</td> <td>Specifies the column index.</td></tr>
 *  <tr><td>Row Address</td> <td>Specifies the row index as multiple of 10h.</td></tr>
 *  <tr><td>Size</td> <td>Text size (?). Actually, always observed 3.</td></tr>
 *  <tr><td>Alignment</td> <td>Specifies text alignment on row. 0=left, 1=center, 2=right(?)</td></tr>
 *  <tr><td>Inverted</td> <td>Inverts text, 0=off, 1=inverted (?).</td></tr>
 *  <tr><td>Payload</td> <td>Variable size, up to 16bytes ASCII text.</td></tr>
 * </table>
 *
 * If there are no errors, the radio responds with
 * @verbinclude opengd77_protocol_command_okay_response.txt
 *
 * @subsection ogd77cmd_render_screen Render Screen (03h)
 * Randers the transmitted screen. This command is quiet simple again, with no payload.
 * @verbinclude opengd77_protocol_command_render_screen_request.txt
 *
 * as is the response:
 * @verbinclude opengd77_protocol_command_okay_response.txt
 *
 * @subsection ogd77cmd_ctrl Control Radio (06h)
 * This command request is used to control the radio. The specific action is transmitted as a
 * single payload byte.
 * @verbinclude opengd77_protocol_command_control_request.txt
 * <table>
 *  <tr><th>Action Code</th><th>Meaning</th></tr>
 *  <tr><td>00h</td> <td>Save settings and rebbot.</td></tr>
 *  <tr><td>01h</td> <td>Reboot</td></tr>
 *  <tr><td>02h</td> <td>Save settings and VFOs, no reboot.</td></tr>
 *  <tr><td>03h</td> <td>Flash LED green.</td></tr>
 *  <tr><td>04h</td> <td>Flash LED red.</td></tr>
 *  <tr><td>05h</td> <td>Re-init internal buffers.</td></tr>
 *  <tr><td>06h</td> <td>Re-init sound buffers.</td></tr>
 *  <tr><td>07h</td> <td>Update date-time from GPS.</td></tr>
 * </table>
 *
 * If there are no errors, the radio responds with
 * @verbinclude opengd77_protocol_command_okay_response.txt
 *
 * @subsection ogd77cmd_gps Start GPS Logging (07h)
 * A simple request without any payload.
 * @verbinclude opengd77_protocol_command_start_gps_request.txt
 *
 * If there is no error, the response should be.
 * @verbinclude opengd77_protocol_command_okay_response.txt
 * After that, the radio will send GPS information via the serial port until any other command
 * is send to the radio.
 *
 * @subsection ogd77cmd_ping Ping Request (06h)
 * A simple request without any payload
 * @verbinclude opengd77_protocol_command_ping_request.txt
 * and the radio should respond with
 * @verbinclude opengd77_protocol_command_okay_response.txt
 *
 *
 * @section ogd77read Read requests
 * The read command is used to obtain different stuff. Not only the code plug. In general, all read
 * share the same form
 * @verbinclude opengd77_protocol_read_request.txt
 *
 * <table>
 *  <tr><th>Code</th> <th>Memory region</th></tr>
 *  <tr><td>01h</td>  <td>Flash</td></tr>
 *  <tr><td>02h</td>  <td>EEPROM</td></tr>
 *  <tr><td>05h</td>  <td>MCU ROM</td></tr>
 *  <tr><td>06h</td>  <td>Display buffer</td></tr>
 *  <tr><td>07h</td>  <td>WAV buffer</td></tr>
 *  <tr><td>08h</td>  <td>AMBE buffer</td></tr>
 *  <tr><td>09h</td>  <td>Radio info</td></tr>
 *  <tr><td>0ah</td>  <td>FLASH security registers</td></tr>
 * </table>
 *
 * Whenever the read request returns some data, it is transmitted with the read response
 * @verbinclude opengd77_protocol_read_response.txt
 *
 * If not, a simple ACK response is send
 * @verbinclude opengd77_protocol_command_okay_response.txt
 *
 * @subsection ogd77info Radio info struct
 * When reading the radio information, the information is returned in a binary struct:
 * @verbinclude opengd77_radio_info.txt
 * <table>
 *  <tr><th>Code</th> <th>Radio Variant</th></tr>
 *  <tr><td>00h</td>  <td>Radioddity GD-77</td></tr>
 *  <tr><td>01h</td>  <td>Radioddity GD-77S</td></tr>
 *  <tr><td>02h</td>  <td>Baofeng DM-1801</td></tr>
 *  <tr><td>03h</td>  <td>Radioddity RD-5R</td></tr>
 *  <tr><td>04h</td>  <td>Baofeng DM-1801A</td></tr>
 *  <tr><td>05h</td>  <td>TyT MD-9600</td></tr>
 *  <tr><td>06h</td>  <td>TyT MD-UV390</td></tr>
 *  <tr><td>07h</td>  <td>TyT MD-380</td></tr>
 *  <tr><td>08h</td>  <td>Baofeng DM-1701</td></tr>
 *  <tr><td>09h</td>  <td>TyT MD-2017</td></tr>
 *  <tr><td>0ah</td>  <td>Baofeng DM-1701 RGB</td></tr>
 * </table>
 *
 * @section ogd77write Write requests
 *
 * @ingroup ogd77 */
class OpenGD77Interface : public USBSerial
{
  Q_OBJECT

public:
  /** The EEPROM memory bank. */
  static const uint32_t EEPROM = 0;
  /** The Flash memory bank. */
  static const uint32_t FLASH  = 1;

  /** Specifies the detected model variant. */
  enum class Variant {
    GD77, UV380
  };

public:
  /** Constructs a new interface to a specific OpenGD77 device.  */
  explicit OpenGD77Interface(const USBDeviceDescriptor &descr,
                             const ErrorStack &err=ErrorStack(), QObject *parent=nullptr);
  /** Destructor. */
  virtual ~OpenGD77Interface();

  /** Closes the interface to the device. */
  void close();

  /** Returns an identifier of the radio. */
  RadioInfo identifier(const ErrorStack &err=ErrorStack());

  /** Returns @c true if the device allows for storing part of the call-sign DB inside the voice
   * prompt memory. This property is valid after identifying the device, i.e., a call to
   * @c identifier. */
  bool extendedCallsignDB() const;

  bool read_start(uint32_t bank, uint32_t addr, const ErrorStack &err=ErrorStack());
  bool read(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes, const ErrorStack &err=ErrorStack());
  bool read_finish(const ErrorStack &err=ErrorStack());

  bool write_start(uint32_t bank, uint32_t addr, const ErrorStack &err=ErrorStack());
  bool write(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes, const ErrorStack &err=ErrorStack());
  bool write_finish(const ErrorStack &err=ErrorStack());

  bool setDateTime(const QDateTime &datetime, const ErrorStack &err);

  bool saveSettingsNotVFOs(const ErrorStack &err=ErrorStack());
  bool saveSettingsAndVFOs(const ErrorStack &err=ErrorStack());
  bool reboot(const ErrorStack &err=ErrorStack());

public:
  /** Returns some information about this interface. */
  static USBDeviceInfo interfaceInfo();
  /** Tries to find all interfaces connected AnyTone radios. */
  static QList<USBDeviceDescriptor> detect(bool saveOnly=true);

protected:
  /** Represents a read message. */
  struct __attribute__((packed)) ReadRequest {
    /** Possible read sources. */
    enum Command {
      READ_FLASH = 1,
      READ_EEPROM = 2,
      READ_MCU_ROM = 5,
      READ_DISPLAY_BUFFER = 6,
      READ_WAV_BUFFER = 7,
      READ_AMBE_BUFFER = 8,
      READ_FIRMWARE_INFO = 9
    };

    /// 'R' read block, 'W' write block, 'C' command.
    char type;
    /// Memory to read from, FLASH, EEPROM, ROM, etc. @see OpenGD77Internface::ReadReqest::Command.
    uint8_t command;
    /// Memory address to read from in big endian.
    uint32_t address;
    /// Amount of data to read in big endian.
    uint16_t length;

    /** Constructs a FLASH read message. */
    bool initReadFlash(uint32_t address, uint16_t length);
    /** Constructs an EEPROM read message. */
    bool initReadEEPROM(uint32_t address, uint16_t length);
    /** Constructs a firmware-info read message. */
    bool initReadFirmwareInfo();
  };

  /** Radio information struct. */
  struct __attribute__((packed)) FirmwareInfo {
    /** Possible radio types, returned by the radio_info struct.*/
    enum class RadioType {
      GD77=0, GD77S=1, DM1801=2, RD5R=3, DM1801A=4, MD9600=5,
      MDUV380=6, MD380=7, DM1701=8, MD2017=9, DM1701RGB=10
    };

    uint32_t structVersion;   ///< Struct version number (currently 3).
    uint32_t radioType;       ///< Device variant (see @c RadioType).
    char fw_revision[16];     ///< Firmware revision ASCII, 0-padded.
    char build_date[16];      ///< Firmware build time, YYYYMMDDhhmmss, 0-padded.
    uint32_t flashChipSerial; ///< Serial number of the flash chip.
    uint16_t features;        ///< Some flags, signaling the presence of some features.

    /** Returns @c true if the devices display is inverted. */
    bool featureInvertedDisplay() const;
    /** Returns @c true if the vocie prompt memory is used for storing callsign db. */
    bool featureExtendedCallsignDB() const;
    /** Returns @c true if the voice prompt data is loaded. */
    bool featureVoicePromptLoaded() const;
  };

  /** Represents a read response message. */
  struct __attribute__((packed)) ReadResponse {
    /// Same code as request. That is 'R' read block, 'W' write block, 'C' command.
    char type;
    /// Length of paylod.
    uint16_t length;
    /// Payload
    union {
      uint8_t data[32];    ///< Data payload.
      FirmwareInfo info;   ///< Firmware information struct.
    };
  };

  /** Represents a write message. */
  struct __attribute__((packed)) WriteRequest {
    /** Possible write destinations. */
    enum Command {
      SET_FLASH_SECTOR = 1,
      WRITE_SECTOR_BUFFER = 2,
      WRITE_FLASH_SECTOR = 3,
      WRITE_EEPROM = 4,
      WRITE_WAV_BUFFER = 7
    };

    /// 'R' read block, 'W'/'X' write block or 'C' command.
    char type;
    /// Command, @see OpenGD77Internface::WriteReqest::Command.
    uint8_t command;

    union {
      /** 24 bit sector number. */
      uint8_t sector[3];
      /** Payload data. */
      struct __attribute__((packed)) {
        /** Target address. */
        uint32_t address;
        /** Payload length. */
        uint16_t length;
        /** Payload data. */
        uint8_t data[32];
      } payload;
    };

    /** Constructs a write-to-eeprom message. */
    bool initWriteEEPROM(Variant variant, uint32_t addr, const uint8_t *data, uint16_t size);
    /** Constructs a set-flash-sector message. */
    bool initSetFlashSector(Variant variant, uint32_t addr);
    /** Constructs a write-to-flash message. */
    bool initWriteFlash(Variant variant, uint32_t addr, const uint8_t *data, uint16_t size);
    /** Constructs a finish-write-to-flash message. */
    bool initFinishWriteFlash(Variant variant);
  };

  /** Represents a write-response message. */
  struct __attribute__((packed)) WriteResponse {
    /// Same code as request. That is 'R' read block, 'W' write block, 'C' command or '-' on Error.
    char type;
    /// Same code as request if OK.
    uint8_t command;
  };

  /** Represents a command message. */
  struct __attribute__((packed)) CommandRequest {
    /** Possible commands. */
    enum Command {
      SHOW_CPS_SCREEN  = 0,
      CLEAR_SCREEN     = 1,
      DISPLAY          = 2,
      RENDER_CPS       = 3,
      CLOSE_CPS_SCREEN = 5,
      COMMAND          = 6
    };

    /** Possible options. */
    enum Option {
      SAVE_SETTINGS_NOT_VFOS = 0,
      REBOOT = 1,
      SAVE_SETTINGS_AND_VFOS = 2,
      FLASH_GREEN_LED = 3,
      FLASH_RED_LED = 4,
      INIT_CODEC = 5,
      INIT_SOUND = 6,
      SET_DATETIME = 7,
      DELAY_10ms = 10
    };

    /** Message type, here 'C' for command. */
    char type;
    /** The command. */
    uint8_t command;
    /** Either a command option or the x position on screen. */
    union {
      /** The x-position on the screen. */
      uint8_t x;
      /** The command option. */
      uint8_t option;
    };
    // Either some text options or a timestamp.
    union {
      struct __attribute__((packed)) {
        /** The y-position on the screen. */
        uint8_t y;
        /** The font size. */
        uint8_t font;
        /** The text alignment. */
        uint8_t alignment;
        /** Is text inverted? */
        uint8_t inverted;
      };
      uint32_t timestamp;
    };

    /** Some text message. */
    char message[16];

    /** Construct "show CPS screen" command message. */
    void initShowCPSScreen();
    /** Construct a clear-screen command message. */
    void initClearScreen();
    /** Construct a "show text on screen" message. */
    void initDisplay(uint8_t x, uint8_t y, const char *message, unsigned int iSize,
                     uint8_t font, uint8_t alignment, uint8_t inverted);
    /** Construct a "render CPS" message. */
    void initRenderCPS();
    /** Construct a "close screen" command message. */
    void initCloseScreen();
    /** Construct a command message with the given option. */
    void initCommand(Option option);
    /** Construct a SET_DATETIME message with the given date time. */
    void initSetDateTime(const QDateTime &dt);
  };

protected:
  /** Read some data from EEPROM at the given address. */
  bool readEEPROM(uint32_t addr, uint8_t *data, uint16_t len, const ErrorStack &err=ErrorStack());
  /** Write some data to EEPROM at the given address. */
  bool writeEEPROM(uint32_t addr, const uint8_t *data, uint16_t len, const ErrorStack &err=ErrorStack());

  /** Read some data from Flash at the given address. */
  bool readFlash(uint32_t addr, uint8_t *data, uint16_t len, const ErrorStack &err=ErrorStack());
  /** Select the correct Flash sector for the given address.
   * This command must be sent before writing to the flash memory. */
  bool setFlashSector(uint32_t addr, const ErrorStack &err=ErrorStack());
  /** Write some data to the given Flash memory. */
  bool writeFlash(uint32_t addr, const uint8_t *data, uint16_t len, const ErrorStack &err=ErrorStack());
  /** Finalize writing to the Flash memory. If not send after writing to a sector,
   * the changes are lost. */
  bool finishWriteFlash(const ErrorStack &err=ErrorStack());

  /** Read radio info struct. */
  bool readFirmwareInfo(FirmwareInfo &radioInfo, const ErrorStack &err=ErrorStack());

  /** Send a "show CPS screen" message. */
  bool sendShowCPSScreen(const ErrorStack &err=ErrorStack());
  /** Send a "clear screen" message. */
  bool sendClearScreen(const ErrorStack &err=ErrorStack());
  /** Send a "display some text" message. */
  bool sendDisplay(uint8_t x, uint8_t y, const char *message, uint8_t iSize, uint8_t alignment, uint8_t inverted, const ErrorStack &err=ErrorStack());
  /** Send a "render CPS screen" message. */
  bool sendRenderCPS(const ErrorStack &err=ErrorStack());
  /** Send a "close screen" message. */
  bool sendCloseScreen(const ErrorStack &err=ErrorStack());
  /** Send a "set date time" message. */
  bool sendSetDateTime(const QDateTime &dt, const ErrorStack &err=ErrorStack());
  /** Sends some command message with the given options. */
  bool sendCommand(CommandRequest::Option option, const ErrorStack &err=ErrorStack());

protected:
  /** The protocol variant determined by the device type obtained by the firmware info. */
  Variant _protocolVariant;
  /** If @c true, the device allows for storing parts of the call-sign DB inside the voice prompt
   *  memory. */
  bool _extendedCallsignDB;
  /** The current Flash sector, set to -1 if none is currently selected. */
  int32_t _sector;
};

#endif // OPENGD77INTERFACE_HH
