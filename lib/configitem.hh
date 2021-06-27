#ifndef CONFIGITEM_HH
#define CONFIGITEM_HH

#include <QObject>
#include <QVector>
#include <QSet>
#include <QHash>
#include <yaml-cpp/yaml.h>

#include "channel.hh"
#include "signaling.hh"
#include "logger.hh"


namespace Configuration {

class Object;
class ScanList;
class GroupList;
class Positioning;
class RoamingZone;
class RadioId;
class APRSPositioning;
class Channel;
class DigitalChannel;
class AnalogChannel;
class DigitalContact;
class GroupCall;
class Reference;

/** Base class of all YAML extensible codeplug/config items.
 * These objects form the CST of the parsed YAML codeplug.
 * @ingroup conf */
class Item : public QObject
{
  Q_OBJECT

public:
  /** Base class of all config declarations. */
  class Declaration
  {
  protected:
    /** Hidden constructor, cannot be instantiated directly.
     * @param desc Gives some short description of the item.
     * @param mandatory If @c true, the item is mandatory for a complete config. */
    Declaration(bool mandatory, const QString &desc="");

  public:
    /** Destructor. */
    virtual ~Declaration();

    /** Returns the description of the config item. */
    const QString &description() const;
    /** Returns @c true if the item is mandatory for a complete config. */
    bool isMandatory() const;
    /** Returns the name of the declaration. */
    const QString &name() const;

    /** Returns @c true if this item is of given type @c T. */
    template <class T>
    bool is() const {
      return nullptr != dynamic_cast<const T *>(this);
    }

    /** Casts the item to the given type. */
    template <class T>
    const T* as() const {
      return dynamic_cast<const T *>(this);
    }

    /** Casts the item to the given type. */
    template <class T>
    T* as() {
      return dynamic_cast<T *>(this);
    }

    /** Verifies the structure of the document. Checks if all mandatory elements are present. */
    virtual bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const = 0;
    /** Verifies if all references are defined. */
    virtual bool verifyReferences(const YAML::Node &node, const QSet<QString> &ctx, QString &msg) const;

    /** Implements the definition phase of the two-step parsing. */
    virtual Item *parseDefine(const YAML::Node &node, QHash<QString, Item*> &ctx, QString &msg) const;
    /** Implements the link phase of the two-step parsing. */
    virtual bool parseLink(Item *item, const YAML::Node &node, QHash<QString, Item*> &ctx, QString &msg) const;

    /** Creates YAML nodes according to this declaration. */
    virtual YAML::Node generate(const Item *item, const QHash<const Item *, QString> &ctx, QString &msg) const = 0;

  protected:
    /** Allocates the parsed item. */
    virtual Item *parseAllocate(const YAML::Node &node, QString &msg) const = 0;
    /** Populates the parsed item. */
    virtual bool parsePopulate(Item *item, const YAML::Node &node, QHash<QString, Item*> &ctx, QString &msg) const = 0;

  protected:
    /** Holds the description of the item. */
    QString _description;
    /** If @c true, the item is mandatory for a complete config. */
    bool _mandatory;
    /** Optional name of declaration. */
    QString _name;
  };

protected:
  /** Hidden constructor.
   * @param declaration Specifies the declaration of the item.
   * @param parent Specifies the QObject parent. */
  Item(const Declaration *declaration, QObject *parent = nullptr);

public:
  /** Destructor. */
  virtual ~Item();

  /** Returns the declaraion of this item. */
  const Declaration *declaraion() const;

  /** Returns @c true if this item is of given type @c T. */
  template <class T>
  bool is() const {
    return nullptr != dynamic_cast<const T *>(this);
  }

  /** Casts the item to the given type. */
  template <class T>
  const T* as() const {
    return dynamic_cast<const T *>(this);
  }

  /** Casts the item to the given type. */
  template <class T>
  T* as() {
    return dynamic_cast<T *>(this);
  }

  /** Serializes the item for debug purposes. */
  virtual QString dump(const QString &prefix="") const;

  /** Recursively assignes Ids to objects. */
  virtual bool generateIds(QHash<const Item *, QString> &ctx, QString &message) const;
  /** Recursively creates YAML nodes. */
  virtual YAML::Node generateNode(const QHash<const Item *, QString> &ctx, QString &message) const;

protected:
  /** Holds the declaration of this item. */
  const Declaration *_declaration;
};



/** Base class of all scalar config items (bool, string, etc).
 * @ingroup conf */
class ScalarItem: public Item
{
  Q_OBJECT

public:
  /** Base class for all scalar value declarations. */
  class Declaration: public Item::Declaration
  {
  protected:
    /** Hidden constructor. */
    Declaration(bool mandatory, const QString &description="");

  public:
    bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const;
  };

protected:
  /** Hidden constructor.
   * @param declaration Specifies the declaration of the item.
   * @param parent Specifies the QObject parent. */
  ScalarItem(const Declaration *declaration, QObject *parent = nullptr);
};


/** Represents a boolean config item.
 * @ingroup conf */
class BoolItem: public ScalarItem
{
  Q_OBJECT

public:
  /** Represents a single boolean valued field.
   * @ingroup conf */
  class Declaration: public ScalarItem::Declaration
  {
  public:
    /** Hidden constructor. */
    Declaration(bool mandatory, const QString &description="");

    bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const;

    Item *parseAllocate(const YAML::Node &node, QString &msg) const;
    bool parsePopulate(Item *item, const YAML::Node &node, QHash<QString, Item*> &ctx, QString &msg) const;

    YAML::Node generate(const Item *item, const QHash<const Item *, QString> &ctx, QString &msg) const;
  };

public:
  /** Constructor.
   * @param value Specifies the value of the boolean item.
   * @param declaration Specifies the item declaraion.
   * @param parent Specifies the parent of the item. */
  BoolItem(bool value, const Declaration *declaration, QObject *parent=nullptr);

  /** Returns the actual value. */
  bool value() const;
  /** Sets the value. */
  void setValue(bool val);

  virtual QString dump(const QString &prefix="") const;

protected:
  /** Holds the value of the item. */
  bool _value;
};


/** Represents an integer config item.
 * @ingroup conf*/
class IntItem: public ScalarItem
{
  Q_OBJECT

public:
  /** Represents a single integer valued field. */
  class Declaration: public ScalarItem::Declaration
  {
  public:
    /** Constructs an integer field declaration.
     * @param min Specifies the minimum value of the field.
     * @param max Specifies the maximum allowed value of the field.
     * @param mandatory If @c true, the field is mandatory.
     * @param description Optional description of the field. */
    Declaration(qint64 min, qint64 max, bool mandatory,
                const QString &description="");

    /** Returns the lower bound. */
    qint64 mininum() const;
    /** Returns the upper bound. */
    qint64 maximum() const;

    bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const;

    Item *parseAllocate(const YAML::Node &node, QString &msg) const;
    bool parsePopulate(Item *item, const YAML::Node &node, QHash<QString, Item*> &ctx, QString &msg) const;

    YAML::Node generate(const Item *item, const QHash<const Item *, QString> &ctx, QString &msg) const;

  protected:
    /** Holds the lower bound. */
    qint64 _min;
    /** Holds the upper bound. */
    qint64 _max;
  };

public:
  /** Constructor.
   * @param value Specifies the value of the item.
   * @param declaration Specifies the item declaraion.
   * @param parent Specifies the parent of the item. */
  IntItem(qint64 value, const Declaration *declaration, QObject *parent=nullptr);

  /** Returns the actual value. */
  qint64 value() const;
  /** Sets the value and limits it to the declaration. */
  void setValue(qint64 value);

  virtual QString dump(const QString &prefix="") const;

protected:
  /** Holds the value of the item. */
  qint64 _value;
};


/** Represents a DMR ID config item.
 * @ingroup conf*/
class DMRId: public IntItem
{
  Q_OBJECT

public:
  /** Represents a DMR ID field, that is an integer in range [0, 16777215]. */
  class Declaration: public IntItem::Declaration
  {
  public:
    /** Constructs a DMR ID field. */
    Declaration(bool mandatory, const QString &description="");

    Item *parseAllocate(const YAML::Node &node, QString &msg) const;
    bool parsePopulate(Item *item, const YAML::Node &node, QHash<QString, Item*> &ctx, QString &msg) const;
  };

public:
  /** Constructor.
   * @param value Specifies the ID.
   * @param declaration Specifies the item declaraion.
   * @param parent Specifies the parent of the item. */
  DMRId(uint32_t value, const Declaration *declaration, QObject *parent=nullptr);

  virtual QString dump(const QString &prefix="") const;
};


/** Represents a floating point number config item.
 * @ingroup conf*/
class FloatItem: public ScalarItem
{
  Q_OBJECT

public:
  /** Represents a single floating point valued field. */
  class Declaration: public ScalarItem::Declaration
  {
  public:
    /** Constructs a floating point value field declaration.
     * @param min Specifies the minimum value of the field.
     * @param max Specifies the maximum allowed value of the field.
     * @param mandatory If @c true, the field is mandatory.
     * @param description Optional description of the field. */
    Declaration(double min, double max, bool mandatory,
                const QString &description="");

    /** Returns the lower bound. */
    double mininum() const;
    /** Returns the upper bound. */
    double maximum() const;

    bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const;

    Item *parseAllocate(const YAML::Node &node, QString &msg) const;
    bool parsePopulate(Item *item, const YAML::Node &node, QHash<QString, Item*> &ctx, QString &msg) const;

    YAML::Node generate(const Item *item, const QHash<const Item *, QString> &ctx, QString &msg) const;

  protected:
    /** Holds the lower bound. */
    double _min;
    /** Holds the upper bound. */
    double _max;
  };

public:
  /** Constructor.
   * @param value Specifies the value of the item.
   * @param declaration Specifies the item declaraion.
   * @param parent Specifies the parent of the item. */
  FloatItem(double value, const Declaration *declaration, QObject *parent=nullptr);

  /** Returns the value of the item. */
  double value() const;
  /** Sets the value and limits it to the declaration. */
  void setValue(double value);

