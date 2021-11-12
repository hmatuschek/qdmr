#ifndef CONFIGOBJECT_HH
#define CONFIGOBJECT_HH

#include <QObject>
#include <QString>
#include <QHash>
#include <QVector>
#include <QMetaProperty>

#include <yaml-cpp/yaml.h>

#include "errorstack.hh"

// Forward declaration
class ConfigObject;
class ConfigExtension;

/** Helper function to test property type. */
template <class T>
bool propIsInstance(QMetaProperty &prop) {
  if (QMetaType::UnknownType == prop.userType())
    return false;
  QMetaType type(prop.userType());
  if (! (QMetaType::PointerToQObject & type.flags()))
    return false;
  const QMetaObject *propType = type.metaObject();
  for (; nullptr != propType; propType = propType->superClass()) {
    if (0==strcmp(T::staticMetaObject.className(), propType->className()))
      return true;
  }
  return false;
}

/** Base class for all configuration objects (channels, zones, contacts, etc).
 *
 * @ingroup config */
class ConfigItem : public QObject, public ErrorStack
{
  Q_OBJECT

public:
  /** Parse context for config objects. During serialization, each config object gets an
   * ID assigned. When reading the config, these IDs must be matched back to the corresponding
   * objects. This is done using this context. */
  class Context
  {
  public:
    /** Empty constructor. */
    Context();
    /** Destructor. */
    virtual ~Context();

    /** Returns the read version string. */
    const QString &version() const;
    /** Sets the version string. */
    void setVersion(const QString &ver);

    /** Returns @c true, if the context contains the given object. */
    virtual bool contains(ConfigObject *obj) const;
    /** Returns @c true, if the context contains the given ID. */
    virtual bool contains(const QString &id) const;

    /** Returns ID of the given object. */
    virtual QString getId(ConfigObject *obj) const;
    /** Returns the object for the given ID. */
    virtual ConfigObject *getObj(const QString &id) const;

    /** Associates the given object with the given ID. */
    virtual bool add(const QString &id, ConfigObject *);

    /** Returns @c true if the property of the class has the specified tag associated. */
    static bool hasTag(const QString &className, const QString &property, const QString &tag);
    /** Returns @c true if the property of the class has the specified object as a tag associated. */
    static bool hasTag(const QString &className, const QString &property, ConfigObject *obj);
    /** Returns the object associated with the tag for the property of the class. */
    static ConfigObject *getTag(const QString &className, const QString &property, const QString &tag);
    /** Returns the tag associated with the object for the property of the class. */
    static QString getTag(const QString &className, const QString &property, ConfigObject *obj);
    /** Associates the given object with the tag for the property of the given class. */
    static void setTag(const QString &className, const QString &property, const QString &tag, ConfigObject *obj);

  protected:
    /** The version string. */
    QString _version;
    /** ID->OBJ look-up table. */
    QHash<QString, ConfigObject *> _objects;
    /** OBJ->ID look-up table. */
    QHash<ConfigObject*, QString> _ids;
    /** Maps tags to singleton objects. */
    static QHash<QString, QHash<QString, ConfigObject *>> _tagObjects;
    /** Maps singleton objects to tags. */
    static QHash<QString, QHash<ConfigObject *, QString>> _tagNames;
  };

protected:
  /** Hidden constructor.
   * @param parent Specifies the QObject parent. */
  explicit ConfigItem(QObject *parent = nullptr);

public:
  /** Copies the given item into this. */
  virtual bool copy(const ConfigItem &other);
  /** Clones this item. */
  virtual ConfigItem *clone() const = 0;

public:
  /** Recursively labels the config object.
   * Does not assign a label if the @c idBase passed to the constructor is empty. */
  virtual bool label(Context &context);
  /** Recursively serializes the configuration to YAML nodes.
   * The complete configuration must be labeled first. */
  virtual YAML::Node serialize(const Context &context);

  /** Allocates an instance for the given property on the given YAML node. */
  virtual ConfigItem *allocateChild(QMetaProperty &prop, const YAML::Node &node, const Context &ctx) = 0;
  /** Parses the given YAML node, updates the given object and updates the given context (IDs). */
  virtual bool parse(const YAML::Node &node, Context &ctx);
  /** Links the given object to the rest of the codeplug using the given context. */
  virtual bool link(const YAML::Node &node, const Context &ctx);

  /** Clears the config object. */
  virtual void clear();

  /** Returns @c true if this object is of class @c Object. */
  template <class Object>
  bool is() const {
    return nullptr != qobject_cast<const Object*>(this);
  }

  /** Casts this object to the given type. */
  template <class Object>
  const Object *as() const {
    return qobject_cast<const Object*>(this);
  }

  /** Casts this object to the given type. */
  template <class Object>
  Object *as() {
    return qobject_cast<Object *>(this);
  }

protected:
  /** Recursively serializes the configuration to YAML nodes.
   * The complete configuration must be labeled first. */
  virtual bool populate(YAML::Node &node, const Context &context);

signals:
  /** Gets emitted once the config object is modified. */
  void modified(ConfigItem *obj);
};


/** Base class of all labled and named objects.
 * @ingroup config */
class ConfigObject: public ConfigItem
{
  Q_OBJECT

