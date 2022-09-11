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
 * writes. This is not unusual as such, but they implemented a weird memory mapping. Everything
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
 * There is no explicit response read back.
 *
 * @subsubsection tytiferb Reboot
 * The request written is
 * @code
 * +------+------+
 * | 0x91 | 0x05 |
 * +------+------+
 * @endcode
 * There is no explicit response read back.
 *
 * @subsubsection tyifid Identify/Get Info
 * The request written is
 * @code
 * +------+------+
 * | 0xa2 | What |
 * +------+------+
 * @endcode
 * There is always some response read back. The size varies with whatever is read back.
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
 * @ingroup tyt */
class TyTInterface : public DFUSEDevice, public RadioInterface
{
  Q_OBJECT

public:
  /** Constructor. Opens an interface to the specified interface. */
  TyTInterface(const USBDeviceDescriptor &descr,
               const ErrorStack &err=ErrorStack(), QObject *parent=nullptr);
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

public:
  /** Returns some information about the interface. */
  static USBDeviceInfo interfaceInfo();
  /** Tries to find all interfaces connected TyT radios. */
  static QList<USBDeviceDescriptor> detect();

protected:
  /** Internal used function to send a control command to the device. */
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
