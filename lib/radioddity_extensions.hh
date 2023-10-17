#ifndef RADIODDITYEXTENSIONS_HH
#define RADIODDITYEXTENSIONS_HH

#include "configobject.hh"
#include "interval.hh"


/** Represents the button settings extension for all radioddity devices.
 * This object is part of the RadiodditySettingsExtension instance. */
class RadioddityButtonSettingsExtension: public ConfigItem
{
  Q_OBJECT

  /** The long-press duration. */
  Q_PROPERTY(Interval longPressDuration READ longPressDuration WRITE setLongPressDuration)
  /** The short-press action for the programmable function key 1 (SK1, P1). */
  Q_PROPERTY(Function funcKey1Short READ funcKey1Short WRITE setFuncKey1Short)
  /** The long-press action for the programmable function key 1 (SK1, P1). */
  Q_PROPERTY(Function funcKey1Long READ funcKey1Long WRITE setFuncKey1Long)
  /** The short-press action for the programmable function key 2 (SK2, P2). */
  Q_PROPERTY(Function funcKey2Short READ funcKey2Short WRITE setFuncKey2Short)
  /** The long-press action for the programmable function key 2 (SK2, P2). */
  Q_PROPERTY(Function funcKey2Long READ funcKey2Long WRITE setFuncKey2Long)
  /** The short-press action for the programmable function key 3 (TK). */
  Q_PROPERTY(Function funcKey3Short READ funcKey3Short WRITE setFuncKey3Short)
  /** The long-press action for the programmable function key 3 (TK). */
  Q_PROPERTY(Function funcKey3Long READ funcKey3Long WRITE setFuncKey3Long)

public:
  /** Possible function key actions. Not all functions are present on all devices. */
  enum class Function {
    None, ToggleAllAlertTones, EmergencyOn, EmergencyOff, ToggleMonitor, OneTouch1,
    OneTouch2, OneTouch3, OneTouch4, OneTouch5, OneTouch6, ToggleTalkaround, ToggleScan,
    ToggleEncryption, ToggleVox, ZoneSelect, BatteryIndicator, ToggleLoneWorker, PhoneExit,
    ToggleFlashLight, ToggleFMRadio, RadioEnable, RadioCheck, RadioDisable, PowerLevel, TBST,
    CallSwell
  };
  Q_ENUM(Function)

public:
  explicit RadioddityButtonSettingsExtension(QObject *parent=nullptr);

  ConfigItem *clone() const;

  /** Returns the long-press duration, usually specified in ms. */
  Interval longPressDuration() const;
  /** Sets the long-press duration, usually specified in ms. */
  void setLongPressDuration(Interval interval);

  /** Returns the short-press function of the progammable function key 1 (SK1, P1). */
  Function funcKey1Short() const;
  /** Sets the short-press function of the progammable function key 1 (SK1, P1). */
  void setFuncKey1Short(Function func);
  /** Returns the long-press function of the progammable function key 1 (SK1, P1). */
  Function funcKey1Long() const;
  /** Sets the long-press function of the progammable function key 1 (SK1, P1). */
  void setFuncKey1Long(Function func);

  /** Returns the short-press function of the progammable function key 2 (SK2, P2). */
  Function funcKey2Short() const;
  /** Sets the short-press function of the progammable function key 2 (SK2, P2). */
  void setFuncKey2Short(Function func);
  /** Returns the long-press function of the progammable function key 2 (SK2, P2). */
  Function funcKey2Long() const;
  /** Sets the long-press function of the progammable function key 2 (SK2, P2). */
  void setFuncKey2Long(Function func);

