#ifndef TYT_CODEPLUG_HH
#define TYT_CODEPLUG_HH

#include <QDateTime>

#include "codeplug.hh"
#include "signaling.hh"
#include "channel.hh"
#include "contact.hh"
#include "tyt_extensions.hh"

class DigitalContact;
class Zone;
class RXGroupList;
class ScanList;
class GPSSystem;


/** Base class of all TyT codeplugs. This class implements the majority of all codeplug elements
 * present in all TyT codeplugs. This eases the support of several TyT radios, as only the
 * differences in the codeplug to this base class must be implemented.
 *
 * @ingroup tyt */
class TyTCodeplug : public Codeplug
{
  Q_OBJECT

public:
  /** Represents a single channel (analog or digital) within the TyT codeplug.
   *
   * Memory layout of encoded channel:
   * @verbinclude tyt_channel.txt */
  class ChannelElement: public Codeplug::Element
  {
  public:
    /** Possible modes for the channel, i.e. analog and digital. */
    enum Mode {
      MODE_ANALOG  = 1,             ///< Analog channel.
      MODE_DIGITAL = 2              ///< Digital channel.
    };

    /** Bandwidth of the channel. */
    enum Bandwidth {
      BW_12_5_KHZ = 0,              ///< 12.5 kHz narrow, (default for binary channels).
      BW_20_KHZ   = 1,              ///< 20 kHz (really?)
      BW_25_KHZ   = 2               ///< 25kHz wide.
    };

    /** Possible privacy types. */
    enum PrivacyType {
      PRIV_NONE = 0,                ///< No privacy.
      PRIV_BASIC = 1,               ///< Basic privacy.
      PRIV_ENHANCED = 2             ///< Enhanced privacy.
    };

    /** TX Admit criterion. */
    enum Admit {
      ADMIT_ALWAYS = 0,             ///< Always allow TX.
      ADMIT_CH_FREE = 1,            ///< Allow TX if channel is free.
      ADMIT_TONE = 2,               ///< Allow TX if CTCSS tone matches.
      ADMIT_COLOR = 3,              ///< Allow TX if color-code matches.
    };

  protected:
    /** Constructs a channel from the given memory. */
    ChannelElement(uint8_t *ptr, size_t size);

  public:
    /** Constructs a channel from the given memory. */
    ChannelElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~ChannelElement();

    /** Returns @c true if channel is valid/enabled. */
    bool isValid() const;
    /** Clears/resets the channel and therefore disables it. */
    void clear();

    /** Returns the mode of the channel. */
    virtual Mode mode() const;
    /** Sets the mode of the channel. */
    virtual void setMode(Mode setMode);

    /** Returns the bandwidth of the (analog) channel. */
    virtual AnalogChannel::Bandwidth bandwidth() const;
    /** Sets the bandwidth of the (analog) channel. */
    virtual void setBandwidth(AnalogChannel::Bandwidth bw);

    /** Returns @c true if the channel has auto scan enabled. */
    virtual bool autoScan() const;
    /** Enables/disables auto scan for this channel. */
    virtual void enableAutoScan(bool enable);

    /** Returns @c true if the channel has lone worker enabled. */
    virtual bool loneWorker() const;
    /** Enables/disables lone worker for this channel. */
    virtual void enableLoneWorker(bool enable);

    /** Returns @c true if the channel has talkaround enabled. */
    virtual bool talkaround() const;
    /** Enables/disables talkaround for this channel. */
    virtual void enableTalkaround(bool enable);

    /** Returns @c true if the channel has rx only enabled. */
    virtual bool rxOnly() const;
    /** Enables/disables rx only for this channel. */
    virtual void enableRXOnly(bool enable);

    /** Returns the time slot of this channel. */
    virtual DigitalChannel::TimeSlot timeSlot() const;
    /** Sets the time slot of this channel. */
    virtual void setTimeSlot(DigitalChannel::TimeSlot ts);

    /** Returns the color code of this channel. */
    virtual uint8_t colorCode() const;
    /** Sets the color code of this channel. */
    virtual void setColorCode(uint8_t ts);

    /** Returns the index of the privacy system (key). */
    virtual uint8_t privacyIndex() const;
    /** Sets the index of the privacy system (key). */
    virtual void setPrivacyIndex(uint8_t ts);
    /** Returns the type of the privacy system. */
    virtual PrivacyType privacyType() const;
    /** Sets the type of the privacy system. */
    virtual void setPrivacyType(PrivacyType type);

    /** Returns @c true if the channel has private call confirmation enabled. */
    virtual bool privateCallConfirm() const;
    /** Enables/disables private call confirmation for this channel. */
    virtual void enablePrivateCallConfirm(bool enable);

    /** Returns @c true if the channel has data call confirmation enabled. */
    virtual bool dataCallConfirm() const;
    /** Enables/disables data call confirmation for this channel. */
    virtual void enableDataCallConfirm(bool enable);

    /** Returns some weird reference frequency setting for reception. */
    virtual TyTChannelExtension::RefFrequency rxRefFrequency() const;
    /** Sets some weird reference frequency setting for reception. */
    virtual void setRXRefFrequency(TyTChannelExtension::RefFrequency ref);
    /** Returns some weird reference frequency setting for transmission. */
    virtual TyTChannelExtension::RefFrequency txRefFrequency() const;
    /** Sets some weird reference frequency setting for transmission. */
    virtual void setTXRefFrequency(TyTChannelExtension::RefFrequency ref);

    /** Returns @c true if the channel has alarm confirmation enabled. */
    virtual bool emergencyAlarmACK() const;
    /** Enables/disables alarm confirmation for this channel. */
    virtual void enableEmergencyAlarmACK(bool enable);

