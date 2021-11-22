#ifndef TYTEXTENSION_HH
#define TYTEXTENSION_HH

#include "configobject.hh"

/** Represents the TyT channel extension.
 *
 * That is, all device specific settings for TyT devices, that are not represented though the common
 * codeplug.
 *
 * @ingroup tyt */
class TyTChannelExtension: public ConfigExtension
{
  Q_OBJECT

  /** The lone worker feature. */
  Q_PROPERTY(bool loneWorker READ loneWorker WRITE enableLoneWorker)
  /** The auto scan feature. */
  Q_PROPERTY(bool autoScan READ autoScan WRITE enableAutoScan)
  /** The talk around feature. */
  Q_PROPERTY(bool talkaround READ talkaround WRITE enableTalkaround)
  /** If @c true, data call confirmation is enabled. */
  Q_PROPERTY(bool dataCallConfirmed READ dataCallConfirmed WRITE enableDataCallConfirmed)
  /** If @c true, private call confirmation is enabled. */
  Q_PROPERTY(bool privateCallConfirmed READ privateCallConfirmed WRITE enablePrivateCallConfirmed)
  /** If @c true, emergency calls are confirmed. */
  Q_PROPERTY(bool emergencyAlarmConfirmed READ emergencyAlarmConfirmed WRITE enableEmergencyAlarmConfirmed)
  /** If @c true, displays analog PTT IDs. */
  Q_PROPERTY(bool displayPTTId READ displayPTTId WRITE enableDisplayPTTId)
  /** Holds the reference frequency setting for RX. */
  Q_PROPERTY(RefFrequency rxRefFrequency READ rxRefFrequency WRITE setRXRefFrequency)
  /** Holds the reference frequency setting for TX. */
  Q_PROPERTY(RefFrequency txRefFrequency READ txRefFrequency WRITE setTXRefFrequency)

  /** The tight-squelch feature. */
  Q_PROPERTY(bool tightSquelch READ tightSquelch WRITE enableTightSquelch)
  /** The compressed UDP header feature. */
  Q_PROPERTY(bool compressedUDPHeader READ compressedUDPHeader WRITE enableCompressedUDPHeader)
  /** The reverse-burst feature. */
  Q_PROPERTY(bool reverseBurst READ reverseBurst WRITE enableReverseBurst)

  /** Holds the kill tone frequency. */
  Q_PROPERTY(KillTone killTone READ killTone WRITE setKillTone)
  /** Holds the in-call criterion. */
  Q_PROPERTY(InCallCriterion inCallCriterion READ inCallCriterion WRITE setInCallCriterion)
  /** Holds the allow-interrupt flag. */
  Q_PROPERTY(bool allowInterrupt READ allowInterrupt WRITE enableAllowInterrupt)
  /** If @c true, enables Dual-Capacity Direct Mode (DCDM, i.e., time-slots for simplex). */
  Q_PROPERTY(bool dcdm READ dcdm WRITE enableDCDM)
  /** If @c true, and dcdm is enabled, this radio is the leader, specifying the clock. */
  Q_PROPERTY(bool dcdmLeader READ dcdmLeader WRITE enableDCDMLeader)

  Q_CLASSINFO("description", "Settings for MD-390, RT8, MD-UV390, RT3S, MD-2017, RT82.")
  Q_CLASSINFO("longDescription", "Device specific channel settings for TyT and Retevis devices."
                                 "Including TyT MD-390, MD-UV390, MD-2017 as well as Retevis RT8, "
                                 "RT3S and RT82.")

public:
  /** Possible reference frequency settings for RX & TX. */
  enum class RefFrequency {
    Low=0, Medium=1, High=2
  };
  Q_ENUM(RefFrequency)

  /** Possible kill-tone settings. */
  enum class KillTone {
    Tone259_2Hz=0, Tone55_2Hz=1, Off=3
  };
  Q_ENUM(KillTone)

  /** Possible in-call criterions. */
  enum class InCallCriterion {
    Always = 0, AdmitCriterion=1, TXInterrupt=2
  };
  Q_ENUM(InCallCriterion)

public:
  /** Default constructor. */
  Q_INVOKABLE explicit TyTChannelExtension(QObject *parent=nullptr);

  ConfigItem *clone() const;

