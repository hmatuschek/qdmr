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

  Q_CLASSINFO("description", "Settings for MD-390, RT8, MD-UV390, RT3S, MD-2017, RT82, DM-1701, RT84.")
  Q_CLASSINFO("longDescription", "Device specific channel settings for TyT and Retevis devices."
                                 "Including TyT MD-390, MD-UV390, MD-2017, Retevis RT8, RT3S and RT82"
                                 " as well as Baofeng DM-1701.")

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
  /*ConfigItem *allocateChild(QMetaProperty &prop, const YAML::Node &node,
                            const Context &ctx, const ErrorStack &err=ErrorStack());*/

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
  /*ConfigItem *allocateChild(QMetaProperty &prop, const YAML::Node &node,
                            const Context &ctx, const ErrorStack &err=ErrorStack());*/

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
  Q_PROPERTY(ButtonAction sideButton1Short READ sideButton1Short WRITE setSideButton1Short)
  /** The action to perform on a long press on side button 1. */
  Q_PROPERTY(ButtonAction sideButton1Long READ sideButton1Long WRITE setSideButton1Long)
  /** The action to perform on a short press on side button 2. */
  Q_PROPERTY(ButtonAction sideButton2Short READ sideButton2Short WRITE setSideButton2Short)
  /** The action to perform on a long press on side button 2. */
  Q_PROPERTY(ButtonAction sideButton2Long READ sideButton2Long WRITE setSideButton2Long)
  /** The action to perform on a short press on side button 3. */
  Q_PROPERTY(ButtonAction sideButton3Short READ sideButton3Short WRITE setSideButton3Short)
  /** The action to perform on a long press on side button 3. */
  Q_PROPERTY(ButtonAction sideButton3Long READ sideButton3Long WRITE setSideButton3Long)
  /** The action to perform on a short press on programmable button 1. */
  Q_PROPERTY(ButtonAction progButton1Short READ progButton1Short WRITE setProgButton1Short)
  /** The action to perform on a long press on programmable button 1. */
  Q_PROPERTY(ButtonAction progButton1Long READ progButton1Long WRITE setProgButton1Long)
  /** The action to perform on a short press on programmable button 2. */
  Q_PROPERTY(ButtonAction progButton2Short READ progButton2Short WRITE setProgButton2Short)
  /** The action to perform on a long press on programmable button 2. */
  Q_PROPERTY(ButtonAction progButton2Long READ progButton2Long WRITE setProgButton2Long)

  /** The duration of a long press in msec. */
  Q_PROPERTY(unsigned longPressDuration READ longPressDuration WRITE setLongPressDuration)

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

  /** Returns the action for the side button 3 short-press (Baofeng DM-1701). */
  ButtonAction sideButton3Short() const;
  /** Sets the action for the side button 3 short-press (Baofeng DM-1701). */
  void setSideButton3Short(ButtonAction action);
  /** Returns the action for the side button 3 long-press (Baofeng DM-1701). */
  ButtonAction sideButton3Long() const;
  /** Sets the action for the side button 3 long-press (Baofeng DM-1701). */
  void setSideButton3Long(ButtonAction action);

  /** Returns the action for the programmable button 1 short-press (Baofeng DM-1701). */
  ButtonAction progButton1Short() const;
  /** Sets the action for the programmable button 1 short-press (Baofeng DM-1701). */
  void setProgButton1Short(ButtonAction action);
  /** Returns the action for the programmable button 1 long-press (Baofeng DM-1701). */
  ButtonAction progButton1Long() const;
  /** Sets the action for the programmable button 1 long-press (Baofeng DM-1701). */
  void setProgButton1Long(ButtonAction action);

  /** Returns the action for the programmable button 2 short-press (Baofeng DM-1701). */
  ButtonAction progButton2Short() const;
  /** Sets the action for the programmable button 2 short-press (Baofeng DM-1701). */
  void setProgButton2Short(ButtonAction action);
  /** Returns the action for the programmable button 2 long-press (Baofeng DM-1701). */
  ButtonAction progButton2Long() const;
  /** Sets the action for the programmable button 2 long-press (Baofeng DM-1701). */
  void setProgButton2Long(ButtonAction action);

  /** Returns the long-press duration in msec. */
  unsigned longPressDuration() const;
  /** Sets the long-press duration in msec. */
  void setLongPressDuration(unsigned dur);

public:
  /*ConfigItem *allocateChild(QMetaProperty &prop, const YAML::Node &node,
                            const Context &ctx, const ErrorStack &err=ErrorStack());*/

