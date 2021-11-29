/** @defgroup rif Radio interfaces
 * Depending on the manufacturer or model, different radios have different computer-radio
 * interfaces. This module collects all classes that provide these interfaces. */
#ifndef RADIOINFERFACE_HH
#define RADIOINFERFACE_HH

#include <QString>
#include "radioinfo.hh"
#include "errorstack.hh"


/** Abstract radio interface.
 * A radion interface must provide means to communicate with the device. That is, open a connection
 * to the device, allow for reading and writing specific memory blocks.
 *
 * This class defines the common interface for all radio-interface classes, irrespective of the
 * actual communication protocol being used by the device.
 *
 * @ingroup rif */
class RadioInterface
{
protected:
  /** Hidden constructor. */
  explicit RadioInterface();

public:
  /** Destructor. */
	virtual ~RadioInterface();

  /** Return @c true if a connection to the device has been established. */
	virtual bool isOpen() const = 0;
  /** Closes the connection to the device. */
	virtual void close() = 0;

  /** Returns a device identifier. */
  virtual RadioInfo identifier(const ErrorStack &err=ErrorStack()) = 0;

  /** Starts the write process into the specified bank and at the given address.
   * @param bank Specifies the memory bank to write to. Usually there is only one bank. Some radios,
   *    however, to have several memory banks to hold the codeplug. For example the Open GD77 has
   *    EEPROM and Flash memory banks with independent addresses.
   * @param addr Specifies the address to write to.
   * @param err Passes an error stack to put error messages on. */
  virtual bool write_start(uint32_t bank, uint32_t addr, const ErrorStack &err=ErrorStack())=0;

  /** Writes a chunk of @c data at the address @c addr.
   * @param bank Specifies the memory bank to write to. Usually there is only one bank. Some radios,
   *    however, to have several memory banks to hold the codeplug. For example the Open GD77 has
   *    EEPROM and Flash memory banks with independent addresses.
   * @param addr Specifies the address to write to.
   * @param data Pointer to the actual data to be written.
   * @param nbytes Specifies the number of bytes to write.
   * @param err Passes an error stack to put error messages on.
   * @returns @c true on success. */
  virtual bool write(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes, const ErrorStack &err=ErrorStack()) = 0;

  /** This function ends a series of @c write operations.
   * This function will be implemented by certain interfaces that need completion of write
   * operations (e.g., HID).
   * @param err Passes an error stack to put error messages on. */
  virtual bool write_finish(const ErrorStack &err=ErrorStack()) = 0;

  /** Starts the read process from the specified bank and at the given address.
   * @param bank Specifies the memory bank to read from. Usually there is only one bank. Some radios,
   *    however, to have several memory banks to hold the codeplug. For example the Open GD77 has
   *    EEPROM and Flash memory banks with independent addresses.
   * @param addr Specifies the address to read from.
   * @param err Passes an error stack to put error messages on. */
  virtual bool read_start(uint32_t bank, uint32_t addr, const ErrorStack &err=ErrorStack()) = 0;

  /** Reads a chunk of data from the block-address @c bno (block number).
   * @param bank Specifies the memory bank to read from. Usually there is only one bank. Some radios,
   *    however, to have several memory banks to hold the codeplug. For example the Open GD77 has
   *    EEPROM and Flash memory banks with independent addresses.
   * @param addr Specifies the address to read from.
   * @param data Pointer where to store the read data.
   * @param nbytes Specifies the number of bytes to read.
   * @param err Passes an error stack to put error messages on.
   * @returns @c true on success. */
  virtual bool read(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes, const ErrorStack &err=ErrorStack()) = 0;

  /** This function ends a series of @c read operations.
   * This function will be re-implemented by certain interfaces that need completion of read
   * operations (e.g., HID).
   * @param err Passes an error stack to put error messages on. */
  virtual bool read_finish(const ErrorStack &err=ErrorStack()) = 0;

  /** Some radios need to be rebooted after being read or programmed. This function
   * will be re-implemented by some interfaces (e.g., DFUDevice) to reboot the radio. By default
   * this function does nothing.
   * @param err Passes an error stack to put error messages on. */
  virtual bool reboot(const ErrorStack &err=ErrorStack());
};

#endif // RADIOINFERFACE_HH