  /** Returns @c true if the lone worker feature is enabled. */
  bool loneWorker() const;
  /** Enables/disables the lone-worker feature. */
  void enableLoneWorker(bool enable);
  /** Returns @c true if the auto scan feature is enabled. */
  bool autoScan() const;
  /** Enables/disables the auto-scan feature. */
  void enableAutoScan(bool enable);
  /** Returns @c true if the talk around feature is enabled. */
  bool talkaround() const;
  /** Enables/disables the talk-around feature. */
  void enableTalkaround(bool enable);
  /** Returns @c true if data call confirmation is enabled. */
  bool dataCallConfirmed() const;
  /** Enables/disables data-call confirmation. */
  void enableDataCallConfirmed(bool enable);
  /** Returns @c true if private call confirmation is enabled. */
  bool privateCallConfirmed() const;
  /** Enables/disables private-call confirmation. */
  void enablePrivateCallConfirmed(bool enable);
  /** Returns @c true if emergency calls are confirmed. */
  bool emergencyAlarmConfirmed() const;
  /** Enables/disables emergency-call confirmation. */
  void enableEmergencyAlarmConfirmed(bool enable);
  /** Returns @c true if analog PTT IDs are shown. */
  bool displayPTTId() const;
  /** Enables/disables analog PTT ID display. */
  void enableDisplayPTTId(bool enable);
  /** Returns the reference frequency setting for RX. */
  RefFrequency rxRefFrequency() const;
  /** Sets the reference frequency setting for RX. */
  void setRXRefFrequency(RefFrequency ref);
  /** Returns the reference frequency setting for TX. */
  RefFrequency txRefFrequency() const;
  /** Sets the reference frequency setting for TX. */
  void setTXRefFrequency(RefFrequency ref);

  /** Returns @c true if the tight squelch is enabled. */
  bool tightSquelch() const;
  /** Enables/disables the tight squelch. */
  void enableTightSquelch(bool enable);
  /** Returns @c true if the compressed UDP header is enabled. */
  bool compressedUDPHeader() const;
  /** Enables/disables the compressed UDP header. */
  void enableCompressedUDPHeader(bool enable);
  /** Returns @c true if the reverse burst is enabled. */
  bool reverseBurst() const;
  /** Enables/disables reverse burst. */
  void enableReverseBurst(bool enable);

  /** Returns the kill tone frequency. */
  KillTone killTone() const;
  /** Sets the kill-tone frequency. */
  void setKillTone(KillTone tone);
  /** Returns the in-call criterion. */
  InCallCriterion inCallCriterion() const;
  /** Sets the in-call criterion. */
  void setInCallCriterion(InCallCriterion crit);
  /** Returns @c true if interrupt is allowed. */
  bool allowInterrupt() const;
  /** Enables/disables interrupt. */
  void enableAllowInterrupt(bool enable);
  /** Returns @c true if the DCDM mode is enabled. */
  bool dcdm() const;
  /** Enables/disables the DCDM mode. */
  void enableDCDM(bool enable);
  /** Returns @c true if this radio is the leader for a DCDM simplex channel. */
  bool dcdmLeader() const;
  /** Enables/disables this radio to be the leader on a DCDM simplex channel. */
  void enableDCDMLeader(bool enable);

public:
  ConfigItem *allocateChild(QMetaProperty &prop, const YAML::Node &node,
                            const Context &ctx, const ErrorStack &err=ErrorStack());

protected:
  // Common properties
  /** Holds the lone-worker flag. */
  bool _loneWorker;
  /** Holds the auto-scan flag. */
  bool _autoScan;
  /** Holds the talk around flag. */
  bool _talkaround;
  /** Holds the data-call confirmation flag. */
  bool _dataCallConfirmed;
  /** Holds the private-call confirmation flag. */
  bool _privateCallConfirmed;
  /** Holds the emergency-call confirmation flag. */
  bool _emergencyAlarmConfirmed;
  /** Holds the display PTT ID flag. */
  bool _displayPTTId;
  /** Holds the reference frequeny setting for RX. */
  RefFrequency _rxRefFrequency;
  /** Holds the reference frequeny setting for TX. */
  RefFrequency _txRefFrequency;

  // MD-390 properties
  /** Holds the tightSquelch flag. */
  bool _tightSquelch;
  /** Holds the compressed UDP header flag. */
  bool _compressedUDPHeader;
  /** Holds the reverse burst flag. */
  bool _reverseBurst;

