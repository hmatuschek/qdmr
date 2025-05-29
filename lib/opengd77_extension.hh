/** @defgroup ogd77ex OpenGD77 Extensions
 * This module collects classes that implement the firmware specific extensions to the common
 * codeplug configuration for radios running the OpenGD77 firmware.
 *
 * @ingroup ogd77 */
#ifndef OPENGD77EXTENSION_HH
#define OPENGD77EXTENSION_HH

#include <QGeoCoordinate>
#include "configobject.hh"
#include "opengd77_extension.hh"


/** Implements the channel extensions for the OpenGD77 radios.
 * @since 0.9.0
 * @ingroup ogd77ex */
class OpenGD77ChannelExtension: public ConfigExtension
{
  Q_OBJECT

  Q_CLASSINFO("description", "Channel settings for OpenGD77 radios.")
  Q_CLASSINFO("longDescription", "This extension implements all channel settings specific to radios "
              "running the OpenGD77 firmware.")

  /** The zone skip flag. */
  Q_PROPERTY(bool scanZoneSkip READ scanZoneSkip WRITE enableScanZoneSkip)
  /** The all skip flag. */
  Q_PROPERTY(bool scanAllSkip READ scanAllSkip WRITE enableScanAllSkip)
  /** The beep enable flag. */
  Q_PROPERTY(bool beep READ beep WRITE enableBeep)
  /** The power save enable flag. */
  Q_PROPERTY(bool powerSave READ powerSave WRITE enablePowerSave)
  /** Sets a fixed location for the APRS report. */
  Q_PROPERTY(QString location READ locator WRITE setLocator)
  /** Sets the talker alias for timeslot 1. */
  Q_PROPERTY(TalkerAlias talkerAliasTS1 READ talkerAliasTS1 WRITE setTalkerAliasTS1)
  /** Sets the talker alias for timeslot 2. */
  Q_PROPERTY(TalkerAlias talkerAliasTS2 READ talkerAliasTS2 WRITE setTalkerAliasTS2)


public:
  enum class TalkerAlias {
    None, APRS, Text, Both
  };
  Q_ENUM(TalkerAlias)


public:
  /** Constructor. */
  Q_INVOKABLE explicit OpenGD77ChannelExtension(QObject *parent=nullptr);

  ConfigItem *clone() const;

  /** Returns @c true if the zone skip flag is set. */
  bool scanZoneSkip() const;
  /** Enables/disables zone skip. */
  void enableScanZoneSkip(bool enable);

  /** Returns @c true if the all-skip flag is set. */
  bool scanAllSkip() const;
  /** Enables/disables all skip. */
  void enableScanAllSkip(bool enable);

  /** Returns @c true if the beep tone is enabled for this channel. */
  bool beep() const;
  /** Enable beep tone for this channel. */
  void enableBeep(bool enable);

  /** Returns @c true, if power save is enabled for this channel (default: true). */
  bool powerSave() const;
  /** Enables power save for this channel. */
  void enablePowerSave(bool enable);

  /** Returns the fixed location for this channel. */
  const QGeoCoordinate &location() const;
  /** Returns the fixed location for this channel. */
  QString locator() const;
  /** Sets the fixed location for this channel. */
  void setLocation(const QGeoCoordinate &loc);
  /** Sets the fixed location for this channel. */
  void setLocator(const QString &locator);

  /** Returns the talker alias setting for timeslot 1. */
  TalkerAlias talkerAliasTS1() const;
  /** Sets the talker alias setting for timeslot 1. */
  void setTalkerAliasTS1(TalkerAlias ta);

  /** Returns the talker alias setting for timeslot 2. */
  TalkerAlias talkerAliasTS2() const;
  /** Sets the talker alias setting for timeslot 2. */
  void setTalkerAliasTS2(TalkerAlias ta);

protected:
  /** Holds the zone skip flag. */
  bool _zoneSkip;
  /** Holds the all skip flag. */
  bool _allSkip;
  /** Holds the beep enable flag. */
  bool _beep;
  /** Holds the power-save flag. */
  bool _powerSave;
  /** Holds the fixed location. */
  QGeoCoordinate _location;
  /** Holds the talker alias setting for timeslot 1. */
  TalkerAlias _txTalkerAliasTS1;
  /** Holds the talker alias setting for timeslot 2. */
  TalkerAlias _txTalkerAliasTS2;
};


/** Implements the contact extensions for the OpenGD77 radios.
 * @since 0.9.0
 * @ingroup ogd77ex */
class OpenGD77ContactExtension: public ConfigExtension
{
  Q_OBJECT

  Q_CLASSINFO("description", "DMR contact settings for OpenGD77 radios.")
  Q_CLASSINFO("longDescription", "This extension implements all contact settings specific to radios "
              "running the OpenGD77 firmware. As the OpenGD77 codeplug is derived from the "
              "Radioddity GD77 codeplug, all Radioddity extension also apply.")

  /** If set, overrides the channel time slot if this contact is selected as the transmit contact. */
  Q_PROPERTY(TimeSlotOverride timeSlotOverride READ timeSlotOverride WRITE setTimeSlotOverride)
  Q_CLASSINFO("timeSlotOverrideDescription", "If set, overrides the channels timeslot.")
  Q_CLASSINFO("timeSlotOverrideLongDescription",
              "The OpenGD77 firmware allows contacts to override the channel time slot if the "
              "contact is selected as the current destination contact for that channel. This allows "
              "to assign a specific time slot to a contact, rather than creating a particular "
              "channel for that contact that only differs in the time slot.")

public:
  /** Possible modes of time slot override. */
  enum class TimeSlotOverride {
    None, ///< Do not override time slot.
    TS1,  ///< Override with time slot 1.
    TS2   ///< Override with time slot 2.
  };
  Q_ENUM(TimeSlotOverride)

public:
  /** Constructor. */
  Q_INVOKABLE explicit OpenGD77ContactExtension(QObject *parent=nullptr);

  ConfigItem *clone() const;

  /** Returns the time slot override. */
  TimeSlotOverride timeSlotOverride() const;
  /** Sets the time slot override. */
  void setTimeSlotOverride(TimeSlotOverride ts);

protected:
  /** Holds the time slot override. */
  TimeSlotOverride _timeSlotOverride;
};


class OpenGD77APRSSystemExtension: public ConfigExtension
{
  Q_OBJECT

  Q_CLASSINFO("description", "OpenGD77 specific APRS settings.")

  /** Sets a fixed location for the APRS report. */
  Q_PROPERTY(QString location READ locator WRITE setLocator)
  Q_CLASSINFO("locationDescription", "Allows to set a fixed location being transmitted.")

public:
  Q_INVOKABLE explicit OpenGD77APRSSystemExtension(QObject *parent=nullptr);

  ConfigItem *clone() const;

  /** Returns the fixed location for this APRS system. */
  const QGeoCoordinate &location() const;
  /** Returns the fixed location for this system. */
  QString locator() const;
  /** Sets the fixed location for this system. */
  void setLocation(const QGeoCoordinate &loc);
  /** Sets the fixed location for this system. */
  void setLocator(const QString &locator);

protected:
  /** Holds the fixed location, if set. */
  QGeoCoordinate _location;
};

#endif // OPENGD77EXTENSION_HH