    /** Returns @c true if the channel has display PTT ID enabled. */
    virtual bool displayPTTId() const;
    /** Enables/disables PTT ID display for this channel. */
    virtual void enableDisplayPTTId(bool enable);

    /** Returns @c true if the channel has VOX enabled. */
    virtual bool vox() const;
    /** Enables/disables VOX for this channel. */
    virtual void enableVOX(bool enable);

    /** Returns the admit criterion for this channel. */
    virtual Admit admitCriterion() const;
    /** Sets the admit criterion for this channel. */
    virtual void setAdmitCriterion(Admit admit);

    /** Returns the transmit contact index (+1) for this channel. */
    virtual uint16_t contactIndex() const;
    /** Sets the transmit contact index (+1) for this channel. */
    virtual void setContactIndex(uint16_t idx);

    /** Returns the transmit time-out in seconds. */
    virtual unsigned txTimeOut() const;
    /** Sets the transmit time-out in seconds. */
    virtual void setTXTimeOut(unsigned tot);
    /** Returns the transmit time-out re-key delay in seconds. */
    virtual uint8_t txTimeOutRekeyDelay() const;
    /** Sets the transmit time-out re-key delay in seconds. */
    virtual void setTXTimeOutRekeyDelay(uint8_t delay);

    /** Returns the emergency system index (+1) for this channel. */
    virtual uint8_t emergencySystemIndex() const;
    /** Sets the emergency system index (+1) for this channel. */
    virtual void setEmergencySystemIndex(uint8_t idx);

    /** Returns the scan-list index (+1) for this channel. */
    virtual uint8_t scanListIndex() const;
    /** Sets the scan-list index (+1) for this channel. */
    virtual void setScanListIndex(uint8_t idx);

    /** Returns the RX group list index (+1) for this channel. */
    virtual uint8_t groupListIndex() const;
    /** Sets the RX group list index (+1) for this channel. */
    virtual void setGroupListIndex(uint8_t idx);

    /** Returns the positioning system index (+1) for this channel. */
    virtual uint8_t positioningSystemIndex() const;
    /** Sets the positioning system index (+1) for this channel. */
    virtual void setPositioningSystemIndex(uint8_t idx);

    /** Returns @c true if the channel has DTMF decoding enabled. */
    virtual bool dtmfDecode(uint8_t idx) const;
    /** Enables/disables DTMF decoding this channel. */
    virtual void setDTMFDecode(uint8_t idx, bool enable);

    /** Returns the RX frequency in Hz. */
    virtual uint32_t rxFrequency() const;
    /** Sets the RX frequency in Hz. */
    virtual void setRXFrequency(uint32_t Hz);
    /** Returns the TX frequency in Hz. */
    virtual uint32_t txFrequency() const;
    /** Sets the TX frequency in Hz. */
    virtual void setTXFrequency(uint32_t Hz);

    /** Returns the CTCSS/DSC signaling for RX. */
    virtual Signaling::Code rxSignaling() const;
    /** Sets the CTCSS/DSC signaling for RX. */
    virtual void setRXSignaling(Signaling::Code code);
    /** Returns the CTCSS/DSC signaling for TX. */
    virtual Signaling::Code txSignaling() const;
    /** Sets the CTCSS/DSC signaling for TX. */
    virtual void setTXSignaling(Signaling::Code code);
    /** Returns the signaling system index (+1) for RX. */
    virtual uint8_t rxSignalingSystemIndex() const;
    /** Sets the signaling system index (+1) for RX. */
    virtual void setRXSignalingSystemIndex(uint8_t idx);
    /** Returns the signaling system index (+1) for TX. */
    virtual uint8_t txSignalingSystemIndex() const;
    /** Sets the signaling system index (+1) for TX. */
    virtual void setTXSignalingSystemIndex(uint8_t idx);

    /** Returns @c true if the channel transmits GPS information enabled. */
    virtual bool txGPSInfo() const;
    /** Enables/disables transmission of GPS information for this channel. */
    virtual void enableTXGPSInfo(bool enable);
    /** Returns @c true if the channel receives GPS information enabled. */
    virtual bool rxGPSInfo() const;
    /** Enables/disables reception of GPS information for this channel. */
    virtual void enableRXGPSInfo(bool enable);

    /** Returns the name of this channel. */
    virtual QString name() const;
    /** Sets the name of this channel. */
    virtual void setName(const QString &setName);

    /** Constructs a generic @c Channel object from the codeplug channel. */
    virtual Channel *toChannelObj() const;
    /** Links a previously constructed channel to the rest of the configuration. */
    virtual bool linkChannelObj(Channel *c, Context &ctx) const;
    /** Initializes this codeplug channel from the given generic configuration. */
    virtual void fromChannelObj(const Channel *c, Context &ctx);
  };

  /** Represents a digital (DMR) contact within the codeplug.
   *
   * Memory layout of encoded contact:
   * @verbinclude tyt_contact.txt */
  class ContactElement: public Codeplug::Element
  {
  protected:
    /** Constructor. */
    ContactElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    ContactElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~ContactElement();

    void clear();
    bool isValid() const;

    /** Returns the DMR ID of the contact. */
    virtual uint32_t dmrId() const;
    /** Sets the DMR ID of the contact. */
    virtual void setDMRId(uint32_t id);

    /** Returns the call-type of the contact. */
    virtual DigitalContact::Type callType() const;
    /** Sets the call-type of the contact. */
    virtual void setCallType(DigitalContact::Type type);