  virtual QString dump(const QString &prefix="") const;

protected:
  /** Holds the value of the item. */
  double _value;
};


/** Represents a string config item.
 * @ingroup conf*/
class StringItem: public ScalarItem
{
  Q_OBJECT

public:
  /** Represents a string field. */
  class Declaration: public ScalarItem::Declaration
  {
  public:
    /** Constructs a string-valued field. */
    Declaration(bool mandatory, const QString &description="");

    bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const;

    Item *parseAllocate(const YAML::Node &node, QString &msg) const;
    bool parsePopulate(Item *item, const YAML::Node &node, QHash<QString, Item*> &ctx, QString &msg) const;

    YAML::Node generate(const Item *item, const QHash<const Item *, QString> &ctx, QString &msg) const;
  };

public:
  /** Constructor.
   * @param value Specifies the value of the item.
   * @param declaration Specifies the item declaraion.
   * @param parent Specifies the parent of the item. */
  StringItem(const QString &value, const Declaration *declaration, QObject *parent=nullptr);

  /** Returns the value of the item. */
  const QString &value() const;
  /** Sets the value. */
  void setValue(const QString &value);

  virtual QString dump(const QString &prefix="") const;

protected:
  /** Holds the value of the item. */
  QString _value;
};


/** Declares a field holding an identifier for cross-refrerence.
 * That is a string, that gets collected to check every reference. See @c CondifDeclRef.
 * @ingroup conf */
class IdDeclaration: public StringItem::Declaration
{
public:
  /** Constructor. */
  IdDeclaration(bool mandatory, const QString &description="");

  bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const;
};

/** Represents an enum value config item.
 * @ingroup conf*/
class Enum: public ScalarItem
{
  Q_OBJECT

public:
  /** Represents a single enumeration field. This is a string valued field that can only take
   * one of a pre-defined set of values. */
  class Declaration: public ScalarItem::Declaration
  {
  public:
    /** Constructor.
     * @param values Specifies the possible values of the enum.
     * @param mandatory If @c true, the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration(const QHash<QString, uint32_t> &values, bool mandatory,
                const QString &description="");

    /** Returns the set of possible values. */
    const QHash<QString, uint32_t> &possibleValues() const;
    /** Retruns the key for the given value. */
    QString key(uint32_t value) const;

    bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const;

    Item *parseAllocate(const YAML::Node &node, QString &msg) const;
    bool parsePopulate(Item *item, const YAML::Node &node, QHash<QString, Item*> &ctx, QString &msg) const;

    YAML::Node generate(const Item *item, const QHash<const Item *, QString> &ctx, QString &msg) const;

  protected:
    /** Holds the set of possible values. */
    QHash<QString, uint32_t> _values;
  };

public:
  /** Empty constructor.
   * @param declaration Specifies the item declaraion.
   * @param parent Specifies the parent of the item. */
  Enum(const Declaration *declaration, QObject *parent=nullptr);

  /** Constructor.
   * @param value Specifies the value of the item.
   * @param declaration Specifies the item declaraion.
   * @param parent Specifies the parent of the item. */
  Enum(uint32_t value, const Declaration *declaration, QObject *parent=nullptr);

  /** Returns the value of the item. */
  uint32_t value(uint32_t default_value=0) const;

  /** Maps to the given enum type. */
  template <class T> T as(T default_value) const { return T(value(default_value)); }

  /** Sets the key and verifies it with the declaration.
   * @returns @c true if the key is defined. */
  bool setKey(const QString &key);

  /** Sets the value.
   * @returns @c true if the key is defined. */
  bool setValue(uint32_t value);

  virtual QString dump(const QString &prefix="") const;

protected:
  /** Holds the enum value. */
  uint32_t _value;
};


/** Represents a mapping, that can only hold a single element.
 * This can be used to specify a predefined set of options.
 * @ingroup conf */
class DispatchDeclaration: public Item::Declaration
{
public:
  /** Declares a dispatch/cases.
   * @param elements The map of possible elements. Only one of these can be used.
   * @param mandatory If @c true, the element is mandatory.
   * @param description Specifies the optional element description. */
  DispatchDeclaration(const QHash<QString, Item::Declaration *> &elements, bool mandatory,
                     const QString &description="");

  /** Adds an element to the set. */
  bool add(const QString &name, Item::Declaration *element);

  bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const;
  bool verifyReferences(const YAML::Node &node, const QSet<QString> &ctx, QString &msg) const;

