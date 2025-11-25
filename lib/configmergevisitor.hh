#ifndef CONFIGMERGEVISITOR_HH
#define CONFIGMERGEVISITOR_HH

#include "visitor.hh"

class ConfigObject;
class RadioID;
class Channel;
class Contact;
class PositioningSystem;
class RoamingChannel;
class ConfigObjectRefList;
class RXGroupList;
class Zone;
class ScanList;
class RoamingZone;


/** Implements a visitor to merge the visited config into a given config.
 * The destination configuration object is passed to the constructor. This allows to merge several
 * configurations into one.
 * @ingroup conf */
class ConfigMergeVisitor : public Visitor
{
public:
  /** Possible strategies to merge atomic items. That is, Channels, Contacts & PositioningSystems,
   * if there are two items with the same name. */
  enum class ItemStrategy {
    Ignore,    ///< Ignore the source item.
    Override,  ///< Override the destination item.
    Duplicate  ///< Add a new item with a modified name.
  };

  /** Possible strategies to merge sets of references. That is, GroupLists, Zones & ScanLists,
   * if there are two sets with the same name. */
  enum class SetStrategy {
    Ignore,    ///< Ignore the source set.
    Override,  ///< Override the destination item.
    Duplicate, ///< Add a new set with a modified name.
    Merge      ///< Merge sets.
  };

public:
  /** Constructor of the merge visitor.
   * @param destination Specifies the destination configuration, the additional configurations are
   *        merged into.
   * @param translation Specifies the translation table to fill of objects being replaced.
   * @param itemStrategy Specifies the item merge strategy.
   * @param setStrategy Specifies the set merge strategy. */
  ConfigMergeVisitor(Config *destination, QHash<ConfigObject *, ConfigObject *> &translation,
                     ItemStrategy itemStrategy=ItemStrategy::Ignore,
                     SetStrategy setStrategy=SetStrategy::Ignore);


  bool processItem(ConfigItem *item, const ErrorStack &err=ErrorStack());

protected:
  /** Handles a RadioID object of the source configuration. */
  bool processRadioID(RadioID *item, const ErrorStack &err = ErrorStack());
  /** Handles a Channel object of the source configuration. */
  bool processChannel(Channel *item, const ErrorStack &err = ErrorStack());
  /** Handles a Contact object of the source configuration. */
  bool processContact(Contact *item, const ErrorStack &err = ErrorStack());
  /** Handles a PositioningSystem object of the source configuration. */
  bool processPositioningSystem(PositioningSystem *item, const ErrorStack &err = ErrorStack());
  /** Handles a RoamingChannel object of the source configuration. */
  bool processRoamingChannel(RoamingChannel *item, const ErrorStack &err = ErrorStack());
  /** Handles a RXGroupList object of the source configuration. */
  bool processGroupList(RXGroupList *item, const ErrorStack &err = ErrorStack());
  /** Handles a Zone object of the source configuration. */
  bool processZone(Zone *item, const ErrorStack &err = ErrorStack());
  /** Handles a ScanList object of the source configuration. */
  bool processScanList(ScanList *item, const ErrorStack &err = ErrorStack());
  /** Handles a RoamingZone object of the source configuration. */
  bool processRoamingZone(RoamingZone *item, const ErrorStack &err = ErrorStack());

  /** Adds a copy of the @c merging object to the given list, containing the colliding @c present
   *  object. Also updates the translation table to bend references to the merging object to that copy.  */
  bool addObject(AbstractConfigObjectList *list, ConfigObject *present, ConfigObject *merging, const ErrorStack &err = ErrorStack());
  /** Does not add the @c merging object, but fixes the translation table to bend references to the merging object. */
  bool ignoreObject(AbstractConfigObjectList *list, ConfigObject *present, ConfigObject *merging, const ErrorStack &err = ErrorStack());
  /** Replaces the @c present object in the given list by a copy of @c merging object. Also fixes
   *  the translation table to bend references to both the removed @c present object as well as the
   *  @c merging object. */
  bool replaceObject(AbstractConfigObjectList *list, ConfigObject *present, ConfigObject *merging, const ErrorStack &err = ErrorStack());
  /** Adds a copy of the @c merging object to the given list. */
  bool duplicateObject(AbstractConfigObjectList *list, ConfigObject *present, ConfigObject *merging, const ErrorStack &err = ErrorStack());
  /** Merges two reference lists. That is, @c merging gets merged into the @c present list. */
  bool mergeList(ConfigObjectRefList *present, ConfigObjectRefList *merging, const ErrorStack &err = ErrorStack());

protected:
  /** The destination configuration. */
  Config *_destination;
  /** Translation table for fixing references. */
  QHash<ConfigObject *, ConfigObject *> &_translation;
  /** The item merge strategy. */
  ItemStrategy _itemStrategy;
  /** The set merge strategy. */
  SetStrategy _setStrategy;
};


/** Just a namespace to provide merging functions.
 * @ingroup conf */
class ConfigMerge
{
public:
  /** Merges the given @c source into the given @c destination using the specified strategies to
   * handle conflicts. Here the @c destination codeplug gets modified, even on errors. */
  static bool mergeInto(Config *destination, Config *source,
                        ConfigMergeVisitor::ItemStrategy itemStrategy=ConfigMergeVisitor::ItemStrategy::Ignore,
                        ConfigMergeVisitor::SetStrategy setStrategy=ConfigMergeVisitor::SetStrategy::Ignore,
                        const ErrorStack &err = ErrorStack());

  /** Merges the given @c source into a copy of the given @c destination, using the specified
   * strategies to handle conflicts. Here the @c destination codeplug does not get modified at
   * all. */
  static Config *merge(Config *destination, Config *source,
                       ConfigMergeVisitor::ItemStrategy itemStrategy=ConfigMergeVisitor::ItemStrategy::Ignore,
                       ConfigMergeVisitor::SetStrategy setStrategy=ConfigMergeVisitor::SetStrategy::Ignore,
                       const ErrorStack &err = ErrorStack());
};

#endif // CONFIGMERGEVISITOR_HH