    /** Returns @c true if the ring-tone is enabled for this contact. */
    virtual bool ringTone() const;
    /** Enables/disables the ring-tone for this contact. */
    virtual void enableRingTone(bool enable);

    /** Returns the name of the contact. */
    virtual QString name() const;
    /** Sets the name of the contact. */
    virtual void setName(const QString &nm);

    /** Encodes the give contact. */
    virtual bool fromContactObj(const DigitalContact *contact);
    /** Creates a contact. */
    virtual DigitalContact *toContactObj() const;
  };

  /** Represents a zone within the codeplug.
   * Please note that a zone consists of two elements the @c ZoneElement and the @c ZoneExtElement.
   * The latter adds additional channels for VFO A and the channels for VFO B.
   *
   * Memory layout of encoded zone:
   * @verbinclude tyt_zone.txt */
  class ZoneElement: public Codeplug::Element
  {
  protected:
    /** Constructor. */
    ZoneElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    ZoneElement(uint8_t *ptr);
    /** Desturctor. */
    virtual ~ZoneElement();

    void clear();
    bool isValid() const;

    /** Returns the name of the zone. */
    virtual QString name() const;
    /** Sets the name of the zone. */
    virtual void setName(const QString &setName);

    /** Returns the index (+1) of the @c n-th member. */
    virtual uint16_t memberIndex(unsigned n) const;
    /** Sets the index (+1) of the @c n-th member. */
    virtual void setMemberIndex(unsigned n, uint16_t idx);

    /** Encodes a given zone object. */
    virtual bool fromZoneObj(const Zone *zone, Context &ctx);
    /** Creates a zone. */
    virtual Zone *toZoneObj() const;
    /** Links the created zone to channels. */
    virtual bool linkZone(Zone *zone, Context &ctx) const;
  };

  /** Representation of an RX group list within the codeplug.
   *
   * Memory layout of encoded RX group list:
   * @verbinclude tyt_grouplist.txt */
  class GroupListElement: public Codeplug::Element
  {
  protected:
    /** Constructor. */
    GroupListElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    GroupListElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~GroupListElement();

    void clear();
    bool isValid() const;

    /** Returns the name of the group list. */
    virtual QString name() const;
    /** Sets the name of the group list. */
    virtual void setName(const QString &nm);

    /** Returns the n-th member index. */
    virtual uint16_t memberIndex(unsigned n) const;
    /** Sets the n-th member index. */
    virtual void setMemberIndex(unsigned n, uint16_t idx);

    /** Encodes the given group list. */
    virtual bool fromGroupListObj(const RXGroupList *lst, Context &ctx);
    /** Creates a group list object. */
    virtual RXGroupList *toGroupListObj(Context &ctx);
    /** Links the given group list. */
    virtual bool linkGroupListObj(RXGroupList *lst, Context &ctx);
  };

  /** Represents a scan list within the codeplug.
   *
   * Memory layout of encoded scan list:
   * @verbinclude tyt_scanlist.txt */
  class ScanListElement: public Codeplug::Element
  {
  protected:
    /** Constructor. */
    ScanListElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    ScanListElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~ScanListElement();

    bool isValid() const;
    void clear();

    /** Returns the name of the scan list. */
    virtual QString name() const;
    /** Sets the name of the scan list. */
    virtual void setName(const QString &nm);

    /** Returns the index (+1) of the first priority channel. */
    virtual uint16_t priorityChannel1Index() const;
    /** Set the index (+1) of the first priority channel. */
    virtual void setPriorityChannel1Index(uint16_t idx);

    /** Returns the index (+1) of the second priority channel. */
    virtual uint16_t priorityChannel2Index() const;
    /** Set the index (+1) of the second priority channel. */
    virtual void setPriorityChannel2Index(uint16_t idx);

    /** Returns the index (+1) of the TX channel. 0=current, 0xffff=none. */
    virtual uint16_t txChannelIndex() const;
    /** Sets the index (+1) of the TX channel. 0=current, 0xffff=none. */
    virtual void setTXChannelIndex(uint16_t idx);

    /** Returns the hold time in ms. */
    virtual unsigned holdTime() const;
    /** Sets the hold time in ms. */
    virtual void setHoldTime(unsigned time);

    /** Returns the priority sample time in ms. */
    virtual unsigned prioritySampleTime() const;
    /** Sets the priority sample time in ms. */
    virtual void setPrioritySampleTime(unsigned time);

    /** Returns the n-th member index. */
    virtual uint16_t memberIndex(unsigned n) const;
    /** Sets the n-th member index. */
    virtual void setMemberIndex(unsigned n, uint16_t idx);

    /** Encodes the given scan list. */
    virtual bool fromScanListObj(const ScanList *lst, Context &ctx);
    /** Creates a scan list. */
    virtual ScanList *toScanListObj(Context &ctx);
    /** Links the scan list object. */
    virtual bool linkScanListObj(ScanList *lst, Context &ctx);
  };

  /** Codeplug representation of the general settings.
   *
   * Memory layout of encoded settings:
   * @verbinclude tyt_settings.txt */
  class GeneralSettingsElement: public Codeplug::Element
  {
  protected:
    /** Hidden constructor. */
    GeneralSettingsElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    explicit GeneralSettingsElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~GeneralSettingsElement();

    void clear();

    /** Returns the first intro line. */
    virtual QString introLine1() const;
    /** Sets the first intro line. */
    virtual void setIntroLine1(const QString line);
    /** Returns the second intro line. */
    virtual QString introLine2() const;
    /** Sets the second intro line. */
    virtual void setIntroLine2(const QString line);

