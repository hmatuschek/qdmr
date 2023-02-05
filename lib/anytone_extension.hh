#ifndef ANYTONEEXTENSION_HH
#define ANYTONEEXTENSION_HH

#include "configobject.hh"
#include "configreference.hh"

#include <QTimeZone>


/** Implements the common properties for analog and digital AnyTone channels.
 * This class cannot be instantiated directly, use one of the derived classes.
 * @ingroup anytone */
class AnytoneChannelExtension: public ConfigExtension
{
  Q_OBJECT

  /** If @c true, talkaround is enabled. */
  Q_PROPERTY(bool talkaround READ talkaround WRITE enableTalkaround)
  /** Holds the frequency correction in some unknown units. */
  Q_PROPERTY(int frequencyCorrection READ frequencyCorrection WRITE setFrequencyCorrection)
  /** If @c true, the hands-free featrue is enabled for this channel. */
  Q_PROPERTY(bool handsFree READ handsFree WRITE enableHandsFree)

protected:
  /** Hidden constructor. */
  explicit AnytoneChannelExtension(QObject *parent=nullptr);

public:
  /** Returns @c true, if talkaround is enabled. */
  bool talkaround() const;
  /** Enables/disables talkaround. */
  void enableTalkaround(bool enable);

  /** Returns the frequency correction in some unknown units. */
  int frequencyCorrection() const;
  /** Sets the frequency correction. */
  void setFrequencyCorrection(int corr);

  /** Returns @c true if the hands-free feature is enabled. */
  bool handsFree() const;
  /** Enables/disables the hands-free feature for this channel. */
  void enableHandsFree(bool enable);


protected:
  /** If @c true, talkaround is enabled. */
  bool _talkaround;
  /** The frequency correction. */
  int _frequencyCorrection;
  /** If @c true, the hands-free featrue is enabled for this channel. */
  bool _handsFree;
};


/** Implements the settings extension for FM channels on AnyTone devices.
 * @ingroup anytone */
class AnytoneFMChannelExtension: public AnytoneChannelExtension
{
  Q_OBJECT

  /** If @c true, the CTCSS phase-reverse burst at the end of transmission is enabled. */
  Q_PROPERTY(bool reverseBurst READ reverseBurst WRITE enableReverseBurst)
  /** If @c true, the custom CTCSS tone is used for RX (open squelch). */
  Q_PROPERTY(bool rxCustomCTCSS READ rxCustomCTCSS WRITE enableRXCustomCTCSS)
  /** If @c true, the custom CTCSS tone is transmitted. */
  Q_PROPERTY(bool txCustomCTCSS READ txCustomCTCSS WRITE enableTXCustomCTCSS)
  /** Holds the custom CTCSS tone frequency in Hz. Resolution is 0.1Hz */
  Q_PROPERTY(double customCTCSS READ customCTCSS WRITE setCustomCTCSS)
  /** Holds the squelch mode. */
  Q_PROPERTY(SquelchMode squelchMode READ squelchMode WRITE setSquelchMode)
  /** If @c true, the analog scrabler is enabled. */
  Q_PROPERTY(bool scrambler READ scrambler WRITE enableScrambler)

public:
  /** Possible squelch mode settings. */
  enum class SquelchMode {
    Carrier = 0,
    SubTone = 1,
    OptSig  = 2,
    SubToneAndOptSig = 3,
    SubToneOrOptSig = 4
  };
  Q_ENUM(SquelchMode)

public:
  /** Default constructor. */
  Q_INVOKABLE explicit AnytoneFMChannelExtension(QObject *parent=nullptr);

  ConfigItem *clone() const;

  /** Returns @c true, if the CTCSS phase-reverse burst is enabled. */
  bool reverseBurst() const;
  /** Enables/disables the CTCSS phase-reverse burst. */
  void enableReverseBurst(bool enable);

  /** Returns @c true, if the custom CTCSS frequency is used for RX (open squelch). */
  bool rxCustomCTCSS() const;
  /** Enables/disables usage of custom CTCSS frequency for RX. */
  void enableRXCustomCTCSS(bool enable);
  /** Returns @c true, if the custom CTCSS frequency is used for TX (open squelch). */
  bool txCustomCTCSS() const;
  /** Enables/disables usage of custom CTCSS frequency for TX. */
  void enableTXCustomCTCSS(bool enable);
  /** Returns the custom CTCSS frequency in Hz. Resolution is 0.1Hz. */
  double customCTCSS() const;
  /** Sets the custom CTCSS frequency in Hz. Resolution is 0.1Hz. */
  void setCustomCTCSS(double freq);

  /** Returns the squelch mode. */
  SquelchMode squelchMode() const;
  /** Sets the squelch mode. */
  void setSquelchMode(SquelchMode mode);

