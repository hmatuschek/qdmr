#ifndef CODEPLUG_HH
#define CODEPLUG_HH

#include <QObject>
#include "dfufile.hh"
#include "userdatabase.hh"
#include <QHash>
#include "config.hh"

//class Config;
class ConfigObject;


/** This class defines the interface all device-specific code-plugs must implement.
 * Device-specific codeplugs are derived from the common configuration and implement the
 * construction/parsing of the device specific binary configuration. */
class Codeplug: public DFUFile
{
	Q_OBJECT

public:
  /** Certain flags passed to CodePlug::encode to control the transfer and encoding of the
   * codeplug. */
  class Flags {
  public:
    /** If @c true, the codeplug will first be downloaded from the device, updated from the
     * abstract config and then written back to the device. This maintains the user-settings
     * made within the device or manufacturer CPS. If @c false, the code-plug gets overridden
     * entirely using some default settings. Default @c true. */
    bool updateCodePlug;
    /** If @c true enables GPS when there is a GPS or APRS system defined that is used by any
     * channel. This may cause automatic transmissions, hence the default is @c false. */
    bool autoEnableGPS;
    /** If @c true enables automatic roaming when there is a roaming zone defined that is used by any
     * channel. This may cause automatic transmissions, hence the default is @c false. */
    bool autoEnableRoaming;

    /** Default constructor, enables code-plug update and disables automatic GPS/APRS and roaming. */
    Flags();
  };

  /** Represents the abstract base class of all codeplug elements. That is a memory region within
   * the codeplug that encodes a specific element. E.g., channels, contacts, zones, etc.
   * This class provies some helper methods to access specific members of the element.
   * @since 0.9.0 */
  class Element
  {
  protected:
    /** Hidden constructor.
     * @param ptr Specifies the pointer to the element within the codeplug.
     * @param size Specifies the size of the element in bytes. */
    Element(uint8_t *ptr, size_t size);

  public:
    /** Copy constructor. */
    Element(const Element &other);
    /** Destructor. */
    virtual ~Element();

    /** Returns @c true if the pointer is not null. */
    virtual bool isValid() const;
    /** Abstract method to reset the element within the codeplug. Any device specific element
     * should implement this method. */
    virtual void clear();

    /** Reads a specific bit at the given byte-offset. */
    bool getBit(uint offset, uint bit) const;
    /** Sets a specific bit at the given byte-offset. */
    void setBit(uint offset, uint bit, bool value=true);
    /** Clears a specific bit at the given byte-offset. */
    void clearBit(uint offset, uint bit);

    /** Reads a 2bit integer at the given byte- and bit-offset. */
    uint8_t getUInt2(uint offset, uint bit) const;
    /** Stores a 2bit integer at the given byte- and bit-offset. */
    void setUInt2(uint offset, uint bit, uint8_t value);

    /** Reads a 3bit integer at the given byte- and bit-offset. */
    uint8_t getUInt3(uint offset, uint bit) const;
    /** Stores a 3bit integer at the given byte- and bit-offset. */
    void setUInt3(uint offset, uint bit, uint8_t value);

    /** Reads a 4bit integer at the given byte- and bit-offset. */
    uint8_t getUInt4(uint offset, uint bit) const;
    /** Stores a 4bit integer at the given byte- and bit-offset. */
    void setUInt4(uint offset, uint bit, uint8_t value);

    /** Reads a 5bit integer at the given byte- and bit-offset. */
    uint8_t getUInt5(uint offset, uint bit) const;
    /** Stores a 5bit integer at the given byte- and bit-offset. */
    void setUInt5(uint offset, uint bit, uint8_t value);

    /** Reads a 6bit integer at the given byte- and bit-offset. */
    uint8_t getUInt6(uint offset, uint bit) const;
    /** Stores a 6bit integer at the given byte- and bit-offset. */
    void setUInt6(uint offset, uint bit, uint8_t value);

    /** Reads a 8bit integer at the given byte- and bit-offset. */
    uint8_t getUInt8(uint offset) const;
    /** Reads a 8bit integer at the given byte- and bit-offset. */
    void setUInt8(uint offset, uint8_t value);

    /** Reads a 16bit big-endian integer at the given byte-offset. */
    uint16_t getUInt16_be(uint offset) const;
    /** Reads a 16bit little-endian integer at the given byte-offset. */
    uint16_t getUInt16_le(uint offset) const;
    /** Stores a 16bit big-endian integer at the given byte-offset. */
    void setUInt16_be(uint offset, uint16_t value);
    /** Stores a 16bit little-endian integer at the given byte-offset. */
    void setUInt16_le(uint offset, uint16_t value);

    /** Reads a 24bit big-endian integer at the given byte-offset. */
    uint32_t getUInt24_be(uint offset) const;
    /** Reads a 24bit little-endian integer at the given byte-offset. */
    uint32_t getUInt24_le(uint offset) const;
    /** Stores a 24bit big-endian integer at the given byte-offset. */
    void setUInt24_be(uint offset, uint32_t value);
    /** Stores a 24bit little-endian integer at the given byte-offset. */
    void setUInt24_le(uint offset, uint32_t value);

    /** Reads a 32bit big-endian integer at the given byte-offset. */
    uint32_t getUInt32_be(uint offset) const;
    /** Reads a 32bit little-endian integer at the given byte-offset. */
    uint32_t getUInt32_le(uint offset) const;
    /** Stores a 32bit big-endian integer at the given byte-offset. */
    void setUInt32_be(uint offset, uint32_t value);
    /** Stores a 32bit little-endian integer at the given byte-offset. */
    void setUInt32_le(uint offset, uint32_t value);

