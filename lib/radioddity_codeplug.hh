#ifndef RADIODDITYCODEPLUG_HH
#define RADIODDITYCODEPLUG_HH

#include "codeplug.hh"
#include "signaling.hh"
#include "channel.hh"
#include "contact.hh"
#include "radioddity_extensions.hh"

class DMRContact;
class Zone;
class RXGroupList;
class ScanList;

/** Base class of all Radioddity codeplugs. This class implements the majority of all codeplug
 * elements present in all Radioddity codeplugs (also some derivatives like OpenGD77). This eases
 * the support of several Radioddity radios, as only the differences in the codeplug to this base
 * class must be implemented.
 *
 * @ingroup radioddity */
class RadioddityCodeplug : public Codeplug
{
  Q_OBJECT

public:
  /** Implements the base for all Radioddity channel encodings.
   *
   * Memory layout of encoded channel:
   * @verbinclude radioddity_channel.txt */
  class ChannelElement: public Codeplug::Element
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
    virtual unsigned txTimeOut() const;
    /** Sets the TX timeout in seconds. Setting it to 0 disables the timeout. */
    virtual void setTXTimeOut(unsigned tot);
    /** Returns the transmit time-out re-key delay in seconds. */
    virtual unsigned txTimeOutRekeyDelay() const;
    /** Sets the transmit time-out re-key delay in seconds. */
    virtual void setTXTimeOutRekeyDelay(unsigned delay);

    /** Returns the admit criterion. */
    virtual Admit admitCriterion() const;
    /** Sets the admit criterion. */
    virtual void setAdmitCriterion(Admit admit);

    /** Returns @c true if a scan list is set. */
    virtual bool hasScanList() const;
    /** Returns the scan list index (+1). */
    virtual unsigned scanListIndex() const;
    /** Sets the scan list index (+1). */
    virtual void setScanListIndex(unsigned index);

    /** Returns the RX subtone. */
    virtual Signaling::Code rxTone() const;
    /** Sets the RX subtone. */
    virtual void setRXTone(Signaling::Code code);
    /** Returns the TX subtone. */
    virtual Signaling::Code txTone() const;
    /** Sets the TX subtone. */
    virtual void setTXTone(Signaling::Code code);

    /** Returns TX signaling index (+1). */
    virtual unsigned txSignalingIndex() const;
    /** Sets TX signaling index (+1). */
    virtual void setTXSignalingIndex(unsigned index);
    /** Returns RX signaling index (+1). */
    virtual unsigned rxSignalingIndex() const;
    /** Sets RX signaling index (+1). */
    virtual void setRXSignalingIndex(unsigned index);

    /** Returns the privacy group. */
    virtual PrivacyGroup privacyGroup() const;
    /** Sets the privacy group. */
    virtual void setPrivacyGroup(PrivacyGroup grp);

    /** Returns the TX color code. */
    virtual unsigned txColorCode() const;
    /** Sets the TX color code. */
    virtual void setTXColorCode(unsigned cc);

    /** Returns @c true if a group list is set. */
    virtual bool hasGroupList() const;
    /** Returns the group-list index (+1). */
    virtual unsigned groupListIndex() const;
    /** Sets the group-list index (+1). */
    virtual void setGroupListIndex(unsigned index);

    /** Returns the RX color code. */
    virtual unsigned rxColorCode() const;
    /** Sets the RX color code. */
    virtual void setRXColorCode(unsigned cc);

    /** Returns @c true if an emergency system is set. */
    virtual bool hasEmergencySystem() const;
    /** Returns the emergency system index (+1). */
    virtual unsigned emergencySystemIndex() const;
    /** Sets the emergency system index (+1). */
    virtual void setEmergencySystemIndex(unsigned index);

    /** Returns @c true if a TX contact is set. */
    virtual bool hasContact() const;
    /** Returns the transmit contact index (+1). */
    virtual unsigned contactIndex() const;
    /** Sets the transmit contact index (+1). */
    virtual void setContactIndex(unsigned index);

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
    virtual DMRChannel::TimeSlot timeSlot() const;
    /** Sets the time slot of the channel. */
    virtual void setTimeSlot(DMRChannel::TimeSlot ts);

    /** Returns @c true if the dual-capacity direct mode is enabled. */
    virtual bool dualCapacityDirectMode() const;
    /** Enables/disables the dual-capacity direct mode. */
    virtual void enableDualCapacityDirectMode(bool enable);
    /** Returns @c true if non-STE is frequency (?!). */
    virtual bool nonSTEFrequency() const;
    /** Enables/disables non-STE is frequency (?!). */
    virtual void enableNonSTEFrequency(bool enable);

    /** Returns the bandwidth. */
    virtual FMChannel::Bandwidth bandwidth() const;
    /** Sets the bandwidth. */
    virtual void setBandwidth(FMChannel::Bandwidth bw);

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
    ChannelBankElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit ChannelBankElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~ChannelBankElement();

    /** Clears the bank. */
    void clear();