    /** Returns the monitor type. */
    virtual TyTSettingsExtension::MonitorType monitorType() const;
    /** Sets the monitor type. */
    virtual void setMonitorType(TyTSettingsExtension::MonitorType type);

    /** Returns @c true if all LEDs are disabled. */
    virtual bool allLEDsDisabled() const;
    /** Enables/disables all LEDs. */
    virtual void disableAllLEDs(bool disable);

    /** Returns @c true, if save preamble is enabled. */
    virtual bool savePreamble() const;
    /** Enables/disables save preamble. */
    virtual void setSavePreamble(bool enable);
    /** Returns @c true, if save RX mode is enabled. */
    virtual bool saveModeRX() const;
    /** Enables/disables save mode RX. */
    virtual void setSaveModeRX(bool enable);
    /** Returns @c true, if all tones are disabled. */
    virtual bool allTonesDisabled() const;
    /** Enables/disables all tones. */
    virtual void disableAllTones(bool disable);
    /** Returns @c true, if the channel free indication tone is enabled. */
    virtual bool chFreeIndicationTone() const;
    /** Enables/disables the channel free indication tone. */
    virtual void setChFreeIndicationTone(bool enable);
    /** Returns @c true, if password and lock is enabled. */
    virtual bool passwdAndLock() const;
    /** Enables/disables password and lock. */
    virtual void enablePasswdAndLock(bool enable);
    /** Returns @c true, if the talk permit tone is enabled for DMR channels. */
    virtual bool talkPermitToneDigital() const;
    /** Enables/disables talk permit tone for DMR channels. */
    virtual void enableTalkPermitToneDigital(bool enable);
    /** Returns @c true, if the talk permit tone is enabled for analog channels. */
    virtual bool talkPermitToneAnalog() const;
    /** Enables/disables talk permit tone for analog channels. */
    virtual void enableTalkPermitToneAnalog(bool enable);

    /** Returns @c true, if intro picture is enabled. */
    virtual bool introPicture() const;
    /** Enables/disables the intro picture. */
    virtual void enableIntroPicture(bool enable);

    /** Returns the default DMR ID of the radio. */
    virtual uint32_t dmrId() const;
    /** Sets the default DMR ID of the radio. */
    virtual void setDMRId(uint32_t id);

    /** Returns the TX preamble duration. */
    virtual unsigned txPreambleDuration() const;
    /** Sets the TX preamble duration. */
    virtual void setTXPreambleDuration(unsigned ms);
    /** Returns the group call hang time. */
    virtual unsigned groupCallHangTime() const;
    /** Sets the group call hang time. */
    virtual void setGroupCallHangTime(unsigned ms);
    /** Returns the private call hang time. */
    virtual unsigned privateCallHangTime() const;
    /** Sets the private call hang time. */
    virtual void setPrivateCallHangTime(unsigned ms);
    /** Returns the VOX sensitivity. */
    virtual unsigned voxSesitivity() const;
    /** Sets the group call hang time. */
    virtual void setVOXSesitivity(unsigned ms);
    /** Returns the low-battery warning interval. */
    virtual unsigned lowBatteryInterval() const;
    /** Sets the low-battery warning interval. */
    virtual void setLowBatteryInterval(unsigned sec);

    /** Returns @c true if the call-alert is continuous. */
    virtual bool callAlertToneIsContinuous() const;
    /** Returns the call-alert tone duration. */
    virtual unsigned callAlertToneDuration() const;
    /** Sets the call-alert tone duration. */
    virtual void setCallAlertToneDuration(unsigned sec);
    /** Sets the call-alert tone continuous. */
    virtual void setCallAlertToneContinuous();

    /** Returns the lone-worker response time. */
    virtual unsigned loneWorkerResponseTime() const;
    /** Sets the lone-worker response time. */
    virtual void setLoneWorkerResponseTime(unsigned min);
    /** Returns the lone-worker reminder time. */
    virtual unsigned loneWorkerReminderTime() const;
    /** Sets the lone-worker reminder time. */
    virtual void setLoneWorkerReminderTime(unsigned min);
    /** Returns the scan digital hang time. */
    virtual unsigned scanDigitalHangTime() const;
    /** Sets the scan digital hang time. */
    virtual void setScanDigitalHangTime(unsigned ms);
    /** Returns the scan analog hang time. */
    virtual unsigned scanAnalogHangTime() const;
    /** Sets the scan analog hang time. */
    virtual void setScanAnalogHangTime(unsigned ms);

    /** Returns @c true if the backlight is always on. */
    virtual bool backlightIsAlways() const;
    /** Returns the backlight time. */
    virtual unsigned backlightTime() const;
    /** Sets the backlight time. */
    virtual void setBacklightTime(unsigned sec);
    /** Turns the backlight always on. */
    virtual void backlightTimeSetAlways();

    /** Returns @c true if the keypad lock is manual. */
    virtual bool keypadLockIsManual() const;
    /** Returns the keypad lock time. */
    virtual unsigned keypadLockTime() const;
    /** Sets the keypad lock time. */
    virtual void setKeypadLockTime(unsigned sec);
    /** Set keypad lock to manual. */
    virtual void keypadLockTimeSetManual();

    /** Returns the 8-digit power-on password. */
    virtual uint32_t powerOnPassword() const;
    /** Sets the 8-digit power-on password. */
    virtual void setPowerOnPassword(uint32_t passwd);

    /** Returns @c true, if the radio programming password is enabled. */
    virtual bool radioProgPasswordEnabled() const;
    /** Returns the 8-digit radio programming password. */
    virtual uint32_t radioProgPassword() const;
    /** Sets the 8-digit radio programming password. */
    virtual void setRadioProgPassword(uint32_t passwd);
    /** Disables the radio programming password. */
    virtual void radioProgPasswordDisable();

