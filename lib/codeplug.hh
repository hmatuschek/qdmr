#ifndef CODEPLUG_HH
#define CODEPLUG_HH

#include <QObject>
#include "dfufile.hh"
#include "userdatabase.hh"

class Config;

/** This class defines the interface all device-specific code-plugs must implement.
 * Device-specific codeplugs are derived from the common configuration and implement the
 * construction/parsing of the device specific binary configuration. */
class CodePlug: public DFUFile
{
	Q_OBJECT

public:
  /** Certain flags passed to CodePlug::encode to controll the transfer and encoding of the
   * code-plug. */
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
   * This class provies some helper methods to access specific members of the element. */
  class Element
  {
  protected:
    /** Hidden constructor.
     * @param ptr Specifies the pointer to the element within the codeplug. */
    Element(uint8_t *ptr=nullptr);

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

    /** Reads a 8-digit (4-byte/32bit) BDC value in big-endian at the given byte-offset. */
    uint32_t getBCD8_be(uint offset) const;
    /** Stores a 8-digit (4-byte/32bit) BDC value in big-endian at the given byte-offset. */
    void setBCD8_be(uint offset, uint32_t value);
    /** Reads a 8-digit (4-byte/32bit) BDC value in little-endian at the given byte-offset. */
    uint32_t getBCD8_le(uint offset) const;
    /** Stores a 8-digit (4-byte/32bit) BDC value in little-endian at the given byte-offset. */
    void setBCD8_le(uint offset, uint32_t value);

    /** Reads upto @c maxlen unicode chars at the given byte-offset using @c eos as the string termination char. */
    QString readUnicode(uint offset, uint maxlen, uint16_t eos=0x0000) const;
    /** Stores upto @c maxlen unicode chars at the given byte-offset using @c eos as the string termination char.
     * The stored string gets padded with @c eos to @c maxlen. */
    void writeUnicode(uint offset, const QString &txt, uint maxlen, uint16_t eos=0x0000);

  protected:
    /** Holds the pointer to the element. */
    uint8_t *_data;
  };

protected:
  /** Hidden default constructor. */
	explicit CodePlug(QObject *parent=nullptr);

public:
  /** Destructor. */
	virtual ~CodePlug();

  /** Decodes a binary codeplug to the given abstract configuration @c config.
   * This must be implemented by the device-specific codeplug. */
	virtual bool decode(Config *config) = 0;
  /** Encodes a given abstract configuration (@c config) to the device specific binary code-plug.
   * This must be implemented by the device-specific codeplug. */
  virtual bool encode(Config *config, const Flags &flags=Flags()) = 0;
};

#endif // CODEPLUG_HH
