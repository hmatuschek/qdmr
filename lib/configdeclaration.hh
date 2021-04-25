/** @defgroup confdecl Configuration declaration classes.
 * @ingroup config
 * This group collects all classes that are used to define the config format. That is all valid
 * fields and structures of the YAML file. These classes are also used to parse the file and
 * verify it.
 */

#ifndef CONFIGDECLARATION_HH
#define CONFIGDECLARATION_HH

#include <QObject>
#include <QSet>
#include <QHash>
#include <yaml-cpp/yaml.h>

/** Base class of all config declarations.
 * @ingroup confdecl */
class ConfigDeclItem: public QObject
{
Q_OBJECT

protected:
  /** Hidden constructor, cannot be instantiated directly.
   * @param desc Gives some short description of the item.
   * @param mandatory If @c true, the item is mandatory for a complete config.
   * @param parent Specifies the QObject parent. */
  ConfigDeclItem(bool mandatory, const QString &desc="", QObject *parent=nullptr);

public:
  /** Destructor. */
  virtual ~ConfigDeclItem();

  /** Returns the description of the config item. */
  const QString &description() const;
  /** Returns @c true if the item is mandatory for a complete config. */
  bool isMandatory() const;

  /** Verifies the structure of the document. Checks if all mandatory elements are present. */
  virtual bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const = 0;
  /** Verifies if all references are defined. */
  virtual bool verifyReferences(const YAML::Node &node, const QSet<QString> &ctx, QString &msg) const;

protected:
  /** Holds the description of the item. */
  QString _description;
  /** If @c true, the item is mandatory for a complete config. */
  bool _mandatory;
};


/** Base class for all scalar values.
 * @ingroup confdecl */
class ConfigDeclScalar: public ConfigDeclItem
{
Q_OBJECT

protected:
  /** Hidden constructor. */
  ConfigDeclScalar(bool mandatory, const QString &description="", QObject *parent=nullptr);

public:
  bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const;
};


/** Represents a single boolean valued field.
 * @ingroup confdecl */
class ConfigDeclBool: public ConfigDeclScalar
{
Q_OBJECT

public:
  /** Constructor. */
  ConfigDeclBool(bool mandatory, const QString &description="", QObject *parent=nullptr);

  bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const;
};


/** Represents a single integer valued field.
 * @ingroup confdecl */
class ConfigDeclInt: public ConfigDeclScalar
{
Q_OBJECT

public:
  /** Constructs an integer field declaration.
   * @param min Specifies the minimum value of the field.
   * @param max Specifies the maximum allowed value of the field.
   * @param mandatory If @c true, the field is mandatory.
   * @param description Optional description of the field.
   * @param parent Specifies the parent QObject. */
  ConfigDeclInt(qint64 min, qint64 max, bool mandatory,
                const QString &description="", QObject *parent=nullptr);

  /** Returns the lower bound. */
  qint64 mininum() const;
  /** Returns the upper bound. */
  qint64 maximum() const;

  bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const;

protected:
  /** Holds the lower bound. */
  qint64 _min;
  /** Holds the upper bound. */
  qint64 _max;
};


/** Represents a DMR ID field, that is an integer in range [0, 16777215].
 * @ingroup confdecl */
class ConfigDeclDMRId: public ConfigDeclInt
{
Q_OBJECT

public:
  /** Constructs a DMR ID field. */
  ConfigDeclDMRId(bool mandatory, const QString &description="", QObject *parent=nullptr);
};


/** Represents a single floating point valued field.
 * @ingroup confdecl */
class ConfigDeclFloat: public ConfigDeclScalar
{
Q_OBJECT

public:
  /** Constructs a floating point value field declaration.
   * @param min Specifies the minimum value of the field.
   * @param max Specifies the maximum allowed value of the field.
   * @param mandatory If @c true, the field is mandatory.
   * @param description Optional description of the field.
   * @param parent Specifies the parent QObject. */
  ConfigDeclFloat(double min, double max, bool mandatory,
                  const QString &description="", QObject *parent=nullptr);

  /** Returns the lower bound. */
  double mininum() const;
  /** Returns the upper bound. */
  double maximum() const;

  bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const;

protected:
  /** Holds the lower bound. */
  double _min;
  /** Holds the upper bound. */
  double _max;
};


/** Represents a string field.
 * @ingroup confdecl */
class ConfigDeclStr: public ConfigDeclScalar
{
  Q_OBJECT

public:
  /** Constructs a string-valued field. */
  ConfigDeclStr(bool mandatory, const QString &description="", QObject *parent=nullptr);

  bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const;
};


/** Declares a field holding an identifier for cross-refrerence.
 * That is a string, that gets collected to check every reference. See @c CondifDeclRef.
 * @ingroup confdecl */
class ConfigDeclID: public ConfigDeclStr
{
  Q_OBJECT

public:
  /** Constructor. */
  ConfigDeclID(bool mandatory, const QString &description="", QObject *parent=nullptr);

  bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const;
};


/** Declares a field referencing a previously defined ID.
 * See @c CondifDeclID.
 * @ingroup confdecl */
class ConfigDeclRef: public ConfigDeclStr
{
  Q_OBJECT

public:
  /** Constructs a reference field declaration. */
  ConfigDeclRef(bool mandatory, const QString &description="", QObject *parent=nullptr);

  bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const;
  bool verifyReferences(const YAML::Node &node, const QSet<QString> &ctx, QString &msg) const;
};


/** Represents a single enumeration field. This is a string valued field that can only take
 * one of a pre-defined set of values.
 * @ingroup confdecl */
class ConfigDeclEnum: public ConfigDeclScalar
{
Q_OBJECT

public:
  /** Constructor.
   * @param values Specifies the possible values of the enum.
   * @param mandatory If @c true, the field is mandatory.
   * @param description Specifies the optional field description.
   * @param parent Specifies the QObject parent. */
  ConfigDeclEnum(const QSet<QString> &values, bool mandatory,
                 const QString &description="", QObject *parent=nullptr);

  /** Returns the set of possible values. */
  const QSet<QString> &possibleValues() const;

  bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const;

protected:
  /** Holds the set of possible values. */
  QSet<QString> _values;
};


/** Represents a mapping, that can only hold a single element.
 * This can be used to specify a predefined set of options.
 * @ingroup confdecl */
class ConfigDeclDispatch: public ConfigDeclItem
{
  Q_OBJECT

public:
  /** Declares a dispatch/cases.
   * @param elements The map of possible elements. Only one of these can be used.
   * @param mandatory If @c true, the element is mandatory.
   * @param description Specifies the optional element description.
   * @param parent Specifies the QObject parent. */
  ConfigDeclDispatch(const QHash<QString, ConfigDeclItem *> &elements, bool mandatory,
                     const QString &description="", QObject *parent=nullptr);

  /** Adds an element to the set. */
  bool add(const QString &name, ConfigDeclItem *element);

  bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const;
  bool verifyReferences(const YAML::Node &node, const QSet<QString> &ctx, QString &msg) const;

protected:
  /** The set of possible elements. */
  QHash<QString, ConfigDeclItem *> _elements;
};


/** Represents an object/mapping within the configuration.
 * @ingroup confdecl */
class ConfigDeclObj: public ConfigDeclItem
{
Q_OBJECT

public:
  /** Constructs an object declaration.
   * @param elements The elements/fields of the object/mapping.
   * @param mandatory If @c true, the object is mandatory.
   * @param description Specifies the optional object description.
   * @param parent Specifies the QObject parent. */
  ConfigDeclObj(const QHash<QString, ConfigDeclItem *> &elements, bool mandatory,
                const QString &description="", QObject *parent=nullptr);

  /** Adds an element/field to the object. */
  bool add(const QString &name, ConfigDeclItem *element);

  bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const;
  bool verifyReferences(const YAML::Node &node, const QSet<QString> &ctx, QString &msg) const;

protected:
  /** The set of mandatory elements. */
  QSet<QString> _mandatoryElements;
  /** The set of all elements. */
  QHash<QString, ConfigDeclItem *> _elements;
};


/** Represents a list within the configuration.
 * The list is a list of equal types.
 * @ingroup confdecl */
class ConfigDeclList: public ConfigDeclItem
{
Q_OBJECT

public:
  /** Declares a list.
   * The list inherits the mandatory flag from the content.
   *
   * @param element Content declaration.
   * @param description The optional description of the list.
   * @param parent Specifies the QObject parent. */
  ConfigDeclList(ConfigDeclItem *element, const QString &description="", QObject *parent=nullptr);

  bool verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const;
  bool verifyReferences(const YAML::Node &node, const QSet<QString> &ctx, QString &msg) const;

protected:
  /** Holds the content element declaration. */
  ConfigDeclItem *_element;
};

/** Base class of all contact declarations.
 * @ingroup confdecl */
class ConfigDeclContact: public ConfigDeclObj
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  ConfigDeclContact(bool mandatory, const QString &description="", QObject *parent=nullptr);
};