  /** Returns the short-press function of the progammable function key 3 (TK). */
  Function funcKey3Short() const;
  /** Sets the short-press function of the progammable function key 3 (TK). */
  void setFuncKey3Short(Function func);
  /** Returns the long-press function of the progammable function key 3 (TK). */
  Function funcKey3Long() const;
  /** Sets the long-press function of the progammable function key 3 (TK). */
  void setFuncKey3Long(Function func);

protected:
  /** The long-press duration. */
  Interval _longPressDuration;
  /** The short-press action for the programmable function key 1 (SK1, P1). */
  Function _funcKey1Short;
  /** The long-press action for the programmable function key 1 (SK1, P1). */
  Function _funcKey1Long;
  /** The short-press action for the programmable function key 2 (SK2, P2). */
  Function _funcKey2Short;
  /** The long-press action for the programmable function key 2 (SK2, P2). */
  Function _funcKey2Long;
  /** The short-press action for the programmable function key 3 (TK). */
  Function _funcKey3Short;
  /** The long-press action for the programmable function key 3 (TK). */
  Function _funcKey3Long;
};


/** Tone settings for Radioddity devices. */
class RadioddityToneSettingsExtension: public ConfigItem
{
  Q_OBJECT

  /** If @c true, the low battery warning is enabled. (GD-73 only) */
  Q_PROPERTY(bool lowBatteryWarn READ lowBatteryWarn WRITE enableLowBatteryWarn)
  /** The low-battery warn interval in seconds. */
  Q_PROPERTY(Interval lowBatteryWarnInterval READ lowBatteryWarnInterval WRITE setLowBatteryWarnInterval)
  /** Returns the low-battery warning volume [1,10]. (GD-73 only)*/
  Q_PROPERTY(bool lowBatteryWarnVolume READ lowBatteryWarnVolume WRITE setLowBatteryWarnVolume)
  /** If @c true, the key tones are enabled. */
  Q_PROPERTY(bool keyTone READ keyTone WRITE enableKeyTone)
  /** Returns the key-tone volume. */
  Q_PROPERTY(bool keyToneVolume READ keyToneVolume WRITE setKeyToneVolume)
  /** The call-alert duration in seconds. */
  Q_PROPERTY(Interval callAlertDuration READ callAlertDuration WRITE setCallAlertDuration)
  /** If @c true the reset tone is enabled. */
  Q_PROPERTY(bool resetTone READ resetTone WRITE enableResetTone)
  /** @c true, the unknown number tone is enabled. */
  Q_PROPERTY(bool unknownNumberTone READ unknownNumberTone WRITE enableUnknownNumberTone)
  /** The ARTS tone mode. */
  Q_PROPERTY(ARTSTone artsToneMode READ artsToneMode WRITE setARTSToneMode)
  /** If @c true, the talk permit tone is enabled for digital channels. */
  Q_PROPERTY(bool digitalTalkPermitTone READ digitalTalkPermitTone WRITE enableDigitalTalkPermitTone)
  /** If @c true, the talk permit tone is enabled for analog channels. */
  Q_PROPERTY(bool analogTalkPermitTone READ analogTalkPermitTone WRITE enableAnalogTalkPermitTone)
  /** If @c true, the self-test tone is enabled. */
  Q_PROPERTY(bool selftestTone READ selftestTone WRITE enableSelftestTone)
  /** If @c true, the frequency indication tone is enabled. */
  Q_PROPERTY(bool channelFreeIndicationTone READ channelFreeIndicationTone WRITE enableChannelFreeIndicationTone)
  /** If @c true, all tones are disabled. */
  Q_PROPERTY(bool allTonesDisabled READ allTonesDisabled WRITE disableAllTones)
  /** If @c true, the TX exit tone is enabled. */
  Q_PROPERTY(bool txExitTone READ txExitTone WRITE enableTXExitTone)
  /** Sets the FM mic gain (GD-73 only). */
  Q_PROPERTY(unsigned int fmMicGain READ fmMicGain WRITE setFMMicGain);

public:
  /** Possible ARTS tone settings. */
  enum class ARTSTone {
    Disabled = 0,        ///< ARTS tone is disabled.
    Once     = 4,        ///< ARTS tone once.
    Always   = 8         ///< ARTS tone always.
  };
  Q_ENUM(ARTSTone)

public:
  /** Default constructor. */
  explicit RadioddityToneSettingsExtension(QObject *parent=nullptr);

