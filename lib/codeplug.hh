#ifndef CODEPLUG_HH
#define CODEPLUG_HH

#include <QObject>
#include "dfufile.hh"
#include "userdatabase.hh"
#include <QHash>
#include "config.hh"

//class Config;
class ConfigItem;


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
   * This class provides some helper methods to access specific members of the element.
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

    /** Fills the memsets the entire element to the given value. */
    bool fill(uint8_t value, unsigned offset=0, int size=-1);

    /** Reads a specific bit at the given byte-offset. */
    bool getBit(unsigned offset, unsigned bit) const;
    /** Sets a specific bit at the given byte-offset. */
    void setBit(unsigned offset, unsigned bit, bool value=true);
    /** Clears a specific bit at the given byte-offset. */
    void clearBit(unsigned offset, unsigned bit);

    /** Reads a 2bit unsigned integer at the given byte- and bit-offset. */
    uint8_t getUInt2(unsigned offset, unsigned bit) const;
    /** Stores a 2bit unsigned integer at the given byte- and bit-offset. */
    void setUInt2(unsigned offset, unsigned bit, uint8_t value);

    /** Reads a 3bit unsigned integer at the given byte- and bit-offset. */
    uint8_t getUInt3(unsigned offset, unsigned bit) const;
    /** Stores a 3bit unsigned integer at the given byte- and bit-offset. */
    void setUInt3(unsigned offset, unsigned bit, uint8_t value);

    /** Reads a 4bit unsigned integer at the given byte- and bit-offset. */
    uint8_t getUInt4(unsigned offset, unsigned bit) const;
    /** Stores a 4bit unsigned integer at the given byte- and bit-offset. */
    void setUInt4(unsigned offset, unsigned bit, uint8_t value);

    /** Reads a 5bit unsigned integer at the given byte- and bit-offset. */
    uint8_t getUInt5(unsigned offset, unsigned bit) const;
    /** Stores a 5bit iunsinged nteger at the given byte- and bit-offset. */
    void setUInt5(unsigned offset, unsigned bit, uint8_t value);

    /** Reads a 6bit unsigned integer at the given byte- and bit-offset. */
    uint8_t getUInt6(unsigned offset, unsigned bit) const;
    /** Stores a 6bit unsigned integer at the given byte- and bit-offset. */
    void setUInt6(unsigned offset, unsigned bit, uint8_t value);

    /** Reads a 8bit unsigned integer at the given byte- and bit-offset. */
    uint8_t getUInt8(unsigned offset) const;
    /** Reads a 8bit unsigned integer at the given byte- and bit-offset. */
    void setUInt8(unsigned offset, uint8_t value);
    /** Reads a 8bit signed integer at the given byte- and bit-offset. */
    int8_t getInt8(unsigned offset) const;
    /** Reads a 8bit signed integer at the given byte- and bit-offset. */
    void setInt8(unsigned offset, int8_t value);

    /** Reads a 16bit big-endian unsigned integer at the given byte-offset. */
    uint16_t getUInt16_be(unsigned offset) const;
    /** Reads a 16bit little-endian unsigned integer at the given byte-offset. */
    uint16_t getUInt16_le(unsigned offset) const;
    /** Stores a 16bit big-endian unsigned integer at the given byte-offset. */
    void setUInt16_be(unsigned offset, uint16_t value);
    /** Stores a 16bit little-endian unsigned integer at the given byte-offset. */
    void setUInt16_le(unsigned offset, uint16_t value);

    /** Reads a 24bit big-endian unsigned integer at the given byte-offset. */
    uint32_t getUInt24_be(unsigned offset) const;
    /** Reads a 24bit little-endian unsigned integer at the given byte-offset. */
    uint32_t getUInt24_le(unsigned offset) const;
    /** Stores a 24bit big-endian unsigned integer at the given byte-offset. */
    void setUInt24_be(unsigned offset, uint32_t value);
    /** Stores a 24bit little-endian unsigned integer at the given byte-offset. */
    void setUInt24_le(unsigned offset, uint32_t value);

    /** Reads a 32bit big-endian unsigned integer at the given byte-offset. */
    uint32_t getUInt32_be(unsigned offset) const;
    /** Reads a 32bit little-endian unsigned integer at the given byte-offset. */
    uint32_t getUInt32_le(unsigned offset) const;
    /** Stores a 32bit big-endian unsigned integer at the given byte-offset. */
    void setUInt32_be(unsigned offset, uint32_t value);
    /** Stores a 32bit little-endian unsigned integer at the given byte-offset. */
    void setUInt32_le(unsigned offset, uint32_t value);

    /** Reads a 2-digit (1-byte/8bit) BDC value in big-endian at the given byte-offset. */
    uint8_t getBCD2(unsigned offset) const;
    /** Stores a 2-digit (1-byte/8bit) BDC value in big-endian at the given byte-offset. */
    void setBCD2(unsigned offset, uint8_t value);

    /** Reads a 4-digit (2-byte/16bit) BDC value in big-endian at the given byte-offset. */
    uint16_t getBCD4_be(unsigned offset) const;
    /** Stores a 4-digit (2-byte/16bit) BDC value in big-endian at the given byte-offset. */
    void setBCD4_be(unsigned offset, uint16_t value);
    /** Reads a 4-digit (2-byte/16bit) BDC value in little-endian at the given byte-offset. */
    uint16_t getBCD4_le(unsigned offset) const;
    /** Stores a 4-digit (1-byte/16bit) BDC value in little-endian at the given byte-offset. */
    void setBCD4_le(unsigned offset, uint16_t value);

    /** Reads a 8-digit (4-byte/32bit) BDC value in big-endian at the given byte-offset. */
    uint32_t getBCD8_be(unsigned offset) const;
    /** Stores a 8-digit (4-byte/32bit) BDC value in big-endian at the given byte-offset. */
    void setBCD8_be(unsigned offset, uint32_t value);
    /** Reads a 8-digit (4-byte/32bit) BDC value in little-endian at the given byte-offset. */
    uint32_t getBCD8_le(unsigned offset) const;
    /** Stores a 8-digit (4-byte/32bit) BDC value in little-endian at the given byte-offset. */
    void setBCD8_le(unsigned offset, uint32_t value);

    /** Reads up to @c maxlen ASCII chars at the given byte-offset using @c eos as the string termination char. */
    QString readASCII(unsigned offset, unsigned maxlen, uint8_t eos=0x00) const;
    /** Stores up to @c maxlen ASCII chars at the given byte-offset using @c eos as the string termination char.
     * The stored string gets padded with @c eos to @c maxlen. */
    void writeASCII(unsigned offset, const QString &txt, unsigned maxlen, uint8_t eos=0x00);

    /** Reads up to @c maxlen unicode chars at the given byte-offset using @c eos as the string termination char. */
    QString readUnicode(unsigned offset, unsigned maxlen, uint16_t eos=0x0000) const;
    /** Stores up to @c maxlen unicode chars at the given byte-offset using @c eos as the string termination char.
     * The stored string gets padded with @c eos to @c maxlen. */
    void writeUnicode(unsigned offset, const QString &txt, unsigned maxlen, uint16_t eos=0x0000);

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
    explicit Context(Config *config);

    /** Returns the reference to the config object. */
    Config *config() const;

    /** Resolves the given index for the specifies element type.
     * @returns @c nullptr if the index is not defined or the type is unknown. */
    ConfigItem *obj(const QMetaObject *elementType, unsigned idx);
    /** Returns the index for the given object.
     * @returns -1 if no index is associated with the object or its type is unknown. */
    int index(ConfigItem *obj);
    /** Associates the given object with the given index. */
    bool add(ConfigItem *obj, unsigned idx);

    /** Adds a table for the given type. */
    bool addTable(const QMetaObject *obj);

    /** Returns the object associated by the given index and type. */
    template <class T>
    T* get(unsigned idx) {
      return this->obj(&(T::staticMetaObject), idx)->template as<T>();
    }

    /** Returns @c true, if the given index is defined for the specified type. */
    template <class T>
    bool has(unsigned idx) {
      return nullptr != this->obj(&(T::staticMetaObject), idx)->template as<T>();
    }

  protected:
    /** Internal used table type to associate objects and indices. */
    class Table {
    public:
      /** The index->object map. */
      QHash<unsigned, ConfigItem *> objects;
      /** The object->index map. */
      QHash<ConfigItem *, unsigned> indices;
    };

  protected:
    /** Returns @c true if a table is defined for the given type. */
    bool hasTable(const QMetaObject *obj) const;
    /** Returns a reference to the table for the given type. */
    Table &getTable(const QMetaObject *obj);

  protected:
    /** A weak reference to the config object. */
    Config *_config;
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
  virtual bool index(Config *config, Context &ctx, const ErrorStack &err=ErrorStack()) const = 0;

  /** Decodes a binary codeplug to the given abstract configuration @c config.
   * This must be implemented by the device-specific codeplug. */
  virtual bool decode(Config *config, const ErrorStack &err=ErrorStack()) = 0;
  /** Encodes a given abstract configuration (@c config) to the device specific binary code-plug.
   * This must be implemented by the device-specific codeplug. */
  virtual bool encode(Config *config, const Flags &flags=Flags(), const ErrorStack &err=ErrorStack()) = 0;
};

#endif // CODEPLUG_HH
