#ifndef ANYTONEEXTENSION_HH
#define ANYTONEEXTENSION_HH

#include "configobject.hh"
#include "configreference.hh"
#include "melody.hh"

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

  /** If enables, the default channels are selected at boot. */
  Q_PROPERTY(bool defaultChannel READ defaultChannelEnabled WRITE enableDefaultChannel)
  /** The default zone for VFO A. */
  Q_PROPERTY(ZoneReference* zoneA READ zoneA)
  /** The default channel for VFO A. */
  Q_PROPERTY(ChannelReference* channelA READ channelA)
  /** The current zone for VFO B. */
  Q_PROPERTY(ZoneReference* zoneB READ zoneB)
  /** The default channel for VFO B. */
  Q_PROPERTY(ChannelReference* channelB READ channelB)
  /** The priority zone for VFO A. */
  Q_PROPERTY(ZoneReference* priorityZoneA READ priorityZoneA)
  /** The priority zone for VFO B. */
  Q_PROPERTY(ZoneReference* priorityZoneB READ priorityZoneB)
  /** The default roaming zone. */
  Q_PROPERTY(RoamingZoneReference* defaultRoamingZone READ defaultRoamingZone)

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

  /** If @c true, the radio switches to the default channel at boot. */
  bool defaultChannelEnabled() const;
  /** Enables/disables boot default channel. */
  void enableDefaultChannel(bool enable);
  /** Returns a reference to the default zone for VFO A. */
  ZoneReference *zoneA() const;
  /** Returns a reference to the default channel for VFO A. */
  ChannelReference *channelA() const;
  /** Returns a reference to the default zone for VFO B. */
  ZoneReference *zoneB() const;
  /** Returns a reference to the default channel for VFO B. */
  ChannelReference *channelB() const;
  /** Returns a reference to the priority zone for VFO A. */
  ZoneReference *priorityZoneA() const;
  /** Returns a reference to the priority zone for VFO B. */
  ZoneReference *priorityZoneB() const;
  /** Returns a reference to the default roaming zone. */
  RoamingZoneReference *defaultRoamingZone() const;

protected:
  BootDisplay _bootDisplay;        ///< The boot display property.
  bool _bootPasswordEnabled;       ///< If true, the boot password is enabled.
  QString _bootPassword;           ///< The boot password
  bool _defaultChannel;            ///< Change to the default channel on boot.
  ZoneReference *_zoneA;           ///< Default zone for VFO A.
  ChannelReference *_channelA;     ///< Default channel for VFO A, must be member of zone for VFO A.
  ZoneReference *_zoneB;           ///< Default zone for VFO B.
  ChannelReference *_channelB;     ///< Default channel for VFO B, must be member of zone for VFO B.
  ZoneReference *_priorityZoneA;   ///< Priority zone for VFO A.
  ZoneReference *_priorityZoneB;   ///< Priority zone for VFO B.
  RoamingZoneReference *_defaultRoamingZone; ///< The default roaming zone.
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

  Q_CLASSINFO("knobLockDescription", "If enabled, the knob gets locked too.")
  /** If @c true, the knob gets locked too. */
  Q_PROPERTY(bool knobLock READ knobLockEnabled WRITE enableKnobLock)

  Q_CLASSINFO("keypadLockDescription", "If enabled, the key-pad gets locked.")
  /** If @c true, the key-pad gets locked too. */
  Q_PROPERTY(bool keypadLock READ keypadLockEnabled WRITE enableKeypadLock)

  Q_CLASSINFO("sideKeysLockDescription", "If enabled, the side-keys get locked.")
  /** If @c true, the side-keys get locked too. */
  Q_PROPERTY(bool sideKeysLock READ sideKeysLockEnabled WRITE enableSideKeysLock)

  Q_CLASSINFO("forcedKeyLockDescription", "If enabled, the key-lock is forced.")
  /** If @c true, the key-lock is forced. */
  Q_PROPERTY(bool forcedKeyLock READ forcedKeyLockEnabled WRITE enableForcedKeyLock)

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

  /** Returns @c true if the knob gets locked too. */
  bool knobLockEnabled() const;
  /** Enables/disables the knob lock. */
  void enableKnobLock(bool enable);
  /** Returns @c true if the key-pad gets locked too. */
  bool keypadLockEnabled() const;
  /** Enables/disables the key-pad lock. */
  void enableKeypadLock(bool enable);
  /** Returns @c true if the side-keys gets locked too. */
  bool sideKeysLockEnabled() const;
  /** Enables/disables the side-keys lock. */
  void enableSideKeysLock(bool enable);
  /** Returns @c true if the key-lock is forced. */
  bool forcedKeyLockEnabled() const;
  /** Enables/disables the forced key-lock. */
  void enableForcedKeyLock(bool enable);

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
  bool _autoKeyLock;                       ///< Auto key-lock property.
  bool _knobLock;                          ///< Knob locked too.
  bool _keypadLock;                        ///< Key-pad is locked.
  bool _sideKeysLock;                      ///< Side-keys are locked.
  bool _forcedKeyLock;                     ///< Forced key-lock.
};


/** Implements the tone settings extension of AnyTone devices.
 * This extension is part of the @c AnytoneSettingsExtension.
 *
 * @ingroup anytone */