  ConfigItem *clone() const;

  /** Returns @c true if a low battery charge is indicated by a warning. */
  bool lowBatteryWarn() const;
  /** Enables/disables low-battery warning. */
  void enableLowBatteryWarn(bool enable);
  /** Returns the low-battery warn interval in seconds. */
  Interval lowBatteryWarnInterval() const;
  /** Sets the low-battery warn interval in seconds. */
  void setLowBatteryWarnInterval(Interval sec);
  /** Returns the volume of the low-battery warning tone [1,10]. */
  unsigned int lowBatteryWarnVolume() const;
  /** Sets the volume of the low-battery warning tone [1,10]. */
  void setLowBatteryWarnVolume(unsigned int);

  /** Retruns @c true, if the key tones are enabled. */
  bool keyTone() const;
  /** Enables/disables key tones. */
  void enableKeyTone(bool enable);
  /** Retruns the key-tone volume. */
  unsigned int keyToneVolume() const;
  /** Sets the key-tone volume. */
  void setKeyToneVolume(unsigned int volume);

  /** Returns the call-alert duration in seconds. */
  Interval callAlertDuration() const;
  /** Sets the call-allert duration in seconds. */
  void setCallAlertDuration(Interval sec);

  /** Returns @c true if the reset tone is enabled. */
  bool resetTone() const;
  /** Enables/disables reset tone. */
  void enableResetTone(bool enable);

  /** Returns @c true if the unknown number tone is enabled. */
  bool unknownNumberTone() const;
  /** Enables/disables reset tone. */
  void enableUnknownNumberTone(bool enable);

  /** Returns the ARTS tone mode. */
  ARTSTone artsToneMode() const;
  /** Sets the ARTS tone mode. */
  void setARTSToneMode(ARTSTone mode);

  /** Returns @c true if the digital channel talk permit tone is enabled. */
  bool digitalTalkPermitTone() const;
  /** Enables/disables digital channel talk permit tone. */
  void enableDigitalTalkPermitTone(bool enable);
  /** Returns @c true if the analog channel talk permit tone is enabled. */
  bool analogTalkPermitTone() const;
  /** Enables/disables analog channel talk permit tone. */
  void enableAnalogTalkPermitTone(bool enable);

  /** Returns @c true if the self-test tone is enabled. */
  bool selftestTone() const;
  /** Enables/disables self-test tone. */
  void enableSelftestTone(bool enable);

  /** Returns @c true if the channel-free indication tone is enabled. */
  bool channelFreeIndicationTone() const;
  /** Enables/disables the channel free indication tone. */
  void enableChannelFreeIndicationTone(bool enable);

  /** Returns @c true if all tones are disabled. */
  bool allTonesDisabled() const;
  /** Disables/enables all tones. */
  void disableAllTones(bool disable);

  /** Returns @c true if the TX exit tone is enabled. */
  bool txExitTone() const;
  /** Enables/disables TX exit tone. */
  void enableTXExitTone(bool enable);

  /** Returns the FM mic gain [1,10]. */
  unsigned int fmMicGain() const;
  /** Sets the FM mic gain [1,10]. */
  void setFMMicGain(unsigned int gain);

protected:
  /** If @c true, a low-battery charge is indicated by a warning. */
  bool _lowBatteryWarn;
  /** Holds the low-battery warn interval in seconds. */
  Interval _lowBatteryWarnInterval;
  /** Holds the volume of the low-battery warning tone. */
  unsigned int _lowBatteryWarnVolume;
  /** Holds the call alert duration in seconds. */
  Interval _callAlertDuration;
  /** If @c true the reset tone is enabled. */
  bool _resetTone;
  /** If @c true, the unknown number tone is enabled. */
  bool _unknownNumberTone;
  /** Holds the ARTS tone mode. */
  ARTSTone _artsToneMode;
  /** If @c true, the talk permit tone is enabled for digital channels. */
  bool _digitalTalkPermitTone;
  /** If @c true, the talk permit tone is enabled for analog channels. */
  bool _analogTalkPermitTone;
  /** If @c true, the self-test tone is enabled. */
  bool _selftestTone;
  /** If @c true, the channel free indication tone is enabled. */
  bool _channelFreeIndicationTone;
  /** If @c true, all tones are disabled. */
  bool _disableAllTones;
  /** If @c true, the TX exit tone is enabled. */
  bool _txExitTone;
  /** If @c true, the key-tones are enabled. */
  bool _keyTone;
  /** Holds the key-tone volume [1,10]. */
  unsigned int _keyToneVolume;
  /** The FM mic gain. */
  unsigned int _fmMicGain;
};

