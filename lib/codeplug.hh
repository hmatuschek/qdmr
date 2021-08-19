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

  class Element
  {
  protected:
    Element(uint8_t *ptr=nullptr);

  public:
    Element(const Element &other);
    virtual ~Element();

    virtual bool isValid() const;
    virtual void clear();

    bool getBit(uint offset, uint bit) const;
    void setBit(uint offset, uint bit, bool value=true);
    void clearBit(uint offset, uint bit);

    uint8_t getUInt2(uint offset, uint bit) const;
    void setUInt2(uint offset, uint bit, uint8_t value);

    uint8_t getUInt3(uint offset, uint bit) const;
    void setUInt3(uint offset, uint bit, uint8_t value);

    uint8_t getUInt4(uint offset, uint bit) const;
    void setUInt4(uint offset, uint bit, uint8_t value);

    uint8_t getUInt6(uint offset, uint bit) const;
    void setUInt6(uint offset, uint bit, uint8_t value);

    uint8_t getUInt8(uint offset) const;
    void setUInt8(uint offset, uint8_t value);

    uint16_t getUInt16_be(uint offset) const;
    uint16_t getUInt16_le(uint offset) const;
    void setUInt16_be(uint offset, uint16_t value);
    void setUInt16_le(uint offset, uint16_t value);

    uint32_t getUInt32_be(uint offset) const;
    uint32_t getUInt32_le(uint offset) const;
    void setUInt32_be(uint offset, uint32_t value);
    void setUInt32_le(uint offset, uint32_t value);

    uint32_t getBCD8_be(uint offset) const;
    void setBCD8_be(uint offset, uint32_t value);
    uint32_t getBCD8_le(uint offset) const;
    void setBCD8_le(uint offset, uint32_t value);

    QString readUnicode(uint offset, uint maxlen, uint16_t eos) const;
    void writeUnicode(uint offset, const QString &txt, uint maxlen, uint16_t eos);

  protected:
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
