#ifndef GD73CODEPLUG_HH
#define GD73CODEPLUG_HH

#include "codeplug.hh"
#include "interval.hh"
#include "ranges.hh"
#include "contact.hh"
#include "rxgrouplist.hh"
#include "channel.hh"
#include "zone.hh"
#include "radioddity_extensions.hh"


/** Represents, encodes and decodes the device specific codeplug for a Radioddity GD-73.
 *
 * <table>
 *  <tr><th>Start</th>   <th>End</th>     <th>Size</th>    <th>Content</th></tr>
 *  <tr><th colspan="4">First segment 0x00000-0x22014</th></tr>
 *  <tr><td>0x00000</td> <td>0x00061</td> <td>0x0061</td>  <td>Basic info, see
 *    @c GD73Codeplug::InformationElement</td></tr>
 *  <tr><td>0x00061</td> <td>0x0010b</td> <td>0x00aa</td>  <td>Radio settings, see
 *    @c GD73Codeplug::SettingsElement</td></tr>
 *  <tr><td>0x0010b</td> <td>0x00d4c</td> <td>0x0c41</td>  <td>Zone bank, see
 *    @c GD73Codeplug::ZoneBankElement</td></tr>
 *  <tr><td>0x00d4c</td> <td>0x1254e</td> <td>0x11802</td> <td>Channel bank, see
 *    @c GD73Codeplug::ChannelBankElement</td></tr>
 *  <tr><td>0x125ff</td> <td>0x1c201</td> <td>0x9c02</td>  <td>Contact bank, see
 *    @c GD73Codeplug::ContactBankElement</td></tr>
 *  <tr><td>0x1c201</td> <td>0x21310</td> <td>0x510f</td>  <td>Group list bank, see
 *    @c GD73Codeplug::GroupListBankElement</td></tr>
 *  <tr><td>0x21310</td> <td>0x21911</td> <td>0x0601</td>  <td>Scan list bank, see
 *    @c GD73Codeplug::ScanListBankElement</td></tr>
 *  <tr><td>0x21911</td> <td>0x2191f</td> <td>0x000e</td>  <td>DMR settings, see
 *    @c GD73Codeplug::DMRSettingsElement</td></tr>
 *  <tr><td>0x2191f</td> <td>0x2196f</td> <td>0x0050</td>  <td>16 encryption keys, see
 *    @c GD73Codeplug::EncryptionKeyBankElement</td></tr>
 *  <tr><td>0x2196f</td> <td>0x21e80</td> <td>0x0511</td>  <td>Message bank, see
 *    @c GD73Codeplug::MessageBankElement</td></tr>
 *  <tr><td>0x21e80</td> <td>0x21e94</td> <td>0x0014</td>  <td>4 DTMF systems, see
 *    @c GD73Codeplug::DTMFSystemBankElement</td></tr>
 *  <tr><td>0x21e94</td> <td>0x21f24</td> <td>0x0090</td>  <td>16 DTMF numbers, see
 *    @c GD73Codeplug::DTMFNumberBankElement</td></tr>
 *  <tr><td>0x21f24</td>  <td>0x21fc4</td> <td>0x00a0</td>  <td>32 DTMF PTT settings, see
 *    @c GD73Codeplug::DTMFPTTSettingBankElement</td></tr>
 *  <tr><td>0x21fc4</td>  <td>0x22014</td> <td>0x0050</td>  <td>Unused, filled with 0x00</td></tr>
 * </table>
 *
 * @ingroup gd73 */
class GD73Codeplug : public Codeplug
{
  Q_OBJECT

public:
  /** Implements the information element.
   *
   * Memory representation of the element (size 000h bytes):
   * @verbinclude gd73_timestamp.txt */
  class InformationElement: public Element
  {
  protected:
    /** Hidden constructor. */
    InformationElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    InformationElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x0061; }

    /** Returns the frequency range, supported by the radio. */
    FrequencyRange frequencyRange() const;
    /** Overrides the frequency range settings. */
    void setFrequencyRange(const FrequencyRange &range);

    /** Returns the timestamp of the last programming. */
    QDateTime timestamp() const;
    /** Sets the timestamp of the last programming. */
    void setTimestamp(const QDateTime &timestamp);

    /** Returns the serial number as a string. */
    QString serial() const;
    /** Returns the model name. */
    QString modelName() const;
    /** Returns the device id. */
    QString deviceID() const;
    /** Returns the model number as a string. */
    QString modelNumber() const;
    /** Returns the software version as a string. */
    QString softwareVersion() const;

  public:
    /** Some limits. */
    struct Limit {
      /** Maximum length of serial number. */
      static constexpr unsigned int serial()                                { return 16; }
      /** Maximum length of model name. */
      static constexpr unsigned int modelName()                             { return 16; }
      /** Maximum length of device id. */
      static constexpr unsigned int deviceID()                              { return 16; }
      /** Maximum length of model number. */
      static constexpr unsigned int modelNumber()                           { return 16; }
      /** Maximum length of software version. */
      static constexpr unsigned int softwareVersion()                       { return 16; }
    };

  protected:
    /** Internal offsets within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int frequencyRange()                        { return 0x0000; }
      static constexpr unsigned int dateCentury()                           { return 0x0001; }
      static constexpr unsigned int dateYear()                              { return 0x0002; }
      static constexpr unsigned int dateMonth()                             { return 0x0003; }
      static constexpr unsigned int dateDay()                               { return 0x0004; }
      static constexpr unsigned int dateHour()                              { return 0x0005; }
      static constexpr unsigned int dateMinute()                            { return 0x0006; }
      static constexpr unsigned int dateSecond()                            { return 0x0007; }
      static constexpr unsigned int serial()                                { return 0x0011; }
      static constexpr unsigned int modelName()                             { return 0x0021; }
      static constexpr unsigned int deviceID()                              { return 0x0031; }
      static constexpr unsigned int modelNumber()                           { return 0x0041; }
      static constexpr unsigned int softwareVersion()                       { return 0x0051; }
      /// @endcond
    };
  };


  /** Implements one of the 5 one-touch settings elements.
   *
   * Memory representation of the element (size 0005h bytes):
   * @verbinclude gd73_one_touch_element.txt */
  class OneTouchSettingElement: public Element
  {
  public:
    /** Possible one-touch actions. */
    enum class Action {
      Call = 0, Message = 1
    };

  protected:
    /** Hidden constructor. */
    OneTouchSettingElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    OneTouchSettingElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x0005; }