protected:
  /** Holds the side button 1 short-press action. */
  ButtonAction _sideButton1Short;
  /** Holds the side button 1 long-press action. */
  ButtonAction _sideButton1Long;
  /** Holds the side button 2 short-press action. */
  ButtonAction _sideButton2Short;
  /** Holds the side button 2 long-press action. */
  ButtonAction _sideButton2Long;
  /** Holds the side button 3 short-press action. */
  ButtonAction _sideButton3Short;
  /** Holds the side button 3 long-press action. */
  ButtonAction _sideButton3Long;
  /** Holds the prog button 1 short-press action. */
  ButtonAction _progButton1Short;
  /** Holds the prog button 1 long-press action. */
  ButtonAction _progButton1Long;
  /** Holds the prog button 2 short-press action. */
  ButtonAction _progButton2Short;
  /** Holds the prog button 2 long-press action. */
  ButtonAction _progButton2Long;

  /** Holds the long-press duration in ms. */
  unsigned _longPressDuration;
};


/** Represents the TyT menu settings extension.
 * @ingroup tyt */
class TyTMenuSettings : public ConfigExtension
{
  Q_OBJECT

  /** If @c true, the menu hang time is infinite. */
  Q_PROPERTY(bool hangtimeIsInfinite READ hangtimeIsInfinite WRITE setHangtimeInfinite)
  /** The menu hang time in seconds. */
  Q_PROPERTY(unsigned hangTime READ hangTime WRITE setHangTime)
  /** If @c true, the text message menu is shown. */
  Q_PROPERTY(bool textMessage READ textMessage WRITE enableTextMessage)
  /** If @c true, the call-alert menu item is shown. */
  Q_PROPERTY(bool callAlert READ callAlert WRITE enableCallAlert)
  /** If @c true, the contact editing menu is shown. */
  Q_PROPERTY(bool contactEditing READ contactEditing WRITE enableContactEditing)
  /** If @c true, the manual dial menu item is shown. */
  Q_PROPERTY(bool manualDial READ manualDial WRITE enableManualDial)
  /** If @c true, the remote radio check menu item is shown. */
  Q_PROPERTY(bool remoteRadioCheck READ remoteRadioCheck WRITE enableRemoteRadioCheck)
  /** If @c true, the remote monitor menu item is shown. */
  Q_PROPERTY(bool remoteMonitor READ remoteMonitor WRITE enableRemoteMonitor)
  /** If @c true, the remote radio enable menu item is shown. */
  Q_PROPERTY(bool remoteRadioEnable READ remoteRadioEnable WRITE enableRemoteRadioEnable)
  /** If @c true, the remote radio disable menu item is shown. */
  Q_PROPERTY(bool remoteRadioDisable READ remoteRadioDisable WRITE enableRemoteRadioDisable)
  /** If @c true, the scan menu item is shown. */
  Q_PROPERTY(bool scan READ scan WRITE enableScan)
  /** If @c true, the scan list editing is enabled. */
  Q_PROPERTY(bool scanListEditing READ scanListEditing WRITE enableScanListEditing)
  /** If @c true, the list of missed calls is shown. */
  Q_PROPERTY(bool callLogMissed READ callLogMissed WRITE enableCallLogMissed)
  /** If @c true, the list of answered calls is shown. */
  Q_PROPERTY(bool callLogAnswered READ callLogAnswered WRITE enableCallLogAnswered)
  /** If @c true, the list of outgoing calls is shown. */
  Q_PROPERTY(bool callLogOutgoing READ callLogOutgoing WRITE enableCallLogOutgoing)
  /** If @c true, the talkaround menu item is shown. */
  Q_PROPERTY(bool talkaround READ talkaround WRITE enableTalkaround)
  /** If @c true, the alert-tone menu item is shown. */
  Q_PROPERTY(bool alertTone READ alertTone WRITE enableAlertTone)
  /** If @c true, the power settings menu item is shown. */
  Q_PROPERTY(bool power READ power WRITE enablePower)
  /** If @c true, the backlight menu item is shown. */
  Q_PROPERTY(bool backlight READ backlight WRITE enableBacklight)
  /** If @c true, the boot-screen settings menu item is shown. */
  Q_PROPERTY(bool bootScreen READ bootScreen WRITE enableBootScreen)
  /** If @c true, the keypad-lock settings menu item is shown. */
  Q_PROPERTY(bool keypadLock READ keypadLock WRITE enableKeypadLock)
  /** If @c true, the LED indicator settings menu item is shown. */
  Q_PROPERTY(bool ledIndicator READ ledIndicator WRITE enableLEDIndicator)
  /** If @c true, the squelch settings menu item is shown. */
  Q_PROPERTY(bool squelch READ squelch WRITE enableSquelch)
  /** If @c true, the VOX settings menu item is shown. */
  Q_PROPERTY(bool vox READ vox WRITE enableVOX)
  /** If @c true, the password menu item is shown. */
  Q_PROPERTY(bool password READ password WRITE enablePassword)
  /** If @c true, the display-mode settings menu item is shown. */
  Q_PROPERTY(bool displayMode READ displayMode WRITE enableDisplayMode)
  /** If @c true, radio programming on the radio is enabled. */
  Q_PROPERTY(bool radioProgramming READ radioProgramming WRITE enableRadioProgramming)
  /** If @c true, the positioning settings menu item is shown. */
  Q_PROPERTY(bool gpsInformation READ gpsInformation WRITE enableGPSInformation)

public:
  /** Constructor. */
  Q_INVOKABLE explicit TyTMenuSettings(QObject *parent=nullptr);