class AnytoneToneSettingsExtension: public ConfigItem
{
  Q_OBJECT
  Q_CLASSINFO("description", "Tone settings for AnyTone devices.")

  Q_CLASSINFO("keyToneDescription", "If true, enables the key tones.")
  /** The key tone setting. */
  Q_PROPERTY(bool keyTone READ keyToneEnabled WRITE enableKeyTone)

  Q_CLASSINFO("keyToneLevelDescription", "Specifies the key-tone level, 0=user adjustable.")
  /** The key tone level setting. */
  Q_PROPERTY(unsigned int keyToneLevel READ keyToneLevel WRITE setKeyToneLevel)

  Q_CLASSINFO("smsAlertDescription", "Enables/disables the SMS alert tone.")
  /** If @c true, the SMS alert tone is enabled. */
  Q_PROPERTY(bool smsAlert READ smsAlertEnabled WRITE enableSMSAlert)

  Q_CLASSINFO("callAlertDescription", "Enables/disables the call alert tone.")
  /** If @c true, the call alert tone is enabled. */
  Q_PROPERTY(bool callAlert READ callAlertEnabled WRITE enableCallAlert)

  Q_CLASSINFO("dmrTalkPermitDescription", "Enables/disables the talk-permit tone for DMR channels.")
  /** If @c true, the DMR talk permit tone is enabled. */
  Q_PROPERTY(bool dmrTalkPermit READ talkPermitDigitalEnabled WRITE enableTalkPermitDigital)

  Q_CLASSINFO("fmTalkPermitDescription", "Enables/disables the talk-permit tone for FM channels.")
  /** If @c true, the FM talk permit tone is enabled. */
  Q_PROPERTY(bool fmTalkPermit READ talkPermitAnalogEnabled WRITE enableTalkPermitAnalog)

  Q_CLASSINFO("dmrResetDescription", "Enables/disables the reset tone for DMR channels.")
  /** If @c true, the DMR reset tone is enabled. */
  Q_PROPERTY(bool dmrReset READ digitalResetToneEnabled WRITE enableDigitalResetTone)

  /** If @c true, the idle tone is enabled. */
  Q_PROPERTY(bool idle READ idleChannelToneEnabled WRITE enableIdleChannelTone)
  /** If @c true, the startup tone is enabled. */
  Q_PROPERTY(bool startup READ startupToneEnabled WRITE enableStartupTone)
  /** The call melody. */
  Q_PROPERTY(Melody * callMelody READ callMelody)
  /** The idle melody. */
  Q_PROPERTY(Melody * idleMelody READ idleMelody)
  /** The reset melody. */
  Q_PROPERTY(Melody * resetMelody READ resetMelody)

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

  /** Returns a reference to the call melody. */
  Melody *callMelody() const;
  /** Returns a reference to the idle melody. */
  Melody *idleMelody() const;
  /** Returns a reference to the reset melody. */
  Melody *resetMelody() const;

  /** Returns the key-tone level. */
  unsigned int keyToneLevel() const;
  /** Sets the key-tone level. */
  void setKeyToneLevel(unsigned int level);

protected:
  bool _keyTone;                   ///< Key tone property.
  bool _smsAlert;                  ///< SMS alert tone enabled.
  bool _callAlert;                 ///< Call alert tone enabled.
  bool _talkPermitDigital;         ///< DMR talk permit tone.
  bool _talkPermitAnalog;          ///< FM talk permit tone.
  bool _resetToneDigital;          ///< DMR reset tone.
  bool _idleChannelTone;           ///< Idle channel tone.
  bool _startupTone;               ///< Startup tone enabled.
  Melody *_callMelody;             ///< Call melody.
  Melody *_idleMelody;             ///< Idle melody.
  Melody *_resetMelody;            ///< Reset melody.
  unsigned int _keyToneLevel;      ///< The level of key-tones, 0=user adjustable.

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
  /** The volume-change prompt is shown. */
  Q_PROPERTY(bool volumeChangePrompt READ volumeChangePromptEnabled WRITE enableVolumeChangePrompt)
  /** The call-end prompt is shown. */
  Q_PROPERTY(bool callEndPrompt READ callEndPromptEnabled WRITE enableCallEndPrompt)
  /** The last-caller display mode. */
  Q_PROPERTY(LastCallerDisplayMode lastCallerDisplay READ lastCallerDisplay WRITE setLastCallerDisplay)
  /** If @c true, the clock is shown. */
  Q_PROPERTY(bool showClock READ showClockEnabled WRITE enableShowClock)
  /** If @c true, the call is shown. */
  Q_PROPERTY(bool showCall READ showCallEnabled WRITE enableShowCall)
  /** The color of the call. */
  Q_PROPERTY(Color callColor READ callColor WRITE setCallColor)
  /** If @c true, the zone and contact names is shown. */
  Q_PROPERTY(bool showZoneAndContact READ showZoneAndContactEnabled WRITE enableShowZoneAndContact)
  /** Specifies the UI language. */
  Q_PROPERTY(Language language READ language WRITE setLanguage)
  /** Shows the channel number. */
  Q_PROPERTY(bool showChannelNumber READ showChannelNumberEnabled WRITE enableShowChannelNumber)
  /** Shows the contact. */
  Q_PROPERTY(bool showContact READ showContactEnabled WRITE enableShowContact)
  /** The standby text color. */
  Q_PROPERTY(Color standbyTextColor READ standbyTextColor WRITE setStandbyTextColor)
  /** Shows the last caller. */
  Q_PROPERTY(bool showLastHeard READ showLastHeardEnabled WRITE enableShowLastHeard)

public:
  /** What to show from the last caller. */
  enum class LastCallerDisplayMode {
    Off = 0, ID = 1, Call = 2, Both = 3
  };
  Q_ENUM(LastCallerDisplayMode)

