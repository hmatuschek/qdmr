#ifndef GPSSYSTEM_H
#define GPSSYSTEM_H

#include "configreference.hh"
#include <QAbstractTableModel>
#include "anytone_extension.hh"
#include "opengd77_extension.hh"

class Config;
class DMRContact;
class DMRChannel;
class FMChannel;


/** Base class of the position reporting systems, that is APRS and DMR position reporting system.
 * @ingroup conf */
class PositionReportingSystem: public ConfigObject
{
  Q_OBJECT
  Q_CLASSINFO("IdPrefix", "aprs")

  /** The update period in seconds. */
  Q_PROPERTY(Interval period READ period WRITE setPeriod SCRIPTABLE false)

protected:
  /** Default constructor. */
  explicit PositionReportingSystem(QObject *parent=nullptr);
  /** Hidden constructor.
   * The PositionReportingSystem class is not instantiated directly, use either @c GPSSystem or
   * @c APRSSystem instead.
   * @param name Specifies the name of the system.
   * @param period Specifies the auto-update period in seconds.
   * @param parent Specified the QObject parent object. */
  PositionReportingSystem(const QString &name, const Interval &period=Interval::fromMinutes(5),
                          QObject *parent=nullptr);

public:
  /** Destructor. */
  virtual ~PositionReportingSystem();

  /** Returns @c true, if the period is disabled. */
  bool periodDisabled() const;
  /** Returns the update period in seconds. */
  Interval period() const;
  /** Sets the update period in seconds. */
  void setPeriod(const Interval &period);
  /** Disable update period. */
  void disablePeriod();

public:
  bool parse(const YAML::Node &node, Context &ctx, const ErrorStack &err=ErrorStack());
  bool link(const YAML::Node &node, const Context &ctx, const ErrorStack &err=ErrorStack());

protected:
  bool populate(YAML::Node &node, const ConfigItem::Context &context, const ErrorStack &err=ErrorStack());

protected slots:
  /** Gets called, whenever a reference is modified. */
  void onReferenceModified();

protected:
  /** Holds the update period in seconds. */
  Interval _period;
};


/** This class represents a DMR position reporting system within the codeplug.
 * @ingroup conf */
class DMRAPRSSystem : public PositionReportingSystem
{
  Q_OBJECT

  /** References the destination contact. */
  Q_PROPERTY(DMRContactReference* contact READ contactRef)
  /** References the revertRef channel. */
  Q_PROPERTY(DMRChannelReference* revert READ revertChannelRef)

public:
  /** Default constructor. */
  Q_INVOKABLE explicit DMRAPRSSystem(QObject *parent=nullptr);
  /** Constructor.
   *
   * Please note, that a contact needs to be set in order for the GPS system to work properly.
   *
   * @param name Specifies the name of the GPS system.
   * @param contact Specifies the contact, the GPS position is sent to.
   * @param revertChannel Specifies the channel on which the GPS is sent on. If @c nullptr, the GPS
   * data is sent on the current channel.
   * @param period Specifies the update period in seconds.
   * @param parent Specifies the QObject parent object. */
  DMRAPRSSystem(const QString &name, DMRContact *contact=nullptr,
                DMRChannel *revertChannel = nullptr, const Interval &period=Interval::fromMinutes(5),
                QObject *parent = nullptr);

  ConfigItem *clone() const;

  /** Returns @c true if a contact is set for the GPS system. */
  bool hasContact() const;
  /** Returns the destination contact for the GPS information or @c nullptr if not set. */
  DMRContact *contact() const;
  /** Sets the destination contact for the GPS information. */
  void setContact(DMRContact *contactObj);
  /** Returns the reference to the destination contactRef. */
  const DMRContactReference *contactRef() const;
  /** Returns the reference to the destination contactRef. */
  DMRContactReference *contactRef();

  /** Returns @c true if the GPS system has a revert channel set. If not, the GPS information will
   * be sent on the current channel. */
  bool hasRevertChannel() const;
  /** Returns the revert channel for the GPS information or @c nullptr if not set. */
  DMRChannel *revertChannel() const;
  /** Sets the revert channel for the GPS information to be sent on. */
  void setRevertChannel(DMRChannel *channel);
  /** Resets the revert channel to the current channel. */
  void resetRevertChannel();

