#ifndef CONFIGITEM_HH
#define CONFIGITEM_HH

#include <QObject>
#include <QVector>
#include <QSet>
#include <QHash>
#include <yaml-cpp/yaml.h>

class ConfigObjItem;
class ConfigScanList;
class ConfigGroupList;
class ConfigPositioning;
class ConfigRoamingZone;
class ConfigRadioId;
class ConfigAPRSPositioning;


/** Base class of all YAML extensible codeplug/config items.
 * These objects form the CST of the parsed YAML codeplug.
 * @ingroup conf */
class ConfigItem : public QObject
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
    virtual ConfigItem *parseDefine(YAML::Node &node, QHash<QString, ConfigItem*> &ctx, QString &msg) const;
    /** Implements the link phase of the two-step parsing. */
    virtual bool parseLink(ConfigItem *item, YAML::Node &node, QHash<QString, ConfigItem*> &ctx, QString &msg) const;

  protected:
    /** Allocates the parsed item. */
    virtual ConfigItem *parseAllocate(YAML::Node &node, QString &msg) const = 0;
    /** Populates the parsed item. */
    virtual bool parsePopulate(ConfigItem *item, YAML::Node &node, QHash<QString, ConfigItem*> &ctx, QString &msg) const = 0;

  protected:
    /** Holds the description of the item. */
    QString _description;
    /** If @c true, the item is mandatory for a complete config. */
    bool _mandatory;
  };

protected:
  /** Hidden constructor.
   * @param declaration Specifies the declaration of the item.
   * @param parent Specifies the QObject parent. */
  ConfigItem(const Declaration *declaration, QObject *parent = nullptr);

public:
  /** Destructor. */
  virtual ~ConfigItem();

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

protected:
  /** Holds the declaration of this item. */
  const Declaration *_declaration;
};



/** Base class of all scalar config items (bool, string, etc).
 * @ingroup conf */
class ConfigScalarItem: public ConfigItem
{
  Q_OBJECT

public:
  /** Base class for all scalar value declarations. */
  class Declaration: public ConfigItem::Declaration
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
  ConfigScalarItem(const Declaration *declaration, QObject *parent = nullptr);
};


/** Represents a boolean config item.
 * @ingroup conf */
class ConfigBoolItem: public ConfigScalarItem
{
  Q_OBJECT

public:
  /** Represents a single boolean valued field.
   * @ingroup conf */
  class Declaration: public ConfigScalarItem::Declaration
  {
  public:
    /** Hidden constructor. */
    Declaration(bool mandatory, const QString &description="");

    bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const;
    ConfigItem *parseAllocate(YAML::Node &node, QString &msg) const;
    bool parsePopulate(ConfigItem *item, YAML::Node &node, QHash<QString, ConfigItem*> &ctx, QString &msg) const;
  };

public:
  /** Constructor.
   * @param value Specifies the value of the boolean item.
   * @param declaration Specifies the item declaraion.
   * @param parent Specifies the parent of the item. */
  ConfigBoolItem(bool value, const Declaration *declaration, QObject *parent=nullptr);

  /** Returns the actual value. */
  bool value() const;
  /** Sets the value. */
  void setValue(bool val);

protected:
  /** Holds the value of the item. */
  bool _value;
};


/** Represents an integer config item.
 * @ingroup conf*/
class ConfigIntItem: public ConfigScalarItem
{
  Q_OBJECT

public:
  /** Represents a single integer valued field. */
  class Declaration: public ConfigScalarItem::Declaration
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
    ConfigItem *parseAllocate(YAML::Node &node, QString &msg) const;
    bool parsePopulate(ConfigItem *item, YAML::Node &node, QHash<QString, ConfigItem*> &ctx, QString &msg) const;

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
  ConfigIntItem(qint64 value, const Declaration *declaration, QObject *parent=nullptr);

  /** Returns the actual value. */
  qint64 value() const;
  /** Sets the value and limits it to the declaration. */
  void setValue(qint64 value);

protected:
  /** Holds the value of the item. */
  qint64 _value;
};


/** Represents a DMR ID config item.
 * @ingroup conf*/
