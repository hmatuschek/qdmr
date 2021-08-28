/** @defgroup yaml YAML Codeplug format.
 *
 * With version 0.9.0, there is a new extensible human-readable codeplug format. In contrast to the
 * former table based text files, this format is easy to extend. This finally allows me to implement
 * device specific setting.
 */

#ifndef CONFIGREADER_HH
#define CONFIGREADER_HH

#include <QObject>
#include <yaml-cpp/yaml.h>
#include "configobject.hh"

class Config;

/** Base class for all extensible YAML reader.
 * @ingroup yaml */
class AbstractConfigReader : public QObject
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit AbstractConfigReader(QObject *parent = nullptr);

public:
  /** Returns the error message when reading fails. */
  const QString &errorMessage() const;

  /** Allocates the @c ConfigObject depending on the parsed node. */
  virtual ConfigObject *allocate(const YAML::Node &node, const ConfigObject::Context &ctx) = 0;
  /** Parses the given YAML node, updates the given object and updates the given context (IDs). */
  virtual bool parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx) = 0;
  /** Links the given object to the rest of the codeplug using the given context. */
  virtual bool link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx) = 0;

protected:
  /** Holds the error message. */
  QString _errorMessage;
};


/** Implements the config reader.
 * @ingroup yaml */
class ConfigReader: public AbstractConfigReader
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit ConfigReader(QObject *parent=nullptr);

  ConfigObject *allocate(const YAML::Node &node, const ConfigObject::Context &ctx);
  bool parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx);
  bool link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx);

public:
  /** Reads the given config. */
  Config *read(YAML::Node &node);
  /** Reads the given config and updates the give context. */
  bool read(Config *obj, YAML::Node &node);

public:
  /** Adds an extension to the config parser. */
  static bool addExtension(const QString &name, AbstractConfigReader *);

protected:
  /** Holds the exentions for the config parser. */
  static QHash<QString, AbstractConfigReader *> _extensions;
};


/** Base class for all readers that parse elements with IDs.
 * @ingroup yaml */
class ObjectReader: public AbstractConfigReader
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit ObjectReader(QObject *parent=nullptr);

public:
  /** Parses the node and updates the given object. This also associates the read ID with the
   * object in the context. */
  bool parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx);
};


/** Reads radio ID definitions.
 * @ingroup yaml */
class RadioIdReader: public ObjectReader
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit RadioIdReader(QObject *parent=nullptr);

  ConfigObject *allocate(const YAML::Node &node, const ConfigObject::Context &ctx);
  bool parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx);
  bool link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx);

public:
  /** Adds an extension to the config parser. */
  static bool addExtension(const QString &name, AbstractConfigReader *);

protected:
  /** Holds the exentions for the config parser. */
  static QHash<QString, AbstractConfigReader *> _extensions;
};


/** Base class for channel parser.
 * @ingroup yaml */
class ChannelReader: public ObjectReader
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit ChannelReader(QObject *parent=nullptr);

public:
  bool parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx);
  bool link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx);

public:
  /** Adds an extension to the config parser. */
  static bool addExtension(const QString &name, AbstractConfigReader *);

protected:
  /** Holds the exentions for the config parser. */
  static QHash<QString, AbstractConfigReader *> _extensions;
};


/** Reads digital channel definitions.
 * @ingroup yaml */
class DigitalChannelReader: public ChannelReader
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit DigitalChannelReader(QObject *parent=nullptr);

  ConfigObject *allocate(const YAML::Node &node, const ConfigObject::Context &ctx);
  bool parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx);
  bool link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx);

public:
  /** Adds an extension to the config parser. */
  static bool addExtension(const QString &name, AbstractConfigReader *);

protected:
  /** Holds the exentions for the config parser. */
  static QHash<QString, AbstractConfigReader *> _extensions;
};


/** Reads analog channel definitions.
 * @ingroup yaml */
class AnalogChannelReader: public ChannelReader
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit AnalogChannelReader(QObject *parent=nullptr);

  ConfigObject *allocate(const YAML::Node &node, const ConfigObject::Context &ctx);
  bool parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx);
  bool link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx);

