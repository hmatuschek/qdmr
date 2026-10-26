#ifndef HD1_INTERFACE_HH
#define HD1_INTERFACE_HH

#include "usbserial.hh"
#include <QDateTime>

/** Implements the serial programming protocol of the Ailunce HD1.
 *
 * The HD1 is programmed through a CH340 USB-serial cable at 8N1. Its UART runs about 6% fast
 * (around 122 kBaud instead of the nominal 115.2 kBaud), so every reply is garbled at 115200
 * baud. The window that works in both directions is about 117.6–123.7 kBaud; this interface
 * uses 120000 baud, which the CH340 hits exactly.
 *
 * @section hd1frames Frames
 * Requests and replies share one frame format:
 * @code
 * 68 <cmd> <dir> 01 <b4> <b5> <len:LE16> <addr:LE16> [payload] 10
 * @endcode
 * @c cmd selects one of three address spaces:
 * - @c 0x0d: the 16-byte radio info block (frequency bands),
 * - @c 0x0f: a byte-addressed space for settings (bank @c HD1Interface::Small),
 * - @c 0x31: a space addressed in 1 KiB pages for channels, contacts, zones, etc. (bank
 *   @c HD1Interface::Bulk; this interface takes byte addresses there and converts them).
 *
 * @c dir is 00 for a read request and 01 for a write request. A read reply repeats the
 * request header (with dir 02 for @c 0x31, unchanged otherwise) followed by the data. A write
 * is acknowledged by its own header without payload, again with dir 02 for @c 0x31. @c b4 is
 * the programming software's progress-bar value and @c b5 a tag (00 for @c 0x0f, 31 for
 * @c 0x31 writes); the radio ignores both on reads.
 *
 * @section hd1sessions Sessions
 * - The radio ignores frames for a few seconds after power-on; the info request is retried
 *   until it answers.
 * - A write session starts with a timestamp frame, the programming time as six LE16 values
 *   (@c 68 0f 05 Y M D h m s 10), which the radio echoes. The manufacturer's software then
 *   reads the info block, the model (@c 0x0f @c 0xe800, "IHD8580") and @c 0x0f @c 0x2950
 *   before writing; this interface does the same.
 * - Every session ends with the ASCII string @c END. The radio then reboots and prints a
 *   boot banner, which is discarded.
 *
 * @ingroup hd1 */
class HD1Interface: public USBSerial
{
  Q_OBJECT

public:
  /** Memory banks, i.e. the address spaces of the radio. */
  enum Bank {
    Small = 0,  ///< Byte-addressed settings space (command 0x0f).
    Bulk  = 1,  ///< Page-addressed space (command 0x31), byte addresses on this interface.
    Info  = 2   ///< The 16-byte info block (command 0x0d).
  };

  /** Size of a page in the bulk space. */
  static constexpr unsigned int pageSize() { return 0x400; }
  /** The model string at @c 0x0f @c 0xe800. */
  static QByteArray model() { return QByteArray("IHD8580"); }

public:
  /** Opens the port of the given descriptor at 120000 baud. */
  explicit HD1Interface(const USBDeviceDescriptor &descr, const ErrorStack &err=ErrorStack(),
                        QObject *parent=nullptr);

  /** The CH340 cable (1a86:7523). It is a generic USB-serial chip, so the radio must be
   * selected explicitly. The port itself is listed by the generic serial-port detection
   * (see @c USBSerial::detect()). */
  static USBDeviceInfo interfaceInfo();

  RadioInfo identifier(const ErrorStack &err=ErrorStack()) override;

  bool read_start(uint32_t bank, uint32_t addr, const ErrorStack &err=ErrorStack()) override;
  bool read(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes, const ErrorStack &err=ErrorStack()) override;
  bool read_finish(const ErrorStack &err=ErrorStack()) override;

  bool write_start(uint32_t bank, uint32_t addr, const ErrorStack &err=ErrorStack()) override;
  bool write(uint32_t bank, uint32_t addr, uint8_t *data, int nbytes, const ErrorStack &err=ErrorStack()) override;
  bool write_finish(const ErrorStack &err=ErrorStack()) override;

  /** Ends the session with @c END; the radio reboots. */
  bool reboot(const ErrorStack &err=ErrorStack()) override;
  /** Sets the time sent in the timestamp frame of the next write session (default: now). */
  bool setDateTime(const QDateTime &datetime, const ErrorStack &err=ErrorStack()) override;

  /** Retries the info request until the radio answers (it is deaf right after booting). */
  bool waitReady(const ErrorStack &err=ErrorStack());

protected slots:
  /** Logs serial port errors except timeouts, which are expected while the radio boots. */
  void onSerialError(QSerialPort::SerialPortError error);

protected:
  /** Reads @c len bytes at @c addr (bytes for 0x0f, pages for 0x31) with one request. */
  bool request(uint8_t cmd, uint16_t addr, uint16_t len, uint8_t *data, const ErrorStack &err);
  /** Writes one frame and checks its acknowledgement. */
  bool writeFrame(uint8_t cmd, uint16_t addr, const uint8_t *data, uint16_t len, const ErrorStack &err);
  /** Sends @c out and receives exactly @c n bytes into @c in. */
  bool transfer(const QByteArray &out, QByteArray &in, int n, int timeout, const ErrorStack &err);
  /** Discards whatever the radio sends until it is quiet for @c quiet ms. */
  void drain(int quiet, int timeout);

protected:
  /** Whether the radio answered since the last session end. */
  bool _ready;
  /** Number of write frames sent in the current write session (selects the progress byte). */
  unsigned int _writeCount;
  /** Time for the next timestamp frame; invalid means now. */
  QDateTime _timestamp;
};

#endif // HD1_INTERFACE_HH
