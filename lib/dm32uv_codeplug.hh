#ifndef DM32UV_CODEPLUG_HH
#define DM32UV_CODEPLUG_HH

#include "codeplug.hh"
#include <QObject>
#include <QString>
#include "frequency.hh"
#include "channel.hh"
#include "contact.hh"

#include <codeplug.hh>
#include <ranges.hh>

// forward declaration
class Zone;
class SMSTemplate;



/** Implementation of the binary codeplug for the Baofeng DM32UV.
 * @ingroup dm32uv */
class DM32UVCodeplug : public Codeplug
{
  Q_OBJECT

public:
  /** Implements encoding channel for the binary codeplug. */
  class ChannelElement: public Element
  {
  public:
    /** Possible channel types. */
    enum class ChannelType {
      FM = 0, DMR = 1, FMFixed = 2, DMRFixed = 3
    };

    /** Possible power settings. */
    enum class Power {
      Low = 0, Medium = 1, High = 2
    };

    /** Possible admit criteria. */
    enum class Admit {
      Always = 0, ChannelFree = 1, ToneOrCCMatch = 2, ToneMismatch = 3
    };

  protected:
    /** Hidden constructor. */
    ChannelElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    ChannelElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x0030; }

    /** Returns the channel name. */
    virtual QString name() const;
    /** Sets the channel name. */
    virtual void setName(const QString &name);

    /** Returns the RX frequency. */
    virtual Frequency rxFrequency() const;
    /** Set RX frequency. */
    virtual void setRXFrequency(const Frequency &freq);

    /** Returns @c true, if a valid TX frequency is stored. */
    virtual bool validTXFrequency() const;
    /** Returns the TX frequency. */
    virtual Frequency txFrequency() const;
    /** Set TX frequency. */
    virtual void setTXFrequency(const Frequency &freq);
    /** Invalidates the TX frequency. */
    virtual void clearTXFrequency();

    /** Returns the channel type. */
    virtual ChannelType channelType() const;
    /** Sets the channel type. */
    virtual void setChannelType(ChannelType type);

    /** Returns the channel power setting. */
    virtual Channel::Power power() const;
    /** Sets the channel power. */
    virtual void setPower(Channel::Power power);

    /** Returns @c true, if the lone-worker feature is enabled. */
    virtual bool loneWorkerEnabled() const;
    /** Enables/disables lone worker feature. */
    virtual void enableLoneWorker(bool enable);

    /** Returns the bandwidth for FM channels. */
    virtual FMChannel::Bandwidth bandwidth() const;
    /** Sets the FM channel bandwidth. */
    virtual void setBandwidth(FMChannel::Bandwidth bw);

    /** Returns @c true, if the scan list index is set. */
    virtual bool validScanListIndex() const;
    /** Returns the 0-based scan list index. */
    virtual unsigned int scanListIndex() const;
    /** Sets the scan list index. */
    virtual void setScanListIndex(unsigned int idx);
    /** Invalidates the scan list index. */
    virtual void clearScanListIndex();

    /** Returns @c true if talkaround cannot be enabled on this channel. */
    virtual bool preventTalkaroundEnabled() const;
    /** Enables/disables prevention of talkaround feature on this channel. */
    virtual void enablePreventTalkaround(bool enable);

    /** Returns the admit criterion. */
    virtual Admit admitCriterion() const;
    /** Sets the admit criterion. */
    virtual void setAdmitCriterion(Admit admit);

    /** Returns @c true if DMR APRS reception is enabled. */
    virtual bool rxDMRAPRSEnabled() const;
    /** Enables/disables DMR APRS reception. */
    virtual void enableRXDMRAPRS(bool enable);

    /** Retruns @c true, if the emergency notification is enabled. */
    virtual bool emergencyNotificationEnabled() const;
    /** Enables/disables emergency notification. */
    virtual void enableEmergencyNotification(bool enable);
    /** Retruns @c true, if the emergency ACK is enabled. */
    virtual bool emergencyACKEnabled() const;
    /** Enables/disables emergency notification. */
    virtual void enableEmergencyACK(bool enable);
    /** Returns @c true if the emergency system index is set. */
    virtual bool validEmergencySystemIndex() const;
    /** Returns the emergency system index. */
    virtual unsigned int emergencySystemIndex() const;
    /** Sets the emergency system index. */
    virtual void setEmergencySystemIndex(unsigned int idx);
    /** Invalidates the emergency system index. */
    virtual void clearEmergencySystemIndex();