/** Represents the boot settings for Radioddity devices.
 * This settings extension is part of the RadiodditySettingsExtension. */
class RadioddityBootSettingsExtension: public ConfigItem
{
  Q_OBJECT

  /** The boot display mode. */
  Q_PROPERTY(DisplayMode display READ display WRITE setDisplay)
  /** The programming password, disabled if empty. */
  Q_PROPERTY(QString bootPassword READ bootPassword WRITE setBootPassword)
  /** The programming password, disabled if empty. */
  Q_PROPERTY(QString progPassword READ progPassword WRITE setProgPassword)

public:
  /** Possible boot display modes. */
  enum class DisplayMode {
    None, Text, Image
  };
  Q_ENUM(DisplayMode)

public:
  /** Default constructor. */
  explicit RadioddityBootSettingsExtension(QObject *parent=nullptr);

  ConfigItem *clone() const;

  /** Returns the boot display mode. */
  DisplayMode display() const;
  /** Sets the boot display mode. */
  void setDisplay(DisplayMode mode);

  /** Returns the boot password. */
  const QString &bootPassword() const;
  /** Sets the boot password. */
  void setBootPassword(const QString &pwd);

  /** Returns the programming password. */
  const QString &progPassword() const;
  /** Sets the programming password. */
  void setProgPassword(const QString &pwd);

protected:
  /** The boot display mode. */
  DisplayMode _displayMode;
  /** Holds the boot password, disabled if empty. */
  QString _bootPasswd;
  /** Holds the programming password, disabled if empty. */
  QString _progPasswd;
};


/** Represents the general settings extension for Radioddity devices.
 * @ingroup radioddity */
class RadiodditySettingsExtension: public ConfigExtension
{
  Q_OBJECT

