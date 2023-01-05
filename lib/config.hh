/** @defgroup conf Common codeplug configuration
 * This module collects all classes that represent the general configuration for all DMR codeplugs.
 *
 * To this end, it aims at covering the important features for ham radio applications.
 * All features that are more related to "professional" applications and are specific to each radio,
 * are implemented by so-called extensions, see @c ConfigExtension. The central class for the
 * abstract configuration is @c Config, this class represents a complete configuration a.k.a.
 * codeplug of a radio. It contains all the information being programmed into the radio irrespective
 * of the model and manufacturer.
 *
 * The entire configuration (abstract, device independent codeplug) consists of a tree of
 * @c ConfigItem instances. This class forms the base-class of all elements in the configuration
 * (excluding lists etc.). Each @c ConfigItem may have a set of properties. These properties are
 * used to implement the majority of the common functionality concerning the abstract codeplug.
 * These are
 *   - Copying & cloning of elements of the configuration.
 *   - Labeling of codeplug objects (eveything that has an ID for cross referencing).
 *   - Serialzation into YAML (all properties are serialized into YAML automatically if not
 *     prevented by marking the property as not @c SCRIPTABLE).
 *   - Parsing of YAML codeplugs (automatic property parsing can be disabled on a per-property
 *     bassis by marking it as not @c SCRIPTABLE).
 *   - Generic editing of the properties in the GUI.
 *   .
 * To this end, the creation of codeplug extensions is pretty easy, as only the properties for the
 * extension must be defined. The rest is taken care of by the default implementation of the
 * @c ConfigItem::copy, @c ConfigItem::clone, @c ConfigItem::label, @c ConfigItem::serialize,
 * @c ConfigItem::parse and @c ConfigItem::link methods. It is not necessary to override any of
 * these methods if there is a one-to-one mapping between the property and its YAML representation.
 *
 * Frequently, however, it is necessary to represent a property in a different way in YAML. This
 * is usually true if a property may hold different specializations of a common type. For example
 * the channel list may hold analog and digital channels. In YAML, the type is specified explicitly
 * as an enclosing map. This structure is not a one-to-one representation of the actual property
 * (the channel list) to the YAML format. In these cases, the @c ConfigItem::parse and
 * @c ConfigItem::link method might be overridden to implement this.
 */

#ifndef CONFIG_HH
#define CONFIG_HH

#include <QTextStream>

#include "configobject.hh"
#include "contact.hh"
#include "rxgrouplist.hh"
#include "channel.hh"
#include "zone.hh"
#include "scanlist.hh"
#include "gpssystem.hh"
#include "roamingchannel.hh"
#include "roaming.hh"
#include "radioid.hh"
#include "radiosettings.hh"
#include "tyt_extensions.hh"
#include "encryptionextension.hh"

// Forward declaration
class UserDatabase;
class EncryptionExtension;

/** The config class, representing the codeplug configuration.
 *
 * It contains the description of the contacts, channels, zones, etc. of the codeplug
 * configuration.
 *
 * @ingroup conf */
class Config : public ConfigItem
{
	Q_OBJECT

  /** The global radio settings. */
  Q_PROPERTY(RadioSettings* settings READ settings SCRIPTABLE false)
  /** The list of radio IDs. */
  Q_PROPERTY(RadioIDList* radioIDs READ radioIDs SCRIPTABLE false)
  /** The list of contacts. */
  Q_PROPERTY(ContactList* contacts READ contacts SCRIPTABLE false)
  /** The list of group lists. */
  Q_PROPERTY(RXGroupLists* groupLists READ rxGroupLists SCRIPTABLE false)
  /** The list of channels. */
  Q_PROPERTY(ChannelList* channels READ channelList SCRIPTABLE false)
  /** The list of zones. */
  Q_PROPERTY(ZoneList* zones READ zones SCRIPTABLE false)
  /** The list of scan lists. */
  Q_PROPERTY(ScanLists* scanLists READ scanlists SCRIPTABLE false)
  /** The list of positioning systems. */
  Q_PROPERTY(PositioningSystems* positioning READ posSystems SCRIPTABLE false)
  /** The list of roaming channels. */
  Q_PROPERTY(RoamingChannelList* roamingChannels READ roamingChannels SCRIPTABLE false)
  /** The list of roaming zones. */
  Q_PROPERTY(RoamingZoneList* roaming READ roaming SCRIPTABLE false)

