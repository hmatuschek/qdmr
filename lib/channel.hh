#ifndef CHANNEL_HH
#define CHANNEL_HH

#include <QObject>
#include <QAbstractTableModel>

#include "configobject.hh"
#include "configreference.hh"
#include "signaling.hh"

class Config;
class RXGroupList;
class DigitalContact;
class ScanList;
class APRSSystem;
class PositioningSystem;
class RoamingZone;
class RadioID;


/** The base class of all channels (analog and digital) of a codeplug configuration.
 *
 * This class holds the common configuration of @c AnalogChannel and @c DigitalChannel, that is
 * the name, RX and TX frequencies, output power, TOT and default scanlist properties.
 *
 * @ingroup conf */
class Channel: public ConfigObject
{
	Q_OBJECT

  /** The name of the channel. */
  Q_PROPERTY(QString name READ name WRITE setName)
  /** The receive frequency of the channel. */
  Q_PROPERTY(double rxFrequency READ rxFrequency WRITE setRXFrequency)
  /** The transmit frequency of the channel. */
  Q_PROPERTY(double txFrequency READ txFrequency WRITE setTXFrequency)
  // /** The transmit power. */
  //Q_PROPERTY(Power power READ power WRITE setPower)
  // /** The transmit timeout in seconds. */
  //Q_PROPERTY(unsigned timeout READ timeout WRITE setTimeout)
  /** If true, the channel is receive only. */
  Q_PROPERTY(bool rxOnly READ rxOnly WRITE setRXOnly)
  /** The scan list. */
  Q_PROPERTY(ScanListReference* scanList READ scanList)
  // /** The VOX setting. */
  //Q_PROPERTY(unsigned vox READ vox WRITE setVOX)

public:
  /** Possible power settings. */
  enum class Power {
    Max,  ///< Highest power setting (e.g. > 5W, if available).
    High, ///< High power setting (e.g, 5W).
    Mid,  ///< Medium power setting (e.g., 2W, if avaliable)
    Low,  ///< Low power setting (e.g., 1W).
    Min   ///< Lowest power setting (e.g., <1W, if available).
  };
  Q_ENUM(Power)

protected:
  /** Hidden constructor.
   * Constructs a new empty channel. */
  explicit Channel(QObject *parent=nullptr);
  /** Copy constructor. */
  Channel(const Channel &other, QObject *parent=nullptr);

public:
  /** Returns @c true if the channel is of type @c T. This can be used to text wheter this channel
   * is actuially an analog or digital channel: @c channel->is<AnalogChannel>(). */
  template<class T>
  bool is() const {
	  return 0 != dynamic_cast<const T *>(this);
  }

  /** Dynamic cast of channels. For example @c channel->as<AnalogChannel>(). */
  template<class T>
  T *as() {
	  return dynamic_cast<T *>(this);
  }

  /** Dynamic cast of channels. For example @c channel->as<AnalogChannel>(). */
  template<class T>
  const T *as() const{
	  return dynamic_cast<const T *>(this);
  }

  /** Returns the name of the channel. */
  const QString &name() const;
  /** (Re-)Sets the name of the channel. */
  bool setName(const QString &name);

  /** Returns the RX frequency of the channel in MHz. */
  double rxFrequency() const;
  /** (Re-)Sets the RX frequency of the channel in MHz. */
  bool setRXFrequency(double freq);
  /** Returns the TX frequency of the channel in MHz. */
  double txFrequency() const;
  /** (Re-)Sets the TX frequency of the channel in MHz. */
  bool setTXFrequency(double freq);

  /** Retunrs @c true if the channel uses the global default power setting. */
  bool defaultPower() const;
  /** Returns the power setting of the channel if the channel does not use the default power. */
  Power power() const;
  /** (Re-)Sets the power setting of the channel, overrides default power. */
  void setPower(Power power);
  /** Sets the channel to use the default power setting. */
  void setDefaultPower();

  /** Returns @c true if the transmit timeout is specified by the global default value. */
  bool defaultTimeout() const;
  /** Returns @c true if the transmit timeout is disabled. */
  bool timeoutDisabled() const;
  /** Returns the TX timeout (TOT) in seconds. */
  unsigned timeout() const;
  /** (Re-)Sets the TX timeout (TOT) in seconds. */
  bool setTimeout(unsigned dur);
  /** Disables the transmit timeout. */
  void disableTimeout();
  /** Sets the timeout to the global default timeout. */
  void setDefaultTimeout();

