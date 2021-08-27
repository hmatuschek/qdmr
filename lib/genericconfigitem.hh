#ifndef GENERIC_CONFIG_ITEM_HH
#define GENERIC_CONFIG_ITEM_HH

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
     * @param description Gives some short description of the item. */
    explicit Declaration(const QString &description="");

  public:
    /** Destructor. */
    virtual ~Declaration();

    /** Returns the description of the config item. */
    const QString &description() const;
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

signals:
  /** Gets emitted whenever this element gets modified. */
  void modified(Item *item);

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
    Declaration(const QString &description="");

  public:
    bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const override;
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
  protected:
    /** Hidden constructor. */
    Declaration(const QString &description="");

  public:
    bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const override;

    Item *parseAllocate(const YAML::Node &node, QString &msg) const override;
    bool parsePopulate(Item *item, const YAML::Node &node, QHash<QString, Item*> &ctx, QString &msg) const override;

    YAML::Node generate(const Item *item, const QHash<const Item *, QString> &ctx, QString &msg) const override;

  public:
    /** Factory method for the declaration instance. */
    static Declaration *get();

  private:
    /** The singleton instance. */
    static Declaration *_instance;
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

  QString dump(const QString &prefix="") const override;

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
     * @param description Optional description of the field. */
    Declaration(qint64 min, qint64 max, const QString &description="");

    /** Returns the lower bound. */
    qint64 mininum() const;
    /** Returns the upper bound. */
    qint64 maximum() const;

    bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const override;

    Item *parseAllocate(const YAML::Node &node, QString &msg) const override;
    bool parsePopulate(Item *item, const YAML::Node &node, QHash<QString, Item*> &ctx, QString &msg) const override;

    YAML::Node generate(const Item *item, const QHash<const Item *, QString> &ctx, QString &msg) const override;

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
  virtual qint64 value() const;
  /** Sets the value and limits it to the declaration. */
  virtual void setValue(qint64 value);

  QString dump(const QString &prefix="") const override;

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
  protected:
    /** Constructs a DMR ID field. */
    Declaration();

  public:
    Item *parseAllocate(const YAML::Node &node, QString &msg) const override;
    bool parsePopulate(Item *item, const YAML::Node &node, QHash<QString, Item*> &ctx, QString &msg) const override;

  public:
    /** Factory method for the singeton instance. */
    static Declaration *get();

  private:
    static Declaration *_instance;
  };

public:
  /** Constructor.
   * @param value Specifies the ID.
   * @param declaration Specifies the item declaraion.
   * @param parent Specifies the parent of the item. */
  DMRId(uint32_t value, const Declaration *declaration, QObject *parent=nullptr);

  QString dump(const QString &prefix="") const override;
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
     * @param description Optional description of the field. */
    Declaration(double min, double max, const QString &description="");

    /** Returns the lower bound. */
    double mininum() const;
    /** Returns the upper bound. */
    double maximum() const;

    bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const override;

    Item *parseAllocate(const YAML::Node &node, QString &msg) const override;
    bool parsePopulate(Item *item, const YAML::Node &node, QHash<QString, Item*> &ctx, QString &msg) const override;

    YAML::Node generate(const Item *item, const QHash<const Item *, QString> &ctx, QString &msg) const override;

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
  virtual double value() const;
  /** Sets the value and limits it to the declaration. */
  virtual void setValue(double value);

  virtual QString dump(const QString &prefix="") const override;

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
  protected:
    /** Hidden constructor for a string-valued field. */
    Declaration(const QString &description="");

  public:
    bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const override;

    Item *parseAllocate(const YAML::Node &node, QString &msg) const override;
    bool parsePopulate(Item *item, const YAML::Node &node, QHash<QString, Item*> &ctx, QString &msg) const override;

    YAML::Node generate(const Item *item, const QHash<const Item *, QString> &ctx, QString &msg) const override;

  public:
    /** Factory method for the declaration. */
    static Declaration *get();

  private:
    /** Singleton instance. */
    static Declaration *_instance;
  };