  // MD-UV390, MD-2017 properties
  /** Holds the kill tone setting. */
  KillTone _killTone;
  /** Holds the in-call criterion. */
  InCallCriterion _inCallCriterion;
  /** Holds the interrupt flag. */
  bool _allowInterrupt;
  /** Holds the DCDM flag. */
  bool _dcdm;
  /** Holds the DCDM-leader flag. */
  bool _dcdmLeader;
};


/** Represents device specific scan-list settings for TyT devices.
 * @ingroup tyt */
class TyTScanListExtension: public ConfigExtension
{
  Q_OBJECT

  /** Holds the hold time in ms. */
  Q_PROPERTY(unsigned holdTime READ holdTime WRITE setHoldTime)
  /** Holds the sample time in ms for priority channels. */
  Q_PROPERTY(unsigned prioritySampleTime READ prioritySampleTime WRITE setPrioritySampleTime)

public:
  /** Default constructor. */
  Q_INVOKABLE explicit TyTScanListExtension(QObject *parent=nullptr);

  ConfigItem *clone() const;

  /** Returns the hold time in ms. */
  unsigned holdTime() const;
  /** Sets the hold time im ms. */
  void setHoldTime(unsigned ms);
  /** Returns the sample time for priority channels in ms. */
  unsigned prioritySampleTime() const;
  /** Sets the sample time for priority channels in ms. */
  void setPrioritySampleTime(unsigned ms);

public:
  ConfigItem *allocateChild(QMetaProperty &prop, const YAML::Node &node,
                            const Context &ctx, const ErrorStack &err=ErrorStack());

protected:
  /** The hold time in ms. */
  unsigned _holdTime;
  /** The sample time for priority channels in ms. */
  unsigned _prioritySampleTime;
};


/** Represents the TyT button settings extension.
 * @ingroup tyt */
class TyTButtonSettings : public ConfigExtension
{
  Q_OBJECT

  /** The action to perform on a short press on side button 1. */
  Q_PROPERTY(ButtonAction sideButton1Short MEMBER _sideButton1Short READ sideButton1Short WRITE setSideButton1Short)
  /** The action to perform on a long press on side button 1. */
  Q_PROPERTY(ButtonAction sideButton1Long MEMBER _sideButton1Long READ sideButton1Long WRITE setSideButton1Long)
  /** The action to perform on a short press on side button 2. */
  Q_PROPERTY(ButtonAction sideButton2Short MEMBER _sideButton2Short READ sideButton2Short WRITE setSideButton2Short)
  /** The action to perform on a long press on side button 2. */
  Q_PROPERTY(ButtonAction sideButton2Long MEMBER _sideButton2Long READ sideButton2Long WRITE setSideButton2Long)
  /** The duration of a long press in msec. */
  Q_PROPERTY(unsigned longPressDuration MEMBER _longPressDuration READ longPressDuration WRITE setLongPressDuration)

public:
  /** Possible actions for the side-buttons. */
  enum ButtonAction {
    Disabled = 0,                       ///< Disabled side-button action.
    ToggleAllAlertTones = 1,            ///< Toggle all alert tones.
    EmergencyOn = 2,                    ///< Enable emergency.
    EmergencyOff = 3,                   ///< Disable emergency.
    PowerSelect = 4,                    ///< Select TX power.
    MonitorToggle = 5,                  ///< Toggle monitor (promiscuous mode on digital channel, open squelch on analog channel).
    NuisanceDelete = 6,                 ///< Nuisance delete.
    OneTouch1 = 7,                      ///< Perform one-touch action 1.
    OneTouch2 = 8,                      ///< Perform one-touch action 2.
    OneTouch3 = 9,                      ///< Perform one-touch action 3.
    OneTouch4 = 10,                     ///< Perform one-touch action 4.
    OneTouch5 = 11,                     ///< Perform one-touch action 5.
    OneTouch6 = 12,                     ///< Perform one-touch action 6.
    RepeaterTalkaroundToggle = 13,      ///< Toggle repater mode / talkaround.
    ScanToggle = 14,                    ///< Start/stop scan.
    SquelchToggle = 21,                 ///< Enable/disable squelch.
    PrivacyToggle = 22,                 ///< Enable/disable privacy system.
    VoxToggle = 23,                     ///< Enable/disable VOX.
    ZoneIncrement = 24,                 ///< Switch to next zone.
    BatteryIndicator = 26,              ///< Show battery charge.
    ManualDialForPrivate = 30,          ///< Manual dial for private.
    LoneWorkerToggle = 31,              ///< Toggle lone-worker.
    RecordToggle = 34,                  ///< Enable/disable recording (dep. on firmware).
    RecordPlayback = 35,                ///< Start/stop playback.
    RecordDeleteAll = 36,               ///< Delete all recordings.
    Tone1750Hz = 38,                    ///< Send 1750Hz tone.
    SwitchUpDown = 47,                  ///< Switch Channel A/B.
    RightKey = 48,                      ///< Who knows?
    LeftKey = 49,                       ///< Who knows?
    ZoneDecrement = 55                  ///< Switch to previous zone.
  };
  Q_ENUM(ButtonAction)

public:
  /** Constructor. */
  Q_INVOKABLE explicit TyTButtonSettings(QObject *parent=nullptr);