  /** Reuturns @c true, if the analog scrambler is enabled. */
  bool scrambler() const;
  /** Enables/disables the analog scrambler. */
  void enableScrambler(bool enable);

protected:
  /** If @c true, the CTCSS phase-reverse burst at the end of transmission is enabled. */
  bool _reverseBurst;
  /** If @c true, the custom CTCSS tone is used for RX (open squelch). */
  bool _rxCustomCTCSS;
  /** If @c true, the custom CTCSS tone is transmitted. */
  bool _txCustomCTCSS;
  /** Holds the custom CTCSS tone frequency in Hz. Resolution is 0.1Hz */
  double _customCTCSS;
  /** Holds the squelch mode. */
  SquelchMode _squelchMode;
  /** If @c true, the analog scrambler is enabled for this channel. */
  bool _scrambler;
};


/** Implements the settings extension for DMR channels on AnyTone devices.
 * @ingroup anytone */
class AnytoneDMRChannelExtension: public AnytoneChannelExtension
{
  Q_OBJECT

  /** If @c true, the call confirmation is enabled. */
  Q_PROPERTY(bool callConfirm READ callConfirm WRITE enableCallConfirm)
  /** If @c true, SMS reception is enabled. */
  Q_PROPERTY(bool sms READ sms WRITE enableSMS)
  /** If @c true, the SMS confirmation is enabled. */
  Q_PROPERTY(bool smsConfirm READ smsConfirm WRITE enableSMSConfirm)
  /** If @c true, the radio will response to received data packages. Should be enabled. */
  Q_PROPERTY(bool dataACK READ dataACK WRITE enableDataACK)
  /** If @c true, the simplex TDMA mode is enabled (aka DCDM). */
  Q_PROPERTY(bool simplexTDMA READ simplexTDMA WRITE enableSimplexTDMA)
  /** If @c true, the adaptive TDMA mode is enabled. This makes only sense, if @c simplexTDMA is
   * enabled too. In this case, the radio is able to receive both simplex TDMA as well as "normal"
   * simplex DMR on the channel. */
  Q_PROPERTY(bool adaptiveTDMA READ adaptiveTDMA WRITE enableAdaptiveTDMA)
  /** If @c true, the lone-worker feature is enabled for this channel. */
  Q_PROPERTY(bool loneWorker READ loneWorker WRITE enableLoneWorker)
  /** If @c true, the through mode is enabled (what ever that means). */
  Q_PROPERTY(bool throughMode READ throughMode WRITE enableThroughMode)

public:
  /** Default constructor. */
  Q_INVOKABLE explicit AnytoneDMRChannelExtension(QObject *parent=nullptr);

  ConfigItem *clone() const;

  /** Returns @c true if the call confirmation is enabled. */
  bool callConfirm() const;
  /** Enables/disables the call confirmation. */
  void enableCallConfirm(bool enabled);
  /** Returns @c true if SMS reception is enabled. */
  bool sms() const;
  /** Enables/disables SMS reception. */
  void enableSMS(bool enable);
  /** Returns @c true if the SMS confirmation is enabled. */
  bool smsConfirm() const;
  /** Enables/disables the SMS confirmation. */
  void enableSMSConfirm(bool enabled);
  /** Returns @c true if the data acknowledgement is enabled. */
  bool dataACK() const;
  /** Enables/disables the data acknowledgement. */
  void enableDataACK(bool enable);
  /** Returns @c true if the simplex TDMA (DCDM) mode is enabled. */
  bool simplexTDMA() const;
  /** Enables/disables the simplex TDMA (DCDM) mode. */
  void enableSimplexTDMA(bool enable);
  /** Returns @c true if the adaptive TDMA mode is enabled. */
  bool adaptiveTDMA() const;
  /** Enables/disables the adaptive TDMA mode. */
  void enableAdaptiveTDMA(bool enable);
  /** Returns @c true if the lone-worker feature is enabled. */
  bool loneWorker() const;
  /** Enables the lone-worker feature for this channel. */
  void enableLoneWorker(bool enable);
  /** Returns @c true if the through mode is enabled. */
  bool throughMode() const;
  /** Enables/disables the through mode. */
  void enableThroughMode(bool enable);

protected:
  /** If @c true, the call confirmation is enabled. */
  bool _callConfirm;
  /** If @c true, the SMS reception is enabled. */
  bool _sms;
  /** If @c true, the SMS confirmation is enabled. */
  bool _smsConfirm;
  /** If @c true, the data acknowledgement is enabled. */
  bool _dataACK;
  /** If @c true, the simplex TDMA mode is enabled. */
  bool _simplexTDMA;
  /** If @c true, the adaptive TDMA mode is enabled. */
  bool _adaptiveTDMA;
  /** If @c true the lone-worker feature is enabled. */
  bool _loneWorker;
  /** If @c true the through mode is enabled. */
  bool _throughMode;
};


/** Implements the AnyTone extensions for zones.
 * @ingroup anytone */
class AnytoneZoneExtension : public ConfigExtension
{
  Q_OBJECT

  /** If @c true, the zone is hidden in the menu. */
  Q_PROPERTY(bool hidden READ hidden WRITE enableHidden)

public:
  /** Default constructor. */
  Q_INVOKABLE explicit AnytoneZoneExtension(QObject *parent=nullptr);