    /** Returns FM and DMR squelch level. */
    virtual unsigned int squelchLevel() const;
    /** Sets the FM and DMR squelch level. */
    virtual void setSquelchLevel(unsigned int level);

    /** Returns @c true if the channel is RX only. */
    virtual bool rxOnlyEnabled() const;
    /** Enables RX only for the channel. */
    virtual void enableRXOnly(bool enable);

    /** Returns @c true if DMR APRS is enabled. */
    virtual bool dmrAPRSEnabled() const;
    /** Enables DMR APRS for the channel. */
    virtual void enableDMRAPRS(bool enable);

    /** Return @c true if private calls are confirmed. */
    virtual bool privateCallACKEnabled() const;
    /** Enables confirmation of private calls. */
    virtual void enablePrivateCallACK(bool enable);
    /** Return @c true if data is confirmed. */
    virtual bool dataACKEnabled() const;
    /** Enables confirmation of data. */
    virtual void enableDataACK(bool enable);
    /** Return @c true if DCDM (dual capacity direct mode) is enabled. */
    virtual bool dcdmEnabled() const;
    /** Enables DCDM (dual capacity direct mode).. */
    virtual void enableDCDM(bool enable);

    /** Returns the timeslot of the channel. */
    virtual DMRChannel::TimeSlot timeslot() const;
    /** Sets the timeslot of the channel. */
    virtual void setTimeslot(DMRChannel::TimeSlot ts);
    /** Returns the color code of the channel. */
    virtual unsigned int colorCode() const;
    /** Sets the color code of the channel. */
    virtual void setColorCode(unsigned int cc);

    /** Returns @c true if encryption is enabled. */
    virtual bool encryptionEnabled() const;
    /* Enables encryption. */
    virtual void enableEncryption(bool enable);

    /** Returns @c true if group list index is valid. */
    virtual bool validGroupListIndex() const;
    /** Get group list index. */
    virtual unsigned int groupListIndex() const;
    /** Set group list index. */
    virtual void setGroupListIndex(unsigned int idx);
    /** Invalidates group list index. */
    virtual void clearGroupListIndex();

    /** Returns DMR APRS report channel index. */
    virtual unsigned int dmrAPRSChannelIndex() const;
    /** Sets the DMR APRS report channel index. */
    virtual void setDMRAPRSChannelIndex(unsigned int idx);

    /** Returns the RX tone. */
    virtual SelectiveCall rxTone() const;
    /** Sets the RX tone. */
    virtual void setRXTone(const SelectiveCall &tone);
    /** Returns the TX tone. */
    virtual SelectiveCall txTone() const;
    /** Sets the TX tone. */
    virtual void setTXTone(const SelectiveCall &tone);

    /** Returns @c true, if the VOX is enabled. */
    virtual bool voxEnabled() const;
    /** Enables the the VOX. */
    virtual void enableVOX(bool enable);

    /** Returns the DMR radio id index. */
    virtual unsigned int dmrIdIndex() const;
    /** Sets the dmr ID index. */
    virtual void setDMRIdIndex(unsigned int id);

    /** Constructs a channel object. */
    virtual Channel *decode(Context &ctx, const ErrorStack &err=ErrorStack()) const;
    /** Links the channel object. */
    virtual bool link(Channel *channel, Context &ctx, const ErrorStack &err=ErrorStack()) const;

  public:
    /** Some limits. */
    struct Limit: Element::Limit {
      /** Maximum name length. */
      static constexpr unsigned int nameLength() { return 16; }
      /** Maximum squelch level. */
      static constexpr unsigned int squelchLevel() { return 15; }
    };

  protected:
    /** Helper function to decode selective call. */
    static SelectiveCall decodeSelectiveCall(uint16_t code);
    /** Helper function to encode selective call. */
    static uint16_t encodeSelectiveCall(const SelectiveCall &tone);

