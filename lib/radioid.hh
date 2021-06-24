#ifndef RADIOID_HH
#define RADIOID_HH

#include <QAbstractListModel>

/** Represents a DMR ID (radio ID) within the abstract config.
 *
 * @ingroup conf */
class RadioID : public QObject
{
  Q_OBJECT

public:
  /** Constructor.
   * @param id Specifies the DMR ID.
   * @param parent Specifies the parent QObject owning this object. */
  explicit RadioID(uint32_t id, QObject *parent = nullptr);

  /** Returns the DMR ID. */
  uint32_t id() const;
  /** Sets the DMR ID. */
  void setId(uint32_t id);

signals:
  /** Gets emitted once the DMR is changed. */
  void modified();

protected:
  /** Holds the DMR ID. */
  uint32_t _id;
};


/** Represents the list of configued DMR IDs (radio IDs) within the abstract config.
 * There must always be at least one valid DMR ID. The first (index 0) ID is always the default
 * DMR ID of the radio.
 *
 * @ingroup conf */
class RadioIDList: public QAbstractListModel
{
  Q_OBJECT

public:
  /** Contructor. */
  explicit RadioIDList(QObject *parent=nullptr);

  /** Clears the list, a default DMR ID is set to 0. */
  void clear();

  /** Returns the number of defined DMR IDs. */
  int count() const;
  /** Returns the current default ID for the radio. */
  RadioID * getDefaultId() const;
  /** Returns the DMR ID specified by the given index. */
  RadioID * getId(uint idx) const;
  /** Searches the DMR ID object associated with the given DMR ID. */
  RadioID *find(uint32_t id) const;
  /** Returns the index of the given DMR ID object. */
  int indexOf(RadioID *id) const;
  /** Sets the default DMR ID. This changes the index of all IDs. */
  bool setDefault(uint idx);

  /** Adds the given DMR ID object. */
  int addId(RadioID *id);
  /** Adds the given DMR ID. */
  int addId(uint32_t id);

  /** Deletes and removes the given DMR ID object. */
  bool delId(RadioID *id);
  /** Deletes and removes the given DMR ID. */
  bool delId(uint32_t id);

  /** Returns the number of DMR IDs, implements the QAbstractListModel interface. */
  int rowCount(const QModelIndex &parent) const;
  /** Returns the DMR ID, implements the QAbstractListModel interface. */
  QVariant data(const QModelIndex &index, int role) const;

signals:
  /** Gets emitted on any change of the list. */
  void modified();

protected slots:
  /** Internal used callback whenever a DMR ID object gets deleted. */
  void onIdDeleted(QObject *obj);

protected:
  /** Holds the list of DMR IDs. */
  QList<RadioID *> _ids;
};


#endif // RADIOID_HH