    /** Returns @c true, if the PC programming password is enabled. */
    virtual bool pcProgPasswordEnabled() const;
    /** Returns the PC programming password. */
    virtual QString pcProgPassword() const;
    /** Sets the PC programming password. */
    virtual void setPCProgPassword(const QString &pass);
    /** Disables the PC programming password. */
    virtual void pcProgPasswordDisable();

    /** Returns the radio name. */
    virtual QString radioName() const;
    /** Sets the radio name. */
    virtual void setRadioName(const QString &name);

    /** Encodes the general settings. */
    virtual bool fromConfig(const Config *config);
    /** Updates config from general settings. */
    virtual bool updateConfig(Config *config);
  };

  /** Codeplug representation of programming time-stamp and CPS version.
   *
   * Memory layout of encoded timestamp:
   * @verbinclude tyt_timestamp.txt */
  class TimestampElement: public Codeplug::Element
  {
  protected:
    /** Hidden constructor. */
    TimestampElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    explicit TimestampElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~TimestampElement();

    void clear();

    /** Returns the time stamp. */
    virtual QDateTime timestamp() const;
    /** Sets the time stamp. */
    virtual void setTimestamp(const QDateTime &ts);

    /** Returns the CPS version. */
    virtual QString cpsVersion() const;
  };

  /** Represents a single GPS system within the codeplug.
   *
   * Memory layout of encoded GPS system (size 0x0010 bytes):
   * @verbinclude tyt_gpssystem.txt */
  class GPSSystemElement: public Codeplug::Element
  {
  protected:
    /** Hidden constructor. */
    GPSSystemElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    explicit GPSSystemElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~GPSSystemElement();

    bool isValid() const;
    void clear();

    /** Returns @c true if the revert channel is the current one. */
    virtual bool revertChannelIsSelected() const;
    /** Returns the revert channel index (+1). */
    virtual uint16_t revertChannelIndex() const;
    /** Sets the revert channel index (+1). */
    virtual void setRevertChannelIndex(uint16_t idx);
    /** Sets the revert channel to the current one. */
    virtual void setRevertChannelSelected();

    /** Returns @c true if the repeat interval is disabled. */
    virtual bool repeatIntervalDisabled() const;
    /** Returns the repeat interval. */
    virtual unsigned repeatInterval() const;
    /** Sets the repeat interval in seconds. */
    virtual void setRepeatInterval(unsigned sec);
    /** Disables the GPS repeat interval. */
    virtual void disableRepeatInterval();

    /** Returns @c true if the destination contact is disabled. */
    virtual bool destinationContactDisabled() const;
    /** Returns the destination contact index (+1). */
    virtual uint16_t destinationContactIndex() const;
    /** Sets the destination contact index (+1). */
    virtual void setDestinationContactIndex(uint16_t idx);
    /** Disables the destination contact. */
    virtual void disableDestinationContact();

    /** Encodes the given GPS system. */
    virtual bool fromGPSSystemObj(GPSSystem *sys, Context &ctx);
    /** Constructs a GPS system. */
    virtual GPSSystem *toGPSSystemObj();
    /** Links the given GPS system. */
    virtual bool linkGPSSystemObj(GPSSystem *sys, Context &ctx);
  };

  /** Represents all menu settings within the codeplug on the radio.
   *
   * Memory representation of the menu settings:
   * @verbinclude tyt_menusettings.txt */
  class MenuSettingsElement: public Codeplug::Element
  {
  protected:
    /** Hidden constructor. */
    MenuSettingsElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    explicit MenuSettingsElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~MenuSettingsElement();

    void clear();

    /** Returns @c true if the menu hang time is infinite. */
    virtual bool menuHangtimeIsInfinite() const;
    /** Returns the menu hang time in seconds. */
    virtual unsigned menuHangtime() const;
    /** Sets the menu hang time in seconds. */
    virtual void setMenuHangtime(unsigned sec);
    /** Sets the menu hang time to be infinite. */
    virtual void infiniteMenuHangtime();

    /** Returns @c true if text message menu is enabled. */
    virtual bool textMessage() const;
    /** Enables/disables text message menu. */
    virtual void enableTextMessage(bool enable);
    /** Returns @c true if call alert menu is enabled. */
    virtual bool callAlert() const;
    /** Enables/disables call alert menu. */
    virtual void enableCallAlert(bool enable);
    /** Returns @c true if contact editing is enabled. */
    virtual bool contactEditing() const;
    /** Enables/disables contact editing. */
    virtual void enableContactEditing(bool enable);
    /** Returns @c true if manual dial is enabled. */
    virtual bool manualDial() const;
    /** Enables/disables manual dial. */
    virtual void enableManualDial(bool enable);
    /** Returns @c true if contact radio-check menu is enabled. */
    virtual bool remoteRadioCheck() const;
    /** Enables/disables contact radio-check menu. */
    virtual void enableRemoteRadioCheck(bool enable);
    /** Returns @c true if remote monitor menu is enabled. */
    virtual bool remoteMonitor() const;
    /** Enables/disables remote monitor menu. */
    virtual void enableRemoteMonitor(bool enable);
    /** Returns @c true if radio enable menu is enabled. */
    virtual bool remoteRadioEnable() const;
    /** Enables/disables radio enable menu. */
    virtual void enableRemoteRadioEnable(bool enable);
    /** Returns @c true if radio disable menu is enabled. */
    virtual bool remoteRadioDisable() const;
    /** Enables/disables radio disable menu. */
    virtual void enableRemoteRadioDisable(bool enable);