  /** Represents the config extension for encryption keys. */
  Q_PROPERTY(CommercialExtension* commercial READ commercialExtension)
  /** Represents the config extension for TyT devices. */
  Q_PROPERTY(TyTConfigExtension* tytExtension READ tytExtension WRITE setTyTExtension)

public:
  /** Constructs an empty configuration. */
  explicit Config(QObject *parent = nullptr);

  bool copy(const ConfigItem &other);
  ConfigItem *clone() const;

  /** Returns @c true if the config was modified, @see modified. */
  bool isModified() const;
  /** Sets the modified flag. */
  void setModified(bool modified);

  /** Returns the radio wide settings. */
  RadioSettings *settings() const;
  /** Returns the list of radio IDs. */
  RadioIDList *radioIDs() const;
  /** Returns the list of contacts. */
	ContactList *contacts() const;
  /** Returns the list of RX group lists. */
  RXGroupLists *rxGroupLists() const;
  /** Returns the list of channels. */
  ChannelList *channelList() const;
  /** Returns the list of zones. */
  ZoneList *zones() const;
  /** Returns the list of scanlists. */
  ScanLists *scanlists() const;
  /** Returns the list of positioning systems. */
  PositioningSystems *posSystems() const;
  /** Returns the list of roaming channels. */
  RoamingChannelList *roamingChannels() const;
  /** Returns the list of roaming zones. */
  RoamingZoneList *roaming() const;

  /** Returns @c true if one of the digital channels has a roaming zone assigned. */
  bool requiresRoaming() const;
  /** Returns @c true if one of the channels has a GPS or APRS system assigned. */
  bool requiresGPS() const;

  /** Clears the complete configuration. */
  void clear();

  const Config *config() const;

  /** Returns the commercial extension. */
  CommercialExtension *commercialExtension() const;

  /** Returns the TyT settings extension.
   * If this extension is not set, returns @c nullptr. */
  TyTConfigExtension *tytExtension() const;
  /** Sets the TyT settings extension. */
  void setTyTExtension(TyTConfigExtension *ext);


public:
  /** Imports a configuration from the given file. */
  bool readCSV(const QString &filename, QString &errorMessage);
  /** Imports a configuration from the given text stream in text format. */
  bool readCSV(QTextStream &stream, QString &errorMessage);

  /** Imports a configuration from the given YAML file. */
  bool readYAML(const QString &filename, const ErrorStack &err=ErrorStack());

  bool parse(const YAML::Node &node, Context &ctx, const ErrorStack &err=ErrorStack());
  bool link(const YAML::Node &node, const Context &ctx, const ErrorStack &err=ErrorStack());

public:
  /** Serializes the configuration into the given stream as text. */
  bool toYAML(QTextStream &stream, const ErrorStack &err=ErrorStack());

protected:
  bool populate(YAML::Node &node, const Context &context, const ErrorStack &err=ErrorStack());

protected slots:
  /** Iternal callback. */
  void onConfigModified();

protected:
  /** If @c true, the configuration was modified. */
  bool _modified;
  /** Radio wide settings. */
  RadioSettings *_settings;
  /** The list of radio IDs. */
  RadioIDList *_radioIDs;
  /** The list of contacts. */
	ContactList *_contacts;
  /** The list of RX group lists. */
  RXGroupLists *_rxGroupLists;
  /** The list of channels. */
  ChannelList *_channels;
  /** The list of zones. */
  ZoneList *_zones;
  /** The list of scan lists. */
  ScanLists *_scanlists;
  /** The list of GPS Systems. */
  PositioningSystems *_gpsSystems;
  /** The list of roaming channels. */
  RoamingChannelList *_roamingChannels;
  /** The list of roaming zones. */
  RoamingZoneList *_roaming;
  /** Owns the TyT settings extension. */
  TyTConfigExtension *_tytExtension;
  /** Owns the commercial extension. */
  CommercialExtension *_commercialExtension;
};

#endif // CONFIG_HH