  /** Possible display colors. */
  enum class Color {
    White = 0, Black = 1, Orange=2, Red=3, Yellow=4, Green=5, Turquoise=6, Blue=7
  };
  Q_ENUM(Color)

  /** Possible UI languages. */
  enum class Language {
    English = 0,                 ///< UI Language is english.
    German  = 1                  ///< UI Language is german.
  };
  Q_ENUM(Language)

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

  /** Returns @c true if the volume-change prompt is shown. */
  bool volumeChangePromptEnabled() const;
  /** Enables/disables the volume-change prompt. */
  void enableVolumeChangePrompt(bool enable);

  /** Returns @c true if the call-end prompt is shown. */
  bool callEndPromptEnabled() const;
  /** Enables/disables the call-end prompt. */
  void enableCallEndPrompt(bool enable);

  /** Returns the last caller display mode. */
  LastCallerDisplayMode lastCallerDisplay() const;
  /** Sets the last caller display mode. */
  void setLastCallerDisplay(LastCallerDisplayMode mode);

  /** Returns @c true if the clock is shown. */
  bool showClockEnabled() const;
  /** Enables/disables clock. */
  void enableShowClock(bool enable);

  /** Returns @c true if the call is shown. */
  bool showCallEnabled() const;
  /** Enables/disables display of call. */
  void enableShowCall(bool enable);

  /** Returns the color of the call. */
  Color callColor() const;
  /** Sets the color of the call. */
  void setCallColor(Color color);

  /** Returns @c true if the zone and contact names are shown. */
  bool showZoneAndContactEnabled() const;
  /** Enables/disables showing the zone and contact names. */
  void enableShowZoneAndContact(bool enable);

  /** Returns the UI language. */
  Language language() const;
  /** Sets the UI language. */
  void setLanguage(Language lang);

  /** Returns @c true if the channel number is shown. */
  bool showChannelNumberEnabled() const;
  /** Enables/disables the display of the channel number. */
  void enableShowChannelNumber(bool enable);

  /** Returns @c true if the contact is shown. */
  bool showContactEnabled() const;
  /** Enables/disables the display of calling contact. */
  void enableShowContact(bool enable);

  /** Returns the standby text color. */
  Color standbyTextColor() const;
  /** Sets the standby text color. */
  void setStandbyTextColor(Color color);

  /** Shows the last caller. */
  bool showLastHeardEnabled() const;
  /** Enables/disables display of last caller. */
  void enableShowLastHeard(bool enable);

protected:
  bool _displayFrequency;                   ///< Display frequency property.
  unsigned int _brightness;                 ///< The display brightness.
  unsigned int _backlightDuration;          ///< Backlight duration in seconds, 0=permanent.
  bool _volumeChangePrompt;                 ///< Volume-change prompt enabled.
  bool _callEndPrompt;                      ///< Call-end prompt enabled.
  LastCallerDisplayMode _lastCallerDisplay; ///< Last-caller display mode.
  bool _showClock;                          ///< Display clock.
  bool _showCall;                           ///< Display call.
  Color _callColor;                         ///< Color of call.
  bool _showZoneAndContact;                 ///< Display zone and contact
  Language _language;                       ///< UI language.
  bool _showChannelNumber;                  ///< Show channel number.
  bool _showContact;                        ///< Enables showing the contact.
  Color _standbyTextColor;                  ///< Standby text color.
  bool _showLastHeard;                      ///< Shows the last caller.
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


/** Implements the config representation of a repeater offset. This is just a transmit
 *  offset-frequency in Hz.
 * @ingroup anytone */
class AnytoneAutoRepeaterOffset: public ConfigObject
{
  Q_OBJECT
  Q_CLASSINFO("IdPrefix", "off")

  Q_CLASSINFO("offsetDecription",
              "Transmit-frequency offset in Hz.")
  Q_CLASSINFO("offsetLongDecription",
              "The transmit-frequency offset is specified as a positive integer in Hz. The offset "
              "direction is specified for each VFO separately.")
  /** The offset frequency. */
  Q_PROPERTY(unsigned int offset READ offset WRITE setOffset)

public:
  /** Default constructor. */
  Q_INVOKABLE AnytoneAutoRepeaterOffset(QObject *parent=nullptr);

  ConfigItem *clone() const;

  /** Returns the transmit frequency offset in Hz. */
  unsigned int offset() const;
  /** Sets the transmit frequency offset in Hz. */
  void setOffset(unsigned int offset);

protected:
  /** The transmit frequency offset in Hz. */
  unsigned int _offset;
};


/** Represents a reference to a repeater offset.
 * @ingroup anytone */
class AnytoneAutoRepeaterOffsetRef: public ConfigObjectReference
{
  Q_OBJECT

public:
  /** Default constructor. */
  explicit AnytoneAutoRepeaterOffsetRef(QObject *parent=nullptr);
};


/** Represents a list of auto-repeater offsets.
 * @ingroup anytone */
class AnytoneAutoRepeaterOffsetList: public ConfigObjectList
{
  Q_OBJECT

public:
  /** Empty constructor. */
  explicit AnytoneAutoRepeaterOffsetList(QObject *parent=nullptr);