  protected:
    /** Some internal offsets. */
    struct Offset: Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int name()        { return 0x0000; }
      static constexpr unsigned int rxFrequency() { return 0x0010; }
      static constexpr unsigned int txFrequency() { return 0x0014; }
      static constexpr Bit channelType()          { return {0x0018, 4}; }
      static constexpr Bit power()                { return {0x0018, 1}; }
      static constexpr Bit loneWorker()           { return {0x0018, 0}; }
      static constexpr Bit bandwidth()            { return {0x0019, 7}; }
      static constexpr Bit scanListIndex()        { return {0x0019, 2}; }
      static constexpr Bit preventTalkaround()    { return {0x001a, 7}; }
      static constexpr Bit admitCriterion()       { return {0x001a, 4}; }
      static constexpr Bit rxDMRAPRS()            { return {0x001a, 2}; }
      static constexpr Bit emergencyNotification() { return {0x001b, 7}; }
      static constexpr Bit emergencyACK()         { return {0x001b, 6}; }
      static constexpr Bit emergencySystemIndex() { return {0x001b, 0}; }
      static constexpr Bit squelchLevel()         { return {0x001c, 4}; }
      static constexpr Bit rxOnly()               { return {0x001c, 3}; }
      static constexpr Bit dmrAPRS()              { return {0x001c, 2}; }
      static constexpr Bit privateCallACK()       { return {0x001d, 7}; }
      static constexpr Bit dataACK()              { return {0x001d, 6}; }
      static constexpr Bit dcdm()                 { return {0x001d, 5}; }
      static constexpr Bit timeslot()             { return {0x001d, 4}; }
      static constexpr Bit colorcode()            { return {0x001d, 0}; }
      static constexpr Bit encryptionEnable()     { return {0x001f, 6}; }
      static constexpr Bit groupListIndex()       { return {0x001f, 0}; }
      static constexpr Bit dmrAPRSChannelIndex()  { return {0x0020, 0}; }
      static constexpr unsigned int rxTone()      { return 0x0021; }
      static constexpr unsigned int txTone()      { return 0x0023; }
      static constexpr Bit vox()                  { return {0x0025, 4}; }
      static constexpr Bit showPTTId()            { return {0x0026, 7}; }
      static constexpr Bit optSigEnable()         { return {0x0026, 4}; }
      static constexpr Bit optSigType()           { return {0x0026, 0}; }
      static constexpr Bit pttIdEnable()          { return {0x0029, 2}; }
      static constexpr unsigned int dmrIdIndex()  { return 0x002b; }
      /// @endcond
    };
  };


  /** Channel bank element. */
  class ChannelBankElement: public Element
  {
  public:
    /** Constructor. */
    ChannelBankElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x0010; }

    /** Returns the channel count. */
    virtual unsigned int channelCount() const;
    /** Sets the channel count. */
    virtual void setChannelCount(unsigned int n);

  public:
    /** Some limits. */
    struct Limit: Element::Limit {
      /** Maximum number of channels. */
      static constexpr unsigned int channels() { return 4000; }
      /** Maximum number of channels per block. */
      static constexpr unsigned int channelsPerBlock() { return 85; }
    };

    /** Some internal offsets. */
    struct Offset: Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int channelCount() { return 0x0000; }
      static constexpr unsigned int channelBlock0() { return 0x0010; }
      static constexpr unsigned int betweenChannelBlocks() { return 0x1000; }
      /// @endcond
    };
  };


  /** Implements a single DMR contact. */
  class ContactElement: public Element
  {
  public:
    /** Possible contact types. */
    enum class Type {
      Private = 3, Group = 4, All = 5
    };

  protected:
    /** Hidden constructor. */
    ContactElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    ContactElement(uint8_t *ptr);

    /** Size of the element. */
    static constexpr unsigned int size() { return 0x0018; }

    /** Returns the name of the contact. */
    virtual QString name() const;
    /** Sets the name of the contact. */
    virtual void setName(const QString &name);

    /** Returns the call type. */
    virtual DMRContact::Type callType() const;
    /** Sets the call type. */
    virtual void setCallType(DMRContact::Type type);

    /** Returns the DMR Id. */
    virtual unsigned int dmrId() const;
    /** Sets the DMR Id. */
    virtual void setDMRId(unsigned int id);

    /** Constructs a contact object. */
    virtual DMRContact *decode(Context &ctx, const ErrorStack &err=ErrorStack()) const;

  public:
    /** Some limits. */
    struct Limit: Element::Limit {
      /** Maximum name length. */
      static constexpr unsigned int nameLength() { return 16; }
    };

  protected:
    /** Some internal offsets. */
    struct Offset: Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int name()        { return 0x0002; }
      static constexpr unsigned int dmrId()       { return 0x0013; }
      static constexpr unsigned int callType()    { return 0x0016; }
    };
  };


  /** Implements the contact bank. */
  class ContactBankElement: public Element
  {
  public:
    /** Some limts for the contact bank. */
    struct Limit: Element::Limit {
      /** Maximum number of contacts. */
      static constexpr unsigned int contacts() { return 800; }
      static constexpr unsigned int contactsPerBlock() { return 170; }
    };

    /** Some internal offsets. */
    struct Offset: Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int betweenBlocks() { return 0x1000; };
      /// @endcond
    };
  };


  /** Contact index block. */
  class ContactIndexElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ContactIndexElement(uint8_t *ptr, size_t size);

  public:
    /** Contact bitmap element. */
    class ContactBitmapElement: public InvertedBitmapElement
    {
    public:
      /** Constructor. */
      ContactBitmapElement(uint8_t *ptr);
      /** Returns the size of the bitmap. */
      static constexpr unsigned int size() { return 0x0064; }
    };

    /** Contact index entry. */
    class EntryElement: public Element
    {
    public:
      typedef ContactElement::Type Type;

    protected:
      /** Hidden constructor. */
      EntryElement(uint8_t *ptr, size_t size);

    public:
      /** Constructor. */
      EntryElement(uint8_t *ptr);

      /** Returns the size of the element. */
      static constexpr unsigned int size() { return 0x0002; }

      /** Returns the call type. */
      virtual DMRContact::Type callType() const;
      /** Sets the call type. */
      virtual void setCallType(DMRContact::Type type);

    protected:
      /** Some internal offsets. */
      struct Offset: Element::Offset {
        /// @cond DO_NOT_DOCUMENT
        static constexpr Bit callType() { return {0x0001, 4}; }
        /// @endcond DO_NOT_DOCUMENT
      };
    };

  public:
    /** Constructor. */
    ContactIndexElement(uint8_t *ptr);

    /** Size of this element. */
    static constexpr unsigned int size() { return 0x1000; }

    /** Returns the number of contacts encoded. */
    virtual unsigned int contactCount() const;
    /** Sets the number of contacts. */
    virtual void setContactCount(unsigned int n);
    /** Returns the number of group call contacts encoded. */
    virtual unsigned int groupCallCount() const;
    /** Sets the number of group call contacts. */
    virtual void setGroupCallCount(unsigned int n);
    /** Returns the number of private call contacts encoded. */
    virtual unsigned int privateCallCount() const;
    /** Sets the number of private call contacts. */
    virtual void setPrivateCallCount(unsigned int n);

    /** Returns the contact bitmap. */
    virtual ContactBitmapElement bitmap() const;

    /** Returns the n-th index entry. */
    virtual EntryElement indexEntry(unsigned n);
    /** Returns the n-th sorted index entry. */
    virtual EntryElement sortedIndexEntry(unsigned n);

  protected:
    /** Some internal offsets. */
    struct Offset: Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static unsigned int contactCount()    { return 0x0000; }
      static unsigned int groupCount()      { return 0x0002; }
      static unsigned int privateCount()    { return 0x0004; }
      static unsigned int bitmap()          { return 0x0010; }
      static unsigned int index()           { return 0x0100; }
      static unsigned int sorted()          { return 0x0740; }
      /// @endcond
    };
  };


  /** Implements the group list bank element. */
  class GroupListElement: public Element
  {
  protected:
    /** Hidden constructor. */
    GroupListElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    GroupListElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x006d; }

    /** Returns the name of the list. */
    virtual QString name() const;
    /** Sets the name of the list. */
    virtual void setName(const QString &name);

    /** Retruns @c true if the n-th ID is set. */
    virtual bool validId(unsigned int n);
    /** Returns the n-th ID. */
    virtual unsigned int id(unsigned int n);
    /** Sets the n-th ID. */
    virtual void setId(unsigned int n, unsigned int id);
    /** Clears the n-th ID. */
    virtual void clearId(unsigned int n);

    /** Decodes this group list. */
    virtual RXGroupList *decode(Context &ctx, const ErrorStack &err=ErrorStack());
    /** Links this group list. */
    virtual bool link(RXGroupList *gl, Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits. */
    struct Limit: Element::Limit {
      /** Maximum name length. */
      static constexpr unsigned int nameLength() { return 11; }
      /** Maximum number of indices. */
      static constexpr unsigned int contacts() { return 32; }
    };

  protected:
    /** Some internal offsets. */
    struct Offset: Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int name() { return 0x0000; }
      static constexpr unsigned int ids() { return 0x000b; }
      static constexpr unsigned int betweenIds() { return 0x0003; }
      /// @endcond
    };
  };


  /** Implements the group list bank element. */
  class GroupListBankElement: public Element
  {
  public:
    /** Bitmap for all group lists. */
    class GroupListBitmapElement : public BitmapElement
    {
    public:
      /** Constructor. */
      GroupListBitmapElement(uint8_t *ptr);
    };

  protected:
    /** Hidden constructor. */
    GroupListBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    GroupListBankElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x1000; }

    /** Retunrs the bitmap. */
    virtual GroupListBitmapElement bitmap() const;

    /** Returns the n-th group list element. */
    virtual GroupListElement groupList(unsigned int n) const;

    /** Decodes all group lists. */
    virtual bool decode(Context &ctx, const ErrorStack &err=ErrorStack());
    /** Links all group lists. */
    virtual bool link(Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits. */
    struct Limit: Element::Limit {
      /** Maximum number of group lists. */
      static constexpr unsigned int groupLists() { return 32; }
    };

  protected:
    /** Some internal offsets. */
    struct Offset: Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int bitmap()     { return 0x0000; }
      static constexpr unsigned int groupLists() { return 0x0011; }
      /// @endcond
    };
  };


  /** Implements a DMR radio ID. */
  class RadioIdElement: public Element
  {
  public:
    /** Contsturctor. */
    RadioIdElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x0010; }

    /** Returns the DMR Id. */
    virtual unsigned int id() const;
    /** Sets the DMR id. */
    virtual void setId(unsigned int id);

    /** Returns the name of the id. */
    virtual QString name() const;
    /** Sets the name of the id. */
    virtual void setName(const QString &name);

    /** Decodes the radio ID. */
    virtual DMRRadioID *decode(Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits. */
    struct Limit: Element::Limit {
      /** Maximum name length. */
      static constexpr unsigned int nameLength() { return 12; }
    };

  protected:
    /** Some internal offsets. */
    struct Offset: Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int id() { return 0x0000; }
      static constexpr unsigned int name() { return 0x0003; }
      /// @endcond
    };
  };


  /** Implements the DMR radio ID bank. */
  class RadioIdBankElement: public Element
  {
  public:
    /** Contsturctor. */
    RadioIdBankElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x1000; }

    /** Returns the number of IDs. */
    virtual unsigned int idCount() const;
    /** Sets the number of encoded IDs. */
    virtual void setIdCount(unsigned int n);

    /** Returns the n-th DMR Id. */
    virtual RadioIdElement id(unsigned int n) const;

    /** Decodes all radio IDs. */
    virtual bool decode(Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits. */
    struct Limit: Element::Limit {
      /** Maximum number of IDs. */
      static constexpr unsigned int ids() { return 250; }
    };

  protected:
    /** Some internal offsets. */
    struct Offset: Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int count()      { return 0x0000; }
      static constexpr unsigned int ids()        { return 0x0010; }
      static constexpr unsigned int betweenIds() { return RadioIdElement::size(); }
      /// @endcond
    };
  };


  /** Implementation of zone. */
  class ZoneElement: public Element
  {
  public:
    /** Constructor. */
    ZoneElement(uint8_t *ptr);

    /** Size of the element. */
    static constexpr unsigned int size() { return 0x0091; }

    /** Returns the name. */
    virtual QString name() const;
    /** Sets the name of the zone. */
    virtual void setName(const QString &name);

    /** Retunrs the number of channels. */
    virtual unsigned int channelCount() const;
    /** Sets the number of channels. */
    virtual void setChannelCount(unsigned int n);

    /** Returns @c true, if the channel index is set. */
    virtual bool channelIndexValid(unsigned int n) const;
    /** Returns the n-th channel index. */
    virtual unsigned int channelIndex(unsigned int n) const;
    /** Sets the n-th channel index. */
    virtual void setChannelIndex(unsigned int n, unsigned int idx);
    /** Clears the n-th channel index. */
    virtual void clearChannelIndex(unsigned int n);

    /** Decode zone. */
    virtual Zone *decode(Context &ctx, const ErrorStack &err=ErrorStack());
    /** Link zone. */
    virtual bool link(Zone *zone, Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits. */
    struct Limit: Element::Limit {
      /** Maximum name length. */
      static constexpr unsigned int nameLength() { return 16; }
      /** Maximum number of channels. */
      static constexpr unsigned int channels()   { return 64; }
    };

  protected:
    /** Some internal offsets. */
    struct Offset: Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int name()         { return 0x0000; }
      static constexpr unsigned int channelCount() { return 0x0010; }
      static constexpr unsigned int channels()     { return 0x0011; }
      /// @endcond
    };
  };


  class ZoneBankElement: public Element
  {
  public:
    /** Constructor. */
    ZoneBankElement(uint8_t *ptr);

    /** Returns the size of a zone bank. */
    static constexpr unsigned int size() { return 0x1000; }

    /** Returns the total number of zones. */
    virtual unsigned int count() const;
    /** Sets the total number of zones. */
    virtual void setCount(unsigned int n);

    /** Returns @c true if the channel index VFO A is set. */
    virtual bool channelIndexAValid() const;
    /** Returns the channel index VFO A. */
    virtual unsigned int channelIndexA() const;
    /** Sets the channel index VFO A. */
    virtual void setChannelIndexA(unsigned int idx);
    /** Clears the channel index VFO A. */
    virtual void clearChannelIndexA();

    /** Returns @c true if the channel index VFO B is set. */
    virtual bool channelIndexBValid() const;
    /** Returns the channel index VFO B. */
    virtual unsigned int channelIndexB() const;
    /** Sets the channel index VFO B. */
    virtual void setChannelIndexB(unsigned int idx);
    /** Clears the channel index VFO B. */
    virtual void clearChannelIndexB();

    /** Returns @c true if the zone index VFO A is set. */
    virtual bool zoneIndexAValid() const;
    /** Returns the zone index VFO A. */
    virtual unsigned int zoneIndexA() const;
    /** Sets the zone index VFO A. */
    virtual void setZoneIndexA(unsigned int idx);
    /** Clears the zone index VFO A. */
    virtual void clearZoneIndexA();

    /** Returns @c true if the zone index VFO B is set. */
    virtual bool zoneIndexBValid() const;
    /** Returns the zone index VFO B. */
    virtual unsigned int zoneIndexB() const;
    /** Sets the zone index VFO B. */
    virtual void setZoneIndexB(unsigned int idx);
    /** Clears the zone index VFO B. */
    virtual void clearZoneIndexB();

  public:
    /** Some limits. */
    struct Limit: Element::Limit {
      /** Maximum number of zones. */
      static constexpr unsigned int zones()   { return 250; }
      /** Maximum number of zones per block. */
      static constexpr unsigned int zonesPerBlock() { return 28; }

    };

    /** Some internal offsets. */
    struct Offset: Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int count()         { return 0x0000; }
      static constexpr unsigned int channelIndexA() { return 0x0001; }
      static constexpr unsigned int channelIndexB() { return 0x0003; }
      static constexpr unsigned int zoneIndexA()    { return 0x0005; }
      static constexpr unsigned int zoneIndexB()    { return 0x0007; }
      static constexpr unsigned int zones0()        { return 0x0010; }
      static constexpr unsigned int betweenBlocks() { return 0x1000; }
      /// @endcond
    };
  };


  /** Implements a scan list. */
  class ScanListElement: public Element
  {
  public:
    /** Possible transmit modes. */
    enum class TransmitMode {
      CurrentChannel=0, ActiveChannel=1, RevertChannel=2
    };

    /** Possible CTCSS/DCS detection modes. */
    enum class ToneDetectionMode {
      None = 0, NonPriority = 1, Priority = 2, All = 3
    };

  public:
    /** Constructor. */
    ScanListElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x0039; }

    /** Returns the name of the scan list. */
    virtual QString name() const;
    /** Sets the name of the scan list. */
    virtual void setName(const QString &name);

    /** Returns the number of channels. */
    virtual unsigned int channelCount() const;
    /** Sets the channel count. */
    virtual void setChannelCount(unsigned int count);

    /** Returns the transmit mode. */
    virtual TransmitMode transmitMode() const;
    /** Sets the transmit mode. */
    virtual void setTransmitMode(TransmitMode mode);

    /** Returns the tone-detection mode. */
    virtual ToneDetectionMode toneDetectionMode() const;
    /** Sets the tone-detection mode. */
    virtual void setToneDetectionMode(ToneDetectionMode mode);

    /** Returns the scan hang time. */
    virtual Interval hangTime() const;
    /** Sets the scan hang time. */
    virtual void setHangTime(const Interval &dur);

    /** Returns @c true if the secondary priority channel index is set. */
    virtual bool secondaryChannelIndexValid() const;
    /** Returns the secondary priority channel index. */
    virtual unsigned int secondaryChannelIndex() const;
    /** Sets the secondary priority channel index. */
    virtual void setSecondaryChannelIndex(unsigned int idx);
    /** Clears the secondary priority channel index. */
    virtual void clearSecondaryChannelIndex();

    /** Returns @c true if the primary priority channel index is set. */
    virtual bool primaryChannelIndexValid() const;
    /** Returns the primary priority channel index. */
    virtual unsigned int primaryChannelIndex() const;
    /** Sets the primary priority channel index. */
    virtual void setPrimaryChannelIndex(unsigned int idx);
    /** Clears the primary priority channel index. */
    virtual void clearPrimaryChannelIndex();

    /** Returns @c true if the revert channel index is set. */
    virtual bool revertChannelIndexValid() const;
    /** Returns the revert channel index. */
    virtual unsigned int revertChannelIndex() const;
    /** Sets the revert channel index. */
    virtual void setRevertChannelIndex(unsigned int idx);
    /** Clears the revert channel index. */
    virtual void clearRevertChannelIndex();

    /** Returns the priority sweep time. */
    virtual Interval prioritySweepTime() const;
    /** Sets the priority sweep time. */
    virtual void setPrioritySweepTime(const Interval &dur);

    /** Returns @c true, if the n-th channel index refers to the current channel. */
    virtual bool isCurrentChannel(unsigned int n) const;
    /** Returns the n-th channel index. */
    virtual unsigned int channelIndex(unsigned int n) const;
    /** Sets the n-th channel index. */
    virtual void setChannelIndex(unsigned int n, unsigned int idx);
    /** Sets the n-th channel index to the current channel. */
    virtual void setCurrentChannel(unsigned int n);

    /** Decodes the scan list. */
    virtual ScanList *decode(Context &ctx, const ErrorStack &err=ErrorStack());
    /** Links the given scan list. */
    virtual bool link(ScanList *lst, Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits. */
    struct Limit: Element::Limit {
      /** Maximum name length. */
      static constexpr unsigned int nameLength() { return 11; }
      /** Maximum number of channels. */
      static constexpr unsigned int channels()   { return 15; }
    };

  protected:
    /** Some internal offsets. */
    struct Offset: Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int name()            { return 0x0000; }
      static constexpr unsigned int channelCount()    { return 0x000b; }
      static constexpr Bit transmitMode()             { return {0x000c, 4}; }
      static constexpr Bit toneDetection()            { return {0x000c, 0}; }
      static constexpr Bit hangTime()                 { return {0x000d, 0}; }
      static constexpr Bit secondaryChannel()         { return {0x000e, 4}; }
      static constexpr Bit primaryChannel()           { return {0x000e, 0}; }
      static constexpr unsigned int revertChannel()   { return 0x000f; }
      static constexpr Bit prioritySweepTime()        { return {0x0015, 2}; }
      static constexpr unsigned int channels()        { return 0x0018; }
      /// @endcond
    };
  };


  /** Implements the scan list bank. */
  class ScanListBankElement: public Element
  {
  public:
    /** Possible scan modes. */
    enum class ScanMode {
      Time = 0 , Carrier = 1, Search = 2
    };

  public:
    /* Constructor from pointer to the element. */
    ScanListBankElement(uint8_t *ptr);

    /** Size of the element. */
    static constexpr unsigned int size() { return 0x1000; }

    /** Returns the number of scan lists. */
    virtual unsigned int count() const;
    /** Sets the number of scan lists. */
    virtual void setCount(unsigned int n);

    /** Returns the n-th scan list. */
    virtual ScanListElement scanList(unsigned int n) const;

    /** Returns the scan mode. */
    virtual ScanMode scanMode() const;
    /** Sets the scan mode. */
    virtual void setScanMode(ScanMode mode);

    /** Returns the VHF scan range. */
    virtual FrequencyRange vhfRange() const;
    /** Sets the VHF scan range. */
    virtual void setVHFRange(const FrequencyRange &range);

    /** Returns the UHF scan range. */
    virtual FrequencyRange uhfRange() const;
    /** Sets the UHF scan range. */
    virtual void setUHFRange(const FrequencyRange &range);

    /** Decode all scan lists. */
    virtual bool decode(Context &ctx, const ErrorStack &err=ErrorStack());
    /** Link all scan lists. */
    virtual bool link(Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits. */
    struct Limit: Element::Limit {
      /** Maximum number of zones. */
      static constexpr unsigned int scanLists()   { return 32; }
    };

    /** Some internal offsets. */
    struct Offset: Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int count()         { return 0x0000; }
      static constexpr unsigned int scanLists()     { return 0x0001; }
      static constexpr unsigned int scanMode()      { return 0x0e00; }
      static constexpr unsigned int vhfLower()      { return 0x0e01; }
      static constexpr unsigned int vhfUpper()      { return 0x0e03; }
      static constexpr unsigned int uhfLower()      { return 0x0e05; }
      static constexpr unsigned int uhfUpper()      { return 0x0e07; }
      /// @endcond
    };
  };


  /** A sinlge preset message. */
  class SMSTemplateElement: public Element
  {
  public:
    /** Constructor from pointer to element. */
    SMSTemplateElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x0081; }

    /** Returns the message text. */
    virtual QString message() const;
    /** Sets the message text. */
    virtual void setMessage(const QString &msg);

    /** Decodes a single message. */
    SMSTemplate *decode(Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits. */
    struct Limit: Element::Limit {
      /** Maximum number of zones. */
      static constexpr unsigned int messageLength()  { return 126; }
    };

  protected:
    /** Some internal offsets. */
    struct Offset: Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int length()         { return 0x0000; }
      static constexpr unsigned int message()        { return 0x0001; }
      /// @endcond
    };
  };


  /** Preset message (SMS) bank. */
  class SMSTemplateBankElement: public Element
  {
  public:
    /** Constructor from pointer. */
    SMSTemplateBankElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x1000; }

    /** Returns the number of messages. */
    virtual unsigned int count() const;
    /** Sets the number of messages. */
    virtual unsigned int setCount(unsigned int n);

    /** Returns the n-th message. */
    virtual SMSTemplateElement message(unsigned int n) const;

    /** Decodes all preset SMS messages. */
    virtual bool decode(Context &ctx, const ErrorStack &err = ErrorStack());

  public:
    /** Some limits. */
    struct Limit: Element::Limit {
      /** Maximum number of zones. */
      static constexpr unsigned int messages()        { return 20; }
    };

  protected:
    /** Some internal offsets. */
    struct Offset: Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int count()           { return 0x0000; }
      static constexpr unsigned int messages()        { return 0x0010; }
      static constexpr unsigned int betweenMessages() { return SMSTemplateElement::size(); }
      /// @endcond
    };
  };

