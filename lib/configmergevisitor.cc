#include "configmergevisitor.hh"
#include "configcopyvisitor.hh"
#include "configobject.hh"
#include "configreference.hh"
#include "config.hh"
#include "channel.hh"


/* ********************************************************************************************* *
 * Implementation of ConfigMergeVisitor
 * ********************************************************************************************* */
ConfigMergeVisitor::ConfigMergeVisitor(Config* destination, QHash<ConfigObject*, ConfigObject*>& translation,
    ItemStrategy itemStrategy, SetStrategy setStrategy)
  : Visitor(), _destination(destination), _translation(translation),
    _itemStrategy(itemStrategy), _setStrategy(setStrategy)
{
  // pass...
}


bool
ConfigMergeVisitor::processItem(ConfigItem *item, const ErrorStack &err) {

  // Dispatch by type
  if (item->is<RadioID>())
    return processRadioID(item->as<RadioID>(), err);
  else if (item->is<Contact>())
    return processContact(item->as<Contact>(), err);
  else if (item->is<RXGroupList>())
    return processGroupList(item->as<RXGroupList>(), err);
  else if (item->is<Channel>())
    return processChannel(item->as<Channel>(), err);
  else if (item->is<Zone>())
    return processZone(item->as<Zone>(), err);
  else if (item->is<ScanList>())
    return processScanList(item->as<ScanList>(), err);
  else if (item->is<PositioningSystem>())
    return processPositioningSystem(item->as<PositioningSystem>(), err);
  else if (item->is<RoamingChannel>())
    return processRoamingChannel(item->as<RoamingChannel>(), err);
  else if (item->is<RoamingZone>())
    return processRoamingZone(item->as<RoamingZone>(), err);

  return Visitor::processItem(item, err);
}


bool
ConfigMergeVisitor::processRadioID(RadioID *item, const ErrorStack &err) {
  if (0 == _destination->radioIDs()->findItemsByName(item->name()).count())
    return addObject(_destination->radioIDs(), nullptr, item, err);

  RadioID *present = _destination->radioIDs()->findItemsByName(item->name())
      .first()->as<RadioID>();

  if (ItemStrategy::Ignore == _itemStrategy)
    return ignoreObject(_destination->radioIDs(), present, item, err);

  if (ItemStrategy::Override == _itemStrategy)
    return replaceObject(_destination->radioIDs(), present, item, err);

  if (ItemStrategy::Duplicate == _itemStrategy)
    return duplicateObject(_destination->radioIDs(), present, item, err);

  return true;
}


bool
ConfigMergeVisitor::processChannel(Channel *item, const ErrorStack &err) {
  if (0 == _destination->channelList()->findItemsByName(item->name()).count())
    return addObject(_destination->channelList(), nullptr, item, err);

  Channel *present = _destination->channelList()->findItemsByName(item->name())
      .first()->as<Channel>();

  if (ItemStrategy::Ignore == _itemStrategy)
    return ignoreObject(_destination->channelList(), present, item, err);

  if (ItemStrategy::Override == _itemStrategy)
    return replaceObject(_destination->channelList(), present, item, err);

  if (ItemStrategy::Duplicate == _itemStrategy)
    return duplicateObject(_destination->channelList(), present, item, err);

  return true;
}


bool
ConfigMergeVisitor::processContact(Contact *item, const ErrorStack &err) {
  if (0 == _destination->contacts()->findItemsByName(item->name()).count())
    return addObject(_destination->contacts(), nullptr, item, err);

  Contact *present = _destination->contacts()->findItemsByName(item->name())
      .first()->as<Contact>();

  if (ItemStrategy::Ignore == _itemStrategy)
    return ignoreObject(_destination->contacts(), present, item, err);

  if (ItemStrategy::Override == _itemStrategy)
    return replaceObject(_destination->contacts(), present, item, err);

  if (ItemStrategy::Duplicate == _itemStrategy)
    return duplicateObject(_destination->contacts(), present, item, err);

  return true;
}


