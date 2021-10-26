/** @defgroup yaml YAML Codeplug format.
 *
 * With version 0.9.0, there is a new extensible human-readable codeplug format. In contrast to the
 * former table based text files, this format is easy to extend. This finally allows me to implement
 * device specific setting.
 */

#ifndef CONFIGREADER_HH
#define CONFIGREADER_HH

#include <QObject>
#include <yaml.h>
#include "configobject.hh"

class Config;
class RadioID;
class Channel;
class AnalogChannel;
class DigitalChannel;
class Zone;
class ScanList;
class Contact;
class DigitalContact;
class DTMFContact;
class RXGroupList;
class PositioningSystem;
class GPSSystem;
class APRSSystem;
class RoamingZone;


/** Base class for all extensible YAML parser.
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
  virtual bool parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx);
  /** Links the given object to the rest of the codeplug using the given context. */
  virtual bool link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx);

protected:
  /** Parses the given extensions for the given object. */
  virtual bool parseExtensions(const QHash<QString, AbstractConfigReader *> &extensions, ConfigObject *obj,
                               const YAML::Node &node, ConfigObject::Context &ctx);
  /** Links the given extensions for the given object. */
  virtual bool linkExtensions(const QHash<QString, AbstractConfigReader *> &extensions, ConfigObject *obj,
                              const YAML::Node &node, const ConfigObject::Context &ctx);

protected:
  /** Holds the error message. */
  QString _errorMessage;
};


/** Base class for all extension parser.
 * This class already implements the @c parse and @c link methods to read all QMetaProperties
 * of the @c ExtensionObject.
 * @ingroup yaml */
class ExtensionReader: public AbstractConfigReader
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit ExtensionReader(QObject *parent=nullptr);
};