  protected:
    /** Internal offsets within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int contact()                               { return 0x0001; }
      static constexpr unsigned int action()                                { return 0x0003; }
      static constexpr unsigned int message()                               { return 0x0001; }
      /// @endcond
    };
  };


  /** Implements the radio settings.
   *
   * Memory representation within the binary codeplug (size: ????h):
   * @verbinclude gd73_settings_element.txt.
   */
  class SettingsElement: public Element
  {
  public:
    /** Possible channel display modes. */
    enum class ChannelDisplayMode {
      Name = 0, Frequency = 1
    };
    /** Possible boot display modes. */
    enum class BootDisplayMode {
      Off = 0, Text = 1, Image = 2, Both = 3
    };
    /** Possible programmable key function. */
    struct KeyFunction {
    public:
      static uint8_t encode(RadioddityButtonSettingsExtension::Function func);
      static RadioddityButtonSettingsExtension::Function decode(uint8_t code);
    protected:
      enum Code {
        None=0, RadioEnable=1, RadioCheck=2, RadioDisable=3, PowerLevel=4,
        Monitor=5, EmergencyOn=6, EmergencyOff=7, ZoneSwitch=8, ToggleScan=9, ToggleVOX=10,
        OneTouch1=11, OneTouch2=12, OneTouch3=13, OneTouch4=14, OneTouch5=15, ToggleTalkaround=16,
        LoneWorker=17, TBST=18, CallSwell=19
      };
    };
    /** Possible languages. */
    enum class Language {
      Chinese=0, English=1
    };

  protected:
    /** Hidden constructor. */
    SettingsElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    SettingsElement(uint8_t *ptr);

    /** Returns the size of the settings element. */
    static constexpr unsigned int size() { return 0x0084; }

    /** Returns the radio name. */
    QString name() const;
    /** Sets the radio name. */
    void setName(const QString &name);

    /** Returns the radio ID. */
    unsigned int dmrID() const;
    /** Sets the radio ID. */
    void setDMRID(unsigned int id);

    /** Retruns the menu language. */
    Language language() const;
    /** Sets the menu language. */
    void setLanguage(Language lang);

    /** Returns the VOX level [0,10]. */
    unsigned int vox() const;
    /** Sets the VOX level [0,10]. */
    void setVOX(unsigned int level);

    /** Returns the squelch level [0,10]. */
    unsigned int squelch() const;
    /** Sets the squelch level [0,10]. */
    void setSquelch(unsigned int level);

    /** Returns @c true, if a transmit time-out is set. */
    bool totIsSet() const;
    /** Returns the transmit time-out. */
    Interval tot() const;
    /** Sets the transmit time-out. */
    void setTOT(const Interval &interval);
    /** Disables transmit time-out. */
    void clearTOT();

    /** Returns @c true if the TX interrupt is enabled. */
    bool txInterruptedEnabled() const;
    /** Enables/disables the TX interrupt. */
    void enableTXInterrupt(bool enable);

    /** Returns @c true if power save is enabled. */
    bool powerSaveEnabled() const;
    /** Enables/disables power save. */
    void enablePowerSave(bool enable);
    /** Returns the power-save time-out. */
    Interval powerSaveTimeout() const;
    /** Sets the power-save time-out. */
    void setPowerSaveTimeout(const Interval &interval);

    /** Returns @c true, if the read lock is enabled. */
    bool readLockEnabled() const;
    /** Enables/disables read lock. */
    void enableReadLock(bool enable);
    /** Returns the read-lock pin (1-6 digits as ASCII). */
    QString readLockPin() const;
    /** Sets the read-lock pin (1-6 digits as ASCII). */
    void setReadLockPin(const QString &pin);

    /** Returns @c true, if the write lock is enabled. */
    bool writeLockEnabled() const;
    /** Enables/disables write lock. */
    void enableWriteLock(bool enable);
    /** Returns the write-lock pin (1-6 digits as ASCII). */
    QString writeLockPin() const;
    /** Sets the write-lock pin (1-6 digits as ASCII). */
    void setWriteLockPin(const QString &pin);

    /** Returns the channel display mode. */
    ChannelDisplayMode channelDisplayMode() const;
    /** Sets the channel display mode. */
    void setChannelDisplayMode(ChannelDisplayMode mode);

    /** Returns the DMR microphone gain [1,10]. */
    unsigned int dmrMicGain() const;
    /** Sets the DMR microphone gain [1,10]. */
    void setDMRMicGain(unsigned int gain);
    /** Returns the FM microphone gain [1,10]. */
    unsigned int fmMicGain() const;
    /** Sets the FM microphone gain [1,10]. */
    void setFMMicGain(unsigned int gain);

    /** Returns the lone-worker response time-out. */
    Interval loneWorkerResponseTimeout() const;
    /** Sets the lone-worker response time-out. */
    void setLoneWorkerResponseTimeout(const Interval &interval);
    /** Returns the lone-worker remind period. */
    Interval loneWorkerRemindPeriod() const;
    /** Sets the lone-worker remind period. */
    void setLoneWorkerRemindPeriod(const Interval &interval);

    /** Returns the boot display mode. */
    BootDisplayMode bootDisplayMode() const;
    /** Sets the boot display mode. */
    void setBootDisplayMode(BootDisplayMode mode);
    /** Returns the first line of the boot text. */
    QString bootTextLine1() const;
    /** Sets the first line of the boot text. */
    void setBootTextLine1(const QString &line);
    /** Returns the second line of the boot text. */
    QString bootTextLine2() const;
    /** Sets the second line of the boot text. */
    void setBootTextLine2(const QString &line);

    /** Returns @c true if the key tones are enabled. */
    bool keyToneEnabled() const;
    /** Enables/disables the key tones. */
    void enableKeyTone(bool enable);
    /** Returns the key-tone volume [0-13]. */
    unsigned int keyToneVolume() const;
    /** Sets the key-tone volume. */
    void setKeyToneVolume(unsigned int vol);
    /** Returns @c true if the low-battery warn tone is enabled. */
    bool lowBatteryToneEnabled() const;
    /** Enables/disables the low-battery warn tone. */
    void enableLowBatteryTone(bool enable);
    /** Returns the low-battery warn-tone volume [0-13]. */
    unsigned int lowBatteryToneVolume() const;
    /** Sets the low-battery warn-tone volume. */
    void setLowBatteryToneVolume(unsigned int vol);

    /** Returns the long-press duration. */
    Interval longPressDuration() const;
    /** Sets the long-press duration. */
    void setLongPressDuration(const Interval &interval);
    /** Long-press function of programmable key 1. */
    RadioddityButtonSettingsExtension::Function keyFunctionLongPressP1() const;
    /** Sets the long-press function of the programmable key 1. */
    void setKeyFunctionLongPressP1(RadioddityButtonSettingsExtension::Function function);
    /** Short-press function of programmable key 1. */
    RadioddityButtonSettingsExtension::Function keyFunctionShortPressP1() const;
    /** Sets the short-press function of the programmable key 1. */
    void setKeyFunctionShortPressP1(RadioddityButtonSettingsExtension::Function function);
    /** Long-press function of programmable key 2. */
    RadioddityButtonSettingsExtension::Function keyFunctionLongPressP2() const;
    /** Sets the long-press function of the programmable key 2. */
    void setKeyFunctionLongPressP2(RadioddityButtonSettingsExtension::Function function);
    /** Short-press function of programmable key 2. */
    RadioddityButtonSettingsExtension::Function keyFunctionShortPressP2() const;
    /** Sets the short-press function of the programmable key 2. */
    void setKeyFunctionShortPressP2(RadioddityButtonSettingsExtension::Function function);
    /** Returns the n-th one-touch setting. */
    OneTouchSettingElement oneTouch(unsigned int n);