  ConfigItem *clone() const;

  /** Returns the action for the side button 1 short-press. */
  ButtonAction sideButton1Short() const;
  /** Sets the action for the side button 1 short-press. */
  void setSideButton1Short(ButtonAction action);
  /** Returns the action for the side button 1 long-press. */
  ButtonAction sideButton1Long() const;
  /** Sets the action for the side button 1 long-press. */
  void setSideButton1Long(ButtonAction action);

  /** Returns the action for the side button 2 short-press. */
  ButtonAction sideButton2Short() const;
  /** Sets the action for the side button 2 short-press. */
  void setSideButton2Short(ButtonAction action);
  /** Returns the action for the side button 2 long-press. */
  ButtonAction sideButton2Long() const;
  /** Sets the action for the side button 2 long-press. */
  void setSideButton2Long(ButtonAction action);

  /** Returns the long-press duration in msec. */
  unsigned longPressDuration() const;
  /** Sets the long-press duration in msec. */
  void setLongPressDuration(unsigned dur);

public:
  ConfigItem *allocateChild(QMetaProperty &prop, const YAML::Node &node,
                            const Context &ctx, const ErrorStack &err=ErrorStack());

protected:
  /** Holds the side button 1 short-press action. */
  ButtonAction _sideButton1Short;
  /** Holds the side button 1 long-press action. */
  ButtonAction _sideButton1Long;
  /** Holds the side button 2 short-press action. */
  ButtonAction _sideButton2Short;
  /** Holds the side button 2 long-press action. */
  ButtonAction _sideButton2Long;
  /** Holds the long-press duration in ms. */
  unsigned _longPressDuration;
};


/** Represents the TyT general settings extension.
 * @ingroup tyt */
class TyTSettingsExtension: public ConfigExtension
{
  Q_OBJECT

