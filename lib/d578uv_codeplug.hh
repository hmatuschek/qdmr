#ifndef D578UV_CODEPLUG_HH
#define D578UV_CODEPLUG_HH

#include <QDateTime>

#include "d878uv_codeplug.hh"

class Channel;
class DMRContact;
class Zone;
class RXGroupList;
class ScanList;
class GPSSystem;


/** Represents the device specific binary codeplug for Anytone AT-D578UV radios.
 *
 * Matches firmware/CPS version 1.21.
 *
 * @ingroup d578uv */
class D578UVCodeplug : public D878UVCodeplug
{
  Q_OBJECT

public:
  /** Represents the actual channel encoded within the binary code-plug. */
  class ChannelElement: public D878UVCodeplug::ChannelElement
  {
  public:
    /** Possible FM scrambler carrier frequencies. */
    enum class FMScramblerFrequency {
      Off = 0, Hz3300 = 1, Hz3200 = 2, Hz3100 = 3, Hz3000 = 4, Hz2900 = 5, Hz2800 = 6, Hz2700 = 7,
      Hz2600 = 8, Hz2500 = 9, Hz4095 = 10, Hz3458 = 11, Custom = 12
    };

  protected:
    /** Hidden constructor. */
    ChannelElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    ChannelElement(uint8_t *ptr);

    /** Retruns @c true, if the RX and TX frequencies (and signaling) are swapped. */
    virtual bool frequenciesSwapped() const;
    /** Retruns @c true, if the RX and TX frequencies (and signaling) are swapped. */
    virtual void enableSwapFrequencies(bool enable);

    /** Returns @c true if bluetooth hands-free is enabled. */
    virtual bool bluetoothEnabled() const;
    /** Enables/disables hands-free. */
    virtual void enableBluetooth(bool enable);

    // moved to a different bit
    bool roamingEnabled() const override;
    void enableRoaming(bool enable) override;

    // moved to a different bit
    bool multipleKeyEncryption() const override;
    void enableMultipleKeyEncryption(bool enable) override;

    bool randomKey() const override;
    void enableRandomKey(bool enable) override;

    bool sms() const override;
    void enableSMS(bool enable) override;

    bool dataACK() const override;
    void enableDataACK(bool enable) override;

    bool autoScan() const override;
    void enableAutoScan(bool enable) override;

    bool sendTalkerAlias() const override;
    void enableSendTalkerAlias(bool enable) override;

    AdvancedEncryptionType advancedEncryptionType() const override;
    void setEncryptionType(AdvancedEncryptionType type) override;

    /** Returns @c true if the analog scambler is enabled. */
    virtual bool analogScamblerEnabled() const;
    /** If enabled, returns the analog scrambler frequency. */
    virtual Frequency analogScramblerFrequency() const;
    /** Sets the analog scambler frequency and enables the scrambler. */
    virtual void setAnalogScamberFrequency(Frequency f);
    /** Disables the scambler*/
    virtual void clearAnalogScambler();

    /// Removed from D578UV codeplug
    bool ctcssPhaseReversal() const override;
    /// Removed from D578UV codeplug
    void enableCTCSSPhaseReversal(bool enable) override;

    Channel *toChannelObj(Context &ctx) const override;
    bool fromChannelObj(const Channel *ch, Context &ctx) override;

  protected:
    /** Internal offsets within the channel element. */
    struct Offset: public D878UVCodeplug::ChannelElement::Offset
    {
      /// @cond DO_NOT_DOCUMENT
      static constexpr Bit swapRxTx()                      { return {0x0009, 4}; }
      static constexpr Bit bluetooth()                     { return {0x0034, 2}; }
      static constexpr Bit noiseReduction()                { return {0x0034, 3}; }
      static constexpr Bit interruptPriority()             { return {0x0034, 4}; }
      static constexpr Bit roaming()                       { return {0x0034, 6}; }
      static constexpr unsigned int fmScrambler()          { return 0x003a; }
      static constexpr unsigned int customScrambler()      { return 0x003b; }
      static constexpr Bit multipleKeyEncryption()         { return {0x003b, 0}; }
      static constexpr Bit randomKey()                     { return {0x003b, 1}; }
      static constexpr Bit sms()                           { return {0x003b, 2}; }
      static constexpr Bit dataACK()                       { return {0x003d, 3}; }
      static constexpr Bit autoScan()                      { return {0x003d, 4}; }
      static constexpr Bit talkerAlias()                   { return {0x003d, 5}; }
      static constexpr Bit advancedEncryptionType()        { return {0x003d, 6}; }
      static constexpr unsigned int fmAPRSFrequencyIndex() { return 0x003e; }
      static constexpr unsigned int arc4KeyIndex()         { return 0x003f; }
      // Deleted
      static constexpr Bit muteFMAPRS()                    { return {0x0000, 0}; }
      static constexpr Bit ctcssPhaseReversal()            { return {0x0000, 0}; }
      /// @endcond
    };
  };