class ConfigDMRIdItem: public ConfigIntItem
{
  Q_OBJECT

public:
  /** Represents a DMR ID field, that is an integer in range [0, 16777215]. */
  class Declaration: public ConfigIntItem::Declaration
  {
  public:
    /** Constructs a DMR ID field. */
    Declaration(bool mandatory, const QString &description="");

    ConfigItem *parseAllocate(YAML::Node &node, QString &msg) const;
    bool parsePopulate(ConfigItem *item, YAML::Node &node, QHash<QString, ConfigItem*> &ctx, QString &msg) const;
  };

public:
  /** Constructor.
   * @param value Specifies the ID.
   * @param declaration Specifies the item declaraion.
   * @param parent Specifies the parent of the item. */
  ConfigDMRIdItem(uint32_t value, const Declaration *declaration, QObject *parent=nullptr);
};


/** Represents a floating point number config item.
 * @ingroup conf*/
class ConfigFloatItem: public ConfigScalarItem
{
  Q_OBJECT

public:
  /** Represents a single floating point valued field. */
  class Declaration: public ConfigScalarItem::Declaration
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
    ConfigItem *parseAllocate(YAML::Node &node, QString &msg) const;
    bool parsePopulate(ConfigItem *item, YAML::Node &node, QHash<QString, ConfigItem*> &ctx, QString &msg) const;

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
  ConfigFloatItem(double value, const Declaration *declaration, QObject *parent=nullptr);

  /** Returns the value of the item. */
  double value() const;
  /** Sets the value and limits it to the declaration. */
  void setValue(double value);

protected:
  /** Holds the value of the item. */
  double _value;
};


/** Represents a string config item.
 * @ingroup conf*/
class ConfigStrItem: public ConfigScalarItem
{
  Q_OBJECT

public:
  /** Represents a string field. */
  class Declaration: public ConfigScalarItem::Declaration
  {
  public:
    /** Constructs a string-valued field. */
    Declaration(bool mandatory, const QString &description="");

    bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const;

    ConfigItem *parseAllocate(YAML::Node &node, QString &msg) const;
    bool parsePopulate(ConfigItem *item, YAML::Node &node, QHash<QString, ConfigItem*> &ctx, QString &msg) const;
  };

public:
  /** Constructor.
   * @param value Specifies the value of the item.
   * @param declaration Specifies the item declaraion.
   * @param parent Specifies the parent of the item. */
  ConfigStrItem(const QString &value, const Declaration *declaration, QObject *parent=nullptr);

  /** Returns the value of the item. */
  const QString &value() const;
  /** Sets the value. */
  void setValue(const QString &value);

protected:
  /** Holds the value of the item. */
  QString _value;
};


/** Declares a field holding an identifier for cross-refrerence.
 * That is a string, that gets collected to check every reference. See @c CondifDeclRef.
 * @ingroup conf */
class ConfigIdDeclaration: public ConfigStrItem::Declaration
{
public:
  /** Constructor. */
  ConfigIdDeclaration(bool mandatory, const QString &description="");

  bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const;
};


/** Declares a field referencing a previously defined ID.
 * See @c CondifDeclID.
 * @ingroup conf */
class ConfigReferenceDeclaration: public ConfigStrItem::Declaration
{
public:
  /** Constructs a reference field declaration. */
  ConfigReferenceDeclaration(bool mandatory, const QString &description="");

  bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const;
  bool verifyReferences(const YAML::Node &node, const QSet<QString> &ctx, QString &msg) const;
};


/** Represents an enum value config item.
 * @ingroup conf*/
class ConfigEnumItem: public ConfigScalarItem
{
  Q_OBJECT

public:
  /** Represents a single enumeration field. This is a string valued field that can only take
   * one of a pre-defined set of values. */
  class Declaration: public ConfigScalarItem::Declaration
  {
  public:
    /** Constructor.
     * @param values Specifies the possible values of the enum.
     * @param mandatory If @c true, the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration(const QSet<QString> &values, bool mandatory,
                const QString &description="");

    /** Returns the set of possible values. */
    const QSet<QString> &possibleValues() const;

    bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const;
    ConfigItem *parseAllocate(YAML::Node &node, QString &msg) const;
    bool parsePopulate(ConfigItem *item, YAML::Node &node, QHash<QString, ConfigItem*> &ctx, QString &msg) const;

