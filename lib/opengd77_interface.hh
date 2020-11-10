#ifndef OPENGD77INTERFACE_HH
#define OPENGD77INTERFACE_HH

#include "usbserial.hh"

/** Implements the interfact to a radio running the Open GD77 firmware.
 *
 * This interface uses a USB serial-port to comunicate with the device. To find the corresponding
 * port, the device-specific VID @c 0x1fc9 and PID @c 0x0094 are used. Hence no udev rules are
 * needed to access these devices. The user, however, should be a member of the @c dialout group
 * to get access to the serial interfaces.
 *
 * @ingroup ogd77
 */
class OpenGD77Interface : public USBSerial
{
  Q_OBJECT

public:
  /** The EEPROM memory bank. */
  static const uint32_t EEPROM = 0;
  /** The Flash memory bank. */
  static const uint32_t FLASH  = 1;

public:
  /** Constructs a new interface to a OpenGD77 device. If a matching device was found, @c isOpen
   * returns @c true. */
  explicit OpenGD77Interface(QObject *parent=nullptr);
  /** Destructor. */
  virtual ~OpenGD77Interface();

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
  /** Represents a read message. */
  typedef struct __attribute__((packed)) {
    /** Possible read sources. */
    typedef enum {
      READ_FLASH = 1,
      READ_EEPROM = 2,
      READ_MCU_ROM = 5,
      READ_DISPLAY_BUFFER = 6,
      READ_WAV_BUFFER = 7,
      READ_AMBE_BUFFER = 8
    } Command;

    /// 'R' read block, 'W' write block, 'C' command.
    char type;
    /// Memory to read from, FLASH, EEPROM, ROM, etc. @see OpenGD77Internface::ReadReqest::Command.
    uint8_t command;
    /// Memory address to read from in big endian.
    uint32_t address;
    /// Amount of data to read, max 32 bytes in big endian.
    uint16_t length;

    /** Constructs a FLASH read message. */
    bool initReadFlash(uint32_t address, uint16_t length);
    /** Constructs a EEPROM read message. */
    bool initReadEEPROM(uint32_t address, uint16_t length);
  } ReadRequest;

  /** Represents a read response message. */
  typedef struct __attribute__((packed)) {
    /// Same code as request. That is 'R' read block, 'W' write block, 'C' command.
    char type;
    /// Length of paylod.
    uint16_t length;
    /// Payload.
    uint8_t data[32];
  } ReadResponse;

  /** Represents a write message. */
  typedef struct __attribute__((packed)) {
    /** Possible write destinations. */
    typedef enum {
      SET_FLASH_SECTOR = 1,
      WRITE_SECTOR_BUFFER = 2,
      WRITE_FLASH_SECTOR = 3,
      WRITE_EEPROM = 4,
      WRITE_WAV_BUFFER = 7
    } Command;

    /// 'R' read block, 'W' write block or 'C' command.
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
    bool initWriteEEPROM(uint32_t addr, const uint8_t *data, uint16_t size);
    /** Constructs a set-flash-sector message. */
    bool initSetFlashSector(uint32_t addr);
    /** Constructs a write-to-flash message. */
    bool initWriteFlash(uint32_t addr, const uint8_t *data, uint16_t size);
    /** Constructs a finish-write-to-flash message. */
    bool initFinishWriteFlash();
  } WriteRequest;

  /** Represents a write-response message. */
  typedef struct __attribute__((packed)) {
    /// Same code as request. That is 'R' read block, 'W' write block, 'C' command or '-' on Error.
    char type;
    /// Same code as request if OK.
    uint8_t command;
  } WriteResponse;

  /** Represents a command message. */
  typedef struct __attribute__((packed)) {
    /** Possible commands. */
    typedef enum {
      SHOW_CPS_SCREEN  = 0,
      CLEAR_SCREEN     = 1,
      DISPLAY          = 2,
      RENDER_CPS       = 3,
      CLOSE_CPS_SCREEN = 5,
      COMMAND          = 6
    } Command;

    /** Possible options. */
    typedef enum {
      SAVE_SETTINGS_NOT_VFOS = 0,
      REBOOT = 1,
      SAVE_SETTINGS_AND_VFOS = 2,
      FLASH_GREEN_LED = 3,
      FLASH_RED_LED = 4
    } Option;

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
    /** The y-position on the screen. */
    uint8_t y;
    /** The size. */
    uint8_t size;
    /** The text alignment. */
    uint8_t alignment;
    /** Is text inverted? */
    uint8_t inverted;
    /** Some text message. */
    char message[16];

    /** Construct "show CPS screen" command message. */
    void initShowCPSScreen();
    /** Construct a clear-screen command message. */
    void initClearScreen();
    /** Construct a "show text on screen" message. */
    void initDisplay(uint8_t x, uint8_t y, const char *message, uint8_t iSize, uint8_t alignment, uint8_t inverted);
    /** Construct a "render CPS" message. */
    void initRenderCPS();
    /** Construct a "close screen" command message. */
    void initCloseScreen();
    /** Construct a command message with the given option. */
    void initCommand(Option option);
  } CommandRequest;

protected:
  /** Write some data to EEPROM at the given address. */
  bool readEEPROM(uint32_t addr, uint8_t *data, uint16_t len);
  /** Read some data from EEPROM at the given address. */
  bool writeEEPROM(uint32_t addr, const uint8_t *data, uint16_t len);
  /** Read some data from Flash at the given address. */
  bool readFlash(uint32_t addr, uint8_t *data, uint16_t len);
  /** Select the correct Flash sector for the given address.
   * This command must be send before writing to the flash memory. */
  bool setFlashSector(uint32_t addr);
  /** Write some data to the given Flash memory. */
  bool writeFlash(uint32_t addr, const uint8_t *data, uint16_t len);
  /** Finalize writing to the Flash memory. If not send after writing to a sector,
   * the changes are lost. */
  bool finishWriteFlash();

  /** Send a "show CPS screen" message. */
  bool sendShowCPSScreen();
  /** Send a "clear screen" message. */
  bool sendClearScreen();
  /** Send a "display some text" message. */
  bool sendDisplay(uint8_t x, uint8_t y, const char *message, uint8_t iSize, uint8_t alignment, uint8_t inverted);
  /** Send a "render CPS screen" message. */
  bool sendRenderCPS();
  /** Send a "close screen" message. */
  bool sendCloseScreen();
  /** Sends some command message with the given options. */
  bool sendCommand(CommandRequest::Option option);

protected:
  /** The current Flash sector, set to -1 if none is currently selected. */
  int32_t _sector;
};

#endif // OPENGD77INTERFACE_HH
