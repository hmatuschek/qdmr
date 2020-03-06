/** @defgroup conf Common codeplug configuration
 * This module collects all classes that represent the general configuration for all DMR codeplugs.
 * To this end, it aims at covering the important features for ham radio applications but ignoring
 * all features that are more related to "professional" applications of these radios.
 *
 * The central class is @c Config, this class represents a complete configuration a.k.a. codeplug
 * of a radio. It contains all the information being programmed into the radio irrespective of the
 * model and manufacturer. */

#ifndef CONFIG_HH
#define CONFIG_HH

#include <QTextStream>

#include "contact.hh"
#include "rxgrouplist.hh"
#include "channel.hh"
#include "zone.hh"
#include "scanlist.hh"
#include "gpssystem.hh"

// Forward declaration
class UserDatabase;

/** The config class, representing the codeplug configuration.
 *
 * It contains the description of the contacts, channels, zones, etc. of the codeplug
 * configuration.
 *
 * @ingroup conf */
class Config : public QObject
{
	Q_OBJECT

public:
  /** Constructs an empty configuration. */
  explicit Config(UserDatabase *userdb=nullptr, QObject *parent = nullptr);

  /** Returns @c true if the config was modified, @see modified. */
  bool isModified() const;
  /** Sets the modified flag. */
  void setModified(bool modified);

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
  /** Returns the list of GPS systems. */
  GPSSystems *gpsSystems() const;

  /** Returns the DMR ID of the radio. */
  uint id() const;
  /** (Re-)Sets the DMR ID of the radio. */
  void setId(uint id);

  /** Returns the name of the radio. */
  const QString &name() const;
  /** (Re-)Sets the name of the radio. */
  void setName(const QString &name);

  /** Returns the first intro line. */
  const QString &introLine1() const;
  /** (Re-)Sets the first intro line. */
  void setIntroLine1(const QString &line);
  /** Returns the second intro line. */
  const QString &introLine2() const;
  /** (Re-)Sets the second intro line. */
  void setIntroLine2(const QString &line);

  /** Returns the MIC amplification level [1,10]. */
  uint micLevel() const;
  /** (Re-)Sets the MIC amplification level [1,10]. */
  void setMicLevel(uint value);

  /** Returns @c true if the speech synthesis is enabled. */
  bool speech() const;
  /** Enables/disables the speech synthesis. */
  void setSpeech(bool enabled);

  /** Should the UserDB be uploaded. */
  bool uploadUserDB() const;
  /** Enables/disables the upload of the user-db. */
  void setUploadUserDB(bool upload);
  /** Returns @c true if a user-db is associated with this config. */
  bool hasUserDB() const;
  /** Get the user-db to upload. */
  UserDatabase *userDB() const;
  /** Sets the user-db to upload. */
  void setUserDB(UserDatabase *userdb);

  /** Clears the complete configuration. */
  void reset();

  /** Imports a configuration from the given file. */
  bool readCSV(const QString &filename, QString &errorMessage);
  /** Imports a configuration from the given text stream in text format. */
  bool readCSV(QTextStream &stream, QString &errorMessage);
  /** Exports the configuration to the given file. */
  bool writeCSV(const QString &filename, QString &errorMessage);
  /** Exports the configuration to the given text stream in text format. */
  bool writeCSV(QTextStream &stream, QString &errorMessage);

signals:
  /** Gets emitted if the configuration gets changed. */
	void modified();

protected slots:
  /** Iternal callback. */
  void onConfigModified();
  /** Gets called whenever the UserDB gets deleted. */
  void onUserDBDeleted();

protected:
  /** If @c true, the configuration was modified. */
  bool _modified;
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
  GPSSystems *_gpsSystems;

  /** DMR id of the radio. */
  uint _id;
  /** The name of the radio. */
  QString _name;
  /** The fist intro line. */
  QString _introLine1;
  /** The second intro line. */
  QString _introLine2;
  /** The MIC amplification level [1-10]. */
  uint _mic_level;
  /** If @c true, speech synthesis is enabled. */
  bool _speech;
  /** If @c true, the user-db should be uploaded. */
  bool _uploadUserDB;
  /** Weak reference to the user database. */
  UserDatabase *_userDB;
};

#endif // CONFIG_HH