    /** Returns @c true if the channel is enabled. */
    virtual bool isEnabled(unsigned idx) const ;
    /** Enable/disable a channel in the bank. */
    virtual void enable(unsigned idx, bool enabled);
    /** Returns a pointer to the channel at the given index. */
    virtual uint8_t *get(unsigned idx) const;
  };

  /** VFO Channel representation within the binary codeplug.
   *
   * Each channel requires 0x38b:
   * @verbinclude radioddity_vfochannel.txt */
  class VFOChannelElement: public ChannelElement
  {
  public:
    /** Possible offset frequency modes. */
    enum class OffsetMode {
      Off = 0,       ///< Disables transmit frequency offset.
      Positive = 1,  ///< Transmit offset frequency is positive (TX above RX).
      Negative = 2   ///< Transmit offset frequency is negative (TX below RX).
    };

    /** Possible tuning step sizes. */
    enum class StepSize {
      SS2_5kHz = 0,  ///< 2.5kHz
      SS5kHz = 1,    ///< 5kHz
      SS6_25kHz = 2, ///< 6.25kHz
      SS10kHz = 3,   ///< 10kHz
      SS12_5kHz = 4, ///< 12.5kHz
      SS20kHz = 5,   ///< 20kHz
      SS30kHz = 6,   ///< 30kHz
      SS50kHz = 7    ///< 50kHz
    };

  protected:
    /** Hidden constructor. */
    VFOChannelElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit VFOChannelElement(uint8_t *ptr);

    void clear();

    /** The VFO channel has no name. */
    QString name() const;
    /** The VFO channel has no name. */
    void setName(const QString &name);

    /** Returns the tuning step-size in kHz. */
    virtual double stepSize() const;
    /** Sets the tuning step-size in kHz. */
    virtual void setStepSize(double kHz);

    /** Returns the transmit frequency offset mode. */
    virtual OffsetMode offsetMode() const;
    /** Returns the transmit frequency offset. */
    virtual double txOffset() const;
    /** Sets the transmit frequency offset in MHz. */
    virtual void setTXOffset(double f);
    /** Sets the transmit frequency offset mode. */
    virtual void setOffsetMode(OffsetMode mode);
  };


  /** Implements the base for digital contacts in Radioddity codeplugs.
   *
   * Memory layout of a digital contact:
   * @verbinclude radioddity_contact.txt */
  class ContactElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ContactElement(uint8_t *ptr, unsigned size);

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
    virtual unsigned number() const;
    /** Sets the DMR number of the contact. */
    virtual void setNumber(unsigned id);

    /** Returns the call type. */
    virtual DMRContact::Type type() const;
    /** Sets the call type. */
    virtual void setType(DMRContact::Type type);

    /** Returns @c true if the ring tone is enabled for this contact. */
    virtual bool ring() const;
    /** Enables/disables ring tone for this contact. */
    virtual void enableRing(bool enable);

    /** Returns the ring tone style for this contact [0-10]. */
    virtual unsigned ringStyle() const;
    /** Sets the ring tone style for this contact [0-10]. */
    virtual void setRingStyle(unsigned style);

    /** Constructs a @c DigitalContact instance from this codeplug contact. */
    virtual DMRContact *toContactObj(Context &ctx) const;
    /** Resets this codeplug contact from the given @c DigitalContact. */
    virtual void fromContactObj(const DMRContact *obj, Context &ctx);
  };

  /** Implements a base DTMF (analog) contact for Radioddity codeplugs.
   *
   * Memory layout of the DTMF contact:
   * @verbinclude radioddity_dtmfcontact.txt
   */
  class DTMFContactElement: public Element
  {
  protected:
    /** Hidden constructor. */
    DTMFContactElement(uint8_t *ptr, unsigned size);

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
   * Memory layout of the zone:
   * @verbinclude radioddity_zone.txt
   */
  class ZoneElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ZoneElement(uint8_t *ptr, unsigned size);

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
    virtual bool hasMember(unsigned n) const;
    /** Returns the n-th member index (+1). */
    virtual unsigned member(unsigned n) const;
    /** Sets the n-th member index (+1). */
    virtual void setMember(unsigned n, unsigned idx);
    /** Clears the n-th member index. */
    virtual void clearMember(unsigned n);

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
   * Memory layout of the zone table/bank:
   * @verbinclude radioddity_zonebank.txt
   */
  class ZoneBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ZoneBankElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit ZoneBankElement(uint8_t *ptr);
    /** Destructor. */
    ~ZoneBankElement();

    /** Resets the bank. */
    void clear();

    /** Returns @c true if the channel is enabled. */
    virtual bool isEnabled(unsigned idx) const ;
    /** Enable/disable a channel in the bank. */
    virtual void enable(unsigned idx, bool enabled);
    /** Returns a pointer to the channel at the given index. */
    virtual uint8_t *get(unsigned idx) const;
  };

  /** Represents a base class for all group lists within Radioddity codeplugs.
   *
   * Memory layout of the RX group list:
   * @verbinclude radioddity_grouplist.txt
   */
  class GroupListElement: public Element
  {
  protected:
    /** Hidden constructor. */
    GroupListElement(uint8_t *ptr, unsigned size);

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
    virtual bool hasMember(unsigned n) const;
    /** Returns the n-th member index (+1). */
    virtual unsigned member(unsigned n) const;
    /** Sets the n-th member index (+1). */
    virtual void setMember(unsigned n, unsigned idx);
    /** Clears the n-th member index. */
    virtual void clearMember(unsigned n);

    /** Constructs a @c RXGroupList object from the codeplug representation. */
    virtual RXGroupList *toRXGroupListObj(Context &ctx);
    /** Links a previously constructed @c RXGroupList to the rest of the generic configuration. */
    virtual bool linkRXGroupListObj(int ncnt, RXGroupList *lst, Context &ctx) const;
    /** Reset this codeplug representation from a @c RXGroupList object. */
    virtual void fromRXGroupListObj(const RXGroupList *lst, Context &ctx);
  };

  /** Implements a base class of group list memory banks for all Radioddity codeplugs.
   *
   * Memory layout of the group list table:
   * @verbinclude radioddity_grouplistbank.txt
   */
  class GroupListBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    GroupListBankElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit GroupListBankElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~GroupListBankElement();

    /** Resets the bank. */
    void clear();

    /** Returns @c true if the n-th group list is enabled. */
    virtual bool isEnabled(unsigned n) const;
    /** Returns the number of contacts in the n-th group list. */
    virtual unsigned contactCount(unsigned n) const;
    /** Sets the number of contacts in the n-th group list.
     * This also enables the n-th group list. */
    virtual void setContactCount(unsigned n, unsigned size);
    /** Disables the n-th group list. */
    virtual void disable(unsigned n);

    /** Returns a pointer to the n-th group list. */
    virtual uint8_t *get(unsigned n) const;
  };

  /** Implements the base class for scan lists of all Radioddity codeplugs.
   *
   * Memory layout of the scan list.
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
    ScanListElement(uint8_t *ptr, unsigned size);

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
    virtual bool hasMember(unsigned n) const;
    /** Returns @c true if the n-th member is selected channel. */
    virtual bool isSelected(unsigned n) const;
    /** Returns the n-th member index. */
    virtual unsigned member(unsigned n) const;
    /** Sets the n-th member index. */
    virtual void setMember(unsigned n, unsigned idx);
    /** Sets the n-th member to be the selected channel. */
    virtual void setSelected(unsigned n);
    /** Clears the n-th member. */
    virtual void clearMember(unsigned n);

    /** Returns @c true if the primary priority channel is set. */
    virtual bool hasPrimary() const;
    /** Returns @c true if the primary priority channel is the selected channel. */
    virtual bool primaryIsSelected() const;
    /** Return the channel index for the primary priority channel. */
    virtual unsigned primary() const;
    /** Sets the primary priority channel index. */
    virtual void setPrimary(unsigned idx);
    /** Sets the primary priority channel to be the selected channel. */
    virtual void setPrimarySelected();
    /** Clears the primary priority channel. */
    virtual void clearPrimary();

    /** Returns @c true if the secondary priority channel is set. */
    virtual bool hasSecondary() const;
    /** Returns @c true if the secondary priority channel is the selected channel. */
    virtual bool secondaryIsSelected() const;
    /** Return the channel index for the secondary priority channel. */
    virtual unsigned secondary() const;
    /** Sets the secondary priority channel index. */
    virtual void setSecondary(unsigned idx);
    /** Sets the secondary priority channel to be the selected channel. */
    virtual void setSecondarySelected();
    /** Clears the secondary priority channel. */
    virtual void clearSecondary();

    /** Returns @c true if the revert channel is set, if @c false the radio will transmit on the
     * last active channel during scan. */
    virtual bool hasRevert() const;
    /** Returns @c true if the revert channel is the selected channel. */
    virtual bool revertIsSelected() const;
    /** Return the channel index for the revert channel. */
    virtual unsigned revert() const;
    /** Sets the revert channel index. */
    virtual void setRevert(unsigned idx);
    /** Sets the revert channel to be the selected one. */
    virtual void setRevertSelected();
    /** Clears the revert channel, sets it to last active. */
    virtual void clearRevert();

    /** Returns the hold time in ms. */
    virtual unsigned holdTime() const;
    /** Sets the hold time in ms. */
    virtual void setHoldTime(unsigned ms);

    /** Returns the priority sample time in ms. */
    virtual unsigned prioritySampleTime() const;
    /** Sets the priority sample time in ms. */
    virtual void setPrioritySampleTime(unsigned ms);

    /** Constructs a @c ScanList object from this codeplug representation. */
    virtual ScanList *toScanListObj(Context &ctx) const;
    /** Links a previously constructed @c ScanList object to the rest of the generic configuration. */
    virtual bool linkScanListObj(ScanList *lst, Context &ctx) const;
    /** Initializes this codeplug representation from the given @c ScanList object. */
    virtual void fromScanListObj(const ScanList *lst, Context &ctx);
  };

  /** Implements the base class of scan lists banks for all Radioddity codeplugs.
   *
   * Memory layout of the scan list table.
   * @verbinclude radioddity_scanlistbank.txt
   */
  class ScanListBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ScanListBankElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit ScanListBankElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~ScanListBankElement();

    /** Resets the scan list bank. */
    void clear();

    /** Returns @c true if the n-th scan list is enabled. */
    virtual bool isEnabled(unsigned n) const;
    /** Enable/disable n-th scan list. */
    virtual void enable(unsigned n, bool enabled);
    /** Returns a pointer to the n-th scan list. */
    virtual uint8_t *get(unsigned n) const;
  };

  /** Implements the base class of general settings for all Radioddity codeplugs.
   *
   * Memory layout of the general settings
   * @verbinclude radioddity_generalsettings.txt
   */
  class GeneralSettingsElement: public Element
  {
  public:
    /** Use monitor type from extension. */
    typedef RadiodditySettingsExtension::MonitorType MonitorType;
    /** Use ARTS tone mode from extension. */
    typedef RadiodditySettingsExtension::ARTSTone ARTSTone;
    /** Use scan mode from extension. */
    typedef RadiodditySettingsExtension::ScanMode ScanMode;

  protected:
    /** Hidden constructor. */
    GeneralSettingsElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit GeneralSettingsElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~GeneralSettingsElement();

    /** Resets the general settings. */
    void clear();

    /** Returns the radio name. */
    virtual QString name() const;
    /** Sets the radio name. */
    virtual void setName(const QString &name);

    /** Returns the DMR radio ID. */
    virtual unsigned radioID() const;
    /** Sets the DMR radio ID. */
    virtual void setRadioID(unsigned id);

    /** Returns the preamble duration in ms. */
    virtual unsigned preambleDuration() const;
    /** Sets the preamble duration in ms. */
    virtual void setPreambleDuration(unsigned ms);

    /** Returns the monitor type. */
    virtual MonitorType monitorType() const;
    /** Sets the monitor type. */
    virtual void setMonitorType(MonitorType type);

    /** Returns the VOX sensitivity [1-10], 0=disabled. */
    virtual unsigned voxSensitivity() const;
    /** Sets the VOX sensitivity. */
    virtual void setVOXSensitivity(unsigned value);

    /** Returns the low-battery warn interval in seconds. */
    virtual unsigned lowBatteryWarnInterval() const;
    /** Sets the low-battery warn interval in seconds. */
    virtual void setLowBatteryWarnInterval(unsigned sec);

    /** Returns the call-alert duration in seconds. */
    virtual unsigned callAlertDuration() const;
    /** Sets the call-allert duration in seconds. */
    virtual void setCallAlertDuration(unsigned sec);

    /** Returns the lone-worker response period in minutes. */
    virtual unsigned loneWorkerResponsePeriod() const;
    /** Sets the lone-worker response period in minutes. */
    virtual void setLoneWorkerResponsePeriod(unsigned min);
    /** Returns the lone-worker reminder period in seconds. */
    virtual unsigned loneWorkerReminderPeriod() const;
    /** Sets the lone-worker reminder period in seconds. */
    virtual void setLoneWorkerReminderPeriod(unsigned sec);

    /** Returns the group call hang time in ms. */
    virtual unsigned groupCallHangTime() const;
    /** Sets the group call hang time in ms. */
    virtual void setGroupCallHangTime(unsigned ms);
    /** Returns the private call hang time in ms. */
    virtual unsigned privateCallHangTime() const;
    /** Sets the private call hang time in ms. */
    virtual void setPrivateCallHangTime(unsigned ms);

    /** Returns @c true if the down-channel mode is VFO. */
    virtual bool downChannelModeVFO() const;
    /** Enables/disables down-channel mode is VFO. */
    virtual void enableDownChannelModeVFO(bool enable);
    /** Returns @c true if the up-channel mode is VFO. */
    virtual bool upChannelModeVFO() const;
    /** Enables/disables up-channel mode is VFO. */
    virtual void enableUpChannelModeVFO(bool enable);
    /** Returns @c true if the reset tone is enabled. */
    virtual bool resetTone() const;
    /** Enables/disables reset tone. */
    virtual void enableResetTone(bool enable);
    /** Returns @c true if the unknown number tone is enabled. */
    virtual bool unknownNumberTone() const;
    /** Enables/disables reset tone. */
    virtual void enableUnknownNumberTone(bool enable);
    /** Returns the ARTS tone mode. */
    virtual ARTSTone artsToneMode() const;
    /** Sets the ARTS tone mode. */
    virtual void setARTSToneMode(ARTSTone mode);

    /** Returns @c true if the digital channel talk permit tone is enabled. */
    virtual bool digitalTalkPermitTone() const;
    /** Enables/disables digital channel talk permit tone. */
    virtual void enableDigitalTalkPermitTone(bool enable);
    /** Returns @c true if the analog channel talk permit tone is enabled. */
    virtual bool analogTalkPermitTone() const;
    /** Enables/disables analog channel talk permit tone. */
    virtual void enableAnalogTalkPermitTone(bool enable);
    /** Returns @c true if the reset tone is enabled. */
    virtual bool selftestTone() const;
    /** Enables/disables reset tone. */
    virtual void enableSelftestTone(bool enable);
    /** Returns @c true if the channel free indication tone is enabled. */
    virtual bool channelFreeIndicationTone() const;
    /** Enables/disables channel free indication tone. */
    virtual void enableChannelFreeIndicationTone(bool enable);
    /** Returns @c true if all tones are disabled. */
    virtual bool allTonesDisabled() const;
    /** Disables/enables all tones. */
    virtual void disableAllTones(bool disable);
    /** Returns @c true if reception is disabled for battery saving. */
    virtual bool batsaveRX() const;
    /** Enables/disables battery saving by disabling RX. */
    virtual void enableBatsaveRX(bool enable);
    /** Returns @c true if preable is disabled for battery saving. */
    virtual bool batsavePreamble() const;
    /** Enables/disables battery saving by disabling preamble. */
    virtual void enableBatsavePreamble(bool enable);

    /** Returns @c true if all LEDs are disabled. */
    virtual bool allLEDsDisabled() const;
    /** Disables/enables all LEDs. */
    virtual void disableAllLEDs(bool disable);
    /** Returns true if quick-key override is inhibited. */
    virtual bool quickKeyOverrideInhibited() const;
    /** Inhibits quick-key override. */
    virtual void inhibitQuickKeyOverride(bool inhibit);

    /** Returns @c true if the TX exit tone is enabled. */
    virtual bool txExitTone() const;
    /** Enables/disables TX exit tone. */
    virtual void enableTXExitTone(bool enable);
    /** Returns @c true if the radio transmits on the active channel on double monitor. */
    virtual bool txOnActiveChannel() const;
    /** Enables/disables transmission on active channel on double monitor. */
    virtual void enableTXOnActiveChannel(bool enable);
    /** Returns @c true if animation is enabled. */
    virtual bool animation() const;
    /** Enables/disables animation. */
    virtual void enableAnimation(bool enable);
    /** Returns the scan mode. */
    virtual ScanMode scanMode() const;
    /** Sets the scan mode. */
    virtual void setScanMode(ScanMode mode);

    /** Returns the repeater end delay in [0-10]. */
    virtual unsigned repeaterEndDelay() const;
    /** Sets the repeater end delay in [0-10]. */
    virtual void setRepeaterEndDelay(unsigned delay);
    /** Returns the repeater STE in [0-10]. */
    virtual unsigned repeaterSTE() const;
    /** Sets the repeater STE in [0-10]. */
    virtual void setRepeaterSTE(unsigned ste);

    /** Returns @c true if a programming password is set. */
    virtual bool hasProgPassword() const;
    /** Returns the programming password. */
    virtual QString progPassword() const;
    /** Sets the programming password. */
    virtual void setProgPassword(const QString &pwd);
    /** Resets the programming password. */
    virtual void clearProgPassword();

    /** Encodes the general setting from the given config. */
    virtual bool fromConfig(const Config *conf, Context &ctx);
    /** Updates the given config from this settings. */
    virtual bool updateConfig(Config *conf, Context &ctx);
  };

  /** Implements the base class of button settings for all Radioddity codeplugs.
   *
   * Encoding of button settings (size 0x20b):
   * @verbinclude radioddity_buttonsettings.txt */
  class ButtonSettingsElement: public Element
  {
  public:
    /** Possible actions for each button on short and long press. */
    enum class Action {
      None                   = 0x00,  ///< Disables button.
      ToggleAllAlertTones    = 0x01,
      EmergencyOn            = 0x02,
      EmergencyOff           = 0x03,
      ToggleMonitor          = 0x05,  ///< Toggle monitor on channel.
      NuiaceDelete           = 0x06,
      OneTouch1              = 0x07,  ///< Performs the first of 6 user-programmable actions (call, message).
      OneTouch2              = 0x08,  ///< Performs the second of 6 user-programmable actions (call, message).
      OneTouch3              = 0x09,  ///< Performs the third of 6 user-programmable actions (call, message).
      OneTouch4              = 0x0a,  ///< Performs the fourth of 6 user-programmable actions (call, message).
      OneTouch5              = 0x0b,  ///< Performs the fifth of 6 user-programmable actions (call, message).
      OneTouch6              = 0x0c,  ///< Performs the sixt of 6 user-programmable actions (call, message).
      ToggleRepeatTalkaround = 0x0d,
      ToggleScan             = 0x0e,
      TogglePrivacy          = 0x10,
      ToggleVox              = 0x11,
      ZoneSelect             = 0x12,
      BatteryIndicator       = 0x13,
      ToggleLoneWorker       = 0x14,
      PhoneExit              = 0x16,
      ToggleFlashLight       = 0x1a,
      ToggleFMRadio          = 0x1b
    };

    /** Possible one-touch actions. */
    enum class OneTouchAction {
      None                   = 0x00,  ///< Disabled.
      DigitalCall            = 0x10,  ///< Calls a digital contact.
      DigitalMessage         = 0x11,  ///< Sends a SMS.
      AnalogCall             = 0x20   ///< Calls an analog contact.
    };

  protected:
    /** Hidden constructor */
    ButtonSettingsElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit ButtonSettingsElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~ButtonSettingsElement();

    /** Clears the button settings. */
    void clear();

    /** Returns the long-press duration in ms. */
    virtual unsigned longPressDuration() const;
    /** Sets the long-press duration in ms. */
    virtual void setLongPressDuration(unsigned ms);

    /** Returns the side-key 1 short-press action. */
    virtual Action sk1ShortPress() const;
    /** Sets the side-key 1 short-press action. */
    virtual void setSK1ShortPress(Action action);
    /** Returns the side-key 1 long-press action. */
    virtual Action sk1LongPress() const;
    /** Sets the side-key 1 long-press action. */
    virtual void setSK1LongPress(Action action);

    /** Returns the side-key 2 short-press action. */
    virtual Action sk2ShortPress() const;
    /** Sets the side-key 2 short-press action. */
    virtual void setSK2ShortPress(Action action);
    /** Returns the side-key 2 long-press action. */
    virtual Action sk2LongPress() const;
    /** Sets the side-key 2 long-press action. */
    virtual void setSK2LongPress(Action action);

    /** Returns the top-key short-press action. */
    virtual Action tkShortPress() const;
    /** Sets the top-key short-press action. */
    virtual void setTKShortPress(Action action);
    /** Returns the top-key long-press action. */
    virtual Action tkLongPress() const;
    /** Sets the top-key long-press action. */
    virtual void setTKLongPress(Action action);

    /** Returns the n-th one-touch action. */
    virtual OneTouchAction oneTouchAction(unsigned n) const;
    /** Returns the n-th one-touch contact index (if action is @c OneTouchAction::DigitalCall). */
    virtual unsigned oneTouchContact(unsigned n) const;
    /** Returns the n-th one-touch message index (if action is @c OneTouchAction::DigitalMessage). */
    virtual unsigned oneTouchMessage(unsigned n) const;
    /** Disables the n-th one-touch action. */
    virtual void disableOneTouch(unsigned n);
    /** Configures n-th one-touch action as a digital call to contact index. */
    virtual void setOneTouchDigitalCall(unsigned n, unsigned index);
    /** Configures n-th one-touch action as a digital message using given index. */
    virtual void setOneTouchDigitalMessage(unsigned n, unsigned index);
    /** Configures n-th one-touch action as a analog call. */
    virtual void setOneTouchAnalogCall(unsigned n);
  };

  /** Implements the base class of menu settings for all Radioddity codeplugs.
   *
   * Encoding of Menu settings (size 0x08b):
   * @verbinclude radioddity_menusettings.txt */
  class MenuSettingsElement: public Element
  {
  public:
    /** Possible channel display modes. */
    enum class ChannelDisplayMode {
      Number    = 0,  ///< Show channel number.
      Name      = 1,  ///< Show channel name.
      Frequency = 2   ///< Show channel frequency.
    };

    /** Possible dual-watch modes. */
    enum class DualWatchMode {
      DualDual = 1,
      DualSingle = 2
    };

  protected:
    /** Hidden constructor. */
    MenuSettingsElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit MenuSettingsElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~MenuSettingsElement();

    /** Resets the menu settings. */
    void clear();

    /** Returns the menu hang-time in seconds. */
    virtual unsigned menuHangTime() const;
    /** Sets the menu hang time in seconds. */
    virtual void setMenuHangTime(unsigned sec);

    /** Returns @c true if the message menu is shown. */
    virtual bool message() const;
    /** Enables/disables the message menu. */
    virtual void enableMessage(bool enable);
    /** Returns @c true if the scan-start menu is shown. */
    virtual bool scanStart() const;
    /** Enables/disables the scan-start menu. */
    virtual void enableScanStart(bool enable);
    /** Returns @c true if the edit scan-list menu is shown. */
    virtual bool editScanList() const;
    /** Enables/disables the edit scan-list menu. */
    virtual void enableEditScanList(bool enable);
    /** Returns @c true if the call-alert menu is shown. */
    virtual bool callAlert() const;
    /** Enables/disables the call-alert menu. */
    virtual void enableCallAlert(bool enable);
    /** Returns @c true if the edit-contact menu is shown. */
    virtual bool editContact() const;
    /** Enables/disables the edit-contact menu. */
    virtual void enableEditContact(bool enable);
    /** Returns @c true if the manual-dial menu is shown. */
    virtual bool manualDial() const;
    /** Enables/disables the manual-dial menu. */
    virtual void enableManualDial(bool enable);
    /** Returns @c true if the radio-check menu is shown. */
    virtual bool radioCheck() const;
    /** Enables/disables the radioCheck menu. */
    virtual void enableRadioCheck(bool enable);
    /** Returns @c true if the remote-monitor menu is shown. */
    virtual bool remoteMonitor() const;
    /** Enables/disables the message menu. */
    virtual void enableRemoteMonitor(bool enable);

    /** Returns @c true if the radio-enable menu is shown. */
    virtual bool radioEnable() const;
    /** Enables/disables the radio-enable menu. */
    virtual void enableRadioEnable(bool enable);
    /** Returns @c true if the radio-disable menu is shown. */
    virtual bool radioDisable() const;
    /** Enables/disables the radio-disable menu. */
    virtual void enableRadioDisable(bool enable);
    /** Returns @c true if the programming-password menu is shown. */
    virtual bool progPassword() const;
    /** Enables/disables the programming-password menu. */
    virtual void enableProgPassword(bool enable);
    /** Returns @c true if the talkaround menu is shown. */
    virtual bool talkaround() const;
    /** Enables/disables the talkaround menu. */
    virtual void enableTalkaround(bool enable);
    /** Returns @c true if the tone menu is shown. */
    virtual bool tone() const;
    /** Enables/disables the tone menu. */
    virtual void enableTone(bool enable);
    /** Returns @c true if the power menu is shown. */
    virtual bool power() const;
    /** Enables/disables the power menu. */
    virtual void enablePower(bool enable);
    /** Returns @c true if the backlight menu is shown. */
    virtual bool backlight() const;
    /** Enables/disables the backlight menu. */
    virtual void enableBacklight(bool enable);
    /** Returns @c true if the intro-screen menu is shown. */
    virtual bool introScreen() const;
    /** Enables/disables the message menu. */
    virtual void enableIntroScreen(bool enable);

    /** Returns @c true if the keypad-lock menu is shown. */
    virtual bool keypadLock() const;
    /** Enables/disables the keypad-lock menu. */
    virtual void enableKeypadLock(bool enable);
    /** Returns @c true if the LED-indicator menu is shown. */
    virtual bool ledIndicator() const;
    /** Enables/disables the LED-indicator menu. */
    virtual void enableLEDIndicator(bool enable);
    /** Returns @c true if the squelch menu is shown. */
    virtual bool squelch() const;
    /** Enables/disables the squelch menu. */
    virtual void enableSquelch(bool enable);
    /** Returns @c true if the privacy menu is shown. */
    virtual bool privacy() const;
    /** Enables/disables the privacy menu. */
    virtual void enablePrivacy(bool enable);
    /** Returns @c true if the VOX menu is shown. */
    virtual bool vox() const;
    /** Enables/disables the VOX menu. */
    virtual void enableVOX(bool enable);
    /** Returns @c true if the password-lock menu is shown. */
    virtual bool passwordLock() const;
    /** Enables/disables the password-lock menu. */
    virtual void enablePasswordLock(bool enable);
    /** Returns @c true if the missed-calls menu is shown. */
    virtual bool missedCalls() const;
    /** Enables/disables the missed-calls menu. */
    virtual void enableMissedCalls(bool enable);
    /** Returns @c true if the answered-calls menu is shown. */
    virtual bool answeredCalls() const;
    /** Enables/disables the answered-calls menu. */
    virtual void enableAnsweredCalls(bool enable);

    /** Returns @c true if the outgoing-calls menu is shown. */
    virtual bool outgoingCalls() const;
    /** Enables/disables the outgoing-calls menu. */
    virtual void enableOutgoingCalls(bool enable);
    /** Returns @c true if the channel display-mode menu is shown. */
    virtual bool channelDisplay() const;
    /** Enables/disables the channel display mode menu. */
    virtual void enableChannelDisplay(bool enable);
    /** Returns @c true if the dual-watch menu is shown. */
    virtual bool dualWatch() const;
    /** Enables/disables the dual-watch menu. */
    virtual void enableDualWatch(bool enable);

    /** Returns the keypad lock time in seconds. */
    virtual unsigned keypadLockTime() const;
    /** Sets the keypad lock time in seconds. */
    virtual void setKeypadLockTime(unsigned sec);

    /** Retunrns the backlight time in seconds. */
    virtual unsigned backlightTime() const;
    /** Sets the backlight time in seconds. */
    virtual void setBacklightTime(unsigned sec);

    /** Returns the channel display mode. */
    virtual ChannelDisplayMode channelDisplayMode() const;
    /** Sets the channel display mode. */
    virtual void setChannelDisplayMode(ChannelDisplayMode mode);

    /** Returns @c true if the keytone is enabled. */
    virtual bool keyTone() const;
    /** Enables/disables the keytone. */
    virtual void enableKeyTone(bool enable);

    /** Returns the dual-watch mode. */
    virtual DualWatchMode dualWatchMode() const;
    /** Sets the dual-watch mode. */
    virtual void setDualWatchMode(DualWatchMode mode);
  };

  /** Implements the base class of boot settings for all Radioddity codeplugs.
   *
   * Encoding of boot settings (size 0x20b):
   * @verbinclude radioddity_bootsettings.txt */
  class BootSettingsElement: public Element
  {
  protected:
    /** Hidden constructor. */
    BootSettingsElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit BootSettingsElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~BootSettingsElement();

    /** Resets the settings. */
    void clear();

    /** Returns @c true if the text is shown on boot, other wise an image is shown. */
    virtual bool bootText() const;
    /** Enables/disables boot text. */
    virtual void enableBootText(bool enable);

    /** Returns @c true if the boot password is enabled. */
    virtual bool bootPasswordEnabled() const;
    /** Enables/disables the boot password. */
    virtual void enableBootPassword(bool enable);
    /** Returns the boot password (6 digit). */
    virtual unsigned bootPassword() const;
    /** Sets the boot password (6 digit). */
    virtual void setBootPassword(unsigned passwd);
  };

  /** Implements the base class of boot messages for all Radioddity codeplugs.
   *
   * Encoding of boot messages (size 0x20b):
   * @verbinclude radioddity_boottext.txt */
  class BootTextElement: public Element
  {
  protected:
    /** Hidden constructor. */
    BootTextElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit BootTextElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~BootTextElement();

    /** Resets the intro text. */
    void clear();

    /** Returns the first line. */
    virtual QString line1() const;
    /** Sets the first line. */
    virtual void setLine1(const QString &text);
    /** Returns the Second line. */
    virtual QString line2() const;
    /** Sets the second line. */
    virtual void setLine2(const QString &text);

    /** Encodes boot text settings from configuration. */
    virtual void fromConfig(Config *conf);
    /** Updates the configuration with the boot text settings. */
    virtual void updateConfig(Config *conf);
  };

  /** Implements the base class of a message bank for all Radioddity message banks.
   *
   * Encoding of messages (size: 0x1248b):
   * @verbinclude radioddity_messagebank.txt */
  class MessageBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    MessageBankElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit MessageBankElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~MessageBankElement();

    /** Resets all messages. */
    void clear();

    /** Returns the number of messages. */
    virtual unsigned numMessages() const;
    /** Returns the n-th message. */
    virtual QString message(unsigned n) const;
    /** Appends a message to the list. */
    virtual void appendMessage(const QString msg);
  };


  /** Represents all encryption keys and settings within the codeplug on the device.
   *
   * Memory representation of encryption settings:
   * @verbinclude radioddity_privacy.txt */
  class EncryptionElement: public Codeplug::Element
  {
  public:
    /** Encodes possible privacy types. For now, only none (encryption disabled) and basic are
     * supported. */
    enum class PrivacyType {
      None,   ///< No encryption at all.
      Basic   ///< Use basic DMR encryption.
    };

  protected:
    /** Hidden constructor. */
    EncryptionElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    explicit EncryptionElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~EncryptionElement();

    void clear();

    /** Returns the privacy type set. */
    virtual PrivacyType privacyType() const;
    /** Sets the privacy type. */
    virtual void setPrivacyType(PrivacyType type);

    /** Returns @c true if the n-th "basic" key (32bit) is set.
     * That is, if it is not filled with 0xff. */
    virtual bool isBasicKeySet(unsigned n) const;
    /** Returns the n-th "basic" key (32bit). */
    virtual QByteArray basicKey(unsigned n) const;
    /** Sets the n-th "basic" key (32bit). */
    virtual void setBasicKey(unsigned n, const QByteArray &key);
    /** Resets the n-th basic key. */
    virtual void clearBasicKey(unsigned n);

    /** Encodes given encryption extension. */
    virtual bool fromCommercialExt(CommercialExtension *ext, Context &ctx);
    /** Constructs the encryption extension. */
    virtual bool updateCommercialExt(Context &ctx);
    /** Links the given encryption extension. */
    virtual bool linkCommercialExt(CommercialExtension *ext, Context &ctx);
  };