bool
ConfigMergeVisitor::processPositioningSystem(PositioningSystem *item, const ErrorStack &err) {
  if (0 == _destination->posSystems()->findItemsByName(item->name()).count())
    return addObject(_destination->posSystems(), nullptr, item, err);

  PositioningSystem *present = _destination->posSystems()->findItemsByName(item->name())
      .first()->as<PositioningSystem>();

  if (ItemStrategy::Ignore == _itemStrategy)
    return ignoreObject(_destination->posSystems(), present, item, err);

  if (ItemStrategy::Override == _itemStrategy)
    return replaceObject(_destination->posSystems(), present, item, err);

  if (ItemStrategy::Duplicate == _itemStrategy)
    return duplicateObject(_destination->posSystems(), present, item, err);

  return true;
}


bool
ConfigMergeVisitor::processRoamingChannel(RoamingChannel *item, const ErrorStack &err) {
  if (0 == _destination->roamingChannels()->findItemsByName(item->name()).count())
    return addObject(_destination->roamingChannels(), nullptr, item, err);

  RoamingChannel *present = _destination->roamingChannels()->findItemsByName(item->name())
      .first()->as<RoamingChannel>();

  if (ItemStrategy::Ignore == _itemStrategy)
    return ignoreObject(_destination->roamingChannels(), present, item, err);

  if (ItemStrategy::Override == _itemStrategy)
    return replaceObject(_destination->roamingChannels(), present, item, err);

  if (ItemStrategy::Duplicate == _itemStrategy)
    return duplicateObject(_destination->roamingChannels(), present, item, err);

  return true;
}


bool
ConfigMergeVisitor::processGroupList(RXGroupList *item, const ErrorStack &err) {
  if (0 == _destination->rxGroupLists()->findItemsByName(item->name()).count())
    return addObject(_destination->rxGroupLists(), nullptr, item, err);

  RXGroupList *present = _destination->rxGroupLists()->findItemsByName(item->name())
      .first()->as<RXGroupList>();

  if (SetStrategy::Ignore == _setStrategy)
    return ignoreObject(_destination->rxGroupLists(), present, item, err);

  if (SetStrategy::Override == _setStrategy)
    return replaceObject(_destination->rxGroupLists(), present, item, err);

  if (SetStrategy::Duplicate == _setStrategy)
    return duplicateObject(_destination->rxGroupLists(), present, item, err);

  if (SetStrategy::Merge == _setStrategy)
    return mergeList(present->contacts(), item->contacts(), err);

  return true;
}


bool
ConfigMergeVisitor::processZone(Zone *item, const ErrorStack &err) {
  if (0 == _destination->zones()->findItemsByName(item->name()).count())
    return addObject(_destination->zones(), nullptr, item, err);

  Zone *present = _destination->zones()->findItemsByName(item->name())
      .first()->as<Zone>();

  if (SetStrategy::Ignore == _setStrategy)
    return ignoreObject(_destination->zones(), present, item, err);

  if (SetStrategy::Override == _setStrategy)
    return replaceObject(_destination->zones(), present, item, err);

  if (SetStrategy::Duplicate == _setStrategy)
    return duplicateObject(_destination->zones(), present, item, err);

  if (SetStrategy::Merge == _setStrategy)
    return mergeList(present->A(), item->A(), err) &&
        mergeList(present->B(), item->B(), err);

  return true;
}


bool
ConfigMergeVisitor::processScanList(ScanList *item, const ErrorStack &err) {
  if (0 == _destination->scanlists()->findItemsByName(item->name()).count())
    return addObject(_destination->scanlists(), nullptr, item, err);

  ScanList *present = _destination->scanlists()->findItemsByName(item->name())
      .first()->as<ScanList>();

  if (SetStrategy::Ignore == _setStrategy)
    return ignoreObject(_destination->scanlists(), present, item, err);

  if (SetStrategy::Override == _setStrategy)
    return replaceObject(_destination->scanlists(), present, item, err);

  if (SetStrategy::Duplicate == _setStrategy)
    return duplicateObject(_destination->scanlists(), present, item, err);

  if (SetStrategy::Merge == _setStrategy)
    return mergeList(present->channels(), item->channels(), err);

  return true;
}