  protected:
    /** Holds the set of possible values. */
    QSet<QString> _values;
  };

public:
  /** Constructor.
   * @param value Specifies the value of the item.
   * @param declaration Specifies the item declaraion.
   * @param parent Specifies the parent of the item. */
  ConfigEnumItem(const QString &value, const Declaration *declaration, QObject *parent=nullptr);

  /** Returns the value of the item. */
  const QString &value() const;
  /** Sets the key and verifies it with the declaration.
   * @returns @c true if the key is defined. */
  bool setValue(const QString &key);

protected:
  /** Holds the enum value. */
  QString _value;
};


/** Represents a mapping, that can only hold a single element.
 * This can be used to specify a predefined set of options.
 * @ingroup conf */
class ConfigDispatchDeclaration: public ConfigItem::Declaration
{
public:
  /** Declares a dispatch/cases.
   * @param elements The map of possible elements. Only one of these can be used.
   * @param mandatory If @c true, the element is mandatory.
   * @param description Specifies the optional element description. */
  ConfigDispatchDeclaration(const QHash<QString, ConfigItem::Declaration *> &elements, bool mandatory,
                     const QString &description="");

  /** Adds an element to the set. */
  bool add(const QString &name, ConfigItem::Declaration *element);

  bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const;
  bool verifyReferences(const YAML::Node &node, const QSet<QString> &ctx, QString &msg) const;

  ConfigItem *parseDefine(YAML::Node &node, QHash<QString, ConfigItem *> &ctx, QString &msg) const;

protected:
  ConfigItem *parseAllocate(YAML::Node &node, QString &msg) const;
  bool parsePopulate(ConfigItem *item, YAML::Node &node, QHash<QString, ConfigItem *> &ctx, QString &msg) const;

protected:
  /** The set of possible elements. */
  QHash<QString, ConfigItem::Declaration *> _elements;
};


/** Represents a key-value map item.
 * @ingroup conf */
class ConfigMapItem: public ConfigItem
{
  Q_OBJECT

public:
  /** Represents a key-value mapping within the configuration. */
  class Declaration: public ConfigItem::Declaration
  {
  public:
    /** Constructs a map declaration.
     * @param elements The fields of the mapping.
     * @param mandatory If @c true, the mapping is mandatory.
     * @param description Specifies the optional object description. */
    Declaration(const QHash<QString, ConfigItem::Declaration *> &elements, bool mandatory,
                const QString &description="");

    /** Adds an element/field to the object. */
    bool add(const QString &name, ConfigItem::Declaration *element);

    bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const;
    bool verifyReferences(const YAML::Node &node, const QSet<QString> &ctx, QString &msg) const;

    ConfigItem *parseAllocate(YAML::Node &node, QString &msg) const;
    bool parsePopulate(ConfigItem *item, YAML::Node &node, QHash<QString, ConfigItem*> &ctx, QString &msg) const;

  protected:
    /** The set of mandatory elements. */
    QSet<QString> _mandatoryElements;
    /** The set of all elements. */
    QHash<QString, ConfigItem::Declaration *> _elements;
  };

public:
  /** Empty constructor.
   * @param declaration Specifies the item declaraion.
   * @param parent Specifies the parent of the item. */
  ConfigMapItem(const Declaration *declaration, QObject *parent=nullptr);

  /** Returns @c true if the object contains the given element. */
  bool has(const QString &name) const;
  /** Adds the given element to the object */
  bool add(const QString &name, ConfigItem *element);
  /** Adds or replaces the given element in the object */
  void set(const QString &name, ConfigItem *element);
  /** Returns the given element by name. */
  ConfigItem *get(const QString &name) const;
  /** Removes the given element from the object and returns it. */
  ConfigItem *take(const QString &name);
  /** Deletes the element from the object. */
  bool del(const QString &name);

protected slots:
  /** Gets called if one of the elements of this object gets deleted. */
  void onElementDeleted(QObject *element);

protected:
  /** Holds the elements of the object. */
  QHash<QString, ConfigItem *>_elements;
};


/** Represents an object config item.
 * @ingroup conf */
