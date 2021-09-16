#ifndef TYTBUTTONSETTINGSEXTENSION_HH
#define TYTBUTTONSETTINGSEXTENSION_HH

#include "configobject.hh"
#include "configreader.hh"

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
  Q_PROPERTY(uint longPressDuration MEMBER _longPressDuration READ longPressDuration WRITE setLongPressDuration)

public:
  /** Possible actions for the side-buttons. */
  enum ButtonAction {
    Disabled = 0,                       ///< Disabled side-button action.
    ToggleAllAlertTones = 1,            ///< Toggle all alert tones.
    EmergencyOn = 2,                    ///< Enable emergency.
    EmergencyOff = 3,                   ///< Disable emergency.
    PowerSelect = 4,                    ///< Select TX power.
    MonitorToggle = 5,                  ///< Toggle monitor (promiscuous mode on digital channel, open squelch on analog channel).
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
  explicit TyTButtonSettings(QObject *parent=nullptr);

  bool label(Context &context);

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
  uint longPressDuration() const;
  /** Sets the long-press duration in msec. */
  void setLongPressDuration(uint dur);

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
  uint _longPressDuration;
};


/** Parses the TyT config extensions.
 * @ingroup tyt */
class TyTButtonSettingsReader: public ExtensionReader
{
  Q_OBJECT
  Q_CLASSINFO("name", "tytButtonSettings")

public:
  /** Constructor. */
  explicit TyTButtonSettingsReader(QObject *parent=nullptr);

  ConfigObject *allocate(const YAML::Node &node, const ConfigObject::Context &ctx);

private:
  /** Holds the instance of the config reader. */
  static AbstractConfigReader *instance;
};


#endif // TYTBUTTONSETTINGSEXTENSION_HH
