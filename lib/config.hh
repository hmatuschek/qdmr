/** @defgroup conf Common codeplug configuration
 * This module collects all classes that represent the general configuration for all DMR codeplugs.
 *
 * To this end, it aims at covering the important features for ham radio applications but ignoring
 * all features that are more related to "professional" applications of these radios.
 *
 * The central class is @c Config, this class represents a complete configuration a.k.a. codeplug
 * of a radio. It contains all the information being programmed into the radio irrespective of the
 * model and manufacturer. */

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
#include "roaming.hh"
#include "radioid.hh"
#include "radiosettings.hh"

// Forward declaration
class UserDatabase;

/** The config class, representing the codeplug configuration.
 *
 * It contains the description of the contacts, channels, zones, etc. of the codeplug
 * configuration.
 *
 * @ingroup conf */
class Config : public ConfigObject
{
	Q_OBJECT

public:
  /** Constructs an empty configuration. */
  explicit Config(QObject *parent = nullptr);

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
  /** Retruns the list of scanlists. */
  ScanLists *scanlists() const;
  /** Returns the list of positioning systems. */
  PositioningSystems *posSystems() const;
  /** Returns the list of roaming zones. */
  RoamingZoneList *roaming() const;

  /** Returns @c true if one of the digital channels has a roaming zone assigned. */
  bool requiresRoaming() const;
  /** Returns @c true if one of the channels has a GPS or APRS system assigned. */
  bool requiresGPS() const;

  /** Clears the complete configuration. */
  void clear();

public:
  /** Imports a configuration from the given file. */
  bool readCSV(const QString &filename, QString &errorMessage);
  /** Imports a configuration from the given text stream in text format. */
  bool readCSV(QTextStream &stream, QString &errorMessage);

  /** Imports a configuration from the given YAML file. */
  bool readYAML(const QString &filename);

  ConfigObject *allocateChild(QMetaProperty &prop, const YAML::Node &node, const Context &ctx);
  bool parse(const YAML::Node &node, Context &ctx);
  bool link(const YAML::Node &node, const Context &ctx);

public:
  bool label(Context &context);
  /** Serializes the configuration into the given stream as text. */
  bool toYAML(QTextStream &stream);

protected:
  bool populate(YAML::Node &node, const Context &context);

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
  /** The list of roaming zones. */
  RoamingZoneList *_roaming;
};

#endif // CONFIG_HH