    /** Returns @c true if scan menu is enabled. */
    virtual bool scan() const;
    /** Enables/disables scan menu. */
    virtual void enableScan(bool enable);
    /** Returns @c true if edit scan-list menu is enabled. */
    virtual bool scanListEditing() const;
    /** Enables/disables edit scan-list menu. */
    virtual void enableScanListEditing(bool enable);
    /** Returns @c true if call-log missed menu is enabled. */
    virtual bool callLogMissed() const;
    /** Enables/disables call-log missed menu. */
    virtual void enableCallLogMissed(bool enable);
    /** Returns @c true if call-log answered menu is enabled. */
    virtual bool callLogAnswered() const;
    /** Enables/disables call-log answered menu. */
    virtual void enableCallLogAnswered(bool enable);
    /** Returns @c true if call-log outgoing menu is enabled. */
    virtual bool callLogOutgoing() const;
    /** Enables/disables call-log outgoing menu. */
    virtual void enableCallLogOutgoing(bool enable);
    /** Returns @c true if talkaround menu is enabled. */
    virtual bool talkaround() const;
    /** Enables/disables talkaround menu. */
    virtual void enableTalkaround(bool enable);
    /** Returns @c true if tone/alert menu is enabled. */
    virtual bool alertTone() const;
    /** Enables/disables tone/alert  menu. */
    virtual void enableAlertTone(bool enable);

    /** Returns @c true if power menu is enabled. */
    virtual bool power() const;
    /** Enables/disables power menu. */
    virtual void enablePower(bool enable);
    /** Returns @c true if backlight menu is enabled. */
    virtual bool backlight() const;
    /** Enables/disables backlight menu. */
    virtual void enableBacklight(bool enable);
    /** Returns @c true if intro screen menu is enabled. */
    virtual bool bootScreen() const;
    /** Enables/disables intro screen menu. */
    virtual void enableBootScreen(bool enable);
    /** Returns @c true if keypad lock menu is enabled. */
    virtual bool keypadLock() const;
    /** Enables/disables keypad lock menu. */
    virtual void enableKeypadLock(bool enable);
    /** Returns @c true if LED indicator menu is enabled. */
    virtual bool ledIndicator() const;
    /** Enables/disables LED indicator menu. */
    virtual void enableLEDIndicator(bool enable);
    /** Returns @c true if squelch menu is enabled. */
    virtual bool squelch() const;
    /** Enables/disables squelch menu. */
    virtual void enableSquelch(bool enable);
    /** Returns @c true if VOX menu is enabled. */
    virtual bool vox() const;
    /** Enables/disables VOX menu. */
    virtual void enableVOX(bool enable);

    /** Returns @c true if password menu is enabled. */
    virtual bool password() const;
    /** Enables/disables password menu. */
    virtual void enablePassword(bool enable);
    /** Returns @c true if display mode menu is enabled. */
    virtual bool displayMode() const;
    /** Enables/disables display mode menu. */
    virtual void enableDisplayMode(bool enable);
    /** Returns @c true if program radio menu is enabled. */
    virtual bool radioProgramming() const;
    /** Enables/disables program radio menu. */
    virtual void enableRadioProgramming(bool enable);

    /** Encodes the menu settings. */
    virtual bool fromConfig(const Config *config);
    /** Updates config from menu settings. */
    virtual bool updateConfig(Config *config);
  };

  /** Represents all button settings within the codeplug on the radio.
   *
   * Memory representation of the button settings:
   * @verbinclude tyt_buttonsettings.txt */
  class ButtonSettingsElement: public Codeplug::Element
  {
  public:
    /** The possible button actions. */
    typedef enum TyTButtonSettings::ButtonAction ButtonAction;

  protected:
    /** Hidden constructor. */
    ButtonSettingsElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    explicit ButtonSettingsElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~ButtonSettingsElement();

    void clear();

    /** Returns the action for a short press on side button 1. */
    virtual ButtonAction sideButton1Short() const;
    /** Sets the action for a short press on side button 1. */
    virtual void setSideButton1Short(ButtonAction action);
    /** Returns the action for a long press on side button 1. */
    virtual ButtonAction sideButton1Long() const;
    /** Sets the action for a short press on side button 1. */
    virtual void setSideButton1Long(ButtonAction action);

    /** Returns the action for a short press on side button 2. */
    virtual ButtonAction sideButton2Short() const;
    /** Sets the action for a short press on side button 2. */
    virtual void setSideButton2Short(ButtonAction action);
    /** Returns the action for a long press on side button 2. */
    virtual ButtonAction sideButton2Long() const;
    /** Sets the action for a short press on side button 2. */
    virtual void setSideButton2Long(ButtonAction action);

    /** Returns the long-press duration in ms. */
    virtual unsigned longPressDuration() const;
    /** Sets the long-press duration in ms. */
    virtual void setLongPressDuration(unsigned ms);

    /** Encodes the button settings. */
    virtual bool fromConfig(const Config *config);
    /** Updates config from button settings. */
    virtual bool updateConfig(Config *config);
  };

  /** Represents a single one-touch setting within the codeplug on the radio.
   *
   * Memory representation of a one-touch setting:
   * @verbinclude tyt_onetouchsettings.txt */
  class OneTouchSettingElement: public Codeplug::Element
  {
  public:
    /** Possible one-touch actions. */
    enum Action {
      CALL    = 0b0000,                 ///< Call someone, see @c contact.
      MESSAGE = 0b0001,                 ///< Send a message, see @c message.
      DTMF1   = 0b1000,                 ///< Analog call DTMF system 1.
      DTMF2   = 0b1001,                 ///< Analog call DTMF system 2.
      DTMF3   = 0b1010,                 ///< Analog call DTMF system 3.
      DTMF4   = 0b1011                  ///< Analog call DTMF system 4.
    };

