/** @defgroup rif Radio interfaces
 * Depending on the manufacturer or model, different radios have different computer-radio
 * interfaces. This module collects all classes that provide these interfaces. */
#ifndef RADIOINFERFACE_HH
#define RADIOINFERFACE_HH

#include <QObject>

/** Abstract radio interface.
 * A radion interface must provide means to communicate with the device. That is, open a connection
 * to the device, allow for reading and writing specific memory blocks.
 *
 * This class defines the common interface for all radio-interface classes, irrespective of the
 * actual communication protocol being used by the device.
 * @ingroup rif */
class RadioInferface : public QObject
{
	Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit RadioInferface(QObject *parent = nullptr);

public:
  /** Destructor. */
	virtual ~RadioInferface();

  /** Return @c true if a connection to the device has been established. */
	virtual bool isOpen() const = 0;
  /** Closes the connection to the device. */
	virtual void close() = 0;

  /** Returns a device identifier. */
	virtual QString identifier() const = 0;

  /** Writes a chunk of @c data at the block-address @c bno (block number).
   * @param bno Specifies the block number to write to.
   * @param data Pointer to the actual data to be written.
   * @param nbytes Specifies the number of bytes to write.
   * @returns @c true on success. */
	virtual bool write_block(int bno, uint8_t *data, int nbytes) = 0;

  /** Reads a chunk of data from the block-address @c bno (block number).
   * @param bno Specifies the block number to read from.
   * @param data Pointer where to store the read data.
   * @param nbytes Specifies the number of bytes to read.
   * @returns @c true on success. */
	virtual bool read_block(int bno, uint8_t *data, int nbytes) = 0;

  /** Some radios needs to be rebooted after being read or programmed. */
	virtual bool reboot();

  /** Returns the last error message. */
  inline const QString &errorMessage() const { return _errorMessage; }

protected:
  /** Last error message. */
  QString _errorMessage;
};

#endif // RADIOINFERFACE_HH