  ConfigItem *allocateChild(const YAML::Node &node, ConfigItem::Context &ctx, const ErrorStack &err);
};


/** Implements the auto-repeater settings extension of AnyTone devices.
 * This extension is part of the @c AnytoneSettingsExtension.
 *
 * @ingroup anytone */
class AnytoneAutoRepeaterSettingsExtension: public ConfigItem
{
  Q_OBJECT
  Q_CLASSINFO("description", "Auto-repeater settings for AnyTone devices.")

  Q_CLASSINFO("directionADescription", "Auto-repeater transmit-frequency offset direction for VFO A.")
  /** Specifies the auto-repeater transmit-frequency offset direction for VFO A. */
  Q_PROPERTY(Direction directionA READ directionA WRITE setDirectionA)

  Q_CLASSINFO("directionBDescription", "Auto-repeater transmit-frequency offset direction for VFO B.")
  /** Specifies the auto-repeater transmit-frequency offset direction for VFO B. */
  Q_PROPERTY(Direction directionB READ directionB WRITE setDirectionB)

  Q_CLASSINFO("vhfMin", "The minimum frequency in Hz of the VHF auto-repeater frequency range.")
  /** The minimum frequency in Hz of the VHF auto-repeater frequency range. */
  Q_PROPERTY(unsigned int vhfMin READ vhfMin WRITE setVHFMin)

  Q_CLASSINFO("vhfMax", "The maximum frequency in Hz of the VHF auto-repeater frequency range.")
  /** The maximum frequency in Hz of the VHF auto-repeater frequency range. */
  Q_PROPERTY(unsigned int vhfMax READ vhfMax WRITE setVHFMax)

  Q_CLASSINFO("uhfMin", "The minimum frequency in Hz of the UHF auto-repeater frequency range.")
  /** The minimum frequency in Hz of the UHF auto-repeater frequency range. */
  Q_PROPERTY(unsigned int uhfMin READ uhfMin WRITE setUHFMin)

  Q_CLASSINFO("uhfMax", "The maximum frequency in Hz of the UHF auto-repeater frequency range.")
  /** The maximum frequency in Hz of the UHF auto-repeater frequency range. */
  Q_PROPERTY(unsigned int uhfMax READ uhfMax WRITE setUHFMax)

  Q_CLASSINFO("vhfDescription", "A reference to an offset frequency for the VHF band.")
  /** A reference to the auto-repeater frequency for VHF. */
  Q_PROPERTY(AnytoneAutoRepeaterOffsetRef* vhf READ vhfRef)

  Q_CLASSINFO("uhfDescription", "A reference to an offset frequency for the UHF band.")
  /** A reference to the auto-repeater frequency for UHF. */
  Q_PROPERTY(AnytoneAutoRepeaterOffsetRef* uhf READ uhfRef)

  Q_CLASSINFO("offsetDescription", "The lists of offset frequencies.")
  /** The repeater transmit offset frequencies. */
  Q_PROPERTY(AnytoneAutoRepeaterOffsetList* offsets READ offsets)

public:
  /** Encodes the auto-repeater offset sign. */
  enum class Direction {
    Off = 0,       ///< Disabled.
    Positive = 1,  ///< Positive frequency offset.
    Negative = 2   ///< Negative frequency offset.
  };
  Q_ENUM(Direction)

public:
  /** Default constructor. */
  explicit AnytoneAutoRepeaterSettingsExtension(QObject *parent=nullptr);

  ConfigItem *clone() const;

  /** The auto-repeater offset direction for VFO A. */
  Direction directionA() const;
  /** Set the auto-repeater offset direction for VFO A. */
  void setDirectionA(Direction dir);
  /** The auto-repeater offset direction for VFO B. */
  Direction directionB() const;
  /** Set the auto-repeater offset direction for VFO V. */
  void setDirectionB(Direction dir);

  /** Returns the minimum frequency (in Hz) of the auto-repeater frequency range in the VHF band. */
  unsigned int vhfMin() const;
  /** Sets the minimum frequency (in Hz) of the auto-repeater frequency range in the VHF band. */
  void setVHFMin(unsigned int Hz);
  /** Returns the maximum frequency (in Hz) of the auto-repeater frequency range in the VHF band. */
  unsigned int vhfMax() const;
  /** Sets the maximum frequency (in Hz) of the auto-repeater frequency range in the VHF band. */
  void setVHFMax(unsigned int Hz);
  /** Returns the minimum frequency (in Hz) of the auto-repeater frequency range in the UHF band. */
  unsigned int uhfMin() const;
  /** Sets the minimum frequency (in Hz) of the auto-repeater frequency range in the UHF band. */
  void setUHFMin(unsigned int Hz);
  /** Returns the maximum frequency (in Hz) of the auto-repeater frequency range in the UHF band. */
  unsigned int uhfMax() const;
  /** Sets the maximum frequency (in Hz) of the auto-repeater frequency range in the UHF band. */
  void setUHFMax(unsigned int Hz);

