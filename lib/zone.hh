#ifndef ZONE_HH
#define ZONE_HH

#include "configobject.hh"
#include <QAbstractListModel>
#include <QVector>

#include "configreference.hh"
#include "anytone_extension.hh"

class Config;

/** Represents a zone within the generic configuration.
 * @ingroup conf */
class Zone : public ConfigObject
{
  Q_OBJECT
  Q_CLASSINFO("IdPrefix", "zone");

  /** The A channels. */
  Q_PROPERTY(ChannelRefList* A READ A)
  /** The B channels. */
  Q_PROPERTY(ChannelRefList* B READ B)

  /** The AnyTone extensions. */
  Q_PROPERTY(AnytoneZoneExtension* anytone READ anytoneExtension WRITE setAnytoneExtension)

public:
  /** Default constructor. */
  explicit Zone(QObject *parent=nullptr);
  /** Constructs an empty Zone with the given name. */
  Zone(const QString &name, QObject *parent = nullptr);

  /** Copies the given zone. */
  Zone &operator =(const Zone &other);

  ConfigItem *clone() const;

  /** Clears this zone. */
  void clear();

  /** Returns the list of channels for VFO A in this zone. */
  const ChannelRefList *A() const;
  /** Returns the list of channels for VFO A in this zone. */
  ChannelRefList* A();
  /** Returns the list of channels for VFO B in this zone. */
  const ChannelRefList *B() const;
  /** Returns the list of channels for VFO B in this zone. */
  ChannelRefList* B();

  /** Returns @c true, if the zone contains the given channel. */
  bool contains(Channel *obj) const;

  /** Returns the AnyTone extension. */
  AnytoneZoneExtension *anytoneExtension() const;
  /** Sets the AnyTone extension. */
  void setAnytoneExtension(AnytoneZoneExtension *ext);

signals:
  /** Gets emitted whenever the zone gets modified. */
	void modified();

protected:
  /** List of channels for VFO A. */
  ChannelRefList _A;
  /** List of channels for VFO B. */
  ChannelRefList _B;
  /** Owns the AnyTone extensions. */
  AnytoneZoneExtension *_anytone;
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
  ConfigItem *allocateChild(const YAML::Node &node, ConfigItem::Context &ctx, const ErrorStack &err=ErrorStack());
};


#endif // ZONE_HH