  ConfigItem *clone() const;

  /** Returns @c true if the hang time is infinite. */
  bool hangtimeIsInfinite() const;
  /** Enables/disables inifinite hang time. */
  void setHangtimeInfinite(bool infinite);

  /** Returns the menu hang time in seconds. */
  unsigned hangTime() const;
  /** Sets the menu hang time in seconds. */
  void setHangTime(unsigned sec);

  /** Returns @c true if the text message menu item is enabled. */
  bool textMessage() const;
  /** Enables/disables the text message menu item. */
  void enableTextMessage(bool enable);

  /** Returns @c true if the call alert menu item is enabled. */
  bool callAlert() const;
  /** Enables/disables the call alert menu item. */
  void enableCallAlert(bool enable);

  /** Returns @c true if contact editing is enabled. */
  bool contactEditing() const;
  /** Enables/disables contact editing. */
  void enableContactEditing(bool enable);

  /** Returns @c true if the manual dial menu item is enabled. */
  bool manualDial() const;
  /** Enables/disables the manual dial menu item. */
  void enableManualDial(bool enable);

  /** Returns @c true if the remote radio check menu item is enabled. */
  bool remoteRadioCheck() const;
  /** Enables/disables the remote radio check menu item. */
  void enableRemoteRadioCheck(bool enable);

  /** Returns @c true if the remote montior menu item is enabled. */
  bool remoteMonitor() const;
  /** Enables/disables the remote monitor menu item. */
  void enableRemoteMonitor(bool enable);

  /** Returns @c true if the remote radio enable menu item is enabled. */
  bool remoteRadioEnable() const;
  /** Enables/disables the remote radio enable menu item. */
  void enableRemoteRadioEnable(bool enable);

  /** Returns @c true if the remote radio disable menu item is enabled. */
  bool remoteRadioDisable() const;
  /** Enables/disables the remote radio disable menu item. */
  void enableRemoteRadioDisable(bool enable);

  /** Returns @c true if the scan menu item is enabled. */
  bool scan() const;
  /** Enables/disables the scan menu item. */
  void enableScan(bool enable);

  /** Returns @c true if the scan list editing menu item is enabled. */
  bool scanListEditing() const;
  /** Enables/disables the scan list editing menu item. */
  void enableScanListEditing(bool enable);

  /** Returns @c true if the list of missed calls menu item is enabled. */
  bool callLogMissed() const;
  /** Enables/disables the list of missed calls menu item. */
  void enableCallLogMissed(bool enable);

  /** Returns @c true if the list of answered calls menu item is enabled. */
  bool callLogAnswered() const;
  /** Enables/disables the list of answered calls menu item. */
  void enableCallLogAnswered(bool enable);

  /** Returns @c true if the list of outgoing calls menu item is enabled. */
  bool callLogOutgoing() const;
  /** Enables/disables the list of outgoing calls menu item. */
  void enableCallLogOutgoing(bool enable);

  /** Returns @c true if the talkaround menu item is enabled. */
  bool talkaround() const;
  /** Enables/disables the talkaround menu item. */
  void enableTalkaround(bool enable);

  /** Returns @c true if the alert tone menu item is enabled. */
  bool alertTone() const;
  /** Enables/disables the alert tone menu item. */
  void enableAlertTone(bool enable);

  /** Returns @c true if the power menu item is enabled. */
  bool power() const;
  /** Enables/disables the power menu item. */
  void enablePower(bool enable);

  /** Returns @c true if the backlight menu item is enabled. */
  bool backlight() const;
  /** Enables/disables the backlight menu item. */
  void enableBacklight(bool enable);

