#ifndef ANYTONEEXTENSION_HH
#define ANYTONEEXTENSION_HH

#include "configobject.hh"
#include "configreference.hh"
#include "frequency.hh"
#include "interval.hh"

#include <QTimeZone>



/** Implements the config representation of an FM APRS frequency.
 * @ingroup anytone */
class AnytoneAPRSFrequency: public ConfigObject
{
  Q_OBJECT

  Q_CLASSINFO("IdPrefix", "af")

  Q_CLASSINFO("frequencyDecription",
              "Transmit-frequency.")

  /** The frequency. */
  Q_PROPERTY(Frequency frequency READ frequency WRITE setFrequency)

public:
  /** Default constructor. */
  Q_INVOKABLE explicit AnytoneAPRSFrequency(QObject *parent=nullptr);

  ConfigItem *clone() const;

  /** Returns the transmit frequency. */
  Frequency frequency() const;
  /** Sets the transmit frequency. */
  void setFrequency(Frequency freq);

protected:
  /** The transmit frequency. */
  Frequency _frequency;
};


/** Represents a reference to an APRS frequency.
 * @ingroup anytone */
class AnytoneAPRSFrequencyRef: public ConfigObjectReference
{
  Q_OBJECT

public:
  /** Default constructor. */
  explicit AnytoneAPRSFrequencyRef(QObject *parent=nullptr);
};


/** Represents a list of APRS transmit frequencies.
 * @ingroup anytone */
class AnytoneAPRSFrequencyList: public ConfigObjectList
{
  Q_OBJECT

public:
  /** Empty constructor. */
  explicit AnytoneAPRSFrequencyList(QObject *parent=nullptr);

  ConfigItem *allocateChild(const YAML::Node &node, ConfigItem::Context &ctx, const ErrorStack &err);
};


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
  /** A reference to the FM APRS frequency. If not set, the default will be used. */
  Q_PROPERTY(AnytoneAPRSFrequencyRef *fmAPRSFrequency READ fmAPRSFrequency())
  /** Specifies if and when the position is send via the associated APRS system, once the PTT is
   * pressed. */
  Q_PROPERTY(APRSPTT aprsPTT READ aprsPTT WRITE setAPRSPTT)

public:
  /** Possible APRS PTT modes. */
  enum class APRSPTT{
    Off, Start, End
  };
  Q_ENUM(APRSPTT)

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

  /** Holds a reference to the FM APRS frequency to be used if FM APRS is enabled on the channel. */
  AnytoneAPRSFrequencyRef *fmAPRSFrequency() const;

  /** Holds the APRS PTT mode. That his, if and when the APRS information is send via the
   * associated APRS system. */
  APRSPTT aprsPTT() const;
  /** Sets the APRS PTT mode. */
  void setAPRSPTT(APRSPTT mode);

protected:
  /** If @c true, talkaround is enabled. */
  bool _talkaround;
  /** The frequency correction. */
  int _frequencyCorrection;
  /** If @c true, the hands-free featrue is enabled for this channel. */
  bool _handsFree;
  /** A reference to the FM APRS frequency. */
  AnytoneAPRSFrequencyRef *_fmAPRSFrequency;
  /** Holds the APRS PTT mode. */
  APRSPTT _aprsPTT;
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
  Q_PROPERTY(Frequency scramblerFrequency READ scramblerFrequency WRITE setScramblerFrequency)

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

  /** Reuturns the FM scrabler carrier frequency. */
  Frequency scramblerFrequency() const;
  /** Sets the FM scrambler carrier frequency. */
  void setScramblerFrequency(const Frequency &freq);

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
  /** Sets the FM scrambler frequency. */
  Frequency _scramblerFrequency;
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


/** Implements some additional settings for the FM APRS system.
 * This extension gets attached to a @c APRSSystem instance.
 */
class AnytoneFMAPRSSettingsExtension: public ConfigExtension
{
  Q_OBJECT

