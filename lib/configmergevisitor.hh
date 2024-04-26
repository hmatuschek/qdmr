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
 * configurations into one. */
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
   * if tehre are two sets with the same name. */
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
   * @param itemStrategy Specifies the item merge strategy.
   * @param setStrategy Specifies the set merge strategy. */
  ConfigMergeVisitor(Config *destination, QHash<ConfigItem *, ConfigItem *> &translation,
                     ItemStrategy itemStrategy=ItemStrategy::Ignore,
                     SetStrategy setStrategy=SetStrategy::Ignore);


  bool processItem(ConfigItem *item, const ErrorStack &err=ErrorStack());

protected:
  bool processRadioID(RadioID *item, const ErrorStack &err = ErrorStack());
  bool processChannel(Channel *item, const ErrorStack &err = ErrorStack());
  bool processContact(Contact *item, const ErrorStack &err = ErrorStack());
  bool processPositioningSystem(PositioningSystem *item, const ErrorStack &err = ErrorStack());
  bool processRoamingChannel(RoamingChannel *item, const ErrorStack &err = ErrorStack());
  bool processGroupList(RXGroupList *item, const ErrorStack &err = ErrorStack());
  bool processZone(Zone *item, const ErrorStack &err = ErrorStack());
  bool processScanList(ScanList *item, const ErrorStack &err = ErrorStack());
  bool processRoamingZone(RoamingZone *item, const ErrorStack &err = ErrorStack());

  bool addObject(AbstractConfigObjectList *list, ConfigObject *present, ConfigObject *merging, const ErrorStack &err = ErrorStack());
  bool ignoreObject(AbstractConfigObjectList *list, ConfigObject *present, ConfigObject *merging, const ErrorStack &err = ErrorStack());
  bool replaceObject(AbstractConfigObjectList *list, ConfigObject *present, ConfigObject *merging, const ErrorStack &err = ErrorStack());
  bool duplicateObject(AbstractConfigObjectList *list, ConfigObject *present, ConfigObject *merging, const ErrorStack &err = ErrorStack());
  bool mergeList(ConfigObjectRefList *present, ConfigObjectRefList *merging, const ErrorStack &err = ErrorStack());

protected:
  /** The destination configuration. */
  Config *_destination;
  /** Translation table for fixing references. */
  QHash<ConfigItem *, ConfigItem *> &_translation;
  /** The item merge strategy. */
  ItemStrategy _itemStrategy;
  /** The set merge strategy. */
  SetStrategy _setStrategy;
};


#endif // CONFIGMERGEVISITOR_HH