public:
  /** Default/empty contructor.
   * Before encoding, @c encode will allocate all elements necessary to encode the codeplug.
   * Before decoding, all elements that are allocated on the device must be allocated within the
   * codeplug. */
  explicit DM32UVCodeplug(QObject *parent = nullptr);

  Config * preprocess(Config *config, const ErrorStack &err) const override;
  bool postprocess(Config *config, const ErrorStack &err) const override;

  bool index(Config *config, Context &ctx, const ErrorStack &err=ErrorStack()) const override;
  bool encode(Config *config, const Flags &flags, const ErrorStack &err=ErrorStack()) override;
  bool decode(Config *config, const ErrorStack &err=ErrorStack()) override;

protected:
  /** Decode codeplug elements. */
  virtual bool decodeElements(Context &ctx, const ErrorStack &err=ErrorStack());
  /** Link decoded elements. */
  virtual bool linkElements(Context &ctx, const ErrorStack &err=ErrorStack());

  /** Decodes all channels defined. */
  virtual bool decodeChannels(Context &ctx, const ErrorStack &err=ErrorStack());
  /** Links all decoded channels. */
  virtual bool linkChannels(Context &ctx, const ErrorStack &err=ErrorStack());

  /** Decode contacts. */
  virtual bool decodeContacts(Context &ctx, const ErrorStack &err=ErrorStack());

  /** Decodes all zones defined. */
  virtual bool decodeZones(Context &ctx, const ErrorStack &err=ErrorStack());
  /** Links all decoded zones. */
  virtual bool linkZones(Context &ctx, const ErrorStack &err=ErrorStack());

protected:
  /** Some internal offsets. */
  struct Offset {
    /// @cond DO_NOT_DOCUMENT
    static constexpr unsigned int contactIndex()   { return 0x0000b000; }
    static constexpr unsigned int groupListBank()  { return 0x0000f000; }
    static constexpr unsigned int scanListBank()   { return 0x00011000; }
    static constexpr unsigned int channelBanks()   { return 0x00012000; }
    static constexpr unsigned int contactBanks()   { return 0x00044000; }
    static constexpr unsigned int zoneBanks()      { return 0x0005c000; }
    static constexpr unsigned int radioIdBank()    { return 0x00067000; }
    /// @endcond
  };
};

#endif // DM32UV_CODEPLUG_HH