  /** Returns @c true, if the channel is RX only. */
  bool rxOnly() const;
  /** Set, whether the channel is RX only. */
  bool setRXOnly(bool enable);

  /** Returns @c true if the VOX is disabled. */
  bool voxDisabled() const;
  /** Retunrs @c true if the VOX is specified by the global default value. */
  bool defaultVOX() const;
  /** Returns the VOX level [0-10]. */
  unsigned vox() const;
  /** Sets the VOX level [0-10]. */
  void setVOX(unsigned level);
  /** Sets the VOX level to the default value. */
  void setVOXDefault();
  /** Disables the VOX. */
  void disableVOX();

  /** Returns the reference to the scan list. */
  const ScanListReference *scanList() const;
  /** Returns the reference to the scan list. */
  ScanListReference *scanList();

  /** Returns the default scan list for the channel. */
  ScanList *scanListObj() const;
  /** (Re-) Sets the default scan list for the channel. */
  bool setScanListObj(ScanList *list);

protected:
  bool populate(YAML::Node &node, const Context &context);

protected slots:
  /** Gets called whenever a referenced object is changed or deleted. */
  void onReferenceModified();

protected:
  /** The channel name. */
  QString _name;
  /** The RX frequency in MHz. */
  double _rxFreq;
  /** The TX frequency in MHz. */
  double _txFreq;
  /** If @c true, the channel uses the global power setting. */
  bool _defaultPower;
  /** The transmit power setting. */
  Power _power;
  /** Transmit timeout in seconds. */
  unsigned _txTimeOut;
  /** RX only flag. */
  bool _rxOnly;
  /** Holds the VOX level. */
  unsigned _vox;
  /** Default scan list of the channel. */
  ScanListReference _scanlist;
};


/** Extension to the @c Channel class to implement an analog channel.
 *
 * This class implements all the properties specific to an analog channel. That is, the admit
 * criterion, squelch, RX and TX tones and bandwidth settings.
 *
 * @ingroup conf */
class AnalogChannel: public Channel
{
  Q_OBJECT

  /** The admit criterion of the channel. */
  Q_PROPERTY(Admit admit READ admit WRITE setAdmit)
  // /** The squelch level of the channel [1-10]. */
  // Q_PROPERTY(unsigned squelch READ squelch WRITE setSquelch)
  /** The band width of the channel. */
  Q_PROPERTY(Bandwidth bandwidth READ bandwidth WRITE setBandwidth)
  /** The APRS system. */
  Q_PROPERTY(APRSSystemReference* aprs READ aprs)

public:
  /** Admit criteria of analog channel. */
  enum class Admit {
    Always,  ///< Allow always.
    Free,    ///< Allow when channel free.
    Tone     ///< Allow when admit tone is present.
  };
  Q_ENUM(Admit)

  /** Possible bandwidth of an analog channel. */
  enum class Bandwidth {
    Narrow,  ///< Narrow bandwidth (12.5kHz).
    Wide     ///< Wide bandwidth (25kHz).
  };
  Q_ENUM(Bandwidth)

public:
  /** Constructs a new empty analog channel. */
  explicit AnalogChannel(QObject *parent=nullptr);
  /** Copy constructor. */
  AnalogChannel(const AnalogChannel &other, QObject *parent=nullptr);

  YAML::Node serialize(const Context &context);

  /** Returns the admit criterion for the analog channel. */
	Admit admit() const;
  /** (Re-)Sets the admit criterion for the analog channel. */
	void setAdmit(Admit admit);

  /** Returns @c true if the global default squelch level is used. */
  bool defaultSquelch() const;
  /** Returns @c true if the squelch is disabled. */
  bool squelchDisabled() const;
  /** Returns the squelch level [0,10]. */
	unsigned squelch() const;
  /** (Re-)Sets the squelch level [0,10]. 0 Disables squelch (on some radios). */
	bool setSquelch(unsigned squelch);
  /** Disables the quelch. */
  void disableSquelch();
  /** Sets the squelch to the global default value. */
  void setSquelchDefault();

