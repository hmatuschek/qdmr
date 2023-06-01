#ifndef DFUFILE_HH
#define DFUFILE_HH

#include <QFile>
#include <QVector>
#include <QByteArray>
#include <QString>
#include <QTextStream>

#include "addressmap.hh"
#include "errorstack.hh"

class CRC32;

/** A collection of images, each consisting of one or more memory sections.
 *
 * This class forms the base of all binary encoded codeplugs and call-sign DBs. To this end, it
 * represents the codeplug or call-sign DB memory as written and stored inside the radio. It also
 * implements the DFU file format (hence the name) and thus allows to store and load binary
 * codeplugs or call-sign DBs in files. Please note, that binary codeplugs and call-sign DBs are
 * highly vendor and device specific. Consequently, they should be be used to exchange codeplugs.
 *
 * DFU File consists of a file prefix followed by several images and a final file suffix. The file
 * prefix consists of a file signature of 5 bytes just consisting of the ASCII string "DfuSe",
 * followed by a single byte indicating the version number (V), here 0x01. The next field is a
 * uint32_t containing the file-size excluding the suffix in little endian. Finally, there is a
 * single byte (N) holding the number of images.
 *
 * @code
 * +---+---+---+---+---+---+---+---+---+---+---+
 * |      "DfuSe"      | V |   file size   | N |
 * +---+---+---+---+---+---+---+---+---+---+---+
 * @endcode
 *
 * The file suffix consists of the device, product and vendor IDs as uint16_t followed by a
 * fixed signature uin16_t 0x011a (little endian) followed by another fixed signature containing the
 * ASCII string "UFD". The next field (S) contains the size of the suffix, that is 16. Finally there
 * is a CRC32 field computed over the entire file excluding the CRC itself and stored in little
 * endian.
 *
 * @code
 * +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 * |  dev  |  prod |  vend |  Sig  |    UFD    | S |     CRC32     |
 * +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 * @endcode
 *
 * Each image section consists of a image prefix followed by several element sections. The image
 * prefix consists of a 6byte signature containing the ASCII string "Target" followed by a
 * single byte indicating the so-called "alternate settings" field, usually 0x01. The 32bit field
 * "is named" just indicates that the 255 byte name field is set (i.e., 0x01 in little endian).
 * The next field contains the 32 bit size of the image excluding the image prefix in little endian.
 * Finally the last 32bit field contains the number of elements, the image consists of in little
 * endian too.
 *
 * @code
 * +---+---+---+---+---+---+---+---+---+---+---+---+...+---+---+---+---+---+---+---+---+---+
 * |       "Target"        | A |    is named   | 255b name |      size     |   N Elements  |
 * +---+---+---+---+---+---+---+---+---+---+---+---+...+---+---+---+---+---+---+---+---+---+
 * @endcode
 *
 * Finally, each element of an image is prefixed by a header containing the target address and size
 * of the element in little endian.
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

public:
  /** Represents a single element within a @c Image. */
	class Element {
	public:
    /** Empty constructor. */
		Element();
    /** Constructs an element for the given address and of the given size. */
		Element(uint32_t addr, uint32_t size);
    /** Copy constructor. */
		Element(const Element &other);
    /** Copying assignment. */
		Element &operator= (const Element &other);

    /** Returns the address of the element. */
		uint32_t address() const;
    /** Sets the address of the element. */
		void setAddress(uint32_t addr);
    /** Returns the size of the element (including headers). */
		uint32_t size() const;
    /** Returns the memory size of the element. */
    uint32_t memSize() const;
    /** Checks if the element address and size is aligned with the given block size. */
    bool isAligned(unsigned blocksize) const;
    /** Returns a reference to the data. */
		const QByteArray &data() const;
    /** Returns a reference to the data. */
		QByteArray &data();

    /** Reads an element from the given file and updates the CRC. */
		bool read(QFile &file, CRC32 &crc, QString &errorMessage);
    /** Writes an element to the given file and updates the CRC. */
		bool write(QFile &file, CRC32 &crc, QString &errorMessage) const;

    /** Dumps a textual representation of the element. */
		void dump(QTextStream &stream) const;

	protected:
    /** The address of the element. */
		uint32_t _address;
    /** The data of the element. */
		QByteArray _data;
	};

  /** Represents a single image within a @c DFUFile. */
	class Image
	{
	public:
    /** Default constructor.
     * Constructs an empty image. */
		Image();
    /** Constructs an image with the given name and optional "alternative settings". */
    Image(const QString &name, uint8_t altSettings=0);
    /** Copy constructor. */
		Image(const Image &other);
    /** Destructor. */
    virtual ~Image();
    /** Copying assignment. */
		Image &operator=(const Image &other);

    /** Returns the alternate settings byte. */
		uint8_t alternateSettings() const;
    /** Sets the alternate settings byte. */
		void setAlternateSettings(uint8_t s);

    /** Returns @c true if the image is named. */
		bool isNamed() const;
    /** Returns the name of the image. */
		const QString &name() const;
    /** Sets the name of the image. */
		void setName(const QString &name);
    /** Returns the total size of the image (including headers). */
		uint32_t size() const;
    /** Returns the memory size stored in the image. */
    uint32_t memSize() const;
    /** Returns the number of elements of this image. */
		int numElements() const;
    /** Returns a reference to the i-th element of the image. */
		const Element &element(int i) const;
    /** Returns a reference to the i-th element of the image. */
    Element &element(int i);
    /** Adds an element to the image with the given address and size at the specified index.
     * If the index is negative, the element gets appended. */
    void addElement(uint32_t addr, uint32_t size, int index=-1);
    /** Adds an element to the image. */
    void addElement(const Element &element);
    /** Removes the i-th element from this image. */
		void remElement(int i);
    /** Checks if all element addresses and sizes is aligned with the given block size. */
    bool isAligned(unsigned blocksize) const;

    /** Reads an image from the given file and updates the CRC. */
		bool read(QFile &file, CRC32 &crc, QString &errorMessage);
    /** Writes this image to the given file and updates the CRC. */
		bool write(QFile &file, CRC32 &crc, QString &errorMessage) const;

    /** Prints a textual representation of the image into the given stream. */
		void dump(QTextStream &stream) const;

    /** Retunrs @c true if the specified address is allocated. */
    virtual bool isAllocated(uint32_t offset) const;

    /** Returns a pointer to the encoded raw data at the specified offset. */
    virtual unsigned char *data(uint32_t offset);
    /** Returns a const pointer to the encoded raw data at the specified offset. */
    virtual const unsigned char *data(uint32_t offset) const;

    /** Sorts all elements with respect to their addresses. */
    void sort();

	protected:
    /** Alternate settings byte. */
		uint8_t  _alternate_settings;
    /** Optional image name. */
		QString _name;
    /** The elements of the image. */
		QVector<Element> _elements;
    /** Maps an address range to element index. */
    AddressMap _addressmap;
	};