  /** The monitor type setting. */
  Q_PROPERTY(MonitorType monitorType READ monitorType WRITE setMonitorType)
  /** If @c true, all LEDs are disabled. */
  Q_PROPERTY(bool allLEDsDisabled READ allLEDsDisabled WRITE disableAllLEDs)
  /** If @c true, the talk permit tone for digital channels is enabled. */
  Q_PROPERTY(bool talkPermitToneDigital READ talkPermitToneDigital WRITE enableTalkPermitToneDigital)
  /** If @c true, the talk permit tone for analog channels is enabled. */
  Q_PROPERTY(bool talkPermitToneAnalog READ talkPermitToneAnalog WRITE enableTalkPermitToneAnalog)
  /** If @c true, the password and lock is enabled. */
  Q_PROPERTY(bool passwordAndLock READ passwordAndLock WRITE enablePasswordAndLock)
  /** If @c true, the channel free tone is enabled. */
  Q_PROPERTY(bool channelFreeIndicationTone READ channelFreeIndicationTone WRITE enableChannelFreeIndicationTone)
  /** If @c true, all tones are disabled. */
  Q_PROPERTY(bool allTonesDisabled READ allTonesDisabled WRITE disableAllTones)
  /** If @c true, the save-mode RX is enabled. */
  Q_PROPERTY(bool saveModeRX READ saveModeRX WRITE enableSaveModeRX)
  /** If @c true, the save preamble is enabled. */
  Q_PROPERTY(bool savePreamble READ savePreamble WRITE enableSavePreamble)
  /** If @c true, a picture is shown during boot. */
  Q_PROPERTY(bool bootPicture READ bootPicture WRITE enableBootPicture)
  /** The transmit preamble duration in ms. */
  Q_PROPERTY(unsigned txPreambleDuration READ txPreambleDuration WRITE setTXPreambleDuration)
  /** The group hang time in ms. */
  Q_PROPERTY(unsigned groupCallHangTime READ groupCallHangTime WRITE setGroupCallHangTime)
  /** The private hang time in ms. */
  Q_PROPERTY(unsigned privateCallHangTime READ privateCallHangTime WRITE setPrivateCallHangTime)
  /** The low battery warn interval in seconds. */
  Q_PROPERTY(unsigned lowBatteryWarnInterval READ lowBatteryWarnInterval WRITE setLowBatteryWarnInterval)
  /** If @c true, the call alert-tone is continuous. */
  Q_PROPERTY(bool callAlertToneContinuous READ callAlertToneContinuous WRITE enableCallAlertToneContinuous)
  /** The call alert duration in seconds. */
  Q_PROPERTY(unsigned callAlertToneDuration READ callAlertToneDuration WRITE setCallAlertToneDuration)
  /** The lone-worker response time in minutes. */
  Q_PROPERTY(unsigned loneWorkerResponseTime READ loneWorkerResponseTime WRITE setLoneWorkerResponseTime)
  /** The lone-worker reminder time in seconds. */
  Q_PROPERTY(unsigned loneWorkerReminderTime READ loneWorkerReminderTime WRITE setLoneWorkerReminderTime)
  /** The digital channel scan hang time in ms. */
  Q_PROPERTY(unsigned digitalScanHangTime READ digitalScanHangTime WRITE setDigitalScanHangTime)
  /** The analog channel scan hang time in ms. */
  Q_PROPERTY(unsigned analogScanHangTime READ analogScanHangTime WRITE setAnalogScanHangTime)
  /** If @c true, the backlight is always on. */
  Q_PROPERTY(bool backlightAlwaysOn READ backlightAlwaysOn WRITE enableBacklightAlwaysOn)
  /** If @c backlightAlwaysOn is @c false, specifies the backlight duration in seconds. */
  Q_PROPERTY(unsigned backlightDuration READ backlightDuration WRITE setBacklightDuration)
  /** If @c true, the keypad is locked manually. */
  Q_PROPERTY(bool keypadLockManual READ keypadLockManual WRITE enableKeypadLockManual)
  /** If @c keypadLockManual is @c false, specifies the keypad lock time. */
  Q_PROPERTY(unsigned keypadLockTime READ keypadLockTime WRITE setKeypadLockTime)
  /** If @c true the power-on password is enabled. */
  Q_PROPERTY(bool powerOnPasswordEnabled READ powerOnPasswordEnabled WRITE enablePowerOnPassword)
  /** If @c powerOnPasswordEnabled is @c true, specifies the power-on password. */
  Q_PROPERTY(unsigned powerOnPassword READ powerOnPassword WRITE setPowerOnPassword)
  /** If @c true the radio programming password is enabled. */
  Q_PROPERTY(bool radioProgPasswordEnabled READ radioProgPasswordEnabled WRITE enableRadioProgPassword)
  /** If @c radioProgPasswordEnabled is @c true, specifies the radio programming password. */
  Q_PROPERTY(unsigned radioProgPassword READ radioProgPassword WRITE setRadioProgPassword)
  /** Specifies the PC programming password. */
  Q_PROPERTY(QString pcProgPassword READ pcProgPassword WRITE setPCProgPassword)

  Q_CLASSINFO("description", "Settings for MD-390, RT8, MD-UV390, RT3S, MD-2017, RT82.")
  Q_CLASSINFO("longDescription", "Device specific radio settings for TyT and Retevis devices."
                                 "Including TyT MD-390, MD-UV390, MD-2017 as well as Retevis RT8, "
                                 "RT3S and RT82.")

public:
  /** Possible monitor types. */
  enum class MonitorType {
    Silent=0, Open=1
  };
  Q_ENUM(MonitorType)

public:
  /** Default constructor. */
  Q_INVOKABLE explicit TyTSettingsExtension(QObject *parent=nullptr);

  ConfigItem *clone() const;

  /** Returns the monitor type. */
  MonitorType monitorType() const;
  /** Sets the monitor type. */
  void setMonitorType(MonitorType type);

  /** Returns @c true if all LEDs are disabled. */
  bool allLEDsDisabled() const;
  /** Disables all LEDs. */
  void disableAllLEDs(bool disable);

