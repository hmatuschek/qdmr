#ifndef TYTINTERFACE_HH
#define TYTINTERFACE_HH

#include "dfu_libusb.hh"
#include "radiointerface.hh"

/** Interface to TyT (and Retevis) devices.
 *
 * This class extends the @c DFUDevice and implements the @c RadioInterface to identify the radio,
 * reading/writing and erasing codeplugs or the callsign DB.
 *
 * @section tytif TyT protocol
 * The communication with the device is kind of weird. It all happens through memory reads and
 * writes. This is not unusual as such, but they implemented a weird memory mapping. Everyhing
 * written to block 0 (i.e., memory address 0x00000000) is a command to the radio. Responses to
 * these commands are read back from the same address/block.
 *
 * The funny thing is, however, that there is a command that sets the memory read and write address,
 * although the DFU interface actually provides means to specify that address. Moreover, commands
 * and data are send and written to the same DFU interface number. Someone did not understood the
 * DFU interface.
 *
 * @subsection Commands
 * The following commands has been seen.
 *
 * @subsubsection tytifepm Enter program mode
 * The request written is
 * @code
 * +------+------+
 * | 0x91 | 0x01 |
 * +------+------+
 * @endcode
 * There is no explict response read back.
 *
 * @subsubsection tytiferb Reboot
 * The request written is
 * @code
 * +------+------+
 * | 0x91 | 0x05 |
 * +------+------+
 * @endcode
 * There is no explict response read back.
 *
 * @subsubsection tyifid Identify/Get Info
 * The request written is
 * @code
 * +------+------+
 * | 0xa2 | What |
 * +------+------+
 * @endcode
 * There is always some response read back. The size varies whith whatever is read back.
 * <table>
 *  <tr><th>WhatCode</th> <th>Resp. Len.</th> <th>Description></th></tr>
 *  <tr><td>0x01</td>     <td>32</td>         <td>The radio identifier as a string + some unknown
 *          information.</td></tr>
 *  <tr><td>0x02</td>     <td>4</td>          <td>Unknown</td></tr>
 *  <tr><td>0x03</td>     <td>24</td>         <td>Unknown</td></tr>
 *  <tr><td>0x04</td>     <td>8</td>          <td>Unknown</td></tr>
 *  <tr><td>0x07</td>     <td>16</td>         <td>Unknown</td></tr>
 * </table>
 *
 * @subsubsection tytidsa Set address
 * The request written is
 * @code
 * +------+------+------+------+------+
 * | 0x21 | Address little endian     |
 * +------+------+------+------+------+
 * @endcode
 * There is no response read back. Before reading or writing memory, the address must be set using
 * this command. The start block for reading/writing this memory, is 2.
 *
 * @subsubsection tytife Erase
 * The request written is
 * @code
 * +------+------+------+------+------+
 * | 0x41 | Address little endian     |
 * +------+------+------+------+------+
 * @endcode
 * There is no response read. The erased page size appears to be 0x010000 bytes.
 *
 * @subsection tytifrw Read and write memory
 * Before writing memory, it must be erased.
 *
 * Before reading and writing memory, the address must be set.
 *
 * After this, the memory can be read/written by simply writing to consecutive blocks starting
 * with block 2.
 *
 * @ingroup tyt */
class TyTInterface : public DFUDevice, public RadioInterface
{
  Q_OBJECT

public:
  /** Costructor. Opens an interface to the specified vendor and product ID. */
  TyTInterface(unsigned vid, unsigned pid, const ErrorStack &err=ErrorStack(), QObject *parent=nullptr);
  /** Destructor. */
  ~TyTInterface();

  bool isOpen() const;
  RadioInfo identifier(const ErrorStack &err=ErrorStack());
  void close();

  bool read_start(uint32_t bank, uint32_t addr, const ErrorStack &err=ErrorStack());
  bool read(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes, const ErrorStack &err=ErrorStack());
  bool read_finish(const ErrorStack &err=ErrorStack());
  bool write_start(uint32_t bank, uint32_t addr, const ErrorStack &err=ErrorStack());
  bool write(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes, const ErrorStack &err=ErrorStack());
  bool write_finish(const ErrorStack &err=ErrorStack());
  bool reboot(const ErrorStack &err=ErrorStack());

  /** Erases a memory section at @c start of size @c size. */
  bool erase(unsigned start, unsigned size, void (*progress)(unsigned, void *)=nullptr, void *ctx=nullptr, const ErrorStack &err=ErrorStack());

protected:
  /** Internal used function to send a controll command to the device. */
  int md380_command(uint8_t a, uint8_t b, const ErrorStack &err=ErrorStack());
  /** Internal used function to set the current I/O address. */
  int set_address(uint32_t address, const ErrorStack &err=ErrorStack());
  /** Internal used function to erase a specific block. */
  int erase_block(uint32_t address, const ErrorStack &err=ErrorStack());
  /** Internal used function to read the device identifier. */
  const char *identify(const ErrorStack &err=ErrorStack());

protected:
  /** Read identifier. */
  RadioInfo _ident;
};

#endif // TYTINTERFACE_HH