    /** Possible one-touch action types. */
    enum Type {
      Disabled = 0b00,                  ///< Disabled one-touch.
      Digital  = 0b01,                  ///< Digital call/message.
      Analog   = 0b10                   ///< Analog call.
    };

  protected:
    /** Hidden constructor. */
    OneTouchSettingElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    explicit OneTouchSettingElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~OneTouchSettingElement();

    bool isValid() const;
    void clear();

    /** Returns the action to perform. */
    virtual Action action() const;
    /** Sets the action to perform. */
    virtual void setAction(Action action);

    /** Returns the type of the action. */
    virtual Type actionType() const;
    /** Sets the type of the action. */
    virtual void setActionType(Type action);

    /** Returns the message index +1. */
    virtual uint8_t messageIndex() const;
    /** Sets the message index +1. */
    virtual void setMessageIndex(uint8_t idx);

    /** Returns the contact index +1. */
    virtual uint16_t contactIndex() const;
    /** Sets the contact index +1. */
    virtual void setContactIndex(uint16_t idx);
  };

  /** Represents the emergency settings within the codeplug on the radio.
   *
   * Memory representation of the emergency settings (size 0x0010 bytes):
   * @verbinclude tyt_emergencysettings.txt */
  class EmergencySettingsElement: public Codeplug::Element
  {
  protected:
    /** Hidden constructor. */
    EmergencySettingsElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    explicit EmergencySettingsElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~EmergencySettingsElement();

    virtual void clear();

    /** Returns @c true if emergency remote monitor is enabled. */
    virtual bool emergencyRemoteMonitor() const;
    /** Enables/disables emergency remote monitor. */
    virtual void enableEmergencyRemoteMonitor(bool enable);
    /** Returns @c true if remote monitor is enabled. */
    virtual bool remoteMonitor() const;
    /** Enables/disables remote monitor. */
    virtual void enableRemoteMonitor(bool enable);
    /** Returns @c true if radio disable is enabled. */
    virtual bool radioDisable() const;
    /** Enables/disables radio disable. */
    virtual void enableRadioDisable(bool enable);

    /** Returns the remote monitor duration in seconds. */
    virtual unsigned remoteMonitorDuration() const;
    /** Sets the remote monitor duration in seconds. */
    virtual void setRemoteMonitorDuration(unsigned sec);

    /** Returns the TX time-out in ms. */
    virtual unsigned txTimeOut() const;
    /** Sets the TX time-out in ms. */
    virtual void setTXTimeOut(unsigned ms);

    /** Returns the message limit. */
    virtual unsigned messageLimit() const;
    /** Sets the message limit. */
    virtual void setMessageLimit(unsigned limit);
  };

  /** Represents a single emergency system within the radio.
   *
   * Memory representation of emergency system (size 0x0028 bytes):
   * @verbinclude tyt_emergencysystem.txt */
  class EmergencySystemElement: public Codeplug::Element
  {
  public:
    /** Possible alarm type for the system. */
    enum AlarmType {
      DISABLED = 0,                     ///< No alarm at all
      REGULAR = 1,                      ///< Regular alarm sound.
      SILENT = 2,                       ///< Silent alarm.
      SILENT_W_VOICE = 3                ///< silent alarm with voice.
    };
    /** Possible alarm modes for the system. */
    enum AlarmMode {
      ALARM = 0,                        ///< Just alarm.
      ALARM_W_CALL = 1,                 ///< Alarm + call.
      ALARM_W_VOICE = 2                 ///< Alarm + call + voice?
    };

  protected:
    /** Hidden constructor. */
    EmergencySystemElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    explicit EmergencySystemElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~EmergencySystemElement();

    bool isValid() const;
    void clear();

    /** Returns the name of the system. */
    virtual QString name() const;
    /** Sets the name of the system. */
    virtual void setName(const QString &name);

    /** Returns the alarm type of the system. */
    virtual AlarmType alarmType() const;
    /** Sets the alarm type of the system. */
    virtual void setAlarmType(AlarmType type);
    /** Returns the alarm mode of the system. */
    virtual AlarmMode alarmMode() const;
    /** Sets the alarm mode of the system. */
    virtual void setAlarmMode(AlarmMode mode);

    /** Returns the number of impolite retries. */
    virtual unsigned impoliteRetries() const;
    /** Sets the number of impolite retries. */
    virtual void setImpoliteRetries(unsigned num);

    /** Returns the number of polite retries. */
    virtual unsigned politeRetries() const;
    /** Sets the number of polite retries. */
    virtual void setPoliteRetries(unsigned num);

    /** Returns the hot MIC duration in seconds. */
    virtual unsigned hotMICDuration() const;
    /** Sets the hot MIC duration in seconds. */
    virtual void setHotMICDuration(unsigned sec);

    /** Returns @c true if the revert channel is the selected one. */
    virtual bool revertChannelIsSelected() const;
    /** Returns the index of the revert channel. */
    virtual uint16_t revertChannelIndex() const;
    /** Sets the revert channel index. */
    virtual void setRevertChannelIndex(uint16_t idx);
    /** Sets revert channel to selected channel. */
    virtual void revertChannelSelected();
  };

  /** Represents all encryption keys and settings within the codeplug on the device.
   *
   * Memory representation of encryption settings:
   * @verbinclude tyt_privacy.txt */
  class EncryptionElement: public Codeplug::Element
  {
  protected:
    /** Hidden constructor. */
    EncryptionElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    explicit EncryptionElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~EncryptionElement();