  /** Returns the CTCSS/DCS RX tone, @c SIGNALING_NONE means disabled. */
  Signaling::Code rxTone() const;
  /** (Re-)Sets the CTCSS/DCS RX tone, @c SIGNALING_NONE disables the RX tone. */
  bool setRXTone(Signaling::Code code);
  /** Returns the CTCSS/DCS TX tone, @c SIGNALING_NONE means disabled. */
  Signaling::Code txTone() const;
  /** (Re-)Sets the CTCSS/DCS TX tone, @c SIGNALING_NONE disables the TX tone. */
  bool setTXTone(Signaling::Code code);

  /** Returns the bandwidth of the analog channel. */
	Bandwidth bandwidth() const;
  /** (Re-)Sets the bandwidth of the analog channel. */
	bool setBandwidth(Bandwidth bw);

  /** Returns the reference to the APRS system. */
  const APRSSystemReference *aprs() const;
  /** Returns the reference to the APRS system. */
  APRSSystemReference *aprs();
  /** Returns the APRS system used for this channel or @c nullptr if disabled. */
  APRSSystem *aprsSystem() const;
  /** Sets the APRS system. */
  void setAPRSSystem(APRSSystem *sys);

protected:
  bool populate(YAML::Node &node, const Context &context);

protected:
  /** Holds the admit criterion. */
	Admit _admit;
  /** Holds the squelch level [0,10]. */
  unsigned _squelch;
  /** The RX CTCSS tone. */
  Signaling::Code _rxTone;
  /** The TX CTCSS tone. */
  Signaling::Code _txTone;
  /** The channel bandwidth. */
	Bandwidth _bw;
  /** A reference to the APRS system used on the channel or @c nullptr if disabled. */
  APRSSystemReference _aprsSystem;
};



/** Extension to the @c Channel class to implement an digital (DMR) channel.
 *
 * That is, the admit criterion, color code, time slot, RX group list and TX contact.
 *
 * @ingroup conf */
class DigitalChannel: public Channel
{
	Q_OBJECT

  /** The admit criterion of the channel. */
  Q_PROPERTY(Admit admit READ admit WRITE setAdmit)
  /** The color code of the channel. */
  Q_PROPERTY(unsigned colorCode READ colorCode WRITE setColorCode)
  /** The time slot of the channel. */
  Q_PROPERTY(TimeSlot timeSlot READ timeSlot WRITE setTimeSlot)
  /** The radio ID. */
  Q_PROPERTY(RadioIDReference* radioID READ radioID)
  /** The rx group list. */
  Q_PROPERTY(GroupListReference* groupList READ groupList)
  /** The tx contact. */
  Q_PROPERTY(DigitalContactReference* contact READ contact)
  /** The positioning system. */
  Q_PROPERTY(PositioningSystemReference* aprs READ aprs)
  /** The roaming zone. */
  Q_PROPERTY(RoamingZoneReference* roaming READ roaming)

public:
  /** Possible admit criteria of digital channels. */
  enum class Admit {
    Always,      ///< No admit criteria, allows to transmit any time.
    Free,        ///< Transmit only if channel is free.
    ColorCode    ///< Transmit only if channel is free and matches given color code.
  };
  Q_ENUM(Admit)

  /** Possible timeslots for digital channels. */
  enum class TimeSlot {
    TS1, ///< Time/repeater slot 1
    TS2  ///< Time/repeater slot 2
  };
  Q_ENUM(TimeSlot)

public:
  /** Constructs a new empty digital (DMR) channel. */
  DigitalChannel(QObject *parent=nullptr);
  /** Copy constructor. */
  DigitalChannel(const DigitalChannel &other, QObject *parent=nullptr);

  YAML::Node serialize(const Context &context);

  /** Returns the admit criterion for the channel. */
	Admit admit() const;
  /** (Re-)Sets the admit criterion for the channel. */
	void setAdmit(Admit admit);

  /** Returns the color code for the channel. */
	unsigned colorCode() const;
  /** (Re-)Sets the color code for the channel. */
	bool setColorCode(unsigned cc);