public:
  /** Constructs an empty DFU file object. */
	DFUFile(QObject *parent=nullptr);

  /** Returns the total size of the DFU file. */
	uint32_t size() const;
  /** Returns the total memory size stored in the DFU file. */
  uint32_t memSize() const;

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

  /** Checks if all image addresses and sizes is aligned with the given block size. */
  bool isAligned(unsigned blocksize) const;

  /** Reads the specified DFU file.
   * @return @c false on error. */
  bool read(const QString &filename, const ErrorStack &err=ErrorStack());
  /** Reads the specified DFU file.
   * @returns @c false on error. */
  bool read(QFile &file, const ErrorStack &err=ErrorStack());

  /** Writes to the specified file.
   * @returns @c false on error. */
  bool write(const QString &filename, const ErrorStack &err=ErrorStack());
  /** Writes to the specified file.
   * @returns @c false on error. */
  bool write(QFile &file, const ErrorStack &err=ErrorStack());

  /** Dumps a text representation of the DFU file structure to the specified text stream. */
	void dump(QTextStream &stream) const;

  /** Returns @c true if the specified address (and image) is allocated. */
  virtual bool isAllocated(uint32_t offset, uint32_t img=0) const;

  /** Returns a pointer to the encoded raw data at the specified offset. */
  virtual unsigned char *data(uint32_t offset, uint32_t img=0);
  /** Returns a const pointer to the encoded raw data at the specified offset. */
  virtual const unsigned char *data(uint32_t offset, uint32_t img=0) const;

protected:
  /// The list of images.
	QVector<Image> _images;
};

#endif // DFUFILE_HH
