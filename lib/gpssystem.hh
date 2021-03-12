#ifndef GPSSYSTEM_H
#define GPSSYSTEM_H

#include <QObject>
#include <QAbstractTableModel>


class Config;
class DigitalContact;
class DigitalChannel;
class AnalogChannel;

/** Base class of the positioning systems, that is APRS and DMR position reporting system.
 * @ingroup conf */
class PositioningSystem: public QObject
{
  Q_OBJECT

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

signals:
  /** Gets emitted if the GPS system is modified. */
  void modified();

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

  /** Returns @c true if a contact is set for the GPS system. */
  bool hasContact() const;
  /** Returns the destination contact for the GPS information or @c nullptr if not set. */
  DigitalContact *contact() const;
  /** Sets the destination contact for the GPS information. */
  void setContact(DigitalContact *contact);

  /** Returns @c true if the GPS system has a revert channel set. If not, the GPS information will
   * be send on the current channel. */
  bool hasRevertChannel() const;
  /** Returns the revert channel for the GPS information or @c nullptr if not set. */
  DigitalChannel *revertChannel() const;
  /** Sets the revert channel for the GPS information to be send on. */
  void setRevertChannel(DigitalChannel *channel);

protected slots:
  /** Internal used callback to get notified if the destination contact is deleted. */
  void onContactDeleted();
  /** Internal used callback to get notified if the revert channel is deleted. */
  void onRevertChannelDeleted();

protected:
  /** Holds the destination contact for the GPS information. */
  DigitalContact *_contact;
  /** Holds the revert channel on which the GPS information is send on. */
  DigitalChannel *_revertChannel;
};


/** Represents an APRS system wihtin the generic config.
 * @ingroup conf */
class APRSSystem: public PositioningSystem
{
  Q_OBJECT

public:
  static const uint PRIMARY_TABLE   = (0<<8);   ///< Primary icon table flag.
  static const uint SECONDARY_TABLE = (1<<8);   ///< Secondary icon table flag.
  static const uint TABLE_MASK      = (3<<8);   ///< Bitmask for icon table flags.
  static const uint ICON_MASK       = 0x7f;     ///< Bitmask for the icon table entry.

  /** All implemented APRS icons. */
  typedef enum {
    APRS_ICON_POLICE_STN = (PRIMARY_TABLE | 0), APRS_ICON_NO_SYMBOL, APRS_ICON_DIGI, APRS_ICON_PHONE,
    APRS_ICON_DX_CLUSTER, APRS_ICON_HF_GATEWAY, APRS_ICON_PLANE_SMALL, APRS_ICON_MOB_SAT_STN,
    APRS_ICON_WHEEL_CHAIR, APRS_ICON_SNOWMOBILE, APRS_ICON_RED_CROSS, APRS_ICON_BOY_SCOUT,
    APRS_ICON_HOME, APRS_ICON_X, APRS_ICON_RED_DOT, APRS_ICON_CIRCLE_0, APRS_ICON_CIRCLE_1,
    APRS_ICON_CIRCLE_2, APRS_ICON_CIRCLE_3, APRS_ICON_CIRCLE_4, APRS_ICON_CIRCLE_5,
    APRS_ICON_CIRCLE_6, APRS_ICON_CIRCLE_7, APRS_ICON_CIRCLE_8, APRS_ICON_CIRCLE_9,
    APRS_ICON_FIRE, APRS_ICON_CAMPGROUND, APRS_ICON_MOTORCYCLE, APRS_ICON_RAIL_ENGINE,
    APRS_ICON_CAR, APRS_ICON_FILE_SERVER, APRS_ICON_HC_FUTURE, APRS_ICON_AID_STN, APRS_ICON_BBS,
    APRS_ICON_CANOE, APRS_ICON_EYEBALL = (PRIMARY_TABLE | 36), APRS_ICON_TRACTOR, APRS_ICON_GRID_SQ,
    APRS_ICON_HOTEL, APRS_ICON_TCP_IP, APRS_ICON_SCHOOL = (PRIMARY_TABLE | 42),
    APRS_ICON_USER_LOGON, APRS_ICON_MAC, APRS_ICON_NTS_STN, APRS_ICON_BALLOON, APRS_ICON_POLICE,
    APRS_ICON_TBD, APRX_ICON_RV, APRS_ICON_SHUTTLE, APRS_ICON_SSTV, APRS_ICON_BUS, APRS_ICON_ATV,
    APRS_ICON_WX_SERVICE, APRS_ICON_HELO, APRS_ICON_YACHT, APRS_ICON_WIN, APRS_ICON_JOGGER,
    APRS_ICON_TRIANGLE, APRS_ICON_PBBS, APRS_ICON_PLANE_LARGE, APRS_ICON_WX_STN, APRS_ICON_DISH_ANT,
    APRS_ICON_AMBULANCE, APRS_ICON_BIKE, APRS_ICON_ICP, APRS_ICON_FIRE_STATION, APRS_ICON_HORSE,
    APRS_ICON_FIRE_TRUCK, APRS_ICON_GLIDER, APRS_ICON_HOSPITAL, APRS_ICON_IOTA, APRS_ICON_JEEP,
    APRS_ICON_TRUCK_SMALL, APRS_ICON_LAPTOP, APRS_ICON_MIC_E, APRS_ICON_NODE, APRS_ICON_EOC,
    APRS_ICON_ROVER, APRS_ICON_GRID, APRS_ICON_ANTENNA, APRS_ICON_POWER_BOAT, APRS_ICON_TRUCK_STOP,
    APRS_ICON_TUCK_LARGE, APRS_ICON_VAN, APRS_ICON_WATER, APRS_ICON_XAPRS, APRS_ICON_YAGI,
    APRS_ICON_SHELTER
  } Icon;

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
   * @param icon Specifies the map icon to send.
   * @param message An optional message to send.
   * @param period Specifies the auto-update period in seconds.
   * @param parent Specifies the QObject parent object. */
  explicit APRSSystem(const QString &name, AnalogChannel *channel, const QString &dest, uint destSSID,
                      const QString &src, uint srcSSID,
                      const QString &path="", Icon icon=APRS_ICON_JOGGER,
                      const QString &message="", uint period=300, QObject *parent=nullptr);

