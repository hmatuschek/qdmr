#ifndef DFUFILE_HH
#define DFUFILE_HH

#include <QFile>
#include <QVector>
#include <QByteArray>
#include <QString>
#include <QTextStream>

class CRC32;

/** Implements reading and writing DFU files.
 *
 * DFU File consists of a file prefix followed by several images and a final file suffix. The file
 * prefix consists of a file signature of 5 bytes just consisting of the ASCII string "DfuSe",
 * followed by a single byte indicating the version number (V), here 0x01. The next field is a
 * uint32_t containing the file-size excluding the prefix. Finally, there is a single byte (N)
 * holding the number of images.
 *
 * @code
 * +---+---+---+---+---+---+---+---+---+---+---+
 * |      "DfuSe"      | V |   file size   | N |
 * +---+---+---+---+---+---+---+---+---+---+---+
 * @endcode
 *
 * The file suffix consists of the device, product and vendor IDs as uint16_t followed by a
 * fixed signature uin16_t 0x011a followed by another fixed signature containing the ASCII string
 * "UFD". The next field (S) contains the size of the suffix, that is 16. Finally there is a CRC32
 * field computed over the entire file excluding the CRC itself.
 *
 * @code
 * +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 * |  dev  |  prod |  vend |  Sig  |    UFD    | S |     CRC32     |
 * +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 * @endcode
 *
 * Each image section consists of a image prefix followed by several element sections. The image
 * prefix consists of a 6byte signature containting the ASCII string "Target" followed by a
 * single byte indicating the so-called "alternate settings" field, usually 0x01. The 32bit field
 * "is named" just indicates that the 255 byte name field is set. The next field contains the 32 bit
 * size of the image excluding the image prefix. Finally the last 32bit field contains the number
 * of elements, the image consists of.
 *
 * @code
 * +---+---+---+---+---+---+---+---+---+---+---+---+...+---+---+---+---+---+---+---+---+---+
 * |       "Target"        | A |    is named   | 255b name |      size     |   N Elements  |
 * +---+---+---+---+---+---+---+---+---+---+---+---+...+---+---+---+---+---+---+---+---+---+
 * @endcode
 *
 * Finally, each element of an image is prefixed by a header containing the target address and size
 * of the element.
 *
 * @code
 * +---+---+---+---+---+---+---+---+---+...+---+
 * |    address    |      size     | el. data  |
 * +---+---+---+---+---+---+---+---+---+...+---+
 * @endcode
 *
 * @ingroup util
 */
class DFUFile: public QObject
{
	Q_OBJECT

  /** Represents a single element within a @c Image. */
	class Element {
	public:
		Element();
		Element(uint32_t addr, uint32_t size);
		Element(const Element &other);

		Element &operator= (const Element &other);

		uint32_t address() const;
		void setAddress(uint32_t addr);

		uint32_t size() const;

    bool isAligned(uint blocksize) const;

		const QByteArray &data() const;
		QByteArray &data();

		bool read(QFile &file, CRC32 &crc, QString &errorMessage);
		bool write(QFile &file, CRC32 &crc, QString &errorMessage) const;

		void dump(QTextStream &stream) const;

	protected:
		uint32_t _address;
		QByteArray _data;
	};

  /** Represents a single image within a @c DFUFile. */
	class Image
	{
	public:
		Image();
    Image(const QString &name, uint8_t altSettings=0);
		Image(const Image &other);

		Image &operator=(const Image &other);

		uint8_t alternateSettings() const;
		void setAlternateSettings(uint8_t s);

		bool isNamed() const;
		const QString &name() const;
		void setName(const QString &name);

		uint32_t size() const;

		int numElements() const;
		const Element &element(int i) const;
		Element &element(int i);
		void addElement(uint32_t addr, uint32_t size);
		void addElement(const Element &element);
		void remElement(int i);

		bool read(QFile &file, CRC32 &crc, QString &errorMessage);
		bool write(QFile &file, CRC32 &crc, QString &errorMessage) const;

		void dump(QTextStream &stream) const;

	protected:
		uint8_t  _alternate_settings;
		QString _name;
		QVector<Element> _elements;
	};

public:
  /** Constructs an empty DFU file object. */
	DFUFile(QObject *parent=nullptr);

  /** Returns the total size of the DFU file. */
	uint32_t size() const;

  /** Returns the number of images within the DFU file. */
	int numImages() const;
  /** Returns a reference to the @c i-th image of the file. */
	const Image &image(int i) const;
  /** Returns a reference to the @c i-th image of the file. */
	Image &image(int i);
  /** Adds a new image to the file. */
	void addImage(const QString &name, uint8_t altSettings=1);
  /** Adds an image to the file. */
	void addImage(const Image &img);
  /** Deletes the @c i-th image from the file. */
	void remImage(int i);

  /** Returns the error message in case of an error. */
	const QString &errorMessage() const;

  /** Reads the specified DFU file.
   * @return @c false on error. */
	bool read(const QString &filename);
  /** Reads the specified DFU file.
   * @returns @c false on error. */
	bool read(QFile &file);

  /** Writes to the specified file.
   * @returns @c false on error. */
	bool write(const QString &filename);
  /** Writes to the specified file.
   * @returns @c false on error. */
	bool write(QFile &file);

  /** Dumps a text representation of the DFU file structure to the specified text stream. */
	void dump(QTextStream &stream) const;

protected:
  /// Holds the error string.
	QString _errorMessage;
  /// The list of images.
	QVector<Image> _images;
};

#endif // DFUFILE_HH