  /** Represents the a channel extension element within the binary codeplug. */
  class ChannelExtensionElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ChannelExtensionElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor from data. */
    ChannelExtensionElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x0040; }

    void clear() override;

    /** Returns the index of the 5-tone ID send at the start of the transmission. */
    virtual unsigned int fiveToneIdIndexBOT() const;
    /** Sets the index of the 5-tone ID send at the start of the transmission. */
    virtual void setFiveToneIdIndexBOT(unsigned int idx);

    /** Returns the index of the 5-tone ID send at the end of the transmission. */
    virtual unsigned int fiveToneIdIndexEOT() const;
    /** Sets the index of the 5-tone ID send at the end of the transmission. */
    virtual void setFiveToneIdIndexEOT(unsigned int idx);

  protected:
    /** Internal offsets within element. */
    struct Offset: public Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int fiveToneIdIndexBOT() { return 0x0000; }
      static constexpr unsigned int fiveToneIdIndexEOT() { return 0x0001; }
      /// @endcond
    };
  };


  /** Represents the general config of the radio within the D578UV binary codeplug.
   *
   * This class implements only the differences to the D878UV general settings
   * @c D878UVCodeplug::GeneralSettingsElement
   *
   * Binary encoding of the general settings (size 0x00f0 bytes):
   * @verbinclude d578uv_generalsettings.txt */
  class GeneralSettingsElement: public AnytoneCodeplug::GeneralSettingsElement
  {
  protected:
    /** Device specific key functions. */
    struct KeyFunction {
    public:
      /** Encodes key function. */
      static uint8_t encode(AnytoneKeySettingsExtension::KeyFunction tone);
      /** Decodes key function. */
      static AnytoneKeySettingsExtension::KeyFunction decode(uint8_t code);

    protected:
      /** Encoded key functions. */
      typedef enum {
        Off = 0x00, Voltage = 0x01, Power = 0x02, Repeater = 0x03, Reverse = 0x04,
        Encryption = 0x05, Call = 0x06, ToggleVFO = 0x07, Scan = 0x08, WFM = 0x09, Alarm = 0x0a,
        RecordSwitch = 0x0b, Record = 0x0c, SMS = 0x0d, Dial = 0x0e, GPSInformation=0x0f, Monitor = 0x10,
        ToggleMainChannel = 0x11, HotKey1 = 0x12, HotKey2 = 0x13, HotKey3 = 0x14, HotKey4 = 0x15,
        HotKey5 = 0x16, HotKey6 = 0x17, WorkAlone = 0x18, SkipChannel = 0x19, DMRMonitor = 0x1a,
        SubChannel = 0x1b, PriorityZone = 0x1c, VFOScan = 0x1d, MICSoundQuality = 0x1e,
        LastCallReply = 0x1f, ChannelType = 0x20, Ranging=0x21, Roaming = 0x22, ChannelRanging = 0x23,
        MaxVolume = 0x24, Slot = 0x25, APRSTypeSwitch=0x26, Zone = 0x27, MuteA = 0x28, MuteB = 0x29,
        RoamingSet = 0x2a, APRSSet = 0x2b, ZoneUp = 0x2c, ZoneDown = 0x2d,
        XBandRepeater = 0x30, Speaker = 0x31, ChannelName = 0x32, Bluetooth = 0x33,
        GPS = 0x34, CDTScan = 0x35, TBSTSend = 0x36, APRSSend = 0x37, APRSInfo = 0x38,
        GPSRoaming = 0x39, Squelch=0x3a, NoiseReductionTX=0x3b
      } KeyFunctionCode;
    };

    /** Device specific time zones. */
    struct TimeZone {
    public:
      /** Encodes time zone. */
      static uint8_t encode(const QTimeZone& zone);
      /** Decodes time zone. */
      static QTimeZone decode(uint8_t code);

    protected:
      /** Vector of possible time-zones. */
      static QVector<QTimeZone> _timeZones;
    };

    /** TBST (open repeater) frequencies. */
    enum class TBSTFrequency {
      Hz1000 = 0, Hz1450 = 1, Hz1750 = 2, Hz2100 = 3
    };

    /** All possible STE (squelch tail eliminate) frequencies. */
    enum class STEFrequency {
      Off = 0, Hz55_2  = 1, Hz259_2 = 2
    };

  protected:
    /** Hidden constructor. */
    GeneralSettingsElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    GeneralSettingsElement(uint8_t *ptr);

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x00f0; }

    bool keyToneEnabled() const;
    void enableKeyTone(bool enable);

    /** Returns the transmit timeout in seconds. */
    virtual unsigned transmitTimeout() const;
    /** Sets the transmit timeout in seconds. */
    virtual void setTransmitTimeout(unsigned tot);

    /** Returns the UI language. */
    virtual AnytoneDisplaySettingsExtension::Language language() const;
    /** Sets the UI language. */
    virtual void setLanguage(AnytoneDisplaySettingsExtension::Language lang);

    AnytoneSettingsExtension::VFOScanType vfoScanType() const;
    void setVFOScanType(AnytoneSettingsExtension::VFOScanType type);

    unsigned int dmrMicGain() const;
    void setDMRMicGain(unsigned int gain);

    bool vfoModeA() const;
    void enableVFOModeA(bool enable);
    bool vfoModeB() const;
    void enableVFOModeB(bool enable);

    /** Returns the STE (squelch tail eliminate) type. */
    virtual AnytoneSettingsExtension::STEType steType() const;
    /** Sets the STE (squelch tail eliminate) type. */
    virtual void setSTEType(AnytoneSettingsExtension::STEType type);
    /** Returns the STE (squelch tail eliminate) frequency setting in Hz.
     * A value of 0 disables the STE. Possible values are 55.2 and 259.2 Hz. */
    virtual double steFrequency() const;
    /** Sets the STE (squelch tail eliminate) frequency setting.
     * A value of 0 disables the STE. Possible values are 55.2 and 259.2 Hz. */
    virtual void setSTEFrequency(double freq);

    /** Returns the group call hang time in seconds. */
    virtual Interval groupCallHangTime() const;
    /** Sets the group call hang time in seconds. */
    virtual void setGroupCallHangTime(Interval sec);
    /** Returns the private call hang time in seconds. */
    virtual Interval privateCallHangTime() const;
    /** Sets the private call hang time in seconds. */
    virtual void setPrivateCallHangTime(Interval sec);

    /** Returns the pre-wave time in ms. */
    virtual Interval preWaveDelay() const;
    /** Sets the pre-wave time in ms. */
    virtual void setPreWaveDelay(Interval ms);
    /** Returns the wake head-period in ms. */
    virtual Interval wakeHeadPeriod() const;
    /** Sets the wake head-period in ms. */
    virtual void setWakeHeadPeriod(Interval ms);

    /** Returns the wide-FM (broadcast) channel index. */
    virtual unsigned wfmChannelIndex() const;
    /** Sets the wide-FM (broadcast) channel index. */
    virtual void setWFMChannelIndex(unsigned idx);
    /** Returns @c true if the WFM RX is in VFO mode. */
    virtual bool wfmVFOEnabled() const;
    /** Enables/disables VFO mode for WFM RX. */
    virtual void enableWFMVFO(bool enable);

    unsigned memoryZoneA() const;
    void setMemoryZoneA(unsigned zone);
    unsigned memoryZoneB() const;
    void setMemoryZoneB(unsigned zone);

    /** Returns @c true, if the WFM/Airband receiver is enabled. */
    virtual bool wfmEnabled() const;
    /** Enables/disables WFM/Airband receiver. */
    virtual void enableWFM(bool enable);

    bool recording() const;
    void enableRecording(bool enable);

    unsigned brightness() const;
    void setBrightness(unsigned level);

    bool gps() const;
    void enableGPS(bool enable);

    bool smsAlert() const;
    void enableSMSAlert(bool enable);

    /** Returns @c true if WFM monitor is enabled. */
    virtual bool wfmMonitor() const;
    /** Enables/disables WFM monitor. */
    virtual void enableWFMMonitor(bool enable);

    bool activeChannelB() const;
    void enableActiveChannelB(bool enable);

    bool subChannel() const;
    void enableSubChannel(bool enable);

    /** Returns the TBST frequency. */
    virtual Frequency tbstFrequency() const;
    /** Sets the TBST frequency. */
    virtual void setTBSTFrequency(Frequency freq);

    bool callAlert() const;
    void enableCallAlert(bool enable);

    QTimeZone gpsTimeZone() const;
    void setGPSTimeZone(const QTimeZone &zone);

    bool dmrTalkPermit() const;
    void enableDMRTalkPermit(bool enable);
    bool fmTalkPermit() const;
    void enableFMTalkPermit(bool enable);
    bool dmrResetTone() const;
    void enableDMRResetTone(bool enable);
    bool idleChannelTone() const;
    void enableIdleChannelTone(bool enable);

    Interval menuExitTime() const;
    void setMenuExitTime(Interval intv);

    /** Returns @c true if the own ID is filtered in call lists. */
    virtual bool filterOwnID() const;
    /** Enables/disables filter of own ID in call lists. */
    virtual void enableFilterOwnID(bool enable);

    bool startupTone() const;
    void enableStartupTone(bool enable);

    bool callEndPrompt() const;
    void enableCallEndPrompt(bool enable);

    unsigned maxSpeakerVolume() const;
    void setMaxSpeakerVolume(unsigned level);

    /** Returns @c true remote stun/kill is enabled. */
    virtual bool remoteStunKill() const;
    /** Enables/disables remote stun/kill. */
    virtual void enableRemoteStunKill(bool enable);

    /** Returns @c true remote monitor is enabled. */
    virtual bool remoteMonitor() const;
    /** Enables/disables remote monitor. */
    virtual void enableRemoteMonitor(bool enable);

    bool getGPSPosition() const;
    void enableGetGPSPosition(bool enable);

    Interval longPressDuration() const;
    void setLongPressDuration(Interval ms);

    bool volumeChangePrompt() const;
    void enableVolumeChangePrompt(bool enable);

    AnytoneAutoRepeaterSettingsExtension::Direction autoRepeaterDirectionA() const;
    void setAutoRepeaterDirectionA(AnytoneAutoRepeaterSettingsExtension::Direction dir);

    /** Returns the monitor slot match. */
    virtual AnytoneDMRSettingsExtension::SlotMatch monitorSlotMatch() const;
    /** Sets the monitor slot match. */
    virtual void setMonitorSlotMatch(AnytoneDMRSettingsExtension::SlotMatch match);
    /** Returns @c true if the monitor matches color code. */
    virtual bool monitorColorCodeMatch() const;
    /** Enables/disables monitor color code match. */
    virtual void enableMonitorColorCodeMatch(bool enable);
    /** Returns @c true if the monitor matches ID. */
    virtual bool monitorIDMatch() const;
    /** Enables/disables monitor ID match. */
    virtual void enableMonitorIDMatch(bool enable);
    /** Returns @c true if the monitor holds the time slot. */
    virtual bool monitorTimeSlotHold() const;
    /** Enables/disables monitor time slot hold. */
    virtual void enableMonitorTimeSlotHold(bool enable);

    AnytoneDisplaySettingsExtension::LastCallerDisplayMode lastCallerDisplayMode() const;
    void setLastCallerDisplayMode(AnytoneDisplaySettingsExtension::LastCallerDisplayMode mode);

    /** Returns the analog call hold in seconds. */
    virtual unsigned fmCallHold() const;
    /** Sets the analog call hold in seconds. */
    virtual void setFMCallHold(unsigned sec);

    bool displayClock() const;
    void enableDisplayClock(bool enable);

    /** Returns @c true if the GPS range reporting is enabled. */
    virtual bool gpsMessageEnabled() const;
    /** Enables/disables GPS range reporting. */
    virtual void enableGPSMessage(bool enable);

    bool enhanceAudio() const;
    void enableEnhancedAudio(bool enable);

    Frequency minVFOScanFrequencyUHF() const;
    void setMinVFOScanFrequencyUHF(Frequency hz);
    Frequency maxVFOScanFrequencyUHF() const;
    void setMaxVFOScanFrequencyUHF(Frequency hz);
    Frequency minVFOScanFrequencyVHF() const;
    void setMinVFOScanFrequencyVHF(Frequency hz);
    Frequency maxVFOScanFrequencyVHF() const;
    void setMaxVFOScanFrequencyVHF(Frequency hz);

    bool hasAutoRepeaterOffsetFrequencyIndexUHF() const;
    unsigned autoRepeaterOffsetFrequencyIndexUHF() const;
    void setAutoRepeaterOffsetFrequenyIndexUHF(unsigned idx);
    void clearAutoRepeaterOffsetFrequencyIndexUHF();
    bool hasAutoRepeaterOffsetFrequencyIndexVHF() const;
    unsigned autoRepeaterOffsetFrequencyIndexVHF() const;
    void setAutoRepeaterOffsetFrequenyIndexVHF(unsigned idx);
    void clearAutoRepeaterOffsetFrequencyIndexVHF();
    Frequency autoRepeaterMinFrequencyVHF() const;
    void setAutoRepeaterMinFrequencyVHF(Frequency Hz);
    Frequency autoRepeaterMaxFrequencyVHF() const;
    void setAutoRepeaterMaxFrequencyVHF(Frequency Hz);
    Frequency autoRepeaterMinFrequencyUHF() const;
    void setAutoRepeaterMinFrequencyUHF(Frequency Hz);
    Frequency autoRepeaterMaxFrequencyUHF() const;
    void setAutoRepeaterMaxFrequencyUHF(Frequency Hz);

    void callToneMelody(Melody &melody) const;
    void setCallToneMelody(const Melody &melody);
    void idleToneMelody(Melody &melody) const;
    void setIdleToneMelody(const Melody &melody);
    void resetToneMelody(Melody &melody) const;
    void setResetToneMelody(const Melody &melody);

    /** Returns the priority Zone A index. */
    virtual unsigned priorityZoneAIndex() const;
    /** Sets the priority zone A index. */
    virtual void setPriorityZoneAIndex(unsigned idx);
    /** Returns the priority Zone B index. */
    virtual unsigned priorityZoneBIndex() const;
    /** Sets the priority zone B index. */
    virtual void setPriorityZoneBIndex(unsigned idx);

    bool displayCall() const;
    void enableDisplayCall(bool enable);

    /** Returns @c true if bluetooth is enabled. */
    virtual bool bluetooth() const;
    /** Enables/disables bluetooth. */
    virtual void enableBluetooth(bool enable);

    /** Returns @c true if the internal mic is additionally active when BT is active. */
    virtual bool btAndInternalMic() const;
    /** Enables/disables the internal mic when BT is active. */
    virtual void enableBTAndInternalMic(bool enable);

    /** Returns @c true if the internal speaker is additionally active when BT is active. */
    virtual bool btAndInternalSpeaker() const;
    /** Enables/disables the internal speaker when BT is active. */
    virtual void enableBTAndInternalSpeaker(bool enable);

    /** Returns @c true if the plug-in record tone is enabled. */
    virtual bool pluginRecTone() const;
    /** Enables/disables the plug-in record tone. */
    virtual void enablePluginRecTone(bool enable);

    /** Returns the GPS ranging interval in seconds. */
    virtual Interval gpsUpdatePeriod() const;
    /** Sets the GPS ranging interval in seconds. */
    virtual void setGPSUpdatePeriod(Interval sec);

    /** Returns the bluetooth microphone gain [1,10]. */
    virtual unsigned int btMicGain() const;
    /** Sets the bluetooth microphone gain [1,10]. */
    virtual void setBTMicGain(unsigned int gain);
    /** Returns the bluetooth speaker gain [1,10]. */
    virtual unsigned int btSpeakerGain() const;
    /** Sets the bluetooth speaker gain [1,10]. */
    virtual void setBTSpeakerGain(unsigned int gain);

    /** Returns @c true if the channel number is displayed. */
    virtual bool displayChannelNumber() const;
    /** Enables/disables display of channel number. */
    virtual void enableDisplayChannelNumber(bool enable);

    bool showCurrentContact() const;
    void enableShowCurrentContact(bool enable);

    /** Returns the auto roaming period in minutes. */
    virtual Interval autoRoamPeriod() const;
    /** Sets the auto roaming period in minutes. */
    virtual void setAutoRoamPeriod(Interval min);

    AnytoneDisplaySettingsExtension::Color callDisplayColor() const;
    void setCallDisplayColor(AnytoneDisplaySettingsExtension::Color color);

    bool gpsUnitsImperial() const;
    void enableGPSUnitsImperial(bool enable);

    bool knobLock() const;
    void enableKnobLock(bool enable);
    bool keypadLock() const;
    void enableKeypadLock(bool enable);
    bool sidekeysLock() const;
    void enableSidekeysLock(bool enable);
    bool keyLockForced() const;
    void enableKeyLockForced(bool enable);

    /** Returns the auto-roam delay in seconds. */
    virtual Interval autoRoamDelay() const;
    /** Sets the auto-roam delay in seconds. */
    virtual void setAutoRoamDelay(Interval sec);

    /** Returns the standby text color. */
    virtual AnytoneDisplaySettingsExtension::Color standbyTextColor() const;
    /** Sets the standby text color. */
    virtual void setStandbyTextColor(AnytoneDisplaySettingsExtension::Color color);
    /** Returns the standby image color. */
    virtual AnytoneDisplaySettingsExtension::Color standbyBackgroundColor() const;
    /** Sets the standby image color. */
    virtual void setStandbyBackgroundColor(AnytoneDisplaySettingsExtension::Color color);

    bool showLastHeard() const;
    void enableShowLastHeard(bool enable);

    /** Returns the SMS format. */
    virtual AnytoneDMRSettingsExtension::SMSFormat smsFormat() const;
    /** Sets the SMS format. */
    virtual void setSMSFormat(AnytoneDMRSettingsExtension::SMSFormat fmt);

    AnytoneAutoRepeaterSettingsExtension::Direction autoRepeaterDirectionB() const;
    void setAutoRepeaterDirectionB(AnytoneAutoRepeaterSettingsExtension::Direction dir);

    /** If enabled, the FM ID is sent together with selected contact. */
    virtual bool fmSendIDAndContact() const;
    /** Enables/disables sending contact with FM ID. */
    virtual void enableFMSendIDAndContact(bool enable);

    bool defaultChannel() const;
    void enableDefaultChannel(bool enable);
    unsigned defaultZoneIndexA() const;
    void setDefaultZoneIndexA(unsigned idx);
    unsigned defaultZoneIndexB() const;
    void setDefaultZoneIndexB(unsigned idx);
    bool defaultChannelAIsVFO() const;
    unsigned defaultChannelAIndex() const;
    void setDefaultChannelAIndex(unsigned idx);
    void setDefaultChannelAToVFO();
    bool defaultChannelBIsVFO() const;
    unsigned defaultChannelBIndex() const;
    void setDefaultChannelBIndex(unsigned idx);
    void setDefaultChannelBToVFO();

    /** Returns the default roaming zone index. */
    virtual unsigned defaultRoamingZoneIndex() const;
    /** Sets the default roaming zone index. */
    virtual void setDefaultRoamingZoneIndex(unsigned idx);

    /** Returns @c true if repeater range check is enabled. */
    virtual bool repeaterRangeCheck() const;
    /** Enables/disables repeater range check. */
    virtual void enableRepeaterRangeCheck(bool enable);
    /** Returns the repeater range check period in seconds. */
    virtual Interval repeaterRangeCheckInterval() const;
    /** Sets the repeater range check interval in seconds. */
    virtual void setRepeaterRangeCheckInterval(Interval sec);
    /** Returns the number of repeater range checks. */
    virtual unsigned repeaterRangeCheckCount() const;
    /** Sets the number of repeater range checks. */
    virtual void setRepeaterRangeCheckCount(unsigned n);
    /** Returns the roaming start condition. */
    virtual AnytoneRoamingSettingsExtension::RoamStart roamingStartCondition() const;
    /** Sets the roaming start condition. */
    virtual void setRoamingStartCondition(AnytoneRoamingSettingsExtension::RoamStart cond);

    /** Returns @c true if the "separate display" is enabled. */
    virtual bool separateDisplay() const;
    /** Enables/disables "separate display. */
    virtual void enableSeparateDisplay(bool enable);

    bool keepLastCaller() const;
    void enableKeepLastCaller(bool enable);

    /** Returns the channel name color. */
    virtual AnytoneDisplaySettingsExtension::Color channelNameColor() const;
    /** Sets the channel name color. */
    virtual void setChannelNameColor(AnytoneDisplaySettingsExtension::Color color);

    /** Returns @c true if repeater check notification is enabled. */
    virtual bool repeaterCheckNotification() const;
    /** Enables/disables repeater check notification. */
    virtual void enableRepeaterCheckNotification(bool enable);

    /** Returns @c true if roaming is enabled. */
    virtual bool roaming() const;
    /** Enables/disables repeater check notification. */
    virtual void enableRoaming(bool enable);

    AnytoneKeySettingsExtension::KeyFunction funcKey1Short() const;
    void setFuncKey1Short(AnytoneKeySettingsExtension::KeyFunction func);
    AnytoneKeySettingsExtension::KeyFunction funcKey2Short() const;
    void setFuncKey2Short(AnytoneKeySettingsExtension::KeyFunction func);
    /** Returns the function for programmable function key 3 short press. */
    virtual AnytoneKeySettingsExtension::KeyFunction funcKey3Short() const;
    /** Sets the function for programmable function key 3 short press. */
    virtual void setFuncKey3Short(AnytoneKeySettingsExtension::KeyFunction func);
    /** Returns the function for programmable function key 4 short press. */
    virtual AnytoneKeySettingsExtension::KeyFunction funcKey4Short() const;
    /** Sets the function for programmable function key 4 short press. */
    virtual void setFuncKey4Short(AnytoneKeySettingsExtension::KeyFunction func);
    /** Returns the function for programmable function key 5 short press. */
    virtual AnytoneKeySettingsExtension::KeyFunction funcKey5Short() const;
    /** Sets the function for programmable function key 5 short press. */
    virtual void setFuncKey5Short(AnytoneKeySettingsExtension::KeyFunction func);
    /** Returns the function for programmable function key 6 short press. */
    virtual AnytoneKeySettingsExtension::KeyFunction funcKey6Short() const;
    /** Sets the function for programmable function key 6 short press. */
    virtual void setFuncKey6Short(AnytoneKeySettingsExtension::KeyFunction func);
    AnytoneKeySettingsExtension::KeyFunction funcKeyAShort() const;
    void setFuncKeyAShort(AnytoneKeySettingsExtension::KeyFunction func);
    AnytoneKeySettingsExtension::KeyFunction funcKeyBShort() const;
    void setFuncKeyBShort(AnytoneKeySettingsExtension::KeyFunction func);
    AnytoneKeySettingsExtension::KeyFunction funcKeyCShort() const;
    void setFuncKeyCShort(AnytoneKeySettingsExtension::KeyFunction func);
    /** Returns the function for programmable function key D short press. */
    virtual AnytoneKeySettingsExtension::KeyFunction funcKeyDShort() const;
    /** Sets the function for programmable function key D short press. */
    virtual void setFuncKeyDShort(AnytoneKeySettingsExtension::KeyFunction func);

    AnytoneKeySettingsExtension::KeyFunction funcKey1Long() const;
    void setFuncKey1Long(AnytoneKeySettingsExtension::KeyFunction func);
    AnytoneKeySettingsExtension::KeyFunction funcKey2Long() const;
    void setFuncKey2Long(AnytoneKeySettingsExtension::KeyFunction func);
    /** Returns the function for programmable function key 3 long press. */
    virtual AnytoneKeySettingsExtension::KeyFunction funcKey3Long() const;
    /** Sets the function for programmable function key 3 long press. */
    virtual void setFuncKey3Long(AnytoneKeySettingsExtension::KeyFunction func);
    /** Returns the function for programmable function key 4 long press. */
    virtual AnytoneKeySettingsExtension::KeyFunction funcKey4Long() const;
    /** Sets the function for programmable function key 4 long press. */
    virtual void setFuncKey4Long(AnytoneKeySettingsExtension::KeyFunction func);
    /** Returns the function for programmable function key 5 long press. */
    virtual AnytoneKeySettingsExtension::KeyFunction funcKey5Long() const;
    /** Sets the function for programmable function key 5 long press. */
    virtual void setFuncKey5Long(AnytoneKeySettingsExtension::KeyFunction func);
    /** Returns the function for programmable function key 6 long press. */
    virtual AnytoneKeySettingsExtension::KeyFunction funcKey6Long() const;
    /** Sets the function for programmable function key 6 long press. */
    virtual void setFuncKey6Long(AnytoneKeySettingsExtension::KeyFunction func);
    AnytoneKeySettingsExtension::KeyFunction funcKeyALong() const;
    void setFuncKeyALong(AnytoneKeySettingsExtension::KeyFunction func);
    AnytoneKeySettingsExtension::KeyFunction funcKeyBLong() const;
    void setFuncKeyBLong(AnytoneKeySettingsExtension::KeyFunction func);
    AnytoneKeySettingsExtension::KeyFunction funcKeyCLong() const;
    void setFuncKeyCLong(AnytoneKeySettingsExtension::KeyFunction func);
    /** Returns the function for programmable function key D long press. */
    virtual AnytoneKeySettingsExtension::KeyFunction funcKeyDLong() const;
    /** Sets the function for programmable function key D long press. */
    virtual void setFuncKeyDLong(AnytoneKeySettingsExtension::KeyFunction func);

    /** Returns the number of repeater check notifications. */
    virtual unsigned repeaterCheckNumNotifications() const;
    /** Sets the number of repeater check notifications. */
    virtual void setRepeaterCheckNumNotifications(unsigned num);

    /** Returns the transmit timeout rekey interval in seconds [0,255], 0=Off. */
    virtual Interval transmitTimeoutRekey() const;
    /** Sets the transmit timeout rekey interval in seconds [0,255], 0=Off. */
    virtual void setTransmitTimeoutRekey(Interval dt);

    /** Returns @c true, if the bluetooth hold time is enabled. */
    virtual bool btHoldTimeEnabled() const;
    /** Returns @c true, if the bluetooth hold time is infinite. */
    virtual bool btHoldTimeInfinite() const;
    /** Returns the bluetooth hold time. */
    virtual Interval btHoldTime() const;
    /** Sets the bluethooth hold time (1-120s). */
    virtual void setBTHoldTime(Interval interval);
    /** Sets the bluethooth hold time to infinite. */
    virtual void setBTHoldTimeInfinite();
    /** Sets the bluethooth hold time to infinite. */
    virtual void disableBTHoldTime();

    /** Returns the bluetooth RX delay in ms. */
    virtual Interval btRXDelay() const;
    /** Sets the bluetooth RX delay in ms. */
    virtual void setBTRXDelay(Interval delay);

    bool fromConfig(const Flags &flags, Context &ctx);
    bool updateConfig(Context &ctx);
    bool linkSettings(RadioSettings *settings, Context &ctx, const ErrorStack &err);

  protected:
    /** Some internal offsets. */
    struct Offset: AnytoneCodeplug::GeneralSettingsElement::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int enableKeyTone()       { return 0x0000; }
      static constexpr unsigned int transmitTimeout()     { return 0x0004; }
      static constexpr unsigned int language()            { return 0x0005; }
      static constexpr unsigned int vfoScanType()         { return 0x000b; }
      static constexpr unsigned int dmrMicGain()          { return 0x000c; }
      static constexpr unsigned int vfoModeA()            { return 0x000d; }
      static constexpr unsigned int vfoModeB()            { return 0x000e; }
      static constexpr unsigned int steType()             { return 0x000f; }

      static constexpr unsigned int steFrequency()        { return 0x0010; }
      static constexpr unsigned int groupCallHangTime()   { return 0x0011; }
      static constexpr unsigned int privateCallHangTime() { return 0x0012; }
      static constexpr unsigned int preWaveDelay()        { return 0x0013; }
      static constexpr unsigned int wakeHeadPeriod()      { return 0x0014; }
      static constexpr unsigned int wfmChannelIndex()     { return 0x0015; }
      static constexpr unsigned int wfmVFOEnabled()       { return 0x0016; }
      static constexpr unsigned int memZoneA()            { return 0x0017; }
      static constexpr unsigned int memZoneB()            { return 0x0018; }
      static constexpr unsigned int wfmEnable()           { return 0x0019; } // new
      static constexpr unsigned int enableRecoding()      { return 0x001a; }
      static constexpr unsigned int displayBrightness()   { return 0x001d; }
      static constexpr unsigned int gpsEnable()           { return 0x001f; }

      static constexpr unsigned int smsAlert()            { return 0x0020; }
      static constexpr unsigned int wfmMonitor()          { return 0x0021;}
      static constexpr unsigned int activeChannelB()      { return 0x0023; }
      static constexpr unsigned int subChannel()          { return 0x0024; }
      static constexpr unsigned int tbstFrequency()       { return 0x0025; }
      static constexpr unsigned int callAlert()           { return 0x0026; }
      static constexpr unsigned int gpsTimeZone()         { return 0x0027; }
      static constexpr unsigned int talkPermit()          { return 0x0028; }
      static constexpr unsigned int dmrResetTone()        { return 0x0029; }
      static constexpr unsigned int idleChannelTone()     { return 0x002a; }
      static constexpr unsigned int menuExitTime()        { return 0x002b; }
      static constexpr unsigned int filterOwnID()         { return 0x002c; }
      static constexpr unsigned int startupTone()         { return 0x002d; }
      static constexpr unsigned int callEndPrompt()       { return 0x002e; }
      static constexpr unsigned int maxSpeakerVolume()    { return 0x002f; }

      static constexpr unsigned int remoteStunKill()      { return 0x0030; }
      static constexpr unsigned int remoteMonitor()       { return 0x0031; }
      static constexpr unsigned int getGPSPosition()      { return 0x0032; }
      static constexpr unsigned int longPressDuration()   { return 0x0033; }
      static constexpr unsigned int volumeChangePrompt()  { return 0x0034; }
      static constexpr unsigned int autoRepeaterDirA()    { return 0x0035; }
      static constexpr unsigned int monSlotMatch()        { return 0x0036; }
      static constexpr unsigned int monColorCodeMatch()   { return 0x0037; }
      static constexpr unsigned int monIDMatch()          { return 0x0038; }
      static constexpr unsigned int monTimeSlotHold()     { return 0x0039; }
      static constexpr unsigned int lastCallerDisplay()   { return 0x003a; }
      static constexpr unsigned int fmCallHold()          { return 0x003c; }
      static constexpr unsigned int showClock()           { return 0x003d; }
      static constexpr unsigned int enableGPSMessage()    { return 0x003e; }
      static constexpr unsigned int enhanceAudio()        { return 0x003f; }

      static constexpr unsigned int minVFOScanUHF()       { return 0x0040; }
      static constexpr unsigned int maxVFOScanUHF()       { return 0x0044; }
      static constexpr unsigned int minVFOScanVHF()       { return 0x0048; }
      static constexpr unsigned int maxVFOScanVHF()       { return 0x004c; }

      static constexpr unsigned int autoRepMinVHF()       { return 0x0050; }
      static constexpr unsigned int autoRepMaxVHF()       { return 0x0054; }
      static constexpr unsigned int autoRepMinUHF()       { return 0x0058; }
      static constexpr unsigned int autoRepMaxUHF()       { return 0x005c; }

      static constexpr unsigned int callToneTones()       { return 0x0060; }
      static constexpr unsigned int callToneDurations()   { return 0x006a; }
      static constexpr unsigned int idleToneTones()       { return 0x0074; }
      static constexpr unsigned int idleToneDurations()   { return 0x007e; }
      static constexpr unsigned int resetToneTones()      { return 0x0088; }
      static constexpr unsigned int resetToneDurations()  { return 0x0092; }

      static constexpr unsigned int autoRepOffsetUHF()    { return 0x009c; }
      static constexpr unsigned int autoRepOffsetVHF()    { return 0x009d; }
      static constexpr unsigned int priorityZoneA()       { return 0x009f; }

      static constexpr unsigned int priorityZoneB()       { return 0x00a0; }
      static constexpr unsigned int callDisplayMode()     { return 0x00a2; }
      static constexpr unsigned int bluetooth()           { return 0x00a4; }
      static constexpr unsigned int btAndInternalMic()    { return 0x00a5; }
      static constexpr unsigned int btAndInternalSpeaker(){ return 0x00a6; }
      static constexpr unsigned int pluginRecTone()       { return 0x00a7; }
      static constexpr unsigned int gpsRangingInterval()  { return 0x00a8; }
      static constexpr unsigned int btMicGain()           { return 0x00a9; }
      static constexpr unsigned int btSpeakerGain()       { return 0x00aa; }
      static constexpr unsigned int showChannelNumber()   { return 0x00ab; }
      static constexpr unsigned int showCurrentContact()  { return 0x00ac; }
      static constexpr unsigned int autoRoamPeriod()      { return 0x00ad; }
      static constexpr unsigned int callColor()           { return 0x00ae; }
      static constexpr unsigned int gpsUnits()            { return 0x00af; }

      static constexpr unsigned int knobLock()            { return 0x00b0; }
      static constexpr unsigned int keypadLock()          { return 0x00b0; }
      static constexpr unsigned int sideKeyLock()         { return 0x00b0; }
      static constexpr unsigned int forceKeyLock()        { return 0x00b0; }
      static constexpr unsigned int autoRoamDelay()       { return 0x00b1; }
      static constexpr unsigned int standbyTextColor()    { return 0x00b2; }
      static constexpr unsigned int standbyBackground()   { return 0x00b3; }
      static constexpr unsigned int showLastHeard()       { return 0x00b4; }
      static constexpr unsigned int smsFormat()           { return 0x00b5; }
      static constexpr unsigned int autoRepeaterDirB()    { return 0x00b6; }
      static constexpr unsigned int fmSendIDAndContact()  { return 0x00b7; }
      static constexpr unsigned int defaultChannels()     { return 0x00b8; }
      static constexpr unsigned int defaultZoneA()        { return 0x00b9; }
      static constexpr unsigned int defaultZoneB()        { return 0x00ba; }
      static constexpr unsigned int defaultChannelA()     { return 0x00bb; }
      static constexpr unsigned int defaultChannelB()     { return 0x00bc; }
      static constexpr unsigned int defaultRoamingZone()  { return 0x00bd; }
      static constexpr unsigned int repRangeCheck()       { return 0x00be; }
      static constexpr unsigned int rangeCheckInterval()  { return 0x00bf; }

      static constexpr unsigned int rangeCheckCount()     { return 0x00c0; }
      static constexpr unsigned int roamStartCondition()  { return 0x00c1; }
      static constexpr unsigned int txBacklightDuration() { return 0x00c2; }
      static constexpr unsigned int displaySeparator()    { return 0x00c3; }
      static constexpr unsigned int keepLastCaller()      { return 0x00c4; }
      static constexpr unsigned int channelNameColor()    { return 0x00c5; }
      static constexpr unsigned int repCheckNotify()      { return 0x00c6; }
      static constexpr unsigned int rxBacklightDuration() { return 0x00c7; }
      static constexpr unsigned int roaming()             { return 0x00c8; }
      static constexpr unsigned int progFuncKey1Short()   { return 0x00c9; }
      static constexpr unsigned int progFuncKey2Short()   { return 0x00ca; }
      static constexpr unsigned int progFuncKey3Short()   { return 0x00cb; }
      static constexpr unsigned int progFuncKey4Short()   { return 0x00cc; }
      static constexpr unsigned int progFuncKey5Short()   { return 0x00cd; }
      static constexpr unsigned int progFuncKey6Short()   { return 0x00ce; }
      static constexpr unsigned int progFuncKeyAShort()   { return 0x00cf; }

      static constexpr unsigned int progFuncKeyBShort()   { return 0x00d0; }
      static constexpr unsigned int progFuncKeyCShort()   { return 0x00d1; }
      static constexpr unsigned int progFuncKeyDShort()   { return 0x00d2; }
      static constexpr unsigned int progFuncKey1Long()    { return 0x00d3; }
      static constexpr unsigned int progFuncKey2Long()    { return 0x00d4; }
      static constexpr unsigned int progFuncKey3Long()    { return 0x00d5; }
      static constexpr unsigned int progFuncKey4Long()    { return 0x00d6; }
      static constexpr unsigned int progFuncKey5Long()    { return 0x00d7; }
      static constexpr unsigned int progFuncKey6Long()    { return 0x00d8; }
      static constexpr unsigned int progFuncKeyALong()    { return 0x00d9; }
      static constexpr unsigned int progFuncKeyBLong()    { return 0x00da; }
      static constexpr unsigned int progFuncKeyCLong()    { return 0x00db; }
      static constexpr unsigned int progFuncKeyDLong()    { return 0x00dc; }
      static constexpr unsigned int repCheckNumNotify()   { return 0x00de; }
      static constexpr unsigned int totRekey()            { return 0x00df; }

      static constexpr unsigned int btHoldTime()          { return 0x00e1; }
      static constexpr unsigned int btRXDelay()           { return 0x00e2; }
      /// @endcond
    };
  };


  /** Represents the hot-key settings of the radio within the D578UV binary codeplug.
   *
   * This class extends the common @c AnytoneCodeplug::HotKeySettings element, encoding 24 instead
   * of 17 @c HotKeySettingsElement.
   *
   * Memory layout of the hot-key settings (size 0x0370 bytes):
   * @verbinclude d578uv_hotkeysettings.txt */
  class HotKeySettingsElement: public AnytoneCodeplug::HotKeySettingsElement
  {
  protected:
    /** Hidden constructor. */
    HotKeySettingsElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    HotKeySettingsElement(uint8_t *ptr);

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x0970; }

    uint8_t *hotKeySetting(unsigned int n) const;

  public:
    /** Some limits for this element. */
    struct Limit {
      static constexpr unsigned int numEntries() { return 24; }   ///< Maximum number of hot-key entries.
    };

  };

  /** Implements the air-band receiver channel.
   *
   * Memory layout of the air-band channel list (size 0x0020 bytes):
   * @verbinclude d578uv_airbandchannel.txt */
  class AirBandChannelElement: public Element
  {
  protected:
    /** Hidden constructor. */
    AirBandChannelElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    AirBandChannelElement(uint8_t *ptr);

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x0020; }

    /** Resets the element. */
    void clear();

    /** The channel frequency. */
    virtual Frequency frequency() const;
    /** Sets the channel frequency. */
    virtual void setFrequency(Frequency freq);

    /** The name of the channel. */
    virtual QString name() const;
    /** Sets the name of the channel. */
    virtual void setName(const QString &name);

  public:
    /** Some limits of the channel. */
    struct Limit {
      static constexpr unsigned int nameLength() { return 16; }     ///< Maximum name length.
    };

  public:
    /** Internal used offsets within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int frequency() { return 0x0000; }
      static constexpr unsigned int name()      { return 0x0004; }
      /// @endcond
    };
  };

  /** Represents the bitmap indicating which channels are valid and which are included in the
   *  air-band scan. */
  class AirBandBitmapElement: public BitmapElement
  {
  protected:
    /** Hidden constructor. */
    AirBandBitmapElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    AirBandBitmapElement(uint8_t *ptr);

    /** The element size. */
    static constexpr unsigned int size() { return 0x0020; }
  };