  /** Returns @c true if the boot screen menu item is enabled. */
  bool bootScreen() const;
  /** Enables/disables the boot screen menu item. */
  void enableBootScreen(bool enable);

  /** Returns @c true if the keypad lock menu item is enabled. */
  bool keypadLock() const;
  /** Enables/disables the keypad lock menu item. */
  void enableKeypadLock(bool enable);

  /** Returns @c true if the LED indicator menu item is enabled. */
  bool ledIndicator() const;
  /** Enables/disables the LED indicator menu item. */
  void enableLEDIndicator(bool enable);

  /** Returns @c true if the squelch menu item is enabled. */
  bool squelch() const;
  /** Enables/disables the squelch menu item. */
  void enableSquelch(bool enable);

  /** Returns @c true if the VOX menu item is enabled. */
  bool vox() const;
  /** Enables/disables the VOX menu item. */
  void enableVOX(bool enable);

  /** Returns @c true if the password menu item is enabled. */
  bool password() const;
  /** Enables/disables the password menu item. */
  void enablePassword(bool enable);

  /** Returns @c true if the display mode menu item is enabled. */
  bool displayMode() const;
  /** Enables/disables the display mode menu item. */
  void enableDisplayMode(bool enable);

  /** Returns @c true if the radio programming menu item is enabled. */
  bool radioProgramming() const;
  /** Enables/disables the radio programming menu item. */
  void enableRadioProgramming(bool enable);

  /** Returns @c true if the GPS information menu item is enabled. */
  bool gpsInformation() const;
  /** Enables/disables the GPS information menu item. */
  void enableGPSInformation(bool enable);

public:
  /*ConfigItem *allocateChild(QMetaProperty &prop, const YAML::Node &node,
                            const Context &ctx, const ErrorStack &err=ErrorStack());*/

protected:
  /** If @c true, the menu hang time is infinite. */
  bool _inifiniteHangTime;
  /** The menu hang time in seconds. */
  unsigned _hangTime;
  /** If @c true, the text message menu is shown. */
  bool _textMessage;
  /** If @c true, the call-alert menu item is shown. */
  bool _callAlert;
  /** If @c true, the contact editing menu is shown. */
  bool _contactEditing;
  /** If @c true, the manual dial menu item is shown. */
  bool _manualDial;
  /** If @c true, the remote radio check menu item is shown. */
  bool _remoteRadioCheck;
  /** If @c true, the remote monitor menu item is shown. */
  bool _remoteMonitor;
  /** If @c true, the remote radio enable menu item is shown. */
  bool _remoteRadioEnable;
  /** If @c true, the remote radio disable menu item is shown. */
  bool _remoteRadioDisable;
  /** If @c true, the scan menu item is shown. */
  bool _scan;
  /** If @c true, the scan list editing is enabled. */
  bool _scanListEditing;
  /** If @c true, the list of missed calls is shown. */
  bool _callLogMissed;
  /** If @c true, the list of answered calls is shown. */
  bool _callLogAnswered;
  /** If @c true, the list of outgoing calls is shown. */
  bool _callLogOutgoing;
  /** If @c true, the talkaround menu item is shown. */
  bool _talkaround;
  /** If @c true, the alert-tone menu item is shown. */
  bool _alertTone;
  /** If @c true, the power settings menu item is shown. */
  bool _power;
  /** If @c true, the backlight menu item is shown. */
  bool _backlight;
  /** If @c true, the boot-screen settings menu item is shown. */
  bool _bootScreen;
  /** If @c true, the keypad-lock settings menu item is shown. */
  bool _keypadLock;
  /** If @c true, the LED indicator settings menu item is shown. */
  bool _ledIndicator;
  /** If @c true, the squelch settings menu item is shown. */
  bool _squelch;
  /** If @c true, the VOX settings menu item is shown. */
  bool _vox;
  /** If @c true, the password menu item is shown. */
  bool _password;
  /** If @c true, the display-mode settings menu item is shown. */
  bool _displayMode;
  /** If @c true, radio programming on the radio is enabled. */
  bool _radioProgramming;
  /** If @c true, the positioning settings menu item is shown. */
  bool _gpsInformation;
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
  /** If @c true, a picture is shown during boot. */
  Q_PROPERTY(bool bootPicture READ bootPicture WRITE enableBootPicture)
  /** If @c true, the radio is in channel mode. */
  Q_PROPERTY(bool channelMode READ channelMode WRITE enableChannelMode)
  /** If @c true or channelMode is true, the VFO A is in channel mode. */
  Q_PROPERTY(bool channelModeA READ channelModeA WRITE enableChannelModeA)
  /** If @c true or channelMode is true, the VFO B is in channel mode. */
  Q_PROPERTY(bool channelModeB READ channelModeB WRITE enableChannelModeB)
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
  /** If @c true, the private call IDs must match. */
  Q_PROPERTY(bool privateCallMatch READ privateCallMatch WRITE enablePrivateCallMatch)
  /** If @c true, the group call IDs must match. */
  Q_PROPERTY(bool groupCallMatch READ groupCallMatch WRITE enableGroupCallMatch)
  /** Holds the channel hang time in ms. */
  Q_PROPERTY(unsigned channelHangTime READ channelHangTime WRITE setChannelHangTime)

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

