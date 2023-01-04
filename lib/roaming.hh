#ifndef ROAMINGZONE_HH
#define ROAMINGZONE_HH

#include <QAbstractListModel>
#include "configreference.hh"
#include "roamingchannel.hh"

/** Represents a RoamingZone within the abstract device configuration.
 *
 * A roaming zone collects a set of repeaters that act as alternatives to each other. When a selected
 * repeater gets out of range, another one might be found automatically from within the roaming zone.
 *
 * @ingroup config */
class RoamingZone : public ConfigObject
{
  Q_OBJECT

  /** The channels in the roaming zone. */
  Q_PROPERTY(DMRChannelRefList * channels READ channels)

public:
  /** Default constructor. */
  explicit RoamingZone(QObject *parent=nullptr);

  /** Constructor.
   * @param name Specifies the name of the roaming zone.
   * @param parent Specifies the QObject parent of this zone. */
  RoamingZone(const QString &name, QObject *parent = nullptr);

  /** Copies the given zone. */
  RoamingZone &operator =(const RoamingZone &other);
  ConfigItem *clone() const;

  /** Returns the number of zones. */
  int count() const;

  /** Clears the zone list. */
  void clear();

  /** Returns the digital channel, which is the member at index @c idx (0-based).
   * @param idx Specifies the index of the member channel. */
  DMRChannel *channel(int idx) const;
  /** Adds a channel to the roaming zone.
   * @param ch Specifies the channel to add.
   * @param row Speicifies the index where to insert the channel
   *        (optional, default insert at end). */
  int addChannel(DMRChannel *ch, int row=-1);
  /** Removes the channel from the roaming zone at index @c row. */
  bool remChannel(int row);
  /** Removes the given channel from the roaming zone. */
  bool remChannel(DMRChannel *ch);

  /** Returns the list of digital channels in this roaming zone. */
  const DMRChannelRefList *channels() const;
  /** Returns the list of digital channels in this roaming zone. */
  DMRChannelRefList *channels();

protected:
  /** Holds the actual channels of the roaming zone. */
  DMRChannelRefList _channel;
};


/** Dummy roaming zone class that represents the default roaming zone.
 *
 * This is a singleton class. That is, there can only be one instance of this class.
 * @ingroup config */
class DefaultRoamingZone: public RoamingZone
{
  Q_OBJECT

protected:
  /** Hidden constructor.
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
  QSet<DMRChannel *> uniqueChannels() const;
  /** Adds all channels of all roaming zones to the given set. */
  void uniqueChannels(QSet<DMRChannel *> &channels) const;

  /** Returns the roaming zone at the given index. */
  RoamingZone *zone(int idx) const;

  int add(ConfigObject *obj, int row=-1);

public:
  ConfigItem *allocateChild(const YAML::Node &node, ConfigItem::Context &ctx, const ErrorStack &err=ErrorStack());
};

#endif // ROAMINGZONE_HH
