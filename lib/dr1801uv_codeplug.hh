#ifndef DR1801UVCODEPLUG_HH
#define DR1801UVCODEPLUG_HH

#include "codeplug.hh"

/** Implements the binary codeplug representation of a BTECH DR-1801UV (a.k.a, BF-1801-A6).
 *
 * This device is kind of a re-release of the BTECH DM-1801. However, the hardware and firmware
 * is completely different. It is just the chassis that remained the same. This version uses the
 * Auctus A6 radio-on-a-chip as its main component.
 *
 * @section dr1801ver Matching firmware versions
 * This class implements the codeplug for the firmware version @c 1.10. The codeplug format usually
 * does not change much with firmware revisions, in particular not with older radios.
 *
 * @section dr1801cpl Codeplug structure within radio
 * The memory representation of the codeplug within the radio is simply a single large block of
 * data, read and written entirely. The total size of the codeplug is 01dd90h bytes.
 *
 * <table>
 *  <tr><th>Start</th>   <th>End</th>      <th>Size</th>    <th>Content</th></tr>
 *  <tr><td>0x00000</td> <td>0x00300</td>  <td>0x00300</td>  <td>Some unkown settings, not
 *      configurable through the CPS. Likely some sort of calibration data. Must be conserved.</td></tr>
 *  <tr><td>0x00300</td> <td>0x003bc</td>  <td>0x000bc</td>  <td>Some information about the radio.
 *      Like serial number, firmware version, etc and timestamp. </td></tr>
 *  <tr><td>0x003b4</td> <td>0x00418</td>  <td>0x00064</td>  <td>General settings, see
 *      @c DR1801UVCodeplug::SettingsElement. </td></tr>
 *  <tr><td>0x00418</td> <td>0x04110</td>  <td>0x00000</td>  <td>Zone bank, see
 *      @c DR1801UVCodeplug::ZoneBankElement. </td></tr>
 *  <tr><td>0x04110</td> <td>0x04274</td>  <td>0x00164</td>  <td>Message bank, see
 *      @c DR1801UVCodeplug::MessageBankElement. </td></tr>
 *  <tr><td>0x04334</td> <td>0x0a338</td>  <td>0x06008</td>  <td>Contact bank, see
 *      @c DR1801UVCodeplug::ContactBankElement. </td></tr>
 *  <tr><td>0x0a338</td> <td>0x0a65c</td>  <td>0x00324</td>  <td>Scan-list bank, see
 *      @c DR1801UVCodeplug::ScanListBankElement. </td></tr>
 *  <tr><td>0x0a65c</td> <td>0x1c660</td>  <td>0x12004</td>  <td>Channel bank, see
 *      @c DR1801UVCodeplug::ChannelBankElement. </td></tr>
 *  <tr><td>0x1c6c4</td> <td>0x1c6dc</td>  <td>0x0018</td>  <td>Key settings, see
 *      @c DR1801UVCodeplug::KeySettingsElement. </td></tr>
 *  <tr><td>0x1c6dc</td> <td>0x1d7e0</td>  <td>0x01104</td>  <td>Group list bank, see
 *      @c DR1801UVCodeplug::GroupListBankElement. </td></tr>
 *  <tr><td>0x1d7e0</td> <td>0x1d858</td>  <td>0x00078</td>  <td>Encryption keys, see
 *      @c DR1801UVCodeplug::EncryptionKeyBankElement. </td></tr>
 *  <tr><td>0x1d858</td> <td>0x1daf4</td>  <td>0x0029c</td>  <td>DTMF signaling settings, see
 *      @c DR1801UVCodeplug::DTMFSettingsElement. </td></tr>
 *  <tr><td>0x1daf4</td> <td>0x1dbb8</td>  <td>0x00c4</td>   <td>Alarm settings, see
 *      @c DR1801UVCodeplug::AlarmSettingsBankElement. </tr>
 *  <tr><td>0x1dbb8</td> <td>0x1dbc8</td>  <td>0x0010</td>  <td>DMR settings, see
 *      @c DR1801UVCodeplug::DMRSettingsElement. </td></tr>
 *  <tr><td>0x1dbc8</td> <td>0x1dbf0</td>  <td>0x00028</td>  <td>One-touch settings, see
 *      @c DR1801UVCodeplug::OneTouchSettingsElement. </td></tr>
 *  <tr><td>0x1dbf0</td> <td>0x1dd00</td>  <td>0x00110</td>  <td>Some unknown settings.</td></tr>
 *  <tr><td>0x1dd00</td> <td>0x1dd90</td>  <td>0x00090</td>  <td>VFO channels, see
 *      @c DR1801UVCodeplug::VFOBankElement. </td></tr>
 * </table>
 *
 * @ingroup dr1801uv */
class DR1801UVCodeplug : public Codeplug
{
  Q_OBJECT

public:
  /** Implements the binary encoding of the channels settings.
   *
   * Memory representation of the channel settings (0034h bytes):
   * @verbinclude dr1801uv_channelelement.txt */
  class ChannelElement: public Element
  {
  public:
    /** Possible channel types. */
    enum class Type {
      FM = 1, DMR = 3
    };
    /** Possible power settings. */
    enum class Power {
      Low = 0, High = 1
    };
    /** Possible values for the admid criterion. */
    enum class Admit {
      Always = 0, ColorCode_or_Tone = 1, ChannelFree = 2
    };
    /** Possible time-slot values. */
    enum class TimeSlot {
      TS1 = 1, TS2 = 2
    };
    /** Possible FM signaling modes. */
    enum class SignalingMode {
      None = 0, DTMF = 1
    };
    /** Possible band width settings. */
    enum class Bandwidth {
      Narrow = 1, Wide = 2
    };
    /** Possible subtone types. */
    enum class SubToneType {
      None = 0, CTCSS = 1, DCS = 2
    };
    /** Possible DCS modes. */
    enum class DCSMode {
      Normal = 0, Inverted = 1
    };

  protected:
    /** Hidden constructor. */
    ChannelElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor from pointer. */
    ChannelElement(uint8_t *ptr);

    bool isValid() const;
    void clear();

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x00034; }

    /** Retunrs the 0-based index of the channel. */
    virtual unsigned int index() const;
    /** Sets the index. */
    virtual void setIndex(unsigned int idx);

    /** Returns the channel type. */
    virtual Type channelType() const;
    /** Sets the channel type. */
    virtual void setChannelType(Type type);

    /** Returns the power settings of the channel. */
    virtual Channel::Power power() const;
    /** Sets the power for the channel. */
    virtual void setPower(Channel::Power pwr);

    /** Retunrs the RX frequency in MHz. */
    virtual double rxFrequency() const;
    /** Sets the RX frequency in MHz. */
    virtual void setRXFrequency(double MHz);
    /** Retunrs the TX frequency in MHz. */
    virtual double txFrequency() const;
    /** Sets the TX frequency in MHz. */
    virtual void setTXFrequency(double MHz);

    /** Returns @c true if a contact index is set. */
    virtual bool hasTransmitContact() const;
    /** Retunrs the contact index. */
    virtual unsigned int transmitContactIndex() const;
    /** Sets the transmit contact index. */
    virtual void setTransmitContactIndex(unsigned int index);
    /** Clears the contact index. */
    virtual void clearTransmitContactIndex();

    /** Returns the admit criterion. */
    virtual Admit admitCriterion() const;
    /** Sets the admit criterion. */
    virtual void setAdmitCriterion(Admit admit);

    /** Returns the color code (0-15). */
    virtual unsigned int colorCode() const;
    /** Sets the color code (0-15). */
    virtual void setColorCode(unsigned int cc);
    /** Returns the time slot. */
    virtual DMRChannel::TimeSlot timeSlot() const;
    /** Sets the time slot. */
    virtual void setTimeSlot(DMRChannel::TimeSlot ts);

    /** Returns @c true if an ecryption key index is set. */
    virtual bool hasEncryptionKey() const;
    /** Returns the encryption key index. */
    virtual unsigned int encryptionKeyIndex() const;
    /** Sets the encryption key index. */
    virtual void setEncryptionKeyIndex(unsigned int index);
    /** Clears the encryption key index. */
    virtual void clearEncryptionKeyIndex();

    /** Returns @c true if dual-capacity direct mode is enabled. */
    virtual bool dcdm() const;
    /** Enables/disables dual-capacity direct mode. */
    virtual void enableDCDM(bool enable);
    /** Returns @c true if private-call confirmation is enabled. */
    virtual bool confirmPrivateCall() const;
    /** Enables/disables private-call confirmation. */
    virtual void enablePrivateCallConfirmation(bool enable);

    /** Returns the FM signaling mode. */
    virtual SignalingMode signalingMode() const;
    /** Sets the FM signaling mode. */
    virtual void setSignalingMode(SignalingMode mode);

    /** Returns @c true if the alarm system index is set. */
    virtual bool hasAlarmSystem() const;
    /** Returns the index of the alarm system. */
    virtual unsigned int alarmSystemIndex() const;
    /** Sets the alarm system index. */
    virtual void setAlarmSystemIndex(unsigned int index);
    /** Clears the alarm system index. */
    virtual void clearAlarmSystemIndex();

    /** Retunrs the band width for FM channels. */
    virtual FMChannel::Bandwidth bandwidth() const;
    /** Sets the band width for FM channels. */
    virtual void setBandwidth(FMChannel::Bandwidth bw);

    /** Retunrs @c true, if the auto-scan is enabled. */
    virtual bool autoScanEnabled() const;
    /** Enables/disables auto-scan. */
    virtual void enableAutoScan(bool enable);
    /** Retunrs @c true if a scan list is assigned. */
    virtual bool hasScanList() const;
    /** Returns the scan list index. */
    virtual unsigned int scanListIndex() const;
    /** Sets the scan list index. */
    virtual void setScanListIndex(unsigned int index);
    /** Clears the scan list index. */
    virtual void clearScanListIndex();