/** Implements the config parser.
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

protected:
  /** Parses radio settings. */
  virtual bool parseSettings(Config *config, const YAML::Node &node, ConfigObject::Context &ctx);
  /** Links radio settings. */
  virtual bool linkSettings(Config *config, const YAML::Node &node, const ConfigObject::Context &ctx);

  /** Parses radio IDs. */
  virtual bool parseRadioIDs(Config *config, const YAML::Node &node, ConfigObject::Context &ctx);
  /** Links radio IDs. */
  virtual bool linkRadioIDs(Config *config, const YAML::Node &node, const ConfigObject::Context &ctx);
  /** Parses a single radio ID. */
  virtual bool parseRadioID(Config *config, const YAML::Node &node, ConfigObject::Context &ctx);
  /** Links a single radio ID. */
  virtual bool linkRadioID(RadioID *id, const YAML::Node &node, const ConfigObject::Context &ctx);
  /** Parses a single DMR radio ID. */
  virtual bool parseDMRRadioID(Config *config, const YAML::Node &node, ConfigObject::Context &ctx);
  /** Links a single DMR radio ID. */
  virtual bool linkDMRRadioID(RadioID *id, const YAML::Node &node, const ConfigObject::Context &ctx);

  /** Parses channels. */
  virtual bool parseChannels(Config *config, const YAML::Node &node, ConfigObject::Context &ctx);
  /** Links channels. */
  virtual bool linkChannels(Config *config, const YAML::Node &node, const ConfigObject::Context &ctx);
  /** Parses single channel. */
  virtual bool parseChannel(Config *config, const YAML::Node &node, ConfigObject::Context &ctx);
  /** Links single channel. */
  virtual bool linkChannel(Channel *channel, const YAML::Node &node, const ConfigObject::Context &ctx);
  /** Parses digital channel. */
  virtual bool parseDigitalChannel(Config *config, const YAML::Node &node, ConfigObject::Context &ctx);
  /** Links digital channel. */
  virtual bool linkDigitalChannel(DigitalChannel *channel, const YAML::Node &node, const ConfigObject::Context &ctx);
  /** Parses analog channel. */
  virtual bool parseAnalogChannel(Config *config, const YAML::Node &node, ConfigObject::Context &ctx);
  /** Links analog channel. */
  virtual bool linkAnalogChannel(AnalogChannel *channel, const YAML::Node &node, const ConfigObject::Context &ctx);

  /** Parses zones. */
  virtual bool parseZones(Config *config, const YAML::Node &node, ConfigObject::Context &ctx);
  /** Links zones. */
  virtual bool linkZones(Config *config, const YAML::Node &node, const ConfigObject::Context &ctx);
  /** Parses single zone. */
  virtual bool parseZone(Config *config, const YAML::Node &node, ConfigObject::Context &ctx);
  /** Links single zone. */
  virtual bool linkZone(Zone *zone, const YAML::Node &node, const ConfigObject::Context &ctx);

  /** Parses scan lists. */
  virtual bool parseScanLists(Config *config, const YAML::Node &node, ConfigObject::Context &ctx);
  /** Links scan lists. */
  virtual bool linkScanLists(Config *config, const YAML::Node &node, const ConfigObject::Context &ctx);
  /** Parses single scan list. */
  virtual bool parseScanList(Config *config, const YAML::Node &node, ConfigObject::Context &ctx);
  /** Links single scan list. */
  virtual bool linkScanList(ScanList *list, const YAML::Node &node, const ConfigObject::Context &ctx);

  /** Parses contacts. */
  virtual bool parseContacts(Config *config, const YAML::Node &node, ConfigObject::Context &ctx);
  /** Links contacts. */
  virtual bool linkContacts(Config *config, const YAML::Node &node, const ConfigObject::Context &ctx);
  /** Parses single contact. */
  virtual bool parseContact(Config *config, const YAML::Node &node, ConfigObject::Context &ctx);
  /** Links single contact. */
  virtual bool linkContact(Contact *contact, const YAML::Node &node, const ConfigObject::Context &ctx);
  /** Parses private-call contact. */
  virtual bool parseDMRContact(Config *config, const YAML::Node &node, ConfigObject::Context &ctx);
  /** Links private-call contact. */
  virtual bool linkDMRContact(DigitalContact *contact, const YAML::Node &node, const ConfigObject::Context &ctx);
  /** Parses DTMF contact. */
  virtual bool parseDTMFContact(Config *config, const YAML::Node &node, ConfigObject::Context &ctx);
  /** Links DTMF contact. */
  virtual bool linkDTMFContact(DTMFContact *contact, const YAML::Node &node, const ConfigObject::Context &ctx);

  /** Parses group lists. */
  virtual bool parseGroupLists(Config *config, const YAML::Node &node, ConfigObject::Context &ctx);
  /** Links group lists. */
  virtual bool linkGroupLists(Config *config, const YAML::Node &node, const ConfigObject::Context &ctx);
  /** Parses single group list. */
  virtual bool parseGroupList(Config *config, const YAML::Node &node, ConfigObject::Context &ctx);
  /** Links single group list. */
  virtual bool linkGroupList(RXGroupList *list, const YAML::Node &node, const ConfigObject::Context &ctx);

  /** Parses positioning systems. */
  virtual bool parsePositioningSystems(Config *config, const YAML::Node &node, ConfigObject::Context &ctx);
  /** Links positioning systems. */
  virtual bool linkPositioningSystems(Config *config, const YAML::Node &node, const ConfigObject::Context &ctx);
  /** Parses single positioning system. */
  virtual bool parsePositioningSystem(Config *config, const YAML::Node &node, ConfigObject::Context &ctx);
  /** Links single positioning system. */
  virtual bool linkPositioningSystem(PositioningSystem *system, const YAML::Node &node, const ConfigObject::Context &ctx);
  /** Parses GPS positioning system. */
  virtual bool parseGPSSystem(Config *config, const YAML::Node &node, ConfigObject::Context &ctx);
  /** Links GPS positioning system. */
  virtual bool linkGPSSystem(GPSSystem *system, const YAML::Node &node, const ConfigObject::Context &ctx);
  /** Parses APRS positioning system. */
  virtual bool parseAPRSSystem(Config *config, const YAML::Node &node, ConfigObject::Context &ctx);
  /** Links APRS positioning system. */
  virtual bool linkAPRSSystem(APRSSystem *system, const YAML::Node &node, const ConfigObject::Context &ctx);

  /** Parses roaming zones. */
  virtual bool parseRoamingZones(Config *config, const YAML::Node &node, ConfigObject::Context &ctx);
  /** Links roaming zones. */
  virtual bool linkRoamingZones(Config *config, const YAML::Node &node, const ConfigObject::Context &ctx);
  /** Parses single roaming zone. */
  virtual bool parseRoamingZone(Config *config, const YAML::Node &node, ConfigObject::Context &ctx);
  /** Links single roaming zone. */
  virtual bool linkRoamingZone(RoamingZone *zone, const YAML::Node &node, const ConfigObject::Context &ctx);

