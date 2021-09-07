#ifndef CONFIGOBJECT_HH
#define CONFIGOBJECT_HH

#include <QObject>
#include <QString>
#include <QHash>
#include <QVector>

#include <yaml-cpp/yaml.h>

// Forward declaration
class ConfigExtension;


/** Base class for configuration objects (channels, zones, contacts, etc).
 * @ingroup config */
class ConfigObject : public QObject
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

  protected:
    /** The version string. */
    QString _version;
    /** ID->OBJ look-up table. */
    QHash<QString, ConfigObject*> _objects;
    /** OBJ->ID look-up table. */
    QHash<ConfigObject*, QString> _ids;
  };

protected:
  /** Hidden constructor. */
  ConfigObject(const QString &idBase="id", QObject *parent = nullptr);

public:
  /** Recursively labels the config object. */
  virtual bool label(Context &context);
  /** Recursively serializes the configuration to YAML nodes.
   * The complete configuration must be labeled first. */
  virtual YAML::Node serialize(const Context &context);

  /** Returns @c true if the config object has the given extension. */
  virtual bool hasExtension(const QString &name) const;
  /** Retunrs the list of extension names associated to this object. */
  virtual QList<QString> extensionNames() const;
  /** Returns the extension object. */
  virtual const ConfigObject *extension(const QString &name) const;
  /** Returns the extension object. */
  virtual ConfigObject *extension(const QString &name);
  /** Adds an extension to this object.
   * A existing extension with the same name will be replaced.*/
  virtual void addExtension(const QString &name, ConfigObject *ext);

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
  virtual bool serialize(YAML::Node &node, const Context &context);

signals:
  /** Gets emitted once the config object is modified. */
  void modified(ConfigObject *obj);

protected:
  /** Base name for all IDs, used by @c label(). */
  QString _idBase;
  /** The table of extensions. */
  QHash<QString, ConfigObject *> _extensions;
};


/** Base class of all device/vendor specific confiuration extensions.
 * This class already implements the serialization of all @c QMetaObject
 * properties. */
class ConfigExtension : public ConfigObject
{
Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit ConfigExtension(const QString &idBase, QObject *parent=nullptr);

protected:
  bool serialize(YAML::Node &node, const Context &context);
};


/** Generic list class for config objects.
 * @ingroup config */
class AbstractConfigObjectList: public QObject
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit AbstractConfigObjectList(const QMetaObject &elementType=ConfigObject::staticMetaObject, QObject *parent = nullptr);

public:
  /** Recursively labels the config object. */
  virtual bool label(ConfigObject::Context &context) = 0;
  /** Recursively serializes the configuration to YAML nodes.
   * The complete configuration must be labeled first. */
  virtual YAML::Node serialize(const ConfigObject::Context &context) = 0;

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
  void onElementModified(ConfigObject *obj);
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
  explicit ConfigObjectList(const QMetaObject &elementType=ConfigObject::staticMetaObject, QObject *parent = nullptr);

public:
  int add(ConfigObject *obj, int row=-1);
  bool take(ConfigObject *obj);
  bool del(ConfigObject *obj);

  bool label(ConfigObject::Context &context);
  YAML::Node serialize(const ConfigObject::Context &context);
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
  bool label(ConfigObject::Context &context);
  YAML::Node serialize(const ConfigObject::Context &context);
};


#endif // CONFIGOBJECT_HH