  Item *parseDefine(const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const;
  bool parseLink(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const;

  YAML::Node generate(const Item *item, const QHash<const Item *, QString> &ctx, QString &msg) const;

protected:
  Item *parseAllocate(const YAML::Node &node, QString &msg) const;
  bool parsePopulate(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const;

protected:
  /** The set of possible elements. */
  QHash<QString, Item::Declaration *> _elements;
};


/** Represents a reference to an arbitrary element.
 * @ingroup conf */
class Reference: public Item
{
  Q_OBJECT
public:
  /** Declares a field referencing a previously defined ID.
   * See @c ConfigIdDeclaration.*/
  class Declaration: public StringItem::Declaration
  {
  protected:
    /** Constructs a reference field declaration. */
    Declaration(bool mandatory, bool (*typeChk)(const Item *), const QString &description="");

  public:
    /** Returns @c true if the given item is of the required type. */
    bool isValidReference(const Item *item) const;

    bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const;
    bool verifyReferences(const YAML::Node &node, const QSet<QString> &ctx, QString &msg) const;
    bool parseLink(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const;

    YAML::Node generate(const Item *item, const QHash<const Item *, QString> &ctx, QString &msg) const;

  public:
    /** Constructs a reference declaration to the given intem class. */
    template <class RefType = Item>
    static Declaration *get(bool mandatory, const QString &description="") {
      return new Reference::Declaration(
            mandatory,
            [](const Item *item)->bool {
              return ((nullptr == item) || (nullptr != dynamic_cast<const RefType *>(item) ));
            }, description);
    }

  protected:
    Item *parseAllocate(const YAML::Node &node, QString &msg) const;
    bool parsePopulate(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const;

  protected:
    /** Holds the declaration of the referenced object. */
    bool (*_ref_type_check)(const Item *);
  };

public:
  /** Empty constructor. */
  Reference(const Declaration *declaraion, QObject *parent=nullptr);
  /** Constructs a reference to the given object. */
  Reference(Object *obj, const Declaration *declaraion, QObject *parent=nullptr);

  /** Returns @c nullptr if nothing is referenced. */
  bool isNull() const;
  /** Returns the referenced object or @c nullptr. */
  template <class ItemType=Item>
  ItemType *get() const {
    if (isNull())
      return nullptr;
    if (! _reference->is<ItemType>()) {
      logDebug() << "Cannot dereference item a " << typeid(ItemType).name()
                 << " referenced object is not of that type.";
      return nullptr;
    }
    return _reference->as<ItemType>();
  }

  /** Removes the reference. */
  Item *take();
  /** Sets the referenced object. */
  bool set(Item *item);

  virtual QString dump(const QString &prefix="") const;

protected slots:
  /** Gets called if the referenced object is deleted. */
  void onReferenceDeleted(QObject *obj);

protected:
  /** The reference to the element. */
  Item *_reference;
};


/** Represents a key-value map item.
 * @ingroup conf */
class Map: public Item
{
  Q_OBJECT

public:
  /** Represents a key-value mapping within the configuration. */
  class Declaration: public Item::Declaration
  {
  public:
    /** Constructs a map declaration.
     * @param elements The fields of the mapping.
     * @param mandatory If @c true, the mapping is mandatory.
     * @param description Specifies the optional object description. */
    Declaration(const QHash<QString, Item::Declaration *> &elements, bool mandatory,
                const QString &description="");

    /** Adds an element/field to the object. */
    bool add(const QString &name, Item::Declaration *element);
    /** Returns @c true if the declaration of the map contains the given element. */
    bool has(const QString &name) const;
    /** Returns the declaration given element. */
    Item::Declaration *get(const QString &name) const;

    bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const;
    bool verifyReferences(const YAML::Node &node, const QSet<QString> &ctx, QString &msg) const;
    bool parseLink(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const;

    YAML::Node generate(const Item *item, const QHash<const Item *, QString> &ctx, QString &msg) const;

  protected:
    Item *parseAllocate(const YAML::Node &node, QString &msg) const;
    bool parsePopulate(Item *item, const YAML::Node &node, QHash<QString, Item*> &ctx, QString &msg) const;

  protected:
    /** The set of mandatory elements. */
    QSet<QString> _mandatoryElements;
    /** The set of all elements. */
    QHash<QString, Item::Declaration *> _elements;
  };

public:
  /** Empty constructor.
   * @param declaration Specifies the item declaraion.
   * @param parent Specifies the parent of the item. */
  Map(const Declaration *declaration, QObject *parent=nullptr);

  /** Returns @c true if the object contains the given element. */
  bool has(const QString &name) const;
  /** Adds the given element to the object */
  bool add(const QString &name, Item *element);

  /** Adds or replaces the given element in the object */
  bool set(const QString &name, Item *element);

  /** Returns the given element by name. */
  template <class ItemType=Item>
  ItemType *get(const QString &name) const {
    if (! has(name))
      return nullptr;
    Item *item = _elements[name];
    if (! item->is<ItemType>()) {
      logDebug() << "Cannot get element " << name
                 << " as " << typeid(ItemType).name() << ".";
      return nullptr;
    }
    return item->as<ItemType>();
  }

  /** Dereferences the given reference by name. */
  template <class ItemType=Item>
  ItemType *getRef(const QString &name) const {
    if (! has(name))
      return nullptr;
    Item *item = _elements[name];
    if (! item->is<Reference>()) {
      logDebug() << "Cannot get reference " << name
                 << " element is not a reference to some other object .";
      return nullptr;
    }
    Reference *ref = item->as<Reference>();
    if (ref->isNull())
      return nullptr;
    return ref->get<ItemType>();
  }

  /** Removes the given element from the object and returns it. */
  Item *take(const QString &name);
  /** Deletes the element from the object. */
  bool del(const QString &name);

  virtual bool generateIds(QHash<const Item *, QString> &ctx, QString &message) const;

  virtual QString dump(const QString &prefix="") const;

protected slots:
  /** Gets called if one of the elements of this object gets deleted. */
  void onElementDeleted(QObject *element);

protected:
  /** Holds the elements of the object. */
  QHash<QString, Item *>_elements;
};


/** Represents an object config item.
 * @ingroup conf */
class Object: public Map
{
  Q_OBJECT

public:
  /** Represents an object (a key-value mapping with an ID) within the configuration. */
  class Declaration: public Map::Declaration
  {
  public:
    /** Constructs a map declaration.
     * @param mandatory If @c true, the object is mandatory.
     * @param description Specifies the optional object description. */
    Declaration(bool mandatory, const QString &description="");

    Item *parseDefine(const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const;
    bool parseLink(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const;

    YAML::Node generate(const Item *item, const QHash<const Item *, QString> &ctx, QString &msg) const;

  protected:
    Item *parseAllocate(const YAML::Node &node, QString &msg) const;
    bool parsePopulate(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const;
  };

public:
  /** Empty constructor.
   * @param declaration Specifies the item declaraion.
   * @param parent Specifies the parent of the item. */
  Object(const Declaration *declaration, QObject *parent=nullptr);

  virtual bool generateIds(QHash<const Item *, QString> &ctx, QString &message) const;

  virtual QString dump(const QString &prefix="") const;

protected:
  /** Holds the object ID template. */
  QString _idTemplate;
};


/** Represents a list of configuration items.
 * @ingroup conf */
class AbstractList: public Item
{
  Q_OBJECT

public:
  /** Represents an abstract list within the configuration.
   * The list is a list of equal types. */
  class Declaration: public Item::Declaration
  {
  protected:
    /** Declares a list.
     * The list inherits the mandatory flag from the content.
     *
     * @param element Content declaration.
     * @param typeChk Specifies the type-check function for the list.
     * @param description The optional description of the list. */
    Declaration(Item::Declaration *element, bool (*typeChk)(const Item *), const QString &description="");

  public:
    /** Returns @c true if the type of the given item is accepted by the list. */
    bool isValidType(const Item *item) const;
    /** Returns the declaration of the elements of this list. */
    Item::Declaration *elementDeclaration() const;

  public:
    bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const;
    bool verifyReferences(const YAML::Node &node, const QSet<QString> &ctx, QString &msg) const;

    bool parseLink(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const;

    YAML::Node generate(const Item *item, const QHash<const Item *, QString> &ctx, QString &msg) const;

  protected:
    bool parsePopulate(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const;

  protected:
    /** Holds the content element declaration. */
    Item::Declaration *_element;
    /** Holds the type-check function. */
    bool (*_element_type_check)(const Item *);
  };

protected:
  /** Empty constructor.
   * @param declaration Specifies the item declaraion.
   * @param parent Specifies the parent of the item. */
  AbstractList(const Declaration *declaration, QObject *parent=nullptr);

public:
  /** Destructor. */
  virtual ~AbstractList();

  /** Returns the number of items in the list. */
  int count() const;
  /** Adds an item to the list. */
  virtual bool add(Item *item);
  /** Returns the i-th item. */
  virtual Item *get(int i) const;
  /** Removes the i-th item from the list and returns it. */
  virtual Item *take(int i);
  /** Deletes the i-th item from the list. */
  virtual bool del(int i);

  virtual bool generateIds(QHash<const Item *, QString> &ctx, QString &message) const;

  virtual QString dump(const QString &prefix="") const;

protected slots:
  /** Gets called if a list item is deleted. */
  void onItemDeleted(QObject *item);

protected:
  /** Holds the list items. */
  QVector<Item *>_items;
};


/** Represents a list of elements owned by the list.
 * @ingroup conf */
class List: public AbstractList
{
  Q_OBJECT

public:
  /** Represents a list of objects within the configuration.
   * The list is a list of equal object types. */
  class Declaration: public AbstractList::Declaration
  {
  protected:
    /** Declares a list.
     * The list inherits the mandatory flag from the content.
     *
     * @param element Content declaration.
     * @param typeChk Specifies the type-check function.
     * @param description The optional description of the list. */
    Declaration(Item::Declaration *element, bool (*typeChk)(const Item *), const QString &description="");

  protected:
    Item *parseAllocate(const YAML::Node &node, QString &msg) const;

  public:
    /** Creates a list declaration for the specified object type.
     * The declaration of this object type must specifiy a static factory function @c get. */
    template<class ObjType=Object>
    static Declaration *get(const QString &description="") {
      return new Declaration(
            ObjType::Declaration::get(),
            [](const Item *item)->bool { return (nullptr != dynamic_cast<const ObjType *>(item)); },
            description);
    }

    /** Creates a list declaration for the specified elment type.
     * The declaration of this object type may not specifiy a static factory function @c get.
     * This method uses the explicitly given declaration. */
    template<class ElmType=Object>
    static Declaration *get(Item::Declaration *declaration, const QString &description="") {
      return new Declaration(
            declaration,
            [](const Item *item)->bool { return (nullptr != dynamic_cast<const ElmType *>(item)); },
            description);
    }
  };

public:
  /** Empty constructor.
   * @param declaration Specifies the item declaraion.
   * @param parent Specifies the parent of the item. */
  List(const Declaration *declaration, QObject *parent=nullptr);

  /** Adds an object to the list. */
  bool add(Item *item);
  /** Removes the i-th item from the list and returns it. */
  Item *take(int i);
  /** Deletes the i-th item from the list. */
  bool del(int i);
};


/** Represents a list of references to objects.
 * @ingroup conf */
class RefList: public AbstractList
{
  Q_OBJECT

public:
  /** Represents a list of references within the configuration.
   * The list is a list of equal object types. */
  class Declaration: public AbstractList::Declaration
  {
  protected:
    /** Declares a list.
     * The list inherits the mandatory flag from the content.
     *
     * @param element Content declaration.
     * @param typeChk Specifies the type-check function.
     * @param description The optional description of the list. */
    Declaration(Item::Declaration *element, bool (*typeChk)(const Item *), const QString &description="");

  public:
    bool parseLink(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const;

  protected:
    Item *parseAllocate(const YAML::Node &node, QString &msg) const;
    bool parsePopulate(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const;

  public:
    /** Constructs a declaration for a list of references to the specified object type.
     * The object declaration must provide a static @c get function. */
    template<class ObjType=Object>
    static Declaration *get(bool mandatory, const QString &description="") {
      return new Declaration(
            Reference::Declaration::get<ObjType>(mandatory),
            [](const Item *item)->bool { return (nullptr != dynamic_cast<const ObjType *>(item)); },
            description);
    }
  };

public:
  /** Empty constructor.
   * @param declaration Specifies the item declaraion.
   * @param parent Specifies the parent of the item. */
  RefList(const Declaration *declaration, QObject *parent=nullptr);

  virtual QString dump(const QString &prefix="") const;
};


/** Represents a radio ID and name definition. *
 * @ingroup conf */
class RadioId: public Object
{
  Q_OBJECT

public:
  /** Declares a radio ID. */
  class Declaration: public Object::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration(bool mandatory, const QString &description="");

  protected:
    Item *parseAllocate(const YAML::Node &node, QString &msg) const;

  public:
    /** Returns/creates the singleton instance. */
    static Declaration *get();

  private:
    /** The singleton instance. */
    static Declaration *_instance;
  };

public:
  /** Empty constructor. */
  explicit RadioId(QObject *parent=nullptr);

public:
  /** Returns the name for this radio id. */
  const QString &name() const;
  /** Returns the radio id. */
  uint32_t number() const;
};


/** Base class for all contact definitions.
 * @ingroup conf */
class Contact: public Object
{
Q_OBJECT

public:
  /** Base class of all contact declarations. */
  class Declaration: public Object::Declaration
  {
  protected:
    /** Hidden constructor. */
    Declaration(bool mandatory, const QString &description="");
  };

protected:
  /** Hidden constructor. */
  Contact(Declaration *declaraion, QObject *parent=nullptr);

public:
  /** Returns the name of the contact. */
  const QString &name() const;
  /** Returns @c true if the contact rings. */
  bool ring() const;
};


/** Base class for all digitial (ie, DMR) contacts.
 * @ingroup conf */
class DigitalContact: public Contact
{
Q_OBJECT

public:
  /** Base class of all digital contact declarations. */
  class Declaration: public Contact::Declaration
  {
  protected:
    /** Hidden constructor. */
    Declaration(bool mandatory, const QString &description="");
  };

protected:
  /** Hidden constructor. */
  DigitalContact(Declaration *declaraion, QObject *parent=nullptr);
};


/** Represents a private call conact.
 * @ingroup conf */
class PrivateCall: public DigitalContact
{
Q_OBJECT

public:
  /** Declares a private call contact. */
  class Declaration: public DigitalContact::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration(bool mandatory, const QString &description="");

    Item *parseAllocate(const YAML::Node &node, QString &msg) const;

  public:
    /** Returns/creates the singleton instance. */
    static Declaration *get();

  private:
    /** The singleton instance. */
    static Declaration *_instance;
  };

public:
  /** Empty constructor. */
  explicit PrivateCall(QObject *parent=nullptr);

  /** Returns the number of the contact. */
  uint32_t number() const;
};


/** Represents a group call contact.
 * @ingroup conf */
class GroupCall: public DigitalContact
{
Q_OBJECT

public:
  /** Declares a group call contact. */
  class Declaration: public DigitalContact::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration(bool mandatory, const QString &description="");

    Item *parseAllocate(const YAML::Node &node, QString &msg) const;

  public:
    /** Returns/creates the singleton instance. */
    static Declaration *get();

  private:
    /** The singleton instance. */
    static Declaration *_instance;
  };

public:
  /** Empty constructor. */
  explicit GroupCall(QObject *parent=nullptr);

  /** Returns the number of the contact. */
  uint32_t number() const;
};


/** Represets an all-call contact.
 * @ingroup conf */
class AllCall: public DigitalContact
{
Q_OBJECT

public:
  /** Declares a all call contact. */
  class Declaration: public DigitalContact::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration(bool mandatory, const QString &description="");

    Item *parseAllocate(const YAML::Node &node, QString &msg) const;

  public:
    /** Returns/creates the singleton instance. */
    static Declaration *get();

  private:
    /** The singleton instance. */
    static Declaration *_instance;
  };

public:
  /** Empty constructor. */
  explicit AllCall(QObject *parent=nullptr);
};


/** Represents an RX group list.
 * @ingroup conf */
class GroupList: public Object
{
  Q_OBJECT

public:
  /** Declares a all call contact. */
  class Declaration: public Object::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
     Declaration(bool mandatory, const QString &description="");

     Item *parseAllocate(const YAML::Node &node, QString &msg) const;

  public:
     /** Returns/creates the singleton instance. */
     static Declaration *get();

  private:
     /** The singleton instance. */
     static Declaration *_instance;
  };

public:
  /** Empty constructor. */
  explicit GroupList(QObject *parent=nullptr);

  /** Returns the name of the group list. */
  const QString &name() const;
  /** Returns the list of members. */
  AbstractList *members() const;
};


/** Base class of all channels.
 * @ingroup conf */
class Channel: public Object
{
  Q_OBJECT

public:
  /** Base class for all channel declarations. */
  class Declaration: public Object::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration(bool mandatory, const QString &description="");
  };

protected:
  /** Hidden constructor. */
  Channel(Declaration *declaraion, QObject *parent=nullptr);

public:
  /** Returns the name of the channel. */
  const QString &name() const;
  /** Returns the RX frequency in Mhz. */
  double rx() const;
  /** Returns the TX frequency in Mhz. */
  double tx() const;
  /** Returns the TX power. */
  ::Channel::Power power() const;
  /** Returns the transmit timeout for this channel. */
  qint64 txTimeout() const;
  /** Returns @c true if the channel is RX only. */
  bool rxOnly() const;
  /** Returns a reference to the scan-list or @c nullptr if none is set. */
  ScanList *scanlist() const;
};


/** Represents a digital channel (ie, DMR channel).
 * @ingroup conf */
class DigitalChannel: public Channel
{
  Q_OBJECT

public:
  /** Declares a digital channel. */
  class Declaration: public Channel::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration(bool mandatory, const QString &description="");

    Item *parseAllocate(const YAML::Node &node, QString &msg) const;

  public:
    /** Returns/creates the singleton instance. */
    static Declaration *get();

  private:
    /** The singleton instance. */
    static Declaration *_instance;
  };

public:
  /** Empty constructor. */
  explicit DigitalChannel(QObject *parent=nullptr);

  /** Returns the admit criterion for this channel. */
  ::DigitalChannel::Admit admit() const;
  /** Returns the color-code of the channel. */
  uint8_t colorCode() const;
  /** Returns the time-slot of the channel. */
  ::DigitalChannel::TimeSlot timeSlot() const;
  /** Returns a reference to the RX group list. */
  GroupList *groupList() const;
  /** Returns a reference to the default TX contact or @c nullptr if none is set. */
  DigitalContact *txContact() const;
  /** Returns a reference to the positioning system or @c nullptr if none is set. */
  Positioning *aprs() const;
  /** Returns a reference to the roaming zone or @c nullptr if none is set. */
  RoamingZone *roamingZone() const;
  /** Returns a reference to the radio used by the channel or @c nullptr if the default should be used. */
  RadioId *radioId() const;
};


/** Represents an analog (ie, FM) channel.
 * @ingroup conf */
class AnalogChannel: public Channel
{
  Q_OBJECT

public:
  /** Declares an analog channel. */
  class Declaration: public Channel::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration(bool mandatory, const QString &description="");

    Item *parseAllocate(const YAML::Node &node, QString &msg) const;

  public:
    /** Returns/creates the singleton instance. */
    static Declaration *get();

  private:
    /** The singleton instance. */
    static Declaration *_instance;
  };

public:
  /** Empty constructor. */
  explicit AnalogChannel(QObject *parent=nullptr);

  /** Returns the admit criterion. */
  ::AnalogChannel::Admit admit() const;
  /** Returns the squelch value in [1,10]. */
  uint8_t squelch() const;
  /** Retunrs the RX signalling. */
  Signaling::Code rxSignalling() const;
  /** Retunrs the TX signalling. */
  Signaling::Code txSignalling() const;
  /** Retunrs the band-width. */
  ::AnalogChannel::Bandwidth bandWidth() const;
  /** Retunrs the APRS positioning system or @c nullptr if none is set. */
  APRSPositioning *aprs() const;
};


/** Represents a zone.
 * @ingroup conf */
class Zone: public Object
{
  Q_OBJECT

public:
  /** Represents the zone declaration. */
  class Declaration: public Object::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration(bool mandatory, const QString &description="");

    Item *parseAllocate(const YAML::Node &node, QString &msg) const;

  public:
    /** Returns/creates the singleton instance. */
    static Declaration *get();

  private:
    /** The singleton instance. */
    static Declaration *_instance;
  };

public:
  /** Empty constructor. */
  explicit Zone(QObject *parent=nullptr);

  /** Returns the name of the zone. */
  const QString &name() const;
  /** Returns the channel list for VFO A. */
  AbstractList *a() const;
  /** Returns the channel list for VFO B. */
  AbstractList *b() const;
};


/** Represents a scan-list.
 * @ingroup conf */
class ScanList: public Object
{
  Q_OBJECT

public:
  /** Represents the scan-list declaration. */
  class Declaration: public Object::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration(bool mandatory, const QString &description="");

    Item *parseAllocate(const YAML::Node &node, QString &msg) const;

  public:
    /** Returns/creates the singleton instance. */
    static Declaration *get();

  private:
    /** The singleton instance. */
    static Declaration *_instance;
  };


public:
  /** Empty constructor. */
  explicit ScanList(QObject *parent=nullptr);

  /** Returns the name of the scan-list. */
  const QString &name() const;
  /** Returns a reference to the primary channel or @c nullptr if none is set. */
  Channel *primary() const;
  /** Returns a reference to the secondary channel or @c nullptr if none is set. */
  Channel *secondary() const;
  /** Returns a reference to the revert/transmit channel or @c nullptr if none is set. */
  Channel *revert() const;
  /** Returns the channel list */
  RefList *channels() const;
};


/** Base class of all positioning systems.
 * @ingroup conf */
class Positioning: public Object
{
  Q_OBJECT

public:
  /** Base class for all positioning declarations. */
  class Declaration: public Object::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration(bool mandatory, const QString &description="");
  };

protected:
  /** Hidden constructor. */
  Positioning(Declaration *declaraion, QObject *parent=nullptr);

public:
  /** Returns the name of the positioning system. */
  const QString &name() const;
  /** Returns the update period in seconds. */
  uint32_t period() const;
};


/** Represents an DMR positioning system.
 * @ingroup conf */
class DMRPositioning: public Positioning
{
  Q_OBJECT

public:
  /** Declares a DMR positioning system. */
  class Declaration: public Positioning::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration(bool mandatory, const QString &description="");

    Item *parseAllocate(const YAML::Node &node, QString &msg) const;

  public:
    /** Returns/creates the singleton instance. */
    static Declaration *get();

  private:
    /** The singleton instance. */
    static Declaration *_instance;
  };

public:
  /** Empty constructor. */
  explicit DMRPositioning(QObject *parent=nullptr);
  /** Returns the destination contact the position is send to. */
  DigitalContact *destination() const;
  /** Returns the transmit channel or @c nullptr if none is set. */
  DigitalChannel *channel() const;
};


/** Represents an APRS positioning system.
 * @ingroup conf */
class APRSPositioning: public Positioning
{
  Q_OBJECT

public:
  /** Declares an APRS positioning system. */
  class Declaration: public Positioning::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration(bool mandatory, const QString &description="");