    /** Retunrs the RX CTCSS/DCS signaling. */
    virtual Signaling::Code rxTone() const;
    /** Sets the RX CTCSS/DCS signaling. */
    virtual void setRXTone(Signaling::Code code);
    /** Retunrs the TX CTCSS/DCS signaling. */
    virtual Signaling::Code txTone() const;
    /** Sets the TX CTCSS/DCS signaling. */
    virtual void setTXTone(Signaling::Code code);

    /** Returns @c true if talkaround is enabled. */
    virtual bool talkaround() const;
    /** Enables/disables talkaround. */
    virtual void enableTalkaround(bool enable);

    /** Returns @c true if a PTT ID is set. */
    virtual bool hasPTTID() const;
    /** Returns the PTT-ID index. */
    virtual unsigned int pttIDIndex() const;
    /** Sets the PTT-ID index. */
    virtual void setPTTIDIndex(unsigned int idx);
    /** Clears the PTT ID. */
    virtual void clearPTTID();

    /** Retruns @c true if a group list is assigned. */
    virtual bool hasGroupList() const;
    /** Retunrs the group-list index. */
    virtual unsigned int groupListIndex() const;
    /** Sets the group-list index. */
    virtual void setGroupListIndex(unsigned int index);
    /** Clears the group list index. */
    virtual void clearGroupListIndex();

    /** Returns @c true if lone-worker is enabled. */
    virtual bool loneWorker() const;
    /** Enables/disables lone-worker. */
    virtual void enableLoneWorker(bool enable);

    /** Constructs a channel object. */
    virtual Channel *toChannelObj(Context &ctx, const ErrorStack &err=ErrorStack()) const;
    /** Links the channel object. */
    virtual bool linkChannelObj(Channel *channel, Context &ctx, const ErrorStack &err=ErrorStack()) const;
    /** Encodes the given channel. */
    virtual bool encode(Channel *channel, Context &ctx, const ErrorStack &err=ErrorStack());

