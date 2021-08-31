#ifndef RADIOID_HH
#define RADIOID_HH

#include "configobject.hh"
#include <QAbstractListModel>

/** Represents a DMR ID (radio ID) within the abstract config.
 *
 * @ingroup conf */
class RadioID : public ConfigObject
{
  Q_OBJECT

  Q_PROPERTY(QString name READ name WRITE setName)
  Q_PROPERTY(uint number READ number WRITE setNumber)

public:
  /** Constructor.
   * @param name Specifies the name of the ID.
   * @param id Specifies the DMR ID.
   * @param parent Specifies the parent QObject owning this object. */
  explicit RadioID(const QString &name, uint32_t number, QObject *parent = nullptr);

  /** Returns the name of the DMR ID. */
  const QString &name() const;
  /** Sets the name of the DMR ID. */
  void setName(const QString &name);

  /** Returns the DMR ID. */
  uint32_t number() const;
  /** Sets the DMR ID. */
  void setNumber(uint32_t number);

  YAML::Node serialize(const Context &context);

signals:
  /** Gets emitted once the DMR is changed. */
  void modified();

protected:
  bool serialize(YAML::Node &node, const Context &context);

protected:
  /** Holds the name of the DMR ID. */
  QString _name;
  /** Holds the DMR ID. */
  uint32_t _number;
};


/** Represents the list of configued DMR IDs (radio IDs) within the abstract config.
 * There must always be at least one valid DMR ID. The first (index 0) ID is always the default
 * DMR ID of the radio.
 *
 * @ingroup conf */
class RadioIDList: public ConfigObjectList
{
  Q_OBJECT

public:
  /** Contructor. */
  explicit RadioIDList(QObject *parent=nullptr);

  void clear();

  /** Returns the radio ID at the given index. */
  RadioID *getId(int idx) const;
  /** Returns the current default ID for the radio. */
  RadioID * defaultId() const;
  /** Sets the default DMR ID. This changes the index of all IDs. */
  bool setDefaultId(uint idx);
  /** Searches the DMR ID object associated with the given DMR ID. */
  RadioID *find(uint32_t id) const;

  int add(ConfigObject *obj, int row=-1);

  /** Adds the given DMR ID. */
  virtual int addId(const QString &name, uint32_t id);
  /** Deletes and removes the given DMR ID. */
  virtual bool delId(uint32_t id);

protected slots:
  /** Gets call whenever the default DMR ID gets deleted. */
  void onDefaultIdDeleted();

protected:
  /** Holds a weak reference to the default DMR radio ID. */
  RadioID *_default;
};


#endif // RADIOID_HH
