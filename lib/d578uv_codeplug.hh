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

protected:
  /** Device specific key function encoding and decoding. */
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
      RoamingSet = 0x2a, APRSSet = 0x2b, ZoneUp = 0x2c, ZoneDown = 0x2d, Exit = 0x2e, Menu = 0x2f,
      XBandRepeater = 0x30, Speaker = 0x31, ChannelName = 0x32, Bluetooth = 0x33,
      GPS = 0x34, CDTScan = 0x35, TBSTSend = 0x36, APRSSend = 0x37, APRSInfo = 0x38,
      GPSRoaming = 0x39, Squelch=0x3a, NoiseReductionTX=0x3b
    } KeyFunctionCode;
  };


public:
  /** Represents the actual channel encoded within the binary code-plug.
   * Matches firmware/CPS version 1.21. */
  class ChannelElement: public D878UVCodeplug::ChannelElement
  {
  public:
    /** Possible FM scrambler carrier frequencies. */
    enum class FMScramblerFrequency {
      Off = 0, Hz3300 = 1, Hz3200 = 2, Hz3100 = 3, Hz3000 = 4, Hz2900 = 5, Hz2800 = 6, Hz2700 = 7,
      Hz2600 = 8, Hz2500 = 9, Hz4095 = 10, Hz3458 = 11, Custom = 12
    };

    /** Possible interrupt priorities. */
    enum class InterruptPriority {
      None = 0, Low = 1, High = 2
    };

  protected:
    /** Hidden constructor. */
    ChannelElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    ChannelElement(uint8_t *ptr);

    /** Returns @c true, if the RX and TX frequencies (and signaling) are swapped. */
    virtual bool frequenciesSwapped() const;
    /** Returns @c true, if the RX and TX frequencies (and signaling) are swapped. */
    virtual void enableSwapFrequencies(bool enable);

    /** Returns @c true if bluetooth hands-free is enabled. */
    virtual bool bluetoothEnabled() const;
    /** Enables/disables hands-free. */
    virtual void enableBluetooth(bool enable);

    bool roamingEnabled() const override;
    void enableRoaming(bool enable) override;

    /** Returns the interrupt priority. */
    virtual InterruptPriority interruptPriority() const;
    /** Sets the interrupt priority. */
    virtual void setInterruptPriority(InterruptPriority pri);

    /** Returns @c true if noise reduction is enabled. */
    virtual bool noiseReductionEnabled() const;
    /** Enables/disables noise reduction. */
    virtual void enableNoiseReduction(bool enable);

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

    unsigned int fmAPRSFrequencyIndex() const override;
    void setFMAPRSFrequencyIndex(unsigned int idx) override;

    bool hasARC4EncryptionKeyIndex() const override;
    unsigned int arc4EncryptionKeyIndex() const override;
    void setARC4EncryptionKeyIndex(unsigned int index) override;
    void clearARC4EncryptionKeyIndex() override;

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


  /** Represents the a channel extension element within the binary codeplug.
   * Matches firmware/CPS version 1.21. */
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
   * This covers the CPS version 1.21. */
  class GeneralSettingsElement: public AnytoneCodeplug::GeneralSettingsElement
  {
  protected:
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

    /** Encoded VFO step sizes. */
    enum class VFOStepSize {
      Hz2500 = 0, Hz5000 = 1, Hz6250 = 2, Hz8330 = 3, kHz10 = 4, Hz12500 = 5, kHz20 = 6,
      kHz25 = 7, kHz30 = 8, kHz50=9
    };

  protected:
    /** Hidden constructor. */
    GeneralSettingsElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    GeneralSettingsElement(uint8_t *ptr);

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x00f0; }

    bool keyToneEnabled() const override;
    void enableKeyTone(bool enable) override;

    /** Returns the transmit timeout in seconds. */
    virtual unsigned transmitTimeout() const;
    /** Sets the transmit timeout in seconds. */
    virtual void setTransmitTimeout(unsigned tot);

    /** Returns the UI language. */
    virtual AnytoneDisplaySettingsExtension::Language language() const;
    /** Sets the UI language. */
    virtual void setLanguage(AnytoneDisplaySettingsExtension::Language lang);

    /** Returns the VFO step size. */
    virtual Frequency vfoStepSize() const;
    /** Sets the VFO step size. */
    virtual void setVFOStepSize(const Frequency &f);

    AnytoneSettingsExtension::VFOScanType vfoScanType() const override;
    void setVFOScanType(AnytoneSettingsExtension::VFOScanType type) override;

    unsigned int dmrMicGain() const override;
    void setDMRMicGain(unsigned int gain) override;

    bool vfoModeA() const override;
    void enableVFOModeA(bool enable) override;
    bool vfoModeB() const override;
    void enableVFOModeB(bool enable) override;

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

    unsigned memoryZoneA() const override;
    void setMemoryZoneA(unsigned zone) override;
    unsigned memoryZoneB() const override;
    void setMemoryZoneB(unsigned zone) override;

    /** Returns @c true, if the WFM/Airband receiver is enabled. */
    virtual bool wfmEnabled() const;
    /** Enables/disables WFM/Airband receiver. */
    virtual void enableWFM(bool enable);

    bool recording() const override;
    void enableRecording(bool enable) override;

    unsigned brightness() const override;
    void setBrightness(unsigned level) override;

    bool gps() const override;
    void enableGPS(bool enable) override;

    bool smsAlert() const override;
    void enableSMSAlert(bool enable) override;

    /** Returns @c true if WFM monitor is enabled. */
    virtual bool wfmMonitor() const;
    /** Enables/disables WFM monitor. */
    virtual void enableWFMMonitor(bool enable);

    bool activeChannelB() const override;
    void enableActiveChannelB(bool enable) override;

    bool subChannel() const override;
    void enableSubChannel(bool enable) override;

    /** Returns the TBST frequency. */
    virtual Frequency tbstFrequency() const;
    /** Sets the TBST frequency. */
    virtual void setTBSTFrequency(Frequency freq);

    bool callAlert() const override;
    void enableCallAlert(bool enable) override;

    QTimeZone gpsTimeZone() const override;
    void setGPSTimeZone(const QTimeZone &zone) override;

    bool dmrTalkPermit() const override;
    void enableDMRTalkPermit(bool enable) override;
    bool fmTalkPermit() const override;
    void enableFMTalkPermit(bool enable) override;
    bool dmrResetTone() const override;
    void enableDMRResetTone(bool enable) override;
    bool idleChannelTone() const override;
    void enableIdleChannelTone(bool enable) override;

    Interval menuExitTime() const override;
    void setMenuExitTime(Interval intv) override;

    /** Returns @c true if the own ID is filtered in call lists. */
    virtual bool filterOwnID() const;
    /** Enables/disables filter of own ID in call lists. */
    virtual void enableFilterOwnID(bool enable);

    bool startupTone() const override;
    void enableStartupTone(bool enable) override;

    bool callEndPrompt() const override;
    void enableCallEndPrompt(bool enable) override;

    unsigned maxSpeakerVolume() const override;
    void setMaxSpeakerVolume(unsigned level) override;

    /** Returns @c true remote stun/kill is enabled. */
    virtual bool remoteStunKill() const;
    /** Enables/disables remote stun/kill. */
    virtual void enableRemoteStunKill(bool enable);

    /** Returns @c true remote monitor is enabled. */
    virtual bool remoteMonitor() const;
    /** Enables/disables remote monitor. */
    virtual void enableRemoteMonitor(bool enable);

    bool getGPSPosition() const override;
    void enableGetGPSPosition(bool enable) override;

    Interval longPressDuration() const override;
    void setLongPressDuration(Interval ms) override;

    bool volumeChangePrompt() const override;
    void enableVolumeChangePrompt(bool enable) override;

    AnytoneAutoRepeaterSettingsExtension::Direction autoRepeaterDirectionA() const override;
    void setAutoRepeaterDirectionA(AnytoneAutoRepeaterSettingsExtension::Direction dir) override;

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

    AnytoneDisplaySettingsExtension::LastCallerDisplayMode lastCallerDisplayMode() const override;
    void setLastCallerDisplayMode(AnytoneDisplaySettingsExtension::LastCallerDisplayMode mode) override;

    /** Returns the analog call hold in seconds. */
    virtual unsigned fmCallHold() const;
    /** Sets the analog call hold in seconds. */
    virtual void setFMCallHold(unsigned sec);

    bool displayClock() const override;
    void enableDisplayClock(bool enable) override;

    /** Returns @c true if the GPS range reporting is enabled. */
    virtual bool gpsMessageEnabled() const;
    /** Enables/disables GPS range reporting. */
    virtual void enableGPSMessage(bool enable);

    bool enhanceAudio() const override;
    void enableEnhancedAudio(bool enable) override;

    Frequency minVFOScanFrequencyUHF() const override;
    void setMinVFOScanFrequencyUHF(Frequency hz) override;
    Frequency maxVFOScanFrequencyUHF() const override;
    void setMaxVFOScanFrequencyUHF(Frequency hz) override;
    Frequency minVFOScanFrequencyVHF() const override;
    void setMinVFOScanFrequencyVHF(Frequency hz) override;
    Frequency maxVFOScanFrequencyVHF() const override;
    void setMaxVFOScanFrequencyVHF(Frequency hz) override;

    bool hasAutoRepeaterOffsetFrequencyIndexUHF() const override;
    unsigned autoRepeaterOffsetFrequencyIndexUHF() const override;
    void setAutoRepeaterOffsetFrequenyIndexUHF(unsigned idx) override;
    void clearAutoRepeaterOffsetFrequencyIndexUHF() override;
    bool hasAutoRepeaterOffsetFrequencyIndexVHF() const override;
    unsigned autoRepeaterOffsetFrequencyIndexVHF() const override;
    void setAutoRepeaterOffsetFrequenyIndexVHF(unsigned idx) override;
    void clearAutoRepeaterOffsetFrequencyIndexVHF() override;
    Frequency autoRepeaterMinFrequencyVHF() const override;
    void setAutoRepeaterMinFrequencyVHF(Frequency Hz) override;
    Frequency autoRepeaterMaxFrequencyVHF() const override;
    void setAutoRepeaterMaxFrequencyVHF(Frequency Hz) override;
    Frequency autoRepeaterMinFrequencyUHF() const override;
    void setAutoRepeaterMinFrequencyUHF(Frequency Hz) override;
    Frequency autoRepeaterMaxFrequencyUHF() const override;
    void setAutoRepeaterMaxFrequencyUHF(Frequency Hz) override;

    void callToneMelody(Melody &melody) const override;
    void setCallToneMelody(const Melody &melody) override;
    void idleToneMelody(Melody &melody) const override;
    void setIdleToneMelody(const Melody &melody) override;
    void resetToneMelody(Melody &melody) const override;
    void setResetToneMelody(const Melody &melody) override;

    /** Returns the priority Zone A index. */
    virtual unsigned priorityZoneAIndex() const;
    /** Sets the priority zone A index. */
    virtual void setPriorityZoneAIndex(unsigned idx);
    /** Returns the priority Zone B index. */
    virtual unsigned priorityZoneBIndex() const;
    /** Sets the priority zone B index. */
    virtual void setPriorityZoneBIndex(unsigned idx);

    bool displayCall() const override;
    void enableDisplayCall(bool enable) override;

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

    bool showCurrentContact() const override;
    void enableShowCurrentContact(bool enable) override;

    /** Returns the auto roaming period in minutes. */
    virtual Interval autoRoamPeriod() const;
    /** Sets the auto roaming period in minutes. */
    virtual void setAutoRoamPeriod(Interval min);

    AnytoneDisplaySettingsExtension::Color callDisplayColor() const override;
    void setCallDisplayColor(AnytoneDisplaySettingsExtension::Color color) override;

    bool gpsUnitsImperial() const override;
    void enableGPSUnitsImperial(bool enable) override;

    bool knobLock() const override;
    void enableKnobLock(bool enable) override;
    bool keypadLock() const override;
    void enableKeypadLock(bool enable) override;
    bool sidekeysLock() const override;
    void enableSidekeysLock(bool enable) override;
    bool keyLockForced() const override;
    void enableKeyLockForced(bool enable) override;

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

    bool showLastHeard() const override;
    void enableShowLastHeard(bool enable) override;

    /** Returns the SMS format. */
    virtual AnytoneDMRSettingsExtension::SMSFormat smsFormat() const;
    /** Sets the SMS format. */
    virtual void setSMSFormat(AnytoneDMRSettingsExtension::SMSFormat fmt);

    AnytoneAutoRepeaterSettingsExtension::Direction autoRepeaterDirectionB() const override;
    void setAutoRepeaterDirectionB(AnytoneAutoRepeaterSettingsExtension::Direction dir) override;

    /** If enabled, the FM ID is sent together with selected contact. */
    virtual bool fmSendIDAndContact() const;
    /** Enables/disables sending contact with FM ID. */
    virtual void enableFMSendIDAndContact(bool enable);

    bool defaultChannel() const override;
    void enableDefaultChannel(bool enable) override;
    unsigned defaultZoneIndexA() const override;
    void setDefaultZoneIndexA(unsigned idx) override;
    unsigned defaultZoneIndexB() const override;
    void setDefaultZoneIndexB(unsigned idx) override;
    bool defaultChannelAIsVFO() const override;
    unsigned defaultChannelAIndex() const override;
    void setDefaultChannelAIndex(unsigned idx) override;
    void setDefaultChannelAToVFO() override;
    bool defaultChannelBIsVFO() const override;
    unsigned defaultChannelBIndex() const override;
    void setDefaultChannelBIndex(unsigned idx) override;
    void setDefaultChannelBToVFO() override;

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

    bool keepLastCaller() const override;
    void enableKeepLastCaller(bool enable) override;

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

    AnytoneKeySettingsExtension::KeyFunction funcKey1Short() const override;
    void setFuncKey1Short(AnytoneKeySettingsExtension::KeyFunction func) override;
    AnytoneKeySettingsExtension::KeyFunction funcKey2Short() const override;
    void setFuncKey2Short(AnytoneKeySettingsExtension::KeyFunction func) override;
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
    AnytoneKeySettingsExtension::KeyFunction funcKeyAShort() const override;
    void setFuncKeyAShort(AnytoneKeySettingsExtension::KeyFunction func) override;
    AnytoneKeySettingsExtension::KeyFunction funcKeyBShort() const override;
    void setFuncKeyBShort(AnytoneKeySettingsExtension::KeyFunction func) override;
    AnytoneKeySettingsExtension::KeyFunction funcKeyCShort() const override;
    void setFuncKeyCShort(AnytoneKeySettingsExtension::KeyFunction func) override;
    /** Returns the function for programmable function key D short press. */
    virtual AnytoneKeySettingsExtension::KeyFunction funcKeyDShort() const;
    /** Sets the function for programmable function key D short press. */
    virtual void setFuncKeyDShort(AnytoneKeySettingsExtension::KeyFunction func);

    AnytoneKeySettingsExtension::KeyFunction funcKey1Long() const override;
    void setFuncKey1Long(AnytoneKeySettingsExtension::KeyFunction func) override;
    AnytoneKeySettingsExtension::KeyFunction funcKey2Long() const override;
    void setFuncKey2Long(AnytoneKeySettingsExtension::KeyFunction func) override;
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
    AnytoneKeySettingsExtension::KeyFunction funcKeyALong() const override;
    void setFuncKeyALong(AnytoneKeySettingsExtension::KeyFunction func) override;
    AnytoneKeySettingsExtension::KeyFunction funcKeyBLong() const override;
    void setFuncKeyBLong(AnytoneKeySettingsExtension::KeyFunction func) override;
    AnytoneKeySettingsExtension::KeyFunction funcKeyCLong() const override;
    void setFuncKeyCLong(AnytoneKeySettingsExtension::KeyFunction func) override;
    /** Returns the function for programmable function key D long press. */
    virtual AnytoneKeySettingsExtension::KeyFunction funcKeyDLong() const;
    /** Sets the function for programmable function key D long press. */
    virtual void setFuncKeyDLong(AnytoneKeySettingsExtension::KeyFunction func);

    /** Returns the number of repeater check notifications. */
    virtual unsigned repeaterCheckNumNotifications() const;
    /** Sets the number of repeater check notifications. */
    virtual void setRepeaterCheckNumNotifications(unsigned num);

    /** Returns @c true, if the bluetooth hold time is enabled. */
    virtual bool btHoldTimeEnabled() const;
    /** Returns @c true, if the bluetooth hold time is infinite. */
    virtual bool btHoldTimeInfinite() const;
    /** Returns the bluetooth hold time. */
    virtual Interval btHoldTime() const;
    /** Sets the Bluetooth hold time (1-120s). */
    virtual void setBTHoldTime(Interval interval);
    /** Sets the Bluetooth hold time to infinite. */
    virtual void setBTHoldTimeInfinite();
    /** Sets the Bluetooth hold time to infinite. */
    virtual void disableBTHoldTime();

    /** Returns the bluetooth RX delay in ms. */
    virtual Interval btRXDelay() const;
    /** Sets the bluetooth RX delay in ms. */
    virtual void setBTRXDelay(Interval delay);

    bool fromConfig(const Flags &flags, Context &ctx) override;
    bool updateConfig(Context &ctx) override;
    bool linkSettings(RadioSettings *settings, Context &ctx, const ErrorStack &err) override;

  protected:
    /** Some internal offsets. */
    struct Offset: AnytoneCodeplug::GeneralSettingsElement::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int enableKeyTone()       { return 0x0000; }
      static constexpr unsigned int transmitTimeout()     { return 0x0004; }
      static constexpr unsigned int language()            { return 0x0005; }
      static constexpr unsigned int vfoStepSize()         { return 0x0008; }
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
      static constexpr unsigned int wfmEnable()           { return 0x0019; }
      static constexpr unsigned int enableRecoding()      { return 0x001a; }
      static constexpr unsigned int displayBrightness()   { return 0x001d; }
      static constexpr unsigned int gpsEnable()           { return 0x001f; }

      static constexpr unsigned int smsAlert()            { return 0x0020; }
      static constexpr unsigned int wfmMonitor()          { return 0x0022; }
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

      static constexpr Bit knobLock()                     { return {0x00b0, 0}; }
      static constexpr Bit keypadLock()                   { return {0x00b0, 1}; }
      static constexpr Bit sideKeyLock()                  { return {0x00b0, 3}; }
      static constexpr Bit forceKeyLock()                 { return {0x00b0, 4}; }
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
      static constexpr unsigned int displaySeparator()    { return 0x00c3; }
      static constexpr unsigned int keepLastCaller()      { return 0x00c4; }
      static constexpr unsigned int channelNameColor()    { return 0x00c5; }
      static constexpr unsigned int repCheckNotify()      { return 0x00c6; }
      static constexpr unsigned int txBacklightDuration() { return 0x00c7; }
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

      static constexpr unsigned int btHoldTime()          { return 0x00e1; }
      static constexpr unsigned int btRXDelay()           { return 0x00e2; }
      /// @endcond
    };
  };


  /** General settings extension element for the D578UV. */
  class ExtendedSettingsElement: public AnytoneCodeplug::ExtendedSettingsElement
  {
  protected:
    /* Encoding of possible speakers. */
    enum class Speaker {
      Microphone=0, Radio=1, Both=2
    };

    /** Encoding of microphone-speaker source. */
    enum class SpeakerSource {
      MainChannel = 0, SubChannel = 1
    };

    /** Encoding of possible GPS modes. */
    enum class GPSMode {
      GPS = 0, Beidou=1, GPS_Beidou=2
    };

    /** Encoding of possible fan-control settings. */
    enum class FanControl {
      PTT=0, Temperature=1, Both=2
    };

    /** Possible mic types. */
    enum class MicType {
      AnyTone = 0, Generic = 1
    };

    /** Encoding of up/down key functions. */
    enum class UpDownKeyFunction {
      Channel = 0, Volume = 1
    };

    /** Encoding of repeater color code match. */
    enum class RepeaterColorCodeMatch {
      None = 0, VFO_A = 1, VFO_B = 2
    };

    /** Encoding of repeater timeslot match. */
    enum class RepeaterTimeSlotMatch {
      Any = 0, RX1_TX2 = 1, RX2_TX1 = 2
    };

  protected:
    /** Hidden Constructor. */
    ExtendedSettingsElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit ExtendedSettingsElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x00000200; }

    /** Resets the settings. */
    void clear();

    /** Returns the talker alias source. */
    virtual AnytoneDMRSettingsExtension::TalkerAliasSource talkerAliasSource() const;
    /** Sets the talker alias source. */
    virtual void setTalkerAliasSource(AnytoneDMRSettingsExtension::TalkerAliasSource mode);

    /** Returns the talker alias encoding. */
    virtual AnytoneDMRSettingsExtension::TalkerAliasEncoding talkerAliasEncoding() const;
    /** Sets the talker alias encoding. */
    virtual void setTalkerAliasEncoding(AnytoneDMRSettingsExtension::TalkerAliasEncoding encoding);

    /** Returns @c true if the weather alarm is enabled. */
    virtual bool weatherAlarmEnabled() const;
    /** Enables/disables the weather alarm. */
    virtual void enableWeatherAlarm(bool enable);

    /** Returns @c true if the repeater function is enabled. */
    virtual bool repeaterEnabled() const;
    /** Enables/disables the repeater function. */
    virtual void enableRepeater(bool enable);

    /** Returns the speaker setting. */
    virtual AnytoneAudioSettingsExtension::Speaker speaker() const;
    /** Sets the speaker setting. */
    virtual void setSpeaker(AnytoneAudioSettingsExtension::Speaker speaker);

    /** Returns the microphone-speaker source. */
    virtual AnytoneAudioSettingsExtension::HandsetSpeakerSource micSpeakerSource() const;
    /** Sets the microphone-speaker source. */
    virtual void setMicSpeakerSource(AnytoneAudioSettingsExtension::HandsetSpeakerSource source);

    /** Returns the GPS mode. */
    virtual AnytoneGPSSettingsExtension::GPSMode gpsMode() const;
    /** Sets the GPS mode. */
    virtual void setGPSMode(AnytoneGPSSettingsExtension::GPSMode mode);

    /** Returns @c true if the BT PTT latch is enabled. */
    virtual bool bluetoothPTTLatch() const;
    /** Enables/disables bluetooth PTT latch. */
    virtual void enableBluetoothPTTLatch(bool enable);

    /** Returns @c true if the bluetooth PTT sleep delay is disabled (infinite). */
    virtual bool infiniteBluetoothPTTSleepDelay() const;
    /** Returns the bluetooth PTT sleep delay in minutes, 0=off. */
    virtual Interval bluetoothPTTSleepDelay() const;
    /** Sets the bluetooth PTT sleep delay in minutes. */
    virtual void setBluetoothPTTSleepDelay(Interval delay);
    /** Sets the bluetooth PTT sleep delay to infinite/disabled. */
    virtual void setInfiniteBluetoothPTTSleepDelay();

    /** Returns the fan-control setting. */
    virtual AnytoneSettingsExtension::FanControl fanControl() const;
    /** Sets the fan-control setting. */
    virtual void setFanControl(AnytoneSettingsExtension::FanControl ctrl);

    /** Returns the weather channel index. */
    virtual unsigned int weatherChannelIndex() const;
    /** Sets the weather channel index. */
    virtual void setWeatherChannelIndex(unsigned int idx);

    /** Returns @c true if the manual dialed group call hang time is infinite. */
    virtual bool infiniteManDialGroupCallHangTime() const;
    /** Returns the manual dial group call hang time. */
    virtual Interval manDialGroupCallHangTime() const;
    /** Sets the manual dial group call hang time. */
    virtual void setManDialGroupCallHangTime(Interval dur);
    /** Sets the manual dial group call hang time to infinite. */
    virtual void setManDialGroupCallHangTimeInfinite();

    /** Returns @c true if the manual dialed private call hang time is infinite. */
    virtual bool infiniteManDialPrivateCallHangTime() const;
    /** Returns the manual dial private call hang time. */
    virtual Interval manDialPrivateCallHangTime() const;
    /** Sets the manual dial private call hang time. */
    virtual void setManDialPrivateCallHangTime(Interval dur);
    /** Sets the manual dial private call hang time to infinite. */
    virtual void setManDialPrivateCallHangTimeInfinite();

    /** Returns the short-press function for the channel knob. */
    virtual AnytoneKeySettingsExtension::KeyFunction chKnobShortPressFunction() const;
    /** Sets the channel knob short-press function. */
    virtual void setChKnobShortPressFunction(AnytoneKeySettingsExtension::KeyFunction func);
    /** Returns the long-press function for the channel knob. */
    virtual AnytoneKeySettingsExtension::KeyFunction chKnobLongPressFunction() const;
    /** Sets the channel knob long-press function. */
    virtual void setChKnobLongPressFunction(AnytoneKeySettingsExtension::KeyFunction func);

    AnytoneDisplaySettingsExtension::Color channelBNameColor() const;
    void setChannelBNameColor(AnytoneDisplaySettingsExtension::Color color);

    /** Returns the encryption mode. */
    virtual AnytoneDMRSettingsExtension::EncryptionType encryption() const;
    /** Sets the encryption mode. */
    virtual void setEncryption(AnytoneDMRSettingsExtension::EncryptionType mode);

    /** Returns @c true if the professional mode is enabled. */
    virtual bool professionalMode() const;
    /** Enables/disables professional mode. */
    virtual void enableProfessionalMode(bool enable);

    /** Returns the STE (squelch tail elimination) duration. */
    virtual Interval steDuration() const;
    /** Sets the STE (squelch tail elimination) duration. */
    virtual void setSTEDuration(Interval dur);

    /** Returns the microphone type. */
    virtual AnytoneAudioSettingsExtension::HandsetType micType() const;
    /** Sets the microphone type. */
    virtual void setMicType(AnytoneAudioSettingsExtension::HandsetType type);

    AnytoneDisplaySettingsExtension::Color zoneANameColor() const;
    void setZoneANameColor(AnytoneDisplaySettingsExtension::Color color);
    AnytoneDisplaySettingsExtension::Color zoneBNameColor() const;
    void setZoneBNameColor(AnytoneDisplaySettingsExtension::Color color);

    /** Returns @c true if the auto-shutdown timer is reset on a call. */
    virtual bool resetAutoShutdownOnCall() const;
    /** Enables/disables reset on call of the auto-shutdown timer. */
    virtual void enableResetAutoShutdownOnCall(bool enable);

    /** Returns @c true if the color code is shown. */
    virtual bool showColorCode() const;
    /** Enables/disables display of color code. */
    virtual void enableShowColorCode(bool enable);
    /** Returns @c true if the time slot is shown. */
    virtual bool showTimeSlot() const;
    /** Enables/disables display of time slot. */
    virtual void enableShowTimeSlot(bool enable);
    /** Returns @c true if the channel type is shown. */
    virtual bool showChannelType() const;
    /** Enables/disables display of channel type. */
    virtual void enableShowChannelType(bool enable);

    /** Returns @c true if the FM idle channel tone is enabled. */
    virtual bool fmIdleTone() const;
    /** Enables/disables FM idle channel tone. */
    virtual void enableFMIdleTone(bool enable);

    /** Returns the date format. */
    virtual AnytoneDisplaySettingsExtension::DateFormat dateFormat() const;
    /** Sets the date format. */
    virtual void setDateFormat(AnytoneDisplaySettingsExtension::DateFormat format);

    /** Returns the FM Mic gain [1,10]. */
    virtual unsigned int fmMicGain() const;
    /** Sets the analog mic gain [1,10]. */
    virtual void setFMMicGain(unsigned int gain);

    /** Returns the short-press function for SK1 of the BT handset. */
    virtual AnytoneKeySettingsExtension::KeyFunction btSK1ShortPressFunction() const;
    /** Sets the SK1 short-press function of the BT handset. */
    virtual void setBtSK1ShortPressFunction(AnytoneKeySettingsExtension::KeyFunction func);
    /** Returns the short-press function for SK2 of the BT handset. */
    virtual AnytoneKeySettingsExtension::KeyFunction btSK2ShortPressFunction() const;
    /** Sets the SK2 short-press function of the BT handset. */
    virtual void setBtSK2ShortPressFunction(AnytoneKeySettingsExtension::KeyFunction func);
    /** Returns the short-press function for SK3 of the BT handset. */
    virtual AnytoneKeySettingsExtension::KeyFunction btSK3ShortPressFunction() const;
    /** Sets the SK3 short-press function of the BT handset. */
    virtual void setBtSK3ShortPressFunction(AnytoneKeySettingsExtension::KeyFunction func);

    /** Returns the lone-press function for SK1 of the BT handset. */
    virtual AnytoneKeySettingsExtension::KeyFunction btSK1LongPressFunction() const;
    /** Sets the SK1 long-press function of the BT handset. */
    virtual void setBtSK1LongPressFunction(AnytoneKeySettingsExtension::KeyFunction func);
    /** Returns the long-press function for SK2 of the BT handset. */
    virtual AnytoneKeySettingsExtension::KeyFunction btSK2LongPressFunction() const;
    /** Sets the SK2 long-press function of the BT handset. */
    virtual void setBtSK2LongPressFunction(AnytoneKeySettingsExtension::KeyFunction func);
    /** Returns the long-press function for SK3 of the BT handset. */
    virtual AnytoneKeySettingsExtension::KeyFunction btSK3LongPressFunction() const;
    /** Sets the SK3 long-press function of the BT handset. */
    virtual void setBtSK3LongPressFunction(AnytoneKeySettingsExtension::KeyFunction func);

    /** Returns the BT handset mic gain [1-10]. */
    virtual unsigned int btHandsetMicGain() const;
    /** Sets the BT handset mic gain. */
    virtual void setBtHandsetMicGain(unsigned int gain);

    /** Returns the bluetooth handset backlight duration. */
    virtual Interval btHandsetBacklightDuration() const;
    /** Sets the bluetooth handset backlight duration. */
    virtual void setBtHandsetBacklightDuration(Interval delay);

    /** Returns the function of up/down keys on microphone. */
    virtual AnytoneKeySettingsExtension::UpDownKeyFunction micUpDownKeyFunction() const;
    /** Sets the microphone up/down key function. */
    virtual void setMicUpDownKeyFunction(AnytoneKeySettingsExtension::UpDownKeyFunction func);

    /** Returns @c true if the transmit timeout notification is enabled. */
    virtual bool totNotification() const;
    /** Enables/disables transmit timeout notification. */
    virtual void enableTOTNotification(bool enable);

    /** Returns @c true if the GPS roaming is enabled. */
    virtual bool gpsRoaming() const;
    /** Enables/disables GPS roaming. */
    virtual void enableGPSRoaming(bool enable);

    /** Returns the repeater colorcode match mode. */
    virtual AnytoneRepeaterSettingsExtension::ColorCode repColorCodeMatch() const;
    /** Sets the repeater colorcode match mode. */
    virtual void setRepColorCodeMatch(AnytoneRepeaterSettingsExtension::ColorCode mode);

    /** Returns the repeater timeslots for VFO A. */
    virtual AnytoneRepeaterSettingsExtension::TimeSlot repTimeSlotAMatch() const;
    /** Sets the repeater timeslots for VFO A. */
    virtual void setRepTimeSlotAMatch(AnytoneRepeaterSettingsExtension::TimeSlot mode);
    /** Returns the repeater timeslots for VFO B. */
    virtual AnytoneRepeaterSettingsExtension::TimeSlot repTimeSlotBMatch() const;
    /** Sets the repeater timeslots for VFO B. */
    virtual void setRepTimeSlotBMatch(AnytoneRepeaterSettingsExtension::TimeSlot mode);

    /** Returns the BT handset squelch level [0, 1-10]. */
    virtual unsigned int btHandsetSquelch() const;
    /** Sets the BT handset squelch level [0, 1-10]. */
    virtual void setBtHandsetSquelch(unsigned int level);

    /** If @c true, the BT handset to shut off automatically, if the device powers down. */
    virtual bool btHandsetAutoPowerOffEnabled() const;
    /** Enables/disables the BT handset to shut off automatically, if the device powers down. */
    virtual void enableBtHandsetAutoPowerOff(bool enable);

    /** Returns the very-long-press function for SK1 of the BT handset. */
    virtual AnytoneKeySettingsExtension::KeyFunction btSK1VeryLongPressFunction() const;
    /** Sets the SK1 very-long-press function of the BT handset. */
    virtual void setBtSK1VeryLongPressFunction(AnytoneKeySettingsExtension::KeyFunction func);
    /** Returns the very-long-press function for SK2 of the BT handset. */
    virtual AnytoneKeySettingsExtension::KeyFunction btSK2VeryLongPressFunction() const;
    /** Sets the SK2 very-long-press function of the BT handset. */
    virtual void setBtSK2VeryLongPressFunction(AnytoneKeySettingsExtension::KeyFunction func);
    /** Returns the very-long-press function for SK3 of the BT handset. */
    virtual AnytoneKeySettingsExtension::KeyFunction btSK3VeryLongPressFunction() const;
    /** Sets the SK3 very-long-press function of the BT handset. */
    virtual void setBtSK3VeryLongPressFunction(AnytoneKeySettingsExtension::KeyFunction func);

    /** Returns the BT handset TX noise reduction level [0,1-10]. */
    virtual unsigned int btHandsetTxNoiseRedLevel() const;
    /** Sets the BT handset TX noise reduction level [0,1-10]. */
    virtual void setBtHandsetTxNoiseRedLevel(unsigned int level);

    /** Returns the BT handset VOX level [0,1-10]. */
    virtual unsigned int btHandsetVOXLevel() const;
    /** Sets the BT handset VOX level [0,1-10]. */
    virtual void setBtHandsetVOXLevel(unsigned int level);

    /** Returns the VOX delay for the BT handset. */
    virtual Interval btHandsetVOXDelay() const;
    /** Sets the VOX delay for the BT handset. */
    virtual void setBtHandsetVOXDelay(Interval delay);

    /** Returns the BT handset volume for VFO A [0,1,10]. */
    virtual unsigned int btHandsetVolumeA() const;
    /** Sets the BF handset volume for VFO A [0,1,10]. */
    virtual void setBtHandsetVolumeA(unsigned int vol);
    /** Returns the BT handset volume for VFO B [0,1,10]. */
    virtual unsigned int btHandsetVolumeB() const;
    /** Sets the BF handset volume for VFO B [0,1,10]. */
    virtual void setBtHandsetVolumeB(unsigned int vol);

    /** Returns the call-end tone melody. */
    virtual void callEndToneMelody(Melody &melody) const;
    /** Sets the call-end tone melody. */
    virtual void setCallEndToneMelody(const Melody &melody);
    /** Returns the all-call tone melody. */
    virtual void allCallToneMelody(Melody &melody) const;
    /** Sets the all-call tone melody. */
    virtual void setAllCallToneMelody(const Melody &melody);


    /** Encodes the settings from the config. */
    virtual bool fromConfig(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
    /** Update config from settings. */
    virtual bool updateConfig(Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits for the settings. */
    struct Limit {
      static constexpr unsigned int maxBluetoothPTTSleepDelay() { return 4; }    ///< Maximum delay in minutes.
      static constexpr unsigned int maxWeatherChannelIndex()    { return 9; }    ///< Maximum weather channel index.
    };

  protected:
    /** Internal used offset within the element. */
    struct Offset : public AnytoneCodeplug::ExtendedSettingsElement::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int talkerAliasDisplay()           { return 0x001e; }
      static constexpr unsigned int talkerAliasEncoding()          { return 0x001f; }

      static constexpr unsigned int weatherAlarm()                 { return 0x0020; }
      static constexpr unsigned int repeater()                     { return 0x0021; }
      static constexpr unsigned int speakers()                     { return 0x0023; }
      static constexpr unsigned int micSpeakerSource()             { return 0x0025; }
      static constexpr unsigned int gpsMode()                      { return 0x0026; }
      static constexpr unsigned int btPTTLatch()                   { return 0x0027; }
      static constexpr unsigned int btPTTSleepDelay()              { return 0x0028; }
      static constexpr unsigned int fanControl()                   { return 0x0029; }
      static constexpr unsigned int weatherChannelIndex()          { return 0x002a; }
      static constexpr unsigned int manGrpCallHangTime()           { return 0x002b; }
      static constexpr unsigned int manPrivCallHangTime()          { return 0x002c; }
      static constexpr unsigned int chKnobShortPressFunction()     { return 0x002d; }
      static constexpr unsigned int chKnobLongPressFunction()      { return 0x002e; }
      static constexpr unsigned int channelBNameColor()            { return 0x002f; }

      static constexpr unsigned int encryptionType()               { return 0x0030; }
      static constexpr unsigned int uiMode()                       { return 0x0031; }
      static constexpr unsigned int steDuration()                  { return 0x0032; }
      static constexpr unsigned int micType()                      { return 0x0033; }
      static constexpr unsigned int zoneANameColor()               { return 0x0034; }
      static constexpr unsigned int zoneBNameColor()               { return 0x0035; }
      static constexpr unsigned int autoShutdownMode()             { return 0x0036; }
      static constexpr Bit displayColorCode()                      { return {0x003b, 2}; }
      static constexpr Bit displayTimeSlot()                       { return {0x003b, 1}; }
      static constexpr Bit displayChannelType()                    { return {0x003b, 0}; }
      static constexpr unsigned int fmIdleTone()                   { return 0x003c; }
      static constexpr unsigned int dateFormat()                   { return 0x003d; }
      static constexpr unsigned int analogMicGain()                { return 0x003e; }
      static constexpr unsigned int btSK1ShortPressFunction()      { return 0x003f; }

      static constexpr unsigned int btSK2ShortPressFunction()      { return 0x0040; }
      static constexpr unsigned int btSK3ShortPressFunction()      { return 0x0041; }
      static constexpr unsigned int btSK1LongPressFunction()       { return 0x0042; }
      static constexpr unsigned int btSK2LongPressFunction()       { return 0x0043; }
      static constexpr unsigned int btSK3LongPressFunction()       { return 0x0044; }
      static constexpr unsigned int btHSMicGain()                  { return 0x0045; }
      static constexpr unsigned int btHSBacklightDuration()        { return 0x0047; }
      static constexpr unsigned int upDownKeyFunction()            { return 0x0048; }
      static constexpr unsigned int totNotification()              { return 0x0049; }
      static constexpr unsigned int gpsRoaming()                   { return 0x004a; }
      static constexpr unsigned int repeaterColorCode()            { return 0x004b; }
      static constexpr unsigned int repeaterATimeslot()            { return 0x004c; }
      static constexpr unsigned int repeaterBTimeslot()            { return 0x004d; }
      static constexpr unsigned int btHSRxNoiseReduction()         { return 0x004e; }
      static constexpr unsigned int btHSShutDown()                 { return 0x004f; }

      static constexpr unsigned int btSK1VeryLongPressFunction()   { return 0x0050; }
      static constexpr unsigned int btSK2VeryLongPressFunction()   { return 0x0051; }
      static constexpr unsigned int btSK3VeryLongPressFunction()   { return 0x0052; }
      static constexpr unsigned int btHSTxNoiseReduction()         { return 0x0053; }
      static constexpr unsigned int btHSVOXLevel()                 { return 0x0054; }
      static constexpr unsigned int btHSVOXDelay()                 { return 0x0055; }
      static constexpr unsigned int btHSVolumeA()                  { return 0x0056; }
      static constexpr unsigned int btHSVolumeB()                  { return 0x0057; }

      static constexpr unsigned int callEndTones()                 { return 0x0058; }
      static constexpr unsigned int callEndDurations()             { return 0x0062; }
      static constexpr unsigned int allCallTones()                 { return 0x006c; }
      static constexpr unsigned int allCallDurations()             { return 0x0076; }
      static constexpr unsigned int headerRep()                    { return 0x0080; }
      /// @endcond
    };
  };



  /** Represents the hot-key settings of the radio within the D578UV binary codeplug.
   *
   * This class extends the common @c AnytoneCodeplug::HotKeySettings element, encoding 24 instead
   * of 17 @c HotKeySettingsElement. */
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
  /** Some limits for the codeplug. */
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
