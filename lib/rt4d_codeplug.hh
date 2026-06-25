//
// Created by hannes on 23.06.26.
//

#ifndef LIBDMRCONF_RT4D_CODEPLUG_HH
#define LIBDMRCONF_RT4D_CODEPLUG_HH

#include "codeplug.hh"
#include "frequency.hh"
#include "interval.hh"
#include "level.hh"
#include "smsextension.hh"


/** Implements the codeplug for the Radtel RT4D.
 *
 * This is a relatively compact, sparse but fixed-size codeplug. See
 * https://dmr-tools.github.io/codeplugs for details.
 *
 * @ingroup rt4d */
class RT4DCodeplug: public Codeplug
{
protected:
  class FrequencyRangeElement: public Element
  {
  public:
    /** Possible lock types. */
    enum class LockType {
      None = 0,      ///< No restriction at all.
      RxOnly = 1,    ///< RX only allowed.
      Forbidden = 2  ///< Not even reception.
    };

  public:
    /** Constructor form pointer. */
    explicit FrequencyRangeElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x0005; }

    /** Returns the lock type for the frequency range. */
    virtual LockType lockType() const;
    /** Sets the lock type for the frequency range. */
    virtual void setLockType(LockType lockType);

    /** Returns the lower frequency bound. */
    virtual Frequency lowerFrequency() const;
    /** Sets the lower frequency bound. */
    virtual void setLowerFrequency(const Frequency &frequency);

    /** Returns the upper frequency bound. */
    virtual Frequency upperFrequency() const;
    /** Sets the upper frequency bound. */
    virtual void setUpperFrequency(const Frequency &frequency);

