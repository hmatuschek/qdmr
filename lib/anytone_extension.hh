#ifndef ANYTONEEXTENSION_HH
#define ANYTONEEXTENSION_HH

#include "configobject.hh"


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
 * @ingroup anytone. */
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


/** Implements the device specific extension for the gerneral settings of AnyTone devices.
 *
 * As there are a huge amount of different settings, they are split into separate extensions.
 * One for each topic.
 *
 * @ingroup anytone */
class AnytoneSettingsExtension: public ConfigExtension
{
  Q_OBJECT

  /** The key tone setting. */
  Q_PROPERTY(bool keyTone READ keyToneEnabled WRITE enableKeyTone)
  /** The display frequency setting. */
  Q_PROPERTY(bool displayFrequency READ displayFrequencyEnabled WRITE enableDisplayFrequency)
  /** The auto key-lock property. */
  Q_PROPERTY(bool autoKeyLock READ autoKeyLockEnabled WRITE enableAutoKeyLock)
  /** The auto shut-down delay in minutes. */
  Q_PROPERTY(unsigned int autoShutDownDelay READ autoShutDownDelay WRITE setAutoShutDownDelay)
  /** The boot settings. */
  Q_PROPERTY(AnytoneBootSettingsExtension * bootSettings READ bootSettings)

public:
  /** Constructor. */
  Q_INVOKABLE explicit AnytoneSettingsExtension(QObject *parent=nullptr);

  ConfigItem *clone() const;

  /** A reference to the boot settings. */
  AnytoneBootSettingsExtension *bootSettings() const;

  /** Returns @c true if the key tone is enabled. */
  bool keyToneEnabled() const;
  /** Enables/disables the key tone. */
  void enableKeyTone(bool enable);

  /** Returns @c true, if the frequency is displayed instead of the channel name. */
  bool displayFrequencyEnabled() const;
  /** Enables/disables display of frequency. */
  void enableDisplayFrequency(bool enable);

  /** Retruns @c true, if the automatic key-lock feature is enabled. */
  bool autoKeyLockEnabled() const;
  /** Enables/disables auto key-lock. */
  void enableAutoKeyLock(bool enabled);

  /** Returns the auto shut-down delay in minutes. */
  unsigned int autoShutDownDelay() const;
  /** Sets the auto shut-down delay. */
  void setAutoShutDownDelay(unsigned int min);

protected:
  /** The boot settings. */
  AnytoneBootSettingsExtension *_bootSettings;

  bool _keyTone;                   ///< Key tone property.
  bool _displayFrequency;          ///< Display frequency property.
  bool _autoKeyLock;               ///< Auto key-lock property.
  bool _autoShutDownDelay;         ///< The auto shut-down delay in minutes.
};

#endif // ANYTONEEXTENSION_HH
