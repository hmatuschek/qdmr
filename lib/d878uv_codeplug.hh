#ifndef D878UV_CODEPLUG_HH
#define D878UV_CODEPLUG_HH

#include <QDateTime>

#include "d868uv_codeplug.hh"
#include "signaling.hh"
#include "gpssystem.hh"

class Channel;
class DMRContact;
class Zone;
class RXGroupList;
class ScanList;
class GPSSystem;
class RoamingChannel;


/** Represents the device specific binary codeplug for Anytone AT-D878UV radios.
 *
 * In contrast to many other code-plugs, the code-plug for Anytone radios are spread over a large
 * memory area. In principle, this is a good idea, as it allows one to upload only the portion of the
 * codeplug that is actually configured. For example, if only a small portion of the available
 * contacts and channels are used, the amount of data that is written to the device can be
 * reduced.
 *
 * However, the implementation of this idea in this device is utter shit. The amount of
 * fragmentation of the codeplug is overwhelming. For example, while channels are organized more or
 * less nicely in continuous banks, zones are distributed throughout the entire code-plug. That is,
 * the names of zones are located in a different memory section that the channel lists. Some lists
 * are defined though bit-masks others by byte-masks. All bit-masks are positive, that is 1
 * indicates an enabled item while the bit-mask for contacts is inverted.
 *
 * In general the code-plug is huge and complex. Moreover, the radio provides a huge amount of
 * options and features. To this end, reverse-engineering this code-plug was a nightmare.
 *
 * More over, the binary code-plug file generate by the windows CPS does not directly relate to
 * the data being written to the radio. To this end the code-plug has been reverse-engineered
 * using wireshark to monitor the USB communication between the windows CPS (running in a virtual
 * box) and the device. The latter makes the reverse-engineering particularly cumbersome.
 *
 * For more details, see https://dmr-tools.github.io/codeplugs.
 *
 * @ingroup d878uv */
class D878UVCodeplug : public D868UVCodeplug
{
  Q_OBJECT

protected:
  /** Channel name and call-sign colors supported by the D878UV. */
  struct NameColor {
  public:
    /** Maps code -> color. */
    static AnytoneDisplaySettingsExtension::Color decode(uint8_t code);
    /** Maps color -> code. */
    static uint8_t encode(AnytoneDisplaySettingsExtension::Color color);

  protected:
    /** Encoding of the supported colors. */
    typedef enum {
      Orange=0, Red=1, Yellow=2, Green=3, Turquoise=4, Blue=5, White = 6
    } CodedColor;
  };

  /** Text colors supported by the D878UV. */
  struct TextColor {
  public:
    /** Maps code -> color. */
    static AnytoneDisplaySettingsExtension::Color decode(uint8_t code);
    /** Maps color -> code. */
    static uint8_t encode(AnytoneDisplaySettingsExtension::Color color);

  protected:
    /** Encoding of the supported colors. */
    typedef enum {
      White=0, Black=1, Orange=2, Red=3, Yellow=4, Green=5, Turquoise=6, Blue=7
    } CodedColor;
  };

public:
  /** Represents the actual channel encoded within the binary D878UV codeplug.
   *
   * This class implements only the differences to the generic @c AnytoneCodeplug::ChannelElement
   * (i.e. D868UVII).
   *
   * Memory layout of encoded channel (size 0x40 bytes):
   * @verbinclude d878uv_channel.txt
   */
  class ChannelElement: public D868UVCodeplug::ChannelElement
  {
  public:
    /** Possible PTT ID settings. */
    enum class PTTId {
      Off = 0,                    ///< Never send PTT-ID.
      Start = 1,                  ///< Send PTT-ID at start.
      End = 2,                    ///< Send PTT-ID at end.
      Both = 3                    ///< Send PTT-ID at start and end.
    };

    /** Possible APRS modes. */
    enum class APRSType {
      Off = 0, FM = 1, DMR = 2
    };

    /** Defines all possible APRS PTT settings. */
    enum class APRSPTT {
      Off   = 0,                  ///< Do not send APRS on PTT.
      Start = 1,                  ///< Send APRS at start of transmission.
      End   = 2                   ///< Send APRS at end of transmission.
    };

    /** Possible encryption types. */
    enum class AdvancedEncryptionType {
      AES, ARC4
    };

  protected:
    /** Hidden constructor. */
    ChannelElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit ChannelElement(uint8_t *ptr);

    /** Resets the channel. */
    void clear()  override;

    /** Returns the PTT ID settings. */
    virtual PTTId pttIDSetting() const;
    /** Sets the PTT ID setting. */
    virtual void setPTTIDSetting(PTTId ptt);

    /** Returns @c true if roaming is enabled. */
    virtual bool roamingEnabled() const;
    /** Enables/disables roaming. */
    virtual void enableRoaming(bool enable);
    // Moved
    /** Returns @c true if the data ACK is enabled. */
    bool dataACK() const override;
    /** Enables/disables data ACK. */
    void enableDataACK(bool enable) override;
    /** Returns @c true, if auto scan is enabled. */
    virtual bool autoScan() const ;
    /** Enable/disable auto scan. */
    virtual void enableAutoScan(bool enable);

    /** Returns APRS type for reporting the position. */
    APRSType txAPRSType() const;
    /** Sets APRS type for reporting the position. */
    void setTXAPRSType(APRSType aprsType);

    /** Returns the analog APRS PTT setting. */
    virtual AnytoneChannelExtension::APRSPTT analogAPRSPTTSetting() const;
    /** Sets the analog APRS PTT setting. */
    virtual void setAnalogAPRSPTTSetting(AnytoneChannelExtension::APRSPTT ptt);
    /** Returns the digital APRS PTT setting. */
    virtual AnytoneChannelExtension::APRSPTT digitalAPRSPTTSetting() const;
    /** Sets the digital APRS PTT setting. */
    virtual void setDigitalAPRSPTTSetting(AnytoneChannelExtension::APRSPTT ptt);

    /** Returns the DMR APRS system index. */
    unsigned digitalAPRSSystemIndex() const override;
    /** Sets the DMR APRS system index. */
    void setDigitalAPRSSystemIndex(unsigned idx) override;

    /** Returns the frequency correction in ???. */
    virtual int frequenyCorrection() const;
    /** Sets the frequency correction in ???. */
    virtual void setFrequencyCorrection(int corr);

    /** Returns the index of the FM APRS frequency [0,7]. */
    virtual unsigned int fmAPRSFrequencyIndex() const;
    /** Sets the FM APRS frequency index [0,7]. */
    virtual void setFMAPRSFrequencyIndex(unsigned int idx);

    /** If set, transmission of talker alias for this channel is enabled. */
    virtual bool sendTalkerAlias() const;
    /** Enable transmission of talker alias. */
    virtual void enableSendTalkerAlias(bool enable);

    /** Returns the encryption type. */
    virtual AdvancedEncryptionType advancedEncryptionType() const;
    /** Sets the encryptionType. */
    virtual void setEncryptionType(AdvancedEncryptionType type);

    /** Returns @c true, if an AES encryption key index is set. */
    virtual bool hasAESEncryptionKeyIndex() const;
    /** Returns the AES encryption key index.
     * The index is 0-based. */
    virtual unsigned int aesEncryptionKeyIndex() const;
    /** Sets the AES encryption key index. */
    virtual void setAESEncryptionKeyIndex(unsigned int index);
    /** Clears the AES encryption key index. */
    virtual void clearAESEncryptionKeyIndex();

    /** Returns @c true, if an ARC4 encryption key index is set. */
    virtual bool hasARC4EncryptionKeyIndex() const;
    /** Returns the ARC4 encryption key index.
     * The index is 0-based. */
    virtual unsigned int arc4EncryptionKeyIndex() const;
    /** Sets the ARC4 encryption key index. */
    virtual void setARC4EncryptionKeyIndex(unsigned int index);
    /** Clears the ARC4 encryption key index. */
    virtual void clearARC4EncryptionKeyIndex();

    /** Returns the encryption type. */
    DMREncryptionType dmrEncryptionType() const override;
    /** Sets the encryption type. */
    void setDMREncryptionType(DMREncryptionType type) override;
    /** Returns @c true if a DMR encryption key is set. */
    bool hasDMREncryptionKeyIndex() const override;
    /** Returns the DMR encryption key index (+1), 0=Off. */
    unsigned dmrEncryptionKeyIndex() const override;
    /** Sets the DMR encryption key index (+1), 0=Off. */
    void setDMREncryptionKeyIndex(unsigned idx) override;
    /** Clears the DMR encryption key index. */
    void clearDMREncryptionKeyIndex() override;

    /** Constructs a Channel object from this element. */
    Channel *toChannelObj(Context &ctx) const override;
    /** Links a previously created channel object. */
    bool linkChannelObj(Channel *c, Context &ctx) const override;
    /** Encodes the given channel object. */
    bool fromChannelObj(const Channel *c, Context &ctx) override;

