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
 *  <tr><td>0x00000</td> <td>0x00000</td>  <td>0x0000</td>  <td></td></tr>
 *  <tr><td>0x003b4</td> <td>0x00418</td>  <td>0x0064</td>  <td>General settings, see
 *      @cDR1801UVCodeplug::SettingsElement. </td></tr>
 *  <tr><td>0x00420</td> <td>0x04110</td>  <td>0x3cf0</td>  <td>150 zones, see
 *      @c DR1801UVCodeplug::ZoneElement. </td></tr>
 *  <tr><td>0x04110</td> <td>0x04274</td>  <td>0x0164</td>  <td>Message bank, see
 *      @c DR1801UVCodeplug::MessageBankElement. </td></tr>
 *  <tr><td>0x04330</td> <td>0x00008</td>  <td>0x0008</td>  <td>Number of contacts.</td></tr>
 *  <tr><td>0x04338</td> <td>0x0a338</td>  <td>0x6000</td>  <td>1024 contacts, see
 *      @c DR1801UVCodeplug::ContactElement. </td></tr>
 *  <tr><td>0x0a33c</td> <td>0x?????</td>  <td>0x????</td>  <td>?? scan lists, see
 *      @c DR1801UVCodeplug::ScanListElement. </td></tr>
 *  <tr><td>0x0a660</td> <td>0x17660</td>  <td>0xd000</td>  <td>1024 Channel settings.
 *      34h bytes each. Names are stored separately. See @c DR1801UVCodeplug::ChannelElement for
 *      details.</td></tr>
 *  <tr><td>0x17660</td> <td>0x1c660</td>  <td>0x5000</td>  <td>1024 Channel names, 20 ASCII chars,
 *      0-terminated. Unset/unused names are filled with 0.</td></tr>
 *  <tr><td>0x1c6e0</td> <td>0x1d7e0</td>  <td>0x1100</td>  <td>64 group lists. See
 *      @c DR1801UVCodeplug::GroupListElement.</td></tr>
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

    /** Retunrs the 0-based index of the channel. */
    virtual uint16_t index() const;
    /** Sets the index. */
    virtual void setIndex(uint16_t idx);

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
    virtual uint8_t encryptionKeyIndex() const;
    /** Sets the encryption key index. */
    virtual void setEncryptionKeyIndex(uint8_t index);
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
    virtual uint8_t alarmSystemIndex() const;
    /** Sets the alarm system index. */
    virtual void setAlarmSystemIndex(uint8_t index);
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
    virtual uint8_t scanListIndex() const;
    /** Sets the scan list index. */
    virtual void setScanListIndex(uint8_t index);
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
    virtual uint8_t pttIDIndex() const;
    /** Sets the PTT-ID index. */
    virtual void setPTTIDIndex(uint8_t idx);
    /** Clears the PTT ID. */
    virtual void clearPTTID();

    /** Retruns @c true if a group list is assigned. */
    virtual bool hasGroupList() const;
    /** Retunrs the group-list index. */
    virtual uint8_t groupListIndex() const;
    /** Sets the group-list index. */
    virtual void setGroupListIndex(uint8_t index);
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

    /** Returns the index of the group list. */
    virtual uint16_t index() const;
    /** Sets the index of the group list. */
    virtual void setIndex(uint16_t index);

    /** Returns the number of elements in the list. */
    virtual uint16_t count() const;
    /** Returns @c true if the n-th member index is set. */
    virtual bool hasMemberIndex(uint8_t n) const;
    /** Retuns the n-th member index. */
    virtual uint16_t memberIndex(uint8_t n) const;
    /** Sets the n-th member index. */
    virtual void setMemberIndex(uint8_t n, uint16_t index);
    /** Clears the n-th member index. */
    virtual void clearMemberIndex(uint8_t n);

    /** Constructs a group list object from this elmeent. */
    virtual RXGroupList *toGroupListObj(Context &ctx, const ErrorStack &err=ErrorStack()) const;
    /** Links the group list object. */
    virtual bool linkGroupListObj(RXGroupList *list, Context &ctx, const ErrorStack &err=ErrorStack());
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
  };

  /** Implements the binary encoding of the preset message bank element.
   *
   * Memory representation of the message bank element (4h bytes):
   * @verbinclude dr1801uv_messagebankelement.txt */
  class MessageBankElemnt: public Element {

  };

  class MessageElement: public Element {

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

  /** Decode channel elements. */
  virtual bool decodeChannels(Context &ctx, const ErrorStack &err=ErrorStack());
  /** Link channel elements. */
  virtual bool linkChannels(Context &ctx, const ErrorStack &err=ErrorStack());

  /** Decode contact elements. */
  virtual bool decodeContacts(Context &ctx, const ErrorStack &err=ErrorStack());
  /** Link contact elements. */
  virtual bool linkContacts(Context &ctx, const ErrorStack &err=ErrorStack());

  /** Decode group list elements. */
  virtual bool decodeGroupLists(Context &ctx, const ErrorStack &err=ErrorStack());
  /** Link group lists. */
  virtual bool linkGroupLists(Context &ctx, const ErrorStack &err=ErrorStack());

  /** Decode zone elements. */
  virtual bool decodeZones(Context &ctx, const ErrorStack &err=ErrorStack());
  /** Link zones. */
  virtual bool linkZones(Context &ctx, const ErrorStack &err=ErrorStack());

  /** Decode settings element. */
  virtual bool decodeSettings(Context &ctx, const ErrorStack &err=ErrorStack());

  /** Decode scan-list elements. */
  virtual bool decodeScanLists(Context &ctx, const ErrorStack &err=ErrorStack());
  /** Link scan-lists. */
  virtual bool linkScanLists(Context &ctx, const ErrorStack &err=ErrorStack());

};

#endif // DR1801UVCODEPLUG_HH
