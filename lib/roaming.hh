#ifndef ROAMINGZONE_HH
#define ROAMINGZONE_HH

#include <QAbstractListModel>
#include <channel.hh>

/** Represents a RoamingZone within the abstract device configuration.
 *
 * A roaming zone collects a set of repeaters that act as alternatives to eachother. When a selected
 * repeater gets out of range, another one might be found automaticall from within the roaming zone.
 *
 * @ingroup config */
class RoamingZone : public ConfigObject
{
  Q_OBJECT

  /** The name of the roaming zone. */
  Q_PROPERTY(QString name READ name WRITE setName)

public:
  /** Constructor.
   * @param name Specifies the name of the roaming zone.
   * @param parent Specifies the QObject parent of this zone. */
  explicit RoamingZone(const QString &name, QObject *parent = nullptr);

  /** Returns the number of zones. */
  int count() const;

  /** Clears the zone list. */
  void clear();

  /** Returns the name of the roaming zone. */
  const QString &name() const;
  /** Sets the name of the roaming zone. */
  void setName(const QString &name);

  /** Retunrs the digital channel, which is the member at index @c idx (0-based).
   * @param idx Specifies the index of the member channel. */
  DigitalChannel *channel(int idx) const;
  /** Adds a channel to the roaming zone.
   * @param ch Specifies the channel to add.
   * @param row Speicifies the index where to insert the channel
   *        (optional, default insert at end). */
  bool addChannel(DigitalChannel *ch, int row=-1);
  /** Removes the channel from the roaming zone at index @c row. */
  bool remChannel(int row);
  /** Removes the given channel from the roaming zone. */
  bool remChannel(DigitalChannel *ch);

  /** Moves the channel at the given row one up. */
  bool moveUp(int row);
  /** Moves the channel at the given row one down. */
  bool moveDown(int row);

protected slots:
  /** Internal used callback to handle deleted channels. */
  void onChannelDeleted(QObject *obj);

protected:
  bool serialize(YAML::Node &node, const Context &context);

protected:
  /** Holds the name of the roaming zone. */
  QString _name;
  /** Holds the actual channels of the roaming zone. */
  QVector<DigitalChannel *> _channel;
};


/** Dummy roaming zone class that represents the default roaming zone.
 *
 * This is a singleton class. That is, there can only be one instance of this class.
 * @ingroup config */
class DefaultRoamingZone: public RoamingZone
{
  Q_OBJECT

protected:
  /** Hidden consturctor.
   * Use @c DefaultRoamingZone::get() to obtain an instance. */
  explicit DefaultRoamingZone(QObject *parent=nullptr);

public:
  /** Returns the singleton instance of this class. */
  static DefaultRoamingZone *get();

protected:
  /** Holds a reference to the singleton instance of this class. */
  static DefaultRoamingZone *_instance;
};


/** Represents the list of roaming zones within the abstract device configuration.
 *
 * @ingroup config */
class RoamingZoneList: public ConfigObjectList
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit RoamingZoneList(QObject *parent=nullptr);

  /** Returns a set of unique channels used in all roaming zones. */
  QSet<DigitalChannel *> uniqueChannels() const;
  /** Adds all channels of all roaming zones to the given set. */
  void uniqueChannels(QSet<DigitalChannel *> &channels) const;

  /** Returns the roaming zone at the given index. */
  RoamingZone *zone(int idx) const;

  int add(ConfigObject *obj, int row=-1);

  /** Implementation of QAbstractListModel, returns the number of rows. */
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  /** Implementation of QAbstractListModel, returns the item data at the given index. */
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  /** Implementation of QAbstractListModel, returns the header data at the given section. */
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
};

#endif // ROAMINGZONE_HH