  /** Returns the time slot for the channel. */
  TimeSlot timeSlot() const;
  /** (Re-)Sets the time slot for the channel. */
	bool setTimeSlot(TimeSlot ts);

  /** Returns a reference to the group list. */
  const GroupListReference *groupList() const;
  /** Returns a reference to the group list. */
  GroupListReference *groupList();
  /** Retruns the RX group list for the channel. */
  RXGroupList *groupListObj() const;
  /** (Re-)Sets the RX group list for the channel. */
  bool setGroupListObj(RXGroupList *rxg);

  /** Returns a reference to the transmit contact. */
  const DigitalContactReference *contact() const;
  /** Returns a reference to the transmit contact. */
  DigitalContactReference *contact();
  /** Returns the default TX contact to call on this channel. */
  DigitalContact *txContactObj() const;
  /** (Re-) Sets the default TX contact for this channel. */
  bool setTXContactObj(DigitalContact *c);

  /** Returns a reference to the positioning system. */
  const PositioningSystemReference *aprs() const;
  /** Returns a reference to the positioning system. */
  PositioningSystemReference *aprs();
  /** Returns the GPS system associated with this channel or @c nullptr if not set. */
  PositioningSystem *aprsObj() const;
  /** Associates the GPS System with this channel. */
  bool setAPRSObj(PositioningSystem *sys);

  /** Returns a reference to the roaming zone. */
  const RoamingZoneReference *roaming() const;
  /** Returns a reference to the roaming zone. */
  RoamingZoneReference *roaming();
  /** Returns the roaming zone associated with this channel or @c nullptr if not set. */
  RoamingZone *roamingZone() const;
  /** Associates the given roaming zone with this channel. */
  bool setRoamingZone(RoamingZone *zone);

  /** Returns the reference to the radio ID. */
  const RadioIDReference *radioID() const;
  /** Returns the reference to the radio ID. */
  RadioIDReference *radioID();
  /** Returns the radio ID associated with this channel or @c nullptr if the default ID is used. */
  RadioID *radioIdObj() const;
  /** Associates the given radio ID with this channel. Pass nullptr to set to default ID. */
  bool setRadioIdObj(RadioID *id);

protected:
  /** The admit criterion. */
	Admit _admit;
  /** The channel color code. */
	unsigned _colorCode;
  /** The time slot for the channel. */
	TimeSlot _timeSlot;
  /** The RX group list for this channel. */
  GroupListReference _rxGroup;
  /** The default TX contact. */
  DigitalContactReference _txContact;
  /** The GPS system. */
  PositioningSystemReference _posSystem;
  /** Roaming zone for the channel. */
  RoamingZoneReference _roaming;
  /** Radio ID to use on this channel. @c nullptr if default ID is used. */
  RadioIDReference _radioId;
};


/** Internal singleton class representing the "currently selected" channel.
 * @ingroup conf */
class SelectedChannel: public Channel
{
  Q_OBJECT

protected:
  /** Constructs the "selected" channel.
   * @warning Do not use this class directly, call @c SelectedChannel::get() instead. */
  explicit SelectedChannel();

public:
  /** Destructor. */
  virtual ~SelectedChannel();

  /** Constructs/gets the singleton instance. */
  static SelectedChannel *get();

protected:
  /** Holds the channel singleton instance. */
  static SelectedChannel *_instance;
};


/** Container class holding all channels (analog and digital) for a specific configuration
 * (@c Config).
 *
 * This class also implements the QAbstractTableModel and can therefore be displayed using a
 * default QTableView instance.
 *
 * @ingroup conf */
class ChannelList: public ConfigObjectList
{
	Q_OBJECT

public:
  /** Constructs an empty channel list. */
	explicit ChannelList(QObject *parent=nullptr);

  int add(ConfigObject *obj, int row=-1);

  /** Gets the channel at the specified index. */
  Channel *channel(int idx) const;
  /** Finds a digial channel with the given frequencies, time slot and color code. */
  DigitalChannel *findDigitalChannel(double rx, double tx, DigitalChannel::TimeSlot ts, unsigned cc) const;
  /** Finds an analog channel with the given frequeny. */
  AnalogChannel *findAnalogChannelByTxFreq(double freq) const;
};


#endif // CHANNEL_HH