  /** The name of the object. */
  Q_PROPERTY(QString name READ name WRITE setName)

protected:
  /** Hidden constructor.
   * @param idBase Prefix for all generated IDs. If empty, no ID gets generated.
   * @param parent Specifies the QObject parent. */
  ConfigObject(const QString &idBase="id", QObject *parent = nullptr);

  /** Hidden constructor.
   * @param name Name of the object.
   * @param idBase Prefix for all generated IDs. If empty, no ID gets generated.
   * @param parent Specifies the QObject parent. */
  ConfigObject(const QString &name, const QString &idBase="id", QObject *parent = nullptr);

public:
  /** Copies the given object into this. */
  virtual bool copy(const ConfigItem &other);

public:
  /** Returns the name of the object. */
  virtual const QString &name() const;
  /** Sets the name of the object. */
  virtual void setName(const QString &name);

public:
  bool label(Context &context);
  bool parse(const YAML::Node &node, Context &ctx);

protected:
  virtual bool populate(YAML::Node &node, const Context &context);

protected:
  /** Holds the base string to derive an ID from. All objects need some ID to be referenced within
   * a codeplug file. */
  QString _idBase;
  /** Holds the name of the object. */
  QString _name;
};


/** Base class of all device/vendor specific confiuration extensions.
 * This class already implements the serialization of all @c QMetaObject
 * properties. */
class ConfigExtension : public ConfigItem
{
Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit ConfigExtension(QObject *parent=nullptr);

public:
  bool copy(const ConfigItem &other);

protected:
  bool populate(YAML::Node &node, const ConfigObject::Context &context);
};


/** Generic list class for config objects.
 * @ingroup config */
class AbstractConfigObjectList: public QObject, public ErrorStack
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit AbstractConfigObjectList(const QMetaObject &elementType=ConfigObject::staticMetaObject, QObject *parent = nullptr);

public:
  /** Copies all elements from @c other to this list. */
  virtual bool copy(const AbstractConfigObjectList &other);

  /** Recursively labels the config object. */
  virtual bool label(ConfigItem::Context &context) = 0;
  /** Recursively serializes the configuration to YAML nodes.
   * The complete configuration must be labeled first. */
  virtual YAML::Node serialize(const ConfigItem::Context &context) = 0;

  /** Returns the number of elements in the list. */
  virtual int count() const;
  /** Retunrs the index of the given object within the list. */
  virtual int indexOf(ConfigObject *obj) const;
  /** Clears the list. */
  virtual void clear();

  /** Returns the list element at the given index or @c nullptr if out of bounds. */
  virtual ConfigObject *get(int idx) const;
  /** Adds an element to the list. */
  virtual int add(ConfigObject *obj, int row=-1);
  /** Removes an element from the list. */
  virtual bool take(ConfigObject *obj);
  /** Removes an element from the list (and deletes it if owned). */
  virtual bool del(ConfigObject *obj);

  /** Moves the channel at index @c idx one step up. */
  virtual bool moveUp(int idx);
  /** Moves the channels at one step up. */
  virtual bool moveUp(int first, int last);
  /** Moves the channel at index @c idx one step down. */
  virtual bool moveDown(int idx);
  /** Moves the channels one step down. */
  virtual bool moveDown(int first, int last);

  /** Returns the element type for this list. */
  const QMetaObject &elementType() const;

signals:
  /** Gets emitted if an element was added to the list. */
  void elementAdded(int idx);
  /** Gets emitted if one of the lists elements gets modified. */
  void elementModified(int idx);
  /** Gets emitted if one of the lists elements gets deleted. */
  void elementRemoved(int idx);

private slots:
  /** Internal used callback to handle modified elments. */
  void onElementModified(ConfigItem *obj);
  /** Internal used callback to handle deleted elments. */
  void onElementDeleted(QObject *obj);

protected:
  /** Holds the static QMetaObject of the element type. */
  QMetaObject _elementType;
  /** Holds the list items. */
  QVector<ConfigObject *> _items;
};


/** List class for config objects.
 * This list owns the config objects it contains. See @c ConfigObjectRefList for a list of weak
 * references.
 * @ingroup config */
class ConfigObjectList: public AbstractConfigObjectList
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit ConfigObjectList(const QMetaObject &elementType=ConfigItem::staticMetaObject, QObject *parent = nullptr);

public:
  int add(ConfigObject *obj, int row=-1);
  bool take(ConfigObject *obj);
  bool del(ConfigObject *obj);
  void clear();

  /** Allocates a member objects for the given YAML node. */
  virtual ConfigItem *allocateChild(const YAML::Node &node, ConfigItem::Context &ctx) = 0;
  /** Parses the list from the YAML node. */
  virtual bool parse(const YAML::Node &node, ConfigItem::Context &ctx);
  /** Links the list from the given YAML node. */
  virtual bool link(const YAML::Node &node, const ConfigItem::Context &ctx);

  bool label(ConfigItem::Context &context);
  YAML::Node serialize(const ConfigItem::Context &context);
};


/** List class for config objects.
 * This list only references the config objects, see @c ConfigObjectList for a list that owns the
 * config objects.
 * @ingroup config */
class ConfigObjectRefList: public AbstractConfigObjectList
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit ConfigObjectRefList(const QMetaObject &elementType=ConfigObject::staticMetaObject, QObject *parent = nullptr);

public:
  bool label(ConfigItem::Context &context);
  YAML::Node serialize(const ConfigItem::Context &context);
};


#endif // CONFIGOBJECT_HH
