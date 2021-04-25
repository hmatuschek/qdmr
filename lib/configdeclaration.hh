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
   * @param name Specifies the name of the item.
   * @param description Gives some short description of the item.
   * @param mandatory If @c true, the item is mandatory for a complete config. */
  ConfigDeclItem(bool mandatory, const QString &desc="", QObject *parent=nullptr);

public:
  /** Destructor. */
  virtual ~ConfigDeclItem();

  /** Returns the description of the config item. */
  const QString &description() const;
  /** Returns @c true if the item is mandatory for a complete config. */
  bool isMandatory() const;

  virtual bool verifyForm(const YAML::Node &node, QString &msg) const = 0;

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
  ConfigDeclScalar(bool mandatory, const QString &description="", QObject *parent=nullptr);

public:
  bool verifyForm(const YAML::Node &node, QString &msg) const;
};


/** Represents a single boolean valued field.
 * @ingroup confdecl */
class ConfigDeclBool: public ConfigDeclScalar
{
Q_OBJECT

public:
  ConfigDeclBool(bool mandatory, const QString &description="", QObject *parent=nullptr);

  bool verifyForm(const YAML::Node &node, QString &msg) const;
};


/** Represents a single integer valued field.
 * @ingroup confdecl */
class ConfigDeclInt: public ConfigDeclScalar
{
Q_OBJECT

public:
  ConfigDeclInt(qint64 min, qint64 max, bool mandatory,
                const QString &description="", QObject *parent=nullptr);

  qint64 mininum() const;
  qint64 maximum() const;

  bool verifyForm(const YAML::Node &node, QString &msg) const;

protected:
  qint64 _min;
  qint64 _max;
};


/** Represents a DMR ID field, that is an integer in range [0, 16777215].
 * @ingroup confdecl */
class ConfigDeclDMRId: public ConfigDeclInt
{
Q_OBJECT

public:
  ConfigDeclDMRId(bool mandatory, const QString &description="", QObject *parent=nullptr);
};


/** Represents a single floating point valued field.
 * @ingroup confdecl */
class ConfigDeclFloat: public ConfigDeclScalar
{
Q_OBJECT

public:
  ConfigDeclFloat(double min, double max, bool mandatory,
                  const QString &description="", QObject *parent=nullptr);

  double mininum() const;
  double maximum() const;

  bool verifyForm(const YAML::Node &node, QString &msg) const;

protected:
  double _min;
  double _max;
};


/** Represents a single integer valued field.
 * @ingroup confdecl */
class ConfigDeclStr: public ConfigDeclScalar
{
  Q_OBJECT

public:
  ConfigDeclStr(bool mandatory, const QString &description="", QObject *parent=nullptr);

  bool verifyForm(const YAML::Node &node, QString &msg) const;
};


class ConfigDeclID: public ConfigDeclStr
{
  Q_OBJECT

public:
  ConfigDeclID(bool mandatory, const QString &description="", QObject *parent=nullptr);

  bool verifyForm(const YAML::Node &node, QString &msg) const;
};


class ConfigDeclRef: public ConfigDeclStr
{
  Q_OBJECT

public:
  ConfigDeclRef(bool mandatory, const QString &description="", QObject *parent=nullptr);

  bool verifyForm(const YAML::Node &node, QString &msg) const;
};


/** Represents a single enumeration field.
 * @ingroup confdecl */
class ConfigDeclEnum: public ConfigDeclScalar
{
Q_OBJECT

public:
  ConfigDeclEnum(const QSet<QString> &values, bool mandatory,
                 const QString &description="", QObject *parent=nullptr);

  const QSet<QString> &possibleValues() const;

  bool verifyForm(const YAML::Node &node, QString &msg) const;

protected:
  QSet<QString> _values;
};


class ConfigDeclDispatch: public ConfigDeclItem
{
  Q_OBJECT

public:
  ConfigDeclDispatch(const QHash<QString, ConfigDeclItem *> &elements, bool mandatory,
                     const QString &description="", QObject *parent=nullptr);

  bool add(const QString &name, ConfigDeclItem *element);

  bool verifyForm(const YAML::Node &node, QString &msg) const;

protected:
  QHash<QString, ConfigDeclItem *> _elements;
};


/** Represents an object within the configuration.
 * @ingroup confdecl */
class ConfigDeclObj: public ConfigDeclItem
{
Q_OBJECT

public:
  ConfigDeclObj(const QHash<QString, ConfigDeclItem *> &elements, bool mandatory,
                const QString &description="", QObject *parent=nullptr);

  bool add(const QString &name, ConfigDeclItem *element);

  bool verifyForm(const YAML::Node &node, QString &msg) const;

protected:
  QSet<QString> _mandatoryElements;
  QHash<QString, ConfigDeclItem *> _elements;
};


/** Represents a list within the configuration.
 * @ingroup confdecl */
class ConfigDeclList: public ConfigDeclItem
{
Q_OBJECT

public:
  ConfigDeclList(ConfigDeclItem *element, const QString &description="", QObject *parent=nullptr);

  bool verifyForm(const YAML::Node &node, QString &msg) const;

protected:
  ConfigDeclItem *_element;
};


class ConfigDeclContact: public ConfigDeclObj
{
  Q_OBJECT

protected:
  ConfigDeclContact(bool mandatory, const QString &description="", QObject *parent=nullptr);
};

class ConfigDeclPrivateCall: public ConfigDeclContact
{
  Q_OBJECT

public:
  ConfigDeclPrivateCall(bool mandatory, const QString &description="", QObject *parent=nullptr);
};

class ConfigDeclGroupCall: public ConfigDeclContact
{
  Q_OBJECT

public:
  ConfigDeclGroupCall(bool mandatory, const QString &description="", QObject *parent=nullptr);
};

class ConfigDeclAllCall: public ConfigDeclContact
{
  Q_OBJECT

public:
  ConfigDeclAllCall(bool mandatory, const QString &description="", QObject *parent=nullptr);
};


class ConfigDeclChannel: public ConfigDeclObj
{
  Q_OBJECT

protected:
  ConfigDeclChannel(bool mandatory, const QString &description="", QObject *parent=nullptr);
};


class ConfigDeclDigitalChannel: public ConfigDeclChannel
{
  Q_OBJECT

public:
  ConfigDeclDigitalChannel(bool mandatory, const QString &description="", QObject *parent=nullptr);
};


class ConfigDeclAnalogChannel: public ConfigDeclChannel
{
  Q_OBJECT

public:
  ConfigDeclAnalogChannel(bool mandatory, const QString &description="", QObject *parent=nullptr);
};


class ConfigDeclaration: public ConfigDeclObj
{
  Q_OBJECT

public:
  explicit ConfigDeclaration(QObject *parent = nullptr);

};

#endif // CONFIGDECLARATION_HH