    void clear();

    /** Returns @c true if the n-th "enhanced" key (128bit) is set.
     * That is, if it is not filled with 0xff. */
    virtual bool isEnhancedKeySet(unsigned n) const;
    /** Returns the n-th "enhanced" key (128bit). */
    virtual QByteArray enhancedKey(unsigned n) const;
    /** Sets the n-th "enhanced" key (128bit). */
    virtual void setEnhancedKey(unsigned n, const QByteArray &key);

    /** Returns @c true if the n-th "basic" key (16bit) is set.
     * That is, if it is not filled with 0xff. */
    virtual bool isBasicKeySet(unsigned n) const;
    /** Returns the n-th "basic" key (16bit). */
    virtual QByteArray basicKey(unsigned n) const;
    /** Sets the n-th "basic" key (16bit). */
    virtual void setBasicKey(unsigned n, const QByteArray &key);

    /** Encodes given commercial extension. */
    virtual bool fromCommercialExt(CommercialExtension *encr, Context &ctx);
    /** Updates the commercial extension. */
    virtual bool updateCommercialExt(Context &ctx);
    /** Links the given encryption extension. */
    virtual bool linkCommercialExt(CommercialExtension *ext, Context &ctx);

  protected:
    /** Number of enhanced keys. */
    unsigned _numEnhancedKeys;
    /** Number of basic keys. */
    unsigned _numBasicKeys;
  };

protected:
  /** Empty constructor. */
  explicit TyTCodeplug(QObject *parent = nullptr);

public:
  /** Destructor. */
  virtual ~TyTCodeplug();

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

  /** Clears the time-stamp in the codeplug. */
  virtual void clearTimestamp() = 0;
  /** Sets the time-stamp. */
  virtual bool encodeTimestamp() = 0;

  /** Clears the general settings in the codeplug. */
  virtual void clearGeneralSettings() = 0;
  /** Updates the general settings from the given configuration. */
  virtual bool encodeGeneralSettings(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Updates the given configuration from the general settings. */
  virtual bool decodeGeneralSettings(Config *config, const ErrorStack &err=ErrorStack()) = 0;

  /** Clears all contacts in the codeplug. */
  virtual void clearContacts() = 0;
  /** Encodes all digital contacts in the configuration into the codeplug. */
  virtual bool encodeContacts(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Adds a digital contact to the configuration for each one in the codeplug. */
  virtual bool createContacts(Config *config, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;

  /** Clears all RX group lists in the codeplug. */
  virtual void clearGroupLists() = 0;
  /** Encodes all group lists in the configuration into the codeplug. */
  virtual bool encodeGroupLists(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Adds a RX group list to the configuration for each one in the codeplug. */
  virtual bool createGroupLists(Config *config, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Links all added RX group lists within the configuration. */
  virtual bool linkGroupLists(Context &ctx, const ErrorStack &err=ErrorStack()) = 0;

  /** Clears all channels in the codeplug. */
  virtual void clearChannels() = 0;
  /** Encodes all channels in the configuration into the codeplug. */
  virtual bool encodeChannels(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Adds a channel to the configuration for each one in the codeplug. */
  virtual bool createChannels(Config *config, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Links all added channels within the configuration. */
  virtual bool linkChannels(Context &ctx, const ErrorStack &err=ErrorStack()) = 0;

  /** Clears all zones in the codeplug. */
  virtual void clearZones() = 0;
  /** Encodes all zones in the configuration into the codeplug. */
  virtual bool encodeZones(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Adds a zone to the configuration for each one in the codeplug. */
  virtual bool createZones(Config *config, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Links all added zones within the configuration. */
  virtual bool linkZones(Context &ctx, const ErrorStack &err=ErrorStack()) = 0;

  /** Clears all scan lists in the codeplug. */
  virtual void clearScanLists() = 0;
  /** Encodes all scan lists in the configuration into the codeplug. */
  virtual bool encodeScanLists(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Adds a scan list to the configuration for each one in the codeplug. */
  virtual bool createScanLists(Config *config, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Links all added scan lists within the configuration. */
  virtual bool linkScanLists(Context &ctx, const ErrorStack &err=ErrorStack()) = 0;

  /** Clears all positioning systems in the codeplug. */
  virtual void clearPositioningSystems() = 0;
  /** Encodes all DMR positioning systems in the configuration into the codeplug. */
  virtual bool encodePositioningSystems(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Adds a GPS positioning system to the configuration for each one in the codeplug. */
  virtual bool createPositioningSystems(Config *config, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Links all added positioning systems within the configuration. */
  virtual bool linkPositioningSystems(Context &ctx, const ErrorStack &err=ErrorStack()) = 0;

  /** Clears the button settings in the codeplug. */
  virtual void clearButtonSettings() = 0;
  /** Encodes the button settings. */
  virtual bool encodeButtonSettings(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Decodes the button settings. */
  virtual bool decodeButtonSetttings(Config *config, const ErrorStack &err=ErrorStack()) = 0;

  /** Clears all encryption keys in the codeplug. */
  virtual void clearPrivacyKeys() = 0;
  /** Encodes the encryption keys. */
  virtual bool encodePrivacyKeys(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Decodes the encryption keys. */
  virtual bool decodePrivacyKeys(Config *config, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;

  /** Clears the menu settings in the codeplug. */
  virtual void clearMenuSettings() = 0;
  /** Clears all text messages in the codeplug. */
  virtual void clearTextMessages() = 0;
  /** Clears all emergency systems in the codeplug. */
  virtual void clearEmergencySystems() = 0;
};

#endif // TYT_CODEPLUG_HH