  /** Returns a reference to the revertChannelRef channel. */
  const DMRChannelReference *revertChannelRef() const;
  /** Returns a reference to the revertChannelRef channel. */
  DMRChannelReference *revertChannelRef();

public:
  YAML::Node serialize(const Context &context, const ErrorStack &err=ErrorStack());

protected:
  /** Holds the destination contact for the GPS information. */
  DMRContactReference _contact;
  /** Holds the revert channel on which the GPS information is sent on. */
  DMRChannelReference _revertChannel;
};


/** Represents an APRS system within the generic config.
 * @ingroup conf */
class FMAPRSSystem: public PositionReportingSystem
{
  Q_OBJECT

  /** The transmit channel. */
  Q_PROPERTY(FMChannelReference* revert READ revertChannelRef)
  /** The destination call. */
  Q_PROPERTY(QString destination READ destination WRITE setDestination SCRIPTABLE false)
  /** The destination SSID. */
  Q_PROPERTY(unsigned int destSSID READ destSSID WRITE setDestSSID SCRIPTABLE false)
  /** The source call. */
  Q_PROPERTY(QString source READ source WRITE setSource SCRIPTABLE false)
  /** The source SSID. */
  Q_PROPERTY(unsigned int srcSSID READ srcSSID WRITE setSrcSSID SCRIPTABLE false)
  /** The APRS path as a string. */
  Q_PROPERTY(QString path READ path WRITE setPath SCRIPTABLE false)

  /** The APRS icon. */
  Q_PROPERTY(Icon icon READ icon WRITE setIcon)
  /** An optional text message. */
  Q_PROPERTY(QString message READ message WRITE setMessage)
  /** Anytone specific settings. */
  Q_PROPERTY(AnytoneFMAPRSSettingsExtension *anytone READ anytoneExtension WRITE setAnytoneExtension)
  /** OpenGD77 specific settings. */
  Q_PROPERTY(OpenGD77APRSSystemExtension *opengd77 READ openGD77Extension WRITE setOpenGD77Extension)

public:
  static const unsigned PRIMARY_TABLE   = (0<<8);   ///< Primary icon table flag.
  static const unsigned SECONDARY_TABLE = (1<<8);   ///< Secondary icon table flag.
  static const unsigned TABLE_MASK      = (3<<8);   ///< Bitmask for icon table flags.
  static const unsigned ICON_MASK       = 0x7f;     ///< Bitmask for the icon table entry.

  /** All implemented APRS icons. */
  enum class Icon {
    PoliceStation = (PRIMARY_TABLE | 0), None, Digipeater, Phone, DXCluster, HFGateway, SmallPlane,
    MobileSatelliteStation, WheelChair, Snowmobile, RedCross, BoyScout, Home, X, RedDot,
    Circle0, Circle1, Circle2, Circle3, Circle4, Circle5, Circle6, Circle7, Circle8, Circle9,
    Fire, Campground, Motorcycle, RailEngine, Car, FileServer, HCFuture, AidStation, BBS, Canoe,
    Eyeball = (PRIMARY_TABLE | 36), Tractor, GridSquare, Hotel, TCPIP, School = (PRIMARY_TABLE | 42),
    Logon, MacOS, NTSStation, Balloon, PoliceCar, TBD, RV, Shuttle, SSTV, Bus, ATV, WXService, Helo,
    Yacht, Windows, Jogger, Triangle, PBBS, LargePlane, WXStation, DishAntenna, Ambulance, Bike,
    ICP, FireStation, Horse, FireTruck, Glider, Hospital, IOTA, Jeep, SmallTruck, Laptop, MicE,
    Node, EOC, Rover, Grid, Antenna, PowerBoat, TruckStop, TruckLarge, Van, Water, XAPRS, Yagi,
    Shelter
  };
  Q_ENUM(Icon)

public:
  /** Default constructor. */
  Q_INVOKABLE explicit FMAPRSSystem(QObject *parent=nullptr);
  /** Constructor for a APRS system.
   * @param name Specifies the name of the APRS system. This property is just a name, it does not
   *        affect the radio configuration.
   * @param channel Specifies the transmit channel. This property is not optional. A transmit
   *        channel must be specified to obtain a working APRS system.
   * @param dest Specifies the destination call, APRS messages are sent to. Usually 'WIDE3' is a
   *        reasonable setting.
   * @param destSSID Specifies the destination SSID. Usually 3 is a reasonable choice.
   * @param src Specifies the source call, usually you call has to be entered here.
   * @param srcSSID The source SSID, usually 7 is a reasonable choice of handhelds.
   * @param path Specifies the APRS path string.
   * @param icon Specifies the map icon to send.
   * @param message An optional message to send.
   * @param period Specifies the auto-update period in seconds.
   * @param parent Specifies the QObject parent object. */
  FMAPRSSystem(const QString &name, FMChannel *channel, const QString &dest, unsigned destSSID,
               const QString &src, unsigned srcSSID, const QString &path="", Icon icon=Icon::Jogger,
               const QString &message="", const Interval &period=Interval::fromMinutes(5),
               QObject *parent=nullptr);