  ConfigItem *clone() const;

  /** Returns @c true if the zone is hidden. */
  bool hidden() const;
  /** Enables/disables hidden zone. */
  void enableHidden(bool enable);

protected:
  /** If @c true, the zone is hidden in the menu. */
  bool _hidden;
};


/** Implements the AnyTone contact extension.
 * @ingroup anytone */
class AnytoneContactExtension: public ConfigExtension
{
  Q_OBJECT

  /** Overrides the ring flag, allows to set None, Ring and Online. */
  Q_PROPERTY(AlertType alertType READ alertType WRITE setAlertType)

public:
  /** Possible ring-tone types. */
  enum class AlertType {
    None = 0,                   ///< Alert disabled.
    Ring = 1,                   ///< Ring tone.
    Online = 2                  ///< WTF?
  };
  Q_ENUM(AlertType)

public:
  /** Default constructor. */
  Q_INVOKABLE explicit AnytoneContactExtension(QObject *parent=nullptr);

  ConfigItem *clone() const;

  /** Returns the alert type for the contact. */
  AlertType alertType() const;
  /** Sets the alert type for the contact. */
  void setAlertType(AlertType type);

protected:
  /** Holds the alert type for the contact. */
  AlertType _alertType;
};


/** Implements the boot settings extension of AnyTone devices.
 * This extension is part of the @c AnytoneSettingsExtension.
 *
 * @ingroup anytone */
class AnytoneBootSettingsExtension: public ConfigItem
{
  Q_OBJECT

  /** The boot display setting. */
  Q_PROPERTY(BootDisplay bootDisplay READ bootDisplay WRITE setBootDisplay)
  /** If @c true, the boot password is enabled. */
  Q_PROPERTY(bool bootPasswordEnabled READ bootPasswordEnabled WRITE enableBootPassword)
  /** Holds the boot password. */
  Q_PROPERTY(QString bootPassword READ bootPassword WRITE setBootPassword)

public:
  /** What to display during boot. */
  enum class BootDisplay {
    Default = 0, CustomText = 1, CustomImage = 2
  };
  Q_ENUM(BootDisplay)

public:
  /** Constructor. */
  explicit AnytoneBootSettingsExtension(QObject *parent=nullptr);

  ConfigItem *clone() const;

  /** Retunrs the boot display setting. */
  BootDisplay bootDisplay() const;
  /** Sets the boot display. */
  void setBootDisplay(BootDisplay mode);
  /** Returns @c true if the boot password is enabled.*/
  bool bootPasswordEnabled() const;
  /** Enables the boot password. */
  void enableBootPassword(bool enable);
  /** Returns the boot password. */
  const QString &bootPassword() const;
  /** Sets the boot password. */
  void setBootPassword(const QString &pass);

protected:
  BootDisplay _bootDisplay;        ///< The boot display property.
  bool _bootPasswordEnabled;       ///< If true, the boot password is enabled.
  QString _bootPassword;           ///< The boot password
};


/** Implements the key settings extension of AnyTone devices.
 * This extension is part of the @c AnytoneSettingsExtension.
 *
 * @ingroup anytone */
class AnytoneKeySettingsExtension: public ConfigItem
{
  Q_OBJECT

