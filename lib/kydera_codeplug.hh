#ifndef KYDERACODEPLUG_HH
#define KYDERACODEPLUG_HH

#include "codeplug.hh"


/** Implements the base class for all Kydera based codeplugs. This includes the codeplug elements
 * like channels etc. The device specific codeplug then only implement the memory addresses and
 * differences to the base Kydera codeplug.
 *
 * @ingroup kydera */
class KyderaCodeplug: public Codeplug
{
  Q_OBJECT

public:
  /** Helper class to read/store addresses within the codeplug. */
  class AddressElement: public Codeplug::Element
  {
  public:
    /** Constuctor. */
    explicit AddressElement(uint8_t *ptr);

    /** Reads the absolute address. */
    unsigned address() const;
    /** Writes an absolute address. */
    void setAddress(unsigned addr);
  };

  /** Base class of all Kydera encoded channel elements.
   *
   * Memory layout of the channel element (size 0x0020 bytes):
   * @verbinclude kydera_channel.txt */
  class ChannelElement: public Codeplug::Element
  {
  public:
    /** Possible channel modes. */
    enum class Mode {
      Analog=0,              ///< Analog, i.e., FM.
      Digital=1,             ///< Digital, i.e., DMR.
      Analog_RX_Digital=2,   ///< Analog channel (TX/RX in FM) but additional RX in FM.
      Digital_RX_Analog=3    ///< Digital channel (TX/RX in DMR) but additional RX in DMR.
    };

  protected:
    /** Hidden constructor. */
    ChannelElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit ChannelElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~ChannelElement();

    /** Resets the channel. */
    void clear();

    /** Internal used index to reference this channel. */
    virtual unsigned index() const;
    /** Sets the internal index. */
    virtual void setIndex(unsigned idx);

    /** Returns the name of the channel. */
    virtual QString name() const;
    /** Sets the name of the channel. */
    virtual void setName(const QString &name);

    /** Returns the RX frequency of the channel in Hz. */
    virtual uint32_t rxFrequency() const;
    /** Sets the RX frequency of the channel in Hz. */
    virtual void setRXFrequency(uint32_t freq);
    /** Returns the TX frequency of the channel in Hz. */
    virtual uint32_t txFrequency() const;
    /** Sets the TX frequency of the channel in Hz. */
    virtual void setTXFrequency(uint32_t freq);

    /** Returns the channel mode. */
    virtual Mode mode() const;
    /** Sets the channel mode. */
    virtual void setMode(Mode mode);

    /** Returns the channel power setting. */
    virtual Channel::Power power() const;
    /** Sets the channel power. */
    virtual void setPower(Channel::Power power);

    /** Returns the analog channel bandwidth. */
    virtual AnalogChannel::Bandwidth bandwidth() const;
    /** Sets the analog channel bandwidth. */
    virtual void setBandwidth(AnalogChannel::Bandwidth bw);

  };

  /** Base class of all Kydera encoded contact elements.
   *
   * Memory layout of the contact element (size 0x0010 bytes):
   * @verbinclude kydera_contact.txt */
  class ContactElement: public Codeplug::Element
  {
  protected:
    /** Hidden constructor. */
    ContactElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    ContactElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~ContactElement();

    /** Resets the contact. */
    void clear();
  };

  /** Base class of all Kydera group list elements.
   *
   * Memory layout of the group list element (size 0x00d2 bytes):
   * @verbinclude kydera_grouplist.txt
   */
  class GroupListElement: public Codeplug::Element
  {
  protected:
    /** Hidden constructor. */
    GroupListElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    GroupListElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~GroupListElement();

    /** Resets the group list. */
    void clear();
  };

  /** Base class of all Kydera zone elements.
   *
   * Memory layout of the zone element (size 0x0020 bytes):
   * @verbinclude kydera_zone.txt
   */
  class ZoneElement: public Codeplug::Element
  {
  protected:
    /** Hidden constructor. */
    ZoneElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    ZoneElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~ZoneElement();

    /** Resets the zone. */
    void clear();
  };

  /** Base class of all Kydera scan-list elements.
   *
   * Memory layout of a scan-list element (size 0x00d8 bytes)
   * @verbinclude kydera_scanlist.txt */
  class ScanListElement: public Codeplug::Element
  {
  protected:
    /** Hidden constructor. */
    ScanListElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    ScanListElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~ScanListElement();

    /** Resets the scan list. */
    void clear();
  };

  /** Base class of all Kydera device info elements.
   *
   * Memory layout of a device-info element (size 0x0080 bytes)
   * @verbinclude kydera_devinfo.txt */
  class DeviceInfoElement: public Codeplug::Element
  {
  protected:
    /** Hidden constructor. */
    DeviceInfoElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    DeviceInfoElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~DeviceInfoElement();
  };

  /** Base class of all Kydera settings elements.
   *
   * Memory layout of a settings element (size 0x0050 bytes)
   * @verbinclude kydera_settings.txt */
  class SettingsElement: public Codeplug::Element
  {
    /** Hidden constructor. */
    SettingsElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    SettingsElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~SettingsElement();

    /** Resets the settings. */
    void clear();
  };

protected:
  /** Hidden constructor. Use one of the derived classes to instantiate a device specific
   * Kydera codeplug. */
  explicit KyderaCodeplug(QObject *parent=nullptr);

public:
  /** Clears and resets the complete codeplug to some default values. */
  virtual void clear() = 0;

  bool index(Config *config, Context &ctx, const ErrorStack &err=ErrorStack()) const;
};

#endif // KYDERACODEPLUG_HH
