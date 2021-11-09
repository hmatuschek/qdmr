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

  /** The name of the radio ID. */
  Q_PROPERTY(QString name READ name WRITE setName)
  /** The number of the radio ID. */
  Q_PROPERTY(unsigned number READ number WRITE setNumber)

public:
  /** Default constructor. */
  explicit RadioID(QObject *parent=nullptr);

  /** Constructor.
   * @param name Specifies the name of the ID.
   * @param number Specifies the DMR ID.
   * @param parent Specifies the parent QObject owning this object. */
  RadioID(const QString &name, uint32_t number, QObject *parent = nullptr);

  /** Returns the name of the DMR ID. */
  const QString &name() const;
  /** Sets the name of the DMR ID. */
  void setName(const QString &name);

  /** Returns the DMR ID. */
  uint32_t number() const;
  /** Sets the DMR ID. */
  void setNumber(uint32_t number);

  YAML::Node serialize(const Context &context);
  ConfigObject *allocateChild(QMetaProperty &prop, const YAML::Node &node, const Context &ctx);
  bool parse(const YAML::Node &node, ConfigObject::Context &ctx);
  bool link(const YAML::Node &node, const ConfigObject::Context &ctx);

signals:
  /** Gets emitted once the DMR is changed. */
  void modified();

protected:
  /** Holds the name of the DMR ID. */
  QString _name;
  /** Holds the DMR ID. */
  uint32_t _number;
};


/** A singleton radio ID representing the default DMR radio ID within the abstract config.
 * @ingroup conf */
class DefaultRadioID: public RadioID
{
  Q_OBJECT

protected:
  /** Contstructor. */
  explicit DefaultRadioID(QObject *parent=nullptr);

public:
  /** Factory method returning the singleton instance. */
  static DefaultRadioID *get();

private:
  /** The singleton instance. */
  static DefaultRadioID *_instance;
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
  /** Sets the default DMR ID. Pass idx=-1 to clear default ID. */
  bool setDefaultId(int idx);
  /** Searches the DMR ID object associated with the given DMR ID. */
  RadioID *find(uint32_t id) const;

  int add(ConfigObject *obj, int row=-1);

  /** Adds the given DMR ID. */
  virtual int addId(const QString &name, uint32_t id);
  /** Deletes and removes the given DMR ID. */
  virtual bool delId(uint32_t id);

public:
  ConfigObject *allocateChild(const YAML::Node &node, ConfigObject::Context &ctx);

protected slots:
  /** Gets call whenever the default DMR ID gets deleted. */
  void onDefaultIdDeleted();

protected:
  /** Holds a weak reference to the default DMR radio ID. */
  RadioID *_default;
};


#endif // RADIOID_HH
