#ifndef RADIODDITYCODEPLUG_HH
#define RADIODDITYCODEPLUG_HH

#include "codeplug.hh"
#include "signaling.hh"
#include "channel.hh"
#include "contact.hh"

class DigitalContact;
class Zone;
class RXGroupList;
class ScanList;


/** Base class of all Radioddity codeplugs. This class implements the majority of all codeplug
 * elements present in all Radioddity codeplugs (also some derivatives like OpenGD77). This eases
 * the support of several Radioddity radios, as only the differences in the codeplug to this base
 * class must be implemented.
 *
 * @ingroup radioddity */
class RadioddityCodeplug : public CodePlug
{
  Q_OBJECT

public:
  /** Implements the base for all Radioddity channel encodings.
   *
   * Memory layout of encoded channel:
   * @verbinclude radioddity_channel.txt */
  class ChannelElement: public CodePlug::Element
  {
  public:
    /** Possible channel types. */
    enum Mode {
      MODE_ANALOG = 0,   ///< Analog channel, aka FM.
      MODE_DIGITAL = 1   ///< Digital channel, aka DMR.
    };

    /** Possible admit criteria. */
    enum Admit {
      ADMIT_ALWAYS = 0,  ///< Allow always.
      ADMIT_CH_FREE = 1, ///< Allow TX on channel free.
      ADMIT_COLOR = 2    ///< Allow TX on matching color-code.
    };

    /** Possible privacy groups, not used in ham radio. */
    enum PrivacyGroup {
      PRIVGR_NONE = 0,     ///< No privacy group, default.
      PRIVGR_53474C39 = 1  ///< Privacy group 53474C39 (wtf?).
    };

  protected:
    /** Constructs a channel from the given memory. */
    ChannelElement(uint8_t *ptr, size_t size);

  public:
    /** Constructs a channel from the given memory. */
    explicit ChannelElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~ChannelElement();

    /** Resets the channel. */
    virtual void clear();

    /** Returns the name of the channel. */
    virtual QString name() const;
    /** Sets the name of the channel. */
    virtual void setName(const QString &n);

    /** Returns the RX frequency of the channel. */
    virtual uint32_t rxFrequency() const;
    /** Sets the RX frequency of the channel. */
    virtual void setRXFrequency(uint32_t freq);
    /** Returns the TX frequency of the channel. */
    virtual uint32_t txFrequency() const;
    /** Sets the TX frequency of the channel. */
    virtual void setTXFrequency(uint32_t freq);

    /** Returns the channel mode. */
    virtual Mode mode() const;
    /** Sets the channel mode. */
    virtual void setMode(Mode mode);

    /** Returns the TX timeout in seconds. A value of 0 means disabled. */
    virtual uint txTimeOut() const;
    /** Sets the TX timeout in seconds. Setting it to 0 disables the timeout. */
    virtual void setTXTimeOut(uint tot);
    /** Returns the transmit time-out re-key delay in seconds. */
    virtual uint txTimeOutRekeyDelay() const;
    /** Sets the transmit time-out re-key delay in seconds. */
    virtual void setTXTimeOutRekeyDelay(uint delay);

    /** Retunrs the admit criterion. */
    virtual Admit admitCriterion() const;
    /** Sets the admit criterion. */
    virtual void setAdmitCriterion(Admit admit);

    /** Returns the scan list index (+1). */
    virtual uint scanListIndex() const;
    /** Sets the scan list index (+1). */
    virtual void setScanListIndex(uint index);

    /** Returns the RX subtone. */
    virtual Signaling::Code rxTone() const;
    /** Sets the RX subtone. */
    virtual void setRXTone(Signaling::Code code);
    /** Returns the TX subtone. */
    virtual Signaling::Code txTone() const;
    /** Sets the TX subtone. */
    virtual void setTXTone(Signaling::Code code);

    /** Returns TX signaling index (+1). */
    virtual uint txSignalingIndex() const;
    /** Sets TX signaling index (+1). */
    virtual void setTXSignalingIndex(uint index);
    /** Returns RX signaling index (+1). */
    virtual uint rxSignalingIndex() const;
    /** Sets RX signaling index (+1). */
    virtual void setRXSignalingIndex(uint index);

    /** Returns the privacy group. */
    virtual PrivacyGroup privacyGroup() const;
    /** Sets the privacy group. */
    virtual void setPrivacyGroup(PrivacyGroup grp);