  /** Programmable function key 1, short press function. */
  Q_PROPERTY(KeyFunction progFuncKey1Short READ progFuncKey1Short WRITE setProgFuncKey1Short)
  /** Programmable function key 1, long press function. */
  Q_PROPERTY(KeyFunction progFuncKey1Long READ progFuncKey1Long WRITE setProgFuncKey1Long)
  /** Programmable function key 2, short press function. */
  Q_PROPERTY(KeyFunction progFuncKey2Short READ progFuncKey2Short WRITE setProgFuncKey2Short)
  /** Programmable function key 2, long press function. */
  Q_PROPERTY(KeyFunction progFuncKey2Long READ progFuncKey2Long WRITE setProgFuncKey2Long)
  /** Programmable function key 3, short press function. */
  Q_PROPERTY(KeyFunction progFuncKey3Short READ progFuncKey3Short WRITE setProgFuncKey3Short)
  /** Programmable function key 3, long press function. */
  Q_PROPERTY(KeyFunction progFuncKey3Long READ progFuncKey3Long WRITE setProgFuncKey3Long)
  /** Function key 1, short press function. */
  Q_PROPERTY(KeyFunction funcKey1Short READ funcKey1Short WRITE setFuncKey1Short)
  /** Programmable function key 1, long press function. */
  Q_PROPERTY(KeyFunction funcKey1Long READ funcKey1Long WRITE setFuncKey1Long)
  /** Function key 2, short press function. */
  Q_PROPERTY(KeyFunction funcKey2Short READ funcKey2Short WRITE setFuncKey2Short)
  /** Programmable function key 2, long press function. */
  Q_PROPERTY(KeyFunction funcKey2Long READ funcKey2Long WRITE setFuncKey2Long)
  /** The long press duration in ms. */
  Q_PROPERTY(unsigned int longPressDuration READ longPressDuration WRITE setLongPressDuration)
  /** The auto key-lock property. */
  Q_PROPERTY(bool autoKeyLock READ autoKeyLockEnabled WRITE enableAutoKeyLock)

public:
  /** All possible key functions. */
  enum class KeyFunction {
    Off = 0x00, Voltage = 0x01, Power = 0x02, Repeater = 0x03, Reverse = 0x04,
    DigitalEncryption = 0x05, Call = 0x06, VOX = 0x07, VFOChannel = 0x08, SubPTT = 0x09,
    Scan = 0x0a, FM = 0x0b, Alarm = 0x0c, RecordSwitch = 0x0d, Record = 0x0e, SMS = 0x0f,
    Dial = 0x10, GPSInformation = 0x11, Monitor = 0x12, MainChannelSwitch = 0x13, HotKey1 = 0x14,
    HotKey2 = 0x15, HotKey3 = 0x16, HotKey4 = 0x17, HotKey5 = 0x18, HotKey6 = 0x19,
    WorkAlone = 0x1a, NuisanceDelete = 0x1b, DigitalMonitor = 0x1c, SubChannelSwitch = 0x1d,
    PriorityZone = 0x1e, VFOScan = 0x1f, MICSoundQuality = 0x20, LastCallReply = 0x21,
    ChannelTypeSwitch = 0x22, Ranging = 0x23, Roaming = 0x24, ChannelRanging = 0x25,
    MaxVolume = 0x26, SlotSwitch = 0x27, APRSTypeSwitch = 0x28, ZoneSelect = 0x29,
    TimedRoamingSet = 0x2a, APRSSet = 0x2b, MuteTimeing = 0x2c, CtcssDcsSet = 0x2d,
    TBSTSend = 0x2e, Bluetooth = 0x2f, GPS = 0x30, ChannelName = 0x31, CDTScan = 0x32,
    APRSSend = 0x33, APRSInfo = 0x34, GPSRoaming = 0x35
  };
  Q_ENUM(KeyFunction)

public:
  /** Empty constructor. */
  explicit AnytoneKeySettingsExtension(QObject *parent=nullptr);

  ConfigItem *clone() const;

  /** Returns the key function for a short press on the programmable function key 1. */
  KeyFunction progFuncKey1Short() const;
  /** Sets the key function for a short press on the programmable function key 1. */
  void setProgFuncKey1Short(KeyFunction func);
  /** Returns the key function for a long press on the programmable function key 1. */
  KeyFunction progFuncKey1Long() const;
  /** Sets the key function for a long press on the programmable function key 1. */
  void setProgFuncKey1Long(KeyFunction func);

  /** Returns the key function for a short press on the programmable function key 2. */
  KeyFunction progFuncKey2Short() const;
  /** Sets the key function for a short press on the programmable function key 2. */
  void setProgFuncKey2Short(KeyFunction func);
  /** Returns the key function for a long press on the programmable function key 2. */
  KeyFunction progFuncKey2Long() const;
  /** Sets the key function for a long press on the programmable function key 2. */
  void setProgFuncKey2Long(KeyFunction func);

  /** Returns the key function for a short press on the programmable function key 3. */
  KeyFunction progFuncKey3Short() const;
  /** Sets the key function for a short press on the programmable function key 3. */
  void setProgFuncKey3Short(KeyFunction func);
  /** Returns the key function for a long press on the programmable function key 3. */
  KeyFunction progFuncKey3Long() const;
  /** Sets the key function for a long press on the programmable function key 3. */
  void setProgFuncKey3Long(KeyFunction func);

  /** Returns the key function for a short press on the function key 1. */
  KeyFunction funcKey1Short() const;
  /** Sets the key function for a short press on the function key 1. */
  void setFuncKey1Short(KeyFunction func);
  /** Returns the key function for a long press on the function key 1. */
  KeyFunction funcKey1Long() const;
  /** Sets the key function for a long press on the function key 1. */
  void setFuncKey1Long(KeyFunction func);

  /** Returns the key function for a short press on the function key 2. */
  KeyFunction funcKey2Short() const;
  /** Sets the key function for a short press on the function key 2. */
  void setFuncKey2Short(KeyFunction func);
  /** Returns the key function for a long press on the function key 2. */
  KeyFunction funcKey2Long() const;
  /** Sets the key function for a long press on the function key 2. */
  void setFuncKey2Long(KeyFunction func);

  /** Returns the long-press duration in ms. */
  unsigned int longPressDuration() const;
  /** Sets the long-press duration in ms. */
  void setLongPressDuration(unsigned int ms);