  protected:
    /** Some offsets within the codeplug. */
    struct Offset: Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int index() { return 0x0000; }
      static constexpr unsigned int channelType() { return 0x0002; }
      static constexpr unsigned int power() { return 0x0003; }
      static constexpr unsigned int rxFrequency() { return 0x0004; }
      static constexpr unsigned int txFrequency() { return 0x0008; }
      static constexpr unsigned int transmitContactIndex() { return 0x000c; }
      static constexpr unsigned int admitCriterion() { return 0x000e; }
      static constexpr unsigned int colorCode() { return 0x0010; }
      static constexpr unsigned int timeSlot() { return 0x0011; }
      static constexpr unsigned int encryptionKeyIndex() { return 0x0014; }
      static constexpr BitOffset dcdm() { return {0x0015, 1} ; }
      static constexpr BitOffset confirmPivateCall() { return {0x0015, 0}; }
      static constexpr unsigned int signalingMode() { return 0x0016; }
      static constexpr unsigned int alarmSystemIndex() { return 0x0018; }
      static constexpr unsigned int bandwidth() { return 0x0019; }
      static constexpr unsigned int autoScan() { return 0x001a; }
      static constexpr unsigned int scanListIndex() { return 0x001b; }
      static constexpr unsigned int rxSubtoneCode() { return 0x001c; }
      static constexpr unsigned int rxSubtoneType() { return 0x001e; }
      static constexpr unsigned int rxDCSMode() { return 0x001f; }
      static constexpr unsigned int txSubtoneCode() { return 0x0020; }
      static constexpr unsigned int txSubtoneType() { return 0x0022; }
      static constexpr unsigned int txDCSMode() { return 0x0023; }
      static constexpr unsigned int talkaround() { return 0x0025; }
      static constexpr unsigned int pttIDIndex() { return 0x0028; }
      static constexpr unsigned int groupListIndex() { return 0x002a; }
      static constexpr unsigned int loneWorker() { return 0x002f; }
      /// @endcond
    };
  };

  /** Implements the binary encoding of the channel bank.
   *
   * Holds up to 1024 @c DR1801UVCodeplug::ChannelElement.
   *
   * Memory representation of the channel bank (12004h bytes):
   * @verbinclude dr1801uv_channelbankelement.txt */
  class ChannelBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ChannelBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    ChannelBankElement(uint8_t *ptr);

    void clear();

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x12004; }

    /** Returns the number of channels. */
    virtual unsigned int channelCount() const;
    /** Sets the number of channels. */
    virtual void setChannelCount(unsigned int count);

    /** Returns a reference to the channel element that the given index. */
    virtual ChannelElement channel(unsigned int index) const;

    /** Returns the name of the channel at the given index. */
    virtual QString channelName(unsigned int index) const;
    /** Sets the name of the channel at the given index. */
    virtual void setChannelName(unsigned int index, const QString &name);

    /** Decodes all defined channels. */
    virtual bool decode(Context &ctx, const ErrorStack &err=ErrorStack()) const;
    /** Links channels. */
    virtual bool link(Context &ctx, const ErrorStack &err=ErrorStack()) const;
    /** Encodes all channles. */
    virtual bool encode(Context &ctx, const ErrorStack &err=ErrorStack());

  protected:
    /** Offsets within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int channelCount() { return 0x00000; }
      static constexpr unsigned int channel()      { return 0x00004; }
      static constexpr unsigned int channelName()  { return 0x0d004; }
      /// @endcond
    };

    /** Limits of some elements. */
    struct Limit {
      /** Returns the maximum number of channels. */
      static constexpr unsigned int channelCount()       { return 1024; }
      /** Returns the maximum length of a channel name. */
      static constexpr unsigned int channelNameLength()  { return 0x00014; }
    };
  };


  /** Implements the binary encoding of a contact.
   *
   * Memory representation of contact (0018h bytes):
   * @verbinclude dr1801uv_contactelement.txt */
  class ContactElement: public Element
  {
  public:
    /** Possible call types. */
    enum class CallType {
      AllCall = 0x20, PrivateCall = 0x40, GroupCall = 0x80
    };

  protected:
    /** Hidden constructor. */
    ContactElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    ContactElement(uint8_t *ptr);

    bool isValid() const;
    void clear();

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x00018; }

    /** Returns @c true if the contact has a successor. */
    virtual bool hasSuccessor() const;
    /** Returns the index of the next element. */
    virtual uint16_t successorIndex() const;
    /** Sets successor index. */
    virtual void setSuccessorIndex(uint16_t index);
    /** Clears the successor index. */
    virtual void clearSuccessorIndex();

    /** Returns the number. */
    virtual uint32_t dmrID() const;
    /** Sets the number. */
    virtual void setDMRID(uint32_t id);

    /** Returns the call type. */
    virtual DMRContact::Type type() const;
    /** Sets the call type. */
    virtual void setCallType(DMRContact::Type type);

    /** Returns the name of the contact. */
    virtual QString name() const;
    /** Sets the name of the contact. */
    virtual void setName(const QString &name);

    /** Constructs a DMR contact object from this contact elmeent. */
    virtual DMRContact *toContactObj(Context &ctx, const ErrorStack &err=ErrorStack()) const;
    /** Links the DMR contact object. */
    virtual bool linkContactObj(DMRContact *contact, Context &ctx, const ErrorStack &err=ErrorStack());
    /** Encodes the contact. */
    virtual bool encode(DMRContact *contact, Context &ctx, const ErrorStack &err=ErrorStack());

  protected:
    /** Defines offsets within the element. */
    struct Offset : public Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int successorIndex() { return 0x0000; }
      static constexpr unsigned int nameLength() { return 0x0002; }
      static constexpr unsigned int dmrID() { return 0x0004; }
      static constexpr unsigned int callType() { return 0x0007; }
      static constexpr unsigned int name() { return 0x0008; }
      /// @endcond
    };
  };

  /** Implements the binary encoding of the contact bank.
   *
   * The bank holds the list of all contacts defined. See @c DR1801UVCodeplug::ContactElement for
   * details.
   *
   * Memory representation of the contact bank (6008h bytes):
   * @verbinclude dr1801uv_contactbankelement.txt */
  class ContactBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ContactBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    ContactBankElement(uint8_t *ptr);

    void clear();

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x06004; }

    /** Returns the number of contacts. */
    virtual unsigned int contactCount() const;
    /** Sets the number of contacts. */
    virtual void setContactCount(unsigned int count);

    /** Returns the index of the first contact. */
    virtual unsigned int firstIndex() const;
    /** Sets the index of the first element. */
    virtual void setFirstIndex(unsigned int index);

    /** Returns a reference to the n-th contact. */
    virtual ContactElement contact(unsigned int index) const;

    /** Decodes all contacts and stores them into the given context and config. */
    virtual bool decode(Context &ctx, const ErrorStack &err=ErrorStack()) const;
    /** Links all contacts. */
    virtual bool link(Context &ctx, const ErrorStack &err=ErrorStack()) const;
    /** Encodes all contacts. */
    virtual bool encode(Context &ctx, const ErrorStack &err=ErrorStack());

  protected:
    /** Offsets within the element. */
    struct Offset : public Element::Offset{
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int contactCount() { return 0x0000; }
      static constexpr unsigned int firstIndex() { return 0x0002; }
      static constexpr unsigned int contacts() { return 0x0004; }
      /// @endcond
    };
    struct Limit {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int contactCount() { return 1024; }
      /// @endcond
    };
  };


  /** Implements the binary encoding of a group list.
   *
   * Memory representation of group list (44h bytes):
   * @verbinclude dr1801uv_grouplistelement.txt */
  class GroupListElement: public Element
  {
  protected:
    /** Hidden constructor. */
    GroupListElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    GroupListElement(uint8_t *ptr);

    bool isValid() const;
    void clear();

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x00044; }

    /** Returns the index of the group list. */
    virtual unsigned int index() const;
    /** Sets the index of the group list. */
    virtual void setIndex(unsigned int index);

    /** Returns the number of elements in the list. */
    virtual unsigned int count() const;
    /** Sets the number of elements in the list. */
    virtual void setCount(unsigned int n);

    /** Returns @c true if the n-th member index is set. */
    virtual bool hasMemberIndex(unsigned int n) const;
    /** Retuns the n-th member index. */
    virtual unsigned int memberIndex(unsigned int n) const;
    /** Sets the n-th member index. */
    virtual void setMemberIndex(unsigned int n, unsigned int index);
    /** Clears the n-th member index. */
    virtual void clearMemberIndex(unsigned int n);

    /** Constructs a group list object from this elmeent. */
    virtual RXGroupList *toGroupListObj(Context &ctx, const ErrorStack &err=ErrorStack()) const;
    /** Links the group list object. */
    virtual bool linkGroupListObj(RXGroupList *list, Context &ctx, const ErrorStack &err=ErrorStack()) const;
    /** Links the group list object. */
    virtual bool encode(RXGroupList *list, Context &ctx, const ErrorStack &err=ErrorStack());

  protected:
    /** Some offset within the codeplug. */
    struct Offset: public Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int count()   { return 0x0000; }
      static constexpr unsigned int index()   { return 0x0002; }
      static constexpr unsigned int members() { return 0x0004; }
      /// @endcond
    };
    /** Some limits. */
    struct Limit {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int members() { return 10; }
      /// @endcond
    };
  };

  /** Implements the binary encoding of the group-list bank.
   *
   * Memory representation of the group-list bank (??h bytes):
   * @verbinclude dr1801uv_grouplistbankelement.txt */
  class GroupListBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    GroupListBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    GroupListBankElement(uint8_t *ptr);

    void clear();

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x01104; }

    /** Returns the number of group lists defined. */
    virtual unsigned int groupListCount() const;
    /** Sets the number of group lists. */
    virtual void setGroupListCount(unsigned int count);

    /** Returns a reference to the group list at the given index. */
    virtual GroupListElement groupList(unsigned int index) const;

    /** Decodes all group lists. */
    virtual bool decode(Context &ctx, const ErrorStack &err=ErrorStack()) const;
    /** Links all group lists. */
    virtual bool link(Context &ctx, const ErrorStack &err=ErrorStack()) const;
    /** Encodes all group lists. */
    virtual bool encode(Context &ctx, const ErrorStack &err=ErrorStack());

  protected:
    /** Some offsets within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int groupListCount() { return 0x0000; }
      static constexpr unsigned int groupLists() { return 0x00004; }
      /// @endcond
    };
    /** Some limits. */
    struct Limit {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int groupListCount() { return 64; }
      /// @endcond
    };
  };


  /** Implements the binary encoding of a zone.
   *
   * Memory representation of zone (68h bytes):
   * @verbinclude dr1801uv_zoneelement.txt */
  class ZoneElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ZoneElement(uint8_t *ptr, size_t size);

  public:
    /** Construtor. */
    ZoneElement(uint8_t *ptr);

    bool isValid() const;
    void clear();

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x00068; }

    /** Returns the name of the zone. */
    virtual QString name() const;
    /** Sets the name of the zone. */
    virtual void setName(const QString &name);

    /** Retunrs the number of entries. */
    virtual unsigned int numEntries() const;
    /** Returns the channel index of the n-th entry. */
    virtual unsigned int entryIndex(unsigned int n);
    /** Sets the n-th entry index. */
    virtual void setEntryIndex(unsigned int n, unsigned int index);

    /** Returns the index of the zone. */
    virtual unsigned int index() const;
    /** Sets the index of the zone. */
    virtual void setIndex(unsigned int index);

    /** Constructs a zone object from this element. */
    virtual Zone *toZoneObj(Context &ctx, const ErrorStack &err=ErrorStack());
    /** Links the zone object. */
    virtual bool linkZoneObj(Zone *obj, Context &ctx, const ErrorStack &err=ErrorStack());
    /** Encodes the zone. */
    virtual bool encode(Zone *obj, Context &ctx, const ErrorStack &err=ErrorStack());

  protected:
    /** Some offset within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int name() { return 0x0000; }
      static constexpr unsigned int nameLength() { return 0x0020; }
      static constexpr unsigned int numEntries() { return 0x0022; }
      static constexpr unsigned int index() { return 0x0024; }
      static constexpr unsigned int members() { return 0x0028; }
      /// @endcond
    };
    /** Some limits. */
    struct Limit {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int nameLength() { return 32; }
      static constexpr unsigned int memberCount() { return 32; }
      /// @endcond
    };
  };

  /** Implements the binary encoding of the zone bank.
   *
   * Memory representation of the zone bank (3cf8h bytes):
   * @verbinclude dr1801uv_zonebankelement.txt */
  class ZoneBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ZoneBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    ZoneBankElement(uint8_t *ptr);

    void clear();

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x03cf8; }

    /** Returns the number of zones. */
    virtual unsigned int zoneCount() const;
    /** Sets the number of zones. */
    virtual void setZoneCount(unsigned int count);

    /** Returns the so-called up-zone index. */
    virtual unsigned int upZoneIndex() const;
    /** Sets the so-called up-zone index. */
    virtual void setUpZoneIndex(unsigned int index);
    /** Returns the so-called down-zone index. */
    virtual unsigned int downZoneIndex() const;
    /** Sets the so-called down-zone index. */
    virtual void setDownZoneIndex(unsigned int index);

    /** Returns a reference to the n-th zone. */
    virtual ZoneElement zone(unsigned int index) const;

    /** Decodes all zones. */
    virtual bool decode(Context &ctx, const ErrorStack &err = ErrorStack()) const;
    /** Links all zones. */
    virtual bool link(Context &ctx, const ErrorStack &err = ErrorStack()) const;
    /** Encodes all zones. */
    virtual bool encode(Context &ctx, const ErrorStack &err = ErrorStack());

  protected:
    /** Some offsets within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int zoneCount() { return 0x0000; }
      static constexpr unsigned int upZoneIndex() { return 0x0002; }
      static constexpr unsigned int downZoneIndex() { return 0x0004; }
      static constexpr unsigned int zones() { return 0x0008; }
      /// @endcond
    };
    /** Some limits. */
    struct Limit {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int zoneCount() { return 150; }
      /// @endcond
    };
  };


  /** Implements the binary encoding of the settings element.
   *
   * Memory representation of settings element (64h bytes):
   * @verbinclude dr1801uv_settingselement.txt */
  class SettingsElement: public Element
  {
  public:
    /** Possible power-save modes. */
    enum class PowerSaveMode {
      Off = 0, Save50 = 1, Save25 = 2, Save12 = 3
    };

    /** Possible UI languages. */
    enum class Language {
      SimplifiedChinese = 0, English = 1
    };

    /** Possible squelch modes. */
    enum class SquelchMode {
      Normal = 0, Silent = 1
    };

    /** Possible ring tone variants. */
    enum class RingTone {
      Off = 0, RingTone1 = 1, RingTone2 = 2, RingTone3 = 3, RingTone4 = 4, RingTone5 = 5,
      RingTone6 = 6, RingTone7 = 7, RingTone8 = 8, RingTone9 = 9, RingTone10 = 10, RingTone11 = 11,
      RingTone12 = 12, RingTone13 = 13, RingTone14 = 14, RingTone15 = 15, RingTone16 = 16,
      RingTone17 = 17, RingTone18 = 18, RingTone19 = 19, RingTone20 = 20
    };

    /** Possible backlight time settings. */
    enum class BacklightTime {
      Infinite = 0, Off = 1, On5s = 2, On10s = 3
    };

    /** Possible tuning modes. */
    enum class TuningMode {
      Channel = 0, VFO = 1
    };

    /** Possible display modes. */
    enum class DisplayMode {
      Number = 0, Name = 1, Frequency = 2
    };

    /** Possible dual-watch modes. */
    enum class DualWatchMode {
      Off = 0, DoubleDouble = 1, DoubleSingle = 2
    };

    /** Possible scan modes. */
    enum class ScanMode {
      Time = 0, Carrier = 1, Search = 2
    };

    /** Possible boot screen modes. */
    enum class BootScreen {
      Picture = 0, Text = 1
    };

  protected:
    /** Hidden constructor. */
    SettingsElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    SettingsElement(uint8_t *ptr);

    void clear();

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x00064; }

    /** Returns the radios DMR ID. */
    virtual unsigned int dmrID() const;
    /** Sets the radios DMR ID. */
    virtual void setDMRID(unsigned int id);

    /** Retunrs the the power-save mode. */
    virtual PowerSaveMode powerSaveMode() const;
    /** Sets the power-save mode.  */
    virtual void setPowerSaveMode(PowerSaveMode mode);

    /** Returns the VOX sensitivity [0,10].
     * 0 means VOX off. */
    virtual unsigned int voxSensitivity() const;
    /** Sets the VOX sensitivity [0,10].
     * 0 means VOX off. */
    virtual void setVOXSensitivity(unsigned int sens);
    /** Returns the VOX delay in ms. */
    virtual unsigned int voxDelay() const;
    /** Sets the VOX delay in ms. */
    virtual void setVOXDelay(unsigned int ms);

    /** Returns @c true if encryption is enabled. */
    virtual bool encryptionEnabled() const;
    /** Enables/disables encryption globally. */
    virtual void enableEncryption(bool enable);

    /** Returns @c true if the key-lock is enabled. */
    virtual bool keyLockEnabled() const;
    /** Enable/disable key-lock. */
    virtual void enableKeyLock(bool enable);
    /** Returns the key-lock delay in seconds. */
    virtual unsigned int keyLockDelay() const;
    /** Sets the key-lock delay in seconds. */
    virtual void setKeyLockDelay(unsigned int sec);
    /** Retunrs @c true if the side-key 1 gets locked too. */
    virtual bool lockSideKey1() const;
    /** Enables/disables locking the side-key 1. */
    virtual void enableLockSideKey1(bool enable);
    /** Retunrs @c true if the side-key 2 gets locked too. */
    virtual bool lockSideKey2() const;
    /** Enables/disables locking the side-key 2. */
    virtual void enableLockSideKey2(bool enable);
    /** Retunrs @c true if the PTT gets locked too. */
    virtual bool lockPTT() const;
    /** Enables/disables locking the PTT. */
    virtual void enableLockPTT(bool enable);

    /** Returns the UI language. */
    virtual Language language() const;
    /** Sets the UI language. */
    virtual void setLanguage(Language lang);

    /** Returns the squelch mode. */
    virtual SquelchMode squelchMode() const;
    /** Sets the squelch mode. */
    virtual void setSquelchMode(SquelchMode mode);

    /** Returns @c true, if the roger tones are enabled. */
    virtual bool rogerTonesEnabled() const;
    /** Enables/disables roger tones. */
    virtual void enableRogerTones(bool enable);
    /** Returns @c true if the DMR call out roger tone is enabled. */
    virtual bool dmrCallOutToneEnabled() const;
    /** Enables/disables the DMR call out roger tone. */
    virtual void enableDMRCallOutTone(bool enable);
    /** Returns @c true if the DMR voice end roger tone is enabled. */
    virtual bool dmrVoiceEndToneEnabled() const;
    /** Enables/disables the DMR voice end roger tone. */
    virtual void enableDMRVoiceEndTone(bool enable);
    /** Returns @c true if the DMR call end roger tone is enabled. */
    virtual bool dmrCallEndToneEnabled() const;
    /** Enables/disables the DMR call end roger tone. */
    virtual void enableDMRCallEndTone(bool enable);
    /** Returns @c true if the FM voice end roger tone is enabled. */
    virtual bool fmVoiceEndToneEnabled() const;
    /** Enables/disables the FM voice end roger tone. */
    virtual void enableFMVoiceEndTone(bool enable);
    /** Returns @c true if the FM call out roger tone is enabled. */
    virtual bool fmCallOutToneEnabled() const;
    /** Enables/disables the FM call out roger tone. */
    virtual void enableFMCallOutTone(bool enable);
    /** Returns @c true if the message tone is enabled. */
    virtual bool messageToneEnabled() const;
    /** Enables/disables message tone. */
    virtual void enableMessageTone(bool enable);

    /** Retuns the ring tone. */
    virtual RingTone ringTone() const;
    /** Sets the ring tone. */
    virtual void setRingTone(RingTone tone);

    /** Retuns the radio name. */
    virtual QString radioName() const;
    /** Sets the radio name. */
    virtual void setRadioName(const QString &name);

    /** Returns the reverse burst frequency in Hz. */
    virtual float reverseBurstFrequency() const;
    /** Sets the reverse burst frequency in Hz. */
    virtual void setReverseBurstFrequency(float Hz);

    /** Returns the backlight time settings. */
    virtual BacklightTime backlightTime() const;
    /** Sets the backlight time. */
    virtual void setBacklightTime(BacklightTime time);

    /** Returns @c true, if campanding is enabled. */
    virtual bool campandingEnabled() const;
    /** Enables/disables campanding. */
    virtual void enableCampanding(bool enable);

    /** Retunrs the tuning mode up-direction. */
    virtual TuningMode tunigModeUp() const;
    /** Sets the tuning mode up-direction. */
    virtual void setTuningModeUp(TuningMode mode);
    /** Retunrs the tuning mode down-direction. */
    virtual TuningMode tunigModeDown() const;
    /** Sets the tuning mode down-direction. */
    virtual void setTuningModeDown(TuningMode mode);

    /** Returns the display mode. */
    virtual DisplayMode displayMode() const;
    /** Sets the display mode. */
    virtual void setDisplayMode(DisplayMode mode);

    /** Returns the dual-watch mode. */
    virtual DualWatchMode dualWatchMode() const;
    /** Sets the dual-watch mode. */
    virtual void setDualWatchMode(DualWatchMode mode);

    /** Returns the scan mode. */
    virtual ScanMode scanMode() const;
    /** Sets the scan mode. */
    virtual void setScanMode(ScanMode mode);

    /** Returns the boot-screen mode. */
    virtual BootScreen bootScreen() const;
    /** Sets the boot-screen mode. */
    virtual void setBootScreen(BootScreen mode);

    /** Returns the boot-screen line 1. */
    virtual QString bootLine1() const;
    /** Sets the boot-screen line 1. */
    virtual void setBootLine1(const QString &line);
    /** Returns the boot-screen line 2. */
    virtual QString bootLine2() const;
    /** Sets the boot-screen line 2. */
    virtual void setBootLine2(const QString &line);

    /** Returns @c true if the LED is enabled. */
    virtual bool ledEnabled() const;
    /** Enables/disables the LED. */
    virtual void enableLED(bool enabled);

    /** Returns the lone-worker response time in seconds. */
    virtual unsigned int loneWorkerResponseTime() const;
    /** Sets the lone-worker response time in seconds. */
    virtual void setLoneWorkerResponseTime(unsigned int sec);
    /** Returns the lone-worker reminder time in seconds. */
    virtual unsigned int loneWorkerReminderTime() const;
    /** Sets the lone-worker resminder time in seconds. */
    virtual void setLoneWorkerReminderTime(unsigned int sec);

    /** Returns @c true if the boot password is enabled. */
    virtual bool bootPasswordEnabled() const;
    /** Returns the boot password. */
    virtual QString bootPassword() const;
    /** Sets and enables boot password. */
    virtual void setBootPassword(const QString &passwd);
    /** Clears and disables boot password. */
    virtual void clearBootPassword();

    /** Returns @c true if the programming password is enabled. */
    virtual bool progPasswordEnabled() const;
    /** Returns the programming password. */
    virtual QString progPassword() const;
    /** Sets and enables programming password. */
    virtual void setProgPassword(const QString &passwd);
    /** Clears and disables programming password. */
    virtual void clearProgPassword();

    /** Updates configuration. */
    virtual bool updateConfig(Config *config, const ErrorStack &err=ErrorStack());
    /** Encode from config. */
    virtual bool fromConfig(Config *config, const ErrorStack &err=ErrorStack());

  protected:
    /** Some offsets within the element. */
    struct Offset: Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int dmrID()                 { return 0x0000; }
      static constexpr unsigned int powerSaveEnabled()      { return 0x0008; }
      static constexpr unsigned int powerSaveMode()         { return 0x0009; }
      static constexpr unsigned int voxSensitivity()        { return 0x000a; }
      static constexpr unsigned int voxDelay()              { return 0x000c; }
      static constexpr unsigned int encryptionEnabled()     { return 0x000d; }
      static constexpr unsigned int keyLockDelay()          { return 0x000e; }
      static constexpr BitOffset lockPTT()                  { return {0x000f, 0}; }
      static constexpr BitOffset lockSideKey1()             { return {0x000f, 1}; }
      static constexpr BitOffset lockSideKey2()             { return {0x000f, 2}; }
      static constexpr unsigned int language()              { return 0x0010; }
      static constexpr unsigned int squelchMode()           { return 0x0011; }
      static constexpr unsigned int rogerTonesEnabled()     { return 0x0013; }
      static constexpr unsigned int keyLockEnabled()        { return 0x0017; }
      static constexpr unsigned int ringTone()              { return 0x0016; }
      static constexpr unsigned int radioName()             { return 0x0018; }
      static constexpr BitOffset dmrCallOutToneEnabled()    { return {0x0028, 1}; }
      static constexpr BitOffset fmCallOutToneEnabled()     { return {0x0028, 2}; }
      static constexpr BitOffset dmrVoiceEndToneEnabled()   { return {0x0028, 3}; }
      static constexpr BitOffset fmVoiceEndToneEnabled()    { return {0x0028, 4}; }
      static constexpr BitOffset dmrCallEndToneEnabled()    { return {0x0028, 5}; }
      static constexpr BitOffset messageToneEnabled()       { return {0x0028, 6}; }
      static constexpr unsigned int reverseBurstFrequency() { return 0x002c; }
      static constexpr unsigned int backlightTime()         { return 0x002f; }
      static constexpr unsigned int voxEnabled()            { return 0x0030; }
      static constexpr unsigned int campandingEnabled()     { return 0x0032; }
      static constexpr unsigned int tuningModeUp()          { return 0x0036; }
      static constexpr unsigned int tunigModeDown()         { return 0x0037; }
      static constexpr unsigned int displayMode()           { return 0x003c; }
      static constexpr unsigned int dualWatchMode()         { return 0x003d; }
      static constexpr unsigned int scanMode()              { return 0x003e; }
      static constexpr unsigned int bootScreen()            { return 0x003f; }
      static constexpr unsigned int bootLine1()             { return 0x0040; }
      static constexpr unsigned int bootLine2()             { return 0x0048; }
      static constexpr unsigned int ledEnabled()            { return 0x0050; }
      static constexpr unsigned int loneWorkerResponseTime() { return 0x0051; }
      static constexpr unsigned int loneWorkerReminderTime() { return 0x005c; }
      static constexpr BitOffset progPasswordEnabled()       { return {0x0052, 0}; }
      static constexpr BitOffset bootPasswordEnabled()       { return {0x0052, 1}; }
      static constexpr unsigned int progPasswordLength()     { return 0x0053; }
      static constexpr unsigned int progPassword()           { return 0x0054; }
      static constexpr unsigned int boolPasswordLength()     { return 0x005d; }
      static constexpr unsigned int bootPassword()           { return 0x005e; }
      /// @endcond
    };

  public:
    /** Some limits. */
    struct Limit {
      /** Maximum radio name length. */
      static constexpr unsigned int radioNameLength()     { return 16; }
      /** Maximum boot-text lines length. */
      static constexpr unsigned int bootLineLength()      { return 8; }
      /** Maximum boot password length. */
      static constexpr unsigned int bootPasswordLength()  { return 6; }
      /** Maximum programming password length. */
      static constexpr unsigned int progPasswordLength()  { return 6; }
    };
  };


  /** Implements the binary encoding of a scan list element.
   *
   * Memory representation of a scan list element (50h bytes):
   * @verbinclude dr1801uv_scanlistelement.txt */
  class ScanListElement: public Element
  {
  public:
    /** Possible priority channel modes. */
    enum class PriorityChannel {
      None = 0, Fixed = 1, Selected = 2
    };

    /** Possible revert channel modes. */
    enum class RevertChannel {
      LastActive = 0, Fixed = 1, Selected = 2
    };

  protected:
    /** Hidden constructor. */
    ScanListElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    ScanListElement(uint8_t *ptr);

    bool isValid() const;
    void clear();

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x00050; }

    /** Retunrs the index of the scan list. */
    virtual unsigned int index() const;
    /** Sets the index of the scan list. */
    virtual void setIndex(unsigned int idx);

    /** Returns the number of entries. */
    virtual unsigned int entryCount() const;
    /** Sets the number of entries. */
    virtual void setEntryCount(unsigned int num);

    /** Retunrs the priority channel 1 setting. */
    virtual PriorityChannel priorityChannel1() const;
    /** Sets the priority channel 1 setting. */
    virtual void setPriorityChannel1(PriorityChannel mode);
    /** Returns the priority channel 1 index. */
    virtual unsigned int priorityChannel1Index() const;
    /** Sets the priority channel 1 index. */
    virtual void setPriorityChannel1Index(unsigned int index);

    /** Retunrs the priority channel 2 setting. */
    virtual PriorityChannel priorityChannel2() const;
    /** Sets the priority channel 2 setting. */
    virtual void setPriorityChannel2(PriorityChannel mode);
    /** Returns the priority channel 2 index. */
    virtual unsigned int priorityChannel2Index() const;
    /** Sets the priority channel 2 index. */
    virtual void setPriorityChannel2Index(unsigned int index);

    /** Retunrs the revert channel setting. */
    virtual RevertChannel revertChannel() const;
    /** Sets the revert channel setting. */
    virtual void setRevertChannel(RevertChannel mode);
    /** Returns the revert channel index. */
    virtual unsigned int revertChannelIndex() const;
    /** Sets the revert channel index. */
    virtual void setRevertChannelIndex(unsigned int index);

    /** Returns the name of the scan list. */
    virtual QString name() const;
    /** Sets the name of the scan list. */
    virtual void setName(const QString &name);

    /** Returns the n-th entry index. */
    virtual unsigned int entryIndex(unsigned int n);
    /** Sets the n-th entry index. */
    virtual void setEntryIndex(unsigned int n, unsigned int index);

    /** Constructs a scan-list object from this element. */
    virtual ScanList *toScanListObj(Context &ctx, const ErrorStack &err=ErrorStack());
    /** Links the scan-list object. */
    virtual bool linkScanListObj(ScanList *obj, Context &ctx, const ErrorStack &err=ErrorStack());
    /** Encodes the scan list. */
    virtual bool encode(ScanList *obj, Context &ctx, const ErrorStack &err=ErrorStack());

  protected:
    /** Some offsets within the element. */
    struct Offset {
      /// @cond DO_NO_DOCUMENT
      static constexpr unsigned int index() { return 0x0000; }
      static constexpr unsigned int name() { return 0x0010; }
      static constexpr unsigned int priorityChannel1() { return 0x0002; }
      static constexpr unsigned int priorityChannel1Index() { return 0x0004; }
      static constexpr unsigned int priorityChannel2() { return 0x0003; }
      static constexpr unsigned int priorityChannel2Index() { return 0x0006; }
      static constexpr unsigned int revertChannel() { return 0x0008; }
      static constexpr unsigned int revertChannelIndex() { return 0x000a; }
      static constexpr unsigned int memberCount() { return 0x0001; }
      static constexpr unsigned int memberIndices() { return 0x0030; }
      /// @endcond
    };

  public:
    /** Some limits. */
    struct Limit {
      /** Maximum length of the name. */
      static constexpr unsigned int nameLength() { return 32; }
      /** Maximum number of channels in scan list. */
      static constexpr unsigned int memberCount() { return 16; }
    };
  };

  /** Implements the binary encoding of the scan-list bank.
   *
   * Holds up to 10 @c DR1801UVCodeplug::ScanListElement.
   *
   * Memory representation of the scan list bank (324h bytes):
   * @verbinclude dr1801uv_scanlistbankelement.txt */
  class ScanListBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ScanListBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    ScanListBankElement(uint8_t *ptr);

    void clear();

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x00324; }

    /** Returns the number of scan lists. */
    virtual unsigned int scanListCount() const;
    /** Sets the number of scan lists. */
    virtual void setScanListCount(unsigned int count);

    /** Returns a reference to the n-th scan list. */
    virtual ScanListElement scanList(unsigned int index) const;

    /** Decodes all scan lists. */
    virtual bool decode(Context &ctx, const ErrorStack &err=ErrorStack()) const;
    /** Links the scan lists. */
    virtual bool link(Context &ctx, const ErrorStack &err=ErrorStack()) const;
    /** Encodes all scan lists. */
    virtual bool encode(Context &ctx, const ErrorStack &err=ErrorStack());

  protected:
    /** Some offsets within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int scanListCount() { return 0x0000; }
      static constexpr unsigned int scanLists() { return 0x0004; }
      /// @endcond
    };

  public:
    /** Some limits. */
    struct Limit {
      /** Maximum number of scan lists. */
      static constexpr unsigned int scanListCount() { return 10; }
    };
  };


  /** Implements the binary representation of a single message.
   *
   * Memory representation of the message element (44h bytes):
   * @verbinclude dr1801uv_messageelement.txt */
  class MessageElement: public Element
  {
  protected:
    /** Hidden constructor. */
    MessageElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    MessageElement(uint8_t *ptr);

    void clear();
    bool isValid() const;

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x0044; }

    /** Returns the index of the message. */
    virtual unsigned int index() const;
    /** Sets the index of the message. */
    virtual void setIndex(unsigned int index);

    /** Returns the message text. */
    virtual QString text() const;
    /** Sets the message text. */
    virtual void setText(const QString &text);

  protected:
    /** Some offsets within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int index() { return 0x0000; }
      static constexpr unsigned int textLength() { return 0x0001; }
      static constexpr unsigned int text()  { return 0x0004; }
      /// @endcond
    };

  public:
    /** Some limits. */
    struct Limit {
      /** The maximum message length. */
      static constexpr unsigned int textLength() { return 64; }
    };
  };

  /** Implements the binary encoding of the preset message bank element.
   *
   * The message bank contains all messages defined. See @c DR1801UVCodeplug::MessageElement for
   * details.
   *
   * Memory representation of the message bank element (164h bytes):
   * @verbinclude dr1801uv_messagebankelement.txt */
  class MessageBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    MessageBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    MessageBankElement(uint8_t *ptr);

    void clear();

    /** Size of the element. */
    static constexpr unsigned int size() { return 0x00164; }

    /** Retunrs the number of elements in the bank. */
    virtual unsigned int messageCount() const;
    /** Sets the number of messages. */
    virtual void setMessageCount(unsigned int count);

    /** Returns a reference to the n-th message. */
    virtual MessageElement message(unsigned int n) const;

  protected:
    /** Offsets within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int messageCount() { return 0x0000; }
      static constexpr unsigned int messages() { return 0x0004; }
      /// @endcond
    };
  };


  /** Implements the binary encoding of the key settings.
   *
   * Memory representation of the message bank element (0018h bytes):
   * @verbinclude dr1801uv_keysettingselement.txt */
  class KeySettingsElement: public Element
  {
  public:
    /** Possible key functions. */
    enum class Function {
      None = 0, ToggleAlertTones = 1, EmergencyOn = 2, EmergencyOff = 3, TogglePower = 4,
      Monitor = 5, DeleteNuisance = 6, OneTouch1 = 7, OneTouch2 = 8, OneTouch3 = 9, OneTouch4 = 10,
      OneTouch5 = 11, ToggleTalkaround = 13, ToggleScan = 14, ToggleEncryption = 15, ToggleVOX = 16,
      ZoneSelect = 17, ToggleLoneWorker = 19, PhoneExit = 20
    };

  protected:
    /** Hidden constructor. */
    KeySettingsElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    KeySettingsElement(uint8_t *ptr);

    void clear();

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x00018; }

    /** Returns the function for the side-key 1, short press. */
    virtual Function sideKey1Short() const;
    /** Sets the function for the side-key 1, short press. */
    virtual void setSideKey1Short(Function func);
    /** Returns the function for the side-key 1, long press. */
    virtual Function sideKey1Long() const;
    /** Sets the function for the side-key 1, long press. */
    virtual void setSideKey1Long(Function func);
    /** Returns the function for the side-key 2, short press. */
    virtual Function sideKey2Short() const;
    /** Sets the function for the side-key 2, short press. */
    virtual void setSideKey2Short(Function func);
    /** Returns the function for the side-key 2, long press. */
    virtual Function sideKey2Long() const;
    /** Sets the function for the side-key 2, long press. */
    virtual void setSideKey2Long(Function func);
    /** Returns the function for the top-key, short press. */
    virtual Function topKeyShort() const;
    /** Sets the function for the top-key, short press. */
    virtual void setTopKeyShort(Function func);
    /** Returns the function for the top-key, long press. */
    virtual Function topKeyLong() const;
    /** Sets the function for the side-key, long press. */
    virtual void setTopKeyLong(Function func);

  protected:
    /** Internal offsets within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int sideKey1Short() { return 0x0001; }
      static constexpr unsigned int sideKey1Long() { return 0x0002; }
      static constexpr unsigned int sideKey2Short() { return 0x0005; }
      static constexpr unsigned int sideKey2Long() { return 0x0006; }
      static constexpr unsigned int topKeyShort() { return 0x0009; }
      static constexpr unsigned int topKeyLong() { return 0x000a; }
      /// @endcond
    };
  };


  /** Implements the binary encoding of the VFO settings.
   *
   * Memory representation of the VFO settings (0090h bytes):
   * @verbinclude dr1801uv_vfobankelement.txt */
  class VFOBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    VFOBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    VFOBankElement(uint8_t *ptr);

    void clear();

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x0090; }

    /** Returns a reference to the VFO A settings. */
    virtual ChannelElement vfoA() const;
    /** Returns a reference to the VFO A settings. */
    virtual ChannelElement vfoB() const;

    /** Retunrs the name of the first VFO. */
    virtual QString nameA() const;
    /** Sets the name for the first VFO. */
    virtual void setNameA(const QString &name);
    /** Retunrs the name of the second VFO. */
    virtual QString nameB() const;
    /** Sets the name for the second VFO. */
    virtual void setNameB(const QString &name);

  public:
    /** Some limits for the element. */
    struct Limit {
      /** The maximum length of the VFO names (not set). */
      static constexpr unsigned int nameLength() { return 20; }
    };

  protected:
    /** Some offsets within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int vfoA() { return 0x0000; }
      static constexpr unsigned int vfoB() { return 0x0034; }
      static constexpr unsigned int nameA() { return 0x0068; }
      static constexpr unsigned int nameB() { return 0x007c; }
      /// @endcond
    };
  };


  /** Implements the binary encoding of an encryption key.
   *
   * Memory representation of the encryption key bank (000ch bytes):
   * @verbinclude dr1801uv_encryptionkeyelement.txt */
  class EncryptionKeyElement: public Element
  {
  protected:
    /** Hidden constructor. */
    EncryptionKeyElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    EncryptionKeyElement(uint8_t *ptr);

    void clear();
    bool isValid() const;

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x000c; }

    /** Returns the index of the key. */
    virtual unsigned int index() const;
    /** Sets the index of the key. */
    virtual void setIndex(unsigned int index);

    /** Retunrs the length of the key. */
    virtual unsigned int keyLength() const;

    /** Returns the key as a string. */
    virtual QString key() const;
    /** Sets the key. */
    virtual void setKey(const QString &key);

    /** Creates a key object for this element. */
    virtual EncryptionKey *toKeyObj(Context &ctx, const ErrorStack &err=ErrorStack());
    /** Links the key object. */
    virtual bool linkKeyObj(EncryptionKey *obj, Context &ctx, const ErrorStack &err=ErrorStack());
    /** Encodes the key. */
    virtual bool encode(EncryptionKey *obj, Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits for the element. */
    struct Limit {
      /** The maximum length of the key. */
      static constexpr unsigned int keyLength() { return 8; }
    };

  protected:
    /** Some internal offsets within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int index() { return 0x0000; }
      static constexpr unsigned int length() { return 0x0001; }
      static constexpr unsigned int key() { return 0x0004; }
      /// @endcond
    };
  };

  /** Implements the binary encoding of the encryption keys.
   *
   * Memory representation of the encryption key bank (0078h bytes):
   * @verbinclude dr1801uv_encryptionkeybankelement.txt */
  class EncryptionKeyBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    EncryptionKeyBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    EncryptionKeyBankElement(uint8_t *ptr);

    void clear();

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x0078;}

    /** Returns a reference to the encryption key. */
    virtual EncryptionKeyElement key(unsigned int index) const;

    /** Decodes the all encryption keys defined. */
    virtual bool decode(Context &ctx, const ErrorStack &err=ErrorStack()) const;
    /** Links all encryption keys. */
    virtual bool link(Context &ctx, const ErrorStack &err=ErrorStack()) const;
    /** Encodes all keys. */
    virtual bool encode(Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits for the key bank. */
    struct Limit {
      /** The number of keys. */
      static constexpr unsigned int keyCount() { return 10; }
    };
  };


  /** Implements the DTMF system.
   *
   * Memory representation of the DTMF system (000ch bytes):
   * @verbinclude dr1801uv_dtmfsystemelement.txt */
  class DTMFSystemElement: public Element
  {
  protected:
    /** Hidden constructor. */
    DTMFSystemElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    DTMFSystemElement(uint8_t *ptr);

    void clear();

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x0c; }

    /** Returns @c true if the side-tone is enabled. */
    virtual bool sideToneEnabled() const;
    /** Enables/disable side-tone. */
    virtual void enableSideTone(bool enable);

    /** Returns the pre-time in milliseconds. */
    virtual unsigned int preTime() const;
    /** Sets the pre-time in milliseconds. */
    virtual void setPreTime(unsigned int ms);

    /** Returns the code duration in milliseconds. */
    virtual unsigned int codeDuration() const;
    /** Sets the code duration in milliseconds. */
    virtual void setCodeDuration(unsigned int ms);

    /** Returns the code intervall in milliseconds. */
    virtual unsigned int codeItervall() const;
    /** Sets the code intervall in milliseconds. */
    virtual void setCodeItervall(unsigned int ms);

    /** Returns the reset time in milliseconds. */
    virtual unsigned int resetTime() const;
    /** Sets the reset time in milliseconds. */
    virtual void setResetTime(unsigned int ms);

  protected:
    /** Some offsets within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int sideTone() { return 0x0000; }
      static constexpr unsigned int preTime() { return 0x0002; }
      static constexpr unsigned int codeDuration() { return 0x0004; }
      static constexpr unsigned int codeIntervall() { return 0x0006; }
      static constexpr unsigned int resetTime() { return 0x0008; }
      /// @endcond
    };
  };

  /** Implements the DTMF systems bank. Holding all defined DTMF systems
   *  (see @c DTMFSystemElement).
   *
   * Memory representation of the DTMF system bank (0034h bytes):
   * @verbinclude dr1801uv_dtmfsystembankelement.txt */
  class DTMFSystemBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    DTMFSystemBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    DTMFSystemBankElement(uint8_t *ptr);

    void clear();

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x34; }

    /** Returns the number of DTMF systems. */
    virtual unsigned int systemCount() const;
    /** Sets the number of DTMF systems. */
    virtual void setSystemCount(unsigned int count);

    /** Returns a reference to the n-th system. */
    virtual DTMFSystemElement system(unsigned int n) const;

  public:
    /** Some limits. */
    struct Limit {
      /** The maximum number of DTMF systems. */
      static constexpr unsigned int systemCount() { return 4; }
    };

  protected:
    /** Some offsets within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int systemCount() { return 0x0000; }
      static constexpr unsigned int systems()     { return 0x0004; }
      /// @endcond
    };
  };


  /** Implements the DTMF ID.
   *
   * Memory representation of the DTMF ID (014h bytes):
   * @verbinclude dr1801uv_dtmfidelement.txt */
  class DTMFIDElement: public Element
  {
  protected:
    /** Hidden constructor. */
    DTMFIDElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    DTMFIDElement(uint8_t *ptr);

    void clear();

    /** Size of the element. */
    static constexpr unsigned int size() { return 0x0014; }

  public:
    /** Returns the DTMF code/number. */
    virtual QString number() const;
    /** Sets the DTMF code/number. */
    virtual void setNumber(const QString &number);

    /** Some limits. */
    struct Limit {
      /** The maximum number of digits of the number. */
      static constexpr unsigned int numberLength() { return 16; }
    };

  protected:
    /** Returns the length of the number. */
    virtual unsigned int numberLength() const;
    /** Sets the number length. */
    virtual void setNumberLength(unsigned int len);

    /** Some internal offset within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int numberLength() { return 0x0000; }
      static constexpr unsigned int number() { return 0x0004; }
      /// @endcond
    };

    /** Translation table. */
    static QVector<QChar> _bin_dtmf_tab;
  };

  /** Implements the DTMF ID bank. Holding all defined DTMF IDS
   *  (@c see DTMFIDElement).
   *
   * Memory representation of the DTMF ID bank (0144h bytes):
   * @verbinclude dr1801uv_dtmfidbankelement.txt */
  class DTMFIDBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    DTMFIDBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    DTMFIDBankElement(uint8_t *ptr);

    void clear();

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x0144; }

    /** Returns the number of IDs encoded. */
    virtual unsigned int idCount() const;
    /** Sets the ID count. */
    virtual void setIDCount(unsigned int n);

    /** Returns a reference to the n-th ID. */
    virtual DTMFIDElement id(unsigned int n) const;

  public:
    /** Some limits. */
    struct Limit {
      /** The maximum number of IDs. */
      static constexpr unsigned int idCount() { return 16; }
    };

  protected:
    /** Some offsets within the codeplug. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int idCount() { return 0x0000; }
      static constexpr unsigned int ids() { return 0x0004; }
      /// @endcond
    };
  };


  /** Implements the PTT ID.
   *
   * Memory representation of the PTT ID (014h bytes):
   * @verbinclude dr1801uv_pttidelement.txt */
  class PTTIDElement: public Element
  {
  public:
    /** Possible modes of transmission. */
    enum class Transmit {
      None = 0, Start = 1, End = 2, Both = 3
    };
    /** Possible ID modes. */
    enum class IDMode {
      Forbidden = 0, Each = 1, Once = 2
    };

  protected:
    /** Hidden constructor. */
    PTTIDElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    PTTIDElement(uint8_t *ptr);

    void clear();

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x14; }

    /** Returns @c true, if the DTMF system is set. */
    virtual bool hasDTMFSystem() const;
    /** Returns the DTMF system index. */
    virtual unsigned int dtmfSystemIndex() const;
    /** Sets the DTMF system index. */
    virtual void setDTMFSystemIndex(unsigned int index);
    /** Clears the DTMF system. */
    virtual void clearDTMFSystem();

    /** Returns the ID transmission mode. */
    virtual Transmit transmitMode() const;
    /** Sets the ID transmission mode. */
    virtual void setTransmitMode(Transmit mode);

    /** Returns the ID mode. */
    virtual IDMode idMode() const;
    /** Sets the ID mode. */
    virtual void setIDMode(IDMode mode);

    /** Returns @c true if the BOT DTMF ID is set. */
    virtual bool hasBOTID() const;
    /** Returns the BOT DTMF ID index. */
    virtual unsigned int botIDIndex() const;
    /** Sets the BOT DTMF ID index. */
    virtual void setBOTIDIndex(unsigned int index);
    /** Clears the BOT DTMF ID index. */
    virtual void clearBOTID();

    /** Returns @c true if the EOT DTMF ID is set. */
    virtual bool hasEOTID() const;
    /** Returns the EOT DTMF ID index. */
    virtual unsigned int eotIDIndex() const;
    /** Sets the EOT DTMF ID index. */
    virtual void setEOTIDIndex(unsigned int index);
    /** Clears the EOT DTMF ID index. */
    virtual void clearEOTID();

  protected:
    /** Some internal offsets within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int dtmfSystemIndex() { return 0x0000; }
      static constexpr unsigned int transmitMode()    { return 0x0001; }
      static constexpr unsigned int idMode()          { return 0x0002; }
      static constexpr unsigned int botIDIndex()      { return 0x0003; }
      static constexpr unsigned int eotIDIndex()      { return 0x0004; }
      /// @endcond
    };
  };

  /** Implements the encoding of the DTMF PTT IDs. Holding all PTT IDs
   *  (see @c PTTIDElement).
   *
   * Memory representation of the PTT ID bank (0104h bytes):
   * @verbinclude dr1801uv_pttidbankelement.txt */
  class PTTIDBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    PTTIDBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    PTTIDBankElement(uint8_t *ptr);

    void clear();

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x0104; }

    /** Returns the number of PTT IDs defined. */
    virtual unsigned int idCount() const;
    /** Set the numbe of PTT IDs. */
    virtual void setPTTIDCount(unsigned int n);

    /** Returns a reference to the n-th PTT ID. */
    virtual PTTIDElement pttID(unsigned int n);

  public:
    /** Some limits. */
    struct Limit {
      /** The maximum number of PTT IDs. */
      static constexpr unsigned int idCount() { return 16; }
    };

  protected:
    /** Some internal offsets within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int idCount() { return 0x0000; }
      static constexpr unsigned int ids()     { return 0x0004; }
      /// @endcond
    };
  };


  /** Implements the binary encoding of the DTMF signaling settings.
   *
   * Memory representation of the settings (029ch bytes):
   * @verbinclude dr1801uv_dtmfsettingselement.txt */
  class DTMFSettingsElement: public Element
  {
  public:
    /** Possible DTMF non-number codes. Usually used as separator and group codes. */
    enum class NonNumber {
      None = 0, A=0xa, B=0xb, C=0xc, D=0xd, Asterisk=0xe, Gate=0xf
    };
    /** Possible responses. */
    enum class Response {
      None=0, Reminder=1, Reply=2, Both=3
    };
    /** Possible kill actions. */
    enum class Kill {
      DisableTX = 0, DisableRXandTX = 1
    };

  protected:
    /** Hidden constructor. */
    DTMFSettingsElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    DTMFSettingsElement(uint8_t *ptr);

    void clear();

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x029c; }

    /** Retunrs the DTMF radio ID as a string. */
    virtual QString radioID() const;
    /** Sets the DTMF radio ID as a string. */
    virtual void setRadioID(const QString &id);
    /** Retunrs the DTMF kill code as a string. */
    virtual QString killCode() const;
    /** Sets the DTMF kill code as a string. */
    virtual void setKillCode(const QString &code);
    /** Retunrs the DTMF wake code as a string. */
    virtual QString wakeCode() const;
    /** Sets the DTMF wake code as a string. */
    virtual void setWakeCode(const QString &code);

    /** Returns the delimiter code. */
    virtual NonNumber delimiter() const;
    /** Sets the delimiter code. */
    virtual void setDelimiter(NonNumber code);
    /** Returns the group code. */
    virtual NonNumber groupCode() const;
    /** Sets the group code. */
    virtual void setGroupCode(NonNumber code);

    /** Returns the decode response. */
    virtual Response response() const;
    /** Sets the decode response. */
    virtual void setResponse(Response resp);

    /** Returns the auto-reset time in seconds [5,60s]. */
    virtual unsigned int autoResetTime() const;
    /** Set the auto-reset time in seconds. */
    virtual void setAutoResetTime(unsigned int sec);

    /** Returns @c true if the kill/wake decoding is endabled. */
    virtual bool killWakeEnabled() const;
    /** Enables/disables the kill/wake decoding. */
    virtual void enableKillWake(bool enable);

    /** Returns the kill type. */
    virtual Kill killType() const;
    /** Sets the kill type. */
    virtual void setKillType(Kill type);

    /** Returns a reference to the DTMF systems. */
    virtual DTMFSystemBankElement dtmfSystems() const;
    /** Returns a reference to the DTMF IDs. */
    virtual DTMFIDBankElement dtmfIDs() const;
    /** Retunrs a reference to the PTT ID bank. */
    virtual PTTIDBankElement pttIDs() const;

  public:
    /** Some limits. */
    struct Limit {
      /** The maximum length of the radio ID. */
      static constexpr unsigned int radioIDLength() { return 5; }
      /** The maximum length of the kill code. */
      static constexpr unsigned int killCodeLength() { return 6; }
      /** The maximum length of the wake code. */
      static constexpr unsigned int wakeCodeLength() { return 6; }
    };

  protected:
    /** Internal offsets within the settings element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int radioID() { return 0x0000; }
      static constexpr unsigned int radioIDLength() { return 0x0005; }
      static constexpr unsigned int killCode() { return 0x0008; }
      static constexpr unsigned int killCodeLength() { return 0x000e; }
      static constexpr unsigned int wakeCode() { return 0x0010; }
      static constexpr unsigned int wakeCodeLength() { return 0x0016; }
      static constexpr unsigned int delimiter() { return 0x0018; }
      static constexpr unsigned int groupCode() { return 0x0019; }
      static constexpr unsigned int response() { return 0x001a; }
      static constexpr unsigned int autoResetTime() { return 0x001b; }
      static constexpr unsigned int killWake() { return 0x001c; }
      static constexpr unsigned int killType() { return 0x001d; }
      static constexpr unsigned int dtmfSystems() { return 0x0020; }
      static constexpr unsigned int dtmfIDs() { return 0x0054; }
      static constexpr unsigned int pttIDs() { return 0x0198; }
      /// @endcond
    };
  };


  /** Implements the binary encoding of the alarm system.
   *
   * Memory representation of the alarm system (0018h bytes):
   * @verbinclude dr1801uv_alarmsystembankelement.txt */
  class AlarmSystemElement: public Element
  {
  protected:
    /** Hidden constructor. */
    AlarmSystemElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    AlarmSystemElement(uint8_t *ptr);

    void clear();
    bool isValid() const;

    /** The size of the alarm system element. */
    static constexpr unsigned int size() { return 0x0018; }

    /** Returns the index of the system. */
    virtual unsigned int index() const;
    /** Sets the index of the element. */
    virtual void setIndex(unsigned int index);
    /** Clears the index. */
    virtual void clearIndex();

    /** Returns @c true if the alarm is enabled. */
    virtual bool alarmEnabled() const;
    /** Enables/disables the alarm. */
    virtual void enableAlarm(bool enable);

    /** Returns @c true if no alarm channel is specified. */
    virtual bool noAlarmChannel() const;
    /** Returns @c true if the alarm channel is the current channel. */
    virtual bool alarmChannelIsSelected() const;
    /** Returns the index of the alarm channel. */
    virtual unsigned int alarmChannelIndex() const;
    /** Sets the alarm channel index. */
    virtual void setAlarmChannelIndex(unsigned int index);
    /** Sets the alarm channel to the selected chanel. */
    virtual void setAlarmChannelSelected();
    /** Clears the alarm channel. */
    virtual void clearAlarmChannel();

    /** Returns the name. */
    virtual QString name() const;
    /** Sets the system name. */
    virtual void setName(const QString &name);

  public:
    /** Some limits. */
    struct Limit {
      /** The maximum name length. */
      static constexpr unsigned int nameLength() { return 16; }
    };

  protected:
    /** Some internal offsets within the codeplug. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int index()             { return 0x0000; }
      static constexpr unsigned int alarmEnabled()      { return 0x0001; }
      static constexpr unsigned int alarmChannelIndex() { return 0x0004; }
      static constexpr unsigned int name()              { return 0x0008; }
      /// @endcond
    };
  };

  /** Implements the binary encoding of the alarm system bank.
   *
   * Holding all alarm systems, see @c AlarmSystemElement.
   *
   * Memory representation of the alarm system bank (00c4h bytes):
   * @verbinclude dr1801uv_alarmsystembankelement.txt */
  class AlarmSystemBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    AlarmSystemBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    AlarmSystemBankElement(uint8_t *ptr);

    void clear();

    /** The size of the alarm system bank. */
    static constexpr unsigned int size() { return 0x00c4; }

    /** Returns the number of alarm systems defined. */
    virtual unsigned int alarmSystemCount() const;
    /** Sets the number of alarm systems. */
    virtual void setAlarmSystemCount(unsigned int n);

    /** Returns a reference to the n-th alarm system. */
    virtual AlarmSystemElement alarmSystem(unsigned int n) const;

  public:
    /** Some limits. */
    struct Limit {
      /** The maximum number of alarm systems. */
      static constexpr unsigned int alarmSystemCount() { return 8; }
    };

  protected:
    /** Some internal offsets within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int alarmSystemCount() { return 0x0000; }
      static constexpr unsigned int alarmSystems() { return 0x0004; }
      /// @endcond
    };
  };


  /** Implements the binary encoding of the DMR settings.
   *
   * Memory representation of the DMR settings (0010h bytes):
   * @verbinclude dr1801uv_dmrsettingselement.txt */
  class DMRSettingsElement: public Element
  {
  public:
    /** Possible SMS encodings. */
    enum class SMSFormat {
      CompressedIP = 0, DefinedData = 1, IPData = 2
    };

  protected:
    /** Hidden constructor. */
    DMRSettingsElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    DMRSettingsElement(uint8_t *ptr);

    void clear();

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x0010; }

    /** Returns the hold-time in seconds. */
    virtual unsigned int holdTime() const;
    /** Sets the hold-time in seconds. */
    virtual void setHoldTime(unsigned int sec);

    /** Returns the remote-listen duration in seconds. */
    virtual unsigned int remoteListen() const;
    /** Sets the remote-listen duration in seconds. */
    virtual void setRemoteListen(unsigned int sec);

    /** Returns the active wait period in ms. */
    virtual unsigned int activeWait() const;
    /** Sets the active wait period in ms. */
    virtual void setActiveWait(unsigned int ms);

    /** Returns the active resend count. */
    virtual unsigned int activeResend() const;
    /** Sets the active resend cound. */
    virtual void setActiveResend(unsigned int count);

    /** Returns the pre-send count. */
    virtual unsigned int preSend() const;
    /** Sets the pre-send count. */
    virtual void setPreSend(unsigned int count);

    /** Returns the voice head count. */
    virtual unsigned int voiceHead() const;
    /** Sets the voice head count. */
    virtual void setVoiceHead(unsigned int count);

    /** Returns the SMS format. */
    virtual SMSFormat smsFormat() const;
    /** Sets the SMS format. */
    virtual void setSMSFormat(SMSFormat format);

    /** Returns @c true if the kill encoding is enabled. */
    virtual bool killEnc() const;
    /** Enables kill encoding. */
    virtual void enableKillEnc(bool enable);
    /** Returns @c true if the kill decoding is enabled. */
    virtual bool killDec() const;
    /** Enables kill decoding. */
    virtual void enableKillDec(bool enable);

    /** Returns @c true if the active encoding is enabled. */
    virtual bool activeEnc() const;
    /** Enables active encoding. */
    virtual void enableActiveEnc(bool enable);
    /** Returns @c true if the active decoding is enabled. */
    virtual bool activeDec() const;
    /** Enables active decoding. */
    virtual void enableActiveDec(bool enable);

    /** Returns @c true if the check encoding is enabled. */
    virtual bool checkEnc() const;
    /** Enables check encoding. */
    virtual void enableCheckEnc(bool enable);
    /** Returns @c true if the check decoding is enabled. */
    virtual bool checkDec() const;
    /** Enables check decoding. */
    virtual void enableCheckDec(bool enable);

    /** Returns @c true if the call alter encoding is enabled. */
    virtual bool callAlterEnc() const;
    /** Enables call alter encoding. */
    virtual void enableCallAlterEnc(bool enable);
    /** Returns @c true if the call alter decoding is enabled. */
    virtual bool callAlterDec() const;
    /** Enables call alter decoding. */
    virtual void enableCallAlterDec(bool enable);

    /** Returns @c true if the remote monitor encoding is enabled. */
    virtual bool remoteMonitorEnc() const;
    /** Enables remote monitor encoding. */
    virtual void enableRemoteMonitorEnc(bool enable);
    /** Returns @c true if the remote monitor decoding is enabled. */
    virtual bool remoteMonitorDec() const;
    /** Enables remote monitor decoding. */
    virtual void enableRemoteMonitorDec(bool enable);

  public:
    /** Some limits. */
    struct Limit: public Element::Limit {
      /** The range of hold time. */
      static constexpr Range<unsigned int> holdTime() { return {1, 90}; }
      /** The range of remote listen duration. */
      static constexpr Range<unsigned int> remoteListen() { return {10, 120}; }
      /** The range of active wait period. */
      static constexpr Range<unsigned int> activeWait() { return {120, 600}; }
      /** The range of active resend count. */
      static constexpr Range<unsigned int> activeResend() { return {1, 10}; }
      /** The range of pre-send count. */
      static constexpr Range<unsigned int> preSend() { return {4, 15}; }
      /** The range of voice head count. */
      static constexpr Range<unsigned int> voiceHead() { return {0, 20}; }
    };

  protected:
    /** Internal offsets within the element. */
    struct Offset: public Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int holdTime() { return 0x0000; }
      static constexpr unsigned int remoteListen() { return 0x0001; }
      static constexpr unsigned int activeWait() { return 0x0002; }
      static constexpr unsigned int activeResend() { return 0x0003; }
      static constexpr unsigned int preSend() { return 0x0004; }
      static constexpr unsigned int killEnc() { return 0x0005; }
      static constexpr unsigned int activeEnc() { return 0x0006; }
      static constexpr unsigned int checkEnc() { return 0x0007; }
      static constexpr unsigned int killDec() { return 0x0008; }
      static constexpr unsigned int activeDec() { return 0x0009; }
      static constexpr unsigned int checkDec() { return 0x000a; }
      static constexpr unsigned int smsFormat() { return 0x000b; }
      static constexpr unsigned int voiceHead() { return 0x000c; }
      static constexpr BitOffset callAlterEnc() { return {0x000d, 0}; }
      static constexpr BitOffset callAlterDec() { return {0x000d, 1}; }
      static constexpr BitOffset remoteMonitorEnc() { return {0x000d, 2}; }
      static constexpr BitOffset remoteMonitorDec() { return {0x000d, 3}; }
      /// @endcond
    };
  };


  /** Implements the binary encoding of a one-touch setting.
   *
   * Memory representation of the one-touch setting (0008h bytes):
   * @verbinclude dr1801uv_onetouchsettingelement.txt */
  class OneTouchSettingElement: public Element
  {
  public:
    /** Possible actions to perform. */
    enum class Action {
      Call = 0, Message = 1
    };
    /** Possible one-touch types. */
    enum class Type {
      Disabled = 0, DMR=1, FM=2
    };

  protected:
    /** Hidden constructor. */
    OneTouchSettingElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    OneTouchSettingElement(uint8_t *ptr);

    void clear();

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x0008; }

    /** Returns @c true if the setting is enabled. */
    bool isValid() const;

    /** Returns @c true if a contact is set. */
    virtual bool hasContact() const;
    /** Returns the DMR contact index. */
    virtual unsigned int contactIndex() const;
    /** Sets the contact index. */
    virtual void setContactIndex(unsigned int index);
    /** Clears the contact. */
    virtual void clearContact();

    /** Returns the one-touch action. */
    virtual Action action() const;
    /** Sets the one-touch action. */
    virtual void setAction(Action action);

    /** Returns @c true, if a message is set. */
    virtual bool hasMessage() const;
    /** Returns the message index. */
    virtual unsigned int messageIndex() const;
    /** Sets the message index. */
    virtual void setMessageIndex(unsigned int index);
    /** Clears the message. */
    virtual void clearMessage();

    /** Retuns the type of the one-touch setting. */
    virtual Type type() const;
    /** Sets the type of the one-touch setting. */
    virtual void setType(Type type);

    /** Returns @c true if a DTMF ID is set. */
    virtual bool hasDTMFID() const;
    /** Returns the DTMF ID index. */
    virtual unsigned int dtmfIDIndex() const;
    /** Sets the DTMF ID index. */
    virtual void setDTMFIDIndex(unsigned int index);
    /** Clears the DTMF ID index. */
    virtual void clearDTMFIDIndex();

  protected:
    /** Some internal offsets. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int contactIndex() { return 0x0000; }
      static constexpr unsigned int action() { return 0x0002; }
      static constexpr unsigned int messageIndex() { return 0x0003; }
      static constexpr unsigned int type() { return 0x0004; }
      static constexpr unsigned int dtmfIDIndex() { return 0x0005; }
      /// @endcond
    };
  };

  /** Implements the binary encoding of the one-touch settings.
   *
   * Holding all one-touch settings, see @c OneTouchSettingElement.
   *
   * Memory representation of the one-touch settings (0028h bytes):
   * @verbinclude dr1801uv_onetouchsettingselement.txt */
  class OneTouchSettingsElement: public Element
  {
  protected:
    /** Hidden constructor. */
    OneTouchSettingsElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    OneTouchSettingsElement(uint8_t *ptr);

    void clear();

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x0028; }

    /** Returns the number of one-touch settings. */
    virtual unsigned int settingsCount() const;

    /** Returns a reference to the n-th one-touch setting. */
    virtual OneTouchSettingElement setting(unsigned int n) const;

  public:
    /** Some limits. */
    struct Limit {
      /** Returns the maximum number of one-touch settings. */
      static constexpr unsigned int settingsCount() { return 5; }
    };

  protected:
    /** Some internal offset. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int settings() { return 0x0000; }
      /// @endcond
    };
  };


public:
  /** Default constructor. */
  explicit DR1801UVCodeplug(QObject *parent = nullptr);

  bool index(Config *config, Context &ctx, const ErrorStack &err=ErrorStack()) const;
  bool encode(Config *config, const Flags &flags, const ErrorStack &err=ErrorStack());
  bool decode(Config *config, const ErrorStack &err=ErrorStack());