    Item *parseAllocate(const YAML::Node &node, QString &msg) const;

  public:
    /** Returns/creates the singleton instance. */
    static Declaration *get();

  private:
    /** The singleton instance. */
    static Declaration *_instance;
  };

public:
  /** Empty constructor. */
  APRSPositioning(QObject *parent=nullptr);

  /** Returns the source call and SSID. */
  const QString &source() const;
  /** Returns the destination call and SSID. */
  const QString &destination() const;
  /** Returns the transmit channel. */
  AnalogChannel *channel() const;
  /** Returns the list of path calls and SSIDs. */
  AbstractList *path() const;
  /** Returns the map icon. */
  const QString &icon() const;
  /** Returns the APRS message. */
  const QString &message() const;
};


/** Represents a roaming zone
 * @ingroup conf */
class RoamingZone: public Object
{
  Q_OBJECT

public:
  /** Declares a roaming zone. */
  class Declaration: public Object::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration(bool mandatory, const QString &description="");

    Item *parseAllocate(const YAML::Node &node, QString &msg) const;

  public:
    /** Returns/creates the singleton instance. */
    static Declaration *get();

  private:
    /** The singleton instance. */
    static Declaration *_instance;
  };

public:
  /** Empty constructor. */
  RoamingZone(QObject *parent=nullptr);

public:
  /** Returns the name of the roaming zone. */
  const QString &name() const;
  /** Returns the list of channels. */
  AbstractList *channels() const;
};


/** Holds the entire abstract codeplug configuration.
 * @ingroup conf */
class Config: public Map
{
  Q_OBJECT

public:
  /** This class represents the complete configuration declaration.
   * That is the codeplug YAML format declaration. */
  class Declaration: public Map::Declaration
  {
  protected:
    /** Constructor. */
    explicit Declaration();

    Item *parseAllocate(const YAML::Node &node, QString &msg) const;

  public:
    /** Verifies the parsed codeplug.
     * @returns @c true on success and @c false otherwise. On error, the @c message argument
     * will contain the error message. */
    virtual bool verify(const YAML::Node &doc, QString &message);

    bool parse(Config *config, YAML::Node &doc, QString &message);
    Config *parse(YAML::Node &doc, QString &message);

    /** Returns/creates the singleton instance. */
    static Config::Declaration *get();

  private:
    /** The singleton instance. */
    static Declaration *_instance;
  };

public:
  /** Empty constructor. */
  explicit Config(QObject *parent = nullptr);

  YAML::Node generate(QString &message) const;
};


}

#endif // CONFIGITEM_HH