  /** Retruns @c true, if the automatic key-lock feature is enabled. */
  bool autoKeyLockEnabled() const;
  /** Enables/disables auto key-lock. */
  void enableAutoKeyLock(bool enabled);

protected:
  KeyFunction _progFuncKey1Short;          ///< Function of the programmable key 1, short press.
  KeyFunction _progFuncKey1Long;           ///< Function of the programmable key 1, long press.
  KeyFunction _progFuncKey2Short;          ///< Function of the programmable key 2, short press.
  KeyFunction _progFuncKey2Long;           ///< Function of the programmable key 2, long press.
  KeyFunction _progFuncKey3Short;          ///< Function of the programmable key 3, short press.
  KeyFunction _progFuncKey3Long;           ///< Function of the programmable key 3, long press.
  KeyFunction _funcKey1Short;              ///< Function of the function key 1, short press.
  KeyFunction _funcKey1Long;               ///< Function of the function key 1, long press.
  KeyFunction _funcKey2Short;              ///< Function of the function key 2, short press.
  KeyFunction _funcKey2Long;               ///< Function of the function key 2, long press.
  unsigned int _longPressDuration;         ///< The long-press duration in ms.
  bool _autoKeyLock;               ///< Auto key-lock property.
};


/** Implements the tone settings extension of AnyTone devices.
 * This extension is part of the @c AnytoneSettingsExtension.
 *
 * @ingroup anytone */
class AnytoneToneSettingsExtension: public ConfigItem
{
  Q_OBJECT

  /** The key tone setting. */
  Q_PROPERTY(bool keyTone READ keyToneEnabled WRITE enableKeyTone)
  /** If @c true, the SMS alert tone is enabled. */
  Q_PROPERTY(bool smsAlert READ smsAlertEnabled WRITE enableSMSAlert)
  /** If @c true, the call alert tone is enabled. */
  Q_PROPERTY(bool callAlert READ callAlertEnabled WRITE enableCallAlert)
  /** If @c true, the DMR talk permit tone is enabled. */
  Q_PROPERTY(bool dmrTalkPermit READ talkPermitDigitalEnabled WRITE enableTalkPermitDigital)
  /** If @c true, the FM talk permit tone is enabled. */
  Q_PROPERTY(bool fmTalkPermit READ talkPermitAnalogEnabled WRITE enableTalkPermitAnalog)
  /** If @c true, the DMR reset tone is enabled. */
  Q_PROPERTY(bool dmrReset READ digitalResetToneEnabled WRITE enableDigitalResetTone)
  /** If @c true, the idle tone is enabled. */
  Q_PROPERTY(bool idle READ idleChannelToneEnabled WRITE enableIdleChannelTone)
  /** If @c true, the startup tone is enabled. */
  Q_PROPERTY(bool startup READ startupToneEnabled WRITE enableStartupTone)

public:
  /** Empty constructor. */
  explicit AnytoneToneSettingsExtension(QObject *parent=nullptr);

  ConfigItem *clone() const;

  /** Returns @c true if the key tone is enabled. */
  bool keyToneEnabled() const;
  /** Enables/disables the key tone. */
  void enableKeyTone(bool enable);

  /** Returns @c true if SMS alert is enabled. */
  bool smsAlertEnabled() const;
  /** Enables/disables SMS alert. */
  void enableSMSAlert(bool enable);
  /** Returns @c true if call alert is enabled. */
  bool callAlertEnabled() const;
  /** Enables/disables call alert. */
  void enableCallAlert(bool enable);

  /** Returns @c true if the talk permit tone is enabled for digital channels. */
  bool talkPermitDigitalEnabled() const;
  /** Enables/disables the talk permit tone for digital channels. */
  void enableTalkPermitDigital(bool enable);
  /** Returns @c true if the talk permit tone is enabled for digital channels. */
  bool talkPermitAnalogEnabled() const;
  /** Enables/disables the talk permit tone for analog channels. */
  void enableTalkPermitAnalog(bool enable);
  /** Returns @c true if the reset tone is enabled for digital calls. */
  bool digitalResetToneEnabled() const;
  /** Enables/disables the reset tone for digital calls. */
  void enableDigitalResetTone(bool enable);
  /** Returns @c true if the idle channel tone is enabled. */
  bool idleChannelToneEnabled() const;
  /** Enables/disables the idle channel tone. */
  void enableIdleChannelTone(bool enable);
  /** Retunrs @c true if the startup tone is enabled. */
  bool startupToneEnabled() const;
  /** Enables/disables the startup tone. */
  void enableStartupTone(bool enable);

protected:
  bool _keyTone;                   ///< Key tone property.
  bool _smsAlert;                  ///< SMS alert tone enabled.
  bool _callAlert;                 ///< Call alert tone enabled.
  bool _talkPermitDigital;         ///< DMR talk permit tone.
  bool _talkPermitAnalog;          ///< FM talk permit tone.
  bool _resetToneDigital;          ///< DMR reset tone.
  bool _idleChannelTone;           ///< Idle channel tone.
  bool _startupTone;               ///< Startup tone enabled.
};


/** Implements the display settings extension of AnyTone devices.
 * This extension is part of the @c AnytoneSettingsExtension.
 *
 * @ingroup anytone */
class AnytoneDisplaySettingsExtension: public ConfigItem
{
  Q_OBJECT