public:
  /** Constructor.
   * @param value Specifies the value of the item.
   * @param declaration Specifies the item declaraion.
   * @param parent Specifies the parent of the item. */
  StringItem(const QString &value, const Declaration *declaration, QObject *parent=nullptr);

  /** Returns the value of the item. */
  virtual const QString &value() const;
  /** Sets the value. */
  virtual void setValue(const QString &value);

  QString dump(const QString &prefix="") const override;

protected:
  /** Holds the value of the item. */
  QString _value;
};


/** Declares a field holding an identifier for cross-refrerence.
 * That is a string, that gets collected to check every reference. See @c CondifDeclRef.
 * @ingroup conf */
class IdDeclaration: public StringItem::Declaration
{
protected:
  /** Hidden constructor. */
  IdDeclaration(const QString &description="");

public:
  bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const override;

public:
  /** Factory method. */
  static IdDeclaration *get();

private:
  /** Singleton instance. */
  static IdDeclaration *_instance;
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
     * @param description Specifies the optional field description. */
    Declaration(const QHash<QString, uint32_t> &values, const QString &description="");

    /** Returns the set of possible values. */
    const QHash<QString, uint32_t> &possibleValues() const;
    /** Retruns the key for the given value. */
    QString key(uint32_t value) const;

    bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const override;

    Item *parseAllocate(const YAML::Node &node, QString &msg) const override;
    bool parsePopulate(Item *item, const YAML::Node &node, QHash<QString, Item*> &ctx, QString &msg) const override;

    YAML::Node generate(const Item *item, const QHash<const Item *, QString> &ctx, QString &msg) const override;

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
  virtual uint32_t value(uint32_t default_value=0) const;

  /** Maps to the given enum type. */
  template <class T> T as(T default_value) const { return T(value(default_value)); }

  /** Sets the key and verifies it with the declaration.
   * @returns @c true if the key is defined. */
  virtual bool setKey(const QString &key);

  /** Sets the value.
   * @returns @c true if the key is defined. */
  virtual bool setValue(uint32_t value);

  QString dump(const QString &prefix="") const override;

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
   * @param description Specifies the optional element description. */
  DispatchDeclaration(const QHash<QString, Item::Declaration *> &elements,
                     const QString &description="");

  /** Adds an element to the set. */
  bool add(const QString &name, Item::Declaration *element);

  bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const override;
  bool verifyReferences(const YAML::Node &node, const QSet<QString> &ctx, QString &msg) const override;