class ConfigObjItem: public ConfigMapItem
{
  Q_OBJECT

public:
  /** Represents an object (a key-value mapping with an ID) within the configuration. */
  class Declaration: public ConfigMapItem::Declaration
  {
  public:
    /** Constructs a map declaration.
     * @param mandatory If @c true, the object is mandatory.
     * @param description Specifies the optional object description. */
    Declaration(bool mandatory, const QString &description="");

    ConfigItem *parseAllocate(YAML::Node &node, QString &msg) const;
    bool parsePopulate(ConfigItem *item, YAML::Node &node, QHash<QString, ConfigItem *> &ctx, QString &msg) const;
    ConfigItem *parseDefine(YAML::Node &node, QHash<QString, ConfigItem *> &ctx, QString &msg) const;
  };

public:
  /** Empty constructor.
   * @param declaration Specifies the item declaraion.
   * @param parent Specifies the parent of the item. */
  ConfigObjItem(const Declaration *declaration, QObject *parent=nullptr);
};


/** Represents a list of configuration items.
 * @ingroup conf */
class ConfigList: public ConfigItem
{
  Q_OBJECT

public:
  /** Represents a list within the configuration.
   * The list is a list of equal types. */
  class Declaration: public ConfigItem::Declaration
  {
  public:
    /** Declares a list.
     * The list inherits the mandatory flag from the content.
     *
     * @param element Content declaration.
     * @param description The optional description of the list. */
    Declaration(ConfigItem::Declaration *element, const QString &description="");

    bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const;
    bool verifyReferences(const YAML::Node &node, const QSet<QString> &ctx, QString &msg) const;

    ConfigItem *parseAllocate(YAML::Node &node, QString &msg) const;
    bool parsePopulate(ConfigItem *item, YAML::Node &node, QHash<QString, ConfigItem *> &ctx, QString &msg) const;

  protected:
    /** Holds the content element declaration. */
    ConfigItem::Declaration *_element;
  };

public:
  /** Empty constructor.
   * @param declaration Specifies the item declaraion.
   * @param parent Specifies the parent of the item. */
  ConfigList(const Declaration *declaration, QObject *parent=nullptr);

  /** Returns the number of items in the list. */
  int count() const;
  /** Adds an item to the list. */
  void add(ConfigItem *item);
  /** Returns the i-th item. */
  ConfigItem *get(int i) const;
  /** Removes the i-th item from the list and returns it. */
  ConfigItem *take(int i);
  /** Deletes the i-th item from the list. */
  bool del(int i);

protected slots:
  /** Gets called if a list item is deleted. */
  void onItemDeleted(QObject *item);

protected:
  /** Holds the list items. */
  QVector<ConfigItem *>_items;
};


/** Represents a radio ID and name definition. *
 * @ingroup conf */
class ConfigRadioId: public ConfigObjItem
{
  Q_OBJECT

public:
  /** Declares a radio ID. */
  class Declaration: public ConfigObjItem::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration(bool mandatory, const QString &description="");

    ConfigItem *parseAllocate(YAML::Node &node, QString &msg) const;

  public:
    /** Returns/creates the singleton instance. */
    static Declaration *get();

  private:
    /** The singleton instance. */
    static Declaration *_instance;
  };

public:
  /** Empty constructor. */
  explicit ConfigRadioId(QObject *parent=nullptr);

public:
  /** Returns the name for this radio id. */
  const QString &name() const;
  /** Returns the radio id. */
  uint32_t number() const;
};


/** Base class for all contact definitions.
 * @ingroup conf */
class ConfigContact: public ConfigObjItem
{
Q_OBJECT

public:
  /** Base class of all contact declarations. */
  class Declaration: public ConfigObjItem::Declaration
  {
  protected:
    /** Hidden constructor. */
    Declaration(bool mandatory, const QString &description="");
  };

protected:
  /** Hidden constructor. */
  ConfigContact(Declaration *declaraion, QObject *parent=nullptr);

public:
  /** Returns the name of the contact. */
  const QString &name() const;
  /** Returns @c true if the contact rings. */
  bool ring() const;
};


/** Base class for all digitial (ie, DMR) contacts.
 * @ingroup conf */
class ConfigDigitalContact: public ConfigContact
{
Q_OBJECT

public:
  /** Base class of all digital contact declarations. */
  class Declaration: public ConfigContact::Declaration
  {
  protected:
    /** Hidden constructor. */
    Declaration(bool mandatory, const QString &description="");
  };

protected:
  /** Hidden constructor. */
  ConfigDigitalContact(Declaration *declaraion, QObject *parent=nullptr);
};


