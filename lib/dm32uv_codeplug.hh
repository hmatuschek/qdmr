#ifndef DM32UV_CODEPLUG_HH
#define DM32UV_CODEPLUG_HH

#include <QObject>
#include <QString>

#include "channel.hh"
#include "codeplug.hh"
#include "contact.hh"
#include "frequency.hh"
#include "roamingchannel.hh"
#include "codeplug.hh"
#include "ranges.hh"
#include  "smsextension.hh"


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
      /** Maximum number of scan lists. */
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


  /** Implements a roaming channel. */
  class RoamingChannelElement: public Element
  {
  public:
    /** Possible time-slot settings. */
    enum class TimeSlot {
      TS1 = 0, TS2 = 1
    };

  public:
    /** Constructor from pointer to element. */
    RoamingChannelElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x001a; }

    /** Returns the name of the channel. */
    virtual QString name() const;
    /** Sets the name of the channel. */
    virtual void setName(const QString &name);

    /** Returns the RX frequency. */
    virtual Frequency rxFrequency() const;
    /** Sets the RX frequency. */
    virtual void setRXFrequency(const Frequency &f);

    /** Returns the TX frequency. */
    virtual Frequency txFrequency() const;
    /** Sets the TX frequency. */
    virtual void setTXFrequency(const Frequency &f);

    /** Return the color code. */
    virtual unsigned int colorCode() const;
    /** Sets the color code. */
    virtual void setColorCode(unsigned int cc);

    /** Returns the time slot. */
    virtual DMRChannel::TimeSlot timeSlot() const;
    /** Sets the time slots. */
    virtual void setTimeSlot(DMRChannel::TimeSlot ts);

    /** Decodes the roaming channel.
     * @returns nullptr on error. */
    RoamingChannel *decode(Context &ctx, const ErrorStack &err=ErrorStack()) const;

  public:
    /** Some limits. */
    struct Limit: Element::Limit {
      /** Maximum name length. */
      static constexpr unsigned int nameLength()  { return 16; }
    };

  protected:
    /** Some internal offsets. */
    struct Offset: Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int name()               { return 0x0000; }
      static constexpr unsigned int rxFrequency()        { return 0x0010; }
      static constexpr unsigned int txFrequency()        { return 0x0014; }
      static constexpr unsigned int colorCode()          { return 0x0018; }
      static constexpr unsigned int timeSlot()           { return 0x0019; }
      /// @endcond
    };
  };


  /** The bank of all roaming channels. */
  class RoamingChannelBankElement: public Element
  {
  public:
    /** Constructor from pointer to element. */
    RoamingChannelBankElement(uint8_t *ptr);

    /** Returns the size of the elment. */
    static constexpr unsigned int size() { return 0x1000; }

    /** Returns the number of channels. */
    virtual unsigned int count() const;
    /** Sets the number of channels. */
    virtual void setCount(unsigned int n);

    /** Returns the n-th channel. */
    virtual RoamingChannelElement channel(unsigned int n);

    /** Decides all romaming channels. */
    virtual bool decode(Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits. */
    struct Limit: Element::Limit {
      /** Maximum number of channels. */
      static constexpr unsigned int channels()        { return 150; }
    };

  protected:
    /** Some internal offsets. */
    struct Offset: Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int channels()        { return 0x0000; }
      static constexpr unsigned int count()           { return 0x0ff0; }
      static constexpr unsigned int betweenChannels() { return RoamingChannelElement::size(); }
      /// @endcond
    };
  };


  /** En/Decodes a roaming zone. */
  class RoamingZoneElement: public Element
  {
  public:
    /** Constructor from pointer to element. */
    RoamingZoneElement(uint8_t *ptr);

    /** Returns the size of the elment. */
    static constexpr unsigned int size() { return 0x0021; }

    /** Returns the name of the zone. */
    virtual QString name() const;
    /** Sets the name. */
    virtual void setName(const QString &name);

    /** Returns the channel count. */
    virtual unsigned int count() const;
    /** Sets the number of channels. */
    virtual void setCount(unsigned int n);

    /** Returns @c true if the n-th channel index is set. */
    virtual bool channelIndexValid(unsigned int n);
    /** Returns the n-th channel index. */
    virtual unsigned int channelIndex(unsigned int n);
    /** Sets the n-th channel index. */
    virtual void setChannelIndex(unsigned int n, unsigned int idx);
    /** Clears the n-th channel index. */
    virtual void clearChannelIndex(unsigned int n);

    /** Decodes the roaming zone. */
    virtual RoamingZone *decode(Context &ctx, const ErrorStack &err=ErrorStack());
    /** Links the given roaming zone. */
    virtual bool link(RoamingZone *zone, Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits. */
    struct Limit: Element::Limit {
      /** Maximum name length. */
      static constexpr unsigned int nameLength()  { return 16; }
      /** Maximum number of channels per zone. */
      static constexpr unsigned int channels()    { return 16; }
    };

  protected:
    /** Some internal offsets. */
    struct Offset: Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int name()               { return 0x0000; }
      static constexpr unsigned int channelCount()       { return 0x0010; }
      static constexpr unsigned int channels()           { return 0x0011; }
      static constexpr unsigned int betweenChannels()    { return 0x0001; }
      /// @endcond
    };
  };


  /** En/decodes the roaming zone bank. */
  class RoamingZoneBankElement : public Element
  {
  public:
    /** Constructor from pointer to element. */
    RoamingZoneBankElement(uint8_t *ptr);

    /** Returns the size of the elment. */
    static constexpr unsigned int size() { return 0x1000; }

    /** Returns the zone count. */
    virtual unsigned int count() const;
    /** Sets the number of zones. */
    virtual void setCount(unsigned int n);

    /** Returns @c true if auto roaming is enabled. */
    virtual bool autoRoamEnabled() const;
    /** Enables auto roaming. */
    virtual void enableAutoRoam(bool enable);

    /** Returns the roaming delay. */
    virtual Interval roamingDelay() const;
    /** Sets the roaming delay. */
    virtual void setRoamingDelay(const Interval &delay);

    /** Returns @c true, if a default roaming zone is set. */
    virtual bool defaultRoamingZoneIndexValid() const;
    /** Returns the default roaming zone index. */
    virtual unsigned int defaultRoamingZoneIndex() const;
    /** Sets the default roaming zone index. */
    virtual void setDefaultRoamingZoneIndex(unsigned int idx);
    /** Clears the default roaming zone index. */
    virtual void clearDefaultRoamingZoneIndex();

    /** Returns the n-th roaming zone. */
    virtual RoamingZoneElement zone(unsigned int n);

    /** Decodes all roaming zones. */
    virtual bool decode(Context &ctx, const ErrorStack &err=ErrorStack());
    /** Links all roaming zones. */
    virtual bool link(Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits. */
    struct Limit: Element::Limit {
      /** Maximum number of zones. */
      static constexpr unsigned int zones()        { return 64; }
    };

  protected:
    /** Some internal offsets. */
    struct Offset: Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int count()              { return 0x0000; }
      static constexpr unsigned int autoRoam()           { return 0x0001; }
      static constexpr unsigned int roamingDelay()       { return 0x0002; }
      static constexpr unsigned int defaultRoamingZone() { return 0x0003; }
      static constexpr unsigned int zones()              { return 0x0010; }
      static constexpr unsigned int betweenZones()       { return RoamingZoneElement::size(); }
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
      /** Maximum message length. */
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
    virtual void setCount(unsigned int n);

    /** Returns the n-th message. */
    virtual SMSTemplateElement message(unsigned int n) const;

    /** Decodes all preset SMS messages. */
    virtual bool decode(Context &ctx, const ErrorStack &err = ErrorStack());

  public:
    /** Some limits. */
    struct Limit: Element::Limit {
      /** Maximum number of messages. */
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


  /** Common settings block. */
  class GeneralSettingsElement: public Element
  {
  public:
    /** Possible boot display settings. */
    enum class BootDisplay {
      Image=0, Message=1, Voltage=2
    };

    /** Possible auto-power-off delays. */
    enum class AutoPowerOffDelay {
      Off = 0, T30Min=1, T60Min=2, T2h=3, T4h=4, T8h=5
    };

    /** Possible FM roger tones. */
    enum class FMRogerTone {
      Off=0, Beep=1, BDC=2
    };

    /** Possible settings for the backlight duration. */
    enum class BacklightDuration {
      Infinity = 0, T5s=1, T10s=2, T15s=3, T20s=4, T25s=5, T30s=6, T1min=7, T2min=8, T3min=9,
      T4min=10, T5min=11
    };

    /** Possible date formats. */
    enum class DateFormat {
      YYYYMMDD = 0, DDMMYYYY = 1
    };

    /** Implements the translation between color names and code.
     * For now, this struct is futile but becomes necessary, once there is a common color name
     * scheme. */
    struct Color {
      enum class Code {
        White = 0, Black = 1, Orange = 2, Red = 3, Yellow = 4, Green = 5, Cyan = 6, Blue = 7
      };

      static unsigned int encode(Code name);
      static Code decode(unsigned int code);
    };

    /** Possible position formats. */
    enum class PositionFormat {
      DD = 0,   ///< Decimal degree
      DMS = 1   ///< Degree, minute, second
    };

    /** GNSS modes. */
    enum class GNSSMode {
      GPS = 0, Baidou = 1, Both = 0
    };

    /** Possible recording modes. */
    enum class RecordMode {
      RX = 0, TX = 1, Both = 2
    };

    /** Possible SMS formats. */
    enum class SMSFormat {
      Hytera = 0, Motorola = 1, DMR = 2
    };

    /** Talker alias formats. */
    enum class TalkerAliasFormat {
      ISO8 = 0, UnicodeU16 = 1
    };

    /** Talker alias sources. */
    enum class TalkerAliasSource {
      CallsignDB = 0, OverTheAir = 1
    };

    /** Possible dual-standby modes. */
    enum class DualStandbyMode {
      SingleVFO = 0, DoubleStandby = 1, SingleStandby = 2
    };

    /** VFO selection. */
    enum class VFO {
      A = 0, B = 1
    };

    /** VFO display modes. */
    enum class VFODisplayMode {
      Frequency = 0, ChannelName = 1
    };

    /** VFO Modes. */
    enum class VFOMode {
      Channel = 0, VFO = 1
    };

    /** Helper encoding/decoding key functions. */
    struct KeyFunction
    {
      enum class Function {
        None=0, PowerSelect=1, Volt=2, Talkaround=3, DMREncryption=4, VOX=6, ChannelMode=7, Alarm=8,
        OneTouch1=9, OneTouch2=10, OneTouch3=11,OneTouch4=12, OneTouch5=13, SMS=14, Contacts=15,
        ZoneUp=16, ZoneDown=17, Scan=18, ToggleRecord=19, PreviousRecord=20, NextRecord=21,
        FMBCRadio=22, FMBCScan=23, GPSInformation=24, Monitor=25, ToggleMainChannel=26, LoneWorker=27,
        KeypadLock=28, Mute=29, TBST=30, APRSTX=31, ChannelType=32, DisplayMode=33, CTCSSDSCScan=34,
        CTCSSDSCSettings=25, SilentTone=36, Roaming=37, SubPTT=38, OneKeyScanFrequency=40,
        Flashlight=41
      };

      static unsigned int encode(Function func);
      static Function decode(unsigned int code);
    };

    /** Possible power-save mode. */
    enum class PowerSaveMode {
      Off=0, Percent50 = 1, Percent66 = 2, Percent75=3
    };

    /** Possible TBST frequencies. */
    enum class TBSTFrequency {
      Hz1000=0, Hz1450=1, Hz1750=2, Hz2100=3
    };

    /** Possible squelch tail-eliminations. */
    enum class STEMode {
      Off = 0, Deg120 = 1, Deg180 = 2, Hz55 = 3
    };


  public:
    /** Constructor from pointer to element. */
    GeneralSettingsElement(uint8_t *ptr);

    /** Returns the size og the element. */
    static constexpr unsigned int size() { return 0x1000; }

    /** Returns the boot display setting. */
    virtual BootDisplay bootDisplay() const;
    /** Sets the boot display. */
    virtual void setBootDisplay(BootDisplay dis);

    /** Returns the first boot message line. */
    virtual QString bootMessage1() const;
    /** Sets the first boot message line. */
    virtual void setBootMessage1(const QString &msg);
    /** Returns the second boot message line. */
    virtual QString bootMessage2() const;
    /** Sets the second boot message line. */
    virtual void setBootMessage2(const QString &msg);

    /** Returns @c true if MCU reset is enabled. */
    virtual bool mcuResetEnabled() const;
    /** Enables MCU reset. */
    virtual void enableMCUReset(bool enable);
    /** Returns the auto-power-off delay. */
    virtual Interval autoPowerOffDelay() const;
    /** Sets the auto-power-off delay. */
    virtual void setAutoPowerOffDelay(const Interval &delay);

    /** Returns @c true if all tones are disabled. */
    virtual bool radioSilentEnabled() const;
    /** Disables all tones. */
    virtual void enableRadioSilent(bool enable);

    /** Returns @c true if key tones are enabled. */
    virtual bool keyToneEnabled() const;
    /** Enables key tones. */
    virtual void enableKeyTone(bool enable);

    /** Returns @c true if sms tones are enabled. */
    virtual bool smsToneEnabled() const;
    /** Enables sms tones. */
    virtual void enableSMSTone(bool enable);

    /** Returns @c true if group call tone is enabled. */
    virtual bool groupCallToneEnabled() const;
    /** Enables group call tones. */
    virtual void enableGroupCallTone(bool enable);
    /** Returns @c true if private call tone is enabled. */
    virtual bool privateCallToneEnabled() const;
    /** Enables group call tones. */
    virtual void enablePrivateCallTone(bool enable);

    /** Returns @c true if EOT tone is enabled. */
    virtual bool eotToneEnabled() const;
    /** Enables EOT tones. */
    virtual void enableEOTTone(bool enable);

    /** Returns @c true if talk permit tone is enabled. */
    virtual bool talkPermitToneEnabled() const;
    /** Enables talk permit tones. */
    virtual void enableTalkPermitTone(bool enable);

    /** Returns @c true if boot tone is enabled. */
    virtual bool bootToneEnabled() const;
    /** Enables boot tones. */
    virtual void enableBootTone(bool enable);

    /** Returns @c true if voice prompt is enabled. */
    virtual bool voicePromptEnabled() const;
    /** Enables voice prompt. */
    virtual void enableVoicePrompt(bool enable);

    /** Returns @c true if low-battery tone is enabled. */
    virtual bool lowBatteryToneEnabled() const;
    /** Enables low-battery tones. */
    virtual void enableLowBatteryTone(bool enable);

    /** Returns the FM roger tone setting. */
    virtual FMRogerTone fmRogerTone() const;
    /** Sets the FM roger tone. */
    virtual void setFMRogerTone(FMRogerTone tone);

    /** Returns the display brightness setting [0-10]. */
    virtual unsigned int displayBrightness() const;
    /** Sets the display brightness [0-10]. */
    virtual void setDisplayBrightness(unsigned int n);

    /** Returns the backlight duration. */
    virtual Interval backlightDuration() const;
    /** Sets the backlight duration. */
    virtual void setBacklightDuration(Interval duration);

    /** Returns the menu hold time. */
    virtual Interval menuDuration() const;
    /** Sets the menu hold time. */
    virtual void setMenuDuration(Interval duration);

    /** Returns @c true if the volume change prompt is shown. */
    virtual bool showVolmueChange() const;
    /** Enable volume change prompt. */
    virtual void enableShowVolumeChange(bool enable);

    /** Returns the date format. */
    virtual DateFormat dateFormat() const;
    /** Sets the date format. */
    virtual void setDateFormat(DateFormat format);

    /** Returns @c true if the clock is shown. */
    virtual bool showClock() const;
    /** Enable clock. */
    virtual void enableShowClock(bool enable);

    /** Returns the call color. */
    virtual Color::Code callColor() const;
    /** Sets the call color. */
    virtual void setCallColor(Color::Code c);
    /** Returns the standby text color. */
    virtual Color::Code standbyColor() const;
    /** Sets the standby text color. */
    virtual void setStandbyColor(Color::Code c);
    /** Returns the channel name A color. */
    virtual Color::Code channelNameAColor() const;
    /** Sets the channel name A color. */
    virtual void setChannelNameAColor(Color::Code c);
    /** Returns the channel name B color. */
    virtual Color::Code channelNameBColor() const;
    /** Sets the channel name B color. */
    virtual void setChannelNameBColor(Color::Code c);
    /** Returns the zone name A color. */
    virtual Color::Code zoneNameAColor() const;
    /** Sets the zone name A color. */
    virtual void setZoneNameAColor(Color::Code c);
    /** Returns the zone name B color. */
    virtual Color::Code zoneNameBColor() const;
    /** Sets the zone name B color. */
    virtual void setZoneNameBColor(Color::Code c);

    /** Returns the position display format. */
    virtual PositionFormat positionFormat() const;
    /** Sets the position display format. */
    virtual void setPositionFormat(PositionFormat format);

    /** Returns the GNSS mode. */
    virtual GNSSMode gnssMode() const;
    /** Sets the GNSS mode. */
    virtual void setGNSSMode(GNSSMode mode);

    /** Returns @c true if GNSS is enabled. */
    virtual bool gnssEnabled() const;
    /** Enables GNSS. */
    virtual void enableGNSS(bool enable);

    /** Returns the time zone. */
    virtual QTimeZone timeZone() const;
    /** Sets the timezone. */
    virtual void setTimeZone(const QTimeZone &timeZone);

    /** Returns the position update period. */
    virtual Interval posUpdatePeriod() const;
    /** Sets the position update period. */
    virtual void setPosUpdatePeriod(const Interval &period);

    /** Returns possible recording modes. */
    virtual RecordMode recordMode() const;
    /** Sets the record mode. */
    virtual void setRecordMode(RecordMode mode);

    /** Returns @c true if recording is enabled. */
    virtual bool recordingEnabled() const;
    /** Enables/disables recording. */
    virtual void enableRecording(bool enable);

    /** Returns @c true if group call must match. */
    virtual bool groupCallMatchEnabled() const;
    /** Enables group call match. */
    virtual void enableGroupCallMatch(bool enable);

    /** Returns @c true if private call must match. */
    virtual bool privateCallMatchEnabled() const;
    /** Enables private call match. */
    virtual void enablePrivateCallMatch(bool enable);

    /** Returns the DMR call hang time. */
    virtual Interval dmrCallHangTime() const;
    /** Sets the DMR call hang time. */
    virtual void setDMRCallHangTime(const Interval &hangTime);

    /** Returns the active wait time. */
    virtual Interval activeWaitTime() const;
    /** Sets the active wait time. */
    virtual void setActiveWaitTime(Interval waitTime);

    /** Returns the number of active retries. */
    virtual unsigned int activeRetries() const;
    /** Sets the number of active retires. */
    virtual void setActiveRetries(unsigned int retries);

    /** Retruns the DMR preamble duration. */
    virtual Interval dmrPreambleDuration() const;
    /** Sets the DMR preamble duration. */
    virtual void setDmrPreambleDuration(Interval duration);

    /** Returns @c true if the DMR remote monitor is enabled. */
    virtual bool dmrRemoteMonitorEnabled() const;
    /** Enables the remote monitor. */
    virtual void enableDMRRemoteMonitor(bool enable);

    /** Returns @c true if the remote kill switch is enabled. */
    virtual bool dmrRemoteKillEnabled() const;
    /** Enables the remote kill swtich. */
    virtual void enableDMRRemoteKill(bool enable);

    /** Returns @c true if the remote radio check is enabled. */
    virtual bool dmrRemoteRadioCheckEnabled() const;
    /** Enables remote radio check. */
    virtual void enableDMRRemoteRadioCheck(bool enable);

    /** Returns @c true if the remote reenable is enabled. */
    virtual bool dmrRemoteReenableEnabled() const;
    /** Enables DMR remote reenable. */
    virtual void enableDMRRemoteReenable(bool enable);

    /** Returns @c true reception of DMR alerts is enabled. */
    virtual bool dmrRXAlertEnabled() const;
    /** Enables enables reception of DMR alerts. */
    virtual void enableDMRRXAlert(bool enable);

    /** Returns the DMR SMS format. */
    virtual SMSExtension::Format smsFormat() const;
    /** Sets the DMR SMS format. */
    virtual void setSMSFormat(SMSExtension::Format format);

    /** Retruns @c true missed call notification is enabled. */
    virtual bool missedCallNotificationEnabled() const;
    /** Enables missed call notification. */
    virtual void enableMissedCallNotification(bool enable);

    /** Returns the remote monitor duration. */
    virtual Interval dmrRemoteMonitorDuration() const;
    /** Sets the remote monitor duration. */
    virtual void setDMRRemoteMonitorDuration(Interval duration);

    /** Returns the talker alias format. */
    virtual TalkerAliasFormat talkerAliasFormat() const;
    /** Sets the talker alias format. */
    virtual void setTalkerAliasFormat(TalkerAliasFormat format);

    /** Returns @c true, if transmission of talker alias is enabled. */
    virtual bool txTalkerAliasEnabled() const;
    /** Enables transmission of talker alias. */
    virtual void enableTXTalkerAlias(bool enable);

    /** Returns the talker alias source. */
    virtual TalkerAliasSource talkerAliasSource() const;
    /** Sets the talker alias source. */
    virtual void setTalkerAliasSource(TalkerAliasSource source);

    /** Returns the dual-standby mode. */
    virtual DualStandbyMode dualStandbyMode() const;
    /** Sets the dual-standby mode. */
    virtual void setDualStandbyMode(DualStandbyMode mode);

    /** Returns the main VFO. */
    virtual VFO mainVFO() const;
    /** Sets the main VFO. */
    virtual void setMainVFO(VFO mainVFO);

    /** Returns the VFO A display mode. */
    virtual VFODisplayMode vfoDisplayModeA() const;
    /** Sets the VFO A display mode. */
    virtual void setVFODisplayModeA(VFODisplayMode mode);
    /** Returns the VFO B display mode. */
    virtual VFODisplayMode vfoDisplayModeB() const;
    /** Sets the VFO B display mode. */
    virtual void setVFODisplayModeB(VFODisplayMode mode);
    /** Returns the VFO A mode. */
    virtual VFOMode vfoModeA() const;

    /** Sets the VFO A mode. */
    virtual void setVFOModeA(VFOMode mode);
    /** Returns the VFO B mode. */
    virtual VFOMode vfoModeB() const;
    /** Sets the VFO B mode. */
    virtual void setVFOModeB(VFOMode mode);

    /** Returns @c true if VFO modes are disabled. */
    virtual bool vfoModeDisabled() const;
    /** Disables VFO modes. */
    virtual void disableVFOMode(bool enable);

    /** Returns the dual-standby hang-time. */
    virtual Interval dualStandbyHangTime() const;
    /** Sets the dual-standby hang-time. */
    virtual void setDualStandbyHangTime(Interval hangTime);

    /** Returns @c true if the side keys are locked. */
    virtual bool sideKeyLockEnabled() const;
    /** Enable side key lock. */
    virtual void enableSideKeyLock(bool enable);
    /** Returns @c true, if the knowb is locked. */
    virtual bool knobLockEnabled() const;
    /** Enables the knob lock. */
    virtual void enableKnobLock(bool enable);
    /** Returns the auto key-lock delay. If set to infinity, auto key-lock is disabled. */
    virtual Interval autoKeyLockDelay() const;
    /** Sets the auto key-lock delay. If set to infinity, the auto key-lock is disabled. */
    virtual void setAutoKeyLockDelay(Interval delay);

    /** Returns the side-key 1 short-press function. */
    virtual KeyFunction::Function sk1Short() const;
    /** Sets the side-key 1 short-press function. */
    virtual void setSK1Short(KeyFunction::Function function);
    /** Returns the side-key 1 long-press function. */
    virtual KeyFunction::Function sk1Long() const;
    /** Sets the side-key 1 long-press function. */
    virtual void setSK1Long(KeyFunction::Function function);
    /** Returns the side-key 2 short-press function. */
    virtual KeyFunction::Function sk2Short() const;
    /** Sets the side-key 2 short-press function. */
    virtual void setSK2Short(KeyFunction::Function function);
    /** Returns the side-key 2 long-press function. */
    virtual KeyFunction::Function sk2Long() const;
    /** Sets the side-key 2 long-press function. */
    virtual void setSK2Long(KeyFunction::Function function);

    /** Returns the programmable key 1 short-press function. */
    virtual KeyFunction::Function p1Short() const;
    /** Sets the programmable key 1 short-press function. */
    virtual void setP1Short(KeyFunction::Function function);
    /** Returns the programmable key 1 long-press function. */
    virtual KeyFunction::Function p1Long() const;
    /** Sets the programmable key 1 long-press function. */
    virtual void setP1Long(KeyFunction::Function function);
    /** Returns the programmable key 2 short-press function. */
    virtual KeyFunction::Function p2Short() const;
    /** Sets the programmable key 2 short-press function. */
    virtual void setP2Short(KeyFunction::Function function);
    /** Returns the programmable key 2 long-press function. */
    virtual KeyFunction::Function p2Long() const;
    /** Sets the programmable key 2 long-press function. */
    virtual void setP2Long(KeyFunction::Function function);

    /** Returns the long-press duration. */
    virtual Interval longPressDuration() const;
    /** Sets the long-press duration. */
    virtual void setLongPressDuration(Interval duration);

    /** Returns the transmit timeout. If infinite, the transmit timeout is disabled. */
    virtual Interval transmitTimeout() const;
    /** Sets transmit timeout. If set to infinity, the tot is disabled. */
    virtual void setTransmitTimeout(Interval timeout);
    /** Returns the transmit timeout reminder. If 0 disabled. */
    virtual Interval transmitTimeoutReminder() const;
    /** Sets transmit timeout reminder. If set to 0 disabled. */
    virtual void setTransmitTimeoutReminder(Interval timeout);

    /** Returns the VOX level. */
    virtual unsigned int voxLevel() const;
    /** Sets the VOX level. */
    virtual void setVOXLevel(unsigned int voxLevel);
    /** Retruns the VOX delay. */
    virtual Interval voxDelay() const;
    /** Sets the VOX delay. */
    virtual void setVoxDelay(Interval delay);

    /** Returns the power-save mode. */
    virtual PowerSaveMode powerSaveMode() const;
    /** Sets the power-save mode. */
    virtual void setPowerSaveMode(PowerSaveMode mode);

    /** Returns @c true, if the NOAA weather alarm is enabled. */
    virtual bool weatherAlarmEnabled() const;
    /** Enables the NOAA weather alarm. */
    virtual void enableWeatherAlarm(bool enable);

    /** Returns @c true if all LEDs are disabled. */
    virtual bool allLEDsDisabled() const;
    /** Disables all LEDs. */
    virtual void disableAllLEDs(bool disable);

    /** Returns the TBST frequency. */
    virtual Frequency tbstFrequency() const;
    /** Sets the TBST frequency. */
    virtual void setTBSTFrequency(const Frequency &frequency);

    /** Returns the squelch tail elimination mode. */
    virtual STEMode steMode() const;
    /** Sets the STE mode. */
    virtual void setSTEMode(STEMode mode);

    /** Returns the FM mic level. */
    virtual unsigned int fmMicLevel() const;
    /** Sets the FM mic level.*/
    virtual void setFMMicLevel(unsigned int level);

    /** Returns the DMR mic level. */
    virtual unsigned int dmrMicLevel() const;
    /** Sets the DMR mic level.*/
    virtual void setDMRMicLevel(unsigned int level);

    /** Decodes the general settings. */
    virtual bool decode(Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits. */
    struct Limit: Element::Limit {
      /** Maximum boot message length */
      static constexpr unsigned int bootMessageLength()  { return 14; }
      /** Maximum display brightness. */
      static constexpr unsigned int maxBrightness()      { return 5; }
      /** Range of valid active retires. */
      static constexpr Range<unsigned int> activeRetires() { return {1, 8}; }
      /** Range of valid long-press durations. */
      static constexpr Range<Interval> longPressDuration() {
        return {Interval::fromSeconds(1), Interval::fromSeconds(5)};
      }
      /** Range of valid transmit timeouts. */
      static constexpr Range<Interval> transmitTimeout() {
        return {Interval::fromSeconds(20), Interval::fromSeconds(500)};
      }
      /** Range of valid VOX delays. */
      static constexpr Range<Interval> voxDelay() {
        return {Interval::fromMilliseconds(300), Interval::fromSeconds(5)};
      }
    };

  protected:
    /** Some internal offsets. */
    struct Offset: Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int bootDisplay()        { return 0x0000; }
      static constexpr unsigned int bootMessage1()       { return 0x0001; }
      static constexpr unsigned int bootMessage2()       { return 0x000f; }
      static constexpr unsigned int mcuReset()           { return 0x001d; }
      static constexpr unsigned int autoPowerOffDelay()  { return 0x001e; }
      static constexpr Bit radioSilent()                 { return {0x0020, 7}; }
      static constexpr Bit keyTone()                     { return {0x0020, 6}; }
      static constexpr Bit smsTone()                     { return {0x0020, 5}; }
      static constexpr Bit groupCallTone()               { return {0x0020, 4}; }
      static constexpr Bit privateCallTone()             { return {0x0020, 3}; }
      static constexpr Bit eotTone()                     { return {0x0020, 2}; }
      static constexpr Bit talkPermitTone()              { return {0x0020, 1}; }
      static constexpr Bit bootTone()                    { return {0x0020, 0}; }
      static constexpr Bit voicePrompt()                 { return {0x0021, 7}; }
      static constexpr Bit lowBatteryTone()              { return {0x0021, 6}; }
      static constexpr Bit fmRogerTone()                 { return {0x0021, 1}; }
      static constexpr unsigned int displayBrightness()  { return 0x0030; }
      static constexpr unsigned int backlightDuration()  { return 0x0031; }
      static constexpr unsigned int menuDuration()       { return 0x0032; }
      static constexpr Bit showVolumeChange()            { return {0x0033, 4}; }
      static constexpr Bit dateFormat()                  { return {0x0033, 3}; }
      static constexpr Bit showClock()                   { return {0x0033, 0}; }
      static constexpr unsigned int callColor()          { return 0x0034; }
      static constexpr unsigned int standbyColor()       { return 0x0035; }
      static constexpr unsigned int channelNameAColor()  { return 0x0038; }
      static constexpr unsigned int channelNameBColor()  { return 0x0039; }
      static constexpr unsigned int zoneNameAColor()     { return 0x003a; }
      static constexpr unsigned int zoneNameBColor()     { return 0x003b; }
      static constexpr Bit positionFormat()              { return {0x0040, 6}; }
      static constexpr Bit gnssMode()                    { return {0x0040, 3}; }
      static constexpr Bit enableGNSS()                  { return {0x0040, 0}; }
      static constexpr unsigned int timeZone()           { return 0x0041; }
      static constexpr unsigned int posUpdatePeriod()    { return 0x0042; }
      static constexpr Bit recordMode()                  { return {0x0043, 2}; }
      static constexpr Bit enableRecording()             { return {0x0045, 0}; }
      static constexpr Bit groupCallMatch()              { return {0x0060, 1}; }
      static constexpr Bit privateCallMatch()            { return {0x0060, 0}; }
      static constexpr unsigned int dmrCallHangTime()    { return 0x0061; }
      static constexpr unsigned int activeWaitTime()     { return 0x0062; }
      static constexpr unsigned int activeReties()       { return 0x0063; }
      static constexpr unsigned int dmrPreambleDur()     { return 0x0064; }
      static constexpr Bit dmrMonitor()                  { return {0x0065, 7}; }
      static constexpr Bit dmrKill()                     { return {0x0065, 6}; }
      static constexpr Bit dmrRadioCheck()               { return {0x0065, 5}; }
      static constexpr Bit dmrReenable()                 { return {0x0065, 4}; }
      static constexpr Bit dmrRXAlert()                  { return {0x0065, 3}; }
      static constexpr Bit smsFormat()                   { return {0x0065, 1}; }
      static constexpr Bit missedCallNotification()      { return {0x0065, 0}; }
      static constexpr unsigned int dmrRemoteMonitorDuration() { return 0x0066; }
      static constexpr Bit dmrTalkerAliasFormat()        { return {0x0067, 4}; }
      static constexpr Bit txTalkerAlias()               { return {0x0067, 3}; }
      static constexpr Bit talkerSource()                { return {0x0067, 2}; }
      static constexpr Bit dualStandbyMode()             { return {0x0080, 6}; }
      static constexpr Bit mainVFO()                     { return {0x0080, 5}; }
      static constexpr Bit displayModeB()                { return {0x0080, 4}; }
      static constexpr Bit displayModeA()                { return {0x0080, 3}; }
      static constexpr Bit vfoModeB()                    { return {0x0080, 2}; }
      static constexpr Bit vfoModeA()                    { return {0x0080, 1}; }
      static constexpr Bit disableVFOMode()              { return {0x0080, 0}; }
      static constexpr unsigned int dualStandbyHangTime() { return 0x0081;}
      static constexpr Bit sideKeyLock()                 { return {0x0085, 2}; }
      static constexpr Bit knobLock()                    { return {0x0085, 1}; }
      static constexpr Bit enableAutoKeyLock()           { return {0x0085, 0}; }
      static constexpr unsigned int autoKeyLockDelay()   { return 0x0086; }
      static constexpr unsigned int sk1Short()           { return 0x0087; }
      static constexpr unsigned int sk1Long()            { return 0x0088; }
      static constexpr unsigned int sk2Short()           { return 0x0089; }
      static constexpr unsigned int sk2Long()            { return 0x008a; }
      static constexpr unsigned int p1Short()            { return 0x008d; }
      static constexpr unsigned int p1Long()             { return 0x008e; }
      static constexpr unsigned int p2Short()            { return 0x008f; }
      static constexpr unsigned int p2Long()             { return 0x0090; }
      static constexpr unsigned int longPressDuration()  { return 0x0094; }
      static constexpr unsigned int transmitTimeout()    { return 0x00a0; }
      static constexpr unsigned int totReminder()        { return 0x00a1; }
      static constexpr unsigned int voxLevel()           { return 0x00a2; }
      static constexpr unsigned int voxDelay()           { return 0x00a3; }
      static constexpr Bit powerSaveMode()               { return {0x00a4, 4}; }
      static constexpr Bit weatherAlarm()                { return {0x00a4, 2}; }
      static constexpr Bit disableLEDs()                 { return {0x00a4, 0}; }
      static constexpr Bit tbstFrequency()               { return {0x00a5, 4}; }
      static constexpr Bit steMode()                     { return {0x00a5, 0}; }
      static constexpr unsigned int fmMicLevel()         { return 0x00a6; }
      static constexpr unsigned int dmrMicLevel()        { return 0x00a7; }
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
    static constexpr unsigned int generalSettings()     { return 0x00004000; }
    static constexpr unsigned int contactIndex()        { return 0x0000b000; }
    static constexpr unsigned int groupListBank()       { return 0x0000f000; }
    static constexpr unsigned int scanListBank()        { return 0x00011000; }
    static constexpr unsigned int channelBanks()        { return 0x00012000; }
    static constexpr unsigned int contactBanks()        { return 0x00044000; }
    static constexpr unsigned int zoneBanks()           { return 0x0005c000; }
    static constexpr unsigned int roamingZoneBank()     { return 0x00065000; }
    static constexpr unsigned int roamingChannelBank()  { return 0x00066000; }
    static constexpr unsigned int radioIdBank()         { return 0x00067000; }
    /// @endcond
  };
};

#endif // DM32UV_CODEPLUG_HH