  /** The display frequency setting. */
  Q_PROPERTY(bool displayFrequency READ displayFrequencyEnabled WRITE enableDisplayFrequency)
  /** The display brightness [1-10]. */
  Q_PROPERTY(unsigned int brightness READ brightness WRITE setBrightness)
  /** The backlight duration in seconds. */
  Q_PROPERTY(unsigned int backlightDuration READ backlightDuration WRITE setBacklightDuration)

public:
  /** Constructor. */
  explicit AnytoneDisplaySettingsExtension(QObject *parent=nullptr);

  ConfigItem *clone() const;

  /** Returns @c true, if the frequency is displayed instead of the channel name. */
  bool displayFrequencyEnabled() const;
  /** Enables/disables display of frequency. */
  void enableDisplayFrequency(bool enable);

  /** Returns the display brightness [1-10]. */
  unsigned int brightness() const;
  /** Sets the display brightness [1-10]. */
  void setBrightness(unsigned int level);

  /** Returns the backlight duration in seconds, 0 means permanent. */
  unsigned int backlightDuration() const;
  /** Sets the backlight duration in seconds, 0 means permanent. */
  void setBacklightDuration(unsigned int sec);

  /** Returns @c true if the call-end prompt is shown. */
  bool callEndPromptEnabled() const;
  /** Enables/disables the call-end prompt. */
  void enableCallEndPrompt(bool enable);

protected:
  bool _displayFrequency;          ///< Display frequency property.
  unsigned int _brightness;        ///< The display brightness.
  unsigned int _backlightDuration; ///< Backlight duration in seconds, 0=permanent.
  bool _callEndPrompt;             ///< Call-end prompt enabled.
};


/** Implements the audio settings extension of AnyTone devices.
 * This extension is part of the @c AnytoneSettingsExtension.
 *
 * @ingroup anytone */
class AnytoneAudioSettingsExtension: public ConfigItem
{
  Q_OBJECT

  /** The VOX delay in ms. */
  Q_PROPERTY(unsigned int voxDelay READ voxDelay WRITE setVOXDelay)
  /** If @c true, recording is enabled. */
  Q_PROPERTY(bool recording READ recordingEnabled WRITE enableRecording)
  /** The VOX source. */
  Q_PROPERTY(VoxSource voxSource READ voxSource WRITE setVOXSource)
  /** The maximum volume setting [0-10]. */
  Q_PROPERTY(unsigned int maxVolume READ maxVolume WRITE setMaxVolume)
  /** The maximum head-phone volume setting [0-10]. */
  Q_PROPERTY(unsigned int maxHeadPhoneVolume READ maxHeadPhoneVolume WRITE setMaxHeadPhoneVolume)
  /** If @c true, the audio is "enhanced". */
  Q_PROPERTY(bool enhance READ enhanceAudioEnabled WRITE enableEnhanceAudio)

public:
  /** Source for the VOX. */
  enum class VoxSource {
    Internal = 0, External = 1, Both = 2
  };
  Q_ENUM(VoxSource)

public:
  /** Default constructor. */
  explicit AnytoneAudioSettingsExtension(QObject *parent=nullptr);

  ConfigItem *clone() const;

  /** Returns the VOX delay in ms. */
  unsigned int voxDelay() const;
  /** Sets the VOX delay in ms. */
  void setVOXDelay(unsigned int ms);

  /** Returns @c true if recording is enabled. */
  bool recordingEnabled() const;
  /** Enables/disables recording. */
  void enableRecording(bool enable);

  /** Returns the VOX source. */
  VoxSource voxSource() const;
  /** Sets the VOX source. */
  void setVOXSource(VoxSource source);

  /** Returns the maximum volume setting [0-10]. */
  unsigned int maxVolume() const;
  /** Sets the maximum volume. */
  void setMaxVolume(unsigned int vol);
  /** Returns the maximum head-phone volume setting [0-10]. */
  unsigned int maxHeadPhoneVolume() const;
  /** Sets the maximum head-phone volume. */
  void setMaxHeadPhoneVolume(unsigned int vol);

  /** Returns @c true if the audio is "enhanced". */
  bool enhanceAudioEnabled() const;
  /** Enables/disables enhanced audio. */
  void enableEnhanceAudio(bool enable);

protected:
  unsigned int _voxDelay;           ///< VOX delay in ms.
  bool _recording;                  ///< Recording enabled.
  VoxSource _voxSource;             ///< The VOX source.
  unsigned int _maxVolume;          ///< The maximum volume.
  unsigned int _maxHeadPhoneVolume; ///< The maximum head-phone volume.
  bool _enhanceAudio;               ///< Enhance audio.
};


/** Implements the menu settings extension of AnyTone devices.
 * This extension is part of the @c AnytoneSettingsExtension.
 *
 * @ingroup anytone */
class AnytoneMenuSettingsExtension: public ConfigItem
{
  Q_OBJECT

