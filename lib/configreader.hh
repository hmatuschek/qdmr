#ifndef CONFIGREADER_HH
#define CONFIGREADER_HH

#include <QObject>
#include <yaml-cpp/yaml.h>
#include "configobject.hh"

class Config;


class AbstractConfigReader : public QObject
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit AbstractConfigReader(QObject *parent = nullptr);

public:
  const QString &errorMessage() const;

  virtual ConfigObject *allocate(const YAML::Node &node, const ConfigObject::Context &ctx) = 0;
  virtual bool parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx) = 0;
  virtual bool link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx) = 0;

protected:
  QString _errorMessage;
};



class ConfigReader: public AbstractConfigReader
{
  Q_OBJECT

public:
  explicit ConfigReader(QObject *parent=nullptr);

  ConfigObject *allocate(const YAML::Node &node, const ConfigObject::Context &ctx);
  bool parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx);
  bool link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx);

public:
  Config *read(YAML::Node &node);
  bool read(Config *obj, YAML::Node &node);
  static bool addExtension(const QString &name, AbstractConfigReader *);

protected:
  static QHash<QString, AbstractConfigReader *> _extensions;
};


class ObjectReader: public AbstractConfigReader
{
  Q_OBJECT

protected:
  explicit ObjectReader(QObject *parent=nullptr);

public:
  bool parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx);
};


class RadioIdReader: public ObjectReader
{
  Q_OBJECT

public:
  explicit RadioIdReader(QObject *parent=nullptr);

  ConfigObject *allocate(const YAML::Node &node, const ConfigObject::Context &ctx);
  bool parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx);
  bool link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx);

public:
  static bool addExtension(const QString &name, AbstractConfigReader *);

protected:
  static QHash<QString, AbstractConfigReader *> _extensions;
};


class ChannelReader: public ObjectReader
{
  Q_OBJECT

protected:
  explicit ChannelReader(QObject *parent=nullptr);

public:
  bool parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx);
  bool link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx);

public:
  static bool addExtension(const QString &name, AbstractConfigReader *);

protected:
  static QHash<QString, AbstractConfigReader *> _extensions;
};


class DigitalChannelReader: public ChannelReader
{
  Q_OBJECT

public:
  explicit DigitalChannelReader(QObject *parent=nullptr);

  ConfigObject *allocate(const YAML::Node &node, const ConfigObject::Context &ctx);
  bool parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx);
  bool link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx);

public:
  static bool addExtension(const QString &name, AbstractConfigReader *);

protected:
  static QHash<QString, AbstractConfigReader *> _extensions;
};


class AnalogChannelReader: public ChannelReader
{
  Q_OBJECT

public:
  explicit AnalogChannelReader(QObject *parent=nullptr);

  ConfigObject *allocate(const YAML::Node &node, const ConfigObject::Context &ctx);
  bool parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx);
  bool link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx);

public:
  static bool addExtension(const QString &name, AbstractConfigReader *);

protected:
  static QHash<QString, AbstractConfigReader *> _extensions;
};


class ZoneReader: public ObjectReader
{
  Q_OBJECT

public:
  explicit ZoneReader(QObject *parent=nullptr);

  ConfigObject *allocate(const YAML::Node &node, const ConfigObject::Context &ctx);
  bool parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx);
  bool link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx);

public:
  static bool addExtension(const QString &name, AbstractConfigReader *);

protected:
  static QHash<QString, AbstractConfigReader *> _extensions;
};


class ContactReader: public ObjectReader
{
  Q_OBJECT

protected:
  explicit ContactReader(QObject *parent=nullptr);

public:
  bool parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx);
  bool link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx);

public:
  static bool addExtension(const QString &name, AbstractConfigReader *);

protected:
  static QHash<QString, AbstractConfigReader *> _extensions;
};


class DigitalContactReader: public ContactReader
{
  Q_OBJECT

protected:
  explicit DigitalContactReader(QObject *parent=nullptr);

public:
  bool parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx);
  bool link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx);

public:
  static bool addExtension(const QString &name, AbstractConfigReader *);

protected:
  static QHash<QString, AbstractConfigReader *> _extensions;
};


class PrivateCallContactReader: public DigitalContactReader
{
  Q_OBJECT

public:
  explicit PrivateCallContactReader(QObject *parent=nullptr);

  ConfigObject *allocate(const YAML::Node &node, const ConfigObject::Context &ctx);
  bool parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx);
};


class GroupCallContactReader: public DigitalContactReader
{
  Q_OBJECT

public:
  explicit GroupCallContactReader(QObject *parent=nullptr);

  ConfigObject *allocate(const YAML::Node &node, const ConfigObject::Context &ctx);
  bool parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx);
};


class AllCallContactReader: public DigitalContactReader
{
  Q_OBJECT

public:
  explicit AllCallContactReader(QObject *parent=nullptr);

  ConfigObject *allocate(const YAML::Node &node, const ConfigObject::Context &ctx);
  bool parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx);
};


class PositioningReader: public ObjectReader
{
  Q_OBJECT

protected:
  explicit PositioningReader(QObject *parent=nullptr);

public:
  bool parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx);
  bool link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx);

public:
  static bool addExtension(const QString &name, AbstractConfigReader *);

protected:
  static QHash<QString, AbstractConfigReader *> _extensions;
};


class GPSSystemReader: public PositioningReader
{
  Q_OBJECT

public:
  explicit GPSSystemReader(QObject *parent=nullptr);

  ConfigObject *allocate(const YAML::Node &node, const ConfigObject::Context &ctx);
  bool parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx);
  bool link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx);

public:
  static bool addExtension(const QString &name, AbstractConfigReader *);

protected:
  static QHash<QString, AbstractConfigReader *> _extensions;
};


class APRSSystemReader: public PositioningReader
{
  Q_OBJECT

public:
  explicit APRSSystemReader(QObject *parent=nullptr);

  ConfigObject *allocate(const YAML::Node &node, const ConfigObject::Context &ctx);
  bool parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx);
  bool link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx);

public:
  static bool addExtension(const QString &name, AbstractConfigReader *);

protected:
  static QHash<QString, AbstractConfigReader *> _extensions;
};




#endif // CONFIGREADER_HH
