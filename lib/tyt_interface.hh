#ifndef TYTINTERFACE_HH
#define TYTINTERFACE_HH

#include "dfu_libusb.hh"
#include "radiointerface.hh"

/** Interface to TyT (and Retevis) devices.
 * This class extends the @c DFUDevice and implements the @c RadioInterface to identify the radio
 * and the reading/writing and erasing of memory.
 *
 * @ingroup tyt */
class TyTInterface : public DFUDevice, public RadioInterface
{
  Q_OBJECT

public:
  /** Costructor. Opens an interface to the specified vendor and product ID. */
  TyTInterface(unsigned vid, unsigned pid, QObject *parent=nullptr);
  /** Destructor. */
  ~TyTInterface();

  bool isOpen() const;
  RadioInfo identifier();
  void close();

  bool read_start(uint32_t bank, uint32_t addr);
  bool read(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes);
  bool read_finish();
  bool write_start(uint32_t bank, uint32_t addr);
  bool write(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes);
  bool write_finish();
  bool reboot();

  /** Erases a memory section at @c start of size @c size. */
  bool erase(unsigned start, unsigned size, void (*progress)(unsigned, void *)=nullptr, void *ctx=nullptr);

  const QString &errorMessage() const;

protected:
  /** Internal used function to send a controll command to the device. */
  int md380_command(uint8_t a, uint8_t b);
  /** Internal used function to set the current I/O address. */
  int set_address(uint32_t address);
  /** Internal used function to erase a specific block. */
  int erase_block(uint32_t address);
  /** Internal used function to read the device identifier. */
  const char *identify();

protected:
  /** Read identifier. */
  RadioInfo _ident;
};

#endif // TYTINTERFACE_HH
