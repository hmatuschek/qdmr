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

public:
  /** Constructor.
   * @param id Specifies the DMR ID.
   * @param parent Specifies the parent QObject owning this object. */
  explicit RadioID(const QString &name, uint32_t id, QObject *parent = nullptr);

  /** Returns the name of the DMR ID. */
  const QString &name() const;
  /** Sets the name of the DMR ID. */
  void setName(const QString &name);

  /** Returns the DMR ID. */
  uint32_t id() const;
  /** Sets the DMR ID. */
  void setId(uint32_t id);

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
  uint32_t _id;
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
  RadioID * getDefaultId() const;
  /** Searches the DMR ID object associated with the given DMR ID. */
  RadioID *find(uint32_t id) const;
  /** Sets the default DMR ID. This changes the index of all IDs. */
  bool setDefault(uint idx);

  int add(ConfigObject *obj, int row=-1);

  /** Adds the given DMR ID. */
  virtual int addId(const QString &name, uint32_t id);
  /** Deletes and removes the given DMR ID. */
  virtual bool delId(uint32_t id);

};


#endif // RADIOID_HH
