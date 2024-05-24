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
  Q_CLASSINFO("IdPrefix", "roam")

  /** The channels in the roaming zone.
   * @todo This property is marked non-scriptable to handle references to DMR channels before
   *       version 0.11.0. Remove in future. */
  Q_PROPERTY(RoamingChannelRefList * channels READ channels SCRIPTABLE false)

public:
  /** Default constructor. */
  Q_INVOKABLE explicit RoamingZone(QObject *parent=nullptr);

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

  /** Returns @c true, if the given roaming channel is member of this zone. */
  bool contains(const RoamingChannel *ch) const;
  /** Returns the roaming channel, which is the member at index @c idx (0-based).
   * @param idx Specifies the index of the member channel. */
  RoamingChannel *channel(int idx) const;
  /** Adds a channel to the roaming zone.
   * @param ch Specifies the channel to add.
   * @param row Speicifies the index where to insert the channel
   *        (optional, default insert at end). */
  int addChannel(RoamingChannel *ch, int row=-1);
  /** Removes the channel from the roaming zone at index @c row. */
  bool remChannel(int row);
  /** Removes the given channel from the roaming zone. */
  bool remChannel(RoamingChannel *ch);

  /** Returns the list of digital channels in this roaming zone. */
  const RoamingChannelRefList *channels() const;
  /** Returns the list of digital channels in this roaming zone. */
  RoamingChannelRefList *channels();

  /** Links the channel reference list.
   * @todo Implemented for backward compatibility with version 0.10.0, remove for 1.0.0. */
  bool link(const YAML::Node &node, const Context &ctx, const ErrorStack &err);
  /** Serializes the channel reference list.
   * @todo Implemented for backward compatibility with version 0.10.0, remove for 1.0.0. */
  bool populate(YAML::Node &node, const Context &context, const ErrorStack &err);

protected:
  /** Holds the actual channels of the roaming zone. */
  RoamingChannelRefList _channel;
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

  /** Returns the roaming zone at the given index. */
  RoamingZone *zone(int idx) const;

  int add(ConfigObject *obj, int row=-1, bool unique=true);

public:
  ConfigItem *allocateChild(const YAML::Node &node, ConfigItem::Context &ctx, const ErrorStack &err=ErrorStack());
};

#endif // ROAMINGZONE_HH