  Item *parseDefine(const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const override;
  bool parseLink(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const override;

  YAML::Node generate(const Item *item, const QHash<const Item *, QString> &ctx, QString &msg) const override;

protected:
  Item *parseAllocate(const YAML::Node &node, QString &msg) const override;
  bool parsePopulate(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const override;

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
    Declaration(bool (*typeChk)(const Item *), const QString &description="");

  public:
    /** Returns @c true if the given item is of the required type. */
    bool isValidReference(const Item *item) const;

    bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const override;
    bool verifyReferences(const YAML::Node &node, const QSet<QString> &ctx, QString &msg) const override;
    bool parseLink(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const override;

    YAML::Node generate(const Item *item, const QHash<const Item *, QString> &ctx, QString &msg) const override;

  public:
    /** Constructs a reference declaration to the given intem class. */
    template <class RefType = Item>
    static Declaration *get() {
      return new Reference::Declaration(
            [](const Item *item)->bool {
              return ((nullptr == item) || (nullptr != dynamic_cast<const RefType *>(item) ));
            });
    }

  protected:
    Item *parseAllocate(const YAML::Node &node, QString &msg) const override;
    bool parsePopulate(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const override;

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

  QString dump(const QString &prefix="") const override;

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
     * @param description Specifies the optional object description. */
    explicit Declaration(const QString &description="");

    /** Adds an element/field to the object. */
    bool add(const QString &name, Item::Declaration *element, bool mandatory, const QString &description);
    /** Returns @c true if the declaration of the map contains the given element. */
    bool has(const QString &name) const;
    /** Returns the declaration given element. */
    Item::Declaration *get(const QString &name) const;

    bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const override;
    bool verifyReferences(const YAML::Node &node, const QSet<QString> &ctx, QString &msg) const override;
    bool parseLink(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const override;

    YAML::Node generate(const Item *item, const QHash<const Item *, QString> &ctx, QString &msg) const override;

  protected:
    Item *parseAllocate(const YAML::Node &node, QString &msg) const override;
    bool parsePopulate(Item *item, const YAML::Node &node, QHash<QString, Item*> &ctx, QString &msg) const override;

  protected:
    /** The set of mandatory elements. */
    QSet<QString> _mandatoryElements;
    /** The set of all elements. */
    QHash<QString, Item::Declaration *> _elements;
    /** The description for each element. */
    QHash<QString, QString> _descriptions;
    /** The order in which the elements are defined. */
    QList<QString> _order;
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

  virtual bool generateIds(QHash<const Item *, QString> &ctx, QString &message) const override;

  virtual QString dump(const QString &prefix="") const override;

signals:
  /** Gets emitted, once any element gets modified. */
  void elementModified(QString key);
  /** Gets emitted, once any element get deleted. */
  void elementDeleted(QString key);

protected slots:
  /** Gets called if one of the elements of this object gets deleted. */
  void onElementDeleted(QObject *element);
  /** Gets called, whenever an element gets modified. */
  void onElementModified(Item *item);

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
     * @param description Specifies the optional object description. */
    explicit Declaration(const QString &description="");

    Item *parseDefine(const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const override;
    bool parseLink(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const override;

    YAML::Node generate(const Item *item, const QHash<const Item *, QString> &ctx, QString &msg) const override;

  protected:
    Item *parseAllocate(const YAML::Node &node, QString &msg) const override;
    bool parsePopulate(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const override;
  };

public:
  /** Empty constructor.
   * @param declaration Specifies the item declaraion.
   * @param parent Specifies the parent of the item. */
  Object(const Declaration *declaration, QObject *parent=nullptr);

  virtual bool generateIds(QHash<const Item *, QString> &ctx, QString &message) const override;

  QString dump(const QString &prefix="") const override;

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
    bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const override;
    bool verifyReferences(const YAML::Node &node, const QSet<QString> &ctx, QString &msg) const override;

    bool parseLink(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const override;

    YAML::Node generate(const Item *item, const QHash<const Item *, QString> &ctx, QString &msg) const override;

  protected:
    bool parsePopulate(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const override;

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

  /** Moves the channel at index @c idx one step up. */
  virtual bool moveUp(int idx);
  /** Moves the channels at one step up. */
  virtual bool moveUp(int first, int last);
  /** Moves the channel at index @c idx one step down. */
  virtual bool moveDown(int idx);
  /** Moves the channels one step down. */
  virtual bool moveDown(int first, int last);

  bool generateIds(QHash<const Item *, QString> &ctx, QString &message) const override;

  QString dump(const QString &prefix="") const override;

signals:
  /** Gets emitted, once any element gets modified. */
  void elementModified(int index);
  /** Gets emitted, once any element get deleted. */
  void elementDeleted(int index);

protected slots:
  /** Gets called if a list item is deleted. */
  void onElementDeleted(QObject *item);
  /** Gets called, whenever an element gets modified. */
  void onElementModified(Item *item);


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
    Item *parseAllocate(const YAML::Node &node, QString &msg) const override;

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
  bool add(Item *item) override;
  /** Removes the i-th item from the list and returns it. */
  Item *take(int i) override;
  /** Deletes the i-th item from the list. */
  bool del(int i) override;
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
    bool parseLink(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const override;

  protected:
    Item *parseAllocate(const YAML::Node &node, QString &msg) const override;
    bool parsePopulate(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const override;

  public:
    /** Constructs a declaration for a list of references to the specified object type.
     * The object declaration must provide a static @c get function. */
    template<class ObjType=Object>
    static Declaration *get() {
      return new Declaration(
            Reference::Declaration::get<ObjType>(),
            [](const Item *item)->bool { return (nullptr != dynamic_cast<const ObjType *>(item)); });
    }
  };

public:
  /** Empty constructor.
   * @param declaration Specifies the item declaraion.
   * @param parent Specifies the parent of the item. */
  RefList(const Declaration *declaration, QObject *parent=nullptr);

  QString dump(const QString &prefix="") const override;
};


}

#endif // GENERIC_CONFIG_ITEM_HH
