#ifndef CODEPLUGCONTEXT_HH
#define CODEPLUGCONTEXT_HH

#include <QHash>
#include "config.hh"

/** Helper class to assemble radio config from binary codeplugs that use index-tables and bitmaps
 * to specify which elements of the codeplug are valid. This may lead to sparse specification
 * of channels etc. Hence an codeplug index to config object mapping is needed. This class
 * provides this mapping.
 *
 * @ingroup util */
class CodeplugContext
{
public:
  /** Wraps the given device config. */
  CodeplugContext(Config *config);

  /** Returns the wrapped config. */
  Config *config() const;

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

protected:
  /** The wrapped radio config (aka abstract code-plug). */
  Config *_config;
  /** Maps a code-plug channel index to the channel index within the wrapped radio config. */
  QHash<int, int> _channelTable;
  /** Maps a code-plug digital-contact index to the contact index within the wrapped radio config. */
  QHash<int, int> _digitalContactTable;
  /** Maps a code-plug RX group-list index to the group-list index within the wrapped radio config. */
  QHash<int, int> _groupListTable;
  /** Maps a code-plug scan-list index to the scan-list index within the wrapped radio config. */
  QHash<int, int> _scanListTable;
};

#endif // CODEPLUGCONTEXT_HH