  /** Returns @c true if the talk permit tone is enabled for digital channels.*/
  bool talkPermitToneDigital() const;
  /** Enables the talk permit tone for digital channels. */
  void enableTalkPermitToneDigital(bool enable);

  /** Returns @c true if the talk permit tone is enabled for analog channels.*/
  bool talkPermitToneAnalog() const;
  /** Enables the talk permit tone for analog channels. */
  void enableTalkPermitToneAnalog(bool enable);

  /** Returns @c true if the password and lock is enabled. */
  bool passwordAndLock() const;
  /** Enables the password and lock. */
  void enablePasswordAndLock(bool enable);

  /** Returns @c true if channel-free indication tone is enabled. */
  bool channelFreeIndicationTone() const;
  /** Enables channel-free indication tone. */
  void enableChannelFreeIndicationTone(bool enable);

  /** Returns @c true if all tones are disabled. */
  bool allTonesDisabled() const;
  /** Disables all tones. */
  void disableAllTones(bool disable);

  /** Returns @c true if the save mode RX is enabled. */
  bool saveModeRX() const;
  /** Enables the RX save-mode. */
  void enableSaveModeRX(bool enable);

  /** Returns @c true if the preamble is saved. */
  bool savePreamble() const;
  /** Enables preamble save. */
  void enableSavePreamble(bool enable);

  /** Returns @c true if a picture is shown during boot. */
  bool bootPicture() const;
  /** Enables the boot picture. */
  void enableBootPicture(bool enable);

  /** Returns the TX preamble duration in ms. */
  unsigned txPreambleDuration() const;
  /** Sets the TX preamble duration in ms. */
  void setTXPreambleDuration(unsigned ms);

  /** Returns the group call hang time in ms. */
  unsigned groupCallHangTime() const;
  /** Sets the group-call hang time in ms. */
  void setGroupCallHangTime(unsigned ms);

  /** Returns the private call hang time in ms. */
  unsigned privateCallHangTime() const;
  /** Sets the private-call hang time in ms. */
  void setPrivateCallHangTime(unsigned ms);

  /** Returns the low-battery warn interval in seconds. */
  unsigned lowBatteryWarnInterval() const;
  /** Sets the low-battery warn interval in seconds. */
  void setLowBatteryWarnInterval(unsigned sec);

  /** Returns @c true if the call alert-tone is continuous. */
  bool callAlertToneContinuous() const;
  /** Sets the call alert-tone continuous. */
  void enableCallAlertToneContinuous(bool enable);
  /** Returns the call alert-tone duration in seconds. */
  unsigned callAlertToneDuration() const;
  /** Sets the call alert-tone duration in seconds. */
  void setCallAlertToneDuration(unsigned sec);

  /** Returns the lone worker response time in minutes. */
  unsigned loneWorkerResponseTime() const;
  /** Sets the lone-worker response time in minutes. */
  void setLoneWorkerResponseTime(unsigned min);

  /** Returns the lone-worker reminder time in seconds. */
  unsigned loneWorkerReminderTime() const;
  /** Sets the lone-worker reminder timer in seconds. */
  void setLoneWorkerReminderTime(unsigned sec);

  /** Returns the hang time scanning for digital channels. */
  unsigned digitalScanHangTime() const;
  /** Sets the scan hang-time for digital channels. */
  void setDigitalScanHangTime(unsigned ms);

  /** Returns the hang time scanning for analog channels. */
  unsigned analogScanHangTime() const;
  /** Sets the scan hang-time for analog channels. */
  void setAnalogScanHangTime(unsigned ms);

  /** Returns @c true if the backlight is always on. */
  bool backlightAlwaysOn() const;
  /** Enables the backlight continuously. */
  void enableBacklightAlwaysOn(bool enable);
  /** Returns the backlight duration in seconds. */
  unsigned backlightDuration() const;
  /** Sets the backlight duration in seconds. */
  void setBacklightDuration(unsigned sec);

  /** Returns @c true if the keypad lock is manual. */
  bool keypadLockManual() const;
  /** Sets the keypad lock to manual. */
  void enableKeypadLockManual(bool enable);
  /** Retunrs the keypad lock time in seconds. */
  unsigned keypadLockTime() const;
  /** Sets the keypad lock time in seconds. */
  void setKeypadLockTime(unsigned sec);