  /** The monitor type. */
  Q_PROPERTY(MonitorType monitorType READ monitorType WRITE setMonitorType)
  /** The lone-worker response time in minutes. */
  Q_PROPERTY(Interval loneWorkerResponseTime READ loneWorkerResponseTime WRITE setLoneWorkerResponseTime)
  /** The lonw-worker reminder period in seconds. */
  Q_PROPERTY(Interval loneWorkerReminderPeriod READ loneWorkerReminderPeriod WRITE setLoneWorkerReminderPeriod)
  /** The group-call hang-time in ms. */
  Q_PROPERTY(Interval groupCallHangTime READ groupCallHangTime WRITE setGroupCallHangTime)
  /** The private-call hang-time in ms. */
  Q_PROPERTY(Interval privateCallHangTime READ privateCallHangTime WRITE setPrivateCallHangTime)
  /** If @c true the down-channel mode is VFO. */
  Q_PROPERTY(bool downChannelModeVFO READ downChannelModeVFO WRITE enableDownChannelModeVFO)
  /** If @c true the up-channel mode is VFO. */
  Q_PROPERTY(bool upChannelModeVFO READ upChannelModeVFO WRITE enableUpChannelModeVFO)
  /** If @c true, the power save mode is enabled. */
  Q_PROPERTY(bool powerSaveMode READ powerSaveMode WRITE enablePowerSaveMode)
  Q_CLASSINFO("powerSaveModeDescription", "Puts the radio into sleep-mode when idle.")
  Q_CLASSINFO("powerSaveModeLongDescription",
              "When enabled, the radio enters a sleep mode when idle. That is, when on receive and "
              "there is no activity on the current channel. However, the radio may need some time "
              "to wake up from this mode. Hence, the 'wakeupPreamble' need to be enabled by all "
              "radios in the network to provide this wake-up delay.")
  /** If @c true, a wakeup preamble is sent. */
  Q_PROPERTY(bool wakeupPreamble READ wakeupPreamble WRITE enableWakeupPreamble)
  /** The preamble duration in ms. */
  Q_PROPERTY(Interval preambleDuration READ preambleDuration WRITE setPreambleDuration)
  Q_CLASSINFO("wakeupPreambleDescription", "If enabled, the radio will transmit a short wake-up "
              "preamble before each call.")
  /** If @c true, all LEDs are disabled. */
  Q_PROPERTY(bool allLEDsDisabled READ allLEDsDisabled WRITE disableAllLEDs)
  /** If @c true, the quick-key override is inhibited. */
  Q_PROPERTY(bool quickKeyOverrideInhibited READ quickKeyOverrideInhibited WRITE inhibitQuickKeyOverride)
  /** If @c true, the radio will transmit on the active channel when double-wait is enabled. */
  Q_PROPERTY(bool txOnActiveChannel READ txOnActiveChannel WRITE enableTXOnActiveChannel)
  /** The scan mode. */
  Q_PROPERTY(ScanMode scanMode READ scanMode WRITE setScanMode)
  /** The repeater end delay in seconds. */
  Q_PROPERTY(Interval repeaterEndDelay READ repeaterEndDelay WRITE setRepeaterEndDelay)
  /** The repeater STE in seconds. */
  Q_PROPERTY(Interval repeaterSTE READ repeaterSTE WRITE setRepeaterSTE)

  /** The button settings. */
  Q_PROPERTY(RadioddityButtonSettingsExtension *buttons READ buttons)
  /** The tone settings. */
  Q_PROPERTY(RadioddityToneSettingsExtension *tone READ tone)
  /** The boot settings. */
  Q_PROPERTY(RadioddityBootSettingsExtension *boot READ boot)

public:
  /** Possible monitor types. */
  enum class MonitorType {
    Open = 0,            ///< Monitoring by opening the squelch.
    Silent = 1           ///< Silent monitoring.
  };
  Q_ENUM(MonitorType)

  /** Possible scan modes. */
  enum class ScanMode {
    Time    = 0,
    Carrier = 1,
    Search  = 2
  };
  Q_ENUM(ScanMode)

public:
  /** Default constructor. */
  Q_INVOKABLE explicit RadiodditySettingsExtension(QObject *parent=nullptr);

  ConfigItem *clone() const;

  /** Returns the preamble duration in ms. */
  Interval preambleDuration() const;
  /** Sets the preamble duration in ms. */
  void setPreambleDuration(Interval ms);

  /** Returns the monitor type. */
  MonitorType monitorType() const;
  /** Sets the monitor type. */
  void setMonitorType(MonitorType type);

  /** Returns the lone-worker response time in minutes. */
  Interval loneWorkerResponseTime() const;
  /** Sets the lone-worker response time in minutes. */
  void setLoneWorkerResponseTime(Interval min);
  /** Returns the lone-worker reminder period in seconds. */
  Interval loneWorkerReminderPeriod() const;
  /** Sets the lone-worker reminder period in seconds. */
  void setLoneWorkerReminderPeriod(Interval sec);

  /** Returns the group call hang time in ms. */
  Interval groupCallHangTime() const;
  /** Sets the group call hang time in ms. */
  void setGroupCallHangTime(Interval ms);
  /** Returns the private call hang time in ms. */
  Interval privateCallHangTime() const;
  /** Sets the private call hang time in ms. */
  void setPrivateCallHangTime(Interval ms);

