#ifndef GD73INTERFACE_HH
#define GD73INTERFACE_HH

#include "c7000device.hh"
#include "radiointerface.hh"

class GD73Interface: public C7000Device, public RadioInterface
{
  Q_OBJECT

public:
  /** Constructs a new interface to GD73A/E radios. If a matching device was found, @c isOpen
   * returns @c true. */
  GD73Interface(const USBDeviceDescriptor &descriptor,
                const ErrorStack &err=ErrorStack(), QObject *parent=nullptr);

  bool isOpen() const;

  RadioInfo identifier(const ErrorStack &err=ErrorStack());

  bool write_start(uint32_t bank, uint32_t addr, const ErrorStack &err=ErrorStack());
  bool write(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes, const ErrorStack &err=ErrorStack());
  bool write_finish(const ErrorStack &err=ErrorStack());

  bool read_start(uint32_t bank, uint32_t addr, const ErrorStack &err=ErrorStack());
  bool read(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes, const ErrorStack &err=ErrorStack());
  bool read_finish(const ErrorStack &err=ErrorStack());

  void close();

protected:
  /** Name of the radio. */
  QString _identifier;
  /** Last received/send sequence number. */
  uint16_t _lastSequence;
};

#endif // GD73INTERFACE_HH