  /** Returns @c true if power-on password is enabled. */
  bool powerOnPasswordEnabled() const;
  /** Enables the power on password. */
  void enablePowerOnPassword(bool enable);
  /** Returns the power-on password. */
  unsigned powerOnPassword() const;
  /** Sets the power-on password. */
  void setPowerOnPassword(unsigned passwd);

  /** Returns @c true if radio programming password is enabled. */
  bool radioProgPasswordEnabled() const;
  /** Enables the radio programming password. */
  void enableRadioProgPassword(bool enable);
  /** Returns the radio programming password. */
  unsigned radioProgPassword() const;
  /** Sets the radio programming password. */
  void setRadioProgPassword(unsigned passwd);

  /** Retunrs the PC programming password. */
  const QString &pcProgPassword() const;
  /** Sets PC programming password. */
  void setPCProgPassword(const QString &passwd);

public:
  ConfigItem *allocateChild(QMetaProperty &prop, const YAML::Node &node,
                            const Context &ctx, const ErrorStack &err=ErrorStack());
protected:
  /** Holds the monitor type. */
  MonitorType _monitorType;
  /** If @c true all LEDs are disabled. */
  bool _allLEDsDisabled;
  /** If @c true the talk-permit tone is enabled for digital channels. */
  bool _talkPermitToneDigital;
  /** If @c true the talk-permit tone is enabled for analog channels. */
  bool _talkPermitToneAnalog;
  /** If @c true the password and lock is enabled. */
  bool _passwdAndLock;
  /** If @c true, the channel free indication tone is enabled. */
  bool _channelFreeIndicationTone;
  /** If @c true, all tones are disabled. */
  bool _allTonesDisabled;
  /** If @c true, the save-mode RX is enabled. */
  bool _saveModeRX;
  /** If @c true the preamble is saved. */
  bool _savePreamble;
  /** If @c true the boot picture is enabled. */
  bool _bootPicture;
  /** Holds the TX preamble duration. */
  unsigned _txPreambleDuration;
  /** Holds the group-call hang time. */
  unsigned _groupCallHangTime;
  /** Holds the private-call hang time. */
  unsigned _privateCallHangTime;
  /** Holds the low-battery warn interval. */
  unsigned _lowBatteryWarnInterval;
  /** If @c true, the call alert-tone is continuous. */
  bool _callAlertToneContinuous;
  /** Holds the call alert-tone duration. */
  unsigned _callAlertToneDuration;
  /** Holds the lone-worker response time. */
  unsigned _loneWorkerResponseTime;
  /** Holds teh lone-worker reminder time. */
  unsigned _loneWorkerReminderTime;
  /** Holds the scan hang-time for digital channels. */
  unsigned _digitalScanHangTime;
  /** Holds the scan hang-time for analog channels. */
  unsigned _analogScanHangTime;
  /** If @c true, the backlight is always on. */
  bool _backlightAlwaysOn;
  /** Holds the backlight duration. */
  unsigned _backlightDuration;
  /** If @c true, the keypad lock is manual. */
  bool _keypadLockManual;
  /** Holds the keypad lock time. */
  unsigned _keypadLockTime;
  /** If @c true, the power-on password is enabled. */
  bool _powerOnPasswordEnabled;
  /** Holds the power-on password. */
  unsigned _powerOnPassword;
  /** If @c true, the radio programming password is enabled. */
  bool _radioProgPasswordEnabled;
  /** Holds the radio programming password. */
  unsigned _radioProgPassword;
  /** Holds the PC programming password. */
  QString  _pcProgPassword;
};


/** Groups several extension for TyT devices.
 * @ingroup tyt */
class TyTConfigExtension: public ConfigExtension
{
  Q_OBJECT

  /** The button settings for TyT devices. */
  Q_PROPERTY(TyTButtonSettings* buttonSettings READ buttonSettings)

public:
  /** Constructor. Also allocates all associates extensions. */
  Q_INVOKABLE explicit TyTConfigExtension(QObject *parent=nullptr);

  ConfigItem *clone() const;

  /** Returns the button settings extension for TyT devices. */
  TyTButtonSettings *buttonSettings() const;

public:
  ConfigItem *allocateChild(QMetaProperty &prop, const YAML::Node &node,
                            const Context &ctx, const ErrorStack &err=ErrorStack());

protected:
  /** Owns the button settings extension. */
  TyTButtonSettings *_buttonSettings;
};

#endif // TYTBUTTONSETTINGSEXTENSION_HH