  /** Returns the reference for the UHF offset freuqency. */
  AnytoneAutoRepeaterOffsetRef *uhfRef() const;
  /** Returns the reference for the VHF offset freuqency. */
  AnytoneAutoRepeaterOffsetRef *vhfRef() const;

  /** Returns a weak reference to the offset list. */
  AnytoneAutoRepeaterOffsetList *offsets() const;

protected:
  /** The auto-repeater direction for VFO A. */
  Direction _directionA;
  /** The auto-repeater direction for VFO B. */
  Direction _directionB;
  /** Minimum frequency of the VHF auto-repeater range. */
  unsigned int _minVHF;
  /** Maximum frequency of the VHF auto-repeater range. */
  unsigned int _maxVHF;
  /** Minimum frequency of the UHF auto-repeater range. */
  unsigned int _minUHF;
  /** Maximum frequency of the UHF auto-repeater range. */
  unsigned int _maxUHF;
  /** A reference to the VHF offset frequency. */
  AnytoneAutoRepeaterOffsetRef *_vhfOffset;
  /** A reference to the UHF offset frequency. */
  AnytoneAutoRepeaterOffsetRef *_uhfOffset;
  /** The list of repeater offsets. */
  AnytoneAutoRepeaterOffsetList *_offsets;
};


/** Implements the DMR settings extension of AnyTone devices.
 * This extension is part of the @c AnytoneSettingsExtension.
 *
 * @ingroup anytone */
class AnytoneDMRSettingsExtension: public ConfigItem
{
  Q_OBJECT

  Q_CLASSINFO("groupCallHangTimeDescription", "Specifies the hang- or hold-time for group calls.");
  /** Group-call hang-time in seconds. */
  Q_PROPERTY(unsigned int groupCallHangTime READ groupCallHangTime WRITE setGroupCallHangTime)

  Q_CLASSINFO("privateCallHangTimeDescription", "Specifies the hang- or hold-time for private calls.");
  /** Private-call hang-time in seconds. */
  Q_PROPERTY(unsigned int privateCallHangTime READ privateCallHangTime WRITE setPrivateCallHangTime)

  Q_CLASSINFO("preWaveDelay", "Sets the pre-wave delay in ms. Should be set to 100ms.")
  /** Pre-wave delay in ms. */
  Q_PROPERTY(unsigned int preWaveDelay READ preWaveDelay WRITE setPreWaveDelay)

  Q_CLASSINFO("wakeHeadPeriod", "Sets the wake head-period in ms. Should be set to 100ms.")
  /** Wake head-period in ms. */
  Q_PROPERTY(unsigned int wakeHeadPeriod READ wakeHeadPeriod WRITE setWakeHeadPeriod)

  Q_CLASSINFO("filterOwnIDDescription", "If enabled, own ID is not shown in call lists.")
  /** Filter own ID from call lists. */
  Q_PROPERTY(bool filterOwnID READ filterOwnIDEnabled WRITE enableFilterOwnID)

  Q_CLASSINFO("monitorSlotMatchDescription", "Time-slot match-mode for DMR monitor.")
  /** Slot-match mode for DMR monitor. */
  Q_PROPERTY(SlotMatch monitorSlotMatch READ monitorSlotMatch WRITE setMonitorSlotMatch)

  Q_CLASSINFO("monitorColorCodeMatchDescription", "If enabled, the DMR monitor will only open for "
              "matching color-codes.")
  /** Color-code match for DMR monitor. */
  Q_PROPERTY(bool monitorColorCodeMatch READ monitorColorCodeMatchEnabled WRITE enableMonitorColorCodeMatch)

  Q_CLASSINFO("monitorIDMatchDescription", "If enabled, the DMR monitor will only open for matching IDs.")
  /** ID match for DMR monitor. */
  Q_PROPERTY(bool monitorIDMatch READ monitorIDMatchEnabled WRITE enableMonitorIDMatch)

  Q_CLASSINFO("monitorTimeSlotHold", "Whether the DMR monitor holds the time-slot.")
  /** The DMR monitor holds the time-slot. */
  Q_PROPERTY(bool monitorTimeSlotHold READ monitorTimeSlotHoldEnabled WRITE enableMonitorTimeSlotHold)

  Q_CLASSINFO("smsFormatDescription", "Specifies the SMS format, select Motorola here.")
  /** The SMS format. */
  Q_PROPERTY(SMSFormat smsFormat READ smsFormat WRITE setSMSFormat)

public:
  /** Possible monitor slot matches. */
  enum class SlotMatch {
    Off = 0, Single = 1, Both = 2
  };
  Q_ENUM(SlotMatch)

  /** Possible SMS formats. */
  enum class SMSFormat {
    Motorola = 0, Hytera = 1, DMR = 2,
  };
  Q_ENUM(SMSFormat)

public:
  /** Constructor. */
  explicit AnytoneDMRSettingsExtension(QObject *parent = nullptr);

  ConfigItem *clone() const;

  /** Returns the group-call hang-time in seconds. */
  unsigned int groupCallHangTime() const;
  /** Sets the group-call hang-time in seconds. */
  void setGroupCallHangTime(unsigned int sec);
  /** Returns the private-call hang-time in seconds. */
  unsigned int privateCallHangTime() const;
  /** Sets the private-call hang-time in seconds. */
  void setPrivateCallHangTime(unsigned int sec);