  protected:
    /** Internal used offsets within the channel element. */
    struct Offset : public D868UVCodeplug::ChannelElement::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int pttIDSetting()         { return 0x0019; }
      static constexpr unsigned int aesKeyIndex()          { return 0x0022; }
      static constexpr Bit roaming()                       { return {0x034,  2}; }
      static constexpr Bit dataACK()                       { return {0x0034, 3}; }
      static constexpr Bit autoScan()                      { return {0x0034, 4}; }
      static constexpr unsigned int fmAPRSPTTSetting()     { return 0x0036; }
      static constexpr unsigned int dmrAPRSPTTSetting()    { return 0x0037; }
      static constexpr unsigned int dmrAPRSSystemIndex()   { return 0x0038; }
      static constexpr unsigned int frequenyCorrection()   { return 0x0039; }
      static constexpr unsigned int dmrEncryptionKey()     { return 0x003a; }
      static constexpr Bit muteFMAPRS()                    { return {0x003b, 3}; }
      static constexpr Bit talkerAlias()                   { return {0x003b, 4}; }
      static constexpr Bit advancedEncryptionType()        { return {0x003b, 5}; }
      static constexpr unsigned int fmAPRSFrequencyIndex() { return 0x003c; }
      static constexpr unsigned int arc4KeyIndex()         { return 0x003d; }
      /// @endcond
    };
  };


  /** Starting from AT-D878UV, all AnyTone devices encode an channel settings extension element
   * at an offset 0x2000 to the original channel. Also the size of the extension element is
   * identical to the channel element itself. This is weird. Anyway. This class encodes/decodes
   * these settings.
   *
   * This implementation is compatible with firmware version 3.06 */
  class ChannelExtensionElement: public Codeplug::Element
  {
  protected:
    /** Hidden constructor. */
    ChannelExtensionElement(uint8_t *ptr, size_t size);

  public:
    /** Default constructor. */
    ChannelExtensionElement(uint8_t *ptr);

    /** The size of the element. */
    static constexpr unsigned int size() { return ChannelElement::size(); }

    /** Resets the channel extension. */
    void clear();

    /** Returns the BOT 5-tone ID index. */
    virtual unsigned int bot5ToneIDIndex() const;
    /** Sets the BOT 5-tone ID index. */
    virtual void setBOT5ToneIDIndex(unsigned int idx);

    /** Returns the EOT 5-tone ID index. */
    virtual unsigned int eot5ToneIDIndex() const;
    /** Sets the EOT 5-tone ID index. */
    virtual void setEOT5ToneIDIndex(unsigned int idx);

    /** Returns the transmit color code. */
    virtual unsigned int txColorCode() const;
    /** Sets the transmit color code. */
    virtual void setTXColorCode(unsigned int cc);

    /** Constructs a Channel object from this element. */
    virtual bool updateChannelObj(Channel *c, Context &ctx) const;
    /** Links a previously created channel object. */
    virtual bool linkChannelObj(Channel *c, Context &ctx) const;
    /** Encodes the given channel object. */
    virtual bool fromChannelObj(const Channel *c, Context &ctx);

  protected:
    /** Some internal used offsets. */
    struct Offset: public Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int bot5ToneIDIndex() { return 0x0000; }
      static constexpr unsigned int eot5ToneIDIndex() { return 0x0001; }
      static constexpr unsigned int txColorCode()     { return 0x0003; }
      /// @endcond
    };
  };


  /** Represents the general config of the radio within the D878UV binary codeplug.
   *
   * This class implements only the differences to the generic
   * @c AnytonCodeplug::GeneralSettingsElement.
   *
   * Binary encoding of the general settings (size 0x00f0 bytes):
   * @verbinclude d878uv_generalsettings.txt */
  class GeneralSettingsElement: public D868UVCodeplug::GeneralSettingsElement
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
      /** Device specific key functions. */
      typedef enum {
        Off = 0x00, Voltage = 0x01, Power = 0x02, Repeater = 0x03, Reverse = 0x04,
        Encryption = 0x05, Call = 0x06, VOX = 0x07, ToggleVFO = 0x08, SubPTT = 0x09,
        Scan = 0x0a, WFM = 0x0b, Alarm = 0x0c, RecordSwitch = 0x0d, Record = 0x0e, SMS = 0x0f,
        Dial = 0x10, Monitor = 0x12, ToggleMainChannel = 0x13, HotKey1 = 0x14,
        HotKey2 = 0x15, HotKey3 = 0x16, HotKey4 = 0x17, HotKey5 = 0x18, HotKey6 = 0x19,
        WorkAlone = 0x1a, SkipChannel = 0x1b, DMRMonitor = 0x1c, SubChannel = 0x1d,
        PriorityZone = 0x1e, VFOScan = 0x1f, MICSoundQuality = 0x20, LastCallReply = 0x21,
        ChannelType = 0x22, Roaming = 0x24, ChannelRanging = 0x25, MaxVolume = 0x26, Slot = 0x27,
        APRSType = 0x28, Zone = 0x29, RoamingSet = 0x2a, APRSSet = 0x2b, Mute=0x2c,
        CtcssDcsSet=0x2d, TBSTSend = 0x2e, Bluetooth = 0x2f, GPS = 0x30,
        ChannelName = 0x31, CDTScan = 0x32, APRSSend = 0x33, APRSInfo = 0x34
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

  protected:
    /** Possible VFO frequency steps. */
    enum FreqStep {
      FREQ_STEP_2_5kHz = 0,             ///< 2.5kHz
      FREQ_STEP_5kHz = 1,               ///< 5kHz
      FREQ_STEP_6_25kHz = 2,            ///< 6.25kHz
      FREQ_STEP_8_33kHz = 3,            ///< 8.33kHz
      FREQ_STEP_10kHz = 4,              ///< 10kHz
      FREQ_STEP_12_5kHz = 5,            ///< 12.5kHz
      FREQ_STEP_20kHz = 6,              ///< 20kHz
      FREQ_STEP_25kHz = 7,              ///< 25kHz
      FREQ_STEP_50kHz = 8               ///< 50kHz
    };

    /** DTMF signalling durations. */
    enum DTMFDuration {
      DTMF_DUR_50ms = 0, DTMF_DUR_100ms = 1, DTMF_DUR_200ms = 2, DTMF_DUR_300ms = 3, DTMF_DUR_500ms = 4
    };

    /** TBST (open repeater) frequencies. */
    enum class TBSTFrequency {
      Hz1000 = 0, Hz1450 = 1, Hz1750 = 2, Hz2100 = 3
    };

    /** All possible STE (squelch tail eliminate) frequencies. */
    enum class STEFrequency {
      Off = 0, Hz55_2  = 1, Hz259_2 = 2
    };

    /** Possible background images. */
    enum class BackgroundImage {
      Default=0, Custom1=1, Custom2=2
    };

    /** Encoding of possible backlight durations. */
    enum class BacklightDuration {
      Infinite = 0, _5s = 1, _10s = 2, _15s = 3, _20s = 4, _25s = 5, _30s = 6, _1min=7, _2min=8,
      _3min = 9, _4min = 10, _5min = 11, _15min = 12, _30min = 13, _45min = 14, _1h = 15
    };

  protected:
    /** Hidden constructor. */
    GeneralSettingsElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    GeneralSettingsElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x00f0; }

    /** Resets the general settings. */
    void clear();

    /** Returns the transmit timeout in seconds. */
    virtual unsigned transmitTimeout() const;
    /** Sets the transmit timeout in seconds. */
    virtual void setTransmitTimeout(unsigned tot);

    /** Returns the UI language. */
    virtual AnytoneDisplaySettingsExtension::Language language() const;
    /** Sets the UI language. */
    virtual void setLanguage(AnytoneDisplaySettingsExtension::Language lang);

    QTimeZone gpsTimeZone() const;
    void setGPSTimeZone(const QTimeZone &zone);

    /** Returns the VFO frequency step in kHz. */
    virtual Frequency vfoFrequencyStep() const;
    /** Sets the VFO frequency step in kHz. */
    virtual void setVFOFrequencyStep(Frequency kHz);

    AnytoneKeySettingsExtension::KeyFunction funcKeyAShort() const;
    void setFuncKeyAShort(AnytoneKeySettingsExtension::KeyFunction func);
    AnytoneKeySettingsExtension::KeyFunction funcKeyBShort() const;
    void setFuncKeyBShort(AnytoneKeySettingsExtension::KeyFunction func);
    AnytoneKeySettingsExtension::KeyFunction funcKeyCShort() const;
    void setFuncKeyCShort(AnytoneKeySettingsExtension::KeyFunction func);
    AnytoneKeySettingsExtension::KeyFunction funcKey1Short() const;
    void setFuncKey1Short(AnytoneKeySettingsExtension::KeyFunction func);
    AnytoneKeySettingsExtension::KeyFunction funcKey2Short() const;
    void setFuncKey2Short(AnytoneKeySettingsExtension::KeyFunction func);
    AnytoneKeySettingsExtension::KeyFunction funcKeyALong() const;
    void setFuncKeyALong(AnytoneKeySettingsExtension::KeyFunction func);
    AnytoneKeySettingsExtension::KeyFunction funcKeyBLong() const;
    void setFuncKeyBLong(AnytoneKeySettingsExtension::KeyFunction func);
    AnytoneKeySettingsExtension::KeyFunction funcKeyCLong() const;
    void setFuncKeyCLong(AnytoneKeySettingsExtension::KeyFunction func);
    AnytoneKeySettingsExtension::KeyFunction funcKey1Long() const;
    void setFuncKey1Long(AnytoneKeySettingsExtension::KeyFunction func);
    AnytoneKeySettingsExtension::KeyFunction funcKey2Long() const;
    void setFuncKey2Long(AnytoneKeySettingsExtension::KeyFunction func);

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

    Interval backlightDuration() const override;
    void setBacklightDuration(Interval intv) override;

    /** Returns the DTMF tone duration in ms. */
    virtual unsigned dtmfToneDuration() const;
    /** Sets the DTMF tone duration in ms. */
    virtual void setDTMFToneDuration(unsigned ms);

    /** Returns @c true if "man down" is enabled. */
    virtual bool manDown() const;
    /** Enables/disables "man down". */
    virtual void enableManDown(bool enable);

    /** Returns @c true if WFM monitor is enabled. */
    virtual bool wfmMonitor() const;
    /** Enables/disables WFM monitor. */
    virtual void enableWFMMonitor(bool enable);

    /** Returns the TBST frequency. */
    virtual Frequency tbstFrequency() const;
    /** Sets the TBST frequency. */
    virtual void setTBSTFrequency(Frequency freq);

    /** Returns @c true if the "pro mode" is enabled. */
    virtual bool proMode() const;
    /** Enables/disables the "pro mode". */
    virtual void enableProMode(bool enable);

    /** Returns @c true if the own ID is filtered in call lists. */
    virtual bool filterOwnID() const;
    /** Enables/disables filter of own ID in call lists. */
    virtual void enableFilterOwnID(bool enable);
    /** Returns @c true remote stun/kill is enabled. */
    virtual bool remoteStunKill() const;
    /** Enables/disables remote stun/kill. */
    virtual void enableRemoteStunKill(bool enable);
    /** Returns @c true remote monitor is enabled. */
    virtual bool remoteMonitor() const;
    /** Enables/disables remote monitor. */
    virtual void enableRemoteMonitor(bool enable);

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

    /** Returns the "man down" delay in seconds. */
    virtual Interval manDownDelay() const;
    /** Sets the "man down" delay in seconds. */
    virtual void setManDownDelay(Interval sec);
    /** Returns the analog call hold in seconds. */
    virtual unsigned fmCallHold() const;
    /** Sets the analog call hold in seconds. */
    virtual void setFMCallHold(unsigned sec);

    /** Returns @c true if the GPS range reporting is enabled. */
    virtual bool gpsMessageEnabled() const;
    /** Enables/disables GPS range reporting. */
    virtual void enableGPSMessage(bool enable);

    /** Returns @c true if the call channel is maintained. */
    virtual bool maintainCallChannel() const;
    /** Enables/disables maintaining the call channel. */
    virtual void enableMaintainCallChannel(bool enable);

    /** Returns the priority Zone A index. */
    virtual unsigned priorityZoneAIndex() const;
    /** Sets the priority zone A index. */
    virtual void setPriorityZoneAIndex(unsigned idx);
    /** Returns the priority Zone B index. */
    virtual unsigned priorityZoneBIndex() const;
    /** Sets the priority zone B index. */
    virtual void setPriorityZoneBIndex(unsigned idx);

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

    bool keyToneLevelAdjustable() const;
    unsigned keyToneLevel() const;
    void setKeyToneLevel(unsigned level);
    void setKeyToneLevelAdjustable();

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
    /** Returns the standby background image. */
    virtual BackgroundImage standbyBackgroundImage() const;
    /** Sets the standby background image. */
    virtual void setStandbyBackgroundImage(D878UVCodeplug::GeneralSettingsElement::BackgroundImage img);

    bool showLastHeard() const;
    void enableShowLastHeard(bool enable);

    /** Returns the SMS format. */
    virtual AnytoneDMRSettingsExtension::SMSFormat smsFormat() const;
    /** Sets the SMS format. */
    virtual void setSMSFormat(AnytoneDMRSettingsExtension::SMSFormat fmt);

    /** Returns the minimum frequency in Hz for the auto-repeater range in VHF band. */
    virtual Frequency autoRepeaterMinFrequencyVHF() const;
    /** Sets the minimum frequency in Hz for the auto-repeater range in VHF band. */
    virtual void setAutoRepeaterMinFrequencyVHF(Frequency Hz);
    /** Returns the maximum frequency in Hz for the auto-repeater range in VHF band. */
    virtual Frequency autoRepeaterMaxFrequencyVHF() const;
    /** Sets the maximum frequency in Hz for the auto-repeater range in VHF band. */
    virtual void setAutoRepeaterMaxFrequencyVHF(Frequency Hz);

    /** Returns the minimum frequency in Hz for the auto-repeater range in UHF band. */
    virtual Frequency autoRepeaterMinFrequencyUHF() const;
    /** Sets the minimum frequency in Hz for the auto-repeater range in UHF band. */
    virtual void setAutoRepeaterMinFrequencyUHF(Frequency Hz);
    /** Returns the maximum frequency in Hz for the auto-repeater range in UHF band. */
    virtual Frequency autoRepeaterMaxFrequencyUHF() const;
    /** Sets the maximum frequency in Hz for the auto-repeater range in UHF band. */
    virtual void setAutoRepeaterMaxFrequencyUHF(Frequency Hz);

    /** Returns the auto-repeater direction for VFO B. */
    virtual AnytoneAutoRepeaterSettingsExtension::Direction autoRepeaterDirectionB() const;
    /** Sets the auto-repeater direction for VFO B. */
    virtual void setAutoRepeaterDirectionB(AnytoneAutoRepeaterSettingsExtension::Direction dir);

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

    /** Returns the backlight duration during TX in seconds. */
    virtual Interval txBacklightDuration() const;
    /** Sets the backlight duration during TX in seconds. */
    virtual void setTXBacklightDuration(Interval sec);

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

    /** Returns the backlight duration during RX in seconds. */
    Interval rxBacklightDuration() const;
    /** Sets the backlight duration during RX in seconds. */
    void setRXBacklightDuration(Interval sec);

    /** Returns @c true if roaming is enabled. */
    virtual bool roaming() const;
    /** Enables/disables repeater check notification. */
    virtual void enableRoaming(bool enable);

    /** Returns the mute delay in minutes. */
    virtual Interval muteDelay() const;
    /** Sets the mute delay in minutes. */
    virtual void setMuteDelay(Interval min);

    /** Returns the number of repeater check notifications. */
    virtual unsigned repeaterCheckNumNotifications() const;
    /** Sets the number of repeater check notifications. */
    virtual void setRepeaterCheckNumNotifications(unsigned num);

    /** Returns @c true if boot GPS check is enabled. */
    virtual bool bootGPSCheck() const;
    /** Enables/disables boot GPS check. */
    virtual void enableBootGPSCheck(bool enable);
    /** Returns @c true if boot reset is enabled. */
    virtual bool bootReset() const;
    /** Enables/disables boot reset. */
    virtual void enableBootReset(bool enable);

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

    bool fromConfig(const Flags &flags, Context &ctx);
    bool updateConfig(Context &ctx);
    bool linkSettings(RadioSettings *settings, Context &ctx, const ErrorStack &err);

  protected:
    /** Some internal used offsets within the element. */
    struct Offset: public D868UVCodeplug::GeneralSettingsElement::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int transmitTimeout()     { return 0x0004; }
      static constexpr unsigned int language()            { return 0x0005; }
      static constexpr unsigned int vfoFrequencyStep()    { return 0x0008; }
      static constexpr unsigned int steType()             { return 0x0017; }
      static constexpr unsigned int steFrequency()        { return 0x0018; }
      static constexpr unsigned int groupCallHangTime()   { return 0x0019; }
      static constexpr unsigned int privateCallHangTime() { return 0x001a; }
      static constexpr unsigned int preWaveDelay()        { return 0x001b; }
      static constexpr unsigned int wakeHeadPeriod()      { return 0x001c; }
      static constexpr unsigned int wfmChannelIndex()     { return 0x001d; }
      static constexpr unsigned int wfmVFOEnabled()       { return 0x001e; }
      static constexpr unsigned int dtmfToneDuration()    { return 0x0023; }
      static constexpr unsigned int manDown()             { return 0x0024; }
      static constexpr unsigned int wfmMonitor()          { return 0x002b; }
      static constexpr unsigned int tbstFrequency()       { return 0x002e; }
      static constexpr unsigned int proMode()             { return 0x0034; }
      static constexpr unsigned int filterOwnID()         { return 0x0038; }
      static constexpr unsigned int remoteStunKill()      { return 0x003c; }
      static constexpr unsigned int remoteMonitor()       { return 0x003e; }
      static constexpr unsigned int monSlotMatch()        { return 0x0049; }
      static constexpr unsigned int monColorCodeMatch()   { return 0x004a; }
      static constexpr unsigned int monIDMatch()          { return 0x004b; }
      static constexpr unsigned int monTimeSlotHold()     { return 0x004c; }
      static constexpr unsigned int manDownDelay()        { return 0x004f; }
      static constexpr unsigned int fmCallHold()          { return 0x0050; }
      static constexpr unsigned int enableGPSMessage()    { return 0x0053; }
      static constexpr unsigned int maintainCallChannel() { return 0x006e; }
      static constexpr unsigned int priorityZoneA()       { return 0x006f; }
      static constexpr unsigned int priorityZoneB()       { return 0x0070; }
      static constexpr unsigned int bluetooth()           { return 0x00b1; }
      static constexpr unsigned int btAndInternalMic()    { return 0x00b2; }
      static constexpr unsigned int btAndInternalSpeaker(){ return 0x00b3; }
      static constexpr unsigned int pluginRecTone()       { return 0x00b4; }
      static constexpr unsigned int gpsRangingInterval()  { return 0x00b5; }
      static constexpr unsigned int btMicGain()           { return 0x00b6; }
      static constexpr unsigned int btSpeakerGain()       { return 0x00b7; }
      static constexpr unsigned int showChannelNumber()   { return 0x00b8; }
      static constexpr unsigned int showCurrentContact()  { return 0x00b9; }
      static constexpr unsigned int autoRoamPeriod()      { return 0x00ba; }
      static constexpr unsigned int keyToneLevel()        { return 0x00bb; }
      static constexpr unsigned int callColor()           { return 0x00bc; }
      static constexpr unsigned int gpsUnits()            { return 0x00bd; }
      static constexpr unsigned int knobLock()            { return 0x00be; }
      static constexpr unsigned int keypadLock()          { return 0x00be; }
      static constexpr unsigned int sideKeyLock()         { return 0x00be; }
      static constexpr unsigned int forceKeyLock()        { return 0x00be; }
      static constexpr unsigned int autoRoamDelay()       { return 0x00bf; }
      static constexpr unsigned int standbyTextColor()    { return 0x00c0; }
      static constexpr unsigned int standbyBackground()   { return 0x00c1; }
      static constexpr unsigned int showLastHeard()       { return 0x00c2; }
      static constexpr unsigned int smsFormat()           { return 0x00c3; }
      static constexpr unsigned int autoRepMinVHF()       { return 0x00c4; }
      static constexpr unsigned int autoRepMaxVHF()       { return 0x00c8; }
      static constexpr unsigned int autoRepMinUHF()       { return 0x00cc; }
      static constexpr unsigned int autoRepMaxUHF()       { return 0x00d0; }
      static constexpr unsigned int autoRepeaterDirB()    { return 0x00d4; }
      static constexpr unsigned int fmSendIDAndContact()  { return 0x00d5; }
      static constexpr unsigned int defaultChannels()     { return 0x00d7; }
      static constexpr unsigned int defaultZoneA()        { return 0x00d8; }
      static constexpr unsigned int defaultZoneB()        { return 0x00d9; }
      static constexpr unsigned int defaultChannelA()     { return 0x00da; }
      static constexpr unsigned int defaultChannelB()     { return 0x00db; }
      static constexpr unsigned int defaultRoamingZone()  { return 0x00dc; }
      static constexpr unsigned int repRangeCheck()       { return 0x00dd; }
      static constexpr unsigned int rangeCheckInterval()  { return 0x00de; }
      static constexpr unsigned int rangeCheckCount()     { return 0x00df; }
      static constexpr unsigned int roamStartCondition()  { return 0x00e0; }
      static constexpr unsigned int txBacklightDuration() { return 0x00e1; }
      static constexpr unsigned int displaySeparator()    { return 0x00e2; }
      static constexpr unsigned int keepLastCaller()      { return 0x00e3; }
      static constexpr unsigned int channelNameColor()    { return 0x00e4; }
      static constexpr unsigned int repCheckNotify()      { return 0x00e5; }
      static constexpr unsigned int rxBacklightDuration() { return 0x00e6; }
      static constexpr unsigned int roaming()             { return 0x00e7; }
      static constexpr unsigned int muteDelay()           { return 0x00e9; }
      static constexpr unsigned int repCheckNumNotify()   { return 0x00ea; }
      static constexpr unsigned int bootGPSCheck()        { return 0x00eb; }
      static constexpr unsigned int bootReset()           { return 0x00ec; }
      static constexpr unsigned int btHoldTime()          { return 0x00ed; }
      static constexpr unsigned int btRXDelay()           { return 0x00ee; }
      /// @endcond
    };
  };

  /** General settings extension element for the D878UV.
   *
   * Memory representation of the encoded settings element (size 0x200 bytes):
   * @verbinclude d878uv_generalsettingsextension.txt */
  class ExtendedSettingsElement: public AnytoneCodeplug::ExtendedSettingsElement
  {
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

    /** Returns @c true if the talker alias is sent. */
    virtual bool sendTalkerAlias() const;
    /** Enables/disables sending the talker alias. */
    virtual void enableSendTalkerAlias(bool enable);

    /** Returns the talker alias source. */
    virtual AnytoneDMRSettingsExtension::TalkerAliasSource talkerAliasSource() const;
    /** Sets the talker alias source. */
    virtual void setTalkerAliasSource(AnytoneDMRSettingsExtension::TalkerAliasSource mode);

    /** Returns the talker alias encoding. */
    virtual AnytoneDMRSettingsExtension::TalkerAliasEncoding talkerAliasEncoding() const;
    /** Sets the talker alias encoding. */
    virtual void setTalkerAliasEncoding(AnytoneDMRSettingsExtension::TalkerAliasEncoding encoding);

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

    /** Returns @c true if the auto repeater UHF 2 offset index is set. */
    virtual bool hasAutoRepeaterUHF2OffsetIndex() const;
    /** Returns the index of the UHF 2 offset frequency. */
    virtual unsigned autoRepeaterUHF2OffsetIndex() const;
    /** Sets the index of the UHF 2 offset frequency. */
    virtual void setAutoRepeaterUHF2OffsetIndex(unsigned idx);
    /** Clears the auto repeater UHF 2 offset frequency index. */
    virtual void clearAutoRepeaterUHF2OffsetIndex();

    /** Returns @c true if the auto repeater VHF 2 offset index is set. */
    virtual bool hasAutoRepeaterVHF2OffsetIndex() const;
    /** Returns the index of the VHF 2 offset frequency. */
    virtual unsigned autoRepeaterVHF2OffsetIndex() const;
    /** Sets the index of the VHF 2 offset frequency. */
    virtual void setAutoRepeaterVHF2OffsetIndex(unsigned idx);
    /** Clears the auto repeater VHF 2 offset frequency index. */
    virtual void clearAutoRepeaterVHF2OffsetIndex();

    /** Returns the minimum frequency in Hz for the auto-repeater VHF 2 band. */
    virtual Frequency autoRepeaterVHF2MinFrequency() const;
    /** Sets the minimum frequency in Hz for the auto-repeater VHF 2 band. */
    virtual void setAutoRepeaterVHF2MinFrequency(Frequency hz);
    /** Returns the maximum frequency in Hz for the auto-repeater VHF 2 band. */
    virtual Frequency autoRepeaterVHF2MaxFrequency() const;
    /** Sets the maximum frequency in Hz for the auto-repeater VHF 2 band. */
    virtual void setAutoRepeaterVHF2MaxFrequency(Frequency hz);
    /** Returns the minimum frequency in Hz for the auto-repeater UHF 2 band. */
    virtual Frequency autoRepeaterUHF2MinFrequency() const;
    /** Sets the minimum frequency in Hz for the auto-repeater UHF 2 band. */
    virtual void setAutoRepeaterUHF2MinFrequency(Frequency hz);
    /** Returns the maximum frequency in Hz for the auto-repeater UHF 2 band. */
    virtual Frequency autoRepeaterUHF2MaxFrequency() const;
    /** Sets the maximum frequency in Hz for the auto-repeater UHF 2 band. */
    virtual void setAutoRepeaterUHF2MaxFrequency(Frequency hz);

    /** Returns the GPS mode. */
    virtual AnytoneGPSSettingsExtension::GPSMode gpsMode() const;
    /** Sets the GPS mode. */
    virtual void setGPSMode(AnytoneGPSSettingsExtension::GPSMode mode);

    /** Returns the STE (squelch tail elimination) duration. */
    virtual Interval steDuration() const;
    /** Sets the STE (squelch tail elimination) duration. */
    virtual void setSTEDuration(Interval dur);

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

    AnytoneDisplaySettingsExtension::Color channelBNameColor() const;
    void setChannelBNameColor(AnytoneDisplaySettingsExtension::Color color);

    /** Returns the encryption mode. */
    virtual AnytoneDMRSettingsExtension::EncryptionType encryption() const;
    /** Sets the encryption mode. */
    virtual void setEncryption(AnytoneDMRSettingsExtension::EncryptionType mode);

    /** Returns @c true if the transmit timeout notification is enabled. */
    virtual bool totNotification() const;
    /** Enables/disables transmit timeout notification. */
    virtual void enableTOTNotification(bool enable);

    /** Returns @c true if the ATPC (Adaptiv Transmission Power Control) is enabled. */
    virtual bool atpc() const;
    /** Enables/disables the ATPC (Adaptiv Transmission Power Control). */
    virtual void enableATPC(bool enable);

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

    /** Returns @c true if the GPS roaming is enabled. */
    virtual bool gpsRoaming() const;
    /** Enables/disables GPS roaming. */
    virtual void enableGPSRoaming(bool enable);

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
    /** Link config from settings extension. */
    virtual bool linkConfig(Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits for the settings. */
    struct Limit {
      static constexpr unsigned int maxBluetoothPTTSleepDelay() { return 4; }    ///< Maximum delay in minutes.
    };

  protected:
    /** Internal used offset within the element. */
    struct Offset: public AnytoneCodeplug::ExtendedSettingsElement::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int sendTalkerAlias()              { return 0x0000; }
      static constexpr unsigned int talkerAliasDisplay()           { return 0x001e; }
      static constexpr unsigned int talkerAliasEncoding()          { return 0x001f; }
      static constexpr unsigned int btPTTLatch()                   { return 0x0020; }
      static constexpr unsigned int autoRepeaterUHF2OffsetIndex()  { return 0x0022; }
      static constexpr unsigned int autoRepeaterVHF2OffsetIndex()  { return 0x0023; }
      static constexpr unsigned int autoRepeaterVHF2MinFrequency() { return 0x0024; }
      static constexpr unsigned int autoRepeaterVHF2MaxFrequency() { return 0x0028; }
      static constexpr unsigned int autoRepeaterUHF2MinFrequency() { return 0x002c; }
      static constexpr unsigned int autoRepeaterUHF2MaxFrequency() { return 0x0030; }
      static constexpr unsigned int btPTTSleepDelay()              { return 0x0034; }
      static constexpr unsigned int gpsMode()                      { return 0x0035; }
      static constexpr unsigned int steDuration()                  { return 0x0036; }
      static constexpr unsigned int manGrpCallHangTime()           { return 0x0037; }
      static constexpr unsigned int manPrivCallHangTime()          { return 0x0038; }
      static constexpr unsigned int channelBNameColor()            { return 0x0039; }
      static constexpr unsigned int encryptionType()               { return 0x003a; }
      static constexpr unsigned int totNotification()              { return 0x003b; }
      static constexpr unsigned int atpc()                         { return 0x003c; }
      static constexpr unsigned int zoneANameColor()               { return 0x003d; }
      static constexpr unsigned int zoneBNameColor()               { return 0x003e; }
      static constexpr unsigned int autoShutdownMode()             { return 0x003f; }
      static constexpr Bit displayColorCode()                      { return {0x0040, 2}; }
      static constexpr Bit displayTimeSlot()                       { return {0x0040, 1}; }
      static constexpr Bit displayChannelType()                    { return {0x0040, 0}; }
      static constexpr unsigned int fmIdleTone()                   { return 0x0041; }
      static constexpr unsigned int dateFormat()                   { return 0x0042; }
      static constexpr unsigned int analogMicGain()                { return 0x0043; }
      static constexpr unsigned int gpsRoaming()                   { return 0x0044; }
      static constexpr unsigned int callEndTones()                 { return 0x0046; }
      static constexpr unsigned int callEndDurations()             { return 0x0050; }
      static constexpr unsigned int allCallTones()                 { return 0x005a; }
      static constexpr unsigned int allCallDurations()             { return 0x0064; }
      /// @endcond
    };
  };


  /** Implements some storage to hold the names for the FM APRS frequencies. */
  class FMAPRSFrequencyNamesElement: public Element
  {
  protected:
    /** Hidden constructor. */
    FMAPRSFrequencyNamesElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    explicit FMAPRSFrequencyNamesElement(uint8_t *ptr);

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x0080; }

    void clear();

    /** Returns the n-th name. The 0-th name, is the name of the FM APRS system. */
    virtual QString name(unsigned int n) const;
    /** Sets the n-th name. The 0-th name, is the name of the FM APRS system. */
    virtual void setName(unsigned int n, const QString &name);

  public:
    /** Some limits for the element. */
    struct Limit {
      static constexpr unsigned int nameLength() { return 16; }       ///< Maximum name length.
    };

  protected:
    /** Some internal offsets within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int betweenNames() { return 0x0010; }
      /// @endcond
    };
  };


  /** Represents the APRS settings within the binary D878UV codeplug.
   *
   * Memory layout of APRS settings (size 0x00f0 bytes):
   * @verbinclude d878uv_aprssetting.txt
   */
  class APRSSettingsElement: public Element
  {
  protected:
    /** Hidden constructor. */
    APRSSettingsElement(uint8_t *ptr, unsigned size);

    /** Possible settings for the FM APRS subtone type. */
    enum class SignalingType {
      Off=0, CTCSS=1, DCS=2
    };

  public:
    /** Constructor. */
    explicit APRSSettingsElement(uint8_t *ptr);

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x0100; }

    /** Resets the settings. */
    void clear();
    bool isValid() const;

    /** Returns the TX delay in ms. */
    virtual Interval fmTXDelay() const;
    /** Sets the TX delay in ms. */
    virtual void setFMTXDelay(Interval ms);

    /** Returns the sub tone settings. */
    virtual SelectiveCall txTone() const;
    /** Sets the sub tone settings. */
    virtual void setTXTone(const SelectiveCall &code);

    /** Returns the manual TX interval in seconds. */
    virtual Interval manualTXInterval() const;
    /** Sets the manual TX interval in seconds. */
    virtual void setManualTXInterval(Interval sec);

    /** Returns @c true if the auto transmit is enabled. */
    virtual bool autoTX() const;
    /** Returns the auto TX interval in seconds. */
    virtual Interval autoTXInterval() const;
    /** Sets the auto TX interval in seconds. */
    virtual void setAutoTXInterval(Interval sec);
    /** Disables auto tx. */
    virtual void disableAutoTX();

    /** Returns @c true if a fixed location is sent. */
    virtual bool fixedLocationEnabled() const;
    /** Returns the fixed location send. */
    virtual QGeoCoordinate fixedLocation() const;
    /** Sets the fixed location to send. */
    virtual void setFixedLocation(QGeoCoordinate &loc);
    /** Disables sending a fixed location. */
    virtual void disableFixedLocation();

    /** Returns the destination call. */
    virtual QString destination() const;
    /** Returns the destination SSID. */
    virtual unsigned destinationSSID() const;
    /** Sets the destination call & SSID. */
    virtual void setDestination(const QString &call, unsigned ssid);
    /** Returns the source call. */
    virtual QString source() const;
    /** Returns the source SSID. */
    virtual unsigned sourceSSID() const;
    /** Sets the source call & SSID. */
    virtual void setSource(const QString &call, unsigned ssid);

    /** Returns the path string. */
    virtual QString path() const;
    /** Sets the path string. */
    virtual void setPath(const QString &path);

    /** Returns the APRS icon. */
    virtual APRSSystem::Icon icon() const;
    /** Sets the APRS icon. */
    virtual void setIcon(APRSSystem::Icon icon);

    /** Returns the transmit power. */
    virtual Channel::Power power() const;
    /** Sets the transmit power. */
    virtual void setPower(Channel::Power power);

    /** Returns the pre-wave delay in ms. */
    virtual Interval fmPreWaveDelay() const;
    /** Sets the pre-wave delay in ms. */
    virtual void setFMPreWaveDelay(Interval ms);

    /** Returns @c true if the channel points to the current/selected channel. */
    virtual bool dmrChannelIsSelected(unsigned n) const;
    /** Returns the digital channel index for the n-th system. */
    virtual unsigned dmrChannelIndex(unsigned n) const;
    /** Sets the digital channel index for the n-th system. */
    virtual void setDMRChannelIndex(unsigned n, unsigned idx);
    /** Sets the channel to the current/selected channel. */
    virtual void setDMRChannelSelected(unsigned n);

    /** Returns the destination contact for the n-th system. */
    virtual unsigned dmrDestination(unsigned n) const;
    /** Sets the destination contact for the n-th system. */
    virtual void setDMRDestination(unsigned n, unsigned idx);

    /** Returns the call type for the n-th system. */
    virtual DMRContact::Type dmrCallType(unsigned n) const;
    /** Sets the call type for the n-th system. */
    virtual void setDMRCallType(unsigned n, DMRContact::Type type);

    /** Returns @c true if the n-th system overrides the channel time-slot. */
    virtual bool dmrTimeSlotOverride(unsigned n);
    /** Returns the time slot if overridden (only valid if @c timeSlot returns true). */
    virtual DMRChannel::TimeSlot dmrTimeSlot(unsigned n) const;
    /** Overrides the time slot of the n-th selected channel. */
    virtual void setDMRTimeSlot(unsigned n, DMRChannel::TimeSlot ts);
    /** Clears the time-slot override. */
    virtual void clearDMRTimeSlotOverride(unsigned n);

    /** Returns @c true if the roaming is enabled. */
    virtual bool dmrRoaming() const;
    /** Enables/disables roaming. */
    virtual void enableDMRRoaming(bool enable);

    /** Returns the the repeater activation delay in ms. */
    virtual Interval dmrPreWaveDelay() const;
    /** Sets the repeater activation delay in ms. */
    virtual void setDMRPreWaveDelay(Interval ms);

    /** Returns @c true if a received APRS message is shown indefinitely. */
    virtual bool infiniteDisplayTime() const;
    /** Returns the time, a received APRS message is shown. */
    virtual Interval displayTime() const;
    /** Sets the time, a received APRS is shown. */
    virtual void setDisplayTime(Interval dur);
    /** Sets the APRS display time to infinite. */
    virtual void setDisplayTimeInifinite();

    /** Returns the FM APRS channel width. */
    virtual AnytoneFMAPRSSettingsExtension::Bandwidth fmChannelWidth() const;
    /** Sets the FM APRS channel width. */
    virtual void setFMChannelWidth(AnytoneFMAPRSSettingsExtension::Bandwidth width);

    /** Returns @c true if the CRC check on received FM APRS messages is disabled. */
    virtual bool fmPassAll() const;
    /** Enables/disables "pass all", that is the CRC check on FM APRS messages is disabled. */
    virtual void enableFMPassAll(bool enable);

    /** Returns @c true if the n-th of 8 FM APRS frequencies is set. */
    virtual bool fmFrequencySet(unsigned int n) const;
    /** Returns the n-th of 8 FM APRS frequencies. */
    virtual Frequency fmFrequency(unsigned int n) const;
    /** Sets the n-th of 8 FM APRS frequencies. */
    virtual void setFMFrequency(unsigned int n, Frequency f);
    /** Clears the n-th of 8 FM APRS frequencies. */
    virtual void clearFMFrequency(unsigned int n);

    /** Returns @c true if the report position flag is set. */
    virtual bool reportPosition() const;
    /** Enables/disables report position flag. */
    virtual void enableReportPosition(bool enable);
    /** Returns @c true if the report Mic-E flag is set. */
    virtual bool reportMicE() const;
    /** Enables/disables report Mic-E flag. */
    virtual void enableReportMicE(bool enable);
    /** Returns @c true if the report object flag is set. */
    virtual bool reportObject() const;
    /** Enables/disables report object flag. */
    virtual void enableReportObject(bool enable);
    /** Returns @c true if the report item flag is set. */
    virtual bool reportItem() const;
    /** Enables/disables report item flag. */
    virtual void enableReportItem(bool enable);
    /** Returns @c true if the report message flag is set. */
    virtual bool reportMessage() const;
    /** Enables/disables report message flag. */
    virtual void enableReportMessage(bool enable);
    /** Returns @c true if the report weather flag is set. */
    virtual bool reportWeather() const;
    /** Enables/disables report weather flag. */
    virtual void enableReportWeather(bool enable);
    /** Returns @c true if the report NMEA flag is set. */
    virtual bool reportNMEA() const;
    /** Enables/disables report NMEA flag. */
    virtual void enableReportNMEA(bool enable);
    /** Returns @c true if the report status flag is set. */
    virtual bool reportStatus() const;
    /** Enables/disables report status flag. */
    virtual void enableReportStatus(bool enable);
    /** Returns @c true if the report other flag is set. */
    virtual bool reportOther() const;
    /** Enables/disables report other flag. */
    virtual void enableReportOther(bool enable);

    /** Configures this APRS system from the given generic config. */
    virtual bool fromFMAPRSSystem(const APRSSystem *sys, Context &ctx,
                                  FMAPRSFrequencyNamesElement &names,
                                  const ErrorStack &err=ErrorStack());
    /** Constructs a generic APRS system configuration from this APRS system. */
    virtual APRSSystem *toFMAPRSSystem(
        Context &ctx, const FMAPRSFrequencyNamesElement &names, const ErrorStack &err=ErrorStack());
    /** Links the transmit channel within the generic APRS system based on the transmit frequency
     * defined within this APRS system. */
    virtual bool linkFMAPRSSystem(APRSSystem *sys, Context &ctx);

    /** Constructs all GPS system from the generic configuration. */
    virtual bool fromDMRAPRSSystems(Context &ctx);
    /** Encodes the given GPS system. */
    virtual bool fromDMRAPRSSystemObj(unsigned int idx, GPSSystem *sys, Context &ctx);
    /** Constructs a generic GPS system from the idx-th encoded GPS system. */
    virtual GPSSystem *toDMRAPRSSystemObj(int idx) const;
    /** Links the specified generic GPS system. */
    virtual bool linkDMRAPRSSystem(int idx, GPSSystem *sys, Context &ctx) const;

  public:
    /** Some static limits for this element. */
    struct Limit {
      /// Maximum length of call signs.
      static constexpr unsigned int callLength()                           { return 0x0006; }
      /// Maximum length of the repeater path string.
      static constexpr unsigned int pathLength()                           { return 0x0020; }
      /// Maximum number of DMR APRS systems.
      static constexpr unsigned int dmrSystems()                           { return 0x0008; }
      /// Maximum number of FM APRS frequencies.
      static constexpr unsigned int fmFrequencies()                        { return 0x0008; }
    };

  protected:
    /** Internal used offsets within the codeplug element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int fmTXDelay()                            { return 0x0005; }
      static constexpr unsigned int fmSigType()                            { return 0x0006; }
      static constexpr unsigned int fmCTCSS()                              { return 0x0007; }
      static constexpr unsigned int fmDCS()                                { return 0x0008; }
      static constexpr unsigned int manualTXInterval()                     { return 0x000a; }
      static constexpr unsigned int autoTXInterval()                       { return 0x000b; }
      static constexpr unsigned int fmTXMonitor()                          { return 0x000c; }
      static constexpr unsigned int fixedLocation()                        { return 0x000d; }
      static constexpr unsigned int fixedLatDeg()                          { return 0x000e; }
      static constexpr unsigned int fixedLatMin()                          { return 0x000f; }
      static constexpr unsigned int fixedLatSec()                          { return 0x0010; }
      static constexpr unsigned int fixedLatSouth()                        { return 0x0011; }
      static constexpr unsigned int fixedLonDeg()                          { return 0x0012; }
      static constexpr unsigned int fixedLonMin()                          { return 0x0013; }
      static constexpr unsigned int fixedLonSec()                          { return 0x0014; }
      static constexpr unsigned int fixedLonWest()                         { return 0x0015; }
      static constexpr unsigned int destinationCall()                      { return 0x0016; }
      static constexpr unsigned int destinationSSID()                      { return 0x001c; }
      static constexpr unsigned int sourceCall()                           { return 0x001d; }
      static constexpr unsigned int sourceSSID()                           { return 0x0023; }
      static constexpr unsigned int path()                                 { return 0x0024; }
      static constexpr unsigned int symbolTable()                          { return 0x0039; }
      static constexpr unsigned int symbol()                               { return 0x003a; }
      static constexpr unsigned int fmPower()                              { return 0x003b; }
      static constexpr unsigned int fmPrewaveDelay()                       { return 0x003c; }
      static constexpr unsigned int dmrChannelIndices()                    { return 0x0040; }
      static constexpr unsigned int betweenDMRChannelIndices()             { return 0x0002; }
      static constexpr unsigned int dmrDestinations()                      { return 0x0050; }
      static constexpr unsigned int betweenDMRDestinations()               { return 0x0004; }
      static constexpr unsigned int dmrCallTypes()                         { return 0x0070; }
      static constexpr unsigned int betweenDMRCallTypes()                  { return 0x0001; }
      static constexpr unsigned int roamingSupport()                       { return 0x0078; }
      static constexpr unsigned int dmrTimeSlots()                         { return 0x0079; }
      static constexpr unsigned int betweenDMRTimeSlots()                  { return 0x0001; }
      static constexpr unsigned int dmrPrewaveDelay()                      { return 0x0081; }
      static constexpr unsigned int displayInterval()                      { return 0x0082; }
      static constexpr unsigned int fixedHeight()                          { return 0x00a6; }
      static constexpr unsigned int reportPosition()                       { return 0x00a8; }
      static constexpr unsigned int reportMicE()                           { return 0x00a8; }
      static constexpr unsigned int reportObject()                         { return 0x00a8; }
      static constexpr unsigned int reportItem()                           { return 0x00a8; }
      static constexpr unsigned int reportMessage()                        { return 0x00a8; }
      static constexpr unsigned int reportWeather()                        { return 0x00a8; }
      static constexpr unsigned int reportNMEA()                           { return 0x00a8; }
      static constexpr unsigned int reportStatus()                         { return 0x00a8; }
      static constexpr unsigned int reportOther()                          { return 0x00a9; }
      static constexpr unsigned int fmWidth()                              { return 0x00aa; }
      static constexpr unsigned int passAll()                              { return 0x00ab; }
      static constexpr unsigned int fmFrequencies()                        { return 0x00ac; }
      static constexpr unsigned int betweenFMFrequencies()                 { return 0x0004; }

      /// @endcond
    };
  };

  /** Represents an (analog/FM) APRS message. */
  class AnalogAPRSMessageElement: public Element
  {
  protected:
    /** Hidden constructor. */
    AnalogAPRSMessageElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    AnalogAPRSMessageElement(uint8_t *ptr);

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x0040; }

    void clear();

    /** Returns the message. */
    virtual QString message() const;
    /** Sets the message. */
    virtual void setMessage(const QString &msg);

  public:
    /** Some limits. */
    struct Limit {
      static constexpr unsigned int length() { return 60; }    ///< Maximum message length.
    };
  };

  /** Represents an analog APRS RX entry.
   *
   * Memory layout of analog APRS RX entry (size 0x0008 bytes):
   * @verbinclude d878uv_aprsrxentry.txt */
  class AnalogAPRSRXEntryElement: public Element
  {
  protected:
    /** Hidden constructor. */
    AnalogAPRSRXEntryElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    AnalogAPRSRXEntryElement(uint8_t *ptr);

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x0008; }

    /** Resets the entry. */
    void clear();
    /** Returns @c true if the APRS RX entry is valid. */
    bool isValid() const;

    /** Returns the call sign. */
    virtual QString call() const;
    /** Returns the SSID. */
    virtual unsigned ssid() const;
    /** Sets the call, SSID and enables the entry. */
    virtual void setCall(const QString &call, unsigned ssid);
  };

  /** Implements the binary representation of a roaming channel within the codeplug.
   *
   * Memory layout of roaming channel (size 0x0020 bytes):
   * @verbinclude d878uv_roamingchannel.txt */
  class RoamingChannelElement: public Element
  {
  protected:
    /** Hidden constructor. */
    RoamingChannelElement(uint8_t *ptr, unsigned size);

  protected:
    /** Special values for the color code. */
    enum ColorCodeValue {
      Disabled = 16
    };

    /** Encoded values for the time slot. */
    enum TimeSlotValue {
      TS1 = 0,
      TS2 = 1
    };

  public:
    /** Constructor. */
    RoamingChannelElement(uint8_t *ptr);

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x0020; }

    /** Resets the roaming channel. */
    void clear();

    /** Returns the RX frequency in Hz. */
    virtual unsigned rxFrequency() const;
    /** Sets the RX frequency in Hz. */
    virtual void setRXFrequency(unsigned hz);
    /** Returns the TX frequency in Hz. */
    virtual unsigned txFrequency() const;
    /** Sets the TX frequency in Hz. */
    virtual void setTXFrequency(unsigned hz);

    /** Returns @c true if the color code is set. */
    virtual bool hasColorCode() const;
    /** Returns the color code. */
    virtual unsigned colorCode() const;
    /** Sets the color code. */
    virtual void setColorCode(unsigned cc);
    /** Disables the color code for the roaming channel. */
    virtual void disableColorCode();

    /** Returns the time slot. */
    virtual DMRChannel::TimeSlot timeSlot() const;
    /** Sets the time slot. */
    virtual void setTimeSlot(DMRChannel::TimeSlot ts);

    /** Returns the name of the channel. */
    virtual QString name() const;
    /** Sets the name of the channel. */
    virtual void setName(const QString &name);

    /** Constructs a roaming channel from the given digital channel. */
    virtual bool fromChannel(const RoamingChannel *ch);
    /** Constructs a @c RoamingChannel instance for this roaming channel. */
    virtual RoamingChannel *toChannel(Context &ctx);

  public:
    /** Some limits. */
    struct Limit {
      static constexpr unsigned int nameLength() { return 16; }       ///< Maximum name length.
    };

  protected:
    /** Some internal offsets within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int rxFrequency() { return 0x0000; }
      static constexpr unsigned int txFrequency() { return 0x0004; }
      static constexpr unsigned int colorCode()   { return 0x0008; }
      static constexpr unsigned int timeSlot()    { return 0x0009; }
      static constexpr unsigned int name()        { return 0x000a; }
      /// @endcond
    };
  };


  /** Represents the bitmap, indicating which roaming channel is valid. */
  class RoamingChannelBitmapElement: public BitmapElement
  {
  protected:
    /** Hidden constructor. */
    RoamingChannelBitmapElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    RoamingChannelBitmapElement(uint8_t *ptr);

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x0020; }
  };

  /** Represents a roaming zone within the binary codeplug.
   *
   * Memory layout of roaming zone (0x80byte):
   * @verbinclude d878uv_roamingzone.txt */
  class RoamingZoneElement: public Element
  {
  protected:
    /** Hidden constructor. */
    RoamingZoneElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    RoamingZoneElement(uint8_t *ptr);

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x0080; }

    /** Clears the roaming zone. */
    void clear();

    /** Returns @c true if the n-th member is set. */
    virtual bool hasMember(unsigned n) const;
    /** Returns the n-th member index. */
    virtual unsigned member(unsigned n) const;
    /** Sets the n-th member index. */
    virtual void setMember(unsigned n, unsigned idx);
    /** Clears the n-th member. */
    virtual void clearMember(unsigned n);

    /** Returns the name of the zone. */
    virtual QString name() const;
    /** Sets the name of the zone. */
    virtual void setName(const QString &name);

    /** Assembles a binary representation of the given RoamingZone instance.*/
    virtual bool fromRoamingZone(RoamingZone *zone, Context& ctx, const ErrorStack &err=ErrorStack());
    /** Constructs a @c RoamingZone instance from this configuration. */
    virtual RoamingZone *toRoamingZone(Context& ctx, const ErrorStack &err=ErrorStack()) const;
    /** Links the given RoamingZone. */
    virtual bool linkRoamingZone(RoamingZone *zone, Context& ctx, const ErrorStack& err=ErrorStack());

  public:
    /** Some limits. */
    struct Limit {
      static constexpr unsigned int nameLength() { return 16; }          ///< Maximum name length.
      static constexpr unsigned int numMembers() { return 64; }          ///< Maximum number of roaming channel in zone.
    };

  protected:
    /** Some internal offsets within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int members()        { return 0x0000; }
      static constexpr unsigned int betweenMembers() { return 0x0001; }
      static constexpr unsigned int name()           { return 0x0040; }
      /// @endcond
    };
  };

  /** Represents the bitmap, indicating which roaming zone is valid. */
  class RoamingZoneBitmapElement: public BitmapElement
  {
  protected:
    /** Hidden constructor. */
    RoamingZoneBitmapElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    RoamingZoneBitmapElement(uint8_t *ptr);

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x0010; }
  };


  /** Represents an AES encryption key.
   *
   * Binary representation of a variable size AES key. The key size is between 4 and 256 bits. */
  class AESEncryptionKeyElement: public Element
  {
  protected:
    /** Hidden constructor. */
    AESEncryptionKeyElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    AESEncryptionKeyElement(uint8_t *ptr);

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x0040; }

    /** Resets the key. */
    void clear();

    /** Returns @c true if the key is set. */
    bool isValid() const;

    /** Returns the key index. */
    virtual unsigned index() const;
    /** Sets the key index. */
    virtual void setIndex(unsigned idx);

    /** Returns the actual key. */
    virtual QByteArray key() const;
    /** Sets the key. */
    virtual void setKey(const QByteArray &key);

  public:
    /** Some limits of the key element. */
    struct Limit: public Element::Limit {
      /// The maximum index.
      static constexpr unsigned int maxIndex() { return 254; }
      /// The maximum key length in bytes.
      static constexpr unsigned int keySize() { return 32; }
    };

  protected:
    /** Some internal offsets. */
    struct Offset: public Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int index() { return 0x0000; }
      static constexpr unsigned int key()   { return 0x0001; }
      static constexpr unsigned int size()  { return 0x0022; }
      /// @endcond
    };
  };

  /** Represents the bitmap, indicating which AES key is valid. */
  class AESEncryptionKeyBitmapElement: public BitmapElement
  {
  protected:
    /** Hidden constructor. */
    AESEncryptionKeyBitmapElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    AESEncryptionKeyBitmapElement(uint8_t *ptr);

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x0020; }
  };


  /** Represents an ARC4 encryption key.
   *
   * Encodes a 8bit ID and 40bit key. A smaller key might be encoded right-aligned. */
  class ARC4EncryptionKeyElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ARC4EncryptionKeyElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    ARC4EncryptionKeyElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x0010; }

    void clear();
    bool isValid() const;

    /** Returns the key index. */
    virtual unsigned index() const;
    /** Sets the key index. */
    virtual void setIndex(unsigned idx);

    /** Returns the actual key. */
    virtual QByteArray key() const;
    /** Sets the key. */
    virtual void setKey(const QByteArray &key);

  public:
    /** Some limits of the key element. */
    struct Limit: public Element::Limit {
      /// The maximum index.
      static constexpr unsigned int maxIndex() { return 254; }
      /// The maximum key length in bytes.
      static constexpr unsigned int keySize() { return 5; }
    };

  protected:
    /** Some internal offsets. */
    struct Offset: public Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int index() { return 0x0000; }
      static constexpr unsigned int key()   { return 0x0001; }
      /// @endcond
    };
  };

  /** Represents the bitmap, indicating which ARC4 key is valid. */
  class ARC4EncryptionKeyBitmapElement: public BitmapElement
  {
  protected:
    /** Hidden constructor. */
    ARC4EncryptionKeyBitmapElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    ARC4EncryptionKeyBitmapElement(uint8_t *ptr);

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x0020; }
  };


  /** Encodes the bitmap, indicating which zone is hidden. */
  class HiddenZoneBitmapElement: public BitmapElement
  {
  protected:
    /** Hidden constructor. */
    HiddenZoneBitmapElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    HiddenZoneBitmapElement(uint8_t *ptr);

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x0020; }
  };

  /** Encodes some information about the radio and firmware.
   *
   * Binary encoding of the info, size 0x0100 bytes:
   * @verbinclude d878uv_radioinfo.txt */
  class RadioInfoElement: public Element
  {
  public:
    /** Possible frequency ranges for the AT-D878UV. */
    enum FrequencyRange {
      RX_400_480_136_174_TX_400_480_136_174 = 0,
      RX_400_480_136_174_TX_400_480_136_174_STEP_12_5kHz = 1,
      RX_430_440_136_174_TX_430_440_136_174 = 2,
      RX_400_480_136_174_TX_430_440_144_146 = 3,
      RX_440_480_136_174_TX_440_480_136_174 = 4,
      RX_440_480_144_146_TX_440_480_144_146 = 5,
      RX_446_447_136_174_TX_446_447_136_174 = 6,
      RX_400_480_136_174_TX_420_450_136_174 = 7,
      RX_400_470_136_174_TX_400_470_136_174 = 8,
      RX_430_432_144_146_TX_430_432_144_146 = 9,
      RX_400_480_136_174_TX_430_450_144_148 = 10,
      RX_400_520_136_174_TX_400_520_136_174 = 11,
      RX_400_490_136_174_TX_400_490_136_174 = 12,
      RX_400_480_136_174_TX_403_470_136_174 = 13,
      RX_400_520_220_225_136_174_TX_400_520_220_225_136_174 = 14,
      RX_420_520_144_148_TX_420_520_144_148 = 15,
      RX_430_440_144_147_TX_430_440_144_147 = 16,
      RX_430_440_136_174_TX_136_174 = 17
    };

  protected:
    /** Hidden constructor. */
    RadioInfoElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit RadioInfoElement(uint8_t *ptr);

    /** Resets the info. */
    void clear();

    /** Returns @c true if full test is enabled.
     * @warning Do not enable, may brick device! */
    virtual bool fullTest() const;

    /** Returns the frequency range. */
    virtual FrequencyRange frequencyRange() const;
    /** Sets the frequency range. */
    virtual void setFrequencyRange(FrequencyRange range);

    /** Returns @c true if "international" is enabled. */
    virtual bool international() const;
    /** Enables/disables "international". */
    virtual void enableInternational(bool enable);

    /** Returns @c true if band select is enabled. */
    virtual bool bandSelect() const;
    /** Enables/disables band select. */
    virtual void enableBandSelect(bool enable);

    /** Returns the band-select password. */
    virtual QString bandSelectPassword() const;
    /** Sets the band-select password. */
    virtual void setBandSelectPassword(const QString &passwd);

    /** Returns the radio type. */
    virtual QString radioType() const;

    /** Returns the program password. */
    virtual QString programPassword() const;
    /** Sets the program password. */
    virtual void setProgramPassword(const QString &passwd);

    /** Returns the area code. */
    virtual QString areaCode() const;
    /** Returns the serial number. */
    virtual QString serialNumber() const;
    /** Returns the production date. */
    virtual QString productionDate() const;
    /** Returns the manufacturer code. */
    virtual QString manufacturerCode() const;
    /** Returns the maintained date. */
    virtual QString maintainedDate() const;
    /** Returns the dealer code. */
    virtual QString dealerCode() const;
    /** Returns the stock date. */
    virtual QString stockDate() const;
    /** Returns the sell date. */
    virtual QString sellDate() const;
    /** Returns the seller. */
    virtual QString seller() const;
    /** Returns the maintainer note. */
    virtual QString maintainerNote() const;
  };