/** Represents a private call conact.
 * @ingroup conf */
class ConfigPrivateCall: public ConfigDigitalContact
{
Q_OBJECT

public:
  /** Declares a private call contact. */
  class Declaration: public ConfigDigitalContact::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration(bool mandatory, const QString &description="");

    ConfigItem *parseAllocate(YAML::Node &node, QString &msg) const;

  public:
    /** Returns/creates the singleton instance. */
    static Declaration *get();

  private:
    /** The singleton instance. */
    static Declaration *_instance;
  };

public:
  /** Empty constructor. */
  explicit ConfigPrivateCall(QObject *parent=nullptr);

  /** Returns the number of the contact. */
  uint32_t number() const;
};


/** Represents a group call contact.
 * @ingroup conf */
class ConfigGroupCall: public ConfigDigitalContact
{
Q_OBJECT

public:
  /** Declares a group call contact. */
  class Declaration: public ConfigDigitalContact::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration(bool mandatory, const QString &description="");

    ConfigItem *parseAllocate(YAML::Node &node, QString &msg) const;

  public:
    /** Returns/creates the singleton instance. */
    static Declaration *get();

  private:
    /** The singleton instance. */
    static Declaration *_instance;
  };

public:
  /** Empty constructor. */
  explicit ConfigGroupCall(QObject *parent=nullptr);

  /** Returns the number of the contact. */
  uint32_t number() const;
};


/** Represets an all-call contact.
 * @ingroup conf */
class ConfigAllCall: public ConfigDigitalContact
{
Q_OBJECT

public:
  /** Declares a all call contact. */
  class Declaration: public ConfigDigitalContact::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration(bool mandatory, const QString &description="");

    ConfigItem *parseAllocate(YAML::Node &node, QString &msg) const;

  public:
    /** Returns/creates the singleton instance. */
    static Declaration *get();

  private:
    /** The singleton instance. */
    static Declaration *_instance;
  };

public:
  /** Empty constructor. */
  explicit ConfigAllCall(QObject *parent=nullptr);
};


/** Represents an RX group list.
 * @ingroup conf */
class ConfigGroupList: public ConfigObjItem
{
  Q_OBJECT

public:
  /** Declares a all call contact. */
  class Declaration: public ConfigObjItem::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
     Declaration(bool mandatory, const QString &description="");

     ConfigItem *parseAllocate(YAML::Node &node, QString &msg) const;

  public:
     /** Returns/creates the singleton instance. */
     static Declaration *get();

  private:
     /** The singleton instance. */
     static Declaration *_instance;
  };

public:
  /** Empty constructor. */
  explicit ConfigGroupList(QObject *parent=nullptr);

  /** Returns the name of the group list. */
  const QString &name() const;
  /** Returns the list of members. */
  ConfigList *members() const;
};


/** Base class of all channels.
 * @ingroup conf */
class ConfigChannel: public ConfigObjItem
{
  Q_OBJECT

public:
  /** Base class for all channel declarations. */
  class Declaration: public ConfigObjItem::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration(bool mandatory, const QString &description="");
  };

protected:
  /** Hidden constructor. */
  ConfigChannel(Declaration *declaraion, QObject *parent=nullptr);

public:
  /** Returns the name of the channel. */
  const QString &name() const;
  /** Returns the RX frequency in Mhz. */
  double rx() const;
  /** Returns the TX frequency in Mhz. */
  double tx() const;
  /** Returns the TX power. */
  const QString &power() const;
  /** Returns the transmit timeout for this channel. */
  qint64 txTimeout() const;
  /** Returns @c true if the channel is RX only. */
  bool rxOnly() const;
  /** Returns a reference to the scan-list or @c nullptr if none is set. */
  ConfigScanList *scanlist() const;
};


/** Represents a digital channel (ie, DMR channel).
 * @ingroup conf */
class ConfigDigitalChannel: public ConfigChannel
{
  Q_OBJECT

public:
  /** Declares a digital channel. */
  class Declaration: public ConfigChannel::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration(bool mandatory, const QString &description="");