  bool copy(const ConfigItem &other);
  ConfigItem *clone() const;

  /** Returns @c true if a revert channel is set. If false, the APRS information is send on the
   * current channel. */
  bool hasRevertChannel() const;
  /** Returns the transmit channel of the APRS system. */
  FMChannel *revertChannel() const;
  /** Sets the transmit channel of the APRS system. */
  void setRevertChannel(FMChannel *revertChannel);
  /** Resets the revert channel to the current one */
  void resetRevertChannel();

  /** Returns a reference to the revertChannelRef channel. */
  const FMChannelReference *revertChannelRef() const;
  /** Returns a reference to the revertChannelRef channel. */
  FMChannelReference *revertChannelRef();

  /** Returns the destination call. */
  const QString &destination() const;
  /** Returns the destination SSID. */
  unsigned destSSID() const;
  /** Sets the destination call and SSID. */
  void setDestination(const QString &call, unsigned ssid);
  /** Sets the destination call. */
  void setDestination(const QString &call);
  /** Sets the destination SSID. */
  void setDestSSID(unsigned ssid);

  /** Returns the source call. */
  const QString &source() const;
  /** Returns the source SSID. */
  unsigned srcSSID() const;
  /** Sets the source call and SSID. */
  void setSource(const QString &call, unsigned ssid);
  /** Sets the source call. */
  void setSource(const QString &call);
  /** Sets the source SSID. */
  void setSrcSSID(unsigned ssid);

  /** Returns the APRS path. */
  const QString &path() const;
  /** Sets the APRS path. */
  void setPath(const QString &path);

  /** Returns the map icon. */
  Icon icon() const;
  /** Sets the map icon. */
  void setIcon(Icon icon);

  /** Returns the optional message. */
  const QString &message() const;
  /** Sets the optional APRS message text. */
  void setMessage(const QString &msg);

  /** Returns the Anytone settings extension, if set. */
  AnytoneFMAPRSSettingsExtension *anytoneExtension() const;
  /** Sets the Anytone settings extension. */
  void setAnytoneExtension(AnytoneFMAPRSSettingsExtension *ext);

  /** Returns the OpenGD77 settings extension, if set. */
  OpenGD77APRSSystemExtension *openGD77Extension() const;
  /** Sets the OpenGD77 settings extension. */
  void setOpenGD77Extension(OpenGD77APRSSystemExtension *ext);

public:
  YAML::Node serialize(const Context &context, const ErrorStack &err=ErrorStack());
  bool parse(const YAML::Node &node, Context &ctx, const ErrorStack &err=ErrorStack());

protected:
  bool populate(YAML::Node &node, const Context &context, const ErrorStack &err=ErrorStack());

protected:
  /** A weak reference to the transmit channel. */
  FMChannelReference _channel;
  /** Holds the destination call. */
  QString _destination;
  /** Holds the destination SSID. */
  unsigned _destSSID;
  /** Holds the source call. */
  QString _source;
  /** Holds the source SSID. */
  unsigned _srcSSID;
  /** Holds the APRS path string. */
  QString _path;
  /** Holds the map icon. */
  Icon _icon;
  /** Holds the optional message. */
  QString _message;
  /** Owns the Anytone settings extension. */
  AnytoneFMAPRSSettingsExtension *_anytone;
  /** Owns the OpenGD77 settings extension. */
  OpenGD77APRSSystemExtension *_openGD77;
};


/** The list of positioning systems.
 * @ingroup conf */
class PositionReportingSystems: public ConfigObjectList
{
Q_OBJECT

public:
  /** Constructs an empty list of GPS systems. */
  explicit PositionReportingSystems(QObject *parent=nullptr);

  /** Returns the positioning system at the specified index. */
  PositionReportingSystem *system(int idx) const;

  int add(ConfigObject *obj, int row=-1, bool unique=true);

public:
  ConfigItem *allocateChild(const YAML::Node &node, ConfigItem::Context &ctx, const ErrorStack &err=ErrorStack());
};


#endif // GPSSYSTEM_H