/** Declares a private call contact.
 * @ingroup confdecl */
class ConfigDeclPrivateCall: public ConfigDeclContact
{
  Q_OBJECT

public:
  /** Constructor.
   * @param mandatory If @c true the field is mandatory.
   * @param description Specifies the optional field description.
   * @param parent Specifies the QObject parent. */
  ConfigDeclPrivateCall(bool mandatory, const QString &description="", QObject *parent=nullptr);
};

/** Declares a group call contact.
 * @ingroup confdecl */
class ConfigDeclGroupCall: public ConfigDeclContact
{
  Q_OBJECT

public:
  /** Constructor.
   * @param mandatory If @c true the field is mandatory.
   * @param description Specifies the optional field description.
   * @param parent Specifies the QObject parent. */
  ConfigDeclGroupCall(bool mandatory, const QString &description="", QObject *parent=nullptr);
};

/** Declares a all call contact.
 * @ingroup confdecl */
class ConfigDeclAllCall: public ConfigDeclContact
{
  Q_OBJECT

public:
  /** Constructor.
   * @param mandatory If @c true the field is mandatory.
   * @param description Specifies the optional field description.
   * @param parent Specifies the QObject parent. */
  ConfigDeclAllCall(bool mandatory, const QString &description="", QObject *parent=nullptr);
};


/** Base class for all channel declarations.
 * @ingroup confdecl */
class ConfigDeclChannel: public ConfigDeclObj
{
  Q_OBJECT

protected:
  /** Constructor.
   * @param mandatory If @c true the field is mandatory.
   * @param description Specifies the optional field description.
   * @param parent Specifies the QObject parent. */
  ConfigDeclChannel(bool mandatory, const QString &description="", QObject *parent=nullptr);
};


/** Declares a digital channel.
 * @ingroup confdecl */
class ConfigDeclDigitalChannel: public ConfigDeclChannel
{
  Q_OBJECT

public:
  /** Constructor.
   * @param mandatory If @c true the field is mandatory.
   * @param description Specifies the optional field description.
   * @param parent Specifies the QObject parent. */
  ConfigDeclDigitalChannel(bool mandatory, const QString &description="", QObject *parent=nullptr);
};

/** Declares an analog channel.
 * @ingroup confdecl */
class ConfigDeclAnalogChannel: public ConfigDeclChannel
{
  Q_OBJECT

public:
  /** Constructor.
   * @param mandatory If @c true the field is mandatory.
   * @param description Specifies the optional field description.
   * @param parent Specifies the QObject parent. */
  ConfigDeclAnalogChannel(bool mandatory, const QString &description="", QObject *parent=nullptr);
};


/** Base class for all positioning declarations.
 * @ingroup confdecl */
class ConfigDeclPositioning: public ConfigDeclObj
{
  Q_OBJECT

protected:
  /** Constructor.
   * @param mandatory If @c true the field is mandatory.
   * @param description Specifies the optional field description.
   * @param parent Specifies the QObject parent. */
  ConfigDeclPositioning(bool mandatory, const QString &description="", QObject *parent=nullptr);
};

/** Declares a DMR positioning system.
 * @ingroup confdecl */
class ConfigDeclDMRPos: public ConfigDeclPositioning
{
  Q_OBJECT

public:
  /** Constructor.
   * @param mandatory If @c true the field is mandatory.
   * @param description Specifies the optional field description.
   * @param parent Specifies the QObject parent. */
  ConfigDeclDMRPos(bool mandatory, const QString &description="", QObject *parent=nullptr);
};

/** Declares an APRS positioning system.
 * @ingroup confdecl */
class ConfigDeclAPRSPos: public ConfigDeclPositioning
{
  Q_OBJECT

public:
  /** Constructor.
   * @param mandatory If @c true the field is mandatory.
   * @param description Specifies the optional field description.
   * @param parent Specifies the QObject parent. */
  ConfigDeclAPRSPos(bool mandatory, const QString &description="", QObject *parent=nullptr);
};

/** This class represents the complete configuration declaration.
 * That is the codeplug YAML format declaration.
 * @ingroup confdecl */
class ConfigDeclaration: public ConfigDeclObj
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit ConfigDeclaration(QObject *parent = nullptr);

  /** Verifies the parsed codeplug.
   * @returns @c true on success and @c false otherwise. On error, the @c message argument
   * will contain the error message. */
  virtual bool verify(const YAML::Node &doc, QString &message);
};

#endif // CONFIGDECLARATION_HH
