#ifndef GPSSYSTEM_H
#define GPSSYSTEM_H

#include "configreference.hh"
#include <QAbstractTableModel>

class Config;
class DigitalContact;
class DigitalChannel;
class AnalogChannel;


/** Base class of the positioning systems, that is APRS and DMR position reporting system.
 * @ingroup conf */
class PositioningSystem: public ConfigObject
{
  Q_OBJECT

  /** The name of the system. */
  Q_PROPERTY(QString name READ name WRITE setName)
  /** The update period in seconds. */
  Q_PROPERTY(uint period READ period WRITE setPeriod)

protected:
  /** Hidden constructor.
   * The PositioningSystem class is not instantiated directly, use either @c GPSSystem or
   * @c APRSSystem instead.
   * @param name Specifies the name of the system.
   * @param period Specifies the auto-update period in seconds.
   * @param parent Specified the QObject parent object. */
  explicit PositioningSystem(const QString &name, uint period=300, QObject *parent=nullptr);

public:
  /** Destructor. */
  virtual ~PositioningSystem();

  using ConfigObject::serialize;

  /** Returns @c true if this positioning system is an instance of the specified system. */
  template <class System>
  bool is() const { return nullptr != dynamic_cast<const System *>(this); }

  /** Casts this positioning system to an instance of the specified system. */
  template <class System>
  System *as() { return dynamic_cast<System *>(this); }

  /** Casts this positioning system to an instance of the specified system. */
  template <class System>
  const System *as() const { return dynamic_cast<const System *>(this); }

  /** Returns the name of the GPS system. */
  const QString &name() const;
  /** Sets the name of the GPS system. */
  void setName(const QString &name);

  /** Returns the update period in seconds. */
  uint period() const;
  /** Sets the update period in seconds. */
  void setPeriod(uint period);

protected:
  bool populate(YAML::Node &node, const ConfigObject::Context &context);

protected slots:
  /** Gets called, whenever a reference is modified. */
  void onReferenceModified();

protected:
  /** Holds the name of the GPS system. */
  QString _name;
  /** Holds the update period in seconds. */
  uint _period;
};


/** This class represents a GPS signalling system within the codeplug.
 * @ingroup conf */
class GPSSystem : public PositioningSystem
{
  Q_OBJECT

  /** References the destination contact. */
  Q_PROPERTY(DigitalContactReference* contact READ contact)
  /** References the revert channel. */
  Q_PROPERTY(DigitalChannelReference* revert READ revert)

public:
  /** Constructor.
   *
   * Please note, that a contact needs to be set in order for the GPS system to work properly.
   *
   * @param name Specifies the name of the GPS system.
   * @param contact Specifies the contact, the GPS position is send to.
   * @param revertChannel Specifies the channel on which the GPS is send on. If @c nullptr, the GPS
   * data is send on the current channel.
   * @param period Specifies the update period in seconds.
   * @param parent Specifies the QObject parent object. */
  GPSSystem(const QString &name, DigitalContact *contact=nullptr,
            DigitalChannel *revertChannel = nullptr, uint period=300,
            QObject *parent = nullptr);

  YAML::Node serialize(const Context &context);

  /** Returns @c true if a contact is set for the GPS system. */
  bool hasContact() const;
  /** Returns the destination contact for the GPS information or @c nullptr if not set. */
  DigitalContact *contactObj() const;
  /** Sets the destination contact for the GPS information. */
  void setContact(DigitalContact *contactObj);
  /** Returns the reference to the destination contact. */
  const DigitalContactReference *contact() const;
  /** Returns the reference to the destination contact. */
  DigitalContactReference *contact();

  /** Returns @c true if the GPS system has a revert channel set. If not, the GPS information will
   * be send on the current channel. */
  bool hasRevertChannel() const;
  /** Returns the revert channel for the GPS information or @c nullptr if not set. */
  DigitalChannel *revertChannel() const;
  /** Sets the revert channel for the GPS information to be send on. */
  void setRevertChannel(DigitalChannel *channel);
  /** Returns a reference to the revert channel. */
  const DigitalChannelReference *revert() const;
  /** Returns a reference to the revert channel. */
  DigitalChannelReference *revert();

protected:
  /** Holds the destination contact for the GPS information. */
  DigitalContactReference _contact;
  /** Holds the revert channel on which the GPS information is send on. */
  DigitalChannelReference _revertChannel;
};


/** Represents an APRS system wihtin the generic config.
 * @ingroup conf */
class APRSSystem: public PositioningSystem
{
  Q_OBJECT