protected:
  /** Decode codeplug elements. */
  virtual bool decodeElements(Context &ctx, const ErrorStack &err=ErrorStack());
  /** Link decoded elements. */
  virtual bool linkElements(Context &ctx, const ErrorStack &err=ErrorStack());
  /** Encode all elements. */
  virtual bool encodeElements(Context &ctx, const ErrorStack &err=ErrorStack());

protected:
  /** Defines the offsets within the codeplug. */
  struct Offset {
    /// @cond DO_NOT_DOCUMENT
    static constexpr unsigned int size()              { return 0x1dd90; }
    static constexpr unsigned int settings()          { return 0x003b4; }
    static constexpr unsigned int zoneBank()          { return 0x00418; }
    static constexpr unsigned int messageBank()       { return 0x04110; }
    static constexpr unsigned int contactBank()       { return 0x04334; }
    static constexpr unsigned int scanListBank()      { return 0x0a338; }
    static constexpr unsigned int channelBank()       { return 0x0a65c; }
    static constexpr unsigned int keySettings()       { return 0x1c6c4; }
    static constexpr unsigned int groupListBank()     { return 0x1c6dc; }
    static constexpr unsigned int encryptionKeyBank() { return 0x1d7e0; }
    static constexpr unsigned int dtmfSettings()      { return 0x1d858; }
    static constexpr unsigned int alarmSettings()     { return 0x1daf4; }
    static constexpr unsigned int vfoBank()           { return 0x1dd00; }
    /// @endcond
  };
};

#endif // DR1801UVCODEPLUG_HH
