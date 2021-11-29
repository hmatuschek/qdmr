#ifndef RADIODDITY_INTERFACE_HH
#define RADIODDITY_INTERFACE_HH

#include <QtGlobal>
#include <QObject>
#include "radiointerface.hh"

#ifdef Q_OS_MACOS
#include "hid_macos.hh"
#else
#include "hid_libusb.hh"
#endif

/** Implements a radio interface for radios using the HID USB schema (i.e. Radioddity devices).
 *
 * @ingroup radioddity */
class RadioddityInterface: public HIDevice, public RadioInterface
{
	Q_OBJECT

public:
  /** Possible memory banks to select. */
  enum MemoryBank {
    MEMBANK_NONE           = -1,    ///< No bank selected.
    MEMBANK_CODEPLUG_LOWER =  0,    ///< Lower memory bank (EEPROM).
    MEMBANK_CODEPLUG_UPPER =  1,    ///< Upper memory bank (FLASH).
    MEMBANK_CALLSIGN_LOWER =  3,    ///< Callsign DB memory lower bank (also FLASH).
    MEMBANK_CALLSIGN_UPPER =  4     ///< Callsign DB memory upper bank (also FLASH).
  };

public:
  /** Connects to the radio with given vendor and product ID. */
  explicit RadioddityInterface(int vid, int pid, const ErrorStack &err=ErrorStack(), QObject *parent = nullptr);
  /** Destructor. */
  virtual ~RadioddityInterface();

  /** Returns @c true if the connection was established. */
	bool isOpen() const;

  void close();

  /** Returns radio identifier string. */
  RadioInfo identifier(const ErrorStack &err=ErrorStack());

  bool read_start(uint32_t bank, uint32_t addr, const ErrorStack &err=ErrorStack());

  /** Reads a block of data from the device at the given block number.
   * @param bank The memory bank to read from.
   * @param addr The address to read from within the memory bank.
   * @param data Pointer to memory where the read data is stored.
   * @param nbytes The number of bytes to read.
   * @param err The error stack, messages are put onto.
   * @returns @c true on success. */
  bool read(uint32_t bank, uint32_t addr, unsigned char *data, int nbytes, const ErrorStack &err=ErrorStack());

  bool read_finish(const ErrorStack &err=ErrorStack());

  bool write_start(uint32_t bank, uint32_t addr, const ErrorStack &err=ErrorStack());

  /** Writes a block of data to the device at the given block number.
   * @param bank The memory bank to read from.
   * @param addr The address to read from within the memory bank.
   * @param data Pointer to memory where the read data is stored.
   * @param nbytes The number of bytes to read.
   * @param err The error stack, messages are put onto.
   * @returns @c true on success. */
  bool write(uint32_t bank, uint32_t addr, unsigned char *data, int nbytes, const ErrorStack &err=ErrorStack());

  bool write_finish(const ErrorStack &err=ErrorStack());

protected:
  /** Internal used function to select a memory bank. */
  bool selectMemoryBank(MemoryBank bank, const ErrorStack &err=ErrorStack());

private:
  /** The currently selected memory bank. */
  MemoryBank _current_bank;
  /** Identifier received when entering the prog mode. */
  RadioInfo _identifier;
};

#endif // RADIODDITY_INTERFACE_HH