  /** The transmit channel. */
  Q_PROPERTY(AnalogChannelReference* revert READ revert)
  /** The APRS icon. */
  Q_PROPERTY(Icon icon READ icon WRITE setIcon)
  /** An optional text message. */
  Q_PROPERTY(QString message READ message WRITE setMessage)

public:
  static const uint PRIMARY_TABLE   = (0<<8);   ///< Primary icon table flag.
  static const uint SECONDARY_TABLE = (1<<8);   ///< Secondary icon table flag.
  static const uint TABLE_MASK      = (3<<8);   ///< Bitmask for icon table flags.
  static const uint ICON_MASK       = 0x7f;     ///< Bitmask for the icon table entry.

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
  /** Constructor for a APRS system.
   * @param name Specifies the name of the APRS system. This property is just a name, it does not
   *        affect the radio configuration.
   * @param channel Specifies the transmit channel. This property is not optional. A transmit
   *        channel must be specified to obtain a working APRS system.
   * @param dest Specifies the destination call, APRS messages are send to. Usually 'WIDE3' is a
   *        reasonable setting.
   * @param destSSID Specifies the destination SSID. Usually 3 is a reasonable choice.
   * @param src Specifies the source call, usually you call has to be entered here.
   * @param srcSSID The source SSID, usually 7 is a reasonable choice of handhelds.
   * @param path Specifies the APRS path string.
   * @param icon Specifies the map icon to send.
   * @param message An optional message to send.
   * @param period Specifies the auto-update period in seconds.
   * @param parent Specifies the QObject parent object. */
  explicit APRSSystem(const QString &name, AnalogChannel *channel, const QString &dest, uint destSSID,
                      const QString &src, uint srcSSID,
                      const QString &path="", Icon icon=Icon::Jogger,
                      const QString &message="", uint period=300, QObject *parent=nullptr);

  YAML::Node serialize(const Context &context);

  /** Returns the transmit channel of the APRS system. */
  AnalogChannel *revertChannel() const;
  /** Sets the transmit channel of the APRS system. */
  void setRevertChannel(AnalogChannel *revertChannel);
  /** Returns a reference to the revert channel. */
  const AnalogChannelReference *revert() const;
  /** Returns a reference to the revert channel. */
  AnalogChannelReference *revert();

  /** Retruns the destination call. */
  const QString &destination() const;
  /** Returns the destination SSID. */
  uint destSSID() const;
  /** Sets the destination call and SSID. */
  void setDestination(const QString &call, uint ssid);

  /** Returns the source call. */
  const QString &source() const;
  /** Returns the source SSID. */
  uint srcSSID() const;
  /** Sets the source call and SSID. */
  void setSource(const QString &call, uint ssid);

  /** Retruns the APRS path. */
  const QString &path() const;
  /** Sets the APRS path. */
  void setPath(const QString &path);

  /** Returns the map icon. */
  Icon icon() const;
  /** Sets the map icon. */
  void setIcon(Icon icon);

  /** Retunrs the optional message. */
  const QString &message() const;
  /** Sets the optional APRS message text. */
  void setMessage(const QString &msg);

protected:
  bool populate(YAML::Node &node, const Context &context);

protected:
  /** A weak reference to the transmit channel. */
  AnalogChannelReference _channel;
  /** Holds the destination call. */
  QString _destination;
  /** Holds the destination SSID. */
  uint _destSSID;
  /** Holds the source call. */
  QString _source;
  /** Holds the source SSID. */
  uint _srcSSID;
  /** Holds the APRS path string. */
  QString _path;
  /** Holds the map icon. */
  Icon _icon;
  /** Holds the optional message. */
  QString _message;
};


/** The list of positioning systems.
 * @ingroup conf */
class PositioningSystems: public ConfigObjectList
{
Q_OBJECT

public:
  /** Constructs an empty list of GPS systems. */
  explicit PositioningSystems(QObject *parent=nullptr);

  /** Returns the positioning system at the specified index. */
  PositioningSystem *system(int idx) const;

  int add(ConfigObject *obj, int row=-1);

  /** Returns the number of defined GPS systems. */
  int gpsCount() const;
  /** Returns the index of the GPS System.
   * This index in only within all defined GPS systems. */
  int indexOfGPSSys(const GPSSystem *gps) const;
  /** Gets the GPS system at the specified index.
   * This index is only within all defined GPS systems. */
  GPSSystem *gpsSystem(int idx) const;

  /** Returns the number of defined APRS system. */
  int aprsCount() const;
  /** Returns the index of the specified APRS system. That index is only within all
   * defined APRS systems. */
  int indexOfAPRSSys(APRSSystem *gps) const;
  /** Returns the APRS system at index @c idx.
   * That index is only within all defined APRS systems. */
  APRSSystem *aprsSystem(int idx) const;
};


#endif // GPSSYSTEM_H
