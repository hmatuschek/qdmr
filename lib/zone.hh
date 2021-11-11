#ifndef ZONE_HH
#define ZONE_HH

#include "configobject.hh"
#include <QAbstractListModel>
#include <QVector>

#include "configreference.hh"
class Config;


/** Represents a zone within the generic configuration.
 * @ingroup conf */
class Zone : public ConfigObject
{
	Q_OBJECT

  /** The A channels. */
  Q_PROPERTY(ChannelRefList* A READ A)
  /** The B channels. */
  Q_PROPERTY(ChannelRefList* B READ B)

public:
  /** Default constructor. */
  explicit Zone(QObject *parent=nullptr);
  /** Constructs an empty Zone with the given name. */
  Zone(const QString &name, QObject *parent = nullptr);

  /** Copies the given zone. */
  Zone &operator =(const Zone &other);

  bool copy(const ConfigItem &other);
  ConfigItem *clone() const;

  /** Clears this zone. */
  void clear();

  /** Retruns the list of channels for VFO A in this zone. */
  const ChannelRefList *A() const;
  /** Retruns the list of channels for VFO A in this zone. */
  ChannelRefList* A();
  /** Retruns the list of channels for VFO B in this zone. */
  const ChannelRefList *B() const;
  /** Retruns the list of channels for VFO B in this zone. */
  ChannelRefList* B();

signals:
  /** Gets emitted whenever the zone gets modified. */
	void modified();

public:
  ConfigItem *allocateChild(QMetaProperty &prop, const YAML::Node &node, const Context &ctx);

protected:
  /** List of channels for VFO A. */
  ChannelRefList _A;
  /** List of channels for VFO B. */
  ChannelRefList _B;
};


/** Represents the list of zones within the generic configuration.
 * @ingroup conf */
class ZoneList : public ConfigObjectList
{
	Q_OBJECT

public:
  /** Constructs an empty list of zones. */
	explicit ZoneList(QObject *parent = nullptr);

  /** Returns the zone at the given index. */
	Zone *zone(int idx) const;

  int add(ConfigObject *obj, int row=-1);

public:
  ConfigItem *allocateChild(const YAML::Node &node, ConfigItem::Context &ctx);
};


#endif // ZONE_HH
