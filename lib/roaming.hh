#ifndef ROAMINGZONE_HH
#define ROAMINGZONE_HH

#include <QAbstractListModel>
#include <channel.hh>


class RoamingZone : public QAbstractListModel
{
  Q_OBJECT

public:
  explicit RoamingZone(const QString &name, QObject *parent = nullptr);

  /** Returns the number of zones. */
  int count() const;
  /** Clears the zone list. */
  void clear();

  const QString &name() const;
  void setName(const QString &name);

  DigitalChannel *channel(int idx) const;
  bool addChannel(DigitalChannel *ch, int row=-1);
  bool remChannel(int row);
  bool remChannel(DigitalChannel *ch);

  /** Moves the channel at the given row one up. */
  bool moveUp(int row);
  /** Moves the channel at the given row one down. */
  bool moveDown(int row);

  /** Implementation of QAbstractListModel, returns the number of rows. */
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  /** Implementation of QAbstractListModel, returns the item data at the given index. */
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  /** Implementation of QAbstractListModel, returns the header data at the given section. */
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

signals:
  /** Gets emitted whenever the zone list or any of its zones is modified. */
  void modified();

protected slots:
  /** Internal used callback to handle deleted channels. */
  void onChannelDeleted(QObject *obj);

protected:
  QString _name;
  QList<DigitalChannel *> _channel;
};


class DefaultRoamingZone: public RoamingZone
{
  Q_OBJECT

protected:
  explicit DefaultRoamingZone(QObject *parent=nullptr);

public:
  static DefaultRoamingZone *get();

protected:
  static DefaultRoamingZone *_instance;
};


class RoamingZoneList: public QAbstractListModel
{
  Q_OBJECT

public:
  explicit RoamingZoneList(QObject *parent=nullptr);

  /** Returns the number of roaming zones. */
  int count() const;
  /** Clears the roaming zone list. */
  void clear();
  /** Returns the index of the given roaming zone within this list.
   * @returns Index or -1 if zone is not a member. */
  int indexOf(RoamingZone *zone) const;

  /** Returns a set of unique channels used in all roaming zones. */
  QSet<DigitalChannel *> uniqueChannels() const;
  /** Adds all channels of all roaming zones to the given set. */
  void uniqueChannels(QSet<DigitalChannel *> &channels) const;

  /** Returns the roaming zone at the given index. */
  RoamingZone *zone(int idx) const;
  /** Adds a roaming zone to the list at the given row.
   * If row<0, the roaming zone is appended to the list. */
  bool addZone(RoamingZone *zone, int row=-1);
  /** Removes the roaming zone at the given index. */
  bool remZone(int idx);
  /** Removes the given roaming zone from the list. */
  bool remZone(RoamingZone *zone);

  /** Moves the roaming zone at the given row one up. */
  bool moveUp(int row);
  /** Moves the roaming zone at the given row one down. */
  bool moveDown(int row);

  /** Implementation of QAbstractListModel, returns the number of rows. */
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  /** Implementation of QAbstractListModel, returns the item data at the given index. */
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  /** Implementation of QAbstractListModel, returns the header data at the given section. */
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

signals:
  /** Gets emitted whenever the zone list or any of its zones is modified. */
  void modified();

protected slots:
  /** Internal used callback to handle deleted roaming zones. */
  void onZoneDeleted(QObject *obj);

protected:
  /** The list of roaming zones. */
  QVector<RoamingZone *> _zones;
};

#endif // ROAMINGZONE_HH