  /** The transmit delay in milliseconds. */
  Q_PROPERTY(Interval txDelay READ txDelay WRITE setTXDelay)
  /** The transmit pre-wave delay in milliseconds. */
  Q_PROPERTY(Interval preWaveDelay READ preWaveDelay WRITE setPreWaveDelay)
  /** If @c true, all APRS messages are processed, including those with invalid CRC. */
  Q_PROPERTY(bool passAll READ passAll WRITE enablePassAll)
  /** If @c true, the report position flag is set. */
  Q_PROPERTY(bool reportPosition READ reportPosition WRITE enableReportPosition)
  /** If @c true, the report Mic-E flag is set. */
  Q_PROPERTY(bool reportMicE READ reportMicE WRITE enableReportMicE)
  /** If @c true, the report object flag is set. */
  Q_PROPERTY(bool reportObject READ reportObject WRITE enableReportObject)
  /** If @c true, the report item flag is set. */
  Q_PROPERTY(bool reportItem READ reportItem WRITE enableReportItem)
  /** If @c true, the report message flag is set. */
  Q_PROPERTY(bool reportMessage READ reportMessage WRITE enableReportMessage)
  /** If @c true, the report weather flag is set. */
  Q_PROPERTY(bool reportWeather READ reportWeather WRITE enableReportWeather)
  /** If @c true, the report NMEA flag is set. */
  Q_PROPERTY(bool reportNMEA READ reportNMEA WRITE enableReportNMEA)
  /** If @c true, the report status flag is set. */
  Q_PROPERTY(bool reportStatus READ reportStatus WRITE enableReportStatus)
  /** If @c true, the report other flag is set. */
  Q_PROPERTY(bool reportOther READ reportOther WRITE enableReportOther)

  /** The list of additional APRS frequencies. */
  Q_PROPERTY(AnytoneAPRSFrequencyList *frequencies READ frequencies)

public:
  /** Possible bandwidth settings. */
  enum class Bandwidth {
    Narrow = 0, Wide = 1
  };
  Q_ENUM(Bandwidth)

public:
  /** Default constructor. */
  explicit Q_INVOKABLE AnytoneFMAPRSSettingsExtension(QObject *parent=nullptr);

  ConfigItem *clone() const;

  /** Returns the transmit delay. */
  Interval txDelay() const;
  /** Sets the transmit delay. */
  void setTXDelay(Interval intv);

  /** Returns the pre-wave delay in ms. */
  Interval preWaveDelay() const;
  /** Sets the pre-wave delay in ms. */
  void setPreWaveDelay(Interval ms);

  /** Returns @c true if all received APRS messages are processed, even those with invalid CRC. */
  bool passAll() const;
  /** Enables processing of all received APRS messages, including those with invalid CRC. */
  void enablePassAll(bool enable);

  /** Returns @c true if the report position flag is set. */
  bool reportPosition() const;
  /** Enables/disables report position flag. */
  void enableReportPosition(bool enable);
  /** Returns @c true if the report Mic-E flag is set. */
  bool reportMicE() const;
  /** Enables/disables report Mic-E flag. */
  void enableReportMicE(bool enable);
  /** Returns @c true if the report object flag is set. */
  bool reportObject() const;
  /** Enables/disables report object flag. */
  void enableReportObject(bool enable);
  /** Returns @c true if the report item flag is set. */
  bool reportItem() const;
  /** Enables/disables report item flag. */
  void enableReportItem(bool enable);
  /** Returns @c true if the report message flag is set. */
  bool reportMessage() const;
  /** Enables/disables report message flag. */
  void enableReportMessage(bool enable);
  /** Returns @c true if the report weather flag is set. */
  bool reportWeather() const;
  /** Enables/disables report weather flag. */
  void enableReportWeather(bool enable);
  /** Returns @c true if the report NMEA flag is set. */
  bool reportNMEA() const;
  /** Enables/disables report NMEA flag. */
  void enableReportNMEA(bool enable);
  /** Returns @c true if the report status flag is set. */
  bool reportStatus() const;
  /** Enables/disables report status flag. */
  void enableReportStatus(bool enable);
  /** Returns @c true if the report other flag is set. */
  bool reportOther() const;
  /** Enables/disables report other flag. */
  void enableReportOther(bool enable);

  /** Returns the list of additional FM APRS frequencies. */
  AnytoneAPRSFrequencyList *frequencies() const;

protected:
  /** The transmit delay. */
  Interval _txDelay;
  /** The pre-wave delay. */
  Interval _preWaveDelay;
  /** If @c true, all APRS messages are processed. */
  bool _passAll;
  /** If @c true the report position flag is set. */
  bool _reportPosition;
  /** The report Mic-E flag. */
  bool _reportMicE;
  /** The report object flag. */
  bool _reportObject;
  /** The report item flag. */
  bool _reportItem;
  /** The report message flag. */
  bool _reportMessage;
  /** The report weather flag. */
  bool _reportWeather;
  /** The report NMEA flag. */
  bool _reportNMEA;
  /** The report status flag. */
  bool _reportStatus;
  /** The report other flag. */
  bool _reportOther;
  /** The list of additional FM APRS frequencies. */
  AnytoneAPRSFrequencyList *_frequencies;
};

#endif // ANYTONEEXTENSION_HH