    ConfigItem *parseAllocate(YAML::Node &node, QString &msg) const;

  public:
    /** Returns/creates the singleton instance. */
    static Declaration *get();

  private:
    /** The singleton instance. */
    static Declaration *_instance;
  };

public:
  /** Empty constructor. */
  explicit ConfigDigitalChannel(QObject *parent=nullptr);

  /** Returns the admit criterion for this channel. */
  const QString &admit() const;
  /** Returns the color-code of the channel. */
  uint8_t colorCode() const;
  /** Returns the time-slot of the channel. */
  uint8_t timeSlot() const;
  /** Returns a reference to the RX group list. */
  ConfigGroupList *groupList() const;
  /** Returns a reference to the default TX contact or @c nullptr if none is set. */
  ConfigDigitalContact *txContact() const;
  /** Returns a reference to the positioning system or @c nullptr if none is set. */
  ConfigPositioning *aprs() const;
  /** Returns a reference to the roaming zone or @c nullptr if none is set. */
  ConfigRoamingZone *roamingZone() const;
  /** Returns a reference to the radio used by the channel or @c nullptr if the default should be used. */
  ConfigRadioId *radioId() const;
};


/** Represents an analog (ie, FM) channel.
 * @ingroup conf */
class ConfigAnalogChannel: public ConfigChannel
{
  Q_OBJECT

public:
  /** Declares an analog channel. */
  class Declaration: public ConfigChannel::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration(bool mandatory, const QString &description="");

    ConfigItem *parseAllocate(YAML::Node &node, QString &msg) const;

  public:
    /** Returns/creates the singleton instance. */
    static Declaration *get();

  private:
    /** The singleton instance. */
    static Declaration *_instance;
  };

public:
  /** Empty constructor. */
  explicit ConfigAnalogChannel(QObject *parent=nullptr);

  /** Returns the admit criterion. */
  const QString &admit() const;
  /** Returns the squelch value. */
  uint8_t squelch() const;
  /** Retunrs the RX signalling. */
  uint32_t rxSignalling() const;
  /** Retunrs the TX signalling. */
  uint32_t txSignalling() const;
  /** Retunrs the band-width. */
  const QString &bandWidth() const;
  /** Retunrs the APRS positioning system or @c nullptr if none is set. */
  ConfigAPRSPositioning *aprs() const;
};


/** Represents a zone.
 * @ingroup conf */
class ConfigZone: public ConfigObjItem
{
  Q_OBJECT

public:
  /** Represents the zone declaration. */
  class Declaration: public ConfigObjItem::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration(bool mandatory, const QString &description="");

    ConfigItem *parseAllocate(YAML::Node &node, QString &msg) const;

  public:
    /** Returns/creates the singleton instance. */
    static Declaration *get();

  private:
    /** The singleton instance. */
    static Declaration *_instance;
  };

public:
  /** Empty constructor. */
  explicit ConfigZone(QObject *parent=nullptr);

  /** Returns the name of the zone. */
  const QString &name() const;
  /** Returns the channel list for VFO A. */
  ConfigList *a() const;
  /** Returns the channel list for VFO B. */
  ConfigList *b() const;
};


/** Represents a scan-list.
 * @ingroup conf */
class ConfigScanList: public ConfigObjItem
{
  Q_OBJECT

public:
  /** Represents the scan-list declaration. */
  class Declaration: public ConfigObjItem::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration(bool mandatory, const QString &description="");

    ConfigItem *parseAllocate(YAML::Node &node, QString &msg) const;

  public:
    /** Returns/creates the singleton instance. */
    static Declaration *get();

  private:
    /** The singleton instance. */
    static Declaration *_instance;
  };


public:
  /** Empty constructor. */
  explicit ConfigScanList(QObject *parent=nullptr);

  /** Returns the name of the scan-list. */
  const QString &name() const;
  /** Returns a reference to the primary channel or @c nullptr if none is set. */
  ConfigChannel *primary() const;
  /** Returns a reference to the secondary channel or @c nullptr if none is set. */
  ConfigChannel *secondary() const;
  /** Returns a reference to the revert/transmit channel or @c nullptr if none is set. */
  ConfigChannel *revert() const;
  /** Returns the channel list */
  ConfigList *channels() const;
};