    /** Updates the given config. */
    bool updateConfig(Context &ctx, const ErrorStack &err=ErrorStack());
    /** Encodes the settings from the given config */
    bool encode(Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits of the settings. */
    struct Limit {
      /** Maximum name length. */
      static constexpr unsigned int name() { return 16; }
      /** Transmit time-out range. */
      static constexpr TimeRange tot() {
        return TimeRange{Interval::fromSeconds(20), Interval::fromSeconds(500)};
      }
      /** Power-save timeout. */
      static constexpr TimeRange powerSaveTimeout() {
        return TimeRange{Interval::fromSeconds(10), Interval::fromSeconds(60)};
      }
      /** Maximum read/write lock pin size. */
      static constexpr unsigned int pin() { return 6; }
      /** Lone-worker response time-out range. */
      static constexpr TimeRange loneWorkerResponse() {
        return TimeRange{Interval::fromMinutes(1), Interval::fromMinutes(480)};
      }
      /** Lone-worker remind period range. */
      static constexpr TimeRange loneWorkerRemindPeriod() {
        return TimeRange{Interval::fromSeconds(10), Interval::fromSeconds(200)};
      }
      /** Maximum length of the boot text lines. */
      static constexpr unsigned int bootTextLine() { return 16; }
      /** Value range for tone-volumes. */
      static constexpr IntRange toneVolume() { return IntRange{0,13}; }
      /** Long-press duration range. */
      static constexpr TimeRange longPressDuration() {
        return TimeRange{Interval::fromSeconds(0), Interval::fromSeconds(31)};
      }
    };

  protected:
    /** Internal offsets within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int name()                                  { return 0x0000; }
      static constexpr unsigned int dmrId()                                 { return 0x0020; }
      static constexpr unsigned int language()                              { return 0x0024; }
      static constexpr unsigned int voxLevel()                              { return 0x0026; }
      static constexpr unsigned int squelchLevel()                          { return 0x0027; }
      static constexpr unsigned int tot()                                   { return 0x0028; }
      static constexpr unsigned int txInterrupt()                           { return 0x0029; }
      static constexpr unsigned int powerSave()                             { return 0x002a; }
      static constexpr unsigned int powerSaveTimeout()                      { return 0x002b; }
      static constexpr unsigned int readLockEnable()                        { return 0x002c; }
      static constexpr unsigned int writeLockEnable()                       { return 0x002d; }
      static constexpr unsigned int channelDisplayMode()                    { return 0x002f; }
      static constexpr unsigned int readLockPin()                           { return 0x0030; }
      static constexpr unsigned int writeLockPin()                          { return 0x0036; }
      static constexpr unsigned int dmrMicGain()                            { return 0x003d; }
      static constexpr unsigned int fmMicGain()                             { return 0x003f; }
      static constexpr unsigned int loneWorkerResponseTimeout()             { return 0x0040; }
      static constexpr unsigned int loneWorkerReminderPeriod()              { return 0x0042; }
      static constexpr unsigned int bootDisplayMode()                       { return 0x0043; }
      static constexpr unsigned int bootTextLine1()                         { return 0x0044; }
      static constexpr unsigned int bootTextLine2()                         { return 0x0064; }
      static constexpr unsigned int keyToneEnable()                         { return 0x0084; }
      static constexpr unsigned int keyToneVolume()                         { return 0x0085; }
      static constexpr unsigned int lowBatToneEnable()                      { return 0x0086; }
      static constexpr unsigned int lowBatToneVolume()                      { return 0x0087; }
      static constexpr unsigned int longPressDuration()                     { return 0x0088; }
      static constexpr unsigned int progFuncKey1ShortPress()                { return 0x008b; }
      static constexpr unsigned int progFuncKey1LongPress()                 { return 0x008c; }
      static constexpr unsigned int progFuncKey2ShortPress()                { return 0x008d; }
      static constexpr unsigned int progFuncKey2LongPress()                 { return 0x008e; }
      static constexpr unsigned int oneTouchSettings()                      { return 0x0090; }
      static constexpr unsigned int betweenOneTouchSettings()               { return OneTouchSettingElement::size(); }
      /// @endcond
    };
  };


  /** Implements a single zone within the binary codeplug.
   *
   * Memory representation of the zone (size 0031h):
   * @verbinclude gd73_zone_element.txt */
  class ZoneElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ZoneElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    ZoneElement(uint8_t *ptr);

    /** Returns the size of the zone element. */
    static constexpr unsigned int size() { return 0x0031; }

    /** Returns the name of the zone. */
    QString name() const;
    /** Sets the name of the zone. */
    void setName(const QString &name);

    /** Decodes the zone element. */
    Zone *toZone(Context &ctx, const ErrorStack &err=ErrorStack());
    /** Links the decoded zone */
    bool linkZone(Zone *zone, Context &ctx, const ErrorStack &err=ErrorStack());
    /** Encodes the given zone. */
    bool encode(Zone *zone, Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits for the element. */
    struct Limit {
      /** Maximum name length. */
      static constexpr unsigned int nameLength()                            { return 8; }
      /** Maximum number of channels per zone. */
      static constexpr unsigned int channelCount()                          { return 16; }
    };

  protected:
    /** Internal offsets within the zone element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int name()                                  { return 0x0000; }
      static constexpr unsigned int channeCount()                           { return 0x0010; }
      static constexpr unsigned int channelIndices()                        { return 0x0011; }
      static constexpr unsigned int betweenChannelIndices()                 { return 0x0002; }
      /// @endcond
    };
  };


  /** Implements the bank of zones.
   *
   * See also @c GD73Codeplug::ZoneElement.
   *
   * Memory representation of the zone bank (size ????h bytes):
   * @verbinclude gd73_zone_bank.txt */
  class ZoneBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ZoneBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    ZoneBankElement(uint8_t *ptr);

    /** Returns the size of the zone bank element. */
    static constexpr unsigned int size() { return 0x0c41; }

    /** Creates all encoded zones, also updates the context. */
    bool createZones(Context &ctx, const ErrorStack &err);
    /** Links all decoded zones. */
    bool linkZones(Context &ctx, const ErrorStack &err);
    /** Encodess all zones. */
    bool encode(Context &ctx, const ErrorStack &err);

  public:
    /** Some limits for the zone bank. */
    struct Limit {
      /** Maximum number of zones. */
      static constexpr unsigned int zoneCount()                             { return 64; }
    };

  protected:
    /** Internal offsets within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int zoneCount()                             { return 0x0000; }
      static constexpr unsigned int zones()                                 { return 0x0001; }
      static constexpr unsigned int betweenZones()                          { return ZoneElement::size(); }
      /// @endcond
    };
  };


  /** Implements an FM/DMR channel.
   *
   * Memory representaion of the channel (size ):
   * @verbinclude gd73_channel_element.txt */
  class ChannelElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ChannelElement(uint8_t *ptr, size_t size);

  public:
    /** Possible channel types. */
    enum class Type {
      FM = 0, DMR = 1
    };
    /** Possible admit criteria. */
    enum class Admit {
      Always = 0, CC_CTCSS = 1, Free=2
    };

  public:
    /** Constructor. */
    ChannelElement(uint8_t *ptr);

    /** Returns the size of the channel element. */
    static constexpr unsigned int size() { return 0x0046; }

    /** Returns the name of the channel. */
    QString name() const;
    /** Sets the channel name. */
    void setName(const QString &name);

    /** Returns the bandwidth of the channel. */
    FMChannel::Bandwidth bandwidth() const;
    /** Sets the bandwidth. */
    void setBandwidth(FMChannel::Bandwidth bandwidth);

    /** Returns @c true, if a scan list index is set. */
    bool hasScanListIndex() const;
    /** Returns the index of the scan list. */
    unsigned int scanListIndex() const;
    /** Sets the scan list index. */
    void setScanListIndex(unsigned int idx);
    /** Clears the scan list index. */
    void clearScanListIndex();

    /** Returns the channel type. */
    Type type() const;
    /** Sets the channel type. */
    void setType(Type type);

    /** Returns @c true if talkaround is enabled. */
    bool talkaroundEnabled() const;
    /** Enable/disable talkaround. */
    void enableTalkaround(bool enable);

    /** Returns @c true if RX only is enabled. */
    bool rxOnly() const;
    /** Enables/disables RX only. */
    void enableRXOnly(bool enable);

    /** Returns @c true if scan auto-start is enabled. */
    bool scanAutoStartEnabled() const;
    /** Enables/disables scan auto-start. */
    void enableScanAutoStart(bool enable);

    /** Returns the RX frequency. */
    Frequency rxFrequency() const;
    /** Sets the RX frequency. */
    void setRXFrequency(const Frequency &f);
    /** Returns the TX frequency. */
    Frequency txFrequency() const;
    /** Sets the TX frequency. */
    void setTXFrequency(const Frequency &f);

    /** Returns @c true if channel has DTMF PTT settings index. */
    bool hasDTMFPTTSettingsIndex() const;
    /** Returns the DTMF PTT settings index. */
    unsigned int dtmfPTTSettingsIndex() const;
    /** Sets the DTMF PTT settings index. */
    void setDTMFPTTSettingsIndex(unsigned int idx);
    /** Resets the DTMF PTT settings index. */
    void clearDTMFPTTSettingsIndex();


    /** Returns the power setting. */
    Channel::Power power() const;
    /** Sets the power. */
    void setPower(Channel::Power power);

    /** Returns the admit criterion. */
    Admit admit() const;
    /** Sets the admit criterion. */
    void setAdmit(Admit admit);

    /** Returns the RX tone. */
    Signaling::Code rxTone() const;
    /** Sets the RX tone. */
    void setRXTone(Signaling::Code code);
    /** Returns the TX tone. */
    Signaling::Code txTone() const;
    /** Sets the TX tone. */
    void setTXTone(Signaling::Code code);

    /** Returns the time slot. */
    DMRChannel::TimeSlot timeSlot() const;
    /** Sets the time slot. */
    void setTimeSlot(DMRChannel::TimeSlot ts);

    /** Returns the color code. */
    unsigned int colorCode() const;
    /** Sets the color code. */
    void setColorCode(unsigned int cc);

    /** Returns @c true, if group list matches current TX contact. */
    bool groupListMatchesContact() const;
    /** Returns @c true, if no group list match is needed (monitor). */
    bool groupListAllMatch() const;
    /** Returns the group list index. */
    unsigned int groupListIndex() const;
    /** Sets the group list index. */
    void setGroupListIndex(unsigned int idx);
    /** Enables, that no group list match is needed (monitor). */
    void setGroupListAllMatch();
    /** Enables, that the group list matches the current TX contact. */
    void setGroupListMatchesContact();

    /** Returns @c true, if the transmit contact is set. */
    bool hasTXContact() const;
    /** Returns the tx contact index. */
    unsigned int txContactIndex() const;
    /** Sets the transmit contact index. */
    void setTXContactIndex(unsigned int idx);
    /** Clears the transmit contact index. */
    void clearTXContactIndex();

    /** Returns @c true if an emergency system index is set. */
    bool hasEmergencySystemIndex() const;
    /** Returns the emergency system index. */
    unsigned int emergencySystemIndex() const;
    /** Sets the emergency system index. */
    void setEmergencySystemIndex(unsigned int idx);
    /** Clears the emergency system index. */
    void clearEmergencySystemIndex();

    /** Returns @c true if an encryption key index is set. */
    bool hasEncryptionKeyIndex() const;
    /** Returns the encryption key index. */
    unsigned int encryptionKeyIndex() const;
    /** Sets the encryption key index. */
    void setEncryptionKeyIndex(unsigned int idx);
    /** Clears the encryption key index. */
    void clearEncryptionKeyIndex();

    /** Decodes the channel. */
    Channel *toChannel(Context &ctx, const ErrorStack &err=ErrorStack());
    /** Links decoded channel. */
    bool linkChannel(Channel *ch, Context &ctx, const ErrorStack &err=ErrorStack());
    /** Encodes the given channel. */
    bool encode(Channel *ch, Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits for the channel. */
    struct Limit {
      /** Maximum name length. */
      static constexpr unsigned int nameLength()                            { return 16; }
    };

  protected:
    /** Internal used offsets within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int name()                                  { return 0x0000; }
      static constexpr unsigned int bandwidth()                             { return 0x0020; }
      static constexpr unsigned int scanList()                              { return 0x0021; }
      static constexpr unsigned int channelType()                           { return 0x0022; }
      static constexpr unsigned int talkaround()                            { return 0x0023; }
      static constexpr unsigned int rxOnly()                                { return 0x0024; }
      static constexpr unsigned int scanAutoStart()                         { return 0x0026; }
      static constexpr unsigned int rxFrequency()                           { return 0x0027; }
      static constexpr unsigned int txFrequency()                           { return 0x002b; }
      static constexpr unsigned int dtmfPTTSettingsIndex()                  { return 0x002f; }
      static constexpr unsigned int power()                                 { return 0x0030; }
      static constexpr unsigned int admid()                                 { return 0x0031; }
      static constexpr unsigned int rxToneMode()                            { return 0x0034; }
      static constexpr unsigned int rxCTCSS()                               { return 0x0035; }
      static constexpr unsigned int rxDCS()                                 { return 0x0036; }
      static constexpr unsigned int txToneMode()                            { return 0x0037; }
      static constexpr unsigned int txCTCSS()                               { return 0x0038; }
      static constexpr unsigned int txDCS()                                 { return 0x0039; }
      static constexpr unsigned int timeslot()                              { return 0x003c; }
      static constexpr unsigned int colorcode()                             { return 0x003d; }
      static constexpr unsigned int groupListIndex()                        { return 0x003e; }
      static constexpr unsigned int contactIndex()                          { return 0x0040; }
      static constexpr unsigned int emergencySystemIndex()                  { return 0x0042; }
      static constexpr unsigned int encryptionKeyIndex()                    { return 0x0044; }
      /// @endcond
    };
  };


  /** Implements the bank of channels within the binary codeplug.
   *
   * See @c GD73Codeplug::ChannelElement for details on how the channels are encoded.
   *
   * Memory representation of the channel bank (size 11802h bytes):
   * @verbinclude gd73_channel_bank.txt */
  class ChannelBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ChannelBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    ChannelBankElement(uint8_t *ptr);

    /** Returns the size of the channel bank. */
    static constexpr unsigned int size() { return 0x11802; }

    /** Creates the encoded channels, also updates context. */
    bool createChannels(Context &ctx, const ErrorStack &err);
    /** Link all decoded channels. */
    bool linkChannels(Context &ctx, const ErrorStack &err);
    /** Encodes all indexed channels. */
    bool encode(Context &ctx, const ErrorStack &err);

  public:
    /** Some limits for the channel bank. */
    struct Limit {
      /** Maximum number of channels. */
      static constexpr unsigned int channelCount()                          { return 1024; }
    };

  protected:
    /** Internal offsets within the bank. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int channelCount()                          { return 0x0000; }
      static constexpr unsigned int channels()                              { return 0x0002; }
      static constexpr unsigned int betweenChannels()                       { return ChannelElement::size(); }
      /// @endcond
    };
  };


  /** Implements the contact element.
   *
   * Memory representation of the contact (size 9c02h bytes):
   * @verbinclude gd73_contact_bank.txt */
  class ContactElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ContactElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    ContactElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x025; }

    /** Returns the name of the contact. */
    QString name() const;
    /** Sets the name of the contact. */
    void setName(const QString &name);

    /** Returns the contact type. */
    DMRContact::Type type() const;
    /** Sets the contact type. */
    void setType(DMRContact::Type type);

    /** Returns the DMR ID. */
    unsigned int id() const;
    /** Sets the DMR ID. */
    void setID(unsigned int id);

    /** Decodes the contact. */
    DMRContact *toContact(Context &ctx, const ErrorStack &err=ErrorStack());
    /** Encodes the given contact. */
    bool encode(const DMRContact *contact, Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits. */
    struct Limit {
      /** The maximum name length. */
      static constexpr unsigned int nameLength()                            { return 16; }
    };

  protected:
    /** Some internal offsets within the contact. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int name()                                  { return 0x0000; }
      static constexpr unsigned int type()                                  { return 0x0020; }
      static constexpr unsigned int id()                                    { return 0x0021; }
      /// @endcond
    };
  };


  /** Implements the contact bank within the codeplug.
   *
   * See @c GD73Codeplug::ContactElement for contact encoding.
   *
   * Memory representation of the contact bank (size 9c02h bytes):
   * @verbinclude gd73_contact_bank.txt */
  class ContactBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ContactBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    ContactBankElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x9c02; }

    /** Adds all encoded contacts, also updates the context */
    bool createContacts(Context &ctx, const ErrorStack &err);
    /** Encodes all defined contacts. */
    bool encode(Context &ctx, const ErrorStack &err);

  public:
    /** Some limits. */
    struct Limit {
      /** The maximum number of contacts. */
      static constexpr unsigned int contactCount()                          { return 1024; }
    };

  protected:
    /** Some internal offsets within the contact bank. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int contactCount()                          { return 0x0000; }
      static constexpr unsigned int contacts()                              { return 0x0802; }
      static constexpr unsigned int betweenContacts()                       { return ContactElement::size(); }
      /// @endcond
    };
  };


  /** Encodes a group list.
   *
   * Memory representation of the group list (size 0053h bytes):
   * @verbinclude gd73_group_list_element.txt */
  class GroupListElement: public Element
  {
  protected:
    /** Hidden constructor. */
    GroupListElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    GroupListElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x0053; }

    /** Returns the name of the group list. */
    QString name() const;
    /** Sets the name of the group list. */
    void setName(const QString &name);

    /** Decodes the group list. */
    RXGroupList *toGroupList(Context &ctx, const ErrorStack &err=ErrorStack());
    /** Links the given RX group list. */
    bool linkGroupList(RXGroupList *lst, Context &ctx, const ErrorStack &err=ErrorStack());
    /** Encodes the group list. */
    bool encode(RXGroupList *lst, Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits. */
    struct Limit {
      /** The maximum name length. */
      static constexpr unsigned int nameLength()                          { return 8; }
      /** The maximum number of members. */
      static constexpr unsigned int memberCount()                         { return 33; }
    };

  protected:
    /** Some internal offsets within the group list. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int name()                                  { return 0x0000; }
      static constexpr unsigned int memberCount()                           { return 0x0010; }
      static constexpr unsigned int members()                               { return 0x0011; }
      static constexpr unsigned int betweenMembers()                        { return 0x0002; }
      /// @endcond
    };
  };


  /** Encodes the bank of group lists.
   *
   * See @c GD73Codeplug::GroupListElement for group list encoding.
   *
   * Memory representation of group list bank (size 510fh bytes):
   * @verbinclude gd73_group_list_bank.txt */
  class GroupListBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    GroupListBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    GroupListBankElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x510f; }

    /** Create all encoded group lists, also update context. */
    bool createGroupLists(Context &ctx, const ErrorStack &err);
    /** Link all decoded group lists. */
    bool linkGroupLists(Context &ctx, const ErrorStack &err);
    /** Encode group lists. */
    bool encode(Context &ctx, const ErrorStack &err);

  public:
    /** Some limits. */
    struct Limit {
      /** The maximum number of members. */
      static constexpr unsigned int memberCount()                         { return 250; }
    };

  protected:
    /** Some internal offsets within the group list bank. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int memberCount()                           { return 0x0000; }
      static constexpr unsigned int members()                               { return 0x0001; }
      static constexpr unsigned int betweenMembers()                        { return GroupListElement::size(); }
      /// @endcond
    };
  };


  /** Implements a scan list.
   *
   * Memory representation of the scan list (size 005fh bytes):
   * @verbinclude gd73_scan_list_element.txt */
  class ScanListElement: public Element
  {
  public:
    /** Possible priority/revert channel modes. */
    enum class ChannelMode {
      None=0, Fixed=1, Selected=2
    };

  protected:
    /** Hidden constructor. */
    ScanListElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    ScanListElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x005f; }

    /** Returns the name of the scan list. */
    QString name() const;
    /** Sets the name of the scan list. */
    void setName(const QString &name);

    /** Returns the primary channel mode. */
    ChannelMode primaryChannelMode() const;
    /** Sets the primary channel mode. */
    void setPrimaryChannelMode(ChannelMode mode);
    /** Returns @c true, if a primary zone is set. */
    bool hasPrimaryZoneIndex() const;
    /** Returns the primary zone index. */
    unsigned int primaryZoneIndex() const;
    /** Sets the primary zone index. */
    void setPrimaryZoneIndex(unsigned int idx);
    /** Clears the primary zone index. */
    void clearPrimaryZoneIndex();
    /** Returns @c true, if a primary channel is set. */
    bool hasPrimaryChannelIndex() const;
    /** Returns the primary channel index. */
    unsigned int primaryChannelIndex() const;
    /** Sets the primary channel index. */
    void setPrimaryChannelIndex(unsigned int idx);
    /** Clears the primary channel index. */
    void clearPrimaryChannelIndex();

    /** Returns the secondary channel mode. */
    ChannelMode secondaryChannelMode() const;
    /** Sets the secondary channel mode. */
    void setSecondaryChannelMode(ChannelMode mode);
    /** Returns @c true, if a secondary zone is set. */
    bool hasSecondaryZoneIndex() const;
    /** Returns the secondary zone index. */
    unsigned int secondaryZoneIndex() const;
    /** Sets the secondary zone index. */
    void setSecondaryZoneIndex(unsigned int idx);
    /** Clears the secondary zone index. */
    void clearSecondaryZoneIndex();
    /** Returns @c true, if a secondary channel is set. */
    bool hasSecondaryChannelIndex() const;
    /** Returns the secondary channel index. */
    unsigned int secondaryChannelIndex() const;
    /** Sets the secondary channel index. */
    void setSecondaryChannelIndex(unsigned int idx);
    /** Clears the secondary channel index. */
    void clearSecondaryChannelIndex();

    /** Returns the revert channel mode. */
    ChannelMode revertChannelMode() const;
    /** Sets the revert channel mode. */
    void setRevertChannelMode(ChannelMode mode);
    /** Returns @c true, if a revert zone is set. */
    bool hasRevertZoneIndex() const;
    /** Returns the revert zone index. */
    unsigned int revertZoneIndex() const;
    /** Sets the revert zone index. */
    void setRevertZoneIndex(unsigned int idx);
    /** Clears the revert zone index. */
    void clearRevertZoneIndex();
    /** Returns @c true, if a revert channel is set. */
    bool hasRevertChannelIndex() const;
    /** Returns the revert channel index. */
    unsigned int revertChannelIndex() const;
    /** Sets the revert channel index. */
    void setRevertChannelIndex(unsigned int idx);
    /** Clears the revert channel index. */
    void clearRevertChannelIndex();

    /** Returns the RX hold time. */
    Interval rxHoldTime() const;
    /** Sets the RX hold time. */
    void setRXHoldTime(const Interval &interval);
    /** Returns the TX hold time. */
    Interval txHoldTime() const;
    /** Sets the TX hold time. */
    void setTXHoldTime(const Interval &interval);

    /** Constructs a ScanList from this elemet. */
    ScanList *toScanList(Context &ctx, const ErrorStack &err=ErrorStack());
    /** Links a decoded scan list. */
    bool linkScanList(ScanList *lst, Context&ctx, const ErrorStack &err=ErrorStack());
    /** Encodes the scan list. */
    bool encode(ScanList *lst, Context&ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits. */
    struct Limit {
      /** The maximum name length. */
      static constexpr unsigned int nameLength()                          { return 8; }
      /** The maximum number of members. */
      static constexpr unsigned int memberCount()                         { return 32; }
      /** The range of hold times. */
      static TimeRange holdTime() {
        return TimeRange{Interval::fromSeconds(0), Interval::fromSeconds(10)};
      }
    };

  protected:
    /** Some internal offsets within the scan list bank. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int name()                                  { return 0x0000; }
      static constexpr unsigned int memberCount()                           { return 0x0010; }
      static constexpr unsigned int members()                               { return 0x0011; }
      static constexpr unsigned int betweenMembers()                        { return 0x0002; }
      static constexpr unsigned int priChannel1Mode()                       { return 0x0051; }
      static constexpr unsigned int priChannel2Mode()                       { return 0x0052; }
      static constexpr unsigned int priChannel1Zone()                       { return 0x0053; }
      static constexpr unsigned int priChannel2Zone()                       { return 0x0054; }
      static constexpr unsigned int priChannel1Channel()                    { return 0x0055; }
      static constexpr unsigned int priChannel2Channel()                    { return 0x0057; }
      static constexpr unsigned int txChannelMode()                         { return 0x0059; }
      static constexpr unsigned int txChannelZone()                         { return 0x005a; }
      static constexpr unsigned int txChannelChannel()                      { return 0x005b; }
      static constexpr unsigned int holdTime()                              { return 0x005d; }
      static constexpr unsigned int txHoldTime()                            { return 0x005e; }
      /// @endcond
    };
  };


  /** Implements the bank of scan lists.
   *
   * See @c GD73Codeplug::ScanListElement for the encoding of the single scan lists.
   *
   * Memory representation of the scan list bank (size 0601h bytes):
   * @verbinclude gd73_scan_list_bank.txt */
  class ScanListBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ScanListBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    ScanListBankElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x601; }

    /** Creates all encoded scan lists, also updates context. */
    bool createScanLists(Context &ctx, const ErrorStack &err);
    /** Links all decoded scan lists. */
    bool linkScanLists(Context &ctx, const ErrorStack &err);
    /** Encodes all scan lists. */
    bool encode(Context &ctx, const ErrorStack &err);

  public:
    /** Some limits. */
    struct Limit {
      /** The maximum number of members. */
      static constexpr unsigned int memberCount()                           { return 16; }
    };

  protected:
    /** Some internal offsets within the scan list bank. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int memberCount()                           { return 0x0000; }
      static constexpr unsigned int members()                               { return 0x0011; }
      static constexpr unsigned int betweenMembers()                        { return ScanListElement::size(); }
      /// @endcond
    };
  };


  /** Implements the DMR settings element.
   *
   * Memory representation of the settings (size 000eh bytes):
   * @verbinclude gd73_dmr_settings_element.txt */
  class DMRSettingsElement: public Element
  {
  protected:
    /** Hidden constructor. */
    DMRSettingsElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    DMRSettingsElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x000e; }

    /** Returns the call hang time (private & group). */
    Interval callHangTime() const;
    /** Sets the call hang time (private & group). */
    void setCallHangTime(const Interval &intv);

    /** Returns the active wait time. */
    Interval activeWaitTime() const;
    /** Sets the active wait time. */
    void setActiveWaitTime(const Interval &interval);
    /** Retruns the number of active reties. */
    unsigned int activeRetries() const;
    /** Sets the number of active retries. */
    void setActiveRetries(unsigned int count);

    /** Retruns the number of TX preambles. */
    unsigned int txPreambles() const;
    /** Sets the number of TX preambles. */
    void setTXPreambles(unsigned int count);

    /** Returns @c true, if decoding of 'disable radio' is enabled. */
    bool decodeDisableRadioEnabled() const;
    /** Enables/disables decoding of 'disable radio'. */
    void enableDecodeDisableRadio(bool enable);
    /** Returns @c true, if decoding of 'radio check' is enabled. */
    bool decodeRadioCheckEnabled() const;
    /** Enables/disables decoding of 'radio check'. */
    void enableDecodeRadioCheck(bool enable);
    /** Returns @c true, if decoding of 'enable radio' is enabled. */
    bool decodeEnableRadioEnabled() const;
    /** Enables/disables decoding of 'enable radio'. */
    void enableDecodeEnableRadio(bool enable);

    /** Updates the settings within the config. */
    bool updateConfig(Context &ctx, const ErrorStack &err=ErrorStack());
    /** Encodes the settings from the given config */
    bool encode(Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits. */
    struct Limit {
      /** The range of call hang times. */
      static constexpr TimeRange callHangTime() { return TimeRange{
          Interval::fromSeconds(1), Interval::fromSeconds(90)
        }; }
      /** The range of active wait times. */
      static constexpr TimeRange activeWaitTime() { return TimeRange{
          Interval::fromMilliseconds(120), Interval::fromMilliseconds(600)
        }; }
      /** The range of active retries. */
      static constexpr IntRange activeRetires() { return IntRange{ 1, 10}; }
      /** The maximum number of TX preambles. */
      static constexpr IntRange txPreambles() { return IntRange{ 0, 63}; }
    };

  protected:
    /** Some internal offsets within the scan list bank. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int callHangTime()                          { return 0x0000; }
      static constexpr unsigned int activeWaitTime()                        { return 0x0001; }
      static constexpr unsigned int activeRetries()                         { return 0x0002; }
      static constexpr unsigned int txPreambles()                           { return 0x0003; }
      static constexpr unsigned int decodeDisableRadio()                    { return 0x0004; }
      static constexpr unsigned int decodeCheckRadio()                      { return 0x0005; }
      static constexpr unsigned int decodeEnableRadio()                     { return 0x0006; }
      /// @endcond
    };
  };


  /** Implements the encryption key element.
   *
   * Memory representation (size 0005h bytes):
   * @verbinclude gd73_encryption_key_element.txt */
  class EncryptionKeyElement: public Element
  {
  protected:
    /** Hidden constructor. */
    EncryptionKeyElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    EncryptionKeyElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x005; }


  protected:
    /** Internal used offsets within the bank. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int size()        { return 0x0000; }
      static constexpr unsigned int key()         { return 0x0001; }
      /// @endcond
    };
  };


  /** Implements the encryption key-bank.
   *
   * Memory representation (size 0050h bytes):
   * @verbinclude gd73_encryption_key_bank.txt */
  class EncryptionKeyBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    EncryptionKeyBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    EncryptionKeyBankElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x0050; }

  public:
    /** Some limits. */
    struct Limit {
      /** The maximum number of keys. */
      static constexpr unsigned int keys()        { return 16; }
    };

  protected:
    /** Internal used offsets within the bank. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int keys()        { return 0x0000; }
      static constexpr unsigned int betweenKeys() { return EncryptionKeyElement::size(); }
      /// @endcond
    };
  };


  /** Implements a message.
   * Memory representation (size 0051h bytes):
   * @verbinclude gd73_message_element.txt */
  class MessageElement: public Element
  {
  protected:
    /** Hidden constructor. */
    MessageElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    MessageElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x051; }


  protected:
    /** Internal used offsets within the bank. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int size()        { return 0x0000; }
      static constexpr unsigned int text()        { return 0x0001; }
      /// @endcond
    };
  };


  /** Implements the message bank element.
   *
   * See @c GD73Codeplug::MessageElement for encoding of the single messages.
   *
   * Memory representation of the bank (size 0511h bytes):
   * @verbinclude gd73_message_bank.txt */
  class MessageBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    MessageBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    MessageBankElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x511; }

  public:
    /** Some limits. */
    struct Limit {
      /** The maximum number of members. */
      static constexpr unsigned int memberCount()                           { return 16; }
    };

  protected:
    /** Some internal offsets within the message bank. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int memberCount()                           { return 0x0000; }
      static constexpr unsigned int members()                               { return 0x0001; }
      static constexpr unsigned int betweenMembers()                        { return MessageElement::size(); }
      /// @endcond
    };
  };


  /** Implements a single DTMF system.
   * Memory representation of the system (size 0005h bytes):
   * @verbinclude gd73_dtmf_system_element.txt */
  class DTMFSystemElement: public Element
  {
  protected:
    /** Hidden constructor. */
    DTMFSystemElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    DTMFSystemElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x005; }

  public:
    /** Some limits. */
    struct Limit {
      /** Maximum preamble duration in ms. */
      static constexpr Interval preambleDuration() { return Interval::fromMilliseconds(1000); }
      /** Range for tone duration. */
      static constexpr TimeRange toneDuration() { return TimeRange{
          Interval::fromMilliseconds(30), Interval::fromMilliseconds(1900)
        }; }
      /** Range for pause duration. */
      static constexpr TimeRange pauseDuration() { return TimeRange{
          Interval::fromMilliseconds(30), Interval::fromMilliseconds(1900)
        }; }
      /** Rang of dead time. */
      static constexpr TimeRange deadTime() { return TimeRange{
          Interval::fromMilliseconds(200), Interval::fromSeconds(33)
        }; }
    };

  protected:
    /** Internal used offsets within the bank. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int sidetone()                              { return 0x0000; }
      static constexpr unsigned int preambleDuration()                      { return 0x0001; }
      static constexpr unsigned int toneDuration()                          { return 0x0002; }
      static constexpr unsigned int pauseDuration()                         { return 0x0003; }
      static constexpr unsigned int deadTime()                              { return 0x0004; }
      /// @endcond
    };
  };


  /** Implements the bank of 4 DTMF systems.
   * @c GD73Codeplug::DTMFSystemElement for encoding of each system. */
  class DTMFSystemBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    DTMFSystemBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    DTMFSystemBankElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x0014; }

