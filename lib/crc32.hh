#ifndef CRC32_HH
#define CRC32_HH

#include <QByteArray>

/** Implements the CRC32 checksum as used in DFU files.
 *
 * @ingroup util */
class CRC32
{
public:
  /** Default constructor. */
	CRC32();
  /** Update CRC with given byte. */
	void update(uint8_t c);
  /** Update CRC with given data. */
	void update(const uint8_t *c, size_t n);
  /** Update CRC with given data. */
	void update(const QByteArray &data);
  /** Returns the current CRC. */
  inline uint32_t get() { return _crc; }

protected:
  /** Current CRC. */
	uint32_t _crc;
};

#endif // CRC32_HH