public:
  /** Reads the given YAML file and updates the give config. */
  bool read(Config *obj, const QString &filename);

public:
  /** Adds an extension to the config parser.
   * At this level, device specific codeplug elements and radio-wide settings can be added. */
  static AbstractConfigReader *addExtension(ExtensionReader *reader);

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

/** Reads the global radio settings.
 * @ingroup yaml */
class RadioSettingsReader: public AbstractConfigReader
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit RadioSettingsReader(QObject *parent=nullptr);

  ConfigObject *allocate(const YAML::Node &node, const ConfigObject::Context &ctx);
  bool parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx);
  bool link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx);

public:
  /** Adds an extension to the config parser. */
  static bool addExtension(ExtensionReader *ext);

protected:
  /** Holds the exentions for the config parser. */
  static QHash<QString, AbstractConfigReader *> _extensions;
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
  static bool addExtension(ExtensionReader *ext);

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
  /** Adds an extension to the config parser.
   * At this level, extensions for all channels can be added. */
  static AbstractConfigReader* addExtension(ExtensionReader *ext);

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
  /** Adds an extension to the config parser.
   * At this level, extensions to digital channels can be added. */
  static bool addExtension(ExtensionReader *ext);

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
  /** Adds an extension to the config parser.
   * At this level, extension to analog channels can be added. */
  static bool addExtension(ExtensionReader *ext);

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
  static bool addExtension(ExtensionReader *ext);

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
  /** Adds an extension to the config parser.
   * At this level, extensions to all contact types can be added. */
  static bool addExtension(ExtensionReader *ext);

protected:
  /** Holds the exentions for the config parser. */
  static QHash<QString, AbstractConfigReader *> _extensions;
};


/** Base class for digital contact readers.
 * @ingroup yaml */
class DMRContactReader: public ContactReader
{
  Q_OBJECT

public:
  /** Hidden constructor. */
  explicit DMRContactReader(QObject *parent=nullptr);

  ConfigObject *allocate(const YAML::Node &node, const ConfigObject::Context &ctx);
  bool parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx);
  bool link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx);

public:
  /** Adds an extension to the config parser.
   * At this level, extensions to all digital contacts can be added. */
  static AbstractConfigReader *addExtension(ExtensionReader *ext);

protected:
  /** Holds the exentions for the config parser. */
  static QHash<QString, AbstractConfigReader *> _extensions;
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
  /** Adds an extension to the config parser.
   * At this level, extension to all positioning systems can be added. */
  static bool addExtension(ExtensionReader *ext);

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
  static bool addExtension(ExtensionReader *ext);

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
  static bool addExtension(ExtensionReader *ext);

protected:
  /** Holds the exentions for the config parser. */
  static QHash<QString, AbstractConfigReader *> _extensions;
};


/** Reads scan-list definitions.
 * @ingroup yaml */
class ScanListReader: public ObjectReader
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit ScanListReader(QObject *parent=nullptr);

  ConfigObject *allocate(const YAML::Node &node, const ConfigObject::Context &ctx);
  bool parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx);
  bool link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx);

public:
  /** Adds an extension to the config parser. */
  static bool addExtension(ExtensionReader *ext);

protected:
  /** Holds the exentions for the config parser. */
  static QHash<QString, AbstractConfigReader *> _extensions;
};


/** Reads group list definitions.
 * @ingroup yaml */
class GroupListReader: public ObjectReader
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit GroupListReader(QObject *parent=nullptr);

  ConfigObject *allocate(const YAML::Node &node, const ConfigObject::Context &ctx);
  bool parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx);
  bool link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx);

public:
  /** Adds an extension to the config parser. */
  static bool addExtension(ExtensionReader *ext);

protected:
  /** Holds the exentions for the config parser. */
  static QHash<QString, AbstractConfigReader *> _extensions;
};


/** Reads roaming zones definitions.
 * @ingroup yaml */
class RoamingReader: public ObjectReader
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit RoamingReader(QObject *parent=nullptr);

  ConfigObject *allocate(const YAML::Node &node, const ConfigObject::Context &ctx);
  bool parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx);
  bool link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx);

public:
  /** Adds an extension to the config parser. */
  static bool addExtension(ExtensionReader *ext);

protected:
  /** Holds the exentions for the config parser. */
  static QHash<QString, AbstractConfigReader *> _extensions;
};



#endif // CONFIGREADER_HH