  /** Returns the pre-wave delay in ms. */
  unsigned int preWaveDelay() const;
  /** Sets the pre-wave delay in ms. */
  void setPreWaveDelay(unsigned int ms);
  /** Returns the wake head-period in ms. */
  unsigned int wakeHeadPeriod() const;
  /** Sets the wake head-period in ms. */
  void setWakeHeadPeriod(unsigned int ms);

  /** If @c true, the own ID is not shown in call lists. */
  bool filterOwnIDEnabled() const;
  /** Enables/disables filtering of own ID. */
  void enableFilterOwnID(bool enable);

  /** Returns the slot-match mode for the DMR monitor. */
  SlotMatch monitorSlotMatch() const;
  /** Sets the slot-match mode for the DMR monitor. */
  void setMonitorSlotMatch(SlotMatch match);
  /** Returns @c true if the CC match is enabled for the DMR monitor. */
  bool monitorColorCodeMatchEnabled() const;
  /** Enables/disables the CC match for the DMR monitor. */
  void enableMonitorColorCodeMatch(bool enable);
  /** Returns @c true if the ID match is enabled for the DMR monitor. */
  bool monitorIDMatchEnabled() const;
  /** Enables/disables ID match for the DMR monitor. */
  void enableMonitorIDMatch(bool enable);
  /** Returns @c true if the time-slot is held by the DMR monitor. */
  bool monitorTimeSlotHoldEnabled() const;
  /** Enables/disables the time-slot hold for the DMR monitor. */
  void enableMonitorTimeSlotHold(bool enable);

  /** Returns the SMS format. */
  SMSFormat smsFormat() const;
  /** Sets the SMS format. */
  void setSMSFormat(SMSFormat format);

protected:
  unsigned int _groupCallHangTime;      ///< Hang-time for group-calls in seconds.
  unsigned int _privateCallHangTime;    ///< Hang-time for private-calls in seconds.
  unsigned int _preWaveDelay;           ///< Pre-wave time in ms, should be 100ms.
  unsigned int _wakeHeadPeriod;         ///< Wake head-period in ms, should be 100ms.
  bool _filterOwnID;                    ///< If enabled, the own ID is not shown in call lists.
  SlotMatch _monitorSlotMatch;          ///< Slot-match mode for DMR monitor.
  bool _monitorColorCodeMatch;          ///< Enables CC match for DMR monitor.
  bool _monitorIDMatch;                 ///< Enables ID match for DMR monitor.
  bool _monitorTimeSlotHold;            ///< Enables the time-slot hold for the DMR monitor.
  SMSFormat _smsFormat;                 ///< Sets the SMS format.
};


/** Implements the ranging/roaming settings extension of AnyTone devices.
 * This extension is part of the @c AnytoneSettingsExtension.
 *
 * @ingroup anytone */
class AnytoneRangingSettingsExtension: public ConfigItem
{
  Q_OBJECT

  Q_CLASSINFO("Description", "Collects all ranging/roaming settings for AnyTone devices.")

  Q_CLASSINFO("gpsRangeReporing", "Enables GPS range reporting.")
  /** Enables GPS range reporing. */
  Q_PROPERTY(bool gpsRangeReporting READ gpsRangeReportingEnabled WRITE enableGPSRangeReporting)

  Q_CLASSINFO("gpsRangingInterval", "Specifies the GPS ranging interval in seconds.")
  /** GPS ranging interval in seconds. */
  Q_PROPERTY(unsigned int gpsRangingInterval READ gpsRangingInterval WRITE setGPSRangingInterval)

  Q_CLASSINFO("autoRoamPeriodDescription", "Specifies the auto-roaming period in minutes.")
  /** The auto-roaming period in minutes. */
  Q_PROPERTY(unsigned int autoRoamPeriod READ autoRoamPeriod WRITE setAutoRoamPeriod)

  Q_CLASSINFO("rangeCheckDescription", "Repeater range check.")
  /** Repeater range check. */
  Q_PROPERTY(bool rangeCheck READ repeaterRangeCheckEnabled WRITE enableRepeaterRangeCheck)

  Q_CLASSINFO("checkIntervalDescription", "Repeater range check interval in seconds.")
  /** Repeater range check intervall in seconds. */
  Q_PROPERTY(bool checkInterval READ repeaterCheckInterval WRITE setRepeaterCheckInterval)

  Q_CLASSINFO("retryCount", "Number of retries to connect to a repeater before giving up.")
  /** Retry count. */
  Q_PROPERTY(unsigned int retryCount READ repeaterRangeCheckCount WRITE setRepeaterRangeCheckCount)

  Q_CLASSINFO("roamStart", "Start condition for auto-roaming.")
  /** Auto-roaming start condition. */
  Q_PROPERTY(RoamStart roamStart READ roamingStartCondition WRITE setRoamingStartCondition)

public:
  /** Possible roaming start conditions. */
  enum class RoamStart {
    Periodic=0, OutOfRange=1
  };
  Q_ENUM(RoamStart)

public:
  /** Constructor. */
  explicit AnytoneRangingSettingsExtension(QObject *parent=nullptr);

  ConfigItem *clone() const;