  public:
    /** Some limits. */
    struct Limit {
      /** The number of members. */
      static constexpr unsigned int memberCount()                           { return 4; }
    };

  protected:
    /** Some internal offsets within the message bank. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int members()                               { return 0x0000; }
      static constexpr unsigned int betweenMembers()                        { return DTMFSystemElement::size(); }
      /// @endcond
    };
  };


  /** Implements a single DTMF number.
   * Memory representation of the DTMF number (size 000ah bytes):
   * @verbinclude gd73_dtmf_code_element.txt */
  class DTMFNumberElement: public Element
  {
  protected:
    /** Hidden constructor. */
    DTMFNumberElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    DTMFNumberElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x000a; }

  public:
    /** Some limits. */
    struct Limit {
      /** Maximum number of digita. */
      static constexpr unsigned int digits() { return 16; }
    };

  protected:
    /** Internal used offsets within the bank. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int count()                                 { return 0x0000; }
      static constexpr unsigned int digits()                                { return 0x0001; }
      /// @endcond
    };
  };


  /** Implements the bank of 16 DTMF numbers.
   * @c GD73Codeplug::DTMFNumberElement for encoding of each system. */
  class DTMFNumberBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    DTMFNumberBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    DTMFNumberBankElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x0090; }

  public:
    /** Some limits. */
    struct Limit {
      /** The number of members. */
      static constexpr unsigned int memberCount()                           { return 16; }
    };