  /** Returns @c true if the power save mode is enabled. */
  bool powerSaveMode() const;
  /** Enables the power save mode. */
  void enablePowerSaveMode(bool enable);

  /** Returns @c true if the wake-up preamble is send. */
  bool wakeupPreamble() const;
  /** Enables transmission of wakeup preamble. */
  void enableWakeupPreamble(bool enable);

  /** Returns @c true if a picture is shown during boot. */
  bool bootPicture() const;
  /** Enables the boot picture. */
  void enableBootPicture(bool enable);

  /** Retunrs @c true if the radio is in channel mode. Overrides @c channelModeA and @c channelModeB. */
  bool channelMode() const;
  /** Enables/disables channel mode for the radio. */
  void enableChannelMode(bool enable);
  /** Retunrs @c true if VFO A is in channel mode. Overridden by @c channelMode. */
  bool channelModeA() const;
  /** Enables/disables channel mode for the VFO A. */
  void enableChannelModeA(bool enable);
  /** Retunrs @c true if VFO B is in channel mode. Overridden by @c channelMode. */
  bool channelModeB() const;
  /** Enables/disables channel mode for the VFO B. */
  void enableChannelModeB(bool enable);

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

  /** Returns @c true if the private call IDs must match. */
  bool privateCallMatch() const;
  /** Enables/disables private call match. */
  void enablePrivateCallMatch(bool enable);
  /** Returns @c true if the group call IDs must match. */
  bool groupCallMatch() const;
  /** Enables/disables group call match. */
  void enableGroupCallMatch(bool enable);

  /** Returns the channel hang time in ms. */
  unsigned channelHangTime() const;
  /** Sets the channel hang time in ms. */
  void setChannelHangTime(unsigned ms);

public:
  /*ConfigItem *allocateChild(QMetaProperty &prop, const YAML::Node &node,
                            const Context &ctx, const ErrorStack &err=ErrorStack());*/
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
  /** If @c true, the power save mode is enabled. */
  bool _powerSaveMode;
  /** If @c true, the wake-up preamble is send. */
  bool _wakeupPreamble;
  /** If @c true the boot picture is enabled. */
  bool _bootPicture;
  /** If @c true or channelMode is true, the VFO A is in channel (memory) mode. */
  bool _channelModeA;
  /** If @c true or channelMode is true, the VFO B is in channel (memory) mode. */
  bool _channelModeB;
  /** If @c true, the radio is in channel (memory) mode. Overrides channelModeA and channelModeB. */
  bool _channelMode;
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
  /** If @c true, the private call IDs must match. */
  bool _privateCallMatch;
  /** If @c true, the group call IDs must match. */
  bool _groupCallMatch;
  /** Holds the channel hang time in ms. */
  unsigned _channelHangTime;
};


/** Groups several extension for TyT devices.
 * @ingroup tyt */
class TyTConfigExtension: public ConfigExtension
{
  Q_OBJECT

  /** The button settings for TyT devices. */
  Q_PROPERTY(TyTButtonSettings* buttonSettings READ buttonSettings)
  /** The menu settings for TyT devices. */
  Q_PROPERTY(TyTMenuSettings* menuSettings READ menuSettings)

public:
  /** Constructor. Also allocates all associates extensions. */
  Q_INVOKABLE explicit TyTConfigExtension(QObject *parent=nullptr);

  ConfigItem *clone() const;

  /** Returns the button settings extension for TyT devices. */
  TyTButtonSettings *buttonSettings() const;

  /** Returns the menu settings extension for TyT devices. */
  TyTMenuSettings *menuSettings() const;

public:
  /*ConfigItem *allocateChild(QMetaProperty &prop, const YAML::Node &node,
                            const Context &ctx, const ErrorStack &err=ErrorStack());*/

protected:
  /** Owns the button settings extension. */
  TyTButtonSettings *_buttonSettings;
  /** Owns the menu settings extension. */
  TyTMenuSettings *_menuSettings;
};

#endif // TYTBUTTONSETTINGSEXTENSION_HH