  /** Returns @c true if the GPS range reporing is enabled. */
  bool gpsRangeReportingEnabled() const;
  /** Enables/disables the GPS range reporting. */
  void enableGPSRangeReporting(bool enable);

  /** Returns the GPS ranging interval in seconds. */
  unsigned int gpsRangingInterval() const;
  /** Sets the GPS ranging interval in seconds. */
  void setGPSRangingInterval(unsigned int sec);

  /** Returns the auto-roaming period in minutes. */
  unsigned int autoRoamPeriod() const;
  /** Sets the auto-roam period in minutes. */
  void setAutoRoamPeriod(unsigned int min);
  /** Returns the auto-roam delay in seconds. */
  unsigned int autoRoamDelay() const;
  /** Sets the auto-roam delay in seconds. */
  void setAutoRoamDelay(unsigned int sec);

  /** Retruns @c true if the repeater range check is enabled. */
  bool repeaterRangeCheckEnabled() const;
  /** Enables/disables repeater range check. */
  void enableRepeaterRangeCheck(bool enable);
  /** Returns the repeater check interval in seconds. */
  unsigned int repeaterCheckInterval() const;
  /** Sets the repeater check interval in seconds. */
  void setRepeaterCheckInterval(unsigned int sec);
  /** Number of retries before givnig up. */
  unsigned int repeaterRangeCheckCount() const;
  /** Sets the number of retries before giving up. */
  void setRepeaterRangeCheckCount(unsigned int count);

  /** Returns the auto-roaming start condition. */
  RoamStart roamingStartCondition() const;
  /** Sets the auto-roaming start condition. */
  void setRoamingStartCondition(RoamStart start);

protected:
  bool _gpsRangeReporting;                     ///< Enables GPS range reporting.
  unsigned int _gpsRangingInterval;            ///< The GPS ranging interval in seconds.
  unsigned int _autoRoamPeriod;                ///< The auto-roam period in minutes.
  unsigned int _autoRoamDelay;                 ///< The auto-roam delay in seconds.
  bool _repeaterRangeCheck;                    ///< Enables the repeater range-check.
  unsigned int _repeaterCheckInterval;         ///< The repeater check interval in seconds.
  unsigned int _repeaterRangeCheckCount;       ///< Number of range checks before giving up.
  RoamStart _roamingStartCondition;            ///< Auto-roaming start condition.
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

  Q_CLASSINFO("description", "Device specific settings for AnyTone devices.")

  Q_CLASSINFO("autoShutDownDelayDescription", "The auto shut-down delay in minutes.")
  /** The auto shut-down delay in minutes. */
  Q_PROPERTY(unsigned int autoShutDownDelay READ autoShutDownDelay WRITE setAutoShutDownDelay)

  /** The power-save mode. */
  Q_PROPERTY(PowerSave powerSave READ powerSave WRITE setPowerSave)

  Q_CLASSINFO("subChannelDescription", "Enables/disables the sub-channel.")
  /** If @c true, the sub-channel is enabled. */
  Q_PROPERTY(bool subChannel READ subChannelEnabled WRITE enableSubChannel)

  /** The current active VFO. */
  Q_PROPERTY(VFO selectedVFO READ selectedVFO WRITE setSelectedVFO)

  /** The mode of VFO A. */
  Q_PROPERTY(VFOMode modeA READ modeA WRITE setModeA)

  /** The mode of VFO B. */
  Q_PROPERTY(VFOMode modeB READ modeB WRITE setModeB)

  /** The current zone for VFO A. */
  Q_PROPERTY(ZoneReference* zoneA READ zoneA)

  /** The current zone for VFO B. */
  Q_PROPERTY(ZoneReference* zoneB READ zoneB)

  /** The time-zone IANA Id. */
  Q_PROPERTY(QString timeZone READ ianaTimeZone WRITE setIANATimeZone)

  /** The VFO scan type. */
  Q_PROPERTY(VFOScanType vfoScanType READ vfoScanType WRITE setVFOScanType)
  /** The minimum UHF VFO-scan frequency in Hz. */
  Q_PROPERTY(unsigned int minVFOScanFrequencyUHF READ minVFOScanFrequencyUHF WRITE setMinVFOScanFrequencyUHF)
  /** The maximum UHF VFO-scan frequency in Hz. */
  Q_PROPERTY(unsigned int maxVFOScanFrequencyUHF READ maxVFOScanFrequencyUHF WRITE setMaxVFOScanFrequencyUHF)
  /** The minimum VHF VFO-scan frequency in Hz. */
  Q_PROPERTY(unsigned int minVFOScanFrequencyVHF READ minVFOScanFrequencyVHF WRITE setMinVFOScanFrequencyVHF)
  /** The maximum VHF VFO-scan frequency in Hz. */
  Q_PROPERTY(unsigned int maxVFOScanFrequencyVHF READ maxVFOScanFrequencyVHF WRITE setMaxVFOScanFrequencyVHF)

  Q_CLASSINFO("unitsDescription", "Specifies the GPS units.")
  /** The GPS units used. */
  Q_PROPERTY(Units units READ units WRITE setUnits)

  Q_CLASSINFO("keepLastCallerDescription", "Keeps the last caller on channel switch")
  /** The keep-last-caller setting. */
  Q_PROPERTY(bool keepLastCaller READ keepLastCallerEnabled WRITE enableKeepLastCaller)