  protected:
    /** Some internal offsets within the number bank. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int members()                               { return 0x0000; }
      static constexpr unsigned int betweenMembers()                        { return DTMFNumberElement::size(); }
      /// @endcond
    };
  };


  /** Implements a single DTMF PTT setting.
   * Memory representation of the DTMF PTT setting (size 0005h bytes):
   * @verbinclude gd73_dtmf_ptt_settings.txt */
  class DTMFPTTSettingElement: public Element
  {
  protected:
    /** Hidden constructor. */
    DTMFPTTSettingElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    DTMFPTTSettingElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x0005; }

  protected:
    /** Internal used offsets within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int systemIndex()                           { return 0x0000; }
      static constexpr unsigned int pttIDType()                             { return 0x0001; }
      static constexpr unsigned int pttIDMode()                             { return 0x0002; }
      static constexpr unsigned int connectIDIndex()                        { return 0x0003; }
      static constexpr unsigned int disconnectIDIndex()                     { return 0x0004; }
      /// @endcond
    };
  };


  /** Implements the bank of 32 DTMF PTT settings.
   * @c GD73Codeplug::DTMFPTTSettingElement for encoding of each element. */
  class DTMFPTTSettingBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    DTMFPTTSettingBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    DTMFPTTSettingBankElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x00a0; }

  public:
    /** Some limits. */
    struct Limit {
      /** The number of members. */
      static constexpr unsigned int memberCount()                           { return 32; }
    };

  protected:
    /** Some internal offsets within the number bank. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int members()                               { return 0x0000; }
      static constexpr unsigned int betweenMembers()                        { return DTMFPTTSettingElement::size(); }
      /// @endcond
    };
  };

public:
  /** Default constructor. */
  explicit GD73Codeplug(QObject *parent = nullptr);

  Config *preprocess(Config *config, const ErrorStack &err=ErrorStack()) const;
  bool postprocess(Config *config, const ErrorStack &err=ErrorStack()) const;

  bool index(Config *config, Context &ctx, const ErrorStack &err=ErrorStack()) const;
  bool decode(Config *config, const ErrorStack &err=ErrorStack());
  bool encode(Config *config, const Flags &flags=Flags(), const ErrorStack &err=ErrorStack());