    /** Returns the TX color code. */
    virtual uint txColorCode() const;
    /** Sets the TX color code. */
    virtual void setTXColorCode(uint cc);

    /** Returns the group-list index (+1). */
    virtual uint groupListIndex() const;
    /** Sets the group-list index (+1). */
    virtual void setGroupListIndex(uint index);

    /** Returns the RX color code. */
    virtual uint rxColorCode() const;
    /** Sets the RX color code. */
    virtual void setRXColorCode(uint cc);

    /** Returns the emergency system index (+1). */
    virtual uint emergencySystemIndex() const;
    /** Sets the emergency system index (+1). */
    virtual void setEmergencySystemIndex(uint index);

    /** Returns the transmit contact index (+1). */
    virtual uint contactIndex() const;
    /** Sets the transmit contact index (+1). */
    virtual void setContactIndex(uint index);

    /** Returns @c true if data-call-confirm is enabled. */
    virtual bool dataCallConfirm() const;
    /** Enables/disables data-call-confirm. */
    virtual void enableDataCallConfirm(bool enable);
    /** Returns @c true if emergency alarm ACK is enabled. */
    virtual bool emergencyAlarmACK() const;
    /** Enables/disables emergency alarm ACK. */
    virtual void enableEmergencyAlarmACK(bool enable);
    /** Returns @c true if private-call-confirm is enabled. */
    virtual bool privateCallConfirm() const;
    /** Enables/disables private-call-confirm. */
    virtual void enablePrivateCallConfirm(bool enable);
    /** Returns @c true if privacy is enabled. */
    virtual bool privacyEnabled() const;
    /** Enables/disables privacy. */
    virtual void enablePrivacy(bool enable);

    /** Returns the time slot of the channel. */
    virtual DigitalChannel::TimeSlot timeSlot() const;
    /** Sets the time slot of the channel. */
    virtual void setTimeSlot(DigitalChannel::TimeSlot ts);

    /** Returns @c true if the dual-capacity direct mode is enabled. */
    virtual bool dualCapacityDirectMode() const;
    /** Enables/disables the dual-capacity direct mode. */
    virtual void enableDualCapacityDirectMode(bool enable);
    /** Returns @c true if non-STE is frequency (?!). */
    virtual bool nonSTEFrequency() const;
    /** Enables/disables non-STE is frequency (?!). */
    virtual void enableNonSTEFrequency(bool enable);

    /** Returns the bandwidth. */
    virtual AnalogChannel::Bandwidth bandwidth() const;
    /** Sets the bandwidth. */
    virtual void setBandwidth(AnalogChannel::Bandwidth bw);

    /** Returns @c true if RX only is enabled. */
    virtual bool rxOnly() const;
    /** Enables/disables RX only. */
    virtual void enableRXOnly(bool enable);
    /** Returns @c true if talkaround is enabled. */
    virtual bool talkaround() const;
    /** Enables/disables talkaround. */
    virtual void enableTalkaround(bool enable);
    /** Returns @c true if VOX is enabled. */
    virtual bool vox() const;
    /** Enables/disables VOX. */
    virtual void enableVOX(bool enable);

    /** Returns the power setting of the channel. */
    virtual Channel::Power power() const;
    /** Sets the power setting of the channel. */
    virtual void setPower(Channel::Power pwr);

    /** Returns the squelch level. */
    virtual uint squelch() const;
    /** Sets the squelch level. */
    virtual void setSquelch(uint level);

    /** Constructs a generic @c Channel object from the codeplug channel. */
    virtual Channel *toChannelObj(Context &ctx) const;
    /** Links a previously constructed channel to the rest of the configuration. */
    virtual bool linkChannelObj(Channel *c, Context &ctx) const;
    /** Initializes this codeplug channel from the given generic configuration. */
    virtual bool fromChannelObj(const Channel *c, Context &ctx);
  };

  /** Implements the base for channel banks in Radioddity codeplugs.
   *
   * Memory layout of a channel bank:
   * @verbinclude radioddity_channelbank.txt */
  class ChannelBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ChannelBankElement(uint8_t *ptr, uint size);

  public:
    /** Constrcutor. */
    explicit ChannelBankElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~ChannelBankElement();

    /** Clears the bank. */
    void clear();

