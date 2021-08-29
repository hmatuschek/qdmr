#ifndef ZONE_HH
#define ZONE_HH

#include "configobject.hh"
#include <QAbstractListModel>
#include <QVector>

class Channel;
class Config;

/** Represents a list of channels that are part of a zone.
 * @ingroup conf */
class ZoneChannelList: public QAbstractListModel
{
  Q_OBJECT

public:
  /** Constructs an empty zone channel list. */
  explicit ZoneChannelList(QObject *parent=nullptr);

  /** Returns the number of channels within this zone. */
	int count() const;
  /** Clears the zone. */
	void clear();

  /** Returns the channel at the given index. */
	Channel *channel(int idx) const;
  /** Appends a channel to the zone. */
	bool addChannel(Channel *channel);
  /** Removes the channel at the given index from the zone. */
	bool remChannel(int idx);
  /** Removes the given channel from the zone. */
	bool remChannel(Channel *channel);

	/** Implementation of QAbstractListModel, returns the number of rows. */
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
  /** Implementation of QAbstractListModel, returns the item data at the given index. */
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  /** Implementation of QAbstractListModel, returns the header data at the given section. */
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

signals:
  /** Gets emitted whenever the zone gets modified. */
	void modified();

protected slots:
  /** Internal used callback to handle deleted channels. */
	void onChannelDeleted(QObject *obj);

protected:
  /** The channel list. */
	QVector<Channel *> _channels;
};



/** Represents a zone within the generic configuration.
 * @ingroup conf */
class Zone : public ConfigObject
{
	Q_OBJECT

public:
  /** Constructs an empty Zone with the given name. */
	explicit Zone(const QString &name, QObject *parent = nullptr);

  using ConfigObject::serialize;

  /** Returns the name of the zone. */
	const QString &name() const;
  /** Sets the name of the zone. */
	bool setName(const QString &name);

  /** Retruns the list of channels for VFO A in this zone. */
  const ZoneChannelList *A() const;
  /** Retruns the list of channels for VFO A in this zone. */
  ZoneChannelList* A();
  /** Retruns the list of channels for VFO B in this zone. */
  const ZoneChannelList *B() const;
  /** Retruns the list of channels for VFO B in this zone. */
  ZoneChannelList* B();

signals:
  /** Gets emitted whenever the zone gets modified. */
	void modified();

protected:
  bool serialize(YAML::Node &node, const Context &context);

protected:
  /** Holds the name of the zone. */
	QString _name;
  /** List of channels for VFO A. */
  ZoneChannelList *_A;
  /** List of channels for VFO B. */
  ZoneChannelList *_B;
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
};


#endif // ZONE_HH