protected:
  /** Decodes the time-stamp field. */
  virtual bool decodeTimestamp(Context &ctx, const ErrorStack &err=ErrorStack());
  /** Encodes the time-stamp field. */
  virtual bool encodeTimestamp(Context &ctx, const ErrorStack &err=ErrorStack());

  /** Creates messages. */
  virtual bool createMessages(Context &ctx, const ErrorStack &err=ErrorStack());

  /** Decodes the settings fields (generic & DMR). */
  virtual bool decodeSettings(Context &ctx, const ErrorStack &err=ErrorStack());
  /** Encode settings fields (generic & DMR settings). */
  virtual bool encodeSettings(Context &ctx, const ErrorStack &err=ErrorStack());

  /** Creates contacts. */
  virtual bool createContacts(Context &ctx, const ErrorStack &err=ErrorStack());
  /** Encode contacts. */
  virtual bool encodeContacts(Context &ctx, const ErrorStack &err=ErrorStack());

  /** Creates DTMF contacts. */
  virtual bool createDTMFContacts(Context &ctx, const ErrorStack &err=ErrorStack());

  /** Create group lists. */
  virtual bool createGroupLists(Context &ctx, const ErrorStack &err=ErrorStack());
  /** Link group lists. */
  virtual bool linkGroupLists(Context &ctx, const ErrorStack &err=ErrorStack());
  /** Encode group lists. */
  virtual bool encodeGroupLists(Context &ctx, const ErrorStack &err=ErrorStack());

  /** Create encryption keys. */
  virtual bool createEncryptionKeys(Context &ctx, const ErrorStack &err=ErrorStack());

  /** Create channels. */
  virtual bool createChannels(Context &ctx, const ErrorStack &err=ErrorStack());
  /** Link channels. */
  virtual bool linkChannels(Context &ctx, const ErrorStack &err=ErrorStack());
  /** Encode channels. */
  virtual bool encodeChannels(Context &ctx, const ErrorStack &err=ErrorStack());

  /** Create zones. */
  virtual bool createZones(Context &ctx, const ErrorStack &err=ErrorStack());
  /** Link zones. */
  virtual bool linkZones(Context &ctx, const ErrorStack &err=ErrorStack());
  /** Encode zones. */
  virtual bool encodeZones(Context &ctx, const ErrorStack &err=ErrorStack());

  /** Create scan lists. */
  virtual bool createScanLists(Context &ctx, const ErrorStack &err=ErrorStack());
  /** Link zones. */
  virtual bool linkScanLists(Context &ctx, const ErrorStack &err=ErrorStack());
  /** Encode zones. */
  virtual bool encodeScanLists(Context &ctx, const ErrorStack &err=ErrorStack());

protected:
  /** Internal used offsets within the codeplug. */
  struct Offset {
    /// @cond DO_NOT_DOCUMENT
    static constexpr unsigned int timestamp()                               { return 0x00000; }
    static constexpr unsigned int settings()                                { return 0x00061; }
    static constexpr unsigned int zones()                                   { return 0x0010b; }
    static constexpr unsigned int channels()                                { return 0x00d4c; }
    static constexpr unsigned int contacts()                                { return 0x125ff; }
    static constexpr unsigned int groupLists()                              { return 0x1c201; }
    static constexpr unsigned int scanLists()                               { return 0x21310; }
    static constexpr unsigned int dmrSettings()                             { return 0x21911; }
    static constexpr unsigned int encryptionKeys()                          { return 0x2191f; }
    static constexpr unsigned int messages()                                { return 0x2196f; }
    static constexpr unsigned int dtmfSystems()                             { return 0x21e80; }
    static constexpr unsigned int dtmfNumbers()                             { return 0x21e94; }
    static constexpr unsigned int dtmfPTTSettings()                         { return 0x21f24; }
    /// @endcond
  };
};


#endif // GD73CODEPLUG_HH