    /** Retruns @c true if the channel is enabled. */
    virtual bool isEnabled(uint idx) const ;
    /** Enable/disable a channel in the bank. */
    virtual void enable(uint idx, bool enabled);
    /** Returns a pointer to the channel at the given index. */
    virtual uint8_t *get(uint idx) const;
  };

  /** Implements the base for digital contacts in Radioddity codeplugs.
   *
   * Memory layout of a digital contact:
   * @verbinclude radioddity_contact.txt */
  class ContactElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ContactElement(uint8_t *ptr, uint size);

  public:
    /** Constructor. */
    explicit ContactElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~ContactElement();

    /** Resets the contact. */
    void clear();
    /** Returns @c true if the contact is valid. */
    bool isValid() const;

    /** Returns the name of the contact. */
    virtual QString name() const;
    /** Sets the name of the contact. */
    virtual void setName(const QString name);

    /** Returns the DMR number of the contact. */
    virtual uint number() const;
    /** Sets the DMR number of the contact. */
    virtual void setNumber(uint id);

    /** Returns the call type. */
    virtual DigitalContact::Type type() const;
    /** Sets the call type. */
    virtual void setType(DigitalContact::Type type);

    /** Returns @c true if the ring tone is enabled for this contact. */
    virtual bool ring() const;
    /** Enables/disables ring tone for this contact. */
    virtual void enableRing(bool enable);

    /** Returns the ring tone style for this contact [0-10]. */
    virtual uint ringStyle() const;
    /** Sets the ring tone style for this contact [0-10]. */
    virtual void setRingStyle(uint style);

    /** Constructs a @c DigitalContact instance from this codeplug contact. */
    virtual DigitalContact *toContactObj(Context &ctx) const;
    /** Resets this codeplug contact from the given @c DigitalContact. */
    virtual void fromContactObj(const DigitalContact *obj, Context &ctx);
  };

  /** Implements a base DTMF (analog) contact for Radioddity codeplugs.
   *
   * Memmory layout of the DTMF contact:
   * @verbinclude radioddity_dtmfcontact.txt
   */
  class DTMFContactElement: Element
  {
  protected:
    /** Hidden constructor. */
    DTMFContactElement(uint8_t *ptr, uint size);

  public:
    /** Constructor. */
    explicit DTMFContactElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~DTMFContactElement();

    /** Resets the contact. */
    void clear();
    /** Returns @c true if the contact is valid. */
    bool isValid() const;

    /** Returns the name of the contact. */
    virtual QString name() const;
    /** Sets the name of the conact. */
    virtual void setName(const QString &name);

    /** Returns the number of the contact. */
    virtual QString number() const;
    /** Sets the number of the contact. */
    virtual void setNumber(const QString &number);

    /** Constructs a @c DTMFContact instance from this codeplug contact. */
    virtual DTMFContact *toContactObj(Context &ctx) const;
    /** Resets this codeplug contact from the given @c DTMFContact. */
    virtual void fromContactObj(const DTMFContact *obj, Context &ctx);
  };

  /** Represents a zone within Radioddity codeplugs.
   *
   * Memmory layout of the zone:
   * @verbinclude radioddity_zone.txt
   */
  class ZoneElement: Element
  {
  protected:
    /** Hidden constructor. */
    ZoneElement(uint8_t *ptr, uint size);

  public:
    /** Constructor. */
    explicit ZoneElement(uint8_t *ptr);
    virtual ~ZoneElement();

    /** Resets the zone. */
    void clear();
    /** Returns @c true if the zone is valid. */
    bool isValid() const;

    /** Returns the name of the zone. */
    virtual QString name() const;
    /** Sets the name of the zone. */
    virtual void setName(const QString &name);

    /** Returns @c true if a member is stored at the given index.
     * That is, if the index is not 0. */
    virtual bool hasMember(uint n) const;
    /** Returns the n-th member index (+1). */
    virtual uint member(uint n) const;
    /** Sets the n-th member index (+1). */
    virtual void setMember(uint n, uint idx);
    /** Clears the n-th member index. */
    virtual void clearMember(uint n);

    /** Constructs a generic @c Zone object from this codeplug zone. */
    virtual Zone *toZoneObj(Context &ctx) const;
    /** Links a previously constructed @c Zone object to the rest of the configuration. That is
     * linking to the referred channels. */
    virtual bool linkZoneObj(Zone *zone, Context &ctx, bool putInB) const;
    /** Resets this codeplug zone representation from the given generic @c Zone object. */
    virtual void fromZoneObjA(const Zone *zone, Context &ctx);
    /** Resets this codeplug zone representation from the given generic @c Zone object. */
    virtual void fromZoneObjB(const Zone *zone, Context &ctx);
  };

  /** Implements the base class for all zone banks of Radioddity codeplugs.
   *
   * Memmory layout of the zone table/bank:
   * @verbinclude radioddity_zonebank.txt
   */
  class ZoneBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ZoneBankElement(uint8_t *ptr, uint size);

  public:
    /** Constuctor. */
    explicit ZoneBankElement(uint8_t *ptr);
    /** Destructor. */
    ~ZoneBankElement();

    /** Resets the bank. */
    void clear();

    /** Retruns @c true if the channel is enabled. */
    virtual bool isEnabled(uint idx) const ;
    /** Enable/disable a channel in the bank. */
    virtual void enable(uint idx, bool enabled);
    /** Returns a pointer to the channel at the given index. */
    virtual uint8_t *get(uint idx) const;
  };

  /** Represents a base class for all group lists within Radioddity codeplugs.
   *
   * Memmory layout of the RX group list:
   * @verbinclude radioddity_grouplist.txt
   */
  class GroupListElement: public Element
  {
  protected:
    /** Hidden constructor. */
    GroupListElement(uint8_t *ptr, uint size);

  public:
    /** Constructor. */
    explicit GroupListElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~GroupListElement();

    /** Resets the group list. */
    void clear();

    /** Returns the name of the group list. */
    virtual QString name() const;
    /** Sets the name of the group list. */
    virtual void setName(const QString &name);

    /** Returns @c true if the group list has an n-th member.
     * That is if the n-th index is not 0. */
    virtual bool hasMember(uint n) const;
    /** Returns the n-th member index (+1). */
    virtual uint member(uint n) const;
    /** Sets the n-th member index (+1). */
    virtual void setMember(uint n, uint idx);
    /** Clears the n-th member index. */
    virtual void clearMember(uint n);

    /** Constructs a @c RXGroupList object from the codeplug representation. */
    virtual RXGroupList *toRXGroupListObj(Context &ctx);
    /** Links a previously constructed @c RXGroupList to the rest of the generic configuration. */
    virtual bool linkRXGroupListObj(int ncnt, RXGroupList *lst, Context &ctx) const;
    /** Reset this codeplug representation from a @c RXGroupList object. */
    virtual void fromRXGroupListObj(const RXGroupList *lst, Context &ctx);
  };

  /** Implements a base class of group list memory banks for all Radioddity codeplugs.
   *
   * Memmory layout of the group list table:
   * @verbinclude radioddity_grouplistbank.txt
   */
  class GroupListBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    GroupListBankElement(uint8_t *ptr, uint size);

  public:
    /** Constructor. */
    explicit GroupListBankElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~GroupListBankElement();

    /** Resets the bank. */
    void clear();

    /** Returns @c true if the n-th group list is enabled. */
    virtual bool isEnabled(uint n) const;
    /** Returns the number of contacts in the n-th group list. */
    virtual uint contactCount(uint n) const;
    /** Sets the number of contacts in the n-th group list.
     * This also enables the n-th group list. */
    virtual void setContactCount(uint n, uint size);
    /** Disables the n-th group list. */
    virtual void disable(uint n);

    /** Returns a pointer to the n-th group list. */
    virtual uint8_t *get(uint n) const;
  };

  /** Implements the base class for scan lists of all Radioddity codeplugs.
   *
   * Memmory layout of the scan list.
   * @verbinclude radioddity_scanlist.txt
   */
  class ScanListElement: public Element
  {
  public:
    /** Possible priority channel types. */
    enum Mode {
      PL_NONPRI = 0,              ///< Only non-priority channels.
      PL_DISABLE = 1,             ///< Disable priority channels.
      PL_PRI = 2,                 ///< Only priority channels.
      PL_PRI_NONPRI = 3           ///< Priority and non-priority channels.
    };

  protected:
    /** Hidden constructor. */
    ScanListElement(uint8_t *ptr, uint size);

  public:
    /** Constructor. */
    explicit ScanListElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~ScanListElement();

    /** Resets the scan list. */
    void clear();

    /** Returns the name of the scan list. */
    virtual QString name() const;
    /** Sets the name of the scan list. */
    virtual void setName(const QString &name);

    /** Returns @c true if channel mark is enabled. */
    virtual bool channelMark() const;
    /** Enables/disables channel mark. */
    virtual void enableChannelMark(bool enable);

    /** Returns the scan mode. */
    virtual Mode mode() const;
    /** Sets the scan mode. */
    virtual void setMode(Mode mode);

    /** Returns @c true if talk back is enabled. */
    virtual bool talkback() const;
    /** Enables/disables talk back. */
    virtual void enableTalkback(bool enable);

    /** Returns @c true if the n-th member is set. */
    virtual bool hasMember(uint n) const;
    /** Returns @c true if the n-th member is selected channel. */
    virtual bool isSelected(uint n) const;
    /** Retunrs the n-th member index. */
    virtual uint member(uint n) const;
    /** Sets the n-th member index. */
    virtual void setMember(uint n, uint idx);
    /** Sets the n-th member to be the selected channel. */
    virtual void setSelected(uint n);
    /** Clears the n-th member. */
    virtual void clearMember(uint n);

    /** Returns @c true if the primary priority channel is set. */
    virtual bool hasPrimary() const;
    /** Returns @c true if the primary priority channel is the selected channel. */
    virtual bool primaryIsSelected() const;
    /** Return the channel index for the primary priority channel. */
    virtual uint primary() const;
    /** Sets the primary priority channel index. */
    virtual void setPrimary(uint idx);
    /** Sets the primary priority channel to be the selected channel. */
    virtual void setPrimarySelected();
    /** Clears the primary priority channel. */
    virtual void clearPrimary();

    /** Returns @c true if the secondary priority channel is set. */
    virtual bool hasSecondary() const;
    /** Returns @c true if the secondary priority channel is the selected channel. */
    virtual bool secondaryIsSelected() const;
    /** Return the channel index for the secondary priority channel. */
    virtual uint secondary() const;
    /** Sets the secondary priority channel index. */
    virtual void setSecondary(uint idx);
    /** Sets the secondary priority channel to be the selected channel. */
    virtual void setSecondarySelected();
    /** Clears the secondary priority channel. */
    virtual void clearSecondary();

    /** Returns @c true if the revert channel is set. */
    virtual bool hasRevert() const;
    /** Return the channel index for the revert channel. */
    virtual uint revert() const;
    /** Sets the revert channel index. */
    virtual void setRevert(uint idx);
    /** Clears the revert channel. */
    virtual void clearRevert();

    /** Returns the hold time in ms. */
    virtual uint holdTime() const;
    /** Sets the hold time in ms. */
    virtual void setHoldTime(uint ms);

    /** Retunrs the priority sample time in ms. */
    virtual uint prioritySampleTime() const;
    /** Sets the priority sample time in ms. */
    virtual void setPrioritySampleTime(uint ms);

    /** Constrcuts a @c ScanList object from this codeplug representation. */
    virtual ScanList *toScanListObj(Context &ctx) const;
    /** Links a previously constructed @c ScanList object to the rest of the generic configuration. */
    virtual bool linkScanListObj(ScanList *lst, Context &ctx) const;
    /** Initializes this codeplug representation from the given @c ScanList object. */
    virtual void fromScanListObj(const ScanList *lst, Context &ctx);
  };

  /** Implements the base class of scan lists banks for all Radioddity codeplugs.
   *
   * Memmory layout of the scan list table.
   * @verbinclude radioddity_scanlistbank.txt
   */
  class ScanListBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ScanListBankElement(uint8_t *ptr, uint size);

  public:
    /** Constructor. */
    explicit ScanListBankElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~ScanListBankElement();

    /** Resets the scan list bank. */
    void clear();

    /** Retruns @c true if the n-th scan list is enabled. */
    virtual bool isEnabled(uint n) const;
    /** Enable/disable n-th scan list. */
    virtual void enable(uint n, bool enabled);
    /** Retunrs a pointer to the n-th scan list. */
    virtual uint8_t *get(uint n) const;
  };

protected:
  /** Hidden constructor, use a device specific class to instantiate. */
  explicit RadioddityCodeplug(QObject *parent=nullptr);

public:
  /** Destructor. */
  virtual ~RadioddityCodeplug();

  /** Clears and resets the complete codeplug to some default values. */
  virtual void clear();

  /** Decodes the binary codeplug and stores its content in the given generic configuration. */
  bool decode(Config *config);
  /** Encodes the given generic configuration as a binary codeplug. */
  bool encode(Config *config, const Flags &flags = Flags());

public:

};

#endif // RADIODDITYCODEPLUG_HH
