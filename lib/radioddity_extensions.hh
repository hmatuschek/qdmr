#ifndef RADIODDITYEXTENSIONS_HH
#define RADIODDITYEXTENSIONS_HH

#include "configobject.hh"

/** Represents the general settings extension for Radioddity devices.
 * @ingroup radioddity */
class RadiodditySettingsExtension: public ConfigExtension
{
  Q_OBJECT

  /** The preamble duration in ms. */
  Q_PROPERTY(unsigned preambleDuration READ preambleDuration WRITE setPreambleDuration)
  /** The monitor type. */
  Q_PROPERTY(MonitorType monitorType READ monitorType WRITE setMonitorType)
  /** The low-battery warn interval in seconds. */
  Q_PROPERTY(unsigned lowBatteryWarnInterval READ lowBatteryWarnInterval WRITE setLowBatteryWarnInterval)
  /** The call-alert duration in seconds. */
  Q_PROPERTY(unsigned callAlertDuration READ callAlertDuration WRITE setCallAlertDuration)
  /** The lone-worker response time in minutes. */
  Q_PROPERTY(unsigned loneWorkerResponseTime READ loneWorkerResponseTime WRITE setLoneWorkerResponseTime)
  /** The lonw-worker reminder period in seconds. */
  Q_PROPERTY(unsigned loneWorkerReminderPeriod READ loneWorkerReminderPeriod WRITE setLoneWorkerReminderPeriod)
  /** The group-call hang-time in ms. */
  Q_PROPERTY(unsigned groupCallHangTime READ groupCallHangTime WRITE setGroupCallHangTime)
  /** The private-call hang-time in ms. */
  Q_PROPERTY(unsigned privateCallHangTime READ privateCallHangTime WRITE setPrivateCallHangTime)
  /** If @c true the down-channel mode is VFO. */
  Q_PROPERTY(bool downChannelModeVFO READ downChannelModeVFO WRITE enableDownChannelModeVFO)
  /** If @c true the up-channel mode is VFO. */
  Q_PROPERTY(bool upChannelModeVFO READ upChannelModeVFO WRITE enableUpChannelModeVFO)
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
  Q_PROPERTY(bool frequencyIndicationTone READ frequencyIndicationTone WRITE enableFrequencyIndicationTone)
  /** If @c true, all tones are disabled. */
  Q_PROPERTY(bool allTonesDisabled READ allTonesDisabled WRITE disableAllTones)
  /** If @c true, the power save mode is enabled. */
  Q_PROPERTY(bool powerSaveMode READ powerSaveMode WRITE enablePowerSaveMode)
  Q_CLASSINFO("powerSaveModeDescription", "Puts the radio into sleep-mode when idle.")
  Q_CLASSINFO("powerSaveModeLongDescription",
              "When enabled, the radio enters a sleep mode when idle. That is, when on receive and "
              "there is no activity on the current channel. However, the radio may need some time "
              "to wake up from this mode. Hence, the 'wakeupPreamble' need to be enabled by all "
              "radios in the network to provide this wake-up delay.")
  /** If @c true, a wakeup preamble is send. */
  Q_PROPERTY(bool wakeupPreamble READ wakeupPreamble WRITE enableWakeupPreamble)
  Q_CLASSINFO("wakeupPreambleDescription", "If enabled, the radio will transmit a short wake-up "
              "preamble before each call.")
  /** If @c true, all LEDs are disabled. */
  Q_PROPERTY(bool allLEDsDisabled READ allLEDsDisabled WRITE disableAllLEDs)
  /** If @c true, the quick-key override is inhibited. */
  Q_PROPERTY(bool quickKeyOverrideInhibited READ quickKeyOverrideInhibited WRITE inhibitQuickKeyOverride)
  /** If @c true, the TX exit tone is enabled. */
  Q_PROPERTY(bool txExitTone READ txExitTone WRITE enableTXExitTone)
  /** If @c true, the radio will transmit on the active channel when double-wait is enabled. */
  Q_PROPERTY(bool txOnActiveChannel READ txOnActiveChannel WRITE enableTXOnActiveChannel)
  /** If @c true, the boot animation is enabled. */
  Q_PROPERTY(bool animation READ animation WRITE enableAnimation)
  /** The scan mode. */
  Q_PROPERTY(ScanMode scanMode READ scanMode WRITE setScanMode)
  /** The repeater end delay in seconds. */
  Q_PROPERTY(unsigned repeaterEndDelay READ repeaterEndDelay WRITE setRepeaterEndDelay)
  /** The repeater STE in seconds. */
  Q_PROPERTY(unsigned repeaterSTE READ repeaterSTE WRITE setRepeaterSTE)
  /** The programming password, disabled if empty. */
  Q_PROPERTY(QString progPassword READ progPassword WRITE setProgPassword)

public:
  /** Possible monitor types. */
  enum class MonitorType {
    Open = 0,            ///< Monitoring by opening the squelch.
    Silent = 1           ///< Silent monitoring.
  };
  Q_ENUM(MonitorType)

  /** Possible ARTS tone settings. */
  enum class ARTSTone {
    Disabled = 0,        ///< ARTS tone is disabled.
    Once     = 4,        ///< ARTS tone once.
    Always   = 8         ///< ARTS tone always.
  };
  Q_ENUM(ARTSTone)

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
  unsigned preambleDuration() const;
  /** Sets the preamble duration in ms. */
  void setPreambleDuration(unsigned ms);

  /** Returns the monitor type. */
  MonitorType monitorType() const;
  /** Sets the monitor type. */
  void setMonitorType(MonitorType type);