  /** Returns @c true if the down-channel mode is VFO. */
  bool downChannelModeVFO() const;
  /** Enables/disables down-channel mode is VFO. */
  void enableDownChannelModeVFO(bool enable);
  /** Returns @c true if the up-channel mode is VFO. */
  bool upChannelModeVFO() const;
  /** Enables/disables up-channel mode is VFO. */
  void enableUpChannelModeVFO(bool enable);


  /** Returns @c true if the power save mode is enabled. */
  bool powerSaveMode() const;
  /** Enables the power save mode. */
  void enablePowerSaveMode(bool enable);
  /** Returns @c true if the wake-up preamble is sent. */
  bool wakeupPreamble() const;
  /** Enables transmission of wakeup preamble. */
  void enableWakeupPreamble(bool enable);

  /** Returns @c true if all LEDs are disabled. */
  bool allLEDsDisabled() const;
  /** Disables/enables all LEDs. */
  void disableAllLEDs(bool disable);

  /** Returns true if quick-key override is inhibited. */
  bool quickKeyOverrideInhibited() const;
  /** Inhibits quick-key override. */
  void inhibitQuickKeyOverride(bool inhibit);

  /** Returns @c true if the radio transmits on the active channel on double monitor. */
  bool txOnActiveChannel() const;
  /** Enables/disables transmission on active channel on double monitor. */
  void enableTXOnActiveChannel(bool enable);

  /** Returns the scan mode. */
  ScanMode scanMode() const;
  /** Sets the scan mode. */
  void setScanMode(ScanMode mode);

  /** Returns the repeater end delay in seconds. */
  Interval repeaterEndDelay() const;
  /** Sets the repeater end delay in seconds. */
  void setRepeaterEndDelay(Interval delay);
  /** Returns the repeater STE in seconds. */
  Interval repeaterSTE() const;
  /** Sets the repeater STE in seconds. */
  void setRepeaterSTE(Interval ste);

  /** Returns a weak reference to the button settings. */
  RadioddityButtonSettingsExtension *buttons() const;
  /** Returns a weak reference to the tone settings. */
  RadioddityToneSettingsExtension *tone() const;
  /** Returns a weak reference to the boot settings. */
  RadioddityBootSettingsExtension *boot() const;

protected:
  /** Holds the preamble duration in ms. */
  Interval _preambleDuration;
  /** Holds the monitor type. */
  MonitorType _monitorType;
  /** Holds the lone-worker response time in minutes. */
  Interval _loneWorkerResponseTime;
  /** Holds the lone-worker reminder period in seconds. */
  Interval _loneWorkerReminderPeriod;
  /** The group-call hang-time in ms. */
  Interval _groupCallHangTime;
  /** The private-call hang-time in ms. */
  Interval _privateCallHangTime;
  /** If @c true down-channel mode is VFO. */
  bool _downChannelModeVFO;
  /** If @c true the up-channel mode is VFO. */
  bool _upChannelModeVFO;
  /** If @c true, the power save mode is enabled. */
  bool _powerSaveMode;
  /** If @c true, the wake-up preamble is sent. */
  bool _wakeupPreamble;
  /** If @c true, all LEDs are disabled. */
  bool _disableAllLEDs;
  /** If @c true, the quick-key override is inhibited. */
  bool _quickKeyOverrideInhibited;
  /** If @c true, the radio will transmit on the active channel when double-wait is enabled. */
  bool _txOnActiveChannel;
  /** Holds the scan mode. */
  ScanMode _scanMode;
  /** Holds the repeater end delay in seconds. */
  Interval _repeaterEndDelay;
  /** Holds the repeater STE in seconds. */
  Interval _repeaterSTE;
  /** Button settings. */
  RadioddityButtonSettingsExtension *_buttonSettings;
  /** Tone settings. */
  RadioddityToneSettingsExtension *_toneSettings;
  /** Boot settings. */
  RadioddityBootSettingsExtension *_bootSettings;
};

#endif // RADIODDITYEXTENSIONS_HH