  /** Menu exit time in seconds. */
  Q_PROPERTY(unsigned int duration READ duration WRITE setDuration)

public:
  /** Default constructor. */
  explicit AnytoneMenuSettingsExtension(QObject *parent=nullptr);

  ConfigItem *clone() const;

  /** Returns the menu duration in seconds. */
  unsigned int duration() const;
  /** Sets the menu duration in seconds. */
  void setDuration(unsigned int sec);

protected:
  unsigned int _menuDuration;      ///< Menu display duration in seconds.
};


/** Implements the device specific extension for the gerneral settings of AnyTone devices.
 *
 * As there are a huge amount of different settings, they are split into separate extensions.
 * One for each topic.
 *
 * @ingroup anytone */
class AnytoneSettingsExtension: public ConfigExtension
{
  Q_OBJECT

  /** The auto shut-down delay in minutes. */
  Q_PROPERTY(unsigned int autoShutDownDelay READ autoShutDownDelay WRITE setAutoShutDownDelay)
  /** The power-save mode. */
  Q_PROPERTY(PowerSave powerSave READ powerSave WRITE setPowerSave)
  /** The VFO scan type. */
  Q_PROPERTY(VFOScanType vfoScanType READ vfoScanType WRITE setVFOScanType)
  /** The mode of VFO A. */
  Q_PROPERTY(VFOMode modeA READ modeA WRITE setModeA)
  /** The mode of VFO B. */
  Q_PROPERTY(VFOMode modeB READ modeB WRITE setModeB)
  /** The current zone for VFO A. */
  Q_PROPERTY(ZoneReference* zoneA READ zoneA)
  /** The current zone for VFO B. */
  Q_PROPERTY(ZoneReference* zoneB READ zoneB)
  /** The current active VFO. */
  Q_PROPERTY(VFO selectedVFO READ selectedVFO WRITE setSelectedVFO)
  /** If @c true, the sub-channel is enabled. */
  Q_PROPERTY(bool subChannel READ subChannelEnabled WRITE enableSubChannel)
  /** The time-zone IANA Id. */
  Q_PROPERTY(QString timeZone READ ianaTimeZone WRITE setIANATimeZone)

  /** The boot settings. */
  Q_PROPERTY(AnytoneBootSettingsExtension* bootSettings READ bootSettings)
  /** The key settings. */
  Q_PROPERTY(AnytoneKeySettingsExtension* keySettings READ keySettings)
  /** The tone settings. */
  Q_PROPERTY(AnytoneToneSettingsExtension* toneSettings READ toneSettings)
  /** The display settings. */
  Q_PROPERTY(AnytoneDisplaySettingsExtension* displaySettings READ displaySettings)
  /** The audio settings. */
  Q_PROPERTY(AnytoneAudioSettingsExtension* audioSettings READ audioSettings)
  /** The menu settings. */
  Q_PROPERTY(AnytoneMenuSettingsExtension* menuSettings READ menuSettings)
  /** The minimum UHF VFO-scan frequency in Hz. */
  Q_PROPERTY(unsigned int minVFOScanFrequencyUHF READ minVFOScanFrequencyUHF WRITE setMinVFOScanFrequencyUHF)
  /** The maximum UHF VFO-scan frequency in Hz. */
  Q_PROPERTY(unsigned int maxVFOScanFrequencyUHF READ maxVFOScanFrequencyUHF WRITE setMaxVFOScanFrequencyUHF)
  /** The minimum VHF VFO-scan frequency in Hz. */
  Q_PROPERTY(unsigned int minVFOScanFrequencyVHF READ minVFOScanFrequencyVHF WRITE setMinVFOScanFrequencyVHF)
  /** The maximum VHF VFO-scan frequency in Hz. */
  Q_PROPERTY(unsigned int maxVFOScanFrequencyVHF READ maxVFOScanFrequencyVHF WRITE setMaxVFOScanFrequencyVHF)

public:
  /** Possible power save modes. */
  enum class PowerSave {
    Off = 0, Save50 = 1, Save66 = 2
  };
  Q_ENUM(PowerSave)

  /** Encodes the possible VFO scan types. */
  enum class VFOScanType {
    TO = 0, CO = 1, SE = 2
  };
  Q_ENUM(VFOScanType)

  /** Possible VFO modes. */
  enum class VFOMode {
    Memory = 0, VFO = 1
  };
  Q_ENUM(VFOMode)

  /** Possible VFOs. */
  enum class VFO {
    A = 0, B = 1
  };
  Q_ENUM(VFO)

public:
  /** Constructor. */
  Q_INVOKABLE explicit AnytoneSettingsExtension(QObject *parent=nullptr);

  ConfigItem *clone() const;

  /** A reference to the boot settings. */
  AnytoneBootSettingsExtension *bootSettings() const;
  /** A reference to the key settings. */
  AnytoneKeySettingsExtension *keySettings() const;
  /** A reference to the tone settings. */
  AnytoneToneSettingsExtension *toneSettings() const;
  /** A reference to the display settings. */
  AnytoneDisplaySettingsExtension *displaySettings() const;
  /** A reference to the audio settings. */
  AnytoneAudioSettingsExtension *audioSettings() const;
  /** A reference to the menu settings. */
  AnytoneMenuSettingsExtension *menuSettings() const;