  /** Returns the low-battery warn interval in seconds. */
  unsigned lowBatteryWarnInterval() const;
  /** Sets the low-battery warn interval in seconds. */
  void setLowBatteryWarnInterval(unsigned sec);

  /** Returns the call-alert duration in seconds. */
  unsigned callAlertDuration() const;
  /** Sets the call-allert duration in seconds. */
  void setCallAlertDuration(unsigned sec);

  /** Returns the lone-worker response time in minutes. */
  unsigned loneWorkerResponseTime() const;
  /** Sets the lone-worker response time in minutes. */
  void setLoneWorkerResponseTime(unsigned min);
  /** Returns the lone-worker reminder period in seconds. */
  unsigned loneWorkerReminderPeriod() const;
  /** Sets the lone-worker reminder period in seconds. */
  void setLoneWorkerReminderPeriod(unsigned sec);

  /** Returns the group call hang time in ms. */
  unsigned groupCallHangTime() const;
  /** Sets the group call hang time in ms. */
  void setGroupCallHangTime(unsigned ms);
  /** Returns the private call hang time in ms. */
  unsigned privateCallHangTime() const;
  /** Sets the private call hang time in ms. */
  void setPrivateCallHangTime(unsigned ms);

  /** Returns @c true if the down-channel mode is VFO. */
  bool downChannelModeVFO() const;
  /** Enables/disables down-channel mode is VFO. */
  void enableDownChannelModeVFO(bool enable);
  /** Returns @c true if the up-channel mode is VFO. */
  bool upChannelModeVFO() const;
  /** Enables/disables up-channel mode is VFO. */
  void enableUpChannelModeVFO(bool enable);

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

  /** Returns @c true if the frequency indication tone is enabled. */
  bool frequencyIndicationTone() const;
  /** Enables/disables frequency indication tone. */
  void enableFrequencyIndicationTone(bool enable);

  /** Returns @c true if all tones are disabled. */
  bool allTonesDisabled() const;
  /** Disables/enables all tones. */
  void disableAllTones(bool disable);

  /** Returns @c true if the power save mode is enabled. */
  bool powerSaveMode() const;
  /** Enables the power save mode. */
  void enablePowerSaveMode(bool enable);
  /** Returns @c true if the wake-up preamble is send. */
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

  /** Returns @c true if the TX exit tone is enabled. */
  bool txExitTone() const;
  /** Enables/disables TX exit tone. */
  void enableTXExitTone(bool enable);

  /** Returns @c true if the radio transmits on the active channel on double monitor. */
  bool txOnActiveChannel() const;
  /** Enables/disables transmission on active channel on double monitor. */
  void enableTXOnActiveChannel(bool enable);

  /** Returns @c true if animation is enabled. */
  bool animation() const;
  /** Enables/disables animation. */
  void enableAnimation(bool enable);

  /** Returns the scan mode. */
  ScanMode scanMode() const;
  /** Sets the scan mode. */
  void setScanMode(ScanMode mode);

  /** Returns the repeater end delay in seconds. */
  unsigned repeaterEndDelay() const;
  /** Sets the repeater end delay in seconds. */
  void setRepeaterEndDelay(unsigned delay);
  /** Returns the repeater STE in seconds.. */
  unsigned repeaterSTE() const;
  /** Sets the repeater STE in seconds. */
  void setRepeaterSTE(unsigned ste);

  /** Returns the programming password. */
  const QString &progPassword() const;
  /** Sets the programming password. */
  void setProgPassword(const QString &pwd);

protected:
  /** Holds the preamble duration in ms. */
  unsigned _preambleDuration;
  /** Holds the monitor type. */
  MonitorType _monitorType;
  /** Holds the low-battery warn interval in seconds. */
  unsigned _lowBatteryWarnInterval;
  /** Holds the call alert duration in seconds. */
  unsigned _callAlertDuration;
  /** Holds the lone-worker response time in minutes. */
  unsigned _loneWorkerResponseTime;
  /** Holds the lone-worker reminder period in seconds. */
  unsigned _loneWorkerReminderPeriod;
  /** The group-call hang-time in ms. */
  unsigned _groupCallHangTime;
  /** The private-call hang-time in ms. */
  unsigned _privateCallHangTime;
  /** If @c true down-channel mode is VFO. */
  bool _downChannelModeVFO;
  /** If @c true the up-channel mode is VFO. */
  bool _upChannelModeVFO;
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
  /** If @c true, the frequency indication tone is enabled. */
  bool _frequencyIndicationTone;
  /** If @c true, all tones are disabled. */
  bool _disableAllTones;
  /** If @c true, the power save mode is enabled. */
  bool _powerSaveMode;
  /** If @c true, the wake-up preamble is send. */
  bool _wakeupPreamble;
  /** If @c true, all LEDs are disabled. */
  bool _disableAllLEDs;
  /** If @c true, the quick-key override is inhibited. */
  bool _quickKeyOverrideInhibited;
  /** If @c true, the TX exit tone is enabled. */
  bool _txExitTone;
  /** If @c true, the radio will transmit on the active channel when double-wait is enabled. */
  bool _txOnActiveChannel;
  /** If @c true, the boot animation is enabled. */
  bool _animation;
  /** Holds the scan mode. */
  ScanMode _scanMode;
  /** Holds the repeater end delay in seconds. */
  unsigned _repeaterEndDelay;
  /** Holds the repeater STE in seconds. */
  unsigned _repeaterSTE;
  /** Holds the programming password, disabled if empty. */
  QString _progPasswd;
};

#endif // RADIODDITYEXTENSIONS_HH
