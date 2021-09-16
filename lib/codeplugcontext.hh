#ifndef CODEPLUGCONTEXT_HH
#define CODEPLUGCONTEXT_HH

#include <QHash>
#include "config.hh"

/** Helper class to assemble radio config from binary codeplugs that use index-tables and bitmaps
 * to specify which elements of the codeplug are valid. This may lead to sparse specification
 * of channels etc. Hence an codeplug index to config object mapping is needed. This class
 * provides this mapping.
 *
 * @deprecated With version 0.9.0, there is an extensible codeplug context @c CodePlug::Context.
 *             This class will be phased out and replaced with the new one.
 * @ingroup util */
class CodeplugContext
{
public:
  /** Wraps the given device config. */
  CodeplugContext(Config *config);

  /** Returns the wrapped config. */
  Config *config() const;

  /** Returns @c true if the radio DMR ID of given index is set.*/
  bool hasRadioId(int index) const;
  /** Stores the given ID at the specified index as the default radio ID. */
  bool setDefaultRadioId(uint32_t id, int index);
  /** Adds the given radio ID at the given index to the list. */
  bool addRadioId(uint32_t id, int index, const QString &name="");
  /** Maps the given index to the associated radio ID. */
  RadioID *getRadioId(int idx) const;

  /** Returns @c true, if the given channel index has been defined before. */
  bool hasChannel(int index) const;
  /** Adds a channel to the config and maps the given index to that channel. */
  bool addChannel(Channel *ch, int index);
  /** Gets a channel for the specified index or @c nullptr if not defined. */
  Channel *getChannel(int index) const;

  /** Returns @c true, if the given digital contact index has been defined before. */
  bool hasDigitalContact(int index) const;
  /** Adds a digital contact to the config and maps the given index to that contact. */
  bool addDigitalContact(DigitalContact *con, int index);
  /** Gets a digital contact for the specified index or @c nullptr if not defined. */
  DigitalContact *getDigitalContact(int index) const;

  /** Returns @c true, if the given analog contact index has been defined before. */
  bool hasAnalogContact(int index) const;
  /** Adds a analog contact to the config and maps the given index to that contact. */
  bool addAnalogContact(DTMFContact *con, int index);
  /** Gets a analog contact for the specified index or @c nullptr if not defined. */
  DTMFContact *getAnalogContact(int index) const;

  /** Returns @c true, if the given RX group list index has been defined before. */
  bool hasGroupList(int index) const;
  /** Adds a RX group list to the config and maps the given index to that group list. */
  bool addGroupList(RXGroupList *grp, int index);
  /** Gets a RX group list for the specified index or @c nullptr if not defined. */
  RXGroupList *getGroupList(int index) const;

  /** Returns @c true, if the given scan list index has been defined before. */
  bool hasScanList(int index) const;
  /** Adds a scan list to the config and maps the given index to that scan list. */
  bool addScanList(ScanList *lst, int index);
  /** Gets a scan list for the specified index or @c nullptr if not defined. */
  ScanList *getScanList(int index) const;

  /** Returns @c true, if the given GPS system index has been defined before. */
  bool hasGPSSystem(int index) const;
  /** Adds a GPS system to the config and maps the given index to that GPS system. */
  bool addGPSSystem(GPSSystem *sys, int index);
  /** Gets a GPS system for the specified index or @c nullptr if not defined. */
  GPSSystem *getGPSSystem(int index) const;

  /** Returns @c true, if the given APRS system index has been defined before. */
  bool hasAPRSSystem(int index) const;
  /** Adds an APRS system to the config and maps the given index to that APRS system. */
  bool addAPRSSystem(APRSSystem *sys, int index);
  /** Gets an APRS system for the specified index or @c nullptr if not defined. */
  APRSSystem *getAPRSSystem(int index) const;

  /** Retruns @c true if the given roaming zone index is defined. */
  bool hasRoamingZone(int index) const;
  /** Associates the given roaming zone with the given index. */
  bool addRoamingZone(RoamingZone *zone, int index);
  /** Returns the roaming zone associated with the given index. */
  RoamingZone *getRoamingZone(int index) const;
  /** Returns @c true if a roaming channel is defined for the given index. */
  bool hasRoamingChannel(int index) const;
  /** Associates the given digital channel as the roaming channel with the given index. */
  bool addRoamingChannel(DigitalChannel *ch, int index);
  /** Returns the roaming channel associated with the given index. */
  DigitalChannel *getRoamingChannel(int index) const;

protected:
  /** The wrapped radio config (aka abstract code-plug). */
  Config *_config;
  /** Maps a code-plug radio ID index to the radio ID index within the wrapped radio config. */
  QHash<int, int> _radioIDTable;
  /** Maps a code-plug channel index to the channel index within the wrapped radio config. */
  QHash<int, int> _channelTable;
  /** Maps a code-plug digital-contact index to the contact index within the wrapped radio config. */
  QHash<int, int> _digitalContactTable;
  /** Maps a code-plug analog-contact index to the contact index within the wrapped radio config. */
  QHash<int, int> _analogContactTable;
  /** Maps a code-plug RX group-list index to the group-list index within the wrapped radio config. */
  QHash<int, int> _groupListTable;
  /** Maps a code-plug scan-list index to the scan-list index within the wrapped radio config. */
  QHash<int, int> _scanListTable;
  /** Maps a code-plug GPS system index to the GPS system index within the wrapped radio config. */
  QHash<int, int> _gpsSystemTable;
  /** Maps a code-plug APRS system index to the APRS system index within the wrapped radio config. */
  QHash<int, int> _aprsSystemTable;
  /** Maps a code-plug roaming zone index to the roaming zone index within the wrapped radio config. */
  QHash<int, int> _roamingZoneTable;
  /** Maps a code-plug roaming channel index to a digital channel index within the wrapped radio config. */
  QHash<int, int> _roamingChannelTable;
};

#endif // CODEPLUGCONTEXT_HH