  /** Returns the transmit channel of the APRS system. */
  AnalogChannel *channel() const;
  /** Sets the transmit channel of the APRS system. */
  void setChannel(AnalogChannel *channel);

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

  const QString &path() const;
  void setPath(const QString &path);

  /** Returns the map icon. */
  Icon icon() const;
  /** Sets the map icon. */
  void setIcon(Icon icon);

  /** Retunrs the optional message. */
  const QString &message() const;
  /** Sets the optional APRS message text. */
  void setMessage(const QString &msg);

protected slots:
  /** Internal call-back if the transmit channel gets deleted. */
  void onChannelDeleted(QObject *obj);

protected:
  /** A weak reference to the transmit channel. */
  AnalogChannel *_channel;
  /** Holds the destination call. */
  QString _destination;
  /** Holds the destination SSID. */
  uint _destSSID;
  /** Holds the source call. */
  QString _source;
  /** Holds the source SSID. */
  uint _srcSSID;
  QString _path;
  /** Holds the map icon. */
  Icon _icon;
  /** Holds the optional message. */
  QString _message;
};


/** The list of positioning systems.
 * @ingroup conf */
class PositioningSystems: public QAbstractTableModel
{
Q_OBJECT

public:
  /** Constructs an empty list of GPS systems. */
  explicit PositioningSystems(QObject *parent=nullptr);

  /** Clears the list. */
  void clear();

  /** Returns the number of Positioning systems in this list. */
  int count() const;
  /** Returns the positioning system at the specified index. */
  PositioningSystem *system(int idx) const;
  /** Returns the index of given positioning system. */
  int indexOf(PositioningSystem *sys) const;
  /** Adds a positioning system to the list at the specified row.
   * If row<0 the system gets appendet to the list.*/
  int addSystem(PositioningSystem *sys, int row=-1);
  /** Removes the given positioning system from the list. */
  bool remSystem(PositioningSystem *gps);
  /** Removes the positioning system at the given index from the list. */
  bool remSystem(int idx);

  /** Returns the number of defined GPS systems. */
  int gpsCount() const;
  /** Returns the index of the GPS System.
   * This index in only within all defined GPS systems. */
  int indexOfGPSSys(GPSSystem *gps) const;
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

  /** Moves the GPS system at index @c idx one step up. */
  bool moveUp(int idx);
  /** Moves the GPS system at index @c idx one step up. */
  bool moveDown(int idx);

  // QAbstractTableModel interface
  /** Implements QAbstractTableModel, returns number of rows. */
  int rowCount(const QModelIndex &index) const;
  /** Implements QAbstractTableModel, returns number of colums. */
  int columnCount(const QModelIndex &index) const;
  /** Implements QAbstractTableModel, returns data at cell. */
  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
  /** Implements QAbstractTableModel, returns header at section. */
  QVariant headerData(int section, Qt::Orientation orientation, int role=Qt::DisplayRole) const;

signals:
  /** Gets emitted once the table has been changed. */
  void modified();

protected slots:
  /** Internal callback on deleted positioning systems. */
  void onSystemDeleted(QObject *obj);
  /** Internal callback on modified positioning systems. */
  void onSystemEdited();

protected:
  /** Just the vector of positioning systems. */
  QVector<PositioningSystem *> _posSystems;
};


#endif // GPSSYSTEM_H