  protected:
    /** Internal offsets. */
    struct Offset: Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int lockType()          { return 0x0000; }
      static constexpr unsigned int lowerFrequency()    { return 0x0001; }
      static constexpr unsigned int upperFrequency()    { return 0x0003; }
      /// @endcond
    };
  };


  /** Implements a single DTMF code. */
  class DTMFCodeElement: public Element
  {
  public:
    /** Constructor from pointer. */
    explicit DTMFCodeElement(uint8_t *ptr);

    /** Size of the element. */
    static constexpr unsigned int size() { return 0x0010; }

    bool isValid() const override;

    /** Returns the DTMF code as a string. */
    virtual QString code() const;
    /** Sets the code. */
    virtual void setCode(const QString &code);

  public:
    /** Some limits. */
    struct Limit: Element::Limit {
      static constexpr unsigned int code() { return 14; }
    };

  protected:
    /** Some internal offsets. */
    struct Offset: Element::Offset {
      /// @cond  DO_NOT_DOCUMENT
      static constexpr unsigned int code()    { return 0x0000; }
      static constexpr unsigned int padByte() { return 0x000e; }
      static constexpr unsigned int length()  { return 0x000f; }
      /// @endcond
    };
  };


  /** Implements the first section of settings. */
  class FirstSettingsElement: public Element
  {
  public:
    /** Possible power-save modes. */
    enum class PowerSaveMode {
      None = 0,
      Save_1_2 = 1,
      Save_2_3 = 2,
      Save_3_4 = 3
    };

    /** Possible talkaround modes. */
    enum class TalkaroundMode {
      Off = 0,
      Enabled = 1,
      Inverted = 2
    };

    /** Possible alarm types. */
    enum class AlarmType {
      Sound = 0,
      Transmit = 1,
      Both = 2
    };

    /** Transmit priority. */
    enum class TransmitPriority {
      Busy = 0, Edit = 1
    };

    /** Specifies the PTT modes. */
    enum class PTTMode {
      VFO_A = 0, MainChannel = 1
    };

    /** Scan modes. */
    enum class ScanMode {
      Carrier = 0, Time = 1, Stop = 2
    };

    /** Possible scan revert channels. */
    enum class ScanRevertChannel {
      Active = 0, Original = 1
    };

    /** Possible frequency precision. */
    enum class FrequencyPrecision {
      SixDigits=0, EightDigits=1
    };

    /** Possible FM EOT tones. */
    enum class FMEotTone {
      None = 0, Roger1=1, Roger2=2, MDC1200=3, GPS=4
    };

    /** Possible ranges for frequency detection. */
    enum class FrequencyDetectionRange {
      Range_18_64MHz=0, Range_64_136MHz=1, Range_136_174MHz=2, Range_174_240MHz=3,
      Range_240_320MHz=4, Range_320_400MHz=5, Range_400_480Mhz=6, Range_480_560MHz=7,
      Range_560_640MHz=8, Range_840_920MHz=9, Range_920_1000MHz=10
    };

    /** Possible FM EOT tones. */
    enum class DMREotTone {
      None = 0, Roger1=1, Roger2=2
    };

    /** Possible talker alias display modes. */
    enum class TalkerAliasDisplayMode {
      City = 0, DmrId = 1
    };

    /** Possible SMS formats. */
    enum class SMSFormat {
      Hytera = 0, Motorola=1
    };

    /** Possible SMS encoding. */
    enum class SMSEncoding {
      Unicode = 0, GBK=1
    };

    /** DTMF Transmission mode. */
    enum class DTMFTransmissionMode
    {
      Off=0, BOT=1, EOT=2, Both=3
    };

  public:
    /** Constructor from pointer. */
    explicit FirstSettingsElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x0400; }

    /** Returns @c true if the boot image is enabled. */
    virtual bool bootImageEnabled() const;
    /** Enables the boot image. */
    virtual void enableBootImage(bool enable);

    /** Returns @c true if the boot tone is enabled. */
    virtual bool bootToneEnabled() const;
    /** Enables the boot tone. */
    virtual void enableBootTone(bool enable);

    /** Returns @c true if the boot text is enabled. */
    virtual bool bootTextEnabled() const;
    /** Enables the boot text. */
    virtual void enableBootText(bool enable);

    /** Returns the line of the boot text. */
    virtual unsigned int bootTextLine() const;
    /** Sets the line of the boot text. */
    virtual void setBootTextLine(unsigned int line);

    /** Returns the column of the boot text. */
    virtual unsigned int bootTextColumn() const;
    /** Sets the column of the boot text. */
    virtual void setBootTextColumn(unsigned int column);

    /** Returns @c true if the boot password is enabled. */
    virtual bool bootPasswordEnabled() const;
    /** Enables the boot password. */
    virtual void enableBootPassword(bool enable);
    /** Returns the boot password. */
    virtual QString bootPassword() const;
    /** Sets the boot password. */
    virtual void setBootPassword(const QString &password);

    /** Returns the boot text. */
    virtual QString bootText() const;
    /** Sets the boot text. */
    virtual void setBootText(const QString &text);

    /** Returns the radio name. */
    virtual QString radioName() const;
    /** Sets the radio name. */
    virtual void setRadioName(const QString &name);

    /** Returns @c true if voice prompt is enabled. */
    virtual bool voicePromptEnabled() const;
    /** Enables voice prompt. */
    virtual void enableVoicePrompt(bool enable);

    /** Returns @c true if key tone is enabled. */
    virtual bool keyToneEnabled() const;
    /** Enables key tone. */
    virtual void enableKeyTone(bool enable);

    /** Returns the key-lock delay. */
    Interval keyLockDelay() const;
    /** Sets key lock delay. */
    virtual void setKeyLockDelay(const Interval &delay);

    /** Returns the display brightness [1-10]. */
    virtual Level backlightBrightness() const;
    /** Sets the display brightness [1-10]. */
    virtual void setBacklightBrightness(const Level &brightness);

    /** Returns the backlight duration. */
    virtual Interval backlightDuration() const;
    /** Sets the backlight duration. */
    virtual void setBacklightDuration(const Interval &duration);

    /** Returns the power-save mode. */
    PowerSaveMode powerSaveMode() const;
    /** Sets the power-save mode. */
    virtual void setPowerSaveMode(PowerSaveMode mode);

    /** Returns the power-save delay. */
    virtual Interval powerSaveDelay() const;
    /** Sets the power-save delay. */
    virtual void setPowerSaveDelay(const Interval &delay);

    /** Returns the menu duration. */
    virtual Interval menuDuration() const;
    /** Sets the menu duration. */
    virtual void setMenuDuration(const Interval &duration);

    /** Returns the talkaround mode. */
    virtual TalkaroundMode talkaroundMode() const;
    /** Sets the talkaround mode. */
    virtual void setTalkaroundMode(TalkaroundMode mode);

    /** Returns the alarm type. */
    virtual AlarmType alarmType() const;
    /** Sets the alarm type. */
    virtual void setAlarmType(AlarmType type);

    /** Returns the auto-power-off delay. */
    virtual Interval autoPowerOffDelay() const;
    /** Sets the auto-power-off delay. */
    virtual void setAutoPowerOffDelay(const Interval &delay);

    /** Returns the TX priority. */
    virtual TransmitPriority txPriority() const;
    /** Sets the TX priority. */
    virtual void setTxPriority(TransmitPriority priority);

    /** Returns the PTT mode. */
    virtual PTTMode pttMode() const;
    /** Sets the PTT Mode. */
    virtual void setPttMode(PTTMode mode);

    /** Returns the n-th frequency range. */
    virtual FrequencyRangeElement frequencyRange(unsigned int n) const;

    /** Returns the scan mode. */
    virtual ScanMode scanMode() const;
    /** Sets the scan mode. */
    virtual void setScanMode(ScanMode mode);

    /** Returns the scan revert channel. */
    virtual ScanRevertChannel scanRevertChannel() const;
    /** Sets the scan revert channel. */
    virtual void setScanRevertChannel(ScanRevertChannel channel);

    /** Returns the scan dwell time. */
    virtual Interval scanDwellDuration() const;
    /** Sets the scan dwell time. */
    virtual void setScanDwellDuration(const Interval &duration);

    /** Returns the scan interval. */
    virtual Interval scanInterval() const;
    /** Sets the scan interval. */
    virtual void setScanInterval(const Interval &interval);

    /** Returns the RSSI update period. */
    virtual Interval rssiUpdatePeriod() const;
    /** Sets the RSSI update period. */
    virtual void setRssiUpdatePeriod(const Interval &period);

    /** Returns the display contrast. */
    virtual Level displayContrast() const;
    /** Sets the display contrast. */
    virtual void setDisplayContrast(const Level &contrast);

    /** Returns the frequency precision. */
    virtual FrequencyPrecision frequencyPrecision() const;
    /** Sets the frequency precision. */
    virtual void setFrequencyPrecision(FrequencyPrecision precision);

    /** Returns the TBST frequency. */
    virtual Frequency tbstFrequency() const;
    /** Sets the TBST frequency. */
    virtual void setTbstFrequency(const Frequency &frequency);

    /** Returns the FM squelch level. */
    virtual Level fmSquelch() const;
    /** Sets the FM squelch level. */
    virtual void setFmSquelch(const Level &squelch);

    /** Returns the FM mic gain. */
    virtual Level fmMicGain() const;
    /** Sets the FM mic gain. */
    virtual void setFmMicGain(const Level &gain);

    /** Returns the FM speaker gain. */
    virtual Level fmSpeakerGain() const;
    /** Sets the FM speaker gain. */
    virtual void setFmSpeakerGain(const Level &gain);

    /** Returns @c true if the FM BOT tone is enabled. */
    virtual bool fmBotToneEnabled() const;
    /** Enable the FM BOT tone. */
    virtual void enableFmBotTone(bool enable);

    /** Returns the FM EOT tone. */
    virtual FMEotTone fmEotTone() const;
    /** Sets the FM EOT tone. */
    virtual void setFMEotTone(FMEotTone tone);

    /** Returns the frequency detection range. */
    virtual FrequencyDetectionRange frequencyDetectionRange() const;
    /** Sets the frequency detection range. */
    virtual void setFrequencyDetectionRange(FrequencyDetectionRange range);

    /** Returns the detection period. */
    virtual Interval frequencyDetectionPeriod() const;
    /** Sets the frequency detection period. */
    virtual void setFrequencyDetectionPeriod(const Interval &period);

    /** Returns the FM glitch threshold. */
    virtual Level fmGlitchThreshold() const;
    /** Sets the FM glitch threshold. */
    virtual void setFmGlitchThreshold(const Level &threshold);

    /** Returns the TBST duration. */
    virtual Interval tbstDuration() const;
    /** Sets the TBST duration. */
    virtual void setTbstDuration(const Interval &duration);

    /** Returns the radios DMR id. */
    virtual unsigned int radioDMRId() const;
    /** Sets the radios DMR id. */
    virtual void setRadioDMRId(unsigned int id);

    /** Returns @c true if the DMR remote control is enabled. */
    virtual bool dmrRemoteControlEnabled() const;
    /** Enables the DMR remote control. */
    virtual void enableDmrRemoteControl(bool enable);

    /** Returns the DMR TX noise reduction. */
    virtual Level dmrTxNoiseReduction() const;
    /** Sets the DMR TX noise reduction. */
    virtual void setDmrTxNoiseReduction(const Level &noiseReduction);
    /** Returns the DMR RX noise reduction. */
    virtual Level dmrRxNoiseReduction() const;
    /** Sets the DMR RX noise reduction. */
    virtual void setDmrRxNoiseReduction(const Level &noiseReduction);

    /** Returns the DMR mic level. */
    virtual Level dmrMicGain() const;
    /** Sets the DMR mig gain. */
    virtual void setDmrMicGain(const Level &gain);
    /** Returns the DMR speaker level. */
    virtual Level dmrSpeakerGain() const;
    /** Sets the DMR speaker level. */
    virtual void setDmrSpeakerGain(const Level &gain);

    /** Returns the DMR EOT tone. */
    virtual DMREotTone dmrEotTone() const;
    /** Sets the DMR EOT tone. */
    virtual void setDmrEotTone(DMREotTone tone);

    /** Returns the group call hang time. */
    virtual Interval groupCallHangTime() const;
    /** Sets the group call hang time. */
    virtual void setGroupCallHangTime(const Interval &time);
    /** Returns the private call hang time. */
    virtual Interval privateCallHangTime() const;
    /** Sets the private call hang time. */
    virtual void setPrivateCallHangTime(const Interval &time);

    /** Returns the DMR squelch level. */
    virtual Level dmrSquelch() const;
    /** Sets the DMR squelch level. */
    virtual void setDmrSquelch(const Level &squelch);

    /** Returns talkeralias display mode. */
    virtual TalkerAliasDisplayMode talkerAliasDisplayMode() const;
    /** Sets the talkeralias display mode. */
    virtual void setTalkerAliasDisplayMode(TalkerAliasDisplayMode mode);

    /** Returns the SMS format. */
    virtual SMSExtension::Format smsFormat() const;
    /** Sets the SMS format. */
    virtual void setSmsFormat(SMSExtension::Format format);
    /** Returns the SMS encoding. */
    virtual SMSEncoding smsEncoding() const;
    /** Sets the SMS encoding. */
    virtual void setSmsEncoding(SMSEncoding encoding);

    /** Returns the DTMF transmit delay. */
    virtual Interval dtmfTransmitDelay() const;
    /** Sets the DTMF transmit delay. */
    virtual void setDtmfTransmitDelay(const Interval &delay);
    /** Returns the DTMF tone duration. */
    virtual Interval dtmfToneDuration() const;
    /** Sets the DTMF tone duration. */
    virtual void setDtmfToneDuration(const Interval &duration);
    /** Returns the DTMF interval. */
    virtual Interval dtmfInterval() const;
    /** Sets the DTMF interval. */
    virtual void setDtmfInterval(const Interval &interval);
    /** Returns the DTMF transmit mode. */
    virtual DTMFTransmissionMode dtmfTransmissionMode() const;
    /** Sets the DTMF transmit mode. */
    virtual void setDtmfTransmissionMode(DTMFTransmissionMode mode);
    /** Returns the DTMF code index. */
    unsigned int dtmfCodeIndex() const;
    /** Sets the DTMF code index. */
    virtual void setDtmfCodeIndex(unsigned int index);
    /** Returns @c true if the received DTMF code is shown. */
    virtual bool displayRxDtmfCodeEnabled() const;
    /** Enables showing the received DTMF codes. */
    virtual void enableDisplayRxDtmfCode(bool enable);
    /** Returns DTMF transmit gain. */
    virtual Level dtmfTxGain() const;
    /** Sets the DTMF transmit gain. */
    virtual void setDtmfTxGain(const Level &gain);
    /** Returns the DTMF decode threshold. */
    virtual Level dtmfDecodeThreshold() const;
    /** Sets the DTMF decode threshold. */
    virtual void setDtmfDecodeThreshold(const Level &threshold);
    /** Returns @c true if the DTMF remote control is enabled. */
    virtual bool dtmfRemoteControlEnabled() const;
    /** Enables DTMF remote control. */
    virtual void enableDtmfRemoteControl(bool enable);
    /** Returns the n-th DTMF code. */
    virtual DTMFCodeElement dtmfCode(unsigned int index) const;
    /** Returns the DTMF remote stun code. */
    virtual DTMFCodeElement dtmfRemoteStunCode() const;
    /** Returns the DTMF remote kill code. */
    virtual DTMFCodeElement dtmfRemoteKillCode() const;
    /** Returns the DTMF remote wake code. */
    virtual DTMFCodeElement dtmfRemoteWakeCode() const;
    /** Returns the DTMF remote monitor code. */
    virtual DTMFCodeElement dtmfRemoteMonitorCode() const;

    /** Returns @c true if the RX and TX frequencies are swapped. */
    virtual bool swapFrequenciesEnabled() const;
    /** Enables swapping of RX and TX frequencies. */
    virtual void enableSwapFrequencies(bool enable);

    /** Returns @c true if the SMS notification is enabled. */
    virtual bool smsNotificationEnabled() const;
    /** Enables SMS notification. */
    virtual void enableSmsNotification(bool enable);

    /** Returns the lower scan frequency. */
    virtual Frequency lowerScanFrequency() const;
    /** Sets the lower scan frequency.*/
    virtual void setLowerScanFrequency(const Frequency &frequency);
    /** Returns the upper scan frequency. */
    virtual Frequency upperScanFrequency() const;
    /** Sets the upper scan frequency.*/
    virtual void setUpperScanFrequency(const Frequency &frequency);

    /** Returns @c true if the LED is enabled. */
    virtual bool ledEnabled() const;
    /** Enables the LED. */
    virtual void enableLed(bool enable);

    /** Encodes the settings element from the given config. */
    virtual bool encode(Context &ctx, const ErrorStack &err);
    /** Decodes the settings element and updates the given config. */
    virtual bool decode(Context &ctx, const ErrorStack &err);

  public:
    /** Some limits for the element. */
    struct Limit: Element::Limit {
      static constexpr unsigned int bootPasswordLength() { return 16; }
      static constexpr unsigned int bootTextLength() { return 32; }
      static constexpr unsigned int radioNameLength() { return 16; }
      static constexpr Range<unsigned int> backlightBrightness() { return {0,3}; }
      static constexpr Range<Interval> powerSaveDelay() { return { Interval::fromSeconds(0), Interval::fromSeconds(200)}; }
      static constexpr unsigned int frequencyRanges() { return 4; }
      static constexpr unsigned int dtmfCodes() { return 16; }
    };

  protected:
    /** Internal offsets within element. */
    struct Offset: Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int enableBootImage()             { return 0x0010; }
      static constexpr unsigned int enableBootTone()              { return 0x0013; }
      static constexpr unsigned int enableBootText()              { return 0x0014; }
      static constexpr unsigned int bootTextLine()                { return 0x0017; }
      static constexpr unsigned int bootTextColumn()              { return 0x0019; }
      static constexpr unsigned int enableBootPassword()          { return 0x001b; }
      static constexpr unsigned int bootPassword()                { return 0x001c; }
      static constexpr unsigned int bootText()                    { return 0x002c; }
      static constexpr unsigned int radioName()                   { return 0x004c; }
      static constexpr unsigned int enableVoicePrompt()           { return 0x005c; }
      static constexpr unsigned int enableKeyTone()               { return 0x005d; }
      static constexpr unsigned int keyLockDelay()                { return 0x005f; }
      static constexpr unsigned int enableBacklight()             { return 0x0060; }
      static constexpr unsigned int backlightBrightness()         { return 0x0061; }
      static constexpr unsigned int backlightDuration()           { return 0x0062; }
      static constexpr unsigned int powerSaveMode()               { return 0x0063; }
      static constexpr unsigned int powerSaveDelay()              { return 0x0064; }
      static constexpr unsigned int menuDuration()                { return 0x0065; }
      static constexpr unsigned int talkaround()                  { return 0x0067; }
      static constexpr unsigned int alarmType()                   { return 0x0068; }
      static constexpr unsigned int enableAutoPowerOff()          { return 0x0069; }
      static constexpr unsigned int autoPowerOffDelay()           { return 0x006a; }
      static constexpr unsigned int txPriority()                  { return 0x007e; }
      static constexpr unsigned int pttMode()                     { return 0x007f; }
      static constexpr unsigned int frequencyRanges()             { return 0x008e; }
      static constexpr unsigned int scanMode()                    { return 0x00a3; }
      static constexpr unsigned int scanRevertChannel()           { return 0x00a4; }
      static constexpr unsigned int scanDwellTime()               { return 0x00a5; }
      static constexpr unsigned int scanInterval()                { return 0x00a6; }
      static constexpr unsigned int rssiUpdatePeriod()            { return 0x00a9; }
      static constexpr unsigned int displayContrast()             { return 0x00e9; }
      static constexpr unsigned int frequencyPrecision()          { return 0x00ea; }
      static constexpr unsigned int tbstFrequency()               { return 0x0100; }
      static constexpr unsigned int fmSquelchLevel()              { return 0x0102; }
      static constexpr unsigned int fmMicGain()                   { return 0x0105; }
      static constexpr unsigned int fmSpeakerGain()               { return 0x0106; }
      static constexpr unsigned int enableFmBotTone()             { return 0x010b; }
      static constexpr unsigned int fmEotTone()                   { return 0x010c; }
      static constexpr unsigned int frequencyDetectionRange()     { return 0x0110; }
      static constexpr unsigned int detectionPeriod()             { return 0x0111; }
      static constexpr unsigned int fmGlitchThreshold()           { return 0x0114; }
      static constexpr unsigned int tbstDuration()                { return 0x0115; }
      static constexpr unsigned int radioDMRId()                  { return 0x0180; }
      static constexpr unsigned int enableDMRRemoteControl()      { return 0x0184; }
      static constexpr unsigned int dmrTxNoiseReduction()         { return 0x0185; }
      static constexpr unsigned int dmrRxNoiseReduction()         { return 0x0186; }
      static constexpr unsigned int dmrMicGain()                  { return 0x0187; }
      static constexpr unsigned int dmrSpeakerGain()              { return 0x0188; }
      static constexpr unsigned int dmrEotTone()                  { return 0x018d; }
      static constexpr unsigned int groupCallHangTime()           { return 0x018f; }
      static constexpr unsigned int privateCallHangTime()         { return 0x0191; }
      static constexpr unsigned int dmrSquelchLevel()             { return 0x0193; }
      static constexpr unsigned int talkerAliasDisplayMode()      { return 0x0194; }
      static constexpr unsigned int smsFormat()                   { return 0x0196; }
      static constexpr unsigned int smsEncoding()                 { return 0x0197; }
      static constexpr unsigned int talkerAliasDisplayDuration()  { return 0x0198; }
      static constexpr unsigned int dtmfTransmitDelay()           { return 0x0200; }
      static constexpr unsigned int dtmfToneDuration()            { return 0x0201; }
      static constexpr unsigned int dtmfInterval()                { return 0x0202; }
      static constexpr unsigned int dtmfTransmitMode()            { return 0x0203; }
      static constexpr unsigned int dtmfCodeIndex()               { return 0x0204; }
      static constexpr unsigned int enableDtmfRxDisplay()         { return 0x0205; }
      static constexpr unsigned int dtmfTransmitGain()            { return 0x0206; }
      static constexpr unsigned int dtmfDecodeThreshold()         { return 0x0207; }
      static constexpr unsigned int enableDtmfRemoteControl()     { return 0x0208; }
      static constexpr unsigned int dtmfCodes()                   { return 0x0200; }
      static constexpr unsigned int dtmfRemoteStun()              { return 0x030a; }
      static constexpr unsigned int dtmfRemoteKill()              { return 0x031a; }
      static constexpr unsigned int dtmfRemoteWake()              { return 0x032a; }
      static constexpr unsigned int dtmfRemoteMonitor()           { return 0x033a; }
      static constexpr unsigned int swapFrequencies()             { return 0x034a; }
      static constexpr unsigned int smsNotification()             { return 0x034b; }
      static constexpr unsigned int lowerScanFrequency()          { return 0x034c; }
      static constexpr unsigned int upperScanFrequency()          { return 0x0350; }
      static constexpr unsigned int enableLed()                   { return 0x0354; }
      /// @endcond
    };
  };


  class ChannelBankElement: public Element
  {
  public:
    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0xc000; }
  };


  class SecondSettingsElement: public Element
  {
  public:
    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x0400; }
  };

  class ZoneBankElement: public Element
  {
  public:
    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x20000; }
  };

  class ContactBankElement: public Element
  {
  public:
    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x34000; }
  };

  class GroupListBankElement: public Element
  {
  public:
    static constexpr unsigned int size() { return 0x5000; }
  };

  class EncryptionKeyBankElement: public Element
  {
  public:
    static constexpr unsigned int size() { return 0x3000; }
  };

  class MessageBankElement: public Element
  {
  public:
    static constexpr unsigned int size() { return 0x1000; }
  };

  class FMBroadcastChannelBankElement: public Element
  {
  public:
    static constexpr unsigned int size() { return 0x1000; }
  };

public:
  /** Default constructor, also pre-allocates the entire codeplug. */
  explicit RT4DCodeplug(QObject *parent = nullptr);

  bool index(Config *config, Context &ctx, const ErrorStack &err) const override;
  bool encode(Config *config, const Flags &flags, const ErrorStack &err) override;

  bool decode(Config *config, const ErrorStack &err) override;

protected:
  /** Some internal offsets. */
  struct Offset {
    /// @cond DO_NOT_DOCUMENT
    static constexpr unsigned int firstSettings()     { return 0x002000; }
    static constexpr unsigned int channels()          { return 0x004000; }
    static constexpr unsigned int secondSettings()    { return 0x01c000; }
    static constexpr unsigned int zones()             { return 0x01e000; }
    static constexpr unsigned int contacts()          { return 0x05e000; }
    static constexpr unsigned int groupLists()        { return 0x0c6000; }
    static constexpr unsigned int keys()              { return 0x0d0000; }
    static constexpr unsigned int messages()          { return 0x0d6000; }
    static constexpr unsigned int fmBroadcast()       { return 0x0f0000; }
    /// @endcond
  };
};

#endif //LIBDMRCONF_RT4D_CODEPLUG_HH