    /** Reads a 2-digit (1-byte/8bit) BDC value in big-endian at the given byte-offset. */
    uint8_t getBCD2(uint offset) const;
    /** Stores a 2-digit (1-byte/8bit) BDC value in big-endian at the given byte-offset. */
    void setBCD2(uint offset, uint8_t value);

    /** Reads a 4-digit (2-byte/16bit) BDC value in big-endian at the given byte-offset. */
    uint16_t getBCD4_be(uint offset) const;
    /** Stores a 4-digit (2-byte/16bit) BDC value in big-endian at the given byte-offset. */
    void setBCD4_be(uint offset, uint16_t value);
    /** Reads a 4-digit (2-byte/16bit) BDC value in little-endian at the given byte-offset. */
    uint16_t getBCD4_le(uint offset) const;
    /** Stores a 4-digit (1-byte/16bit) BDC value in little-endian at the given byte-offset. */
    void setBCD4_le(uint offset, uint16_t value);

    /** Reads a 8-digit (4-byte/32bit) BDC value in big-endian at the given byte-offset. */
    uint32_t getBCD8_be(uint offset) const;
    /** Stores a 8-digit (4-byte/32bit) BDC value in big-endian at the given byte-offset. */
    void setBCD8_be(uint offset, uint32_t value);
    /** Reads a 8-digit (4-byte/32bit) BDC value in little-endian at the given byte-offset. */
    uint32_t getBCD8_le(uint offset) const;
    /** Stores a 8-digit (4-byte/32bit) BDC value in little-endian at the given byte-offset. */
    void setBCD8_le(uint offset, uint32_t value);

    /** Reads upto @c maxlen ASCII chars at the given byte-offset using @c eos as the string termination char. */
    QString readASCII(uint offset, uint maxlen, uint8_t eos=0x00) const;
    /** Stores upto @c maxlen ASCII chars at the given byte-offset using @c eos as the string termination char.
     * The stored string gets padded with @c eos to @c maxlen. */
    void writeASCII(uint offset, const QString &txt, uint maxlen, uint8_t eos=0x00);

    /** Reads upto @c maxlen unicode chars at the given byte-offset using @c eos as the string termination char. */
    QString readUnicode(uint offset, uint maxlen, uint16_t eos=0x0000) const;
    /** Stores upto @c maxlen unicode chars at the given byte-offset using @c eos as the string termination char.
     * The stored string gets padded with @c eos to @c maxlen. */
    void writeUnicode(uint offset, const QString &txt, uint maxlen, uint16_t eos=0x0000);

  protected:
    /** Holds the pointer to the element. */
    uint8_t *_data;
    /** Holds the size of the element. */
    size_t _size;
  };

  /** Base class for all codeplug contexts.
   * Each device specific codeplug may extend this class to allow for device specific elements to
   * be indexed in a separate index. By default tables for @c DigitalContact, @c RXGroupList,
   * @c Channel, @c Zone and @c ScanList are defined. For any other type, an additional table must
   * be defined first using @c addTable.
   * @since 0.9.0 */
  class Context
  {
  public:
    /** Empty constructor. */
    Context();

    /** Resolves the given index for the specifies element type.
     * @returns @c nullptr if the index is not defined or the type is unknown. */
    ConfigObject *obj(const QMetaObject *elementType, uint idx);
    /** Returns the index for the given object.
     * @returns -1 if no index is associated with the object or its type is unknown. */
    int index(ConfigObject *obj);
    /** Associates the given object with the given index. */
    bool add(ConfigObject *obj, uint idx);

    /** Adds a table for the given type. */
    bool addTable(const QMetaObject *obj);

    /** Returns the object associated by the given index and type. */
    template <class T>
    T* get(uint idx) {
      return this->obj(&(T::staticMetaObject), idx)->template as<T>();
    }

    /** Returns @c true, if the given index is defined for the specified type. */
    template <class T>
    bool has(uint idx) {
      return nullptr != this->obj(&(T::staticMetaObject), idx)->template as<T>();
    }

  protected:
    /** Internal used table type to associate objects and indices. */
    class Table {
    public:
      /** The index->object map. */
      QHash<uint, ConfigObject *> objects;
      /** The object->index map. */
      QHash<ConfigObject *, uint> indices;
    };

  protected:
    /** Returns @c true if a table is defined for the given type. */
    bool hasTable(const QMetaObject *obj) const;
    /** Returns a reference to the table for the given type. */
    Table &getTable(const QMetaObject *obj);

  protected:
    /** Table of tables. */
    QHash<QString, Table> _tables;
  };

protected:
  /** Hidden default constructor. */
	explicit Codeplug(QObject *parent=nullptr);

public:
  /** Destructor. */
	virtual ~Codeplug();

  /** Indexes all elements of the codeplug.
   * This method must be implemented by any device or vendor specific codeplug to map config
   * objects to indices used within the binary codeplug to address each element (e.g., channels,
   * contacts etc.). */
  virtual bool index(Config *config, Context &ctx) const = 0;

  /** Decodes a binary codeplug to the given abstract configuration @c config.
   * This must be implemented by the device-specific codeplug. */
	virtual bool decode(Config *config) = 0;
  /** Encodes a given abstract configuration (@c config) to the device specific binary code-plug.
   * This must be implemented by the device-specific codeplug. */
  virtual bool encode(Config *config, const Flags &flags=Flags()) = 0;
};

#endif // CODEPLUG_HH