  /** Returns the auto shut-down delay in minutes. */
  unsigned int autoShutDownDelay() const;
  /** Sets the auto shut-down delay. */
  void setAutoShutDownDelay(unsigned int min);

  /** Returns the power-save mode. */
  PowerSave powerSave() const;
  /** Sets the power-save mode. */
  void setPowerSave(PowerSave mode);

  /** Returns the VFO scan type. */
  VFOScanType vfoScanType() const;
  /** Sets the VFO scan type. */
  void setVFOScanType(VFOScanType type);

  /** Returns mode for VFO A. */
  VFOMode modeA() const;
  /** Sets the mode for VFO A. */
  void setModeA(VFOMode mode);
  /** Returns mode for VFO B. */
  VFOMode modeB() const;
  /** Sets the mode for VFO B. */
  void setModeB(VFOMode mode);

  /** Retruns a reference to the current zone for VFO A. */
  ZoneReference *zoneA();
  /** Retruns a reference to the current zone for VFO A. */
  const ZoneReference *zoneA() const;
  /** Returns a reference to the current zone for VFO B. */
  ZoneReference *zoneB();
  /** Returns a reference to the current zone for VFO B. */
  const ZoneReference *zoneB() const;

  /** Returns the selected VFO. */
  VFO selectedVFO() const;
  /** Sets the selected VFO. */
  void setSelectedVFO(VFO vfo);

  /** Returns @c true if the sub-channel is enabled. */
  bool subChannelEnabled() const;
  /** Enables/disables the sub-channel. */
  void enableSubChannel(bool enable);

  /** Returns the IANA ID of the time zone. */
  QString ianaTimeZone() const;
  /** Returns the time-zone. */
  QTimeZone timeZone() const;
  /** Sets the time zone by IANA ID. */
  void setIANATimeZone(const QString &id);
  /** Sets the time zone. */
  void setTimeZone(const QTimeZone &zone);

  /** Returns the minimum VFO scan frequency for the UHF band in Hz. */
  unsigned minVFOScanFrequencyUHF() const;
  /** Sets the minimum VFO scan frequency for the UHF band in Hz. */
  void setMinVFOScanFrequencyUHF(unsigned hz);
  /** Returns the maximum VFO scan frequency for the UHF band in Hz. */
  unsigned maxVFOScanFrequencyUHF() const;
  /** Sets the maximum VFO scan frequency for the UHF band in Hz. */
  void setMaxVFOScanFrequencyUHF(unsigned hz);

  /** Returns the minimum VFO scan frequency for the VHF band in Hz. */
  unsigned minVFOScanFrequencyVHF() const;
  /** Sets the minimum VFO scan frequency for the VHF band in Hz. */
  void setMinVFOScanFrequencyVHF(unsigned hz);
  /** Returns the maximum VFO scan frequency for the VHF band in Hz. */
  unsigned maxVFOScanFrequencyVHF() const;
  /** Sets the maximum VFO scan frequency for the VHF band in Hz. */
  void setMaxVFOScanFrequencyVHF(unsigned hz);

protected:
  /** The boot settings. */
  AnytoneBootSettingsExtension *_bootSettings;
  /** The key settings. */
  AnytoneKeySettingsExtension *_keySettings;
  /** The tone settings. */
  AnytoneToneSettingsExtension *_toneSettings;
  /** The display settings. */
  AnytoneDisplaySettingsExtension *_displaySettings;
  /** The audio settings. */
  AnytoneAudioSettingsExtension *_audioSettings;
  /** The audio settings. */
  AnytoneMenuSettingsExtension *_menuSettings;

  bool _autoShutDownDelay;         ///< The auto shut-down delay in minutes.
  PowerSave _powerSave;            ///< Power save mode property.
  VFOScanType _vfoScanType;        ///< The VFO scan-type property.
  VFOMode _modeA;                  ///< Mode of VFO A.
  VFOMode _modeB;                  ///< Mode of VFO B.
  ZoneReference _zoneA;            ///< The current zone for VFO A.
  ZoneReference _zoneB;            ///< The current zone for VFO B.
  VFO _selectedVFO;                ///< The current VFO.
  bool _subChannel;                ///< If @c true, the sub-channel is enabled.
  QTimeZone _timeZone;             ///< The time zone.
  unsigned int _minVFOScanFrequencyUHF; ///< The minimum UHF VFO-scan frequency in Hz.
  unsigned int _maxVFOScanFrequencyUHF; ///< The maximum UHF VFO-scan frequency in Hz.
  unsigned int _minVFOScanFrequencyVHF; ///< The minimum VHF VFO-scan frequency in Hz.
  unsigned int _maxVFOScanFrequencyVHF; ///< The maximum VHF VFO-scan frequency in Hz.
};

#endif // ANYTONEEXTENSION_HH