protected:
  /** Hidden constructor. */
  explicit D878UVCodeplug(const QString &label, QObject *parent = nullptr);

public:
  /** Empty constructor. */
  explicit D878UVCodeplug(QObject *parent = nullptr);

  Config *preprocess(Config *config, const ErrorStack &err) const;

protected:
  bool allocateBitmaps();
  void setBitmaps(Context &ctx);
  void allocateForDecoding();
  void allocateUpdated();
  void allocateForEncoding();

  bool encodeElements(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createElements(Context &ctx, const ErrorStack &err=ErrorStack());
  bool linkElements(Context &ctx, const ErrorStack &err=ErrorStack());

  void allocateChannels();
  bool encodeChannels(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createChannels(Context &ctx, const ErrorStack &err=ErrorStack());
  bool linkChannels(Context &ctx, const ErrorStack &err=ErrorStack());

  virtual void allocateZones();
  virtual bool encodeZone(int i, Zone *zone, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  virtual bool decodeZone(int i, Zone *zone, Context &ctx, const ErrorStack &err=ErrorStack());

  void allocateGeneralSettings();
  bool encodeGeneralSettings(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool decodeGeneralSettings(Context &ctx, const ErrorStack &err=ErrorStack());
  bool linkGeneralSettings(Context &ctx, const ErrorStack &err=ErrorStack());

  void allocateGPSSystems();
  bool encodeGPSSystems(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createGPSSystems(Context &ctx, const ErrorStack &err=ErrorStack());
  bool linkGPSSystems(Context &ctx, const ErrorStack &err=ErrorStack());

  /** Allocates memory to store all roaming channels and zones. */
  virtual void allocateRoaming();
  /** Encodes the roaming channels and zones. */
  virtual bool encodeRoaming(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  /** Creates roaming channels and zones from codeplug. */
  virtual bool createRoaming(Context &ctx, const ErrorStack &err=ErrorStack());
  /** Links roaming channels and zones. */
  virtual bool linkRoaming(Context &ctx, const ErrorStack &err=ErrorStack());

  /** Allocates memory to encode/decode AES keys. */
  virtual void allocateAESKeys();
  /** Encode all AES keys. */
  virtual bool encodeAESKeys(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  /** Decode AES keys from the codeplug. */
  virtual bool createAESKeys(Context &ctx, const ErrorStack &err=ErrorStack());

  /** Allocates memory to encode/decode ARC4 keys. */
  virtual void allocateARC4Keys();
  /** Encode all ARC4 keys. */
  virtual bool encodeARC4Keys(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  /** Decode ARC4 keys from the codeplug. */
  virtual bool createARC4Keys(Context &ctx, const ErrorStack &err=ErrorStack());


public:
  /** Some limits. */
  struct Limit: public D868UVCodeplug::Limit {
    static constexpr unsigned int analogAPRSRXEntries() { return 32; }   ///< Maximum number of analog APRS RX entries.
    static constexpr unsigned int roamingChannels()     { return 250; }  ///< Maximum number of roaming channels.
    static constexpr unsigned int roamingZones()        { return 64; }   ///< Maximum number of roaming zones.
    static constexpr unsigned int aesKeys()             { return 255; }  ///< Maximum number of AES keys.
    static constexpr unsigned int arc4Keys()            { return 255; }  ///< Maximum number of ARC4 keys.
  };

protected:
  /** Internal offsets within the codeplug. */
  struct Offset: public D868UVCodeplug::Offset {
    /// @cond DO_NOT_DOCUMENT
    static constexpr unsigned int toChannelExtension()          { return 0x00002000; }
    static constexpr unsigned int settingsExtension()           { return 0x02501400; }
    static constexpr unsigned int aprsSettings()                { return 0x02501000; }
    static constexpr unsigned int analogAPRSMessage()           { return 0x02501200; }
    static constexpr unsigned int analogAPRSRXEntries()         { return 0x02501800; }
    static constexpr unsigned int fmAPRSFrequencyNames()        { return 0x02502000; }
    static constexpr unsigned int hiddenZoneBitmap()            { return 0x024c1360; }
    static constexpr unsigned int roamingChannelBitmap()        { return 0x01042000; }
    static constexpr unsigned int roamingChannels()             { return 0x01040000; }
    static constexpr unsigned int roamingZoneBitmap()           { return 0x01042080; }
    static constexpr unsigned int roamingZones()                { return 0x01043000; }
    static constexpr unsigned int aesKeys()                     { return 0x024C4000; }
    static constexpr unsigned int aesKeyBitmap()                { return 0x024C8000; }
    static constexpr unsigned int arc4Keys()                    { return 0x025C0C00; }
    static constexpr unsigned int arc4KeyBitmap()               { return 0x025C1C00; }
    /// @endcond
  };
};

#endif // D878UVCODEPLUG_HH