protected:
  /** Hidden constructor. */
  explicit D578UVCodeplug(const QString &label, QObject *parent = nullptr);

public:
  /** Empty constructor. */
  explicit D578UVCodeplug(QObject *parent = nullptr);

protected:
  bool allocateBitmaps();

  void allocateUpdated();

  void allocateHotKeySettings();

  bool encodeChannels(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createChannels(Context &ctx, const ErrorStack &err=ErrorStack());
  bool linkChannels(Context &ctx, const ErrorStack &err=ErrorStack());

  void allocateContacts();
  bool encodeContacts(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());

  void allocateGeneralSettings();
  bool encodeGeneralSettings(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool decodeGeneralSettings(Context &ctx, const ErrorStack &err=ErrorStack());
  bool linkGeneralSettings(Context &ctx, const ErrorStack &err=ErrorStack());

  /** Allocates the air-band channels und VFO settings. */
  virtual void allocateAirBand();

public:
  /** Some limtis for the codeplug. */
  struct Limit: D878UVCodeplug::Limit {
    /// Maximum number of air-band channels.
    static constexpr unsigned int airBandChannels() { return 100; }
  };

protected:
  /** Internal used offsets within the codeplug. */
  struct Offset: D878UVCodeplug::Offset {
    /// @cond DO_NOT_DOCUMENT
    static constexpr unsigned int contactIdTable()       { return 0x04800000; }
    static constexpr unsigned int settings()             { return 0x02500000; }
    static constexpr unsigned int gpsMessages()          { return 0x02501280; }
    static constexpr unsigned int settingsExtension()    { return 0x02501400; }
    static constexpr unsigned int airBandChannels()      { return 0x02BC0000; }
    static constexpr unsigned int airBandVFO()           { return 0x02BC1000; }
    static constexpr unsigned int airBandChannelBitmap() { return 0x02BC1020; }
    static constexpr unsigned int airBandScanBitmap()    { return 0x02BC1040; }
    /// @endcond
  };
};

#endif // D578UV_CODEPLUG_HH