protected:
  /** Hidden constructor, use a device specific class to instantiate. */
  explicit RadioddityCodeplug(QObject *parent=nullptr);

public:
  /** Destructor. */
  virtual ~RadioddityCodeplug();

  /** Clears and resets the complete codeplug to some default values. */
  virtual void clear();

  bool index(Config *config, Context &ctx, const ErrorStack &err=ErrorStack()) const;

  /** Decodes the binary codeplug and stores its content in the given generic configuration. */
  bool decode(Config *config, const ErrorStack &err=ErrorStack());
  /** Encodes the given generic configuration as a binary codeplug. */
  bool encode(Config *config, const Flags &flags = Flags(), const ErrorStack &err=ErrorStack());

public:
  /** Decodes the binary codeplug and stores its content in the given generic configuration using
   * the given context. */
  virtual bool decodeElements(Context &ctx, const ErrorStack &err=ErrorStack());
  /** Encodes the given generic configuration as a binary codeplug using the given context. */
  virtual bool encodeElements(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());

  /** Clears the general settings in the codeplug. */
  virtual void clearGeneralSettings() = 0;
  /** Updates the general settings from the given configuration. */
  virtual bool encodeGeneralSettings(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Updates the given configuration from the general settings. */
  virtual bool decodeGeneralSettings(Config *config, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;

  /** Clears the button settings. */
  virtual void clearButtonSettings() = 0;

  /** Clears the messages. */
  virtual void clearMessages() = 0;

  /** Clears all contacts in the codeplug. */
  virtual void clearContacts() = 0;
  /** Encodes all digital contacts in the configuration into the codeplug. */
  virtual bool encodeContacts(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Adds a digital contact to the configuration for each one in the codeplug. */
  virtual bool createContacts(Config *config, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;

  /** Clears all DTMF contacts in the codeplug. */
  virtual void clearDTMFContacts() = 0;
  /** Encodes all DTMF contacts. */
  virtual bool encodeDTMFContacts(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Adds all DTMF contacts to the configuration. */
  virtual bool createDTMFContacts(Config *config, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;

  /** Clear all channels. */
  virtual void clearChannels() = 0;
  /** Encode all channels. */
  virtual bool encodeChannels(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Adds all defined channels to the configuration. */
  virtual bool createChannels(Config *config, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Links all channels. */
  virtual bool linkChannels(Config *config, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;

  /** Clear boot settings. */
  virtual void clearBootSettings() = 0;

  /** Clears menu settings. */
  virtual void clearMenuSettings() = 0;

  /** Clears boot text. */
  virtual void clearBootText() = 0;
  /** Encodes boot text. */
  virtual bool encodeBootText(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Updates the given configuration from the boot text settings. */
  virtual bool decodeBootText(Config *config, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;

  /** Clears the VFO settings. */
  virtual void clearVFOSettings() = 0;

  /** Clears all zones. */
  virtual void clearZones() = 0;
  /** Encodes zones. */
  virtual bool encodeZones(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Adds zones to the configuration. */
  virtual bool createZones(Config *config, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Links all zones within the configuration. */
  virtual bool linkZones(Config *config, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;

  /** Clears all scan lists. */
  virtual void clearScanLists() = 0;
  /** Encodes all scan lists. */
  virtual bool encodeScanLists(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Creates all scan lists. */
  virtual bool createScanLists(Config *config, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Links all scan lists. */
  virtual bool linkScanLists(Config *config, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;

  /** Clears all group lists. */
  virtual void clearGroupLists() = 0;
  /** Encodes all group lists. */
  virtual bool encodeGroupLists(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Creates all group lists. */
  virtual bool createGroupLists(Config *config, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Links all group lists. */
  virtual bool linkGroupLists(Config *config, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;

  /** Clears all encryption keys. */
  virtual void clearEncryption() = 0;
  /** Encodes all encryption keys defined. */
  virtual bool encodeEncryption(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Creates all encryption keys. */
  virtual bool createEncryption(Config *config, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Links all encryption keys. */
  virtual bool linkEncryption(Config *config, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
};

#endif // RADIODDITYCODEPLUG_HH
