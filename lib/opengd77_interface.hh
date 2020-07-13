#ifndef OPENGD77INTERFACE_HH
#define OPENGD77INTERFACE_HH

#include "usbserial.hh"


class OpenGD77Interface : public USBSerial
{
  Q_OBJECT

public:
  static const uint32_t EEPROM = 0;
  static const uint32_t FLASH  = 1;

public:
  explicit OpenGD77Interface(QObject *parent=nullptr);
  virtual ~OpenGD77Interface();

  void close();

  QString identifier();

  bool read_start(uint32_t bank, uint32_t addr);
  bool read(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes);
  bool read_finish();

  bool write_start(uint32_t bank, uint32_t addr);
  bool write(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes);
  bool write_finish();

protected:
  typedef struct __attribute__((packed)) {
    typedef enum {
      READ_FLASH = 1,
      READ_EEPROM = 2,
      READ_MCU_ROM = 5,
      READ_DISPLAY_BUFFER = 6,
      READ__WAV_BUFFER = 7,
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

    bool initReadFlash(uint32_t address, uint16_t length);
    bool initReadEEPROM(uint32_t address, uint16_t length);
  } ReadRequest;

  typedef struct __attribute__((packed)) {
    /// Same code as request. That is 'R' read block, 'W' write block, 'C' command.
    char type;
    /// Length of paylod.
    uint16_t length;
    /// Payload.
    uint8_t data[32];
  } ReadResponse;

  typedef struct __attribute__((packed)) {
    typedef enum {
      SET_FLASH_SECTOR = 1,
      WRITE_SECTOR_BUFFER = 2,
      WRITE_FLASH_SECTOR = 3,
      WRITE_EEPROM = 4,
      WRITE_WAV_BUFFER = 7
    } Command;

    /// 'R' read block, 'W' write block, 'C' command.
    char type;
    /// Command, @see OpenGD77Internface::WriteReqest::Command.
    uint8_t command;

    union {
      uint8_t sector[3];
      struct __attribute__((packed)) {
        uint32_t address;
        uint16_t length;
        uint8_t data[32];
      } payload;
    };

    bool initWriteEEPROM(uint32_t addr, const uint8_t *data, uint16_t size);
    bool initSetFlashSector(uint32_t addr);
    bool initWriteFlash(uint32_t addr, const uint8_t *data, uint16_t size);
    bool initFinishWriteFlash();
  } WriteRequest;

  typedef struct __attribute__((packed)) {
    /// Same code as request. That is 'R' read block, 'W' write block, 'C' command or '-' on Error.
    char type;
    /// Same code as request if OK.
    uint8_t command;
  } WriteResponse;

  typedef struct __attribute__((packed)) {
    typedef enum {
      SHOW_CPS_SCREEN  = 0,
      CLEAR_SCREEN     = 1,
      DISPLAY          = 2,
      RENDER_CPS       = 3,
      CLOSE_CPS_SCREEN = 5,
      COMMAND          = 6
    } Command;

    typedef enum {
      SAVE_SETTINGS_NOT_VFOS = 0,
      REBOOT = 1,
      SAVE_SETTINGS_AND_VFOS = 2,
      FLASH_GREEN_LED = 3,
      FLASH_RED_LED = 4
    } Option;

    char type;
    uint8_t command;
    union {
      uint8_t x;
      uint8_t option;
    };
    uint8_t y;
    uint8_t size;
    uint8_t alignment;
    uint8_t inverted;
    char message[16];

    void initShowCPSScreen();
    void initClearScreen();
    void initDisplay(uint8_t x, uint8_t y, const char *message, uint8_t size, uint8_t alignment, uint8_t inverted);
    void initRenderCPS();
    void initCloseScreen();
    void initCommand(Option option);
  } CommandRequest;

protected:
  bool readEEPROM(uint32_t addr, uint8_t *data, uint16_t len);
  bool writeEEPROM(uint32_t addr, const uint8_t *data, uint16_t len);
  bool readFlash(uint32_t addr, uint8_t *data, uint16_t len);
  bool setFlashSector(uint32_t addr);
  bool writeFlash(uint32_t addr, const uint8_t *data, uint16_t len);
  bool finishWriteFlash();

protected:
  int32_t _sector;
};

#endif // OPENGD77INTERFACE_HH