/** Base class of all positioning systems.
 * @ingroup conf */
class ConfigPositioning: public ConfigObjItem
{
  Q_OBJECT

public:
  /** Base class for all positioning declarations. */
  class Declaration: public ConfigObjItem::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration(bool mandatory, const QString &description="");
  };

protected:
  /** Hidden constructor. */
  ConfigPositioning(Declaration *declaraion, QObject *parent=nullptr);

public:
  /** Returns the name of the positioning system. */
  const QString &name() const;
  /** Returns the update period in seconds. */
  uint32_t period() const;
};


/** Represents an DMR positioning system.
 * @ingroup conf */
class ConfigDMRPositioning: public ConfigPositioning
{
  Q_OBJECT

public:
  /** Declares a DMR positioning system. */
  class Declaration: public ConfigPositioning::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration(bool mandatory, const QString &description="");

    ConfigItem *parseAllocate(YAML::Node &node, QString &msg) const;

  public:
    /** Returns/creates the singleton instance. */
    static Declaration *get();

  private:
    /** The singleton instance. */
    static Declaration *_instance;
  };

public:
  /** Empty constructor. */
  explicit ConfigDMRPositioning(QObject *parent=nullptr);
  /** Returns the destination contact the position is send to. */
  ConfigDigitalContact *destination() const;
  /** Returns the transmit channel or @c nullptr if none is set. */
  ConfigDigitalChannel *channel() const;
};


/** Represents an APRS positioning system.
 * @ingroup conf */
class ConfigAPRSPositioning: public ConfigPositioning
{
  Q_OBJECT

public:
  /** Declares an APRS positioning system. */
  class Declaration: public ConfigPositioning::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration(bool mandatory, const QString &description="");

    ConfigItem *parseAllocate(YAML::Node &node, QString &msg) const;

  public:
    /** Returns/creates the singleton instance. */
    static Declaration *get();

  private:
    /** The singleton instance. */
    static Declaration *_instance;
  };

public:
  /** Empty constructor. */
  ConfigAPRSPositioning(QObject *parent=nullptr);

  /** Returns the source call and SSID. */
  const QString &source() const;
  /** Returns the destination call and SSID. */
  const QString &destination() const;
  /** Returns the transmit channel. */
  ConfigAnalogChannel *channel() const;
  /** Returns the list of path calls and SSIDs. */
  ConfigList *path() const;
  /** Returns the map icon. */
  const QString &icon() const;
  /** Returns the APRS message. */
  const QString &message() const;
};


/** Represents a roaming zone
 * @ingroup conf */
class ConfigRoamingZone: public ConfigObjItem
{
  Q_OBJECT

public:
  /** Declares a roaming zone. */
  class Declaration: public ConfigObjItem::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration(bool mandatory, const QString &description="");

    ConfigItem *parseAllocate(YAML::Node &node, QString &msg) const;

  public:
    /** Returns/creates the singleton instance. */
    static Declaration *get();

  private:
    /** The singleton instance. */
    static Declaration *_instance;
  };

public:
  /** Empty constructor. */
  ConfigRoamingZone(QObject *parent=nullptr);

public:
  /** Returns the name of the roaming zone. */
  const QString &name() const;
  /** Returns the list of channels. */
  ConfigList *channels() const;
};


/** Holds the entire abstract codeplug configuration.
 * @ingroup conf */
class Configuration: public ConfigMapItem
{
  Q_OBJECT

public:
  /** This class represents the complete configuration declaration.
   * That is the codeplug YAML format declaration. */
  class Declaration: public ConfigMapItem::Declaration
  {
  protected:
    /** Constructor. */
    explicit Declaration();

    ConfigItem *parseAllocate(YAML::Node &node, QString &msg) const;

  public:
    /** Verifies the parsed codeplug.
     * @returns @c true on success and @c false otherwise. On error, the @c message argument
     * will contain the error message. */
    virtual bool verify(const YAML::Node &doc, QString &message);

    /** Returns/creates the singleton instance. */
    static Declaration *get();

  private:
    /** The singleton instance. */
    static Declaration *_instance;
  };

public:
  /** Empty constructor. */
  explicit Configuration(QObject *parent = nullptr);
};



#endif // CONFIGITEM_HH