public:
  /** Adds an extension to the config parser. */
  static bool addExtension(const QString &name, AbstractConfigReader *);

protected:
  /** Holds the exentions for the config parser. */
  static QHash<QString, AbstractConfigReader *> _extensions;
};


/** Reads digital zone definitions.
 * @ingroup yaml */
class ZoneReader: public ObjectReader
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit ZoneReader(QObject *parent=nullptr);

  ConfigObject *allocate(const YAML::Node &node, const ConfigObject::Context &ctx);
  bool parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx);
  bool link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx);

public:
  /** Adds an extension to the config parser. */
  static bool addExtension(const QString &name, AbstractConfigReader *);

protected:
  /** Holds the exentions for the config parser. */
  static QHash<QString, AbstractConfigReader *> _extensions;
};


/** Base class for all contact parser.
 * @ingroup yaml */
class ContactReader: public ObjectReader
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit ContactReader(QObject *parent=nullptr);

public:
  bool parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx);
  bool link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx);

public:
  /** Adds an extension to the config parser. */
  static bool addExtension(const QString &name, AbstractConfigReader *);

protected:
  /** Holds the exentions for the config parser. */
  static QHash<QString, AbstractConfigReader *> _extensions;
};


/** Base class for digital contact readers.
 * @ingroup yaml */
class DigitalContactReader: public ContactReader
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit DigitalContactReader(QObject *parent=nullptr);

public:
  bool parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx);
  bool link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx);

public:
  /** Adds an extension to the config parser. */
  static bool addExtension(const QString &name, AbstractConfigReader *);

protected:
  /** Holds the exentions for the config parser. */
  static QHash<QString, AbstractConfigReader *> _extensions;
};


/** Reads private call contact definitions.
 * @ingroup yaml */
class PrivateCallContactReader: public DigitalContactReader
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit PrivateCallContactReader(QObject *parent=nullptr);

  ConfigObject *allocate(const YAML::Node &node, const ConfigObject::Context &ctx);
  bool parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx);
};


/** Reads group call contact definitions.
 * @ingroup yaml */
class GroupCallContactReader: public DigitalContactReader
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit GroupCallContactReader(QObject *parent=nullptr);

  ConfigObject *allocate(const YAML::Node &node, const ConfigObject::Context &ctx);
  bool parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx);
};


/** Reads all call contact definitions.
 * @ingroup yaml */
class AllCallContactReader: public DigitalContactReader
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit AllCallContactReader(QObject *parent=nullptr);

  ConfigObject *allocate(const YAML::Node &node, const ConfigObject::Context &ctx);
  bool parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx);
};


/** Base class for all positioning system parsers.
 * @ingroup yaml */
class PositioningReader: public ObjectReader
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit PositioningReader(QObject *parent=nullptr);

public:
  bool parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx);
  bool link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx);

public:
  /** Adds an extension to the config parser. */
  static bool addExtension(const QString &name, AbstractConfigReader *);

protected:
  /** Holds the exentions for the config parser. */
  static QHash<QString, AbstractConfigReader *> _extensions;
};


/** Reads GPS system definitions.
 * @ingroup yaml */
class GPSSystemReader: public PositioningReader
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit GPSSystemReader(QObject *parent=nullptr);

  ConfigObject *allocate(const YAML::Node &node, const ConfigObject::Context &ctx);
  bool parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx);
  bool link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx);

public:
  /** Adds an extension to the config parser. */
  static bool addExtension(const QString &name, AbstractConfigReader *);

protected:
  /** Holds the exentions for the config parser. */
  static QHash<QString, AbstractConfigReader *> _extensions;
};


/** Reads APRS system definitions.
 * @ingroup yaml */
class APRSSystemReader: public PositioningReader
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit APRSSystemReader(QObject *parent=nullptr);

  ConfigObject *allocate(const YAML::Node &node, const ConfigObject::Context &ctx);
  bool parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx);
  bool link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx);

public:
  /** Adds an extension to the config parser. */
  static bool addExtension(const QString &name, AbstractConfigReader *);

protected:
  /** Holds the exentions for the config parser. */
  static QHash<QString, AbstractConfigReader *> _extensions;
};


#endif // CONFIGREADER_HH
