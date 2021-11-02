#ifndef OPENRTXINTERFACE_HH
#define OPENRTXINTERFACE_HH

#include "radiointerface.hh"
#include "dfu_libusb.hh"

/** Implements the communication interface to radios running the OpenRTX firmware.
 *
 * @ingroup ortx */
class OpenRTXInterface : public DFUDevice, public RadioInterface
{
  Q_OBJECT

public:
  explicit OpenRTXInterface(QObject *parent = nullptr);

  bool isOpen() const;
  void close();

  const QString &errorMessage() const;

  RadioInfo identifier();

  bool read_start(uint32_t bank, uint32_t addr);
  bool read(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes);
  bool read_finish();

  bool write_start(uint32_t bank, uint32_t addr);
  bool write(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes);
  bool write_finish();

  bool reboot();


};

#endif // OPENRTXINTERFACE_HH