bool
ConfigMergeVisitor::processRoamingZone(RoamingZone *item, const ErrorStack &err) {
  if (0 == _destination->roamingZones()->findItemsByName(item->name()).count())
    return addObject(_destination->roamingZones(), nullptr, item, err);

  RoamingZone *present = _destination->roamingZones()->findItemsByName(item->name())
      .first()->as<RoamingZone>();

  if (SetStrategy::Ignore == _setStrategy)
    return ignoreObject(_destination->roamingZones(), present, item, err);

  if (SetStrategy::Override == _setStrategy)
    return replaceObject(_destination->roamingZones(), present, item, err);

  if (SetStrategy::Duplicate == _setStrategy)
    return duplicateObject(_destination->roamingZones(), present, item, err);

  if (SetStrategy::Merge == _setStrategy)
    return mergeList(present->channels(), item->channels(), err);

  return true;
}


bool
ConfigMergeVisitor::addObject(AbstractConfigObjectList *list, ConfigObject *present, ConfigObject *merging, const ErrorStack &err) {
  Q_UNUSED(present)

  ConfigObject *newObject = ConfigCopy::copy(merging, err)->as<ConfigObject>();
  if (nullptr == newObject) {
    errMsg(err) << "Cannot copy item '" << merging->name() << "'.";
    return false;

  }

  list->add(newObject);
  _translation[merging] = newObject;

  return true;
}


bool
ConfigMergeVisitor::ignoreObject(AbstractConfigObjectList *list, ConfigObject *present, ConfigObject *merging, const ErrorStack &err) {
  Q_UNUSED(list); Q_UNUSED(err)

  _translation[merging] = present;

  return true;
}


bool
ConfigMergeVisitor::replaceObject(AbstractConfigObjectList *list, ConfigObject *present, ConfigObject *merging, const ErrorStack &err) {
  ConfigObject *newObject = ConfigCopy::copy(merging, err)->as<ConfigObject>();
  if (nullptr == newObject) {
    errMsg(err) << "Cannot copy item '" << merging->name() << "'.";
    return false;
  }

  list->replace(newObject, list->indexOf(present));
  _translation[merging] = newObject;
  _translation[present] = newObject;

  return true;
}


bool
ConfigMergeVisitor::duplicateObject(AbstractConfigObjectList *list, ConfigObject *present, ConfigObject *merging, const ErrorStack &err) {
  Q_UNUSED(present)

  ConfigObject *newItem = ConfigCopy::copy(merging, err)->as<ConfigObject>();
  if (nullptr == newItem) {
    errMsg(err) << "Cannot copy item '" << merging->name() << "'.";
    return false;
  }

  newItem->setName(newItem->name()+" (copy)");
  list->add(newItem);
  _translation[merging] = newItem;

  return true;
}

bool
ConfigMergeVisitor::mergeList(ConfigObjectRefList *present, ConfigObjectRefList *merging, const ErrorStack &err) {
  Q_UNUSED(err);

  for (int i=0; i<merging->count(); i++) {
    ConfigObject *mergingItem = merging->get(i);
    mergingItem = _translation.value(mergingItem, mergingItem)->as<ConfigObject>();
    if (! present->has(mergingItem)) {
      present->add(mergingItem);
    }
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of ConfigMerge
 * ********************************************************************************************* */
bool
ConfigMerge::mergeInto(Config *destination, Config *source,
                       ConfigMergeVisitor::ItemStrategy itemStrategy,
                       ConfigMergeVisitor::SetStrategy setStrategy,
                       const ErrorStack &err)
{
  QHash<ConfigObject *, ConfigObject *> referenceTable;
  ConfigMergeVisitor mergeVisitor(destination, referenceTable, itemStrategy, setStrategy);
  if (! mergeVisitor.process(source, err)) {
    errMsg(err) << "Cannot merge configurations.";
    return false;
  }

  FixReferencesVisistor linkVisitor(referenceTable, true);
  if (! linkVisitor.process(destination, err)) {
    errMsg(err) << "Cannot fix references in merged configuration.";
    return false;
  }

  return true;
}


Config *
ConfigMerge::merge(Config *destination, Config *source,
                   ConfigMergeVisitor::ItemStrategy itemStrategy,
                   ConfigMergeVisitor::SetStrategy setStrategy,
                   const ErrorStack &err)
{
  Config *copy = ConfigCopy::copy(destination, err)->as<Config>();
  if (nullptr == copy) {
    errMsg(err) << "Cannot merge configurations.";
    return nullptr;
  }

  if (! mergeInto(copy, source, itemStrategy, setStrategy, err)) {
    delete copy;
    return nullptr;
  }

  return copy;
}