  Q_CLASSINFO("vfoStep", "Specifes the VFO tuning steps in kHz.")
  /** The VFO tuning step-size in kHz. */
  Q_PROPERTY(double vfoStep READ vfoStep WRITE setVFOStep)

  Q_CLASSINFO("steTypeDescription", "Specifies the STE (squelch tail elimination) type.")
  /** The STE type. */
  Q_PROPERTY(STEType steType READ steType WRITE setSTEType)
  Q_CLASSINFO("steFrequencyDescription", "Specifies the STE (squelch tail elimination) frequency in Hz.")
  /** The STE frequency in Hz. */
  Q_PROPERTY(double steFrequency READ steFrequency WRITE setSTEFrequency)

  Q_CLASSINFO("tbstFrequencyDescription", "Specifies the TBST frequency in Hz. Should be one of "
                                          "1000, 1450, 1750 and 2100 Hz.")
  /** The TBST frequency in Hz. */
  Q_PROPERTY(unsigned int tbstFrequency READ tbstFrequency WRITE setTBSTFrequency)

  /** If @c true, the "pro mode" is enabled. */
  Q_PROPERTY(bool proMode READ proModeEnabled WRITE enableProMode)

  /** If @c true, the call-channel is maintained (whatever that means). */
  Q_PROPERTY(bool maintainCallChannel READ maintainCallChannelEnabled WRITE enableMaintainCallChannel)

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
  /** The auto-repeater settings. */
  Q_PROPERTY(AnytoneAutoRepeaterSettingsExtension* autoRepeaterSettings READ autoRepeaterSettings)
  /** The DMR settings. */
  Q_PROPERTY(AnytoneDMRSettingsExtension* dmrSettings READ dmrSettings)
  /** The Raging/Roaming settings. */
  Q_PROPERTY(AnytoneRangingSettingsExtension* rangingSettings READ rangingSettings)

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

  /** Possible unit systems. */
  enum class Units {
    Metric = 0, Imperial = 1
  };
  Q_ENUM(Units)

  /** All possible STE (squelch tail eliminate) types. */
  enum class STEType {
    Off = 0, Silent = 1, Deg120 = 2, Deg180 = 3, Deg240 = 4
  };
  Q_ENUM(STEType)


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
  /** A reference to the auto-repeater settings. */
  AnytoneAutoRepeaterSettingsExtension *autoRepeaterSettings() const;
  /** A reference to the DMR settings. */
  AnytoneDMRSettingsExtension *dmrSettings() const;
  /** A reference to the ranging settings. */
  AnytoneRangingSettingsExtension *rangingSettings() const;

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

  /** Returns the GPS units used. */
  Units units() const;
  /** Sets the GPS units. */
  void setUnits(Units units);

  /** Returns @c true if the last caller is kept on channel switch. */
  bool keepLastCallerEnabled() const;
  /** Enables/disables keeping the last caller on channel switch. */
  void enableKeepLastCaller(bool enable);

  /** Returns the VFO tuning step in kHz. */
  double vfoStep() const;
  /** Sets the VFO tuning step in kHz. */
  void setVFOStep(double step);

  /** Returns the STE (squelch tail elimination) type. */
  STEType steType() const;
  /** Sets the STE (squelch tail elimination) type. */
  void setSTEType(STEType type);
  /** Retruns the STE (squelch tail elimination) frequency in Hz.
   * A frequency of 0 disables the STE. Possible values are 55.2 and 259.2 Hz. */
  double steFrequency() const;
  /** Sets the STE (squelch tail elimination) frequency in Hz.
   * A frequency of 0 disables the STE. Possible values are 55.2 and 259.2 Hz. */
  void setSTEFrequency(double freq);

  /** Returns the TBST frequency in Hz. */
  unsigned int tbstFrequency() const;
  /** Sets the TBST frequency in Hz. Should be one of 1000, 1450, 1750 and 2100 Hz. */
  void setTBSTFrequency(unsigned int Hz);

  /** Returns @c true, if the "pro mode" is enabled. */
  bool proModeEnabled() const;
  /** Enables/disables the "pro mode". */
  void enableProMode(bool enable);

  /** Returns @c true if the call-channel is maintained. */
  bool maintainCallChannelEnabled() const;
  /** Enables/disables maintaining the call-channel. */
  void enableMaintainCallChannel(bool enable);

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
  /** The menu settings. */
  AnytoneMenuSettingsExtension *_menuSettings;
  /** The auto-repeater settings. */
  AnytoneAutoRepeaterSettingsExtension *_autoRepeaterSettings;
  /** The DMR settings. */
  AnytoneDMRSettingsExtension *_dmrSettings;
  /** The ranging settings. */
  AnytoneRangingSettingsExtension *_rangingSettings;

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
  Units _gpsUnits;                 ///< The GPS units.
  bool _keepLastCaller;            ///< If @c true, the last caller is kept on channel switch.
  double _vfoStep;                 ///< The VFO tuning step in kHz.
  STEType _steType;                ///< The STE type.
  double _steFrequency;            ///< STE Frequency in Hz.
  unsigned int _tbstFrequency;     ///< The TBST frequency in Hz.
  bool _proMode;                   ///< The "pro mode" flag.
  bool _maintainCallChannel;       ///< Maintains the call channel.
};

#endif // ANYTONEEXTENSION_HH
